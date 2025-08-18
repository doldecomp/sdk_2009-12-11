#ifndef BTE_BTA_HH_INT_H
#define BTE_BTA_HH_INT_H

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * bta/hh/bta_hh_int.h
 */

/******************************************************************************
 *
 *  Copyright (C) 2005-2012 Broadcom Corporation
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
#include "bta_api.h"
#include "data_types.h"

#include "bta_hh_api.h"
#include "sdp_api.h"
#include "hiddefs.h"

/*******************************************************************************
 * macros
 */

#define BTA_HH_MAX_RPT_CHARS    8

#define BTA_HH_FST_TRANS_CB_EVT         BTA_HH_GET_RPT_EVT
#define BTA_HH_FST_BTE_TRANS_EVT        HID_TRANS_GET_REPORT

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

enum
{
    BTA_HH_API_CLOSE_EVT = 5889,
    BTA_HH_SDP_CMPL_EVT = 5895,
    BTA_HH_OPEN_CMPL_EVT = 5899,
    BTA_HH_INT_OPEN_EVT = 5890,
    BTA_HH_INT_CLOSE_EVT = 5891,
    BTA_HH_INT_DATA_EVT = 5892,
    BTA_HH_INT_HANDSK_EVT = 5894,
    BTA_HH_INT_CTRL_DATA = 5893,
    BTA_HH_INVALID_EVT = 5904, // was 5903

    BTA_HH_API_ENABLE_EVT = 5900,
    BTA_HH_API_DISABLE_EVT = 5901,
    BTA_HH_API_OPEN_EVT = 5888,
    BTA_HH_API_WRITE_DEV_EVT = 5896,
    BTA_HH_API_GET_DSCP_EVT = 5897,
    BTA_HH_API_MAINT_DEV_EVT = 5898,
    BTA_HH_API_GET_ACL_Q_EVT = 5902,
    BTA_HH_DISC_CMPL_EVT = 5903, // was 5902
};

typedef UINT8 tBTA_HH_STATE;
enum
{
    BTA_HH_NULL_ST = 0,
    BTA_HH_IDLE_ST = 1,
    BTA_HH_W4_CONN_ST = 2,
    BTA_HH_CONN_ST = 3,
};

typedef UINT16 tBTA_HH_INT_EVT;         /* HID host internal events */

typedef struct
{
    tBTA_HH_DEVT        tod;        /* type of device               */
    UINT8               app_id;     /* corresponding application ID */
} tBTA_HH_SPT_TOD;

typedef struct
{
    UINT8                max_devt_spt; /* max number of types of devices spt */
    tBTA_HH_SPT_TOD     *p_devt_list;  /* supported types of device list     */
    UINT16               sdp_db_size;
} tBTA_HH_CFG;

typedef struct
{
    BOOLEAN upper; // offset 0x0, size 0x1
    BOOLEAN ctrl; // offset 0x1, size 0x1
    BOOLEAN alt; // offset 0x2, size 0x1
    BOOLEAN num_lock; // offset 0x3, size 0x1
    BOOLEAN caps_lock; // offset 0x4, size 0x1
    UINT8 last_report[BTA_HH_MAX_RPT_CHARS]; // offset 0x5, size 0x8
} tBTA_HH_KB_CB;

typedef struct
{
    tHID_DEV_DSCP_INFO  dscp_info;      /* report descriptor and DI information */
    BD_ADDR             addr;           /* BD-Addr of the HID device */
    UINT16              attr_mask;      /* attribute mask */
    UINT16              w4_evt;         /* W4_handshake event name */
    UINT8               index;          /* index number referenced to handle index */
    UINT8               sub_class;      /* Cod sub class */
    UINT8               sec_mask;       /* security mask */
    UINT8               app_id;         /* application ID for this connection */
    UINT8               hid_handle;     /* device handle */
    BOOLEAN             vp;             /* virtually unplug flag */
    BOOLEAN             in_use;         /* control block currently in use */
    BOOLEAN             incoming_conn;  /* is incoming connection? */
    BOOLEAN             opened;         /* TRUE if device successfully opened HID connection */
    tBTA_HH_PROTO_MODE  mode;           /* protocol mode */
    tBTA_HH_STATE       state;          /* CB state */
} tBTA_HH_DEV_CB;

typedef struct
{
    tBTA_HH_KB_CB           kb_cb;                  /* key board control block,
                                                       suppose BTA will connect
                                                       to only one keyboard at
                                                        the same time */
    tBTA_HH_DEV_CB          kdev[BTA_HH_MAX_KNOWN]; /* device control block */
    tBTA_HH_DEV_CB*         p_cur;              /* current device control
                                                       block idx, used in sdp */
    UINT8                   cb_index[BTA_HH_MAX_KNOWN]; /* maintain a CB index
                                                        map to dev handle */
    tBTA_HH_CBACK          *p_cback;               /* Application callbacks */
    tSDP_DISCOVERY_DB*      p_disc_db;
    UINT8                   trace_level;            /* tracing level */
    UINT8                   cnt_num;                /* connected device number */
    BOOLEAN                 w4_disable;             /* w4 disable flag */
} tBTA_HH_CB;

typedef struct
{
    BT_HDR          hdr;
    UINT8           sec_mask;
    UINT8           service_name[BTA_SERVICE_NAME_LEN + 1];
    tBTA_HH_CBACK   *p_cback;
} tBTA_HH_API_ENABLE;

typedef struct
{
    BT_HDR          hdr;
    BD_ADDR         bd_addr;
    UINT8           sec_mask;
    tBTA_HH_PROTO_MODE  mode;
} tBTA_HH_API_CONN;

typedef struct
{
    BT_HDR          hdr;
    UINT8           t_type;
    UINT8           param;
    UINT8           rpt_id;
    UINT16          data;
    BT_HDR          *p_data;
} tBTA_HH_CMD_DATA;

typedef struct
{
    BT_HDR          hdr;
    UINT32          data;
    BT_HDR          *p_data;
} tBTA_HH_CBACK_DATA;

typedef struct
{
    BT_HDR              hdr;
    BD_ADDR             bda;
    UINT16              attr_mask;
    UINT16              sub_event;
    UINT8               sub_class;
    UINT8               app_id;
    tHID_DEV_DSCP_INFO  dscp_info;
} tBTA_HH_MAINT_DEV;

typedef union
{
    BT_HDR                   hdr;
    tBTA_HH_API_ENABLE       api_enable;
    tBTA_HH_API_CONN         api_conn;
    tBTA_HH_CMD_DATA         api_sndcmd;
    tBTA_HH_CBACK_DATA       hid_cback;
    tBTA_HH_STATUS           status;
    tBTA_HH_MAINT_DEV        api_maintdev;
} tBTA_HH_DATA;

/*******************************************************************************
 * external globals
 */

extern tBTA_HH_CB bta_hh_cb;
extern tBTA_HH_CFG *p_bta_hh_cfg;

/*******************************************************************************
 * functions
 */

void bta_hh_disc_cmpl(void);
void bta_hh_sm_execute(tBTA_HH_DEV_CB *p_cb, UINT16 event,
                       tBTA_HH_DATA *p_data);
BOOLEAN bta_hh_tod_spt(tBTA_HH_DEV_CB *p_cb, UINT8 sub_class);
void bta_hh_add_device_to_list(tBTA_HH_DEV_CB *p_cb, UINT8 handle,
                               UINT16 attr_mask,
                               tHID_DEV_DSCP_INFO *p_dscp_info, UINT8 sub_class,
                               UINT8 app_id);
void bta_hh_clean_up_kdev(tBTA_HH_DEV_CB *p_cb);
void bta_hh_trace_dev_db(void);
BOOLEAN bta_hh_hdl_event(BT_HDR *p_msg);
void bta_hh_parse_keybd_rpt(tBTA_HH_BOOT_RPT *p_kb_data, UINT8 *p_report,
                            UINT16 report_len);
void bta_hh_parse_mice_rpt(tBTA_HH_BOOT_RPT *p_kb_data, UINT8 *p_report,
                           UINT16 report_len);
UINT8 bta_hh_find_cb(BD_ADDR bda);

// ---

void bta_hh_api_enable(tBTA_HH_DATA *p_data);
void bta_hh_api_disable(void);
void bta_hh_disc_cmpl(void);

void bta_hh_start_sdp(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
void bta_hh_sdp_cmpl(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
void bta_hh_api_disc_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
void bta_hh_open_cmpl_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
void bta_hh_open_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
void bta_hh_data_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
void bta_hh_handsk_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
void bta_hh_ctrl_dat_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
void bta_hh_close_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
void bta_hh_get_dscp_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
void bta_hh_maint_dev_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
void bta_hh_get_acl_q_info(void);
void bta_hh_write_dev_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);

void bta_hh_sm_execute(tBTA_HH_DEV_CB *p_cb, UINT16 event,
                       tBTA_HH_DATA *p_data);
BOOLEAN bta_hh_hdl_event(BT_HDR *p_msg);

UINT8 bta_hh_find_cb(BD_ADDR bda);
void bta_hh_clean_up_kdev(tBTA_HH_DEV_CB *p_cb);
void bta_hh_add_device_to_list(tBTA_HH_DEV_CB *p_cb, UINT8 handle,
                               UINT16 attr_mask,
                               tHID_DEV_DSCP_INFO *p_dscp_info, UINT8 sub_class,
                               UINT8 app_id);
BOOLEAN bta_hh_tod_spt(tBTA_HH_DEV_CB *p_cb, UINT8 sub_class);
void bta_hh_parse_keybd_rpt(tBTA_HH_BOOT_RPT *p_kb_data, UINT8 *p_report,
                            UINT16 report_len);
void bta_hh_parse_mice_rpt(tBTA_HH_BOOT_RPT *p_mice_data, UINT8 *p_report,
                           UINT16 report_len);
void bta_hh_trace_dev_db(void);

#ifdef __cplusplus
	}
#endif

#endif // BTE_BTA_HH_INT_H
