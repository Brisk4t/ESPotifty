
#include <FastLED.h>
#include <TJpg_Decoder.h>
#include <functional>
#include <FastLED_NeoMatrix.h>
#include <Adafruit_Protomatter.h>

// #define LED_PIN     5
// #define NUM_LEDS    256
// #define MATRIX_WIDTH 16
// #define MATRIX_HEIGHT 16


#define MATRIX_WIDTH  64             // Matrix width in pixels
#define MATRIX_HEIGHT 64             // Matrix height in pixel


bool drawImageCallback(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap);
extern Adafruit_Protomatter matrix;
//extern CRGB leds[NUM_LEDS];

class NeoPixelImage {
  public:
    NeoPixelImage(int scale, bool setSwapBytes = true);
    bool drawImage(String filename);
    void begin();

  private:
    int scale; // Scale factor for the image
    bool setSwapBytes; // Flag to swap bytes for RGB565 format


    int XY(int x, int y); // Convert 2D coordinates to 1D index for chain-indexed matrix
    const char* filename; // Filename of the image to be drawn

};
