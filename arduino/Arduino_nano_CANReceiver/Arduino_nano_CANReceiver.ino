// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <CAN.h>

#define PRINT_STR 0

char buf[8];
int pos;
int cnt;

void setup() {
  Serial.begin(230400);
  while (!Serial);


  Serial.println("CAN Receiver");

  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
  Serial.println("Starting receiving CAN:");
}

void loop() {
  uint8_t b;
  
  // try to parse packet
  int packetSize = CAN.parsePacket();

  if (packetSize) 
  {
    // received a packet
    Serial.print("");

    if (CAN.packetExtended()) 
    {
      Serial.print("EX ");
    }

    if (CAN.packetRtr()) 
    {
      // Remote transmission request, packet contains no data
      Serial.print("RTR ");
    }

    Serial.print(millis());
    
    Serial.print(" ID 0x");
    Serial.print(CAN.packetId(), HEX);
    Serial.print(" LEN ");

    if (CAN.packetRtr()) 
    {
      Serial.print(CAN.packetDlc());
    } 
    else 
    {
      Serial.print(packetSize);
      Serial.print(" ");
      // only print packet data for non-RTR packets

      cnt = 0;
      
      while (CAN.available()) 
      {
        b = CAN.read();
        buf[cnt] = b;
        if (b < 16)
        {
          Serial.print("0");
        }
        Serial.print(b, HEX);
        Serial.print(":");
        cnt++;
      }
      for (pos=0; pos<cnt; pos++)
      {
#if PRINT_STR        
        Serial.print(buf[pos]);
        Serial.print(" ");
#endif
      }
    }
    Serial.println();
  }
}
