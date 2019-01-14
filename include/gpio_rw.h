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
int cGPIO_export(int pin);

/**
 * @brief: unexport a GPIO port
 *
 * @param[in] pin bcm number of the pin
 *
 * @returns \-1 failded or \0 successful
 */
int cGPIO_unexport(int pin);

/**
 * @brief: set direction
 *
 * @param[in] pin bcm number of the pin
 * @param[in] dir direction \0 In, \1 Out
 *
 * @returns \-1 failded or \0 successful
 */
int cGPIO_direction(int pin, int dir);

/**
 * @brief: read a value from gpio pin
 *
 * @param[in] pin bcm number of the pin
 *
 * @returns input on this gpio port
 */
int cGPIO_read(int pin);

/**
 * @brief: export a GPIO port
 *
 * @param[in] pin bcm number of the pin
 * @param[in] value \0 Low, \1 High
 *
 * @returns \-1 failded or \0 successful
 */
int cGPIO_write(int pin, int value);

#endif