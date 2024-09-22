#ifndef LED_EFFECTS_H
#define LED_EFFECTS_H

#include <FastLED.h>

class LedEffects {
  private:
    CRGB* leds;          // Pointer to the LED array
    int numLeds;         // Number of LEDs
    int brightness;      // Current brightness level for effects
    unsigned long effectStartTime;  // When an effect (like rainbow) started
    bool rainbowInitialized;        // To ensure the rainbow starts correctly
    uint8_t hue;         // For rainbow effect: tracks hue value

  public:
    // Constructor to initialize the LED array and number of LEDs
    LedEffects(CRGB* leds, int numLeds) {
      this->leds = leds;
      this->numLeds = numLeds;
      this->brightness = 255;        // Default brightness set to full
      this->effectStartTime = 0;
      this->rainbowInitialized = false;
      this->hue = 0;                 // Start hue at 0 for rainbow effect
    }

    // Pulse effect: smoothly increase/decrease brightness of a given color, with a specified pulse duration
    void pulse(CRGB color, unsigned long pulseDuration) {
    unsigned long currentTime = millis();
    
    // If this is the first call, initialize the pulse start time
    if (!rainbowInitialized) {
        effectStartTime = currentTime;
        rainbowInitialized = true;
    }

    // Calculate elapsed time since the pulse started
    unsigned long elapsedTime = currentTime - effectStartTime;
    unsigned long halfPulseDuration = pulseDuration >> 1;  // Faster division by 2 using bit shift

    // Reset the pulse if it completed the full duration
    if (elapsedTime >= pulseDuration) {
        effectStartTime = currentTime;  // Restart the pulse
        elapsedTime = 0;
    }

    // Instead of using map(), we'll directly calculate brightness linearly
    int brightness;
    if (elapsedTime <= halfPulseDuration) {
        // Fade-in: brightness increases linearly from 0 to 255
        brightness = (elapsedTime * 255) / halfPulseDuration;
    } else {
        // Fade-out: brightness decreases linearly from 255 to 0
        brightness = ((pulseDuration - elapsedTime) * 255) / halfPulseDuration;
    }

    // Set brightness for each LED
    for (int i = 0; i < numLeds; i++) {
        leds[i] = color;
        leds[i].nscale8(brightness);  // Scale the color based on calculated brightness
    }    
}


    // Rainbow effect: cycle through rainbow colors at a given speed
    void rainbow(unsigned long speedDelay) {
      unsigned long currentTime = millis();

      // Initialize rainbow effect start time only once
      if (!rainbowInitialized) {
        effectStartTime = currentTime;
        rainbowInitialized = true;
      }

      // Check if it's time to update the rainbow
      if (currentTime - effectStartTime >= speedDelay) {
        effectStartTime = currentTime;  // Update the start time

        // Cycle through hues for a rainbow effect
        for (int i = 0; i < numLeds; i++) {
          leds[i] = CHSV(hue + (i * 256 / numLeds), 255, brightness);  // Hue shifts across LEDs
        }

        // Increment hue for smooth transition
        hue++;
        if (hue == 255) {
          hue = 0;  // Loop back to the start of the rainbow
        }
      }
    }

    // Solid color effect
    void solidColor(CRGB color) {
      for (int i = 0; i < numLeds; i++) {
        leds[i] = color;
      }
    }
};

#endif
