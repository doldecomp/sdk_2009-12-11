#ifndef BTE_L2CAP_API_H
#define BTE_L2CAP_API_H

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/include/l2c_api.h
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

/*******************************************************************************
 * macros
 */

#define L2CAP_MIN_OFFSET		9 // NOTE: not 13

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef struct
{
	UINT8	mode;			// size 0x01, offset 0x00
	UINT8	tx_win_sz;		// size 0x01, offset 0x01
	UINT8	max_transmit;	// size 0x01, offset 0x02
	/* 1 byte padding */
	UINT16	rtrans_tout;	// size 0x02, offset 0x04
	UINT16	mon_tout;		// size 0x02, offset 0x06
	UINT16	mps;			// size 0x02, offset 0x08
	UINT8	tx_pool_id;		// size 0x01, offset 0x0a
	/* 1 byte padding */
	UINT16	tx_buf_sz;		// size 0x02, offset 0x0c
	UINT8	rx_pool_id;		// size 0x01, offset 0x0e
	/* 1 byte padding */
} tL2CAP_FCR_OPTS; // size 0x10

typedef struct
{
	UINT16			result;				// size 0x02, offset 0x00
	BOOLEAN			mtu_present;		// size 0x01, offset 0x02
	/* 1 byte padding */
	UINT16			mtu;				// size 0x02, offset 0x04
	BOOLEAN			qos_present;		// size 0x01, offset 0x06
	/* 1 byte padding */
	FLOW_SPEC		qos;				// size 0x18, offset 0x08
	BOOLEAN			flush_to_present;	// size 0x01, offset 0x20
	/* 1 byte padding */
	UINT16			flush_to;			// size 0x02, offset 0x22
	BOOLEAN			fcr_present;		// size 0x01, offset 0x24
	/* 1 byte padding */
	tL2CAP_FCR_OPTS	fcr;				// size 0x10, offset 0x26
	UINT16			cong_param;			// size 0x02, offset 0x36
	UINT16			flags;				// size 0x02, offset 0x38
} tL2CAP_CFG_INFO; // size 0x3c

typedef void tL2CA_CONNECT_IND_CB(BD_ADDR, UINT16, UINT16, UINT8);
typedef void tL2CA_CONNECT_CFM_CB(UINT16, UINT16);
typedef void tL2CA_CONNECT_PND_CB(UINT16);
typedef void tL2CA_CONFIG_IND_CB(UINT16, tL2CAP_CFG_INFO *);
typedef void tL2CA_CONFIG_CFM_CB(UINT16, tL2CAP_CFG_INFO *);
typedef void tL2CA_DISCONNECT_IND_CB(UINT16, BOOLEAN);
typedef void tL2CA_DISCONNECT_CFM_CB(UINT16, UINT16);
typedef void tL2CA_QOS_VIOLATION_IND_CB(BD_ADDR);
typedef void tL2CA_DATA_IND_CB(UINT16, BT_HDR *);
typedef void tL2CA_CONGESTION_STATUS_CB(UINT16, BOOLEAN);

typedef struct
{
	tL2CA_CONNECT_IND_CB		*pL2CA_ConnectInd_Cb;		// size 0x04, offset 0x00
	tL2CA_CONNECT_CFM_CB		*pL2CA_ConnectCfm_Cb;		// size 0x04, offset 0x04
	tL2CA_CONNECT_PND_CB		*pL2CA_ConnectPnd_Cb;		// size 0x04, offset 0x08
	tL2CA_CONFIG_IND_CB			*pL2CA_ConfigInd_Cb;		// size 0x04, offset 0x0c
	tL2CA_CONFIG_CFM_CB			*pL2CA_ConfigCfm_Cb;		// size 0x04, offset 0x10
	tL2CA_DISCONNECT_IND_CB		*pL2CA_DisconnectInd_Cb;	// size 0x04, offset 0x14
	tL2CA_DISCONNECT_CFM_CB		*pL2CA_DisconnectCfm_Cb;	// size 0x04, offset 0x18
	tL2CA_QOS_VIOLATION_IND_CB	*pL2CA_QoSViolationInd_Cb;	// size 0x04, offset 0x1c
	tL2CA_DATA_IND_CB			*pL2CA_DataInd_Cb;			// size 0x04, offset 0x20
	tL2CA_CONGESTION_STATUS_CB	*pL2CA_CongestionStatus_Cb;	// size 0x04, offset 0x24
} tL2CAP_APPL_INFO; // size 0x28

/*******************************************************************************
 * external globals
 */

/*******************************************************************************
 * functions
 */

UINT8 L2CA_Register(UINT16 psm, tL2CAP_APPL_INFO *p_cb_info);
UINT16 L2CA_ConnectReq(UINT16 psm, BD_ADDR p_bd_addr);
BOOLEAN L2CA_DisconnectReq(UINT16 cid);
UINT8 L2CA_DataWrite(UINT16 cid, BT_HDR *p_data);
BOOLEAN L2CA_ConfigReq(UINT16 cid, tL2CAP_CFG_INFO *p_cfg);
BOOLEAN L2CA_SetIdleTimeout(UINT16 cid, UINT16 timeout, BOOLEAN is_global);
BOOLEAN L2CA_DisconnectReq(UINT16 cid);
BOOLEAN L2CA_ConfigReq(UINT16 cid, tL2CAP_CFG_INFO *p_cfg);
BOOLEAN L2CA_ConnectRsp(BD_ADDR p_bd_addr, UINT8 id, UINT16 lcid, UINT16 result,
                        UINT16 status);
BOOLEAN L2CA_ConfigRsp(UINT16 cid, tL2CAP_CFG_INFO *p_cfg);
BOOLEAN L2CA_DisconnectRsp(UINT16 cid);
void L2CA_Deregister(UINT16 psm);

#ifdef __cplusplus
	}
#endif

#endif // BTE_L2CAP_API_H
