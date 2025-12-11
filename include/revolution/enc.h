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

#define ENC_SRC_UNLIMITED			(-1)
#define ENC_DST_INVALID				(-1)
#define ENC_CHECK_ENCODING_MAX_NUM	24

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

/* NOTE: May be a language enum, as ENCIs7BitEncoding has a (coalesced) ternary
 * without integer constant expression branchless optimization
 */
typedef enum ENCResult
{
	ENC_ERR_OK				=  0,
	ENC_ERR_NO_SPACE_IN_DST	= -1,
	ENC_ERR_2				= -2, // cannot be represented in destination?
	ENC_ERR_BUFFER_INVALID	= -3,
	ENC_ERR_SRC_MALFORMED	= -4,
	ENC_ERR_5				= -5, // is ENC_ENCODING_NONE, or out of range?
	ENC_ERR_6				= -6, // i'm thinking ENOTSUPP for this one?

// Fun Fact! All of these are reserved identifiers and technically UB.
#define ENC_ESUCCESS	ENC_ERR_OK
#define ENC_ENOSPC		ENC_ERR_NO_SPACE_IN_DST
#define ENC_E2			ENC_ERR_2
#define ENC_EINVAL		ENC_ERR_BUFFER_INVALID
#define ENC_EILSEQ		ENC_ERR_SRC_MALFORMED
#define ENC_E5			ENC_ERR_5
#define ENC_E6			ENC_ERR_6
} ENCResult;

typedef unk4_t ENCEncoding;
enum ENCEncoding_et
{
	ENC_ENCODING_NONE			=  0,

	// Can convert from "internal encoding" (UTF-16BE)
	ENC_ENCODING_US_ASCII		=  1,
	ENC_ENCODING_UTF_8			=  2,
	ENC_ENCODING_UTF_16BE		=  3,
	ENC_ENCODING_UTF_32BE		=  4,
	ENC_ENCODING_ISO_8859_1		=  5,
	ENC_ENCODING_ISO_8859_2		=  6,
	ENC_ENCODING_ISO_8859_3		=  7,
	ENC_ENCODING_ISO_8859_7		=  8,
	ENC_ENCODING_ISO_8859_10	=  9,
	ENC_ENCODING_ISO_8859_15	= 10,
	ENC_ENCODING_ISO_2022_JP	= 11,
	ENC_ENCODING_SHIFT_JIS		= 12,
	ENC_ENCODING_WINDOWS_1252	= 13,

	ENC_ENCODING_EUC_KR			= 14,
	ENC_ENCODING_GB3212			= 15,

	// Cannot convert from "internal encoding"
	ENC_ENCODING_UTF_7			= 16,
	ENC_ENCODING_UTF_16			= 17,
	ENC_ENCODING_UTF_16LE		= 18,
	ENC_ENCODING_WINDOWS_1250	= 19,
	ENC_ENCODING_WINDOWS_1253	= 20,
	ENC_ENCODING_MACINTOSH		= 21,
	ENC_ENCODING_X_MAC_GREEK	= 22,
	ENC_ENCODING_X_MAC_CE		= 23,
	ENC_ENCODING_IBM850			= 24,
	ENC_ENCODING_IBM852			= 25,

	/* Likely ERRATUM: ENC_MAX_ENCODING (or whatever the original was called)
	 * was very likely set to 24 instead of 26, judging from the bounds checks
	 * in the encconvert.c functions (enc <= 0 || enc >= 24). This is most
	 * apparent in ENCGetNextCharacter width, where there is one case in the
	 * switch block that cannot be reached because of the bounds check:
	 * ENC_ENCODING_IBM850 (25). Judging by the placement of the next two cases,
	 * ENC_ENCODING_EUC_KR (14) and ENC_ENCODING_GB3212 (15), these two
	 * encodings nay have been added at a later point after the rest, and then
	 * ENC_MAX_ENCODING was not updated correctly. They were most likely put in
	 * the middle of the enum because that is the end of the list of encodings
	 * that you are able to convert to from the internal encoding.
	 *
	 * Also, the bounds check is supposed to be > 24, not >= 24.
	 */
	ENC_MAX_ENCODING			= 24,
	ENC_MAX_ENCODING_ACTUAL		= 26,

	// Alternative names
	ENC_ENCODING_ASCII			= ENC_ENCODING_US_ASCII,
	ENC_ENCODING_UTF8			= ENC_ENCODING_UTF_8,
	ENC_ENCODING_UTF16BE		= ENC_ENCODING_UTF_16BE,
	ENC_ENCODING_UTF32BE		= ENC_ENCODING_UTF_32BE,
	ENC_ENCODING_LATIN_1		= ENC_ENCODING_ISO_8859_1,
	ENC_ENCODING_LATIN_2		= ENC_ENCODING_ISO_8859_2,
	ENC_ENCODING_LATIN_3		= ENC_ENCODING_ISO_8859_3,
	ENC_ENCODING_GREEK			= ENC_ENCODING_ISO_8859_7,
	ENC_ENCODING_LATIN_6		= ENC_ENCODING_ISO_8859_10,
	ENC_ENCODING_LATIN_9		= ENC_ENCODING_ISO_8859_15,
	ENC_ENCODING_JIS			= ENC_ENCODING_ISO_2022_JP,
	ENC_ENCODING_SJIS			= ENC_ENCODING_SHIFT_JIS,
	ENC_ENCODING_WIN1252		= ENC_ENCODING_WINDOWS_1252,
	ENC_ENCODING_EUCKR			= ENC_ENCODING_EUC_KR,
	// ENC_ENCODING_GB3212
	ENC_ENCODING_UTF7			= ENC_ENCODING_UTF_7,
	ENC_ENCODING_UTF16			= ENC_ENCODING_UTF_16,
	ENC_ENCODING_UTF16LE		= ENC_ENCODING_UTF_16LE,
	ENC_ENCODING_WIN1250		= ENC_ENCODING_WINDOWS_1250,
	ENC_ENCODING_WIN1253		= ENC_ENCODING_WINDOWS_1253,
	ENC_ENCODING_MAC_ROMAN		= ENC_ENCODING_MACINTOSH,
	ENC_ENCODING_MAC_GREEK		= ENC_ENCODING_X_MAC_GREEK,
	ENC_ENCODING_MAC_CE			= ENC_ENCODING_X_MAC_CE,
	// ENC_ENCODING_IBM850
	// ENC_ENCODING_IBM852
};

typedef unk4_t ENCBreakType;
enum ENCBreakType_et
{
	ENC_BREAK_TYPE_NONE,
	ENC_BREAK_TYPE_WINDOWS,
	ENC_BREAK_TYPE_CLASSIC_MAC_OS,
	ENC_BREAK_TYPE_UNIX,
	// Hey do you think anyone uses \n\r? Or maybe ^_? What about \u0085
};

typedef unk4_t signed ENCState; // akin to mbstate_t
#define ENC_STATE_INITIAL 0

// TODO: enc_char_t, enc_char1_t, enc_char2_t, enc_char4_t, or whatever fits
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

typedef struct ENCContext
{
	ENCEncoding		encoding;	// size 0x04, offset 0x00
	ENCBreakType	breakType;	// size 0x04, offset 0x04
	ENCState		mbState;	// size 0x04, offset 0x08
	char16_t		at_0x0c;	// size 0x02, offset 0x0c
	char16_t		at_0x0e;	// size 0x02, offset 0x0e
} ENCContext; // size 0x10? (probably)

/*******************************************************************************
 * functions
 */

/* encconvert.c (priority placement: main api) */

ENCResult ENCInitContext(ENCContext *context);
ENCResult ENCDuplicateContext(ENCContext *dst, ENCContext const *src);
ENCResult ENCSetExternalEncoding(ENCContext *context,
                                 unsigned char const *encodingName);
ENCResult ENCGetExternalEncoding(ENCContext *context,
                                 unsigned char *encodingName);
ENCResult ENCSetBreakType(ENCContext *context, ENCBreakType breakType);
ENCResult ENCSetAlternativeCharacter(ENCContext *context, char16_t, char16_t);
ENCResult ENCGetExternalCharacterWidth(ENCContext *context, u32 *width);
ENCResult ENCConvertToInternalEncoding(ENCContext *context, char16_t *dst,
                                       unk4_t signed *dstSize, void const *src,
                                       unk4_t signed *srcSize);
ENCResult ENCConvertFromInternalEncoding(ENCContext *context, void *dst,
                                         unk4_t signed *dstSize,
                                         char16_t const *src,
                                         unk4_t signed *srcSize);
ENCResult ENCCheckEncoding(int *lastIndex, unsigned char const *encodingNames[],
                           int num, char16_t const *src, u32 srcSize);
ENCResult ENCIs7BitEncoding(BOOL *is7Bit, unsigned char const *encodingName);

ENCResult ENCGetNextCharacterWidth(ENCContext *context, unk1_t unsigned const *,
                                   u32 *width);

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

/* enclatin.h */

ENCResult ENCConvertStringLatin1ToUnicode(char16_t *dst, unk4_t signed *dstSize,
                                          unsigned char const *src,
                                          unk4_t signed *srcSize);
ENCResult ENCConvertStringUnicodeToLatin1(unsigned char *dst,
                                          unk4_t signed *dstSize,
                                          char16_t const *src,
                                          unk4_t signed *srcSize);
ENCResult ENCConvertStringLatin2ToUnicode(char16_t *dst, unk4_t signed *dstSize,
                                          unsigned char const *src,
                                          unk4_t signed *srcSize);
ENCResult ENCConvertStringUnicodeToLatin2(unsigned char *dst,
                                          unk4_t signed *dstSize,
                                          char16_t const *src,
                                          unk4_t signed *srcSize);
ENCResult ENCConvertStringLatin3ToUnicode(char16_t *dst, unk4_t signed *dstSize,
                                          unsigned char const *src,
                                          unk4_t signed *srcSize);
ENCResult ENCConvertStringUnicodeToLatin3(unsigned char *dst,
                                          unk4_t signed *dstSize,
                                          char16_t const *src,
                                          unk4_t signed *srcSize);
ENCResult ENCConvertStringGreekToUnicode(char16_t *dst, unk4_t signed *dstSize,
                                         unsigned char const *src,
                                         unk4_t signed *srcSize);
ENCResult ENCConvertStringUnicodeToGreek(unsigned char *dst,
                                         unk4_t signed *dstSize,
                                         char16_t const *src,
                                         unk4_t signed *srcSize);
ENCResult ENCConvertStringLatin6ToUnicode(char16_t *dst, unk4_t signed *dstSize,
                                          unsigned char const *src,
                                          unk4_t signed *srcSize);
ENCResult ENCConvertStringUnicodeToLatin6(unsigned char *dst,
                                          unk4_t signed *dstSize,
                                          char16_t const *src,
                                          unk4_t signed *srcSize);
ENCResult ENCConvertStringLatin9ToUnicode(char16_t *dst, unk4_t signed *dstSize,
                                          unsigned char const *src,
                                          unk4_t signed *srcSize);
ENCResult ENCConvertStringUnicodeToLatin9(unsigned char *dst,
                                          unk4_t signed *dstSize,
                                          char16_t const *src,
                                          unk4_t signed *srcSize);
ENCResult ENCConvertStringWin1252ToUnicode(char16_t *dst,
                                           unk4_t signed *dstSize,
                                           unsigned char const *src,
                                           unk4_t signed *srcSize);
ENCResult ENCConvertStringUnicodeToWin1252(unsigned char *dst,
                                           unk4_t signed *dstSize,
                                           char16_t const *src,
                                           unk4_t signed *srcSize);
ENCResult ENCConvertStringWin1250ToUnicode(char16_t *dst,
                                           unk4_t signed *dstSize,
                                           unsigned char const *src,
                                           unk4_t signed *srcSize);
ENCResult ENCConvertStringWin1253ToUnicode(char16_t *dst,
                                           unk4_t signed *dstSize,
                                           unsigned char const *src,
                                           unk4_t signed *srcSize);
ENCResult ENCConvertStringMacromanToUnicode(char16_t *dst,
                                            unk4_t signed *dstSize,
                                            unsigned char const *src,
                                            unk4_t signed *srcSize);
ENCResult ENCConvertStringMacgreekToUnicode(char16_t *dst,
                                            unk4_t signed *dstSize,
                                            unsigned char const *src,
                                            unk4_t signed *srcSize);
ENCResult ENCConvertStringMacceToUnicode(char16_t *dst, unk4_t signed *dstSize,
                                         unsigned char const *src,
                                         unk4_t signed *srcSize);
ENCResult ENCConvertStringIbm850ToUnicode(char16_t *dst, unk4_t signed *dstSize,
                                          unsigned char const *src,
                                          unk4_t signed *srcSize);
ENCResult ENCConvertStringIbm852ToUnicode(char16_t *dst, unk4_t signed *dstSize,
                                          unsigned char const *src,
                                          unk4_t signed *srcSize);

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_ENC_H
