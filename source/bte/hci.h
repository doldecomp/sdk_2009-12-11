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

typedef BT_HDR HC_BT_HDR;

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef struct
{
	UINT8	at_0x00;
	UINT8	at_0x01;
	UINT8	at_0x02;
	UINT8	at_0x03;
} tHCI_CFG;

typedef unk_t tHCI_INIT(UINT8, UINT8, UINT16);
typedef unk_t tHCI_OPEN(tHCI_CFG *p_cfg);
typedef unk_t tHCI_CLOSE(void);
typedef void tHCI_SEND(HC_BT_HDR *p_msg);
typedef unk_t tHCI_HANDLE_EVENT();

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

#ifdef __cplusplus
	}
#endif

#endif // BTE_HCI_H
