#ifndef BTE_HID_DEFS_H
#define BTE_HID_DEFS_H

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/include/hiddefs.h
 */

/******************************************************************************
 *
 *  Copyright (C) 2002-2012 Broadcom Corporation
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
#include "data_types.h"

#include "sdp_api.h"

/*******************************************************************************
 * macros
 */

#define HID_GET_TRANS_FROM_HDR(x)	(((x) >> 4) & 0x0f)
#define HID_GET_PARAM_FROM_HDR(x)	( (x)       & 0x0f)
#define HID_BUILD_HDR(t, p)			(UINT8)(((t) << 4) | ((p) & 0x0f))

#define HID_PAR_PROTOCOL_BOOT_MODE	0x00
#define HID_PAR_PROTOCOL_REPORT		0x01
#define HID_PAR_REP_TYPE_MASK		0x03

#define HID_TRANS_HANDSHAKE			0
#define HID_TRANS_CONTROL			1
#define HID_TRANS_GET_REPORT		4
#define HID_TRANS_SET_REPORT		5
#define HID_TRANS_GET_PROTOCOL		6
#define HID_TRANS_SET_PROTOCOL		7
#define HID_TRANS_GET_IDLE			8
#define HID_TRANS_SET_IDLE			9
#define HID_TRANS_DATA				10
#define HID_TRANS_DATAC				11

#define HID_PAR_CONTROL_VIRTUAL_CABLE_UNPLUG        (5)

#define HID_L2CAP_CONN_FAIL	0x0100
#define HID_L2CAP_REQ_FAIL	0x0200
#define HID_L2CAP_CFG_FAIL	0x0400

#define HID_PAR_HANDSHAKE_RSP_SUCCESS               (0)
#define HID_PAR_HANDSHAKE_RSP_NOT_READY             (1)
#define HID_PAR_HANDSHAKE_RSP_ERR_INVALID_REP_ID    (2)
#define HID_PAR_HANDSHAKE_RSP_ERR_UNSUPPORTED_REQ   (3)
#define HID_PAR_HANDSHAKE_RSP_ERR_INVALID_PARAM     (4)
#define HID_PAR_HANDSHAKE_RSP_ERR_UNKNOWN           (14)
#define HID_PAR_HANDSHAKE_RSP_ERR_FATAL             (15)


/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef UINT8 tHID_STATUS;
enum
{
	HID_SUCCESS,
	HID_ERR_NOT_REGISTERED,
	HID_ERR_ALREADY_REGISTERED,
	HID_ERR_NO_RESOURCES,
	HID_ERR_NO_CONNECTION,
	HID_ERR_INVALID_PARAM,
	HID_ERR_UNSUPPORTED,
	HID_ERR_UNKNOWN_COMMAND,
	HID_ERR_CONGESTED,
	HID_ERR_CONN_IN_PROCESS,
	HID_ERR_ALREADY_CONN,
	HID_ERR_DISCONNECTING,
	HID_ERR_SET_CONNABLE_FAIL,

	/* Device specific error codes */
	HID_ERR_HOST_UNKNOWN,
	HID_ERR_L2CAP_FAILED,
	HID_ERR_AUTH_FAILED,
	HID_ERR_SDP_BUSY,

	HID_ERR_INVALID = 0xFF
};

typedef struct desc_info
{
	UINT16	dl_len;		// size 0x02, offset 0x00
	/* 2 bytes padding */
	UINT8	*dsc_list;	// size 0x04, offset 0x00
} tHID_DEV_DSCP_INFO; // size 0x08

typedef struct sdp_info
{
	char				svc_name[HID_MAX_SVC_NAME_LEN];		// size 0x20, offset 0x00
	char				svc_descr[HID_MAX_SVC_DESCR_LEN];	// size 0x20, offset 0x20
	char				prov_name[HID_MAX_PROV_NAME_LEN];	// size 0x20, offset 0x40
	UINT16				rel_num;							// size 0x02, offset 0x60
	UINT16				hpars_ver;							// size 0x02, offset 0x62
	UINT8				sub_class;							// size 0x01, offset 0x64
	UINT8				ctry_code;							// size 0x01, offset 0x65
	UINT16				sup_timeout;						// size 0x02, offset 0x66
	tHID_DEV_DSCP_INFO	dscp_info;							// size 0x08, offset 0x68
	tSDP_DISC_REC		*p_sdp_layer_rec;					// size 0x04, offset 0x70
} tHID_DEV_SDP_INFO; // size 0x74

/*******************************************************************************
 * external globals
 */

/*******************************************************************************
 * functions
 */

#ifdef __cplusplus
	}
#endif

#endif // BTE_HID_DEFS_H
