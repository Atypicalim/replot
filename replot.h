// replot

#ifndef REPLOT_HEADER_H
#define REPLOT_HEADER_H

#include "RDefine.h"


#ifndef REPLOT_MATRIX_IMPLEMENTATION
#define REPLOT_MATRIX_IMPLEMENTATION
#include "RMatrix.h"
#endif

#ifndef REPLOT_FONT_IMPLEMENTATION
#define REPLOT_FONT_IMPLEMENTATION
#include "RFont.h"
#endif

#ifndef REPLOT_IMAGE_IMPLEMENTATION
#define REPLOT_IMAGE_IMPLEMENTATION
#include "RImage.h"
#endif

/////////////////////////////////////////////////////

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
    int t = p1->x;
    p1->x = p2->x;
    p2->x = t;
    t = p1->y;
    p1->y = p2->y;
    p2->y = t;
}

void RC_FLOAT_SWAP(float *a, float *b) {
    float t = *a;
    *a = *b;
    *b = t;
}

void RC_INT_SWAP(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

RPoint RC_2POINTS_CENTER(RPoint ps[2]) {
    int x = (ps[0].x + ps[1].x) / 2;
    int y = (ps[0].y + ps[1].y) / 2;
    return RPOINT(x, y);
}

RPoint RC_VECTOR_LERP(RPoint p1, RPoint p2, float t) {
    return (RPoint) {replot_math_lerp(p1.x, p2.x, t), replot_math_lerp(p1.y, p2.y, t)};
}

RPoint RC_POINT_LERP(RPoint p1, RPoint p2, float t) {
    return (RPoint) {replot_math_lerp(p1.x, p2.x, t), replot_math_lerp(p1.y, p2.y, t)};
}

RPoint RC_POINT_MIDDLE(RPoint p1, RPoint p2) {
    return (RPoint) {replot_math_lerp(p1.x, p2.x, 0.5), replot_math_lerp(p1.y, p2.y, 0.5)};
}

// Function to swap two points
void replot_swap_point(RPoint* a, RPoint* b) {
    RPoint temp = *a;
    *a = *b;
    *b = temp;
}

// Bubble sort function to sort points by their y-coordinate
void replot_sort_points(RPoint* points, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (points[j].y > points[j + 1].y) {
                replot_swap_point(&points[j], &points[j + 1]);
            }
        }
    }
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
    bool isBlend;
    //
    RMatrix drawMatrix;
    RMatrix txtrMatrix;
    int drawRotation;
    int txtrRotation;
    RScale drawScale;
    RScale txtrScale;
    RSkew drawSkew;
    RSkew txtrSkew;
    //
    RRect drawFocus;
    RRect txtrLimit;
    //
    RTexture stencil;
    int stencilW;
    int stencilH;
    int stencilSize;
    //
    REPLOT_PLOT_FUNC plot;
    //
} Replot;

void Replot_clear(Replot *, RColor);

#include "replot.c"

#endif
