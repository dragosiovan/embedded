// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <CAN.h>

byte rx;


void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("CAN Receiver");

  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
}

void loop() {
  // try to parse packet
  int packetSize = CAN.parsePacket();

  if (packetSize) {
    // received a packet
    //Serial.print("Received ");

    if (CAN.packetExtended()) {
      Serial.print("extended ");
    }

    if (CAN.packetRtr()) {
      // Remote transmission request, packet contains no data
      Serial.print("RTR ");
    }

    Serial.print(millis());
    Serial.print(" ");
    //Serial.print("packet with id 0x");
    Serial.print("Id 0x");
    Serial.print(CAN.packetId(), HEX);

    if (CAN.packetRtr()) {
      Serial.print(" and requested length ");
      Serial.println(CAN.packetDlc());
    } else {
//      Serial.print(" and length ");
      Serial.print(" ");
//      Serial.println(packetSize);
      Serial.print(packetSize);
      Serial.print(" ");

      // only print packet data for non-RTR packets
      while (CAN.available()) {
        rx = CAN.read();
        //Serial.print((char)CAN.read());
        if (rx < 16)
          Serial.print("0");
        Serial.print(rx, HEX);
        Serial.print(":");
      }
//      Serial.println();
    }

    Serial.println();
  }
}
