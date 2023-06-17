#include <Arduino.h>
#include <ESP8266SAM_ES.h>
#include <AudioOutputI2S.h>

AudioOutputI2S *out = NULL;

void setup()
{
  out = new AudioOutputI2S();
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
