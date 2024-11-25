
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>


#ifndef REPLOT_STB_NO_INCLUDE
    #define REPLOT_STB_INCLUDED
    #ifndef STB_IMAGE_IMPLEMENTATION
        #define STB_IMAGE_IMPLEMENTATION
        #include "./others/stb_image.h"
    #endif
    #ifndef STB_IMAGE_WRITE_IMPLEMENTATION
        #define STB_IMAGE_WRITE_IMPLEMENTATION
        #include "./others/stb_image_write.h"
    #endif
#endif

#ifndef REPLOT_STB_NO_IMPLEMENT
    #define REPLOT_STB_IMPLEMENTED
#endif

#ifndef RTexture
	typedef unsigned char *RTexture;
#endif

#define RC_IMAGE_TYPE_PNG 1
#define RC_IMAGE_TYPE_JPG 2

int _rimage_path_type(char *_path) {
    char *path = strdup(_path);
    for(int i = 0; path[i]; i++) {
        path[i] = tolower(path[i]);
    }
    char *_end = strrchr(path, '.');
    int _type = 0;
    if (strcmp(_end, ".png") == 0) {
        _type = RC_IMAGE_TYPE_PNG;
    } else if (strcmp(_end, ".jpg") == 0) {
        _type = RC_IMAGE_TYPE_JPG;
    }
    free(path);
    return _type;
}

RTexture rimage_read(char *path, int *w, int *h, int *c);
void rimage_write(char *path, RTexture *buffer, int w, int h, int c);
void rimage_free(RTexture *buffer);

/////////////////////////////////////////////////////

#ifdef REPLOT_STB_IMPLEMENTED
RTexture rimage_read(char *path, int *w, int *h, int *c) {
    int _type = _rimage_path_type(path);
    RTexture txtr = NULL;
    if (_type == RC_IMAGE_TYPE_PNG || _type == RC_IMAGE_TYPE_JPG) {
        txtr = stbi_load(path, w, h, c, 4);
    } else {
        printf("file format not supported");
    }
    return txtr;
}
#endif

#ifdef REPLOT_STB_IMPLEMENTED
void rimage_write(char *path, RTexture *buffer, int w, int h, int c) {
    int _type = _rimage_path_type(path);
    if (_type == RC_IMAGE_TYPE_PNG) {
        stbi_write_png(path, w, h, c, *buffer, w*c);
    } else if (_type == RC_IMAGE_TYPE_JPG) {
        stbi_write_jpg(path, w, h, c, *buffer, 100);
    } else {
        printf("file format not supported");
    }
}
#endif

#ifdef REPLOT_STB_IMPLEMENTED
void rimage_free(RTexture *buffer) {
    // stbi_image_free(img);
}
#endif
