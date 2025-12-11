#include "encconvert.h"

/*******************************************************************************
 * headers
 */

#include <string.h>

#include <decomp.h>
#include <macros.h>

#include <revolution/enc.h>
#include "encutility.h"

/*******************************************************************************
 * macros
 */

#undef NULL
#define NULL	((void *)(0))

/*******************************************************************************
 * local function declarations
 */

static ENCEncoding ENCiGetEncodingTypeFromHashTable(
	struct encoding_hash const *table, int tableLength,
	unsigned char const *encodingName);

static ENCResult ENCiConvertToInternalEncoding(ENCContext *context,
                                               char16_t *dst,
                                               unk4_t signed *dstSize,
                                               void const *src,
                                               unk4_t signed *srcSize);
static ENCResult ENCiConvertFromInternalEncoding(ENCContext *context, void *dst,
                                                 unk4_t signed *dstSize,
                                                 char16_t const *src,
                                                 unk4_t signed *srcSize);

/*******************************************************************************
 * variables
 */

// .rodata

// clang-format off
struct encoding const encoding_array[] =
{
	{0, false, ""            },
	{1,  true, "US-ASCII"    },
	{1, false, "UTF-8"       },
	{2, false, "UTF-16BE"    },
	{4, false, "UTF-32BE"    },
	{1, false, "ISO-8859-1"  },
	{1, false, "ISO-8859-2"  },
	{1, false, "ISO-8859-3"  },
	{1, false, "ISO-8859-7"  },
	{1, false, "ISO-8859-10" },
	{1, false, "ISO-8859-15" },
	{1,  true, "ISO-2022-JP" },
	{1, false, "Shift_JIS"   },
	{1, false, "windows-1252"},
	{1, false, "EUC-KR"      },
	{1, false, "GB2312"      },
	{1,  true, "UTF-7"       },
	{2, false, "UTF-16"      },
	{2, false, "UTF-16LE"    },
	{1, false, "windows-1250"},
	{1, false, "windows-1253"},
	{1, false, "macintosh"   },
	{1, false, "x-mac-greek" },
	{1, false, "x-mac-ce"    },
	{1, false, "IBM850"      },
	{1, false, "IBM852"      },
};

struct encoding_hash const encoding_table[] =
{
	// Hashes (TODO: this entry chosen how?)
	[0x00] = {"iso885910",               ENC_ENCODING_ISO_8859_10,       0},
	[0x01] = {"iso88591windows30latin1", ENC_ENCODING_WINDOWS_1252,      1},
	[0x03] = {"utf7",                    ENC_ENCODING_UTF_7,             3},
	[0x04] = {"utf8",                    ENC_ENCODING_UTF_8,             4},
	[0x08] = {"iso646irv1991",           ENC_ENCODING_US_ASCII,          8},
	[0x0c] = {"ansix341986",             ENC_ENCODING_US_ASCII,         12},
	[0x0d] = {"isolatin1",               ENC_ENCODING_ISO_8859_1,       13},
	[0x0f] = {"isolatin3",               ENC_ENCODING_ISO_8859_3,       15},
	[0x10] = {"iso646us",                ENC_ENCODING_US_ASCII,         16},
	[0x11] = {"iso10646ucs2le",          ENC_ENCODING_UTF_16LE,         17},
	[0x12] = {"isolatin6",               ENC_ENCODING_ISO_8859_10,      18},
	[0x13] = {"cp1200",                  ENC_ENCODING_UTF_16,           19},
	[0x14] = {"isoir6",                  ENC_ENCODING_US_ASCII,         20},
	[0x17] = {"iso2022jp2",              ENC_ENCODING_ISO_2022_JP,      23},
	[0x18] = {"cp1250",                  ENC_ENCODING_WINDOWS_1250,     24},
	[0x19] = {"unicode20utf8",           ENC_ENCODING_UTF_8,            25},
	[0x1a] = {"cp1252",                  ENC_ENCODING_WINDOWS_1252,     26},
	[0x1b] = {"cp1253",                  ENC_ENCODING_WINDOWS_1253,     27},
	[0x1d] = {"l2",                      ENC_ENCODING_ISO_8859_2,       29},
	[0x1f] = {"unicode20",               ENC_ENCODING_UTF_16BE,         31},
	[0x23] = {"iso10646",                ENC_ENCODING_UTF_16BE,         35},
	[0x24] = {"iso10646unicodelatin1",   ENC_ENCODING_UTF_16BE,         36},
	[0x26] = {"unicodelatin1",           ENC_ENCODING_UTF_16BE,         38},
	[0x27] = {"ibm819",                  ENC_ENCODING_ISO_8859_1,       39},
	[0x2b] = {"greek8",                  ENC_ENCODING_ISO_8859_7,       43},
	[0x2f] = {"mac",                     ENC_ENCODING_MACINTOSH,        47},
	[0x30] = {"iso885931988",            ENC_ENCODING_ISO_8859_3,       48},
	[0x31] = {"utf32",                   ENC_ENCODING_UTF_32BE,         49},
	[0x32] = {"isoir100",                ENC_ENCODING_ISO_8859_1,       50},
	[0x34] = {"isolating" "reek",        ENC_ENCODING_ISO_8859_7,       52},
	[0x35] = {"iso885971987",            ENC_ENCODING_ISO_8859_7,       53},
	[0x37] = {"suneugreek",              ENC_ENCODING_ISO_8859_7,       55},
	[0x38] = {"mscp932",                 ENC_ENCODING_SHIFT_JIS,        56},
	[0x39] = {"macgreek",                ENC_ENCODING_X_MAC_GREEK,      57},
	[0x3a] = {"isoir126",                ENC_ENCODING_ISO_8859_7,       58},
	[0x3b] = {"isoir109",                ENC_ENCODING_ISO_8859_3,       59},
	[0x3c] = {"mskanji" /* + */ " ",     ENC_ENCODING_SHIFT_JIS,        60}, // NOTE the trailing space (ERRATUM?)
	[0x3d] = {"windows31j",              ENC_ENCODING_SHIFT_JIS,        61},
	[0x40] = {"ucs2be",                  ENC_ENCODING_UTF_16BE,         64},
	[0x41] = {"iso88591windows31latin1", ENC_ENCODING_WINDOWS_1252,     65},
	[0x44] = {"iso10646ucs4",            ENC_ENCODING_UTF_32BE,         68},
	[0x45] = {"latin1",                  ENC_ENCODING_ISO_8859_1,       69},
	[0x49] = {"windows30latin1",         ENC_ENCODING_WINDOWS_1252,     73},
	[0x4b] = {"cp65000",                 ENC_ENCODING_UTF_7,            75},
	[0x4c] = {"windows1250",             ENC_ENCODING_WINDOWS_1250,     76},
	[0x4f] = {"windows1253",             ENC_ENCODING_WINDOWS_1253,     79},
	[0x50] = {"windows31latin1",         ENC_ENCODING_WINDOWS_1252,     80},
	[0x53] = {"elot928",                 ENC_ENCODING_ISO_8859_7,       83},
	[0x54] = {"ibm852",                  ENC_ENCODING_IBM852,           84},
	[0x55] = {"ibm367",                  ENC_ENCODING_US_ASCII,         85},
	[0x56] = {"iso88591",                ENC_ENCODING_ISO_8859_1,       86},
	[0x59] = {"iso8859101992",           ENC_ENCODING_ISO_8859_15,      89},
	[0x5a] = {"chinese",                 ENC_ENCODING_GB3212,           90},
	[0x5b] = {"unicode20utf7",           ENC_ENCODING_UTF_7,            91},
	[0x5d] = {"ucs2le",                  ENC_ENCODING_UTF_16LE,         93},
	[0x5e] = {"shiftjis",                ENC_ENCODING_SHIFT_JIS,        94},
	[0x5f] = {"pcp852",                  ENC_ENCODING_IBM852,           95},
	[0x60] = {"euckr",                   ENC_ENCODING_EUC_KR,           96},
	[0x61] = {"ksc56011987",             ENC_ENCODING_EUC_KR,           97},
	[0x62] = {"iso10646j1",              ENC_ENCODING_UTF_16BE,         98},
	[0x63] = {"ksc56011989",             ENC_ENCODING_EUC_KR,           99},
	[0x64] = {"isoir149",                ENC_ENCODING_EUC_KR,          100},
	[0x65] = {"ksc5601",                 ENC_ENCODING_EUC_KR,          101},
	[0x66] = {"iso2022jp",               ENC_ENCODING_ISO_2022_JP,     102},
	[0x68] = {"uhc",                     ENC_ENCODING_EUC_KR,          104},
	[0x69] = {"cp949",                   ENC_ENCODING_EUC_KR,          105},
	[0x6b] = {"gb231280",                ENC_ENCODING_GB3212,          107},
	[0x6c] = {"usascii",                 ENC_ENCODING_US_ASCII,        108},
	[0x6d] = {"isoir58",                 ENC_ENCODING_GB3212,          109},
	[0x6f] = {"iso58gb231280",           ENC_ENCODING_GB3212,          111},
	[0x70] = {"cp852",                   ENC_ENCODING_IBM852,          112},
	[0x71] = {"cp367",                   ENC_ENCODING_US_ASCII,        113},
	[0x72] = {"utf8n",                   ENC_ENCODING_UTF_8,           114},
	[0x73] = {"cp819",                   ENC_ENCODING_ISO_8859_1,      115},
	[0x74] = {"euccn",                   ENC_ENCODING_GB3212,          116},
	[0x75] = {"iso88592windowslatin2",   ENC_ENCODING_WINDOWS_1250,    117},
	[0x78] = {"ibm1200",                 ENC_ENCODING_UTF_16,          120},
	[0x79] = {"utf16be",                 ENC_ENCODING_UTF_16BE,        121},
	[0x7a] = {"windows31latin2",         ENC_ENCODING_WINDOWS_1250,    122},
	[0x7c] = {"korean",                  ENC_ENCODING_EUC_KR,          124},
	[0x7d] = {"ucs4",                    ENC_ENCODING_UTF_32BE,        125},
	[0x7e] = {"850",                     ENC_ENCODING_IBM850,          126},
	[0x7f] = {"pc850multilingual",       ENC_ENCODING_IBM850,          127},
	[0x80] = {"852",                     ENC_ENCODING_IBM852,          128},

#if 0
	// Hash collision slides, in order of the first entries' appearance
	// idk i thought it was interesting

	"utf16le"          -> "iso88591windows31latin1"
	"ascii"            -> "iso885910"
	"iso10646ucs2be"   -> "mskanji "
	"iso10646ucs4be"   -> "ksc5601"
	"greek"            -> "iso885915"               -> "euccn"
	"iso10646ucsbasic" -> "isolatin2"               -> "gb2312"        -> "isoir58"
	"iso2022jp1"       -> "euckr"
	"l1"               -> "850"
	"l3"               -> "852"
	"l6"               -> "iso88592windowslatin2"
	"windows949"       -> "iso58gb231280"
	"ecma118"          -> "windows30latin1"
	"iso885911987"     -> "windows31latin1"
	"iso885921987"     -> "windows31latin2"
	"utf16"            -> "isoir101"                -> "iso885971987"
	"sjis"             -> "suneugreek"
	"isoir157"         -> "uhc"
	"macintosh"        -> "isoir149"
	"iso10646ucs2"     -> "ucs4be"                  -> "greek8"
	"unicode11"        -> "unicode20"
	"latin2"           -> "unicode11utf7"           -> "unicode20utf7"
	"latin3"           -> "unicode11utf8"           -> "unicode20utf8"
	"macroman"         -> "iso10646"
	"latin6"           -> "ucs2le"
	"latin9"           -> "mscp932"
	"windows1252"      -> "iso646us"
	"ibm850"           -> "windows31j"
	"iso88592"         -> "ibm819"
	"iso88593"         -> "isoir6"
	"iso88597"         -> "ansix341968"             -> "ansix341986"
	"us"               -> "unicodeascii"            -> "isolatingreek"
	"cp850"            -> "iso10646unicodelatin1"
	"macce"            -> "unicode20utf16"          -> "gb231280"
	"utf32be"          -> "cp949"
	"ucs2"             -> "pc850multilingual"
#endif

	// Collisions placed in free slots (TODO: sorted how?)
	[  2] = {"utf16le",                 ENC_ENCODING_UTF_16LE,     0x41}, // -> "iso88591windows31latin1"
	[  5] = {"ascii",                   ENC_ENCODING_US_ASCII,     0x00}, // -> "iso885910"
	[  6] = {"iso885915",               ENC_ENCODING_ISO_8859_15,  0x74}, // -> "euccn"
	[  7] = {"iso10646ucs2be",          ENC_ENCODING_UTF_16BE,     0x3c}, // -> "mskanji "
	[  9] = {"iso10646ucs4be",          ENC_ENCODING_UTF_32BE,     0x65}, // -> "ksc5601"
	[ 10] = {"greek",                   ENC_ENCODING_ISO_8859_7,   0x06}, // -> "iso885915"
	[ 11] = {"ansix341968",             ENC_ENCODING_US_ASCII,     0x0c}, // -> "ansix341986"
	[ 14] = {"iso10646ucsbasic",        ENC_ENCODING_UTF_16BE,     0x28}, // -> "isolatin2"
	[ 21] = {"unicode11utf8",           ENC_ENCODING_UTF_8,        0x19}, // -> "unicode20utf8"
	[ 22] = {"iso2022jp1",              ENC_ENCODING_ISO_2022_JP,  0x60}, // -> "euckr"
	[ 28] = {"l1",                      ENC_ENCODING_ISO_8859_1,   0x7e}, // -> "850"
	[ 30] = {"l3",                      ENC_ENCODING_ISO_8859_3,   0x80}, // -> "852"
	[ 32] = {"unicode20utf16",          ENC_ENCODING_UTF_16BE,     0x6b}, // -> "gb231280"
	[ 33] = {"l6",                      ENC_ENCODING_ISO_8859_10,  0x75}, // -> "iso88592windowslatin2"
	[ 34] = {"unicodeascii",            ENC_ENCODING_UTF_16BE,     0x34}, // -> "isolatingreek"
	[ 37] = {"ucs4be",                  ENC_ENCODING_UTF_32BE,     0x2b}, // -> "greek8"
	[ 40] = {"isolatin2",               ENC_ENCODING_ISO_8859_2,   0x6a}, // -> "gb2312"
	[ 41] = {"isoir101",                ENC_ENCODING_ISO_8859_2,   0x35}, // -> "iso885971987"
	[ 42] = {"windows949",              ENC_ENCODING_EUC_KR,       0x6f}, // -> "iso58gb231280"
	[ 44] = {"ecma118",                 ENC_ENCODING_ISO_8859_7,   0x49}, // -> "windows30latin1"
	[ 45] = {"iso885911987",            ENC_ENCODING_ISO_8859_1,   0x50}, // -> "windows31latin1"
	[ 46] = {"iso885921987",            ENC_ENCODING_ISO_8859_2,   0x7a}, // -> "windows31latin2"
	[ 51] = {"utf16",                   ENC_ENCODING_UTF_16,       0x29}, // -> "isoir101"
	[ 54] = {"sjis",                    ENC_ENCODING_SHIFT_JIS,    0x37}, // -> "suneugreek"
	[ 62] = {"isoir157",                ENC_ENCODING_ISO_8859_15,  0x68}, // -> "uhc"
	[ 63] = {"macintosh",               ENC_ENCODING_MACINTOSH,    0x64}, // -> "isoir149"
	[ 66] = {"iso10646ucs2",            ENC_ENCODING_UTF_16BE,     0x25}, // -> "ucs4be"
	[ 67] = {"unicode11",               ENC_ENCODING_UTF_16BE,     0x1f}, // -> "unicode20"
	[ 70] = {"latin2",                  ENC_ENCODING_ISO_8859_2,   0x51}, // -> "unicode11utf7"
	[ 71] = {"latin3",                  ENC_ENCODING_ISO_8859_3,   0x15}, // -> "unicode11utf8"
	[ 72] = {"macroman",                ENC_ENCODING_MACINTOSH,    0x23}, // -> "iso10646"
	[ 74] = {"latin6",                  ENC_ENCODING_ISO_8859_10,  0x5d}, // -> "ucs2le"
	[ 77] = {"latin9",                  ENC_ENCODING_ISO_8859_15,  0x38}, // -> "mscp932"
	[ 78] = {"windows1252",             ENC_ENCODING_WINDOWS_1252, 0x10}, // -> "iso646us"
	[ 81] = {"unicode11utf7",           ENC_ENCODING_UTF_7,        0x5b}, // -> "unicode20utf7"
	[ 82] = {"ibm850",                  ENC_ENCODING_IBM850,       0x3d}, // -> "windows31j"
	[ 87] = {"iso88592",                ENC_ENCODING_ISO_8859_2,   0x27}, // -> "ibm819"
	[ 88] = {"iso88593",                ENC_ENCODING_ISO_8859_3,   0x14}, // -> "isoir6"
	[ 92] = {"iso88597",                ENC_ENCODING_ISO_8859_7,   0x0b}, // -> "ansix341968"
	[103] = {"us",                      ENC_ENCODING_US_ASCII,     0x22}, // -> "unicodeascii"
	[106] = {"gb2312",                  ENC_ENCODING_GB3212,       0x6d}, // -> "isoir58"
	[110] = {"cp850",                   ENC_ENCODING_IBM850,       0x24}, // -> "iso10646unicodelatin1"
	[118] = {"macce",                   ENC_ENCODING_X_MAC_CE,     0x20}, // -> "unicode20utf16"
	[119] = {"utf32be",                 ENC_ENCODING_UTF_32BE,     0x69}, // -> "cp949"
	[123] = {"ucs2",                    ENC_ENCODING_UTF_16BE,     0x7f}, // -> "pc850multilingual"
};
// clang-format on

// .sdata
ATTR_WEAK BOOL enc_tbl_jp_loaded = true;
ATTR_WEAK BOOL enc_tbl_kr_loaded = true;
ATTR_WEAK BOOL enc_tbl_uhc_loaded = true;
ATTR_WEAK BOOL enc_tbl_cn_loaded = true;

/*******************************************************************************
 * functions
 */

ENCResult ENCInitContext(ENCContext *context)
{
	ENCiRegisterVersion();

	if (context == nullptr)
		return ENC_EINVAL;

	context->encoding	= ENC_ENCODING_NONE;
	context->breakType	= ENC_BREAK_TYPE_NONE;
	context->mbState	= ENC_STATE_INITIAL;
	context->at_0x0c	= L'\0';
	context->at_0x0e	= L'\0';

	return ENC_ESUCCESS;
}

ENCResult ENCDuplicateContext(ENCContext *dst, ENCContext const *src)
{
	if (dst == nullptr || src == nullptr)
		return ENC_EINVAL;

	*dst = *src;

	return ENC_ESUCCESS;
}

ENCResult ENCSetExternalEncoding(ENCContext *context,
                                 unsigned char const *encoding)
{
	if (context == nullptr || encoding == nullptr)
		return ENC_EINVAL;

	context->encoding = ENCiGetEncodingTypeFromHashTable(
		encoding_table, ARRAY_LENGTH(encoding_table), encoding);

	if (context->encoding == ENC_ENCODING_NONE)
		return ENC_E5;

	if (!enc_tbl_jp_loaded
	    && (context->encoding == ENC_ENCODING_SHIFT_JIS
	        || context->encoding == ENC_ENCODING_ISO_2022_JP))
	{
		return ENC_E6;
	}

	if (!enc_tbl_kr_loaded && !enc_tbl_uhc_loaded
	    && context->encoding == ENC_ENCODING_EUC_KR)
	{
		return ENC_E6;
	}

	if (!enc_tbl_cn_loaded && context->encoding == ENC_ENCODING_GB3212)
		return ENC_E6;

	// TODO: Replace with proper state enums after moving them to the headers
	if (context->encoding == ENC_ENCODING_UTF_16BE
	    || context->encoding == ENC_ENCODING_UTF_16)
	{
		context->mbState = 0;
	}
	else if (context->encoding == ENC_ENCODING_UTF_16LE)
	{
		context->mbState = 1;
	}
	else if (context->encoding == ENC_ENCODING_ISO_2022_JP)
	{
		context->mbState = 1;
	}
	else if (context->encoding == ENC_ENCODING_UTF_7)
	{
		context->mbState = 0;
	}
	else
	{
		context->mbState = ENC_STATE_INITIAL;
	}

	return ENC_ESUCCESS;
}

ENCResult ENCGetExternalEncoding(ENCContext *context, unsigned char *encoding)
{
	if (context == nullptr || encoding == nullptr)
		return ENC_EINVAL;

	if (context->encoding <= ENC_ENCODING_NONE
	    || context->encoding >= ENC_MAX_ENCODING)
	{
		strncpy((char *)encoding,
		        (char *)encoding_array[ENC_ENCODING_NONE].name,
		        ENCODING_NAME_LENGTH);

		return ENC_E5;
	}
	else
	{
		strncpy((char *)encoding,
		        (char *)encoding_array[context->encoding].name,
		        ENCODING_NAME_LENGTH);

		return ENC_ESUCCESS;
	}
}

ENCResult ENCSetBreakType(ENCContext *context, ENCBreakType breakType)
{
	if (context == nullptr || breakType < ENC_BREAK_TYPE_NONE
	    || breakType > ENC_BREAK_TYPE_UNIX)
	{
		return ENC_EINVAL;
	}

	context->breakType = breakType;

	return ENC_ESUCCESS;
}

ENCResult ENCSetAlternativeCharacter(ENCContext *context, char16_t param_2,
                                     char16_t param_3)
{
	if (context == nullptr)
		return ENC_EINVAL;

	context->at_0x0c = param_2;
	context->at_0x0e = param_3;

	return ENC_ESUCCESS;
}

ENCResult ENCGetExternalCharacterWidth(ENCContext *context, u32 *width)
{
	if (context == nullptr || width == nullptr)
		return ENC_EINVAL;

	if (context->encoding <= ENC_ENCODING_NONE
	    || context->encoding >= ENC_MAX_ENCODING)
	{
		return ENC_E5;
	}

	*width = encoding_array[context->encoding].width;

	return ENC_ESUCCESS;
}

ENCResult ENCConvertToInternalEncoding(ENCContext *context, char16_t *dst,
                                       unk4_t signed *dstSize, void const *src,
                                       unk4_t signed *srcSize)
{
	ENCResult ret;

	if (context == nullptr || context->breakType < ENC_BREAK_TYPE_NONE
	    || context->breakType > ENC_BREAK_TYPE_UNIX)
	{
		return ENC_EINVAL;
	}

	if (context->encoding <= ENC_ENCODING_NONE
	    || context->encoding >= ENC_MAX_ENCODING)
	{
		return ENC_E5;
	}

	if (dstSize == nullptr || srcSize == nullptr)
		return ENC_EINVAL;

	u32 width = encoding_array[context->encoding].width;

	if (dst != nullptr && *dstSize > 0)
		*dstSize /= sizeof *dst;

	if (*srcSize > 0)
		*srcSize /= width;

	unk4_t signed tmpDstSize = *dstSize;
	unk4_t signed tmpSrcSize = *srcSize;
	unk4_t signed dstCnt = 0;
	unk4_t signed srcCnt = 0;

	ret = ENCiConvertToInternalEncoding(context, dst, &tmpDstSize, src,
	                                    &tmpSrcSize);

	while (ret != ENC_ESUCCESS)
	{
		char16_t replacement = L'\0';

		if ((ret != ENC_E2 || !context->at_0x0c)
		    && (ret != ENC_EILSEQ || !context->at_0x0e))
		{
			break;
		}

		dstCnt += tmpDstSize;
		srcCnt += tmpSrcSize;

		tmpDstSize = *dstSize - dstCnt;
		tmpSrcSize = *srcSize > 0 ? *srcSize - srcCnt : *srcSize;

		u32 nextWidth;
		ENCGetNextCharacterWidth(context, POINTER_ADD(src, srcCnt), &nextWidth);

		srcCnt += nextWidth / width;

		tmpSrcSize = tmpSrcSize > 0 ? tmpSrcSize - nextWidth / width : tmpSrcSize;

		if (ret == ENC_EILSEQ && context->at_0x0e != L'\uffff')
			replacement = context->at_0x0e;
		else if (ret == ENC_E2 && context->at_0x0c != L'\uffff')
			replacement = context->at_0x0c;

		if (replacement)
		{
			if (dst)
			{
				dst[dstCnt] = replacement;

				--tmpDstSize;
			}

			++dstCnt;
		}

		if (dst)
		{
			ret = ENCiConvertToInternalEncoding(
				context, dst + dstCnt, &tmpDstSize,
				(unsigned char const *)src + srcCnt, &tmpSrcSize);
		}
		else
		{
			ret = ENCiConvertToInternalEncoding(
				context, nullptr, &tmpDstSize,
				(unsigned char const *)src + srcCnt, &tmpSrcSize);
		}
	}

	*dstSize = dstCnt + tmpDstSize;
	*srcSize = srcCnt + tmpSrcSize;

	if (*dstSize > 0)
		*dstSize *= sizeof *dst;

	if (srcSize != nullptr && *srcSize > 0)
		*srcSize *= width;

	return ret;
}

ENCResult ENCConvertFromInternalEncoding(ENCContext *context, void *dst,
                                         unk4_t signed *dstSize,
                                         char16_t const *src,
                                         unk4_t signed *srcSize)
{
	ENCResult ret;

	if (context == nullptr || context->breakType < ENC_BREAK_TYPE_NONE
	    || context->breakType > ENC_BREAK_TYPE_UNIX)
	{
		return ENC_EINVAL;
	}

	if (context->encoding <= ENC_ENCODING_NONE
	    || context->encoding >= ENC_MAX_ENCODING)
	{
		return ENC_E5;
	}

	// first encoding not convertible to "internal encoding" from
	if (context->encoding >= ENC_ENCODING_UTF_7)
		return ENC_E6;

	if (srcSize == nullptr)
		return ENC_EINVAL;

	u32 width = encoding_array[context->encoding].width;

	if (dst != nullptr && *dstSize > 0)
		*dstSize /= width;

	if (*srcSize > 0)
		*srcSize /= sizeof *src;

	unk4_t signed tmpDstSize = *dstSize;
	unk4_t signed tmpSrcSize = *srcSize;
	unk4_t signed dstCnt = 0;
	unk4_t signed srcCnt = 0;

	ret = ENCiConvertFromInternalEncoding(context, dst, &tmpDstSize, src,
	                                      &tmpSrcSize);

	while ((ret == ENC_E2 && context->at_0x0c)
	       || (ret == ENC_EILSEQ && context->at_0x0e))
	{
		char16_t replacement = L'\0';

		dstCnt += tmpDstSize;
		srcCnt += tmpSrcSize;

		tmpDstSize = *dstSize - dstCnt;
		++srcCnt;
		tmpSrcSize = *srcSize > 0 ? *srcSize - srcCnt : *srcSize;

		if (ret == ENC_EILSEQ && context->at_0x0e != L'\uffff')
			replacement = context->at_0x0e;
		else if (ret == ENC_E2 && context->at_0x0c != L'\uffff')
			replacement = context->at_0x0c;

		if (replacement)
		{
			unk4_t signed replacementSize = 1;

			if (dst)
			{
				ret = ENCiConvertFromInternalEncoding(
					context, (unsigned char *)dst + dstCnt, &tmpDstSize,
					&replacement, &replacementSize);
			}
			else
			{
				ret = ENCiConvertFromInternalEncoding(context, nullptr,
				                                      &tmpDstSize, &replacement,
				                                      &replacementSize);
			}

			if (ret != ENC_ESUCCESS)
				break;

			dstCnt += tmpDstSize;
			tmpDstSize = *dstSize - dstCnt;
		}

		if (dst)
		{
			ret = ENCiConvertFromInternalEncoding(
				context, (unsigned char *)dst + dstCnt, &tmpDstSize, src + srcCnt,
				&tmpSrcSize);
		}
		else
		{
			ret = ENCiConvertFromInternalEncoding(context, nullptr, &tmpDstSize,
			                                      src + srcCnt, &tmpSrcSize);
		}
	}

	*dstSize = dstCnt + tmpDstSize;
	*srcSize = srcCnt + tmpSrcSize;

	if (*dstSize > 0)
		*dstSize *= width;

	if (srcSize != nullptr && *srcSize > 0)
		*srcSize *= sizeof *src;

	return ret;
}

ENCResult ENCCheckEncoding(int *lastIndex, unsigned char const *encodingNames[],
                           int num, char16_t const *src, u32 srcSize)
{
	int i;

	ENCEncoding encodings[ENC_CHECK_ENCODING_MAX_NUM];
	unk4_t signed dstSize = -1;

	ENCiRegisterVersion();

	if (lastIndex == nullptr || encodingNames == nullptr || num <= 0
	    || num > ENC_CHECK_ENCODING_MAX_NUM - 1 || src == nullptr)
	{
		return ENC_EINVAL;
	}

	for (i = 0; i < num; ++i)
	{
		unsigned char const *encodingName = encodingNames[i];
		*lastIndex = i;

		if (encodingName == nullptr)
			return ENC_EINVAL;

		ENCEncoding encoding = ENCiGetEncodingTypeFromHashTable(
			encoding_table, ARRAY_LENGTH(encoding_table), encodingName);

		if (encoding == ENC_ENCODING_NONE)
			return ENC_E5;

		if (encoding >= ENC_MAX_ENCODING)
			return ENC_E6;

		if (!enc_tbl_jp_loaded
		    && (encoding == ENC_ENCODING_SHIFT_JIS
		        || encoding == ENC_ENCODING_ISO_2022_JP))
		{
			return ENC_E6;
		}

		if (!enc_tbl_kr_loaded && !enc_tbl_uhc_loaded
		    && encoding == ENC_ENCODING_EUC_KR)
		{
			return ENC_E6;
		}

		if (!enc_tbl_cn_loaded && encoding == ENC_ENCODING_GB3212)
			return ENC_E6;

		encodings[i] = encoding;
	}

	ENCContext context;
	ENCInitContext(&context);

	for (i = 0; i < num; ++i)
	{
		unk4_t signed iSrcSize = srcSize / sizeof *src;

		*lastIndex = i;
		context.encoding = encodings[i];

		ENCResult ret = ENCiConvertFromInternalEncoding(
			&context, nullptr, &dstSize, src, &iSrcSize);

		if (ret == ENC_ESUCCESS)
			return ENC_ESUCCESS;
	}

	*lastIndex = -1;
	return ENC_ESUCCESS;
}

ENCResult ENCIs7BitEncoding(BOOL *is7Bit, unsigned char const *encodingName)
{
	ENCiRegisterVersion();

	if (is7Bit == nullptr || encodingName == nullptr)
		return ENC_EINVAL;

	ENCEncoding encoding = ENCiGetEncodingTypeFromHashTable(
		encoding_table, ARRAY_LENGTH(encoding_table), encodingName);

	*is7Bit = encoding_array[encoding].is7bit;

	if (encoding == ENC_ENCODING_NONE)
		return ENC_E5;
	
	return ENC_ESUCCESS;
}

static ENCEncoding ENCiGetEncodingTypeFromHashTable(
	struct encoding_hash const *table, int tableLength,
	unsigned char const *encodingName)
{
	int i;

	byte_t alnumName[ENCODING_TABLE_ENTRY_NAME_LENGTH];
	int hash = 0;
	long cnt = 0;

	// /^(x-|cs)/i
	if (((encodingName[0] == 'x' || encodingName[0] == 'X')
	     && encodingName[1] == '-')
	    || ((encodingName[0] == 'c' || encodingName[0] == 'C')
	        && (encodingName[1] == 's' || encodingName[1] == 'S')))
	{
		i = 2;
	}
	else
	{
		i = 0;
	}


	while (cnt < ENCODING_TABLE_ENTRY_NAME_LENGTH)
	{
		unsigned char cc = encodingName[i];

		if (cc == '\0')
		{
			alnumName[cnt++] = 0x00;
			break;
		}

		if ((cc >= '0' && cc < '9' + 1) || (cc > 'a' - 1 && cc <= 'z'))
			alnumName[cnt++] = cc;
		else if (cc > 'A' - 1 && cc <= 'Z')
			alnumName[cnt++] = cc + 0x20u; // ascii-tolower

		++i;
	}

	for (i = 0; i < cnt; ++i)
		hash += alnumName[i];

	long index;
	for (index = hash % tableLength;
	     strncmp((char *)alnumName, (char *)table[index].name,
	             ENCODING_TABLE_ENTRY_NAME_LENGTH)
	     != 0;
	     index = table[index].nextIndex)
	{
		if (index == table[index].nextIndex)
			return ENC_ENCODING_NONE;
	}

	return table[index].encoding;
}

ENCResult ENCGetNextCharacterWidth(ENCContext *context,
                                   unsigned char const *stream, u32 *width)
{
	if (context == nullptr || stream == nullptr || width == nullptr)
		return ENC_EINVAL;

	if (context->encoding <= ENC_ENCODING_NONE
	    || context->encoding >= ENC_MAX_ENCODING)
	{
		return ENC_E5;
	}

	switch (context->encoding)
	{
	case ENC_ENCODING_NONE:
		return ENC_E5;

	case ENC_ENCODING_US_ASCII:
		*width = 1;
		break;

	case ENC_ENCODING_UTF_8:
		*width = 1;
		break;

	case ENC_ENCODING_UTF_16BE:
	{
		char16_t c16;
		memcpy(&c16, stream, sizeof c16); // ?

		// why are we grabbing it? stripped debug print or something?

		*width = 2;
	}
		break;

	case ENC_ENCODING_UTF_32BE:
		*width = 4;
		break;

	case ENC_ENCODING_ISO_8859_1:
	case ENC_ENCODING_ISO_8859_2:
	case ENC_ENCODING_ISO_8859_3:
	case ENC_ENCODING_ISO_8859_7:
	case ENC_ENCODING_ISO_8859_10:
	case ENC_ENCODING_ISO_8859_15:
		*width = 1;
		break;

	case ENC_ENCODING_ISO_2022_JP:
		if (stream[0] == '\x1b')
		{
			*width = 0;
			break;
		}

		switch (context->mbState)
		{
		case 0:
		case 1:
		case 2:
		case 3:
			*width = 1;
			break;

		case 4:
		case 5:
		case 6:
		case 7:
			*width = 2;
			break;

		default:
			*width = 1;
			break;
		}

		break;

	case ENC_ENCODING_SHIFT_JIS:
		if (stream[0] >= 0x81 && stream[0] <= 0xFC)
			*width = 2;
		else
			*width = 1;

		break;

	case ENC_ENCODING_WINDOWS_1252:
		*width = 1;
		break;

	case ENC_ENCODING_UTF_7:
		*width = 1;
		break;

	case ENC_ENCODING_UTF_16:
	case ENC_ENCODING_UTF_16LE:
	{
		char16_t c16;

		if (context->mbState == 0)
			memcpy(&c16, stream, sizeof c16);
		else
			c16 = stream[1] << 8 | stream[0];

		// why is it only the utf16 encodings? why not the other ones?

		*width = 2;
	}
		break;

	case ENC_ENCODING_WINDOWS_1250:
	case ENC_ENCODING_WINDOWS_1253:
	case ENC_ENCODING_MACINTOSH:
	case ENC_ENCODING_X_MAC_GREEK:
	case ENC_ENCODING_X_MAC_CE:
	case ENC_ENCODING_IBM850:
	case ENC_ENCODING_IBM852:
		*width = 1;
		break;

	case ENC_ENCODING_EUC_KR:
	case ENC_ENCODING_GB3212:
		if (stream[0] >= 0x81 && stream[0] <= 0xFE)
			*width = 2;
		else
			*width = 1;

		break;

	default:
		return ENC_E6;
	}

	return ENC_ESUCCESS;
}

#if defined(__clang__)
# pragma clang diagnostic ignored "-Wimplicit-function-declaration"
#endif

static ENCResult ENCiConvertToInternalEncoding(ENCContext *context,
                                               char16_t *dst,
                                               unk4_t signed *dstSize,
                                               void const *src,
                                               unk4_t signed *srcSize)
{
	switch (context->encoding)
	{
	case ENC_ENCODING_NONE:
		return ENC_E5;

	case ENC_ENCODING_US_ASCII:
		return ENCiConvertStringAsciiToUnicode(dst, dstSize, src, srcSize,
		                                       context->breakType);

	case ENC_ENCODING_UTF_8:
		return ENCiConvertStringUtf8ToUtf16(dst, dstSize, src, srcSize,
		                                    context->breakType);

	case ENC_ENCODING_UTF_16BE:
		return ENCiConvertStringUtf16ToUtf16(
			dst, dstSize, src, srcSize, context->breakType, &context->mbState);

	case ENC_ENCODING_UTF_32BE:
		return ENCiConvertStringUtf32ToUtf16(dst, dstSize, src, srcSize,
		                                     context->breakType);

	case ENC_ENCODING_ISO_8859_1:
		return ENCiConvertStringLatin1ToUnicode(dst, dstSize, src, srcSize,
		                                        context->breakType);

	case ENC_ENCODING_ISO_8859_2:
		return ENCiConvertStringLatin2ToUnicode(dst, dstSize, src, srcSize,
		                                        context->breakType);

	case ENC_ENCODING_ISO_8859_3:
		return ENCiConvertStringLatin3ToUnicode(dst, dstSize, src, srcSize,
		                                        context->breakType);

	case ENC_ENCODING_ISO_8859_7:
		return ENCiConvertStringGreekToUnicode(dst, dstSize, src, srcSize,
		                                       context->breakType);

	case ENC_ENCODING_ISO_8859_10:
		return ENCiConvertStringLatin6ToUnicode(dst, dstSize, src, srcSize,
		                                        context->breakType);

	case ENC_ENCODING_ISO_8859_15:
		return ENCiConvertStringLatin9ToUnicode(dst, dstSize, src, srcSize,
		                                        context->breakType);

	case ENC_ENCODING_ISO_2022_JP:
		return ENCiConvertStringJisToUnicode(
			dst, dstSize, src, srcSize, context->breakType, &context->mbState);

	case ENC_ENCODING_SHIFT_JIS:
		return ENCiConvertStringSjisToUnicode(dst, dstSize, src, srcSize,
		                                      context->breakType);

	case ENC_ENCODING_WINDOWS_1252:
		return ENCiConvertStringLatin1ToUnicode(dst, dstSize, src, srcSize,
		                                        context->breakType);

	case ENC_ENCODING_EUC_KR:
		return ENCiConvertStringUhcToUnicode(dst, dstSize, src, srcSize,
		                                     context->breakType);

	case ENC_ENCODING_GB3212:
		return ENCiConvertStringGb2312ToUnicode(dst, dstSize, src, srcSize,
		                                        context->breakType);

	case ENC_ENCODING_UTF_7:
		return ENCiConvertStringUtf7ToUtf16(
			dst, dstSize, src, srcSize, context->breakType, &context->mbState);

	case ENC_ENCODING_UTF_16:
		return ENCiConvertStringUtf16ToUtf16(
			dst, dstSize, src, srcSize, context->breakType, &context->mbState);

	case ENC_ENCODING_UTF_16LE:
		return ENCiConvertStringUtf16ToUtf16(
			dst, dstSize, src, srcSize, context->breakType, &context->mbState);

	case ENC_ENCODING_WINDOWS_1250:
		return ENCiConvertStringWin1250ToUnicode(dst, dstSize, src, srcSize,
		                                         context->breakType);

	case ENC_ENCODING_WINDOWS_1253:
		return ENCiConvertStringWin1253ToUnicode(dst, dstSize, src, srcSize,
		                                         context->breakType);

	case ENC_ENCODING_MACINTOSH:
		return ENCiConvertStringMacromanToUnicode(dst, dstSize, src, srcSize,
		                                          context->breakType);

	case ENC_ENCODING_X_MAC_GREEK:
		return ENCiConvertStringMacgreekToUnicode(dst, dstSize, src, srcSize,
		                                          context->breakType);

	case ENC_ENCODING_X_MAC_CE:
		return ENCiConvertStringMacceToUnicode(dst, dstSize, src, srcSize,
		                                       context->breakType);

	case ENC_ENCODING_IBM850:
		return ENCiConvertStringIbm850ToUnicode(dst, dstSize, src, srcSize,
		                                        context->breakType);

	case ENC_ENCODING_IBM852:
		return ENCiConvertStringIbm852ToUnicode(dst, dstSize, src, srcSize,
		                                        context->breakType);

	default:
		return ENC_E6;
	}
}

static ENCResult ENCiConvertFromInternalEncoding(ENCContext *context,
                                                 void *dst,
                                                 unk4_t signed *dstSize,
                                                 char16_t const *src,
                                                 unk4_t signed *srcSize)
{
	switch (context->encoding)
	{
	case ENC_ENCODING_NONE:
		return ENC_E5;

	case ENC_ENCODING_US_ASCII:
		return ENCiConvertStringUnicodeToAscii(dst, dstSize, src, srcSize,
		                                       context->breakType);

	case ENC_ENCODING_UTF_8:
		return ENCiConvertStringUtf16ToUtf8(dst, dstSize, src, srcSize,
		                                    context->breakType);

	case ENC_ENCODING_UTF_16BE:
		return ENCiConvertStringUtf16ToUtf16(
			dst, dstSize, src, srcSize, context->breakType, &context->mbState);

	case ENC_ENCODING_UTF_32BE:
		return ENCiConvertStringUtf16ToUtf32(dst, dstSize, src, srcSize,
		                                     context->breakType);

	case ENC_ENCODING_ISO_8859_1:
		return ENCiConvertStringUnicodeToLatin1(dst, dstSize, src, srcSize,
		                                        context->breakType);

	case ENC_ENCODING_ISO_8859_2:
		return ENCiConvertStringUnicodeToLatin2(dst, dstSize, src, srcSize,
		                                        context->breakType);

	case ENC_ENCODING_ISO_8859_3:
		return ENCiConvertStringUnicodeToLatin3(dst, dstSize, src, srcSize,
		                                        context->breakType);

	case ENC_ENCODING_ISO_8859_7:
		return ENCiConvertStringUnicodeToGreek(dst, dstSize, src, srcSize,
		                                       context->breakType);

	case ENC_ENCODING_ISO_8859_10:
		return ENCiConvertStringUnicodeToLatin6(dst, dstSize, src, srcSize,
		                                        context->breakType);

	case ENC_ENCODING_ISO_8859_15:
		return ENCiConvertStringUnicodeToLatin9(dst, dstSize, src, srcSize,
		                                        context->breakType);

	case ENC_ENCODING_ISO_2022_JP:
		return ENCiConvertStringUnicodeToJis(
			dst, dstSize, src, srcSize, context->breakType, &context->mbState);

	case ENC_ENCODING_SHIFT_JIS:
		return ENCiConvertStringUnicodeToSjis(dst, dstSize, src, srcSize,
		                                      context->breakType);

	case ENC_ENCODING_WINDOWS_1252:
		return ENCiConvertStringUnicodeToWin1252(dst, dstSize, src, srcSize,
		                                         context->breakType);

	case ENC_ENCODING_EUC_KR:
		return ENCiConvertStringUnicodeToUhc(dst, dstSize, src, srcSize,
		                                     context->breakType);

	case ENC_ENCODING_GB3212:
		return ENCiConvertStringUnicodeToGb2312(dst, dstSize, src, srcSize,
		                                        context->breakType);

	default:
		return ENC_E6;
	}
}
