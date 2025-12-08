#ifndef __STDC_STRING_H__
#define __STDC_STRING_H__

#include <__internal/__size_t.h>
#include <__internal/__NULL.h>

#ifdef __cplusplus
	extern "C" {
#endif

extern char *(strcpy)(char * restrict s1, const char * restrict s2);
extern void *(memcpy)(void * restrict s1, const void * restrict s2, size_t n);

extern int (strcmp)(const char *s1, const char *s2);
extern int (memcmp)(const void *s1, const void *s2, size_t n);

extern void *(memset)(void *s, int c, size_t n);

extern size_t (strlen)(const char *s);

#ifdef __cplusplus
	}
#endif

#endif // __STDC_STRING_H__
