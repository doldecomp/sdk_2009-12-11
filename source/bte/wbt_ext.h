#ifndef BTE_WBT_EXT_H
#define BTE_WBT_EXT_H

/*******************************************************************************
 * headers
 */

#include "data_types.h"

/*******************************************************************************
 * functions
 */

#ifdef __cplusplus
	extern "C" {
#endif

BOOLEAN WBT_ExtCreateRecord(void);
BOOLEAN WBT_ExtAddPinCode(void);
UINT32 wbt_sdp_show_ext(int, char *service_name);

#ifdef __cplusplus
	}
#endif

#endif // BTE_WBT_EXT_H
