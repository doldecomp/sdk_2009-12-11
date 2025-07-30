#ifndef BTE_HIDD_INT_H
#define BTE_HIDD_INT_H

/*******************************************************************************
 * headers
 */

#include "bt_types.h"
#include "data_types.h"

#include "gki.h"
#include "hidd_api.h"
#include "hid_conn.h"
#include "l2c_api.h"

/*******************************************************************************
 * macros
 */

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef tHID_STATUS tHID_DEV_PROC_EVT(UINT8 event, void *p_data);

typedef struct hid_control_block {
    // total size: 0x144
    BD_ADDR host_addr; // offset 0x0, size 0x6
    BOOLEAN host_known; // offset 0x6, size 0x1
    UINT8 virtual_cable; // offset 0x7, size 0x1
    UINT8 dev_state; // offset 0x8, size 0x1
    UINT8 conn_tries; // offset 0x9, size 0x1
    UINT8 sec_mask; // offset 0xA, size 0x1
    UINT16 get_rep_buf_sz; // offset 0xC, size 0x2
    tHID_CONN conn; // offset 0x10, size 0x24
    TIMER_LIST_ENT idle_tle; // offset 0x34, size 0x18
    tHID_DEV_PWR_MD pm_params[3]; // offset 0x4C, size 0x1E
    tHID_DEV_CURR_PM_SETTING curr_pm; // offset 0x6A, size 0x4
    BOOLEAN pm_ctrl_busy; // offset 0x6E, size 0x1
    UINT8 conn_substate; // offset 0x6F, size 0x1
    tHID_DEV_PWR_MD final_pm; // offset 0x70, size 0xA
    BOOLEAN use_qos_flg; // offset 0x7A, size 0x1
    BOOLEAN unplug_on; // offset 0x7B, size 0x1
    tQOS_INFO qos_info; // offset 0x7C, size 0x48
    tHID_DEV_REG_INFO_CBACK * callback; // offset 0xC4, size 0x4
    tL2CAP_CFG_INFO l2cap_ctrl_cfg; // offset 0xC8, size 0x3C
    tL2CAP_CFG_INFO l2cap_int_cfg; // offset 0x104, size 0x3C
    BOOLEAN reg_flag; // offset 0x140, size 0x1
    UINT8 trace_level; // offset 0x141, size 0x1
} tHID_DEV_CTB;

/*******************************************************************************
 * external globals
 */

extern tHID_DEV_CTB hd_cb;
extern tHID_DEV_PROC_EVT * const hidd_sm_proc_evt[];

/*******************************************************************************
 * functions
 */

tHID_STATUS hidd_conn_reg(void);
void hidd_conn_dereg(void);
void hidd_conn_disconnect(void);
tHID_STATUS hidd_conn_initiate(void);
void hidd_sec_check_complete_term(BD_ADDR bd_addr, void *p_ref_data, UINT8 res);
void hidd_sec_check_complete_orig(BD_ADDR bd_addr, void *p_ref_data, UINT8 res);
void hidd_l2c_connected(tHID_CONN *p_hcon);
tHID_STATUS hidd_conn_snd_data(tSND_DATA_PARAMS *p_data);

tHID_STATUS hidd_mgmt_process_evt(UINT8 event, void *p_data);
void hidd_proc_repage_timeout(void);
void hidd_mgmt_conn_closed(UINT16 reason);

UINT8 hidd_pm_set_power_mode(tHID_DEV_PWR_MD *p_req_mode);
UINT8 hidd_pm_stop(void);
void hidd_pm_init(void);
UINT8 hidd_pm_start(void);
UINT8 hidd_pm_suspend_evt(void);

#ifdef __cplusplus
	}
#endif

#endif // BTE_HIDD_INT_H
