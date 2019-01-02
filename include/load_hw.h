#ifndef __LOAD_HW_H__
#define __LOAD_HW_H__

#include <common.h>

#define HW_SPEC_FILE_LEN 2048

#define HW_SPEC_FILE   "hw_spec.json"

#define HW_LED_NUM     "led_num"
#define HW_LED_SPI_BUS "spi_bus"
#define HW_LED_SPI_DEV "spi_dev"
#define HW_POWER       "power"
#define HW_BUTTON      "button"
#define HW_GPIO_PIN    "gpio_pin"
#define HW_GPIO_VAL    "gpio_val"

/**
 * @brief Load specification to system, including leds_number, power/button pin etc
 *
 * @param[in] model The model name that is supposed to be loaded
 *
 * @returns \-1 faild or \0 successful
 */
int load_hw_spec_json(const char *hw_model);

#endif