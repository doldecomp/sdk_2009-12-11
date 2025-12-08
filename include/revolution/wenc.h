#ifndef RVL_SDK_WENC_H
#define RVL_SDK_WENC_H

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef s16 WENCPCMSample;
typedef u8 WENCADPCMSample;

// [SPQE7T]/ISpyD.elf:.debug_info::0x29f653
// [R89JEL]/bin/RVL/Debug/mainD.elf:.debug::0x415e77
typedef struct /* explicitly untagged */
{
	byte_t	data[32];
} WENCInfo; // size 0x20

/*******************************************************************************
 * functions
 */

s32 WENCGetEncodeData(WENCInfo *info, u32 flag, WENCPCMSample const *pbyPcmData,
                      s32 nSampleNum, WENCADPCMSample *pbyAdpcmData);

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_WENC_H
