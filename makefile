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
          src/cAPA102.c\
          src/state_handler.c\
          src/gpio_rw.c\
          src/parse_opts.c\
          src/load_hw.c\
          src/load_sw.c\
          src/cJSON.c\
          src/mqtt_client.c\
          src/cCONFIG.c\
          src/verbose.c\
          src/button.c

APP = action-led_animation_1.6.0

all: $(APP)

$(APP): src/main.c $(SOURCES)
	$(CC) $(CFLAGS) $^ -lpthread -o $@

clean:
	rm action-*
