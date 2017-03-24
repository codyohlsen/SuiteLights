  // @ SUITE HOUSE PRODUCTIONS

//#include <Adafruit_NeoPixel.h>
#include <OctoWS2811.h>
#include <FastLED.h>
#include <Audio.h>
#include <Wire.h>

#define NUM_SIDE_STRIPS 2
#define SIDE_STRIP_LEN 60

#define NUM_CUPS 10
#define NUM_PIXELS_PER_CUP 24
#define NUM_LEDS_CUPS (NUM_CUPS * NUM_PIXELS_PER_CUP)

#define MATRIX_WIDTH 60
#define MATRIX_HEIGHT 8

#define MAIN_CUPS_PIN 9
#define GUEST_CUPS_PIN 10
#define SIDE_STRIPS_PIN 11
#define LINE_IN_PIN A3
// MATRIX pins are predefined by the OctoWS library.

//#define NEOPIXEL_CONFIG (NEO_RGB + NEO_KHZ800)
#define OCTOWS_CONFIG (WS2811_RGB | WS2811_800kHz)

#define INVALID Serial.println("Should not be here.")

#include "MusicHandlers.h"
#include "LedsWrappers.h"
#include "SuiteLightsComponent.h"

//// Audio library objects for listening to line in
AudioInputAnalog         adc1(LINE_IN_PIN);       //xy=99,55
AudioAnalyzeFFT1024      fft;            //xy=265,75
AudioConnection          patchCord1(adc1, fft);

// Music stuff
StripMusicHandler stripMusicHandler = StripMusicHandler(fft);
TenCupsMusicHandler cupsMuiscHandler = TenCupsMusicHandler(fft);
MatrixMusicHandler matrixMusicHandler = MatrixMusicHandler(fft);

// Callbacks
void mainCupsComplete(uint8_t cupNumber);
void guestCupsComplete(uint8_t cupNumber);
void matrixComplete(uint8_t unused);
void sidesComplete(uint8_t unused);

// Side Strips
LedsFastWrapper stripsWrapper = LedsFastWrapper(SIDE_STRIPS_PIN);
SuiteLights sideStrips = SuiteLights(stripsWrapper, stripMusicHandler, &sidesComplete, NUM_SIDE_STRIPS);

// Cups lights
//LedsNeoWrapper mainCupsWrapper = LedsNeoWrapper(NUM_CUPS * NUM_PIXELS_PER_CUP, MAIN_CUPS_PIN, NEOPIXEL_CONFIG);
//LedsNeoWrapper guestCupsWrapper = LedsNeoWrapper(NUM_CUPS * NUM_PIXELS_PER_CUP, GUEST_CUPS_PIN, NEOPIXEL_CONFIG);
LedsFastWrapper guestCupsWrapper = LedsFastWrapper(GUEST_CUPS_PIN);
LedsFastWrapper mainCupsWrapper = LedsFastWrapper(MAIN_CUPS_PIN);

SuiteLights mainCups = SuiteLights(mainCupsWrapper, cupsMuiscHandler, &mainCupsComplete, NUM_CUPS);
SuiteLights guestCups = SuiteLights(guestCupsWrapper, cupsMuiscHandler, &guestCupsComplete, NUM_CUPS);

// OctoWS2811 matrix
const int ledsPerPin = MATRIX_WIDTH * MATRIX_HEIGHT / 8;
DMAMEM int displayMemory[ledsPerPin*6];
int drawingMemory[ledsPerPin*6];
const int config = OCTOWS_CONFIG;
LedsOctoWrapper matrixWrapper = LedsOctoWrapper(ledsPerPin, displayMemory, drawingMemory, config);
SuiteLights suiteMatrix = SuiteLights(matrixWrapper, matrixMusicHandler, &matrixComplete, 1);

void setup()
{
  delay(1000);
  Serial.begin(115200);
  // the audio library needs to be given memory to start working
  AudioMemory(12);    

  pinMode(GUEST_CUPS_PIN, OUTPUT);
  pinMode(MAIN_CUPS_PIN, OUTPUT);
  pinMode(SIDE_STRIPS_PIN, OUTPUT);

  Serial.println("INitializing Suite House components...");
  // Begins the suite house components.
  Serial.println("side strips..");
  sideStrips.begin();
  Serial.println("main cups..");
  mainCups.begin();
  Serial.println("guest cups..");
  guestCups.begin();
  Serial.println("suite matrx..");
  suiteMatrix.begin();
  Serial.println("Finished initializing. Starting defautls..");

  defaultControls();
  delay(1000);
}

// Callback definitions.
void mainCupsComplete(uint8_t cupNumber) { };
void guestCupsComplete(uint8_t cupNumber) { };
void matrixComplete ( uint8_t unused) { };
void sidesComplete ( uint8_t unused) { };

// Main loop
void loop()
{
//  Serial.println("hello");
  checkControls();
  boolean fftAvailable = fft.available();
  suiteMatrix.Update(fftAvailable);
  mainCups.Update(fftAvailable);
  guestCups.Update(fftAvailable);
  sideStrips.Update(fftAvailable);
}

// Default everything to music.
void defaultControls() {
  uint32_t interval = 10;
//  CRGB color2 = CRGB( 50, 50, 0);//guestCups.Color(0, 255, 0);
//  CRGB color1 = CRGB( 0, 50, 50);//150;//guestCups.Color(255, 0, 0);
  CRGB color1 = CRGB( 50, 0, 0);//150;//guestCups.Color(255, 0, 0);
  CRGB color2 = CRGB( 0, 0, 0);//150;//guestCups.Color(255, 0, 0);
  suiteMatrix.Music(color1, color2);
//  suiteMatrix.RainbowCycleAll(2000);
//  suiteMatrix.ScannerAll(color1, 400);
//  mainCups.RainbowCycleAll(1000);
//  guestCups.RainbowCycleAll(1000);
//  mainCups.Music(color1, color2);
//  guestCups.Music(color1, color2);
//  mainCups.Scanner(0, color1, 100);
//  guestCups.Scanner(0, color1, 100);
//   mainCups.Scanner(1, color1, 100);
//  guestCups.Scanner(1, color1, 100);
//  mainCups.ScannerAll(color1, interval);
//  guestCups.ScannerAll(color1, interval);
//  sideStrips.ScannerAll(color1, 1000);
  mainCups.TheaterChase(0, color1, color2, 1000);
  guestCups.TheaterChase(0, color1, color2, 1000);
  sideStrips.TheaterChase(0, color1, color2, 100);
////  suiteMatrix.TheaterChase(0, color1, color2, interval);
//  for(int i = 0; i < 10; i++) {
//    guestCups.TheaterChase(i, color1, color2, 100);
//    mainCups.TheaterChase(i, color1, color2, 100);
//  }
}

// Checks controls to change modes on table.
void checkControls() {
//    // Switch patterns on a button press:
//    if (digitalRead(8) == LOW) // Button #1 pressed
//    {
//        // Switch Strip1 to FASE pattern
//        Strip1.ActivePattern = FADE;
//        Strip1.Interval = 20;
//        // Speed up the rainbow on Strip2
//        Strip2.Interval = 0;
//        // Set stick to all red
//        Stick.ColorSet(Stick.Color(255, 0, 0));
//    }
//    else if (digitalRead(9) == LOW) // Button #2 pressed
//    {
//        // Switch to alternating color wipes on Rings1 and 2
//        Strip1.ActivePattern = COLOR_WIPE;
//        Strip2.ActivePattern = COLOR_WIPE;
//        Strip2.TotalSteps = Strip2.numPixels();
//        // And update tbe stick
//        Stick.Update();
//    }
//    else // Back to normal operation
//    {
//        // Restore all pattern parameters to normal values
//        Strip1.ActivePattern = THEATER_CHASE;
//        Strip1.Interval = 100;
//        Strip2.ActivePattern = RAINBOW_CYCLE;
//        Strip2.TotalSteps = 255;
//        Strip2.Interval = min(10, Strip2.Interval);
//        // And update tbe stick
//        Stick.Update();
//    }  
}

