#ifndef __STDC_UCHAR_H__
#define __STDC_UCHAR_H__

/* NOTE: <uchar.h>, char16_t, and char32_t were added in C11, and char8_t in
 * C23; they are provided here as a backport extension because they are
 * nonetheless useful here as annotation types.
 */

#include <__internal/__least.h>

typedef unsigned char char8_t;
typedef uint_least16_t char16_t;
typedef uint_least32_t char32_t;

#endif // __STDC_UCHAR_H__
