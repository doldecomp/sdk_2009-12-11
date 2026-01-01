#ifndef __STDC_STDINT_H__
#define __STDC_STDINT_H__

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed long int32_t;
typedef signed long long int64_t;

/* TODO: This is for the C++ version of the charX_t types. Redo language support
 * sometime? I saw Amber doing a lang.h in xenoblade. That might be interesting
 * to add here with a prefix flag, but then it would have to be noted somewhere
 * that that's being done and needs to be taken into account when using code
 * from this repo.
 */
#include <__internal/__uint_least_16_32_t.h>

#endif // __STDC_STDINT_H__
