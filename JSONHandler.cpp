#include "JSONHandler.h"

#define PLAIN "plain"

JSONHandler::JSONHandler(ESP8266WebServer *server)
{
  _server = server;
  
}

JsonObject& JSONHandler::getFromRequest() {
  DynamicJsonBuffer newBuffer;
  Serial.println("Request JSON: ");
  Serial.println(_server->arg(PLAIN));
  JsonObject& json = newBuffer.parseObject(_server->arg(PLAIN));
}
