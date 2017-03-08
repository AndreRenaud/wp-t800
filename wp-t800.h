#ifndef WP_T800_H
#define WP_T800_H

int wp_t800_open(const char *device);
int wp_t800_close(int wp);
int wp_t800_reset(int wp);

int wp_t800_cut_paper(int wp, int partial);
int wp_t800_bell(int wp);
int wp_t800_flush(int wp, int line_feeds);

/**
 * Draws the binary line of data from 'data', which is width bytes long
 * (ie: width * 8 pixels)
 */
int wp_t800_bitmap_line(int wp, char *data, int width);

/**
 * Prints a pbm file to the printer
 * See http://netpbm.sourceforge.net/doc/pbm.html &
 * http://en.wikipedia.org/wiki/Portable_bitmap_format
 * for details on the format
 */
int wp_t800_print_pbm(int wp, const char *pbm_file);

#endif
