#include <stdio.h>
#include <string.h>

#include "common.c"

int main(int argc, char *argv[])
{
    FILE *led_files[5];
    get_rgbwa_leds(led_files);
    if (argc == 1)
    {
        fprintf(stderr, "Usage: %s <red 0-255> <green 0-255> <blue 0-255> [<white 0-1> <amber 0-1>]\n", argv[0]);
    }
    else if (argc == 4)
    {
        set_rgbwa_char(led_files, (char *[]){argv[1], argv[2], argv[3], "0", "0"});
    }
    else if (argc == 6)
    {
        set_rgbwa_char(led_files, (char *[]){argv[1], argv[2], argv[3], argv[4], argv[5]});
    }

    fclose(led_files[0]);
    fclose(led_files[1]);
    fclose(led_files[2]);
    fclose(led_files[3]);
    fclose(led_files[4]);

    return 0;
}
