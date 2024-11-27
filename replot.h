// replot

#ifndef REPLOT_HEADER_H
#define REPLOT_HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>

#ifndef u8
typedef unsigned char u8;
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

#ifndef RSOFT_IMPLEMENTATION
#define RSOFT_IMPLEMENTATION
#include "RSoft.h"
#endif

#ifndef RIMAGE_IMPLEMENTATION
#define RIMAGE_IMPLEMENTATION
#include "RImage.h"
#endif

/////////////////////////////////////////////////////

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

/////////////////////////////////////////////////////

#define _REPLOT_FONT_HEIGHT 7 
#define _REPLOT_FONT_WIDTH 5 
static char _REPLOT_FONT_GLYPHS[128][_REPLOT_FONT_HEIGHT] = {
    ['a'] = {
        0b00000,
        0b01100,
        0b00010,
        0b01110,
        0b10010,
        0b01110,
        0b00000,
    },
    ['b'] = {
        0b10000,
        0b11100,
        0b10010,
        0b10010,
        0b10010,
        0b11100,
        0b00000,
    },
    ['c'] = {
        0b00000,
        0b01100,
        0b10010,
        0b10000,
        0b10010,
        0b01100,
        0b00000,
    },
    ['d'] = {
        0b00010,
        0b01110,
        0b10010,
        0b10010,
        0b10010,
        0b01110,
        0b00000,
    },
    ['e'] = {
        0b00000,
        0b01100,
        0b10010,
        0b11110,
        0b10000,
        0b01110,
        0b00000,
    },
    ['f'] = {
        0b00110,
        0b01000,
        0b11110,
        0b01000,
        0b01000,
        0b01000,
        0b00000,
    },
    ['g'] = {
        0b00000,
        0b01110,
        0b10010,
        0b10010,
        0b01110,
        0b00010,
        0b01100,
    },
    ['h'] = {
        0b10000,
        0b11100,
        0b10010,
        0b10010,
        0b10010,
        0b10010,
        0b00000,
    },
    ['i'] = {
        0b00100,
        0b00000,
        0b01100,
        0b00100,
        0b00100,
        0b01110,
        0b00000,
    },
    ['j'] = {
        0b00100,
        0b00000,
        0b11100,
        0b00100,
        0b00100,
        0b00100,
        0b11000,
    },
    ['k'] = {
        0b10000,
        0b10010,
        0b10100,
        0b11000,
        0b10100,
        0b10010,
        0b00000,
    },
    ['l'] = {
        0b01100,
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b01110,
        0b00000,
    },
    ['m'] = {
        0b00000,
        0b11010,
        0b10101,
        0b10101,
        0b10101,
        0b10101,
        0b00000,
    },
    ['n'] = {
        0b00000,
        0b11100,
        0b10010,
        0b10010,
        0b10010,
        0b10010,
        0b00000,
    },
    ['o'] = {
        0b00000,
        0b01100,
        0b10010,
        0b10010,
        0b10010,
        0b01100,
        0b00000,
    },
    ['p'] = {
        0b00000,
        0b11100,
        0b10010,
        0b10010,
        0b10010,
        0b11100,
        0b10000,
    },
    ['q'] = {
        0b00000,
        0b01110,
        0b10010,
        0b10010,
        0b10010,
        0b01110,
        0b00010,
    },
    ['r'] = {
        0b00000,
        0b10110,
        0b11000,
        0b10000,
        0b10000,
        0b10000,
        0b00000,
    },
    ['s'] = {
        0b00000,
        0b01110,
        0b10000,
        0b11110,
        0b00010,
        0b11100,
        0b00000,
    },
    ['t'] = {
        0b01000,
        0b01000,
        0b11110,
        0b01000,
        0b01000,
        0b01110,
        0b00000,
    },
    ['u'] = {
        0b00000,
        0b10010,
        0b10010,
        0b10010,
        0b10010,
        0b01110,
        0b00000,
    },
    ['v'] = {
        0b00000,
        0b10010,
        0b10010,
        0b10010,
        0b10010,
        0b01100,
        0b00000,
    },
    ['w'] = {
        0b00000,
        0b10001,
        0b10101,
        0b10101,
        0b10101,
        0b01010,
        0b00000,
    },
    ['x'] = {
        0b00000,
        0b01010,
        0b01010,
        0b00100,
        0b01010,
        0b01010,
        0b00000,
    },
    ['y'] = {
        0b00000,
        0b01010,
        0b01010,
        0b00100,
        0b00100,
        0b00100,
        0b00000,
    },
    ['z'] = {
        0b00000,
        0b11110,
        0b00010,
        0b01100,
        0b10000,
        0b11110,
        0b00000,
    },
    ['0'] = {
        0b01100,
        0b10010,
        0b10010,
        0b10010,
        0b10010,
        0b01100,
        0b00000,
    },
    ['1'] = {
        0b00100,
        0b01100,
        0b00100,
        0b00100,
        0b00100,
        0b01110,
        0b00000,
    },
    ['2'] = {
        0b01100,
        0b10010,
        0b00010,
        0b01100,
        0b10000,
        0b11110,
        0b00000,
    },
    ['3'] = {
        0b01100,
        0b10010,
        0b00100,
        0b00010,
        0b10010,
        0b01100,
        0b00000,
    },
    ['4'] = {
        0b00100,
        0b01100,
        0b10100,
        0b10100,
        0b11110,
        0b00100,
        0b00000,
    },
    ['5'] = {
        0b11110,
        0b10000,
        0b11100,
        0b00010,
        0b10010,
        0b01100,
        0b00000,
    },
    ['6'] = {
        0b01110,
        0b10000,
        0b11100,
        0b10010,
        0b10010,
        0b01100,
        0b00000,
    },
    ['7'] = {
        0b11110,
        0b00010,
        0b00100,
        0b01000,
        0b01000,
        0b01000,
        0b00000,
    },
    ['8'] = {
        0b01100,
        0b10010,
        0b01100,
        0b10010,
        0b10010,
        0b01100,
        0b00000,

    },
    ['9'] = {
        0b01100,
        0b10010,
        0b10010,
        0b01110,
        0b00010,
        0b01100,
        0b00000,
    },
    ['+'] = {
        0b00000,
        0b00000,
        0b00100,
        0b01110,
        0b00100,
        0b00000,
        0b00000,
    },
    ['-'] = {
        0b00000,
        0b00000,
        0b00000,
        0b01110,
        0b00000,
        0b00000,
        0b00000,
    },
    ['*'] = {
        0b00000,
        0b00000,
        0b01010,
        0b00100,
        0b01010,
        0b00000,
        0b00000,
    },
    ['/'] = {
        0b00000,
        0b00100,
        0b00000,
        0b01110,
        0b00000,
        0b00100,
        0b00000,
    },
    ['='] = {
        0b00000,
        0b00000,
        0b01110,
        0b00000,
        0b01110,
        0b00000,
        0b00000,
    },
    [','] = {
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00100,
        0b00100,
        0b01000,
    },
    ['.'] = {
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00100,
        0b00100,
        0b00000,
    },
    ['?'] = {
        0b01100,
        0b00010,
        0b00010,
        0b01100,
        0b00000,
        0b01000,
        0b00000,
    },
    ['!'] = {
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b00000,
        0b00100,
        0b00000,
    },
    ['%'] = {
        0b01010,
        0b00010,
        0b00100,
        0b00100,
        0b01000,
        0b01010,
        0b00000,
    },
    ['$'] = {
        0b00100,
        0b01111,
        0b10100,
        0b01110,
        0b00101,
        0b11110,
        0b00100,
    },
};
// flags for adjusting special chars
static bool _REPLOT_FONT_FLAG[128][1] = {
    ['w'] = {true},
    ['m'] = {true},
    ['$'] = {true},
};

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
#define RRect RSoft_rect
#endif

#ifndef RRECT
#define RRECT(x, y, w, h) RSOFT_RECT(x, y, w, h)
#endif

#ifndef RSize
#define RSize RSoft_area
#endif

#ifndef RSIZE
#define RSIZE(w, h) RSOFT_AREA(w, h)
#endif

#ifndef RPoint
#define RPoint RSoft_point
#endif

#ifndef RPOINT
#define RPOINT(x, y) RSOFT_POINT(x, y)
#endif

#ifndef RArea
#define RArea RSoft_area
#endif

#ifndef RAREA
#define RAREA(w, h) RSOFT_AREA(w, h)
#endif

/////////////////////////////////////////////////////

float _replot_math_lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

double _replot_math_angle(double x, double y) {
    double angleRadians = atan2(y, x);
    double angleDegrees = angleRadians * (180.0 / M_PI);
    if (angleDegrees < 0) {
        angleDegrees += 360.0;
    }
    return angleDegrees;
}

RPoint RC_POINT_ROTATE(RPoint point, RPoint anchor, int angle) {
    double radians = angle * M_PI / 180.0;
    int px = point.x;
    int py = point.y;
    int ax = anchor.x;
    int ay = anchor.y;
    int outX = ax + (px - ax) * cos(radians) - (py - ay) * sin(radians);
    int outY = ay + (px - ax) * sin(radians) + (py - ay) * cos(radians);
    return RPOINT(outX, outY);
}

void RC_POINT_SWAP(RPoint *p1, RPoint *p2) {
    int t = (*p1).x;
    (*p1).x = (*p2).x;
    (*p2).x = t;
    t = (*p1).y;
    (*p1).y = (*p2).y;
    (*p2).y = t;
}

void RC_FLOAT_SWAP(float *a, float *b) {
    float t = *a;
    *a = *b;
    *b = t;
}

RPoint RC_2POINTS_CENTER(RPoint ps[2]) {
    int x = (ps[0].x + ps[1].x) / 2;
    int y = (ps[0].y + ps[1].y) / 2;
    return RPOINT(x, y);
}

RPoint RC_3POINTS_CENTER(RPoint ps[3]) {
    int x = (ps[0].x + ps[1].x + ps[2].x) / 3;
    int y = (ps[0].y + ps[1].y + ps[2].y) / 3;
    return RPOINT(x, y);
}

RSoft_point RC_VECTOR_LERP(RSoft_point p1, RSoft_point p2, float t) {
    return (RSoft_point) {_replot_math_lerp(p1.x, p2.x, t), _replot_math_lerp(p1.y, p2.y, t)};
}

RSoft_point RC_POINT_LERP(RSoft_point p1, RSoft_point p2, float t) {
    return (RSoft_point) {_replot_math_lerp(p1.x, p2.x, t), _replot_math_lerp(p1.y, p2.y, t)};
}

RPoint RC_POINT_MIDDLE(RPoint p1, RPoint p2) {
    return (RSoft_point) {_replot_math_lerp(p1.x, p2.x, 0.5), _replot_math_lerp(p1.y, p2.y, 0.5)};
}

/////////////////////////////////////////////////////

typedef void (*REPLOT_PLOT_FUNC)(int, int, int, int, int, int);

typedef struct Replot {
    int w;
    int h;
    int c;
    RTexture buffer;
    int centerX;
    int centerY;
    int sizeW;
    int sizeH;
    //
    RPixel drawColor;
    int drawRotation;
    //
    RTexture stencil;
    int stencilW;
    int stencilH;
    //
    int lmtX;
    int lmtY;
    int lmtW;
    int lmtH;
    //
    int fcsX;
    int fcsY;
    int fcsW;
    int fcsH;
    //
    REPLOT_PLOT_FUNC plot;
    //
} Replot;

void Replot_clear(Replot *, RColor);

#include "replot.c"

#endif
