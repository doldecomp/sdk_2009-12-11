#ifndef RVL_SDK_ENC_UNICODE_H
#define RVL_SDK_ENC_UNICODE_H

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

ENCResult ENCiConvertStringUnicodeToAscii(ascii_t *dst, unk4_t signed *dstSize,
                                          char16_t const *src,
                                          unk4_t signed *srcSize,
                                          ENCBreakType breakType);
ENCResult ENCiConvertStringAsciiToUnicode(char16_t *dst, unk4_t signed *dstSize,
                                          ascii_t const *src,
                                          unk4_t signed *srcSize,
                                          ENCBreakType breakType);
ENCResult ENCiConvertStringUtf32ToUtf16(char16_t *dst, unk4_t signed *dstSize,
                                        char32_t const *src,
                                        unk4_t signed *srcSize,
                                        ENCBreakType breakType);
ENCResult ENCiConvertStringUtf16ToUtf32(char32_t *dst, unk_t *dstSize,
                                        char16_t const *src, unk_t *srcSize,
                                        ENCBreakType breakType);
ENCResult ENCiConvertStringUtf32ToUtf8(char8_t *dst, unk4_t signed *dstSize,
                                       char32_t const *src,
                                       unk4_t signed *srcSize,
                                       ENCBreakType breakType);
ENCResult ENCiConvertStringUtf8ToUtf32(char32_t *dst, unk4_t signed *dstSize,
                                       char8_t const *src,
                                       unk4_t signed *srcSize,
                                       ENCBreakType breakType);
ENCResult ENCiConvertStringUtf16ToUtf8(char8_t *dst, unk4_t signed *dstSize,
                                       char16_t const *src,
                                       unk4_t signed *srcSize,
                                       ENCBreakType breakType);
ENCResult ENCiConvertStringUtf8ToUtf16(char16_t *dst, unk4_t signed *dstSize,
                                       char8_t const *src,
                                       unk4_t signed *srcSize,
                                       ENCBreakType breakType);
ENCResult ENCiConvertStringUtf7ToUtf16(char16_t *dst, unk4_t signed *dstSize,
                                       utf7_t const *src,
                                       unk4_t signed *srcSize,
                                       ENCBreakType breakType, ENCState *state);
ENCResult ENCiConvertStringUtf16ToUtf16(char16_t *dst, unk4_t signed *dstSize,
                                        char16_t const *src,
                                        unk4_t signed *srcSize,
                                        ENCBreakType breakType,
                                        ENCEndianness *endianness);

#ifdef __cplusplus
	}
#endif

#endif //  RVL_SDK_ENC_UNICODE_H

