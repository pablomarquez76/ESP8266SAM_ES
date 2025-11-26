#include "lector.h"
// Initial setup
void setup() {
  Serial.begin(115200);
  out = new AudioOutputInternalDAC();  // ESP32 pin 25
  out->begin();
  double D = 456285;
  decirNumero(numACadena(D));
  float F = 0.5265;
  decirNumero(numACadena(F));
  long L = -45000678;
  decirNumero(L);
}
// Main loop
void loop() {
  if (Serial.available() > 0) {
    String texto = Serial.readString();
    decirNumero(texto);
  }
}
