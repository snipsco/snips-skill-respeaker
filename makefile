CC = gcc

CFLAGS =  -std=gnu99\
          -Wall\
          -Iinclude\
          -fomit-frame-pointer \
          -I/opt/vc/include\
          -L/opt/vc/lib

SOURCES = src/mqtt.c\
          src/mqtt_pal.c\
          src/animation.c\
          src/apa102.c\
          src/get_config.c\
          src/state_handler.c\
          src/gpio_rw.c\
          src/parse_opts.c\
          src/load_hw.c\
          src/cJSON.c\
          src/mqtt_client.c

APP = action-app_respeaker

all: $(APP)

$(APP): src/main.c $(SOURCES)
	$(CC) $(CFLAGS) $^ -lpthread -o $@

clean:
	rm $(APP)
