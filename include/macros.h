#ifndef MACROS_H
#define MACROS_H

// macro helpers

#define SWALLOW_SEMICOLON()						__static_assert(true, "")

// keywords

#ifndef alignas
# define alignas									ATTR_ALIGN
#endif

#ifndef decltype
# define decltype									__decltype__
#endif

// attributes

#define ATTR_ALIGN(x)								__attribute__((aligned(x)))
#define ATTR_UNUSED									__attribute__((unused))
#define ATTR_WEAK									__attribute__((weak))

#if defined(__MWERKS__)
# define AT_ADDRESS(x)								: x
# define ATTR_FALLTHROUGH							/* no known attribute, but mwcc doesn't seem to care */
# define ATTR_NOINLINE								__attribute__((never_inline))
#else
# define AT_ADDRESS(x)
# define ATTR_FALLTHROUGH							__attribute__((fallthrough))
# define ATTR_NOINLINE								__attribute__((noinline))
#endif

// useful stuff

#define MIN(x, y)									((x) < (y) ? (x) : (y))
#define MIN_EQ(x, y)								((x) <= (y) ? (x) : (y))
#define CLAMP(x, low, high)							((x) <  (low) ? (low) : (x) > (high) ? (high) : (x))

#define ROUND_UP(x, align)							(((x) + ((align) - 1)) & -(align))
#define ROUND_DOWN(x, align)						( (x)                  & -(align))
#define ROUND_DOWN_PTR(x, align)					((void *)((unsigned long)(x) & -(align)))

#define POINTER_ADD_TYPE(type_, ptr_, offset_)		((type_)((unsigned long)(ptr_) + (unsigned long)(offset_)))
#define POINTER_ADD(ptr_, offset_)					POINTER_ADD_TYPE(__typeof__(ptr_), ptr_, offset_)

#define IS_ALIGNED(x, align)						(((unsigned long)(x) & ((align) - 1)) == 0)

#define ARRAY_LENGTH(x)								(sizeof (x) / sizeof (x)[0])

// boolean macros

#define BOOLIFY_TERNARY_TRUE_TYPE(type_, expr_)		((expr_) ? ((type_)(1)) : ((type_)(0)))
#define BOOLIFY_TERNARY_TRUE(expr_)					BOOLIFY_TERNARY_TRUE_TYPE(int, expr_)

#define BOOLIFY_TERNARY_FALSE_TYPE(type_, expr_)	((expr_) ? ((type_)(0)) : ((type_)(1)))
#define BOOLIFY_TERNARY_FALSE(expr_)				BOOLIFY_TERNARY_FALSE_TYPE(int, expr_)

#define BOOLIFY_TERNARY_TYPE						BOOLIFY_TERNARY_TRUE_TYPE
#define BOOLIFY_TERNARY								BOOLIFY_TERNARY_TRUE

// math

#define M_PI_F										3.1415926f
#define M_TAU										6.283185307179586

#define DEG_TO_RAD_MULT_CONSTANT					(M_PI_F / 180.0f)
#define RAD_TO_DEG_MULT_CONSTANT					(180.0f / M_PI_F)

#define DEG_TO_RAD(x)								((x) * DEG_TO_RAD_MULT_CONSTANT)
#define RAD_TO_DEG(x)								((x) * RAD_TO_DEG_MULT_CONSTANT)

#endif // MACROS_H
