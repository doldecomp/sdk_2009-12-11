#ifndef BTE_SDP_INT_H
#define BTE_SDP_INT_H

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/sdp/sdpint.h
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

#include "gki.h"
#include "l2c_api.h"
#include "sdp_api.h"

/*******************************************************************************
 * macros
 */

#define     MAX_UUIDS_PER_SEQ       16
#define     MAX_ATTR_PER_SEQ        16

#define  SDP_PDU_ERROR_RESPONSE                 0x01
#define  SDP_PDU_SERVICE_SEARCH_REQ             0x02
#define  SDP_PDU_SERVICE_SEARCH_RSP             0x03
#define  SDP_PDU_SERVICE_ATTR_REQ               0x04
#define  SDP_PDU_SERVICE_ATTR_RSP               0x05
#define  SDP_PDU_SERVICE_SEARCH_ATTR_REQ        0x06
#define  SDP_PDU_SERVICE_SEARCH_ATTR_RSP        0x07

#define SDP_CONTINUATION_LEN        2
#define SDP_MAX_CONTINUATION_LEN    16          /* As per the spec */

#define SDP_INACT_TIMEOUT       30              /* Inactivity timeout         */

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef struct
{
    UINT32  len;           /* Number of bytes in the entry */
    UINT8   *value_ptr;    /* Points to attr_pad */
    UINT16  id;
    UINT8   type;
} tSDP_ATTRIBUTE;

typedef struct
{
    UINT32              record_handle;
    UINT32              free_pad_ptr;
    UINT16              num_attributes;
    tSDP_ATTRIBUTE      attribute[SDP_MAX_REC_ATTR];
    UINT8               attr_pad[SDP_MAX_PAD_LEN];
} tSDP_RECORD;

#define SDP_STATE_IDLE              0
#define SDP_STATE_CONN_SETUP        1
#define SDP_STATE_CFG_SETUP         2
#define SDP_STATE_CONNECTED         3

#define SDP_FLAGS_IS_ORIG           0x01
#define SDP_FLAGS_HIS_CFG_DONE      0x02
#define SDP_FLAGS_MY_CFG_DONE       0x04

#define SDP_DISC_WAIT_CONN          0
#define SDP_DISC_WAIT_HANDLES       1
#define SDP_DISC_WAIT_ATTR          2
#define SDP_DISC_WAIT_SEARCH_ATTR   3
#define SDP_DISC_WAIT_PASS_THRU     4
#define SDP_DISC_WAIT_CANCEL        5

typedef struct
{
    UINT8             con_state;
    UINT8             con_flags;
    BD_ADDR           device_address;
    TIMER_LIST_ENT    timer_entry;
    UINT16            rem_mtu_size;
    UINT16            connection_id;
    UINT16            list_len;
    UINT8             rsp_list[1000];
    tSDP_DISCOVERY_DB *p_db;
    tSDP_DISC_CMPL_CB *p_cb;
    UINT32            handles[SDP_MAX_DISC_SERVER_RECS];
    UINT16            num_handles;
    UINT16            cur_handle;
    UINT16            transaction_id;
    UINT16            disconnect_reason;
    UINT8             disc_state;
    UINT8             is_attr_search;
    UINT16            cont_offset;
} tCONN_CB;

typedef struct
{
    UINT32         di_primary_handle;       /* Device ID Primary record or NULL if nonexistent */
    BOOLEAN        brcm_di_registered;
    UINT16         num_records;
    tSDP_RECORD    record[SDP_MAX_RECORDS];
} tSDP_DB;

typedef struct
{
    tL2CAP_CFG_INFO   l2cap_my_cfg;             /* My L2CAP config     */
    tCONN_CB          ccb[SDP_MAX_CONNECTIONS];
    tSDP_DB           server_db;
    tL2CAP_APPL_INFO  reg_info;                 /* L2CAP Registration info */
    UINT16            max_attr_list_size;       /* Max attribute list size to use   */
    UINT16            max_recs_per_search;      /* Max records we want per seaarch  */
    UINT8             trace_level;
} tSDP_CB;

typedef struct
{
    UINT16     len;
    UINT8      value[MAX_UUID_SIZE];
} tUID_ENT;

typedef struct
{
    UINT16      num_uids;
    tUID_ENT    uuid_entry[MAX_UUIDS_PER_SEQ];
} tSDP_UUID_SEQ;

typedef struct
{
    UINT16      start;
    UINT16      end;
} tATT_ENT;

typedef struct
{
    UINT16      num_attr;
    tATT_ENT    attr_entry[MAX_ATTR_PER_SEQ];
} tSDP_ATTR_SEQ;

/*******************************************************************************
 * external globals
 */

extern tSDP_CB sdp_cb;

/*******************************************************************************
 * functions
 */

tSDP_RECORD *sdp_db_service_search(tSDP_RECORD *p_rec, tSDP_UUID_SEQ *p_seq);
/**/
tSDP_RECORD *sdp_db_find_record(UINT32 handle);
tSDP_ATTRIBUTE *sdp_db_find_attr_in_rec(tSDP_RECORD *p_rec, UINT16 start_attr,
                                        UINT16 end_attr);

void sdp_disc_connected(tCONN_CB *p_ccb);
void sdp_disc_server_rsp(tCONN_CB *p_ccb, BT_HDR *p_msg);

void sdp_init(void);
UINT16 sdp_set_max_attr_list_size(UINT16 max_size);
/**/
tCONN_CB *sdp_conn_originate(UINT8 *p_bd_addr);
void sdp_disconnect(tCONN_CB *p_ccb, UINT16 reason);
/**/
void sdp_conn_timeout(tCONN_CB *p_ccb);

void sdp_server_handle_client_req(tCONN_CB *p_ccb, BT_HDR *p_msg);

tCONN_CB *sdpu_find_ccb_by_cid(UINT16 cid);
tCONN_CB *sdpu_allocate_ccb(void);
void sdpu_release_ccb(tCONN_CB *p_ccb);
UINT8 *sdpu_build_attrib_seq(UINT8 *p_out, UINT16 *p_attr, UINT16 num_attrs);
UINT8 *sdpu_build_attrib_entry(UINT8 *p_out, tSDP_ATTRIBUTE *p_attr);
void sdpu_build_n_send_error(tCONN_CB *p_ccb, UINT16 trans_num,
                             UINT16 error_code, char *p_error_text);
UINT8 *sdpu_extract_uid_seq(UINT8 *p, UINT16 param_len, tSDP_UUID_SEQ *p_seq);
UINT8 *sdpu_extract_attr_seq(UINT8 *p, UINT16 param_len, tSDP_ATTR_SEQ *p_seq);
UINT8 *sdpu_get_len_from_type(UINT8 *p, UINT8 type, UINT32 *p_len);
BOOLEAN sdpu_is_base_uuid(UINT8 *p_uuid);
BOOLEAN sdpu_compare_uuid_arrays(UINT8 *p_uuid1, UINT32 len1, UINT8 *p_uuid2,
                                 UINT16 len2);
BOOLEAN sdpu_compare_bt_uuids(tBT_UUID *p_uuid1, tBT_UUID *p_uuid2);
BOOLEAN sdpu_compare_uuid_with_attr(tBT_UUID *p_btuuid, tSDP_DISC_ATTR *p_attr);
void sdpu_sort_attr_list(UINT16 num_attr, tSDP_DISCOVERY_DB *p_db);

#ifdef __cplusplus
	}
#endif

#endif // BTE_SDP_INT_H
