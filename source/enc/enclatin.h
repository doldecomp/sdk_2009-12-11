#ifndef RVL_SDK_ENC_LATIN_H
#define RVL_SDK_ENC_LATIN_H

/*******************************************************************************
 * headers
 */

#include <uchar.h>

#include <decomp.h>

#include <revolution/enc.h>
#include "encutility.h"

/*******************************************************************************
 * types
 */

struct hash_table
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

extern struct hash_table const unicode_latin1_array[];
extern struct hash_table const unicode_latin2_array[];
extern struct hash_table const unicode_latin3_array[];
extern struct hash_table const unicode_greek_array[];
extern struct hash_table const unicode_latin6_array[];
extern struct hash_table const unicode_latin9_array[];
extern struct hash_table const unicode_win1252_array[];
extern struct hash_table const unicode_win1250_array[];
extern struct hash_table const unicode_win1253_array[];
extern struct hash_table const unicode_macroman_array[];
extern struct hash_table const unicode_macgreek_array[];
extern struct hash_table const unicode_macce_array[];
extern struct hash_table const unicode_ibm850_array[];
extern struct hash_table const unicode_ibm852_array[];

/*******************************************************************************
 * functions
 */

ENCResult ENCiConvertStringLatin1ToUnicode(char16_t *dst,
                                           unk4_t signed *dstSize,
                                           unsigned char const *src,
                                           unk4_t signed *srcSize,
                                           ENCBreakType breakType);
ENCResult ENCiConvertStringUnicodeToLatin1(unsigned char *dst,
                                           unk4_t signed *dstSize,
                                           char16_t const *src,
                                           unk4_t signed *srcSize,
                                           ENCBreakType breakType);
ENCResult ENCiConvertStringLatin2ToUnicode(char16_t *dst,
                                           unk4_t signed *dstSize,
                                           unsigned char const *src,
                                           unk4_t signed *srcSize,
                                           ENCBreakType breakType);
ENCResult ENCiConvertStringUnicodeToLatin2(unsigned char *dst,
                                           unk4_t signed *dstSize,
                                           char16_t const *src,
                                           unk4_t signed *srcSize,
                                           ENCBreakType breakType);
ENCResult ENCiConvertStringLatin3ToUnicode(char16_t *dst,
                                           unk4_t signed *dstSize,
                                           unsigned char const *src,
                                           unk4_t signed *srcSize,
                                           ENCBreakType breakType);
ENCResult ENCiConvertStringUnicodeToLatin3(unsigned char *dst,
                                           unk4_t signed *dstSize,
                                           char16_t const *src,
                                           unk4_t signed *srcSize,
                                           ENCBreakType breakType);
ENCResult ENCiConvertStringGreekToUnicode(char16_t *dst, unk4_t signed *dstSize,
                                          unsigned char const *src,
                                          unk4_t signed *srcSize,
                                          ENCBreakType breakType);
ENCResult ENCiConvertStringUnicodeToGreek(unsigned char *dst,
                                          unk4_t signed *dstSize,
                                          char16_t const *src,
                                          unk4_t signed *srcSize,
                                          ENCBreakType breakType);
ENCResult ENCiConvertStringLatin6ToUnicode(char16_t *dst,
                                           unk4_t signed *dstSize,
                                           unsigned char const *src,
                                           unk4_t signed *srcSize,
                                           ENCBreakType breakType);
ENCResult ENCiConvertStringUnicodeToLatin6(unsigned char *dst,
                                           unk4_t signed *dstSize,
                                           char16_t const *src,
                                           unk4_t signed *srcSize,
                                           ENCBreakType breakType);
ENCResult ENCiConvertStringLatin9ToUnicode(char16_t *dst,
                                           unk4_t signed *dstSize,
                                           unsigned char const *src,
                                           unk4_t signed *srcSize,
                                           ENCBreakType breakType);
ENCResult ENCiConvertStringUnicodeToLatin9(unsigned char *dst,
                                           unk4_t signed *dstSize,
                                           char16_t const *src,
                                           unk4_t signed *srcSize,
                                           ENCBreakType breakType);
ENCResult ENCiConvertStringUnicodeToWin1252(unsigned char *dst,
                                            unk4_t signed *dstSize,
                                            char16_t const *src,
                                            unk4_t signed *srcSize,
                                            ENCBreakType breakType);
ENCResult ENCiConvertStringWin1250ToUnicode(char16_t *dst,
                                            unk4_t signed *dstSize,
                                            unsigned char const *src,
                                            unk4_t signed *srcSize,
                                            ENCBreakType breakType);
ENCResult ENCiConvertStringWin1253ToUnicode(char16_t *dst,
                                            unk4_t signed *dstSize,
                                            unsigned char const *src,
                                            unk4_t signed *srcSize,
                                            ENCBreakType breakType);
ENCResult ENCiConvertStringMacromanToUnicode(char16_t *dst,
                                             unk4_t signed *dstSize,
                                             unsigned char const *src,
                                             unk4_t signed *srcSize,
                                             ENCBreakType breakType);
ENCResult ENCiConvertStringMacgreekToUnicode(char16_t *dst,
                                             unk4_t signed *dstSize,
                                             unsigned char const *src,
                                             unk4_t signed *srcSize,
                                             ENCBreakType breakType);
ENCResult ENCiConvertStringMacceToUnicode(char16_t *dst, unk4_t signed *dstSize,
                                          unsigned char const *src,
                                          unk4_t signed *srcSize,
                                          ENCBreakType breakType);
ENCResult ENCiConvertStringIbm850ToUnicode(char16_t *dst,
                                           unk4_t signed *dstSize,
                                           unsigned char const *src,
                                           unk4_t signed *srcSize,
                                           ENCBreakType breakType);
ENCResult ENCiConvertStringIbm852ToUnicode(char16_t *dst,
                                           unk4_t signed *dstSize,
                                           unsigned char const *src,
                                           unk4_t signed *srcSize,
                                           ENCBreakType breakType);

#endif // RVL_SDK_ENC_LATIN_h
