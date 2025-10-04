#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

// WiFi Credentials
const char* ssid = "SpotWelder";
const char* password = "12345678";

// Pin Definitions
#define SSR_PIN 4
#define MICROSWITCH_PIN 2
#define BUZZER_PIN 5
#define ACS712_PIN 34
#define STATUS_LED 13

// Welding Parameters
int weldTime = 100;
int pulseCount = 1;
int pulseInterval = 50;
String currentMode = "manual";

AsyncWebServer server(80);

void setupWebServer();
void handleMicroswitch();
void startWelding();
void beep(int times);

void setup() {
  Serial.begin(115200);
  
  // Initialize pins
  pinMode(SSR_PIN, OUTPUT);
  pinMode(MICROSWITCH_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(SSR_PIN, LOW);
  
  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  
  // Setup WiFi Access Point
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  setupWebServer();
  
  // Attach interrupt for microswitch
  attachInterrupt(digitalPinToInterrupt(MICROSWITCH_PIN), handleMicroswitch, FALLING);
  
  beep(1); // Startup beep
}

void loop() {
  digitalWrite(STATUS_LED, HIGH);
  delay(500);
  digitalWrite(STATUS_LED, LOW);
  delay(500);
}

void setupWebServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });
  
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });
  
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script.js", "text/javascript");
  });
  
  server.on("/weld", HTTP_GET, [](AsyncWebServerRequest *request){
    startWelding();
    request->send(200, "text/plain", "Welding Started");
  });
  
  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request){
    if(request->hasParam("time")) weldTime = request->getParam("time")->value().toInt();
    if(request->hasParam("pulses")) pulseCount = request->getParam("pulses")->value().toInt();
    if(request->hasParam("mode")) currentMode = request->getParam("mode")->value();
    request->send(200, "text/plain", "Settings Updated");
  });
  
  server.begin();
}

void IRAM_ATTR handleMicroswitch() {
  startWelding();
}

void startWelding() {
  beep(2);
  
  for(int i = 0; i < pulseCount; i++) {
    digitalWrite(SSR_PIN, HIGH);
    delay(weldTime);
    digitalWrite(SSR_PIN, LOW);
    if(i < pulseCount - 1) delay(pulseInterval);
  }
  
  beep(1);
}

void beep(int times) {
  for(int i = 0; i < times; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    delay(100);
  }
}
