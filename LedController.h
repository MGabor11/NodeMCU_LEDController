#ifndef LedController_h
#define LedController_h

#include "Arduino.h"
#include "Adafruit_NeoPixel.h"

class LedController
{
  public:
    LedController(Adafruit_NeoPixel pixel, int pixelCount);
    void pulse();
    void pulse(int red, int green, int blue);
    void setLightsToBrightness(int red, int green, int blue);
    void setLightsToBrightness(int brightness, int red, int green, int blue);
  private:
    int _currentRed, _currentGreen, _currentBlue;
    Adafruit_NeoPixel _pixel;
    int _pixelCount;
    void setLightsToColor(int red, int green, int blue);
};

#endif
