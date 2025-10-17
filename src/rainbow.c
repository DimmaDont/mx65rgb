#include <signal.h>
#include <unistd.h>

#include "common.c"

#define msleep(t) usleep(t * 1000)

#define COLOR_COUNT 36
#define DELAY_MS 125

int COLORS[COLOR_COUNT][3] = {
    {255, 0, 0},
    {255, 42, 0},
    {255, 84, 0},
    {255, 127, 0},
    {255, 170, 0},
    {255, 212, 0},
    {255, 255, 0},
    {212, 255, 0},
    {170, 255, 0},
    {127, 255, 0},
    {84, 255, 0},
    {42, 255, 0},
    {0, 255, 0},
    {0, 255, 42},
    {0, 255, 84},
    {0, 255, 127},
    {0, 255, 170},
    {0, 255, 212},
    {0, 255, 255},
    {0, 212, 255},
    {0, 170, 255},
    {0, 127, 255},
    {0, 84, 255},
    {0, 42, 255},
    {0, 0, 255},
    {42, 0, 255},
    {84, 0, 255},
    {127, 0, 255},
    {170, 0, 255},
    {212, 0, 255},
    {255, 0, 255},
    {255, 0, 212},
    {255, 0, 170},
    {255, 0, 127},
    {255, 0, 84},
    {255, 0, 42},
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

    // Initialize
    set_rgb(led_files, (int[]){0, 0, 0});

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
