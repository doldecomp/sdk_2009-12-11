#ifndef __INTERNAL_SIZE_T_H__
#define __INTERNAL_SIZE_T_H__

#ifdef __cplusplus
	extern "C" {
#endif

typedef unsigned long int size_t;

#if defined(__cplusplus)
namespace std { extern "C" { using ::size_t; } }
#endif

#ifdef __cplusplus
	}
#endif

#endif // __INTERNAL_SIZE_T_H__
