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

#ifndef __cAPA102_H__
#define __cAPA102_H__
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define B_OFF_SET 1
#define G_OFF_SET 2
#define R_OFF_SET 3

#define BITRATE 32000000

/* Beware: you may need to change the formate here */
#define SPI_DEVICE "/dev/spidev%d.%d"

#define RETRY_TIMES 5
#define RETRY_GAP_SEC 10

typedef struct{
    uint32_t number;
    int      fd_spi;
    uint8_t  *pixels;
    uint8_t  brightness;
}cAPA102_LEDs;

/**
 * @brief: Initialise a set of apa102 LEDs
 *
 * @param[in] led_num: Number of leds (0-255)
 * @param[in] spi_bus: SPI bus number (0-255)
 * @param[in] spi_dev: SPI device number (0-255)
 * @param[in] brightness: Global brightness (0-31)
 *
 * @returns:  0\ Success
 *           -1\ Error
 */
int cAPA102_Init(uint32_t led_num, uint8_t spi_bus, uint8_t spi_dev, uint8_t brightness);

/**
 * @brief: Change the global brightness and fresh
 *
 * @param[in] brightness: New brightness value
 */
void cAPA102_Change_Brightness(uint8_t brightness);

/**
 * @brief: Get the brightness
 *
 * @return current brightness value (0-31)
 */
int cAPA102_Get_Brightness(void);

/**
 * @brief: Set color for a specific pixel by giving R, G and B value separately
 *
 * @param[in] index: Index of the target led (0-255)
 * @param[in] red: Intensity of red colour (0-255)
 * @param[in] green: Intensity of green colour (0-255)
 * @param[in] blue: Intensity of blue colour (0-255)
 */
void cAPA102_Set_Pixel_RGB(uint32_t index, uint8_t red, uint8_t green, uint8_t blue);

/**
 * @brief: Get colour form a specific pixel for R, G and B separately
 *
 * @param[in] index: Index of the target led (0-255)
 * @param[out] red: Intensity of red colour (0-255)
 * @param[out] green: Intensity of green colour (0-255)
 * @param[out] blue: Intensity of blue colour (0-255)
 */
void cAPA102_Get_Pixel_RGB(uint32_t index, uint8_t *red, uint8_t *green, uint8_t *blue);

/**
 * @brief: Set color for a specific pixel by using 4byte date
 *
 * @param[in] index: Index of the target led (0-255)
 * @param[in] red: Intensity of red colour (0-255)
 * @param[in] green: Intensity of green colour (0-255)
 * @param[in] blue: Intensity of blue colour (0-255)
 *
 * @example: cAPA102_Get_Pixel_RGB(1, 0xFF0000) sets the 1st LED to red colour
 */
void cAPA102_Set_Pixel_4byte(uint32_t index, uint32_t colour);

/**
 * @brief: Get colour form a specific pixel
 *
 * @param[in] index: Index of the target led (0-255)
 *
 * @returns: 32 bits colour data
 */
uint32_t cAPA102_Get_Pixel_4byte(uint32_t index);

/**
 * @brief: Clear all the pixels
 */
void cAPA102_Clear_All(void);

/**
 * @brief: Refresh display (After modifing pixel colour)
 */
void cAPA102_Refresh(void);

/**
 * @brief: Close SPI file, release memory
 */
void cAPA102_Close(void);
#endif