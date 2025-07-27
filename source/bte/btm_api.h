#ifndef BTE_BTM_API_H
#define BTE_BTM_API_H

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/include/btm_api.h
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
#include "data_types.h"

#include "sdp_api.h"

/*******************************************************************************
 * macros
 */

/* class of device field macros */
#define BTM_COD_FORMAT_TYPE_MASK	0x03
#define BTM_COD_MINOR_CLASS_MASK	0xfc
#define BTM_COD_MAJOR_CLASS_MASK	0x1f
#define BTM_COD_SERVICE_CLASS_LO_B	0x00e0
#define BTM_COD_SERVICE_CLASS_MASK	0xffe0

#define BTM_SET_COD_FORMAT_TYPE(pd, fm)		\
	do { *(fm) = (pd)[2] & BTM_COD_FORMAT_TYPE_MASK; } while (FALSE)

#define BTM_SET_COD_MINOR_CLASS(pd, mn)		\
	do { *(mn) = (pd)[2] & BTM_COD_MINOR_CLASS_MASK; } while (FALSE)

#define BTM_SET_COD_MAJOR_CLASS(pd, mj)		\
	do { *(mj) = (pd)[1] & BTM_COD_MAJOR_CLASS_MASK; } while (FALSE)

#define BTM_SET_COD_SERVICE_CLASS(pd, sv)				\
	do													\
	{													\
		*(sv) = (pd)[0];								\
		*(sv) <<= 8;									\
		*(sv) += (pd)[1] & BTM_COD_SERVICE_CLASS_LO_B;	\
	} while (FALSE)

/* to set the fields (assumes that format type is always 0) */
#define MAKE_COD_FROM_FIELDS(pd, mn, mj, sv)					\
	do															\
	{															\
		(pd)[2] = (mn);											\
		(pd)[1] = (mj) + ((sv) & BTM_COD_SERVICE_CLASS_LO_B);	\
		(pd)[0] = (sv) >> 8;									\
	} while (FALSE)

#define BTM_CONNECTABLE	1

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef UINT8 tBTM_STATUS;
enum
{
	BTM_SUCCESS,
	BTM_CMD_STARTED,
	BTM_BUSY,
	BTM_NO_RESOURCES,
	BTM_MODE_UNSUPPORTED,
	BTM_ILLEGAL_VALUE,
	BTM_WRONG_MODE,
	BTM_UNKNOWN_ADDR,
	BTM_DEVICE_TIMEOUT,
	BTM_BAD_VALUE_RET,
	BTM_ERR_PROCESSING,
	BTM_NOT_AUTHORIZED,
	BTM_DEV_RESET,
	BTM_CMD_STORED,
	BTM_ILLEGAL_ACTION,
	BTM_DELAY_CHECK,
	BTM_SCO_BAD_LENGTH,
	BTM_SUCCESS_NO_SECURITY,
	BTM_FAILED_ON_SECURITY,
	BTM_REPEATED_ATTEMPTS,
};

// NOTE: does not seem to be BTM_MAX_REM_BD_NAME_LEN + 1
typedef UINT8 tBTM_BD_NAME[64 + 1];

typedef void tBTM_CMPL_CB(void *p1);
typedef UINT8 tBTM_AUTHORIZE_CALLBACK(BD_ADDR bd_addr, DEV_CLASS dev_class,
                                      tBTM_BD_NAME bd_name, UINT8 *service_name,
                                      UINT8 service_id, BOOLEAN is_originator);
typedef UINT8 tBTM_PIN_CALLBACK(BD_ADDR bd_addr, DEV_CLASS dev_class,
                                tBTM_BD_NAME bd_name);
typedef UINT8 tBTM_LINK_KEY_REQ_CALLBACK(BD_ADDR bd_addr, LINK_KEY key);
typedef UINT8 tBTM_LINK_KEY_CALLBACK(BD_ADDR bd_addr, DEV_CLASS dev_class,
                                     tBTM_BD_NAME bd_name, UINT8 *key,
                                     UINT8 key_type);
typedef UINT8 tBTM_AUTH_COMPLETE_CALLBACK(BD_ADDR bd_addr, DEV_CLASS dev_class,
                                          tBTM_BD_NAME bd_name, int result);
typedef UINT8 tBTM_ABORT_CALLBACK(BD_ADDR bd_addr, DEV_CLASS dev_class,
                                  tBTM_BD_NAME bd_name);

typedef void tBTM_SEC_CBACK(BD_ADDR bd_addr, void *p_ref_data,
                            tBTM_STATUS result);

typedef struct
{
	DEV_CLASS	dev_class;		// size 0x03, offset 0x00
	DEV_CLASS	dev_class_mask;	// size 0x03, offset 0x03
} tBTM_COD_COND; // size 0x06

typedef union
{
	BD_ADDR			bdaddr_cond;	// size 0x06
	tBTM_COD_COND	cod_cond;		// size 0x06
} tBTM_INQ_FILT_COND; // size 0x06

typedef struct
{
	UINT8				mode;				// size 0x01, offset 0x00
	UINT8				duration;			// size 0x01, offset 0x01
	UINT8				max_resps;			// size 0x01, offset 0x02
	UINT8				filter_cond_type;	// size 0x01, offset 0x03
	tBTM_INQ_FILT_COND	filter_cond;		// size 0x03, offset 0x04
} tBTM_INQ_PARMS; // size 0x0a

typedef struct
{
	tBTM_STATUS	status;		// size 0x01, offset 0x00
	UINT8		num_resp;	// size 0x01, offset 0x01
} tBTM_INQUIRY_CMPL; // size 0x02

typedef struct
{
	UINT16		clock_offset;		// size 0x02, offset 0x00
	BD_ADDR		remote_bd_addr;		// size 0x06, offset 0x02
	DEV_CLASS	dev_class;			// size 0x03, offset 0x08
	UINT8		page_scan_rep_mode;	// size 0x01, offset 0x0b
	UINT8		page_scan_per_mode;	// size 0x01, offset 0x0c
	UINT8		page_scan_mode;		// size 0x01, offset 0x0d
	INT8		rssi;				// size 0x01, offset 0x0e
} tBTM_INQ_RESULTS; // size 0x10

typedef void tBTM_INQ_RESULTS_CB(tBTM_INQ_RESULTS *p_inq_results, UINT8 *p_eir);

typedef struct
{
	tBTM_INQ_RESULTS	results;				// size 0x10, offset 0x00
	BOOLEAN				appl_knows_rem_name;	// size 0x01, offset 0x10
	/* 1 byte padding */
} tBTM_INQ_INFO; // size 0x12

typedef struct
{
	tBTM_AUTHORIZE_CALLBACK		*p_authorize_callback;		// size 0x04, offset 0x00
	tBTM_PIN_CALLBACK			*p_pin_callback;			// size 0x04, offset 0x04
	tBTM_LINK_KEY_CALLBACK		*p_link_key_callback;		// size 0x04, offset 0x08
	tBTM_LINK_KEY_REQ_CALLBACK	*p_link_key_req_callback;	// size 0x04, offset 0x0c
	tBTM_AUTH_COMPLETE_CALLBACK	*p_auth_complete_callback;	// size 0x04, offset 0x10
	tBTM_ABORT_CALLBACK			*p_abort_callback;			// size 0x04, offset 0x14
} tBTM_APPL_INFO; // size 0x18

typedef struct
{
	UINT16	status;			// size 0x02, offset 0x00
	UINT16	length;			// size 0x02, offset 0x02
	BD_NAME	remote_bd_name;	// size 0xf8, offset 0x04
} tBTM_REMOTE_DEV_NAME; // size 0xfc

/*******************************************************************************
 * external globals
 */

/*******************************************************************************
 * functions
 */

UINT8 *BTM_ReadDeviceClass(void);
tBTM_STATUS BTM_SetDeviceClass(DEV_CLASS_PTR dev_class);
tBTM_STATUS BTM_SetDiscoverability(UINT16 inq_mode, UINT16 window,
                                   UINT16 interval);
UINT16 BTM_ReadDiscoverability(UINT16 *p_window, UINT16 *p_interval);
tBTM_STATUS BTM_SetConnectability(UINT16 page_mode, UINT16 window,
                                  UINT16 interval);
tBTM_STATUS BTM_StartInquiry(tBTM_INQ_PARMS *p_inqparms,
                             tBTM_INQ_RESULTS_CB *p_results_cb,
                             tBTM_CMPL_CB *p_cmpl_cb);
UINT16 BTM_ReadConnectability(UINT16 *p_window, UINT16 *p_interval);
void BTM_SetSecurityMode(UINT8 sec_mode);
tBTM_STATUS BTM_SecBond(BD_ADDR bd_addr, UINT8 pin_len, UINT8 *p_pin,
                        UINT32 *trusted_mask);
BOOLEAN BTM_SecRegister(tBTM_APPL_INFO *p_cb_info);
void BTM_PINCodeReply(BD_ADDR bd_addr, UINT8 res, UINT8 pin_len, UINT8 *p_pin,
                      UINT32 *trusted_mask);
void BTM_DeviceAuthorized(BD_ADDR bd_addr, UINT8 res, UINT32 *trusted_mask);
void BTM_SetPairableMode(BOOLEAN allow_pairing, BOOLEAN connect_only_paired);
tBTM_STATUS BTM_SetPeriodicInquiryMode(tBTM_INQ_PARMS *p_inqparms,
                                       UINT16 max_delay, UINT16 min_delay,
                                       tBTM_INQ_RESULTS_CB *p_results_cb);
tBTM_STATUS BTM_CancelInquiry(void);
tBTM_STATUS BTM_CancelPeriodicInquiry(void);
tBTM_INQ_INFO *BTM_InqFirstResult(void);
tBTM_INQ_INFO *BTM_InqNextResult(tBTM_INQ_INFO *p_cur);
tBTM_STATUS BTM_ReadRemoteDeviceName(BD_ADDR remote_bda, tBTM_CMPL_CB *p_cb);
void BTM_SetDiscoveryParams(UINT16 num_uuid, tSDP_UUID *p_uuid_list,
                            UINT16 num_attr, UINT16 *p_attr_list);
BOOLEAN BTM_SetSecurityLevel(BOOLEAN is_originator, char *p_name,
                             UINT8 service_id, UINT8 sec_level, UINT16 psm,
                             UINT32 mx_proto_id, UINT32 mx_chan_id);
tBTM_INQ_INFO *BTM_InqDbNext(tBTM_INQ_INFO *p_cur);
tBTM_INQ_INFO *BTM_InqDbFirst(void);

#ifdef __cplusplus
	}
#endif

#endif // BTE_BTM_API_H
