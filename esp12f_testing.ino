#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <Adafruit_NeoPixel.h>
#include "front_end.h"
// #include "LittleFS.h"
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

const int pixelCount = 144;
const int pinNumber = 14;
Adafruit_NeoPixel pixels(pixelCount, pinNumber, NEO_GRB + NEO_KHZ800);

#include "wifi_prihlaseni.h"

const char* ssid = STASSID;
const char* password = STAPSK;

String ledStripState = "ON";


AsyncWebServer server(80);

/* nastavení String ke zpětnému načtení do frontend */
String processor(const String& var){
    Serial.println(var);
    if(var == "STATE"){
      return ledStripState;
    return "";
  }
}


void setup(void) {
  pinMode(2, OUTPUT);
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor); // index html in a different header file
  });

  // turn action 
  server.on("/turn", HTTP_GET, [](AsyncWebServerRequest *request) {
    turnLedStrip();
    request->send_P(200, "text/html", index_html, processor); // index html in a different header file
  });


  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin(); // asyncWebServer starts
  Serial.println("HTTP server started");

  pixels.begin();
  pixels.clear(); //clears the neopixel

  changeColors(255, 230, 255);
}

void loop(void) {
  digitalWrite(2, millis() % 1000 > 500 ? HIGH : LOW); // turning on and off the builtin led for debug

}

void turnLedStrip() {
  // turning off when its on and vice versa
  if (ledStripState == "ON") { // if its off
    changeColors(0, 0, 0);
    ledStripState = "OFF";
  } else { // if it is on
    changeColors(255, 230, 255); // the neopixel is a bit green so i make it withe by lowering the g value
    ledStripState = "ON";
  }
}

void changeColors(int r, int g, int b) {
  for(int i = 0; i < pixelCount-20; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
    pixels.show();
  }
}