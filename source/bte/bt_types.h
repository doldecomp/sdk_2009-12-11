#ifndef BTE_BT_TYPES_H
#define BTE_BT_TYPES_H

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
	UINT16	event;			// size 0x02, offset 0x00
	UINT16	len;			// size 0x02, offset 0x02
	UINT16	offset;			// size 0x02, offset 0x04
	UINT16	layer_specific;	// size 0x02, offset 0x06
} BT_HDR; // size 0x08

// only for bc; do not use
#define BT_HDR_SIZE		(sizeof(BT_HDR))

#define BD_ADDR_LEN		6
typedef UINT8 BD_ADDR[BD_ADDR_LEN];
typedef UINT8 *BD_ADDR_PTR;

#define DEV_CLASS_LEN	3
typedef UINT8 DEV_CLASS[DEV_CLASS_LEN];
typedef UINT8 *DEV_CLASS_PTR;

#ifdef __cplusplus
	}
#endif

#endif // BTE_BT_TYPES_H
