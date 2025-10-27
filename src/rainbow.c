#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include "common.h"

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

static struct option long_options[] = {
    // {"verbose", no_argument, 0, 'v'},
    {"daemonize", no_argument, NULL, 'd'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0},
};

void printHelp(char *argv0)
{
    printf("Usage: %s [-d|--daemonize]\n", argv0);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sigHandler);
    signal(SIGTERM, sigHandler);

    nice(19);

    if (check_max_brightness())
        return 1;

    int daemonize = 0;

    int opt;
    while ((opt = getopt_long(argc, argv, "dh", long_options, NULL)) != -1)
    {
        switch (opt)
        {
        case 'd':
            daemonize = 1;
            break;
        case 'h':
            printHelp(argv[0]);
            return 0;
        default:
            printHelp(argv[0]);
            return 1;
        }
    }

    if (daemonize)
        daemon(1, 0);

    FILE *led_files[3];
    get_rgb_leds(led_files);

    struct timespec ts;
    ts.tv_sec = DELAY_MS / 1000;
    ts.tv_nsec = (DELAY_MS % 1000) * 1000000;

    while (!sig_received)
    {
        for (int i = 0; i < COLOR_COUNT; i++)
        {
            int *rgb = COLORS[i];
            set_rgb(led_files, rgb);
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
