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

/*******************************************************************************
 * functions
 */

void ENCiRegisterVersion(void);
ENCResult ENCiCheckParameters(BOOL dstValid, unk_t signed *dstSizeIn,
                              unk_t signed *dstSizeOut, BOOL *dstValidOut,
                              BOOL srcValid, unk_t signed *srcSizeIn,
                              unk_t signed *srcSizeOut, BOOL *srcLimitedOut);
int ENCiCheckBreakType(unsigned a, unsigned b);
int ENCiWriteBreakType(void *stream, unk_t width, ENCBreakType type,
                       BOOL valid);

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_ENC_UTILITY_H
