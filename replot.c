
#ifndef REPLOT_SOURCE_H
#define REPLOT_SOURCE_H

#include "replot.h"

void _replot_prepare(Replot *this, int w, int h) {
    this->w = w;
    this->h = h;
    this->c = 4;
    int count = w * h * this->c;
    int size = count * sizeof(RByte);
    this->buffer = (RTexture)realloc(this->buffer, size);
    // memset(this->buffer, 10, size);  
    Replot_clear(this, RCOLOR(10, 10, 10, 255));
}

void _rplot_reset(Replot *this) {
    this->drawColor[0] = 255;
    this->drawColor[1] = 255;
    this->drawColor[2] = 255;
    this->drawColor[3] = 255; 
    this->isBlend = false;
    this->drawRotation = 0;
    this->txtrRotation = 0;
    this->drawScale = (RScale){1, 1};
    this->txtrScale = (RScale){1, 1};
    this->drawSkew = (RSkew){0, 0};
    this->txtrSkew = (RSkew){0, 0};
    this->drawFocus = (RRect){0, 0, 0, 0};
    this->txtrLimit = (RRect){0, 0, 0, 0};
    this->drawMatrix = RMatrix_initMatrix();
    this->txtrMatrix = RMatrix_initMatrix();
}

Replot *Replot_new(int w, int h) {
    Replot *rplt = (Replot *)malloc(sizeof(Replot));
    //
    rplt->plot = NULL;
    rplt->buffer = NULL;
    rplt->stencil = NULL;
    //
    _rplot_reset(rplt);
    _replot_prepare(rplt, w, h);
    return rplt;
}

Replot *Replot_wrap(RTexture *txtr, int w, int h) {
    Replot *rplt = Replot_new(w, h);
    int _chnl = RCHANNEL;
    int _size = w * h * _chnl;
    rplt->buffer = (RTexture)realloc(rplt->buffer, _size);
    memcpy(rplt->buffer, *txtr, _size);
    return rplt;
}

void Replot_setDrawArgs(Replot *this, int rotation, int scaleX, int scaleY, int skewX, int skewY) {
    this->drawRotation = rotation;
}

void Replot_setTxtrMatrix(Replot *this, int rotation, int scaleX, int scaleY, int skewX, int skewY) {
    this->txtrRotation = rotation;
}

void Replot_setRotation(Replot *this, int rotation) {
    this->drawRotation = rotation;
}

void Replot_setSkew(Replot *this, float skewX, float skewY) {
    this->drawSkew.x = skewX;
    this->drawSkew.y = skewY;
}

void Replot_setScale(Replot *this, float scaleX, float scaleY) {
    this->drawScale.x = scaleX;
    this->drawScale.y = scaleY;
}

void _replot_rotateDrawXY(Replot *this, int *x, int *y) {
    RMatrix_matrixRotate(this->drawMatrix, x, y);
}

void _replot_rotateTxtrXY(Replot *this, int *x, int *y) {
    RMatrix_matrixRotate(this->txtrMatrix, x, y);
}

void _replot_rotateDrawPoint(Replot *this, RPoint *p) {
    _replot_rotateDrawXY(this, &p->x, &p->y);
}

void _replot_rotateTxtrPoint(Replot *this, RPoint *p) {
    _replot_rotateTxtrXY(this, &p->x, &p->y);
}

void Replot_setColor(Replot *this, RColor color) {
    this->drawColor[0] = color.r;
    this->drawColor[1] = color.g;
    this->drawColor[2] = color.b;
    this->drawColor[3] = color.a;
}

void _replot_delStencil(Replot *this) {
    if (this->stencil) {
        free(this->stencil);
    } 
    this->stencil = NULL;
    this->stencilW = 0;
    this->stencilH = 0;
    this->stencilSize = 0;
}

// TODO:support point
void _replot_setStencil(Replot *this, RTexture txtr, int w, int h) {
    int _chnl = 4;
    int _size = w * h * _chnl;
    this->stencil = (RTexture)realloc(this->stencil, _size);
    memcpy(this->stencil, txtr, _size);
    this->stencilW = w;
    this->stencilH = h;
    this->stencilSize = w * h;
}

void Replot_delStencil(Replot *this) {
    this->stencil = NULL;
}

void _replot_getColor(Replot *this, int px, int py, RPixel output) {

	if (this->stencil == NULL) {
        output[0] = this->drawColor[0];
        output[1] = this->drawColor[1];
        output[2] = this->drawColor[2];
        output[3] = this->drawColor[3];
        return;
    }
    int oldX = px;
    int oldY = py;
    _replot_rotateTxtrXY(this, &px, &py);
	size_t index = (py * this->stencilW) + px;

	if (index >= this->stencilSize) {
        output[0] = 0;
        output[1] = 0;
        output[2] = 0;
        output[3] = 0;
    } else {
        size_t indent = index * RCHANNEL;
        output[0] = this->stencil[indent];
        output[1] = this->stencil[indent + 1];
        output[2] = this->stencil[indent + 2];
        output[3] = this->stencil[indent + 3];
    }

    if (this->isBlend) {
        output[0] *= (float)(this->drawColor[0] / 255.0f);
        output[1] *= (float)(this->drawColor[1] / 255.0f);
        output[2] *= (float)(this->drawColor[2] / 255.0f);
        output[3] *= (float)(this->drawColor[3] / 255.0f);
    }

}

void _replot_limitWithXYWH(Replot *this, int cx, int cy, int sw, int sh) {
    this->txtrLimit.x = cx;
    this->txtrLimit.y = cy;
    this->txtrLimit.w = sw;
    this->txtrLimit.h = sh;
}

void _replot_limitWithWH(Replot *this, RPoint *center, int sw, int sh) {
    _replot_limitWithXYWH(this, center->x, center->y, sw, sh);
}

void _replot_limitWithXY(Replot *this, int cx, int cy, RSize *size) {
    _replot_limitWithXYWH(this, cx, cy, (*size).w, (*size).h);
}

void _replot_limit(Replot *this, RPoint *center, RSize *size) {
    _replot_limitWithXYWH(this, center->x, center->y, (*size).w, (*size).h);
}

void _replot_focusWithXYWH(Replot *this, int cx, int cy, int sw, int sh) {
    this->drawFocus.x = cx;
    this->drawFocus.y = cy;
    this->drawFocus.w = sw;
    this->drawFocus.h = sh;
}

void _replot_focusWithWH(Replot *this, RPoint *center, int sw, int sh) {
    _replot_focusWithXYWH(this, center->x, center->y, sw, sh);
}

void _replot_focusWithXY(Replot *this, int cx, int cy, RSize *size) {
    _replot_focusWithXYWH(this, cx, cy, (*size).w, (*size).h);
}

void _replot_focus(Replot *this, RPoint *center, RSize *size) {
    _replot_focusWithXYWH(this, center->x, center->y, (*size).w, (*size).h);
}

void _replot_applyChanges(Replot *this) {
    // 
    float dsx = this->drawScale.x;
    float dsy = this->drawScale.y;
    // 
    RMatrix dm = RMatrix_initMatrix();
    dm = RMatrix_translateMatrix(dm, RVECTOR(-this->drawFocus.x, -this->drawFocus.y));
    dm = RMatrix_scaleMatrix(dm, dsx, dsy);
    dm = RMatrix_skewMatrix(dm, this->drawSkew.x, this->drawSkew.y);
    dm = RMatrix_rotateMatrix(dm, -this->drawRotation);
    dm = RMatrix_translateMatrix(dm, RVECTOR(this->drawFocus.x, this->drawFocus.y));
    this->drawMatrix = dm;
    // 
    // fill limit to focus
    float rate1 = 1.0f * this->drawFocus.w / this->txtrLimit.w * dsx;
    float rate2 = 1.0f * this->drawFocus.h / this->txtrLimit.h * dsy;
    float tsx = this->txtrScale.x * rate1;
    float tsy = this->txtrScale.y * rate2;
    // 
    RMatrix tm = RMatrix_initMatrix();
    tm = RMatrix_translateMatrix(tm, RVECTOR(-this->drawFocus.x, -this->drawFocus.y));
    tm = RMatrix_rotateMatrix(tm, this->txtrRotation + this->drawRotation);
    tm = RMatrix_skewMatrix(tm, this->txtrSkew.x, this->txtrSkew.y);
    tm = RMatrix_scaleMatrix(tm, 1.0f / tsx, 1.0f / tsy);
    tm = RMatrix_translateMatrix(tm, RVECTOR(this->txtrLimit.x, this->txtrLimit.y));
    this->txtrMatrix = tm;
    //
}

void Replot_printChanges(Replot *this) {
    printf("Replot:\n");
    printf("slf:%p w:%i h:%i \n", this, this->w, this->h);
    printf("snl:%p w:%i h:%i \n", this->stencil, this->stencilW, this->stencilH);
    printf("rot:%d \n", this->drawRotation);
    RByte *clr = this->drawColor;
    printf("clr:%p r:%i g:%i b:%i a:%i \n", clr, clr[0], clr[1], clr[2], clr[3]);
}

void Replot_resetChanges(Replot *this) {
    _rplot_reset(this);
    this->stencil = NULL;
}

void _replot_delPixels(Replot *this, u8 color[4]) {
    if (color[0] == color[1] && color[0] == color[2] && color[0] == color[3]) {
        memset(this->buffer, color[0], this->w * this->h * RCHANNEL * sizeof(u8));  
        return;
    }
    u32 x, y;
    for (y = 0; y < this->h; y++) {
        for (x = 0; x < this->w; x++) {
            u32 index = (y * RCHANNEL * this->w) + x * RCHANNEL;
            memcpy(this->buffer + index, color, RCHANNEL * sizeof(u8));
        }
    }    
}

void Replot_clear(Replot *this, RColor color) {
    _replot_delPixels(this, (RPixel){color.r, color.g, color.b, color.a});
}

void _replot_setPixel(Replot *this, int px, int py, RPixel color) {
    u8 *buffer = this->buffer;
    int bufferW = this->w;
    int bufferH = this->h;

	if (px < 0 || py < 0 || px >= bufferW || py >= bufferH) {
		return;
    }

    #ifndef REPLOT_NO_PLOT
        if (this->plot) {
            this->plot(px, py, color[0], color[1], color[2], color[3]);
            return;
        }
    #endif

	int index = ((u32)py) * (RCHANNEL * bufferW) + (u32)px * RCHANNEL;
	if (color[3] != 255) {
		float alpha = color[3] / 255.0f;
		color[0] = (color[0] * alpha) + ((1 - alpha) * buffer[index]);
		color[1] =  (color[1] * alpha) + ((1 - alpha) * buffer[index + 1]);
		color[2] =  (color[2] * alpha) + ((1 - alpha) * buffer[index + 2]);
		color[3] =  (color[3] * alpha) + ((1 - alpha) * buffer[index + 3]);
	}
    memcpy(buffer + index, color, 4 * sizeof(u8));
}

RPixel __replotPointColor = {0};

void _replot_fillPointWithXY(Replot *this, int px, int py) {
    _replot_getColor(this, px, py, __replotPointColor);
    _replot_setPixel(this, px, py, __replotPointColor);
}

void _replot_fillPoint(Replot *this, RPoint *point) {
    _replot_fillPointWithXY(this, point->x, point->y);
}

void _replot_fillLineWithXY(Replot *this, int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    while (1) {
        _replot_fillPointWithXY(this, x0, y0);
        if (x0 == x1 && y0 == y1) break;
        int err2 = err * 2;
        if (err2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (err2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void _replot_fillLine(Replot *this, RPoint *start, RPoint *end) {
    _replot_fillLineWithXY(this, start->x, start->y, end->x, end->y);
}

void Replot_drawPoint(Replot *this, RPoint point, int size) {
    _replot_fillPoint(this, &point);
}

void Replot_drawLine(Replot *this, RPoint point1, RPoint point2, int thickness) {
    _replot_fillLine(this, &point1, &point2);
}

void Replot_drawLine1(Replot *this, RPoint point1, RPoint point2, int thickness, RPoint anchor) {
    RPoint _last = (RPoint){0, 0};
    for (float t = 0; t <= 1; t += 0.01) {
        RPoint p1 = RC_VECTOR_LERP(point1, anchor, t);
        RPoint p2 = RC_VECTOR_LERP(anchor, point2, t);
        RPoint p0 = RC_VECTOR_LERP(p1, p2, t);
        RPoint _point = (RPoint){p0.x, p0.y};
        if (t != 0) {
            _replot_fillLine(this, &_last, &_point);
        }
        _last.x = _point.x;
        _last.y = _point.y;
    }
}

void Replot_drawLine2(Replot *this, RPoint point1, RPoint point2, int thickness, RPoint anchor1, RPoint anchor2) {
    RPoint _last = (RPoint){0, 0};
    for (float t = 0; t <= 1; t += 0.01) {
        RPoint p1 = RC_VECTOR_LERP(point1, anchor1, t);
        RPoint p2 = RC_VECTOR_LERP(anchor1, anchor2, t);
        RPoint p3 = RC_VECTOR_LERP(anchor2, point2, t);
        RPoint _p1 = RC_VECTOR_LERP(p1, p2, t);
        RPoint _p2 = RC_VECTOR_LERP(p2, p3, t);
        RPoint p0 = RC_VECTOR_LERP(_p1, _p2, t);
        RPoint _point = (RPoint){p0.x, p0.y};
        if (t != 0) {
            _replot_fillLine(this, &_last, &_point);
        }
        _last.x = _point.x;
        _last.y = _point.y;
    }
}

int __triangleMinY = 0;
int __triangleMaxY = 0;
int __triangleThickness = 0;
typedef void (*__TRIANGLE_LINE_FUNC)(Replot *, int, int, int, int);

void __replot_lineTrianleLTR(Replot *this, int x1, int x2, int y, int thickness) {
    _replot_fillLineWithXY(this, x1, y, x1 + thickness, y);
    _replot_fillLineWithXY(this, x2 - thickness, y, x2, y);
}

void __replot_lineTrianleRTL(Replot *this, int x1, int x2, int y, int thickness) {
    _replot_fillLineWithXY(this, x1, y, x1 - thickness, y);
    _replot_fillLineWithXY(this, x2 + thickness, y, x2, y);
}

void __replot_loopTrianleBorder(Replot *this, int fromY, int toY, float *_x1, float *_x2, float x1Slope, float x2Slope, __TRIANGLE_LINE_FUNC func) {
    for(int y = fromY ; y < toY; y++){
        int x1 = replot_math_round(*_x1);
        int x2 = replot_math_round(*_x2);
        if (y - __triangleThickness <= __triangleMinY || y + __triangleThickness >= __triangleMaxY) {
            _replot_fillLineWithXY(this, x1, y, x2, y);
        } else {
            int distance = abs(x1 - x2);
            int thickness = MIN(__triangleThickness, distance);
            func(this, x1, x2, y, thickness);
        }
        *_x1 += x1Slope;
        *_x2 += x2Slope;
    }
}

// zero when paint triangles for other shapes
// to avoid sverly painting the borders
bool __rTriangleExtend = 1;

void __replot_paintTriangleSimple(Replot *this, int fromY, int toY, float *_x1, float *_x2, float x1Slope, float x2Slope, int thickness) {

    if (thickness <= 0) {
        for(int y = fromY ; y < toY; y++) { // __rTriangleExtend
            int x1 = replot_math_round(*_x1);
            int x2 = replot_math_round(*_x2);
            if (x1 > x2) {
                RC_INT_SWAP(&x1, &x2);
            }
            x2 = MAX(x1, x2); // __rTriangleExtend
            _replot_fillLineWithXY(this, x1, y, x2, y);
            *_x1 += x1Slope;
            *_x2 += x2Slope;
        }
    } else if (*_x1 <= *_x2) {
        __replot_loopTrianleBorder(this, fromY, toY, _x1, _x2, x1Slope, x2Slope, __replot_lineTrianleLTR);
    } else {
        __replot_loopTrianleBorder(this, fromY, toY, _x1, _x2, x1Slope, x2Slope, __replot_lineTrianleRTL);
    }
}

void __replot_paintTriangleComplex(Replot *this, RPoint *points, int thickness){

    if(points[0].y > points[1].y) RC_POINT_SWAP(&points[0], &points[1]);
    if(points[1].y > points[2].y) RC_POINT_SWAP(&points[1], &points[2]);
    if(points[0].y > points[1].y) RC_POINT_SWAP(&points[0], &points[1]);

    __triangleMinY = points[0].y;
    __triangleMaxY = points[2].y;
    __triangleThickness = thickness;
    int _topY = __triangleMinY;
    int _mdlY = points[1].y;
    int _btmY = __triangleMaxY;
    
    float slope01 = ((float)(points[1].x-points[0].x)/(points[1].y-points[0].y));
    float slope12 = ((float)(points[2].x-points[1].x)/(points[2].y-points[1].y));
    float slope02 = ((float)(points[2].x-points[0].x)/(points[2].y-points[0].y));
    
    float _x1; float _x2;
    if(points[0].y == points[1].y){
        if(points[1].x < points[0].x){
            RC_FLOAT_SWAP(&slope02, &slope12);
            RC_POINT_SWAP(&points[0], &points[1]);
        }
        _x1 = points[0].x;
        _x2 = points[1].x;
        // _replot_fillLine(this, &points[0], &points[1]);
        __replot_paintTriangleSimple(this, _topY, _btmY, &_x1, &_x2, slope02, slope12, thickness);
    }else if(points[1].y == points[2].y){
        if(points[2].x < points[1].x){
            RC_FLOAT_SWAP(&slope01, &slope02);
            RC_POINT_SWAP(&points[1], &points[2]);
        }
        _x1 = points[0].x;
        _x2 = points[0].x;
        // _replot_fillLine(this, &points[1], &points[2]);
        __replot_paintTriangleSimple(this, _topY, _btmY, &_x1, &_x2, slope01, slope02, thickness);
    }else{
        _x1 = points[0].x;
        _x2 = points[0].x;
        __replot_paintTriangleSimple(this, _topY, _mdlY, &_x1, &_x2, slope01, slope02, thickness);
        __replot_paintTriangleSimple(this, _mdlY, _btmY, &_x1, &_x2, slope12, slope02, thickness);
    }
}

void _replot_doTriangle(Replot *this, RPoint *point1, RPoint *point2, RPoint *point3, int thickness) {
    __rTriangleExtend = 1;
    RPoint points[3] = {*point1, *point2, *point3};
    int left = MIN(MIN(point1->x, point2->x), point3->x);
    int rght = MAX(MAX(point1->x, point2->x), point3->x);
    int top  = MIN(MIN(point1->y, point2->y), point3->y);
    int btm  = MAX(MAX(point1->y, point2->y), point3->y);
    int w = rght - left;
    int h = btm - top;
    RPoint center = RPOINT(replot_math_round((left + rght) / 2.0f), replot_math_round((top + btm) / 2.0f));
    _replot_focusWithWH(this, &center, w, h);
    _replot_applyChanges(this);
    for (int i = 0; i < 3; i++) {
        RPoint *point = &points[i];
        _replot_rotateDrawPoint(this, point);
    }
    __replot_paintTriangleComplex(this, points, thickness);
    this->stencil = NULL;
    _replot_fillPointWithXY(this, center.x, center.y);
}

void Replot_drawTriangle(Replot *this, RPoint point1, RPoint point2, RPoint point3) {
    _replot_doTriangle(this, &point1, &point2, &point3, 1);
}

void Replot_fillTriangle(Replot *this, RPoint point1, RPoint point2, RPoint point3) {
    _replot_doTriangle(this, &point1, &point2, &point3, 0);
}


void __replot_paintRect(Replot *this, RPoint point, RSize size, bool isFill) {
    int rw = size.w / 2;
    int rh = size.h / 2;
    int left = point.x - rw;
    int top = point.y - rh;
    RPoint _p1 = RPOINT(left, top);
    RPoint _p2 = RPOINT(left + size.w, top);
    RPoint _p3 = RPOINT(left + size.w, top + size.h);
    RPoint _p4 = RPOINT(left, top + size.h);
    //
    RPoint points[4] = {_p1, _p2, _p3, _p4};
    for (int i = 0; i < 4; i++) {
        _replot_rotateDrawPoint(this, &points[i]);
    }
    replot_sort_points(points, 4);
    //
    RPoint p1 = points[0];
    RPoint p2 = points[1];
    RPoint p3 = points[2];
    RPoint p4 = points[3];
    //
    int _topY = p1.y;
    int _btmY = p4.y;
    int _upperY = p2.y;
    int _lowerY = p3.y;
    int _mddleY = (_upperY + _lowerY) / 2;
    int _leftX = MIN(p1.x, MIN(p2.x, MIN(p3.x, p4.x)));
    int _rightX = MAX(p1.x, MAX(p2.x, MAX(p3.x, p4.x)));
    int _mddleX = (_leftX + _rightX) / 2;
    int is2InRight = p2.x > p3.x;
    //
    if (!isFill) {
        _replot_fillLine(this, &_p1, &_p2);	
        _replot_fillLine(this, &_p2, &_p3);	
        _replot_fillLine(this, &_p3, &_p4);	
        _replot_fillLine(this, &_p4, &_p1);
        return;
    }
    // % 90 == 0 gradus
    if (p1.y == p2.y && p3.y == p4.y) {
        for (int y = p1.y; y <= p3.y; y++) {
            _replot_fillLineWithXY(this, p1.x, y, p2.x - 1, y);
        }
        return;
    }
    float slope12 = ((float)(p2.x-p1.x)/(p2.y-p1.y));
    float slope13 = ((float)(p3.x-p1.x)/(p3.y-p1.y));
    float slope24 = ((float)(p4.x-p2.x)/(p4.y-p2.y));
    float slope42 = ((float)(p2.x-p4.x)/(p2.y-p4.y));
    float slope43 = ((float)(p3.x-p4.x)/(p3.y-p4.y));
    // % 90 == 45 gradus
    if (p2.y == p3.y) {
        float _x12 = p1.x;
        float _x13 = p1.x;
        for(int y = _topY ; y < _mddleY; y++){
            int x12 = replot_math_round(_x12);
            int x13 = replot_math_round(_x13);
            _replot_fillLineWithXY(this, x12, y, x13, y);
            _x12 += slope12;
            _x13 += slope13;
        }
        float _x42 = p4.x;
        float _x43 = p4.x;
        for(int y = _btmY - 1 ; y >= _mddleY; y--){
            int x42 = replot_math_round(_x42);
            int x43 = replot_math_round(_x43);
            _replot_fillLineWithXY(this, x42, y, x43, y);
            _x42 -= slope42;
            _x43 -= slope43;
        }
        return;
    }
    // % 90 == other gradus
    float _x12 = p1.x;
    float _x13 = p1.x;
    float _x42 = p4.x;
    float _x43 = p4.x;
    if (is2InRight) {
        _x12--;
        _x42--;
    } else {
        _x13--;
        _x43--;
    }
    for(int y = _topY ; y < _upperY; y++){
        int x12 = replot_math_round(_x12);
        int x13 = replot_math_round(_x13);
        _replot_fillLineWithXY(this, x12, y, x13, y);
        _x12 += slope12;
        _x13 += slope13;
    }
    for(int y = _btmY - 1 ; y >= _lowerY; y--){
        int x42 = replot_math_round(_x42);
        int x43 = replot_math_round(_x43);
        _replot_fillLineWithXY(this, x42, y, x43, y);
        _x42 -= slope42;
        _x43 -= slope43;
    }
    float _x22 = _x12;
    float _x33 = _x13;
    for(int y = _upperY ; y < _lowerY; y++){
        int x22 = replot_math_round(_x22);
        int x33 = replot_math_round(_x33);
        _replot_fillLineWithXY(this, x22, y, x33, y);
        _x22 += slope13;
        _x33 += slope24;
    }
    //
}

void _Replot_doRect(Replot *this, RPoint point, RSize size, bool isFill) {
    _replot_focusWithWH(this, &point, size.w, size.h);
    _replot_applyChanges(this);
    __replot_paintRect(this, point, size, isFill);
}

void Replot_drawRect(Replot *this, RPoint point, RSize size) {
    _Replot_doRect(this, point, size, false);
}

void Replot_fillRect(Replot *this, RPoint point, RSize size) {
    _Replot_doRect(this, point, size, true);
}

int __circularAngleFrom = 0;
int __circularAngleTo = 360;
int __circularLimitNum = 0;
typedef void (*_CIRCULAR_POINT_FUNC)(Replot *, int, int, int, int);

void _replot_pointCircularWithAngle(Replot *this, int cx, int cy, int x, int y) {
    float angle = replot_math_angle(x, y);
    if (angle >= __circularAngleFrom && angle <= __circularAngleTo) {
        int _x = cx + x;
        int _y = cy + y;
        // _replot_rotateDrawXY(this, &_x, &_y);
        _replot_fillPointWithXY(this, _x, _y);
    }
}

void _replot_pointCircularNoAngle(Replot *this, int cx, int cy, int x, int y) {
    int _x = cx + x;
    int _y = cy + y;
    // _replot_rotateDrawXY(this, &_x, &_y);
    _replot_fillPointWithXY(this, _x, _y);
}

void _replot_paintCircularRunLoop(Replot *this, RPoint *point, int rx, int ry, _CIRCULAR_POINT_FUNC func) {
    int w2 = rx*rx;
    int h2 = ry*ry;
    int w2h2 = h2*w2;
    for(int y=-ry; y<=ry; y++) {
        for(int x=-rx; x<=rx; x++) {
            int num = x*x*h2+y*y*w2;
            if(num <= w2h2 && num > __circularLimitNum) {
                func(this, point->x, point->y, x, y);
            }
        }
    }
    if (__circularLimitNum <= 0) {
        // _replot_rotateDrawPoint(this, point);
        _replot_fillPointWithXY(this, point->x, point->y);
    }
}

void _replot_doCircular(Replot *this, RPoint point, int rx, int ry, int limit, float fromAngle, float toAngle) {
    _replot_focusWithWH(this, &point, rx * 2, ry * 2);
    _replot_applyChanges(this);
    rx = MAX(0, MIN(1024, rx));
    ry = MAX(0, MIN(1024, ry));
    fromAngle = MAX(0, MIN(360, fromAngle));
    toAngle = MAX(0, MIN(360, toAngle));
    __circularAngleFrom = MIN(fromAngle, toAngle);
    __circularAngleTo = MAX(fromAngle, toAngle);
    __circularLimitNum = limit * limit;
    if (fromAngle == toAngle) {
        return;
    } else if (fromAngle == 0 && toAngle == 360) {
        _replot_paintCircularRunLoop(this, &point, rx, ry, _replot_pointCircularNoAngle);
    } else {
        _replot_paintCircularRunLoop(this, &point, rx, ry, _replot_pointCircularWithAngle);
	}
}

void Replot_drawEllipse(Replot *this, RPoint point, int rx, int ry) {
    int limit = (rx - 1) * (ry - 1);
    _replot_doCircular(this, point, rx, ry, limit, 0, 360);
}

void Replot_fillEllipse(Replot *this, RPoint point, int rx, int ry) {
    int limit = 0;
    _replot_doCircular(this, point, rx, ry, limit, 0, 360);
}

void Replot_drawCircle(Replot *this, RPoint point, int r) {
    int limit = (r - 1) * (r - 1);
    _replot_doCircular(this, point, r, r, limit, 0, 360);
}

void Replot_fillCircle(Replot *this, RPoint point, int r) {
    int limit = 0;
    _replot_doCircular(this, point, r, r, limit, 0, 360);
}

void Replot_drawArc(Replot *this, RPoint point, int r, float fromAngle, float toAngle) {
    int limit = (r - 1) * (r - 1);
    _replot_doCircular(this, point, r, r, limit, fromAngle, toAngle);
}

void Replot_fillArc(Replot *this, RPoint point, int r, float fromAngle, float toAngle) {
    int limit = 0;
    _replot_doCircular(this, point, r, r, limit, fromAngle, toAngle);
}

void Replot_drawRing(Replot *this, RPoint point, int r, int thickness) {
    int _r = r + thickness;
    int limit = (r - 1) * (r - 1);
    int _limit = (_r - 1) * (_r - 1);
    _replot_doCircular(this, point, r, r, limit, 0, 360);
    _replot_doCircular(this, point, _r, _r, _limit, 0, 360);
}

void Replot_fillRing(Replot *this, RPoint point, int r, int thickness) {
    int limit = (r - thickness) * (r - thickness);
    _replot_doCircular(this, point, r, r, limit, 0, 360);
}

#define _SHAPE_MAX_SIDE 64
#define _SHAPE_MAX_RADIUS 512
RPoint __rShapePoints[_SHAPE_MAX_SIDE];

void __replot_paintPolygon(Replot *this, int sides, RPoint point, int radius, bool isFill) {
    //
    sides = MIN(_SHAPE_MAX_SIDE, sides);
    radius = MIN(_SHAPE_MAX_RADIUS, radius);
    double angleIncrement = (2 * M_PI) / sides; // Angle between each vertex
    for (int i = 0; i < sides; i++) {
        double angle = i * angleIncrement; // Current angle
        RPoint *p = &__rShapePoints[i];
        p->x = radius * cos(angle); // x-coordinate
        p->y = radius * sin(angle); // y-coordinate
        RPOINT_ADD_TO(p, &point);
        _replot_rotateDrawPoint(this, p);
    }
    //
    for (int i = 0; i < sides; i++) {
        RPoint p1 = __rShapePoints[i];
        RPoint p2 = __rShapePoints[i == sides - 1 ? 0 : i + 1];
        if (isFill) {
            RPoint points[3] = {p1, p2, point};
            __replot_paintTriangleComplex(this, points, 0);
        } else {
            _replot_fillLine(this, &p1, &p2);
        }
    }
}

void _replot_doPolygon(Replot *this, int sides, RPoint point, int radius, bool isFill) {
    __rTriangleExtend = 0;
    int _size = radius * 2;
    _replot_focusWithWH(this, &point, _size, _size);
    _replot_applyChanges(this);
    __replot_paintPolygon(this, sides, point, radius, isFill);
}

void Replot_drawPolygon(Replot *this, int sides, RPoint point, int radius) {
    _replot_doPolygon(this, sides, point, radius, false);
}

void Replot_fillPolygon(Replot *this, int sides, RPoint point, int radius) {
    _replot_doPolygon(this, sides, point, radius, true);
}

void _replot_doStar(Replot *this, int sides, RPoint point, int radius1, int radius2, bool isFill) {
    __rTriangleExtend = 0;
    int _size = (radius1 + radius2) * 2;
    _replot_focusWithWH(this, &point, _size, _size);
    _replot_applyChanges(this);
    __replot_paintPolygon(this, sides, point, radius1, isFill);
    for (int i = 0; i < sides; i++) {
        RPoint p1 = __rShapePoints[i];
        RPoint p2 = __rShapePoints[i == sides - 1 ? 0 : i + 1];
        RPoint middle = RPOINT((p1.x + p2.x) / 2, (p1.y + p2.y) / 2);
        int dx = middle.x - point.x;
        int dy = middle.y - point.y;
        float magnitude = sqrt(dx * dx + dy * dy);
        float rateX = magnitude == 0 ? 0 : dx / magnitude;
        float rateY = magnitude == 0 ? 1 : dy / magnitude;
        RVector vector = RVECTOR(radius2 * rateX, radius2 * rateY);
        RPoint p3 = RPOINT(middle.x + vector.x, middle.y + vector.y);
        RPoint points[3] = {p1, p2, p3};
        int thickness = isFill ? 0 : 1;
        __replot_paintTriangleComplex(this, points, thickness);
    }
}

void Replot_drawStar(Replot *this, int sides, RPoint point, int radius1, int radius2) {
    _replot_doStar(this, sides, point, radius1, radius2, false);
}

void Replot_fillStar(Replot *this, int sides, RPoint point, int radius1, int radius2) {
    _replot_doStar(this, sides, point, radius1, radius2, true);
}

/////////////////////////////////////////////////////

void _Replot_doPrint(Replot *this, int cx, int cy, char ch, RFont *font, int size) {
    __rTriangleExtend = 0;
    int sw = font->w * size;
    int sh = font->h * size;
    // _Replot_doRect(this, RPOINT(cx, cy), RSIZE(sw + 2, sh + 2), false);
    int fontIndent = (int)ch * font->h;
    _replot_focusWithXYWH(this, cx, cy, sw, sh);
    _replot_applyChanges(this);
    for (int i = 0; i < font->h; i++) {
        int _cy = cy + replot_math_round((i - font->h / 2.0f) * size);
        int idx = fontIndent + i;
        uchar num = font->glyphs[idx];
        uchar _num = num;
        for (size_t i = 0; i < font->w; i++) {
            int needDot = (num >> (i + font->fix)) & 1;
            if (!needDot) continue;
            int __cx = cx + replot_math_round(size * (font->w / 2.0f - i));
            int __cy = _cy;
            __replot_paintRect(this, RPOINT(__cx, __cy), RSIZE(size, size), true);
        }
    }
}

RFont *__replot_selectFont(int *size) {
    *size = MAX(1, MIN(50, abs(*size)));
    RFont *font = NULL;
    if (*size % 2 == 0) {
        font = &_replotFont2;
        *size = *size - 1;
    } else {
        font = &_replotFont1;
    }
    return font;
}

void Replot_printChar(Replot *this, RPoint point, int size, char ch) {
    RFont *font = __replot_selectFont(&size);
    _Replot_doPrint(this, point.x, point.y, ch, font, size);
}

int __textPosX[1024];
int __textPosY[1024];

void Replot_printText(Replot *this, RPoint point, int size, char *text) {
    RFont *font = __replot_selectFont(&size);
    int length = strlen(text);
    int distance = size * font->w + 1;
    int sw = distance * length;
    int sh = size * font->h;
    int rotation = this->drawRotation;
    _replot_focusWithWH(this, &point, sw, sh);
    _replot_applyChanges(this);
    // _Replot_doRect(this, point, RSIZE(sw, sh), false);
    for (int i = 0; i < length; i++) {
        int x = point.x + replot_math_round(distance * (i - length / 2.0f + 0.5));
        int y = point.y;
        _replot_rotateDrawXY(this, &x, &y);
        __textPosX[i] = x;
        __textPosY[i] = y;
    } 
    for (int i = 0; i < length; i++) {
        char ch = text[i];
        int x = __textPosX[i];
        int y = __textPosY[i];
        _Replot_doPrint(this, x, y, ch, font, size);
    } 
}

/////////////////////////////////////////////////////

void Replot_free(Replot *this) {
    _replot_delStencil(this);
    free(this->buffer);
    free(this);
}

void Replot_syncTo(Replot *this, Replot *other) {
    for (int y = 0; y < this->h; y++) {
        for (int x = 0; x < this->w; x++) {
            u32 index = (y * RCHANNEL * this->w) + x * RCHANNEL;
            memcpy(other->buffer + index, this->buffer + index, RCHANNEL * sizeof(u8));
        }
    } 
}

RByte *Replot_getPixel(Replot *this, int x, int y) {
    size_t index = (y * this->w) + x;
    size_t indent = index * RCHANNEL;
    return &this->buffer[indent];
}

void Replot_setPixel(Replot *this, int x, int y, RByte *pixel) {
    size_t index = (y * this->w) + x;
    size_t indent = index * RCHANNEL;
    this->buffer[indent + 0] = pixel[0];
    this->buffer[indent + 1] = pixel[1];
    this->buffer[indent + 2] = pixel[2];
    this->buffer[indent + 3] = pixel[3];
}

void _replot_validatePointSize(RPoint *point, RSize *size, int w, int h) {
    point->x = MAX(0, MIN(w, point->x));
    point->y = MAX(0, MIN(h, point->y));
    (*size).w = MIN(w - point->x, replot_math_round((*size).w / 2.0f)) * 2;
    (*size).h = MIN(h - point->y, replot_math_round((*size).h / 2.0f)) * 2;
}

/////////////////////////////////////////////////////

void Replot_drawCanvasExt(Replot *this, RPoint toP, RSize toS, Replot *rplt, RPoint frmP, RSize frmS) {
    _replot_validatePointSize(&toP, &toS, this->w, this->h);
    _replot_validatePointSize(&frmP, &frmS, rplt->w, rplt->h);
    _replot_setStencil(this, rplt->buffer, rplt->w, rplt->h);
    _replot_limit(this, &frmP, &frmS);
    _Replot_doRect(this, toP, toS, true);
    _replot_delStencil(this);
}

void Replot_drawCanvas(Replot *this, RPoint point, RSize size, Replot *rplt) {
    _replot_validatePointSize(&point, &size, this->w, this->h);
    int _w2 = rplt->w / 2;
    int _h2 = rplt->h / 2;
    RPoint frmP = RPOINT(_w2, _h2);
    RSize frmS = RSIZE(_w2 * 2, _h2 * 2);
    _replot_setStencil(this, rplt->buffer, rplt->w, rplt->h);
    _replot_limit(this, &frmP, &frmS);
    _Replot_doRect(this, point, size, true);
    _replot_delStencil(this);
}

// TODO:fix bug
Replot *Replot_copy(Replot *this, RPoint point, RSize size) {
    Replot *rplt = Replot_new(size.w, size.h);
    RPoint toP = RPOINT(replot_math_round(size.w / 2.0f), replot_math_round(size.h / 2.0f));
    Replot_drawCanvasExt(rplt, toP, size, this, point, size);
    return rplt;
}

// TODO:fix bug
void Replot_crop(Replot *this, RPoint point, RSize size) {
    _replot_setStencil(this, this->buffer, this->w, this->h);
    _replot_limit(this, &point, &size);
    _replot_prepare(this, size.w, size.h);
    RPoint toP = RPOINT(replot_math_round(size.w / 2.0f), replot_math_round(size.h / 2.0f));
    _Replot_doRect(this, toP, size, true);
    _replot_delStencil(this);
}

void Replot_flipX(Replot *this) {
    RPixel temp = {0};
    for (int y = 0; y < this->h; y++) {
        for (int x = 0; x < this->w / 2; x++) {
	        size_t index1 = (y * this->w) + x;
	        size_t index2 = (y * this->w) + this->w - 1 - x;
            size_t indent1 = index1 * RCHANNEL;
            size_t indent2 = index2 * RCHANNEL;
            RPIXEL_COPY_TO(temp, 0, this->buffer, indent1);
            RPIXEL_COPY_TO(this->buffer, indent1, this->buffer, indent2);
            RPIXEL_COPY_TO(this->buffer, indent2, temp, 0);
        }
    }
}

void Replot_flipY(Replot *this) {
    RPixel temp = {0};
    for (int y = 0; y < this->h / 2; y++) {
        for (int x = 0; x < this->w; x++) {
	        size_t index1 = (y * this->w) + x;
	        size_t index2 = ((this->h -1 - y) * this->w) + x;
            size_t indent1 = index1 * RCHANNEL;
            size_t indent2 = index2 * RCHANNEL;
            RPIXEL_COPY_TO(temp, 0, this->buffer, indent1);
            RPIXEL_COPY_TO(this->buffer, indent1, this->buffer, indent2);
            RPIXEL_COPY_TO(this->buffer, indent2, temp, 0);
        }
    }
}

void Replot_flip(Replot *this, bool flipX, bool flipY) {
    if (flipX) Replot_flipX(this);
    if (flipY) Replot_flipY(this);
}

void Replot_invert(Replot *this) {
    for (int y = 0; y < this->h; y++) {
        for (int x = 0; x < this->w; x++) {
	        size_t index = (y * this->w) + x;
            size_t indent = index * RCHANNEL;
            for (int i = 0; i < 3; i++) {
                this->buffer[indent] = 255 - this->buffer[indent];
            }
        }
    }
}

void Replot_tint(Replot *this, RColor tintColor, float intensity) {
    for (int y = 0; y < this->h; y++) {
        for (int x = 0; x < this->w; x++) {
            RByte *pix = Replot_getPixel(this, x, y);
            pix[0] = (unsigned char)((1 - intensity) * pix[0] + intensity * tintColor.r);
            pix[1] = (unsigned char)((1 - intensity) * pix[1] + intensity * tintColor.g);
            pix[2] = (unsigned char)((1 - intensity) * pix[2] + intensity * tintColor.b);
        }
    }
}

void Replot_fade(Replot *this, float intensity) {
    for (int y = 0; y < this->h; y++) {
        for (int x = 0; x < this->w; x++) {
            RByte *pix = Replot_getPixel(this, x, y);
            pix[3] = (unsigned char)(intensity * 255);
        }
    }
}

void Replot_blur(Replot *this, int radius) {
    Replot *blurred = Replot_new(this->w, this->h);

    // Loop through each pixel in the image
    for (int y = 0; y < this->h; y++) {
        for (int x = 0; x < this->w; x++) {
            int rSum = 0, gSum = 0, bSum = 0;
            int count = 0;
            // Loop over the neighboring pixels (3x3)
            for (int dy = -radius; dy <= radius; dy++) {
                for (int dx = -radius; dx <= radius; dx++) {
                    int newY = y + dy;
                    int newX = x + dx;

                    // Check bounds
                    if (newY >= 0 && newY < this->h && newX >= 0 && newX < this->w) {
                       RByte *pix = Replot_getPixel(this, newX, newY);
                        rSum += pix[0];
                        gSum += pix[1];
                        bSum += pix[2];
                        count++;
                    }
                }
            }
            // Compute the average color and store it in the blurred image
            RPixel pix = {0};
            pix[0] = (unsigned char)(rSum / count);
            pix[1] = (unsigned char)(gSum / count);
            pix[2] = (unsigned char)(bSum / count);
            pix[3] = 255;
            Replot_setPixel(blurred, x, y, pix);
        }
    }

    Replot_syncTo(blurred, this);
    Replot_free(blurred);
}

/////////////////////////////////////////////////////

void Replot_setTexture(Replot *this, RTexture txtr, int w, int h) {
    RPoint point = RPOINT(replot_math_round(w / 2.0f), replot_math_round(h / 2.0f));
    RSize size = RSIZE(w, h);
    _replot_setStencil(this, txtr, w, h);
    _replot_limit(this, &point, &size);
}

void Replot_setGradient(Replot *this, int w, int h, RColor ltColor, RColor rtColor, RColor lbColor, RColor rbColor) {
    RTexture texture = replot_generate_gradient(
        w, h,
        ltColor, rtColor,
        lbColor, rbColor
    );
    Replot_setTexture(this, texture, w, h);
}

void Replot_setGradientH(Replot *this, int w, int h, RColor lColor, RColor rColor) {
    RTexture texture = replot_generate_gradient(
        w, h,
        lColor, rColor,
        lColor, rColor
    );
    Replot_setTexture(this, texture, w, h);
}

void Replot_setGradientV(Replot *this, int w, int h, RColor tColor, RColor bColor) {
    RTexture texture = replot_generate_gradient(
        w, h,
        tColor, tColor,
        bColor, bColor
    );
    Replot_setTexture(this, texture, w, h);
}

#ifdef REPLOT_USE_IMAGE
void Replot_setImageExt(Replot *this, char *path, RPoint point, RSize size) {
    if (path == NULL) {
        _replot_delStencil(this);
    } else {
        int w, h, c;
        RTexture txtr = rimage_read(path, &w, &h, &c);
        _replot_validatePointSize(&point, &size, w, h);
        _replot_setStencil(this, txtr, w, h);
        _replot_limit(this, &point, &size);
        rimage_free(&txtr);
    }
}
void Replot_setImage(Replot *this, char *path) {
    if (path == NULL) {
        _replot_delStencil(this);
    } else {
        int w, h, c;
        RTexture txtr = rimage_read(path, &w, &h, &c);
        RPoint point = RPOINT(replot_math_round(w / 2.0f), replot_math_round(h / 2.0f));
        RSize size = RSIZE(w, h);
        _replot_setStencil(this, txtr, w, h);
        _replot_limit(this, &point, &size);
        rimage_free(&txtr);
    }
}
void Replot_drawImageExt(Replot *this, RPoint toP, RSize toS, char *path, RPoint frmP, RSize frmS) {
    Replot_setImageExt(this, path, frmP, frmS);
    Replot_drawRect(this, toP, toS);
    Replot_setImage(this, NULL);
}
void Replot_drawImage(Replot *this, RPoint point, RSize size, char *path) {
    Replot_setImage(this, path);
    Replot_drawRect(this, point, size);
    Replot_setImage(this, NULL);
}
#endif

/////////////////////////////////////////////////////

#ifdef REPLOT_USE_IMAGE
Replot *Replot_read(char *path) {
    int w, h, c;
    RTexture txtr = rimage_read(path, &w, &h, &c);
    Replot *rplt = Replot_wrap(&txtr, w, h);
    rimage_free(&txtr);
    return rplt;
}

void Replot_write(Replot *this, char *path) {
    rimage_write(path, &this->buffer, this->w, this->h, this->c);
}
#endif

/////////////////////////////////////////////////////

#endif
