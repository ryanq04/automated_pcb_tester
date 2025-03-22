import time

import click
import cv2 as cv
import numpy as np
from serial import Serial

PORT = "COM5"
BAUDRATE = 115200

PREAMBLE = "PREAMBLE!\r\n"
SUFFIX = ""

ROWS = 144
COLS = 174


@click.command()
@click.option(
    "-p", "--port", default=PORT, help="Serial (COM) port of the target board"
)
@click.option("-br", "--baudrate", default=BAUDRATE, help="Serial port baudrate")
@click.option("--timeout", default=1, help="Serial port timeout")
@click.option("--rows", default=ROWS, help="Number of rows in the image")
@click.option("--cols", default=COLS, help="Number of columns in the image")
@click.option("--preamble", default=PREAMBLE, help="Preamble string before the frame")
@click.option(
    "--suffix", default=SUFFIX, help="Suffix string after receiving the frame"
)
def monitor(
    port: str,
    baudrate: int,
    timeout: int,
    rows: int,
    cols: int,
    preamble: str,
    suffix: str,
) -> None:
    """
    Display images transferred through serial port. Press 'q' to close.
    """
    prev_frame_ts = None  # keep track of frames per second

    click.echo(f"Opening communication on port {port} with baudrate {baudrate}")

    with Serial(port, baudrate, timeout=timeout) as ser:
        while True:
            #ser.reset_input_buffer()
            click.echo("Waiting for input data...")

            wait_for_preamble(ser, preamble)

            click.echo("Receiving picture...")

            try:
                frame = get_frame_rgb565(ser, rows, cols, suffix)
            except ValueError as e:
                click.echo(f"Malformed frame: {e}")
                continue

            now = time.time()
            if prev_frame_ts is not None:
                fps = 1 / (now - prev_frame_ts)
                click.echo(f"Frames per second: {fps:.2f}")
            prev_frame_ts = now

            cv.namedWindow("Video Stream", cv.WINDOW_KEEPRATIO)
            cv.imshow("Video Stream", frame)

            # Wait for 'q' to stop the program
            if cv.waitKey(1) == ord("q"):
                break

    cv.destroyAllWindows()


def wait_for_preamble(ser: Serial, preamble: str) -> None:
    """
    Wait for a preamble string in the serial port.
    """
    while True:
        try:
            if ser.readline().decode() == preamble:
                return
        except UnicodeDecodeError:
            pass


def get_frame_rgb565(ser: Serial, rows: int, cols: int, suffix: bytes = b"") -> np.array:
    """
    Get a frame from the serial port. --> new func for RGB565
    """
    rx_len = rows * cols * 2 + len(suffix) # 2 bytes per pixel
    max_tries = 1000
    raw_img = b""
   
    for _ in range(max_tries):
        #bytes_received = len(ser.read(rx_len - len(raw_img)))
        #print(f"Bytes received: {bytes_received}")
        raw_img += ser.read(rx_len - len(raw_img))

        if len(raw_img) >= rx_len:
            break
    else:
        raise ValueError("Max tries exceeded.")

    raw_img = raw_img[: rows * cols * 2]
        # Example Usage (After receiving `raw_img` from UART)
    


    
    # Convert raw bytes to RGB565 (16-bit) and then to RGB888
    img_rgb565 = np.frombuffer(raw_img, dtype="<u2").reshape((rows, cols))
    #r_img = reverse_bits_16bit(img_rgb565)
    # img_rgb565 = img_rgb565.byteswap()

    
    # Convert RGB565 to RGB888
    r = ((img_rgb565 >> 11) & 0x1F) * (255 // 31)
    g = ((img_rgb565 >> 5) & 0x3F) * (255 // 63)
    b = (img_rgb565 & 0x1F) * (255 // 31)
    
    # Stack to form an RGB image
    frame = np.stack([b, r, g], axis=-1).astype(np.uint8)
    #frame = np.stack([b, r, g], axis=-1).astype(np.uint8)
    
    return frame

import numpy as np

def reverse_raw_bits(raw_img: bytes) -> bytes:
    """
    Reverses the bit order of each byte in the raw byte stream.

    Parameters:
        raw_img (bytes): The original byte stream.

    Returns:
        bytes: A new byte stream with each byte's bits reversed.
    """
    # Lookup table for fast bit reversal (precomputed for all 256 possible bytes)
    bit_reverse_table = np.array([int(f'{i:08b}'[::-1], 2) for i in range(256)], dtype=np.uint8)

    # Convert bytes to NumPy array for efficient processing
    raw_array = np.frombuffer(raw_img, dtype=np.uint8)

    # Reverse bits using the lookup table
    reversed_array = bit_reverse_table[raw_array]

    # Convert back to bytes
    return reversed_array.tobytes()


def reverse_bits_16bit(arr: np.ndarray) -> np.ndarray:
    """
    Reverses the bit order in each 16-bit value of a NumPy array.

    Parameters:
        arr (np.ndarray): A NumPy array of shape (rows, cols) with dtype uint16.

    Returns:
        np.ndarray: A NumPy array with bit-reversed values, same shape and dtype.
    """
    # Create a lookup table for fast bit reversal of 8-bit values
    bit_reverse_table = np.array([int(f'{i:08b}'[::-1], 2) for i in range(256)], dtype=np.uint8)
    
    # Split each 16-bit number into two 8-bit halves
    high_byte = (arr >> 8) & 0xFF  # Extract high 8 bits
    low_byte = arr & 0xFF          # Extract low 8 bits
    
    # Reverse bits in each 8-bit part using the lookup table
    reversed_high = bit_reverse_table[low_byte]  # Reverse low byte first (becomes high)
    reversed_low = bit_reverse_table[high_byte]  # Reverse high byte (becomes low)

    # Combine reversed parts back into 16-bit values
    reversed_arr = (reversed_high << 8) | reversed_low
    
    return reversed_arr

def get_frame(ser: Serial, rows: int, cols: int, suffix: bytes = b"") -> np.array:
    """
    Get a frame from the serial port.
    """
    rx_len = rows * cols + len(suffix)
    max_tries = 1000
    raw_img = b""

    for _ in range(max_tries):
        raw_img += ser.read(rx_len - len(raw_img))

        if len(raw_img) >= rx_len:
            break
    else:
        raise ValueError("Max tries exceeded.")

    raw_img = raw_img[: rows * cols]

    return np.frombuffer(raw_img, dtype=np.uint8).reshape((rows, cols, 1))


def get_frame_rgb(ser: Serial, rows: int, cols: int, suffix: bytes = b"") -> np.array:
    """
    Get a frame from the serial port.
    """
    rx_len = rows * cols * 3 + len(suffix)
    print(f"Expected data length (rx_len): {rx_len}") 
    
    max_tries = 1000
    raw_img = b""

    for _ in range(max_tries):
        
        bytes_received = len(ser.read(rx_len - len(raw_img)))
        print(f"Bytes received: {bytes_received}")
        
        raw_img += ser.read(rx_len - len(raw_img))

        if len(raw_img) >= rx_len:
            print(f"Received sufficient data: {len(raw_img)} bytes")
            break
    else:
        raise ValueError("Max tries exceeded.")

    raw_img = raw_img[: rows * cols * 3]
    
    frame = np.frombuffer(raw_img, dtype=np.uint8).reshape((rows, cols, 3))

    return frame


if __name__ == "__main__":
    monitor()
