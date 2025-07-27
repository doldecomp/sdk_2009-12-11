#ifndef BTE_BTA_SYS_H
#define BTE_BTA_SYS_H

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * bta/sys/bta_sys.h
 */

/******************************************************************************
 *
 *  Copyright (C) 2003-2012 Broadcom Corporation
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

/*******************************************************************************
 * macros
 */

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef UINT8 tBTA_SYS_CONN_STATUS;
enum
{
	BTA_SYS_CONN_OPEN,
	BTA_SYS_CONN_CLOSE,
	BTA_SYS_APP_OPEN,
	BTA_SYS_APP_CLOSE,
	BTA_SYS_SCO_OPEN,
	BTA_SYS_SCO_CLOSE,
	BTA_SYS_CONN_IDLE,
	BTA_SYS_CONN_BUSY,
};

typedef BOOLEAN tBTA_SYS_EVT_HDLR(BT_HDR *p_msg);
typedef void tBTA_SYS_DISABLE(void);
typedef void tBTA_SYS_CONN_CBACK(tBTA_SYS_CONN_STATUS status, UINT8 id,
                                 UINT8 app_id, BD_ADDR peer_addr);

typedef struct
{
	tBTA_SYS_EVT_HDLR	*evt_hdlr;	// size 0x04, offset 0x00
	tBTA_SYS_DISABLE	*disable;	// size 0x04, offset 0x04
} tBTA_SYS_REG; // size 0x08

/*******************************************************************************
 * functions
 */

void bta_sys_rm_register(tBTA_SYS_CONN_CBACK *p_cback);
void bta_sys_compress_register(tBTA_SYS_CONN_CBACK *p_cback);
void bta_sys_pm_register(tBTA_SYS_CONN_CBACK *p_cback);
void bta_sys_conn_open(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
void bta_sys_conn_close(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
void bta_sys_app_open(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
void bta_sys_app_close(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
void bta_sys_sco_open(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
void bta_sys_sco_close(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
void bta_sys_idle(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
void bta_sys_busy(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
void bta_sys_sendmsg(void *p_msg);

#ifdef __cplusplus
	}
#endif

#endif // BTE_BTA_SYS_H
