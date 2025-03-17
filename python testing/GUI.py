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
@click.option("-p", "--port", default=PORT, help="Serial (COM) port of the target board")
@click.option("-br", "--baudrate", default=BAUDRATE, help="Serial port baudrate")
@click.option("--timeout", default=1, help="Serial port timeout")
@click.option("--rows", default=ROWS, help="Number of rows in the image")
@click.option("--cols", default=COLS, help="Number of columns in the image")
@click.option("--preamble", default=PREAMBLE, help="Preamble string before the frame")
@click.option("--suffix", default=SUFFIX, help="Suffix string after receiving the frame")
def monitor(port: str, baudrate: int, timeout: int, rows: int, cols: int, preamble: str, suffix: str) -> None:
    """
    Display images transferred through serial port with click detection. Press 'q' to close.
    Press 'w' to capture a new image.
    """
    prev_frame_ts = None  # Keep track of frames per second
    click.echo(f"Opening communication on port {port} with baudrate {baudrate}")

    with Serial(port, baudrate, timeout=timeout) as ser:
        while True:
            click.echo("Waiting for input data...")
            wait_for_preamble(ser, preamble)
            click.echo("Receiving picture...")

            try:
                frame = get_frame(ser, rows, cols, suffix)
            except ValueError as e:
                click.echo(f"Malformed frame: {e}")
                continue

            now = time.time()
            if prev_frame_ts is not None:
                fps = 1 / (now - prev_frame_ts)
                click.echo(f"Frames per second: {fps:.2f}")
            prev_frame_ts = now

            enlarged_frame = cv.resize(frame, (cols * 3, rows * 3), interpolation=cv.INTER_LINEAR)
            if not show_clickable_image(ser, enlarged_frame, rows, cols, suffix, preamble):
                break

    cv.destroyAllWindows()

def wait_for_preamble(ser: Serial, preamble: str) -> None:
    """Wait for a preamble string in the serial port."""
    while True:
        try:
            if ser.readline().decode() == preamble:
                return
        except UnicodeDecodeError:
            pass

def get_frame(ser: Serial, rows: int, cols: int, suffix: bytes = b"") -> np.array:
    """Get a frame from the serial port."""
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

def show_clickable_image(ser, img, rows, cols, suffix, preamble):
    """
    Displays the image and captures click positions. Returns False if 'q' is pressed.
    Press 'w' to capture a new image.
    """
    def on_mouse_click(event, x, y, flags, param):
        if event == cv.EVENT_LBUTTONDOWN:
            print(f"Clicked at: ({x}, {y})")

    cv.namedWindow("Video Stream")
    cv.setMouseCallback("Video Stream", on_mouse_click)

    while True:
        cv.imshow("Video Stream", img)
        key = cv.waitKey(1) & 0xFF

        if key == ord('q'):
            cv.destroyAllWindows()
            return False
        elif key == ord('w'):
            print("Capturing new image...")
            ser.reset_input_buffer()
            wait_for_preamble(ser, preamble)
            try:
                new_frame = get_frame(ser, rows, cols, suffix)
                img[:] = cv.resize(new_frame, (cols * 3, rows * 3), interpolation=cv.INTER_LINEAR)
            except ValueError:
                print("Failed to capture new image.")
    return True

if __name__ == "__main__":
    monitor()

"""
ensures script doesnt execute when imported
"""