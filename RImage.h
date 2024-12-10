
#ifndef REPLOT_IMAGE
#define REPLOT_IMAGE

#include "RDefine.h"

#ifndef REPLOT_NO_CUTE
    #define REPLOT_USE_CUTE
    #ifndef CUTE_PNG_IMPLEMENTATION
    #define CUTE_PNG_IMPLEMENTATION
    #include "./others/cute_png.h"
    #endif
#endif


#ifdef REPLOT_USE_CUTE
    #define REPLOT_USE_IMAGE
#endif

#ifndef RTexture
	typedef unsigned char *RTexture;
#endif

#define __CHANNEL 4

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

// typedef long long llt;
// long long __get_time() {
//     long long tm = (long long)(clock() * 1000 / CLOCKS_PER_SEC);
//     return tm;
// }

void __invalid_type(char *path) {
    printf("REPLOT type not supported: %s\n", path);
}

#ifdef REPLOT_USE_IMAGE
RTexture rimage_read(char *path, int *w, int *h, int *c) {
    int _type = _rimage_path_type(path);
    RTexture txtr = NULL;
    // 
    #ifdef REPLOT_USE_CUTE
    if (_type == RC_IMAGE_TYPE_PNG) {
        cp_image_t img = cp_load_png(path);
        *w = img.w;
        *h = img.h;
        *c = __CHANNEL;
        int _w = *w;
        int _h = *h;
        int size = _w*_h*__CHANNEL*sizeof(unsigned char);
        txtr = (RTexture)malloc(size);
        for (int y = 0; y < _h; y++) {
            for (int x = 0; x < _w; x++) {
                int idx = y * _w + x;
                int _idx = idx * __CHANNEL;
                cp_pixel_t pix = img.pix[idx];
                txtr[_idx + 0] = pix.r;
                txtr[_idx + 1] = pix.g;
                txtr[_idx + 2] = pix.b;
                txtr[_idx + 3] = pix.a;
            } 
        }
        free(img.pix);
        CUTE_PNG_MEMSET(&img, 0, sizeof(img));
    } else {
        __invalid_type(path);
    }
    #endif
    //
    return txtr;
}
#endif

#ifdef REPLOT_USE_IMAGE
void rimage_write(char *path, RTexture *buffer, int w, int h, int c) {
    int _type = _rimage_path_type(path);
    RTexture txtr = *buffer;
    //
    #ifdef REPLOT_USE_CUTE
    if (_type == RC_IMAGE_TYPE_PNG) {
        cp_pixel_t* pixes = (cp_pixel_t*)malloc(sizeof(cp_pixel_t) * w * h);
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                int idx = y * w + x;
                int _idx = idx * 4;
                cp_pixel_t pix;
                pix.r = (uint8_t)(txtr[_idx + 0]);
                pix.g = (uint8_t)(txtr[_idx + 1]);
                pix.b = (uint8_t)(txtr[_idx + 2]);
                pix.a = (uint8_t)(txtr[_idx + 3]);
                pixes[idx] = pix;
            }
        }
        cp_image_t img =(cp_image_t){
            .w = w,
            .h = h,
            .pix = pixes,
        };
        int code = cp_save_png(path, &img);
    } else {
        __invalid_type(path);
    }
    #endif
    //
}
#endif

#ifdef REPLOT_USE_IMAGE
void rimage_free(RTexture *buffer) {
    free(*buffer);
}
#endif

#endif
