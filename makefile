bin_PROGRAMS = snipsLed
snipsLed_SOURCES = mqtt.c mqtt_pal.c main.c
AM_CPPFLAGS = -I$(srcdir)/include
LDADD = $(srcdir)/output/libmpdclient.a
AM_CFLAGS = -Wextra -Wall -std=gnu99 -Wno-unused-parameter -Wno-unused-variable -Wno-duplicate-decl-specifier -lpthread
