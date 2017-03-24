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
  virtual void setPixelColor(uint16_t index, CRGB color) {INVALID; }
  virtual CRGB getPixelColor(uint16_t index) { INVALID; return 0;}
  virtual CRGB Color(uint8_t r, uint8_t g, uint8_t b) { INVALID; return 0;}
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
//   void setPixelColor(uint16_t index, CRGB color) override {Adafruit_NeoPixel::setPixelColor(index, Adafruit_NeoPixel::Color(color.red, color.green, color.blue)); }
//   uint8_t getPixelColor(uint16_t index) override { return Adafruit_NeoPixel::getPixelColor(index);}
//   CRGB Color(uint8_t r, uint8_t g, uint8_t b) override { return Adafruit_NeoPixel::Color(r,g,b);}
//};

class LedsFastWrapper : public LedsWrapper {
  struct CRGB leds[NUM_LEDS_CUPS]; // using this same length for the side strips even though it is larger.
  uint8_t m_pin;
  public: 
  LedsFastWrapper(uint8_t pin) {
    m_pin = pin;
    if (m_pin == MAIN_CUPS_PIN) {
      FastLED.addLeds<WS2811, MAIN_CUPS_PIN, RGB>((struct CRGB*)leds, NUM_LEDS_CUPS);
    } else if (m_pin == GUEST_CUPS_PIN) {
      FastLED.addLeds<WS2811, GUEST_CUPS_PIN, RGB>((struct CRGB*)leds, NUM_LEDS_CUPS);
    } else if (m_pin == SIDE_STRIPS_PIN) {
      FastLED.addLeds<WS2811, SIDE_STRIPS_PIN, RGB>((struct CRGB*)leds, SIDE_STRIP_LEN * NUM_SIDE_STRIPS);
    }
  }
  void show() override {
    FastLED.show();
  }
  void begin() override {
    // no need for initialization from FASTLED
  }
   uint16_t numPixels() override { uint16_t ret =  m_pin == SIDE_STRIPS_PIN? SIDE_STRIP_LEN * NUM_SIDE_STRIPS : NUM_LEDS_CUPS; 
//   Serial.print("Returning: ");
//   Serial.println(ret);
   return ret;
   }
   void setPixelColor(uint16_t index, CRGB color) override { leds[index] = color; }
   CRGB getPixelColor(uint16_t index) override { return leds[index];}
   CRGB Color(uint8_t r, uint8_t g, uint8_t b) override { return CRGB(r, g, b);}
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
  CRGB getPixelColor(uint16_t index) override { 
    uint32_t pix = getPixel(index); 
    return CRGB(pix & 0xFF, (pix & 0x00FF) << 8, (pix & 0x0000FF) << 16);
  }
   void setPixelColor(uint16_t index, CRGB color) override {
    uint32_t color_octo = color.blue;
    color_octo = color_octo << 8;
    color_octo += color.green;
    color_octo = color_octo << 8;
    color_octo += color.red;
    OctoWS2811::setPixel(index, color_octo);
    }
   CRGB Color(uint8_t r, uint8_t g, uint8_t b) override { return CRGB(r, g, b);}
};
