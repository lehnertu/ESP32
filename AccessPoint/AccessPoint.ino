#include "heltec.h"
#include <WiFi.h>
#include "FS.h"
#include "SPIFFS.h"
#include "oled.hpp"

// todo:
// give the access point a custom name "LoRaFC-Ground"
// give the host a name "ATC"
// display IP number and name on OLED

// OLED display
HeltecOLED *oled;

// web server
WiFiServer *server;

// Variable to store the template/response page
String templ;
String page;

// Auxiliar variables to store the current output state
int LEDState = 0;
volatile int countISR = 0;
volatile int uptime = 0;
static hw_timer_t * timer = NULL;
static portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer()
{
    // disable ISR and other tasks
    portENTER_CRITICAL_ISR(&timerMux);
    countISR++;
    if (countISR>=1000)
    {
        countISR = 0;
        uptime++;
    };
    portEXIT_CRITICAL_ISR(&timerMux);
}

// handle the HTTP request
// The internal state changes depending on the commands present in the request.
String request;
void handle_request()
{
    Serial.println("request:");
    Serial.print(request);
    Serial.println();
    // turn the GPIO on/off
    if (request.indexOf("GET /25/on") >= 0)
    {
        LEDState = 1;
        digitalWrite(LED, HIGH);
    };
    if (request.indexOf("GET /25/off") >= 0)
    {
        LEDState = 0;
        digitalWrite(LED, LOW);
    };
}

// fill the information for the current status
// into the response template
void fill_template()
{
    char line[80];
    page = templ;
    sprintf(line,"up time %d:%02d<br>\n",uptime / 60, uptime % 60);
    page.replace("%UPTIME%",line);
    if (LEDState)
    {
        page.replace("%GPIO25STATE%","<p>GPIO 25 - State ON </p>");
        page.replace("%GPIO25BUTTON%","<p><a href=\"/25/off\"><button class=\"button\">OFF</button></a></p>");
    }
    else
    {
        page.replace("%GPIO25STATE%","<p>GPIO 25 - State OFF </p>");
        page.replace("%GPIO25BUTTON%","<p><a href=\"/25/on\"><button class=\"button button2\">ON</button></a></p>");
    }
}

void setup()
{
    // init display
    oled = new HeltecOLED();
    oled -> init();
    oled -> flipScreenVertically();
    oled -> setFont(ArialMT_Plain_10);
    oled -> drawString(0, 0, "Access Point");
    oled -> display();

    // use timer 0, prescaler 80, counting up from zero
    timer = timerBegin(0, 80, true);
    // attach onTimer function to our timer, edge type
    timerAttachInterrupt(timer, &onTimer, true);
    // set alarm, time in microseconds, repeat
    timerAlarmWrite(timer, 1000, true);
    // Start an alarm
    timerAlarmEnable(timer);

    Serial.begin(115200);
    // Initialize the output variables as outputs
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);

    // initialize file system
    if(!SPIFFS.begin())
    {
        Serial.println("SPIFFS Mount Failed");
        return;
    }

    // load HTML template from file system
    char buffer[4096];
    templ = "";
    Serial.println("Reading file: /template.html\r\n");
    File file = SPIFFS.open("/template.html");
    if(!file || file.isDirectory())
    {
        Serial.println("- failed to open file for reading");
        return;
    }
    size_t len = file.readBytes(buffer,4095);
    buffer[len] = 0;
    templ = String(buffer);
    Serial.print(templ);
    Serial.println();
    
    // setup Wi-Fi network with SSID and password
    WiFi.softAP("LoRa-FC");

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    // setup HTTP server on port 80
    server = new WiFiServer(80);
    server->begin();

    // show access point details
    char message[32];
    sprintf(message,"server: %s", IP.toString().c_str());
    // sprintf(message,"server: ", IP);
    oled -> drawString(0, 12, message);
    oled -> display();
    
}

void loop(){
    WiFiClient client = server->available();    // Listen for incoming clients
    
    if (client) {                               // If a new client connects,
        Serial.println("New Client.");          // print a message out in the serial port
        String currentLine = "";                // make a String to hold incoming data from the client
        while (client.connected())              // connected() essentially is a non-blocking recv()
        {
            // loop while the client's connected
            if (client.available()) {           // if there's bytes to read from the client,
                char c = client.read();         // read a byte, then
                Serial.write(c);                // print it out the serial monitor
                request += c;
                if (c == '\n')
                {
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response
                    if (currentLine.length() == 0)
                    {
                        // handle all status changes resulting from the request
                        handle_request();
                        // fill in the response template to create the page
                        fill_template();
                        // sned page to the client
                        client.print(page);
                        // Break out of the while loop
                        break;
                    }
                    else
                    {
                        // if you got a newline, then clear currentLine
                        currentLine = "";
                    }
                }
                else
                {
                    if (c != '\r')
                    {
                        // if you got anything else but a carriage return character,
                        currentLine += c;      // add it to the end of the currentLine
                    }
                }
            }
        }
        // Clear the request variable
        request = "";
        // Close the connection
        client.stop();
        Serial.println("Client disconnected.");
        Serial.println("");
    }
}
