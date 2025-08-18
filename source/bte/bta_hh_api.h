#ifndef BTE_BTA_HH_API_H
#define BTE_BTA_HH_API_H

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * bta/include/bta_hh_api.h
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
#include "bt_types.h"
#include "bta_api.h"
#include "data_types.h"
#include "hiddefs.h"

/*******************************************************************************
 * macros
 */

#define BTA_HH_MAX_KNOWN        HID_HOST_MAX_DEVICES

#define BTA_HH_INVALID_HANDLE   0xff

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef UINT8 tBTA_HH_STATUS;
enum
{
    BTA_HH_OK = 0,
    BTA_HH_ERR = 6,
    BTA_HH_ERR_SDP = 7,
    BTA_HH_ERR_TOD_UNSPT = 255, // was 10
    BTA_HH_ERR_NO_RES = 11,          /* out of system resources */
    BTA_HH_ERR_16 = 16,
    BTA_HH_ERR_PROTO = 8,
    BTA_HH_HS_TRANS_NOT_SPT = 3,    /* handshake error : transaction not spt */
    BTA_HH_ERR_AUTH_FAILED = 12,     /* authentication fail */
    BTA_HH_HS_ERROR = 5,            /* handshake error : unspecified HS error */
    BTA_HH_ERR_DB_FULL = 9,
    BTA_HH_ERR_HDL = 13,

};

typedef UINT16 tBTA_HH_EVT;
#define BTA_HH_ENABLE_EVT       0       /* HH enabled */
#define BTA_HH_DISABLE_EVT      1       /* HH disabled */
#define BTA_HH_OPEN_EVT         2       /* connection opened */
#define BTA_HH_CLOSE_EVT        3       /* connection closed */
#define BTA_HH_GET_RPT_EVT      4       /* BTA_HhGetReport callback */
#define BTA_HH_SET_RPT_EVT      5       /* BTA_HhSetReport callback */
#define BTA_HH_GET_PROTO_EVT    6       /* BTA_GetProtoMode callback */
#define BTA_HH_SET_PROTO_EVT    7       /* BTA_HhSetProtoMode callback */
#define BTA_HH_GET_IDLE_EVT     8       /* BTA_HhGetIdle comes callback */
#define BTA_HH_SET_IDLE_EVT     9       /* BTA_HhSetIdle finish callback */
#define BTA_HH_GET_DSCP_EVT     10      /* Get report descripotor */
#define BTA_HH_ADD_DEV_EVT      11      /* Add Device callback */
#define BTA_HH_RMV_DEV_EVT      12      /* remove device finished */
#define BTA_HH_VC_UNPLUG_EVT    13      /* virtually unplugged */
#define BTA_HH_GET_ACL_Q_EVT    15

typedef UINT8 tBTA_HH_PROTO_MODE;
#define BTA_HH_PROTO_RPT_MODE	0
#define BTA_HH_PROTO_BOOT_MODE	1
#define BTA_HH_PROTO_UNKNOWN	255

typedef UINT8 tBTA_HH_TRANS_CTRL_TYPE;
enum
{
    BTA_HH_CTRL_VIRTUAL_CABLE_UNPLUG = 5,                   /* virtual unplug   */
    BTA_HH_CTRL_SUSPEND = 3,                               /* enter suspend    */
    BTA_HH_CTRL_EXIT_SUSPEND = 4,                          /* exit suspend     */
};

typedef UINT8 tBTA_HH_RPT_TYPE;
enum
{
    BTA_HH_RPTT_OUTPUT = 2,     /* output report    */
};

typedef UINT8 tBTA_HH_BOOT_RPT_ID;
enum
{
    BTA_HH_KEYBD_RPT_ID  =               1,
    BTA_HH_MOUSE_RPT_ID = 2,

};

typedef UINT8 tBTA_HH_DEVT;
#define BTA_HH_DEVT_UNKNOWN      0x00

typedef UINT16 tBTA_HH_ATTR_MASK;

typedef struct
{
    BD_ADDR         bda;                /* HID device bd address    */
    tBTA_HH_STATUS  status;             /* operation status         */
    UINT8           handle;             /* device handle            */
} tBTA_HH_CONN;

typedef tBTA_HH_CONN tBTA_HH_DEV_INFO;

typedef struct
{
    tBTA_HH_STATUS              status;     /* operation status         */
    UINT8                       handle;     /* device handle            */
} tBTA_HH_CBDATA;

typedef struct
{
    tBTA_HH_STATUS  status;                 /* handshake status */
    UINT8           handle;                 /* device handle    */
    union
    {
        tBTA_HH_PROTO_MODE      proto_mode; /* GET_PROTO_EVT :protocol mode */
        BT_HDR                  rpt_data;   /* GET_RPT_EVT   : report data  */
        UINT8                   idle_rate;  /* GET_IDLE_EVT  : idle rate    */
    } rsp_data;
} tBTA_HH_HSDATA;

typedef struct
{
    // total size: 0x6
    unsigned char handle; // offset 0x0, size 0x1
    unsigned short num_queued; // offset 0x2, size 0x2
    unsigned short num_not_acked; // offset 0x4, size 0x2
} tBTA_HH_QUEUE_INFO;

typedef struct
{
    // total size: 0x1E
    unsigned short num_avail_buffs; // offset 0x0, size 0x2
    unsigned short num_buffs; // offset 0x2, size 0x2
    unsigned short num_links; // offset 0x4, size 0x2
    tBTA_HH_QUEUE_INFO queue_info[4]; // offset 0x6, size 0x18
} tBTA_HH_ACL_QUEUE_INFO; // offset 0x0, size 0x1E

typedef union
{
    tBTA_HH_DEV_INFO        dev_info;           /* BTA_HH_ADD_DEV_EVT, BTA_HH_RMV_DEV_EVT   */
    tBTA_HH_CONN            conn;               /* BTA_HH_OPEN_EVT      */
    tBTA_HH_CBDATA          dev_status;         /* BTA_HH_CLOSE_EVT,
                                                   BTA_HH_SET_PROTO_EVT
                                                   BTA_HH_SET_RPT_EVT
                                                   BTA_HH_SET_IDLE_EVT  */

    tBTA_HH_STATUS          status;             /* BTA_HH_ENABLE_EVT */
    tHID_DEV_DSCP_INFO      dscp_info;          /* BTA_HH_GET_DSCP_EVT */
    tBTA_HH_HSDATA          hs_data;            /* GET_ transaction callback
                                                   BTA_HH_GET_RPT_EVT
                                                   BTA_HH_GET_PROTO_EVT
                                                   BTA_HH_GET_IDLE_EVT */
    tBTA_HH_ACL_QUEUE_INFO acl_queue_info;
} tBTA_HH;

typedef void tBTA_HH_CBACK(tBTA_HH_EVT event, tBTA_HH *p_data);

typedef struct
{
    UINT8               this_char;       /* virtual key code     */
    BOOLEAN             caps_lock;          /* is caps locked       */
    BOOLEAN             upper;
    BOOLEAN             ctrl;
    BOOLEAN             alt;
    BOOLEAN             num_lock;           /* is Num key pressed   */
} tBTA_HH_KEYBD_RPT;

typedef struct
{
    UINT8               mouse_button;       /* mouse button is clicked   */
    INT8                delta_x;            /* displacement x            */
    INT8                delta_y;            /* displacement y            */
} tBTA_HH_MICE_RPT;

typedef struct
{
    tBTA_HH_BOOT_RPT_ID dev_type;           /* type of device report */
    union
    {
        tBTA_HH_KEYBD_RPT   keybd_rpt;      /* keyboard report      */
        tBTA_HH_MICE_RPT    mice_rpt;       /* mouse report         */
    }                   data_rpt;
} tBTA_HH_BOOT_RPT;

/*******************************************************************************
 * external globals
 */

/*******************************************************************************
 * functions
 */

void BTA_HhEnable(tBTA_SEC sec_mask, tBTA_HH_CBACK *p_cback);
void BTA_HhDisable(void);
void BTA_HhClose(UINT8 dev_handle);
void BTA_HhOpen(BD_ADDR dev_bda, tBTA_HH_PROTO_MODE mode, tBTA_SEC sec_mask);

void BTA_HhSetReport(UINT8 dev_handle, tBTA_HH_RPT_TYPE r_type, BT_HDR *p_data);
void BTA_HhGetReport(UINT8 dev_handle, tBTA_HH_RPT_TYPE r_type, UINT8 rpt_id,
                     UINT16 buf_size);
void BTA_HhSetProtoMode(UINT8 dev_handle, tBTA_HH_PROTO_MODE p_type);
void BTA_HhGetProtoMode(UINT8 dev_handle);
void BTA_HhSetIdle(UINT8 dev_handle, UINT16 idle_rate);
void BTA_HhGetIdle(UINT8 dev_handle);
void BTA_HhSendCtrl(UINT8 dev_handle, tBTA_HH_TRANS_CTRL_TYPE c_type);
void BTA_HhSendData(UINT8 dev_handle, BT_HDR *p_data);
void BTA_HhGetDscpInfo(UINT8 dev_handle);
void BTA_HhAddDev(BD_ADDR bda, tBTA_HH_ATTR_MASK attr_mask, UINT8 sub_class,
                  UINT8 app_id, tHID_DEV_DSCP_INFO dscp_info);
void BTA_HhRemoveDev(UINT8 dev_handle);
void BTA_HhGetAclQueueInfo(void);
void BTA_HhParseBootRpt(tBTA_HH_BOOT_RPT *p_data, UINT8 *p_report,
                        UINT16 report_len);

#ifdef __cplusplus
	}
#endif

#endif // BTE_BTA_HH_API_H
