#include "bta_prm_int.h"

/* Original source:
 * <repo>
 * <path>
 */

/*******************************************************************************
 * headers
 */

#include <string.h>

#include "bt_trace.h"
#include "bt_types.h"
#include "data_types.h"

#include "bta_fs_co.h"
#include "bta_sys.h"
#include "gki.h"

/*******************************************************************************
 * macros
 */

/*******************************************************************************
 * types
 */

typedef void BCM2045_PrmCallback(UINT8);

/*******************************************************************************
 * local function declarations
 */

static void bta_lower_level_prm_cback(UINT8 status);

extern int BCM2045_PrmInit(BCM2045_PrmCallback *, UINT8);
extern int BCM2045_PrmLaunchRam(void);
extern int BCM2045_PrmLoadData(UINT8 *, UINT16);

/*******************************************************************************
 * variables
 */

// .bss
tBTA_PRM_CB bta_prm_cb;

/*******************************************************************************
 * functions
 */

static void bta_lower_level_prm_cback(UINT8 status)
{
	BT_HDR *p_buf;
	UINT16 event;

	APPL_TRACE(EVENT, "bta_lower_level_prm_cback status 0x%x", status);

	if (status == 0)
		event = 0x1603;
	else if (status == 1)
		event = 0x1604;
	else if (status == 2)
		event = 0x1605;
	else
		return;

	if ((p_buf = GKI_getbuf(sizeof *p_buf)) != NULL)
	{
		p_buf->event = event;
		bta_sys_sendmsg(p_buf);
	}
}

UINT8 bta_prm_hdl_event(BT_HDR *p_msg)
{
	tBTA_PRM_DATA *p_prm_msg = (tBTA_PRM_DATA *)p_msg;
	tBTA_PRM_OPEN_EVT *p_open;
	tBTA_PRM_READ_EVT *p_revt;

	APPL_TRACE(EVENT, "PRM state  0x%x, event 0x%x", bta_prm_cb.state,
	           p_msg->event);

	switch (p_msg->event)
	{
	case 0x1600:
		memset(&bta_prm_cb, 0, sizeof bta_prm_cb);
		bta_prm_cb.app_id = p_prm_msg->start_evt.fs_app_id;
		bta_prm_cb.p_cback = p_prm_msg->start_evt.p_cback;

		if (!p_prm_msg->start_evt.p_name)
		{
			bta_prm_cb.internal_patch = TRUE;
			bta_prm_cb.state = 3;

			BCM2045_PrmInit(&bta_lower_level_prm_cback, 1);
		}
		else
		{
			bta_prm_cb.internal_patch = FALSE;
			bta_prm_cb.state = 1;

			bta_fs_co_open(p_prm_msg->start_evt.p_name, 0, 0, 0x1601,
			               bta_prm_cb.app_id);

			GKI_freebuf(p_prm_msg->start_evt.p_name);
		}

		break;

	case 0x1601:
		p_open = &p_prm_msg->open_evt;

		if (p_open->status == 0)
		{
			bta_prm_cb.state = 3;
			bta_prm_cb.file_length = p_open->file_size;
			bta_prm_cb.fd = p_open->fd;

			BCM2045_PrmInit(&bta_lower_level_prm_cback, 0);
		}
		else
		{
			if (bta_prm_cb.p_cback)
				(*bta_prm_cb.p_cback)(1);
		}

		break;

	case 0x1604:
		if (bta_prm_cb.p_cback)
			(*bta_prm_cb.p_cback)(0);

		memset(&bta_prm_cb, 0, sizeof bta_prm_cb);

		break;

	case 0x1603:
		memset(bta_prm_cb.patch_data, 0, sizeof bta_prm_cb.patch_data);

		bta_prm_cb.state = 2;

		if (bta_prm_cb.read_length == bta_prm_cb.file_length)
		{
			bta_prm_cb.state = 3;

			bta_fs_co_close(bta_prm_cb.fd, bta_prm_cb.app_id);
			bta_prm_cb.fd = 0;

			BCM2045_PrmLaunchRam();
		}
		else
		{
			bta_fs_co_read(bta_prm_cb.fd, bta_prm_cb.patch_data, 0xfa, 0x1602,
			               bta_prm_cb.app_id);
		}

		break;

	case 0x1602:
		p_revt = &p_prm_msg->read_evt;

		bta_prm_cb.read_length += p_revt->num_read;
		bta_prm_cb.state = 3;

		if (p_revt->num_read != 0)
		{
			BCM2045_PrmLoadData(bta_prm_cb.patch_data, p_revt->num_read);
		}
		else if (p_revt->num_read == 0 && p_revt->status == 4)
		{
			bta_fs_co_close(bta_prm_cb.fd, bta_prm_cb.app_id);
			bta_prm_cb.fd = 0;

			BCM2045_PrmLaunchRam();
		}
		else if (p_revt->status == 1)
		{
			bta_fs_co_close(bta_prm_cb.fd, bta_prm_cb.app_id);
			bta_prm_cb.fd = 0;

			if (bta_prm_cb.p_cback)
				(*bta_prm_cb.p_cback)(1);
		}

		break;

	case 0x1605:
		if (bta_prm_cb.fd != 0)
			bta_fs_co_close(bta_prm_cb.fd, bta_prm_cb.app_id);

		if (bta_prm_cb.p_cback)
			(*bta_prm_cb.p_cback)(1);

		memset(&bta_prm_cb, 0, sizeof bta_prm_cb);

		break;

	}

	return 1;
}
