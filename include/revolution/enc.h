#ifndef RVL_SDK_ENC_H
#define RVL_SDK_ENC_H

/*******************************************************************************
 * headers
 */

#include <uchar.h>

#include <decomp.h>

#include "revolution/types.h"

/*******************************************************************************
 * macros
 */

// Fun Fact! Both of these are reserved identifiers and technically UB.
#define ENC_SRC_UNLIMITED	(-1)
#define ENC_DST_INVALID		(-1)

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef unk4_t signed ENCResult;
enum ENCResult_et
{
	ENC_ERR_OK				=  0,
	ENC_ERR_NO_SPACE_IN_DST	= -1,
	ENC_ERR_2				= -2, // cannot be represented in destination?
	ENC_ERR_BUFFER_INVALID	= -3,
	ENC_ERR_SRC_MALFORMED	= -4,

// Fun Fact! All of these are reserved identifiers and technically UB.
#define ENC_ESUCCESS	ENC_ERR_OK
#define ENC_ENOSPC		ENC_ERR_NO_SPACE_IN_DST
#define ENC_E2			ENC_ERR_2
#define ENC_EINVAL		ENC_ERR_BUFFER_INVALID
#define ENC_EILSEQ		ENC_ERR_SRC_MALFORMED
};

typedef unk4_t signed ENCState; // akin to mbstate_t

// <uchar.h> lists the Unicode typedefs. As for the rest...
typedef unsigned char ascii_t; // Plain ASCII (SBCS)
typedef byte_t utf7_t; // UTF-7 (Stateful)
typedef byte_t jis_t, full_jis_t[2]; // ISO-2022-JP-EXT (Stateful)
typedef byte_t sjis_t, full_sjis_t[2]; // Shift_JIS (MBCS)

typedef unk4_t ENCEndianness;
enum ENCEndianness_et
{
	ENC_BIG_ENDIAN,
	ENC_LITTLE_ENDIAN
};

/*******************************************************************************
 * functions
 */

/* encunicode.c */

ENCResult ENCConvertStringUnicodeToAscii(unsigned char *dst,
                                         unk_t signed *dstSize,
                                         char16_t const *src,
                                         unk_t signed *srcSize);
ENCResult ENCConvertStringAsciiToUnicode(char16_t *dst, unk_t signed *dstSize,
                                         unsigned char const *src,
                                         unk_t signed *srcSize);
ENCResult ENCConvertStringUtf32ToUtf16(char16_t *dst, unk_t signed *dstSize,
                                       char32_t const *src,
                                       unk_t signed *srcSize);
ENCResult ENCConvertStringUtf16ToUtf32(char32_t *dst, unk_t signed *dstSize,
                                       char16_t const *src,
                                       unk_t signed *srcSize);
ENCResult ENCConvertStringUtf32ToUtf8(char8_t *dst, unk_t signed *dstSize,
                                      char32_t const *src,
                                      unk_t signed *srcSize);
ENCResult ENCConvertStringUtf8ToUtf32(char32_t *dst, unk_t signed *dstSize,
                                      char8_t const *src,
                                      unk_t signed *srcSize);
ENCResult ENCConvertStringUtf16ToUtf8(char8_t *dst, unk_t signed *dstSize,
                                      char16_t const *src,
                                      unk_t signed *srcSize);
ENCResult ENCConvertStringUtf8ToUtf16(char16_t *dst, unk_t signed *dstSize,
                                      char8_t const *src,
                                      unk_t signed *srcSize);
ENCResult ENCConvertStringUtf16ToUtf16(char16_t *dst, unk_t signed *dstSize,
                                       char16_t const *src,
                                       unk_t signed *srcSize);
ENCResult ENCConvertStringUtf16LEToUtf16BE(char16_t *dst, unk_t signed *dstSize,
                                           char16_t const *src,
                                           unk_t signed *srcSize);
ENCResult ENCConvertStringUtf7ToUtf16(char16_t *dst, unk_t signed *dstSize,
                                      unsigned char const *src,
                                      unk_t signed *srcSize);

ENCResult ENCSetUnicodeBOM(char16_t *dst, unk_t signed dstSize);
ENCResult ENCSetUnicodeBOM32(char32_t *dst, unk_t signed dstSize);
ENCResult ENCSetUnicodeBOM16(char16_t *dst, unk_t signed dstSize);
ENCResult ENCSetUnicodeBOM8(char8_t *dst, unk_t signed dstSize);

/* encjapanese.c */

ENCResult ENCConvertStringSjisToUnicode(char16_t *dst, unk4_t signed *dstSize,
                                        sjis_t const *src,
                                        unk4_t signed *srcSize);
ENCResult ENCConvertStringUnicodeToSjis(sjis_t *dst, unk4_t signed *dstSize,
                                        char16_t const *src,
                                        unk4_t signed *srcSize);
ENCResult ENCConvertStringJisToUnicode(char16_t *dst, unk4_t signed *dstSize,
                                       jis_t const *src,
                                       unk4_t signed *srcSize);
ENCResult ENCConvertStringUnicodeToJis(jis_t *dst, unk4_t signed *dstSize,
                                       char16_t const *src,
                                       unk4_t signed *srcSize);
ENCResult ENCConvertStringJisToSjis(sjis_t *dst, unk4_t signed *dstSize,
                                    jis_t const *src, unk4_t signed *srcSize);
ENCResult ENCConvertStringSjisToJis(jis_t *dst, unk4_t signed *dstSize,
                                    sjis_t const *src, unk4_t signed *srcSize);

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_ENC_H
