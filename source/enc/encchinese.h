#ifndef RVL_SDK_ENC_CHINESE_H
#define RVL_SDK_ENC_CHINESE_H

/*******************************************************************************
 * headers
 */

#include <uchar.h> // char16_t

#include <revolution/types.h>

#include <revolution/enc.h>
#include "encmacros.h"

/*******************************************************************************
 * external globals
 */

#ifdef __cplusplus
	extern "C" {
#endif

// .rodata
extern char16_t const enc_tbl_cn_mbtowc[];
extern byte_t const enc_tbl_cn_wctomb[];
extern u16 const enc_offset_cn[];
extern byte2_t const enc_tbl_cnex_mbtowc[];

// .sdata
extern BOOL enc_tbl_cn_loaded;

/*******************************************************************************
 * functions
 */

DECLARE_PRIVATE_ENC_FUNCTION_TO_UTF16(Gb2312)
DECLARE_PRIVATE_ENC_FUNCTION_FROM_UTF16(Gb2312)

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_ENC_CHINESE_H
