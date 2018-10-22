#ifndef __SNIPS_LED_H__
	#include <snips_led.h>
#endif

#ifndef __TYPEDEF_H__
	#include <typedef.h>
#endif

#define STEP_COUNT 20

void *on_off(void);
void *on_idle(void);
void *on_listen(void);
void *on_think(void);
void *on_speak(void);
void *to_mute(void);
void *to_unmute(void);
void *on_error(void);
void *on_success(void);