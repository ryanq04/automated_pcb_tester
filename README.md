# Automated PCB Tester

### University of Toronto — ECE342 Embedded Systems  
**Ryan Qi & Avani Yadav**

We designed and built an **automated PCB testing prototype** inspired by **In-Circuit Test (ICT)** systems and **flying probe testers** used to verify printed circuit boards on production lines.

The system automatically positions test probes over a PCB and communicates with a PC-based user interface for control and testing.

This project was completed in approximately **80 hours** as our final project for **ECE342: Embedded Systems** at the University of Toronto.

---

## Hardware

The system is built around the following components:

- **STM32F446ZE** microcontroller
- **OV7670** camera
- **NEMA 17** stepper motor
- **SG90** servo motors
- **PCA9685** servo driver
- **L298N** stepper motor driver

---

## Software & Communication

- **Embedded firmware:** C
- **PC user interface:** Python
- **Communication:** UART

The STM32 handles the real-time motor, servo, camera, and probing control, while the Python application provides the user interface and communicates with the tester over UART.

---

## Demo

### Automated Testing / Signal Analysis

**Click the image below to watch the demo.**

<p align="center">
  <a href="https://www.youtube.com/shorts/35WJPPRGhOk">
    <img src="adc_fft.jpg" alt="Automated PCB Tester Demo" width="700"/>
  </a>
</p>

### Probe Movement

**Click the image below to watch the probe system in action.**

<p align="center">
  <a href="https://www.youtube.com/shorts/IuJ4Q_L5Wx0">
    <img src="ece342_probe.jpg" alt="PCB Probe Movement Demo" width="700"/>
  </a>
</p>

---

## Project Overview

The goal of this project was to prototype a low-cost automated PCB testing platform capable of performing repeatable electrical tests without requiring an operator to manually probe individual test points.

The design combines embedded motion control, computer vision hardware, PC-to-microcontroller communication, and a desktop user interface into a single automated testing system.
