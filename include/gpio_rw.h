#ifndef __GPIO_RW_H__
#define __GPIO_RW_H__

#define GPIO_IN  0
#define GPIO_OUT 1

#define GPIO_LOW  0
#define GPIO_HIGH 1

#define BUFFER_MAX 3
#define VALUE_MAX 30
#define DIRECTION_MAX 35

/**
 * @brief: export a GPIO port
 *
 * @param[in] pin bcm number of the pin
 *
 * @returns \-1 failded or \0 successful
 */
int GPIO_export(int pin);

/**
 * @brief: unexport a GPIO port
 *
 * @param[in] pin bcm number of the pin
 *
 * @returns \-1 failded or \0 successful
 */
int GPIO_unexport(int pin);

/**
 * @brief: set direction
 *
 * @param[in] pin bcm number of the pin
 * @param[in] dir direction \0 In, \1 Out
 *
 * @returns \-1 failded or \0 successful
 */
int GPIO_direction(int pin, int dir);

/**
 * @brief: read a value from gpio pin
 *
 * @param[in] pin bcm number of the pin
 *
 * @returns input on this gpio port
 */
int GPIO_read(int pin);

/**
 * @brief: export a GPIO port
 *
 * @param[in] pin bcm number of the pin
 * @param[in] value \0 Low, \1 High
 *
 * @returns \-1 failded or \0 successful
 */
int GPIO_write(int pin, int value);

#endif

/* example of using */
// /*
//  * Enable GPIO pins
//  */
// if (-1 == GPIOExport(POUT) || -1 == GPIOExport(PIN))
//
// /*
//  * Set GPIO directions
//  */
// if (-1 == GPIODirection(POUT, OUT) || -1 == GPIODirection(PIN, IN))
//
//
// /*
//  * Write GPIO value
//  */
// if (-1 == GPIOWrite(POUT, repeat % 2))
//
// /*
//  * Read GPIO value
//  */
// printf("I'm reading %d in GPIO %d\n", GPIORead(PIN), PIN);
//
// /*
//  * Disable GPIO pins
//  */
// if (-1 == GPIOUnexport(POUT) || -1 == GPIOUnexport(PIN))