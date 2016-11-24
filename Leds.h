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
#define COLOR_ORDER BGR
#define NUM_LEDS    360

#define BRIGHTNESS         255
#define FRAMES_PER_SECOND  120

enum Modes {
    STOP_MODE,
    COLOR_MODE,
    PATTERN_MODE,
    PATTERN_CYCLE_MODE,
    LAST_MODE
};

class Leds {
  enum Modes mode;
  bool running;
  typedef void (Leds::*funcPtr_t)(void);
  std::map <String, funcPtr_t> stringCom;
  std::map <String, funcPtr_t> patterns;
  std::map <String, funcPtr_t>::iterator currentPattern;
  CRGB currentRgb;
  CHSV currentHue;
  int hueCycleTime;
  int patCycleTime;
  std::vector <uint8_t> pixelMap;
  uint8_t diagDirection;
  uint8_t shuffleCnt;
  uint8_t num;
  CRGBPalette16 gPal;
  CRGBPalette16 grPal;

public:
  CRGB leds[NUM_LEDS];

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
  void bpm(void);
  void juggle(void);
  void christmasConfetti(void);
  void christmasLights(void);
  void allChristmasLights(void);
  void wipe(void);
  void ledTest(void);
  void loop(void);
  void processCommands(char const *command, uint16_t len);
};
#endif

