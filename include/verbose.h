#ifndef __VERBOSE_H__
#define __VERBOSE_H__
#include "common.h"

#define NONE            "\033[m"
#define RED             "\033[1;32;31m"
#define LIGHT_RED       "\033[1;31m"
#define GREEN           "\033[1;32;32m"
#define LIGHT_GREEN     "\033[1;32m"
#define BLUE            "\033[1;32;34m"
#define LIGHT_BLUE      "\033[1;34m"
#define DARY_GRAY       "\033[1;30m"
#define CYAN            "\033[0;36m"
#define LIGHT_CYAN      "\033[1;36m"
#define PURPLE          "\033[0;35m"
#define LIGHT_PURPLE    "\033[1;35m"
#define BROWN           "\033[0;33m"
#define YELLOW          "\033[1;32;33m"
#define LIGHT_GRAY      "\033[0;37m"
#define WHITE           "\033[1;37m"

typedef enum{
    V_NORMAL = 0,
    VV_INFO,
    VVV_DEBUG
}VERBOSE;

int verbose(VERBOSE lvl, FILE *stream, const char * restrict format, ...);
void setVerbose(VERBOSE lvl);

#endif