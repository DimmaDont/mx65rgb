#include <signal.h>
#include <stdbool.h>
#include <time.h>

#include "common.h"

#define COLOR_COUNT 12
#define DELAY_MS 1000

int COLORS[COLOR_COUNT][5] = {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0},
    {15, 255, 127, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0},
};

volatile sig_atomic_t sig_received = false;

void sigHandler(int sig_num)
{
    sig_received = true;
}

int main()
{
    signal(SIGINT, sigHandler);
    signal(SIGTERM, sigHandler);

    if (check_max_brightness())
        return 1;

    FILE *led_files[5];
    get_rgbwa_leds(led_files);

    struct timespec ts;
    ts.tv_sec = DELAY_MS / 1000;
    ts.tv_nsec = (DELAY_MS % 1000) * 1000000;

    while (!sig_received)
    {
        for (int i = 0; i < COLOR_COUNT; i++)
        {
            int *rgbwa = COLORS[i];
            set_rgbwa(led_files, rgbwa);
            if (nanosleep(&ts, NULL) == -1)
                break;
        }
#ifdef DEBUG
        printf("loop\n");
        fflush(stdout);
#endif
    }

    // Reset to green
    set_rgbwa(led_files, (int[]){0, 255, 0, 0, 0});

    fclose(led_files[0]);
    fclose(led_files[1]);
    fclose(led_files[2]);
    fclose(led_files[3]);
    fclose(led_files[4]);

    return 0;
}
