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

#include "bt_types.h"
#include "data_types.h"

#include "gki_target.h"

/*******************************************************************************
 * macros
 */

#ifndef BTM_MAX_REM_BD_NAME_LEN
# define BTM_MAX_REM_BD_NAME_LEN	248 /* NOTE: confirmed 248 */
#endif

#ifndef HCI_CMD_POOL_ID
# define HCI_CMD_POOL_ID			GKI_POOL_ID_2
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

void bte_hcisu_send(BT_HDR *p_msg, UINT16 event);
#ifndef HCI_ACL_DATA_TO_LOWER
#define HCI_ACL_DATA_TO_LOWER(p)	\
	bte_hcisu_send((BT_HDR *)(p), BT_EVT_TO_LM_HCI_ACL);
#endif

#ifndef HID_MAX_SVC_NAME_LEN
# define HID_MAX_SVC_NAME_LEN		32
#endif

#ifndef HID_MAX_SVC_DESCR_LEN
# define HID_MAX_SVC_DESCR_LEN		32
#endif

#ifndef HID_MAX_PROV_NAME_LEN
# define HID_MAX_PROV_NAME_LEN		32
#endif

#define HID_HOST_MAX_DEVICES		16

#ifndef HID_HOST_MAX_DEVICES
# define HID_HOST_MAX_DEVICES		7
#endif

#define SDP_MAX_ATTR_FILTERS		12

#ifndef SDP_MAX_ATTR_FILTERS
# define SDP_MAX_ATTR_FILTERS		15
#endif

#ifndef SDP_MAX_UUID_FILTERS
# define SDP_MAX_UUID_FILTERS		3
#endif

#define HID_HOST_MAX_CONN_RETRY		0

#ifndef HID_HOST_MAX_CONN_RETRY
# define HID_HOST_MAX_CONN_RETRY	3
#endif

#ifndef HID_HOST_MTU
# define HID_HOST_MTU				640
#endif

#ifndef HID_HOST_FLUSH_TO
# define HID_HOST_FLUSH_TO			0xffff
#endif

#ifndef HID_CONTROL_POOL_ID
# define HID_CONTROL_POOL_ID		2
#endif

#ifndef HID_INTERRUPT_POOL_ID
# define HID_INTERRUPT_POOL_ID		2
#endif

#ifndef HID_HOST_REPAGE_WIN
# define HID_HOST_REPAGE_WIN		2
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
