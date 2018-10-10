CC = gcc

CFLAGS = 	-Ofast -Wextra -std=gnu99 -Wall\
		 	-Iinclude\
			-Wno-unused-parameter\
			-Wno-unused-variable\
			-fomit-frame-pointer \
			-I/opt/vc/include\
			-L/opt/vc/lib

SOURCES = src/mqtt.c src/mqtt_pal.c src/animation.c src/apa102.c src/get_config.c

APP = action-skill_respeaker

all: $(APP)

$(APP): main/action-skill_respeaker_c.c $(SOURCES)
	$(CC) $(CFLAGS) $^ -lpthread -o $@
