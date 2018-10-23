#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

typedef struct{
	uint8_t numLEDs;
	int     fd_spi;
	uint8_t *pixels;
	uint8_t brightness;
}APA102;

typedef struct{
	char key[20];
	char value[50];
}snipsSkillConfig;

enum {
	ON_IDLE = 0,
	ON_LISTEN,
	ON_SPEAK,
	TO_MUTE,
	TO_UNMUTE,
	ON_SUCCESS,
	ON_ERROR,
	ON_DISABLED
}curr_state;
#endif