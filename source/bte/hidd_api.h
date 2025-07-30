#ifndef BTE_HIDD_API_H
#define BTE_HIDD_API_H

/*******************************************************************************
 * headers
 */

#include "bt_types.h"
#include "data_types.h"

#include "hiddefs.h"

/*******************************************************************************
 * macros
 */

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef struct rep_data
{
    // total size: 0x2
    UINT8 rep_type; // offset 0x0, size 0x1
    UINT8 rep_id; // offset 0x1, size 0x1
} tHID_DEV_REP_DATA;

typedef struct snd_data_params
{
    // total size: 0x8
    UINT8 ctrl_ch; // offset 0x0, size 0x1
    UINT8 trans_type; // offset 0x1, size 0x1
    UINT8 param; // offset 0x2, size 0x1
    BT_HDR *buf; // offset 0x4, size 0x4
} tSND_DATA_PARAMS;

typedef struct pwr_md
{
    // total size: 0xA
    UINT16 max; // offset 0x0, size 0x2
    UINT16 min; // offset 0x2, size 0x2
    UINT16 attempt; // offset 0x4, size 0x2
    UINT16 timeout; // offset 0x6, size 0x2
    UINT8 mode; // offset 0x8, size 0x1
} tHID_DEV_PWR_MD;

typedef struct curr_pm_setting
{
    // total size: 0x4
    UINT8 mode; // offset 0x0, size 0x1
    UINT16 interval; // offset 0x2, size 0x2
} tHID_DEV_CURR_PM_SETTING;

typedef struct
{
    // total size: 0x48
    FLOW_SPEC ctrl_ch; // offset 0x0, size 0x18
    FLOW_SPEC int_ch; // offset 0x18, size 0x18
    FLOW_SPEC hci; // offset 0x30, size 0x18
} tQOS_INFO;

typedef union
{
    UINT8 host_bdaddr[6]; // offset 0x0, size 0x6
    BT_HDR * buffer; // offset 0x0, size 0x4
    tHID_DEV_REP_DATA get_rep; // offset 0x0, size 0x2
    UINT8 pm_err_code; // offset 0x0, size 0x1
    UINT16 pm_interval; // offset 0x0, size 0x2
} tHID_DEV_REG_INFO_CBACK_DATA;

typedef void tHID_DEV_REG_INFO_CBACK(UINT8, UINT32,
                                     tHID_DEV_REG_INFO_CBACK_DATA *);

typedef struct
{
    // total size: 0x10
    UINT8 host_addr[6]; // offset 0x0, size 0x6
    tQOS_INFO * qos_info; // offset 0x8, size 0x4
    tHID_DEV_REG_INFO_CBACK *app_cback; // offset 0xC, size 0x4
} tHID_DEV_REG_INFO;

/*******************************************************************************
 * external globals
 */

/*******************************************************************************
 * functions
 */

UINT32 HID_DevSetSDPRecord(tHID_DEV_SDP_INFO *p_sdp_info);
void HID_DevInit(void);
tHID_STATUS HID_DevRegister(tHID_DEV_REG_INFO *p_reg_info);
tHID_STATUS HID_DevDeregister(void);
tHID_STATUS HID_DevConnect(void);
tHID_STATUS HID_DevDisconnect(void);
tHID_STATUS HID_DevHandShake(UINT8 res_code);
tHID_STATUS HID_DevVirtualUnplug(void);
tHID_STATUS HID_DevSendData(UINT8 control_ch, UINT8 rep_type, BT_HDR *data_buf);
tHID_STATUS HID_DevSetSecurityLevel(char *serv_name, UINT8 sec_lvl);

#if defined(__MWERKS__)
# pragma cplusplus on
# define REF	&
extern "C"
#else
# define REF	*
#endif

// What the actual fuck lol
tHID_STATUS HID_DevSetPowerMgmtParams(UINT8 conn_substate,
                                      tHID_DEV_PWR_MD REF pm_params);

#undef REF

#if defined(__MWERKS__)
# pragma cplusplus reset
#endif

#ifdef __cplusplus
	}
#endif

#endif // BTE_HIDD_API_H
