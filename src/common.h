#include <stdio.h>

int check_max_brightness();

void get_rgb_leds(FILE **led_files);
void get_rgbwa_leds(FILE **led_files);

void set_rgb(FILE **led_files, int values[]);
void set_rgbwa(FILE **led_files, int values[]);
void set_rgbwa_char(FILE **led_files, char *values[]);
