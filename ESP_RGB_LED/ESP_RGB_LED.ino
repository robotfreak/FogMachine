// this code for testing wifi access point control
// www.dcmote.duino,lk
// coding by Tharindu Ireshana : Leader of DCmote Open Sourse Project
// www.facebook.com/dcremote
// https://github.com/irashana
// www.facebook.com/tharinduireshana
// tirashana@gmail.com
// 94715376785


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

/* Set these to your desired credentials. */
const char* ssid = "ESP-Accesspoint";
const char* password = "12345678";  // set to "" for open access point w/o passwortd

ESP8266WebServer server(80);

const String HTTP_HEAD           = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";
const String HTTP_STYLE  = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;}  input{width:90%;}  body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.6rem;background-color:#1fb3ec;color:#fdd;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .button2 {background-color: #008CBA;} .button3 {background-color: #f44336;} .button4 {background-color: #e7e7e7; color: black;} .button5 {background-color: #555555;} .button6 {background-color: #4CAF50;} </style>";
const String HTTP_SCRIPT        = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const String HTTP_HEAD_END      = "</head><body><div style='text-align:left;display:inline-block;min-width:260px;'>";

const String HOMEPAGE = "<form action=\"/cmd1\" method=\"get\"><button class=\"button3\">On</button></form><br/><form action=\"/cmd2\" method=\"get\"><button class=\"button4\">Off</button></form><br/>    ";

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/
void handleRoot() {
  String s = HTTP_HEAD;
  s += HTTP_STYLE;
  s += HTTP_SCRIPT;
  s += HTTP_HEAD_END;
  s += "<H3>ESP8266 Access Point</H3>";
  s += HOMEPAGE;
  server.send(200, "text/html", s);

}

void cmd1() {
  String s = HTTP_HEAD;
  s += HTTP_STYLE;
  s += HTTP_SCRIPT;
  s += HTTP_HEAD_END;
  s += "<H3>ESP8266 Access Point</H3>";
  s += HOMEPAGE;
  server.send(200, "text/html", s);
  digitalWrite(D4, LOW);
}

void cmd2() {
  String s = HTTP_HEAD;
  s += HTTP_STYLE;
  s += HTTP_SCRIPT;
  s += HTTP_HEAD_END;
  s += "<H3>ESP8266 Access Point</H3>";
  s += HOMEPAGE;
  server.send(200, "text/html", s);
  digitalWrite(D4, HIGH);

}

void setup() {
  delay(1000);
  pinMode(D4, OUTPUT);

  digitalWrite(D4, HIGH);

  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/cmd1", cmd1);
  server.on("/cmd2", cmd2);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
