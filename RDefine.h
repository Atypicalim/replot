
#ifndef REPPLOT_DEFINE_H
#define REPPLOT_DEFINE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

/////////////////////////////////////////////////////

#ifndef u8
typedef unsigned char u8;
#endif
#ifndef u16
typedef unsigned short u16;
#endif
#ifndef u32
typedef unsigned int u32;
#endif
#ifndef u64
typedef unsigned long u64;
#endif

#ifndef RByte
typedef unsigned char RByte;
#endif

#ifndef RPixel
typedef unsigned char RPixel[4];
#endif

#ifndef RTexture
typedef unsigned char *RTexture;
#endif

/////////////////////////////////////////////////////

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif


#ifndef DEG2RAD
#define DEG2RAD M_PI/180
#endif

/////////////////////////////////////////////////////

typedef struct RC_Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} RC_Color;

/////////////////////////////////////////////////////

#ifndef RColor
#define RColor RC_Color
#endif

#ifndef RCOLOR
#define RCOLOR(r, g, b, a) (RC_Color){r, g, b, a}
#endif

#ifndef RRect
typedef struct RRect { int x, y;  int w, h; } RRect;
#endif

#ifndef RRECT
#define RRECT(x, y, w, h) (RRect){x, y, w, h}
#endif

#ifndef RSize
typedef struct RSize { int w, h; } RSize;
#endif

#ifndef RSIZE
#define RSIZE(w, h) (RSize){w, h}
#endif

#ifndef RScale
typedef struct RScale { float x, y; } RScale;
#endif

#ifndef RSCALE
#define RSCALE(x, y) (RScale){w, h}
#endif

#ifndef RSkew
typedef struct RSkew { float x, y; } RSkew;
#endif

#ifndef RSKEW
#define RSKEW(x, y) (RSkew){x, y}
#endif

#ifndef RAnchor
typedef struct RAnchor { float x, y; } RAnchor;
#endif

#ifndef RANCHOR
#define RANCHOR(w, h) (RAnchor){w, h}
#endif

#ifndef RPoint
typedef struct RPoint { int x, y; } RPoint;
#endif

#ifndef RPOINT
#define RPOINT(x, y) (RPoint){x, y}
#endif

#ifndef RVector
#define RVector RPoint
#endif

#ifndef RVECTOR
#define RVECTOR(x, y) RPOINT(x, y)
#endif

#ifndef RArea
#define RArea RSize
#endif

#ifndef RAREA
#define RAREA(w, h) RSIZE(w, h)
#endif

/////////////////////////////////////////////////////

void RPOINT_ADD_TO(RPoint *to, RPoint *p) {
    to->x = to->x + p->x;
    to->y = to->y + p->y;
}

void RSIZE_ADD_TO(RSize *to, RSize *p) {
    to->w = to->w + p->w;
    to->h = to->h + p->h;
}

/////////////////////////////////////////////////////

int replot_math_round(float n) {
    return (int)(n + 0.5);
}

long long replot_time_now() {
    long long now = (long long)(clock() * 1000 / CLOCKS_PER_SEC);
	return now;
}

float replot_math_lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

double replot_math_angle(double x, double y) {
    double angleRadians = atan2(y, x);
    double angleDegrees = angleRadians * (180.0 / M_PI);
    if (angleDegrees < 0) {
        angleDegrees += 360.0;
    }
    return angleDegrees;
}

float replot_math_sin(int angle) {
	static float sinLUT[360] = {1};
	if (sinLUT[0]) {
		for (size_t i = 0; i < 360; i++) {
			sinLUT[i] = sin(i * DEG2RAD);
        }
	}
	while (angle < 0) {
        angle += 360;
    }	
	if (angle >= 360) {
		angle -= 360 * (u32)(angle / 360);
    }
	return sinLUT[angle];
}

float replot_math_cos(int angle) {
	static float cosLUT[360] = {0};
	if (cosLUT[0] == 0) {
		for (size_t i = 0; i < 360; i++) {
			cosLUT[i] = cos(i * DEG2RAD);
        }
	}
	while (angle < 0) {
        angle += 360;
    }
	if (angle >= 360) {
		angle -= 360 * (u32)(angle / 360);
    }
	return cosLUT[angle];
}

/////////////////////////////////////////////////////

#endif
