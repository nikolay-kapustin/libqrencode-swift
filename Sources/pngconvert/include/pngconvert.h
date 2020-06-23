//
//  pngconvert.h
//  
//
//  Created by Nikolay Kapustin on 19.06.2020.
//

#ifndef PNGCONVERT_H
#define PNGCONVERT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <png.h>

typedef struct {
    int version;         ///< version of the symbol
    int width;           ///< width of the symbol
    unsigned char *data; ///< symbol data
} QRcode;

int writePNG(const QRcode *qrcode, const char *outfile, unsigned char fg_color[4]);

#ifdef __cplusplus
}
#endif

#endif // PNGCONVERT_H

