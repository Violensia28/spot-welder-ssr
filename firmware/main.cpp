#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

const char* ssid = "SpotWelder";
const char* password = "12345678";

#define SSR_PIN 4
#define MICROSWITCH_PIN 2
#define BUZZER_PIN 5
#define STATUS_LED 13

int weldTime = 100;
int pulseCount = 1;

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  
  pinMode(SSR_PIN, OUTPUT);
  pinMode(MICROSWITCH_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  
  SPIFFS.begin(true);
  
  WiFi.softAP(ssid, password);
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });
  
  server.on("/weld", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(SSR_PIN, HIGH);
    delay(weldTime);
    digitalWrite(SSR_PIN, LOW);
    request->send(200, "text/plain", "Welding Done");
  });
  
  server.begin();
}

void loop() {
  digitalWrite(STATUS_LED, HIGH);
  delay(500);
  digitalWrite(STATUS_LED, LOW);
  delay(500);
}
