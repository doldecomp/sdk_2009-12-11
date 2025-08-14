#ifndef BTE_BTM_INT_H
#define BTE_BTM_INT_H

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/btm/btm_int.h
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

#include "btm_api.h"
#include "gki.h"
#include "hcidefs.h"
#include "rfcdefs.h"
#include "sdp_api.h"

/*******************************************************************************
 * macros
 */

#define BTM_MIN_INQUIRY_LENGTH		1
#define BTM_MAX_INQUIRY_LENGTH		HCI_MAX_INQUIRY_LENGTH

#define BTM_PER_INQ_MIN_MIN_PERIOD	HCI_PER_INQ_MIN_MIN_PERIOD
#define BTM_PER_INQ_MAX_MIN_PERIOD	HCI_PER_INQ_MAX_MIN_PERIOD

#define BTM_PER_INQ_MIN_MAX_PERIOD	HCI_PER_INQ_MIN_MAX_PERIOD
#define BTM_PER_INQ_MAX_MAX_PERIOD	HCI_PER_INQ_MAX_MAX_PERIOD

#define BTM_MAX_SCN					PORT_MAX_RFC_PORTS

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef char tBTM_LOC_BD_NAME[BTM_MAX_LOC_BD_NAME_LEN + 1];

typedef tBTM_SEC_CBACK tBTM_SEC_CALLBACK;

typedef struct
{
	UINT32	mx_proto_id;										// size 0x04, offset 0x00
	UINT32	orig_mx_chan_id;									// size 0x04, offset 0x04
	UINT32	term_mx_chan_id;									// size 0x04, offset 0x08
	UINT16	psm;												// size 0x02, offset 0x0c
	UINT8	security_flags;										// size 0x01, offset 0x0e
	UINT8	service_id;											// size 0x01, offset 0x0f
	UINT8	orig_service_name[BTM_SEC_SERVICE_NAME_LEN + 1];	// size 0x16, offset 0x10
	UINT8	term_service_name[BTM_SEC_SERVICE_NAME_LEN + 1];	// size 0x16, offset 0x26
} tBTM_SEC_SERV_REC; // size 0x3c

typedef struct
{
	tBTM_SEC_SERV_REC	*p_cur_service;								// size 0x04, offset 0x00
	tBTM_SEC_CALLBACK	*p_callback;								// size 0x04, offset 0x04
	void				*p_ref_data;								// size 0x04, offset 0x08
	UINT32				timestamp;									// size 0x04, offset 0x0c
	UINT32				trusted_mask[BTM_SEC_SERVICE_ARRAY_SIZE];	// size 0x08, offset 0x10
	UINT16				hci_handle;									// size 0x02, offset 0x18
	UINT16				clock_offset;								// size 0x02, offset 0x1a
	BD_ADDR				bd_addr;									// size 0x06, offset 0x1c
	DEV_CLASS			dev_class;									// size 0x03, offset 0x22
	LINK_KEY			link_key;									// size 0x10, offset 0x25
	tBTM_BD_NAME		sec_bd_name;								// size 0x41, offset 0x35
	UINT8				sec_flags;									// size 0x01, offset 0x76
	BD_FEATURES			features;									// size 0x08, offset 0x77
	UINT8				sec_state;									// size 0x01, offset 0x7f
	BOOLEAN				is_originator;								// size 0x01, offset 0x80
	UINT8				security_required;							// size 0x01, offset 0x81
	BOOLEAN				link_key_not_sent;							// size 0x01, offset 0x82
	UINT8				link_key_type;								// size 0x01, offset 0x83
	BOOLEAN				link_key_changed;							// size 0x01, offset 0x84
	BOOLEAN				sec_acl_connected;							// size 0x01, offset 0x85
	/* 2 bytes padding */
} tBTM_SEC_DEV_REC; // size 0x88

typedef struct
{
	tBTM_LOC_BD_NAME	bd_name;			// size 0x20, offset 0x00
	BOOLEAN				pin_type;			// size 0x01, offset 0x20
	UINT8				pin_code_len;		// size 0x01, offset 0x21
	PIN_CODE			pin_code;			// size 0x10, offset 0x22
	BOOLEAN				connectable;		// size 0x01, offset 0x32
	UINT8				def_inq_scan_mode;	// size 0x01, offset 0x33
} tBTM_CFG; // size 0x34
enum
{
	BTM_ACL_SWKEY_STATE_IDLE,
	BTM_ACL_SWKEY_STATE_MODE_CHANGE,
	BTM_ACL_SWKEY_STATE_ENCRYPTION_OFF,
	BTM_ACL_SWKEY_STATE_SWITCHING,
	BTM_ACL_SWKEY_STATE_ENCRYPTION_ON,
	BTM_ACL_SWKEY_STATE_IN_PROGRESS,
};

typedef struct
{
	UINT16		hci_handle;			// size 0x002, offset 0x000
	UINT16		pkt_types_mask;		// size 0x002, offset 0x002
	UINT16		restore_pkt_types;	// size 0x002, offset 0x004
	UINT16		clock_offset;		// size 0x002, offset 0x006
	BD_ADDR		remote_addr;		// size 0x006, offset 0x008
	DEV_CLASS	remote_dc;			// size 0x003, offset 0x00e
	BD_NAME		remote_name;		// size 0x0f8, offset 0x011
	/* 1 byte padding */
	UINT16		manufacturer;		// size 0x002, offset 0x10a
	UINT16		lmp_subversion;		// size 0x002, offset 0x10c
	UINT16		link_super_tout;	// size 0x002, offset 0x10e
	BD_FEATURES	features;			// size 0x008, offset 0x110
	UINT8		lmp_version;		// size 0x001, offset 0x118
	BOOLEAN		in_use;				// size 0x001, offset 0x119
	UINT8		link_role;			// size 0x001, offset 0x11a
	UINT8		switch_role_state;	// size 0x001, offset 0x11b
} tACL_CONN; // size 0x11c

typedef struct
{
	tBTM_PM_PWR_MD	req_mode[BTM_MAX_PM_RECORDS];	// size 0x14, offset 0x00
	tBTM_PM_PWR_MD	set_mode;						// size 0x0a, offset 0x14
	UINT16			interval;						// size 0x02, offset 0x1e
	UINT8			state;							// size 0x01, offset 0x20
	UINT8			chg_ind;						// size 0x01, offset 0x21
} tBTM_PM_MCB; // size 0x22

typedef void tBTM_PM_STATUS_CBACK(UINT8 *, UINT8, UINT16, UINT8);

typedef struct
{
	tBTM_PM_STATUS_CBACK	*cback;	// size 0x04, offset 0x00
	UINT8					mask;	// size 0x01, offset 0x04
} tBTM_PM_RCB; // size 0x08

typedef struct
{
	UINT32	inq_count;	// size 0x04, offset 0x00
	BD_ADDR	bd_addr;	// size 0x06, offset 0x04
	/* 2 bytes padding */
} tINQ_BDADDR; // size 0x0c

typedef struct
{
	UINT32			time_of_resp;	// size 0x04, offset 0x00
	UINT32			inq_count;		// size 0x04, offset 0x04
	tBTM_INQ_INFO	inq_info;		// size 0x12, offset 0x08
	BOOLEAN			in_use;			// size 0x01, offset 0x1a
	/* 1 byte padding */
} tINQ_DB_ENT; // size 0x1c

#define BTM_EXT_RMT_NAME_TIMEOUT    40

#define BTM_RMT_NAME_INACTIVE       0
#define BTM_RMT_NAME_EXT            0x1     /* Initiated through API */
#define BTM_RMT_NAME_SEC            0x2     /* Initiated internally by security manager */
#define BTM_RMT_NAME_INQ            0x4     /* Remote name initiated internally by inquiry */

#define BTM_INQ_INACTIVE_STATE      0
#define BTM_INQ_CLR_FILT_STATE      1   /* Currently clearing the inquiry filter preceeding the inquiry request */
                                        /* (bypassed if filtering is not used)                                  */
#define BTM_INQ_SET_FILT_STATE      2   /* Sets the new filter (or turns off filtering) in this state */
#define BTM_INQ_ACTIVE_STATE        3   /* Actual inquiry or periodic inquiry is in progress */
#define BTM_INQ_REMNAME_STATE       4   /* Remote name requests are active  */

typedef void tf_17(void *);

typedef struct
{
	tBTM_CMPL_CB			*p_remname_cmpl_cb;				// size 0x004, offset 0x000
	TIMER_LIST_ENT			rmt_name_timer_ent;				// size 0x018, offset 0x004
	UINT16					discoverable_mode;				// size 0x002, offset 0x01c
	UINT16					connectable_mode;				// size 0x002, offset 0x01e
	UINT16					page_scan_window;				// size 0x002, offset 0x020
	UINT16					page_scan_period;				// size 0x002, offset 0x022
	UINT16					inq_scan_window;				// size 0x002, offset 0x024
	UINT16					inq_scan_period;				// size 0x002, offset 0x026
	UINT16					inq_scan_type;					// size 0x002, offset 0x028
	UINT16					page_scan_type;					// size 0x002, offset 0x02a
	BD_ADDR					remname_bda;					// size 0x006, offset 0x02c
	BOOLEAN					remname_active;					// size 0x001, offset 0x032
	/* 1 byte padding */
	tBTM_CMPL_CB			*p_inq_cmpl_cb;					// size 0x004, offset 0x034
	tf_17					*p_inq_results_cb;				// size 0x004, offset 0x038
	tBTM_CMPL_CB			*p_inqfilter_cmpl_cb;			// size 0x004, offset 0x03c
	tBTM_INQ_DB_CHANGE_CB	*p_inq_change_cb;				// size 0x004, offset 0x040
	UINT32					inq_counter;					// size 0x004, offset 0x044
	TIMER_LIST_ENT			inq_timer_ent;					// size 0x018, offset 0x048
	tINQ_BDADDR				*p_bd_db;						// size 0x004, offset 0x060
	UINT16					num_bd_entries;					// size 0x002, offset 0x064
	UINT16					max_bd_entries;					// size 0x002, offset 0x066
	tINQ_DB_ENT				inq_db[BTM_INQ_DB_SIZE];		// size 0x150, offset 0x068
	tBTM_INQ_PARMS			inqparms;						// size 0x00a, offset 0x1b8
	tBTM_INQUIRY_CMPL		inq_cmpl_info;					// size 0x002, offset 0x1c2
	UINT16					per_min_delay;					// size 0x002, offset 0x1c4
	UINT16					per_max_delay;					// size 0x002, offset 0x1c6
	BOOLEAN					inqfilt_active;					// size 0x001, offset 0x1c8
	UINT8					pending_filt_complete_event;	// size 0x001, offset 0x1c9
	UINT8					inqfilt_type;					// size 0x001, offset 0x1ca
	UINT8					state;							// size 0x001, offset 0x1cb
	UINT8					inq_active;						// size 0x001, offset 0x1cc
	/* 3 bytes padding */
} tBTM_INQUIRY_VAR_ST; // size 0x1d0

typedef struct
{
	tBTM_SCO_CB		*p_conn_cb;		// size 0x04, offset 0x00
	tBTM_SCO_CB		*p_disc_cb;		// size 0x04, offset 0x04
	UINT16			state;			// size 0x02, offset 0x08
	UINT16			hci_handle;		// size 0x02, offset 0x0a
	BOOLEAN			is_orig;		// size 0x01, offset 0x0c
	BOOLEAN			rem_bd_known;	// size 0x01, offset 0x0d
	/* 2 bytes padding */
	tBTM_ESCO_INFO	esco;			// size 0x24, offset 0x10
} tSCO_CONN; // size 0x34

typedef void tBTM_SCO_IND_CBACK(UINT16);

typedef struct
{
	tBTM_SCO_IND_CBACK	*app_sco_ind_cb;			// size 0x04, offset 0x00
	tSCO_CONN			sco_db[BTM_MAX_SCO_LINKS];	// size 0x9c, offset 0x04
	BD_ADDR				xfer_addr;					// size 0x06, offset 0xa0
	UINT16				sco_disc_reason;			// size 0x02, offset 0xa6
	tBTM_ESCO_PARAMS	def_esco_parms;				// size 0x10, offset 0xa8
	BOOLEAN				esco_supported;				// size 0x01, offset 0xb8
	tBTM_SCO_TYPE		desired_sco_mode;			// size 0x01, offset 0xb9
	tBTM_SCO_TYPE		xfer_sco_type;				// size 0x01, offset 0xba
	/* 1 byte padding */
} tSCO_CB; // size 0xbc

typedef tSDP_DISCOVERY_DB tBTM_DISCOVERY_DB;

// TODO: where should this go?
typedef struct
{
	TIMER_LIST_ENT	disc_timer_ent;		// size 0x0018, offset 0x0000
	UINT16			num_uuid_filters;	// size 0x0002, offset 0x0018
	/* 2 bytes padding */
	tBT_UUID		uuid_filters[3];	// size 0x003c, offset 0x001c
	UINT16			num_attr_filters;	// size 0x0002, offset 0x0058
	UINT16			attr_filters[12];	// size 0x0018, offset 0x005a
	/* 2 bytes padding */
	tBTM_INQ_INFO	*p_cur_ii;			// size 0x0004, offset 0x0074
	tBTM_CMPL_CB	*p_disc_cmpl_cb;	// size 0x0004, offset 0x0078 // TODO: is this actually something else?
	UINT8			btm_db_area[4000];	// size 0x0fa0, offset 0x007c
	UINT8			disc_active;		// size 0x0001, offset 0x101c
	/* 3 bytes padding */
} tBTM_DISCOVERY_VAR_ST; // size 0x1020

#define BTM_SEC_MAX_LINK_KEY_CALLBACKS	2
#define BTM_SEC_MAX_RMT_NAME_CALLBACKS	2

typedef struct
{
	tBTM_CFG				cfg;													// size 0x0034, offset 0x0000
	tACL_CONN				acl_db[MAX_L2CAP_LINKS];								// size 0x0470, offset 0x0034
	UINT8					btm_scn[BTM_MAX_SCN];									// size 0x001f, offset 0x04a4
	/* 1 byte padding */
	UINT16					btm_def_link_policy;									// size 0x0002, offset 0x04c4
	UINT16					btm_def_link_super_tout;								// size 0x0002, offset 0x04c6
	tBTM_ACL_DB_CHANGE_CB	*p_acl_changed_cb;										// size 0x0004, offset 0x04c8
	tBTM_PM_MCB				pm_mode_db[MAX_L2CAP_LINKS];							// size 0x0088, offset 0x04cc
	tBTM_PM_RCB				pm_reg_db[BTM_MAX_PM_RECORDS];							// size 0x0010, offset 0x0554
	UINT8					pm_pend_link;											// size 0x0001, offset 0x0564
	UINT8					pm_pend_id;												// size 0x0001, offset 0x0565
	/* 2 bytes padding */
	tBTM_DEVCB				devcb;													// size 0x00ec, offset 0x0568
	UINT16					btm_acl_pkt_types_supported;							// size 0x0002, offset 0x0654
	UINT16					btm_sco_pkt_types_supported;							// size 0x0002, offset 0x0656
	tBTM_DISCOVERY_VAR_ST	btm_disc_vars;											// size 0x1020, offset 0x0658
	tBTM_DISCOVERY_DB		*p_btm_ddb;												// size 0x0004, offset 0x1678
	tBTM_INQUIRY_VAR_ST		btm_inq_vars;											// size 0x01d0, offset 0x167c
	tBTM_FILTER_CB			*p_inq_filter_cb;										// size 0x0004, offset 0x184c
	tSCO_CB					sco_cb;													// size 0x00bc, offset 0x1850
	tBTM_APPL_INFO			api;													// size 0x0018, offset 0x190c
	tBTM_LINK_KEY_CALLBACK	*p_link_key_callback[BTM_SEC_MAX_LINK_KEY_CALLBACKS];	// size 0x0008, offset 0x1924
	tBTM_RMT_NAME_CALLBACK	*p_rmt_name_callback[BTM_SEC_MAX_RMT_NAME_CALLBACKS];	// size 0x0008, offset 0x192c
	tBTM_FILTER_CB			*p_conn_filter_cb;										// size 0x0004, offset 0x1934
	tBTM_SEC_DEV_REC		*p_collided_dev_rec;									// size 0x0004, offset 0x1938
	TIMER_LIST_ENT			sec_pin_code_req_tle;									// size 0x0018, offset 0x193c
	BD_ADDR					sec_pin_code_req_bd_addr;								// size 0x0006, offset 0x1954
	/* 2 bytes padding */
	TIMER_LIST_ENT			sec_collision_tle;										// size 0x0018, offset 0x195c
	UINT32					dev_rec_count;											// size 0x0004, offset 0x1974
	UINT8					security_mode;											// size 0x0001, offset 0x1978
	BOOLEAN					pairing_disabled;										// size 0x0001, offset 0x1979
	BOOLEAN					connect_only_paired;									// size 0x0001, offset 0x197a
	BOOLEAN					security_mode_changed;									// size 0x0001, offset 0x197b
	BOOLEAN					pin_type_changed;										// size 0x0001, offset 0x197c
	BOOLEAN					bond_created_connection;								// size 0x0001, offset 0x197d
	UINT8					disconnect_after_bond_reason;							// size 0x0001, offset 0x197e
	UINT8					pin_code_len;											// size 0x0001, offset 0x197f
	PIN_CODE				pin_code;												// size 0x0010, offset 0x1980
	tBTM_SEC_SERV_REC		sec_serv_rec[BTM_SEC_MAX_SERVICE_RECORDS];				// size 0x0600, offset 0x1990
	tBTM_SEC_DEV_REC		sec_dev_rec[BTM_SEC_MAX_DEVICE_RECORDS];				// size 0x0880, offset 0x1f90
	tBTM_MKEY_CALLBACK		*mkey_cback;											// size 0x0004, offset 0x2810
	BD_ADDR					connecting_bda;											// size 0x0006, offset 0x2814
	DEV_CLASS				connecting_dc;											// size 0x0003, offset 0x281a
	UINT8					first_disabled_channel;									// size 0x0001, offset 0x281d
	UINT8					last_disabled_channel;									// size 0x0001, offset 0x281e
	UINT8					acl_disc_reason;										// size 0x0001, offset 0x281f
	UINT8					trace_level;											// size 0x0001, offset 0x2820
	/* 3 bytes padding */
} tBTM_CB;

/*******************************************************************************
 * external globals
 */

extern tBTM_CB btm_cb;

/*******************************************************************************
 * functions
 */

tBTM_STATUS btm_sec_mx_access_request(BD_ADDR bd_addr, UINT16 psm,
                                      BOOLEAN is_originator, UINT32 mx_proto_id,
                                      UINT32 mx_chan_id,
                                      tBTM_SEC_CALLBACK *p_callback,
                                      void *p_ref_data);
UINT16 btm_get_acl_disc_reason_code(void);
BOOLEAN btm_acl_notif_conn_collision(BD_ADDR bda);
unsigned char btm_sec_l2cap_access_req(
	BD_ADDR bd_addr, UINT16 psm, UINT16 handle, BOOLEAN is_originator,
	void (*p_callback)(unsigned char *, void *, unsigned char));
void btm_sec_abort_access_req(BD_ADDR bd_addr);
BOOLEAN btm_dev_support_switch(BD_ADDR bd_addr);
unsigned char btm_sec_is_bonding(unsigned short handle);
tBTM_SEC_DEV_REC *btm_find_dev(BD_ADDR bd_addr);
void btm_acl_created(BD_ADDR bda, DEV_CLASS dc, BD_NAME bdn, UINT16 hci_handle,
                     UINT8 link_role);
void btm_sco_acl_removed(BD_ADDR bda);
void btm_remove_sco_links(BD_ADDR bda);
void btm_acl_removed(BD_ADDR bda);
tBTM_STATUS btm_sec_disconnect(UINT16 handle, UINT8 reason);
BOOLEAN btm_is_sco_active_by_bdaddr(BD_ADDR remote_bda);
UINT16 btm_get_max_packet_size(BD_ADDR addr);
tBTM_STATUS btm_remove_acl(BD_ADDR bd_addr);
UINT32 *btm_read_trusted_mask(BD_ADDR bd_addr);

#ifdef __cplusplus
	}
#endif

#endif // BTE_BTM_INT_H
