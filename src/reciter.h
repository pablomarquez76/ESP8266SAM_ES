/*
  ESP8266SAM_ES
  Port of SAM to the ESP8266 in spanish
  
  Copyright (C) 2022  Juan Pablo Marquez
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
*/

#define MAX_LONG 20
byte regionN = 0; // regionN: 0 AR (Argentina y Uruguay)  1 ES (España) 2 OT (Otros)

boolean esVocal(char cr) {
  if (cr == 'A' ||
      cr == 'E' ||
      cr == 'I' ||
      cr == 'O' ||
      cr == 'U')
    return true;
  return false;
}

boolean esVocalFuerte(char cr) {
  if (cr == 'A' ||
      cr == 'E' ||
      cr == 'O')
    return true;
  return false;
}

boolean esPuntuacion(char cr) {
  if (cr == ' ' ||
      cr == '-' ||
      cr == ',' ||
      cr == ';' ||
      cr == '.' ||
      cr == '?' ||
      cr == '!')
    return true;
  return false;
}

boolean tieneTilde(char* palabra, int len) {
  for (int ind = 0; ind < len; ind++) {
    if (palabra[ind] == 195) {
      char c2 = palabra[ind + 1];
      if (c2 == 161 || c2 == 129 ||   // Á
          c2 == 169 || c2 == 137 ||   // É
          c2 == 173 || c2 == 141 ||   // Í
          c2 == 179 || c2 == 147 ||   // Ó
          c2 == 186 || c2 == 154) {   // Ú
        return true;
      }
    }
  }
  return false;
}

boolean terminaNSVocal(char* palabra, int len) {
  char cr = palabra[len - 1];
  if (cr == 'N' ||
      cr == 'S' ||
      esVocal(cr))
    return true;
  return false;
}

boolean tildeEI(char cr) {
  if (cr == 169 || cr == 137 || // É
      cr == 173 || cr == 141)   // Í
    return true;
  return false;
}

int indiceUltVoc(char* palabra, int len) {
  for (int i = len - 1; i > -1; i--) {
    if (esVocal(palabra[i])) {
      return i;
    }
  }
  return MAX_LONG;
}

int indiceAntUltVoc(char* palabra, int len) {
  int uv = indiceUltVoc(palabra, len);
  if (esVocalFuerte(palabra[uv]) && esVocalFuerte(palabra[uv - 1])) return uv - 1;
  for (int i = uv - 2; i > -1; i--) {
    if (esVocal(palabra[i])) {
      return i;
    }
  }
  return MAX_LONG;
}

int indiceAcento(char* palabra, int len) {
  if (!tieneTilde(palabra, len)) {
    if (terminaNSVocal(palabra, len)) {
      return indiceAntUltVoc(palabra, len);
    } else {
      return indiceUltVoc(palabra, len);
    }
  }
  return -1;
}

void textoAFonemas(char* txt, char* fonema) {
  int ind_fon = 0;
  char cr, c1, c2, c3;
  boolean punt = true;
  int ind_acento = MAX_LONG;

  for (int ind_txt = 0; ind_txt < 60; ind_txt++) {
    txt[ind_txt] = toupper(txt[ind_txt]);
  }

  for (int ind_txt = 0; ind_txt < 60; ind_txt++) {
    cr = txt[ind_txt];
    c1 = txt[ind_txt + 1];
    c2 = txt[ind_txt + 2];
    c3 = txt[ind_txt + 3];

    if (esPuntuacion(cr)) {
      punt = true;
    }
    else if (punt) {
      int ind = 0;
      while (ind < MAX_LONG) {
        char car = txt[ind_txt + ind];
        if (esPuntuacion(car) || car == 0) {
          char palabra[ind];
          strncpy(palabra, txt + ind_txt, ind);
          ind_acento = ind_txt + indiceAcento(palabra, ind);
          break;
        }
        else ind++;
      }
      punt = false;
    }

    switch (cr) {

      // orden: _, E, A, O, S, R, N, I, D, L, C, T, U, M, P, B,
      //        ., ,, G, V, Y, Q, H, F, Z, J, Ñ, X, K, W.

      case ' ':
        fonema[ind_fon++] = cr;
        if (c1 == 'R') {
          fonema[ind_fon++] = 'R';
        }
        break;

      case 'E':
        fonema[ind_fon++] = 'E';
        fonema[ind_fon++] = 'H';
        break;

      case 'A':
        fonema[ind_fon++] = 'A';
        fonema[ind_fon++] = 'A';
        break;

      case 'O':
        fonema[ind_fon++] = 'O';
        fonema[ind_fon++] = 'H';
        break;

      case 'S':
        fonema[ind_fon++] = 'S';
        if (c1 == 'R') {
          fonema[ind_fon++] = 'R';
        }
        break;

      case 'R':
        if (ind_txt == 0) {
          fonema[ind_fon++] = 'R';
          fonema[ind_fon++] = 'R';
        } else {
          fonema[ind_fon++] = 'R';
        }
        break;

      case 'N':
        fonema[ind_fon++] = 'N';
        if (c1 == 'R') {
          fonema[ind_fon++] = 'R';
        }
        break;

      case 'I':
        fonema[ind_fon++] = 'I';
        fonema[ind_fon++] = 'Y';
        break;

      case 'D':
        fonema[ind_fon++] = 'D';
        fonema[ind_fon++] = 'H';
        break;

      case 'L':
        if (c1 == 'L' ) {
          if (regionN == 0) {
            fonema[ind_fon++] = 'S'; // argentina
            fonema[ind_fon++] = 'H'; // argentina
          } else {
            fonema[ind_fon++] = 'Y'; // otros
          }
          ind_txt++;
        } else if (c1 == 'R') {
          fonema[ind_fon++] = 'L';
          fonema[ind_fon++] = 'R';
        } else {
          fonema[ind_fon++] = 'L';
        }
        break;

      case 'C':
        if (c1 == 'E' ||
            c1 == 'I' ||
            (c1 == 195 && tildeEI(c2))) {
          if (regionN == 1) {
            fonema[ind_fon++] = 'T'; // españa
            fonema[ind_fon++] = 'H'; // españa
          } else {
            fonema[ind_fon++] = 'S'; // otros
          }
        } else if (c1 == 'H') {
          fonema[ind_fon++] = 'C';
          fonema[ind_fon++] = 'H';
          ind_txt++;
        } else {
          fonema[ind_fon++] = 'K';
        }
        break;

      case 'T':
        fonema[ind_fon++] = 'T';
        fonema[ind_fon++] = 'T';
        break;

      case 'U':
        fonema[ind_fon++] = 'U';
        fonema[ind_fon++] = 'W';
        break;

      case 'M':
        fonema[ind_fon++] = 'M';
        break;

      case 'P':
        fonema[ind_fon++] = 'P';
        break;

      case 'B':
        fonema[ind_fon++] = 'B';
        break;

      case '-':
      case ',':
      case '.':
      case '?':
        fonema[ind_fon++] = cr;
        break;

      case 'G':
        if (c1 == 'E' || c1 == 'I' ||
            (c1 == 195 && tildeEI(c2))) {
          fonema[ind_fon++] = '/';
          fonema[ind_fon++] = 'H';
        } else if (c1 == 'U' &&
                   (c2 == 'E' ||
                    c2 == 'I' ||
                    (c2 == 195 && tildeEI(c3)))) {
          fonema[ind_fon++] = 'G';
          ind_txt++;
        }  else {
          fonema[ind_fon++] = 'G';
        }
        break;

      case 'V':
        fonema[ind_fon++] = 'V';
        break;

      case 'Y':
        if (esVocal(c1)) {
          if (regionN == 0) {
            fonema[ind_fon++] = 'S'; // argentina
            fonema[ind_fon++] = 'H'; // argentina
          } else {
            fonema[ind_fon++] = 'Y'; // otros
          }
        } else {
          fonema[ind_fon++] = 'I';
          fonema[ind_fon++] = 'Y';
        }
        break;

      case 'Q':
        if (c1 == 'U'
            && (c2 == 'E' ||
                c2 == 'I' ||
                (c2 == 195 && tildeEI(c3)))) {
          fonema[ind_fon++] = 'K';
          ind_txt++;
        }  else {
          fonema[ind_fon++] = 'K';
        }
        break;

      case 'H':
        break;

      case 'F':
        fonema[ind_fon++] = 'F';
        break;

      case 'Z':
        if (regionN == 1) {
          fonema[ind_fon++] = 'T'; // españa
          fonema[ind_fon++] = 'H'; // españa
        } else {
          fonema[ind_fon++] = 'S'; // otros
        }
        break;

      case 'J':
        fonema[ind_fon++] = '/';
        fonema[ind_fon++] = 'H';
        break;

      case 195:
        if (c1 == 177 || c1 == 145) {        // Ñ
          fonema[ind_fon++] = 'N';
          fonema[ind_fon++] = 'I';
          fonema[ind_fon++] = 'Y';
        } else if (c1 == 161 || c1 == 129) { // Á
          fonema[ind_fon++] = 'A';
          fonema[ind_fon++] = 'A';
          fonema[ind_fon++] = '4';
        } else if (c1 == 169 || c1 == 137) { // É
          fonema[ind_fon++] = 'E';
          fonema[ind_fon++] = 'H';
          fonema[ind_fon++] = '4';
        } else if (c1 == 173 || c1 == 141) { // Í
          fonema[ind_fon++] = 'I';
          fonema[ind_fon++] = 'Y';
          fonema[ind_fon++] = '4';
        } else if (c1 == 179 || c1 == 147) { // Ó
          fonema[ind_fon++] = 'O';
          fonema[ind_fon++] = 'H';
          fonema[ind_fon++] = '4';
        } else if (c1 == 186 || c1 == 154) { // Ú
          fonema[ind_fon++] = 'U';
          fonema[ind_fon++] = 'W';
          fonema[ind_fon++] = '4';
        } else if (c1 == 188 || c1 == 156) { // Ü
          fonema[ind_fon++] = 'U';
          fonema[ind_fon++] = 'W';
        }
        ind_txt++;
        break;

      case 'X':
        fonema[ind_fon++] = 'K';
        fonema[ind_fon++] = 'S';
        break;

      case 'K':
        fonema[ind_fon++] = 'K';
        break;

      case 'W':
        fonema[ind_fon++] = 'W';
        break;

      case ';':
      case '!':
        fonema[ind_fon++] = '.';
        break;

      case 10:
      case 13:
        fonema[ind_fon++] = ' ';
        break;

      case 0:
        fonema[ind_fon++] = 0;
        return;
        break;

      case '0':
        if (regionN == 1) {
          fonema[ind_fon++] = 'T'; // españa
          fonema[ind_fon++] = 'H'; // españa
        } else {
          fonema[ind_fon++] = 'S'; // otros
        }
        fonema[ind_fon++] = 'E';
        fonema[ind_fon++] = 'H';
        fonema[ind_fon++] = '4';
        fonema[ind_fon++] = 'R';
        fonema[ind_fon++] = 'O';
        fonema[ind_fon++] = 'H';
        fonema[ind_fon++] = ' ';
        break;

      case '1':
        fonema[ind_fon++] = 'U';
        fonema[ind_fon++] = 'W';
        fonema[ind_fon++] = '4';
        fonema[ind_fon++] = 'N';
        fonema[ind_fon++] = 'O';
        fonema[ind_fon++] = 'H';
        fonema[ind_fon++] = ' ';
        break;

      case '2':
        fonema[ind_fon++] = 'D';
        fonema[ind_fon++] = 'H';
        fonema[ind_fon++] = 'O';
        fonema[ind_fon++] = 'H';
        fonema[ind_fon++] = '4';
        fonema[ind_fon++] = 'S';
        fonema[ind_fon++] = ' ';
        break;

      case '3':
        fonema[ind_fon++] = 'T';
        fonema[ind_fon++] = 'T';
        fonema[ind_fon++] = 'R';
        fonema[ind_fon++] = 'E';
        fonema[ind_fon++] = 'H';
        fonema[ind_fon++] = '4';
        fonema[ind_fon++] = 'S';
        fonema[ind_fon++] = ' ';
        break;

      case '4':
        fonema[ind_fon++] = 'K';
        fonema[ind_fon++] = 'U';
        fonema[ind_fon++] = 'W';
        fonema[ind_fon++] = 'A';
        fonema[ind_fon++] = 'A';
        fonema[ind_fon++] = '4';
        fonema[ind_fon++] = 'T';
        fonema[ind_fon++] = 'T';
        fonema[ind_fon++] = 'R';
        fonema[ind_fon++] = 'O';
        fonema[ind_fon++] = 'H';
        fonema[ind_fon++] = ' ';
        break;

      case '5':
        if (regionN == 1) {
          fonema[ind_fon++] = 'T'; // españa
          fonema[ind_fon++] = 'H'; // españa
        } else {
          fonema[ind_fon++] = 'S'; // otros
        }
        fonema[ind_fon++] = 'I';
        fonema[ind_fon++] = 'Y';
        fonema[ind_fon++] = '4';
        fonema[ind_fon++] = 'N';
        fonema[ind_fon++] = 'K';
        fonema[ind_fon++] = 'O';
        fonema[ind_fon++] = 'H';
        fonema[ind_fon++] = ' ';
        break;

      case '6':
        fonema[ind_fon++] = 'S';
        fonema[ind_fon++] = 'E';
        fonema[ind_fon++] = 'H';
        fonema[ind_fon++] = '4';
        fonema[ind_fon++] = 'I';
        fonema[ind_fon++] = 'Y';
        fonema[ind_fon++] = 'S';
        fonema[ind_fon++] = ' ';
        break;

      case '7':
        fonema[ind_fon++] = 'S';
        fonema[ind_fon++] = 'I';
        fonema[ind_fon++] = 'Y';
        fonema[ind_fon++] = 'E';
        fonema[ind_fon++] = 'H';
        fonema[ind_fon++] = '4';
        fonema[ind_fon++] = 'T';
        fonema[ind_fon++] = 'T';
        fonema[ind_fon++] = 'E';
        fonema[ind_fon++] = 'H';
        fonema[ind_fon++] = ' ';
        break;

      case '8':
        fonema[ind_fon++] = 'O';
        fonema[ind_fon++] = 'H';
        fonema[ind_fon++] = '4';
        fonema[ind_fon++] = 'C';
        fonema[ind_fon++] = 'H';
        fonema[ind_fon++] = 'O';
        fonema[ind_fon++] = 'H';
        fonema[ind_fon++] = ' ';
        break;

      case '9':
        fonema[ind_fon++] = 'N';
        fonema[ind_fon++] = 'U';
        fonema[ind_fon++] = 'W';
        fonema[ind_fon++] = 'E';
        fonema[ind_fon++] = 'H';
        fonema[ind_fon++] = '4';
        fonema[ind_fon++] = 'V';
        fonema[ind_fon++] = 'E';
        fonema[ind_fon++] = 'H';
        fonema[ind_fon++] = ' ';
        break;
    }
    if (ind_txt == ind_acento) fonema[ind_fon++] = '4';
  }
}
