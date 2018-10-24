#include "rsp_corev2.h"
#include "get_config.h"

void switch_on_power(void){
    int fd_gpio;
    char gpio_66[] = {'6','6'};
    char direction[] = {'o','u','t'};
    char value[] = {'0'};

    if(if_config_true(C_MODEL_STR, configList, "rsp_corev2")){
        // export gpio
        if((fd_gpio = open("/sys/class/gpio/export", O_RDWR))<0){
            fprintf(stderr, "[Error] Can't open gpio export, please manually echo '66' to this file\n");
            return;
        }

        if(write(fd_gpio, gpio_66, sizeof(gpio_66))>0){
            fprintf(stdout, "[Info] Exported GPIO66..\n");
            close(fd_gpio);
        }
        else{
            close(fd_gpio);
            fprintf(stdout, "[Info] Failed to GPIO66..\n");
            return;
        }
        // set direction
        if((fd_gpio = open("/sys/class/gpio/gpio66/direction", O_RDWR))<0){
            fprintf(stderr, "[Error] Can't open gpio export/gpio66/direction, please manually echo 'out' to this file\n");
            return;
        }
        
        if(write(fd_gpio, direction, sizeof(direction))>0){
            fprintf(stdout, "[Info] Set GPIO66 direction out..\n");
            close(fd_gpio);
        }
        else{
            close(fd_gpio);
            fprintf(stdout, "[Info] Failed to set GPIO66 direction， please manually echo 'out' to this file\n");
            return;
        }
        // enable by set to 0
        if((fd_gpio = open("/sys/class/gpio/gpio66/value", O_RDWR))<0){
            fprintf(stderr, "[Error] Can't open gpio export/gpio66/value, please manually echo '0' to this file\n");
            return;
        }
        
        if(write(fd_gpio, value, sizeof(value))>0){
            fprintf(stdout, "[Info] Set GPIO66 value 0..\n");
            close(fd_gpio);
        }
        else{
            close(fd_gpio);
            fprintf(stdout, "[Info] Failed to set GPIO66 value 0, please manually echo '0' to this file\n");
            return;
        }
    }
}