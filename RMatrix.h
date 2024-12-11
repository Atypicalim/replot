
#include "RDefine.h"

#ifndef REPLOT_MATRIX_H
#define REPLOT_MATRIX_H

 // 3 * 3 matrix
typedef struct RMatrix { float m[9];} RMatrix;

RMatrix RMatrix_initMatrix(void);
RMatrix RMatrix_multiplyMatrix(float left[9], float right[9]);
RMatrix RMatrix_rotateMatrix(RMatrix matrix, float angle);
RMatrix RMatrix_translateMatrix(RMatrix matrix, RVector v);
RPoint RMatrix_applyMatrix(RMatrix matrix, RPoint v);

RMatrix RMatrix_initMatrix(void) {
	/* sets the x, y, w base values to 1.0 (so we aren't multiplying by zero) */
	return (RMatrix) {{
    //		 x	   y	 w	
	/* x */  1.0f, 0.0f, 0.0f,
	/* y */	 0.0f, 1.0f, 0.0f,
	/* w */	 0.0f, 0.0f, 1.0f
    }};
}

RMatrix RMatrix_multiplyMatrix(float left[16], float right[16]) {
	return (RMatrix) {{
		(left[0] * right[0]) + (left[1] * right[3]) + (left[2] * right[6]),
		(left[0] * right[1]) + (left[1] * right[4]) + (left[2] * right[7]),
		(left[0] * right[2]) + (left[1] * right[5]) + (left[2] * right[8]),
		//
		(left[3] * right[0]) + (left[4] * right[3]) + (left[5] * right[6]),
		(left[3] * right[1]) + (left[4] * right[4]) + (left[5] * right[7]),
		(left[3] * right[2]) + (left[4] * right[5]) + (left[5] * right[8]),
		// 
		(left[6] * right[0]) + (left[7] * right[3]) + (left[8] * right[6]),
		(left[6] * right[1]) + (left[7] * right[4]) + (left[8] * right[7]),
		(left[6] * right[2]) + (left[7] * right[5]) + (left[8] * right[8])
	}};
}

RMatrix RMatrix_rotateMatrix(RMatrix matrix, float angle) {
	float sinres = replot_math_sin(angle);
	float cosres = replot_math_cos(angle);
	float rotateMatrix[9] = {
		cosres,		-sinres,	0.0f,
		sinres,		cosres,		0.0f,
		0.0f,		0.0f,		1.0f
	};
	return RMatrix_multiplyMatrix(matrix.m, rotateMatrix);
}

RMatrix RMatrix_skewMatrix(RMatrix matrix, float sx, float sy) {
	float scaleMatrix[9] = {
		1.0f,	sy,			0.0f,
		sx,	    1.0f,		0.0f,
		0.0f,	0.0f,		1.0f
	};
	return RMatrix_multiplyMatrix(matrix.m, scaleMatrix);
}

RMatrix RMatrix_scaleMatrix(RMatrix matrix, float sx, float sy) {
	float scaleMatrix[9] = {
		sx,		0.0f,		0.0f,
		0.0f,	sy,			0.0f,
		0.0f,	0.0f,		1.0f
	};
	return RMatrix_multiplyMatrix(matrix.m, scaleMatrix);
}

RMatrix RMatrix_translateMatrix(RMatrix matrix, RVector v) {
	float rotateMatrix[9] = {
		1.0,		0.0,		0.0f,
		0.0,		1.0,		0.0f,
		v.x,		v.y,		1.0f
	};
	return RMatrix_multiplyMatrix(matrix.m, rotateMatrix);
}

RPoint RMatrix_applyMatrix(RMatrix matrix, RPoint v) {
	return (RPoint) {
		//	x * Mx[x]	    +    y * My[x]			+ 1 * Mw[x]			 			 
		(v.x * matrix.m[0]) + (v.y * matrix.m[3]) + matrix.m[6],
		(v.x * matrix.m[1]) + (v.y * matrix.m[4]) + matrix.m[7]
	}; 
}

void RMatrix_matrixRotate(RMatrix matrix, int *_px, int *_py) {	 			 
    int px = *_px;
    int py = *_py;
    float __px = (px * matrix.m[0]) + (py * matrix.m[3]) + matrix.m[6];
    float __py = (px * matrix.m[1]) + (py * matrix.m[4]) + matrix.m[7];
	*_px = replot_math_round(__px);
	*_py = replot_math_round(__py);
}

#endif
