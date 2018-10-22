#ifndef __SNIPS_LED_H__
	#include <snips_led.h>
#endif

#ifndef __TYPEDEF_H__
	#include <typedef.h>
#endif

#define ROFFSET 1
#define GOFFSET 2
#define BOFFSET 3

#define BITRATE 32000000

int begin(void);
void set_index_rgb(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
void set_index_4byte(uint8_t index, uint32_t color);
void show(void);
void clear(void);