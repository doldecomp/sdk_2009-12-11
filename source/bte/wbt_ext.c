#include "wbt_ext.h"

/*******************************************************************************
 * headers
 */

#include <string.h>

#include <macros.h>

#include "data_types.h"

/*******************************************************************************
 * macros
 */

// TODO: sdp_api.h

#define SDP_MAX_ATTR_LEN	80

/*******************************************************************************
 * types
 */

// TODO: sdp_api.h

typedef UINT16 tSDP_STATUS;
#define SDP_SUCCESS	0


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

/*******************************************************************************
 * local function declarations
 */

// TODO: sdp_api.h

extern tSDP_STATUS SDP_SetLocalDiRecord(tSDP_DI_RECORD *p_device_info,
                                        UINT32 *p_handle);
extern tSDP_STATUS SDP_GetLocalDiRecord(tSDP_DI_GET_RECORD *p_device_info,
                                        UINT32 *p_handle);
extern BOOLEAN SDP_AddUuidSequence(UINT32 handle, UINT16 attr_id,
                                 UINT16 num_uuids, UINT16 *p_uuids);
extern BOOLEAN SDP_AddAttribute(UINT32 handle, UINT16 attr_id, UINT8 attr_type,
                              UINT32 attr_len, UINT8 *p_val);

/*******************************************************************************
 * functions
 */

BOOLEAN WBT_ExtCreateRecord(void)
{
	tSDP_DI_GET_RECORD get_device_info;
	UINT32 sdp_record_handle = 0;
	tSDP_DI_RECORD *p_set_info;
	UINT8 temp_u16[2];
	UINT16 list = 0x1002;

	if (SDP_GetLocalDiRecord(&get_device_info, &sdp_record_handle)
	    != SDP_SUCCESS)
	{
		p_set_info = &get_device_info.rec;

		memset(p_set_info, 0, sizeof *p_set_info);
		p_set_info->vendor = 14;
		p_set_info->vendor_id_source = 1;
		p_set_info->primary_record = TRUE;

		if (SDP_SetLocalDiRecord(p_set_info, &sdp_record_handle) != SDP_SUCCESS)
		{
			sdp_record_handle = 0;
			return FALSE;
		}
	}

	SDP_AddUuidSequence(sdp_record_handle, 5, 1, &list);

	temp_u16[0] = 0;
	temp_u16[1] = 1;
	SDP_AddAttribute(sdp_record_handle, 0x8001, 1, ARRAY_LENGTH(temp_u16),
	                 temp_u16);

	return TRUE;
}

BOOLEAN WBT_ExtAddPinCode(void)
{
	UINT32 sdp_record_handle = 0;
	UINT8 temp_u32[4];
	tSDP_DI_GET_RECORD get_device_info;

	if (SDP_GetLocalDiRecord(&get_device_info, &sdp_record_handle)
	    == SDP_SUCCESS)
	{
		temp_u32[0] = 0;
		temp_u32[1] = 0;
		temp_u32[2] = 0;
		temp_u32[3] = 0;
		SDP_AddAttribute(sdp_record_handle, 0x8002, 1, ARRAY_LENGTH(temp_u32),
		                 temp_u32);

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

UINT32 wbt_sdp_show_ext(int param_1 ATTR_UNUSED, char *service_name)
{
	tSDP_DI_RECORD device_info;
	UINT32 sdp_record_handle;

	device_info.client_executable_url[0] = '\0';
	device_info.documentation_url[0] = '\0';
	strcpy(device_info.service_description, service_name);
	device_info.vendor = 15;
	device_info.vendor_id_source = 1;
	device_info.product = 0x1234;
	device_info.version = 1;
	device_info.primary_record = TRUE;

	if (SDP_SetLocalDiRecord(&device_info, &sdp_record_handle) != SDP_SUCCESS)
		sdp_record_handle = 0;

	return sdp_record_handle;
}
