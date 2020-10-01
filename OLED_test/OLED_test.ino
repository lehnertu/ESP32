/*
 *  Test of derived class of the Heltec OLED display
 */

#include "oled.hpp"

int count;
HeltecOLED *oled;

void setup() {
    oled = new HeltecOLED();
    count = 0;
    // Heltec.begin(true /*DisplayEnable Enable*/, true /*Serial Enable*/);
    oled -> init();
    oled -> flipScreenVertically();
    oled -> setFont(ArialMT_Plain_10);
}

void loop() {
    char message[32];
    
    count++;
    sprintf(message,"count %d", count);
    oled -> drawString(0,count % 30, message);
    oled -> display();
    delay(1000);
    
    oled -> clearLine (0, count % 30);
    oled -> display();
    delay(300);
}
