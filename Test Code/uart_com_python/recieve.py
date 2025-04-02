import serial

SERIAL_PORT = "/dev/tty.usbmodem1203"  # Replace with actual port
BAUD_RATE = 115200

ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
ser.flushInput()  # Clear any stale data

print("Waiting for messages from STM32...\n")

try:
    while True:
        message = ser.readline().decode().strip()  # Read a full line
        if message:
            print("Received:", message)
except KeyboardInterrupt:
    print("\nStopping communication.")
    ser.close()
