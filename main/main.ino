#include <FS.h>    
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>          
#include <DNSServer.h>
#include <WiFiManager.h>

HTTPClient http;
WiFiManager wifiManager;

#define SWITCH_BOARD D1
#define SWITCH_CPU D2

bool POWER_ON = false;
bool CHANGE_OCCURED = false;
String STATE_PREVIOUS = "0";
String URL_ = "Your API here";
void wifi() {
  wifiManager.setBreakAfterConfig(true);
  if (!wifiManager.autoConnect("PC_CONTROLLER", "pccontroller")) {
    Serial.println("failed to connect, we should reset as see if it connects");
    delay(3000);
    ESP.reset();
    delay(5000);
  }
  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  Serial.println("local ip");
  Serial.println(WiFi.localIP());
}

void wifireset() {
  wifiManager.resetSettings();
  delay(500);
  ESP.reset();
}

void observer() {
   http.begin(URL_);
   delay(200);
   
   int httpCode = http.GET();
   String payload;
   Serial.println(httpCode);
   payload = http.getString();
   delay(100);
   http.end();

   if(payload != STATE_PREVIOUS)
   {
      if(payload == "1")
      {
        POWER_ON = true;
        CHANGE_OCCURED = true;
        controller();
        STATE_PREVIOUS = payload;
      }
      else if(payload == "0")
      {
        POWER_ON = false;
        CHANGE_OCCURED = true;
        controller();
        STATE_PREVIOUS = payload;
      }
   }
}

void controller() {
  if(CHANGE_OCCURED)
  {
      if(POWER_ON)
      {
        digitalWrite(SWITCH_BOARD, LOW);
        delay(3000);
        digitalWrite(SWITCH_CPU, LOW);
        delay(2000);
        digitalWrite(SWITCH_CPU, HIGH);
      }
      else
      {
        delay(60000);
        digitalWrite(SWITCH_BOARD, HIGH);
      }
      CHANGE_OCCURED = false;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(SWITCH_BOARD, OUTPUT);
  pinMode(SWITCH_CPU, OUTPUT);
  digitalWrite(SWITCH_BOARD, HIGH);
  digitalWrite(SWITCH_CPU, HIGH);
  wifi();
}

void loop() {
  // put your main code here, to run repeatedly:
  observer();
}
