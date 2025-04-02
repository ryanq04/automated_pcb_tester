import time
import serial

PORT = "/dev/tty.usbmodem11103"
BAUDRATE = 115200

ser = serial.Serial(PORT, BAUDRATE, timeout=1)

print("[STM32 SIM] Sending PREAMBLE...")
ser.write(b"PREAMBLE!\r\n")
time.sleep(1)  # Delay to let the receiver sync

print("[STM32 SIM] Sending test data...")
ser.write(b"HELLO FROM STM32!\r\n")
ser.flush()

print("[STM32 SIM] Transmission complete.")
ser.close()
