# Repository Guidelines

## Project Structure & Module Organization
`software/` contains all code contributors typically change. Main sketches are `software/doorbell_sender/doorbell_sender.ino` for the ESP32 button and `software/doorbell_receiver/doorbell_receiver.ino` for the ESP8266 chime. Helpers live in `software/autoflash/` and `software/espnow_test_sender/`. Hardware files and printable assets are under `hardware/` and `3d models/`. Repository-specific workflow notes are in `.github/copilot-instructions.md`.

## Build, Test, and Development Commands
Use `arduino-cli` or the Arduino IDE.

```bash
arduino-cli compile --fqbn esp8266:esp8266:d1_mini software/doorbell_receiver
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C3 software/doorbell_sender
arduino-cli upload -p COM3 --fqbn esp8266:esp8266:d1_mini software/doorbell_receiver
pip install -r software/autoflash/requirements.txt
python software/autoflash/autoflash.py Wemos_D1_Mini software/doorbell_receiver
python software/espnow_test_sender/serial_trigger.py --port COM3 --count 3
```

Compile before pushing. For normal setup, flash the receiver first, copy its printed MAC address into `broadcastAddress[]` in the sender sketch, then upload the sender.

## Coding Style & Naming Conventions
Match the existing file style instead of reformatting aggressively. Arduino sketches use braces on their own line in some functions, 2-space to 4-space indentation, and straightforward globals such as `doorbellMessage` and `broadcastAddress`. Python helpers use snake_case and standard library style. Keep board-specific code separated: ESP32 code stays in sender/test sender sketches; ESP8266 code stays in receiver sketches.

## Testing Guidelines
There is no automated unit test suite. Validation is hardware-focused:
- Confirm the receiver prints `ESP Board MAC Address` at `115200`.
- Send a test ring via the sender sketch or `serial_trigger.py`.
- Re-test any deep-sleep or ESP-NOW struct changes on real hardware before merging.

When changing `software/autoflash/`, verify board detection and a dry compile or upload flow.

## Commit & Pull Request Guidelines
Recent history uses short, imperative subjects like `Fix memory leak`, `Add autoflash script`, and `Adjust sender with my MAC address`. Keep commit titles concise, descriptive, and under about 72 characters.

Pull requests should state which device(s) were changed, list required board/FQBN assumptions, and describe how you verified the change. Include serial logs, photos, or short videos when the change affects flashing, wiring, LED behavior, audio playback, or enclosure fit.
