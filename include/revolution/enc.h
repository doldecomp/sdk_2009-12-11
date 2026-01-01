#ifndef RVL_SDK_ENC_H
#define RVL_SDK_ENC_H

/*******************************************************************************
 * headers
 */

#include <uchar.h>

#include "revolution/types.h"

/*******************************************************************************
 * macros
 */

// Fun Fact! All of these are reserved macro identifiers and technically UB.
#define ENC_DST_INVALID					(-1) // s32 *dstSize
#define ENC_SRC_UNLIMITED				(-1) // s32 *srcSize

#define ENC_CHECK_ENCODING_NOT_FOUND	(-1) // int *index
#define ENC_CHECK_ENCODING_MAX_NUM		24 // = ENC_BAD_MAX_ENCODING?

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

// NOTE: Different library version [release Feb 27 2009 10:05:42 (0x4302_145)]
// [SDFE4Q]/singitD.elf:.debug_info::0x213b6
/* How nice, ...0x213a6 also says that it's an identity typedef. Thank you
 * dwarf2typedefchains, very cool! I was already going to make it one, but it's
 * cool to know about it regardless.
 */
typedef enum ENCResult
{
	ENC_OK						=  0,
	ENC_ERR_NO_BUF_LEFT			= -1,
	ENC_ERR_NO_MAP_RULE			= -2,
	ENC_ERR_INVALID_PARAM		= -3,
	ENC_ERR_INVALID_FORMAT		= -4,
	ENC_ERR_UNKNOWN_ENCODING	= -5,
	ENC_ERR_UNSUPPORTED			= -6,

	/* This error code is not used at all because all of the tables are already
	 * included. Is there a different version of this library where they are
	 * not? Or maybe previous versions of the library didn't, and now it's
	 * obsolete?
	 */
	ENC_ERR_NOT_LOADED			= -7,

// Fun Fact! All of these are reserved macro identifiers and technically UB.
#define ENC_ESUCCESS	ENC_OK
#define ENC_ENOBUFS		ENC_ERR_NO_BUF_LEFT
#define ENC_ERANGE		ENC_ERR_NO_MAP_RULE // Ok But Is It Wrong Though
#define ENC_EINVAL		ENC_ERR_INVALID_PARAM
#define ENC_EILSEQ		ENC_ERR_INVALID_FORMAT
#define ENC_EBADTYPE	ENC_ERR_UNKNOWN_ENCODING
#define ENC_ENOTSUPP	ENC_ERR_UNSUPPORTED
} ENCResult;

// NOTE: Different library version [release Feb 27 2009 10:05:42 (0x4302_145)]
// [RGWJ41]/LynWiiF.elf:.debug_info::0x238532 (associated type only, not enumerator names)
typedef long ENCEncoding;
enum ENCEncoding_et
{
	ENC_ENCODING_NONE,

	// Can convert from "internal encoding" (UTF-16BE)
	ENC_ENCODING_US_ASCII,
	ENC_ENCODING_UTF_8,
	ENC_ENCODING_UTF_16BE,
	ENC_ENCODING_UTF_32BE,
	ENC_ENCODING_ISO_8859_1,
	ENC_ENCODING_ISO_8859_2,
	ENC_ENCODING_ISO_8859_3,
	ENC_ENCODING_ISO_8859_7,
	ENC_ENCODING_ISO_8859_10,
	ENC_ENCODING_ISO_8859_15,
	ENC_ENCODING_ISO_2022_JP,
	ENC_ENCODING_SHIFT_JIS,
	ENC_ENCODING_WINDOWS_1252,

	ENC_ENCODING_EUC_KR,
	ENC_ENCODING_GB2312,

	// Cannot convert from "internal encoding"
	ENC_ENCODING_UTF_7,
	ENC_ENCODING_UTF_16,
	ENC_ENCODING_UTF_16LE,
	ENC_ENCODING_WINDOWS_1250,
	ENC_ENCODING_WINDOWS_1253,
	ENC_ENCODING_MACINTOSH,
	ENC_ENCODING_X_MAC_GREEK,
	ENC_ENCODING_X_MAC_CE,
	ENC_ENCODING_IBM850,
	ENC_ENCODING_IBM852,

	ENC_MAX_ENCODING

	/* Likely ERRATUM: ENC_MAX_ENCODING (or whatever the original was called)
	 * was very likely set to 24 instead of 26, judging from the bounds checks
	 * in the encconvert.c functions (x <= 0 || x >= 24). This is most apparent
	 * in ENCGetNextCharacterWidth, where there is one case in the switch block
	 * that cannot be reached because of the bounds check: ENC_ENCODING_IBM850
	 * (25). Judging from the placement of the next two cases,
	 * ENC_ENCODING_EUC_KR (14) and ENC_ENCODING_GB2312 (15), these two
	 * encodings may have been added after the rest, and ENC_MAX_ENCODING was
	 * not updated accordingly. They were most likely put in the middle of the
	 * enum because that is the end of the list of encodings that you are able
	 * to convert to from the internal encoding.
	 *
	 * Also, the bounds check is supposed to be > 24, not >= 24.
	 */,
	ENC_BAD_MAX_ENCODING	= 24,
};

// NOTE: Different library version [release Feb 27 2009 10:05:42 (0x4302_145)]
// [RGWJ41]/LynWiiF.elf:.debug_info::0x238589
typedef enum ENCBreakType
{
	ENC_BR_KEEP,
	ENC_BR_CRLF,
	ENC_BR_CR,
	ENC_BR_LF,
	// Hey do you think anyone uses \n\r? Or maybe ^_? What about \u0085
} ENCBreakType;

// NOTE: Different library version [release Feb 27 2009 10:05:42 (0x4302_145)]
// [RGWJ41]/LynWiiF.elf:.debug_info::0x238556 (associated type)
typedef s32 ENCMBState; // akin to mbstate_t
#define ENC_STATE_INITIAL	0

// NOTE: Different library version [release Feb 27 2009 10:05:42 (0x4302_145)]
// [RGWJ41]/LynWiiF.elf:.debug_info::0x238521
typedef struct ENCContext
{
	ENCEncoding		encoding;	// size 0x04, offset 0x00
	ENCBreakType	brtype;		// size 0x04, offset 0x04
	ENCMBState		state;		// size 0x04, offset 0x08
	char16_t		nomap;		// size 0x02, offset 0x0c
	char16_t		invalid;	// size 0x02, offset 0x0e
} ENCContext; // size 0x10

/*******************************************************************************
 * functions
 */

/* encconvert.c (priority placement: main api) */
ENCResult ENCInitContext(ENCContext *context);
ENCResult ENCDuplicateContext(ENCContext *dst, ENCContext const *src);
ENCResult ENCSetExternalEncoding(ENCContext *context, char const *name);
ENCResult ENCGetExternalEncoding(ENCContext *context, char *name);
ENCResult ENCSetBreakType(ENCContext *context, ENCBreakType breakType);
ENCResult ENCSetAlternativeCharacter(ENCContext *context, char16_t nomap,
                                     char16_t invalid);
ENCResult ENCGetExternalCharacterWidth(ENCContext *context, u32 *width);
ENCResult ENCConvertToInternalEncoding(ENCContext *context, char16_t *dst,
                                       s32 *dstSize, void const *src,
                                       s32 *srcSize);
ENCResult ENCConvertFromInternalEncoding(ENCContext *context, void *dst,
                                         s32 *dstSize, char16_t const *src,
                                         s32 *srcSize);
ENCResult ENCCheckEncoding(int *index, char const *names[], int num,
                           char16_t const *src, u32 srcSize);
ENCResult ENCIs7BitEncoding(BOOL *is7Bit, char const *name);

ENCResult ENCGetNextCharacterWidth(ENCContext *context, void const *src,
                                   u32 *width);

/* encunicode.c */
ENCResult ENCConvertStringUnicodeToAscii(byte_t *dst, s32 *dstSize,
                                         char16_t const *src, s32 *srcSize);
ENCResult ENCConvertStringAsciiToUnicode(char16_t *dst, s32 *dstSize,
                                         byte_t const *src, s32 *srcSize);
ENCResult ENCConvertStringUtf32ToUtf16(char16_t *dst, s32 *dstSize,
                                       char32_t const *src, s32 *srcSize);
ENCResult ENCConvertStringUtf16ToUtf32(char32_t *dst, s32 *dstSize,
                                       char16_t const *src, s32 *srcSize);
ENCResult ENCConvertStringUtf32ToUtf8(char8_t *dst, s32 *dstSize,
                                      char32_t const *src, s32 *srcSize);
ENCResult ENCConvertStringUtf8ToUtf32(char32_t *dst, s32 *dstSize,
                                      char8_t const *src, s32 *srcSize);
ENCResult ENCConvertStringUtf16ToUtf8(char8_t *dst, s32 *dstSize,
                                      char16_t const *src, s32 *srcSize);
ENCResult ENCConvertStringUtf8ToUtf16(char16_t *dst, s32 *dstSize,
                                      char8_t const *src, s32 *srcSize);
ENCResult ENCConvertStringUtf16ToUtf16(char16_t *dst, s32 *dstSize,
                                       char16_t const *src, s32 *srcSize);
ENCResult ENCConvertStringUtf16LEToUtf16BE(char16_t *dst, s32 *dstSize,
                                           char16_t const *src, s32 *srcSize);
ENCResult ENCConvertStringUtf7ToUtf16(char16_t *dst, s32 *dstSize,
                                      byte_t const *src, s32 *srcSize);

// Byte order mark functions
ENCResult ENCSetUnicodeBOM(char16_t *dst, s32 dstSize);
ENCResult ENCSetUnicodeBOM32(char32_t *dst, s32 dstSize);
ENCResult ENCSetUnicodeBOM16(char16_t *dst, s32 dstSize);
ENCResult ENCSetUnicodeBOM8(char8_t *dst, s32 dstSize); // See body

/* encjapanese.c */
ENCResult ENCConvertStringSjisToUnicode(char16_t *dst, s32 *dstSize,
                                        byte_t const *src, s32 *srcSize);
ENCResult ENCConvertStringUnicodeToSjis(byte_t *dst, s32 *dstSize,
                                        char16_t const *src, s32 *srcSize);
ENCResult ENCConvertStringJisToUnicode(char16_t *dst, s32 *dstSize,
                                       byte_t const *src, s32 *srcSize);
ENCResult ENCConvertStringUnicodeToJis(byte_t *dst, s32 *dstSize,
                                       char16_t const *src, s32 *srcSize);
ENCResult ENCConvertStringJisToSjis(byte_t *dst, s32 *dstSize,
                                    byte_t const *src, s32 *srcSize);
ENCResult ENCConvertStringSjisToJis(byte_t *dst, s32 *dstSize,
                                    byte_t const *src, s32 *srcSize);

/* enclatin.c */
ENCResult ENCConvertStringLatin1ToUnicode(char16_t *dst, s32 *dstSize,
                                          byte_t const *src, s32 *srcSize);
ENCResult ENCConvertStringUnicodeToLatin1(byte_t *dst, s32 *dstSize,
                                          char16_t const *src, s32 *srcSize);
ENCResult ENCConvertStringLatin2ToUnicode(char16_t *dst, s32 *dstSize,
                                          byte_t const *src, s32 *srcSize);
ENCResult ENCConvertStringUnicodeToLatin2(byte_t *dst, s32 *dstSize,
                                          char16_t const *src, s32 *srcSize);
ENCResult ENCConvertStringLatin3ToUnicode(char16_t *dst, s32 *dstSize,
                                          byte_t const *src, s32 *srcSize);
ENCResult ENCConvertStringUnicodeToLatin3(byte_t *dst, s32 *dstSize,
                                          char16_t const *src, s32 *srcSize);
ENCResult ENCConvertStringGreekToUnicode(char16_t *dst, s32 *dstSize,
                                         byte_t const *src, s32 *srcSize);
ENCResult ENCConvertStringUnicodeToGreek(byte_t *dst, s32 *dstSize,
                                         char16_t const *src, s32 *srcSize);
ENCResult ENCConvertStringLatin6ToUnicode(char16_t *dst, s32 *dstSize,
                                          byte_t const *src, s32 *srcSize);
ENCResult ENCConvertStringUnicodeToLatin6(byte_t *dst, s32 *dstSize,
                                          char16_t const *src, s32 *srcSize);
ENCResult ENCConvertStringLatin9ToUnicode(char16_t *dst, s32 *dstSize,
                                          byte_t const *src, s32 *srcSize);
ENCResult ENCConvertStringUnicodeToLatin9(byte_t *dst, s32 *dstSize,
                                          char16_t const *src, s32 *srcSize);
ENCResult ENCConvertStringWin1252ToUnicode(char16_t *dst, s32 *dstSize,
                                           byte_t const *src, s32 *srcSize);
ENCResult ENCConvertStringUnicodeToWin1252(byte_t *dst, s32 *dstSize,
                                           char16_t const *src, s32 *srcSize);

// These are only to Unicode, not from
ENCResult ENCConvertStringWin1250ToUnicode(char16_t *dst, s32 *dstSize,
                                           byte_t const *src, s32 *srcSize);
ENCResult ENCConvertStringWin1253ToUnicode(char16_t *dst, s32 *dstSize,
                                           byte_t const *src, s32 *srcSize);
ENCResult ENCConvertStringMacromanToUnicode(char16_t *dst, s32 *dstSize,
                                            byte_t const *src, s32 *srcSize);
ENCResult ENCConvertStringMacgreekToUnicode(char16_t *dst, s32 *dstSize,
                                            byte_t const *src, s32 *srcSize);
ENCResult ENCConvertStringMacceToUnicode(char16_t *dst, s32 *dstSize,
                                         byte_t const *src, s32 *srcSize);
ENCResult ENCConvertStringIbm850ToUnicode(char16_t *dst, s32 *dstSize,
                                          byte_t const *src, s32 *srcSize);
ENCResult ENCConvertStringIbm852ToUnicode(char16_t *dst, s32 *dstSize,
                                          byte_t const *src, s32 *srcSize);

/* encchinese.c */
ENCResult ENCConvertStringGb2312ToUnicode(char16_t *dst, s32 *dstSize,
                                          byte_t const *src, s32 *srcSize);
ENCResult ENCConvertStringUnicodeToGb2312(byte_t *dst, s32 *dstSize,
                                          char16_t const *src, s32 *srcSize);

/* enckorean.c */
ENCResult ENCConvertStringUhcToUnicode(char16_t *dst, s32 *dstSize,
                                       byte_t const *src, s32 *srcSize);
ENCResult ENCConvertStringUnicodeToUhc(byte_t *dst, s32 *dstSize,
                                       char16_t const *src, s32 *srcSize);

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_ENC_H
