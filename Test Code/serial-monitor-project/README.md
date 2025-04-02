# Serial Monitor

Python application to visualize incoming images from serial port.

## Requirements

- Python 3.10 with PIP.

## Installation

The recommended method is to install in a virtualenv with `venv`.

- (First time only) Create a virtualenv with `python3 -m venv ./venv`.
- Activate the virtualenv with `source ./venv/bin/activate` on Mac/Linux or `./venv/scripts/activate` on Windows.
- Install dependencies with `pip install -r requirements.txt`.

### Generate executable

These steps are only necessary if you want to generate an `.exe` file to run the application.
You may skip these steps if you want to run the serial monitor directly from source code.

- Run `pyinstaller serial_monitor.py`.
- The Windows/Linux executable will be found inside `dist/serial_monitor/`.

## Usage

### Run from source

- Run with `python serial_monitor.py`

### Run from executable

- Run executable from `dist/serial_monitor`.
