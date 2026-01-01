#ifndef RVL_SDK_ENC_CONVERT_H
#define RVL_SDK_ENC_CONVERT_H

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

#include <revolution/enc.h> // ENCEncoding

/*******************************************************************************
 * macros
 */

#define ENCODING_NAME_LENGTH					16
#define ENCODING_HASH_TABLE_ENTRY_NAME_LENGTH	24

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

struct encoding
{
	u32			width;						// size 0x04, offset 0x00
	BOOL		is7bit;						// size 0x04, offset 0x04
	char const	name[ENCODING_NAME_LENGTH];	// size 0x10, offset 0x08
}; // size 0x18

struct encoding_hash_table_entry
{
	char const	name[ENCODING_HASH_TABLE_ENTRY_NAME_LENGTH];	// size 0x18, offset 0x00
	ENCEncoding	encoding;										// size 0x04, offset 0x18
	int			nextIndex;										// size 0x04, offset 0x1c
}; // size 0x20

/*******************************************************************************
 * external globals
 */

// .rodata
extern struct encoding const encoding_array[];
extern struct encoding_hash_table_entry const encoding_table[];

// .sdata
extern BOOL enc_tbl_jp_loaded;
extern BOOL enc_tbl_kr_loaded;
extern BOOL enc_tbl_uhc_loaded;
extern BOOL enc_tbl_cn_loaded;

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_ENC_CONVERT_H
