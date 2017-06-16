#include "Leds.h"
#include <FS.h>
#include <ArduinoJson.h>
#include "artnet.h"
#include <WiFiUdp.h>

#define DBG_OUTPUT_PORT Serial

// here are the maps for the leds

std::vector<uint16_t> LedMap0 = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,16,21,25,30,34,38,43,47,51,56,61,65,70,74,79,84,89,94,99,103,108,114,119,124,129,134,139,144,149,154,159,164,170,175,180,184,189,194,199,204,209,214,219,224,229,234,238,242,247,252,256,261,266,270,275,280,284,288,293,298,303,308,313,318,323,328,334,338,344,349,354,359,364,370,375,380,384};
std::vector<uint16_t> LedMap1 = {252,247,318,256,180,175,170,242,313,364,323,261,184,103,99,94,89,164,238,308,359,384,370,328,266,189,108,34,30,25,21,84,159,234,303,354,380,375,334,270,194,114,38,7,6,16,14,79,154,229,298,349,344,338,275,199,119,43,8,1,5,4,13,74,149,224,293,288,284,280,204,124,47,9,2,0,3,12,70,144,219,214,209,129,51,10,11,65,139,134,56,61};
std::vector<uint16_t> LedMap2 = {238,303,308,242,164,234,229,298,354,359,313,247,170,89,159,154,149,224,293,349,380,384,364,318,252,175,94,21,84,79,74,70,144,219,288,344,375,370,323,256,180,99,25,16,14,13,12,65,139,214,284,338,334,328,261,184,103,30,6,5,4,3,11,61,134,209,280,275,270,266,189,108,4,7,1,0,2,10,56,129,204,199,194,114,38,8,9,51,124,119,43,47};
std::vector<uint16_t> LedMap3 = {293,288,349,298,224,219,214,284,344,380,354,303,229,149,144,139,134,209,280,338,375,384,359,308,238,234,180,74,70,65,61,56,129,204  ,275,334,370,364,313,242,164,159,79,13,12,11,10,51,124,199,270  ,328,323,318,247,170,89,84,14,4,3,2,9,47,119,194,266,261,256,252,175,94,21,16,5,0,1,8,43,114,189,184,180,99,25,6,7,38,108,103,30,34};
std::vector<uint16_t> LedMap4 = {280,275,338,284,209,204,199,270,334,375,344,288,214,134,129,124,119,194,266,328,370,384,380,349,293,219,139,61,56,51,47,43,114,189,261,323,364,359,354,298,224,144,65,11,10,9,8,38,108,184,256,318,313,308,303,229,149,70,12,3,2,1,7,34,103,180,252,247,242,238,234,154,74,13,4,0,5,6,30,99,175,170,164,159,79,14,16,25,94,89,84,21};
std::vector<uint16_t> LedMap5 = {266,261,328,270,194,189,184,256,323,370,334,275,199,119,114,108,103,180,252,318,364,384,375,338,280,204,124,47,43,38,34,30,99,175,247,313,359,380,344,284,209,129,51,9,8,7,6,25,94,170,242,308,354,349,288,214,134,56,10,2,1,5,16,21,89,164,238,303,298,293,219,139,61,11,3,0,4,14,84,159,234,229,224,144,65,12,13,79,154,149,70,74};
std::vector<uint16_t> LedMap6 = {74,70,149,154,79,13,12,65,144,224,229,234,159,84,14,4,0,3,11,61,139,219,293,298,303,238,164,89,21,16,5,1,2,10,56,134,214,288,349,354,308,242,170,94,25,6,7,8,9,51,129,209,284,344,380,359,313,247,175,99,30,34,38,43,47,124,204,280,338,375,384,364,318,252,180,103,108,114,119,199,275,334,370,323,256,184,189,194,270,328,261,266};
std::vector<uint16_t> LedMap7 = {21,84,89,94,25,16,14,79,159,164,170,175,99,30,6,5,0,4,13,74,154,234,238,242,247,252,180,103,34,7,1,2,3,12,70,149,229,303,308,313,318,256,184,108,38,8,9,10,11,65,144,224,298,354,359,364,323,261,189,114,43,47,51,56,61,139,219,293,349,380,384,370,328,266,194,119,124,129,134,214,288,344,375,334,270,199,204,209,284,338,275,280};
std::vector<uint16_t> LedMap8 = {34,30,103,108,38,7,6,25,99,180,184,189,114,43,8,1,0,5,16,21,94,175,252,256,261,266,194,119,47,9,2,3,4,14,84,89,170,247,318,323,328,270,199,124,51,10,11,12,13,79,159,164,242,313,364,370,334,275,204,129,56,61,65,70,74,180,234,238,308,359,384,375,338,280,209,134,139,144,149,229,303,354,380,344,284,214,219,224,298,349,288,293};
std::vector<uint16_t> LedMap9 = {47,43,119,124,51,9,8,38,114,194,199,204,129,56,10,2,0,1,7,4,108,189,266,270,275,280,209,134,61,11,3,4,5,6,30,103,184,261,328,334,338,284,214,139,65,12,13,14,16,25,99,180,256,323,370,375,344,288,219,144,70,74,79,84,21,94,175,252,318,364,384,380,349,293,224,149,154,159,89,170,247,313,359,354,298,229,234,164,242,308,303,238};
std::vector<uint16_t> LedMap10 = {61,56,134,139,65,11,10,51,129,209,214,219,144,70,12,3,0,2,9,47,124,204,280,284,288,293,224,149,74,13,4,5,1,8,43,119,199,275,338,344,349,298,229,154,79,14,16,6,7,38,114,194,270,334,375,380,354,303,234,159,84,21,25,30,34,108,189,266,328,370,384,359,308,238,164,89,94,99,103,184,261,323,364,313,242,170,175,180,256,318,247,252};
std::vector<uint16_t> LedMap11 = {384,380,375,370,364,359,354,349,344,338,334,328,323,318,313,308,303,298,293,288,284,280,275,270,266,261,256,252,247,242,238,234,229,224,219,214,209,204,199,194,189,184,180,175,170,164,159,154,149,144,139,134,129,124,119,114,108,103,99,94,89,84,79,74,70,65,61,56,51,47,43,38,34,30,25,21,16,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0};

std::vector<std::vector<uint16_t>> LedMaps = {LedMap0, LedMap1, LedMap2, LedMap3, LedMap4, LedMap5, LedMap6, LedMap7, LedMap8, LedMap9, LedMap10, LedMap11};

std::vector<uint16_t>  Ring0 = {0};
std::vector<uint16_t>  Ring1 = {1,2,3,4,5};
std::vector<uint16_t>  Ring2 = {6,7,8,9,10,11,12,13,14,15};
std::vector<uint16_t>  Ring3 = {16,17,18,19,20,21,22,23,24,25,26,27,28,29,30};
std::vector<uint16_t>  Ring4 = {31,32,33,34,35,36,37,38,39,40,41,42,43,44,45};
std::vector<uint16_t>  Ring5 = {46,47,48,49,50,51,52,53,54,55,56,57,58,59,60};
std::vector<uint16_t>  Ring6 = {61,62,63,64,65,66,67,68,69,70,71,72,73,74,75};
std::vector<uint16_t>  Ring7 = {76,77,78,79,80,81,82,83,84,85};
std::vector<uint16_t>  Ring8 = {86,87,88,89,90};
std::vector<uint16_t>  Ring9 = {91};

std::vector<std::vector<uint16_t>> Rings = {Ring0, Ring1, Ring2, Ring3, Ring4, Ring5, Ring6, Ring7, Ring8, Ring9};

String modeNames[LAST_MODE + 1] = {
		"Stop",
		"Color",
		"Pattern",
		"Pattern_Cycle",
    "Artnet",
		"Last"
};

extern Leds *myLeds;

// there should probably be one per Led object

static WiFiUDP _artnetUdp;
static  Artnet *_artnet;
void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data);

int startUniverse = 0; // sometimes software may start at 1

// Check if we got all universes
int maxUniverses = 0;
bool universesReceived[MAX_UNIS];
unsigned long artNetRecved = 0;

Leds::Leds()
{
#if NOT_YET
	stringCom["Play"] = &Leds::play;
	stringCom["Stop"] = &Leds::stop;
	stringCom["Pause"] = &Leds::pause;
	stringCom["Rewind"] = &Leds::rew;
	//    stringCom["Record"] = &Leds::changeDiag;
	stringCom["FastForward"] = &Leds::ff;
	stringCom["Start"] = &Leds::colorDown;
	stringCom["Shuffle"] = &Leds::shuffle;
	stringCom["End"] = &Leds::colorUp;
#endif   

	currentHue = CHSV(HUE_BLUE, 240, 255);

	// these are the defaults if there is a config file it can be overwritten.

	// initial config stuff

	startTime = FIVE_PM * MINUTES_PER_HOUR;    // by default on at 5pm
	stopTime = ONE_AM * MINUTES_PER_HOUR;      // by default off at 1am
  timeZone_ = (PST_TIME * SECONDS_PER_HOUR);

	hueCycleTime = 20; // this is milliseconds
	patCycleTime = 60; // this is in seconds

  numLeds = DEFAULT_NUM_LEDS;
  
	// set to cycle through the patterns

	mode = PATTERN_CYCLE_MODE;

  // start with the default color order

  colorOrder_ = COLOR_ORDER;

  // default the artnetWaitTime, it may get overwritten in the config file

  artnetWaitTime = ARTNET_WAIT_TIME;
  
  // build our vector of patterns here
  patterns["pulse"] = &Leds::pulse;
  patterns["wipe"] = &Leds::wipe;
//  patterns["christmasConfetti"] = &Leds::christmasConfetti;
  patterns["chase"] = &Leds::chase;
//  patterns["christmasLights"] = &Leds::christmasLights;
  patterns["sweep"] = &Leds::sweep;
//  patterns["dark"] = &Leds::dark;

  currentPattern = patterns.begin();

  // create some palettes
  
  gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Green, CRGB::Blue);
  grPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Green, CRGB::Green);

  // read the config for the leds and then we can uses all the info

	readConfig();
 
	shuffleCnt = 0;

	num = 3;

	// create the default pixel mapping. This is a normal map 0 = 0, 1 = 1, ... 24 = 24.

  #if (LED_TYPE == NEOPIXEL)
      FastLED.addLeds<LED_TYPE, SPI_DATA>(leds, numLeds).setCorrection(TypicalLEDStrip);;
  #else
  // add the leds, this is a little cumbersome, but it does work.
    
  switch (colorOrder_) { 
    case RGB:
    {
      FastLED.addLeds<LED_TYPE, SPI_DATA, SPI_CLOCK, RGB>(leds, numLeds).setCorrection(TypicalLEDStrip);;
      break;
    }
    case RBG:
    {
      FastLED.addLeds<LED_TYPE, SPI_DATA, SPI_CLOCK, RBG>(leds, numLeds).setCorrection(TypicalLEDStrip);;
      break;
    }
    case GRB:
    {
      FastLED.addLeds<LED_TYPE, SPI_DATA, SPI_CLOCK, GRB>(leds, numLeds).setCorrection(TypicalLEDStrip);;
      break;
    }
    case GBR:
    {
      FastLED.addLeds<LED_TYPE, SPI_DATA, SPI_CLOCK, GBR>(leds, numLeds).setCorrection(TypicalLEDStrip);;
      break;
    }
    case BRG:
    {
      FastLED.addLeds<LED_TYPE, SPI_DATA, SPI_CLOCK, BRG>(leds, numLeds).setCorrection(TypicalLEDStrip);;
      break;
    }
    case BGR:
    {
      FastLED.addLeds<LED_TYPE, SPI_DATA, SPI_CLOCK, BGR>(leds, numLeds).setCorrection(TypicalLEDStrip);;
      break;
    }
    default:
    {
      FastLED.addLeds<LED_TYPE, SPI_DATA, SPI_CLOCK, RGB>(leds, numLeds).setCorrection(TypicalLEDStrip);;
      break;
    }    
  }
#endif

  // start running and we will figure it out later.
  
	running = true;

  if (mode == COLOR_MODE)
  {
    // this will start things
    loop(-1);
  }

  // look and see if we should open the artnet port

  if (artnetEnabled)
  {
    startUniverse = _startUniverse;
    maxUniverses = (numLeds + (NUM_UNI_LEDS - 1))/ NUM_UNI_LEDS;
    _artnet = new Artnet(_artnetUdp);
    _artnet->begin(artnetPort);
    _artnet->setArtDmxCallback(onDmxFrame);
  }
}

Leds::~Leds()
{
}

bool Leds::isRunning(void)
{
	return running;
}

CRGB Leds::color(void)
{
	return currentRgb;
}

bool Leds::setColor(CRGB newColor)
{
	currentRgb = newColor;
	return true;
}

bool Leds::setColor(CHSV newColor)
{
	currentHue = newColor;
	currentRgb = currentHue; 
	return true;
}

CHSV Leds::hue(void)
{
	return currentHue;
}

String Leds::getMode(void)
{
	return modeNames[mode];
}

bool Leds::setMode(enum Modes newMode)
{
	if ((newMode < STOP_MODE) || (newMode >= LAST_MODE))
	{
		// out of bounds
		return false;
	}

  // save the last mode, just in case
  lastMode = mode;
	mode = newMode;
}

void Leds::setStartTime(int newTime)
{
	startTime = newTime;
}

int Leds::getStartTime(void)
{
	return startTime;
}

void Leds::setStopTime(int newTime)
{
	stopTime = newTime;
}

int Leds::getStopTime(void)
{
	return stopTime;
}

/**@brief Function for doing something with the leds when we connect.
 *
 * @details This function will be called when we are connected to by a device.
 *
 * @param[in] void * p_data, uint16_t length of p_data (both unused)
 */

void Leds::connected(void *p_data, uint16_t length)
{
	(void) p_data;
	(void) length;

	/* this maybe should not set a mode */
	/* flash to let us know it happened */
	FastLED.showColor(CRGB::FairyLight);
	delay(100);
	FastLED.show();
}

/**@brief Function for doing something with the leds when we disconnect.
 *
 * @details This function will be called when we are disconnected from a device.
 *
 * @param[in] void
 */

void Leds::disconnected(void)
{
	/* this should set a mode, just keep doing what you are doing */

	FastLED.showColor(CRGB::Purple);
	delay(100);
	FastLED.show();
}

/**@brief Function for doing something with the leds when we disconnect.
 *
 * @details This function will be called when we are disconnected from a device.
 *
 * @param[in] void
 */

void Leds::sleepMode(void)
{
	for(uint8_t x = 0 ; x < 30 ; x++)
	{
		leds[10] = CRGB::Red;
		FastLED.show();
		delay(100);
		leds[10] = CRGB::Black;
		FastLED.show();
		delay(100);
		leds[10] = CRGB::Blue;
		FastLED.show();
		delay(100);
		leds[10] = CRGB::Black;
		FastLED.show();
		delay(100);
		leds[10] = CRGB::Green;
		FastLED.show();
		delay(100);
		leds[10] = CRGB::Black;
		FastLED.show();
		delay(100);
	}
}

/**@brief Function for moving the led mode forward.
 *
 * @details This function will be called from the string_com table.
 *
 * @param[in] void
 */

void Leds::ff(void)
{ 
	if (++currentPattern == patterns.end())
	{
		currentPattern = patterns.begin();

    // choose a new random mapping
    
    ledMap = LedMaps[random8(LedMaps.size())];
	}

	fill_solid(leds, numLeds, CRGB::Black);
}

/**@brief Function for moving the led mode backward.
 *
 * @details This function will be called from the string_com table.
 *
 * @param[in] void
 */

void Leds::rew(void)
{
	if (currentPattern == patterns.begin())
	{
		currentPattern = patterns.end();

    // choose a new random mapping
    
    ledMap = LedMaps[random8(LedMaps.size())];
	}

	currentPattern--;


	fill_solid(leds, numLeds, CRGB::Black);
}

/**@brief Function for doing something with the leds when we receive a play command.
 *
 * @details This function will be called when e receive a matching string see string_com[] above.
 *
 * @param[in] void
 */
void Leds::play(void)
{
	running = 1;
}

/**@brief Function for doing something with the leds when we receive a play command.
 *
 * @details This function will be called when e receive a matching string see string_com[] above.
 *
 * @param[in] void
 */
void Leds::pause(void)
{
	running = 0;
}

/**@brief Function for doing something with the leds when we receive a play command.
 *
 * @details This function will be called when e receive a matching string see string_com[] above.
 *
 * @param[in] void
 */
void Leds::stop(void)
{
	running = 0;
	FastLED.clear();
	FastLED.show();
}

/**@brief Function for changing the color to a new color. this will move the color by 7
 *
 * @details This function will be called from the string_com table.
 *
 * @param[in] void
 */

void Leds::colorDown(void)
{
	currentHue.hue -= 7;
	currentRgb = currentHue;
}

/**@brief Function for changing the color to a new color. this will move the color by 7
 *
 * @details This function will be called from the string_com table.
 *
 * @param[in] void
 */

void Leds::colorUp(void)
{
	currentHue.hue += 7;
	currentRgb = currentHue;
}

void Leds::shuffle(void)
{
	shuffleCnt = (shuffleCnt + 1) % 2;
}

/**@brief Function for each pattern, this one blinks 2 lights.
 *
 * @details This function will be called tick when it is the active patterns.
 *
 * @param[in] void
 */
void Leds::blinkSimple2(void)
{
	static uint8_t frame = 0;
  uint16_t numToDo = ledMap.size() ? ledMap.size() : numLeds;

	EVERY_N_MILLISECONDS( 500 )
	{
		uint8_t x;

		if (frame)
		{
			for( x = 0 ; x < numToDo ; x++ )
			{
				if ( x % 2 )
				{
					leds[Map(x)] = CRGB::Black;
				}
				else
				{
					leds[Map(x)] = currentRgb;
				}
			}

			frame = 0;
		}
		else
		{
			for( x = 0 ; x < numToDo ; x++ )
			{
				if ( x % 2 )
				{
					leds[Map(x)] = currentRgb;
				}
				else
				{
					leds[Map(x)] = CRGB::Black;
				}
			}

			frame = 1;
		}
	}

	FastLED.show();
}

/**@brief Function for each pattern, this one lights the leds all one color.
 *
 * @details This function will be called tick when it is the active patterns.
 *
 * @param[in] void
 */
void Leds::simpleColor(void)
{
	// go all blue till we get a command or timeout
	fill_solid(leds, numLeds, currentRgb);
	FastLED.show();
}

/**@brief Function for each pattern, this one blinks 2 lights.
 *
 * @details This function will be called tick when it is the active patterns.
 *
 * @param[in] void
 */
void Leds::hueColor(void)
{
	CRGB rgb = currentHue;

	fill_solid(leds, numLeds, rgb);
	FastLED.show();
}

/**@brief Function for each pattern, this one is a 1 light chase.
 *
 * @details This function will be called tick when it is the active patterns.
 *
 * @param[in] void
 */
void Leds::chase(void)
{
	// simple single led chase
	static int pos = 0;
  uint16_t numToDo = ledMap.size() ? ledMap.size() : numLeds;

	fill_solid(leds, numLeds, CRGB::Black);
	leds[Map(pos)] = currentRgb;
	pos = (pos + 1) % numToDo;
	FastLED.show();
}

void Leds::chase2()
{
	static int j = 0;
  uint16_t numToDo = ledMap.size() ? ledMap.size() : numLeds;

	EVERY_N_MILLISECONDS(300) {
		for(int x = 0 ; x < numLeds ; x++)
		{
			leds[x] = CRGB::Black;
		}

		for(int i = j ; i < numToDo ; i += num)
		{
			leds[Map(i)] = ColorFromPalette(gPal, beatsin8(5));
		}

		j++;
		j = j % num;
	}
}

/**@brief Function for each pattern, this one shows a rainbow that cycles.
 *
 * @details This function will be called tick when it is the active pattern.
 *
 * @param[in] void
 */
void Leds::rainbow(void)
{
	fill_rainbow(leds, numLeds, currentHue.hue, 7);
	FastLED.show();
}

void Leds::addGlitter( fract8 chanceOfGlitter)
{
  uint16_t numToDo = ledMap.size() ? ledMap.size() : numLeds;

	if( random8() < chanceOfGlitter) {
		leds[ Map(random16(numToDo)) ] += CRGB::White;
	}
}

void Leds::rainbowWithGlitter(void)
{
	// built-in FastLED rainbow, plus some random sparkly glitter
	rainbow();
	addGlitter(80);
}

void Leds::gConfetti(void)
{
	// random colored speckles that blink in and fade smoothly
  uint16_t numToDo = ledMap.size() ? ledMap.size() : numLeds;

	fadeToBlackBy( leds, numLeds, 10);
	int pos = random16(numToDo);
	leds[Map(pos)] += CHSV( HUE_GREEN + random8(64), 200, 255);
}

void Leds::rConfetti(void)
{
	// random colored speckles that blink in and fade smoothly
  uint16_t numToDo = ledMap.size() ? ledMap.size() : numLeds;

	fadeToBlackBy( leds, numLeds, 10);
	int pos = random16(numToDo);
	leds[Map(pos)] += CHSV( HUE_RED + random8(64), 200, 255);
}

void Leds::confetti(void)
{
	// random colored speckles that blink in and fade smoothly
  uint16_t numToDo = ledMap.size() ? ledMap.size() : numLeds;

	fadeToBlackBy( leds, numLeds, 10);
	int pos = random16(numToDo);
	CHSV confetti = currentHue;

	confetti.h += random8(64);

	leds[Map(pos)] += confetti;
}

void Leds::sinelon(void)
{
	// a colored dot sweeping back and forth, with fading trails
  uint16_t numToDo = ledMap.size() ? ledMap.size() : numLeds;

	fadeToBlackBy( leds, numLeds, 20);
	int pos = beatsin16(13,0,numToDo);
	leds[Map(pos)] += ColorFromPalette(gPal, beatsin8(20));
}

void Leds::greenlon(void)
{
	// a colored dot sweeping back and forth, with fading trails
  uint16_t numToDo = ledMap.size() ? ledMap.size() : numLeds;

	fadeToBlackBy( leds, numLeds, 20);
	int pos = beatsin16(13,0,numToDo);
	leds[Map(pos)] += CRGB::Green;
	addGlitter(40);
}

void Leds::redlon(void)
{
  uint16_t numToDo = ledMap.size() ? ledMap.size() : numLeds;

	// a colored dot sweeping back and forth, with fading trails
	fadeToBlackBy( leds, numLeds, 20);
	int pos = beatsin16(13,0,numToDo);
	leds[Map(pos)] += CRGB::Red;
	addGlitter(40);
}

void Leds::sweep(void)
{
  uint16_t numToDo = ledMap.size() ? ledMap.size() : numLeds;

	for(int i = 0 ; i < numToDo ; i++)
	{
		leds[Map(i)] = ColorFromPalette(grPal, beatsin8(5));
	}

	addGlitter(10);
}

void Leds::dark(void)
{
	for(int i = 0 ; i < numLeds ; i++)
	{
		leds[i] = CRGB::Black;
	}
}

void Leds::bpm(void)
{
	// colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint16_t numToDo = ledMap.size() ? ledMap.size() : numLeds;

	uint8_t BeatsPerMinute = 62;
	CRGBPalette16 palette = PartyColors_p;
	uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
	for( int i = 0; i < numToDo; i++) { //9948
		leds[Map(i)] = ColorFromPalette(palette, currentHue.hue + (i*2), beat-currentHue.hue + (i*10));
	}
}

void Leds::juggle(void) {
	// eight colored dots, weaving in and out of sync with each other
  uint16_t numToDo = ledMap.size() ? ledMap.size() : numLeds;

	fadeToBlackBy( leds, numLeds, 20);
	byte dothue = 0;
	for( int i = 0; i < 8; i++) {
		leds[Map(beatsin16(i+7,0,numToDo))] |= CHSV(dothue, 200, 255);
		dothue += 32;
	}
}

void Leds::christmasConfetti(void) {
	// random colored speckles that blink in and fade smoothly
  uint16_t numToDo = ledMap.size() ? ledMap.size() : numLeds;
  
	fadeToBlackBy( leds, numLeds, 10);
	int pos = random16(numToDo);
	leds[Map(pos)] =  ColorFromPalette( gPal, random8(255));
}

void Leds::christmasLights(void) {
	int x;
	CRGB ltColors[] = { CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Yellow };
	CRGB color;
	static int times = 0;
  uint16_t numToDo = ledMap.size() ? ledMap.size() : numLeds;

	EVERY_N_MILLISECONDS(600){
		for(int x = 0 ; x < numToDo ; x++)
		{
			leds[Map(x)] = CRGB::Black;
		}

		color = ltColors[times % (sizeof(ltColors) / sizeof(color))];

		for( x = 0 + (times % 4) ; x < numToDo ; x += 4)
		{
			leds[Map(x)] = color;
		}

		times++;
	}
}

void Leds::allChristmasLights(void) {
	int x;
	CRGB ltColors[] = { CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Yellow };
	static int times = 0;
  uint16_t numToDo = ledMap.size() ? ledMap.size() : numLeds;

	EVERY_N_MILLISECONDS(600){
		for( x = 0 ; x < numToDo ; x += 8)
		{
			for(int y = 0 ; y < 8 ; y++)
			{
				leds[Map(x + y)] = ltColors[(times + (x/8)) % (sizeof(ltColors) / sizeof(CRGB))];
			}
		}

		times++;
	}
}

void Leds::wipe(void) {
	int x;
	static CRGB color;
	static int wiping = 0;
  int numToWipe = ledMap.size() ? ledMap.size() : numLeds;
  
	if (wiping >= numToWipe)
	{
		wiping = 0;
		color = ColorFromPalette( gPal, random8(255));
	}

	leds[Map(wiping++)] = color;
}

void Leds::pulse(void)
{
  static uint8_t ringNum = 0;
  std::vector<uint16_t>::iterator ringIt;

  EVERY_N_MILLISECONDS(100)
  {
    // clear the entire strip
    fill_solid(leds, numLeds, CRGB::Black); 

    // we want to turn on all the lights in the current ring

    for(ringIt = Rings[ringNum].begin() ; ringIt < Rings[ringNum].end() ; ringIt++)
    {
      leds[Map(*ringIt)] = currentHue;
    }

    ringNum++;
    if (ringNum >= Rings.size())
    {
      ringNum = 0;
    }
    
    FastLED.show();
  }
}

/**@brief Function set a mapping on the LED strip.
 *
 * @details This function will be called to allow mapping of LEDS to other LEDS.
 * it is going to be used now to map the 91 vertices of the geodesic dome.
 * @param[in] pointer to the map and the number of entries in the map
 */

bool Leds::setLedMap(std::vector <uint16_t> *newMap)
{
  // we need to replace our map with the new map

  ledMap = *newMap;
  return true;
}

/**@brief Function show a test pattern on the LED strip.
 *
 * @details This function will be called to show a test on the LEDS.
 *
 * @param[in] void
 */

void Leds::ledTest(void) {
	// flash leds
	FastLED.showColor(CRGB::Red);
	delay(1000);
	FastLED.showColor(CRGB::Green);
	delay(1000);
	FastLED.showColor(CRGB::Blue);
	delay(1000);
	FastLED.showColor(CRGB::Black);
	FastLED.show();
}

String Leds::pattern(void)
{
	return(currentPattern->first);
}

std::vector <String> Leds::getPatterns(void)
{
	std::vector <String> patStrs;
	std::map <String, funcPtr_t>::iterator pat = patterns.begin();

	while(pat != patterns.end())
	{
		patStrs.push_back(pat->first);
		pat++;
	}

	return patStrs;
}

bool Leds::setPattern(String newPattern)
{    
	// see if we can find the newPattern in our list of patterns, and if so then we can set the current pattern to it.

	std::map <String, funcPtr_t>::iterator match = patterns.find(newPattern);

	if (match != patterns.end())
	{
		// we need to set the current pattern to the match.
		currentPattern = match;
		return(true);
	}

	return(false);
}

/**@brief Function for doing all the fastled stuff. this happens every 100ms or so.
 *
 * @details This function will be called each fastled tick.
 *
 * @param[in] void
 */

void Leds::loop(int nowTime)
{
  static bool _inited = false;

  if (!_inited && (nowTime > 0)) {
    // we should figure out if we are running or not, we start running so only stop us if needed

    if ((startTime >= 0) && (stopTime >= 0) && (startTime != stopTime)) {
      if (startTime > stopTime) {
        if ((nowTime >= stopTime) && (nowTime < startTime)) {
          // we are not running
          stop();
        }
      }
      else if ((nowTime < startTime) || (nowTime >= stopTime)) {
        stop();
      }
    }
    
    // don't do this again
    
    _inited = true;
  }
  
	// only if there are non-negative non-matching times
	if (_inited && ((startTime >= 0) && (stopTime >= 0) && (startTime != stopTime)))
	{
		if ((nowTime == startTime) && !running) 
		{
			play();
		}
		else if ((nowTime == stopTime) && running)
		{
			stop();     
		}
	}

  if (artnetEnabled)
  {
    _artnet->read();
  }
  
	if (!running)
	{
		return;  
	}

	switch(mode)
	{
	case PATTERN_MODE:
	case PATTERN_CYCLE_MODE:
	{
		// call the current pattern

		((*this).*currentPattern->second)();

		// do some periodic updates
		EVERY_N_MILLISECONDS( hueCycleTime )
		{
			currentHue.hue++;
			currentRgb = currentHue;
		} // slowly cycle the "base color" through the rainbow

		if (mode == PATTERN_CYCLE_MODE)
		{
			EVERY_N_SECONDS( patCycleTime )
      {
				ff(); // every 5 minutes change the mode
      }
		}

		break;
	}

	case COLOR_MODE:
	{
		simpleColor();
		break;  
	}

  case ARTNET_MODE:
  {
    // if we are in artnet mode and haven't seen a packet in a while, then change back to the old mode
    if ((artNetRecved + artnetWaitTime) < millis())
    {
      mode = lastMode;
    }
    break;
  }
  
	case STOP_MODE:
	default:
		// do nothing for now
		break;
	}
}

#if 0
/**@brief Function for parsing the commands to see what to do.
 *
 * @details This function will be called whenever a change to the mode is needed.
 *
 * @param[in]   char const *command, uint8_t length of command string
 */

void Leds::processCommands(char const *command, uint16_t len)
{
	String s(command, len);
	std::map <String, funcPtr_t>::iterator match = stringCom.find(s);

	if (match != stringCom.end())
	{
		((*this).*match->second)();
	}
}
#endif

int Leds::timezone(void) {
  return(timeZone_);
}

static EOrder strToColorOrder(String colorOrder)
{
  if (colorOrder == "RBG")
  {
    return RBG;  
  }

  if (colorOrder == "GRB")
  {
    return GRB;  
  }
 
  if (colorOrder == "GBR")
  {
    return GBR;  
  }

  if (colorOrder == "BRG")
  {
    return BRG;  
  }
 
  if (colorOrder == "BGR")
  {
    return BGR;  
  }
  
  return RGB;
}

static String colorOrderToStr(EOrder colorOrder)
{
  switch (colorOrder)
  {
    case RGB:
      return("RGB");
      break;
    case RBG:
      return("RBG");
      break;
    case GRB:
      return("GRB");
      break;
    case GBR:
      return("GBR");
      break;
    case BRG:
      return("BRG");
      break;
    case BGR:
      return("BGR");
      break;
    default:
      return("RGB");
      break;
  }

  return("");
}

static const char *configFilename = "/led.json";
static const char *tmpConfigFilename = "/led.tmp";
static const char *configBackFilename = "/led.bak.json";

bool Leds::readConfig(void) {
	bool status = false;
	int configSize = 0;

	if (!SPIFFS.exists(configFilename)) {
		// if it doesn't exist then return false

		DBG_OUTPUT_PORT.print("Config file ");
		DBG_OUTPUT_PORT.print(configFilename);
		DBG_OUTPUT_PORT.println(" doesn't exist");
		return false;
	}

	// file exists, open it

	File configFile = SPIFFS.open(configFilename, "r");

	if (!configFile) {
		// if we can't open it then return false
		DBG_OUTPUT_PORT.print("Config file ");
		DBG_OUTPUT_PORT.print(configFilename);
		DBG_OUTPUT_PORT.println(" open failed");
		return false;
	}

	// get the size of the file

	configSize = configFile.size();

	DynamicJsonBuffer jsonBuffer;

	// allocate space for the file

	char *configJson = (char *) calloc(configSize, 1);

	// read in the file

	if (configFile.readBytes(configJson, configSize) == 0) {
		DBG_OUTPUT_PORT.print("Config file ");
		DBG_OUTPUT_PORT.print(configFilename);
		DBG_OUTPUT_PORT.print(" read failed size ");
		DBG_OUTPUT_PORT.println(configSize);
		goto cleanup;
	} else {
		// we have hopefully a JSON string in memory, we need to parse it

		JsonObject& ledJson = jsonBuffer.parseObject(configJson);

		if (ledJson.success()) {
			// we should be able to do cool things here. But for now let's print it

			ledJson.prettyPrintTo(Serial);
			Serial.println();

			// see if there is a mode

			if (ledJson.containsKey("mode"))
			{
				String configMode;

				// read it in.

				configMode = ledJson["mode"].asString();

				for(int x = STOP_MODE; x < LAST_MODE ; x++)
				{
					if (configMode == modeNames[x])
					{
						mode = (Modes) x;
					}
				}
			}

			if (ledJson.containsKey("startTime"))
			{
				int tm;

				// read it in.

				tm = ledJson["startTime"];

				if ((tm >= 0) && (tm < ONE_DAY))
				{
					startTime = tm;
				}
			}

			if (ledJson.containsKey("stopTime"))
			{
				int tm;

				// read it in.

				tm = ledJson["stopTime"];

				if ((tm >= 0) && (tm < ONE_DAY))
				{
					stopTime = tm;
				}
			}

      if (ledJson.containsKey("timeZone"))
      {
        timeZone_ = (int) (ledJson["timeZone"]) * 3600;
      }
      
			if (ledJson.containsKey("hueCycleTime"))
			{
				int tm;

				// read it in.

				tm = ledJson["hueCycleTime"];

				if ((tm >= 0) && (tm < 1000))
				{
					hueCycleTime = tm;
				}
			}

			if (ledJson.containsKey("patCycleTime"))
			{
				int tm;

				// read it in.

				tm = ledJson["patCycleTime"];

				if ((tm >= 0) && (tm < 1000))
				{
					patCycleTime = tm;
				}
			}

			if (ledJson.containsKey("color"))
			{
				extern uint8_t convertHex(const char *str);
				String colorString;
				const char *colorStr;
				CRGB newColor;

				// read it in.

				colorString = ledJson["color"].asString();

				colorStr = colorString.c_str();

				if (colorStr[0] == '#')
				{
					Serial.print("We got a color (");
					Serial.print(colorStr);
					Serial.println(")");

					// get the rest of the info
					newColor.red = convertHex(colorStr + 1);
					newColor.green = convertHex(colorStr + 3);
					newColor.blue = convertHex(colorStr + 5);

					// set the color

					currentRgb = newColor;
				}
			}

      if (ledJson.containsKey("colorOrder"))
      {
        Serial.print("We got a colorOrder (");
        Serial.print(ledJson["colorOrder"].asString());
        Serial.println(")");
        
        colorOrder_ = strToColorOrder(ledJson["colorOrder"].asString());
      }
      
      if (ledJson.containsKey("numLeds"))
      {
        // read it in

        numLeds = ledJson["numLeds"];
      }
 
      if (ledJson.containsKey("artnetEnabled"))
      {
        // read it in

        artnetEnabled = (bool) ledJson["artnetEnabled"];
      }
      
      if (ledJson.containsKey("artnetWaitTime"))
      {
        // read it in

        artnetWaitTime = (int) (ledJson["artnetWaitTime"]) * 1000;
      }
      
      if (ledJson.containsKey("artnetPort"))
      {
        // read it in, but it is an IP port and can really only be 16Bits

        artnetPort = (uint16_t) ledJson["artnetPort"];
      }
      
      if (ledJson.containsKey("startUniverse"))
      {
        _startUniverse = ledJson["startUniverse"];
      }
      
			// we parsed it and stored it in the passed in struct, return true
      
      DBG_OUTPUT_PORT.println("Successfully read config file ");

			status = true;
			goto cleanup;
		}
		else {
			// failed to parse, return false
			DBG_OUTPUT_PORT.print("Config file ");
			DBG_OUTPUT_PORT.print(configFilename);
			DBG_OUTPUT_PORT.println(" failed to parse JSON");
			goto cleanup;
		}
	}

	cleanup:
	configFile.close();

	free(configJson);
	return status;
}

bool Leds::writeConfig(void) {
	// so now we have to build a jsonObject

	DynamicJsonBuffer jsonBuffer;
	JsonObject& ledJson = jsonBuffer.createObject();

	ledJson["mode"] = String(modeNames[mode]);
	ledJson["startTime"] = startTime;
	ledJson["stopTime"] = stopTime;
  ledJson["timeZone"] = timeZone_ / 3600;
	ledJson["hueCycleTime"] = hueCycleTime;
	ledJson["patCycleTime"] = patCycleTime;\

	// color is a little more complex
	char tmpBuf[20];

	sprintf(tmpBuf, "#%02x%02x%02x", currentRgb.red, currentRgb.green, currentRgb.blue);

	ledJson["color"] = String(tmpBuf);

  ledJson["colorOrder"] = colorOrderToStr(colorOrder_);
  ledJson["numLeds"] = (int) numLeds;

  ledJson["artnetEnabled"] = (int) artnetEnabled;
  ledJson["artnetWaitTime"] = (int) artnetWaitTime / 1000;
  ledJson["artnetPort"] = artnetPort;
  ledJson["startUniverse"] = _startUniverse;
    
	ledJson.prettyPrintTo(Serial);
	Serial.println("");

	// so now we should write this out to a file
	// then rename the original to name.bak
	// then rename our file to name

	// create the file or truncate it

	File configFile = SPIFFS.open(tmpConfigFilename, "w");

	if (!configFile) {
		// if we can't open it then return false
		DBG_OUTPUT_PORT.print("Write Config file ");
		DBG_OUTPUT_PORT.print(tmpConfigFilename);
		DBG_OUTPUT_PORT.println(" open failed");
		return false;
	}

	// write to the file

	ledJson.prettyPrintTo(configFile);
	configFile.close();

  // get rid of any old backups

  SPIFFS.remove(configBackFilename);
  
	// now I want to move the old file out of the way

	SPIFFS.rename(configFilename, configBackFilename);
	SPIFFS.rename(tmpConfigFilename, configFilename);

	DBG_OUTPUT_PORT.println("Write Config file OK");
	return true;
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  int sendFrame = 1;
  
  // if we are not in ARTNET mode then we should be

  if (myLeds->getMode() != "Artnet")
  {
    myLeds->setMode(ARTNET_MODE);
  }

  artNetRecved = millis();  // keep track of the last time we were being controlled
  
  // set brightness of the whole strip
  if (universe == 15)
  {
    FastLED.setBrightness(data[0]);
    FastLED.show();
  }

  // Store which universe has got in
  if ((universe - startUniverse) < maxUniverses)
  {
    universesReceived[universe - startUniverse] = 1;
  }
  
  for (int i = 0 ; i < maxUniverses ; i++)
  {
    if (universesReceived[i] == 0)
    {
      //Serial.println("Broke");
      sendFrame = 0;
      break;
    }
  }

  int maxLeds = ((length / 3) < NUM_UNI_LEDS) ? (length / 3) : NUM_UNI_LEDS;

  // read universe and put into the right part of the display buffer
  for (int i = 0; i < maxLeds; i++)
  {
    uint16_t numToDo = myLeds->mapSize() ? myLeds->mapSize() : myLeds->numLeds;
    
    int led = i + (universe - startUniverse) * NUM_UNI_LEDS;
    
    if (led < numToDo)
    {
      myLeds->leds[myLeds->Map(i)] = CRGB(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
  }

  if (sendFrame)
  {
    FastLED.show();
    // Reset universeReceived to 0
    memset(universesReceived, 0, maxUniverses);
  }
}

