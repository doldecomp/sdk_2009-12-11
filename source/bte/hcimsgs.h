#ifndef BTE_HCI_MSGS_H
#define BTE_HCI_MSGS_H

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/include/hcimsgs.h
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

#include "bt_target.h"
#include "bt_types.h"

#include "gki.h"

/*******************************************************************************
 * macros
 */

#define HCIC_PARAM_SIZE_INQUIRY					5
#define HCIC_PARAM_SIZE_INQ_CANCEL				0
#define HCIC_PARAM_SIZE_PER_INQ_MODE			9
#define HCIC_PARAM_SIZE_EXIT_PER_INQ			0
#define HCIC_PARAM_SIZE_CREATE_CONN				13
#define HCIC_PARAM_SIZE_DISCONNECT				3
#define HCIC_PARAM_SIZE_ADD_SCO_CONN			4
#define HCIC_PARAM_SIZE_CREATE_CONN_CANCEL		6
#define HCIC_PARAM_SIZE_ACCEPT_CONN				7
#define HCIC_PARAM_SIZE_REJECT_CONN				7
#define HCIC_PARAM_SIZE_LINK_KEY_REQ_REPLY		22
#define HCIC_PARAM_SIZE_LINK_KEY_NEG_REPLY		6
#define HCIC_PARAM_SIZE_PIN_CODE_REQ_REPLY		23
#define HCIC_PARAM_SIZE_PIN_CODE_NEG_REPLY		6
#define HCIC_PARAM_SIZE_CHANGE_CONN_TYPE		4
#define HCIC_PARAM_SIZE_CMD_HANDLE				2
#define HCIC_PARAM_SIZE_SET_CONN_ENCRYPT		3
#define HCIC_PARAM_SIZE_MASTER_LINK_KEY			1
#define HCIC_PARAM_SIZE_RMT_NAME_REQ			10
#define HCIC_PARAM_SIZE_RMT_NAME_REQ_CANCEL		6
#define HCIC_PARAM_SIZE_RMT_EXT_FEATURES		3
#define HCIC_PARAM_SIZE_LOCAL_EXT_FEATURES		1
#define HCIC_PARAM_SIZE_SETUP_ESCO				17
#define HCIC_PARAM_SIZE_ACCEPT_ESCO				21
#define HCIC_PARAM_SIZE_REJECT_ESCO				7
#define HCIC_PARAM_SIZE_HOLD_MODE				6
#define HCIC_PARAM_SIZE_SNIFF_MODE				10
#define HCIC_PARAM_SIZE_PARK_MODE				6
#define HCIC_PARAM_SIZE_QOS_SETUP				20
#define HCIC_PARAM_SIZE_SWITCH_ROLE				7
#define HCIC_PARAM_SIZE_WRITE_POLICY_SET		4
#define HCIC_PARAM_SIZE_READ_DEF_POLICY_SET		0
#define HCIC_PARAM_SIZE_WRITE_DEF_POLICY_SET	2
#define HCIC_PARAM_SIZE_FLOW_SPEC				21
#define HCIC_PARAM_SIZE_SNIFF_SUB_RATE			8
#define HCIC_PARAM_SIZE_EXT_INQ_RESP			241
#define HCIC_PARAM_SIZE_SET_EVENT_MASK			8
#define HCIC_PARAM_SIZE_RESET					0
#define HCIC_PARAM_SIZE_NEW_UNIT_KEY			0
#define HCIC_PARAM_SIZE_READ_STORED_KEY			7
#define HCIC_PARAM_SIZE_DELETE_STORED_KEY		7
#define HCIC_PARAM_SIZE_CHANGE_NAME				BD_NAME_LEN
#define HCIC_PARAM_SIZE_READ_CMD				0
#define HCIC_PARAM_SIZE_WRITE_PARAM1			1
#define HCIC_PARAM_SIZE_WRITE_PARAM2			2
#define HCIC_PARAM_SIZE_WRITE_PARAM3			3
#define HCIC_PARAM_SIZE_SET_AFH_CHANNELS		10
#define HCIC_PARAM_SIZE_WRITE_PAGESCAN_CFG		4
#define HCIC_PARAM_SIZE_WRITE_INQSCAN_CFG		4
#define HCIC_PARAM_SIZE_WRITE_AUTO_FLUSH_TOUT	4
#define HCIC_PARAM_SIZE_READ_TX_POWER			3
#define HCIC_PARAM_SIZE_SET_HOST_BUF_SIZE		7
#define HCIC_PARAM_SIZE_WRITE_LINK_SUPER_TOUT	4
#define HCIC_PARAM_SIZE_READ_CLOCK				3

#define HCI_MAX_NUM_OF_LINK_KEYS_PER_CMD		11

#define HCI_GET_CMD_BUF(paramlen)	((BT_HDR *)GKI_getpoolbuf(HCI_CMD_POOL_ID))

#define HCI_DATA_EVENT_MASK   3
#define HCI_DATA_EVENT_OFFSET 12
#define HCID_GET_EVENT(u16)   (UINT8)(((u16) >> HCI_DATA_EVENT_OFFSET) & HCI_DATA_EVENT_MASK)

#define HCI_DATA_HANDLE_MASK 0x0FFF
#define HCID_GET_HANDLE(u16) (UINT16)((u16) & HCI_DATA_HANDLE_MASK)

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

#endif // BTE_HCI_MSGS_H
