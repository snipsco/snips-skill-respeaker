#include "snips_led.h"

#define 	CONFIG_NUM 		10

typedef struct{
    char key[20];
    char value[50];
}snipsSkillConfig;


int strkv(char *src, char *key, char *value);
void config(char *configFilePath, snipsSkillConfig *configVar, int configNum);
char *get_default_config_value(char *key, snipsSkillConfig *configVar);

// Usage
//config("config.ini", configList, sizeof(configList)/sizeof(struct configItem));