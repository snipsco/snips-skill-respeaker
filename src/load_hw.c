#include "load_hw.h"
#include "cJSON.h"
#include "gpio_rw.h"
#include "apa102.h"
#include "verbose.h"

static HW_GPIO_SPEC hw_power_gpio = {-1, -1};

static int load_json_file(const char *_DIR, char *json_file_buffer);
static int load_power_pin(cJSON *spec);
static int load_led_num(cJSON *spec);

/**
 * @brief Load specification to system, including leds_number, power/button pin etc
 *
 * @param[in] model The model name that is supposed to be loaded
 *
 * @returns \-1 faild or \0 successful
 */
int load_hw_spec_json(const char *hw_model){
    char buffer[HW_SPEC_FILE_LEN];
    cJSON *hw_spec_model = NULL;

    if(-1 == load_json_file(HW_SPEC_FILE, buffer))
        return -1;

    cJSON *hw_spec_json = cJSON_Parse(buffer);
    if (hw_spec_json == NULL){
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
            verbose(V_NORMAL, stderr, "from parsing message : %s", error_ptr);
        return -1;
    }

    hw_spec_model = cJSON_GetObjectItemCaseSensitive(hw_spec_json, hw_model);

    if (-1 == load_led_num(hw_spec_model))
        return -1;
    if (-1 == load_power_pin(hw_spec_model))
        return -1;
    cJSON_Delete(hw_spec_json);
    return 0;
}

/**
 * @brief Load a json file
 *
 * @param[in] DIR directory of the target json file
 * @param[out] json_file_buffer A buffer for saving the json string
 *
 * @returns \-1 error or number of loaded bytes
 */
static int load_json_file(const char *_DIR, char *json_file_buffer){
    FILE * pFILE;
    char temp;
    int count = 0;
    pFILE = fopen(_DIR, "r");
    if (NULL == pFILE){
        verbose(V_NORMAL, stderr, BLUE"[%s]"NONE" Can not load "HW_SPEC_FILE" ", __FUNCTION__);
        return -1;
    }

    do{
        temp = fgetc(pFILE);
        if( feof(pFILE) )
            break ;

        if(temp >32 && temp < 126)
            json_file_buffer[count++] = temp;
    }while(1);

    fclose(pFILE);
    json_file_buffer[count] = '\0';
    return count;
}

static int load_led_num(cJSON *spec){
    const cJSON *led_num = cJSON_GetObjectItemCaseSensitive(spec, HW_LED_NUM);
    if (led_num != NULL && cJSON_IsNumber(led_num)) {
        if ( (led_num->valueint <= 255) && (led_num->valueint > 0) ){
            set_leds_number(led_num->valueint);
        }
        else{
            verbose(V_NORMAL, stderr, BLUE"[%s]"NONE" Invalide led number (0-255)", __FUNCTION__);
            return -1;
        }
    }else{
        verbose(VV_INFO, stdout, BLUE"[%s]"NONE" No led number specification found", __FUNCTION__);
        return -1;
    }
    return 0;
}

/**
 * @brief Load the power pin
 *
 * @returns \-1 error or \0 no power pin config \1 successful
 */
static int load_power_pin(cJSON *spec){
    cJSON *pin = NULL;
    cJSON *val = NULL;

    cJSON *power_spec = cJSON_GetObjectItemCaseSensitive(spec, HW_POWER);
    if (NULL == power_spec){
        verbose(VV_INFO, stdout, BLUE"[%s]"NONE" Model has no power pin", __FUNCTION__);
        return 0;
    }else{
        pin = cJSON_GetObjectItemCaseSensitive(power_spec, HW_GPIO_PIN);
        val = cJSON_GetObjectItemCaseSensitive(power_spec, HW_GPIO_VAL);
        if (!cJSON_IsNumber(pin) || !cJSON_IsNumber(val)){
            verbose(V_NORMAL, stderr, BLUE"[%s]"NONE" Model has invalide pin and val", __FUNCTION__);
            return -1;
        }

        if (pin->valueint > 0)
            hw_power_gpio.pin = pin->valueint;
        else{
            verbose(V_NORMAL, stderr, BLUE"[%s]"NONE" Model has invalide gpio_pin number", __FUNCTION__);
            return -1;
        }

        if (val->valueint == 0 || val->valueint == 1)
            hw_power_gpio.val = val->valueint;
        else{
            verbose(V_NORMAL, stderr, BLUE"[%s]"NONE" Model has invalide gpio_val number (0,1)", __FUNCTION__);
            return -1;
        }
        return 1;
    }
}

/**
 * @brief Set the power pin
 *
 * @returns \-1 error or \0 no power pin \1 successful
 */
int set_power_pin(void){
    if ( hw_power_gpio.val == -1 || hw_power_gpio.val == -1 ){
        verbose(VV_INFO, stdout, BLUE"[%s]"NONE" Mode has no power pin", __FUNCTION__);
        return 0;
    }

    if (-1 == GPIO_export(hw_power_gpio.pin))
        return -1;

    if (-1 == GPIO_direction(hw_power_gpio.pin, GPIO_OUT))
        return -1;

    if (-1 == GPIO_write(hw_power_gpio.pin, hw_power_gpio.val))
        return -1;

    verbose(VV_INFO, stdout, BLUE"[%s]"NONE" Set power pin %d to "LIGHT_GREEN"<%s>"NONE, __FUNCTION__, hw_power_gpio.pin, (hw_power_gpio.val)?"HIGH":"LOW");
    return 1;
}

/**
 * @brief Release the power pin
 *
 * @returns \-1 error or \0 no power pin \1 successful
 */
int reset_power_pin(void){
    if ( hw_power_gpio.val == -1 || hw_power_gpio.val == -1 ){
        verbose(VV_INFO, stdout, BLUE"[%s]"NONE" Mode has no power pin", __FUNCTION__);
        return 0;
    }

    if (-1 == GPIO_unexport(hw_power_gpio.pin))
        return -1;

    verbose(VV_INFO, stdout, BLUE"[%s]"NONE" Released power pin", __FUNCTION__);
    return 1;
}
