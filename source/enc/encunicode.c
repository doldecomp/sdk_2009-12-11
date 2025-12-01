// #include "encunicode.h"

/*******************************************************************************
 * headers
 */

#include <stddef.h>
#include <uchar.h>

#include <decomp.h>

#include <revolution/types.h>

#include <revolution/enc.h>
#include "encmacros.h"

#include "encutility.h"

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wimplicit-function-declaration"
#pragma clang diagnostic error "-Wunused-variable"
#endif

/*******************************************************************************
 * macros
 */

#undef NULL
#define NULL	((void *)(0))

/*******************************************************************************
 * local function declarations
 */

static char32_t ENCiConvertUtf8To32(char8_t const *src, int length);
static void ENCiConvertUtf32To8(char8_t *dst, int length, char32_t c32);

static char32_t ENCiConvertUtf16To32(char16_t const *src, int length);
static void ENCiConvertUtf32To16(char16_t *dst, int length, char32_t c32);

static u8 ENCiGetBase64Value(unsigned char c);

/*******************************************************************************
 * variables
 */

// .data
static u8 base64_array[] =
{
#if defined(__clang__)
# pragma clang diagnostic push
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

/*******************************************************************************
 * functions
 */

ENCResult ENCConvertStringUnicodeToAscii(unsigned char *dst,
                                         unk_t signed *dstSize,
                                         char16_t const *src,
                                         unk_t signed *srcSize)
{
	return ENCiConvertStringUnicodeToAscii(dst, dstSize, src, srcSize,
	                                       ENC_BREAK_TYPE_NONE);
}

ENCResult ENCConvertStringAsciiToUnicode(char16_t *dst, unk_t signed *dstSize,
                                         unsigned char const *src,
                                         unk_t signed *srcSize)
{
	return ENCiConvertStringAsciiToUnicode(dst, dstSize, src, srcSize,
	                                       ENC_BREAK_TYPE_NONE);
}

ENCResult ENCConvertStringUtf32ToUtf16(char16_t *dst, unk_t signed *dstSize,
                                       char32_t const *src,
                                       unk_t signed *srcSize)
{
	return ENCiConvertStringUtf32ToUtf16(dst, dstSize, src, srcSize,
	                                     ENC_BREAK_TYPE_NONE);
}

ENCResult ENCConvertStringUtf16ToUtf32(char32_t *dst, unk_t signed *dstSize,
                                       char16_t const *src,
                                       unk_t signed *srcSize)
{
	return ENCiConvertStringUtf16ToUtf32(dst, dstSize, src, srcSize,
	                                     ENC_BREAK_TYPE_NONE);
}

ENCResult ENCConvertStringUtf32ToUtf8(char8_t *dst, unk_t signed *dstSize,
                                      char32_t const *src,
                                      unk_t signed *srcSize)
{
	return ENCiConvertStringUtf32ToUtf8(dst, dstSize, src, srcSize,
	                                    ENC_BREAK_TYPE_NONE);
}

ENCResult ENCConvertStringUtf8ToUtf32(char32_t *dst, unk_t signed *dstSize,
                                      char8_t const *src, unk_t signed *srcSize)
{
	return ENCiConvertStringUtf8ToUtf32(dst, dstSize, src, srcSize,
	                                    ENC_BREAK_TYPE_NONE);
}

ENCResult ENCConvertStringUtf16ToUtf8(char8_t *dst, unk_t signed *dstSize,
                                      char16_t const *src,
                                      unk_t signed *srcSize)
{
	return ENCiConvertStringUtf16ToUtf8(dst, dstSize, src, srcSize,
	                                    ENC_BREAK_TYPE_NONE);
}

ENCResult ENCConvertStringUtf8ToUtf16(char16_t *dst, unk_t signed *dstSize,
                                      char8_t const *src, unk_t signed *srcSize)
{
	return ENCiConvertStringUtf8ToUtf16(dst, dstSize, src, srcSize,
	                                    ENC_BREAK_TYPE_NONE);
}

ENCResult ENCConvertStringUtf16ToUtf16(char16_t *dst, unk_t signed *dstSize,
                                       char16_t const *src,
                                       unk_t signed *srcSize)
{
	return ENCiConvertStringUtf16ToUtf16(dst, dstSize, src, srcSize,
	                                     ENC_BREAK_TYPE_NONE, nullptr);
}

ENCResult ENCConvertStringUtf16LEToUtf16BE(char16_t *dst, unk_t signed *dstSize,
                                           char16_t const *src,
                                           unk_t signed *srcSize)
{
	ENCEndianness endianness = ENC_LITTLE_ENDIAN;

	return ENCiConvertStringUtf16ToUtf16(dst, dstSize, src, srcSize,
	                                     ENC_BREAK_TYPE_NONE, &endianness);
}

ENCResult ENCConvertStringUtf7ToUtf16(char16_t *dst, unk_t signed *dstSize,
                                      unsigned char const *src,
                                      unk_t signed *srcSize)
{
	unk4_t x = 0;

	return ENCiConvertStringUtf7ToUtf16(dst, dstSize, src, srcSize,
	                                    ENC_BREAK_TYPE_NONE, &x);
}

ENCResult ENCSetUnicodeBOM(char16_t *dst, unk_t signed dstSize)
{
	return ENCSetUnicodeBOM16(dst, dstSize);
}

ENCResult ENCSetUnicodeBOM32(char32_t *dst, unk_t signed dstSize)
{
	ENCiRegisterVersion();

	if (dst == nullptr || dstSize < UTF32_BOM_LEN)
		return ENC_ENOSPC;

	*dst = UTF32_BE_BOM;

	return ENC_ESUCCESS;
}

ENCResult ENCSetUnicodeBOM16(char16_t *dst, unk_t signed dstSize)
{
	ENCiRegisterVersion();

	if (dst == nullptr || dstSize < UTF16_BOM_LEN)
		return ENC_ENOSPC;

	*dst = UTF16_BE_BOM;

	return ENC_ESUCCESS;
}

ENCResult ENCSetUnicodeBOM8(char8_t *dst, unk_t signed dstSize)
{
	ENCiRegisterVersion();

	if (dst == nullptr || dstSize < UTF8_BOM_LEN)
		return ENC_ENOSPC;

	*dst++ = 0xFE; // ERRATUM: The correct byte is 0xEF, not 0xFE
	*dst++ = UTF8_BOM_1;
	*dst = UTF8_BOM_2;

	return ENC_ESUCCESS;
}

ENCResult ENCiConvertStringUnicodeToAscii(unsigned char *dst,
                                          unk_t signed *dstSize,
                                          char16_t const *src,
                                          unk_t signed *srcSize,
                                          ENCBreakType breakType)
{
	CREATE_STATE_VARIABLES(dstCnt, dstLimit, dstValid, srcCnt, srcLimit,
	                       srcLimited)

	ENCResult ret;

	CHECK_PARAMETERS(dst, dstSize, dstLimit, dstValid, src, srcSize, srcLimit,
	                 srcLimited, ret);

	UTF16_CHECK_BOM(src, srcSize, srcLimit, srcLimited, srcCnt, dstSize);

	while (*src && (srcCnt < srcLimit || !srcLimited))
	{
		char16_t cur = *src;

		CHECK_DST_SPACE(dstCnt, dstLimit, dstValid, ret);

		CHECK_BREAK_TYPE(dst, dstCnt, dstLimit, dstValid, src, srcCnt, srcLimit,
		                 srcLimited, breakType, ret);

		if (IS_ASCII(cur))
		{
			COPY_CHAR_AS_IS(cur, dst, dstCnt, dstValid, src, srcCnt);
		}
		else
		{
			THROW_AND_QUIT(ret, ENC_E2);
		}
	}

	WRITE_BACK_SIZES(srcSize, srcCnt, dstSize, dstCnt);

	return ret;
}

ENCResult ENCiConvertStringAsciiToUnicode(char16_t *dst, unk_t signed *dstSize,
                                          unsigned char const *src,
                                          unk_t signed *srcSize,
                                          ENCBreakType breakType)
{
	CREATE_STATE_VARIABLES(dstCnt, dstLimit, dstValid, srcCnt, srcLimit,
	                       srcLimited)

	ENCResult ret;

	CHECK_PARAMETERS(dst, dstSize, dstLimit, dstValid, src, srcSize, srcLimit,
	                 srcLimited, ret);

	while (*src && (srcCnt < srcLimit || !srcLimited))
	{
		CHECK_DST_SPACE(dstCnt, dstLimit, dstValid, ret);

		CHECK_BREAK_TYPE(dst, dstCnt, dstLimit, dstValid, src, srcCnt, srcLimit,
		                 srcLimited, breakType, ret);

		// reversed from above?
		if (!IS_ASCII(*src))
		{
			THROW_AND_QUIT(ret, ENC_EILSEQ);
		}

		COPY_CHAR_AS_IS(*src, dst, dstCnt, dstValid, src, srcCnt);
	}

	WRITE_BACK_SIZES(srcSize, srcCnt, dstSize, dstCnt);

	return ret;
}

ENCResult ENCiConvertStringUtf32ToUtf16(char16_t *dst, unk_t signed *dstSize,
                                        char32_t const *src,
                                        unk_t signed *srcSize,
                                        ENCBreakType breakType)
{
	CREATE_STATE_VARIABLES(dstCnt, dstLimit, dstValid, srcCnt, srcLimit,
	                       srcLimited)

	ENCResult ret;

	CHECK_PARAMETERS(dst, dstSize, dstLimit, dstValid, src, srcSize, srcLimit,
	                 srcLimited, ret);

	UTF32_CHECK_BOM(src, srcSize, srcLimit, srcLimited, srcCnt, dstSize);

	while (*src && (srcCnt < srcLimit || !srcLimited))
	{
		char32_t cur = *src;

		CHECK_DST_SPACE(dstCnt, dstLimit, dstValid, ret);

		CHECK_BREAK_TYPE(dst, dstCnt, dstLimit, dstValid, src, srcCnt, srcLimit,
		                 srcLimited, breakType, ret);

		if (cur > 0xFFFF)
		{
			if (!UTF_IS_DEFINED_CODEPOINT(cur))
			{
				THROW_AND_QUIT(ret, ENC_EILSEQ);
			}
			
			if (dstValid)
			{
				if (dstLimit - dstCnt < 2)
				{
					THROW_AND_QUIT(ret, ENC_ENOSPC);
				}
				
				ENCiConvertUtf32To16(dst, 2, cur);
				++dst;
			}
			
			++dstCnt;
		}
		else if (UTF16_IS_SURROGATE(cur))
		{
			THROW_AND_QUIT(ret, ENC_EILSEQ);
		}
		else
		{
			if (dstValid)
				*dst = cur;
		}

		INCREMENT_STREAMS(dst, dstCnt, dstValid, src, srcCnt);
	}

	WRITE_BACK_SIZES(srcSize, srcCnt, dstSize, dstCnt);

	return ret;
}

ENCResult ENCiConvertStringUtf16ToUtf32(char32_t *dst, unk_t signed *dstSize,
                                        char16_t const *src,
                                        unk_t signed *srcSize,
                                        ENCBreakType breakType)
{
	CREATE_STATE_VARIABLES(dstCnt, dstLimit, dstValid, srcCnt, srcLimit,
	                       srcLimited)

	ENCResult ret;

	CHECK_PARAMETERS(dst, dstSize, dstLimit, dstValid, src, srcSize, srcLimit,
	                 srcLimited, ret);

	UTF16_CHECK_BOM(src, srcSize, srcLimit, srcLimited, srcCnt, dstSize);

	while (*src && (srcCnt < srcLimit || !srcLimited))
	{
		char16_t cur = *src;

		CHECK_DST_SPACE(dstCnt, dstLimit, dstValid, ret);

		CHECK_BREAK_TYPE(dst, dstCnt, dstLimit, dstValid, src, srcCnt, srcLimit,
		                 srcLimited, breakType, ret);

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

		INCREMENT_STREAMS(dst, dstCnt, dstValid, src, srcCnt);
	}

	WRITE_BACK_SIZES(srcSize, srcCnt, dstSize, dstCnt);

	return ret;
}

ENCResult ENCiConvertStringUtf32ToUtf8(char8_t *dst, unk_t signed *dstSize,
                                       char32_t const *src,
                                       unk_t signed *srcSize,
                                       ENCBreakType breakType)
{
	CREATE_STATE_VARIABLES(dstCnt, dstLimit, dstValid, srcCnt, srcLimit,
	                       srcLimited)

	ENCResult ret;

	CHECK_PARAMETERS(dst, dstSize, dstLimit, dstValid, src, srcSize, srcLimit,
	                 srcLimited, ret);

	UTF32_CHECK_BOM(src, srcSize, srcLimit, srcLimited, srcCnt, dstSize);

	while (*src && (srcCnt < srcLimit || !srcLimited))
	{
		char32_t cur = *src;

		CHECK_DST_SPACE(dstCnt, dstLimit, dstValid, ret);

		CHECK_BREAK_TYPE(dst, dstCnt, dstLimit, dstValid, src, srcCnt, srcLimit,
		                 srcLimited, breakType, ret);

		if (IS_ASCII(cur))
		{
			if (dstValid)
				*dst = cur;
		}
		else
		{
			int dstMbLen = 0;

			if (UTF16_IS_SURROGATE(cur) || !UTF_IS_DEFINED_CODEPOINT(cur))
			{
				THROW_AND_QUIT(ret, ENC_EILSEQ);
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
					THROW_AND_QUIT(ret, ENC_ENOSPC);
				}

				ENCiConvertUtf32To8(dst, dstMbLen, cur);
				dst += dstMbLen - 1;
			}

			dstCnt += dstMbLen - 1;
		}

		INCREMENT_STREAMS(dst, dstCnt, dstValid, src, srcCnt);
	}

	WRITE_BACK_SIZES(srcSize, srcCnt, dstSize, dstCnt);

	return ret;
}

ENCResult ENCiConvertStringUtf8ToUtf32(char32_t *dst, unk_t signed *dstSize,
                                       char8_t const *src,
                                       unk_t signed *srcSize,
                                       ENCBreakType breakType)
{
	CREATE_STATE_VARIABLES(dstCnt, dstLimit, dstValid, srcCnt, srcLimit,
	                       srcLimited)

	ENCResult ret;

	CHECK_PARAMETERS(dst, dstSize, dstLimit, dstValid, src, srcSize, srcLimit,
	                 srcLimited, ret);


	UTF8_CHECK_BOM(src, srcLimit, srcLimited, srcCnt);

	while (*src && (srcCnt < srcLimit || !srcLimited))
	{
		char32_t c32; // why is it out here>

		char8_t cur = *src;
		
		CHECK_DST_SPACE(dstCnt, dstLimit, dstValid, ret);

		CHECK_BREAK_TYPE(dst, dstCnt, dstLimit, dstValid, src, srcCnt, srcLimit,
		                 srcLimited, breakType, ret);


		if (IS_ASCII(cur))
		{
			c32 = cur;
		}
		else
		{
			int srcMbLen = 0;

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
				THROW_AND_QUIT(ret, ENC_EILSEQ);
			}
			
			if (srcLimit - srcCnt < srcMbLen && srcLimited)
				break;
			
			c32 = ENCiConvertUtf8To32(src, srcMbLen);

			if (!c32)
			{
				THROW_AND_QUIT(ret, ENC_EILSEQ);
			}

			src += srcMbLen - 1;
			srcCnt += srcMbLen - 1;
		}

		COPY_CHAR_AS_IS(c32, dst, dstCnt, dstValid, src, srcCnt);
	}

	WRITE_BACK_SIZES(srcSize, srcCnt, dstSize, dstCnt);

	return ret;
}

ENCResult ENCiConvertStringUtf16ToUtf8(char8_t *dst, unk_t signed *dstSize,
                                       char16_t const *src,
                                       unk_t signed *srcSize,
                                       ENCBreakType breakType)
{
	CREATE_STATE_VARIABLES(dstCnt, dstLimit, dstValid, srcCnt, srcLimit,
	                       srcLimited)

	ENCResult ret;

	CHECK_PARAMETERS(dst, dstSize, dstLimit, dstValid, src, srcSize, srcLimit,
	                 srcLimited, ret);

	UTF16_CHECK_BOM(src, srcSize, srcLimit, srcLimited, srcCnt, dstSize);

	while (*src && (srcCnt < srcLimit || !srcLimited))
	{
		char16_t cur = *src;

		CHECK_DST_SPACE(dstCnt, dstLimit, dstValid, ret);

		CHECK_BREAK_TYPE(dst, dstCnt, dstLimit, dstValid, src, srcCnt, srcLimit,
		                 srcLimited, breakType, ret);

		if (IS_ASCII(cur))
		{
			COPY_CHAR_AS_IS(cur, dst, dstCnt, dstValid, src, srcCnt);
		}
		else
		{
			int dstMbLen = 0;
			int srcMbLen = 0;

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
					THROW_AND_QUIT(ret, ENC_ENOSPC);
				}

				char32_t c32 = ENCiConvertUtf16To32(src, dstMbLen);
				ENCiConvertUtf32To8(dst, srcMbLen, c32);

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

ENCResult ENCiConvertStringUtf8ToUtf16(char16_t *dst, unk_t signed *dstSize,
                                       char8_t const *src,
                                       unk_t signed *srcSize,
                                       ENCBreakType breakType)
{
	CREATE_STATE_VARIABLES(dstCnt, dstLimit, dstValid, srcCnt, srcLimit,
	                       srcLimited)

	ENCResult ret;

	CHECK_PARAMETERS(dst, dstSize, dstLimit, dstValid, src, srcSize, srcLimit,
	                 srcLimited, ret);

	UTF8_CHECK_BOM(src, srcLimit, srcLimited, srcCnt);

	while (*src && (srcCnt < srcLimit || !srcLimited))
	{
		char8_t cur = *src;

		CHECK_DST_SPACE(dstCnt, dstLimit, dstValid, ret);

		CHECK_BREAK_TYPE(dst, dstCnt, dstLimit, dstValid, src, srcCnt, srcLimit,
		                 srcLimited, breakType, ret);

		if (IS_ASCII(cur))
		{
			COPY_CHAR_AS_IS(cur, dst, dstCnt, dstValid, src, srcCnt);
		}
		else
		{
			int srcMbLen = 0;
			int dstMbLen = 0;

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
				THROW_AND_QUIT(ret, ENC_EILSEQ);
			}

			if (srcLimit - srcCnt < srcMbLen && srcLimited)
				break;

			char32_t c32 = ENCiConvertUtf8To32(src, srcMbLen);
			if (!c32)
			{
				THROW_AND_QUIT(ret, ENC_EILSEQ);
			}

			if (dstValid)
			{
				if (dstLimit - dstCnt < dstMbLen)
				{
					THROW_AND_QUIT(ret, ENC_ENOSPC);
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

ENCResult ENCiConvertStringUtf7ToUtf16(char16_t *dst, unk_t signed *dstSize,
                                       unsigned char const *src,
                                       unk_t signed *srcSize,
                                       ENCBreakType breakType, byte4_t *state)
{
	CREATE_STATE_VARIABLES(dstCnt, dstLimit, dstValid, srcCnt, srcLimit,
		                       srcLimited)

	BOOL _7BitMode = false;

	unk4_t unsigned a = 0;
	unk4_t unsigned b = 0;

	ENCResult ret;

	CHECK_PARAMETERS(dst, dstSize, dstLimit, dstValid, src, srcSize, srcLimit,
	                 srcLimited, ret);

	if (state && *state & 8)
	{
		_7BitMode = true;

		a = *state & 0xFFFC0000;
		b = (*state & 7) << 1;
	}

	while (*src && (srcCnt < srcLimit || !srcLimited))
	{
		if (_7BitMode)
		{
			unsigned char cur = *src;

			if (dstCnt >= dstLimit && dstValid && cur != '-')
			{
				THROW_AND_QUIT(ret, ENC_ENOSPC);
			}
			
			if (cur == '-')
			{
				_7BitMode = false;

				++src;
				++srcCnt;
				continue;
			}

			unk4_t unsigned c = ENCiGetBase64Value(cur);

			if (c > 63) // in particular, 0xff is the error sentinel
			{
				THROW_AND_QUIT(ret, ENC_EILSEQ);
			}

			a |= c << (26 - b);

			if (b < 10)
			{
				b += 6;
			}
			else
			{
				if (dstValid)
				{
					*dst = a >> 16;
					++dst;
				}

				a <<= 16;
				b -= 10;
				dstCnt += 1;
			}

			++src;
			++srcCnt;
		}
		else
		{
			unsigned char cur = *src;

			if (dstCnt >= dstLimit && dstValid && cur != '+')
			{
				THROW_AND_QUIT(ret, ENC_ENOSPC);
			}

			if (cur == '+')
			{
				_7BitMode = true;
				a = 0;
				b = 0;

				++src;
				++srcCnt;

				continue;
			}

			CHECK_BREAK_TYPE(dst, dstCnt, dstLimit, dstValid, src, srcCnt,
			                 srcLimit, srcLimited, breakType, ret);

			COPY_CHAR_AS_IS(cur, dst, dstCnt, dstValid, src, srcCnt);
		}
	}

	if (state)
	{
		if (_7BitMode)
			*state = 8 | (a & 0xFFFC0000) | ((unsigned)(b >> 1) & 7);
		else
			*state = 0;
	}

	WRITE_BACK_SIZES(srcSize, srcCnt, dstSize, dstCnt);

	return ret;
}

ENCResult ENCiConvertStringUtf16ToUtf16(char16_t *dst, unk_t signed *dstSize,
                                        char16_t const *src,
                                        unk_t signed *srcSize,
                                        ENCBreakType breakType,
                                        ENCEndianness *endianness)
{
	CREATE_STATE_VARIABLES(dstCnt, dstLimit, dstValid, srcCnt, srcLimit,
	                       srcLimited)

	BOOL littleEndian = false;

	ENCResult ret;

	CHECK_PARAMETERS(dst, dstSize, dstLimit, dstValid, src, srcSize, srcLimit,
	                 srcLimited, ret);

	if (srcLimit > 0 || !srcLimited)
	{
		if (*src == UTF16_BE_BOM)
		{
			if (endianness != nullptr)
				*endianness = ENC_BIG_ENDIAN;

			++srcCnt;
			++src;
		}
		else if (*src == UTF16_LE_BOM)
		{
			if (endianness != nullptr)
				*endianness = ENC_LITTLE_ENDIAN;

			littleEndian = true;
			++srcCnt;
			++src;
		}
		else
		{
			if (endianness != nullptr && *endianness == ENC_LITTLE_ENDIAN)
				littleEndian = true;
		}
	}

	while (*src && (srcCnt < srcLimit || !srcLimited))
	{
		char16_t cur = *src;

		if (littleEndian)
			cur = SWAP_ENDIAN_16(cur);

		if (dstCnt >= dstLimit && dstValid)
		{
			THROW_AND_QUIT(ret, ENC_ENOSPC);
		}
		
		if (breakType > 0)
		{
			int srcBrkLen = 0;

			char16_t x = 0;

			if (srcLimit - srcCnt > 1 || !srcLimited)
			{
				x = src[1];

				if (littleEndian)
					x = SWAP_ENDIAN_16(x);
			}

			srcBrkLen = ENCiCheckBreakType(cur, x);

			if (srcBrkLen > 0)
			{
				int dstBrkLen =
					ENCiWriteBreakType(dst, 2, breakType, dstValid);

				if (dstLimit - dstCnt < dstBrkLen && dstValid)
				{
					THROW_AND_QUIT(ret, ENC_ENOSPC);
				}
				
				src += srcBrkLen;
				srcCnt += srcBrkLen;
				dstCnt += dstBrkLen;
				
				if (dstValid)
					dst += dstBrkLen;

				continue;
			}
		}

		COPY_CHAR_AS_IS(cur, dst, dstCnt, dstValid, src, srcCnt);
	}

	WRITE_BACK_SIZES(srcSize, srcCnt, dstSize, dstCnt);

	return ret;
}

static char32_t ENCiConvertUtf8To32(char8_t const *src, int length)
{
	char32_t c32;
	int i;

	switch (length)
	{
	case 1:
		c32 = UTF8_GET_INITIAL_1(src[0]);
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
		return 0;
	}

	for (i = 1; i < length; ++i)
	{
		if (!UTF8_IS_CONT(src[i]))
			return 0;

		c32 <<= 6;
		c32 += UTF8_GET_CONT(src[i]);
	}

	return c32;
}

static void ENCiConvertUtf32To8(char8_t *dst, int length, char32_t c32)
{
	int i;

	switch (length)
	{
	case 1:
		dst[0] = UTF8_MAKE_INITIAL_1(c32);
		break;

	case 2:
		dst[0] = UTF8_MAKE_INITIAL_2(c32 >> 6 * 1);
		break;

	case 3:
		dst[0] = UTF8_MAKE_INITIAL_3(c32 >> 6 * 2);
		break;

	case 4:
		dst[0] = UTF8_MAKE_INITIAL_4(c32 >> 6 * 3);
		break;

	default:
		return;
	}

	for (i = 1; i < length; ++i)
		dst[i] = UTF8_MAKE_CONT((c32 >> (length - i - 1) * 6) & 0x3F);
}

static char32_t ENCiConvertUtf16To32(char16_t const *src, int length)
{
	if (length == 1)
	{
		return src[0];
	}
	/* else */ if (length == 2)
	{
		char32_t c32;

		c32 = UTF16_GET_HIGH_SURROGATE(src[0]);
		c32 += UTF16_GET_LOW_SURROGATE(src[1]);

		return c32;
	}
	/* else */
	{
		return 0;
	}
}

static void ENCiConvertUtf32To16(char16_t *dst, int length, char32_t c32)
{
	if (length == 1)
	{
		dst[0] = c32;
	}
	/* else */ if (length == 2)
	{
		dst[0] = UTF16_MAKE_HIGH_SURROGATE(c32 >> 10);
		dst[1] = UTF16_MAKE_LOW_SURROGATE(c32 & 0x3FF);
	}
	/* else */
	{
		/* I bet the lasses wrangling IDO would absolutely fucking hate the
		 * way I'm keeping these functions (kind of) consistent
		 */
	}
}

static u8 ENCiGetBase64Value(unsigned char c)
{
	if (IS_OUTSIDE_ASCII(c))
		return 0xff;

	return base64_array[c];
}
