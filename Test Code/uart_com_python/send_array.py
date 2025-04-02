import serial
import struct
import time

SERIAL_PORT = "/dev/tty.usbmodem1303"  # Adjust with the correct port
BAUD_RATE = 115200

# Set up serial communication
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
ser.flushInput()

# Array of 3 floats to send
floats_to_send = [1.23, 4.56, 7.89]

while True:
    # Pack the floats into a byte array using struct (3 floats, each 4 bytes)
    byte_array = struct.pack('<fff', *floats_to_send)  # Little-endian format

    # Send the byte array over UART
    ser.write(byte_array)
    ser.flush()

    print(f"Sent: {floats_to_send}")

    # Wait for response or acknowledgment
    response = ser.read(12)  # Read exactly 12 bytes (3 floats)

    # Print the raw response in bytes for debugging
    print(f"Raw response (bytes): {response}")

    # Ensure that we received exactly 12 bytes before unpacking
    if len(response) == 12:
        # Unpack the received byte array into floats
        received_floats = struct.unpack('<fff', response)  # Little-endian format
        print(f"Received floats: {received_floats}")
    else:
        print(f"Error: Expected 12 bytes but received {len(response)} bytes.")
    
    time.sleep(1)  # Sleep for 1 second before sending the next set of data
