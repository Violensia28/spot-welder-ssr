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
  delay(1000);
  
  Serial.println("=== SPOT WELDER SSR STARTING ===");
  
  // Initialize pins
  pinMode(SSR_PIN, OUTPUT);
  pinMode(MICROSWITCH_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(SSR_PIN, LOW);
  
  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("SPIFFS Mount Failed");
  } else {
    Serial.println("SPIFFS Mount Success");
  }
  
  // WiFi Configuration
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));
  
  bool apStatus = WiFi.softAP(ssid, password, 6, 0, 4);
  
  if(!apStatus) {
    Serial.println("WiFi AP Failed to Start");
  } else {
    Serial.println("WiFi AP Started");
  }
  
  delay(2000);
  
  Serial.print("AP SSID: ");
  Serial.println(ssid);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
  
  setupWebServer();
  
  attachInterrupt(digitalPinToInterrupt(MICROSWITCH_PIN), handleMicroswitch, FALLING);
  
  beep(2);
  Serial.println("Spot Welder Ready!");
}

void loop() {
  static unsigned long lastStatusCheck = 0;
  
  if(millis() - lastStatusCheck > 10000) {
    lastStatusCheck = millis();
    int stations = WiFi.softAPgetStationNum();
    Serial.print("Connected Stations: ");
    Serial.println(stations);
  }
  
  int blinkDelay = (WiFi.softAPgetStationNum() > 0) ? 200 : 500;
  
  digitalWrite(STATUS_LED, HIGH);
  delay(blinkDelay);
  digitalWrite(STATUS_LED, LOW);
  delay(blinkDelay);
}

void setupWebServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if(SPIFFS.exists("/index.html")) {
      request->send(SPIFFS, "/index.html", "text/html");
    } else {
      request->send(200, "text/html", "<h1>Spot Welder Ready</h1>");
    }
  });
  
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    if(SPIFFS.exists("/style.css")) {
      request->send(SPIFFS, "/style.css", "text/css");
    }
  });
  
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    if(SPIFFS.exists("/script.js")) {
      request->send(SPIFFS, "/script.js", "text/javascript");
    }
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
