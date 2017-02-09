/*
 * led_class.cpp
 *
 *  Created on: Oct 30, 2016
 *      Author: mlw
 */

#include "NTPClient.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <ESP8266WebServer.h>
#include <FS.h>
#include "Leds.h"

int timeHour = 0;
int off = 0;

WiFiUDP ntpUDP;

// By default 'time.nist.gov' is used with 60 seconds update interval and
// no offset
// we are PST so -8 hours

NTPClient *timeClient;

extern ESP8266WebServer server;

// we now use a pointer to try to get the config info before we construct.

class Leds *myLeds;

extern void FSBsetup(void);

void ledsOff()
{
  myLeds->stop();
}

void handleLedPattern(void) {
  String output = "[";
  String patName = server.arg("pattern");
    
  if (patName != "")
  {
    // try to set the pattern to the new value
  
     if (myLeds->setPattern(patName) != true)
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
    output += myLeds->pattern();
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
      myLeds->setMode(STOP_MODE);
    }
    else if (mode == "Pattern")
    {
      myLeds->setMode(PATTERN_MODE);
    }
    else if (mode == "Pattern_Cycle")
    {
      myLeds->setMode(PATTERN_CYCLE_MODE);
    }
    else if (mode == "Color")
    {
      myLeds->setMode(COLOR_MODE);
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
      myLeds->setColor(newColor);

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
    CRGB color = myLeds->color();
    sprintf(tmpBuf, "#%02x%02x%02x", color.red, color.green, color.blue);
  
    output += "\"color\":\"";
    output += String(tmpBuf);
    output += "\""; 

    output += "]";
    server.send(200, "text/json", output);
  }
}


void handleLedStartTime(void) {
  String newTime = server.arg("startTime");
  
  if (newTime != "")
  {
    String hour;
    String minutes;
    int index = newTime.indexOf(':');

    if (index != 2)
    {
      // bad format
      server.send(400, "text/plain", "Bad format for Start Time");
    }

    // format should be hh:mm
    
    hour = newTime.substring(0,index);
    minutes = newTime.substring(index + 1);
    
    myLeds->setStartTime( hour.toInt() * 60 + minutes.toInt());
    server.send(200, "text/plain", "Start Time set");
  }
  else
  {
    // they want to know the start time
  }
}

void handleLedStopTime(void) {
  String newTime = server.arg("stopTime");
  
  if (newTime != "")
  {
    String hour;
    String minutes;
    int index = newTime.indexOf(':');

    if (index != 2)
    {
      // bad format
      server.send(400, "text/plain", "Bad format for Stop Time");
    }

    // format should be hh:mm
    
    hour = newTime.substring(0,index);
    minutes = newTime.substring(index + 1);
    
    myLeds->setStopTime( hour.toInt() * 60 + minutes.toInt());
    server.send(200, "text/plain", "Stop Time set");
  }
  else
  {
    // they want to know the stop time
  }
}

void handleLedRunning(void) {
  String newStatus = server.arg("running");
  
  if (newStatus != "")
  {
    if (newStatus == "true")
    {
      myLeds->play();
      server.send(200, "text/plain", "Set running to true");
    }
    else if (newStatus == "false")
    {
      myLeds->stop();
      server.send(200, "text/plain", "Set running to false");
    }
  }
  else
  {
    // they want to know the stop time
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
  else if (server.hasArg("startTime"))
  {
    Serial.println("Got startTime");
    handleLedStartTime();
  }
  else if (server.hasArg("stopTime"))
  {
    Serial.println("Got stopTime");
    handleLedStopTime();
  }
  else if (server.hasArg("running"))
  {
    Serial.println("Got running");
    handleLedRunning();
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
  std::vector <String> patterns = myLeds->getPatterns();
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
  output += myLeds->isRunning() ? "true" : "false";
  output += "\",";

  // mode
  output += "\"mode\":\"";
  output += myLeds->getMode();
  output += "\",";

  // color

  char tmpBuf[30];
  CRGB color = myLeds->color();
  sprintf(tmpBuf, "#%02x%02x%02x", color.red, color.green, color.blue);
  
  output += "\"color\":\"";
  output += String(tmpBuf);
  output += "\"";  
  
  // pattern
  
  output += ",\"pattern\":\"";
  output += myLeds->pattern();
  output += "\"";

  // time
  output += ",\"time\":\"";
  output += timeClient->getFormattedTime();
  output += "\"";
  
  output += "}";
  server.send(200, "text/json", output);
}

void handleTimesGet(void)
{
  String output = "{";
  char buf[20];
  
  // start time
  output += "\"startTime\":\"";
  sprintf(buf, "%02d:%02d", myLeds->getStartTime() / 60, myLeds->getStartTime() % 60);
  output += String(buf);
  output += "\"";
  
  // stop time
  output += ",\"stopTime\":\"";
  sprintf(buf, "%02d:%02d", myLeds->getStopTime() / 60, myLeds->getStopTime() % 60);
  output += String(buf);
  output += "\"";
  
  output += "}";
  server.send(200, "text/json", output);   
}

void handleConfigSave(void) {
   if (myLeds->writeConfig())
   {
      server.send(200, "text/plain", "Config file written");      
   }
   else
   {
      server.send(400, "text/plain", "Config file write failed");    
   }
}

void setup() {
  Serial.begin(115200);

  // setup the SPIFFS first so we can read our config file 
  
  SPIFFS.begin(); 

  // we need a way to pass the config into the LED constructor
  
  myLeds = new Leds();

  // for now the led config has the timezone
  
  timeClient = new NTPClient(ntpUDP, myLeds->timezone());
  
  // setup the LED page
  server.on("/leds", HTTP_GET, handleLedGet);
  server.on("/leds", HTTP_PUT, handleLedPut);
  server.on("/leds/status", HTTP_GET, handleLedStatusGet);
  server.on("/leds/color", handleLedColor);
  server.on("/leds/patterns", HTTP_GET, handleLedPatternsGet);
  server.on("/leds/times", HTTP_GET, handleTimesGet);
  server.on("/leds/consave", HTTP_GET, handleConfigSave);
  server.on("/heap", HTTP_GET, [](){
    String json = "{";
    json += "\"heap\":"+String(ESP.getFreeHeap());
    json += "}";
    server.send(200, "text/json", json);
    json = String();
  });
  
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

  /* stop the test for now
  
  myLeds->ledTest();
  */
  
  // call the browser setup first

  FSBsetup();
}

extern void FSBloop(void);

void loop()
{
  int curTime = -1;
  
  // make sure we deal with the time
  
  if (timeClient->update())
  {
    curTime = (timeClient->getEpochTime() % 86400L) / 60;
  }

  // we are only going to use minutes to control things so get the minutes since 12:00

  myLeds->loop(curTime);

  FSBloop();

  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND);
}

