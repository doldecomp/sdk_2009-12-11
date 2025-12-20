#ifndef RVL_SDK_ENC_KOREAN_H
#define RVL_SDK_ENC_KOREAN_H

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
extern char16_t const enc_tbl_kr_mbtowc1[];
extern char16_t const enc_tbl_kr_mbtowc2[];
extern byte_t const enc_tbl_kr_wctomb1[];
extern byte_t const enc_tbl_kr_wctomb2[];
extern u16 const enc_offset_kr1[];
extern u16 const enc_offset_kr2[];
extern char16_t const enc_tbl_uhc_mbtowc[];
extern u16 const enc_offset_uhc[];

// .sdata
extern BOOL enc_tbl_kr_loaded;
extern BOOL enc_tbl_uhc_loaded;

/*******************************************************************************
 * functions
 */

DECLARE_PRIVATE_ENC_FUNCTION_TO_UTF16(Uhc)
DECLARE_PRIVATE_ENC_FUNCTION_FROM_UTF16(Uhc)

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_ENC_KOREAN_H
