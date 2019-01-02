/*
    The MIT License

    Copyright (c) 2018 Coor Fang

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

/* cAPA102 */

#include "cAPA102.h"

static cAPA102_LEDs cAPA012_BUF = {0, -1, NULL, 31};

/**
 * @brief: Try to open the SPI device by given times
 *
 * @param[in] retry_times
 * @param[in] retry_gap_sec:
 *
 * @returns: int\ The file descriptor of SPI device or
 *            -1\ Error
 */
static int cAPA102_Try_Open_SPI_Dev(uint8_t retry_times, uint8_t retry_gap_sec, uint8_t spi_bus, uint8_t spi_dev);

/**
 * @brief Open the SPI device file
 *
 * @returns: int\ The file descriptor of SPI device or
 *            -1\ Error
 */
static int cAPA102_Open_SPI_Dev(uint8_t spi_bus, uint8_t spi_dev);

int cAPA102_Init(uint32_t led_num, uint8_t spi_bus, uint8_t spi_dev, uint8_t brightness){
    cAPA012_BUF.number = led_num;
    if (brightness > 31)
        cAPA012_BUF.brightness = 0xFF;
    else
        cAPA012_BUF.brightness = 0xE0 | (0x1F & brightness);
    cAPA012_BUF.pixels = (uint8_t *)malloc(cAPA012_BUF.number * 4);
    cAPA012_BUF.fd_spi = cAPA102_Try_Open_SPI_Dev(RETRY_TIMES, RETRY_GAP_SEC, spi_bus, spi_dev);
    if (cAPA012_BUF.fd_spi)
        return -1;
    cAPA102_Clear_All();
    return 0;
}

void cAPA102_Change_Brightness(uint8_t brightness){
    if (brightness > 31)
        cAPA012_BUF.brightness = 0xFF;
    else
        cAPA012_BUF.brightness = 0xE0 | (0x1F & brightness);
    cAPA102_Refresh();
}

int cAPA102_Get_Brightness(void){
    return cAPA012_BUF.brightness & 0x1F;
}

void cAPA102_Set_Pixel_RGB(uint32_t index, uint8_t red, uint8_t green, uint8_t blue){
    if (index < cAPA012_BUF.number) {
        uint8_t *ptr = &cAPA012_BUF.pixels[index * 4];
        ptr[R_OFF_SET] = red;
        ptr[G_OFF_SET] = green;
        ptr[B_OFF_SET] = blue;
    }
}

void cAPA102_Get_Pixel_RGB(uint32_t index, uint8_t *red, uint8_t *green, uint8_t *blue){
    if (index < cAPA012_BUF.number) {
        uint8_t *ptr = &cAPA012_BUF.pixels[index * 4];
        red = ptr + R_OFF_SET;
        green = ptr + G_OFF_SET;
        blue = ptr + B_OFF_SET;
    }
}

void cAPA102_Set_Pixel_4byte(uint32_t index, uint32_t colour){
    uint8_t  r, g, b;
    r = colour >> 16;
    g = colour >> 8;
    b = colour;
    cAPA102_Set_Pixel_RGB(index, r, g, b);
}

uint32_t cAPA102_Get_Pixel_4byte(uint32_t index){
    uint8_t r, g, b;
    uint32_t colour = 0;
    cAPA102_Get_Pixel_RGB(index, &r, &g, &b);
    r <<= 16;
    g <<= 8;
    colour = r | g | b;
    return colour;
}

void cAPA102_Clear_All(void){
    uint8_t *ptr;
    uint32_t i;
    for(ptr = cAPA012_BUF.pixels, i=0; i<cAPA012_BUF.number; i++, ptr += 4) {
        ptr[1] = 0x00;
        ptr[2] = 0x00;
        ptr[3] = 0x00;
    }
    cAPA102_Refresh();
}

void cAPA102_Refresh(void){
    int ret;
    uint32_t i;
    uint32_t buf_len = 4 + 4 * cAPA012_BUF.number + (cAPA012_BUF.number + 15) / 16 + 1;
    uint8_t *ptr, *qtr;
    uint8_t *tx = (uint8_t *)malloc(buf_len);

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .len = buf_len,
        .speed_hz = BITRATE,
        .bits_per_word = 8,
    };

    for (i = 0; i < 4; i++)
        *(tx + i) = 0x00;
    qtr = tx + 4;

    for(ptr = cAPA012_BUF.pixels, i=0; i<cAPA012_BUF.number; i++, ptr += 4, qtr += 4) {
        qtr[0] = cAPA012_BUF.brightness;
        qtr[1] = ptr[1];
        qtr[2] = ptr[2];
        qtr[3] = ptr[3];
    }

    for (i = cAPA012_BUF.number * 4 + 4; i < buf_len; i++)
        *(tx + i) = 0x01;

    ret = ioctl(cAPA012_BUF.fd_spi, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        fprintf(stdout, "[Error] can't send spi message\n");

    free(tx);
}

void cAPA102_Close(void){
    cAPA102_Clear_All();
    if (cAPA012_BUF.fd_spi != -1)
        close(cAPA012_BUF.fd_spi);
    if (cAPA012_BUF.pixels)
        free(cAPA012_BUF.pixels);
}

static int cAPA102_Try_Open_SPI_Dev(uint8_t retry_times, uint8_t retry_gap_sec, uint8_t spi_bus, uint8_t spi_dev){
    int i,res;
    i = 0;
    do{
        res = cAPA102_Open_SPI_Dev(spi_bus, spi_dev);
        if (-1 != res)
          return res;
        i++;
        if(i > retry_times)
          break;
        fprintf(stderr, "[Error] Failed to open SPI! Retry [%d] in %d seconds. \n", i, retry_gap_sec);
    	   sleep(retry_gap_sec);
    }while(res);
    return -1;
}

static int cAPA102_Open_SPI_Dev(uint8_t spi_bus, uint8_t spi_dev){
    char spi_file_buff[50];
    int fd_temp;
    sprintf(spi_file_buff, SPI_DEVICE, spi_bus, spi_dev);
    fd_temp = open(spi_file_buff, O_RDWR);
    if(-1 == fd_temp) {
        fprintf(stderr, "[Error] Can't open %s (try 'sudo')", spi_file_buff);
        return -1;
    }
    ioctl(fd_temp, SPI_IOC_WR_MODE, SPI_MODE_0 | SPI_NO_CS);
    ioctl(fd_temp, SPI_IOC_WR_MAX_SPEED_HZ, BITRATE);
    return fd_temp;
}