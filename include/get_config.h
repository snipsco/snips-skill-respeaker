#include "snips_led.h"

#define 	CONFIG_NUM 		17

typedef struct{
    char key[20];
    char value[50];
}snipsSkillConfig;


int strkv(char *src, char *key, char *value);
void config(snipsSkillConfig *configVar, int configNum);
char *get_config_value(char *key, snipsSkillConfig *configVar);
int if_config_true(char *key, snipsSkillConfig *configVar, char *value);
void parse_hour_minute(char *raw_value, uint8_t *hour, uint8_t *minute);

