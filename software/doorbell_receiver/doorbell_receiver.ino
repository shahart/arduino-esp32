#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorRTTTL.h"
#include "AudioOutputI2S.h"
#include <ESP8266WiFi.h>
#include <espnow.h>

const char ring[] PROGMEM = 
"Star Wars:d=8,o=6,b=180:f5,f5,f5,2a#5.,2f.,d#,d,c,2a#.,4f.,d#,d,c,2a#.,4f.,d#,d,d#,2c,4p,f5,f5,f5,2a#5.,2f.,d#,d,c,2a#.,4f.,d#,d,c,2a#.,4f.,d#,d,d#,2c";
// Plenty more at: http://mines.lumpylumpy.com/Electronics/Computers/Software/Cpp/MFC/RingTones.RTTTL

struct doorbell_message_t {
    bool play;
};

doorbell_message_t doorbellMessage;

AudioGeneratorRTTTL *rtttl;
AudioFileSourcePROGMEM *file;
AudioOutputI2S *out;
bool isPlaying = false;

void playRing()
{
  file = new AudioFileSourcePROGMEM(ring, strlen_P(ring));
  out = new AudioOutputI2S();
  rtttl = new AudioGeneratorRTTTL();
  rtttl->begin(file, out);
  isPlaying = true;
}

void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&doorbellMessage, incomingData, sizeof(doorbellMessage));
  Serial.print("Bytes received: ");
  Serial.println(len);
  // Serial.print("Ring name: ");
  // Serial.println(doorbellMessage.ring_name);
  Serial.print("should play: ");
  Serial.println(doorbellMessage.play);
  Serial.println();
}



void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  doorbellMessage.play = false;

  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);

  Serial.printf("RTTTL start\n");

  audioLogger = &Serial;

}

void loop()
{
  if (isPlaying && rtttl->isRunning()) {
    if (!rtttl->loop())  
    {
      rtttl->stop();
      isPlaying = false;
    }
  } else {
    delay(1000);
    if (doorbellMessage.play) {
      doorbellMessage.play = false;
      playRing();
    }
  }
}

