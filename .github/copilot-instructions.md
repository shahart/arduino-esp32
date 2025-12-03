# Copilot / AI Agent Instructions for arduino-esp32

This file contains focused, actionable guidance for AI coding agents working in this repository. Aim to be precise, conservative, and explicit about where to make changes.

Repository snapshot (key locations):
- `software/doorbell_sender/doorbell_sender.ino` — ESP32 sender firmware (deep-sleeping button, uses `esp_now.h`)
- `software/doorbell_receiver/doorbell_receiver.ino` — ESP8266 receiver firmware (RTTTL audio playback, uses `espnow.h`)
- `software/autoflash/` — small Python helper to detect boards and run `arduino-cli` uploads
- `hardware/doorbell/` — KiCad project and schematics

Big picture / architecture
- Two-device system: an external battery-powered ESP32 button (sender) and an internal USB-powered ESP8266 (receiver with speaker).
- Communication uses ESP-NOW (peer-to-peer) rather than Wi‑Fi TCP. The workflow: receiver prints its MAC, developer copies that MAC into the sender `broadcastAddress` array, then uploads sender firmware to the ESP32.
- Sender is deeply optimized for battery life: it uses RTC persistence (`RTC_DATA_ATTR`), calculates time since sleep and re-enters deep sleep quickly after sending a single ESP-NOW message. Be careful when editing sleep logic.
- Receiver acts as a slave role and triggers RTTTL playback when it receives a message. Audio is created in PROGMEM and played with `AudioGeneratorRTTTL` / `AudioOutputI2S`.

Developer workflows / commands
- Requirements:
  - `arduino-cli` available on PATH for automated uploads, or use the Arduino IDE.
  - `python` and `pyserial` for `autoflash.py` (install with `pip install -r software/autoflash/requirements.txt`).

- Typical flow to get a working pair:
  1. Upload `software/doorbell_receiver/doorbell_receiver.ino` to an ESP8266 (Wemos D1 Mini). Open Serial Monitor (115200). The receiver prints its MAC as:
     - `ESP Board MAC Address:  xx:xx:xx:xx:xx:xx`
  2. Copy that MAC and paste into `software/doorbell_sender/doorbell_sender.ino` replacing the `broadcastAddress[]` bytes array.
  3. Upload `software/doorbell_sender/doorbell_sender.ino` to the ESP32 board.

- Using `arduino-cli` (examples for bash on Windows):
  ```bash
  # Compile receiver (example fqbn for D1 mini)
  arduino-cli compile --fqbn esp8266:esp8266:d1_mini software/doorbell_receiver
  # Upload (replace /dev/ttyUSB0 or COM3 as appropriate)
  arduino-cli upload -p /dev/ttyUSB0 --fqbn esp8266:esp8266:d1_mini software/doorbell_receiver

  # Compile sender (example fqbn for XIAO_ESP32C3)
  arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C3 software/doorbell_sender
  arduino-cli upload -p /dev/ttyUSB1 --fqbn esp32:esp32:XIAO_ESP32C3 software/doorbell_sender
  ```

- Using the provided autoflash helper (auto-detects by VID/PID):
  ```bash
  pip install -r software/autoflash/requirements.txt
  python software/autoflash/autoflash.py Wemos_D1_Mini software/doorbell_receiver
  python software/autoflash/autoflash.py XIAO_ESP32C3 software/doorbell_sender
  ```
  - Add new board types by editing `software/autoflash/autoflash.py` `board_types` mapping (FQBN, VID, PID).

Autoflash: board examples and how to discover VID/PID
- The `software/autoflash/autoflash.py` contains a `board_types` dictionary with entries of the form:
  `"BoardName": [VID, PID, "fqbn:package:board"]`.

- Example entries you can add (copy/paste into `board_types`):
  - `"Wemos_D1_Mini": [0x1A86, 0x7523, "esp8266:esp8266:d1_mini"]`  # CH340-based Wemos/WeMos clones
  - `"NodeMCU_CP210x": [0x10C4, 0xEA60, "esp8266:esp8266:nodemcuv2"]`  # CP210x-based NodeMCU
  - `"FTDI_ESP8266": [0x0403, 0x6001, "esp8266:esp8266:d1_mini"]`     # FTDI-based serial adapters (example)
  - `"XIAO_ESP32C3": [0x303A, 0x1001, "esp32:esp32:XIAO_ESP32C3"]`   # Seeed XIAO ESP32-C3
  - `"ESP32_DevModule": [0x10C4, 0xEA60, "esp32:esp32:esp32dev"]`    # Generic ESP32 dev module (example VID for CP210x)

- Note about VID/PID accuracy: VIDs/PIDs vary by USB-serial chipset and vendor. The values above are common examples (CH340: `0x1A86:0x7523`, CP210x: `0x10C4:0xEA60`, FTDI: `0x0403:0x6001`). Always verify on your machine before adding a mapping.

- How to discover your board's VID/PID and an appropriate FQBN:
  - Use Python + pyserial to enumerate ports (this repo's helper already does this). Quick command:
    ```bash
    python -c "import serial.tools.list_ports as lp; print('\n'.join([str(p) for p in lp.comports()]))"
    ```
    Look for `VID:PID` or `hwid` fields in the output.
  - On Windows you can also check Device Manager > Ports (COMx) > Details > Hardware Ids.
  - To find the correct Arduino FQBN for your board, use `arduino-cli board list` (shows connected boards) or search platforms with `arduino-cli core search esp32` / `arduino-cli core search esp8266`.

- After you add an entry to `board_types`, run the autoflash helper like:
  ```bash
  python software/autoflash/autoflash.py NodeMCU_CP210x software/doorbell_receiver
  ```
  The script will scan connected COM ports and attempt to flash ports matching the VID/PID mapping.

+- If a board isn't detected, re-run the Python listing command above and confirm the VID/PID values and FQBN.

Repository-specific patterns & gotchas
- Cross-platform board mix: the receiver is ESP8266-based (`#include <ESP8266WiFi.h>`), while the sender targets ESP32 (`<WiFi.h>`, `<esp_now.h>`). Do not interchange libraries across these devices.
- Sender deep-sleep behavior:
  - `RTC_DATA_ATTR struct timeval sleep_enter_time;` is used to persist wake time across deep sleep cycles.
  - The code re-enters deep sleep unless sufficient time has passed; editing timing or the wake pin (currently `D1`) requires careful testing to avoid bricking the battery device in a tight wake loop.
  - The wake condition uses `esp_deep_sleep_enable_gpio_wakeup(BIT(D1), ESP_GPIO_WAKEUP_GPIO_LOW)`; when changing pins, keep GPIO numbering and ESP SDK semantics in mind.
- ESP-NOW usage:
  - Sender constructs `esp_now_peer_info_t peerInfo` and calls `esp_now_add_peer()` using the MAC bytes in `broadcastAddress` — this is a targeted peer send, not a broadcast. If you want true ESP-NOW broadcast, change peer setup accordingly.
  - Receiver uses `esp_now_register_recv_cb(OnDataRecv)` to populate `doorbellMessage` struct; the struct is a compact `bool play;` — maintain struct size and layout across both sketches.
- Serial/debugging:
  - Both sketches use `Serial.begin(115200)` and print clear markers (`starting`, `ESP Board MAC Address:`). For the sender, serial output is brief because the device sleeps soon after sending; add `delay()` during development if you need time to read logs.

Where to make changes (safe-edit checklist)
- Small functional edits (ring melody, RTTTL string) → `software/doorbell_receiver/doorbell_receiver.ino` (safe to change).
- Network/peer changes (MACs, esp-now behavior) → edit `software/doorbell_sender/doorbell_sender.ino` and `doorbell_receiver.ino` together to keep struct/layout compatibility.
- Board/CI automation → edit `software/autoflash/autoflash.py` (add FQBNs, VIDs/PIDs) and update `requirements.txt` as needed.

Testing & verification notes
- To verify receiver: open Serial Monitor at 115200 and confirm it prints the MAC. Then send a message from sender (or use `esp_now_send` test harness) and confirm the RTTTL plays.
- To verify sender: because it sleeps fast, plug into USB and keep Serial Monitor open before pressing the button. You may temporarily comment out `enterDeepSleep()` or increase `delay()` during development.

If you need me to:
- Update or extend the mapping in `software/autoflash/autoflash.py` to cover more boards
- Add a small test harness to emulate ESP-NOW messages from a PC (via `espnow_send` helper sketch)

Ask for feedback: tell me which parts you want expanded (e.g., board mapping, CI steps, or audio library notes).
