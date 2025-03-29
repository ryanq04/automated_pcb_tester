import time
import click
import cv2 as cv
import numpy as np
from serial import Serial
import datetime
import os

PORT = "COM8"
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
                frame = get_frame_ycbcr(ser, rows, cols, suffix)
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


def get_frame_ycbcr(ser: Serial, rows: int, cols: int, suffix: bytes = b"") -> np.array:
    """
    Get a frame from the serial port assuming YCbCr 4:2:2 format.
    Each pixel pair consists of 4 bytes: Y0, Cb, Y1, Cr.
    """
    rx_len = rows * cols * 2 + len(suffix)
    max_tries = 1000
    raw_img = b""

    for _ in range(max_tries):
        raw_img += ser.read(rx_len - len(raw_img))
        if len(raw_img) >= rx_len:
            break
    else:
        raise ValueError("Max tries exceeded.")

    raw_img = raw_img[: rows * cols * 2]

    # Reshape into (rows, cols/2, 4) because 4 bytes = 2 pixels
    img_ycbcr422 = np.frombuffer(raw_img, dtype=np.uint8).reshape((rows, cols // 2, 4))

    # Extract Y, Cb, Cr components
    Y0 = img_ycbcr422[:, :, 0].astype(np.float32)
    Cb = img_ycbcr422[:, :, 1].astype(np.float32)
    Y1 = img_ycbcr422[:, :, 2].astype(np.float32)
    Cr = img_ycbcr422[:, :, 3].astype(np.float32)

    # Combine Y values
    Y = np.zeros((rows, cols), dtype=np.float32)
    Y[:, 0::2] = Y0
    Y[:, 1::2] = Y1

    # Upsample Cb and Cr to full resolution
    Cb = np.repeat(Cb, 2, axis=1)
    Cr = np.repeat(Cr, 2, axis=1)

  

    # Convert to RGB
    R = Y + 1.402 * (Cr - 128)
    G = Y - 0.344136 * (Cb - 128) - 0.714136 * (Cr - 128)
    B = Y + 1.772 * (Cb - 128)

    # Clip to valid 0-255 range and convert to uint8
    R = np.clip(R, 0, 255).astype(np.uint8)
    G = np.clip(G, 0, 255).astype(np.uint8)
    B = np.clip(B, 0, 255).astype(np.uint8)

    # Stack as BGR for OpenCV display
    frame = np.stack((B, R, G), axis=-1)
    #frame = np.clip(frame, 0, 255).astype(np.uint8)
    return frame

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

output_dir = "calibration_images"
os.makedirs(output_dir, exist_ok=True)

def show_clickable_image(ser, img, rows, cols, suffix, preamble):
    """
    Displays the image and captures click positions. 
    - Press 'q' to quit
    - Press 's' to save image
    - Press 'w' to capture a new image
    Returns False if 'q' is pressed.
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
            print("Quitting...")
            cv.destroyAllWindows()
            return False

        elif key == ord('s'):
            timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
            filename = os.path.join(output_dir, f"saved_{timestamp}.jpg")
            cv.imwrite(filename, img)
            print(f"Image saved as {filename}")

        elif key == ord('w'):
            print("Capturing new image...")
            ser.reset_input_buffer()
            wait_for_preamble(ser, preamble)
            try:
                new_frame = get_frame_ycbcr(ser, rows, cols, suffix)
                img[:] = cv.resize(new_frame, (cols * 3, rows * 3), interpolation=cv.INTER_LINEAR)
                print("New image captured.")
            except ValueError:
                print("Failed to capture new image.")


if __name__ == "__main__":
    monitor()

"""
ensures script doesnt execute when imported
"""
