#include "bta_dm_int.h"

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * bta/dm/bta_dm_main.c
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

#define BTA_DM_NUM_ACTIONS		2
#define BTA_DM_IGNORE			13

#define BTA_DM_SEARCH_NUM_COLS	3
#define BTA_DM_SEARCH_IGNORE	18
#define BTA_DM_SEARCH_ACTIONS	2

/*******************************************************************************
 * types
 */

typedef UINT8 tBTA_DM_ST[BTA_DM_SEARCH_NUM_COLS];

typedef void tBTA_DM_ACTION(tBTA_DM_MSG *p_data);

/*******************************************************************************
 * variables
 */

// .rodata
tBTA_DM_ACTION * const bta_dm_action[] =
{
	&bta_dm_enable,
	&bta_dm_disable,
	&bta_dm_set_dev_name,
	&bta_dm_set_visibility,
	&bta_dm_bond,
	&bta_dm_pin_reply,
	&bta_dm_auth_reply,
	&bta_dm_signal_strength,
	&bta_dm_acl_change,
	&bta_dm_pm_btm_status,
	&bta_dm_pm_timer,
	&bta_dm_keep_acl,
	&bta_dm_send_hci_reset,
};

tBTA_DM_ST const bta_dm_st_table[] =
{
	{ 0, 13, 0},
	{ 1, 13, 0},
	{ 2, 13, 0},
	{ 3, 13, 0},
	{ 7, 13, 0},
	{ 8, 13, 0},
	{ 4, 13, 0},
	{ 5, 13, 0},
	{ 6, 13, 0},
	{ 9, 13, 0},
	{10, 13, 0},
	{11, 13, 0},
	{12, 13, 0},
};

tBTA_DM_ACTION * const bta_dm_search_action[] =
{
	&bta_dm_search_start,
	&bta_dm_search_cancel,
	&bta_dm_discover,
	&bta_dm_inq_cmpl,
	&bta_dm_rmt_name,
	&bta_dm_sdp_result,
	&bta_dm_search_cmpl,
	&bta_dm_free_sdp_db,
	&bta_dm_disc_result,
	&bta_dm_search_result,
	&bta_dm_queue_search,
	&bta_dm_queue_disc,
	&bta_dm_search_clear_queue,
	&bta_dm_search_cancel_cmpl,
	&bta_dm_search_cancel_notify,
	&bta_dm_search_cancel_transac_cmpl,
	&bta_dm_disc_rmt_name,
	&bta_dm_cancel_rmt_name,
};

tBTA_DM_ST const bta_dm_search_idle_st_table[] =
{
	{ 0, 18, 1},
	{14, 18, 0},
	{ 2, 18, 3},
	{18, 18, 0},
	{18, 18, 0},
	{18, 18, 0},
	{ 7, 18, 0},
	{18, 18, 0},
	{18, 18, 0},
};

tBTA_DM_ST const bta_dm_search_search_active_st_table[] =
{
	{18, 18, 1},
	{ 1, 18, 2},
	{18, 18, 1},
	{ 3, 18, 1},
	{ 4, 18, 1},
	{17, 18, 1},
	{ 5, 18, 1},
	{ 6, 18, 0},
	{ 9, 18, 1},
};

tBTA_DM_ST const bta_dm_search_search_cancelling_st_table[] =
{
	{10, 18, 2},
	{12, 14, 2},
	{11, 18, 2},
	{13, 18, 0},
	{15, 13, 0},
	{18, 18, 2},
	{15, 13, 0},
	{13, 18, 0},
	{13, 18, 0},
};

tBTA_DM_ST const bta_dm_search_disc_active_st_table[] =
{
	{18, 18, 3},
	{14, 18, 3},
	{18, 18, 3},
	{18, 18, 3},
	{16, 18, 3},
	{17, 18, 3},
	{ 5, 18, 3},
	{18, 18, 0},
	{ 8, 18, 3},
};

tBTA_DM_ST const * const bta_dm_search_st_tbl[] =
{
	bta_dm_search_idle_st_table,
	bta_dm_search_search_active_st_table,
	bta_dm_search_search_cancelling_st_table,
	bta_dm_search_disc_active_st_table,
};

// .bss
tBTA_DM_SEARCH_CB bta_dm_search_cb;
tBTA_DM_CB bta_dm_cb;

/*******************************************************************************
 * functions
 */

BOOLEAN bta_dm_sm_execute(BT_HDR *p_msg)
{
	tBTA_DM_ST const *state_table = bta_dm_st_table;
	UINT8 action;
	int i;

	for (i = 0; i < BTA_DM_NUM_ACTIONS; ++i)
	{
		if ((action = state_table[p_msg->event & 0xff][i]) == BTA_DM_IGNORE)
			break;

		(*bta_dm_action[action])((tBTA_DM_MSG *)p_msg);
	}

	return TRUE;
}

BOOLEAN bta_dm_search_sm_execute(BT_HDR *p_msg)
{
	tBTA_DM_ST const *state_table =
		bta_dm_search_st_tbl[bta_dm_search_cb.state];
	UINT8 action;
	int i;

	bta_dm_search_cb.state = state_table[p_msg->event & 0xff][2];

	for (i = 0; i < BTA_DM_SEARCH_ACTIONS; ++i)
	{
		if ((action = state_table[p_msg->event & 0xff][i])
		    == BTA_DM_SEARCH_IGNORE)
		{
			break;
		}

		(*bta_dm_search_action[action])((tBTA_DM_MSG *)p_msg);
	}

	return TRUE;
}
