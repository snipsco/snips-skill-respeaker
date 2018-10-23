#ifndef __COMMON_H__
	#include <common.h>
#endif

#define STEP_COUNT 20

void *on_idle(void);
void *on_listen(void);
void *on_speak(void);
void *to_mute(void);
void *to_unmute(void);
void *on_disabled(void);
void delay_on_state(int ms, int state);