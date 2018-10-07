#include "snips_led.h"

#ifndef __TYPEDEF__H__
	#include <typedef.h>
#endif

#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define	ROFFSET	1
#define GOFFSET 2
#define BOFFSET 3

#define	BITRATE	32000000

void begin(void);
void set_index_rgb(uint8_t index, uint8_t b, uint8_t g, uint8_t r);
void set_index_4byte(uint8_t index, uint32_t color);
void show(void);
void clear(void);

