# Doorbell #

This is a doorbell project with both an external component (battery powered) and internal component (USB powered).
Both components communicate using the ESPNOW protocol.
This project was part of the Makers972 meetup at Sparks 2025.

## What is in this repository?

* Firmware for the internal and external component
  
## Important assembly highlights

  *  Pay attention - Don't pinch the battery (button assembly can be dangerous)
  *  Carefully guid the led into the hole (right eye) while closing the printed robot
  *  Upload the "doorbell_reciver" to get the MAC address of the esp8266 (the one with the speaker)
  *  Update your doorbell_reciver MAC address into the code of the "doorbell_sender" and upload it to the esp32 (the one with the button and LED) 

## Development / Debugging helpers

This repo includes a lightweight ESP-NOW test sender and a serial-trigger helper to aid development when hardware is limited.

- `software/espnow_test_sender/espnow_test_sender.ino` — a minimal ESP32 sketch that sends the doorbell message via ESP-NOW when you type `s` in Serial Monitor. Use this to test the receiver when you have an ESP32 available.

- `software/espnow_test_sender/serial_trigger.py` — a Python helper that opens a USB serial connection to the receiver and sends the `s` trigger (useful when you don't have an ESP32 sender). Install dependencies with:

```bash
pip install -r software/espnow_test_sender/requirements.txt
```

Usage example (Windows bash):

```bash
# Trigger once on COM3
python software/espnow_test_sender/serial_trigger.py --port COM3

# Trigger three times with half-second spacing
python software/espnow_test_sender/serial_trigger.py --port COM3 --count 3 --delay 0.5
```

Notes:
- The receiver sketch (`software/doorbell_receiver/doorbell_receiver.ino`) listens for a single-character `s` over serial and will play the ringtone when received. This is intended for development only and is safe to leave enabled while debugging over USB.
- For production deployments (battery-powered sender), use the ESP-NOW sender (`doorbell_sender.ino`) and the Wi-Fi peer workflow described in `.github/copilot-instructions.md`.

Also see `.github/copilot-instructions.md` for development workflows and board autoflash guidance.
