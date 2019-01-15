#include "button.h"
#include "gpio_rw.h"
#include "verbose.h"

#include <sys/time.h>

extern SNIPS_RUN_PARA RUN_PARA;

static uint8_t flag_long_press = 0;
static uint8_t flag_short_press = 0;
static uint8_t flag_double_press = 0;
static uint8_t current_raw = 1;
static uint8_t last_raw = 1;
static time_t start_time = 0;
static int time_duration = 0;
/* for double press */
static uint8_t pressed = 0;
static long first_time_stamp = 0;
/* for long press */
static uint8_t long_press_returned = 0;

static pthread_t key_status_observer_daemon;

void(*short_press_callback)(void);
void(*double_press_callback)(void);
void(*long_press_callback)(void);

static int button_pin = -1;
static int button_val = -1;

static long get_time_ms(void){
    struct timeval temp;
    gettimeofday( &temp, NULL );
    long res = 1000 * temp.tv_sec + temp.tv_usec/1000;
    return res;
}

static void key_status_handler(void) {
    long curr_time = get_time_ms();
    uint8_t PATTERN_PRESS = (!button_val << 1) | button_val;
    uint8_t PATTERN_RELEASE = (button_val << 1) | !button_val;
    uint8_t PATTERN_HOLD = (button_val << 1) | button_val;

    time_duration = curr_time - start_time;
    uint8_t res = (last_raw << 1) | (current_raw);

    if (PATTERN_PRESS == res)
        start_time = curr_time;
    else if (PATTERN_RELEASE == res) {
        if ((long) time_duration < LONG_PRESS_SEC) {
            if (!pressed) {
                pressed = 1;
                first_time_stamp = get_time_ms();
            }else{
                flag_double_press = 1;
            }
        }
        time_duration = (long) 0;
        long_press_returned = 0;
    } else if (PATTERN_HOLD == res)
        if ((long) time_duration > LONG_PRESS_SEC && !long_press_returned) {
            flag_long_press = 1;
            long_press_returned = 1;
        }
    last_raw = current_raw;
}

/**
 * @brief: Read a GPIO key input
 */
static void key_scan(void) {
    current_raw = cGPIO_read(button_pin);
    if (button_val == current_raw) {
        usleep(20);
        current_raw = cGPIO_read(button_pin);
        if (button_val == current_raw) {
            key_status_handler();
        }
    } else {
        current_raw = !button_val;
        key_status_handler();
    }
}

static void * key_status_observer(void * mqtt_client) {
    verbose(VVV_DEBUG, stdout, BLUE "[%s]"
        NONE " is successful started!", __FUNCTION__);
    while (1) {
        usleep(10000U);
        key_scan();
        if (flag_long_press) {
            verbose(VVV_DEBUG, stdout, BLUE"[%s]"NONE" "PURPLE"LONG"NONE" press is detected", __FUNCTION__);
            if (ON_IDLE == RUN_PARA.curr_state || ON_DISABLED == RUN_PARA.curr_state)
                long_press_callback();
            flag_long_press = 0;
        }

        if (flag_short_press) {
            verbose(VVV_DEBUG, stdout, BLUE"[%s]"NONE" "PURPLE"SHORT"NONE" press is detected", __FUNCTION__);
            if (ON_IDLE == RUN_PARA.curr_state || ON_DISABLED == RUN_PARA.curr_state)
                short_press_callback();
            flag_short_press = 0;
            pressed = 0;
            first_time_stamp = 0;
        }

        if (flag_double_press) {
            verbose(VVV_DEBUG, stdout, BLUE"[%s]"NONE" "PURPLE"DOUBLE"NONE" press is detected", __FUNCTION__);
            if (ON_IDLE == RUN_PARA.curr_state || ON_DISABLED == RUN_PARA.curr_state)
                double_press_callback();
            flag_double_press = 0;
            pressed = 0;
            first_time_stamp = 0;
        }

        if (pressed && first_time_stamp) {
            long _time = get_time_ms();
            if ( _time - first_time_stamp > DOUBLE_PRESS_TIMEOUT) {
                flag_short_press = 1;
                first_time_stamp = 0;
                pressed = 1;
            }
        }
    }
    return NULL;
}

/**
 * @brief: Create a button from GPIO port
 *
 * @param[in] pin: GPIO port number
 * @param[in] lvl: trigger level
 * @param[in] short_callback: callback function for a short press detection
 * @param[in] long_callback: callback function for a long press detection
 *
 * @returns: -1/ Error
 *            0/ No button pin
 *            1/ Success
 */
int init_key(int pin, int lvl, void( * short_callback)(void), void( * double_callback)(void), void( * long_callback)(void)) {
    if (-1 == pin) {
        verbose(VV_INFO, stdout, BLUE "[%s]"
            NONE " Mode has no button", __FUNCTION__);
        return 0;
    }
    button_pin = pin;
    button_val = lvl;
    short_press_callback = short_callback;
    double_press_callback = double_callback;
    long_press_callback = long_callback;

    if (-1 == cGPIO_export(button_pin))
        return -1;

    sleep(1);

    if (-1 == cGPIO_direction(button_pin, GPIO_IN))
        return -1;

    if (pthread_create(&key_status_observer_daemon, NULL, key_status_observer, &key_status_observer)) {
        verbose(V_NORMAL, stderr, BLUE "[%s]"
            NONE " Failed to start client daemon", __FUNCTION__);
        return -1;
    }

    verbose(VVV_DEBUG, stdout, BLUE "[%s]"
        NONE " Button has been created on GPIO"
        GREEN "%d"
        NONE, __FUNCTION__, button_pin);
    return 0;
}

/**
 * @brief: Release the GPIO port which is used for the button and kill the refreshing process
 *
 * @returns: -1/ Error
 *            0/ Success
 */
int destroy_key(void) {
    if (-1 == button_pin)
        return 0;
    if (-1 == cGPIO_unexport(button_pin))
        return -1;
    if (key_status_observer_daemon)
        pthread_cancel(key_status_observer_daemon);
    verbose(VVV_DEBUG, stdout, BLUE "[%s]"
        NONE " Button has been released on GPIO"
        GREEN "%d"
        NONE, __FUNCTION__, button_pin);
    return 0;
}