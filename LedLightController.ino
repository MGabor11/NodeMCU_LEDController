#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include "LedController.h"
#include "constants.h"

//Settings constants part
// ID of the settings block
#define CONFIG_VERSION "ls1"

// Tell it where to store your config data in EEPROM
#define CONFIG_START 32

//LED constants part
// LED connected to digital pin D6
int dataPin = 12;

//Init LED LED handler
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(Constants::NUMPIXELS, dataPin);

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

LedController ledController = LedController(pixel, Constants::NUMPIXELS);

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
  server.on("/setNumberOfPixels", HTTP_POST, generatePixels);

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
    if (lastPulsing < (millis() - Constants::PULSING_TIME)) {
      stopPulse();
    } else {
      ledController.pulse();
    }
  }
}

void generatePixels(){
  
}

void setNotification() {
  DynamicJsonBuffer newBuffer;
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
  DynamicJsonBuffer newBuffer;
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

  ledController.setLightsToBrightness(brightness, red, green, blue);

  String output;
  json.printTo(output);
  sendResponse(output);
}

void turnOn() {
  isLampOn = true;
  isThreadRunning = false;
  isInfinitePulsing = false;
  ledController.setLightsToBrightness(Constants::DEFAULT_BRIGHTNESS, 255, 255, 255);
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
  startPulseOnThread(Constants::GMAIL_RED, Constants::GMAIL_GREEN, Constants::GMAIL_BLUE);

}

void turnOnSkype() {
  sendResponse("{\"notification_type\":\"skype\"}");
  startPulseOnThread(Constants::SKYPE_RED, Constants::SKYPE_GREEN, Constants::SKYPE_BLUE);
}

void turnOnDefault () {
  sendResponse("{\"notification_type\":\"default\"}");
  startPulseOnThread(Constants::DEFAULT_RED, Constants::DEFAULT_GREEN, Constants::DEFAULT_BLUE);
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


