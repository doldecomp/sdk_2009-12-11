#ifndef RVL_SDK_ENC_CHINESE_H
#define RVL_SDK_ENC_CHINESE_H

/*******************************************************************************
 * headers
 */

#include <uchar.h>

#include <decomp.h>

#include <revolution/enc.h>

/*******************************************************************************
 * external globals
 */

#ifdef __cplusplus
	extern "C" {
#endif

// .rodata
extern char16_t const enc_tbl_cn_mbtowc[];
extern unsigned char const enc_tbl_cn_wctomb[];
extern u16 const enc_offset_cn[];
extern byte2_t const enc_tbl_cnex_mbtowc[];

// .sdata
extern BOOL enc_tbl_cn_loaded;

/*******************************************************************************
 * functions
 */

ENCResult ENCiConvertStringGb2312ToUnicode(char16_t *dst,
                                           unk4_t signed *dstSize,
                                           unsigned char const *src,
                                           unk4_t signed *srcSize,
                                           ENCBreakType breakType);
ENCResult ENCiConvertStringUnicodeToGb2312(unsigned char *dst,
                                           unk4_t signed *dstSize,
                                           char16_t const *src,
                                           unk4_t signed *srcSize,
                                           ENCBreakType breakType);

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_ENC_CHINESE_H
