import sys
import cv2
import time
import struct
import numpy as np
import serial
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QWidget, QLabel, QPushButton, QVBoxLayout, QHBoxLayout, QLineEdit
)
from PyQt5.QtGui import QImage, QPixmap, QPainter, QColor, QPen, QFont
from PyQt5.QtCore import QTimer, Qt
import pyqtgraph as pg

import ctypes
ctypes.windll.shcore.SetProcessDpiAwareness(1)

PORT = 'COM7'
BAUDRATE = 115200
ADC_LENGTH = 2048
FFT_LENGTH = 2048
SAMPLE_RATE = 1_000_000  # 1 MSps

class SignalViewer(QMainWindow):
    def __init__(self):
        super().__init__()
        self.port = PORT
        self.baudrate = BAUDRATE
        self.setWindowTitle("Signal Viewer with FFT")
        self.setup_ui()
        self.setup_timer()
        self.awaiting_click_coords = False
        self.serial_for_coords = None

    def setup_ui(self):
        central_widget = QWidget()
        self.setCentralWidget(central_widget)

        main_layout = QHBoxLayout()
        left_layout = QVBoxLayout()
        right_layout = QVBoxLayout()

        self.image_label = QLabel("Image will appear here")
        self.image_label.setFixedSize(384, 384)
        self.image_label.setAlignment(Qt.AlignCenter)
        self.image_label.mousePressEvent = self.get_image_click_position
        left_layout.addWidget(self.image_label)

        self.port_input = QLineEdit(self.port)
        self.port_input.setPlaceholderText("Enter COM port")
        left_layout.addWidget(self.port_input)

        self.capture_button = QPushButton("Capture Dummy Image")
        self.real_capture_button = QPushButton("Capture Real Image")
        self.test_button = QPushButton("Test Signal")
        self.uart_button = QPushButton("ADC + FFT")
        self.quit_button = QPushButton("Quit")

        self.capture_button.clicked.connect(self.capture_image_only)
        self.real_capture_button.clicked.connect(self.capture_real_image)
        self.test_button.clicked.connect(self.run_test_signal)
        self.uart_button.clicked.connect(self.handle_test_uart)
        self.quit_button.clicked.connect(self.close)

        for btn in (self.capture_button, self.real_capture_button, self.test_button, self.uart_button, self.quit_button):
            left_layout.addWidget(btn)

        self.time_plot = pg.PlotWidget(title="Time Domain Signal")
        self.time_curve = self.time_plot.plot(pen='c')
        self.time_plot.setLabel('left', 'Amplitude')
        self.time_plot.setLabel('bottom', 'Time', units='s')
        self.time_plot.enableAutoRange(axis=pg.ViewBox.XYAxes, enable=True)
        right_layout.addWidget(self.time_plot)

        self.fft_plot = pg.PlotWidget(title="FFT Output")
        self.fft_zero_line = pg.InfiniteLine(pos=0, angle=0, pen=pg.mkPen('w', width=1, style=Qt.DashLine))
        self.fft_curve = None
        self.fft_plot.setLabel('left', 'Magnitude (dBV)')
        self.fft_plot.setLabel('bottom', 'Frequency', units='Hz')
        self.fft_plot.enableAutoRange(axis=pg.ViewBox.XYAxes, enable=True)
        self.fft_plot.addItem(self.fft_zero_line)
        right_layout.addWidget(self.fft_plot)

        main_layout.addLayout(left_layout)
        main_layout.addLayout(right_layout)
        central_widget.setLayout(main_layout)

    def setup_timer(self):
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_plot)
        self.timer.start(1000)

    def update_plot(self):
        pass

    def wait_for_response(self, ser, expected, timeout=5):
        start_time = time.time()
        while True:
            if time.time() - start_time > timeout:
                print(f"Timeout waiting for '{expected}'")
                return False
            try:
                line = ser.readline().decode().strip()
                print(f"Received: '{line}'")
                if line == expected:
                    return True
            except UnicodeDecodeError:
                pass

    
    def wait_for_float_echo(self, ser, expected_x, expected_y, timeout=5):
        import struct, time
        start_time = time.time()
        while True:
            if time.time() - start_time > timeout:
                print(f"Timeout waiting for float echo ({expected_x:.2f}, {expected_y:.2f})")
                return False
            try:
                echoed = ser.read(8)
                print(f"Raw echoed bytes: {list(echoed)}")
                if len(echoed) == 8:
                    echoed_x, echoed_y = struct.unpack('<ff', echoed)
                    print(f"Received echo: ({echoed_x:.2f}, {echoed_y:.2f})")
                    if abs(echoed_x - expected_x) < 0.01 and abs(echoed_y - expected_y) < 0.01:
                        return True
                    else:
                        print("Echo mismatch.")
            except Exception as e:
                print(f"Error reading float echo: {e}")


    def capture_image_only(self):
        dummy_data = np.random.randint(0, 255, (144, 174), dtype=np.uint8)
        image = QImage(dummy_data.data, dummy_data.shape[1], dummy_data.shape[0],
                       dummy_data.strides[0], QImage.Format_Grayscale8)
        pixmap = QPixmap.fromImage(image)
        scaled_pixmap = pixmap.scaled(self.image_label.width(), self.image_label.height(), Qt.KeepAspectRatio)
        self.image_label.setPixmap(scaled_pixmap)
        self.latest_image = dummy_data
        self.image_shape = dummy_data.shape
        self.displayed_image_size = (scaled_pixmap.width(), scaled_pixmap.height())

    def capture_real_image(self):
        try:
            port_name = self.port_input.text().strip() or self.port
            ser = serial.Serial(port_name, self.baudrate, timeout=2)
            self.serial_for_coords = ser

            print("Sending TAKEPC command...")
            ser.write(b"TAKEPC\r\n")

            print("Waiting for STM32 echo handshake...")
            if not self.wait_for_response(ser, "TAKEPC"):
                ser.close()
                return

            print("Waiting for PREAMBLE...")
            if not self.wait_for_response(ser, "PREAMBLE!", timeout=10):
                ser.close()
                return

            print("PREAMBLE received. Reading image...")
            rows, cols = 144, 174
            expected_bytes = rows * cols * 2
            raw = b''
            while len(raw) < expected_bytes:
                raw += ser.read(expected_bytes - len(raw))

            img_ycbcr422 = np.frombuffer(raw, dtype=np.uint8).reshape((rows, cols // 2, 4))
            Y0 = img_ycbcr422[:, :, 0].astype(np.float32)
            Cb = img_ycbcr422[:, :, 1].astype(np.float32)
            Y1 = img_ycbcr422[:, :, 2].astype(np.float32)
            Cr = img_ycbcr422[:, :, 3].astype(np.float32)

            Y = np.zeros((rows, cols), dtype=np.float32)
            Y[:, 0::2] = Y0
            Y[:, 1::2] = Y1
            Cb = np.repeat(Cb, 2, axis=1)
            Cr = np.repeat(Cr, 2, axis=1)

            R = np.clip((Y + 1.402 * (Cr - 128)), 0, 255).astype(np.uint8)
            G = np.clip((Y - 0.344136 * (Cb - 128) - 0.714136 * (Cr - 128)), 0, 255).astype(np.uint8)
            B = np.clip((Y + 1.772 * (Cb - 128)), 0, 255).astype(np.uint8)

            frame = np.stack((G, R, B), axis=-1)
            frame = np.rot90(frame, 2)

            self.latest_image = frame
            self.image_shape = frame.shape
            self.awaiting_click_coords = True
            self.show_select_pad_overlay()

        except Exception as e:
            print(f"Failed to capture real image: {e}")

    def show_select_pad_overlay(self):
        if hasattr(self, 'latest_image'):
            frame = self.latest_image.copy()
            height, width = frame.shape[:2]
            image = QImage(frame.data.tobytes(), width, height, width * 3, QImage.Format_RGB888)
            pixmap = QPixmap.fromImage(image)

            painter = QPainter(pixmap)
            pen = QPen(QColor(0, 255, 0), 4)
            painter.setPen(pen)
            painter.drawRect(0, 0, pixmap.width() - 1, pixmap.height() - 1)
            painter.setFont(QFont("Arial", 10, QFont.Bold))
            painter.drawText(pixmap.rect(), Qt.AlignTop | Qt.AlignHCenter, "Select pad")
            painter.end()

            scaled_pixmap = pixmap.scaled(self.image_label.width(), self.image_label.height(), Qt.KeepAspectRatio)
            self.image_label.setPixmap(scaled_pixmap)
            self.displayed_image_size = (scaled_pixmap.width(), scaled_pixmap.height())


    def get_image_click_position(self, event):
        if not self.awaiting_click_coords:
            print("Click ignored: not in coordinate selection mode.")
            return

        if hasattr(self, 'image_shape') and self.image_label.pixmap() is not None:
            label_width = self.image_label.width()
            label_height = self.image_label.height()
            displayed_width, displayed_height = self.displayed_image_size

            offset_x = (label_width - displayed_width) // 2
            offset_y = (label_height - displayed_height) // 2

            x = event.pos().x() - offset_x
            y = event.pos().y() - offset_y

            if 0 <= x < displayed_width and 0 <= y < displayed_height:
                img_height, img_width = self.image_shape[:2]
                img_x = float(x * img_width / displayed_width)
                img_y = float(y * img_height / displayed_height)
                img_arr2D = [img_x, img_y]
                print(f"Clicked on actual image at: ({x}, {y})")
                
                img_arr3D = project_2D_to_3D(img_arr2D)
                print(f"Corresponding 3D coord: ({img_arr3D[0]:.2f}, {img_arr3D[1]:.2f})")

                try:
                    ser = self.serial_for_coords
                    if not ser:
                        print("No serial connection available.")
                        return
                    
                    print("Sending COORDS command...")
                    ser.write(b"COORDS\r\n")
                    if not self.wait_for_response(ser, "COORDS"):
                        print("No echo back for COORDS.")
                        return
                    ser.reset_input_buffer()

                    packed = struct.pack('<ff', *img_arr3D)
                    print("Packed bytes (as ints):", list(packed))
                    # packed = struct.pack('<ff', img_x, img_y)
                    ser.write(packed)
                    print(f"Sent coordinates: ({img_arr3D[0]:.2f}, {img_arr3D[1]:.2f})")

                    if self.wait_for_float_echo(ser, img_arr3D[0], img_arr3D[1]):
                        print("Float echo verified successfully.")
                    else:
                        print("Float echo mismatch or timeout.")

                except Exception as e:
                    print(f"Error sending coordinates: {e}")

                finally:
                    self.awaiting_click_coords = False
                    self.serial_for_coords = None
                    self.display_real_image(self.latest_image)
            else:
                print("Click was outside of image area.")



    def display_real_image(self, frame):
        height, width, channels = frame.shape
        bytes_per_line = channels * width
        image = QImage(frame.data.tobytes(), width, height, bytes_per_line, QImage.Format_RGB888)
        pixmap = QPixmap.fromImage(image)
        scaled_pixmap = pixmap.scaled(self.image_label.width(), self.image_label.height(), Qt.KeepAspectRatio)
        self.image_label.setPixmap(scaled_pixmap)
        self.displayed_image_size = (scaled_pixmap.width(), scaled_pixmap.height())

    def run_test_signal(self):
        t = np.linspace(0, ADC_LENGTH / SAMPLE_RATE, ADC_LENGTH)
        signal_type = np.random.choice(['sine', 'square'])
        freq = np.random.uniform(1000, 100_000)

        if signal_type == 'sine':
            signal = np.sin(2 * np.pi * freq * t)
        else:
            signal = np.sign(np.sin(2 * np.pi * freq * t))

        signal += 0.2 * np.random.randn(ADC_LENGTH)
        signal *= 4095.0 / 3.3
        print(f"Test signal: {signal_type} wave, Frequency: {freq:.1f} Hz, with mild noise")
        self.update_plots(signal)

    def update_plots(self, signal):
        t = np.arange(len(signal)) / SAMPLE_RATE
        signal_volts = (signal * 3.3 / 4095.0)
        self.time_curve.setData(t, signal_volts)
        fft = np.abs(np.fft.rfft(signal_volts))
        fft_db = 20 * np.log10((fft * 3.3 / 4095.0) + 1e-12)
        freqs = np.fft.rfftfreq(len(signal), d=1/SAMPLE_RATE)

        if self.fft_curve is None:
            self.fft_curve = pg.PlotDataItem(freqs, fft_db, pen='y')
            self.fft_plot.addItem(self.fft_curve)
        else:
            self.fft_curve.setData(freqs, fft_db)

    def handle_test_uart(self):
        try:
            with serial.Serial(self.port, self.baudrate, timeout=2) as ser:
                print("Waiting for ADC preamble...")
                while ser.readline().decode().strip() != "ADC":
                    pass

                adc_data = ser.read(ADC_LENGTH * 2)
                adc_values = np.frombuffer(adc_data, dtype=np.uint16)
                t = np.arange(len(adc_values)) / SAMPLE_RATE
                self.time_curve.setData(t, adc_values)

                print("Waiting for FFT preamble...")
                while ser.readline().decode().strip() != "FFT":
                    pass

                freqs = ser.read((FFT_LENGTH // 2) * 4)
                mags = ser.read((FFT_LENGTH // 2) * 4)
                frequencies = np.frombuffer(freqs, dtype='<f4')
                magnitudes = np.frombuffer(mags, dtype='<f4')

                magnitudes_db = 20 * np.log10((magnitudes * 3.3 / 4095.0) + 1e-12)
                if not np.isnan(magnitudes_db).any():
                    if self.fft_curve is None:
                        self.fft_curve = pg.PlotDataItem(frequencies, magnitudes_db, pen='y')
                        self.fft_plot.addItem(self.fft_curve)
                    else:
                        self.fft_curve.setData(frequencies, magnitudes_db)
        except Exception as e:
            print(f"Error during UART communication: {e}")
            
def project_2D_to_3D(pt_2D):
    
    # Image points (2D) and their real-world counterparts (3D but assumed to be on a plane)
    image_pts = np.array([[133.7, 3.2], [132.7, 62.2], [77.5, 3.6], [24.5, 120.8], [76.1, 61.3]], dtype=np.float32)
    world_pts = np.array([[0, 0, 0], [3, 0, 0], [0, 3, 0], [6, 6, 0], [3, 3, 0]], dtype=np.float32)

    # Compute homography
    H, _ = cv2.findHomography(image_pts, world_pts[:, :2])

    pt_3D = np.dot(H, np.array([pt_2D[0], pt_2D[1], 1]))
    pt_3D_normalized = pt_3D / pt_3D[2]
    pt_3D_rounded = np.round(pt_3D_normalized, 2)

    return ([pt_3D_rounded[0], pt_3D_rounded[1]])

if __name__ == "__main__":
    app = QApplication(sys.argv)
    viewer = SignalViewer()
    viewer.show()
    sys.exit(app.exec_())