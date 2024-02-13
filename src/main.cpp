
// vesion 1 Brightness LED
#include <Arduino.h>

#define LED_PIN 18


const int WAITTIME = 50;
const int STEP = 5;

void setup(){
  pinMode( LED_PIN, OUTPUT );
  Serial.begin(115200);
}

void loop(){
  int i;

  i = 0;
  while ( i <= 255 ){
      analogWrite( LED_PIN, i );
      Serial.println(i);
      delay( WAITTIME );
      i = i + STEP;
  }
  i = 255;
  while ( i >= 0 ){
      analogWrite( LED_PIN, i );
      delay( WAITTIME );
      i = i - STEP;
  }
}

// // version 1 LED on-off
// void setup(){
//   Serial.begin(115200);
//   pinMode (LED, OUTPUT);
// }

// void loop(){
//   digitalWrite(LED_PIN, HIGH);
//   delay(1000);
//   digitalWrite(LED_PIN,LOW);
//   delay(1000);
// }