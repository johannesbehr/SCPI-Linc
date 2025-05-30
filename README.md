# SCPI-Linc

**SCPI-Linc** is a lightweight SCPI (Standard Commands for Programmable Instruments) command interpreter for Arduino, enabling control over I2C and SPI buses via a serial terminal. It is designed for simple test setups and embedded debugging through familiar SCPI-style commands.

## Features

* SCPI-style command parsing via serial terminal
* I2C communication (set address, write, read, scan)
* SPI communication (write/read bytes)
* Basic device identification and reset commands
* Built-in help command
* Watchdog integration for reliability

## Example Commands

```text
*IDN?                       # Identify device
*RST                        # Reset state
I2C:ADDR 3C                 # Set I2C address to 0x3C
I2C:WRITE AA BB CC          # Write bytes to I2C device
I2C:READ? 4                 # Read 4 bytes from I2C device
I2C:SCAN?                   # Scan I2C bus for devices
SPI:WRITE 01 02 03          # Send and receive SPI bytes
SPI:READ? 3                 # Read 3 SPI bytes (dummy write)
```

## Requirements

* Arduino board (tested on ATmega328-based boards)
* Arduino IDE or PlatformIO
* Serial terminal at 115200 baud

## Getting Started

1. Clone or download this repository.
2. Open `SCPI-Linc.ino` in the Arduino IDE.
3. Upload to your board.
4. Open a serial monitor (115200 baud).
5. Send commands to interact with I2C/SPI devices.

## License

MIT License
