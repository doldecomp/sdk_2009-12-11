#ifndef BTE_SDP_API_H
#define BTE_SDP_API_H

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/include/sdp_api.h
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

/*******************************************************************************
 * macros
 */

#define SDP_MAX_ATTR_LEN	80

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef UINT16 tSDP_STATUS;
enum
{
	SDP_SUCCESS,
};

typedef tBT_UUID tSDP_UUID;

typedef struct t_sdp_di_record
{
	UINT16	vendor;										// size 0x02, offset 0x00
	UINT16	vendor_id_source;							// size 0x02, offset 0x02
	UINT16	product;									// size 0x02, offset 0x04
	UINT16	version;									// size 0x02, offset 0x06
	BOOLEAN	primary_record;								// size 0x01, offset 0x08
	char	client_executable_url[SDP_MAX_ATTR_LEN];	// size 0x50, offset 0x09
	char	service_description[SDP_MAX_ATTR_LEN];		// size 0x50, offset 0x59
	char	documentation_url[SDP_MAX_ATTR_LEN];		// size 0x50, offset 0xa9
} tSDP_DI_RECORD; // size 0xfa

typedef struct t_sdp_di_get_record
{
	UINT16			spec_id;	// size 0x02, offset 0x00
	tSDP_DI_RECORD	rec;		// size 0xfa, offset 0x02
} tSDP_DI_GET_RECORD; // size 0xfc

typedef struct
{
	UINT16	protocol_uuid;						// size 0x02, offset 0x00
	UINT16	num_params;							// size 0x02, offset 0x02
	UINT16	params[SDP_MAX_PROTOCOL_PARAMS];	// size 0x04, offset 0x04
} tSDP_PROTOCOL_ELEM; // size 0x08

/*******************************************************************************
 * external globals
 */

/*******************************************************************************
 * functions
 */

tSDP_STATUS SDP_SetLocalDiRecord(tSDP_DI_RECORD *p_device_info,
                                 UINT32 *p_handle);
tSDP_STATUS SDP_GetLocalDiRecord(tSDP_DI_GET_RECORD *p_device_info,
                                 UINT32 *p_handle);
BOOLEAN SDP_AddUuidSequence(UINT32 handle, UINT16 attr_id, UINT16 num_uuids,
                            UINT16 *p_uuids);
BOOLEAN SDP_AddAttribute(UINT32 handle, UINT16 attr_id, UINT8 attr_type,
                         UINT32 attr_len, UINT8 *p_val);
UINT32 SDP_CreateRecord(void);
BOOLEAN SDP_AddServiceClassIdList(UINT32 handle, UINT16 num_services,
                                  UINT16 *p_service_uuids);
BOOLEAN SDP_AddProtocolList(UINT32 handle, UINT16 num_elem,
                            tSDP_PROTOCOL_ELEM *p_elem_list);
BOOLEAN SDP_AddProfileDescriptorList(UINT32 handle, UINT16 profile_uuid,
                                     UINT16 version);
BOOLEAN SDP_DeleteRecord(UINT32 handle);

#ifdef __cplusplus
	}
#endif

#endif // BTE_SDP_API_H
