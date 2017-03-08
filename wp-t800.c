#include <stdio.h>
#include <errno.h>
#define __USE_ISOC99
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int wp_t800_open(const char *device)
{
    int retval;

    retval = open(device, O_WRONLY);
    if (retval < 0) {
        fprintf(stderr, "Unable to open '%s': %s\n", device, strerror(errno));
        return -1;
    }

    return retval;
}

int wp_t800_close(int wp)
{
    close(wp);
    return 0;
}

static int wp_write(int wp, char *buffer, int len)
{
    return write(wp, buffer, len);
}

int wp_t800_reset(int wp)
{
    wp_write(wp, "\x1b\x40", 2);
    wp_write(wp, "\x1b\x02\x1b\x61\x01", 5);
    return 0;
}

int wp_t800_cut_paper(int wp, int partial)
{
    if (partial)
        return wp_write(wp, "\x1d\x56\x41\x00", 4);
    return wp_write(wp, "\x1d\x56\x00\x40", 4);
}

int wp_t800_bell(int wp)
{
    return wp_write(wp, "\x1d\x07\x01\x01\x01", 5);
}

int wp_t800_bitmap_line(int wp, char *data, int width)
{
    char ch;
    wp_write(wp, "\x1d\x76\x30\x00", 4);
    ch = width & 0xff;
    wp_write(wp, &ch, 1);
    ch = (width >> 8) & 0xff;
    wp_write(wp, &ch, 1);
    wp_write(wp, "\x01\x00", 2);
    wp_write(wp, data, width);
    return 0;
}

int wp_t800_flush(int wp, int line_feeds)
{
    char ch;
    wp_write(wp, "\x1b\x4a", 2);
    ch = line_feeds;
    wp_write(wp, &ch, 1);
    wp_write(wp, "\x1b\x03", 2); // what is this for?
    return 0;
}

static int read_word(FILE *fp, char *buffer, int max_len)
{
    int ch;
    char line[512];
    int written = 0;

    /* Skip past any leading whitespace */
    do {
        ch = fgetc(fp);
        if (ch < 0)
            return -1;
        if (isspace(ch))
            continue;
        if (ch == '#') {
            fgets(line, sizeof(line), fp);
            continue;
        }
        buffer[written++] = ch;
        break;
    } while (1);

    // have now read one non-whitespace char
    while (written < max_len) {
        ch = fgetc(fp);
        if (isspace(ch))
            break;
        buffer[written++] = ch;
    }

    buffer[written] = '\0'; // null terminate
    return written;
}

int wp_t800_print_pbm(int wp, const char *pbm_file)
{
    FILE *fp;
    char token[20];
    int width, height, byte_width, h, w;
    char *line_buffer = NULL;

    if ((fp = fopen(pbm_file, "r")) == NULL) {
        fprintf(stderr, "Unable to open '%s': %s\n", pbm_file, strerror(errno));
        return -1;
    }

    if (read_word(fp, token, sizeof(token)) < 0)
        goto err;
    if (strcmp(token, "P1") != 0)
        goto err;

    if (read_word(fp, token, sizeof(token)) < 0)
        goto err;
    width = atoi(token);
    if (read_word(fp, token, sizeof(token)) < 0)
        goto err;
    height = atoi(token);

    byte_width = (width + 7) / 8;
    //printf("PBM is %dx%d (%d bytes)\n", width, height, byte_width);
    line_buffer = malloc(byte_width);

    for (h = 0; h < height; h++) {
        memset(line_buffer, 0, byte_width);
        for (w = 0; w < width; w++) {
            if (read_word(fp, token, 1) < 0)
                goto err;
            if (token[0] == '1')
                line_buffer[w / 8] |= (1 << (7 - (w % 8)));
        }
        if (wp_t800_bitmap_line(wp, line_buffer, byte_width) < 0)
            goto err;
    }

    free(line_buffer);
    fclose(fp);
    return 0;

err:
    if (line_buffer)
        free(line_buffer);
    fprintf(stderr, "Unable to parse PBM file '%s'\n", pbm_file);
    fclose(fp);
    return -1;
}

#if 0
static void output_line(FILE *output, char *data, int width)
{
    int w;
    //fprintf(output, "\x1b*\x21");
    fprintf(output, "\x1d\x76\x30");
    fputc('\x00', output);
    fputc(width & 0xff, output);
    fputc((width >> 8) & 0xff, output);
    fputc('\x01', output);
    fputc('\x00', output);
    for (w = 0; w < width; w ++)
        fputc(data[w], output);
    //fprintf(output, "\x1bJ\x18");
}

int main(int argc, char *argv[])
{
    int width, height, h, colour, i;
    char *data;
    FILE *output = stdout;

    width = 512;
    height = 100;

    data = malloc(width * 3);

    fprintf(output, "\x1b\x02\x1b\x61\x01");

    output_line(output, data, width);
    for (h = 0; h < height; h++) {
        if (h % 2)
            memset(data, 0xff, width * 3);
        else
            memset(data, 0x0, width * 3);
        output_line(output, data, width);
    }
    fprintf(output, "\x1bJ\x18");

    fprintf(output, "\x1b\x03\x1d\x56\x41");
    fputc(0, output);

    return 0;
}
#endif
