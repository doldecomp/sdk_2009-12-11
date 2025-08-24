#ifndef BTE_BTA_PBC_API_H
#define BTE_BTA_PBC_API_H

/*******************************************************************************
 * headers
 */

#include "data_types.h"

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef struct
{
	UINT8	at_0x00[4];	// size 0x04, offset 0x00
	void	*at_0x04;	// size 0x04, offset 0x04
	UINT8	at_0x08[4]; // size 0x04, offset 0x08
} tBTA_PBC_CFG; // size 0x0c

#ifdef __cplusplus
	}
#endif

#endif // BTE_BTA_PBC_API_H
