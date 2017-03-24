// @ SUITE HOUSE PRODUCTIONS
// Suite Lights Object - Each compoenent on the Suite Table is an object.
// Pattern types supported:
enum  pattern { NONE, RAINBOW_CYCLE, THEATER_CHASE, COLOR_WIPE, SCANNER, FADE, MUSIC};
// Patern directions supported:
enum  direction { FORWARD, REVERSE };

#define MAX_NUM_SPLITS 10

class SuiteLights
{
    public:
    MusicHandler& music;
    
    // Member Variables:  
    uint16_t numPixelsPerCup;
    uint8_t numCups;
    LedsWrapper& leds;
    pattern ActivePatterns[MAX_NUM_SPLITS];  // which pattern is running
    direction Directions[MAX_NUM_SPLITS];     // direction to run the pattern
    
    unsigned long Interval;   // milliseconds between updates
    unsigned long lastUpdate; // last update of position (all 10 cups)
    
    CRGB Color1, Color2;  // What colors are in use (global for now)
    uint16_t TotalSteps[MAX_NUM_SPLITS];  // total number of steps in the pattern
    uint16_t Indexes[MAX_NUM_SPLITS];  // current step within the pattern
    
    void (*OnComplete)(uint8_t);  // Callback on completion of pattern
    
    // Constructor - calls base-class constructor to initialize strip
     SuiteLights(LedsWrapper& l, MusicHandler& handler, void (*callback)(uint8_t), uint8_t cups = MAX_NUM_SPLITS) : music(handler), numCups(cups), leds(l)
    {
        OnComplete = callback;
    }

    void begin() {
      music.begin();
      leds.begin();
      leds.show();
    }

    boolean isPlayingMusic() {
      return ActivePatterns[0] == MUSIC;
    }
    
    // Update the pattern
    void Update(boolean isFFTavailable)
    {
        if((millis() - lastUpdate) > Interval) // time to update
        {
            lastUpdate = millis();

            if (isPlayingMusic()) {
              if (isFFTavailable) {
                MusicUpdate();
              }
            } else {
              for (int i = 0; i < numCups; ++i) {
                switch(ActivePatterns[i])
                {
                    case RAINBOW_CYCLE:
                        RainbowCycleUpdate(i);
                        break;
                    case THEATER_CHASE:
                        TheaterChaseUpdate(i);
                        break;
                    case COLOR_WIPE:
                        ColorWipeUpdate(i);
                        break;
                    case SCANNER:
                        ScannerUpdate(i);
                        break;
                    case FADE:
                        FadeUpdate(i);
                        break;
                    default:
                        break;
                }
              }
            }

            leds.show();
            Increment();
        }
    }
  
    // Increment the Index and reset at the end
    void Increment()
    {
      uint8_t completed[numCups];
      for (int i = 0; i < numCups; ++i) {
        if (Directions[i] == FORWARD)
        {
           Indexes[i]++;
           if (Indexes[i] >= TotalSteps[i])
            {
                Indexes[i] = 0; // Restart
                completed[i] = 1;
            } else {
              completed[i] = 0;
            }
        }
        else // Directions[i] == REVERSE
        {
            Indexes[i]--;
            if (Indexes[i] <= 0)
            {
                Indexes[i] = TotalSteps[i]-1; // Restart
                completed[i] = 1;
            } else {
              completed[i] = 0;
            }
        }
      }

      // Done with state changes, call the callbacks
      if (OnComplete != NULL) {
        for (int i = 0; i < numCups; i++) {
          if (completed[i]) {
            OnComplete(i);
          }
        }
      }
    }
    
    // Reverse pattern direction
    void Reverse()
    {
      for (int i = 0; i < numCups; i++) {
        if (Directions[i] == FORWARD)
        {
            Directions[i] = REVERSE;
            Indexes[i] = TotalSteps[i]-1;
        }
        else
        {
            Directions[i] = FORWARD;
            Indexes[i] = 0;
        }
      }
    }

    void Music(CRGB color1, CRGB color2) {
      // Sets ALL to music (no option for just one of them for now)
      for (int i = 0; i < numCups; ++i) {
        ActivePatterns[i] = MUSIC;
        Interval = 0;
        TotalSteps[i] = 1000;
        Indexes[i] = 0;
        Directions[i] = FORWARD;
      }
      Color1 = color1;
      Color2 = color2;
    }

    void MusicUpdate() {
      unsigned int x, y, freqBin;
      float level;
      freqBin = 0;
  
      for (x=0; x < music.width(); x++) {
        // get the volume for each horizontal pixel position
        level = music.readFFT(freqBin, x);
  
        // uncomment to see the spectrum in Arduino's Serial Monitor
        // Serial.print(level);
        // Serial.print("  ");
  
        for (y=0; y < music.height(); y++) {
          // for each vertical pixel, check if above the threshold
          // and turn the LED on or off
          if (music.aboveThreshold(level, y)) { //level >= thresholdVertical[y]) {
            leds.setPixelColor(music.xy(x, y), Color1);
          } else {
            leds.setPixelColor(music.xy(x, y), Color2);
          }
        }
        // increment the frequency bin count, so we display
        // low to higher frequency from left to right
        freqBin = freqBin + music.horizontalFrequencyBin(x); //frequencyBinsHorizontal[x];
      }
    }

    void RainbowCycleAll(uint8_t interval, direction dir = FORWARD) {
      for (int i = 0; i < numCups; i++) {
        RainbowCycle(i, interval, dir);
      }
    }
    
    // Initialize for a RainbowCycle
    void RainbowCycle(uint8_t i, uint8_t interval, direction dir = FORWARD)
    {
        ActivePatterns[i] = RAINBOW_CYCLE;
        Interval = interval;
        TotalSteps[i] = 255;
        Indexes[i] = 0;
        Directions[i] = dir;
    }
    
    // Update the Rainbow Cycle Pattern
    void RainbowCycleUpdate(uint8_t index)
    {
      for(int i=0; i< leds.numPixels() / numCups; i++)
      {
        leds.setPixelColor(numPixelsPerCup * index  + i, Wheel(((i * 256 / (leds.numPixels() / numCups)) + Indexes[index]) & 255));
      }
    }

    // Initialize for a Theater Chase
    void TheaterChase(uint8_t i, CRGB color1, CRGB color2, uint8_t interval, direction dir = FORWARD)
    {
        ActivePatterns[i] = THEATER_CHASE;
        Interval = interval;
        TotalSteps[i] = leds.numPixels();
        Color1 = color1;
        Color2 = color2;
        Indexes[i] = 0;
        Directions[i] = dir;
   }
    
    // Update the Theater Chase Pattern
    void TheaterChaseUpdate(uint8_t index)
    {
        for(int i=0; i< leds.numPixels() / numCups; i++)
        {
            if ((i + Indexes[index] ) % 3 == 0)
            {
                leds.setPixelColor(numPixelsPerCup * index  + i, Color1);
            }
            else
            {
                leds.setPixelColor(numPixelsPerCup * index  + i, Color2);
            }
        }
    }

    // Initialize for a ColorWipe
    void ColorWipe(uint8_t i, uint32_t color, uint8_t interval, direction dir = FORWARD)
    {
        ActivePatterns[i] = COLOR_WIPE;
        Interval = interval;
        TotalSteps[i] = leds.numPixels();
        Color1 = color;
        Indexes[i] = 0;
        Directions[i] = dir;
    }
    
    // Update the Color Wipe Pattern
    void ColorWipeUpdate(uint8_t i)
    {
        leds.setPixelColor(numPixelsPerCup * i + Indexes[i], Color1);
    }

    void ScannerAll(CRGB color1, uint8_t interval) {
      for (int i = 0; i < numCups; i++) {
        Scanner(i, color1, interval);
      }
    }
    
    // Initialize for a SCANNNER
    void Scanner(uint8_t i, CRGB color1, uint8_t interval)
    {
        ActivePatterns[i] = SCANNER;
        Interval = interval;
        TotalSteps[i] = (leds.numPixels() - 1) * 2;
        Color1 = color1;
        Indexes[i] = 0;
    }

    // Update the Scanner Pattern
    void ScannerUpdate(uint8_t index)
    { 
        for (int i = 0; i < leds.numPixels() / numCups; i++)
        {
            if (i == Indexes[index])  // Scan Pixel to the right
            {
                 leds.setPixelColor(numPixelsPerCup * index + i, Color1);
            }
            else if (i == TotalSteps[index] - Indexes[index]) // Scan Pixel to the left
            {
                 leds.setPixelColor(numPixelsPerCup * index + i, Color1);
            }
            else // Fading tail
            {
                 leds.setPixelColor(numPixelsPerCup * index + i, DimColor(leds.getPixelColor(numPixelsPerCup * index + i)));
            }
        }
    }

    void FadeAll(CRGB color1, CRGB color2, uint16_t steps, uint8_t interval, direction dir = FORWARD) {
      for (int i = 0; i < numCups; i++) {
        Fade(i, color1, color2, steps, interval, dir);
      }
    }
    
    // Initialize for a Fade
    void Fade(uint8_t i, CRGB color1, CRGB color2, uint16_t steps, uint8_t interval, direction dir = FORWARD)
    {
        ActivePatterns[i] = FADE;
        Interval = interval;
        TotalSteps[i] = steps;
        Color1 = color1;
        Color2 = color2;
        Indexes[i] = 0;
        Directions[i] = dir;
    }
    
    // Update the Fade Pattern
    void FadeUpdate(uint8_t i)
    {
        // Calculate linear interpolation between Color1 and Color2
        // Optimise order of operations to minimize truncation error
        uint8_t red = ((Color1.red * (TotalSteps[i] - Indexes[i])) + (Color2.red * Indexes[i])) / TotalSteps[i];
        uint8_t green = ((Color1.green * (TotalSteps[i] - Indexes[i])) + (Color2.green * Indexes[i])) / TotalSteps[i];
        uint8_t blue = ((Color1.blue * (TotalSteps[i] - Indexes[i])) + (Color2.blue * Indexes[i])) / TotalSteps[i];
        
        ColorSet(i, leds.Color(red, green, blue));
    }
   
    // Calculate 50% dimmed version of a color (used by ScannerUpdate)
    CRGB DimColor(CRGB color)
    {
        // Shift R, G and B components one bit to the right
        CRGB dimColor = leds.Color(color.red >> 1, color.green >> 1, color.blue >> 1);
        return dimColor;
    }

    // Set all pixels to a color (synchronously)
    void ColorSet(uint8_t index, CRGB color)
    {
        for (int i = 0; i < leds.numPixels() / numCups; i++)
        {
            leds.setPixelColor(numPixelsPerCup * index + i, color);
        }
    }

    uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
      return leds.Color(r, g, b);
    }
    
    // Input a value 0 to 255 to get a color value.
    // The colours are a transition r - g - b - back to r.
    CRGB Wheel(byte WheelPos)
    {
        WheelPos = 255 - WheelPos;
        if(WheelPos < 85)
        {
            return leds.Color(255 - WheelPos * 3, 0, WheelPos * 3);
        }
        else if(WheelPos < 170)
        {
            WheelPos -= 85;
            return leds.Color(0, WheelPos * 3, 255 - WheelPos * 3);
        }
        else
        {
            WheelPos -= 170;
            return leds.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
        }
    }
};
