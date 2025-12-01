#ifndef RVL_SDK_ENC_UTILITY_H
#define RVL_SDK_ENC_UTILITY_H

/*******************************************************************************
 * headers
 */

#include <decomp.h>

#include <revolution/types.h>

#include <revolution/enc.h>

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef unk_t ENCBreakType;
enum ENCBreakType_et
{
	ENC_BREAK_TYPE_NONE,
	ENC_BREAK_TYPE_WINDOWS,
	ENC_BREAK_TYPE_CLASSIC_MAC_OS,
	ENC_BREAK_TYPE_UNIX,
	// Hey do you think anyone uses \n\r? Or maybe ^_? What about \u0085
};

/*******************************************************************************
 * functions
 */

void ENCiRegisterVersion(void);
ENCResult ENCiCheckParameters(BOOL dstValid, unk_t signed *dstSizeIn,
                              unk_t signed *dstSizeOut, BOOL *dstValidOut,
                              BOOL srcValid, unk_t signed *srcSizeIn,
                              unk_t signed *srcSizeOut, BOOL *srcLimitedOut);
int ENCiCheckBreakType(unsigned a, unsigned b);
int ENCiWriteBreakType(void *stream, unk_t  length, ENCBreakType type,
                       BOOL valid);

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_ENC_UTILITY_H
