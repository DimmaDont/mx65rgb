#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>

#include "common.h"

#define msleep(t) usleep(t * 1000)

#define COLOR_COUNT 156
#define DELAY_MS 80

int COLORS[COLOR_COUNT][3];

volatile sig_atomic_t sig_received = false;

void sigHandler(int sig_num)
{
    sig_received = true;
}

void rainbow(int *buf, float i)
{
    // sin(x) range -1 to 1
    // +1 to range 0 to 2
    // * 128 to range 0 to 256
    // floor() to 0 to 255
    buf[0] = (int)floor(128 * (1 + sin(i)));
    buf[1] = (int)floor(128 * (1 + sin(i + 2.0944))); // 2pi/3
    buf[2] = (int)floor(128 * (1 + sin(i + 4.1888))); // 4pi/3
}

int main()
{
    signal(SIGINT, sigHandler);
    signal(SIGTERM, sigHandler);

    if (check_max_brightness())
        return 1;

    // 2pi, 6.28 divided by 0.04 = 157 steps
    // subtract a step so it doesn't overlap
    for (int i = 0; i < COLOR_COUNT; i++)
    {
        int rgb[3] = {0, 0, 0};
        rainbow(rgb, (float)i * 0.04);
        COLORS[i][0] = rgb[0];
        COLORS[i][1] = rgb[1];
        COLORS[i][2] = rgb[2];
    }

    FILE *led_files[3];
    get_rgb_leds(led_files);

    while (!sig_received)
    {
        for (int i = 0; i < COLOR_COUNT; i++)
        {
            set_rgb(led_files, COLORS[i]);
            msleep(DELAY_MS);
        }
#ifdef DEBUG
        printf("loop\n");
        fflush(stdout);
#endif
    }

    // Reset to green
    set_rgb(led_files, (int[]){0, 255, 0});

    fclose(led_files[0]);
    fclose(led_files[1]);
    fclose(led_files[2]);

    return 0;
}
