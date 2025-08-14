#ifndef BTE_BTA_API_H
#define BTE_BTA_API_H

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * bta/include/bta_api.h
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

#define BTA_SUCCESS             0           /* Successful operation. */
#define BTA_FAILURE             1           /* Generic failure. */

#define BTA_DM_INQ_RES_EVT              0       /* Inquiry result for a peer device. */
#define BTA_DM_INQ_CMPL_EVT             1       /* Inquiry complete. */
#define BTA_DM_DISC_RES_EVT             2       /* Discovery result for a peer device. */
#define BTA_DM_DISC_CMPL_EVT            3       /* Discovery complete. */
#define BTA_DM_SEARCH_CANCEL_CMPL_EVT   4       /* Search cancelled */

#define BTA_DM_DISABLE_EVT              1       /* Disable Event */
#define BTA_DM_PIN_REQ_EVT              2       /* PIN request. */
#define BTA_DM_AUTH_CMPL_EVT            3       /* Authentication complete indication. */
#define BTA_DM_AUTHORIZE_EVT            4       /* Authorization request. */
#define BTA_DM_LINK_UP_EVT              5       /* Connection UP event */
#define BTA_DM_LINK_DOWN_EVT            6       /* Connection DOWN event */
#define BTA_DM_SIG_STRENGTH_EVT         7       /* Signal strength for bluetooth connection */

#define BTA_DM_AUTH_PERM        0      /* Authorized for future connections to the service */
#define BTA_DM_NOT_AUTH         2      /* Not authorized for the service */

#define BTA_MAX_SERVICE_ID      23

#define BTA_SIG_STRENGTH_RSSI_MASK 1
#define BTA_SIG_STRENGTH_LINK_QUALITY_MASK 2

#define BTA_ANY_ROLE          0x00

#define BTA_MASTER_ROLE_ONLY 2

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef UINT8 tBTA_STATUS;

typedef UINT32 tBTA_SERVICE_MASK;
typedef UINT8 tBTA_DM_INQ_MODE;
typedef UINT8 tBTA_DM_INQ_FILT;
typedef UINT8 tBTA_SERVICE_ID;
typedef UINT8 tBTA_SIG_STRENGTH_MASK;
typedef INT8 tBTA_DM_RSSI_VALUE;
typedef UINT8 tBTA_DM_LINK_QUALITY_VALUE;
typedef UINT8 tBTA_DM_SEC_EVT;
typedef UINT8 tBTA_DM_DISC;
typedef UINT8 tBTA_DM_CONN;
typedef UINT8 tBTA_DM_SEARCH_EVT;
typedef UINT8 tBTA_AUTH_RESP;
typedef UINT8 tBTA_DM_PM_ACTION;

enum
{
    BTA_DM_NO_SCATTERNET,
    BTA_DM_PARTIAL_SCATTERNET,
};

typedef struct
{
    BD_ADDR         bd_addr;                /* BD address peer device. */
    DEV_CLASS       dev_class;              /* Device class of peer device. */
    INT8            rssi;                   /* The rssi value */
    BOOLEAN         remt_name_not_required; /* Application sets this flag if it already knows the name of the device */
                                            /* If the device name is known to application BTA skips the remote name request */
} tBTA_DM_INQ_RES;

typedef struct
{
    UINT8           num_resps;          /* Number of inquiry responses. */
} tBTA_DM_INQ_CMPL;

typedef struct
{
    BD_ADDR             bd_addr;        /* BD address peer device. */
    BD_NAME             bd_name;        /* Name of peer device. */
    tBTA_SERVICE_MASK   services;       /* Services found on peer device. */
    tBTA_STATUS         result;
} tBTA_DM_DISC_RES;

typedef struct
{
    DEV_CLASS       dev_class;        /* device class of interest */
    DEV_CLASS       dev_class_mask;   /* mask to determine the bits of device class of interest */
} tBTA_DM_COD_COND;

typedef union
{
    BD_ADDR              bd_addr;            /* BD address of  device to filter. */
    tBTA_DM_COD_COND     dev_class_cond;     /* Device class filter condition */
} tBTA_DM_INQ_COND;

typedef struct
{
    tBTA_DM_INQ_MODE    mode;           /* Inquiry mode, limited or general. */
    UINT8               duration;       /* Inquiry duration in 1.28 sec units. */
    UINT8               max_resps;      /* Maximum inquiry responses.  Set to zero for unlimited responses. */
    tBTA_DM_INQ_FILT    filter_type;    /* Filter condition type. */
    tBTA_DM_INQ_COND    filter_cond;    /* Filter condition data. */
} tBTA_DM_INQ;

typedef union
{
    tBTA_DM_INQ_RES     inq_res;        /* Inquiry result for a peer device. */
    tBTA_DM_INQ_CMPL    inq_cmpl;       /* Inquiry complete. */
    tBTA_DM_DISC_RES    disc_res;       /* Discovery result for a peer device. */
} tBTA_DM_SEARCH;

typedef void tBTA_DM_SEARCH_CBACK(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH *p_data);

typedef struct
{
    BD_ADDR         bd_addr;            /* BD address of local device. */
} tBTA_DM_ENABLE;

typedef struct
{
    BD_ADDR         bd_addr;            /* BD address peer device. */
    BD_NAME         bd_name;            /* Name of peer device. */
    DEV_CLASS       dev_class;          /* Class of Device */
} tBTA_DM_PIN_REQ;

typedef struct
{
    BD_ADDR         bd_addr;            /* BD address peer device. */
    BD_NAME         bd_name;            /* Name of peer device. */
    BOOLEAN         key_present;        /* Valid link key value in key element */
    LINK_KEY        key;                /* Link key associated with peer device. */
    BOOLEAN         success;            /* TRUE of authentication succeeded, FALSE if failed. */
} tBTA_DM_AUTH_CMPL;

typedef struct
{
    BD_ADDR         bd_addr;            /* BD address peer device. */
    BD_NAME         bd_name;            /* Name of peer device. */
    tBTA_SERVICE_ID service;            /* Service ID to authorize. */
} tBTA_DM_AUTHORIZE;

typedef struct
{
    BD_ADDR         bd_addr;            /* BD address peer device. */
} tBTA_DM_LINK_UP;

typedef struct
{
    BD_ADDR         bd_addr;            /* BD address peer device. */
    UINT8           status;             /* connection open/closed */
} tBTA_DM_LINK_DOWN;

typedef struct
{
    BD_ADDR         bd_addr;            /* BD address peer device. */
    tBTA_SIG_STRENGTH_MASK mask;        /* mask for the values that are valid */
    tBTA_DM_RSSI_VALUE  rssi_value;
    tBTA_DM_LINK_QUALITY_VALUE link_quality_value;
} tBTA_DM_SIG_STRENGTH;

typedef struct
{
    UINT8           level;     /* when paging or inquiring, level is 10.
                                    Otherwise, the number of ACL links */
} tBTA_DM_BUSY_LEVEL;

typedef union
{
    tBTA_DM_ENABLE      enable;         /* BTA enabled */
    tBTA_DM_PIN_REQ     pin_req;        /* PIN request. */
    tBTA_DM_AUTH_CMPL   auth_cmpl;      /* Authentication complete indication. */
    tBTA_DM_AUTHORIZE   authorize;      /* Authorization request. */
    tBTA_DM_LINK_UP     link_up;       /* ACL connection down event */
    tBTA_DM_LINK_DOWN   link_down;       /* ACL connection down event */
    tBTA_DM_SIG_STRENGTH sig_strength;  /* rssi and link quality value */
    tBTA_DM_BUSY_LEVEL  busy_level;     /* System busy level */
} tBTA_DM_SEC;

typedef void tBTA_DM_SEC_CBACK(tBTA_DM_SEC_EVT event, tBTA_DM_SEC *p_data);

/*******************************************************************************
 * external globals
 */

/*******************************************************************************
 * functions
 */

#ifdef __cplusplus
	}
#endif

#endif // BTE_BTA_API_H
