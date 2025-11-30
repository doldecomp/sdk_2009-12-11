#ifndef __STDC_WCHAR_H__
#define __STDC_WCHAR_H__

#if !defined(__cplusplus) // is a keyword in C++
typedef unsigned short wchar_t;
#endif

typedef unsigned short wint_t;

#define WEOF	((wint_t)(-1))

wchar_t *(wcschr)(wchar_t const *str, wchar_t ch);

#endif // __STDC_WCHAR_H__
