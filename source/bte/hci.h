#ifndef BTE_HCI_H
#define BTE_HCI_H

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * hci/include/hci.h
 */

/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
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

#include <decomp.h>

#include "bt_types.h"
#include "data_types.h"

/*******************************************************************************
 * macros
 */

#define MSG_HC_TO_STACK_HCI_EVT        0x1000 /* eq. BT_EVT_TO_BTU_HCI_EVT */
#define MSG_HC_TO_STACK_HCI_ACL        0x1100 /* eq. BT_EVT_TO_BTU_HCI_ACL */
#define MSG_HC_TO_STACK_HCI_SCO        0x1200 /* eq. BT_EVT_TO_BTU_HCI_SCO */
#define MSG_HC_TO_STACK_HCI_ERR        0x1300 /* eq. BT_EVT_TO_BTU_HCIT_ERR */
#define MSG_HC_TO_STACK_L2C_SEG_XMIT   0x1900 /* eq. BT_EVT_TO_BTU_L2C_SEG_XMIT */

#define MSG_STACK_TO_HC_HCI_CMD        0x2000 /* eq. BT_EVT_TO_LM_HCI_CMD */
#define MSG_STACK_TO_HC_HCI_ACL        0x2100 /* eq. BT_EVT_TO_LM_HCI_ACL */
#define MSG_STACK_TO_HC_HCI_SCO        0x2200 /* eq. BT_EVT_TO_LM_HCI_SCO */

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef BT_HDR HC_BT_HDR;

typedef struct
{
	UINT16	at_0x00;	// size 0x02, offset 0x00
	UINT16	at_0x02;	// size 0x02, offset 0x02
} tHCI_CFG; // size 0x04

typedef void tHCI_INIT(UINT8, unk1_t, UINT16);
typedef BOOLEAN tHCI_OPEN(tHCI_CFG *p_cfg);
typedef void tHCI_CLOSE(void);
typedef BOOLEAN tHCI_SEND(HC_BT_HDR *p_msg);
typedef unk_t tHCI_HANDLE_EVENT(UINT16 len);

typedef struct
{
	tHCI_INIT			*init;
	tHCI_OPEN			*open;
	tHCI_CLOSE			*close;
	tHCI_SEND			*send;
	tHCI_HANDLE_EVENT	*handle_event;
} tHCI_IF;

/*******************************************************************************
 * external globals
 */

extern tHCI_IF const hcisu_h2;

/*******************************************************************************
 * functions
 */

void hcisu_h2_init(UINT8, unk1_t, UINT16);
BOOLEAN hcisu_h2_open(tHCI_CFG *p_cfg);
void hcisu_h2_close(void);
BOOLEAN hcisu_h2_send(HC_BT_HDR *p_msg);
unk_t hcisu_h2_handle_event(UINT16 len);

#ifdef __cplusplus
	}
#endif

#endif // BTE_HCI_H
