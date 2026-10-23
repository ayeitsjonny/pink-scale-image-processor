/*
* processor.c
*
* Image processor for INF1102 project.
* Reads a PPM (P6) image from stdin, applies the pink_scale operation,
* and writes the result to stdout.
*
* Operation : pink_scale
* THRESHOLD_VAL : 162 (>= 128 and < 256, so double is used for all
* pixel values and calculations)
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "inf1102_config.h"

#define IMAGE_SIZE (INF1102_IMAGE_WIDTH * INF1102_IMAGE_HEIGHT * 3)


/*
* do_processing: applies pink_scale to the whole image.
*
* The images are passed by reference (as pointers), not by value:
* im_in - input image, read only (R,G,B bytes for each pixel)
* im_out - output image, written by this function
*
* For every pixel:
* new = 0.299*old_R + 0.587*old_G + 0.114*old_B
* new_R = new
* new_B = new
* new_G = new / 2
*/
static void do_processing(const uint8_t *im_in, uint8_t *im_out,
                        int width, int height)
{
    double in_r, in_g, in_b; /* input pixel values, as double */
    double grey; /* weighted combination of old R, G, B */
    double out_r, out_g, out_b; /* output pixel values, as double */

    /* loop from top to bottom */
    for (int y = 0; y < height; y++)
    {
        /* loop from left to right */
        for (int x = 0; x < width; x++)
        {
            int idx = (y * width + x) * 3; /* index of this pixel's R byte */

            /* cast each input byte to double */
            in_r = (double)im_in[idx]; /* R */
            in_g = (double)im_in[idx + 1]; /* G */
            in_b = (double)im_in[idx + 2]; /* B */

            /* combine the OLD pixel values */
            grey = 0.299 * in_r + 0.587 * in_g + 0.114 * in_b;

            /* guard against rounding pushing the value just past 255 */
            if (grey > 255.0) grey = 255.0;

            /* new R and B are the same; new G is half the intensity */
            out_r = grey;
            out_b = grey;
            out_g = grey / 2.0;

            /* cast back to uint8_t when writing the output */
            im_out[idx] = (uint8_t)out_r;
            im_out[idx + 1] = (uint8_t)out_g;
            im_out[idx + 2] = (uint8_t)out_b;
        }
    }
}





/* ==================== Main ==================== */

int main(void) 
{
    char magic[3];
    int width, height, maxval;
    //read the "magic" value that tells us what kind of image it is, and the size
    if (scanf("%2s %d %d %d", magic, &width, &height, &maxval) != 4) 
    {
    fprintf(stderr, "failed to read PPM header\n");
    return 1;
    }
    getchar(); // consume newline

    //request two memory spaces, one for the input image and one for the output image
    uint8_t *im_in = malloc(width * height * 3);
    uint8_t *im_out = malloc(width * height * 3);
    //end with an error if either memory space can't be allocated
    if (!im_in || !im_out) { perror("malloc"); return 1; }

    //read in the entire image (*3 is for the three colours, R, G & B)
    size_t nread = fread(im_in, 1, width * height * 3, stdin);
    //if the thing that got read in is the wrong size, stop with an error.
    if ((int)nread != width * height * 3) 
    {
        fprintf(stderr, "short read from stdin\n");
        return 1;
    }

    /****************************************************************************/ 
    //PROCESS THE IMAGE: apply the pink_scale operation (passing images by reference)
    do_processing(im_in, im_out, width, height);
    /****************************************************************************/

    fprintf(stderr, "Writing output...\n");

    //write the first line of the image (the type, ad the size)
    fprintf(stdout, "P6\n%d %d\n255\n", width, height);
    fflush(stdout);

    //write the entire output image
    size_t nw = fwrite(im_out, 1, width * height * 3, stdout);
    if ((int)nw != width * height * 3) 
    {
        fprintf(stderr, "short write to stdout\n");
        return 1;
    }


    //free up the memory we used
    free(im_in);
    free(im_out);
    return 0;
}
