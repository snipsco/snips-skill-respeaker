#include "common.h"
#include "button.h"
#include "gpio_rw.h"
#include "verbose.h"

#include <pthread.h>
#include <time.h>

extern SNIPS_RUN_PARA RUN_PARA;

static uint8_t flag_long_press = 0;
static uint8_t flag_short_press = 0;
static uint8_t current_raw = 1;
static uint8_t last_raw = 1;
static time_t start_time = 0;
static int time_duration = 0;
static uint8_t long_press_returned = 0;

static pthread_t key_status_observer_daemon;

void (*short_press_callback)(void);
void (*long_press_callback)(void);

static int button_pin = -1;

static void Key_Status_Handler(void){
    time_t curr_time;
    time(&curr_time);

    time_duration = curr_time - start_time;
    uint8_t res = (last_raw << 1) | (current_raw);
    switch (res) {
        case 0b10:
            start_time = curr_time;
            break;
        case 0b01: // released
            if ((int)time_duration < LONG_PRESS_SEC)
                flag_short_press = 1;
            time_duration = (time_t) 0;
            long_press_returned = 0;
            break;
        case 0b00: // still pressing
            if ((int)time_duration > LONG_PRESS_SEC && !long_press_returned){
                flag_long_press = 1;
                long_press_returned = 1;
            }
            break;
        default:
            ;
    }
    last_raw = current_raw;
}

/**
 * @brief: Read a GPIO key input
 */
static void Key_Scan(void){
    current_raw = GPIO_read(button_pin);
    if ( !current_raw ){
        usleep(20);
        current_raw = GPIO_read(button_pin);
        if ( !current_raw ) {
            Key_Status_Handler();
        }
    }else{
        current_raw = 1;
        Key_Status_Handler();
    }
}

static void* Key_Status_Observer(void* mqtt_client){
    verbose(VVV_DEBUG, stdout, BLUE"[%s]"NONE" is successful started!", __FUNCTION__);
    while (1){
        usleep(100000U);
        Key_Scan();
        if ( flag_long_press ) {
            verbose(VVV_DEBUG, stdout, BLUE"[%s]"NONE" "PURPLE"LONG"NONE" press is detected", __FUNCTION__);
            if ( RUN_PARA.curr_state == ON_IDLE )
                long_press_callback();
            flag_long_press = 0;
        }

        if ( flag_short_press ) {
            verbose(VVV_DEBUG, stdout, BLUE"[%s]"NONE" "PURPLE"SHORT"NONE" press is detected", __FUNCTION__);
            if ( RUN_PARA.curr_state == ON_IDLE )
                short_press_callback();
            flag_short_press = 0;
        }
    }
    return NULL;
}

/**
 * @brief: Create a button from GPIO port
 *
 * @param[in] pin: GPIO port number
 * @param[in] short_callback: callback function for a short press detection
 * @param[in] long_callback: callback function for a long press detection
 *
 * @returns: -1/ Error
 *            0/ No button pin
 *            1/ Success
 */
int Init_Key(int pin, void (*short_callback)(void), void (*long_callback)(void) ){
    if ( pin == -1 ){
        verbose(VV_INFO, stdout, BLUE"[%s]"NONE" Mode has no button", __FUNCTION__);
        return 0;
    }
    button_pin = pin;

    short_press_callback = short_callback;
    long_press_callback = long_callback;

    if ( -1 == GPIO_export(button_pin) )
		return -1;

    sleep(1);

    if ( -1 == GPIO_direction(button_pin, GPIO_IN) )
        return -1;

    if(pthread_create(&key_status_observer_daemon, NULL, Key_Status_Observer, &Key_Status_Observer)) {
        verbose(V_NORMAL, stderr, BLUE"[%s]"NONE" Failed to start client daemon", __FUNCTION__);
        return -1;
    }

    verbose(VVV_DEBUG, stdout, BLUE"[%s]"NONE" Button has been created on GPIO"GREEN"%d"NONE, __FUNCTION__, button_pin);
    return 0;
}

/**
 * @brief: Release the GPIO port which is used for the button and kill the refreshing process
 *
 * @returns: -1/ Error
 *            0/ Success
 */
int Destroy_Key(void){
    if ( -1 == button_pin)
        return 0;
    if ( -1 == GPIO_unexport(button_pin) )
	      return -1;
    if (key_status_observer_daemon)
        pthread_cancel(key_status_observer_daemon);
    verbose(VVV_DEBUG, stdout, BLUE"[%s]"NONE" Button has been released on GPIO"GREEN"%d"NONE, __FUNCTION__, button_pin);
    return 0;
}