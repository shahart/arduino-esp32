import serial.tools.list_ports as list_ports
import argparse
import subprocess
import os
import shutil

VERBOSE = False

VID_INDEX = 0
PID_INDEX = 1
FQBN_INDEX = 2

board_types = {
     # board type : [VID, PID]
     "XIAO_ESP32C3": [0x303A, 0x1001, "esp32:esp32:XIAO_ESP32C3"],
     "Wemos_D1_Mini": [0x1A86, 0x7523, "esp8266:esp8266:d1_mini"] # WEMOS D1 mini uses CH430
}

def print_port_info(port_info):
            print(
f"""
Device: {port_info.device}
Name: {port_info.name}
Description: {port_info.description}
HWID: {port_info.hwid}
VID: {port_info.vid or 0:X}
PID: {port_info.pid or 0:X}
Serial Number: {port_info.serial_number}
Location: {port_info.location}
Manufacturer: {port_info.manufacturer}
Product: {port_info.product}
Interface: {port_info.interface}
"""
        )


def get_list_of_board_ports(board):
    ports = []
    vid = board_types[board][VID_INDEX]
    pid = board_types[board][PID_INDEX]
    for port_info in list_ports.comports():
        if VERBOSE:
              print_port_info(port_info)
        if port_info.vid == vid and port_info.pid == pid:
              ports.append(port_info.name)
    return ports

def flash_port(port, sketch_path, fqbn):
    subprocess.run(["arduino-cli", "upload", "-p", f"{port}", "--fqbn", fqbn, sketch_path], shell=True)

def main():
    global VERBOSE
    parser = argparse.ArgumentParser()
    parser.add_argument("--verbose", help="increase output verbosity", action="store_true")
    parser.add_argument("board", type=str, help="board type")
    parser.add_argument("sketch_path", type=str, help="path to sketch folder")
    args = parser.parse_args()
    VERBOSE = args.verbose

    if args.board not in board_types.keys():
        print("""Non-supported board type.
        Please choose one of the following""")

        for board in board_types.keys():
            print(board)
        return
    for port in get_list_of_board_ports(args.board):
         flash_port(port, args.sketch_path, board_types[args.board][FQBN_INDEX])


if __name__ == '__main__':
    main()