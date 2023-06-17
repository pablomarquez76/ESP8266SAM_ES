#include <Arduino.h>
#include <ESP8266SAM_ES.h>
#include <AudioOutputI2S.h>

AudioOutputI2S *out = NULL;

char linea[60];
int pos = 0;
int punt_linea = 0;

void setup() {
  Serial.begin(115200);
  out = new AudioOutputI2S(0, 1);  // to use the internal DAC channel 1 (pin25) on ESP32
  out->begin();
}

void loop() {
  if (Serial.available() > 0) {

    char cr;
    if (pos < 59) {
      cr = (char)Serial.read();
    } else {
      cr = '\n';
    }

    if (cr == '\n') {

      if (punt_linea > 0 && pos == 59) {
        linea[punt_linea] = 0;
      } else {
        linea[pos++] = 0;
        pos = 0;
        punt_linea = pos;
      }
      Serial.println(linea);

      ESP8266SAM_ES *sam = new ESP8266SAM_ES;
      sam->SetPhonetic(false);
      if (linea[0] == '-') {
        sam->SetPhonetic(true);
        linea[0] = ' ';
      } else if (linea[0] == '+') {  // SetVoice
        if (linea[1] == '0') sam->SetVoice(sam->SAMVoice::VOICE_SAM);
        else if (linea[1] == '1') sam->SetVoice(sam->SAMVoice::VOICE_ELF);
        else if (linea[1] == '2') sam->SetVoice(sam->SAMVoice::VOICE_ROBOT);
        else if (linea[1] == '3') sam->SetVoice(sam->SAMVoice::VOICE_STUFFY);
        else if (linea[1] == '4') sam->SetVoice(sam->SAMVoice::VOICE_OLDLADY);
        else if (linea[1] == '5') sam->SetVoice(sam->SAMVoice::VOICE_ET);
        else if (linea[1] == 'a') sam->SetRegion(sam->SAMRegion::REGION_AR);
        else if (linea[1] == 'e') sam->SetRegion(sam->SAMRegion::REGION_ES);
        else if (linea[1] == 'o') sam->SetRegion(sam->SAMRegion::REGION_OT);
        linea[0] = ' ';
        linea[1] = ' ';
      }
      sam->Say(out, linea);
      delete sam;

      if (punt_linea > 0) {
        int i = 0;
        for (int ind = punt_linea + 1; ind < pos; ind++) {
          linea[i++] = linea[ind];
        }
        pos = i;
        punt_linea = 0;
      }

    } else {
      if (esPuntuacion2(cr)) {
        punt_linea = pos;
      }
      linea[pos++] = cr;
    }
  }
}

boolean esPuntuacion2(char cr) {
  if (cr == ' ' || cr == '-' || cr == ',' || cr == ';' || cr == '.' || cr == '?' || cr == '!')
    return true;
  return false;
}
