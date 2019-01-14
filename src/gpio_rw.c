#include "gpio_rw.h"
#include "verbose.h"

#include <sys/stat.h>
#include <sys/types.h>

int GPIO_export(int pin){
#define BUFFER_MAX 3
  	char buffer[BUFFER_MAX];
  	ssize_t bytes_written;
  	int fd;

  	fd = open("/sys/class/gpio/export", O_WRONLY);
  	if (-1 == fd) {
        verbose(V_NORMAL, stderr, BLUE"[%s]"NONE" Failed to open export for writing!", __FUNCTION__);
  		return(-1);
  	}

  	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
  	write(fd, buffer, bytes_written);
  	close(fd);
  	return(0);
}

int GPIO_unexport(int pin){
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (-1 == fd) {
        verbose(V_NORMAL, stderr, BLUE"[%s]"NONE" Failed to open unexport for writing!", __FUNCTION__);
		return(-1);
	}

	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}

int GPIO_direction(int pin, int dir){
  	static const char s_directions_str[]  = "in\0out";

 #define DIRECTION_MAX 35
  	char path[DIRECTION_MAX];
  	int fd;

  	snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
  	fd = open(path, O_WRONLY);
  	if (-1 == fd) {
        verbose(V_NORMAL, stderr, BLUE"[%s]"NONE" Failed to open gpio direction for writing!", __FUNCTION__);
  		return(-1);
  	}

  	if (-1 == write(fd, &s_directions_str[GPIO_IN == dir ? 0 : 3], GPIO_IN == dir ? 2 : 3)) {
        verbose(V_NORMAL, stderr, BLUE"[%s]"NONE" Failed to set direction!", __FUNCTION__);
  		return(-1);
  	}

  	close(fd);
  	return(0);
}

int GPIO_read(int pin){
#define VALUE_MAX 30
  	char path[VALUE_MAX];
  	char value_str[3];
  	int fd;

  	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
  	fd = open(path, O_RDONLY);
  	if (-1 == fd) {
        verbose(V_NORMAL, stderr, BLUE"[%s]"NONE" Failed to open gpio value for reading!", __FUNCTION__);
  		return(-1);
  	}

  	if (-1 == read(fd, value_str, 3)) {
        verbose(V_NORMAL, stderr, BLUE"[%s]"NONE" Failed to read value!", __FUNCTION__);
  		return(-1);
  	}

  	close(fd);

  	return(atoi(value_str));
}

int GPIO_write(int pin, int value){
  	static const char s_values_str[] = "01";

  	char path[VALUE_MAX];
  	int fd;

  	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
  	fd = open(path, O_WRONLY);
  	if (-1 == fd) {
        verbose(V_NORMAL, stderr, BLUE"[%s]"NONE" Failed to open gpio value for writing!", __FUNCTION__);
  		return(-1);
  	}

  	if (1 != write(fd, &s_values_str[GPIO_LOW == value ? 0 : 1], 1)) {
        verbose(V_NORMAL, stderr, BLUE"[%s]"NONE" Failed to write value!", __FUNCTION__);
  		return(-1);
  	}

  	close(fd);
  	return(0);
}