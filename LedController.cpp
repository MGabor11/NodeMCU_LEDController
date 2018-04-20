#include "Arduino.h"
#include "LedController.h"

#include "Adafruit_NeoPixel.h"

#define MAXIMUM_BRIGHTNESS 70
#define MINIMUM_BRIGHTNESS 10

LedController::LedController(Adafruit_NeoPixel pixel, int pixelCount)
{
  _pixel = pixel;
  _currentRed = 0;
  _currentGreen = 0;
  _currentBlue = 0;
  _pixelCount = pixelCount;
}

void LedController::pulse() {
  pulse(_currentRed, _currentGreen, _currentBlue);
}

void LedController::pulse(int red, int green, int blue) {
  _currentRed = red;
  _currentGreen = green;
  _currentBlue = blue;

  // fade in
  for (int x = MINIMUM_BRIGHTNESS; x < MAXIMUM_BRIGHTNESS; x++) {
    setLightsToBrightness(x, red, green, blue);
  }
  // fade out
  for (int x = MAXIMUM_BRIGHTNESS; x >= MINIMUM_BRIGHTNESS; x--) {
    setLightsToBrightness(x, red, green, blue);
  }
}

void LedController::setLightsToColor(int red, int green, int blue) {
  for (uint8_t i = 0; i < _pixelCount; i++) {
    _pixel.setPixelColor(i, _pixel.Color(red, green, blue));
  }
}

void LedController::setLightsToBrightness(int brightness, int red, int green, int blue) {
  _pixel.setBrightness(brightness);
  setLightsToColor(red, green, blue);
  _pixel.show();
  delay(30);
}
