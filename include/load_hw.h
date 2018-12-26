#ifndef __LOAD_HW_H__
#define __LOAD_HW_H__

#include <common.h>

extern snipsSkillConfig configList[CONFIG_NUM];

#define RESP_CORE_V2_PIN 66
#define RESP_CORE_V2_VAL 0

void switch_on_power(void);
#endif