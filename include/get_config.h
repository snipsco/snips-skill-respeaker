#ifndef __SNIPS_LED_H__
    #include <snips_led.h>
#endif

#ifndef __TYPEDEF_H__
    #include <typedef.h>
#endif

#define 	CONFIG_NUM 		18

int strkv(char *src, char *key, char *value);
void config(snipsSkillConfig *configVar, int configNum);
char *get_config_value(char *key, snipsSkillConfig *configVar);
int if_config_true(char *key, snipsSkillConfig *configVar, char *value);
void parse_hour_minute(char *raw_value, uint8_t *hour, uint8_t *minute);