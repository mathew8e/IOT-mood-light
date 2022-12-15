#ifndef front_end_h
#define front_end_h

#include <Arduino.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Light interface</title>
  <meta charset="UTF-8">
  <style>
    body {
        font-family: "Helvetica";
    }
    .mainControll {
      text-align:center;
    }
  </style>
  </head>
  <body>
    <div class="mainControll">
      <h1> Led Lamp </h1>
      <a href="/turn"> <button>%STATE%</button> </a>
      <br>
      <br>
      <a href="/funMode"> <button>Fun Mode</button> </a>
      
      <p> teplota: %TEMP% </p>
      <p> humidity: %HUMID% </p>
      <p> light intensity: %LIT% </p>

      <a href="/"> <button> UPDATE </button> </a>

      <div class="LED">
 
      <h2>LED Sliders</h2>

      <p>
      <input type="range" onchange="updateSliderPWM(this)" id="pwmSliderRED" min="0" max="255" value="%SLIDERVALUERED%" step="1" class="sliderR">

      <span id="textSliderValueRED">%REDVALUE%</span>
      </p>
      <p>
      <input type="range" onchange="updateSliderPWM(this)" id="pwmSliderGREEN" min="0" max="255" value="%SLIDERVALUEGREEN%" step="1" class="sliderG">

      <span id="textSliderValueGREEN">%GREENVALUE%</span>
      </p>
      <p>
      <input type="range" onchange="updateSliderPWM(this)" id="pwmSliderBLUE" min="0" max="255" value="%SLIDERVALUEBLUE%" step="1" class="sliderB">

      <span id="textSliderValueBLUE">%BLUEVALUE%</span>
      </p>
 </div>


      <br>
      <br>
      <br>
      version 1.0.1
    <div>
  </body>
</html>)rawliteral";

#endif
