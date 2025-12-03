/*
  espnow_test_sender.ino

  Simple ESP-NOW test sender for debugging.

  - Update `broadcastAddress` with the receiver MAC (or use an example MAC).
  - Upload this sketch to an ESP32 development board.
  - Open Serial Monitor (115200) and type `s` + Enter to send the doorbell message.

  This is intentionally minimal and does NOT enter deep sleep.
*/

#include <esp_now.h>
#include <WiFi.h>

// Replace with the receiver MAC address printed by the receiver's Serial Monitor
uint8_t broadcastAddress[] = { 0x48, 0x3F, 0xDA, 0x66, 0xE3, 0xDD };

struct doorbell_message_t {
  bool play;
};

doorbell_message_t doorbellMessage;

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("ESP-NOW Test Sender starting");

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);

  // Register peer (target receiver)
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Warning: failed to add peer (may already exist)");
  } else {
    Serial.println("Peer added");
  }

  Serial.println("Type 's' + ENTER to send a doorbell message to the configured MAC.");
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.equalsIgnoreCase("s")) {
      doorbellMessage.play = true;
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&doorbellMessage, sizeof(doorbellMessage));
      if (result == ESP_OK) {
        Serial.println("esp_now_send() returned ESP_OK");
      } else {
        Serial.print("esp_now_send() failed: ");
        Serial.println(result);
      }
    }
  }
  delay(10);
}
