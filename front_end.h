#ifndef front_end_h
#define front_end_h

#include <Arduino.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Light interface</title>
  <style>
    
  </style>
  </head>
  <body>
    <h1> Led Lamp </h1>
    <a href="/turn"> <button>%STATE%</button> </a>
    
    <p> teplota: %TEMP% </p>
    <p> humidity: %HUMID% </p>
    <p> light intensity: %LIT% </p>

    <a href="/"> <button> UPDATE </button> </a>
  </body>
</html>)rawliteral";

#endif
