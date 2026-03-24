# Autonomous Doghouse Heating System

A fully autonomous embedded temperature control system designed to regulate a 12V heating pad inside an outdoor doghouse. Built from the ground up — from requirements and schematic capture through custom PCB fabrication, firmware development, and structured verification testing.

## System Overview

The system activates a heating pad when ambient temperature falls between 0°F and 40°F and deactivates it when the temperature rises above that range. A secondary sensor monitors the heating pad surface and limits output if it approaches 115°F, preventing thermal overshoot. Heating intensity is regulated using a proportional PWM algorithm.

**Key specs:**
- Activation range: 0°F to 40°F (±2°F tolerance)
- Pad temperature limit: 115°F
- Activation response time: <30 seconds
- Sensor accuracy: ±2°F (verified against calibrated IR thermometer)
- Input power: 12V DC (compatible with battery or solar supply)
- Total system power: <50W

## Hardware Architecture

| Component | Part | Purpose |
|---|---|---|
| Microcontroller | MSP430FR2310 | Control logic, ADC, PWM generation |
| Temperature sensors | TMP36 (x2) | Ambient air + heating pad surface |
| Power switching | AO3400A N-channel MOSFET | Low-side switch for heating pad |
| Power regulation | LM2594MX-3.3 buck converter | 12V → 3.3V for MCU and sensors |
| Heating element | 12V DC heating pad | Primary heat source |
| Protection | 4A fuse (load), 100mA fuse (logic) | Overcurrent protection |

Custom PCB designed in Altium Designer. Two-layer board integrating power regulation, microcontroller, dual sensing, and MOSFET switching into a single compact footprint.

## Firmware

Written in C for the MSP430FR2310 using Code Composer Studio. Key functions:

- `readTemperature()` — triggers ADC conversion on selected channel, converts raw ADC value to °F using TMP36 transfer function (10mV/°C, 500mV offset at 0°C)
- `controlHeater()` — evaluates ambient and pad temperatures against thresholds, calculates proportional PWM duty cycle
- `setPWM()` — configures Timer_A PWM output on P1.2; fully disconnects peripheral and drives pin low when duty cycle is 0 to ensure MOSFET is reliably off

## Verification Results

All specifications verified through structured testing across freezer, refrigerator, and outdoor environments.

| Specification | Requirement | Result |
|---|---|---|
| Sensor accuracy | ±2°F of reference | Max observed error: 0.7°F — PASS |
| Activation response | <60 seconds | Range: 5–25 seconds — PASS |
| Deactivation response | <60 seconds | Range: 8–35 seconds — PASS |
| Activation range | 0°F–40°F ±2°F | Correct in all 12 trials — PASS |

Full verification data and subsystem documentation available in `Final_Product_Report.pdf`.

## Repository Contents

| File | Description |
|---|---|
| `main.c` | MSP430 firmware source code |
| `TopFinal(Circuit Diagram).SchDoc` | Altium schematic |
| `TopFinal.PcbDoc` | Altium PCB layout |
| `TopViewPCB.png` | PCB layout top view |
| `Final_Product_Report.pdf` | Full design and verification report |
