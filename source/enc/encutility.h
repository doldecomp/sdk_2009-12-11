#ifndef RVL_SDK_ENC_UTILITY_H
#define RVL_SDK_ENC_UTILITY_H

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

#include <revolution/enc.h>

/*******************************************************************************
 * functions
 */

#ifdef __cplusplus
	extern "C" {
#endif

void ENCiRegisterVersion(void);
ENCResult ENCiCheckParameters(BOOL dstValid, s32 *dstSizeIn, s32 *dstSizeOut,
                              BOOL *dstValidOut, BOOL srcValid, s32 *srcSizeIn,
                              s32 *srcSizeOut, BOOL *srcLimitedOut);
int ENCiCheckBreakType(unsigned a, unsigned b);
int ENCiWriteBreakType(void *stream, int width, ENCBreakType breakType,
                       BOOL valid);

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_ENC_UTILITY_H
