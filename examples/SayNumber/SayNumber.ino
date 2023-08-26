#include <Arduino.h>
#include <ESP8266SAM_ES.h>
#include "AudioOutputI2SNoDAC.h"
#define MAX_LONG 20
#define MILLON 1000000
#define MIL 1000
#define CIEN 100
#define DIEZ 10
AudioOutputI2SNoDAC *out = NULL;
char linea[60];
int pos = 0;
String numero;
// Initial setup
void setup() {
  Serial.begin(115200);
  out = new AudioOutputI2SNoDAC();  // with ESP8266 use pin GPIO03(RX) but with ESP32 use pin GPIO22
  out->begin();
}
// Main loop
void loop() {
  if (Serial.available() > 0) {
    char cr;
    if (pos < 21) {  // Reads only numbers with 21 characters
      cr = (char)Serial.read();
    } else {
      cr = '\n';
    }
    if (cr == '\n') {
      long val;  // range -2,147,483,648 to 2,147,483,647
      linea[pos++] = 0;
      Serial.print(linea);
      Serial.print(">");
      if (pos == 1) val = random(0, 2147483647);  // Says random number integer if press "Enter" alone
      else val = atoi(linea);
      Serial.print(val);
      decirNumero(val);  // Says integer part
      int punt_linea;
      while (punt_linea > -1) {
        punt_linea = -1;
        for (int i = 0; i < pos; i++) {
          if (linea[i] == '.' || linea[i] == ',' || linea[i] == 'e') {  // Find decimal separator (by country "," or ".")
            punt_linea = i;
            break;
          }
        }
        if (punt_linea > -1) {
          ESP8266SAM_ES *sam = new ESP8266SAM_ES;
          if (linea[punt_linea] == ',') {
            Serial.print(",");
            sam->Say(out, " coma ");
          } else if (linea[punt_linea] == '.') {
            Serial.print(".");
            sam->Say(out, " punto ");
          } else {
            Serial.print(".10^");
            sam->Say(out, " por diez a la ");
          }
          delete sam;
          for (int i = 0; i < pos - punt_linea; i++)
            linea[i] = linea[i + punt_linea + 1];
          val = atoi(linea);
          Serial.print(val);
          decirNumero(val);  // Says decimal part
        }
      }
      Serial.println();
      pos = 0;
    } else {
      linea[pos++] = cr;
    }
  }
}
// Says number
void decirNumero(long val) {
  if (val < 0) {
    val = -val;
    numero = "menos ";
  } else
    numero = "";
  numATexto(val);
  char texto[60];
  String num;
  int inicio = 0, punt_linea = 0;
  ESP8266SAM_ES *sam = new ESP8266SAM_ES;
  while (numero.length() - inicio > 60) {
    for (int i = inicio; i < inicio + 60; i++)
      if (numero[i] == ' ') punt_linea = i;
    num = numero.substring(inicio, punt_linea);
    num.toCharArray(texto, 60);
    sam->Say(out, texto);
    inicio = punt_linea + 1;
  }
  num = numero.substring(inicio);
  num.toCharArray(texto, 60);
  sam->Say(out, texto);
  delete sam;
}
// Returns part of number string
char *enLetrasAux(int indice1, int indice2) {
  char *unidades[] = { "un", "dós", "trés", "cuatro", "cinco", "séis", "siete", "ocho", "nueve" };
  char *dieces[] = { "once", "doce", "trece", "catorce", "quince", "dieciséis", "diecisiete", "dieciocho", "diecinueve" };
  char *decenas[] = { "diez", "veint", "treinta", "cuarenta", "cincuenta", "sesenta", "setenta", "ochenta", "noventa" };
  char *centenas[] = { "cien", "doscientos", "trescientos", "cuatrocientos", "quinientos", "seiscientos", "setecientos", "ochocientos", "novecientos" };
  char *millares[] = { "mil", "millón" };
  switch (indice2) {
    case 0: return unidades[indice1];
    case 1: return dieces[indice1];
    case 2: return decenas[indice1];
    case 3: return centenas[indice1];
  }
  return millares[indice1];
}
// Process thusands
void millares(const int a_numerode, const int indice) {
  if (indice == 0) {
    numero += enLetrasAux(0, 4);
  } else {
    if (a_numerode > 1) {
      numero += enLetrasAux(1, 4);
      numero += "es";
    } else {
      numero += enLetrasAux(0, 0);
      numero += " ";
      numero += enLetrasAux(1, 4);
    }
  }
  numero += " ";
}
// Process number
void enLetras(const long a_numerode) {
  if (a_numerode < 10) {
    numero += enLetrasAux(a_numerode - 1, 0);
  } else if (a_numerode > 10 && a_numerode < 20) {
    numero += enLetrasAux(a_numerode % DIEZ - 1, 1);
  } else if ((a_numerode % DIEZ) == 0 && a_numerode < CIEN) {
    numero += enLetrasAux((a_numerode / DIEZ) - 1, 2);
  } else if ((a_numerode % CIEN) == 0 && a_numerode < MIL) {
    numero += enLetrasAux((a_numerode / CIEN) - 1, 3);
    if (a_numerode == CIEN) numero += "to";
    numero += " ";
  } else if ((a_numerode % MIL) == 0 && a_numerode < MILLON) {
    millares(a_numerode / MIL, 0);
  } else if ((a_numerode % MILLON) == 0) {
    millares(a_numerode / MILLON, 1);
  }
}
// Finds residue to evaluate number
void residuo(long *cantidad, const long valor) {
  enLetras(*cantidad - (*cantidad % valor));
  *cantidad = *cantidad % valor;
}
// Main number processing function (recursive)
void numATextoAux(long a_cantidad) {
  while (a_cantidad > 0) {
    if (a_cantidad >= MILLON) {
      if (a_cantidad / MILLON > 1) numATextoAux(a_cantidad / MILLON);  // --> recursive
      residuo(&a_cantidad, MILLON);
    } else if (a_cantidad >= MIL) {
      if (a_cantidad / MIL > 1) numATextoAux(a_cantidad / MIL);  // --> recursive
      residuo(&a_cantidad, MIL);
    } else if (a_cantidad >= CIEN) {
      residuo(&a_cantidad, CIEN);
    } else if (a_cantidad > 29) {
      residuo(&a_cantidad, DIEZ);
      numero += " ";
      if (a_cantidad > 0) numero += "y ";
    } else if (a_cantidad > 20) {
      residuo(&a_cantidad, DIEZ);
      numero += "i";
    } else if (a_cantidad == 20) {
      residuo(&a_cantidad, DIEZ);
      numero += "e ";
    } else if (a_cantidad == DIEZ) {
      residuo(&a_cantidad, DIEZ);
      numero += " ";
    } else {
      residuo(&a_cantidad, a_cantidad);
      numero += " ";
    }
  }
}
// Main number to string function
void numATexto(long a_cantidad) {
  numATextoAux(a_cantidad);
  if (a_cantidad == 0) {
    numero = "zero";
  } else if (a_cantidad % DIEZ == 1 && a_cantidad % CIEN != 11) {
    numero = numero.substring(0, numero.length() - 1);
    numero += "o";
  }
}
