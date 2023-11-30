/**
 * @file CANSender.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief M5Unit CAN Sender Example
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
    // send packet: id is 11 bits, packet can contain up to 8 bytes of data
    Serial.print("Sending packet ... ");

    CAN.beginPacket(0x12);
    CAN.write('h');
    CAN.write('e');
    CAN.write('l');
    CAN.write('l');
    CAN.write('o');
    CAN.endPacket();

    Serial.println("done");

    delay(1000);

    // send extended packet: id is 29 bits, packet can contain up to 8 bytes of
    // data
    Serial.print("Sending extended packet ... ");

    CAN.beginExtendedPacket(0xabcdef);
    CAN.write('w');
    CAN.write('o');
    CAN.write('r');
    CAN.write('l');
    CAN.write('d');
    CAN.endPacket();

    Serial.println("done");

    delay(1000);
}
