#include <NeoPixelImage.h>


#define LED_PIN     5
#define NUM_LEDS    256
#define MATRIX_WIDTH 16
#define MATRIX_HEIGHT 16

static CRGB imageBuffer[MATRIX_HEIGHT][MATRIX_WIDTH]; // Buffer to hold the image data

bool drawImageCallback(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap){
    

    // uint16_t width;
    // uint16_t height;

    //TJpgDec.getFsJpgSize(&width, &height, filename); // Get the size of the image
    
    // Callback for TJpg_Decoder
    for (int16_t j = 0; j < h; j++) {
        for (int16_t i = 0; i < w; i++) {
          int src_x = x + i;
          int src_y = y + j;
    
          // Map source to 16x16 target
          int target_x = map(src_x, 0, w - 1, 0, MATRIX_WIDTH - 1);
          int target_y = map(src_y, 0, h - 1, 0, MATRIX_HEIGHT - 1);
    
          // Set pixel in buffer
          uint16_t color = bitmap[j * w + i];
          uint8_t r = ((color >> 11) & 0x1F) << 3;
          uint8_t g = ((color >> 5) & 0x3F) << 2;
          uint8_t b = (color & 0x1F) << 3;
    
          imageBuffer[target_y][target_x] = CRGB(r, g, b);
        }
      }

    return true; // Return true to continue drawing
}
    
NeoPixelImage::NeoPixelImage(int scale, bool setSwapBytes){
        TJpgDec.setJpgScale(scale);
        TJpgDec.setSwapBytes(setSwapBytes);
        TJpgDec.setCallback(drawImageCallback); // Set the callback function for image drawing


        FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
        FastLED.clear();
        // FastLED.show();
    }

int NeoPixelImage::XY(int x, int y) { // Convert 2D coordinates to 1D index for chain-indexed matrix 
    if (y % 2 == 0) {
        return y * MATRIX_WIDTH + x;
    } 
    
    else {
        return y * MATRIX_WIDTH + (MATRIX_WIDTH - 1 - x);
    }
    }
    

bool NeoPixelImage::drawImage(const char* path) {
        // Load the image from the file system and draw it

        filename = path; // Set the filename to be used in the callback function

        // Clear the image buffer before drawing
        for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
            for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
              imageBuffer[y][x] = CRGB::Black;
            }
        }

        // Draw buffer to the LED matrix  
        if (TJpgDec.drawFsJpg(0, 0, filename) == 0) {
            Serial.println("Image drawn successfully");
        } 
        
        else {
            Serial.println("Failed to draw image");
            return false;
        }

        // Transfer buffer to LED matrix
        for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
            for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
            leds[XY(x, y)] = imageBuffer[y][x];
            }
        }

        FastLED.show(); // Update the LED matrix with the new image
        return true;
}

