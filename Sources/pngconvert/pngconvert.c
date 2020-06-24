//
//  pngconvert.c
//  
//
//  Created by Nikolay Kapustin on 15.06.2020.
//
// This file was manually imported from libpng library, because lib doesn't expose writePNG func
//
//

#include "pngconvert.h"

#define INCHES_PER_METER (100.0/2.54)
#define PNG_TYPE 0
#define PNG32_TYPE 1

static unsigned char gfg_color[4] = {0, 0, 0, 255};
static unsigned char gbg_color[4] = {255, 255, 255, 255};

static int size = 5;
static int margin = 5;
static int dpi = 72;

static void fillRow(unsigned char *row, int num, const unsigned char color[])
{
    int i;

    for(i = 0; i < num; i++) {
        memcpy(row, color, 4);
        row += 4;
    }
}

 int writePNG(const QRcode *qrcode, const char *outfile, unsigned char fg_color[4])
{
    if(fg_color == NULL) {
        fg_color = gfg_color;
    }
    static FILE *fp; // avoid clobbering by setjmp.
    int type = PNG32_TYPE;
    png_structp png_ptr;
    png_infop info_ptr;
    png_colorp palette = NULL;
    png_byte alpha_values[2];
    unsigned char *row, *p, *q;
    int x, y, xx, yy, bit;
    int realwidth;

    realwidth = (qrcode->width + margin * 2) * size;
    if(type == PNG_TYPE) {
        row = (unsigned char *)malloc((size_t)((realwidth + 7) / 8));
    } else if(type == PNG32_TYPE) {
        row = (unsigned char *)malloc((size_t)realwidth * 4);
    } else {
        fprintf(stderr, "Internal error.\n");
        exit(EXIT_FAILURE);
    }
    if(row == NULL) {
        fprintf(stderr, "Failed to allocate memory.\n");
        exit(EXIT_FAILURE);
    }

    if(outfile[0] == '-' && outfile[1] == '\0') {
        fp = stdout;
    } else {
        fp = fopen(outfile, "wb");
        if(fp == NULL) {
            fprintf(stderr, "Failed to create file: %s\n", outfile);
            perror(NULL);
            exit(EXIT_FAILURE);
        }
    }

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(png_ptr == NULL) {
        fprintf(stderr, "Failed to initialize PNG writer.\n");
        exit(EXIT_FAILURE);
    }

    info_ptr = png_create_info_struct(png_ptr);
    if(info_ptr == NULL) {
        fprintf(stderr, "Failed to initialize PNG write.\n");
        exit(EXIT_FAILURE);
    }

    if(setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fprintf(stderr, "Failed to write PNG image.\n");
        exit(EXIT_FAILURE);
    }

    if(type == PNG_TYPE) {
        palette = (png_colorp) malloc(sizeof(png_color) * 2);
        if(palette == NULL) {
            fprintf(stderr, "Failed to allocate memory.\n");
            exit(EXIT_FAILURE);
        }
        palette[0].red   = fg_color[0];
        palette[0].green = fg_color[1];
        palette[0].blue  = fg_color[2];
        palette[1].red   = gbg_color[0];
        palette[1].green = gbg_color[1];
        palette[1].blue  = gbg_color[2];
        alpha_values[0] = fg_color[3];
        alpha_values[1] = gbg_color[3];
        png_set_PLTE(png_ptr, info_ptr, palette, 2);
        png_set_tRNS(png_ptr, info_ptr, alpha_values, 2, NULL);
    }

    png_init_io(png_ptr, fp);
    if(type == PNG_TYPE) {
        png_set_IHDR(png_ptr, info_ptr,
                (unsigned int)realwidth, (unsigned int)realwidth,
                1,
                PNG_COLOR_TYPE_PALETTE,
                PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_DEFAULT,
                PNG_FILTER_TYPE_DEFAULT);
    } else {
        png_set_IHDR(png_ptr, info_ptr,
                (unsigned int)realwidth, (unsigned int)realwidth,
                8,
                PNG_COLOR_TYPE_RGB_ALPHA,
                PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_DEFAULT,
                PNG_FILTER_TYPE_DEFAULT);
    }
    png_set_pHYs(png_ptr, info_ptr,
            dpi * INCHES_PER_METER,
            dpi * INCHES_PER_METER,
            PNG_RESOLUTION_METER);
    png_write_info(png_ptr, info_ptr);

    if(type == PNG_TYPE) {
//    /* top margin */
        memset(row, 0xff, (size_t)((realwidth + 7) / 8));
        for(y = 0; y < margin * size; y++) {
            png_write_row(png_ptr, row);
        }

        /* data */
        p = qrcode->data;
        for(y = 0; y < qrcode->width; y++) {
            memset(row, 0xff, (size_t)((realwidth + 7) / 8));
            q = row;
            q += margin * size / 8;
            bit = 7 - (margin * size % 8);
            for(x = 0; x < qrcode->width; x++) {
                for(xx = 0; xx < size; xx++) {
                    *q ^= (*p & 1) << bit;
                    bit--;
                    if(bit < 0) {
                        q++;
                        bit = 7;
                    }
                }
                p++;
            }
            for(yy = 0; yy < size; yy++) {
                png_write_row(png_ptr, row);
            }
        }
        /* bottom margin */
        memset(row, 0xff, (size_t)((realwidth + 7) / 8));
        for(y = 0; y < margin * size; y++) {
            png_write_row(png_ptr, row);
        }
    } else {
    /* top margin */
        fillRow(row, realwidth, gbg_color);
        for(y = 0; y < margin * size; y++) {
            png_write_row(png_ptr, row);
        }

        /* data */
        p = qrcode->data;
        for(y = 0; y < qrcode->width; y++) {
            fillRow(row, realwidth, gbg_color);
            for(x = 0; x < qrcode->width; x++) {
                for(xx = 0; xx < size; xx++) {
                    if(*p & 1) {
                        memcpy(&row[((margin + x) * size + xx) * 4], fg_color, 4);
                    }
                }
                p++;
            }
            for(yy = 0; yy < size; yy++) {
                png_write_row(png_ptr, row);
            }
        }
        /* bottom margin */
        fillRow(row, realwidth, gbg_color);
        for(y = 0; y < margin * size; y++) {
            png_write_row(png_ptr, row);
        }
    }

    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);

    fclose(fp);
    free(row);
    free(palette);

    return 0;
}

////////////////////////////
// write into mem buffer

struct libpng_inmem_write_struct { /* This is from png.c */
  unsigned char * pngBfr;  /* destination memory */
  unsigned long pngSiz;  /* destination memory size (bytes) */
} p_io;

void wrtBgPng(png_structp pngWrtPtr, png_bytep data, png_size_t length) {
    struct libpng_inmem_write_struct * p = (struct libpng_inmem_write_struct *) png_get_io_ptr(pngWrtPtr);
    p->pngBfr = (unsigned char *) realloc(p->pngBfr, p->pngSiz + length); /* From png.c */
    //if (!p->pngBfr) freeExit_w_msg((char *) "The PNG write memory did not correctly allocate.");
    memmove(p->pngBfr + p->pngSiz, data, length);
    p->pngSiz += length;
}

struct libpng_inmem_write_struct* writePNGBuffer(const QRcode *qrcode) {
    png_structp pngWrtPtr; /* The pointer that points the PNG write structure */
    png_infop pngWrtInfoPtr = NULL; ; /* The pointer that points the PNG write information */
    //struct libpng_inmem_write_struct p_io; /* Holds the encoded PNG data */
    //FILE * fw; /* The file pointer of the test file that will be wrote. */

    p_io.pngBfr = NULL;
    p_io.pngSiz = 0;
    int realwidth;

    realwidth = (qrcode->width + margin * 2) * size;
    int imgWdth = realwidth;
    int imgHght = realwidth;

    pngWrtPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); /* write_ptr */
    //if (!pngWrtPtr) freeExit_w_msg((char *) "The PNG write memory did not correctly allocate.");
    pngWrtInfoPtr = png_create_info_struct(pngWrtPtr);
    //if (!pngWrtInfoPtr) freeExit_w_msg((char *) "The PNG write information memory did not correctly allocate.");
    png_set_IHDR(pngWrtPtr, pngWrtInfoPtr, imgWdth, imgHght, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_byte ** row_pointers = (png_byte **) png_malloc(pngWrtPtr, imgHght * sizeof(png_byte *));
    size_t bytesPerRow = imgWdth << 2; /* 4 Bytes per pixel */
    unsigned char * imgBfr = (unsigned char *) calloc(1, imgHght * bytesPerRow * sizeof(unsigned char));

    for (int rw = 0; rw < imgHght; rw++) {
        png_byte *rwPtr = row_pointers[rw] = (png_byte *) (imgBfr + rw * bytesPerRow);

        for (int pxl = 0, byt = 0; pxl < imgWdth; pxl++) { /* Write a black background */
            for (int clr = 0; clr < 3; clr++) rwPtr[byt++] = 0;
            rwPtr[byt++] = 0xff;
        }
    }
    p_io.pngBfr = (unsigned char *) malloc(4); /* Defines final PNG data location */
    p_io.pngSiz = 4;
    png_init_io(pngWrtPtr, (png_FILE_p) &p_io);
    png_set_rows(pngWrtPtr, pngWrtInfoPtr, &row_pointers[0]);
    png_set_write_fn(pngWrtPtr, &p_io, wrtBgPng, NULL);
    png_write_png(pngWrtPtr, pngWrtInfoPtr, PNG_TRANSFORM_IDENTITY, NULL);
    //fwrite(p_io.pngBfr + 4, 1, p_io.pngSiz, fw); /* Test file */
    //freeExit_w_msg((char *) "The exit was normal.");

    png_destroy_write_struct(&pngWrtPtr, &pngWrtInfoPtr);
    //free(p_io.pngBfr);
    return &p_io;
}



