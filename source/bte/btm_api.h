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

#include "bt_types.h"
#include "data_types.h"

#include "gki.h"
#include "hcidefs.h"
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

#define BTM_SEC_COPY_TRUSTED_DEVICE(p_src, p_dst)					\
	do																\
	{																\
		register int trst;											\
																	\
		for (trst = 0; trst < BTM_SEC_SERVICE_ARRAY_SIZE; ++trst)	\
			((UINT32 *)(p_dst))[trst] = ((UINT32 *)(p_src))[trst];	\
	} while (FALSE)

#define BTM_COD_SERVICE_LMTD_DISCOVER       0x0020

#define BTM_NON_DISCOVERABLE        0
#define BTM_LIMITED_DISCOVERABLE    1
#define BTM_GENERAL_DISCOVERABLE    2
#define BTM_MAX_DISCOVERABLE        BTM_GENERAL_DISCOVERABLE
#define BTM_DISCOVERABLE_MASK       (BTM_LIMITED_DISCOVERABLE|BTM_GENERAL_DISCOVERABLE)

#define BTM_NON_CONNECTABLE         0
#define BTM_CONNECTABLE	1
#define BTM_CONNECTABLE_MASK        (BTM_NON_CONNECTABLE | BTM_CONNECTABLE)

#define BTM_SEC_SERVICE_HID_SEC_CTRL	32
#define BTM_SEC_SERVICE_HID_NOSEC_CTRL	33
#define BTM_SEC_SERVICE_HID_INTR		34

#define BTM_SEC_PROTO_HID	6

#define BTM_SEC_NONE	0
#define BTM_SEC_OUT_AUTHORIZE      0x0008 /* Outbound call requires authorization */
#define BTM_SEC_OUT_ENCRYPT        0x0020 /* Outbound call requires encryption */
#define BTM_SEC_OUT_AUTHENTICATE   0x0010 /* Outbound call requires authentication */
#define BTM_SEC_OUT_MITM           0x2000 /* outbound Do man in the middle protection */
#define BTM_SEC_FORCE_MASTER       0x0100 /* Need to switch connection to be master */
#define BTM_SEC_ATTEMPT_MASTER     0x0200 /* Try to switch connection to be master */
#define BTM_SEC_FORCE_SLAVE        0x0400 /* Need to switch connection to be master */
#define BTM_SEC_ATTEMPT_SLAVE      0x0800 /* Try to switch connection to be slave */
#define BTM_SEC_IN_AUTHORIZE       0x0001 /* Inbound call requires authorization */
#define BTM_SEC_IN_ENCRYPT         0x0004 /* Inbound call requires encryption */
#define BTM_SEC_IN_AUTHENTICATE    0x0002 /* Inbound call requires authentication */
#define BTM_SEC_IN_MITM            0x1000 /* inbound Do man in the middle protection */

#define BTM_SEC_FLAG_AUTHORIZED     0x01

#define BTM_SEC_MAX_SERVICES 46

#ifndef BTM_SEC_MAX_SERVICES
# define BTM_SEC_MAX_SERVICES            65
#endif

#define BTM_SEC_ARRAY_BITS          32
#define BTM_SEC_SERVICE_ARRAY_SIZE  (((UINT32)BTM_SEC_MAX_SERVICES / BTM_SEC_ARRAY_BITS) + \
                                    (((UINT32)BTM_SEC_MAX_SERVICES % BTM_SEC_ARRAY_BITS) ? 1 : 0))

#define BTM_ACL_MODE_NORMAL	HCI_MODE_ACTIVE
#define BTM_ACL_MODE_HOLD	HCI_MODE_HOLD
#define BTM_ACL_MODE_SNIFF	HCI_MODE_SNIFF
#define BTM_ACL_MODE_PARK	HCI_MODE_PARK

#define BTM_SEC_SERVICE_SDP_SERVER      0

#define BTM_PM_REG_SET      1 /* The module wants to set the desired power mode */
#define BTM_PM_REG_NOTIF    2 /* The module wants to receive mode change event */
#define BTM_PM_DEREG        4 /* The module does not want to involve with PM anymore */

#define BTM_GENERAL_INQUIRY         0
#define BTM_LIMITED_INQUIRY         1
#define BTM_BR_INQUIRY_MASK         0x0f

#define BTM_INQ_RESULT_STANDARD     0
#define BTM_INQ_RESULT_WITH_RSSI    1

#define BTM_ROLE_UNDEFINED      0xff     /* undefined value (error status) */

#define BTM_CB_EVT_RETURN_LINK_KEYS         1

#define BTM_SEC_MODE_SP             4

#define BTM_ROLE_UNDEFINED      0xff     /* undefined value (error status) */

#define BTM_SEC_FLAG_ENCRYPTED      0x04

#define BTM_ROLE_MASTER         HCI_ROLE_MASTER

#define BTM_ACL_PKT_TYPES_MASK_DM1      HCI_PKT_TYPES_MASK_DM1
#define BTM_ACL_PKT_TYPES_MASK_DH1      HCI_PKT_TYPES_MASK_DH1
#define BTM_ACL_PKT_TYPES_MASK_DM3      HCI_PKT_TYPES_MASK_DM3
#define BTM_ACL_PKT_TYPES_MASK_DH3      HCI_PKT_TYPES_MASK_DH3
#define BTM_ACL_PKT_TYPES_MASK_DM5      HCI_PKT_TYPES_MASK_DM5
#define BTM_ACL_PKT_TYPES_MASK_DH5      HCI_PKT_TYPES_MASK_DH5
#define BTM_ACL_PKT_TYPES_MASK_NO_2_DH1 HCI_PKT_TYPES_MASK_NO_2_DH1
#define BTM_ACL_PKT_TYPES_MASK_NO_3_DH1 HCI_PKT_TYPES_MASK_NO_3_DH1
#define BTM_ACL_PKT_TYPES_MASK_NO_2_DH3 HCI_PKT_TYPES_MASK_NO_2_DH3
#define BTM_ACL_PKT_TYPES_MASK_NO_3_DH3 HCI_PKT_TYPES_MASK_NO_3_DH3
#define BTM_ACL_PKT_TYPES_MASK_NO_2_DH5 HCI_PKT_TYPES_MASK_NO_2_DH5
#define BTM_ACL_PKT_TYPES_MASK_NO_3_DH5 HCI_PKT_TYPES_MASK_NO_3_DH5

#define BTM_SEC_FLAG_LKEY_KNOWN     0x10

#define BTM_SCO_PKT_TYPES_MASK_HV1  HCI_ESCO_PKT_TYPES_MASK_HV1
#define BTM_SCO_PKT_TYPES_MASK_HV2  HCI_ESCO_PKT_TYPES_MASK_HV2
#define BTM_SCO_PKT_TYPES_MASK_HV3  HCI_ESCO_PKT_TYPES_MASK_HV3
#define BTM_SCO_PKT_TYPES_MASK_EV3  HCI_ESCO_PKT_TYPES_MASK_EV3
#define BTM_SCO_PKT_TYPES_MASK_EV4  HCI_ESCO_PKT_TYPES_MASK_EV4
#define BTM_SCO_PKT_TYPES_MASK_EV5  HCI_ESCO_PKT_TYPES_MASK_EV5

#define BTM_ESCO_LINK_ONLY_MASK (BTM_SCO_PKT_TYPES_MASK_EV3 | \
                                 BTM_SCO_PKT_TYPES_MASK_EV4 | \
                                 BTM_SCO_PKT_TYPES_MASK_EV5)
#define BTM_SCO_PKT_TYPES_MASK_NO_2_EV3  HCI_ESCO_PKT_TYPES_MASK_NO_2_EV3
#define BTM_SCO_PKT_TYPES_MASK_NO_2_EV5  HCI_ESCO_PKT_TYPES_MASK_NO_2_EV5
#define BTM_SCO_PKT_TYPES_MASK_NO_3_EV5  HCI_ESCO_PKT_TYPES_MASK_NO_3_EV5
#define BTM_SCO_PKT_TYPES_MASK_NO_3_EV3  HCI_ESCO_PKT_TYPES_MASK_NO_3_EV3

#define BTM_SCAN_TYPE_STANDARD      0
#define BTM_SCAN_TYPE_INTERLACED    1       /* 1.2 devices only */

#define BTM_CB_EVT_READ_STORED_LINK_KEYS    2
#define BTM_CB_EVT_WRITE_STORED_LINK_KEYS   3
#define BTM_CB_EVT_DELETE_STORED_LINK_KEYS  4

#define BTM_LIMITED_INQUIRY_ACTIVE  0x1     /* a limited inquiry is in progress */
#define BTM_GENERAL_INQUIRY_ACTIVE  0x2     /* a general inquiry is in progress */
#define BTM_PERIODIC_INQUIRY_ACTIVE 0x4     /* a periodic inquiry is active */

#define BTM_CLR_INQUIRY_FILTER          0                   /* Inquiry Filtering is turned off */
#define BTM_INQUIRY_INACTIVE        0x0     /* no inquiry in progress */

#define BTM_FILTER_COND_DEVICE_CLASS    HCI_FILTER_COND_DEVICE_CLASS /* Filter on device class */
#define BTM_FILTER_COND_BD_ADDR         HCI_FILTER_COND_BD_ADDR /* Filter on device addr */

#define BTM_INQ_RES_IGNORE_RSSI     0x7f    /* RSSI value not supplied (ignore it) */

#define BTM_64KBITS_RATE            0x00001f40  /* 64 kbits/sec data rate */

#define BTM_ESCO_RETRANS_POWER      1

#define BTM_INVALID_SCO_DISC_REASON 0xFFFF

#define BTM_SCO_LINK_ONLY_MASK  (BTM_SCO_PKT_TYPES_MASK_HV1 | \
                                 BTM_SCO_PKT_TYPES_MASK_HV2 | \
                                 BTM_SCO_PKT_TYPES_MASK_HV3)

#define BTM_INVALID_SCO_INDEX       0xFFFF
#define BTM_INVALID_HCI_HANDLE      0xFFFF

#define BTM_IGNORE_SCO_PKT_TYPE     0

#define BTM_SEC_MODE_NONE           1
#define BTM_SEC_MODE_SERVICE        2
#define BTM_SEC_MODE_LINK           3
#define BTM_SEC_MODE_SP_DEBUG       5
#define BTM_SEC_MODE_SP             4

#define BTM_SEC_STATE_AUTHORIZING       4
#define BTM_SEC_STATE_IDLE              0
#define BTM_SEC_STATE_GETTING_NAME      3
#define BTM_SEC_STATE_ENCRYPTING        2

#define BTM_SEC_FLAG_AUTHENTICATED  0x02

#define BTM_COD_MAJOR_PERIPHERAL            0x05
#define BTM_COD_MINOR_KEYBOARD              0x40

#define BTM_SEC_IS_SERVICE_TRUSTED(p, service)                               \
	(((((UINT32 *)(p))[(((UINT32)(service)) / BTM_SEC_ARRAY_BITS)])          \
	  & (UINT32)(((UINT32)1 << (((UINT32)(service)) % BTM_SEC_ARRAY_BITS)))) \
	     ? TRUE                                                              \
	     : FALSE)

#define BTM_SEC_TRUST_ALL               0xFFFFFFFF  /* for each array element */

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

typedef UINT8 tBTM_BL_EVENT;
typedef UINT8 tBTA_PREF_ROLES;

enum
{
    BTM_BL_COLLISION_EVT
	 = 4,
};

enum
{
    BTM_PM_STS_ACTIVE = HCI_MODE_ACTIVE,
    BTM_PM_STS_HOLD   = HCI_MODE_HOLD,
    BTM_PM_STS_SNIFF  = HCI_MODE_SNIFF,
    BTM_PM_STS_PARK   = HCI_MODE_PARK,
    BTM_PM_STS_SSR,     /* report the SSR parameters in HCI_SNIFF_SUB_RATE_EVT */
    BTM_PM_STS_PENDING = 4,   /* when waiting for status from controller */
    BTM_PM_STS_ERROR = 5,   /* when HCI command status returns error */
};
typedef UINT8 tBTM_PM_STATUS;

enum
{
    BTM_PM_ST_ACTIVE  = BTM_PM_STS_ACTIVE,
};

typedef UINT8 tBTM_SCO_TYPE;
enum
{
	BTM_LINK_TYPE_SCO	= HCI_LINK_TYPE_SCO,
	BTM_LINK_TYPE_ESCO	= HCI_LINK_TYPE_ESCO,
};

/* BTM Power manager modes */
typedef UINT8 tBTM_PM_MODE;
enum
{
    BTM_PM_MD_ACTIVE = BTM_PM_STS_ACTIVE,
    BTM_PM_MD_HOLD   = BTM_PM_STS_HOLD,
    BTM_PM_MD_SNIFF  = BTM_PM_STS_SNIFF,
    BTM_PM_MD_PARK   = BTM_PM_STS_PARK,
    BTM_PM_MD_FORCE  = 0x10 /* OR this to force ACL link to a certain mode */
};

#define BTM_PM_SET_ONLY_ID  0x80

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
typedef void tBTM_MKEY_CALLBACK(BD_ADDR bd_addr, UINT8 status, UINT8 key_flag);
typedef UINT8 tBTM_FILTER_CB(BD_ADDR bd_addr, DEV_CLASS dc);
typedef void tBTM_RMT_NAME_CALLBACK(BD_ADDR bd_addr, DEV_CLASS dc,
                                    tBTM_BD_NAME bd_name);
typedef void tBTM_ACL_DB_CHANGE_CB(BD_ADDR p_bda, DEV_CLASS p_dc, BD_NAME p_bdn,
                                   BD_FEATURES features, BOOLEAN is_new);
typedef void tBTM_SCO_CB(UINT16 sco_inx);
typedef void tBTM_INQ_DB_CHANGE_CB(void *p1, BOOLEAN is_new);

typedef struct
{
    UINT16  rx_pkt_len;
    UINT16  tx_pkt_len;
    BD_ADDR bd_addr;
    UINT8   link_type;  /* BTM_LINK_TYPE_SCO or BTM_LINK_TYPE_ESCO */
    UINT8   tx_interval;
    UINT8   retrans_window;
    UINT8   air_mode;
} tBTM_ESCO_DATA;

#define BTM_ESCO_CHG_EVT        1
#define BTM_ESCO_CONN_REQ_EVT   2
typedef UINT8 tBTM_ESCO_EVT;

typedef struct
{
    UINT16  sco_inx;
    UINT16  rx_pkt_len;
    UINT16  tx_pkt_len;
    BD_ADDR bd_addr;
    UINT8   hci_status;
    UINT8   tx_interval;
    UINT8   retrans_window;
} tBTM_CHG_ESCO_EVT_DATA;

typedef struct
{
    UINT16        sco_inx;
    BD_ADDR       bd_addr;
    DEV_CLASS     dev_class;
    tBTM_SCO_TYPE link_type;
} tBTM_ESCO_CONN_REQ_EVT_DATA;

typedef union
{
    tBTM_CHG_ESCO_EVT_DATA      chg_evt;
    tBTM_ESCO_CONN_REQ_EVT_DATA conn_evt;
} tBTM_ESCO_EVT_DATA;

typedef void tBTM_ESCO_CBACK(tBTM_ESCO_EVT event, tBTM_ESCO_EVT_DATA *p_data);

typedef struct
{
    UINT32 tx_bw;
    UINT32 rx_bw;
    UINT16 max_latency;
    UINT16 voice_contfmt;  /* Voice Settings or Content Format */
    UINT16 packet_types;
    UINT8  retrans_effort;
} tBTM_ESCO_PARAMS;

typedef struct
{
    UINT16 max_latency;
    UINT16 packet_types;
    UINT8  retrans_effort;
} tBTM_CHG_ESCO_PARAMS;

typedef struct
{
    UINT16  opcode;
    UINT16  param_len;
    UINT8   param_buf[255];
} tBTM_VSC_CMPL;

typedef void tBTM_VSC_CMPL_CB(tBTM_VSC_CMPL *p1);

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

typedef void tBTM_INQ_RESULTS_CB(void *);

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

typedef struct
{
	UINT16			max;		// size 0x02, offset 0x00
	UINT16			min;		// size 0x02, offset 0x02
	UINT16			attempt;	// size 0x02, offset 0x04
	UINT16			timeout;	// size 0x02, offset 0x06
	tBTM_PM_MODE	mode;		// size 0x01, offset 0x08
	/* 1 byte padding */
} tBTM_PM_PWR_MD; // size 0x0a

typedef struct
{
	UINT8	hci_version;	// size 0x01, offset 0x00
	/* 1 byte padding */
	UINT16	hci_revision;	// size 0x02, offset 0x02
	UINT8	lmp_version;	// size 0x01, offset 0x04
	/* 1 byte padding */
	UINT16	manufacturer;	// size 0x02, offset 0x06
	UINT16	lmp_subversion;	// size 0x02, offset 0x08
} tBTM_VERSION_INFO; // size 0x0a

typedef struct
{
	UINT8	hci_status;		// size 0x01, offset 0x00
	UINT8	role;			// size 0x01, offset 0x01
	BD_ADDR	remote_bd_addr;	// size 0x06, offset 0x02
} tBTM_ROLE_SWITCH_CMPL; // size 0x08

typedef UINT8 tBTM_DEV_STATUS;
enum
{
	BTM_DEV_STATUS_UP,
	BTM_DEV_STATUS_DOWN,
	BTM_DEV_STATUS_CMD_TOUT
};

typedef void tBTM_DEV_STATUS_CB(tBTM_DEV_STATUS status);
typedef void tBTM_VS_EVT_CB(UINT8 len, UINT8 *p);

typedef struct
{
	tBTM_DEV_STATUS_CB		*p_dev_status_cb;			// size 0x04, offset 0x00
	tBTM_VS_EVT_CB			*p_vend_spec_cb;			// size 0x04, offset 0x04
	tBTM_CMPL_CB			*p_stored_link_key_cmpl_cb;	// size 0x04, offset 0x08
	TIMER_LIST_ENT			reset_timer;				// size 0x18, offset 0x0c
	tBTM_CMPL_CB			*p_reset_cmpl_cb;			// size 0x04, offset 0x24
	TIMER_LIST_ENT			rln_timer;					// size 0x18, offset 0x28
	tBTM_CMPL_CB			*p_rln_cmpl_cb;				// size 0x04, offset 0x40
	TIMER_LIST_ENT			rlinkp_timer;				// size 0x18, offset 0x44
	tBTM_CMPL_CB			*p_rlinkp_cmpl_cb;			// size 0x04, offset 0x5c
	TIMER_LIST_ENT			rssi_timer;					// size 0x18, offset 0x60
	tBTM_CMPL_CB			*p_rssi_cmpl_cb;			// size 0x04, offset 0x78
	TIMER_LIST_ENT			lnk_quality_timer;			// size 0x18, offset 0x7c
	tBTM_CMPL_CB			*p_lnk_qual_cmpl_cb;		// size 0x04, offset 0x94
	TIMER_LIST_ENT			qossu_timer;				// size 0x18, offset 0x98
	tBTM_CMPL_CB			*p_qossu_cmpl_cb;			// size 0x04, offset 0xb0
	tBTM_CMPL_CB			*p_vsc_cmpl_cb;				// size 0x04, offset 0xb4
	tBTM_CMPL_CB			*p_reset_only_cmpl_cb;		// size 0x04, offset 0xb8
	tBTM_ROLE_SWITCH_CMPL	switch_role_ref_data;		// size 0x08, offset 0xbc
	tBTM_CMPL_CB			*p_switch_role_cb;			// size 0x04, offset 0xc4
	BD_ADDR					local_addr;					// size 0x06, offset 0xc8
	tBTM_VERSION_INFO		local_version;				// size 0x0a, offset 0xce
	BD_FEATURES				local_features;				// size 0x08, offset 0xd8
	DEV_CLASS				dev_class;					// size 0x03, offset 0xe0
	/* 1 byte padding */
	UINT16					page_timeout;				// size 0x02, offset 0xe4
	UINT8					state;						// size 0x01, offset 0xe6
	UINT8					retry_count;				// size 0x01, offset 0xe7
	UINT8					vsc_busy;					// size 0x01, offset 0xe8
	/* 3 bytes padding */
} tBTM_DEVCB; // size 0xec

typedef struct
{
	tBTM_ESCO_CBACK		*p_esco_cback;	// size 0x04, offset 0x00
	tBTM_ESCO_PARAMS	setup;			// size 0x10, offset 0x04
	tBTM_ESCO_DATA		data;			// size 0x0e, offset 0x14
	UINT8				hci_status;		// size 0x01, offset 0x22
	/* 1 byte padding */
} tBTM_ESCO_INFO; // size 0x24

typedef struct
{
    tBTM_STATUS status;
    UINT8       hci_status;
    INT8        rssi;
    BD_ADDR     rem_bda;
} tBTM_RSSI_RESULTS;

typedef struct
{
    tBTM_STATUS status;
    UINT8       hci_status;
    UINT8       link_quality;
    BD_ADDR     rem_bda;
} tBTM_LINK_QUALITY_RESULTS;

typedef void tBTM_PM_STATUS_CBACK(BD_ADDR p_bda, tBTM_PM_STATUS status,
                                  UINT16 value, UINT8 hci_status);

typedef struct
{
    UINT8          event;
    UINT8          num_keys;
} tBTM_RETURN_LINK_KEYS_EVT;

typedef struct
{
    tBTM_STATUS status;
    UINT8       hci_status;
    BD_ADDR     rem_bda;
    UINT16      settings;
} tBTM_LINK_POLICY_RESULTS;

#if defined(__clang__)
typedef tBTM_LINK_POLICY_RESULTS tBTM_LNK_POLICY_RESULTS __attribute__((deprecated("", "tBTM_LINK_POLICY_RESULTS")));
#endif

typedef struct
{
    FLOW_SPEC flow;
    UINT16 handle;
    UINT8 status;
} tBTM_QOS_SETUP_CMPL;

typedef struct
{
    UINT8          event;
    UINT8          status;
    UINT16         max_keys;
    UINT16         read_keys;
} tBTM_READ_STORED_LINK_KEY_COMPLETE;

typedef struct
{
    UINT8          event;
    UINT8          status;
    UINT8          num_keys;
} tBTM_WRITE_STORED_LINK_KEY_COMPLETE;

typedef struct
{
    UINT8          event;
    UINT8          status;
    UINT16         num_keys;
} tBTM_DELETE_STORED_LINK_KEY_COMPLETE;

/*******************************************************************************
 * external globals
 */

/*******************************************************************************
 * functions
 */

tBTM_STATUS BTM_GetRole(BD_ADDR remote_bd_addr, UINT8 *p_role);
tBTM_STATUS BTM_SwitchRole(BD_ADDR remote_bd_addr, UINT8 new_role,
                           tBTM_CMPL_CB *p_cb);
/**/
tBTM_STATUS BTM_SetLinkPolicy(BD_ADDR remote_bda, UINT16 *settings);
void BTM_SetDefaultLinkPolicy(UINT16 settings);
tBTM_STATUS BTM_ReadLinkPolicy(BD_ADDR remote_bda, tBTM_CMPL_CB *p_cb);
/**/
void BTM_SetDefaultLinkSuperTout(UINT16 timeout);
tBTM_STATUS BTM_SetLinkSuperTout(BD_ADDR remote_bda, UINT16 timeout);
tBTM_STATUS BTM_SetPacketTypes(BD_ADDR remote_bda, UINT16 pkt_types);
UINT16 BTM_ReadPacketTypes(BD_ADDR remote_bda);
UINT16 BTM_ReadClockOffset(BD_ADDR remote_bda);
BOOLEAN BTM_IsAclConnectionUp(BD_ADDR remote_bda);
UINT16 BTM_GetNumAclLinks(void);
/**/
UINT16 BTM_GetHCIConnHandle(BD_ADDR remote_bda);
/**/
UINT8 BTM_AllocateSCN(void);
BOOLEAN BTM_FreeSCN(UINT8 scn);
/**/
tBTM_STATUS BTM_ReadRemoteVersion(BD_ADDR addr, UINT8 *lmp_version,
                                  UINT16 *manufacturer, UINT16 *lmp_sub_version);
tBTM_STATUS BTM_AclRegisterForChanges(tBTM_ACL_DB_CHANGE_CB *p_cb);
tBTM_STATUS BTM_SetQoS(BD_ADDR bd, FLOW_SPEC *p_flow, tBTM_CMPL_CB *p_cb);
/**/
tBTM_STATUS BTM_ReadRSSI(BD_ADDR remote_bda, tBTM_CMPL_CB *p_cb);
tBTM_STATUS BTM_ReadLinkQuality(BD_ADDR remote_bda, tBTM_CMPL_CB *p_cb);
/**/
UINT8 BTM_SetTraceLevel(UINT8 new_level);
/**/
void BTM_GetAclBufInfo(UINT16 *p_num_avail, UINT16 *p_num_bufs,
                       UINT16 *p_num_queued);

BOOLEAN BTM_SecAddDevice(BD_ADDR bd_addr, DEV_CLASS dev_class, BD_NAME bd_name,
                         BD_FEATURES features, UINT32 *trusted_mask,
                         LINK_KEY link_key);
BOOLEAN BTM_SecDeleteDevice(BD_ADDR bd_addr);
char *BTM_SecReadDevName(BD_ADDR bd_addr);

void BTM_DeviceReset(tBTM_CMPL_CB *p_cb);
void BTM_SendHciReset(tBTM_CMPL_CB *p_cb);
BOOLEAN BTM_IsDeviceUp(void);
tBTM_STATUS BTM_SetAfhChannels(UINT8 first, UINT8 last);
tBTM_STATUS BTM_SetAfhChannelAssessment(BOOLEAN enable_or_disable);
void BTM_ContinueReset(void);
/**/
tBTM_STATUS BTM_SetLocalDeviceName(char *p_name);
tBTM_STATUS BTM_ReadLocalDeviceName(tBTM_CMPL_CB *p_cb);
/**/
tBTM_STATUS BTM_ReadLocalDeviceAddr(tBTM_CMPL_CB *p_cb);
/**/
tBTM_STATUS BTM_ReadLocalVersion(tBTM_VERSION_INFO *p_vers);
tBTM_STATUS BTM_SetDeviceClass(DEV_CLASS dev_class);
DEV_CLASS_PTR BTM_ReadDeviceClass(void);
UINT8 *BTM_ReadLocalFeatures(void);
tBTM_DEV_STATUS_CB *BTM_RegisterForDeviceStatusNotif(tBTM_DEV_STATUS_CB *p_cb);
tBTM_STATUS BTM_VendorSpecificCommand(UINT16 opcode, UINT8 param_len,
                                      UINT8 *p_param_buf, tBTM_CMPL_CB *p_cb);
/**/
tBTM_STATUS BTM_RegisterForVSEvents(tBTM_VS_EVT_CB *p_cb);
tBTM_STATUS BTM_WritePageTimeout(UINT16 timeout);
tBTM_STATUS BTM_WriteVoiceSettings(UINT16 settings);
tBTM_STATUS BTM_EnableTestMode(void);
/**/
tBTM_STATUS BTM_ReadStoredLinkKey(BD_ADDR bd_addr, tBTM_CMPL_CB *p_cb);
tBTM_STATUS BTM_WriteStoredLinkKey(UINT8 num_keys, BD_ADDR *bd_addr,
                                   LINK_KEY *link_key, tBTM_CMPL_CB *p_cb);
tBTM_STATUS BTM_DeleteStoredLinkKey(BD_ADDR bd_addr, tBTM_CMPL_CB *p_cb);

void BTM_SetDiscoveryParams(UINT16 num_uuid, tSDP_UUID *p_uuid_list,
                            UINT16 num_attr, UINT16 *p_attr_list);
tBTM_STATUS BTM_StartDiscovery(tBTM_CMPL_CB *p_cmpl_cb, BD_ADDR p_rem_addr);
tSDP_DISC_REC *BTM_FindAttribute(UINT16 attr_id, tSDP_DISC_REC *p_start_rec);
tSDP_DISC_REC *BTM_FindService(UINT16 service_uuid, tSDP_DISC_REC *p_start_rec);

tBTM_STATUS BTM_SetDiscoverability(UINT16 inq_mode, UINT16 window,
                                   UINT16 interval);
tBTM_STATUS BTM_SetInquiryScanType(UINT16 scan_type);
tBTM_STATUS BTM_SetPageScanType(UINT16 scan_type);
tBTM_STATUS BTM_SetInquiryMode(UINT8 mode);
UINT16 BTM_ReadDiscoverability(UINT16 *p_window, UINT16 *p_interval);
tBTM_STATUS BTM_SetPeriodicInquiryMode(tBTM_INQ_PARMS *p_inqparms,
                                       UINT16 max_delay, UINT16 min_delay,
                                       tBTM_INQ_RESULTS_CB *p_results_cb);
tBTM_STATUS BTM_CancelPeriodicInquiry(void);
tBTM_STATUS BTM_SetConnectability(UINT16 page_mode, UINT16 window,
                                  UINT16 interval);
UINT16 BTM_ReadConnectability(UINT16 *p_window, UINT16 *p_interval);
UINT16 BTM_IsInquiryActive(void);
tBTM_STATUS BTM_CancelInquiry(void);
tBTM_STATUS BTM_StartInquiry(tBTM_INQ_PARMS *p_inqparms,
                             tBTM_INQ_RESULTS_CB *p_results_cb,
                             tBTM_CMPL_CB *p_cmpl_cb);
tBTM_STATUS BTM_ReadRemoteDeviceName(BD_ADDR remote_bda, tBTM_CMPL_CB *p_cb);
tBTM_STATUS BTM_CancelRemoteDeviceName(void);
tBTM_INQ_INFO *BTM_InqFirstResult(void);
tBTM_INQ_INFO *BTM_InqNextResult(tBTM_INQ_INFO *p_cur);
tBTM_INQ_INFO *BTM_InqDbRead(BD_ADDR p_bda);
tBTM_INQ_INFO *BTM_InqDbFirst(void);
tBTM_INQ_INFO *BTM_InqDbNext(tBTM_INQ_INFO *p_cur);
tBTM_STATUS BTM_ClearInqDb(BD_ADDR p_bda);
UINT8 BTM_ReadNumInqDbEntries(void);
tBTM_STATUS BTM_InquiryRegisterForChanges(tBTM_INQ_DB_CHANGE_CB *p_cb);
void BTM_SetInquiryFilterCallback(tBTM_FILTER_CB *p_callback);

tBTM_STATUS BTM_PmRegister(UINT8 mask, UINT8 *p_pm_id,
                           tBTM_PM_STATUS_CBACK *p_cb);
tBTM_STATUS BTM_SetPowerMode(UINT8 pm_id, BD_ADDR remote_bda,
                             tBTM_PM_PWR_MD *p_mode);
tBTM_STATUS BTM_ReadPowerMode(BD_ADDR remote_bda, tBTM_PM_MODE *p_mode);
/**/
BOOLEAN BTM_IsPowerManagerOn(void);

tBTM_STATUS BTM_CreateSco(BD_ADDR remote_bda, BOOLEAN is_orig, UINT16 pkt_types,
                          UINT16 *p_sco_inx, tBTM_SCO_CB *p_conn_cb,
                          tBTM_SCO_CB *p_disc_cb);
/**/
tBTM_STATUS BTM_RemoveSco(UINT16 sco_inx);
/**/
tBTM_STATUS BTM_SetScoPacketTypes(UINT16 sco_inx, UINT16 pkt_types);
UINT16 BTM_ReadScoPacketTypes(UINT16 sco_inx);
UINT16 BTM_ReadScoDiscReason(void);
UINT16 BTM_ReadDeviceScoPacketTypes(void);
/**/
UINT16 BTM_ReadScoHandle(UINT16 sco_inx);
BD_ADDR_PTR BTM_ReadScoBdAddr(UINT16 sco_inx);
tBTM_STATUS BTM_SetEScoMode(tBTM_SCO_TYPE sco_mode, tBTM_ESCO_PARAMS *p_parms);
tBTM_STATUS BTM_RegForEScoEvts(UINT16 sco_inx, tBTM_ESCO_CBACK *p_esco_cback);
tBTM_STATUS BTM_ReadEScoLinkParms(UINT16 sco_inx, tBTM_ESCO_DATA *p_parms);
tBTM_STATUS BTM_ChangeEScoLinkParms(UINT16 sco_inx,
                                    tBTM_CHG_ESCO_PARAMS *p_parms);
void BTM_EScoConnRsp(UINT16 sco_inx, UINT8 hci_status,
                     tBTM_ESCO_PARAMS *p_parms);

BOOLEAN BTM_SecRegister(tBTM_APPL_INFO *p_cb_info);
BOOLEAN BTM_SecAddLinkKeyNotifyCallback(tBTM_LINK_KEY_CALLBACK *p_callback);
BOOLEAN BTM_SecDeleteLinkKeyNotifyCallback(tBTM_LINK_KEY_CALLBACK *p_callback);
BOOLEAN BTM_SecAddRmtNameNotifyCallback(tBTM_RMT_NAME_CALLBACK *p_callback);
BOOLEAN BTM_SecDeleteRmtNameNotifyCallback(tBTM_RMT_NAME_CALLBACK *p_callback);
void BTM_SecSetConnectFilterCallback(tBTM_FILTER_CB *p_callback);
void BTM_SetSecurityMode(UINT8 security_mode);
void BTM_SetPinType(UINT8 pin_type, PIN_CODE pin_code, UINT8 pin_code_len);
void BTM_SetPairableMode(BOOLEAN allow_pairing, BOOLEAN connect_only_paired);
BOOLEAN BTM_SetSecurityLevel(BOOLEAN is_originator, char *p_name,
                             UINT8 service_id, UINT8 sec_level, UINT16 psm,
                             UINT32 mx_proto_id, UINT32 mx_chan_id);
UINT8 BTM_SecClrService(UINT8 service_id);
void BTM_PINCodeReply(BD_ADDR bd_addr, UINT8 res, UINT8 pin_len, UINT8 *p_pin,
                      UINT32 *trusted_mask);
void BTM_DeviceAuthorized(BD_ADDR bd_addr, UINT8 res, UINT32 *trusted_mask);
tBTM_STATUS BTM_SecBond(BD_ADDR bd_addr, UINT8 pin_len, UINT8 *p_pin,
                        UINT32 *trusted_mask);
tBTM_STATUS BTM_SecUseMasterLinkKey(BOOLEAN use_master_key);
void BTM_SetMasterKeyCompCback(tBTM_MKEY_CALLBACK *mkey_cback);
tBTM_STATUS BTM_SecGetDeviceLinkKey(BD_ADDR bd_addr, LINK_KEY link_key);
tBTM_STATUS BTM_SetEncryption(BD_ADDR bd_addr, tBTM_SEC_CBACK *p_callback,
                              void *p_ref_data);

#ifdef __cplusplus
	}
#endif

#endif // BTE_BTM_API_H
