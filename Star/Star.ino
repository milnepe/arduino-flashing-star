
/*
  A multi-mode, non-blocking Neo-pixel flashing star
  
  Copywrite 2021 Peter Milne
  License: GPL-3.0 License
*/

// A multi-mode, non-blocking Neo-pixel flashing star

#include <Adafruit_CircuitPlayground.h>

#define LED_COUNT 10
#define SOUND_SENSITIVITY 70  // Sound threashold

const uint16_t pixelNumber = LED_COUNT;  // Total Number of Pixels
int state = 1;
int pulse_state = 1;
int party_state = 1;
unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long currentMillis = 0;
unsigned long previousPulseMillis = 0;

void setup() {
  CircuitPlayground.begin(25);  // 255 means set pixel colors to max brightness.
  CircuitPlayground.clearPixels();
  CircuitPlayground.setBrightness(50);
}

void loop() {
  currentMillis = millis(); //  Update current time

  if (CircuitPlayground.slideSwitch()) {  // On

    if (CircuitPlayground.leftButton()) {
      state = 1; // Pulse mode
      delay(100);
    }

    if (CircuitPlayground.rightButton()) {
      state = 2;  // Party mode
      delay(100);
    }

    // If a sound pulse is detected, switch pixel mode
    float level = CircuitPlayground.mic.soundPressureLevel(10);
    if (level > SOUND_SENSITIVITY) {
      if (state == 1) {
        pulse();
      }
      if (state == 2) {
        party();
      }
    }

    switch (state) {
      case 1: // Pulse mode
        switch (pulse_state) {
          case 1:
            myPulse(25, 10);
            break;
          case 2: // Chase for a bit then return to pulse mode
            if ((currentMillis - previousPulseMillis) >= 1000) {
              pulse_state = 1;
              previousPulseMillis = currentMillis;
            }
            myTheaterChase(127, 127, 127, 100); // White, half brightness
        }
      case 2: // Party mode
        switch (party_state) {
          case 1:
            myTheaterChase(127, 127, 127, 100);
            break;
          case 2:
            myTheaterChaseRainbow(100);
            break;
          case 3:
            myRainbow(100);
        }
    }
  } else {  // Turn off
    CircuitPlayground.clearPixels();
  }
  delay(1);
}

void pulse() {
  pulse_state = 2;
}

void party() {
  if (party_state == 1)
    party_state = 2;
  else if (party_state == 2)
    party_state = 3;
  else if (party_state == 3)
    party_state = 1;
}

// Theater-marquee-style chasing lights without using delay.
// Pass in 32-bit rgb colour value, and interval in ms
// between frames
void myTheaterChase(uint8_t r, uint8_t g, uint8_t b, int interval) {
  // Set a starting pixel between 0 and 2
  static int startPixel;

  // Update pixels if time has expired
  if ((currentMillis - previousMillis) >= interval) {
    previousMillis = currentMillis;

    // Clear all pixels
    CircuitPlayground.clearPixels();

    // Turn on every 3rd pixel by increamenting in steps of 3
    for (int pixelCurrent = startPixel; pixelCurrent < pixelNumber; pixelCurrent += 3) {
      CircuitPlayground.setPixelColor(pixelCurrent, r, g, b); // Set current pixel colour
    }

    // Change starting pixel for next time arround
    startPixel++;
    if (startPixel > 2) {
      startPixel = 0;
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void myRainbow(uint8_t interval) {
  // Pattern Pixel Cycle
  static uint8_t pixelCycle;

  if ((currentMillis - previousMillis) >= interval) {
    previousMillis = currentMillis;

    for (uint16_t pixelCurrent = 0; pixelCurrent < pixelNumber; pixelCurrent++) {
      // Update colour
      CircuitPlayground.setPixelColor(pixelCurrent, CircuitPlayground.colorWheel((pixelCurrent + pixelCycle) % 255));
    }
    //    strip.show();  // Update strip with new contents
    pixelCycle++;  // Advance current cycle
    if (pixelCycle >= 256)
      pixelCycle = 0;  // Loop back begining
  }
}

// Pulse NeoPixel brightness. Pass max brightnes value up to 255
// and delay time (in ms) between frames.
void myPulse(const uint8_t max_brightness, uint8_t interval) {
  if ((currentMillis - previousMillis) >= interval) {
    previousMillis = currentMillis;

    static int bright_count = 0;
    // Convert mod bright_count degrees to radians
    float brightness = ((bright_count += 2) % 180) * PI / 180;
    brightness = sin(brightness) * max_brightness;
    if (brightness > 255) {
      brightness = 0;
    }
    CircuitPlayground.setBrightness((uint16_t)brightness);

    for (uint16_t pixelCurrent = 0; pixelCurrent < pixelNumber; pixelCurrent++) {
      CircuitPlayground.setPixelColor(pixelCurrent, 127, 127, 127);
    }
  }
}

// Multi-colour Theater-marquee-style chasing lights without using delay.
// Pass in interval in ms between frames
void myTheaterChaseRainbow(uint8_t interval) {
  // Set a starting pixel between 0 and 2
  static int startPixel;
  static uint8_t pixelCycle;

  // Update pixels if time has expired
  if ((currentMillis - previousMillis) >= interval) {
    previousMillis = currentMillis;

    // Clear all pixels
    CircuitPlayground.clearPixels();

    // Turn on every 3rd pixel by increamenting in steps of 3
    for (int pixelCurrent = startPixel; pixelCurrent < pixelNumber; pixelCurrent += 3) {
      // Update colour
      CircuitPlayground.setPixelColor(pixelCurrent, CircuitPlayground.colorWheel((pixelCurrent + pixelCycle) % 255));
    }

    // Change starting pixel for next time arround
    startPixel++;
    pixelCycle++;
    if (startPixel > 2) {
      startPixel = 0;
    }
    if (pixelCycle >= 256) {
      pixelCycle = 0;
    }
  }
}
