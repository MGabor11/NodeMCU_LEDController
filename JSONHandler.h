#ifndef JSONHandler_h
#define JSONHandler_h

#include "Arduino.h"
#include <ArduinoJson.h>
#include "ESP8266WebServer.h"

struct StandardRequest {
  int notificationType;
  int brightness;
  int red;
  int green;
  int blue;
};

class JSONHandler
{
  public:
    JSONHandler(ESP8266WebServer *server);
    JsonObject& getFromRequest();
    StandardRequest getStandardRequest();
    String getResponse(StandardRequest request);
  private:
    ESP8266WebServer *_server;
    StandardRequest createStandardRequest(int notificationType,
                                            int brightness,
                                            int red,
                                            int green,
                                            int blue);
};

#endif
