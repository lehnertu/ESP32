/*
 *  Test of ADC for measurement of battary voltage
 */

#include "heltec.h"
#include "oled.hpp"

int count;
HeltecOLED *oled;

// ADC range is 0 - 1.10 V
// voltage divider is 0.3125
// full range is 3.52 V
// LSB voltage is 0.0008594 V
// attenuation factors [0dB 2.5dB 6dB 11dB ] [1.0 0.753 0.544 0.292]

// measured LSB voltage is 0.001523 V

void setup()
{
    // VextON() - switch on the voltage divider at the battery pin, otherwise the full voltage goes to the ADC
    // Vext = GPIO21
    pinMode(Vext, OUTPUT);
    digitalWrite(Vext, LOW);
    // pinMode(LED,OUTPUT);
    
    oled = new HeltecOLED();
    count = 0;
    // Heltec.begin(true /*DisplayEnable Enable*/, true /*Serial Enable*/);
    oled -> init();
    oled -> flipScreenVertically();
    oled -> setFont(ArialMT_Plain_10);

    analogReadResolution(12);         // set resolution 12-bit (0...4095)
    analogSetWidth(12);               // set sample bit width
    // we have board version 2.3 the battery voltage is atteched to GPIO37
    analogSetAttenuation(ADC_6db);    // 0=ADC_0db  3=ADC_11db
    analogSetCycles(8);               // averaging per sample default=8
    analogSetClockDiv(255);           // 26 kS/s
}

void loop()
{
    char message[32];
    
    count++;
    sprintf(message,"count %d", count);
    oled -> clearLine (0, 0);
    oled -> drawString(0, 0, message);

    uint32_t before = micros();
    uint16_t adc  =  analogRead(37);
    uint32_t after = micros();
    sprintf(message,"adc : %d", adc);
    oled -> clearLine (0, oled->fontHeight());
    oled -> drawString(0, oled->fontHeight(), message);
    sprintf(message,"time : %d µs", after-before);
    oled -> clearLine (0, 2* oled->fontHeight());
    oled -> drawString(0, 2* oled->fontHeight(), message);

    oled -> clearRect (100, 0, 38, 23);
    oled -> drawString(100, 0, "VBAT");
    sprintf(message,"%5.3f", 0.001523*adc);
    oled -> drawString(100, 10, message);

    oled -> display();
    delay(500);
    
}
