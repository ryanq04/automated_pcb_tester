import sys
from PyQt5.QtWidgets import QApplication
from gui import SignalViewer

if __name__ == "__main__":
    app = QApplication(sys.argv)
    viewer = SignalViewer()
    viewer.show()
    sys.exit(app.exec_())
