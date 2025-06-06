#include <NeoPixelImage.h>

// CRGB leds[NUM_LEDS];
// FastLED_NeoMatrix matrix(
//     leds, 
//     MATRIX_WIDTH, 
//     MATRIX_HEIGHT, 
//     // flags
//     NEO_MATRIX_TOP + 
//     NEO_MATRIX_LEFT +
//     NEO_MATRIX_ROWS + 
//     NEO_MATRIX_ZIGZAG);


uint8_t rgbPins[]  = {4, 12, 13, 14, 15, 21};
uint8_t addrPins[] = {18, 19, 25, 26};
uint8_t clockPin   = 27; // Must be on same port as rgbPins
uint8_t latchPin   = 32;
uint8_t oePin      = 33;

Adafruit_Protomatter matrix(
  64,          // Width of matrix (or matrix chain) in pixels
  4,           // Bit depth, 1-6
  1, rgbPins,  // # of matrix chains, array of 6 RGB pins for each
  4, addrPins, // # of address pins (height is inferred), array of pins
  clockPin, latchPin, oePin, // Other matrix control pins
  false);      // No double-buffering here (see "doublebuffer" example)


// Function to downsample the bitmap to fit a 16x16 matrix
void downsampleBitmap(uint16_t *inputBitmap, uint16_t inputWidth, uint16_t inputHeight, uint16_t *outputBitmap, uint16_t outputWidth, uint16_t outputHeight) {
    for (uint16_t y = 0; y < outputHeight; y++) {
        for (uint16_t x = 0; x < outputWidth; x++) {
            // Calculate the corresponding coordinates in the original bitmap
            uint16_t origX = x * inputWidth / outputWidth;
            uint16_t origY = y * inputHeight / outputHeight;

            // Get the color of the pixel from the original bitmap
            uint16_t color = inputBitmap[origY * inputWidth + origX];

            // Assign this color to the downsampled bitmap
            outputBitmap[y * outputWidth + x] = color;
        }
    }
}


bool display_rgbBitmap(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) { 
    //matrix.clear(); // Clear the matrix before drawing
    matrix.fillScreen(0);
    uint16_t downsampledBitmap[16 * 16];  // Create a downsampled bitmap (16x16)
    downsampleBitmap(bitmap, w, h, downsampledBitmap, 16, 16);  // Downsample to 16x16

    matrix.drawRGBBitmap(x, y, downsampledBitmap, 16, 16);
    matrix.show();
    return true;
}


NeoPixelImage::NeoPixelImage(int scale, bool setSwapBytes){
    this->scale = scale; // Set the scale factor for the image
    this->setSwapBytes = setSwapBytes; // Set the swap bytes flag
}

void NeoPixelImage::begin(){ // Begin function to make sure matrix is initialized only after global variables are initialized

        Serial.println("Initializing Tjpg_decoder....."); // Debugging line
        TJpgDec.setJpgScale(scale);
        TJpgDec.setSwapBytes(setSwapBytes);
        TJpgDec.setCallback(display_rgbBitmap); // Set the callback function for image drawing

        Serial.println("Initializing FastLed....."); // Debugging line
        //FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);

        // Initialize the NeoMatrix
        matrix.begin();
        //matrix.setBrightness(5); // Set brightness to 50%
        //uint16_t color = matrix.Color(255, 0, 0);
        matrix.fillScreen(0); // Clear the matrix
        matrix.show();
}

int NeoPixelImage::XY(int x, int y) { // Convert 2D coordinates to 1D index for chain-indexed matrix 
    if (y % 2 == 0) {
        return y * MATRIX_WIDTH + x;
    } 
    
    else {
        return y * MATRIX_WIDTH + (MATRIX_WIDTH - 1 - x);
    }
}
    

bool NeoPixelImage::drawImage(String path) {
        // Load the image from the file system and draw it

        String filename = path; // Set the filename to be used in the callback function
        
        if(!SPIFFS.exists("/current_cover.jpg")){ // Check if the file exists
            Serial.println("File does not exist: " + filename);
        } 
        
        else {
            Serial.println("Drawing buffer.... " + String(filename)); // Debugging line
            // Draw buffer to the LED matrix  
            if (TJpgDec.drawFsJpg(0, 0, "/current_cover.jpg")) {
                Serial.println("Image drawn successfully");
                return true;
            }
            
            else {
                Serial.println("Failed to draw image");
                return false;
            }
        
        } 
        
        return false; // Return false if the image could not be drawn
}