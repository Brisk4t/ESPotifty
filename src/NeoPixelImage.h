
#include <FastLED.h>
#include <TJpg_Decoder.h>
#include <functional>

bool drawImageCallback(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap);

class NeoPixelImage {
  public:
    NeoPixelImage(int scale, bool setSwapBytes = true);
    bool drawImage(const char* filename);
    
  private:
    CRGB leds[256];
    int XY(int x, int y); // Convert 2D coordinates to 1D index for chain-indexed matrix
    const char* filename; // Filename of the image to be drawn

};
