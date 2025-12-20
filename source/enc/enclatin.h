#ifndef RVL_SDK_ENC_LATIN_H
#define RVL_SDK_ENC_LATIN_H

/*******************************************************************************
 * headers
 */

#include <uchar.h> // char16_t

#include <revolution/types.h>

#include <revolution/enc.h>
#include "encmacros.h"

/*******************************************************************************
 * types
 */

struct latin_hash_table_entry
{
	char16_t		c16;	// size 0x02, offset 0x00
	unsigned char	cc;		// size 0x01, offset 0x02
	u8				next;	// size 0x02, offset 0x03
}; // size 0x04

/*******************************************************************************
 * external globals
 */

extern char16_t const latin1_unicode_array[];
extern char16_t const latin2_unicode_array[];
extern char16_t const latin3_unicode_array[];
extern char16_t const greek_unicode_array[];
extern char16_t const latin6_unicode_array[];
extern char16_t const latin9_unicode_array[];
extern char16_t const win1250_unicode_array[];
extern char16_t const win1253_unicode_array[];
extern char16_t const macroman_unicode_array[];
extern char16_t const macgreek_unicode_array[];
extern char16_t const macce_unicode_array[];
extern char16_t const ibm850_unicode_array[];
extern char16_t const ibm852_unicode_array[];

extern struct latin_hash_table_entry const unicode_latin1_array[];
extern struct latin_hash_table_entry const unicode_latin2_array[];
extern struct latin_hash_table_entry const unicode_latin3_array[];
extern struct latin_hash_table_entry const unicode_greek_array[];
extern struct latin_hash_table_entry const unicode_latin6_array[];
extern struct latin_hash_table_entry const unicode_latin9_array[];
extern struct latin_hash_table_entry const unicode_win1252_array[];
extern struct latin_hash_table_entry const unicode_win1250_array[];
extern struct latin_hash_table_entry const unicode_win1253_array[];
extern struct latin_hash_table_entry const unicode_macroman_array[];
extern struct latin_hash_table_entry const unicode_macgreek_array[];
extern struct latin_hash_table_entry const unicode_macce_array[];
extern struct latin_hash_table_entry const unicode_ibm850_array[];
extern struct latin_hash_table_entry const unicode_ibm852_array[];

/*******************************************************************************
 * functions
 */

ENCResult ENCiConvertStringLatinToUnicode(char16_t *dst, s32 *dstSize,
                                          byte_t const *src, s32 *srcSize,
                                          ENCBreakType breakType,
                                          char16_t const *table);
ENCResult ENCiConvertStringUnicodeToLatin(
	byte_t *dst, s32 *dstSize, char16_t const *src, s32 *srcSize,
	ENCBreakType breakType, struct latin_hash_table_entry const *table);

DECLARE_PRIVATE_ENC_FUNCTION_TO_UTF16(Latin1)
DECLARE_PRIVATE_ENC_FUNCTION_FROM_UTF16(Latin1)
DECLARE_PRIVATE_ENC_FUNCTION_TO_UTF16(Latin2)
DECLARE_PRIVATE_ENC_FUNCTION_FROM_UTF16(Latin2)
DECLARE_PRIVATE_ENC_FUNCTION_TO_UTF16(Latin3)
DECLARE_PRIVATE_ENC_FUNCTION_FROM_UTF16(Latin3)
DECLARE_PRIVATE_ENC_FUNCTION_TO_UTF16(Greek)
DECLARE_PRIVATE_ENC_FUNCTION_FROM_UTF16(Greek)
DECLARE_PRIVATE_ENC_FUNCTION_TO_UTF16(Latin6)
DECLARE_PRIVATE_ENC_FUNCTION_FROM_UTF16(Latin6)
DECLARE_PRIVATE_ENC_FUNCTION_TO_UTF16(Latin9)
DECLARE_PRIVATE_ENC_FUNCTION_FROM_UTF16(Latin9)
DECLARE_PRIVATE_ENC_FUNCTION_TO_UTF16(Win1252)
DECLARE_PRIVATE_ENC_FUNCTION_FROM_UTF16(Win1252)

// All of these are only convertible from, not to
DECLARE_PRIVATE_ENC_FUNCTION_TO_UTF16(Win1250)
DECLARE_PRIVATE_ENC_FUNCTION_TO_UTF16(Win1253)
DECLARE_PRIVATE_ENC_FUNCTION_TO_UTF16(Macroman)
DECLARE_PRIVATE_ENC_FUNCTION_TO_UTF16(Macgreek)
DECLARE_PRIVATE_ENC_FUNCTION_TO_UTF16(Macce)
DECLARE_PRIVATE_ENC_FUNCTION_TO_UTF16(Ibm850)
DECLARE_PRIVATE_ENC_FUNCTION_TO_UTF16(Ibm852)

#endif // RVL_SDK_ENC_LATIN_H
