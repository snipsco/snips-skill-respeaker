#include "cAPA102.h"
#include "state_handler.h"
#include "mqtt_client.h"
#include "load_hw.h"
#include "parse_opts.h"
#include "cCONFIG.h"
#include "verbose.h"
#include "load_sw.h"
#include "gpio_rw.h"

#include <mqtt.h>
#include <common.h>
#include <pthread.h>

SNIPS_RUN_PARA RUN_PARA = {
    /* Hardware */
    "",
    {-1, -1, -1},
    {-1, -1},
    {-1, -1},

    /* Brightness */
    31,
    /* MQTT connection */
    "localhost",
    "1883",
    "",
    "",
    /* SiteId */
    "default",
    /* Client ID */
    NULL,
    /* Animation thread */
    0, // NULL
    ON_IDLE,
    /* Colour */
    {GREEN_C, BLUE_C, PURPLE_C, YELLOW_C, GREEN_C},
    /* Sleep mode */
    0,
    0,
    0,
    0,
    /* Flags */
    0,
    1,
    0,
    /* Animation Enable */
    {1, 1, 1, 1, 1}
};

void interrupt_handler(int sig){
    RUN_PARA.if_terminate = 1;
}

int set_power_pin(void){
    if ( RUN_PARA.power.val == -1 || RUN_PARA.power.val == -1 ){
        verbose(VV_INFO, stdout, BLUE"[%s]"NONE" Mode has no power pin", __FUNCTION__);
        return 0;
    }

    if (-1 == GPIO_export(RUN_PARA.power.pin))
        return -1;

    if (-1 == GPIO_direction(RUN_PARA.power.pin, GPIO_OUT))
        return -1;

    if (-1 == GPIO_write(RUN_PARA.power.pin, RUN_PARA.power.val))
        return -1;

    verbose(VV_INFO, stdout, BLUE"[%s]"NONE" Set power pin %d to "LIGHT_GREEN"<%s>"NONE, __FUNCTION__, RUN_PARA.power.pin, (RUN_PARA.power.val)?"HIGH":"LOW");
    return 1;
}

int reset_power_pin(void){
    if ( RUN_PARA.power.val == -1 || RUN_PARA.power.val == -1 ){
        verbose(VV_INFO, stdout, BLUE"[%s]"NONE" Mode has no power pin", __FUNCTION__);
        return 0;
    }

    if (-1 == GPIO_unexport(RUN_PARA.power.pin))
        return -1;

    verbose(VV_INFO, stdout, BLUE"[%s]"NONE" Released power pin", __FUNCTION__);
    return 1;
}

void check_nightmode(void){
    time_t curr_time;
    struct tm *read_time = NULL;

    curr_time = time(NULL);
    read_time = localtime(&curr_time);

    if(read_time->tm_hour == RUN_PARA.sleep_hour &&
        read_time->tm_min == RUN_PARA.sleep_minute &&
        RUN_PARA.curr_state != ON_DISABLED){
        RUN_PARA.curr_state = ON_DISABLED;
        RUN_PARA.if_update = 1;
        verbose(VV_INFO, stdout, "Nightmode started");
    }
    if(read_time->tm_hour == RUN_PARA.weak_hour &&
        read_time->tm_min == RUN_PARA.weak_minute &&
        RUN_PARA.curr_state == ON_DISABLED){
        RUN_PARA.curr_state = ON_IDLE;
        RUN_PARA.if_update = 1;
        verbose(VV_INFO, stdout, "Nightmode terminated");
    }
}

void close_all(int status){
    reset_power_pin();
    terminate_mqtt_client();
    cAPA102_Close();
    pthread_cancel(RUN_PARA.curr_thread);
    exit(status);
}

int main(int argc, char *argv[]){
    setVerbose(VV_INFO);
    if ( -1 == load_sw_spec() )
        close_all(EXIT_FAILURE);
    parse_opts(argc, argv);
    signal(SIGINT, interrupt_handler);

    if ( -1 == load_hw_spec_json(RUN_PARA.hardware_model))
        close_all(EXIT_FAILURE);

    if( -1 == set_power_pin())
        close_all(EXIT_FAILURE);

    debug_run_para_dump();

    if (!start_mqtt_client(RUN_PARA.client_id,
                           RUN_PARA.mqtt_host,
                           RUN_PARA.mqtt_port,
                           RUN_PARA.mqtt_user,
                           RUN_PARA.mqtt_pass))
        close_all(EXIT_FAILURE);

    if ( -1 == cAPA102_Init(RUN_PARA.LEDs.number,
                            RUN_PARA.LEDs.spi_bus,
                            RUN_PARA.LEDs.spi_dev,
                            RUN_PARA.max_brightness))
        close_all(EXIT_FAILURE);

    dump_running_info();

    while(1){
        if (RUN_PARA.if_sleepmode)
            check_nightmode();

        if (RUN_PARA.if_update)
            state_machine_update();

        if (RUN_PARA.if_terminate)
            break;
        usleep(10000);
    }
    close_all(EXIT_SUCCESS);
    return 0;
}