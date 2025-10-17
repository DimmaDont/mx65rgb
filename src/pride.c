#include <signal.h>
#include <unistd.h>

#include "common.c"

#define msleep(t) usleep(t * 1000)

#define COLOR_COUNT 6
#define DELAY_MS 1000

// TODO use the amber LED for orange

int COLORS[COLOR_COUNT][3] = {
    {229, 0, 0},   // red
    {255, 127, 0}, // orange
    {255, 238, 0}, // yellow
    {2, 191, 33},  // green
    {0, 76, 255},  // blue
    {119, 0, 136}, // purple
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

    FILE *led_files[3];
    get_rgb_leds(led_files);

    while (!sig_received)
    {
        for (int i = 0; i < COLOR_COUNT; i++)
        {
            int *rgb = COLORS[i];
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
