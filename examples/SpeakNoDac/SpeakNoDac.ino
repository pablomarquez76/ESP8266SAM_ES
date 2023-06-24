#include <Arduino.h>
#include <ESP8266SAM_ES.h>
#include "AudioOutputI2SNoDAC.h"

AudioOutputI2SNoDAC *out = NULL;

void setup()
{
  out = new AudioOutputI2SNoDAC(); // with ESP8266 use pin GPIO03(RX) but with ESP32 use pin GPIO22
  out->begin();
}

void loop()
{
  ESP8266SAM_ES *sam = new ESP8266SAM_ES;
  sam->Say(out, "Puedes oirme?");
  delay(500);
  sam->Say(out, "Sí, por supuesto!");
  delete sam;
}
