#include <Arduino.h>
#include <ESP8266SAM_ES.h>
#include <AudioOutputInternalDAC.h>
AudioOutputInternalDAC *out = NULL;
#define MILLON 1000000
#define MIL 1000
#define CIEN 100
#define DIEZ 10
String numero;
// Devuelve parte del numero en texto
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
// Procesa miles
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
// Procesa numero
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
// Encuentra el residuo al ecaluar el numero
void residuo(long *cantidad, const long valor) {
  enLetras(*cantidad - (*cantidad % valor));
  *cantidad = *cantidad % valor;
}
// Procesa numero en forma recursiva
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
// Funcion principal que cambia numero a texto
void numATexto(long a_cantidad) {
  numATextoAux(a_cantidad);
  if (a_cantidad == 0) {
    numero = "zero";
  } else if (a_cantidad % DIEZ == 1 && a_cantidad % CIEN != 11) {
    numero = numero.substring(0, numero.length() - 1);
    numero += "o";
  }
}
// Dice texto -----------------------------------------------------------------------------------------------
void decirTexto(String stexto) {
  char texto[60];
  stexto.toCharArray(texto, 60);
  ESP8266SAM_ES *sam = new ESP8266SAM_ES;
  sam->Say(out, texto);
  delete sam;
}
// Dice numero entero
void decirNumero(long val) {
  if (val < 0) {
    val = -val;
    numero = "menos ";
  } else
    numero = "";
  numATexto(val);
  String num;
  int inicio = 0, punt_linea = 0;
  while (numero.length() - inicio > 60) {
    for (int i = inicio; i < inicio + 60; i++)
      if (numero[i] == ' ') punt_linea = i;
    num = numero.substring(inicio, punt_linea);
    decirTexto(num);
    inicio = punt_linea + 1;
  }
  num = numero.substring(inicio);
  decirTexto(num);
}
// Dice numero formato texto
void decirNumero(String num) {
  int pos = 0;
  char linea[20];
  strcpy(linea, num.c_str());
  for (int i = 0; i < 21; i++) {
    if (linea[i] == '\n') {  // encuentra fin de linea
      pos = i;
      break;
    } else if (linea[i] == '.') {  // encuentra separador decimal
      linea[i] = ',';
    }
  }
  long val;  // rango -2,147,483,648 to 2,147,483,647
  linea[pos++] = 0;
  val = atoi(linea);
  decirNumero(val);  // dice parte entera
  int punt_linea;
  while (punt_linea > -1) {
    punt_linea = -1;
    for (int i = 0; i < pos; i++) {
      if (linea[i] == ',' || linea[i] == 'e') {  // encuentra el separador decimal y exponente
        punt_linea = i;
        break;
      }
    }
    if (punt_linea > -1) {
      if (linea[punt_linea] == ',') {
        decirTexto("coma");
      } else {
        decirTexto("por diez a la");
      }
      for (int i = 0; i < pos - punt_linea; i++)
        linea[i] = linea[i + punt_linea + 1];
      pos = pos - punt_linea - 1;
      val = atoi(linea);
      decirNumero(val);  // dice parte decimal
    }
  }
}
// Cambia numero a cadena
String numACadena(int num) {
  char buffer[22];
  snprintf(buffer, sizeof(buffer), "%d\n", num);
  return buffer;
}
// Cambia numero a cadena
String numACadena(float num) {
  char buffer[22];
  snprintf(buffer, sizeof(buffer), "%g\n", num);
  return buffer;
}
// Cambia numero a cadena
String numACadena(double num) {
  char buffer[22];
  snprintf(buffer, sizeof(buffer), "%g\n", num);
  return buffer;
}
// Dice numero
void decirNumero(int num) {
  decirNumero(numACadena(num));
}
// Dice numero
void decirNumero(float num) {
  decirNumero(numACadena(num));
}
// Dice numero
void decirNumero(double num) {
  decirNumero(numACadena(num));
}
