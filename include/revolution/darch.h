#ifndef RVL_SDK_DARCH_H
#define RVL_SDK_DARCH_H

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

/*******************************************************************************
 * macros
 */

#define DARCH_PATH_MAX	0x103

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef struct DARCHFileInfo
{
	char	*pathName;	// size 0x04, offset 0x00	/* name known from asserts */
	void	*fileData;	// size 0x04, offset 0x04
	u32		fileSize;	// size 0x04, offset 0x08
} DARCHFileInfo; // size 0x0c

/*******************************************************************************
 * functions
 */

u32 DARCHGetArcSize(DARCHFileInfo *fileInfos, u32 fileInfoCount);

BOOL DARCHCreate(void *dstStartBuffer, u32 dstSize, DARCHFileInfo *fileInfos,
                 u32 fileInfoCount);

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_DARCH_H
