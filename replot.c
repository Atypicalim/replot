
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

Replot *Replot_new(int w, int h) {
    Replot *rplt = (Replot *)malloc(sizeof(Replot));
    //
    rplt->plot = NULL;
    rplt->buffer = NULL;
    rplt->stencil = NULL;
    //
    rplt->drawColor[0] = 255;
    rplt->drawColor[1] = 255;
    rplt->drawColor[2] = 255;
    rplt->drawColor[3] = 255;
    rplt->drawRotation = 0;
    //
    rplt->fcsX = 0;
    rplt->fcsY = 0;
    rplt->fcsW = w;
    rplt->fcsH = h;
    //
    _replot_prepare(rplt, w, h);
    return rplt;
}

Replot *Replot_wrap(RTexture *txtr, int w, int h) {
    Replot *rplt = Replot_new(w, h);
    int _chnl = 4;
    int _size = w * h * _chnl;
    rplt->buffer = (RTexture)realloc(rplt->buffer, _size);
    memcpy(rplt->buffer, *txtr, _size);
    return rplt;
}

void Replot_setRotation(Replot *this, int rotation) {
    this->drawRotation = rotation;
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
}

// TODO:support point
void _replot_setStencil(Replot *this, RTexture txtr, int w, int h) {
    int _chnl = 4;
    int _size = w * h * _chnl;
    this->stencil = (RTexture)realloc(this->stencil, _size);
    memcpy(this->stencil, txtr, _size);
    this->stencilW = w;
    this->stencilH = h;
}

u32 _replot_getColor(Replot *this, int fcsX, int fcsY) {
	u8 output[4] = {
        this->drawColor[0],
        this->drawColor[1],
        this->drawColor[2],
        this->drawColor[3]
    };
	if (this->stencil == NULL) {
        return *((u32*)output);
    }

    int _fcsX = fcsX - this->fcsX;
    int _fcsY = fcsY - this->fcsY;
	int _lmtX = _fcsX / ((float)this->fcsW / (float)this->lmtH); 
	int _lmtY = _fcsY / ((float)this->fcsH / (float)this->lmtH);
    int lmtX = this->lmtX + _lmtX;
    int lmtY = this->lmtY + _lmtY;

	size_t index = (lmtY * this->stencilW) + lmtX;
    size_t indent = index * 4;

	if (index > this->stencilW * this->stencilH) {
        return *((u32*)output);
    }

	output[0] = this->stencil[indent];
	output[1] = this->stencil[indent + 1];
	output[2] = this->stencil[indent + 2];
	output[3] = this->stencil[indent + 3];

	output[0] *= (float)(this->drawColor[0] / 255.0f);
	output[1] *= (float)(this->drawColor[1] / 255.0f);
	output[2] *= (float)(this->drawColor[2] / 255.0f);
	output[3] *= (float)(this->drawColor[3] / 255.0f);

	return *((u32*)output);
}

void _replot_limitWithXYWH(Replot *this, int cx, int cy, int sw, int sh) {
    this->lmtX = cx;
    this->lmtY = cy;
    this->lmtW = sw;
    this->lmtH = sh;
}

void _replot_limitWithWH(Replot *this, RPoint *center, int sw, int sh) {
    _replot_limitWithXYWH(this, (*center).x, (*center).y, sw, sh);
}

void _replot_limitWithXY(Replot *this, int cx, int cy, RSize *size) {
    _replot_limitWithXYWH(this, cx, cy, (*size).w, (*size).h);
}

void _replot_limit(Replot *this, RPoint *center, RSize *size) {
    _replot_limitWithXYWH(this, (*center).x, (*center).y, (*size).w, (*size).h);
}

void _replot_focusWithXYWH(Replot *this, int cx, int cy, int sw, int sh) {
    this->fcsX = cx;
    this->fcsY = cy;
    this->fcsW = sw;
    this->fcsH = sh;
}

void _replot_focusWithWH(Replot *this, RPoint *center, int sw, int sh) {
    _replot_focusWithXYWH(this, (*center).x, (*center).y, sw, sh);
}

void _replot_focusWithXY(Replot *this, int cx, int cy, RSize *size) {
    _replot_focusWithXYWH(this, cx, cy, (*size).w, (*size).h);
}

void _replot_focus(Replot *this, RPoint *center, RSize *size) {
    _replot_focusWithXYWH(this, (*center).x, (*center).y, (*size).w, (*size).h);
}

void _replot_applyChanges(Replot *this) {
    RSoft_matrix m = RSoft_initMatrix();
    m = RSoft_translateMatrix(m, RSOFT_VECTOR2D(-this->fcsX, -this->fcsY));
    m = RSoft_simpleRotateMatrix(m, -this->drawRotation);
    m = RSoft_translateMatrix(m, RSOFT_VECTOR2D(this->fcsX, this->fcsY));
    RSoft_setMatrix(m);
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
    RSoft_matrix x = RSoft_initMatrix();
    RSoft_setMatrix(x);
    this->drawRotation = 0;
    this->stencil = NULL;
}

void _replot_rotatePointXY(Replot *this, int *x, int *y) {
    _replot_matrixRotate(RSoft_renderInfo.matrix, x, y);
}

void _replot_rotatePoint(Replot *this, RPoint *p) {
    _replot_rotatePointXY(this, &(*p).x, &(*p).y);
}

void _replot_delPixels(Replot *this, u8 color[4]) {
    if (color[0] == color[1] && color[0] == color[2] && color[0] == color[3]) {
        memset(this->buffer, color[0], this->w * this->h * 4 * sizeof(u8));  
        return;
    }
    u32 x, y;
    for (y = 0; y < this->h; y++) {
        for (x = 0; x < this->w; x++) {
            u32 index = (y * 4 * this->w) + x * 4;
            memcpy(this->buffer + index, color, 4 * sizeof(u8));
        }
    }    
}

void Replot_clear(Replot *this, RColor color) {
    _replot_delPixels(this, (RPixel){color.r, color.g, color.b, color.a});
}

void _replot_setPixel(Replot *this, int px, int py, u8 color[4]) {
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

	int index = ((u32)py) * (4 * bufferW) + (u32)px * 4;
	if (color[3] != 255) {
		float alpha = color[3] / 255;
		color[0] = (color[0] * alpha) + ((1 - alpha) * buffer[index]);
		color[1] =  (color[1] * alpha) + ((1 - alpha) * buffer[index + 1]);
		color[2] =  (color[2] * alpha) + ((1 - alpha) * buffer[index + 2]);
		color[3] =  (color[3] * alpha) + ((1 - alpha) * buffer[index + 3]);
	}
    memcpy(buffer + index, color, 4 * sizeof(u8));
}

void _replot_fillPointWithXY(Replot *this, int px, int py) {
    u32 color = _replot_getColor(this, px, py);
    _replot_rotatePointXY(this, &px, &py);
    _replot_setPixel(this, px, py, (u8*)&color);
}

void _replot_fillPoint(Replot *this, RPoint *point) {
    _replot_fillPointWithXY(this, (*point).x, (*point).y);
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
    _replot_fillLineWithXY(this, (*start).x, (*start).y, (*end).x, (*end).y);
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
typedef void (*_TRIANGLE_LINE_FUNC)(Replot *, float, float, int, int);

void _replot_lineTrianleLTR(Replot *this, float x1, float x2, int y, int thickness) {
    _replot_fillLineWithXY(this, x1, y, x1 + thickness, y);
    _replot_fillLineWithXY(this, x2 - thickness, y, x2, y);
}

void _replot_lineTrianleRTL(Replot *this, float x1, float x2, int y, int thickness) {
    _replot_fillLineWithXY(this, x1, y, x1 - thickness, y);
    _replot_fillLineWithXY(this, x2 + thickness, y, x2, y);
}

void _replot_lineTrianleRunLoop(Replot *this, int fromY, int toY, float *_x1, float *_x2, float x1Slope, float x2Slope, _TRIANGLE_LINE_FUNC func) {
    for(int y = fromY ; y < toY; y++){
        if (y - __triangleThickness <= __triangleMinY || y + __triangleThickness >= __triangleMaxY) {
            _replot_fillLineWithXY(this, *_x1, y, *_x2, y);
        } else {
            int distance = abs(*_x1 - *_x2);
            int thickness = MIN(__triangleThickness, distance);
            func(this, *_x1, *_x2, y, thickness);
        }
        *_x1 += x1Slope;
        *_x2 += x2Slope;
    }
}

void _replot_lineTriangle(Replot *this, int fromY, int toY, float *_x1, float *_x2, float x1Slope, float x2Slope, int thickness) {
    if (thickness <= 0) {
        for(int y = fromY ; y < toY; y++){
            _replot_fillLineWithXY(this, *_x1, y, *_x2, y);
            *_x1 += x1Slope;
            *_x2 += x2Slope;
        }
    } else {
        if (*_x1 <= *_x2) {
            _replot_lineTrianleRunLoop(this, fromY, toY, _x1, _x2, x1Slope, x2Slope, _replot_lineTrianleLTR);
        } else {
            _replot_lineTrianleRunLoop(this, fromY, toY, _x1, _x2, x1Slope, x2Slope, _replot_lineTrianleRTL);
        }
    }
}

void _replot_doTriangle(Replot *this, RPoint points[3], int thickness){
    int x1 = points[0].x;
    int y1 = points[0].y;
    int x2 = points[1].x;
    int y2 = points[1].y;
    int x3 = points[2].x;
    int y3 = points[2].y;
    int left = MIN(MIN(x1, x2), x3);
    int right = MAX(MAX(x1, x2), x3);
    int top = MIN(MIN(y1, y2), y3);
    int bottom = MAX(MAX(y1, y2), y3);
    int w = right - left;
    int h = bottom - top;
    RPoint center = RC_3POINTS_CENTER(points);
    _replot_focusWithWH(this, &center, w, h);
    _replot_applyChanges(this);
    
    if(points[0].y > points[1].y) RC_POINT_SWAP(&points[0], &points[1]);
    if(points[1].y > points[2].y) RC_POINT_SWAP(&points[1], &points[2]);
    if(points[0].y > points[1].y) RC_POINT_SWAP(&points[0], &points[1]);

    __triangleMinY = points[0].y;
    __triangleMaxY = points[2].y;
    __triangleThickness = thickness;
    
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
        _replot_fillLine(this, &points[0], &points[1]);
        _replot_lineTriangle(this, points[0].y, points[2].y, &_x1, &_x2, slope02, slope12, thickness);
    }else if(points[1].y == points[2].y){
        if(points[2].x < points[1].x){
            RC_FLOAT_SWAP(&slope01, &slope02);
            RC_POINT_SWAP(&points[1], &points[2]);
        }
        _x1 = points[0].x;
        _x2 = points[0].x;
        _replot_fillLine(this, &points[1], &points[2]);
        _replot_lineTriangle(this, points[0].y, points[2].y, &_x1, &_x2, slope01, slope02, thickness);
    }else{
        _x1 = points[0].x;
        _x2 = points[0].x;
        _replot_lineTriangle(this, points[0].y, points[1].y, &_x1, &_x2, slope02, slope01, thickness);
        _replot_lineTriangle(this, points[1].y, points[2].y, &_x1, &_x2, slope02, slope12, thickness);
    }
    
}

void Replot_drawTriangle(Replot *this, RPoint point1, RPoint point2, RPoint point3) {
    RPoint points[3] = {point1, point2, point3};
    _replot_doTriangle(this, points, 1);
}

void Replot_fillTriangle(Replot *this, RPoint point1, RPoint point2, RPoint point3) {
    RPoint points[3] = {point1, point2, point3};
    _replot_doTriangle(this, points, 0);
}

void _Replot_doRect(Replot *this, RPoint point, RSize size, bool isFill) {
    _replot_focusWithWH(this, &point, size.w, size.h);
    _replot_applyChanges(this);
    int rw = size.w / 2;
    int rh = size.h / 2;
    RPoint ltp = RPOINT(point.x - rw, point.y - rh);
    RPoint lbp = RPOINT(point.x - rw, point.y + rh - 1);
    RPoint rbp = RPOINT(point.x + rw - 1, point.y + rh - 1);
    RPoint rtp = RPOINT(point.x + rw - 1, point.y - rh);
    _replot_fillLine(this, &ltp, &lbp);	
    _replot_fillLine(this, &lbp, &rbp);	
    _replot_fillLine(this, &rbp, &rtp);	
    _replot_fillLine(this, &rtp, &ltp);
    if (!isFill) return; 
    for (int y = ltp.y; y <= rbp.y; y++) {
        _replot_fillLineWithXY(this, ltp.x, y, rtp.x, y); // Draw a horizontal line at each y
    }
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
    float angle = _replot_math_angle(x, y);
    if (angle >= __circularAngleFrom && angle <= __circularAngleTo) {
        _replot_fillPointWithXY(this, cx + x, cy + y);
    }
}

void _replot_pointCircularNoAngle(Replot *this, int cx, int cy, int x, int y) {
    _replot_fillPointWithXY(this, cx + x, cy + y);
}

void _replot_lineCircularRunLoop(Replot *this, RPoint *point, int rx, int ry, _CIRCULAR_POINT_FUNC func) {
    int w2 = rx*rx;
    int h2 = ry*ry;
    int w2h2 = h2*w2;
    for(int y=-ry; y<=ry; y++) {
        for(int x=-rx; x<=rx; x++) {
            int num = x*x*h2+y*y*w2;
            if(num <= w2h2 && num > __circularLimitNum) {
                func(this, (*point).x, (*point).y, x, y);
            }
        }
    }
    if (__circularLimitNum <= 0) {
        _replot_fillPointWithXY(this, (*point).x, (*point).y);
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
        _replot_lineCircularRunLoop(this, &point, rx, ry, _replot_pointCircularNoAngle);
    } else {
        _replot_lineCircularRunLoop(this, &point, rx, ry, _replot_pointCircularWithAngle);
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

void _replot_doPolygon(Replot *this, RPoint point, int radius, int sides, bool isFill) {
    int centerX = point.x;
    int centerY = point.y;
    _replot_focusWithWH(this, &point, radius * 2, radius * 2);
    _replot_applyChanges(this);
    // Calculate the angle between each vertex
    double angleStep = 2 * M_PI / sides;
    RPoint ctr = RPOINT(centerX, centerY);
    // Calculate the vertices and draw lines between them
    int x0, y0, x1, y1;
    for (int i = 0; i < sides; i++) {
        // Calculate the current vertex
        x0 = centerX + (int)(radius * cos(i * angleStep));
        y0 = centerY + (int)(radius * sin(i * angleStep));

        // Calculate the next vertex (wrap around using modulo)
        x1 = centerX + (int)(radius * cos((i + 1) * angleStep));
        y1 = centerY + (int)(radius * sin((i + 1) * angleStep));

        RPoint p0 = RPOINT(x0, y0);
        RPoint p1 = RPOINT(x1, y1);
        RPoint pm = RC_POINT_MIDDLE(p0, p1);
        _replot_fillLine(this, &p0, &p1);
        //
        if (!isFill) continue;
        //
        for (float t = 0; t <= 1; t += 0.001) {
            RPoint _p0 = RC_VECTOR_LERP(p0, ctr, t);
            RPoint _p1 = RC_VECTOR_LERP(p1, ctr, t);
            RPoint _pm = RC_POINT_MIDDLE(_p0, _p1);
            _replot_fillLine(this, &_p0, &_p1);
            _replot_fillLine(this, &p0, &_p1);
            _replot_fillLine(this, &p1, &_p0);
            _replot_fillLine(this, &pm, &_p1);
            _replot_fillLine(this, &pm, &_p0);
            if (t>1) {
                _replot_fillLine(this, &p0, &_p0);
                _replot_fillLine(this, &p1, &_p1);
                break;
            };
        }
    }
}

void Replot_drawPolygon(Replot *this, RPoint point, int radius, int angles) {
    _replot_doPolygon(this, point, radius, angles, false);
}

void Replot_fillPolygon(Replot *this, RPoint point, int radius, int angles) {
    _replot_doPolygon(this, point, radius, angles, true);
}

/////////////////////////////////////////////////////

void _Replot_doPrint(Replot *this, int cx, int cy, char ch, int size) {
    ch = tolower(ch);
    int sw = _REPLOT_FONT_WIDTH * size;
    int sh = _REPLOT_FONT_HEIGHT * size;
    // _Replot_doRect(this, RPOINT(cx, cy), RSIZE(sw + 2, sh + 2), false);
    int isFat = _REPLOT_FONT_FLAG[ch][0];
    if (!isFat) cx = cx + size / 2;
    for (int i = 0; i < _REPLOT_FONT_HEIGHT; i++) {
        int charLine = _REPLOT_FONT_GLYPHS[ch][i];
        for (int j = 0; j < _REPLOT_FONT_WIDTH; j++) {
            int charDot = (charLine >> j) & 1;
            if (!charDot) continue;
            _replot_focusWithXYWH(this, cx, cy, sw, sh);
            _replot_applyChanges(this);
            int y = cy + size * (i - _REPLOT_FONT_HEIGHT / 2);
            int x = cx + size * (_REPLOT_FONT_WIDTH / 2 - j);
            _replot_rotatePointXY(this, &x, &y);
            _Replot_doRect(this, RPOINT(x, y), RSIZE(size, size), true);
        }
    }
}

void Replot_printChar(Replot *this, RPoint point, int size, char ch) {
    _Replot_doPrint(this, point.x, point.y, ch, size);
}

int __textPosX[1024];
int __textPosY[1024];

void Replot_printText(Replot *this, RPoint point, int size, char *text) {
    int length = strlen(text);
    int distance = size * _REPLOT_FONT_WIDTH + 1;
    int sw = distance * length;
    int sh = size * _REPLOT_FONT_HEIGHT;
    int rotation = this->drawRotation;
    _replot_focusWithWH(this, &point, sw, sh);
    _replot_applyChanges(this);
    // _Replot_doRect(this, point, RSIZE(sw, sh), false);
    for (int i = 0; i < length; i++) {
        int x = point.x + distance * (i - length / 2.0f + 0.5);
        int y = point.y;
        _replot_rotatePointXY(this, &x, &y);
        __textPosX[i] = x;
        __textPosY[i] = y;
    } 
    for (int i = 0; i < length; i++) {
        char ch = text[i];
        int x = __textPosX[i];
        int y = __textPosY[i];
        _Replot_doPrint(this, x, y, ch, size);
    } 
}

/////////////////////////////////////////////////////

void _replot_validatePointSize(RPoint *point, RSize *size, int w, int h) {
    (*point).x = MAX(0, MIN(w, (*point).x));
    (*point).y = MAX(0, MIN(h, (*point).y));
    (*size).w = MIN(w - (*point).x, (*size).w / 2) * 2;
    (*size).h = MIN(h - (*point).y, (*size).h / 2) * 2;
}

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
    RPoint frmP = RPOINT((*rplt).w / 2, (*rplt).h / 2);
    RSize frmS = RSIZE((*rplt).w, (*rplt).h);
    _replot_setStencil(this, rplt->buffer, rplt->w, rplt->h);
    _replot_limit(this, &frmP, &frmS);
    _Replot_doRect(this, point, size, true);
    _replot_delStencil(this);
}

// TODO:fix bug
Replot *Replot_copy(Replot *this, RPoint point, RSize size) {
    Replot *rplt = Replot_new(size.w, size.h);
    RPoint toP = RPOINT(size.w / 2, size.h / 2);
    Replot_drawCanvasExt(rplt, toP, size, this, point, size);
    return rplt;
}

// TODO:fix bug
void Replot_crop(Replot *this, RPoint point, RSize size) {
    _replot_setStencil(this, this->buffer, this->w, this->h);
    _replot_limit(this, &point, &size);
    _replot_prepare(this, size.w, size.h);
    RPoint toP = RPOINT(size.w / 2, size.h / 2);
    _Replot_doRect(this, toP, size, true);
    _replot_delStencil(this);
}

/////////////////////////////////////////////////////

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
        RPoint point = RPOINT(w / 2, h / 2);
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

void Replot_free(Replot *this) {
    _replot_delStencil(this);
    free(this->buffer);
    free(this);
}

#endif
