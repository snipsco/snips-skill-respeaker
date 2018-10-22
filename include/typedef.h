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
#endif