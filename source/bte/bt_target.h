#ifndef BTE_BT_TARGET_H
#define BTE_BT_TARGET_H

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * include/bt_target.h
 */

/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/* Includes changes by muff1n1634 */

/*******************************************************************************
 * headers
 */

#include "gki_target.h"

/*******************************************************************************
 * macros
 */

#ifndef BTM_MAX_REM_BD_NAME_LEN
# define BTM_MAX_REM_BD_NAME_LEN	248 /* NOTE: confirmed 248 */
#endif

#ifndef GAP_DATA_POOL_ID
# define GAP_DATA_POOL_ID			GKI_POOL_ID_3
#endif

#ifndef L2CAP_MTU_SIZE
# define L2CAP_MTU_SIZE				1691
#endif

#ifndef SDP_MAX_PROTOCOL_PARAMS
# define SDP_MAX_PROTOCOL_PARAMS	2
#endif

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

/*******************************************************************************
 * external globals
 */

/*******************************************************************************
 * functions
 */

#ifdef __cplusplus
	}
#endif

#endif // BTE_BT_TARGET_H
