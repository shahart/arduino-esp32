# Doorbell #

This is a doorbell project with both an external component (battery powered) and internal component (USB powered).
Both components communicate using the ESPNOW protocol.
This project was part of the Makers972 meetup at Sparks 2025.

## What is in this repository?

* Firmware for the internal and external component
  
## Important assembly highlights

  1.Pay attention - Don't pinch the battery (button assembly can be dangerous)
  2.Carefully guid the led into the hole (right eye) while closing the printed robot
  3.Upload the "doorbell_reciver" to get the MAC address of the esp8266 (the one with the speaker)
  4.Update your doorbell_reciver MAC address into the code of the "doorbell_sender" and upload it to the esp32 (the one with the button and LED) 
