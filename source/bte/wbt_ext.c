#include "wbt_ext.h"

/*******************************************************************************
 * headers
 */

#include <string.h>

#include <macros.h>

#include "data_types.h"

#include "sdp_api.h"

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
		p_set_info->vendor = 15;
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
