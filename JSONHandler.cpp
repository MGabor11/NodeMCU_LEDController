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

  if (!json.success()) {
    Serial.println("parseObject() failed");
  }
  return json;
}

StandardRequest JSONHandler::getStandardRequest() {
  JsonObject& json = getFromRequest();
  int red = json["red"];
  int green = json["green"];
  int blue = json["blue"];
  int brightness = json["brightness"];
  int notificationType = json["notification_type"];

  return createStandardRequest( notificationType,
                                 brightness,
                                 red,
                                 green,
                                 blue);
}

String JSONHandler::getResponse(StandardRequest request) {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();

  json["red"] = request.red;
  json["green"] = request.green;
  json["blue"] = request.blue;
  json["brightness"] = request.brightness;
  json["notification_type"] = request.notificationType;

  String result;
  json.printTo(result);
  return result;
}

StandardRequest JSONHandler::createStandardRequest(int notificationType,
    int brightness,
    int red,
    int green,
    int blue) {
  StandardRequest request;
  request.notificationType = notificationType;
  request.brightness = brightness;
  request.red = red;
  request.green = green;
  request.blue = blue;

  return request;
};

