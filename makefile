CC = gcc
CFLAGS = -Ofast -Wextra -Wall -std=gnu99 -Iinclude\
			-Wno-unused-parameter\
			-Wno-unused-variable\
			-fomit-frame-pointer \
			-I/opt/vc/include -L/opt/vc/lib

SOURCES = src/mqtt.c src/mqtt_pal.c src/animation.c src/apa102.c src/get_config.c
action-skill_respeaker_c = bin/action-skill_respeaker_c

BINDIR = bin

all: $(BINDIR) $(action-skill_respeaker_c)

bin/action-skill_respeaker_c: action-skill_respeaker_c.c $(SOURCES)
	$(CC) $(CFLAGS) $^ -lpthread -o $@

$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -rf $(BINDIR)