#include <getopt.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "common.h"

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

volatile sig_atomic_t sig_received = false;

void sigHandler(int sig_num)
{
    sig_received = true;
}

void rainbow(int *buf, float i, float brightness)
{
    // sin(x) range -1 to 1
    // +1 to range 0 to 2
    // * 128 to range 0 to 256
    // floor() to 0 to 255
    buf[0] = (int)floor(128 * brightness * (1 + sin(i)));
    buf[1] = (int)floor(128 * brightness * (1 + sin(i + M_PI * 2 / 3)));
    buf[2] = (int)floor(128 * brightness * (1 + sin(i + M_PI * 4 / 3)));
}

static struct option long_options[] = {
    // {"verbose", no_argument, 0, 'v'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0},
};

void printHelp(char *argv0) {
    printf("Usage: %s [<delay ms> <number of colors>] [<brightness 0-100>]\n", argv0);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sigHandler);
    signal(SIGTERM, sigHandler);

    nice(19);

    if (check_max_brightness())
        return 1;


    int opt;
    while ((opt = getopt_long(argc, argv, "dh", long_options, NULL)) != -1)
    {
        switch (opt)
        {
        case 'h':
            printHelp(argv[0]);
            return 0;
        default:
            printHelp(argv[0]);
            return 1;
        }
    }

    int positionals = argc - optind;
    if (positionals == 1 || positionals > 3)
    {
        printHelp(argv[0]);
        return 1;
    }

    int color_count = 50;
    int delay_ms = 200;
    float brightness = 1.0;
    for (int i = optind; i < argc; i++)
    {
        switch (i - optind)
        {
        case 0:
            delay_ms = atoi(argv[i]);
            break;
        case 1:
            color_count = atoi(argv[i]);
            break;
        case 2:
            brightness = max(0, min(1.0, atoi(argv[i]) / 100.0));
            break;
        }
    }

    int COLORS[color_count][3];

    // period is 2pi
    float step = 2 * M_PI / color_count;

    for (int i = 0; i < color_count; i++)
    {
        int rgb[3] = {0, 0, 0};
        rainbow(rgb, (float)i * step, brightness);
        COLORS[i][0] = rgb[0];
        COLORS[i][1] = rgb[1];
        COLORS[i][2] = rgb[2];
    }

    FILE *led_files[3];
    get_rgb_leds(led_files);

    struct timespec ts;
    ts.tv_sec = delay_ms / 1000;
    ts.tv_nsec = (delay_ms % 1000) * 1000000;

    while (!sig_received)
    {
        for (int i = 0; i < color_count; i++)
        {
            set_rgb(led_files, COLORS[i]);
            if (nanosleep(&ts, NULL) == -1)
                break;
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
