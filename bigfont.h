#ifndef BIGFONT_H
#define BIGFONT_H

#include <stdint.h>
#include <avr/pgmspace.h>

struct bigfont {
  const uint8_t width;
  const uint8_t* table;
};

#define FONT60_HEIGHT 55
#define FONT60_MAX_WIDTH 55
extern const bigfont font60[];
extern const uint8_t _char_97[] PROGMEM;
extern const uint8_t _char_100[] PROGMEM;
extern const uint8_t _char_99[] PROGMEM;


#endif
