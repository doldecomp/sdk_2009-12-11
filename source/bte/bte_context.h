#pragma once

#include "bt_types.h"
#include "data_types.h"

// TODO: bta_dm_act.h

extern void bta_dm_immediate_disable(void);

// TODO: bta_sys.h

extern void bta_sys_sendmsg(void *p_msg);

// TODO: btm_api.h

typedef UINT8 tBTM_STATUS;
enum
{
    BTM_SUCCESS,                        /* 0  Command succeeded                 */
    BTM_CMD_STARTED,                    /* 1  Command started OK.               */
    BTM_BUSY,                           /* 2  Device busy with another command  */
    BTM_NO_RESOURCES,                   /* 3  No resources to issue command     */
    BTM_MODE_UNSUPPORTED,               /* 4  Request for 1 or more unsupported modes */
    BTM_ILLEGAL_VALUE,                  /* 5  Illegal parameter value           */
    BTM_WRONG_MODE,                     /* 6  Device in wrong mode for request  */
    BTM_UNKNOWN_ADDR,                   /* 7  Unknown remote BD address         */
    BTM_DEVICE_TIMEOUT,                 /* 8  Device timeout                    */
    BTM_BAD_VALUE_RET,                  /* 9  A bad value was received from HCI */
    BTM_ERR_PROCESSING,                 /* 10 Generic error                     */
    BTM_NOT_AUTHORIZED,                 /* 11 Authorization failed              */
    BTM_DEV_RESET,                      /* 12 Device has been reset             */
    BTM_CMD_STORED,                     /* 13 request is stored in control block */
    BTM_ILLEGAL_ACTION,                 /* 14 state machine gets illegal command */
    BTM_DELAY_CHECK,                    /* 15 delay the check on encryption     */
    BTM_SCO_BAD_LENGTH,                 /* 16 Bad SCO over HCI data length      */
    BTM_SUCCESS_NO_SECURITY,            /* 17 security passed, no security set  */
    BTM_FAILED_ON_SECURITY,             /* 18 security failed                   */
    BTM_REPEATED_ATTEMPTS,              /* 19 repeated attempts for LE security requests */
};

extern UINT8 *BTM_ReadDeviceClass(void);
extern tBTM_STATUS BTM_SetDeviceClass(DEV_CLASS_PTR dev_class);

/* class of device field macros */
#define BTM_COD_FORMAT_TYPE_MASK	0x03
#define BTM_COD_MINOR_CLASS_MASK	0xfc
#define BTM_COD_MAJOR_CLASS_MASK	0x1f
#define BTM_COD_SERVICE_CLASS_LO_B	0x00e0
#define BTM_COD_SERVICE_CLASS_MASK	0xffe0

#define BTM_SET_COD_FORMAT_TYPE(pd, fm)		\
	do { *(fm) = (pd)[2] & BTM_COD_FORMAT_TYPE_MASK; } while (FALSE)

#define BTM_SET_COD_MINOR_CLASS(pd, mn)		\
	do { *(mn) = (pd)[2] & BTM_COD_MINOR_CLASS_MASK; } while (FALSE)

#define BTM_SET_COD_MAJOR_CLASS(pd, mj)		\
	do { *(mj) = (pd)[1] & BTM_COD_MAJOR_CLASS_MASK; } while (FALSE)

#define BTM_SET_COD_SERVICE_CLASS(pd, sv)				\
	do													\
	{													\
		*(sv) = (pd)[0];								\
		*(sv) <<= 8;									\
		*(sv) += (pd)[1] & BTM_COD_SERVICE_CLASS_LO_B;	\
	} while (FALSE)

/* to set the fields (assumes that format type is always 0) */
#define MAKE_COD_FROM_FIELDS(pd, mn, mj, sv)					\
	do															\
	{															\
		(pd)[2] = (mn);											\
		(pd)[1] = (mj) + ((sv) & BTM_COD_SERVICE_CLASS_LO_B);	\
		(pd)[0] = (sv) >> 8;									\
	} while (FALSE)


// TODO: gki.h

typedef void TIMER_CBACK(void *p_tle);
#define TIMER_PARAM_TYPE	UINT32

typedef struct _tle
{
	struct _tle			*p_next;	// size 0x04, offset 0x00
	struct _tle			*p_prev;	// size 0x04, offset 0x04
	TIMER_CBACK			*p_cback;	// size 0x04, offset 0x08
	INT32				ticks;		// size 0x04, offset 0x0c
	TIMER_PARAM_TYPE	param;		// size 0x04, offset 0x10
	UINT16				event;		// size 0x02, offset 0x14
	UINT8				in_use;		// size 0x01, offset 0x16
	/* 1 byte padding */
} TIMER_LIST_ENT; // size 0x18

typedef struct
{
	TIMER_LIST_ENT	*p_first;	// size 0x04, offset 0x00
	TIMER_LIST_ENT	*p_last;	// size 0x04, offset 0x04
	INT32			last_ticks;	// size 0x04, offset 0x08
} TIMER_LIST_Q; // size 0x0c

extern UINT8 GKI_get_taskid(void);

extern void GKI_send_msg(UINT8, UINT8, void *);

extern void GKI_freebuf(void *);
extern void *GKI_getbuf(UINT16);

extern void GKI_add_to_timer_list(TIMER_LIST_Q *, TIMER_LIST_ENT *);
extern void GKI_init_timer_list(TIMER_LIST_Q *);
extern void GKI_remove_from_timer_list(TIMER_LIST_Q *, TIMER_LIST_ENT *);
extern void GKI_start_timer(UINT8, INT32, BOOLEAN);
extern void GKI_stop_timer(UINT8);
extern UINT16 GKI_update_timer_list(TIMER_LIST_Q *, INT32);

// TODO: gki_target.h

/* A conversion value for translating ticks to calculate GKI timer.  */
#ifndef TICKS_PER_SEC
#define TICKS_PER_SEC	100
#endif

#ifndef GKI_MS_TO_TICKS
#define GKI_MS_TO_TICKS(x)	((x) / (1000 / TICKS_PER_SEC))
#endif

// TODO: sdp_api.h

#define SDP_MAX_ATTR_LEN	80

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

extern tSDP_STATUS SDP_SetLocalDiRecord(tSDP_DI_RECORD *p_device_info,
                                        UINT32 *p_handle);
extern tSDP_STATUS SDP_GetLocalDiRecord(tSDP_DI_GET_RECORD *p_device_info,
                                        UINT32 *p_handle);
extern BOOLEAN SDP_AddUuidSequence(UINT32 handle, UINT16 attr_id,
                                 UINT16 num_uuids, UINT16 *p_uuids);
extern BOOLEAN SDP_AddAttribute(UINT32 handle, UINT16 attr_id, UINT8 attr_type,
                              UINT32 attr_len, UINT8 *p_val);
