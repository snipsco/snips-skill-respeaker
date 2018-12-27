#include "parse_opts.h"

void parse_opts(int argc, char *argv[]){
	while (1) {
		static const struct option lopts[] = {
			{ "help",       0, 0, 'H' },
            { "debug",      0, 0, 'd' },
            { "verbose",    0, 0, 'v' },
            { "version",    0, 0, 'V' },
            { "mqtt-host",  1, 0, 'h' },
            { "mqtt-port",  1, 0, 'p' },
            { "mqtt-user",  1, 0, 'P' },
            { "mqtt-pass",  1, 0, 'U' },
            { "led-n",      1, 0, 'N' },
            { "spi-dev",    1, 0, 'D' },
            { "site-id",    1, 0, 'S' },
			{ NULL,         0, 0,  0  },
		};
		int c;

		c = getopt_long(argc, argv, "HdvVh:p:U:P:N:D:S:",lopts, NULL);

		if (c == -1)
			break;

		switch (c) {
		case 'H':
			print_usage(argv[0]);
			break;
		case 'd':

			break;
		case 'v':

			break;
		case 'V':
			print_version();
			break;
		case 'h':

			break;
		case 'p':

			break;
		case 'U':

			break;
		case 'P':

			break;
		case 'N':
			
			break;
		case 'D':

			break;
		case 'S':

			break;
		default:
			print_usage(argv[0]);
			break;
		}
	}
}

static void print_usage(const char *prog){
    printf("Usage: %s <options> command\n", prog);
    puts("\n"
        "Global options: \n"
        "  -H, --help         this help\n"
        "  -d, --debug        debug mode\n"
        "  -v, --verbose      enable verbose mode\n"
        "  -V, --version      print version of this program\n");
        "\n"
        "MQTT connection options: \n"
        "  -h, --mqtt-host    hostname of mqtt bus (default localhost)\n"
        "  -p, --mqtt-port    port of mqtt bus (default 1883)\n"
        "  -P, --mqtt-pass    password of mqtt connection\n"
        "  -U, --mqtt-user    username of mqtt connection\n"
        "\n"
        "APA102 LED options: \n"
        "  -N, --led-n        number of leds\n"
        "  -D, --spi-dev      spi device to use (default /dev/spidev0.0)\n"
        "\n"
        "Snips device info: \n"
        "  -S, --site-id      siteId to use (default)\n"
    exit(1);
}

static void print_version(void){
    printf("snips-respeaker-skill version 0.5.1 \n");
    exit(1);
}