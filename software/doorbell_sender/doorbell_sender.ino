#include <esp_now.h>
#include <WiFi.h>

static RTC_DATA_ATTR struct timeval sleep_enter_time;

uint8_t broadcastAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

struct doorbell_message_t {
  bool play;
};

doorbell_message_t doorbellMessage;

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void enterDeepSleep()
{
  esp_deep_sleep_enable_gpio_wakeup(BIT(D1), ESP_GPIO_WAKEUP_GPIO_LOW);
  esp_deep_sleep_start();
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  Serial.println("starting");

  pinMode(D5, OUTPUT); // LED


  if (ESP_SLEEP_WAKEUP_GPIO != esp_sleep_get_wakeup_cause()) 
  {
    gettimeofday(&sleep_enter_time, NULL);
    enterDeepSleep();
  }

  struct timeval now;
  gettimeofday(&now, NULL);
  int sleep_time_ms = (now.tv_sec - sleep_enter_time.tv_sec) * 1000 + (now.tv_usec - sleep_enter_time.tv_usec) / 1000;
  
  if (sleep_time_ms < 500) {
    enterDeepSleep();
  }

  digitalWrite(D5, HIGH);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // doorbellMessage.ring_name = "star wars";
  doorbellMessage.play = true;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&doorbellMessage, sizeof(doorbellMessage));
  digitalWrite(D5, LOW);
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }
  delay(10); //Take some time to open up the Serial Monitor
  //Go to sleep now
  Serial.println("Going to sleep now");
  gettimeofday(&sleep_enter_time, NULL);
  enterDeepSleep();
}

void loop() {
}