/*--------------------------------------------------
  HTTP 1.1 Webserver as AccessPoint for ESP8266
  for ESP8266 adapted Arduino IDE

  by Stefan Thesen 08/2015 - free for anyone

  Does HTTP 1.1 with defined connection closing.
  Handles empty requests in a defined manner.
  Handle requests for non-exisiting pages correctly.

  This demo allows to switch two functions:
  Function 1 creates serial output and toggels GPIO2
  Function 2 just creates serial output.

  Serial output can e.g. be used to steer an attached
  Arduino, Raspberry etc.
  --------------------------------------------------*/

//#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "ESP-FogMachine";
const char* password = "Fog54321";  // set to "" for open access point w/o passwortd

unsigned long ulReqcount;

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const int ledPin = D4;
const int relaisPin = D1;

// Variables will change:
int ledState = LOW;             // ledState used to set the LED
int mode = 1;
long onInterval = 2000;           // interval at which to blink (milliseconds)
long offInterval = 2000;           // interval at which to blink (milliseconds)

// Create an instance of the server on Port 80
WiFiServer server(80);

void setup()
{
  // setup globals
  ulReqcount = 0;

  // prepare GPIOs
  pinMode(ledPin, OUTPUT);
  pinMode(relaisPin, OUTPUT);
  digitalWrite(ledPin, 0);
  digitalWrite(relaisPin, 1);

  // start serial
  Serial.begin(9600);
  delay(1);

  // AP mode
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  server.begin();
}

void loop()
{
  // check to see if it's time to blink the LED; that is, if the difference
  // between the current time and last time you blinked the LED is bigger than
  // the interval at which you want to blink the LED.
  unsigned long currentMillis = millis();

  if (mode) {
    if (ledState == LOW) {
      if (currentMillis - previousMillis >= onInterval) {
        // save the last time you blinked the LED
        previousMillis = currentMillis;
        ledState = HIGH;
      }
      // set the LED with the ledState of the variable:
      digitalWrite(ledPin, ledState);
      digitalWrite(relaisPin, !ledState);
    }
    else if (ledState == HIGH) {
      if (currentMillis - previousMillis >= offInterval) {
        // save the last time you blinked the LED
        previousMillis = currentMillis;
        ledState = LOW;
      }
      // set the LED with the ledState of the variable:
      digitalWrite(ledPin, ledState);
      digitalWrite(relaisPin, !ledState);
    }
  }

  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  unsigned long ultimeout = millis() + 250;
  while (!client.available() && (millis() < ultimeout) )
  {
    delay(1);
  }
  if (millis() > ultimeout)
  {
    Serial.println("client connection time-out!");
    return;
  }

  // Read the first line of the request
  String sRequest = client.readStringUntil('\r');
  //Serial.println(sRequest);
  client.flush();

  // stop client, if request is empty
  if (sRequest == "")
  {
    Serial.println("empty request! - stopping client");
    client.stop();
    return;
  }

  // get path; end of path is either space or ?
  // Syntax is e.g. GET /?pin=MOTOR1STOP HTTP/1.1
  String sPath = "", sParam = "", sCmd = "";
  String sGetstart = "GET ";
  int iStart, iEndSpace, iEndQuest;
  iStart = sRequest.indexOf(sGetstart);
  if (iStart >= 0)
  {
    iStart += +sGetstart.length();
    iEndSpace = sRequest.indexOf(" ", iStart);
    iEndQuest = sRequest.indexOf("?", iStart);

    // are there parameters?
    if (iEndSpace > 0)
    {
      if (iEndQuest > 0)
      {
        // there are parameters
        sPath  = sRequest.substring(iStart, iEndQuest);
        sParam = sRequest.substring(iEndQuest, iEndSpace);
      }
      else
      {
        // NO parameters
        sPath  = sRequest.substring(iStart, iEndSpace);
      }
    }
  }

  ///////////////////////////////////////////////////////////////////////////////
  // output parameters to serial, you may connect e.g. an Arduino and react on it
  ///////////////////////////////////////////////////////////////////////////////
  if (sParam.length() > 0)
  {
    int iEqu = sParam.indexOf("=");
    if (iEqu >= 0)
    {
      sCmd = sParam.substring(iEqu + 1, sParam.length());
      Serial.println(sCmd);
    }
  }


  ///////////////////////////
  // format the html response
  ///////////////////////////
  String sResponse, sHeader;

  ////////////////////////////
  // 404 for non-matching path
  ////////////////////////////
  if (sPath != "/")
  {
    sResponse = "<html><head><title>404 Not Found</title></head><body><h1>Not Found</h1><p>The requested URL was not found on this server.</p></body></html>";

    sHeader  = "HTTP/1.1 404 Not found\r\n";
    sHeader += "Content-Length: ";
    sHeader += sResponse.length();
    sHeader += "\r\n";
    sHeader += "Content-Type: text/html\r\n";
    sHeader += "Connection: close\r\n";
    sHeader += "\r\n";
  }
  ///////////////////////
  // format the html page
  ///////////////////////
  else
  {
    ulReqcount++;
    sResponse  = "<html><head><title>Relais Control</title></head><body>";
    sResponse += "<font color=\"#000000\"><body bgcolor=\"#d0d0f0\">";
    sResponse += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=yes\">";
    sResponse += "<h1>ESP8266 Access Point</h1>";
    sResponse += "Switch relais.<BR>";
    sResponse += "<FONT SIZE=+1>";
    sResponse += "<p>Relais <a href=\"?pin=FUNCTION1ON\"><button>on</button></a>&nbsp;<a href=\"?pin=FUNCTION1OFF\"><button>off</button></a></p>";

    //////////////////////
    // react on parameters
    //////////////////////
    if (sCmd.length() > 0)
    {
      // write received command to html page
      sResponse += "Command:" + sCmd + "<BR>";

      // switch GPIO
      if (sCmd.indexOf("FUNCTION1ON") >= 0)
      {
        mode = 1;
        previousMillis = 0;
        digitalWrite(ledPin, 0);
        digitalWrite(relaisPin, 1);
        ledState = LOW;
      }
      else if (sCmd.indexOf("FUNCTION1OFF") >= 0)
      {
        mode = 0;
        previousMillis = 0;
        digitalWrite(ledPin, 1);
        digitalWrite(relaisPin, 0);
        ledState = LOW;
      }
    }

    sResponse += "<FONT SIZE=-2>";
    sResponse += "<BR>counter=";
    sResponse += ulReqcount;
    sResponse += "</body></html>";

    sHeader  = "HTTP/1.1 200 OK\r\n";
    sHeader += "Content-Length: ";
    sHeader += sResponse.length();
    sHeader += "\r\n";
    sHeader += "Content-Type: text/html\r\n";
    sHeader += "Connection: close\r\n";
    sHeader += "\r\n";
  }

  // Send the response to the client
  client.print(sHeader);
  client.print(sResponse);

  // and stop the client
  client.stop();
  Serial.println("Client disonnected");
}
