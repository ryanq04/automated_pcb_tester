import time
import struct

def wait_for_response(ser, expected, timeout=5):
    start_time = time.time()
    while time.time() - start_time < timeout:
        try:
            line = ser.readline().decode().strip()
            print(f"Received: '{line}'")
            if line == expected:
                return True
        except UnicodeDecodeError:
            continue
    print(f"Timeout waiting for '{expected}'")
    return False

def wait_for_float_echo(ser, expected_x, expected_y, timeout=5):
    start_time = time.time()
    while time.time() - start_time < timeout:
        echoed = ser.read(8)
        if len(echoed) == 8:
            echoed_x, echoed_y = struct.unpack('<ff', echoed)
            print(f"Echoed: ({echoed_x:.2f}, {echoed_y:.2f})")
            if abs(echoed_x - expected_x) < 0.01 and abs(echoed_y - expected_y) < 0.01:
                return True
    print("Timeout or mismatch for float echo.")
    return False
