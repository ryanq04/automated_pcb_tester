import sys
import ctypes  # <-- Option 1: DPI Awareness
import numpy as np
from PyQt5.QtWidgets import QApplication, QMainWindow, QWidget, QVBoxLayout
from PyQt5.QtCore import Qt
import pyqtgraph as pg

# === Option 1: Force Windows DPI awareness (must be before QApplication) ===
ctypes.windll.shcore.SetProcessDpiAwareness(1)

class SineWavePlot(QWidget):
    def __init__(self):
        super().__init__()

        self.plot_widget = pg.PlotWidget()
        self.plot_widget.setBackground('w')
        self.plot_widget.setTitle("Centered Sine Wave", color='black', size='12pt')
        self.plot_widget.setLabel('left', 'Amplitude')
        self.plot_widget.setLabel('bottom', 'Time')

        # Generate sine wave
        x = np.linspace(0, 4 * np.pi, 1000)
        y = np.sin(x)

        # Debug: confirm the signal is properly centered
        print(f"Sine range: min={np.min(y):.3f}, max={np.max(y):.3f}, mean={np.mean(y):.3f}")

        # Plot sine wave
        self.plot_widget.plot(x, y, pen=pg.mkPen(color='b', width=2))

        # Plot horizontal zero reference line
        zero_line = pg.InfiniteLine(pos=0, angle=0, pen=pg.mkPen(color='gray', style=Qt.DashLine))
        self.plot_widget.addItem(zero_line)

        # Force y-range and disable auto-scaling
        self.plot_widget.setYRange(-1.1, 1.1)
        self.plot_widget.enableAutoRange(axis=pg.ViewBox.YAxis, enable=False)

        layout = QVBoxLayout()
        layout.addWidget(self.plot_widget)
        self.setLayout(layout)


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Sine Wave Viewer")
        self.setGeometry(100, 100, 800, 600)
        self.setCentralWidget(SineWavePlot())


if __name__ == '__main__':
    app = QApplication(sys.argv)
    main_window = MainWindow()
    main_window.show()
    sys.exit(app.exec_())
