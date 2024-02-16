#include "ArduinoJson.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define LED_PIN 32 // set LED pin

const char *ssid = "Nunnaphut's"; // your wifi ssid
const char *password = "12345678"; // your wifi password

// MQTT Server spec
const char *mqttServer = "188.166.191.227";
const int mqttPort = 1883;
const char *mqttUser = "HW_MQTT_01";
const char *mqttPassword = "hw_mqtt_beonit";

WiFiClient espClient;
PubSubClient client(espClient);
WebServer server(80);

const long utcOffsetInSeconds = 25200; // ค่า offset ของเขตเวลา GMT+7 (เวลาไทย)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);



void handleRoot() {
  String ledState = digitalRead(LED_PIN) ? "checked" : "";
  String page = "<h1>ESP32 LED Control</h1>";
  page += "<label for='brightness'>Brightness (0-10):</label>";
  page += "<input type='range' id='brightness' name='brightness' min='0' max='10' value='0'";
  page += "onchange='setBrightness(this.value)'><br>";
  page += "<script>";
  page += "function setBrightness(value) {";
  page += "var xhr = new XMLHttpRequest();";
  page += "xhr.open('POST', '/led', true);";
  page += "xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');";
  page += "xhr.send('brightness=' + encodeURIComponent(value));";
  page += "}";
  page += "</script>";
  server.send(200, "text/html", page);
}

void handleLED() {
  int brightness = server.arg("brightness").toInt();
  brightness = constrain(brightness, 0, 10); // brightness 0 to 10
  analogWrite(LED_PIN, map(brightness, 0, 10, 0, 255)); // convert 0 - 255 to 0 - 10
}

unsigned long startTime = 0;

void setup(void)
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Connected to the WiFi network");

  client.setServer(mqttServer, mqttPort);

  pinMode(LED_PIN, OUTPUT);
  server.on("/", HTTP_GET, handleRoot);
  server.on("/led", HTTP_POST, handleLED);
  server.begin();

  timeClient.begin();
  startTime = millis();
}
void loop(void)
{
  if (!client.connected())
  {
      Serial.println("Connecting to MQTT...");
  
      if (client.connect("Nunnaphut", mqttUser, mqttPassword))
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

  timeClient.update();
  
  unsigned long elapsedTime = (millis() - startTime) / 1000;
  int elapsedMinutes = elapsedTime / 60; // second

  delay(1000);

  if ((elapsedMinutes % 30 == 15 || elapsedMinutes % 30 == 45) && elapsedMinutes > 0)
  {
    StaticJsonBuffer<300> JSONbuffer;
    JsonObject& JSONencoder = JSONbuffer.createObject();

    JSONencoder["status"] = server.arg("brightness").toInt() > 0 ? "ON" : "OFF";
    JSONencoder["brightness"] = server.arg("brightness").toInt();
    JSONencoder["duration"] = (int)(elapsedMinutes / 60);
    JSONencoder["Esp32Time"] = timeClient.getFormattedTime();

    char JSONmessageBuffer[100];
    JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));

    Serial.println(JSONmessageBuffer);
   
    if (client.publish("HW_mqtt/testing/01", JSONmessageBuffer) == true) {
        Serial.println("Success sending message");
    } else {
        Serial.println("Error sending message");
    }
  }
  client.loop();
  server.handleClient();
}
