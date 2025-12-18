#ifndef RVL_SDK_ENC_KOREAN_H
#define RVL_SDK_ENC_KOREAN_H

/*******************************************************************************
 * headers
 */

#include <uchar.h>

#include <decomp.h>

#include <revolution/enc.h>
#include "encutility.h"

/*******************************************************************************
 * external globals
 */

#ifdef __cplusplus
	extern "C" {
#endif

/*******************************************************************************
 * functions
 */

ENCResult ENCiConvertStringUhcToUnicode(char16_t *dst, unk4_t signed *dstSize,
                                        unsigned char const *src,
                                        unk4_t signed *srcSize,
                                        ENCBreakType breakType);
ENCResult ENCiConvertStringUnicodeToUhc(unsigned char *dst,
                                        unk4_t signed *dstSize,
                                        char16_t const *src,
                                        unk4_t signed *srcSize,
                                        ENCBreakType breakType);

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_ENC_KOREAN_H
