#include <Arduino.h>

#define LED 2

void setup() {
  // initialize LED digital pin as an output.
  pinMode(LED, OUTPUT);
}

void loop() {
  // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED, HIGH);
  // wait for a second
  delay(500);
  // turn the LED off by making the voltage LOW
  digitalWrite(LED, LOW);
   // wait for a second
  delay(2000);
}
