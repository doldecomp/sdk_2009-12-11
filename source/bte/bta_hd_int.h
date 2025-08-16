#ifndef BTE_BTA_HD_INT_H
#define BTE_BTA_HD_INT_H

/*******************************************************************************
 * headers
 */

#include "bt_types.h"
#include "data_types.h"

#include "bta_hd_api.h"
#include "gki.h"
#include "hidd_api.h"
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

typedef struct
{
	BT_HDR			hdr;				// size 0x08, offset 0x00
	char			service_name[36];	// size 0x24, offset 0x08
	tBTA_HD_CBACK	*p_cback;			// size 0x04, offset 0x2c
	BD_ADDR			bd_addr;			// size 0x06, offset 0x30
	UINT8			sec_mask;			// size 0x01, offset 0x36
	UINT8			app_id;				// size 0x01, offset 0x37
} tBTA_HD_API_ENABLE; // size 0x38

typedef struct
{
	BT_HDR	hdr;		// size 0x08, offset 0x00
	UINT8	sec_mask;	// size 0x01, offset 0x08
} tBTA_HD_API_OPEN; // size 0x0a

typedef struct
{
	BT_HDR	hdr;		// size 0x08, offset 0x00
	UINT8	rid;		// size 0x01, offset 0x08
	UINT8	keyCode;	// size 0x01, offset 0x09
	UINT8	keyCode2;	// size 0x01, offset 0x0a
	UINT8	wheel;		// size 0x01, offset 0x0b
	UINT8	buttons;	// size 0x01, offset 0x0c
} tBTA_HD_API_INPUT; // size 0x0e

typedef struct
{
	BT_HDR	hdr;	// size 0x08, offset 0x00
	UINT8	rid;	// size 0x01, offset 0x08
	UINT8	len;	// size 0x01, offset 0x09
	UINT8	*seq;	// size 0x04, offset 0x0c
} tBTA_HD_API_SPEC; // size 0x10

typedef struct
{
	BT_HDR							hdr;	// size 0x08, offset 0x00
	UINT8							event;	// size 0x01, offset 0x08
	UINT32							data;	// size 0x04, offset 0x0c
	tHID_DEV_REG_INFO_CBACK_DATA	*pdata;	// size 0x04, offset 0x10
} tBTA_HD_CBACK_DATA; // size 0x14

typedef union
{
	tBTA_HD_API_ENABLE	api_enable;	// size 0x38
	tBTA_HD_API_OPEN	api_open;	// size 0x0a
	tBTA_HD_API_INPUT	api_input;	// size 0x0e
	tBTA_HD_API_SPEC	api_spec;	// size 0x10
	tBTA_HD_CBACK_DATA	cback_data;	// size 0x14
} tBTA_HD_MSG; // size 0x38

typedef struct
{
	BD_ADDR			peer_addr;	// size 0x06, offset 0x00
	UINT32			sdp_handle;	// size 0x04, offset 0x08
	tBTA_HD_CBACK	*p_cback;	// size 0x04, offset 0x0c
	BUFFER_Q		out_q;		// size 0x0c, offset 0x10
	UINT8			sec_mask;	// size 0x01, offset 0x1c
	UINT8			state;		// size 0x01, offset 0x1d
	UINT8			proto;		// size 0x01, offset 0x1e
	UINT8			app_id;		// size 0x01, offset 0x1f
} tBTA_HD_CB; // size 0x20

typedef struct
{
	tQOS_INFO			qos;		// size 0x48, offset 0x00
	tHID_DEV_SDP_INFO	sdp_info;	// size 0x74, offset 0x48
	BOOLEAN				use_qos;	// size 0x01, offset 0xbc
} tBTA_HD_CFG; // size 0xc0

/*******************************************************************************
 * external globals
 */

extern tBTA_HD_CB bta_hd_cb;
extern tBTA_HD_CFG *p_bta_hd_cfg;

/*******************************************************************************
 * functions
 */

void bta_hd_init_con_act(tBTA_HD_CB *p_cb, tBTA_HD_MSG *p_data);
void bta_hd_close_act(tBTA_HD_CB *p_cb, tBTA_HD_MSG *p_data);
void bta_hd_disable_act(tBTA_HD_CB *p_cb, tBTA_HD_MSG *p_data);
void bta_hd_open_act(tBTA_HD_CB *p_cb, tBTA_HD_MSG *p_data);
void bta_hd_opn_cb_act(tBTA_HD_CB *p_cb, tBTA_HD_MSG *p_data);
void bta_hd_input_act(tBTA_HD_CB *p_cb, tBTA_HD_MSG *p_data);
void bta_hd_discntd_act(tBTA_HD_CB *p_cb, tBTA_HD_MSG *p_data);
void bta_hd_discnt_act(tBTA_HD_CB *p_cb, tBTA_HD_MSG *p_data);
void bta_hd_hidd_cback(UINT8 event, UINT32 data,
                       tHID_DEV_REG_INFO_CBACK_DATA *pdata);

void bta_hd_sm_execute(tBTA_HD_CB *p_cb, UINT16 event, tBTA_HD_MSG *p_data);
BOOLEAN bta_hd_hdl_event(BT_HDR *p_msg);

#ifdef __cplusplus
	}
#endif

#endif // BTE_BTA_HD_INT_H
