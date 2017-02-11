#ifndef _LEDS_H_
#define _LEDS_H_

#define FASTLED_INTERNAL
#define FASTLED_ESP8266_RAW_PIN_ORDER

#define SPI_DATA 13
#define SPI_CLOCK 14

#include <stdint.h>
#include <string.h>
#include <map>
#include <string>
#include <vector>

#include "FastLED.h"
FASTLED_USING_NAMESPACE;

#define LED_TYPE    DOTSTAR

#define COLOR_ORDER GBR
#define DEFAULT_NUM_LEDS 25
#define MAX_NUM_LEDS 480

#define NUM_UNI_LEDS 60
#define MAX_UNIS     10

#define BRIGHTNESS         255
#define FRAMES_PER_SECOND  120

#define FIVE_PM 17
#define ONE_AM 1
#define MINUTES_PER_HOUR 60
#define SECONDS_PER_HOUR 3600
#define ONE_DAY (24 * 60)
#define PST_TIME -8

#define ARTNET_WAIT_TIME (60 * 1000) // 1 minute in milliseconds

enum Modes {
    STOP_MODE,
    COLOR_MODE,
    PATTERN_MODE,
    PATTERN_CYCLE_MODE,
    ARTNET_MODE,
    LAST_MODE
};

class Leds {
  enum Modes mode;
  enum Modes lastMode;
  int startTime;      // minutes from 12:00 to turn on LEDs
  int stopTime;       // minutes from 12:00 to turn off LEDs
  int hueCycleTime;
  int patCycleTime;
  bool running;
  typedef void (Leds::*funcPtr_t)(void); // this is a function pointer so we can pass patterns around
  std::map <String, funcPtr_t> stringCom;
  std::map <String, funcPtr_t> patterns;
  std::map <String, funcPtr_t>::iterator currentPattern;
  CRGB currentRgb;
  CHSV currentHue;
  // the next 2 variables are for a matrix
  std::vector <uint8_t> pixelMap;
  uint8_t diagDirection;
  uint8_t shuffleCnt;
  uint8_t num;
  CRGBPalette16 gPal;
  CRGBPalette16 grPal;
  int timeZone_;  // place to keep how many seconds we are from UTC
  enum EOrder colorOrder_; // order of our LED colors
  
  bool artnetEnabled;
  int artnetWaitTime;
  uint16_t artnetPort;
  int _startUniverse;

public:
  CRGB leds[MAX_NUM_LEDS];
  int numLeds;

  /* Constructor, init the LEDS */

  Leds();
  ~Leds();
  bool isRunning(void);
  String pattern(void);
  std::vector <String> getPatterns(void);
  bool setPattern(String newPattern);
  CRGB color(void);
  bool setColor(CRGB newColor);
  bool setColor(CHSV newColor);
  CHSV hue(void);
  String getMode(void);
  bool setMode(enum Modes);
  void setStartTime(int newTime);
  int getStartTime(void);
  void setStopTime(int newTime);
  int getStopTime(void);
  void connected(void *p_data, uint16_t length);
  void disconnected(void);
  void sleepMode(void);
  void ff(void);
  void rew(void);
  void play(void);
  void pause(void);
  void stop(void);
  void colorDown(void);
  void colorUp(void);
  void shuffle(void);
  
  void blinkSimple2(void);
  void simpleColor(void);
  void hueColor(void);
  void chase(void);
  void chase2();
  void rainbow(void);
  void addGlitter( fract8 chanceOfGlitter);
  void rainbowWithGlitter(void);
  void gConfetti(void);
  void rConfetti(void);
  void confetti(void);
  void sinelon(void);
  void greenlon(void);
  void redlon(void);
  void sweep(void);
  void dark(void);
  void bpm(void);
  void juggle(void);
  void christmasConfetti(void);
  void christmasLights(void);
  void allChristmasLights(void);
  void wipe(void);
  
  void ledTest(void);
  void loop(int);
  void processCommands(char const *command, uint16_t len);
  bool readConfig(void);
  bool writeConfig(void);
  int timezone(void);
};
#endif

