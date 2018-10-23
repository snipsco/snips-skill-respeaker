#include "apa102.h"
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

extern APA102 leds;

int apa102_spi_setup(void){
    int i,res;
    leds.pixels = (uint8_t *)malloc(leds.numLEDs * 4);
    i = 0;
    do{
        res = begin();
        if (!res)
            return 1;
        i++;
        if(i > 4)
            break;
    }while(res);
    printf("[Error] Failed to start SPI!\n");
    return 0;
}

int begin(void){
    if((leds.fd_spi = open("/dev/spidev0.0", O_RDWR)) < 0) {
        printf("[Error] Can't open /dev/spidev0.0 (try 'sudo')");
        return -1;
    }
    ioctl(leds.fd_spi, SPI_IOC_WR_MODE, SPI_MODE_0 | SPI_NO_CS);
    ioctl(leds.fd_spi, SPI_IOC_WR_MAX_SPEED_HZ, BITRATE);
    return 0;
}

void set_index_rgb(uint8_t index, uint8_t r, uint8_t g, uint8_t b){
    if(index < leds.numLEDs && index >= 0) {
        uint8_t *ptr = &leds.pixels[index * 4];
        ptr[ROFFSET] = r;
        ptr[GOFFSET] = g;
        ptr[BOFFSET] = b;
    }
}

void set_index_4byte(uint8_t index, uint32_t color){
	uint8_t  r, g, b;
    r = color >> 16;
    g = color >>  8;
    b = color;

    if(index < leds.numLEDs && index >= 0) {
        uint8_t *ptr = &leds.pixels[index * 4];
        ptr[ROFFSET] = r;
        ptr[GOFFSET] = g;
        ptr[BOFFSET] = b;
    }
}

void show(void){ // global bri range: 0 - 31
    if (leds.fd_spi < 0) return;

    uint32_t    i;
    uint8_t     *ptr    =   leds.pixels;

    uint8_t x[4];
    // Start Signal: 32 bits 0
    x[0] = 0x00;
    i    = 4;
    while(i--) (void)write(leds.fd_spi, x, 1);

    // Payload:
    //x[0] = 0xFF & brightness;
    x[0]=0xFF;
    for(i=0; i<leds.numLEDs; i++, ptr += 4) {

        x[1] = ptr[1];
        x[2] = ptr[2];
        x[3] = ptr[3];

        (void)write(leds.fd_spi, x, 4);
    }

    // Stop Signal:
    x[0] = 0x01;
    //i = (numLEDs + 15) / 16;
    i = 32;
    while(i--) (void)write(leds.fd_spi, x, 1);
}

void clear(void){
    uint8_t *ptr;
    uint32_t i;
    for(ptr = leds.pixels, i=0; i<leds.numLEDs; i++, ptr += 4) {
        ptr[1] = 0x00; ptr[2] = 0x00; ptr[3] = 0x00;
    }
    show();
}