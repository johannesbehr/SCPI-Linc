import serial
import time
import argparse

def run_scpi_script(port, baudrate, filepath):
    with serial.Serial(port, baudrate, timeout=1) as ser:
        # wait till Arduino is ready, show greatings
        time.sleep(2.0) 
        while ser.in_waiting:
            response = ser.readline().decode('utf-8', errors='ignore').strip()
            if response:
                print(f"< {response}")
        with open(filepath, 'r') as file:
            for line in file:
                line = line.strip()

                # Skip empty lines and comments
                if not line or line.startswith("#") and not line.upper().startswith("#DELAY"):
                    continue

                # Handle delay directive
                if line.upper().startswith("#DELAY"):
                    try:
                        delay_ms = int(line.split()[1])
                        print(f"[Delay] {delay_ms} ms")
                        time.sleep(delay_ms / 1000.0)
                    except (IndexError, ValueError):
                        print(f"[Warning] Invalid delay syntax: {line}")
                    continue

                # Send SCPI command
                print(f"> {line}")
                ser.write((line + '\n').encode('utf-8'))

                # Read and print response (if any)
                time.sleep(0.2)
                while ser.in_waiting:
                    response = ser.readline().decode('utf-8', errors='ignore').strip()
                    if response:
                        print(f"< {response}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Run SCPI commands from a file over serial.")
    parser.add_argument("port", help="Serial port (e.g., COM3 or /dev/ttyUSB0)")
    parser.add_argument("file", help="Text file with SCPI commands")
    parser.add_argument("--baud", type=int, default=115200, help="Baudrate (default: 115200)")
    args = parser.parse_args()

    run_scpi_script(args.port, args.baud, args.file)
