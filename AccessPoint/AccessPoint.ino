#include <WiFi.h>
#include "FS.h"
#include "SPIFFS.h"

// todo:
// give the access point a custom name "LoRaFC-Ground"
// give the host a name "ATC"
// display IP number and name on OLED

// web server
WiFiServer *server;

// Variable to store the template/response page
String templ;
String page;

// Auxiliar variables to store the current output state
int output25State = 0;
const int output25 = 25;

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
        output25State = 1;
        digitalWrite(output25, HIGH);
    };
    if (request.indexOf("GET /25/off") >= 0)
    {
        output25State = 0;
        digitalWrite(output25, LOW);
    };
}

// fill the information for the current status
// into the response template
void fill_template()
{
    page = templ;
    if (output25State)
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
    Serial.begin(115200);
    // Initialize the output variables as outputs
    pinMode(output25, OUTPUT);
    digitalWrite(output25, LOW);

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
