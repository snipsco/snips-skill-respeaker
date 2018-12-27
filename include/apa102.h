#ifndef __APA102_H__
#define __APA102_H__

#include <common.h>

#define BOFFSET 1
#define GOFFSET 2
#define ROFFSET 3

#define BITRATE 32000000

typedef struct{
    uint8_t numLEDs;
    uint8_t spi_bus;
    uint8_t spi_dev;
    int     fd_spi;
    uint8_t *pixels;
    uint8_t brightness;
}APA102;

void set_leds_number(int number);
void set_leds_brightness(uint8_t bri);
int apa102_spi_setup(void);
int begin(void);
void set_index_rgb(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
void set_index_4byte(uint8_t index, uint32_t color);
void show(void);
void clear(void);
void terminate_spi(void);
#endif