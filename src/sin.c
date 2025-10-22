#include <popt.h>
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

int main(int argc, char *argv[])
{
    signal(SIGINT, sigHandler);
    signal(SIGTERM, sigHandler);

    nice(19);

    if (check_max_brightness())
        return 1;

    int daemonize = 0;

    struct poptOption optionsTable[] = {
        {"daemon", 'd', POPT_ARG_NONE, &daemonize, 0, "daemonize", NULL},
        POPT_AUTOHELP
            POPT_TABLEEND};

    poptContext optCon = poptGetContext(NULL, argc, (const char **)argv, optionsTable, 0);
    poptSetOtherOptionHelp(optCon, "[OPTION...] [<delay_ms 10-60000> <number_of_colors 1-5000> [<brightness 1-100>]]");

    char opt;
    while ((opt = poptGetNextOpt(optCon)) >= 0)
    {
        switch (opt)
        {
        case 'd':
            break;
        default:
            poptPrintUsage(optCon, stderr, 0);
            poptFreeContext(optCon);
            exit(1);
        }
    }

    if (opt < -1)
    {
        /* an error occurred during option processing */
        fprintf(stderr, "%s: %s\n",
                poptBadOption(optCon, POPT_BADOPTION_NOALIAS),
                poptStrerror(opt));
        return 1;
    }

    int color_count = 50;
    int delay_ms = 200;
    float brightness = 1.0;

    int pos = 0;
    char *arg;
    while ((arg = (char *)poptGetArg(optCon)) != NULL)
    {
        switch (pos)
        {
        case 0:
            delay_ms = max(10, min(60000, atoi(arg)));
            break;
        case 1:
            color_count = max(3, min(5000, atoi(arg)));
            break;
        case 2:
            brightness = max(0.01, min(1.0, atoi(arg) / 100.0));
            break;
        }
        pos++;
    }

    if (pos == 1 || pos > 3)
    {
        poptPrintUsage(optCon, stderr, 0);
        poptFreeContext(optCon);
        exit(1);
    }

    poptFreeContext(optCon);

    if (daemonize)
        daemon(1, 0);

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
