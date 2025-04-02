import serial
import time

SERIAL_PORT = "/dev/tty.usbmodem1203"  # Replace with correct port
BAUD_RATE = 115200

# Set up serial communication
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
ser.flushInput()  # Clear any stale data

# Timeout settings (in seconds)
response_timeout = 5  # 5 seconds timeout for waiting for the response

while True:
    # Send message to STM32
    message = "Python"
    ser.write(message.encode())  # Send the message as bytes
    print("Sent:", message.strip())

    # Wait for response from STM32 with timeout
    response = ser.readline().decode().strip()
    
    if response:
        print("Received from STM32:", response)
    else:
        print("No response from STM32 within timeout")

    time.sleep(1)  # Wait before sending another message
