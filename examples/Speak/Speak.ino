#include <Arduino.h>
#include <ESP8266SAM_ES.h>
#include <AudioOutputInternalDAC.h>

AudioOutputInternalDAC *out = NULL;

void setup()
{
  out = new AudioOutputInternalDAC(); // ESP32 pin 25
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
