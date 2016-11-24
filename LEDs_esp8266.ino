/*
 * led_class.cpp
 *
 *  Created on: Oct 30, 2016
 *      Author: mlw
 */


#include <ESP8266WebServer.h>
#include "Leds.h"

int timeHour = 0;
int off = 0;

extern ESP8266WebServer server;

class Leds myLeds;

extern void FSBsetup(void);

void handleLedPattern(void) {
  String output = "[";
  String patName = server.arg("pattern");
    
  if (patName != "")
  {
    // try to set the pattern to the new value
  
     if (myLeds.setPattern(patName) != true)
     {
        server.send(404, "text/plain", "Pattern " + patName + " not found");
     }
     else
     {
        server.send(200, "text/plain", "Pattern " + patName + " set");
     }
  }
  else
  {
    output += "{\"pattern\":\"";
    output += myLeds.pattern();
    output += "\"}";
    
    output += "]";
    server.send(200, "text/json", output);
  }  
}

uint8_t convertHex(const char *str)
{
  uint8_t hexVal = 0;
  
  if (str[0] >= '0' && str[0] <= '9')
  {
    hexVal = str[0] - 0x30;
  }
  else if (str[0] >= 'a' && str[0] <= 'f')
  {
    hexVal = str[0] - 'a' + 10;
  }
  else if (str[0] >= 'A' && str[0] <= 'F')
  {
    hexVal = str[0] - 'A' + 10;
  }

  hexVal = hexVal << 4;
  
  if (str[1] >= '0' && str[1] <= '9')
  {
    hexVal += str[1] - 0x30;
  }
  else if (str[1] >= 'a' && str[1] <= 'f')
  {
    hexVal += str[1] - 'a' + 10;
  }
  else if (str[1] >= 'A' && str[1] <= 'F')
  {
    hexVal += str[1] - 'A' + 10;
  }

  return hexVal;
}

void handleLedMode(void) {
  String mode = server.arg("mode");

  if (mode != "")
  {
    Serial.print(mode);

    if (mode == "Stop")
    {
      myLeds.setMode(STOP_MODE);
    }
    else if (mode == "Pattern")
    {
      myLeds.setMode(PATTERN_MODE);
    }
    else if (mode == "Pattern_Cycle")
    {
      myLeds.setMode(PATTERN_CYCLE_MODE);
    }
    else if (mode == "Color")
    {
      myLeds.setMode(COLOR_MODE);
    }
    else
    {
       server.send(400, "text/plain", "Bad Mode");
       return;
    }
    
    server.send(200, "text/plain", "Mode set");    
  }
  else
  {
    server.send(400, "text/plain", "Error No Mode");
  }
}

void handleLedColor(void) {
  String color = server.arg("color");
  CRGB newColor;
  const char *colorStr = color.c_str();;
  
  if (color != "")
  {
    Serial.print("Color length = ");
    Serial.print(color.length());
    Serial.print(" (");
    Serial.print(color);
    Serial.println(")");
      
    if (color.length() != 7)
    {
      // bad format for arg
      server.send(400, "text/plain", "Bad len for color");
    }

    if (colorStr[0] == '#')
    {
      Serial.print("We got a color (");
      Serial.print(colorStr);
      Serial.println(")");
        
      // get the rest of the info
      newColor.red = convertHex(colorStr + 1);
      newColor.green = convertHex(colorStr + 3);
      newColor.blue = convertHex(colorStr + 5);
      myLeds.setColor(newColor);
      myLeds.simpleColor();

      server.send(200, "text/plain", "Led color set");
    }
    else
    {
      server.send(400, "text/plain", "Bad format for color");
    }
  }
  else
  {
    // get the color and return it
    Serial.println("We got a request for color");
   
    String output = "[";

    char tmpBuf[30];
    CRGB color = myLeds.color();
    sprintf(tmpBuf, "#%02x%02x%02x", color.red, color.green, color.blue);
  
    output += "\"color\":\"";
    output += String(tmpBuf);
    output += "\""; 

    output += "]";
    server.send(200, "text/json", output);
  }
}

void handleLedGet(void) {
  if (server.hasArg("pattern"))
  {
    Serial.println("Got pattern");
    handleLedPattern();
  }
  else if (server.hasArg("color"))
  {
    Serial.println("Got color");
    handleLedColor();
  }
  else if (server.hasArg("mode"))
  {
    Serial.println("Got mode");
    handleLedMode();
  }
  else
  {
    Serial.println("Got unknown");
    server.send(405, "text/plain", "Not handled");
  }
}

void handleLedPut(void) {
}

void handleLedPatternsGet() {
  String output = "{\"Patterns\":[";
  std::vector <String> patterns = myLeds.getPatterns();
  std::vector <String>::iterator pattern = patterns.begin();

  while(pattern != patterns.end())
  {
    if (pattern != patterns.begin())
    {
      // we need a comma between entries
      output += ",";
    }
    // add this entry to the array
    output += "{\"name\":\"";
    output += *pattern++;
    output += "\"}";
  }
  output += "]";

  output += "}";
  server.send(200, "text/json", output);
}

void handleLedStatusGet() {
  String output = "{";

  // running status
  output += "\"running\":\"";
  output += myLeds.isRunning() ? "true" : "false";
  output += "\",";

  // mode
  output += "\"mode\":\"";
  output += myLeds.getMode();
  output += "\",";

  // color

  char tmpBuf[30];
  CRGB color = myLeds.color();
  sprintf(tmpBuf, "#%02x%02x%02x", color.red, color.green, color.blue);
  
  output += "\"color\":\"";
  output += String(tmpBuf);
  output += "\",";  
  
  // pattern
  
  output += "\"pattern\":\"";
  output += myLeds.pattern();
  output += "\"";

  output += "}";
  server.send(200, "text/json", output);
}

void setup() {
  // setup the LED page
  server.on("/leds", HTTP_GET, handleLedGet);
  server.on("/leds", HTTP_PUT, handleLedPut);
  server.on("/leds/status", HTTP_GET, handleLedStatusGet);
  server.on("/leds/color", handleLedColor);
  server.on("/leds/patterns", HTTP_GET, handleLedPatternsGet);

#if 0
   //get heap status, analog input value and all GPIO statuses in one json call
  server.on("/all", HTTP_GET, [](){
    String json = "{";
    json += "\"heap\":"+String(ESP.getFreeHeap());
    handlepatternArray(json);
    json += "}";
    server.send(200, "text/json", json);
    json = String();
  });
#endif
  /* call the browser setup first */

  FSBsetup();
  myLeds.ledTest();
}

#define FIVE_PM 17
#define ONE_AM 1

extern void FSBloop(void);

void loop()
{
  // look at what hour it is and turn on the lights at 16 and off at 1
#if 0

  timeHour = Time.hour() - 8;

  if (timeHour < 0)
  {
    timeHour += 24;
  }

  if ((timeHour >= ONE_AM) && (timeHour < FIVE_PM))
  {
    if (!off)
    {

      // clear the Leds and show them once

      for (int x = 0; x < NUM_LEDS ; x++)
      {
        leds[x] = CRGB::Black;
        FastLED.show();
        off = 1;
      }

      FastLED.delay(1000);
    }
  }
  else
  {
    off = 0;

    myLeds.loop();
  }
#else
    myLeds.loop();
#endif

  FSBloop();

  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND);
}

