// @ SUITE HOUSE PRODUCTIONS
// Classes to wrap Neopixel and OctoWS libraries as a singular interface to be used as SuiteHouse Objects.
class LedsNeoWrapper;
class LedsWrapper
{
  public:
  LedsWrapper() { }
  LedsWrapper(LedsNeoWrapper& other) {
    
  }
  virtual void show() {INVALID;}
  virtual void begin() { INVALID;}
  virtual uint16_t numPixels() { INVALID; return 0;}
  virtual void setPixelColor(uint16_t index, uint8_t color) {INVALID; }
  virtual uint8_t getPixelColor(uint16_t index) { INVALID; return 0;}
  virtual uint8_t Color(uint8_t r, uint8_t g, uint8_t b) { INVALID; return 0;}
};
//
//class LedsNeoWrapper : public Adafruit_NeoPixel, public LedsWrapper {
//  public: 
//  LedsNeoWrapper(uint16_t pixels, uint8_t pin, uint8_t type) : Adafruit_NeoPixel( pixels,  pin,  type) { }
//  void show() override {
//    Adafruit_NeoPixel::show();
//  }
//  void begin() override {
//    Adafruit_NeoPixel::begin();
//  }
//   uint16_t numPixels() override { return Adafruit_NeoPixel::numPixels(); }
//   void setPixelColor(uint16_t index, uint8_t color) override {Adafruit_NeoPixel::setPixelColor(index, color); }
//   uint8_t getPixelColor(uint16_t index) override { return Adafruit_NeoPixel::getPixelColor(index);}
//   uint8_t Color(uint8_t r, uint8_t g, uint8_t b) override { return Adafruit_NeoPixel::Color(r,g,b);}
//};

class LedsFastWrapper : public LedsWrapper {
  struct CRGB leds[NUM_LEDS_FASTLED];
  public: 
  LedsFastWrapper(uint8_t pin) {
    if (pin == 9) {
      FastLED.addLeds<WS2811, 9, RGB>((struct CRGB*)leds, NUM_LEDS_FASTLED);
    } else if (pin == 10) {
      FastLED.addLeds<WS2811, 10, RGB>((struct CRGB*)leds, NUM_LEDS_FASTLED);
    }
  }
  void show() override {
    FastLED.show();
  }
  void begin() override {
    // no need for initialization from FASTLED
  }
   uint16_t numPixels() override { return NUM_LEDS_FASTLED; }
   void setPixelColor(uint16_t index, uint8_t color) override { leds[index] = color; }
   uint8_t getPixelColor(uint16_t index) override { return leds[index];}
   uint8_t Color(uint8_t r, uint8_t g, uint8_t b) override { return 50;}//Adafruit_NeoPixel::Color(r,g,b);}
};

class LedsOctoWrapper : public OctoWS2811, public LedsWrapper { 
  public: 
  uint32_t num;
  LedsOctoWrapper(uint32_t numPerStrip, void *frameBuf, void *drawBuf, uint8_t config) : OctoWS2811( numPerStrip,  frameBuf,  drawBuf, config) {
    num = numPerStrip;
  }
  void show() override {
    OctoWS2811::show();
  }
  void begin() override {
    OctoWS2811::begin();
  }
  uint16_t numPixels() override { return num * 8; }
  uint8_t getPixelColor(uint16_t index) override { return getPixel(index); }
   void setPixelColor(uint16_t index, uint8_t color) override {OctoWS2811::setPixel(index, color); }
   uint8_t Color(uint8_t r, uint8_t g, uint8_t b) override { return 200;}//Adafruit_NeoPixel::Color(r,g,b);}
};
