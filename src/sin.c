#include <math.h>
#include <signal.h>
#include <unistd.h>

#include "common.c"

#define msleep(t) usleep(t * 1000)

#define DELAY_MS 250

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
    buf[0] = (int)floor(128 * (sin(i) + 1));
    buf[1] = (int)floor(128 * (sin(i + 2.0944) + 1)); // 2pi/3
    buf[2] = (int)floor(128 * (sin(i + 4.1888) + 1)); // 4pi/3
}

int main()
{
    signal(SIGINT, sigHandler);
    signal(SIGTERM, sigHandler);

#ifdef MOCK
    FILE *led_files[] = {
        fopen("/dev/null", "w"),
        fopen("/dev/null", "w"),
        fopen("/dev/null", "w"),
    };
#else
    if (check_max_brightness())
        return 1;

    char *leds[] = {
        "/sys/class/leds/red:indicator/brightness",
        "/sys/class/leds/green:power/brightness",
        "/sys/class/leds/blue:indicator/brightness",

    };

    FILE *led_files[] = {
        fopen(leds[0], "w"),
        fopen(leds[1], "w"),
        fopen(leds[2], "w"),
    };
#endif

    while (!sig_received)
    {
        // 2pi, 6.28 divided by 0.04 = 157 steps
        // subtract a step so it doesn't overlap
        for (float i = 0; i < 6.24; i += 0.04)
        {
            int rgb[3] = {0, 0, 0};
            rainbow(rgb, i);
            set_rgb(led_files, rgb);
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
