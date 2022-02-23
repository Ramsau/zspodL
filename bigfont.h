#ifndef BIGFONT_H
#define BIGFONT_H

#include <stdint.h>
#include <avr/pgmspace.h>

struct BigFont {
  const uint8_t height;
  const uint8_t max_width;
  const uint8_t space_width;
  const uint8_t number_location;
  const uint8_t* letters[];
};

#define FONT60_HEIGHT 55
#define FONT60_MAX_WIDTH 55

#define FONT110_HEIGHT 103
#define FONT110_MAX_WIDTH 104

#define FONT40_HEIGHT 38
#define FONT40_MAX_WIDTH 38

extern const BigFont font110;
extern const BigFont font40;

#endif
