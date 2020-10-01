/*
 * https://github.com/teebr/Influx-Arduino.git
 */

#include "oled.hpp"
#include <WiFi.h>
#include "InfluxArduino.hpp"
// include "InfluxCert.hpp"

InfluxArduino influx;
//connection/ database stuff that needs configuring
const char WIFI_NAME[] = "lehnert";
const char WIFI_PASS[] = "4pilence";
const char INFLUX_DATABASE[] = "lora";
const char INFLUX_HOST[] = "homeserv";
// const char INFLUX_USER[] = ""; //username if authorization is enabled.
// const char INFLUX_PASS[] = ""; //password for if authorization is enabled.
const char INFLUX_MEASUREMENT[] = "test"; //measurement name for the database. (in practice, you can use several, this example just uses the one)

unsigned long count;
HeltecOLED *oled;

void WIFISetUp(void)
{
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.disconnect(true);
    delay(1000);
    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect(true);
    WiFi.begin(WIFI_NAME, WIFI_PASS);
    delay(100);
  
    oled -> clear();
    oled -> drawString(0, 0, "Connecting");
    byte count = 0;
    while(WiFi.status() != WL_CONNECTED && count<15)
      {
        count ++;
        char line[16];
        line[15] = '\0';
        for (int i=0; i<15; i++) line[i]=' ';
        for (int i=0; i<count; i++) line[i]='.';
        oled -> drawString(0, oled->fontHeight(), line);
        oled -> display();
        delay(2000);
      }
    oled -> clearLine (0, oled->fontHeight());
    if(WiFi.status() == WL_CONNECTED)
      {
        oled -> drawString(0, oled->fontHeight(), "WIFI Setup OK.");
        oled -> display();
      } else {
        oled -> drawString(0, oled->fontHeight(), "WIFI Setup failed.");
        oled -> display();
      }
}

void setup()
{   
    Serial.begin(115200);
    
    oled = new HeltecOLED();
    count = 0;
    // Heltec.begin(true /*DisplayEnable Enable*/, true /*Serial Enable*/);
    oled -> init();
    oled -> flipScreenVertically();
    oled -> setFont(ArialMT_Plain_10);

    WIFISetUp();
    delay(500);
    
    influx.configure(INFLUX_DATABASE, INFLUX_HOST); //third argument (port number) defaults to 8086

    // influx.authorize(INFLUX_USER,INFLUX_PASS); //if you have set the Influxdb .conf variable auth-enabled to true, uncomment this
    // influx.addCertificate(ROOT_CERT); //uncomment if you have generated a CA cert and copied it into InfluxCert.hpp
    // Serial.print("Using HTTPS: ");
    // Serial.println(influx.isSecure()); //will be true if you've added the InfluxCert.hpp file.
} 

void loop()
{   
    char message[32];
    count++;

    // just ping the database server
    int response = influx.ping();
    sprintf(message,"ping response %d", response);
    oled -> clearLine (0, 2*oled->fontHeight());
    oled -> drawString(0, 2*oled->fontHeight(), message);
    oled -> display();

    delay(1000);
    
    //write our variables.
    char tags[32];
    char fields[32];
    sprintf(tags,"source=ESP32_1");
    sprintf(fields,"count=%ld", count);
    bool writeSuccessful = influx.write(INFLUX_MEASUREMENT, tags, fields);
    if(writeSuccessful)
      {
        sprintf(message,"DB  write (%ld) OK.", count);
      } else {
        sprintf(message,"DB (%ld) error: %d", count, influx.getResponse());
      }
    oled -> clearLine (0, 3*oled->fontHeight());
    oled -> drawString(0, 40, message);
    oled -> display();
    
    delay(1000);
}
