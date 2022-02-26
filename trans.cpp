#include <RTClib.h>

namespace Trans {
  const char* weekdays[7] = {
    "Sonntag",
    "Montag",
    "Dienstag",
    "Mittwoch",
    "Donnerstag",
    "Freitag",
    "Samstag",
  };


  const char* hours[12]  = {
    "zwoife",
    "oas",
    "zwoa",
    "drei",
    "viere",
    "fünfe",
    "sechse",
    "siebne",
    "ochte",
    "neine",
    "zehne",
    "öfe",
  };
  const char* minutes[5] = {
    "fünf",
    "zehn",
    "fiatl",
    "zwoanzg",
    "dreifiatl",
  };
  const char* before =  "fo";
  const char* after = "noch";
  const char* over = "iwa";
  const char* half = "hoiba";


  int roundTo(int num, int stepSize){
    return num % stepSize > stepSize / 2 ? 
      num + stepSize - num % stepSize :
      num - num % stepSize;
  }

  void upperString(DateTime dt,  char* buf) {
    int mins = roundTo(dt.minute(), 5);
    switch(mins) {
      case 0:
        buf[0] = 0;
        break;
      case 5: // fünf noch
        strcpy(buf, minutes[0]);
        buf += strlen(minutes[0]);
        *(buf++) = ' ';
        strcpy(buf, after);
        break;
      case 10: // zehn noch
        strcpy(buf, minutes[1]);
        buf += strlen(minutes[1]);
        *(buf++) = ' ';
        strcpy(buf, after);
        break;
      case 15: // fiatl iwa
        strcpy(buf, minutes[2]);
        buf += strlen(minutes[2]);
        *(buf++) = ' ';
        strcpy(buf, over);
        break;
      case 20: // zwoanzg [noch]
        strcpy(buf, minutes[3]);
        buf += strlen(minutes[3]);
        break;
      case 25: // fünf foa [hoiba]
        strcpy(buf, minutes[0]);
        buf += strlen(minutes[0]);
        *(buf++) = ' ';
        strcpy(buf, before);
        break;
      case 30:  // [hoiba]
        strcpy(buf, half);
        break;
      case 35:  // fünf noch [hoiba]
        strcpy(buf, minutes[0]);
        buf += strlen(minutes[0]);
        *(buf++) = ' ';
        strcpy(buf, after);
        break;
      case 40: // zwoanzg [foa hour+1]
        strcpy(buf, minutes[3]);
        break;
      case 45: // dreifiatl [hour+1]
        strcpy(buf, minutes[4]);
        break;
      case 50: // zehn foa [hour+1]
        strcpy(buf, minutes[1]);
        buf += strlen(minutes[1]);
        *(buf++) = ' ';
        strcpy(buf, before);
        break;
      case 55: // fünf foa [hour+1]
        strcpy(buf, minutes[0]);
        buf += strlen(minutes[0]);
        *(buf++) = ' ';
        strcpy(buf, before);
        break;
      case 60: // [hour+1]
        buf[0] = 0;
        break;
    }
  }

  void lowerString(DateTime dt, char* buf) {
    int hour = dt.hour(); 
    int mins = roundTo(dt.minute(), 5);

    if (mins >= 25) {  // add "half" to second line
      hour++;
    }

    
      if (mins == 35 || mins == 25) {
        strcpy(buf, half);
        buf += strlen(half);
        *(buf++) = ' ';
      }

     // add "before" or "after" to second line because "twenty" is too long
    if (mins == 20) {
      strcpy(buf, after);
      buf += strlen(after);
      *(buf++) = ' ';
    }

    if (mins == 40) {
      strcpy(buf, before);
      buf += strlen(before);
      *(buf++) = ' ';
    }
    
    strcpy(buf, hours[hour % 12]);
  }
}
