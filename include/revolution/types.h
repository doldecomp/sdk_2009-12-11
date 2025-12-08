#ifndef TYPES_H
#define TYPES_H

#include <stddef.h> // IWYU pragma: export
#include <stdint.h>

// NULL

#define nullptr					NULL

// Fixed-width types

typedef uint8_t					u8;
typedef uint16_t				u16;
typedef uint32_t				u32;
typedef uint64_t				u64;

typedef  int8_t					s8;
typedef  int16_t				s16;
typedef  int32_t				s32;
typedef  int64_t				s64;

// Floating-point types

typedef float					f32;
typedef double					f64;

// Booleans

typedef int						BOOL;
#define true					1
#define false					0

#define TRUE					true
#define FALSE					false

// Character types (C++ only)

#if defined(__cplusplus) && __cplusplus < 201103L
// If in C, use <uchar.h>.
typedef unsigned char			char8_t;
typedef uint_least16_t			char16_t;
typedef uint_least32_t			char32_t;
#else
// they are keywords by now
#endif

// Byte types

typedef unsigned char			byte1_t;
typedef unsigned short int		byte2_t;
typedef unsigned long int		byte4_t;
typedef unsigned long long int	byte8_t;

typedef byte1_t					byte_t;

// Other types

typedef unsigned long int		register_t;

#endif // TYPES_H
