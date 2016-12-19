#include "Leds.h"
#include <FS.h>
#include <ArduinoJson.h>

#define DBG_OUTPUT_PORT Serial

String modeNames[LAST_MODE + 1] = {
		"Stop",
		"Color",
		"Pattern",
		"Pattern_Cycle",
		"Last"
};

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

	// set to cycle through the patterns

	mode = PATTERN_CYCLE_MODE;

	readConfig();

	shuffleCnt = 0;

	num = 3;

	// create the default pixel mapping. This is a normal map 0 = 0, 1 = 1, ... 24 = 24.

	FastLED.addLeds<LED_TYPE, SPI_DATA, SPI_CLOCK, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);;

	// build our vector of patterns here
	patterns["allChristmasLights"] = &Leds::allChristmasLights;
	patterns["wipe"] = &Leds::wipe;
	patterns["christmasConfetti"] = &Leds::christmasConfetti;
	patterns["chase"] = &Leds::chase;
	patterns["christmasLights"] = &Leds::christmasLights;
	patterns["sweep"] = &Leds::sweep;
	patterns["dark"] = &Leds::dark;
#if 0
	patterns.push_back(&Leds::blinkSimple2);
	patterns.push_back(&Leds::simpleColor);
	patterns.push_back(&Leds::hueColor);
	patterns.push_back(&Leds::chase);
	patterns.push_back(&Leds::rainbow);
	patterns.push_back(&Leds::diag);
#endif
// this is not really approved, but we just put patterns in there, 0 must be valid

	currentPattern = patterns.begin();
	running = 1;

	gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Green, CRGB::Blue);
	grPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Green, CRGB::Green);
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
	}

	fill_solid(leds, NUM_LEDS, CRGB::Black);
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
	}

	currentPattern--;


	fill_solid(leds, NUM_LEDS, CRGB::Black);
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

	EVERY_N_MILLISECONDS( 500 )
	{
		uint8_t x;

		if (frame)
		{
			for( x = 0 ; x < NUM_LEDS ; x++ )
			{
				if ( x % 2 )
				{
					leds[x] = CRGB::Black;
				}
				else
				{
					leds[x] = currentRgb;
				}
			}

			frame = 0;
		}
		else
		{
			for( x = 0 ; x < NUM_LEDS ; x++ )
			{
				if ( x % 2 )
				{
					leds[x] = currentRgb;
				}
				else
				{
					leds[x] = CRGB::Black;
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
	fill_solid(leds, NUM_LEDS, currentRgb);
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

	fill_solid(leds, NUM_LEDS, rgb);
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

	fill_solid(leds, NUM_LEDS, CRGB::Black);
	leds[pos] = currentRgb;
	pos = (pos + 1) % NUM_LEDS;
	FastLED.show();
}

void Leds::chase2()
{
	static int j = 0;

	EVERY_N_MILLISECONDS(300) {
		for(int x = 0 ; x < NUM_LEDS ; x++)
		{
			leds[x] = CRGB::Black;
		}

		for(int i = j ; i < NUM_LEDS ; i += num)
		{
			leds[i] = ColorFromPalette(gPal, beatsin8(5));
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
	fill_rainbow(leds, NUM_LEDS, currentHue.hue, 7);
	FastLED.show();
}

void Leds::addGlitter( fract8 chanceOfGlitter)
{
	if( random8() < chanceOfGlitter) {
		leds[ random16(NUM_LEDS) ] += CRGB::White;
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
	fadeToBlackBy( leds, NUM_LEDS, 10);
	int pos = random16(NUM_LEDS);
	leds[pos] += CHSV( HUE_GREEN + random8(64), 200, 255);
}

void Leds::rConfetti(void)
{
	// random colored speckles that blink in and fade smoothly
	fadeToBlackBy( leds, NUM_LEDS, 10);
	int pos = random16(NUM_LEDS);
	leds[pos] += CHSV( HUE_RED + random8(64), 200, 255);
}

void Leds::confetti(void)
{
	// random colored speckles that blink in and fade smoothly
	fadeToBlackBy( leds, NUM_LEDS, 10);
	int pos = random16(NUM_LEDS);
	CHSV confetti = currentHue;

	confetti.h += random8(64);

	leds[pos] += confetti;
}

void Leds::sinelon(void)
{
	// a colored dot sweeping back and forth, with fading trails
	fadeToBlackBy( leds, NUM_LEDS, 20);
	int pos = beatsin16(13,0,NUM_LEDS);
	leds[pos] += ColorFromPalette(gPal, beatsin8(20));
}

void Leds::greenlon(void)
{
	// a colored dot sweeping back and forth, with fading trails
	fadeToBlackBy( leds, NUM_LEDS, 20);
	int pos = beatsin16(13,0,NUM_LEDS);
	leds[pos] += CRGB::Green;
	addGlitter(40);
}

void Leds::redlon(void)
{
	// a colored dot sweeping back and forth, with fading trails
	fadeToBlackBy( leds, NUM_LEDS, 20);
	int pos = beatsin16(13,0,NUM_LEDS);
	leds[pos] += CRGB::Red;
	addGlitter(40);
}

void Leds::sweep(void)
{
	for(int i = 0 ; i < NUM_LEDS ; i++)
	{
		leds[i] = ColorFromPalette(grPal, beatsin8(5));
	}

	addGlitter(10);
}

void Leds::dark(void)
{
	for(int i = 0 ; i < NUM_LEDS ; i++)
	{
		leds[i] = CRGB::Black;
	}
}

void Leds::bpm(void)
{
	// colored stripes pulsing at a defined Beats-Per-Minute (BPM)
	uint8_t BeatsPerMinute = 62;
	CRGBPalette16 palette = PartyColors_p;
	uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
	for( int i = 0; i < NUM_LEDS; i++) { //9948
		leds[i] = ColorFromPalette(palette, currentHue.hue + (i*2), beat-currentHue.hue + (i*10));
	}
}

void Leds::juggle(void) {
	// eight colored dots, weaving in and out of sync with each other
	fadeToBlackBy( leds, NUM_LEDS, 20);
	byte dothue = 0;
	for( int i = 0; i < 8; i++) {
		leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
		dothue += 32;
	}
}

void Leds::christmasConfetti(void) {
	// random colored speckles that blink in and fade smoothly
	fadeToBlackBy( leds, NUM_LEDS, 10);
	int pos = random16(NUM_LEDS);
	leds[pos] =  ColorFromPalette( gPal, random8(255));
}

void Leds::christmasLights(void) {
	int x;
	CRGB ltColors[] = { CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Yellow };
	CRGB color;
	static int times = 0;

	EVERY_N_MILLISECONDS(600){
		for(int x = 0 ; x < NUM_LEDS ; x++)
		{
			leds[x] = CRGB::Black;
		}

		color = ltColors[times % (sizeof(ltColors) / sizeof(color))];

		for( x = 0 + (times % 4) ; x < NUM_LEDS ; x += 4)
		{
			leds[x] = color;
		}

		times++;
	}
}

void Leds::allChristmasLights(void) {
	int x;
	CRGB ltColors[] = { CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Yellow };
	static int times = 0;

	EVERY_N_MILLISECONDS(600){
		for( x = 0 ; x < NUM_LEDS; x += 8)
		{
			for(int y = 0 ; y < 8 ; y++)
			{
				leds[x + y] = ltColors[(times + (x/8)) % (sizeof(ltColors) / sizeof(CRGB))];
			}
		}

		times++;
	}
}

void Leds::wipe(void) {
	int x;
	static CRGB color;
	static int wiping = 0;

	if (wiping >= NUM_LEDS)
	{
		wiping = 0;
		color = ColorFromPalette( gPal, random8(255));
	}

	leds[wiping++] = color;
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
	// only if there are non-negative non-matching times
	if ((startTime >= 0) && (stopTime >= 0) && (startTime != stopTime))
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
        timeZone_ = ledJson["timeZone"];
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

			// we parsed it and stored it in the passed in struct, return true

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
  ledJson["timeZone"] = timeZone_;
	ledJson["hueCycleTime"] = hueCycleTime;
	ledJson["patCycleTime"] = patCycleTime;

	// color is a little more complex
	char tmpBuf[20];

	sprintf(tmpBuf, "#%02x%02x%02x", currentRgb.red, currentRgb.green, currentRgb.blue);

	ledJson["color"] = String(tmpBuf);

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

	ledJson.printTo(configFile);
	configFile.close();

	// now I want to move the old file out of the way

	SPIFFS.rename(configFilename, configBackFilename);
	SPIFFS.rename(tmpConfigFilename, configFilename);

	DBG_OUTPUT_PORT.println("Write Config file OK");
	return true;
}


