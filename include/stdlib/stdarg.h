#ifndef __STDC_STDARG_H__
#define __STDC_STDARG_H__

#include <__internal/__va_list.h>

#define va_start(ap, parmN)	((void)(parmN), __builtin_va_info(&ap))
#define va_end(ap)			((void)(0))

#endif // __STDC_STDARG_H__
