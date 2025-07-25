#ifndef DATA_TYPES_H
#define DATA_TYPES_H

/*******************************************************************************
 * headers
 */

#include <stdint.h>

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef uint8_t UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32;

typedef int8_t INT8;
typedef int16_t INT16;
typedef int32_t INT32;

typedef uint8_t BOOLEAN;

#ifndef TRUE
# define TRUE	1
#endif

#ifndef FALSE
# define FALSE	0
#endif

#ifdef __cplusplus
	}
#endif

#endif // DATA_TYPES_H
