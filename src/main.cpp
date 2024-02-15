#include "ArduinoJson.h"
#include <WiFi.h>
#include <PubSubClient.h>

#define LED_PIN 18

const char *ssid = "guest";
const char *password = "Guest42guest";

const char *mqttServer = "188.166.191.227";
const int mqttPort = 1883;
const char *mqttUser = "HW_MQTT_01";
const char *mqttPassword = "hw_mqtt_beonit";

WiFiClient espClient;
PubSubClient client(espClient);

void setup(void)
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Connected to the WiFi network");

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  pinMode(LED_PIN, OUTPUT);
}

void loop(void)
{
  if (!client.connected())
  {
      Serial.println("Connecting to MQTT...");
  
      if (client.connect("new client", mqttUser, mqttPassword ))
      {
        Serial.println("Connected");
      }
      else
      {
        Serial.print("Failed with state ");
        Serial.println(client.state());
        delay(2000);
      }
  }

  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();

  JSONencoder["status"] = analogRead(LED_PIN) > 0 ? "ON" : "OFF";
  JSONencoder["brightness"] = (int)(analogRead(LED_PIN) / 25.5);
  // JSONencoder["duration"] = "DO SOMETHING" (if timestamp function --> check if minutes of hour is 0, 15, 45)
  // JSONencoder["Esp32Time"] = "SEND TIMESTAMP";
  // JsonArray values = JSONencoder.createNestedArray("values");
  
  // values.add(20);
  // values.add(21);
  // values.add(23);

  char JSONmessageBuffer[100];
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));

  Serial.println(JSONmessageBuffer);
 
  if (client.publish("HW_mqtt/testing/01", JSONmessageBuffer) == true) {
      Serial.println("Success sending message");
  } else {
      Serial.println("Error sending message");
  }
  
  client.loop();
}
