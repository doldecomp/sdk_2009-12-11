#ifndef RVL_SDK_ENC_UNICODE_H
#define RVL_SDK_ENC_UNICODE_H

/*******************************************************************************
 * headers
 */

#include <uchar.h>

#include <revolution/enc.h>

#include "encutility.h"

/*******************************************************************************
 * functions
 */

ENCResult ENCiConvertStringUnicodeToAscii(unsigned char *dst,
                                          unk_t signed *dstSize,
                                          char16_t const *src,
                                          unk_t signed *srcSize,
                                          ENCBreakType breakType);
ENCResult ENCiConvertStringAsciiToUnicode(char16_t *dst, unk_t signed *dstSize,
                                          unsigned char const *src,
                                          unk_t signed *srcSize,
                                          ENCBreakType breakType);
ENCResult ENCiConvertStringUtf32ToUtf16(char16_t *dst, unk_t signed *dstSize,
                                        char32_t const *src,
                                        unk_t signed *srcSize,
                                        ENCBreakType breakType);
ENCResult ENCiConvertStringUtf16ToUtf32(char32_t *dst, unk_t *dstSize,
                                        char16_t const *src, unk_t *srcSize,
                                        ENCBreakType breakType);
ENCResult ENCiConvertStringUtf32ToUtf8(char8_t *dst, unk_t signed *dstSize,
                                       char32_t const *src,
                                       unk_t signed *srcSize,
                                       ENCBreakType breakType);
ENCResult ENCiConvertStringUtf8ToUtf32(char32_t *dst, unk_t signed *dstSize,
                                       char8_t const *src,
                                       unk_t signed *srcSize,
                                       ENCBreakType breakType);
ENCResult ENCiConvertStringUtf16ToUtf8(char8_t *dst, unk_t signed *dstSize,
                                       char16_t const *src,
                                       unk_t signed *srcSize,
                                       ENCBreakType breakType);
ENCResult ENCiConvertStringUtf8ToUtf16(char16_t *dst, unk_t signed *dstSize,
                                       char8_t const *src,
                                       unk_t signed *srcSize,
                                       ENCBreakType breakType);
ENCResult ENCiConvertStringUtf7ToUtf16(char16_t *dst, unk_t signed *dstSize,
                                       unsigned char const *src,
                                       unk_t signed *srcSize,
                                       ENCBreakType breakType, byte4_t *state);
ENCResult ENCiConvertStringUtf16ToUtf16(char16_t *dst, unk_t signed *dstSize,
                                        char16_t const *src,
                                        unk_t signed *srcSize,
                                        ENCBreakType breakType,
                                        ENCEndianness *endianness);

#endif //  RVL_SDK_ENC_UNICODE_H

