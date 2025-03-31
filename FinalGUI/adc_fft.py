import numpy as np
import pyqtgraph as pg
from utils import wait_for_response

ADC_LENGTH = 2048
FFT_LENGTH = 2048
SAMPLE_RATE = 1_000_000

def generate_test_signal():
    t = np.linspace(0, ADC_LENGTH / SAMPLE_RATE, ADC_LENGTH)
    freq = np.random.uniform(1000, 100_000)
    wave_type = np.random.choice(['sine', 'square'])
    signal = np.sin(2 * np.pi * freq * t) if wave_type == 'sine' else np.sign(np.sin(2 * np.pi * freq * t))
    signal += 0.2 * np.random.randn(ADC_LENGTH)
    signal *= 4095.0 / 3.3
    return signal, freq, wave_type

def update_time_and_fft(signal, time_curve, fft_curve, fft_plot):
    t = np.arange(len(signal)) / SAMPLE_RATE
    signal_volts = signal * 3.3 / 4095.0
    time_curve.setData(t, signal_volts)

    fft = np.abs(np.fft.rfft(signal_volts))
    fft_db = 20 * np.log10(fft + 1e-12)
    freqs = np.fft.rfftfreq(len(signal), d=1 / SAMPLE_RATE)

    if fft_curve is None:
        fft_curve = pg.PlotDataItem(freqs, fft_db, pen='y')
        fft_plot.addItem(fft_curve)
    else:
        fft_curve.setData(freqs, fft_db)
    return fft_curve

def handle_uart_adc_fft(ser, time_curve, fft_curve, fft_plot):
    try:
        if not ser or not ser.is_open:
            print("Serial port not available.")
            return fft_curve

        ser.write(b"ADCFFT\r\n")
        if not wait_for_response(ser, "ADCFFT"): 
            print("No echo back for ADCFFT")
            return
        ser.reset_input_buffer()

        while ser.readline().decode().strip() != "ADC":
            pass

        adc_data = ser.read(ADC_LENGTH * 2)
        adc_values = np.frombuffer(adc_data, dtype=np.uint16)
        t = np.arange(len(adc_values)) / SAMPLE_RATE
        time_curve.setData(t, adc_values)

        while ser.readline().decode().strip() != "FFT":
            pass

        freqs = ser.read((FFT_LENGTH // 2) * 4)
        mags = ser.read((FFT_LENGTH // 2) * 4)
        frequencies = np.frombuffer(freqs, dtype='<f4')
        magnitudes = np.frombuffer(mags, dtype='<f4')

        magnitudes_db = 20 * np.log10((magnitudes * 3.3 / 4095.0) + 1e-12)
        if not np.isnan(magnitudes_db).any():
            if fft_curve is None:
                fft_curve = pg.PlotDataItem(frequencies, magnitudes_db, pen='y')
                fft_plot.addItem(fft_curve)
            else:
                fft_curve.setData(frequencies, magnitudes_db)

    except Exception as e:
        print(f"Error during UART communication: {e}")

    return fft_curve
