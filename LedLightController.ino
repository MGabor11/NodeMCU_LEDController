#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include "LedController.h"

//Settings constants part
// ID of the settings block
#define CONFIG_VERSION "ls1"

// Tell it where to store your config data in EEPROM
#define CONFIG_START 32

//LED constants part
// LED connected to digital pin D6
int dataPin = 12;
#define NUMPIXELS          13

#define DEFAULT_BRIGHTNESS 50
#define PULSING_TIME 10000

//GMAIL notification
#define GMAIL_TYPE 0
#define GMAIL_RED 255
#define GMAIL_GREEN 0
#define GMAIL_BLUE 0

//SKYPE notification
#define SKYPE_TYPE 1
#define SKYPE_RED 26
#define SKYPE_GREEN 248
#define SKYPE_BLUE 255

//DEFAULT notification
#define DEFAULT_TYPE 2
#define DEFAULT_RED 255
#define DEFAULT_GREEN 137
#define DEFAULT_BLUE 0

//Init LED LED handler
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(NUMPIXELS, dataPin);

//Threading variables
boolean isThreadRunning = false;
boolean isInfinitePulsing = false;
//TODO int currentRed, currentGreen, currentBlue;

//Last pulsing time
unsigned long lastPulsing = 0;
//TODO if lastPulsing > 100000 and isThreadRunning false reset to 0

//Init server on port 80
ESP8266WebServer server(80);

//Is lamp on
boolean isLampOn = false;

LedController ledController(pixel, NUMPIXELS);

void setup() {
  Serial.begin(115200);
  Serial.println("start");

  WiFi.begin("<SSID>", "<PASSWORD>"); //Connect to the WiFi network

  while (WiFi.status() != WL_CONNECTED) { //Wait for connection
    delay(500);
    Serial.println("Waiting to connect…");
  }

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); //Print the local IP

  Serial.println();
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress()); //Print MAC address

  server.on("/on", HTTP_GET, turnOn);
  server.on("/off", HTTP_GET, turnOff);
  server.on("/gmail", HTTP_GET, turnOnGmail);
  server.on("/skype", HTTP_GET, turnOnSkype);
  server.on("/default", HTTP_GET, turnOnDefault);
  server.on("/notification", HTTP_POST , setNotification);
  server.on("/setledmanually", HTTP_POST, handleManually); //TODO save setting
  server.on("/setMinMaxBrightness", HTTP_POST, handleManually); //TODO save setting

  server.begin(); //Start the server
  Serial.println();
  Serial.println("Server listening");

  pixel.begin();
  turnOff();
}

void loop() {
  server.handleClient();
  //Serial.println(millis());
  if (isInfinitePulsing) {
    ledController.pulse();
  } else if (isThreadRunning) {
    if (lastPulsing < (millis() - PULSING_TIME)) {
      stopPulse();
    } else {
      ledController.pulse();
    }
  }
}

void setNotification() {
  StaticJsonBuffer<500> newBuffer;
  Serial.println("Request JSON: ");
  Serial.println(server.arg("plain"));
  JsonObject& json = newBuffer.parseObject(server.arg("plain"));

  int notificationType = json["notification_type"];
  int red = json["red"];
  int green = json["green"];
  int blue = json["blue"];

  Serial.println("NOTIFICATION_TYPE: " + String(notificationType) + ", RED: " + String(red) + ",GREEN:" + String(green) + "BLUE: " + String(blue));
  //TODO isInfinitePulsing = true; only for testing
  isInfinitePulsing = true;
  isThreadRunning = false;

  ledController.pulse(red, green, blue);

  String output;
  json.printTo(output);
  sendResponse(output);
}

void handleManually() {
  StaticJsonBuffer<500> newBuffer;
  Serial.println("Request JSON: ");
  Serial.println(server.arg("plain"));
  JsonObject& json = newBuffer.parseObject(server.arg("plain"));


  int brightness = json["brightness"];
  int red = json["red"];
  int green = json["green"];
  int blue = json["blue"];

  Serial.println("BRIGHTNESS: " + String(brightness) + ",RED: " + String(red) + ",GREEN:" + String(green) + "BLUE: " + String(blue));

  isThreadRunning = false;
  isInfinitePulsing = false;

  //setLightsToBrightness(brightness, red, green, blue);
  ledController.setLightsToBrightness(brightness, red, green, blue);

  String output;
  json.printTo(output);
  sendResponse(output);
}

void turnOn() {
  isLampOn = true;
  isThreadRunning = false;
  isInfinitePulsing = false;
  ledController.setLightsToBrightness(DEFAULT_BRIGHTNESS, 255, 255, 255);
  sendResponse("{\"led\": \"on\"}");
}

void turnOff() {
  isLampOn = false;
  isThreadRunning = false;
  isInfinitePulsing = false;
  //setLightsToBrightness(0, 0, 0, 0);
  ledController.setLightsToBrightness(0, 0, 0, 0);
  sendResponse("{\"led\":\"off\"}");
}

void turnOnGmail() {
  sendResponse("{\"notification_type\": \"gmail\" }");
  startPulseOnThread(GMAIL_RED, GMAIL_GREEN, GMAIL_BLUE);

}

void turnOnSkype() {
  sendResponse("{\"notification_type\":\"skype\"}");
  startPulseOnThread(SKYPE_RED, SKYPE_GREEN, SKYPE_BLUE);
}

void turnOnDefault () {
  sendResponse("{\"notification_type\":\"default\"}");
  startPulseOnThread(DEFAULT_RED, DEFAULT_GREEN, DEFAULT_BLUE);
}



void sendResponse(String json) {
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

void startPulseOnThread(int red, int green, int blue) {
  isThreadRunning = false;
  isInfinitePulsing = false;
  lastPulsing = millis();
  ledController.pulse(red, green, blue);
  isThreadRunning = true;
}


void stopPulse() {
  Serial.println("Stop pulsing");
  isLampOn = false;
  isInfinitePulsing = false;
  isThreadRunning = false;
  ledController.setLightsToBrightness(0, 0, 0, 0);
}




/*

   JsonObject& prepareResponse(JsonBuffer& jsonBuffer) {
  JsonObject& root = jsonBuffer.createObject();
  JsonArray& tempValues = root.createNestedArray("temperature");
    tempValues.add(pfTemp);
  JsonArray& humiValues = root.createNestedArray("humidity");
    humiValues.add(pfHum);
  JsonArray& dewpValues = root.createNestedArray("dewpoint");
    dewpValues.add(pfDew);
  JsonArray& EsPvValues = root.createNestedArray("Systemv");
    EsPvValues.add(pfVcc/1000, 3);
  return root;
  }


   void writeResponse(WiFiClient& client, JsonObject& json) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close");
  client.println();

  json.prettyPrintTo(client);
  }

   #include <ArduinoJson.h>
  #include <ESP8266WiFi.h>
  JsonObject& json = prepareResponse(jsonBuffer);
   StaticJsonBuffer<500> jsonBuffer;
      JsonObject& json = prepareResponse(jsonBuffer);
      writeResponse(client, json);
      delay(1);
    client.stop();*/



