// command the esp8266 to sent its MAC address via uart when starting
// set baud rate of serial comm to 115200

#include <ESP8266WiFi.h>

void setup() {
  //start serial conn
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);  // Set WiFi to station mode
  delay(100);           // Short delay to ensure WiFi mode is set
  Serial.println("wifi began");
  
  // Get the MAC address
  String macAddress = WiFi.macAddress();
  
  // Print the MAC address to the Serial Monitor
  Serial.println("\n MAC Address: " + macAddress);
}

bool firstRunStarted = true;
void loop() {

}
