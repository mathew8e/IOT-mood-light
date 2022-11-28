#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

const int pixelCount = 144;
const int pinNumber = 14;
Adafruit_NeoPixel pixels(pixelCount, pinNumber, NEO_GRB + NEO_KHZ800);

#include "wifi_prihlaseni.h"

const char* ssid = STASSID;
const char* password = STAPSK;

AsyncWebServer server(80);

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
    request->send(200, "text/plain", ssid);
  });

  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin(); // asyncWebServer starts
  Serial.println("HTTP server started");

  pixels.begin();
  pixels.clear(); //clears the neopixel
}

void loop(void) {
  digitalWrite(2, millis() % 1000 > 500 ? HIGH : LOW); // turning on and off the builtin led for debug

  for(int i = 0; i < pixelCount; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 100, 255));
  }

  pixels.show();
}