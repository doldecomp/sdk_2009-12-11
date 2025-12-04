#ifndef RVL_SDK_ENC_JAPANESE_H
#define RVL_SDK_ENC_JAPANESE_H

/*******************************************************************************
 * headers
 */

#include <uchar.h>

#include <decomp.h>

#include <revolution/enc.h>

#include "encutility.h"

/*******************************************************************************
 * functions
 */

#ifdef __cplusplus
	extern "C" {
#endif

ENCResult ENCiConvertStringSjisToUnicode(char16_t *dst, unk4_t signed *dstSize,
                                         sjis_t const *src,
                                         unk4_t signed *srcSize,
                                         ENCBreakType breakType);
ENCResult ENCiConvertStringUnicodeToSjis(sjis_t *dst, unk4_t signed *dstSize,
                                         char16_t const *src,
                                         unk4_t signed *srcSize,
                                         ENCBreakType breakType);
ENCResult ENCiConvertStringJisToSjis(sjis_t *dst, unk4_t signed *dstSize,
                                     jis_t const *src, unk4_t signed *srcSize,
                                     ENCBreakType breakType, ENCState *state);
ENCResult ENCiConvertStringSjisToJis(jis_t *dst, unk4_t signed *dstSize,
                                     sjis_t const *src, unk4_t signed *srcSize,
                                     ENCBreakType breakType, ENCState *state);
ENCResult ENCiConvertStringJisToUnicode(char16_t *dst, unk4_t signed *dstSize,
                                        jis_t const *src,
                                        unk4_t signed *srcSize,
                                        ENCBreakType breakType,
                                        ENCState *state);
ENCResult ENCiConvertStringUnicodeToJis(jis_t *dst, unk4_t signed *dstSize,
                                        char16_t const *src,
                                        unk4_t signed *srcSize,
                                        ENCBreakType breakType,
                                        ENCState *state);

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_ENC_JAPANESE_H
