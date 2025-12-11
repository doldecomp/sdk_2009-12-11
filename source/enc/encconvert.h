#ifndef RVL_SDK_ENC_CONVERT_H
#define RVL_SDK_ENC_CONVERT_H

/*******************************************************************************
 * headers
 */

#include <uchar.h>

#include <decomp.h>

#include <revolution/enc.h>
#include "encutility.h"

/*******************************************************************************
 * macros
 */

#define ENCODING_NAME_LENGTH				16
#define ENCODING_TABLE_ENTRY_NAME_LENGTH	24

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

struct encoding
{
	u32					width;						// size 0x04, offset 0x00
	BOOL				is7bit;						// size 0x04, offset 0x04
	unsigned char const	name[ENCODING_NAME_LENGTH];	// size 0x10, offset 0x08
}; // size 0x18

struct encoding_hash
{
	unsigned char const	name[ENCODING_TABLE_ENTRY_NAME_LENGTH];	// size 0x18, offset 0x00
	ENCEncoding			encoding;								// size 0x04, offset 0x18
	long				nextIndex;								// size 0x04, offset 0x1c
}; // size 0x20

/*******************************************************************************
 * external globals
 */

extern struct encoding const encoding_array[];
extern struct encoding_hash const encoding_table[];

/*******************************************************************************
 * functions
 */

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_ENC_CONVERT_H
