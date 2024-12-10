// RSoft
// https://github.com/ColleagueRiley/rsoft

/*
* Copyright (C) 2024 ColleagueRiley
*
* libpng license
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgment in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
*
*/


#ifndef RSOFT_MALLOC
	#include <stdlib.h>

	#ifndef __USE_POSIX199309
	#define __USE_POSIX199309
	#endif

	#include <time.h>
	#define RSOFT_MALLOC malloc
	#define RSOFT_CALLOC calloc
	#define RSOFT_FREE free
#endif


#if !_MSC_VER
	#ifndef inline
		#ifndef __APPLE__
			#define inline __inline
		#endif
	#endif
#endif

#if defined(RSOFT_EXPORT) ||  defined(RSOFT_IMPORT)
	#if defined(_WIN32)
		#if defined(__TINYC__) && (defined(RSOFT_EXPORT) ||  defined(RSOFT_IMPORT))
			#define __declspec(x) __attribute__((x))
		#endif

		#if defined(RSOFT_EXPORT)
			#define RSOFTDEF __declspec(dllexport)
		#else 
			#define RSOFTDEF __declspec(dllimport)
		#endif
	#else
		#if defined(RSOFT_EXPORT)
			#define RSOFTDEF __attribute__((visibility("default")))
		#endif
	#endif
#endif 

#ifndef RSOFTDEF
	#ifdef __clang__
		#define RSOFTDEF static inline
	#else
		#define RSOFTDEF inline
	#endif
#endif

#ifndef RSOFT_ENUM
	#define RSOFT_ENUM(type, name) type name; enum
#endif

#ifndef RSOFT_UNUSED
	#define RSOFT_UNUSED(x) (void)(x);
#endif

#if defined(__cplusplus) && !defined(__EMSCRIPTEN__)
	extern "C" {
#endif

	/* makes sure the header file part is only defined once by default */
#ifndef RSOFT_HEADER

#define RSOFT_HEADER

#if !defined(u8)
	#if ((defined(_MSC_VER) || defined(__SYMBIAN32__)) && !defined(RSOFT_STD_INT)) /* MSVC might not have stdint.h */
		typedef unsigned char 	u8;
		typedef signed char		i8;
		typedef unsigned short  u16;
		typedef signed short 	i16;
		typedef unsigned int 	u32;
		typedef signed int		i32;
		typedef unsigned long	u64;
		typedef signed long		i64;
	#else /* use stdint standard types instead of c ""standard"" types */
		#include <stdint.h>

		typedef uint8_t     u8;
		typedef int8_t      i8;
		typedef uint16_t   u16;
		typedef int16_t    i16;
		typedef uint32_t   u32;
		typedef int32_t    i32;
		typedef uint64_t   u64;
		typedef int64_t    i64;
	#endif
#endif

#if !defined(b8) /* RSOFT bool type */
	typedef u8 b8;
	typedef u32 b32;
#endif

#define RSOFT_TRUE 1
#define RSOFT_FALSE 0

#ifndef RSoft_point
typedef struct RSoft_point { i32 x, y, z; } RSoft_point;
#endif

#define RSOFT_POINT(x, y) (RSoft_point){x, y}

#ifndef RSoft_area
typedef struct RSoft_area { u32 w, h; } RSoft_area;
#endif

#define RSOFT_AREA(w, h) (RSoft_area){w, h}

#ifndef RSoft_rect
typedef struct RSoft_rect { i32 x, y;  u32 w, h; } RSoft_rect;
#endif

#define RSOFT_RECT(x, y, w, h) (RSoft_rect){x, y, w, h}

#ifndef RSoft_vector
typedef struct RSoft_vector { float x, y, z; } RSoft_vector;
#endif

#define RSOFT_VECTOR2D(x, y) (RSoft_vector){x, y, 1}
#define RSOFT_VECTOR3D(x, y, z) (RSoft_vector){x, y, z}

#ifndef RSoft_rectF
typedef struct RSoft_rectF { float x, y, w, h; } RSoft_rectF;
#endif

#define RSOFT_RECTF(x, y, w, h) (RSoft_rectF){x, y, w, h}

#include <math.h>

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif


#ifndef DEG2RAD
#define DEG2RAD M_PI/180
#endif

/* LUT based trig functions (uses degrees) */
RSOFTDEF float RSoft_sin(i32 angle);
RSOFTDEF float RSoft_cos(i32 angle);

 // 4 * 4 matrix
typedef struct RSoft_matrix { float m[16];} RSoft_matrix;

RSOFTDEF void RSoft_copyBuffer(u8* dest, size_t width, u8* src, RSoft_rect rect);

RSOFTDEF void RSoft_setMatrix(RSoft_matrix matrix);
RSOFTDEF RSoft_matrix RSoft_initMatrix(void);
RSOFTDEF RSoft_matrix RSoft_multiplyMatrix(float left[16], float right[16]);
RSOFTDEF RSoft_matrix RSoft_rotateMatrix(RSoft_matrix matrix, float angle);
RSOFTDEF RSoft_matrix RSoft_translateMatrix(RSoft_matrix matrix, RSoft_vector v);
RSOFTDEF RSoft_vector RSoft_applyMatrix(RSoft_matrix matrix, RSoft_vector v);

#endif /* RSOFT_HEADER */

#ifdef RSOFT_IMPLEMENTATION

#ifdef __x86_64__
#define RSOFT_X86
#include <xmmintrin.h> 
#include <x86intrin.h>
#endif

typedef struct RSoft_renderInfoStruct {
	RSoft_matrix matrix;
	u8* texture; 
} RSoft_renderInfoStruct;

RSoft_renderInfoStruct RSoft_renderInfo = {.matrix = (RSoft_matrix) {{
	/* x */  1.0f, 0.0f, 0.0f, 0.0f,
	/* y */	 0.0f, 1.0f, 0.0f, 0.0f,
	/* z */	 0.0f, 0.0f, 1.0f, 0.0f,
    /* w */  0.0f, 0.0f, 0.0f, 1.0f
    }},
	.texture = NULL
};

float RSoft_sin(i32 angle) {
	static float sinLUT[360] = {1};
	if (sinLUT[0]) {
		for (size_t i = 0; i < 360; i++)
			sinLUT[i] = sin(i * DEG2RAD);
	}

	while (angle < 0) angle += 360;	

	if (angle >= 360)
		angle -= 360 * (u32)(angle / 360);
	
	return sinLUT[angle];
}

float RSoft_cos(i32 angle) {
	static float cosLUT[360] = {0};
	if (cosLUT[0] == 0) {
		for (size_t i = 0; i < 360; i++)
			cosLUT[i] = cos(i * DEG2RAD);
	}
	
	while (angle < 0) angle += 360;
	
	if (angle >= 360)
		angle -= 360 * (u32)(angle / 360);

	return cosLUT[angle];
}

void RSoft_copyBuffer(u8* dest, size_t width, u8* src, RSoft_rect rect) {
	u32 y;
    for (y = 0; y < (u32)rect.h; y++) {
        u32 index = ((rect.y + y) * (4 * width)) + rect.x * 4;
        memcpy(dest + index, src + (4 * rect.w * y), rect.w * 4 * sizeof(u8));
    }
}

void RSoft_setMatrix(RSoft_matrix matrix) {
	RSoft_renderInfo.matrix = matrix;
}

	
RSoft_matrix RSoft_initMatrix(void) {
	/* sets the x, y, z, w base values to 1.0 (so we aren't multiplying by zero) */
	return (RSoft_matrix) {{
    //		 x	   y	 z	   w	
	/* x */  1.0f, 0.0f, 0.0f, 0.0f,
	/* y */	 0.0f, 1.0f, 0.0f, 0.0f,
	/* z */	 0.0f, 0.0f, 1.0f, 0.0f,
    /* w */  0.0f, 0.0f, 0.0f, 1.0f
    }};
}

RSoft_matrix RSoft_multiplyMatrix(float left[16], float right[16]) {
	return (RSoft_matrix) {{
		(left[0] * right[0]) + (left[1] * right[4]) + (left[2] * right[8]) + (left[3] * right[12]),
		(left[0] * right[1]) + (left[1] * right[5]) + (left[2] * right[9]) + (left[3] * right[13]),
		(left[0] * right[2]) + (left[1] * right[6]) + (left[2] * right[10]) + (left[3] * right[14]),
		(left[0] * right[3]) + (left[1] * right[7]) + (left[2] * right[11]) + (left[3] * right[15]),
		(left[4] * right[0]) + (left[5] * right[4]) + (left[6] * right[8]) + (left[7] * right[12]),
		(left[4] * right[1]) + (left[5] * right[5]) + (left[6] * right[9]) + (left[7] * right[13]),
		(left[4] * right[2]) + (left[5] * right[6]) + (left[6] * right[10]) + (left[7] * right[14]),
		(left[4] * right[3]) + (left[5] * right[7]) + (left[6] * right[11]) + (left[7] * right[15]),
		(left[8] * right[0]) + (left[9] * right[4]) + (left[10] * right[8]) + (left[11] * right[12]),
		(left[8] * right[1]) + (left[9] * right[5]) + (left[10] * right[9]) + (left[11] * right[13]),
		(left[8] * right[2]) + (left[9] * right[6]) + (left[10] * right[10]) + (left[11] * right[14]),
		(left[8] * right[3]) + (left[9] * right[7]) + (left[10] * right[11]) + (left[11] * right[15]),
		(left[12] * right[0]) + (left[13] * right[4]) + (left[14] * right[8]) + (left[15] * right[12]),
		(left[12] * right[1]) + (left[13] * right[5]) + (left[14] * right[9]) + (left[15] * right[13]),
		(left[12] * right[2]) + (left[13] * right[6]) + (left[14] * right[10]) + (left[15] * right[14]),
		(left[12] * right[3]) + (left[13] * right[7]) + (left[14] * right[11]) + (left[15] * right[15])
	}};
}

RSoft_matrix RSoft_rotateMatrix(RSoft_matrix matrix, float angle) {
	float sinres = RSoft_sin(angle);
	float cosres = RSoft_cos(angle);
	
	/* this acts a 4 x 4 2D matrix */
	float rotateMatrix[16] = {
					cosres,						-sinres,					0.0f,   	0.0f,
					sinres,						cosres,						0.0f,		0.0f,
					0.0f,						0.0f,						1.0f,		0.0f,
					0.0f,   					0.0f,   					0.0f,   	1.0f
	};

	return RSoft_multiplyMatrix(matrix.m, rotateMatrix);
}

RSoft_matrix RSoft_skewMatrix(RSoft_matrix matrix, float sx, float sy) {
	float scaleMatrix[16] = {
		1.0f,	sy,			0.0f,   	0.0f,
		sx,	    1.0f,		0.0f,		0.0f,
		0.0f,	0.0f,		1.0f,		0.0f,
		0.0f,	0.0f,		0.0f,		1.0f
	};
	return RSoft_multiplyMatrix(matrix.m, scaleMatrix);
}

RSoft_matrix RSoft_scaleMatrix(RSoft_matrix matrix, float sx, float sy) {
	float scaleMatrix[16] = {
		sx,		0.0f,		0.0f,   	0.0f,
		0.0f,	sy,			0.0f,		0.0f,
		0.0f,	0.0f,		1.0f,		0.0f,
		0.0f,	0.0f,		0.0f,		1.0f
	};
	return RSoft_multiplyMatrix(matrix.m, scaleMatrix);
}

RSoft_matrix RSoft_translateMatrix(RSoft_matrix matrix, RSoft_vector v) {
	/* this acts a 4 x 4 2D matrix */
	float rotateMatrix[16] = {
					1.0,						0.0,						0.0f,   	0.0f,
					0.0,						1.0,						0.0f,		0.0f,
					0.0,						0.0f,						1.0f,		0.0f,
					v.x,						v.y,						v.z,		1.0f
	};

	return RSoft_multiplyMatrix(matrix.m, rotateMatrix);
}

RSoft_vector RSoft_applyMatrix(RSoft_matrix matrix, RSoft_vector v) {
	return (RSoft_vector) {
		//	x * Mx[x]	    +    y * My[x]		  +    z * Mz[x]		+ 1 * Mw[x]			 			 
		(v.x * matrix.m[0]) + (v.y * matrix.m[4]) + (v.z * matrix.m[8]) + matrix.m[12],
		(v.x * matrix.m[1]) + (v.y * matrix.m[5]) + (v.z * matrix.m[9]) + matrix.m[13],
		(v.x * matrix.m[2]) + (v.y * matrix.m[6]) + (v.z * matrix.m[10]) + matrix.m[14],
	}; 
}

int math_round(float n) {
    return (int)(n + 0.5);
}

long long time_now() {
    long long now = (long long)(clock() * 1000 / CLOCKS_PER_SEC);
	return now;
}

void RMatrix_matrixRotate(RSoft_matrix matrix, int *_px, int *_py) {	 			 
    int px = *_px * 1.0f;
    int py = *_py * 1.0f;
    int pz = 0;
    float __px = (px * matrix.m[0]) + (py * matrix.m[4]) + (pz * matrix.m[8]) + matrix.m[12];
    float __py = (px * matrix.m[1]) + (py * matrix.m[5]) + (pz * matrix.m[9]) + matrix.m[13];
    pz = (px * matrix.m[2]) + (py * matrix.m[6]) + (pz * matrix.m[10]) + matrix.m[14];
	*_px = math_round(__px);
	*_py = math_round(__py);
}

#endif
