#ifndef __COMMON_H__
	#include <common.h>
#endif

int read_key_value(char *src, char *key, char *value);
void read_config_file(snipsSkillConfig *configVar, int configNum);
char *get_config_value(const char *key, snipsSkillConfig *configVar);
int if_config_true(const char *key, snipsSkillConfig *configVar, char *value);
int parse_hour_minute(char *raw_value, uint8_t *hour, uint8_t *minute);