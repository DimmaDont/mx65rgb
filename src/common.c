#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int check_max_brightness()
{

#ifdef MOCK
    return 0;
#endif

    char *leds[] = {
        "/sys/class/leds/red:indicator/max_brightness",
        "/sys/class/leds/green:power/max_brightness",
        "/sys/class/leds/blue:indicator/max_brightness",
    };

    // Check the three leds max brightness
    for (int i = 0; i < 3; i++)
    {
        FILE *f = fopen(leds[i], "r");

        if (f == NULL)
        {
            char *err;
            asprintf(&err, "%s max_brightness", leds[i]);
            perror(err);
            free(err);
            return 1;
        }

        char *buffer = malloc(3);
        fread(buffer, 1, 3, f);
        fclose(f);

        bool good = strcmp("255", buffer) == 0;
#ifdef DEBUG
        printf("%s %s %s\n", leds[i], buffer, good ? "OK" : "NO");
#endif
        free(buffer);

        if (!good)
        {
            return 1;
        }
    }
    return 0;
}

void get_rgb_leds(FILE **led_files)
{
    char *leds[] = {
#ifdef MOCK
        "/dev/null",
        "/dev/null",
        "/dev/null",
#else
        "/sys/class/leds/red:indicator/brightness",
        "/sys/class/leds/green:power/brightness",
        "/sys/class/leds/blue:indicator/brightness",
#endif
    };

    led_files[0] = fopen(leds[0], "w");
    led_files[1] = fopen(leds[1], "w");
    led_files[2] = fopen(leds[2], "w");
}

void get_rgbwa_leds(FILE **led_files)
{
    char *leds[] = {
#ifdef MOCK
        "/dev/null",
        "/dev/null",
        "/dev/null",
        "/dev/null",
        "/dev/null",
#else
        "/sys/class/leds/red:indicator/brightness",
        "/sys/class/leds/green:power/brightness",
        "/sys/class/leds/blue:indicator/brightness",
        "/sys/class/leds/white:status/brightness",
        "/sys/class/leds/amber:fault/brightness",
#endif
    };

    led_files[0] = fopen(leds[0], "w");
    led_files[1] = fopen(leds[1], "w");
    led_files[2] = fopen(leds[2], "w");

    led_files[3] = fopen(leds[3], "w"); // max 1
    led_files[4] = fopen(leds[4], "w"); // max 1
}

void set_rgb(FILE **led_files, int values[])
{
#ifdef DEBUG
    printf("\x1b[38;2;");
    char *v[3];
#endif

    for (int j = 0; j < 3; j++)
    {
        char *value;
        asprintf(&value, "%d", values[j]);
        fwrite(value, 1, strlen(value), led_files[j]);
        fflush(led_files[j]);
#ifdef DEBUG
        if (j < 2)
            printf("%s;", value);
        else
            printf("%s", value);
        v[j] = value;
#else
        free(value);
#endif
    }

#ifdef DEBUG
    printf("m%3s %3s %3s\x1b[0m ", v[0], v[1], v[2]);
    fflush(stdout);
    free(v[0]);
    free(v[1]);
    free(v[2]);
#endif
}

void set_rgbwa(FILE **led_files, int values[])
{
    for (int j = 0; j < 5; j++)
    {
        char *value;
        asprintf(&value, "%d", values[j]);
        fwrite(value, 1, strlen(value), led_files[j]);
        fflush(led_files[j]);
        free(value);
    }
}

void set_rgbwa_char(FILE **led_files, char *values[])
{
    for (int j = 0; j < 5; j++)
    {
        fwrite(values[j], 1, strlen(values[j]), led_files[j]);
        fflush(led_files[j]);
    }
}
