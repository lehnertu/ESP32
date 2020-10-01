/* 
  Check the new incoming messages, and print via serialin 115200 baud rate.
  
  by Aaron.Lee from HelTec AutoMation, ChengDu, China
  成都惠利特自动化科技有限公司
  www.heltec.cn
  
  this project also realess in GitHub:
  https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series
*/

#include "heltec.h"
#include "oled.hpp"

#define BAND    868E6  //you can set band here directly,e.g. 433E6,915E6

HeltecOLED *oled;

void setup()
{
    Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
    oled = new HeltecOLED();
    oled -> init();
    oled -> flipScreenVertically();
    oled -> setFont(ArialMT_Plain_10);
}

void loop()
{
    char line[32];
    char *lp;

    // try to parse packet
    digitalWrite(25, HIGH);     // turn the LED on
    // this blocks until a packet is received
    int packetSize = LoRa.parsePacket();
    digitalWrite(25, LOW);      // turn the LED off
    if (packetSize)
    {
        oled -> clear();
        // received a packet
        // Serial.print("Received packet '");
        sprintf(line,"Received packet");
        oled -> drawString(0, 0, line);
        // read packet and copy into line
        lp = line;
        while (LoRa.available() and lp<line+32)
        {
            // Serial.print((char)LoRa.read());
            *lp++ = (char)LoRa.read();
        }
        *lp = '\0';
        oled -> drawString(0, 13, line);
        // print RSSI of packet
        // Serial.print("' with RSSI ");
        // Serial.println(LoRa.packetRssi());
        sprintf(line,"RSSI : %d", LoRa.packetRssi());
        oled -> drawString(0, 26, line);
        oled -> display();
    } 
}
