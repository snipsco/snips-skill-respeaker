CC = gcc
CFLAGS = -Wextra -Wall -std=gnu99 -Iinclude -Wno-unused-parameter -Wno-unused-variable -Wno-duplicate-decl-specifier

MQTT_C_SOURCES = src/mqtt.c src/mqtt_pal.c
snipsLed = bin/snipsLed

BINDIR = bin

all: $(BINDIR) $(snipsLed)

bin/snips%: snips%.c $(MQTT_C_SOURCES)
	$(CC) $(CFLAGS) $^ -lpthread -o $@

$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -rf $(BINDIR)