
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "heltec.h" // alias for `#include "SSD1306Wire.h"`
#include "WiFi.h"

void WIFISetUp(void)
{
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect(true);
  WiFi.begin("lehnert","4pilence");
  delay(100);

  Heltec.display -> clear();
  Heltec.display -> drawString(0, 0, "Connecting");
  byte count = 0;
  while(WiFi.status() != WL_CONNECTED && count<15)
  {
    count ++;
    char line[16];
    line[15] = '\0';
    for (int i=0; i<15; i++) line[i]=' ';
    for (int i=0; i<count; i++) line[i]='.';
    Heltec.display -> drawString(0, 10, line);
    Heltec.display -> display();
    delay(500);
  }
  if(WiFi.status() == WL_CONNECTED)
  {
    Heltec.display -> drawString(0, 20, "WIFI Setup OK.");
    Heltec.display -> display();
  } else {
    Heltec.display -> drawString(0, 20, "WIFI Setup failed.");
    Heltec.display -> display();
  }
  delay(500);
}


void setup()
{

  pinMode(LED,OUTPUT);
  digitalWrite(LED,HIGH);

  delay(2000);

  Heltec.begin(true /*DisplayEnable Enable*/, true /*Serial Enable*/);
  Heltec.display->init();
  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_10);

  delay(2000);

  WIFISetUp();

  digitalWrite(LED,LOW);

}

void loop()
{

  delay(500);
  digitalWrite(LED,HIGH);
  delay(500);
  digitalWrite(LED,LOW);

}
