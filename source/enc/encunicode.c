#include "encunicode.h"

/*******************************************************************************
 * headers
 */

#include <uchar.h>

#include <decomp.h>

#include <revolution/types.h>

#include <revolution/enc.h>
#include "encmacros.h"
#include "encutility.h"

/*******************************************************************************
 * macros
 */

// WARNING: No double-evaluation protection
#define SWAP_ENDIAN_16(x)			(((x) >> 8) & 0x00ff) | (((x) << 8) & 0xff00)

// Unicode

#define UTF_IS_DEFINED_CODEPOINT(x)	((x) <= 0x10FFFF)
#define UTF_IS_2_BYTES_IN_UTF8(x)	((x) <    0x0800)
#define UTF_IS_3_BYTES_IN_UTF8(x)	((x) <=   0xFFFF)

// UTF-8

#define UTF8_BOM_LEN				3
#define UTF8_BOM_0					0xEF
#define UTF8_BOM_1					0xBB
#define UTF8_BOM_2					0xBF

#define UTF8_IS_CONT(x)				((unsigned)((x) & 0xC0) == 0x80)
#define UTF8_IS_INITIAL_2(x)		((unsigned)((x) & 0xE0) == 0xC0)
#define UTF8_IS_INITIAL_3(x)		((unsigned)((x) & 0xF0) == 0xE0)
#define UTF8_IS_INITIAL_4(x)		((unsigned)((x) & 0xF8) == 0xF0)

#define UTF8_GET_INITIAL_2(x)		((x) & 0x1F)
#define UTF8_GET_INITIAL_3(x)		((x) & 0x0F)
#define UTF8_GET_INITIAL_4(x)		((x) & 0x07)
#define UTF8_GET_CONT(x)			((x) & 0x3F)

#define UTF8_MAKE_INITIAL_2(x)		(0xC0 + ( (x) >>  6                    ))
#define UTF8_MAKE_INITIAL_3(x)		(0xE0 + ( (x) >> 12                    ))
#define UTF8_MAKE_INITIAL_4(x)		(0xF0 + ( (x) >> 18                    ))
#define UTF8_MAKE_CONT(x, offset_)	(0x80 + (((x) >>  6 * (offset_)) & 0x3F))

#define UTF8_CHECK_BOM(stream_, limit_, limited_, cnt_)	\
	do if ((limit_) >= UTF8_BOM_LEN || !(limited_))		\
	{													\
		char8_t const *bom = *(stream_);				\
														\
		if (*bom == UTF8_BOM_0)							\
		{												\
			++bom;										\
														\
			if (*bom == UTF8_BOM_1)						\
			{											\
				++bom;									\
														\
				if (*bom == UTF8_BOM_2)					\
				{										\
					*(stream_) = bom + 1;				\
					*(cnt_) += UTF8_BOM_LEN;			\
				}										\
			}											\
		}												\
	} while (0)

// UTF-16

#define UTF16_BOM_LEN					1
#define UTF16_BE_BOM					0xFEFF
#define UTF16_LE_BOM					0xFFFE

#define UTF16_IS_SURROGATE(x)			((unsigned)((x) & 0xF800) == 0xD800)

#define UTF16_GET_HIGH_SURROGATE(x)		(((x) - 0xD7C0) << 10)
#define UTF16_GET_LOW_SURROGATE(x)		( (x) & 0x03EF       )

#define UTF16_MAKE_HIGH_SURROGATE(x)	(0xD7C0 + ((x) >> 10))
#define UTF16_MAKE_LOW_SURROGATE(x)		(0xDC00 + ((x) & 0x3FF))

// UTF-32

#define UTF32_BOM_LEN					1
#define UTF32_BE_BOM					0x0000FEFF
#define UTF32_LE_BOM					0xFFFE0000

#define UTF32_CHECK_BOM(stream_, srcSize_, limit_, limited_, cnt_, dstSize_)	\
	do if ((limit_) > 0 || !(limited_))											\
	{																			\
		if (**(stream_) == UTF32_BE_BOM)										\
		{																		\
			++*(cnt_);															\
			++*(stream_);														\
		}																		\
		else if (**(stream_) == UTF32_LE_BOM)									\
		{																		\
			if (dstSize_)														\
				*(dstSize_) = 0;												\
																				\
			*(srcSize_) = 0;													\
			return ENC_EILSEQ;													\
		}																		\
	} while (0)

/*******************************************************************************
 * local function declarations
 */

static char32_t ENCiConvertUtf8To32(char8_t const *src, int length);
static void ENCiConvertUtf32To8(char8_t *dst, int length, char32_t src);
static char32_t ENCiConvertUtf16To32(char16_t const *src, int length);
static void ENCiConvertUtf32To16(char16_t *dst, int length, char32_t src);
static u8 ENCiGetBase64Value(unsigned char c);

/*******************************************************************************
 * variables
 */

// .data

// clang-format off
static u8 base64_array[128] =
{
#if defined(__clang__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wgnu-designator"
# pragma clang diagnostic ignored "-Winitializer-overrides"
#endif

	[0 ... 127] = 0xff,

	['A'] =  0, ['B'] =  1, ['C'] =  2, ['D'] =  3,
	['E'] =  4, ['F'] =  5, ['G'] =  6, ['H'] =  7,
	['I'] =  8, ['J'] =  9, ['K'] = 10, ['L'] = 11,
	['M'] = 12, ['N'] = 13, ['O'] = 14, ['P'] = 15,
	['Q'] = 16, ['R'] = 17, ['S'] = 18, ['T'] = 19,
	['U'] = 20, ['V'] = 21, ['W'] = 22, ['X'] = 23,
	['Y'] = 24, ['Z'] = 25, ['a'] = 26, ['b'] = 27,
	['c'] = 28, ['d'] = 29, ['e'] = 30, ['f'] = 31,
	['g'] = 32, ['h'] = 33, ['i'] = 34, ['j'] = 35,
	['k'] = 36, ['l'] = 37, ['m'] = 38, ['n'] = 39,
	['o'] = 40, ['p'] = 41, ['q'] = 42, ['r'] = 43,
	['s'] = 44, ['t'] = 45, ['u'] = 46, ['v'] = 47,
	['w'] = 48, ['x'] = 49, ['y'] = 50, ['z'] = 51,
	['0'] = 52, ['1'] = 53, ['2'] = 54, ['3'] = 55,
	['4'] = 56, ['5'] = 57, ['6'] = 58, ['7'] = 59,
	['8'] = 60, ['9'] = 61, ['+'] = 62, ['/'] = 63,

#if defined(__clang__)
# pragma clang diagnostic pop
#endif
};
// clang-format on

/*******************************************************************************
 * functions
 */

DEFINE_PUBLIC_ENC_TRAMPOLINE_FROM_UTF16(Ascii)
DEFINE_PUBLIC_ENC_TRAMPOLINE_TO_UTF16(Ascii)
DEFINE_PUBLIC_ENC_TRAMPOLINE(Utf32, char32_t, Utf16, char16_t)
DEFINE_PUBLIC_ENC_TRAMPOLINE(Utf16, char16_t, Utf32, char32_t)
DEFINE_PUBLIC_ENC_TRAMPOLINE(Utf32, char32_t, Utf8, char8_t)
DEFINE_PUBLIC_ENC_TRAMPOLINE(Utf8, char8_t, Utf32, char32_t)
DEFINE_PUBLIC_ENC_TRAMPOLINE(Utf16, char16_t, Utf8, char8_t)
DEFINE_PUBLIC_ENC_TRAMPOLINE(Utf8, char8_t, Utf16, char16_t)
DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_MB_STATE(Utf16, char16_t, Utf16, char16_t)
DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_GIVEN_MB_STATE_VIA(
	Utf16LE, char16_t, Utf16, Utf16BE, char16_t, Utf16,
	ENC_UTF16_STATE_LITTLE_ENDIAN)
DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_GIVEN_MB_STATE(Utf7, byte_t, Utf16, char16_t,
                                                 ENC_UTF7_STATE_INITIAL)

ENCResult ENCSetUnicodeBOM(char16_t *dst, s32 dstSize)
{
	return ENCSetUnicodeBOM16(dst, dstSize);
}

ENCResult ENCSetUnicodeBOM32(char32_t *dst, s32 dstSize)
{
	ENCiRegisterVersion();

	if (dst == nullptr || dstSize < UTF32_BOM_LEN)
		return ENC_ENOBUFS;

	*dst = UTF32_BE_BOM;

	return ENC_ESUCCESS;
}

ENCResult ENCSetUnicodeBOM16(char16_t *dst, s32 dstSize)
{
	ENCiRegisterVersion();

	if (dst == nullptr || dstSize < UTF16_BOM_LEN)
		return ENC_ENOBUFS;

	*dst = UTF16_BE_BOM;

	return ENC_ESUCCESS;
}

ENCResult ENCSetUnicodeBOM8(char8_t *dst, s32 dstSize)
{
	ENCiRegisterVersion();

	if (dst == nullptr || dstSize < UTF8_BOM_LEN)
		return ENC_ENOBUFS;

	*dst++ = 0xFE; // ERRATUM: The correct byte is 0xEF, not 0xFE
	*dst++ = UTF8_BOM_1;
	*dst = UTF8_BOM_2;

	// I hope they fix it in a later version !

	return ENC_ESUCCESS;
}

ENCResult ENCiConvertStringUnicodeToAscii(byte_t *dst, s32 *dstSize,
                                          char16_t const *src, s32 *srcSize,
                                          ENCBreakType breakType)
{
	CREATE_STATE_VARIABLES(dstCnt, dstLimit, dstValid, srcCnt, srcLimit,
	                       srcLimited)

	ENCResult ret;

	CHECK_PARAMETERS(&ret, dst, dstSize, &dstLimit, &dstValid, src, srcSize,
	                 &srcLimit, &srcLimited);

	UTF16_CHECK_BOM(&src, srcSize, srcLimit, srcLimited, &srcCnt, dstSize);

	while (*src && (srcCnt < srcLimit || !srcLimited))
	{
		char16_t cur = *src;

		UNSAFE_CHECK_DST_SPACE(dstCnt, dstLimit, dstValid, &ret);

		UNSAFE_CHECK_BREAK_TYPE(&dst, &dstCnt, dstLimit, dstValid, &src,
		                        &srcCnt, srcLimit, srcLimited, breakType, &ret);

		if (IS_ASCII(cur))
		{
			WRITE_CHAR(cur, &dst, &dstCnt, dstValid, &src, &srcCnt);
		}
		else
		{
			UNSAFE_THROW_AND_QUIT(&ret, ENC_ERANGE);
		}
	}

	WRITE_BACK_SIZES(srcSize, srcCnt, dstSize, dstCnt);

	return ret;
}

ENCResult ENCiConvertStringAsciiToUnicode(char16_t *dst, s32 *dstSize,
                                          byte_t const *src, s32 *srcSize,
                                          ENCBreakType breakType)
{
	CREATE_STATE_VARIABLES(dstCnt, dstLimit, dstValid, srcCnt, srcLimit,
	                       srcLimited)

	ENCResult ret;

	CHECK_PARAMETERS(&ret, dst, dstSize, &dstLimit, &dstValid, src, srcSize,
	                 &srcLimit, &srcLimited);

	while (*src && (srcCnt < srcLimit || !srcLimited))
	{
		UNSAFE_CHECK_DST_SPACE(dstCnt, dstLimit, dstValid, &ret);

		UNSAFE_CHECK_BREAK_TYPE(&dst, &dstCnt, dstLimit, dstValid, &src,
		                        &srcCnt, srcLimit, srcLimited, breakType, &ret);

		// reversed from above?
		if (!IS_ASCII(*src))
		{
			UNSAFE_THROW_AND_QUIT(&ret, ENC_EILSEQ);
		}
		else
		{
			WRITE_CHAR(*src, &dst, &dstCnt, dstValid, &src, &srcCnt);
		}
	}

	WRITE_BACK_SIZES(srcSize, srcCnt, dstSize, dstCnt);

	return ret;
}

ENCResult ENCiConvertStringUtf32ToUtf16(char16_t *dst, s32 *dstSize,
                                        char32_t const *src, s32 *srcSize,
                                        ENCBreakType breakType)
{
	CREATE_STATE_VARIABLES(dstCnt, dstLimit, dstValid, srcCnt, srcLimit,
	                       srcLimited)

	ENCResult ret;

	CHECK_PARAMETERS(&ret, dst, dstSize, &dstLimit, &dstValid, src, srcSize,
	                 &srcLimit, &srcLimited);

	UTF32_CHECK_BOM(&src, srcSize, srcLimit, srcLimited, &srcCnt, dstSize);

	while (*src && (srcCnt < srcLimit || !srcLimited))
	{
		char32_t cur = *src;

		UNSAFE_CHECK_DST_SPACE(dstCnt, dstLimit, dstValid, &ret);

		UNSAFE_CHECK_BREAK_TYPE(&dst, &dstCnt, dstLimit, dstValid, &src,
		                        &srcCnt, srcLimit, srcLimited, breakType, &ret);

		if (cur > 0xFFFF)
		{
			if (!UTF_IS_DEFINED_CODEPOINT(cur))
			{
				UNSAFE_THROW_AND_QUIT(&ret, ENC_EILSEQ);
			}
			
			if (dstValid)
			{
				if (dstLimit - dstCnt < 2)
				{
					UNSAFE_THROW_AND_QUIT(&ret, ENC_ENOBUFS);
				}
				
				ENCiConvertUtf32To16(dst, 2, cur);
				++dst;
			}
			
			++dstCnt;
		}
		else if (UTF16_IS_SURROGATE(cur))
		{
			UNSAFE_THROW_AND_QUIT(&ret, ENC_EILSEQ);
		}
		else
		{
			if (dstValid)
				*dst = cur;
		}

		INCREMENT_STREAMS(&dst, &dstCnt, dstValid, &src, &srcCnt);
	}

	WRITE_BACK_SIZES(srcSize, srcCnt, dstSize, dstCnt);

	return ret;
}

ENCResult ENCiConvertStringUtf16ToUtf32(char32_t *dst, s32 *dstSize,
                                        char16_t const *src, s32 *srcSize,
                                        ENCBreakType breakType)
{
	CREATE_STATE_VARIABLES(dstCnt, dstLimit, dstValid, srcCnt, srcLimit,
	                       srcLimited)

	ENCResult ret;

	CHECK_PARAMETERS(&ret, dst, dstSize, &dstLimit, &dstValid, src, srcSize,
	                 &srcLimit, &srcLimited);

	UTF16_CHECK_BOM(&src, srcSize, srcLimit, srcLimited, &srcCnt, dstSize);

	while (*src && (srcCnt < srcLimit || !srcLimited))
	{
		char16_t cur = *src;

		UNSAFE_CHECK_DST_SPACE(dstCnt, dstLimit, dstValid, &ret);

		UNSAFE_CHECK_BREAK_TYPE(&dst, &dstCnt, dstLimit, dstValid, &src,
		                        &srcCnt, srcLimit, srcLimited, breakType, &ret);

		if (!UTF16_IS_SURROGATE(cur))
		{
			if (dstValid)
				*dst = cur;
		}
		else
		{
			if (srcLimit - srcCnt < 2 && srcLimited)
				break;

			if (dstValid)
				*dst = ENCiConvertUtf16To32(src, 2);

			++src;
			++srcCnt;
		}

		INCREMENT_STREAMS(&dst, &dstCnt, dstValid, &src, &srcCnt);
	}

	WRITE_BACK_SIZES(srcSize, srcCnt, dstSize, dstCnt);

	return ret;
}

ENCResult ENCiConvertStringUtf32ToUtf8(char8_t *dst, s32 *dstSize,
                                       char32_t const *src, s32 *srcSize,
                                       ENCBreakType breakType)
{
	CREATE_STATE_VARIABLES(dstCnt, dstLimit, dstValid, srcCnt, srcLimit,
	                       srcLimited)

	ENCResult ret;

	CHECK_PARAMETERS(&ret, dst, dstSize, &dstLimit, &dstValid, src, srcSize,
	                 &srcLimit, &srcLimited);

	UTF32_CHECK_BOM(&src, srcSize, srcLimit, srcLimited, &srcCnt, dstSize);

	while (*src && (srcCnt < srcLimit || !srcLimited))
	{
		char32_t cur = *src;

		UNSAFE_CHECK_DST_SPACE(dstCnt, dstLimit, dstValid, &ret);

		UNSAFE_CHECK_BREAK_TYPE(&dst, &dstCnt, dstLimit, dstValid, &src,
		                        &srcCnt, srcLimit, srcLimited, breakType, &ret);

		if (IS_ASCII(cur))
		{
			if (dstValid)
				*dst = cur;
		}
		else
		{
			unk4_t long dstMbLen = 0;

			if (UTF16_IS_SURROGATE(cur) || !UTF_IS_DEFINED_CODEPOINT(cur))
			{
				UNSAFE_THROW_AND_QUIT(&ret, ENC_EILSEQ);
			}

			if (UTF_IS_2_BYTES_IN_UTF8(cur))
				dstMbLen = 2;
			else if (UTF_IS_3_BYTES_IN_UTF8(cur))
				dstMbLen = 3;
			else
				dstMbLen = 4;

			if (dstValid)
			{
				if (dstLimit - dstCnt < dstMbLen)
				{
					UNSAFE_THROW_AND_QUIT(&ret, ENC_ENOBUFS);
				}

				ENCiConvertUtf32To8(dst, dstMbLen, cur);
				dst += dstMbLen - 1;
			}

			dstCnt += dstMbLen - 1;
		}

		INCREMENT_STREAMS(&dst, &dstCnt, dstValid, &src, &srcCnt);
	}

	WRITE_BACK_SIZES(srcSize, srcCnt, dstSize, dstCnt);

	return ret;
}

ENCResult ENCiConvertStringUtf8ToUtf32(char32_t *dst, s32 *dstSize,
                                       char8_t const *src, s32 *srcSize,
                                       ENCBreakType breakType)
{
	CREATE_STATE_VARIABLES(dstCnt, dstLimit, dstValid, srcCnt, srcLimit,
	                       srcLimited)

	ENCResult ret;

	CHECK_PARAMETERS(&ret, dst, dstSize, &dstLimit, &dstValid, src, srcSize,
	                 &srcLimit, &srcLimited);

	UTF8_CHECK_BOM(&src, srcLimit, srcLimited, &srcCnt);

	while (*src && (srcCnt < srcLimit || !srcLimited))
	{
		char8_t cur = *src;

		UNSAFE_CHECK_DST_SPACE(dstCnt, dstLimit, dstValid, &ret);

		char32_t c32; // why is it over here?

		UNSAFE_CHECK_BREAK_TYPE(&dst, &dstCnt, dstLimit, dstValid, &src,
		                        &srcCnt, srcLimit, srcLimited, breakType, &ret);

		if (IS_ASCII(cur))
		{
			c32 = cur;
		}
		else
		{
			unk4_t long srcMbLen = 0;

			if (UTF8_IS_INITIAL_2(cur))
			{
				srcMbLen = 2;
			}
			else if (UTF8_IS_INITIAL_3(cur))
			{
				srcMbLen = 3;
			}
			else if (UTF8_IS_INITIAL_4(cur))
			{
				srcMbLen = 4;
			}
			else
			{
				UNSAFE_THROW_AND_QUIT(&ret, ENC_EILSEQ);
			}
			
			if (srcLimit - srcCnt < srcMbLen && srcLimited)
				break;
			
			c32 = ENCiConvertUtf8To32(src, srcMbLen);
			if (!c32)
			{
				UNSAFE_THROW_AND_QUIT(&ret, ENC_EILSEQ);
			}

			src += srcMbLen - 1;
			srcCnt += srcMbLen - 1;
		}

		WRITE_CHAR(c32, &dst, &dstCnt, dstValid, &src, &srcCnt);
	}

	WRITE_BACK_SIZES(srcSize, srcCnt, dstSize, dstCnt);

	return ret;
}

ENCResult ENCiConvertStringUtf16ToUtf8(char8_t *dst, s32 *dstSize,
                                       char16_t const *src, s32 *srcSize,
                                       ENCBreakType breakType)
{
	CREATE_STATE_VARIABLES(dstCnt, dstLimit, dstValid, srcCnt, srcLimit,
	                       srcLimited)

	ENCResult ret;

	CHECK_PARAMETERS(&ret, dst, dstSize, &dstLimit, &dstValid, src, srcSize,
	                 &srcLimit, &srcLimited);

	UTF16_CHECK_BOM(&src, srcSize, srcLimit, srcLimited, &srcCnt, dstSize);

	while (*src && (srcCnt < srcLimit || !srcLimited))
	{
		char16_t cur = *src;

		UNSAFE_CHECK_DST_SPACE(dstCnt, dstLimit, dstValid, &ret);

		UNSAFE_CHECK_BREAK_TYPE(&dst, &dstCnt, dstLimit, dstValid, &src,
		                        &srcCnt, srcLimit, srcLimited, breakType, &ret);

		if (IS_ASCII(cur))
		{
			WRITE_CHAR(cur, &dst, &dstCnt, dstValid, &src, &srcCnt);
		}
		else
		{
			unk4_t long dstMbLen = 0;
			unk4_t long srcMbLen = 0;

			if (UTF_IS_2_BYTES_IN_UTF8(cur))
			{
				dstMbLen = 1;
				srcMbLen = 2;
			}
			else if (!UTF16_IS_SURROGATE(cur))
			{
				dstMbLen = 1;
				srcMbLen = 3;
			}
			else
			{
				dstMbLen = 2;
				srcMbLen = 4;
			}

			if (srcLimit - srcCnt < dstMbLen && srcLimited)
				break;

			if (dstValid)
			{
				if (dstLimit - dstCnt < srcMbLen)
				{
					UNSAFE_THROW_AND_QUIT(&ret, ENC_ENOBUFS);
				}

				char32_t c32 = ENCiConvertUtf16To32(src, dstMbLen);
				ENCiConvertUtf32To8(dst, srcMbLen, c32); // NOLINT (it's not)

				dst += srcMbLen;
			}

			src += dstMbLen;
			srcCnt += dstMbLen;
			dstCnt += srcMbLen;
		}
	}

	WRITE_BACK_SIZES(srcSize, srcCnt, dstSize, dstCnt);

	return ret;
}

ENCResult ENCiConvertStringUtf8ToUtf16(char16_t *dst, s32 *dstSize,
                                       char8_t const *src, s32 *srcSize,
                                       ENCBreakType breakType)
{
	CREATE_STATE_VARIABLES(dstCnt, dstLimit, dstValid, srcCnt, srcLimit,
	                       srcLimited)

	ENCResult ret;

	CHECK_PARAMETERS(&ret, dst, dstSize, &dstLimit, &dstValid, src, srcSize,
	                 &srcLimit, &srcLimited);

	UTF8_CHECK_BOM(&src, srcLimit, srcLimited, &srcCnt);

	while (*src && (srcCnt < srcLimit || !srcLimited))
	{
		char8_t cur = *src;

		UNSAFE_CHECK_DST_SPACE(dstCnt, dstLimit, dstValid, &ret);

		UNSAFE_CHECK_BREAK_TYPE(&dst, &dstCnt, dstLimit, dstValid, &src,
		                        &srcCnt, srcLimit, srcLimited, breakType, &ret);

		if (IS_ASCII(cur))
		{
			WRITE_CHAR(cur, &dst, &dstCnt, dstValid, &src, &srcCnt);
		}
		else
		{
			unk4_t long srcMbLen = 0;
			unk4_t long dstMbLen = 0;

			if (UTF8_IS_INITIAL_2(cur))
			{
				srcMbLen = 2;
				dstMbLen = 1;
			}
			else if (UTF8_IS_INITIAL_3(cur))
			{
				srcMbLen = 3;
				dstMbLen = 1;
			}
			else if (UTF8_IS_INITIAL_4(cur))
			{
				srcMbLen = 4;
				dstMbLen = 2;
			}
			else
			{
				UNSAFE_THROW_AND_QUIT(&ret, ENC_EILSEQ);
			}

			if (srcLimit - srcCnt < srcMbLen && srcLimited)
				break;

			char32_t c32 = ENCiConvertUtf8To32(src, srcMbLen);

			if (!c32)
			{
				UNSAFE_THROW_AND_QUIT(&ret, ENC_EILSEQ);
			}

			if (dstValid)
			{
				if (dstLimit - dstCnt < dstMbLen)
				{
					UNSAFE_THROW_AND_QUIT(&ret, ENC_ENOBUFS);
				}

				ENCiConvertUtf32To16(dst, dstMbLen, c32);
				dst += dstMbLen;
			}

			dstCnt += dstMbLen;
			src += srcMbLen;
			srcCnt += srcMbLen;
		}
	}

	WRITE_BACK_SIZES(srcSize, srcCnt, dstSize, dstCnt);

	return ret;
}

ENCResult ENCiConvertStringUtf7ToUtf16(char16_t *dst, s32 *dstSize,
                                       byte_t const *src, s32 *srcSize,
                                       ENCBreakType breakType,
                                       ENCMBState *mbState)
{
	CREATE_STATE_VARIABLES(dstCnt, dstLimit, dstValid, srcCnt, srcLimit,
	                       srcLimited)

	BOOL base64Mode = false;

	byte4_t partialBits = 0x00000000;
	unsigned int partialOffset = 0;

	ENCResult ret;

	CHECK_PARAMETERS(&ret, dst, dstSize, &dstLimit, &dstValid, src, srcSize,
	                 &srcLimit, &srcLimited);

	if (mbState && ENC_UTF7_STATE_IS_BASE64_MODE(*mbState))
	{
		base64Mode = true;

		partialBits = ENC_UTF7_STATE_GET_PARTIAL_BITS(*mbState);
		partialOffset = ENC_UTF7_STATE_GET_PARTIAL_OFFSET(*mbState);
	}

	while (*src && (srcCnt < srcLimit || !srcLimited))
	{
		if (base64Mode)
		{
			byte_t cur = *src;

			if (dstCnt >= dstLimit && dstValid && cur != '-')
			{
				UNSAFE_THROW_AND_QUIT(&ret, ENC_ENOBUFS);
			}
			
			if (cur == '-') // shift out
			{
				base64Mode = false;

				++src;
				++srcCnt;

				continue;
			}

			unk4_t unsigned b64 = ENCiGetBase64Value(cur);

			if (b64 > 63) // in particular, 0xff is the error sentinel
			{
				UNSAFE_THROW_AND_QUIT(&ret, ENC_EILSEQ);
			}

			/* Calculations with offset subtract 6 to account for the size of
			 * the base64 unit that gets shifted
			 */
			partialBits |= b64 << (32 - 6 - partialOffset);

			if (partialOffset < 16 - 6)
			{
				partialOffset += 6;
			}
			else
			{
				if (dstValid)
				{
					/* NOTE: Doubled expressions */
					*dst = partialBits >> 16; ++dst;
				}

				partialBits <<= 16;

				partialOffset -= 16 - 6;

				++dstCnt;
			}

			++src;
			++srcCnt;
		}
		else
		{
			byte_t cur = *src;

			if (dstCnt >= dstLimit && dstValid && cur != '+')
			{
				UNSAFE_THROW_AND_QUIT(&ret, ENC_ENOBUFS);
			}

			if (cur == '+') // shift in
			{
				base64Mode = true;

				partialBits = 0x00000000;
				partialOffset = 0;

				++src;
				++srcCnt;

				continue;
			}

			UNSAFE_CHECK_BREAK_TYPE(&dst, &dstCnt, dstLimit, dstValid, &src,
			                        &srcCnt, srcLimit, srcLimited, breakType,
			                        &ret);

			WRITE_CHAR(cur, &dst, &dstCnt, dstValid, &src, &srcCnt);
		}
	}

	if (mbState)
	{
		if (base64Mode)
			*mbState = ENC_UTF7_STATE_COLLECT(partialBits, partialOffset);
		else
			*mbState = ENC_UTF7_STATE_INITIAL;
	}

	WRITE_BACK_SIZES(srcSize, srcCnt, dstSize, dstCnt);

	return ret;
}

ENCResult ENCiConvertStringUtf16ToUtf16(char16_t *dst, s32 *dstSize,
                                        char16_t const *src, s32 *srcSize,
                                        ENCBreakType breakType,
                                        ENCMBState *mbState)
{
	CREATE_STATE_VARIABLES(dstCnt, dstLimit, dstValid, srcCnt, srcLimit,
	                       srcLimited)

	BOOL littleEndian = false;

	ENCResult ret;

	CHECK_PARAMETERS(&ret, dst, dstSize, &dstLimit, &dstValid, src, srcSize,
	                 &srcLimit, &srcLimited);

	// UTF16_CHECK_BOM but also manipulates state
	if (srcLimit > 0 || !srcLimited)
	{
		if (*src == UTF16_BE_BOM)
		{
			if (mbState != nullptr)
				*mbState = ENC_UTF16_STATE_BIG_ENDIAN;

			++srcCnt;
			++src;
		}
		else if (*src == UTF16_LE_BOM)
		{
			if (mbState != nullptr)
				*mbState = ENC_UTF16_STATE_LITTLE_ENDIAN;

			littleEndian = true;

			++srcCnt;
			++src;
		}
		else
		{
			if (mbState != nullptr && *mbState == ENC_UTF16_STATE_LITTLE_ENDIAN)
				littleEndian = true;
		}
	}

	while (*src && (srcCnt < srcLimit || !srcLimited))
	{
		char16_t cur = *src;

		if (littleEndian)
			cur = SWAP_ENDIAN_16(cur);

		UNSAFE_CHECK_DST_SPACE(dstCnt, dstLimit, dstValid, &ret);

		if (breakType > ENC_BR_KEEP)
		{
			int srcBrkLen = 0;

			char16_t cur2 = L'\0';

			// UNSAFE_CHECK_BREAK_TYPE but also potentially swaps endianness
			if (srcLimit - srcCnt > 1 || !srcLimited)
			{
				cur2 = src[1];

				if (littleEndian)
					cur2 = SWAP_ENDIAN_16(cur2);
			}

			srcBrkLen = ENCiCheckBreakType(cur, cur2);

			if (srcBrkLen > 0)
			{
				// This can be reused though
				UNSAFE_CHECK_DST_BREAK_TYPE(&dst, &dstCnt, dstLimit, dstValid,
				                            &src, &srcCnt, breakType, &ret);
			}
		}

		WRITE_CHAR(cur, &dst, &dstCnt, dstValid, &src, &srcCnt);
	}

	WRITE_BACK_SIZES(srcSize, srcCnt, dstSize, dstCnt);

	return ret;
}

static char32_t ENCiConvertUtf8To32(char8_t const *src, int length)
{
	int i;

	char32_t c32;

	switch (length)
	{
	case 1:
		c32 = src[0];
		break;

	case 2:
		c32 = UTF8_GET_INITIAL_2(src[0]);
		break;

	case 3:
		c32 = UTF8_GET_INITIAL_3(src[0]);
		break;

	case 4:
		c32 = UTF8_GET_INITIAL_4(src[0]);
		break;

	default:
		return L'\0';
	}

	for (i = 1; i < length; ++i)
	{
		if (!UTF8_IS_CONT(src[i]))
			return L'\0';

		c32 <<= 6;
		c32 += UTF8_GET_CONT(src[i]);
	}

	return c32;
}

static void ENCiConvertUtf32To8(char8_t *dst, int length, char32_t src)
{
	int i;

	switch (length)
	{
	case 1:
		dst[0] = src;
		break;

	case 2:
		dst[0] = UTF8_MAKE_INITIAL_2(src);
		break;

	case 3:
		dst[0] = UTF8_MAKE_INITIAL_3(src);
		break;

	case 4:
		dst[0] = UTF8_MAKE_INITIAL_4(src);
		break;

	default:
		return;
	}

	for (i = 1; i < length; ++i)
		dst[i] = UTF8_MAKE_CONT(src, length - i - 1);
}

static char32_t ENCiConvertUtf16To32(char16_t const *src, int length)
{
	if (length == 1)
	{
		return *src;
	}

	if (length == 2)
	{
		char32_t c32;

		c32 = UTF16_GET_HIGH_SURROGATE(src[0]);
		c32 += UTF16_GET_LOW_SURROGATE(src[1]);

		return c32;
	}

	return L'\0';
}

static void ENCiConvertUtf32To16(char16_t *dst, int length, char32_t src)
{
	if (length == 1)
	{
		*dst = src;
	}

	/* NOTE: No else clause here */

	if (length == 2)
	{
		dst[0] = UTF16_MAKE_HIGH_SURROGATE(src);
		dst[1] = UTF16_MAKE_LOW_SURROGATE(src);
	}
}

static u8 ENCiGetBase64Value(unsigned char c)
{
	if (c > 0x7F)
		return 0xff;

	return base64_array[c];
}
