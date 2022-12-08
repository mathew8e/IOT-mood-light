#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <Adafruit_NeoPixel.h>
#include "front_end.h"

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// #include "LittleFS.h"
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

const int PIXEL_COUNT = 72;
const int LED_STRIP_PIN = 12;
const int DHT_PIN = 14;
const int PHOTO_RES_PIN = A0;
const int BUTTON_PIN = 4;

// const int CALIBRATION_VALUE = 2; // calibration value for the dht sensor to be more precise

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

int temperature; // temperature value in C
int humidity;
int buttonCurVal;
int buttonPrevVal;
String ledStripState = "ON";
uint32_t delayMS = 1000; // how often is the humidity sensor going to update
bool once; // boolean that make sure that the code doesnt run multiple times in one millisecond

int ledState = HIGH;        // the current state of the output pin
int buttonState;            // the current reading from the input pin
int lastButtonState = LOW;  // the previous reading from the input pin

DHT_Unified dht(DHT_PIN, DHT11);
Adafruit_NeoPixel pixels(PIXEL_COUNT, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

#include "wifi_prihlaseni.h"

const char* ssid = STASSID;
const char* password = STAPSK;

AsyncWebServer server(80);

void changeColors(int r, int g, int b) {
  for(int i = 0; i < PIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
    pixels.show();
  }
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

void sensorRead() {
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
  }
  else {
    temperature = event.temperature;
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    humidity = event.relative_humidity;
  }
}

void funMode() { // still works wierd
  changeColors(255, 0, 0);
  changeColors(0, 255, 0);
  changeColors(0, 0, 255);
  changeColors(0, 0, 0);
}


/* nastavení String ke zpětnému načtení do frontend */
String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    return ledStripState;
  } if (var == "TEMP") {
    return String(temperature);
  } if (var == "HUMID") {
    return String(humidity);
  } if (var == "LIT") {
    return String(analogRead(PHOTO_RES_PIN));
  } if (var == "REDVALUE") {
    return "";
  } if (var == "GREENVALUE") {
    return "";
  } if (var == "BLUEVALUE") {
    return "";
  }
  return "";
}

void setup(void) {
  pinMode(2, OUTPUT); // setting the builtin led to ouptut to blink
  pinMode(PHOTO_RES_PIN, INPUT); // setting the photo resistor pin to input
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  dht.begin();

  sensor_t sensor;

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

  // fun mode
  server.on("/funMode", HTTP_GET, [](AsyncWebServerRequest *request) {
    funMode();
    request->send_P(200, "text/html", index_html, processor); // index html in a different header file
  });


  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin(); // asyncWebServer starts
  Serial.println("HTTP server started");

  pixels.begin();
  pixels.clear(); // clears the neopixel

  changeColors(255, 230, 255);
}



void loop(void) {
  
  buttonDebounce();

  // buttonCurVal = digitalRead(BUTTON);
  // if (buttonCurVal == 0 && buttonPrevVal == 1) {
  //   turnLedStrip();
  // }


  digitalWrite(2, millis() % 1000 > 500 ? HIGH : LOW); // turning on and off the builtin led for debug

  // if (millis() % 1000 == 1){ // every second the dht sensor will be read and stored into temperature variable
  //   if (dht.readTemperature() != 2147483647) { // if the dht sensor isn't acting wierd and throwing NAN
  //       temperature = dht.readTemperature();
  //       Serial.println(temperature);
  //   }
  // }

  // Serial.println(buttonCurVal);
  


  if (millis() % delayMS == 1 && once){ // if modulo millis is the minimum time delay for the sensor it will one once the once boolean will switch and wait for the second millisecond
                                                  // this makes sure that the function doesnt rune multiple times when in the one millisecond
    sensorRead();
    once = false;
  } else if (millis() % delayMS == 2) {
    once = true;
  }
  
  // buttonPrevVal = digitalRead(BUTTON);
  delay(1);
  
  
}

void buttonDebounce(){
    // read the state of the switch into a local variable:
  int reading = digitalRead(BUTTON_PIN);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        turnLedStrip();
      }
    }
  }

  // set the LED:
  // digitalWrite(ledPin, ledState);

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
}


