CC = gcc

CFLAGS = 	-Ofast -std=gnu99 -Wall\
		 	-Iinclude\
			-fomit-frame-pointer \
			-I/opt/vc/include\
			-L/opt/vc/lib

SOURCES = src/mqtt.c src/mqtt_pal.c src/animation.c src/apa102.c src/get_config.c src/state_handler.c src/rsp_corev2.c

APP = action-app_respeaker

all: $(APP)

$(APP): main/app.c $(SOURCES)
	$(CC) $(CFLAGS) $^ -lpthread -o $@
