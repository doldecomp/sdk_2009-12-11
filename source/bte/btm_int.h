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

#include "data_types.h"

#include "btm_api.h"
#include "hcidefs.h"

/*******************************************************************************
 * macros
 */

#define BTM_MIN_INQUIRY_LENGTH		1
#define BTM_MAX_INQUIRY_LENGTH		HCI_MAX_INQUIRY_LENGTH

#define BTM_PER_INQ_MIN_MIN_PERIOD	HCI_PER_INQ_MIN_MIN_PERIOD
#define BTM_PER_INQ_MAX_MIN_PERIOD	HCI_PER_INQ_MAX_MIN_PERIOD

#define BTM_PER_INQ_MIN_MAX_PERIOD	HCI_PER_INQ_MIN_MAX_PERIOD
#define BTM_PER_INQ_MAX_MAX_PERIOD	HCI_PER_INQ_MAX_MAX_PERIOD

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef tBTM_SEC_CBACK tBTM_SEC_CALLBACK;

/*******************************************************************************
 * external globals
 */

/*******************************************************************************
 * functions
 */

tBTM_STATUS btm_sec_mx_access_request(BD_ADDR bd_addr, UINT16 psm,
                                      BOOLEAN is_originator, UINT32 mx_proto_id,
                                      UINT32 mx_chan_id,
                                      tBTM_SEC_CALLBACK *p_callback,
                                      void *p_ref_data);
UINT16 btm_get_acl_disc_reason_code(void);

#ifdef __cplusplus
	}
#endif

#endif // BTE_BTM_INT_H
