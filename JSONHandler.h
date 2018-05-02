#ifndef JSONHandler_h
#define JSONHandler_h

#include "Arduino.h"
#include "ArduinoJson.h"
#include "ESP8266WebServer.h"

class JSONHandler
{
  public:
    JSONHandler(ESP8266WebServer *server);
    JsonObject& getFromRequest();
  private:
    ESP8266WebServer *_server;
};

#endif
