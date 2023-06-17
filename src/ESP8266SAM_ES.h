/*
  ESP8266SAM_ES
  Port of SAM to the ESP8266 Spanish

  Copyright (C) 2017  Earle F. Philhower, III
  Modified by Juan Pablo Marquez
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
  
  added: Regional accents
*/

#ifndef _ESP8266SAM_ES_H
#define _ESP8266SAM_ES_H

#include <Arduino.h>
#include <AudioOutput.h>

class ESP8266SAM_ES {

public:
  ESP8266SAM_ES()
  {
    singmode = false;
    phonetic = false;
    pitch = 0;
    mouth = 0;
    throat = 0;
    speed = 0;
    output = NULL;
  };
  
  ~ESP8266SAM_ES()
  {
  }

  enum SAMVoice { VOICE_SAM, VOICE_ELF, VOICE_ROBOT, VOICE_STUFFY, VOICE_OLDLADY, VOICE_ET };
  enum SAMRegion { REGION_AR, REGION_ES, REGION_OT };
  void SetVoice(enum SAMVoice voice);
  void SetRegion(enum SAMRegion region);

  void SetSingMode(bool val) { singmode = val; }
  void SetPhonetic(bool val) { phonetic = val; }
  void SetPitch(uint8_t val) { pitch = val; }
  void SetMouth(uint8_t val) { mouth = val; }
  void SetThroat(uint8_t val) { throat = val; }
  void SetSpeed(uint8_t val) { speed = val; }

  bool Say(AudioOutput *out, const char *str);

private:
  static void OutputByteCallback(void *cbdata, unsigned char b);
  void OutputByte(unsigned char b);
  bool singmode;
  bool phonetic;
  int pitch;
  int speed;
  int mouth;
  int throat;
  AudioOutput *output;
};

#endif
