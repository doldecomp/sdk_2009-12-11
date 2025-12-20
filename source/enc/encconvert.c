#include "encconvert.h"

/*******************************************************************************
 * headers
 */

#include <string.h>

#include <macros.h>

#include <revolution/enc.h>
#include "encchinese.h"
#include "encjapanese.h"
#include "enckorean.h"
#include "enclatin.h"
#include "encunicode.h"
#include "encutility.h" // ENCiRegisterVersion

/*******************************************************************************
 * local function declarations
 */

static ENCEncoding ENCiGetEncodingTypeFromHashTable(
	struct encoding_hash_table_entry const *table, int tableLength,
	unsigned char const *encodingName);

static ENCResult ENCiConvertToInternalEncoding(ENCContext *context,
                                               char16_t *dst, s32 *dstSize,
                                               void const *src, s32 *srcSize);
static ENCResult ENCiConvertFromInternalEncoding(ENCContext *context, void *dst,
                                                 s32 *dstSize,
                                                 char16_t const *src,
                                                 s32 *srcSize);

/*******************************************************************************
 * variables
 */

// .rodata

// clang-format off
struct encoding const encoding_array[] =
{
	{      (                0), false, ""            }, // none
	{sizeof(         char    ),  true, "US-ASCII"    }, // ASCII
	{sizeof(         char8_t ), false, "UTF-8"       }, // UTF-8
	{sizeof(         char16_t), false, "UTF-16BE"    }, // UTF-16
	{sizeof(         char32_t), false, "UTF-32BE"    }, // UTF-32
	{sizeof(unsigned char    ), false, "ISO-8859-1"  }, // extASCII
	{sizeof(unsigned char    ), false, "ISO-8859-2"  }, // extASCII
	{sizeof(unsigned char    ), false, "ISO-8859-3"  }, // extASCII
	{sizeof(unsigned char    ), false, "ISO-8859-7"  }, // extASCII
	{sizeof(unsigned char    ), false, "ISO-8859-10" }, // extASCII
	{sizeof(unsigned char    ), false, "ISO-8859-15" }, // extASCII
	{sizeof(         char    ),  true, "ISO-2022-JP" }, // stateful
	{sizeof(         byte_t  ), false, "Shift_JIS"   }, // multi-byte
	{sizeof(unsigned char    ), false, "windows-1252"}, // extASCII
	{sizeof(         byte_t  ), false, "EUC-KR"      }, // multi-byte
	{sizeof(         byte_t  ), false, "GB2312"      }, // multi-byte
	{sizeof(         char    ),  true, "UTF-7"       }, // stateful
	{sizeof(         char16_t), false, "UTF-16"      }, // UTF-16
	{sizeof(         char16_t), false, "UTF-16LE"    }, // UTF-16
	{sizeof(unsigned char    ), false, "windows-1250"}, // extASCII
	{sizeof(unsigned char    ), false, "windows-1253"}, // extASCII
	{sizeof(unsigned char    ), false, "macintosh"   }, // extASCII
	{sizeof(unsigned char    ), false, "x-mac-greek" }, // extASCII
	{sizeof(unsigned char    ), false, "x-mac-ce"    }, // extASCII
	{sizeof(unsigned char    ), false, "IBM850"      }, // extASCII
	{sizeof(unsigned char    ), false, "IBM852"      }, // extASCII
};

struct encoding_hash_table_entry const encoding_table[] =
{
	// Hash table entries
	// I would love to know how this table was sorted, that would be kinda cool
	// Hex indices redirect to another entry, decimal entries do not
	{"iso885910",               ENC_ENCODING_ISO_8859_10,     0},
	{"iso88591windows30latin1", ENC_ENCODING_WINDOWS_1252,    1},
	{"utf16le",                 ENC_ENCODING_UTF_16LE,     0x41}, // -> "iso88591windows31latin1"
	{"utf7",                    ENC_ENCODING_UTF_7,           3},
	{"utf8",                    ENC_ENCODING_UTF_8,           4},
	{"ascii",                   ENC_ENCODING_US_ASCII,     0x00}, // -> "iso885910"
	{"iso885915",               ENC_ENCODING_ISO_8859_15,  0x74}, // -> "euccn"
	{"iso10646ucs2be",          ENC_ENCODING_UTF_16BE,     0x3c}, // -> "mskanji "
	{"iso646irv1991",           ENC_ENCODING_US_ASCII,        8},
	{"iso10646ucs4be",          ENC_ENCODING_UTF_32BE,     0x65}, // -> "ksc5601"
	{"greek",                   ENC_ENCODING_ISO_8859_7,   0x06}, // -> "iso885915"
	{"ansix341968",             ENC_ENCODING_US_ASCII,     0x0c}, // -> "ansix341986"
	{"ansix341986",             ENC_ENCODING_US_ASCII,       12},
	{"isolatin1",               ENC_ENCODING_ISO_8859_1,     13},
	{"iso10646ucsbasic",        ENC_ENCODING_UTF_16BE,     0x28}, // -> "isolatin2"
	{"isolatin3",               ENC_ENCODING_ISO_8859_3,     15},
	{"iso646us",                ENC_ENCODING_US_ASCII,       16},
	{"iso10646ucs2le",          ENC_ENCODING_UTF_16LE,       17},
	{"isolatin6",               ENC_ENCODING_ISO_8859_10,    18},
	{"cp1200",                  ENC_ENCODING_UTF_16,         19},
	{"isoir6",                  ENC_ENCODING_US_ASCII,       20},
	{"unicode11utf8",           ENC_ENCODING_UTF_8,        0x19}, // -> "unicode20utf8"
	{"iso2022jp1",              ENC_ENCODING_ISO_2022_JP,  0x60}, // -> "euckr"
	{"iso2022jp2",              ENC_ENCODING_ISO_2022_JP,    23},
	{"cp1250",                  ENC_ENCODING_WINDOWS_1250,   24},
	{"unicode20utf8",           ENC_ENCODING_UTF_8,          25},
	{"cp1252",                  ENC_ENCODING_WINDOWS_1252,   26},
	{"cp1253",                  ENC_ENCODING_WINDOWS_1253,   27},
	{"l1",                      ENC_ENCODING_ISO_8859_1,   0x7e}, // -> "850"
	{"l2",                      ENC_ENCODING_ISO_8859_2,     29},
	{"l3",                      ENC_ENCODING_ISO_8859_3,   0x80}, // -> "852"
	{"unicode20",               ENC_ENCODING_UTF_16BE,       31},
	{"unicode20utf16",          ENC_ENCODING_UTF_16BE,     0x6b}, // -> "gb231280"
	{"l6",                      ENC_ENCODING_ISO_8859_10,  0x75}, // -> "iso88592windowslatin2"
	{"unicodeascii",            ENC_ENCODING_UTF_16BE,     0x34}, // -> "isolatingreek"
	{"iso10646",                ENC_ENCODING_UTF_16BE,       35},
	{"iso10646unicodelatin1",   ENC_ENCODING_UTF_16BE,       36},
	{"ucs4be",                  ENC_ENCODING_UTF_32BE,     0x2b}, // -> "greek8"
	{"unicodelatin1",           ENC_ENCODING_UTF_16BE,       38},
	{"ibm819",                  ENC_ENCODING_ISO_8859_1,     39},
	{"isolatin2",               ENC_ENCODING_ISO_8859_2,   0x6a}, // -> "gb2312"
	{"isoir101",                ENC_ENCODING_ISO_8859_2,   0x35}, // -> "iso885971987"
	{"windows949",              ENC_ENCODING_EUC_KR,       0x6f}, // -> "iso58gb231280"
	{"greek8",                  ENC_ENCODING_ISO_8859_7,     43},
	{"ecma118",                 ENC_ENCODING_ISO_8859_7,   0x49}, // -> "windows30latin1"
	{"iso885911987",            ENC_ENCODING_ISO_8859_1,   0x50}, // -> "windows31latin1"
	{"iso885921987",            ENC_ENCODING_ISO_8859_2,   0x7a}, // -> "windows31latin2"
	{"mac",                     ENC_ENCODING_MACINTOSH,      47},
	{"iso885931988",            ENC_ENCODING_ISO_8859_3,     48},
	{"utf32",                   ENC_ENCODING_UTF_32BE,       49},
	{"isoir100",                ENC_ENCODING_ISO_8859_1,     50},
	{"utf16",                   ENC_ENCODING_UTF_16,       0x29}, // -> "isoir101"
	{"isolating" "reek",        ENC_ENCODING_ISO_8859_7,     52}, // Well then maybe it shouldn't be so isolating
	{"iso885971987",            ENC_ENCODING_ISO_8859_7,     53},
	{"sjis",                    ENC_ENCODING_SHIFT_JIS,    0x37}, // -> "suneugreek"
	{"suneugreek",              ENC_ENCODING_ISO_8859_7,     55},
	{"mscp932",                 ENC_ENCODING_SHIFT_JIS,      56},
	{"macgreek",                ENC_ENCODING_X_MAC_GREEK,    57},
	{"isoir126",                ENC_ENCODING_ISO_8859_7,     58},
	{"isoir109",                ENC_ENCODING_ISO_8859_3,     59},
	{"mskanji" /* + */ " ",     ENC_ENCODING_SHIFT_JIS,      60}, // NOTE the trailing space here (ERRATUM?)
	{"windows31j",              ENC_ENCODING_SHIFT_JIS,      61},
	{"isoir157",                ENC_ENCODING_ISO_8859_15,  0x68}, // -> "uhc"
	{"macintosh",               ENC_ENCODING_MACINTOSH,    0x64}, // -> "isoir149"
	{"ucs2be",                  ENC_ENCODING_UTF_16BE,       64},
	{"iso88591windows31latin1", ENC_ENCODING_WINDOWS_1252,   65},
	{"iso10646ucs2",            ENC_ENCODING_UTF_16BE,     0x25}, // -> "ucs4be"
	{"unicode11",               ENC_ENCODING_UTF_16BE,     0x1f}, // -> "unicode20"
	{"iso10646ucs4",            ENC_ENCODING_UTF_32BE,       68},
	{"latin1",                  ENC_ENCODING_ISO_8859_1,     69},
	{"latin2",                  ENC_ENCODING_ISO_8859_2,   0x51}, // -> "unicode11utf7"
	{"latin3",                  ENC_ENCODING_ISO_8859_3,   0x15}, // -> "unicode11utf8"
	{"macroman",                ENC_ENCODING_MACINTOSH,    0x23}, // -> "iso10646"
	{"windows30latin1",         ENC_ENCODING_WINDOWS_1252,   73},
	{"latin6",                  ENC_ENCODING_ISO_8859_10,  0x5d}, // -> "ucs2le"
	{"cp65000",                 ENC_ENCODING_UTF_7,          75},
	{"windows1250",             ENC_ENCODING_WINDOWS_1250,   76},
	{"latin9",                  ENC_ENCODING_ISO_8859_15,  0x38}, // -> "mscp932"
	{"windows1252",             ENC_ENCODING_WINDOWS_1252, 0x10}, // -> "iso646us"
	{"windows1253",             ENC_ENCODING_WINDOWS_1253,   79},
	{"windows31latin1",         ENC_ENCODING_WINDOWS_1252,   80},
	{"unicode11utf7",           ENC_ENCODING_UTF_7,        0x5b}, // -> "unicode20utf7"
	{"ibm850",                  ENC_ENCODING_IBM850,       0x3d}, // -> "windows31j"
	{"elot928",                 ENC_ENCODING_ISO_8859_7,     83},
	{"ibm852",                  ENC_ENCODING_IBM852,         84},
	{"ibm367",                  ENC_ENCODING_US_ASCII,       85},
	{"iso88591",                ENC_ENCODING_ISO_8859_1,     86},
	{"iso88592",                ENC_ENCODING_ISO_8859_2,   0x27}, // -> "ibm819"
	{"iso88593",                ENC_ENCODING_ISO_8859_3,   0x14}, // -> "isoir6"
	{"iso8859101992",           ENC_ENCODING_ISO_8859_15,    89},
	{"chinese",                 ENC_ENCODING_GB2312,         90},
	{"unicode20utf7",           ENC_ENCODING_UTF_7,          91},
	{"iso88597",                ENC_ENCODING_ISO_8859_7,   0x0b}, // -> "ansix341968"
	{"ucs2le",                  ENC_ENCODING_UTF_16LE,       93},
	{"shiftjis",                ENC_ENCODING_SHIFT_JIS,      94},
	{"pcp852",                  ENC_ENCODING_IBM852,         95},
	{"euckr",                   ENC_ENCODING_EUC_KR,         96},
	{"ksc56011987",             ENC_ENCODING_EUC_KR,         97},
	{"iso10646j1",              ENC_ENCODING_UTF_16BE,       98},
	{"ksc56011989",             ENC_ENCODING_EUC_KR,         99},
	{"isoir149",                ENC_ENCODING_EUC_KR,        100},
	{"ksc5601",                 ENC_ENCODING_EUC_KR,        101},
	{"iso2022jp",               ENC_ENCODING_ISO_2022_JP,   102},
	{"us",                      ENC_ENCODING_US_ASCII,     0x22}, // -> "unicodeascii"
	{"uhc",                     ENC_ENCODING_EUC_KR,        104},
	{"cp949",                   ENC_ENCODING_EUC_KR,        105},
	{"gb2312",                  ENC_ENCODING_GB2312,       0x6d}, // -> "isoir58"
	{"gb231280",                ENC_ENCODING_GB2312,        107},
	{"usascii",                 ENC_ENCODING_US_ASCII,      108},
	{"isoir58",                 ENC_ENCODING_GB2312,        109},
	{"cp850",                   ENC_ENCODING_IBM850,       0x24}, // -> "iso10646unicodelatin1"
	{"iso58gb231280",           ENC_ENCODING_GB2312,        111},
	{"cp852",                   ENC_ENCODING_IBM852,        112},
	{"cp367",                   ENC_ENCODING_US_ASCII,      113},
	{"utf8n",                   ENC_ENCODING_UTF_8,         114},
	{"cp819",                   ENC_ENCODING_ISO_8859_1,    115},
	{"euccn",                   ENC_ENCODING_GB2312,        116},
	{"iso88592windowslatin2",   ENC_ENCODING_WINDOWS_1250,  117},
	{"macce",                   ENC_ENCODING_X_MAC_CE,     0x20}, // -> "unicode20utf16"
	{"utf32be",                 ENC_ENCODING_UTF_32BE,     0x69}, // -> "cp949"
	{"ibm1200",                 ENC_ENCODING_UTF_16,        120},
	{"utf16be",                 ENC_ENCODING_UTF_16BE,      121},
	{"windows31latin2",         ENC_ENCODING_WINDOWS_1250,  122},
	{"ucs2",                    ENC_ENCODING_UTF_16BE,     0x7f}, // -> "pc850multilingual"
	{"korean",                  ENC_ENCODING_EUC_KR,        124},
	{"ucs4",                    ENC_ENCODING_UTF_32BE,      125},
	{"850",                     ENC_ENCODING_IBM850,        126},
	{"pc850multilingual",       ENC_ENCODING_IBM850,        127},
	{"852",                     ENC_ENCODING_IBM852,        128},

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
	context->brtype		= ENC_BR_KEEP;
	context->state		= ENC_STATE_INITIAL;
	context->nomap		= L'\0';
	context->invalid	= L'\0';

	return ENC_ESUCCESS;
}

ENCResult ENCDuplicateContext(ENCContext *dst, ENCContext const *src)
{
	if (dst == nullptr || src == nullptr)
		return ENC_EINVAL;

	*dst = *src;

	return ENC_ESUCCESS;
}

ENCResult ENCSetExternalEncoding(ENCContext *context, char const *name)
{
	if (context == nullptr || name == nullptr)
		return ENC_EINVAL;

	context->encoding = ENCiGetEncodingTypeFromHashTable(
		encoding_table, ARRAY_LENGTH(encoding_table),
		(unsigned char const *)name);

	if (context->encoding == ENC_ENCODING_NONE)
		return ENC_EBADTYPE;

	if (!enc_tbl_jp_loaded
	    && (context->encoding == ENC_ENCODING_SHIFT_JIS
	        || context->encoding == ENC_ENCODING_ISO_2022_JP))
	{
		return ENC_ENOTSUPP;
	}

	if (!enc_tbl_kr_loaded && !enc_tbl_uhc_loaded
	    && context->encoding == ENC_ENCODING_EUC_KR)
	{
		return ENC_ENOTSUPP;
	}

	if (!enc_tbl_cn_loaded && context->encoding == ENC_ENCODING_GB2312)
		return ENC_ENOTSUPP;

	if (context->encoding == ENC_ENCODING_UTF_16BE
	    || context->encoding == ENC_ENCODING_UTF_16)
	{
		context->state = ENC_UTF16_STATE_BIG_ENDIAN;
	}
	else if (context->encoding == ENC_ENCODING_UTF_16LE)
	{
		context->state = ENC_UTF16_STATE_LITTLE_ENDIAN;
	}
	else if (context->encoding == ENC_ENCODING_ISO_2022_JP)
	{
		context->state = ENC_JIS_STATE_US_ASCII;
	}
	else if (context->encoding == ENC_ENCODING_UTF_7)
	{
		context->state = ENC_UTF7_STATE_INITIAL;
	}
	else
	{
		context->state = ENC_STATE_INITIAL;
	}

	return ENC_ESUCCESS;
}

ENCResult ENCGetExternalEncoding(ENCContext *context, char *name)
{
	if (context == nullptr || name == nullptr)
		return ENC_EINVAL;

	if (context->encoding <= ENC_ENCODING_NONE
	    || context->encoding >= ENC_BAD_MAX_ENCODING)
	{
		strncpy(name, encoding_array[ENC_ENCODING_NONE].name,
		        ENCODING_NAME_LENGTH);

		return ENC_EBADTYPE;
	}
	else
	{
		strncpy(name, encoding_array[context->encoding].name,
		        ENCODING_NAME_LENGTH);

		return ENC_ESUCCESS;
	}
}

ENCResult ENCSetBreakType(ENCContext *context, ENCBreakType breakType)
{
	if (context == nullptr || breakType < ENC_BR_KEEP || breakType > ENC_BR_LF)
		return ENC_EINVAL;

	context->brtype	= breakType;

	return ENC_ESUCCESS;
}

ENCResult ENCSetAlternativeCharacter(ENCContext *context, char16_t nomap,
                                     char16_t invalid)
{
	if (context == nullptr)
		return ENC_EINVAL;

	context->nomap		= nomap;
	context->invalid	= invalid;

	return ENC_ESUCCESS;
}

ENCResult ENCGetExternalCharacterWidth(ENCContext *context, u32 *width)
{
	if (context == nullptr || width == nullptr)
		return ENC_EINVAL;

	if (context->encoding <= ENC_ENCODING_NONE
	    || context->encoding >= ENC_BAD_MAX_ENCODING)
	{
		return ENC_EBADTYPE;
	}

	*width = encoding_array[context->encoding].width;

	return ENC_ESUCCESS;
}

ENCResult ENCConvertToInternalEncoding(ENCContext *context, char16_t *dst,
                                       s32 *dstSize, void const *src,
                                       s32 *srcSize)
{
	ENCResult ret;

	if (context == nullptr || context->brtype < ENC_BR_KEEP
	    || context->brtype > ENC_BR_LF)
	{
		return ENC_EINVAL;
	}

	if (context->encoding <= ENC_ENCODING_NONE
	    || context->encoding >= ENC_BAD_MAX_ENCODING)
	{
		return ENC_EBADTYPE;
	}

	if (dstSize == nullptr || srcSize == nullptr)
		return ENC_EINVAL;

	u32 width = encoding_array[context->encoding].width;

	if (dst != nullptr && *dstSize > 0)
		*dstSize /= sizeof *dst;

	if (*srcSize > 0)
		*srcSize /= width;

	s32 tmpDstSize = *dstSize;
	s32 tmpSrcSize = *srcSize;
	s32 dstCnt = 0;
	s32 srcCnt = 0;

	ret = ENCiConvertToInternalEncoding(context, dst, &tmpDstSize, src,
	                                    &tmpSrcSize);

	while (ret != ENC_ESUCCESS)
	{
		char16_t replacement = L'\0';

		if ((ret != ENC_ERANGE || !context->nomap)
		    && (ret != ENC_EILSEQ || !context->invalid))
		{
			break;
		}

		dstCnt += tmpDstSize;
		srcCnt += tmpSrcSize;

		tmpDstSize = *dstSize - dstCnt;
		tmpSrcSize = *srcSize > 0 ? *srcSize - srcCnt : *srcSize;

		u32 nextWidth;
		ENCGetNextCharacterWidth(context, (byte_t const *)src + srcCnt,
		                         &nextWidth);

		srcCnt += nextWidth / width;

		tmpSrcSize =
			tmpSrcSize > 0 ? tmpSrcSize - nextWidth / width : tmpSrcSize;

		if (ret == ENC_EILSEQ && context->invalid != L'\uffff')
			replacement = context->invalid;
		else if (ret == ENC_ERANGE && context->nomap != L'\uffff')
			replacement = context->nomap;

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
				(byte_t const *)src + srcCnt, &tmpSrcSize);
		}
		else
		{
			ret = ENCiConvertToInternalEncoding(context, nullptr, &tmpDstSize,
			                                    (byte_t const *)src + srcCnt,
			                                    &tmpSrcSize);
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
                                         s32 *dstSize, char16_t const *src,
                                         s32 *srcSize)
{
	ENCResult ret;

	if (context == nullptr || context->brtype < ENC_BR_KEEP
	    || context->brtype > ENC_BR_LF)
	{
		return ENC_EINVAL;
	}

	if (context->encoding <= ENC_ENCODING_NONE
	    || context->encoding >= ENC_BAD_MAX_ENCODING)
	{
		return ENC_EBADTYPE;
	}

	// first encoding not convertible to "internal encoding" from
	if (context->encoding >= ENC_ENCODING_UTF_7)
		return ENC_ENOTSUPP;

	if (srcSize == nullptr)
		return ENC_EINVAL;

	u32 width = encoding_array[context->encoding].width;

	if (dst != nullptr && *dstSize > 0)
		*dstSize /= width;

	if (*srcSize > 0)
		*srcSize /= sizeof *src;

	s32 tmpDstSize = *dstSize;
	s32 tmpSrcSize = *srcSize;
	s32 dstCnt = 0;
	s32 srcCnt = 0;

	ret = ENCiConvertFromInternalEncoding(context, dst, &tmpDstSize, src,
	                                      &tmpSrcSize);

	while ((ret == ENC_ERANGE && context->nomap != L'\0')
	       || (ret == ENC_EILSEQ && context->invalid != L'\0'))
	{
		char16_t replacement = L'\0';

		dstCnt += tmpDstSize;
		srcCnt += tmpSrcSize;

		tmpDstSize = *dstSize - dstCnt;
		++srcCnt;
		tmpSrcSize = *srcSize > 0 ? *srcSize - srcCnt : *srcSize;

		if (ret == ENC_EILSEQ && context->invalid != L'\uffff')
			replacement = context->invalid;
		else if (ret == ENC_ERANGE && context->nomap != L'\uffff')
			replacement = context->nomap;

		if (replacement)
		{
			s32 replacementSize = sizeof(byte_t);

			if (dst)
			{
				ret = ENCiConvertFromInternalEncoding(
					context, (byte_t *)dst + dstCnt, &tmpDstSize, &replacement,
					&replacementSize);
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
				context, (byte_t *)dst + dstCnt, &tmpDstSize, src + srcCnt,
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

ENCResult ENCCheckEncoding(int *lastIndex, char const *names[], int num,
                           char16_t const *src, u32 srcSize)
{
	int i;

	ENCEncoding encodings[ENC_CHECK_ENCODING_MAX_NUM];
	s32 dstSize = ENC_DST_INVALID;

	ENCiRegisterVersion();

	if (lastIndex == nullptr || names == nullptr || num <= 0
	    || num > ENC_CHECK_ENCODING_MAX_NUM - 1 || src == nullptr)
	{
		return ENC_EINVAL;
	}

	for (i = 0; i < num; ++i)
	{
		char const *encodingName = names[i];
		*lastIndex = i;

		if (encodingName == nullptr)
			return ENC_EINVAL;

		ENCEncoding encoding = ENCiGetEncodingTypeFromHashTable(
			encoding_table, ARRAY_LENGTH(encoding_table),
			(unsigned char const *)encodingName);

		if (encoding == ENC_ENCODING_NONE)
			return ENC_EBADTYPE;

		if (encoding >= ENC_BAD_MAX_ENCODING)
			return ENC_ENOTSUPP;

		if (!enc_tbl_jp_loaded
		    && (encoding == ENC_ENCODING_SHIFT_JIS
		        || encoding == ENC_ENCODING_ISO_2022_JP))
		{
			return ENC_ENOTSUPP;
		}

		if (!enc_tbl_kr_loaded && !enc_tbl_uhc_loaded
		    && encoding == ENC_ENCODING_EUC_KR)
		{
			return ENC_ENOTSUPP;
		}

		if (!enc_tbl_cn_loaded && encoding == ENC_ENCODING_GB2312)
			return ENC_ENOTSUPP;

		encodings[i] = encoding;
	}

	ENCContext context;
	ENCInitContext(&context);

	for (i = 0; i < num; ++i)
	{
		s32 tmpSrcSize = srcSize / sizeof *src;

		*lastIndex = i;
		context.encoding = encodings[i];

		ENCResult ret = ENCiConvertFromInternalEncoding(
			&context, nullptr, &dstSize, src, &tmpSrcSize);

		if (ret == ENC_ESUCCESS)
			return ENC_ESUCCESS;
	}

	*lastIndex = ENC_CHECK_ENCODING_NOT_FOUND;
	return ENC_ESUCCESS;
}

ENCResult ENCIs7BitEncoding(BOOL *is7Bit, char const *encodingName)
{
	ENCiRegisterVersion();

	if (is7Bit == nullptr || encodingName == nullptr)
		return ENC_EINVAL;

	ENCEncoding encoding = ENCiGetEncodingTypeFromHashTable(
		encoding_table, ARRAY_LENGTH(encoding_table),
		(unsigned char const *)encodingName);

	*is7Bit = encoding_array[encoding].is7bit;

	if (encoding == ENC_ENCODING_NONE)
		return ENC_EBADTYPE;
	
	return ENC_ESUCCESS;
}

static ENCEncoding ENCiGetEncodingTypeFromHashTable(
	struct encoding_hash_table_entry const *table, int tableLength,
	unsigned char const *encodingName)
{
	int i;

	byte_t stripped[ENCODING_HASH_TABLE_ENTRY_NAME_LENGTH];
	int hash = 0;
	long cnt = 0;

	// /^(x-|cs)/i
	if (((encodingName[0] == 'x' || encodingName[0] == 'X')
	     && encodingName[1] == '-')
	    || ((encodingName[0] == 'c' || encodingName[0] == 'C')
	        && (encodingName[1] == 's' || encodingName[1] == 'S')))
	{
		i = 2; // skip them
	}
	else
	{
		i = 0;
	}

	while (cnt < ENCODING_HASH_TABLE_ENTRY_NAME_LENGTH)
	{
		/* Not sure this follows UTS #22 1.4 exactly; I think this code doesn't
		 * strip leading zeroes of digit sequences.
		 */
		unsigned char cc = encodingName[i];

		if (cc == '\0')
		{
			stripped[cnt++] = 0x00;
			break;
		}

		if ((cc >= '0' && cc < '9' + 1) || (cc > 'a' - 1 && cc <= 'z'))
			stripped[cnt++] = cc;
		else if (cc > 'A' - 1 && cc <= 'Z')
			stripped[cnt++] = cc + 0x20u; // ascii-tolower

		++i;
	}

	for (i = 0; i < cnt; ++i)
		hash += stripped[i];

	long index;
	for (index = hash % tableLength;
	     strncmp((char *)stripped, table[index].name,
	             ENCODING_HASH_TABLE_ENTRY_NAME_LENGTH)
	     != 0;
	     index = table[index].nextIndex)
	{
		if (index == table[index].nextIndex)
			return ENC_ENCODING_NONE;
	}

	return table[index].encoding;
}

ENCResult ENCGetNextCharacterWidth(ENCContext *context, void const *src,
                                   u32 *width)
{
	if (context == nullptr || src == nullptr || width == nullptr)
		return ENC_EINVAL;

	if (context->encoding <= ENC_ENCODING_NONE
	    || context->encoding >= ENC_BAD_MAX_ENCODING)
	{
		return ENC_EBADTYPE;
	}

	switch (context->encoding)
	{
	case ENC_ENCODING_NONE:
		return ENC_EBADTYPE;

	case ENC_ENCODING_US_ASCII:
		*width = sizeof(char);
		break;

	case ENC_ENCODING_UTF_8:
		*width = sizeof(char8_t);
		break;

	case ENC_ENCODING_UTF_16BE:
	{
		char16_t c16;
		memcpy(&c16, src, sizeof c16); // ?

		// why are we grabbing it? stripped debug print or something?

		*width = sizeof(char16_t);
	}
		break;

	case ENC_ENCODING_UTF_32BE:
		*width = sizeof(char32_t);
		break;

	case ENC_ENCODING_ISO_8859_1:
	case ENC_ENCODING_ISO_8859_2:
	case ENC_ENCODING_ISO_8859_3:
	case ENC_ENCODING_ISO_8859_7:
	case ENC_ENCODING_ISO_8859_10:
	case ENC_ENCODING_ISO_8859_15:
		*width = sizeof(unsigned char);
		break;

	case ENC_ENCODING_ISO_2022_JP:
		if (*(byte_t const *)src == '\x1b')
		{
			// Unknown length because this is an escape sequence
			*width = 0;
			break;
		}

		switch (context->state)
		{
		case ENC_JIS_STATE_INVALID:
		case ENC_JIS_STATE_US_ASCII:
		case ENC_JIS_STATE_JIS_C6220_1969_RO:
		case ENC_JIS_STATE_JIS_C6220_1969_JP:
			*width = sizeof(char);
			break;

		case ENC_JIS_STATE_JIS_X_0208_1978:
		case ENC_JIS_STATE_JIS_X_0208_1983:
		case ENC_JIS_STATE_JIS_X_0212_1990:
		case ENC_JIS_STATE_7: // ?
			*width = sizeof(byte2_t);
			break;

		default:
			*width = 1;
			break;
		}

		break;

	case ENC_ENCODING_SHIFT_JIS:
		if (*(byte_t const *)src >= 0x81 && *(byte_t const *)src <= 0xFC)
			*width = sizeof(byte2_t);
		else
			*width = sizeof(byte1_t);

		break;

	case ENC_ENCODING_WINDOWS_1252:
		*width = sizeof(unsigned char);
		break;

	case ENC_ENCODING_UTF_7:
		*width = sizeof(char);
		break;

	case ENC_ENCODING_UTF_16:
	case ENC_ENCODING_UTF_16LE:
	{
		char16_t c16;

		if (context->state == ENC_UTF16_STATE_BIG_ENDIAN)
			memcpy(&c16, src, sizeof c16);
		else // ENC_UTF16_STATE_LITTLE_ENDIAN
			c16 = ((byte_t const *)src)[1] << 8 | ((byte_t const *)src)[0];

		// why is it only the utf16 encodings? why not the other ones?

		*width = sizeof(char16_t);
	}
		break;

	case ENC_ENCODING_WINDOWS_1250:
	case ENC_ENCODING_WINDOWS_1253:
	case ENC_ENCODING_MACINTOSH:
	case ENC_ENCODING_X_MAC_GREEK:
	case ENC_ENCODING_X_MAC_CE:
	case ENC_ENCODING_IBM850:
	case ENC_ENCODING_IBM852:
		*width = sizeof(unsigned char);
		break;

	case ENC_ENCODING_EUC_KR:
	case ENC_ENCODING_GB2312:
		if (*(byte_t const *)src >= 0x81 && *(byte_t const *)src <= 0xFE)
			*width = sizeof(byte2_t);
		else
			*width = sizeof(byte1_t);

		break;

	default:
		return ENC_ENOTSUPP;
	}

	/* Possibly a side effect of declaring src as void const * instead of byte_t
	 * const *
	 */
	(void)src;

	return ENC_ESUCCESS;
}

static ENCResult ENCiConvertToInternalEncoding(ENCContext *context,
                                               char16_t *dst, s32 *dstSize,
                                               void const *src, s32 *srcSize)
{
	switch (context->encoding)
	{
	case ENC_ENCODING_NONE:
		return ENC_EBADTYPE;

	case ENC_ENCODING_US_ASCII:
		return ENCiConvertStringAsciiToUnicode(dst, dstSize, src, srcSize,
		                                       context->brtype);

	case ENC_ENCODING_UTF_8:
		return ENCiConvertStringUtf8ToUtf16(dst, dstSize, src, srcSize,
		                                    context->brtype);

	case ENC_ENCODING_UTF_16BE:
		return ENCiConvertStringUtf16ToUtf16(
			dst, dstSize, src, srcSize, context->brtype, &context->state);

	case ENC_ENCODING_UTF_32BE:
		return ENCiConvertStringUtf32ToUtf16(dst, dstSize, src, srcSize,
		                                     context->brtype);

	case ENC_ENCODING_ISO_8859_1:
		return ENCiConvertStringLatin1ToUnicode(dst, dstSize, src, srcSize,
		                                        context->brtype);

	case ENC_ENCODING_ISO_8859_2:
		return ENCiConvertStringLatin2ToUnicode(dst, dstSize, src, srcSize,
		                                        context->brtype);

	case ENC_ENCODING_ISO_8859_3:
		return ENCiConvertStringLatin3ToUnicode(dst, dstSize, src, srcSize,
		                                        context->brtype);

	case ENC_ENCODING_ISO_8859_7:
		return ENCiConvertStringGreekToUnicode(dst, dstSize, src, srcSize,
		                                       context->brtype);

	case ENC_ENCODING_ISO_8859_10:
		return ENCiConvertStringLatin6ToUnicode(dst, dstSize, src, srcSize,
		                                        context->brtype);

	case ENC_ENCODING_ISO_8859_15:
		return ENCiConvertStringLatin9ToUnicode(dst, dstSize, src, srcSize,
		                                        context->brtype);

	case ENC_ENCODING_ISO_2022_JP:
		return ENCiConvertStringJisToUnicode(
			dst, dstSize, src, srcSize, context->brtype, &context->state);

	case ENC_ENCODING_SHIFT_JIS:
		return ENCiConvertStringSjisToUnicode(dst, dstSize, src, srcSize,
		                                      context->brtype);

	case ENC_ENCODING_WINDOWS_1252:
		return ENCiConvertStringLatin1ToUnicode(dst, dstSize, src, srcSize,
		                                        context->brtype);

	case ENC_ENCODING_EUC_KR:
		return ENCiConvertStringUhcToUnicode(dst, dstSize, src, srcSize,
		                                     context->brtype);

	case ENC_ENCODING_GB2312:
		return ENCiConvertStringGb2312ToUnicode(dst, dstSize, src, srcSize,
		                                        context->brtype);

	case ENC_ENCODING_UTF_7:
		return ENCiConvertStringUtf7ToUtf16(
			dst, dstSize, src, srcSize, context->brtype, &context->state);

	case ENC_ENCODING_UTF_16:
		return ENCiConvertStringUtf16ToUtf16(
			dst, dstSize, src, srcSize, context->brtype, &context->state);

	case ENC_ENCODING_UTF_16LE:
		return ENCiConvertStringUtf16ToUtf16(
			dst, dstSize, src, srcSize, context->brtype, &context->state);

	case ENC_ENCODING_WINDOWS_1250:
		return ENCiConvertStringWin1250ToUnicode(dst, dstSize, src, srcSize,
		                                         context->brtype);

	case ENC_ENCODING_WINDOWS_1253:
		return ENCiConvertStringWin1253ToUnicode(dst, dstSize, src, srcSize,
		                                         context->brtype);

	case ENC_ENCODING_MACINTOSH:
		return ENCiConvertStringMacromanToUnicode(dst, dstSize, src, srcSize,
		                                          context->brtype);

	case ENC_ENCODING_X_MAC_GREEK:
		return ENCiConvertStringMacgreekToUnicode(dst, dstSize, src, srcSize,
		                                          context->brtype);

	case ENC_ENCODING_X_MAC_CE:
		return ENCiConvertStringMacceToUnicode(dst, dstSize, src, srcSize,
		                                       context->brtype);

	case ENC_ENCODING_IBM850:
		return ENCiConvertStringIbm850ToUnicode(dst, dstSize, src, srcSize,
		                                        context->brtype);

	case ENC_ENCODING_IBM852:
		return ENCiConvertStringIbm852ToUnicode(dst, dstSize, src, srcSize,
		                                        context->brtype);

	default:
		return ENC_ENOTSUPP;
	}
}

static ENCResult ENCiConvertFromInternalEncoding(ENCContext *context, void *dst,
                                                 s32 *dstSize,
                                                 char16_t const *src,
                                                 s32 *srcSize)
{
	switch (context->encoding)
	{
	case ENC_ENCODING_NONE:
		return ENC_EBADTYPE;

	case ENC_ENCODING_US_ASCII:
		return ENCiConvertStringUnicodeToAscii(dst, dstSize, src, srcSize,
		                                       context->brtype);

	case ENC_ENCODING_UTF_8:
		return ENCiConvertStringUtf16ToUtf8(dst, dstSize, src, srcSize,
		                                    context->brtype);

	case ENC_ENCODING_UTF_16BE:
		return ENCiConvertStringUtf16ToUtf16(
			dst, dstSize, src, srcSize, context->brtype, &context->state);

	case ENC_ENCODING_UTF_32BE:
		return ENCiConvertStringUtf16ToUtf32(dst, dstSize, src, srcSize,
		                                     context->brtype);

	case ENC_ENCODING_ISO_8859_1:
		return ENCiConvertStringUnicodeToLatin1(dst, dstSize, src, srcSize,
		                                        context->brtype);

	case ENC_ENCODING_ISO_8859_2:
		return ENCiConvertStringUnicodeToLatin2(dst, dstSize, src, srcSize,
		                                        context->brtype);

	case ENC_ENCODING_ISO_8859_3:
		return ENCiConvertStringUnicodeToLatin3(dst, dstSize, src, srcSize,
		                                        context->brtype);

	case ENC_ENCODING_ISO_8859_7:
		return ENCiConvertStringUnicodeToGreek(dst, dstSize, src, srcSize,
		                                       context->brtype);

	case ENC_ENCODING_ISO_8859_10:
		return ENCiConvertStringUnicodeToLatin6(dst, dstSize, src, srcSize,
		                                        context->brtype);

	case ENC_ENCODING_ISO_8859_15:
		return ENCiConvertStringUnicodeToLatin9(dst, dstSize, src, srcSize,
		                                        context->brtype);

	case ENC_ENCODING_ISO_2022_JP:
		return ENCiConvertStringUnicodeToJis(
			dst, dstSize, src, srcSize, context->brtype, &context->state);

	case ENC_ENCODING_SHIFT_JIS:
		return ENCiConvertStringUnicodeToSjis(dst, dstSize, src, srcSize,
		                                      context->brtype);

	case ENC_ENCODING_WINDOWS_1252:
		return ENCiConvertStringUnicodeToWin1252(dst, dstSize, src, srcSize,
		                                         context->brtype);

	case ENC_ENCODING_EUC_KR:
		return ENCiConvertStringUnicodeToUhc(dst, dstSize, src, srcSize,
		                                     context->brtype);

	case ENC_ENCODING_GB2312:
		return ENCiConvertStringUnicodeToGb2312(dst, dstSize, src, srcSize,
		                                        context->brtype);

	default:
		return ENC_ENOTSUPP;
	}
}
