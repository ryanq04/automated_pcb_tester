import serial
import time

SERIAL_PORT = "/dev/tty.usbmodem1203"  # Replace with correct port
BAUD_RATE = 115200
TIMEOUT = 1  # Timeout for reading the serial data

# Set up the serial communication
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=TIMEOUT)
ser.flushInput()  # Clear any stale data

def send_message(message):
    """Send a message to the STM32 and wait for a response."""
    print(f"Sent: {message.strip()}")
    
    # Send message to STM32
    ser.write(message.encode())  # Send the message as bytes
    ser.flush()

    # Wait for response from STM32
    response = ser.readline().decode().strip()  # Read a line of text from STM32
    return response

def main():
    while True:
        message = "LED ON\n"  # Command to turn LED on
        response = send_message(message)  # Send the message and get the response
        if response:
            print(f"Received from STM32: {response}")

        # Add a delay between messages
        time.sleep(1)

        message = "LED OFF\n"  # Command to turn LED off
        response = send_message(message)  # Send the message and get the response
        if response:
            print(f"Received from STM32: {response}")

        time.sleep(1)

if __name__ == "__main__":
    main()
