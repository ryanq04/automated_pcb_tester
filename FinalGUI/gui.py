# gui.py
import struct
import serial
import numpy as np
from PyQt5.QtWidgets import QMainWindow, QLabel, QPushButton, QVBoxLayout, QHBoxLayout, QLineEdit, QWidget
from PyQt5.QtCore import QTimer, Qt
from PyQt5.QtGui import QImage, QPixmap, QPainter, QColor, QPen, QFont
import pyqtgraph as pg

from adc_fft import generate_test_signal, update_time_and_fft, handle_uart_adc_fft
from camera import decode_ycbcr422, project_2D_to_3D
from utils import wait_for_response, wait_for_float_echo

PORT = 'COM7'
BAUDRATE = 115200
ADC_LENGTH = 2048
SAMPLE_RATE = 1_000_000

class SignalViewer(QMainWindow):
    def __init__(self):
        super().__init__()
        self.port = PORT
        self.baudrate = BAUDRATE
        self.setWindowTitle("Signal Viewer with FFT")
        self.serial = None
        self.awaiting_click_coords = False

        try:
            self.serial = serial.Serial(self.port, self.baudrate, timeout=2)
            print(f"Opened serial port {self.port} at {self.baudrate} baud.")
        except Exception as e:
            print(f"Failed to open serial port: {e}")

        self.setup_ui()
        self.setup_timer()

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

        self.real_capture_button = QPushButton("Capture Real Image")
        self.probe_button = QPushButton("Probe PCB")
        self.send_home_button = QPushButton("Send Home")  # NEW
        self.test_button = QPushButton("Test Signal")
        self.uart_button = QPushButton("ADC + FFT")
        self.quit_button = QPushButton("Quit")

        self.real_capture_button.clicked.connect(self.capture_real_image)
        self.probe_button.clicked.connect(self.enable_probing)
        self.send_home_button.clicked.connect(self.send_home_coordinates)  # NEW
        self.test_button.clicked.connect(self.run_test_signal)
        self.uart_button.clicked.connect(self.handle_test_uart)
        self.quit_button.clicked.connect(self.close)

        for btn in (
            self.real_capture_button,
            self.probe_button,
            self.send_home_button,  # NEW
            self.test_button,
            self.uart_button,
            self.quit_button,
        ):
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
        self.timer.timeout.connect(lambda: None)
        self.timer.start(1000)

    def capture_real_image(self):
        try:
            ser = self.serial
            ser.reset_input_buffer()
            if not ser or not ser.is_open:
                print("Serial port is not available.")
                return

            ser.write(b"TAKEPC\r\n")
            ser.flush()
            if not wait_for_response(ser, "TAKEPC"): return
            if not wait_for_response(ser, "PREAMBLE!", timeout=10): return

            rows, cols = 144, 174
            raw = b''
            while len(raw) < rows * cols * 2:
                raw += ser.read((rows * cols * 2) - len(raw))

            frame = decode_ycbcr422(raw, rows, cols)
            self.latest_image = frame
            self.image_shape = frame.shape
            self.awaiting_click_coords = False
            self.display_real_image(self.latest_image)

        except Exception as e:
            print(f"Failed to capture real image: {e}")

    def enable_probing(self):
        self.awaiting_click_coords = True
        self.show_select_pad_overlay()

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
                print("Clicked at:", img_x, img_y)

                img_arr3D = project_2D_to_3D([img_x, img_y])
                try:
                    ser = self.serial
                    if not ser: return
                    ser.write(b"COORDS\r\n")
                    ser.flush()
                    if not wait_for_response(ser, "COORDS"): return
                    ser.reset_input_buffer()

                    packed = struct.pack('<ff', *img_arr3D)
                    ser.write(packed)
                    ser.flush()

                    if wait_for_float_echo(ser, img_arr3D[0], img_arr3D[1]):
                        print("Float echo verified successfully.")
                except Exception as e:
                    print(f"Failed to send coordinates: {e}")

    def display_real_image(self, frame):
        height, width, channels = frame.shape
        bytes_per_line = channels * width
        image = QImage(frame.data.tobytes(), width, height, bytes_per_line, QImage.Format_RGB888)
        pixmap = QPixmap.fromImage(image)
        scaled_pixmap = pixmap.scaled(self.image_label.width(), self.image_label.height(), Qt.KeepAspectRatio)
        self.image_label.setPixmap(scaled_pixmap)
        self.displayed_image_size = (scaled_pixmap.width(), scaled_pixmap.height())

    def run_test_signal(self):
        signal, freq, wave_type = generate_test_signal()
        print(f"Test signal: {wave_type}, Frequency: {freq:.1f} Hz")
        self.fft_curve = update_time_and_fft(signal, self.time_curve, self.fft_curve, self.fft_plot)

    def handle_test_uart(self):
        self.fft_curve = handle_uart_adc_fft(self.serial, self.time_curve, self.fft_curve, self.fft_plot)
        

    def closeEvent(self, event):
        if self.serial and self.serial.is_open:
            print("Closing serial port...")
            self.serial.close()
        super().closeEvent(event)


    def send_home_coordinates(self):
        home_coords = (6.0, 6.0)
        try:
            ser = self.serial
            if not ser:
                print("Serial port not available.")
                return

            print("Sending COORDS for home position...")
            ser.write(b"COORDS\r\n")
            ser.flush()
            if not wait_for_response(ser, "COORDS"):
                print("No echo back for COORDS.")
                return
            ser.reset_input_buffer()

            packed = struct.pack('<ff', *home_coords)
            ser.write(packed)
            ser.flush()
            print(f"Sent home coordinates: {home_coords}")

            if wait_for_float_echo(ser, *home_coords):
                print("Home float echo verified successfully.")
            else:
                print("Float echo mismatch or timeout.")

        except Exception as e:
            print(f"Error sending home coordinates: {e}")
