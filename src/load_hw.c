#include "load_hw.h"
#include "cJSON.h"
#include "gpio_rw.h"
#include "cAPA102.h"
#include "verbose.h"

extern SNIPS_RUN_PARA RUN_PARA;

static int get_cJOSN_int(cJSON *spec, const char *key_name){
    const cJSON *res = cJSON_GetObjectItemCaseSensitive(spec, key_name);
    if (res != NULL && cJSON_IsNumber(res)) {
        return res->valueint;
    }else{
        verbose(VV_INFO, stdout, BLUE"[%s]"NONE" No %s info found", __FUNCTION__, key_name);
        return -1;
    }
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
        verbose(VV_INFO, stdout, BLUE"[%s]"NONE" This model has no power pin", __FUNCTION__);
        return 0;
    }else{
        pin = cJSON_GetObjectItemCaseSensitive(power_spec, HW_GPIO_PIN);
        val = cJSON_GetObjectItemCaseSensitive(power_spec, HW_GPIO_VAL);
        if (!cJSON_IsNumber(pin) || !cJSON_IsNumber(val)){
            verbose(V_NORMAL, stderr, BLUE"[%s]"NONE" Model has invalide pin and val", __FUNCTION__);
            return -1;
        }

        if (pin->valueint > 0)
            RUN_PARA.power.pin = pin->valueint;
        else{
            verbose(V_NORMAL, stderr, BLUE"[%s]"NONE" Model has invalide gpio_pin number", __FUNCTION__);
            return -1;
        }

        if (val->valueint == 0 || val->valueint == 1)
            RUN_PARA.power.val = val->valueint;
        else{
            verbose(V_NORMAL, stderr, BLUE"[%s]"NONE" Model has invalide gpio_val number (0,1)", __FUNCTION__);
            return -1;
        }
        return 1;
    }
}

static int load_LEDs_spec(cJSON *spec){
    int number;
    int spi_bus;
    int spi_dev;

    number = get_cJOSN_int(spec, HW_LED_NUM);
    if (-1 == number)
        return -1;
    else if ( number > 255 ){
        verbose(V_NORMAL, stderr, BLUE"[%s]"NONE" Unvalid LED number (0,255)", __FUNCTION__);
        return -1;
    }
    spi_bus = get_cJOSN_int(spec, HW_LED_SPI_BUS);
    if (-1 == spi_bus)
        return -1;
    spi_dev = get_cJOSN_int(spec, HW_LED_SPI_DEV);
    if (-1 == spi_dev)
        return -1;

    RUN_PARA.LEDs.number = number;
    RUN_PARA.LEDs.spi_bus = spi_bus;
    RUN_PARA.LEDs.spi_dev = spi_dev;
    //cAPA102_Init(number, spi_bus, spi_dev, 31);
    return 0;
}

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
    if ( NULL == hw_spec_model){
        verbose(V_NORMAL, stderr, "No hardware specification found for model: %s", hw_model);
        return -1;
    }
    if (-1 == load_LEDs_spec(hw_spec_model))
        return -1;
    if (-1 == load_power_pin(hw_spec_model))
        return -1;

    cJSON_Delete(hw_spec_json);
    return 0;
}