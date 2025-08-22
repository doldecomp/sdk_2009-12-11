#ifndef __STDC_STDIO_H__
#define __STDC_STDIO_H__

#include <__internal/__size_t.h>
#include <__internal/__va_list.h>

int sprintf(char * restrict s, const char * restrict format, ...);
int vsprintf(char * restrict s, const char * restrict format, va_list arg);

#endif // __STDC_STDIO_H__
