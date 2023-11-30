/**
 * @file CANReceiver.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief M5Unit CAN Receiver Example
 * @version 0.1
 * @date 2023-11-30
 * 
 * 
 * @Hardwares: M5Unit CAN
 * @Platform Version: Arduino M5Stack Board Manager v2.0.7
 * @Dependent Library:
 * arduino-CAN: https://github.com/sandeepmistry/arduino-CAN
 * M5Unified: https://github.com/m5stack/M5Unified
 */


#include "M5Unified.h"
#include <CAN.h>

#define CAN_TX_PIN 26
#define CAN_RX_PIN 36

void setup() {
    M5.begin();

    Serial.println("CAN Sender");

    CAN.setPins(CAN_RX_PIN, CAN_TX_PIN);

    // start the CAN bus at 500 kbps
    if (!CAN.begin(500E3)) {
        Serial.println("Starting CAN failed!");
        while (1)
            ;                   
    }
}

void loop() {
  // try to parse packet
  int packetSize = CAN.parsePacket();

  if (packetSize && CAN.packetId() != -1) {
    // received a packet
    Serial.print("Received ");

    if (CAN.packetExtended()) {
      Serial.print("extended ");
    }

    if (CAN.packetRtr()) {
      // Remote transmission request, packet contains no data
      Serial.print("RTR ");
    } 

    Serial.print("packet with id 0x");
    Serial.print(CAN.packetId(), HEX);

    if (CAN.packetRtr()) {
      Serial.print(" and requested length ");
      Serial.println(CAN.packetDlc());
    } else {
      Serial.print(" and length ");
      Serial.println(packetSize);

      // only print packet data for non-RTR packets
      while (CAN.available()) {
        Serial.print((char)CAN.read());
      }
      Serial.println();
    }

    Serial.println();
  }
}
