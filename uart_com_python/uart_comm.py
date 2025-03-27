import serial
import time

SERIAL_PORT = "/dev/tty.usbmodem1203"  # Replace with correct port
BAUD_RATE = 115200

ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

while True:
    # Send message to STM32
    message = "Python received\n"
    ser.write(message.encode())  # Convert string to bytes and send
    print("Sent:", message.strip())

    # Wait for response from STM32
    response = ser.readline().decode().strip()  # Read response
    if response:
        print("Received from STM32:", response)

    time.sleep(1)  # Wait before sending the next message
