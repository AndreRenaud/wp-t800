#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "wp-t800.h"
#include "video_font.h"

/**
 * Prints the given string, but each 'pixel' of font becomes
 * 8 pixels wide & 4 high
 */
static int print_string(int wp, char *s)
{
    char buffer[200];
    int w, h;
    while (*s) {
        unsigned char *font = &video_fontdata[*s * VIDEO_FONT_HEIGHT];
        for (h = 0; h < VIDEO_FONT_HEIGHT; h++) {
            memset(buffer, 0, sizeof(buffer));
            for (w = 0; w < 8; w++)
                if (font[h] & (1 << w))
                    buffer[7 - w] = 0xff;
            wp_t800_bitmap_line(wp, buffer, 8);
            wp_t800_bitmap_line(wp, buffer, 8);
            wp_t800_bitmap_line(wp, buffer, 8);
            wp_t800_bitmap_line(wp, buffer, 8);
        }

        s++;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int wp, i;
    //char *data = malloc(512);

    wp = wp_t800_open("/dev/ttyUSB0");
    if (wp < 0)
        return EXIT_FAILURE;

    wp_t800_reset(wp);
    //wp_t800_bell(wp);
    
    wp_t800_print_pbm(wp, "gorilla.pbm");
    //wp_t800_print_pbm(wp, "logo_linux_mono.pbm");

    //print_string(wp, "Fnord");
#if 0
    for (i = 0; i < 10; i++) {
        memset(data, 0xff, 512);
        wp_t800_bitmap_line(wp, data, 512);
        memset(data, 0x00, 512);
        wp_t800_bitmap_line(wp, data, 512);
    }
#endif

    wp_t800_flush(wp, 0x58);
    wp_t800_flush(wp, 0x58);
    wp_t800_flush(wp, 0x58);
    wp_t800_cut_paper(wp, 0);
    wp_t800_close(wp);


    return  EXIT_SUCCESS;
}
