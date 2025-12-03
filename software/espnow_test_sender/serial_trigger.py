#!/usr/bin/env python3
"""
serial_trigger.py

Simple helper to trigger the doorbell receiver over its USB serial connection.
This sends the character `s` (followed by newline) which the development build
of `doorbell_receiver.ino` listens for and uses to play the ringtone.

Usage:
  python serial_trigger.py --port COM3
  python serial_trigger.py --port /dev/ttyUSB0 --count 3 --delay 0.5

Requires: pyserial (pip install pyserial)
"""

import argparse
import time
import serial


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", required=True, help="Serial port (COMx or /dev/ttyUSBx)")
    parser.add_argument("--baud", type=int, default=115200, help="Serial baud rate")
    parser.add_argument("--count", type=int, default=1, help="How many times to send the trigger")
    parser.add_argument("--delay", type=float, default=0.2, help="Delay between triggers (seconds)")
    args = parser.parse_args()

    print(f"Opening serial port {args.port} @ {args.baud}")
    with serial.Serial(args.port, args.baud, timeout=1) as ser:
        # Give the board a moment to reset / settle
        time.sleep(0.2)
        for i in range(args.count):
            ser.write(b"s\n")
            print(f"Sent trigger {i+1}/{args.count}")
            time.sleep(args.delay)

    print("Done")


if __name__ == '__main__':
    main()
