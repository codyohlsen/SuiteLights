// @ SUITE HOUSE PRODUCTIONS
// Classes to handle parameters for different Suite house components for playing to music.
class MatrixMusicHandler;
class MusicHandler
{
  public:
  MusicHandler() { }
  MusicHandler(MatrixMusicHandler& other) {
  }
    virtual void begin() {INVALID;}
    virtual unsigned int  xy(unsigned int x, unsigned int y) { INVALID; return 0;}
    virtual float readFFT(unsigned int freqBin, unsigned int x) { INVALID; return 0.0;}
    virtual int width() { INVALID; return 0;}
    virtual int height() { INVALID; return 0;}
    virtual boolean aboveThreshold(float level, unsigned int y) { INVALID; return false;}
    virtual unsigned int horizontalFrequencyBin(unsigned int x) {INVALID; return 0;}
};

class MatrixMusicHandler : public MusicHandler
{
  public:
  AudioAnalyzeFFT1024& fft;
  MatrixMusicHandler(AudioAnalyzeFFT1024& fft_) : fft(fft_) { }
  
  //// This array holds the volume level (0 to 1.0) for each
    //// vertical pixel to turn on.  Computed in setup() using
    //// the 3 parameters above.
    float thresholdVertical[MATRIX_HEIGHT];

    //// This array specifies how many of the FFT frequency bin
    //// to use for each horizontal pixel.  Because humans hear
    //// in octaves and FFT bins are linear, the low frequencies
    //// use a small number of bins, higher frequencies use more.
    int frequencyBinsHorizontal[MATRIX_WIDTH] = {
       1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
       2,  2,  2,  2,  2,  2,  2,  2,  2,  3,
       3,  3,  3,  3,  4,  4,  4,  4,  4,  5,
       5,  5,  6,  6,  6,  7,  7,  7,  8,  8,
       9,  9, 10, 10, 11, 12, 12, 13, 14, 15,
      15, 16, 17, 18, 19, 20, 22, 23, 24, 25
    };

    //// These parameters adjust the vertical thresholds
    const float maxLevel = 0.1;      // 1.0 = max, lower is more "sensitive"
    const float dynamicRange = 20.0; // total range to display, in decibels
    const float linearBlend = 0.3;   // useful range is 0 to 0.

    void begin() override {
      computeVerticalLevels();
    }

    //     Run once from setup, the compute the vertical levels
    void computeVerticalLevels() {
      unsigned int y;
      float n, logLevel, linearLevel;
    
      for (y=0; y < MATRIX_HEIGHT; y++) {
        n = (float)y / (float)(MATRIX_HEIGHT - 1);
        logLevel = pow10f(n * -1.0 * (dynamicRange / 20.0));
        linearLevel = 1.0 - n;
        linearLevel = linearLevel * linearBlend;
        logLevel = logLevel * (1.0 - linearBlend);
        thresholdVertical[y] = (logLevel + linearLevel) * maxLevel;
      }
    }

    // A simple xy() function to turn display matrix coordinates
    // into the index numbers OctoWS2811 requires.  If your LEDs
    // are arranged differently, edit this code...
    unsigned int xy(unsigned int x, unsigned int y) override {
      return y * MATRIX_WIDTH + x;
    }

    float readFFT(unsigned int freqBin, unsigned int x) override {
      return fft.read(freqBin, freqBin + frequencyBinsHorizontal[x] - 1);
    }

    int width() override {
      return MATRIX_WIDTH;
    }

    int height() override {
      return MATRIX_HEIGHT;
    }

    boolean aboveThreshold(float level, unsigned int y) override {
      return level >= thresholdVertical[y];
    }

    unsigned int horizontalFrequencyBin(unsigned int x) override {
      return frequencyBinsHorizontal[x];
    }
};

const int total_leds = NUM_CUPS * NUM_PIXELS_PER_CUP;
const int ten_cups_width = 30;
const int ten_cups_height = total_leds / ten_cups_width;
class TenCupsMusicHandler : public MusicHandler
{
  public:
  AudioAnalyzeFFT1024& fft;
  TenCupsMusicHandler(AudioAnalyzeFFT1024& fft_) : fft(fft_) { }
  
  //// This array holds the volume level (0 to 1.0) for each
    //// vertical pixel to turn on.  Computed in setup() using
    //// the 3 parameters above.
    float thresholdVertical[ten_cups_height];

    //// This array specifies how many of the FFT frequency bin
    //// to use for each horizontal pixel.  Because humans hear
    //// in octaves and FFT bins are linear, the low frequencies
    //// use a small number of bins, higher frequencies use more.
    // We treat the 10 cups as a 30 x 8 matrix. for now, at least.
    int frequencyBinsHorizontal[ten_cups_width] = {
       1,  1,  1,  1,  1, 
       2,  2,  2,  3,  3, 
       3,  3,  4,  4,  4,
       5,  5,  5,  6,  6, 
       7,  8,  9, 10, 12,
      14, 17, 20, 23, 25
    };

    //// These parameters adjust the vertical thresholds
    const float maxLevel = 0.1;      // 1.0 = max, lower is more "sensitive"
    const float dynamicRange = 20.0; // total range to display, in decibels
    const float linearBlend = 0.3;   // useful range is 0 to 0.

    void begin() override {
      computeVerticalLevels();
    }

    //     Run once from setup, the compute the vertical levels
    void computeVerticalLevels() {
      unsigned int y;
      float n, logLevel, linearLevel;
    
      for (y=0; y < ten_cups_height; y++) {
        n = (float)y / (float)(ten_cups_height - 1);
        logLevel = pow10f(n * -1.0 * (dynamicRange / 20.0));
        linearLevel = 1.0 - n;
        linearLevel = linearLevel * linearBlend;
        logLevel = logLevel * (1.0 - linearBlend);
        thresholdVertical[y] = (logLevel + linearLevel) * maxLevel;
      }
    }

    // A simple xy() function to turn display matrix coordinates
    // into the index numbers OctoWS2811 requires.  If your LEDs
    // are arranged differently, edit this code...
    unsigned int xy(unsigned int x, unsigned int y) override {
      return y * ten_cups_width + x;
    }

    float readFFT(unsigned int freqBin, unsigned int x) override {
      return fft.read(freqBin, freqBin + frequencyBinsHorizontal[x] - 1);
    }

    int width() override {
      return ten_cups_width;
    }

    int height() override {
      return ten_cups_height;
    }

    boolean aboveThreshold(float level, unsigned int y) override {
      return level >= thresholdVertical[y];
    }

    unsigned int horizontalFrequencyBin(unsigned int x) override {
      return frequencyBinsHorizontal[x];
    }
};

const int strip_num_leds = SIDE_STRIP_LEN * NUM_SIDE_STRIPS;
const int strip_width = NUM_SIDE_STRIPS;
const int strip_height = strip_num_leds / strip_width;
class StripMusicHandler : public MusicHandler
{
  public:
  AudioAnalyzeFFT1024& fft;
  StripMusicHandler(AudioAnalyzeFFT1024& fft_) : fft(fft_) { }
  
  //// This array holds the volume level (0 to 1.0) for each
    //// vertical pixel to turn on.  Computed in setup() using
    //// the 3 parameters above.
    float thresholdVertical[strip_height];

    //// This array specifies how many of the FFT frequency bin
    //// to use for each horizontal pixel.  Because humans hear
    //// in octaves and FFT bins are linear, the low frequencies
    //// use a small number of bins, higher frequencies use more.
    // We treat the 10 cups as a 30 x 8 matrix. for now, at least.
    int frequencyBinsHorizontal[strip_width] = {
       10, 50
    };

    //// These parameters adjust the vertical thresholds
    const float maxLevel = 0.1;      // 1.0 = max, lower is more "sensitive"
    const float dynamicRange = 20.0; // total range to display, in decibels
    const float linearBlend = 0.3;   // useful range is 0 to 0.

    void begin() override {
      computeVerticalLevels();
    }

    //     Run once from setup, the compute the vertical levels
    void computeVerticalLevels() {
      unsigned int y;
      float n, logLevel, linearLevel;
      for (y=0; y < strip_height; y++) {
        n = (float)y / (float)(ten_cups_height - 1);
        logLevel = pow10f(n * -1.0 * (dynamicRange / 20.0));
        linearLevel = 1.0 - n;
        linearLevel = linearLevel * linearBlend;
        logLevel = logLevel * (1.0 - linearBlend);
        thresholdVertical[y] = (logLevel + linearLevel) * maxLevel;
      }
    }

    // A simple xy() function to turn display matrix coordinates
    // into the index numbers OctoWS2811 requires.  If your LEDs
    // are arranged differently, edit this code...
    unsigned int xy(unsigned int x, unsigned int y) override {
      return y * strip_width + x;
    }

    float readFFT(unsigned int freqBin, unsigned int x) override {
      return fft.read(freqBin, freqBin + frequencyBinsHorizontal[x] - 1);
    }

    int width() override {
      return strip_width;
    }

    int height() override {
      return strip_height;
    }

    boolean aboveThreshold(float level, unsigned int y) override {
      return level >= thresholdVertical[y];
    }

    unsigned int horizontalFrequencyBin(unsigned int x) override {
      return frequencyBinsHorizontal[x];
    }
};
