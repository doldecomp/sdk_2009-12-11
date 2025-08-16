#include "bta_hd_int.h"

/*******************************************************************************
 * headers
 */

#include <string.h>

#include "bt_trace.h"
#include "bt_types.h"
#include "data_types.h"

#include "bta_sys.h"
#include "bd.h"
#include "bta_hd_api.h"
#include "gki.h"
#include "hidd_api.h"
#include "hiddefs.h"

/*******************************************************************************
 * types
 */

typedef UINT8 tBTA_HD_ST[2];

typedef void tBTA_HD_ACTION(tBTA_HD_CB *p_cb, tBTA_HD_MSG *p_data);

/*******************************************************************************
 * local function declarations
 */

static void bta_hd_api_enable(tBTA_HD_MSG *p_data);

/*******************************************************************************
 * variables
 */

// .rodata
tBTA_HD_ACTION * const bta_hd_action[] =
{
	&bta_hd_init_con_act,
	&bta_hd_close_act,
	&bta_hd_disable_act,
	&bta_hd_open_act,
	&bta_hd_opn_cb_act,
	&bta_hd_input_act,
	&bta_hd_discntd_act,
	&bta_hd_discnt_act,
};

tBTA_HD_ST const bta_hd_st_listen[] =
{
	{0, 1},
	{1, 1},
	{2, 0},
	{8, 1},
	{8, 1},
	{3, 2},
	{6, 1},
};

tBTA_HD_ST const bta_hd_st_open[] =
{
	{8, 2},
	{1, 2},
	{2, 0},
	{5, 2},
	{4, 2},
	{8, 2},
	{7, 1},
};

// .bss
tBTA_HD_CB bta_hd_cb;

// .sdata2
tBTA_HD_ST const * const bta_hd_st_tbl[] =
{
	bta_hd_st_listen,
	bta_hd_st_open,
};

/*******************************************************************************
 * functions
 */

static void bta_hd_api_enable(tBTA_HD_MSG *p_data)
{
	tHID_DEV_SDP_INFO sdp_info;
	tHID_DEV_REG_INFO reg_info;
	UINT8 sec_mask = p_data->api_enable.sec_mask;
	UINT8 status = 1;

	memset(&bta_hd_cb, 0, sizeof bta_hd_cb);
	GKI_init_q(&bta_hd_cb.out_q);
	bta_hd_cb.p_cback = p_data->api_enable.p_cback;
	reg_info.app_cback = &bta_hd_hidd_cback;
	bdcpy(reg_info.host_addr, p_data->api_enable.bd_addr);

	if (p_bta_hd_cfg->use_qos)
		reg_info.qos_info = &p_bta_hd_cfg->qos;
	else
		reg_info.qos_info = NULL;

	if (HID_DevRegister(&reg_info) == HID_SUCCESS)
	{
		HID_DevSetSecurityLevel("BT HID Combo Mouse/Keyboard", sec_mask);
		memcpy(&sdp_info, &p_bta_hd_cfg->sdp_info, sizeof sdp_info);

		if (p_data->api_enable.service_name[0] != '\0')
		{
			BCM_STRNCPY_S(sdp_info.svc_name, HID_MAX_SVC_NAME_LEN,
			              p_data->api_enable.service_name, 36);
		}

		bta_hd_cb.sdp_handle = HID_DevSetSDPRecord(&sdp_info);
	}

	if (bta_hd_cb.sdp_handle != 0)
	{
		status = 0;
		bta_hd_cb.state = 1;
	}

	(*bta_hd_cb.p_cback)(0, (tBTA_HD *)&status);
}

void bta_hd_sm_execute(tBTA_HD_CB *p_cb, UINT16 event, tBTA_HD_MSG *p_data)
{
	tBTA_HD_ST const *state_table;
	UINT8 action;

	if (p_cb->state == 0)
	{
		APPL_TRACE(EVENT, "HD event=0x%x received in IDLE", event);
		return;
	}

	state_table = bta_hd_st_tbl[p_cb->state - 1];
	event &= 0xff;

	APPL_TRACE(EVENT, "HD event=0x%x state=%d, next: %d", event, p_cb->state,
	           state_table[event][1]);
	p_cb->state = state_table[event][1];

	action = state_table[event][0];
	if (action != 8)
		(*bta_hd_action[action])(p_cb, p_data);
}

BOOLEAN bta_hd_hdl_event(BT_HDR *p_msg)
{
	switch (p_msg->event)
	{
	case 0x1307:
		bta_hd_api_enable((tBTA_HD_MSG *)p_msg);
		break;

	default:
		bta_hd_sm_execute(&bta_hd_cb, p_msg->event, (tBTA_HD_MSG *)p_msg);
		break;
	}

	return TRUE;
}
