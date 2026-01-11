# 3D Printer Extruder System

A precision extruder system designed for a 2x2x2 bilateral 3D printer capable of processing HDPE plastic filament with accurate temperature control and variable extrusion speed.

## 🖼️ Preview

![Extruder System](/assets/extruder.webp)

## Overview

This project implements a custom extruder system for a bilateral 3D printer configuration. The system features precise temperature control for HDPE plastic extrusion and variable speed control via a NEMA23 stepper motor, enabling high-quality prints with consistent material flow.

## Key Features

- **Temperature Control**: Precise thermal regulation for optimal HDPE plastic melting
- **Variable Speed Extrusion**: NEMA23 stepper motor with adjustable RPM for controlled material flow
- **Bilateral Printing Support**: Designed specifically for 2x2x2 bilateral printer architecture
- **HDPE Optimized**: Engineered for High-Density Polyethylene printing applications
- **Arduino-Based Control**: Open-source firmware for reliable operation

## Technical Specifications

- **Motor Type**: NEMA23 Stepper Motor
- **Material Compatibility**: HDPE (High-Density Polyethylene)
- **Operating Temperature Range**: 180°C - 250°C (adjustable)
- **Extrusion Speed Range**: Variable (based on stepper motor control)
- **Control Interface**: Arduino-based microcontroller
- **Filament Diameter**: Standard 1.0mm

## Hardware Requirements

- NEMA23 Stepper Motor
- Arduino-compatible microcontroller board
- Temperature sensor (NTC 100K)
- Heating element
- Extruder body/hot end assembly
- Motor driver DM542
- Power supply appropriate for heating elements and motors
- Wiring and connectors

## Software Requirements

- Arduino IDE or compatible development environment
- Required libraries (as specified in the .ino file)
- USB cable for programming and communication

## Installation

1. Connect the NEMA23 stepper motor to the appropriate motor driver
2. Wire the temperature sensor to the designated analog pin
3. Connect the heating element to the PWM-controlled power output
4. Upload the firmware (`src/extruder.ino`) to your Arduino board
5. Calibrate temperature and motor settings according to your specific hardware

## Configuration

The system parameters can be adjusted in the `extruder.ino` file:

- Temperature setpoints for different materials
- Stepper motor steps per millimeter
- PID controller values for temperature regulation
- Acceleration and speed profiles for smooth operation

## Usage

1. Power on the system and allow it to initialize
2. Set the desired temperature for HDPE extrusion (typically 220-250°C)
3. Wait for temperature stabilization
4. Configure extrusion speed based on your print requirements
5. Begin extrusion by sending appropriate commands via serial interface or integrated printer controller

## Calibration

For optimal performance:

- Calibrate steps per millimeter for precise material feed
- Tune Hysterysys control for stable temperature control
- Test extrusion rates at various speeds and temperatures
- Verify consistent material flow across different temperature settings

## Troubleshooting

### Common Issues

- **Temperature Fluctuations**: Check Hystserysys tuning values and ensure proper sensor placement
- **Inconsistent Extrusion**: Verify motor steps calibration and check for mechanical binding
- **Material Clogging**: Clean nozzle regularly and verify temperature settings are appropriate

### Maintenance

- Regular cleaning of the hot end to prevent material buildup
- Check electrical connections periodically
- Lubricate mechanical components as needed
- Inspect wiring for wear or damage

## Contributing

Contributions to improve the extruder system are welcome. Please submit pull requests with detailed descriptions of changes or additions.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Authors

- [Priyo Adi Wibowo] - Initial work on the extruder system

## Acknowledgments

- Inspiration from the open-source 3D printing community
- Hardware and software libraries that made this project possible