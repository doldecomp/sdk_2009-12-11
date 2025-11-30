#ifndef __STDC_MATH_H__
#define __STDC_MATH_H__

#ifdef __cplusplus
	extern "C" {
#endif

extern double (atan)(double x);

extern double (cos)(double x);
extern double (sin)(double x);
extern double (tan)(double x);

extern double (pow)(double x, double y);
extern float (powf)(float x, float y);

extern double (sqrt)(double x);
extern float (sqrtf)(float x);

/* Technically NDEBUG is only supposed to control the definition of the assert()
 * macro in <assert.h>, but it also kinda makes sense to use it like this, so
 */
#if defined(NDEBUG)
inline float (sqrtf)(float x) { return (float)(sqrt)(x); }
inline float (powf)(float x, float y) { return (float)(pow)(x, y); }
#endif

#ifdef __cplusplus
	}
#endif

#endif // __STDC_MATH_H__
