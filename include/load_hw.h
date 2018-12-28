#ifndef __LOAD_HW_H__
#define __LOAD_HW_H__

#include <common.h>

#define HW_SPEC_FILE_LEN 2048
#define HW_SPEC_FILE "hw_spec.json"

#define HW_LED_NUM "led_num"
#define HW_POWER "power"
#define HW_BUTTON "button"
#define HW_GPIO_PIN "gpio_pin"
#define HW_GPIO_VAL "gpio_val"

extern snipsSkillConfig configList[CONFIG_NUM];

typedef struct{
    int pin;
    int val;
}HW_GPIO_SPEC;

/**
 * @brief Load specification to system, including leds_number, power/button pin etc
 *
 * @param[in] model The model name that is supposed to be loaded
 *
 * @returns \-1 faild or \0 successful
 */
int load_hw_spec_json(const char *hw_model);

/**
 * @brief Set the power pin
 *
 * @returns \-1 error or \0 no power pin \1 successful
 */
int set_power_pin(void);

/**
 * @brief Release the power pin
 *
 * @returns \-1 error or \0 no power pin \1 successful
 */
int reset_power_pin(void);

#endif
