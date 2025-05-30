# SCPI Runner

**scpi_runner.py** is a simple Python script to automate communication with an Arduino (or similar device) running the SCPI-Linc interpreter via a serial connection. It reads SCPI commands from a text file and sends them to the serial port, optionally including delays between commands.

## Features

- Send SCPI commands from a text file to a serial device
- Supports `DELAY <ms>` to wait between commands
- Displays responses from the device
- Simple and lightweight for scripting test routines

## Requirements

- Python 3.x
- `pyserial` package

Install with:

```bash
pip install pyserial
````

## Usage

```bash
python scpi_runner.py <port> <script.txt>
```

* `<port>`: Serial port of the device (e.g. `COM3`, `/dev/ttyUSB0`)
* `<script.txt>`: Text file containing SCPI commands and optional delays

## Example Script File

```text
*IDN?
DELAY 500
I2C:ADDR 3C
DELAY 200
I2C:WRITE 80 AF 80 A4
```

## Output

The script will show sent commands (`>`) and received responses (`<`):

```text
> *IDN?
< SCPI-Linc v1.0
> I2C:ADDR 3C
< OK
```

## License

MIT License
Developed as part of the SCPI-Linc project.
---
