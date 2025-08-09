#ifndef BTE_RFCOMM_PORT_API_H
#define BTE_RFCOMM_PORT_API_H

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/include/port_api.h
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

#include "data_types.h"
#include "bt_types.h"

/*******************************************************************************
 * macros
 */

#define PORT_ERR_BASE				0

#define PORT_SUCCESS				(PORT_ERR_BASE + 0)
#define PORT_UNKNOWN_ERROR			(PORT_ERR_BASE + 1)
#define PORT_ALREADY_OPENED			(PORT_ERR_BASE + 2)
#define PORT_CMD_PENDING			(PORT_ERR_BASE + 3)
#define PORT_NO_MEM					(PORT_ERR_BASE + 5)
#define PORT_NO_RESOURCES			(PORT_ERR_BASE + 6)
#define PORT_BAD_HANDLE				(PORT_ERR_BASE + 9)
#define PORT_NOT_OPENED				(PORT_ERR_BASE + 10)
#define PORT_LINE_ERR				(PORT_ERR_BASE + 11)
#define PORT_START_FAILED			(PORT_ERR_BASE + 12)
#define PORT_PORT_NEG_FAILED		(PORT_ERR_BASE + 14)
#define PORT_PEER_CONNECTION_FAILED	(PORT_ERR_BASE + 16)
#define PORT_PEER_TIMEOUT			(PORT_ERR_BASE + 18)
#define PORT_CLOSED					(PORT_ERR_BASE + 19)
#define PORT_TX_FULL				(PORT_ERR_BASE + 20)
#define PORT_TX_QUEUE_DISABLED		(PORT_ERR_BASE + 23)
#define PORT_PAGE_TIMEOUT			(PORT_ERR_BASE + 24)
#define PORT_INVALID_SCN			(PORT_ERR_BASE + 25)

#define PORT_SET_CTSRTS         0x03
#define PORT_CLR_CTSRTS         0x04
#define PORT_SET_DTRDSR         0x01
#define PORT_CLR_DTRDSR         0x02
#define PORT_SET_RI             0x05        /* DCE only */
#define PORT_CLR_RI             0x06        /* DCE only */
#define PORT_SET_DCD            0x07        /* DCE only */
#define PORT_CLR_DCD            0x08        /* DCE only */
#define PORT_BREAK              0x09        /* Break event */

#define PORT_DTRDSR_ON		0x01
#define PORT_CTSRTS_ON		0x02
#define PORT_RING_ON		0x04
#define PORT_DCD_ON			0x08

#define PORT_OBEX_DEFAULT_SIGNAL_STATE	(PORT_DTRDSR_ON | PORT_CTSRTS_ON | PORT_DCD_ON)
#define PORT_SPP_DEFAULT_SIGNAL_STATE	(PORT_DTRDSR_ON | PORT_CTSRTS_ON | PORT_DCD_ON)
#define PORT_PPP_DEFAULT_SIGNAL_STATE	(PORT_DTRDSR_ON | PORT_CTSRTS_ON | PORT_DCD_ON)
#define PORT_DUN_DEFAULT_SIGNAL_STATE	(PORT_DTRDSR_ON | PORT_CTSRTS_ON)

#define PORT_EV_RXCHAR      0x00000001  /* Any Character received */
#define PORT_EV_RXFLAG      0x00000002  /* Received certain character */
#define PORT_EV_TXEMPTY     0x00000004  /* Transmitt Queue Empty */
#define PORT_EV_CTS         0x00000008  /* CTS changed state */
#define PORT_EV_DSR         0x00000010  /* DSR changed state */
#define PORT_EV_RLSD        0x00000020  /* RLSD changed state */
#define PORT_EV_BREAK       0x00000040  /* BREAK received */
#define PORT_EV_ERR         0x00000080  /* Line status error occurred */
#define PORT_EV_RING        0x00000100  /* Ring signal detected */
#define PORT_EV_CONNECTED   0x00000200  /* RFCOMM connection established */
#define PORT_EV_CTSS        0x00000400  /* CTS state */
#define PORT_EV_DSRS        0x00000800  /* DSR state */
#define PORT_EV_RLSDS       0x00001000  /* RLSD state */
#define PORT_EV_OVERRUN     0x00002000  /* receiver buffer overrun */
#define PORT_EV_TXCHAR      0x00004000  /* Any character transmitted */
#define PORT_EV_CONNECT_ERR 0x00008000  /* Was not able to establish connection */
#define PORT_EV_FC          0x00010000  /* data flow enabled flag changed by remote */
#define PORT_EV_FCS         0x00020000  /* data flow enable status true = enabled */

#define PORT_PURGE_RXCLEAR  0x02
#define PORT_PURGE_TXCLEAR  0x01

#define PORT_ERR_BREAK      0x01    /* Break condition occured on the peer device */
#define PORT_ERR_OVERRUN    0x02    /* Overrun is reported by peer device */

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef void tPORT_CALLBACK(UINT32 code, UINT16 port_handle);
typedef int tPORT_DATA_CALLBACK(UINT16 port_handle, void *p_data, UINT16 len);

enum
{
	PORT_BAUD_RATE_2400,
	PORT_BAUD_RATE_4800,
	PORT_BAUD_RATE_7200,
	PORT_BAUD_RATE_9600,
	PORT_BAUD_RATE_19200,
	PORT_BAUD_RATE_38400,
	PORT_BAUD_RATE_57600,
	PORT_BAUD_RATE_115200,
	PORT_BAUD_RATE_230400,
};

enum
{
	PORT_5_BITS,
	PORT_6_BITS,
	PORT_7_BITS,
	PORT_8_BITS,
};

enum
{
	PORT_ONESTOPBIT,
	PORT_ONE5STOPBITS,
};

enum
{
	PORT_PARITY_NO,
	PORT_PARITY_YES,
};

enum
{
	PORT_ODD_PARITY,
	PORT_EVEN_PARITY,
	PORT_MARK_PARITY,
	PORT_SPACE_PARITY,
};

enum
{
	PORT_FC_OFF					= 0,

	PORT_FC_XONXOFF_ON_INPUT	= 1 << 0,
	PORT_FC_XONXOFF_ON_OUTPUT	= 1 << 1,
	PORT_FC_CTS_ON_INPUT		= 1 << 2,
	PORT_FC_CTS_ON_OUTPUT		= 1 << 3,
	PORT_FC_DSR_ON_INPUT		= 1 << 4,
	PORT_FC_DSR_ON_OUTPUT		= 1 << 5,
};

#define PORT_XON_DC1	0x11
#define PORT_XOFF_DC3	0x13

typedef struct
{
	UINT8	baud_rate;		// size 0x01, offset 0x00
	UINT8	byte_size;		// size 0x01, offset 0x01
	UINT8	stop_bits;		// size 0x01, offset 0x02
	UINT8	parity;			// size 0x01, offset 0x03
	UINT8	parity_type;	// size 0x01, offset 0x04
	UINT8	fc_type;		// size 0x01, offset 0x05
	UINT8	rx_char1;		// size 0x01, offset 0x06
	UINT8	xon_char;		// size 0x01, offset 0x07
	UINT8	xoff_char;		// size 0x01, offset 0x08
} tPORT_STATE; // size 0x09

enum
{
	PORT_FLAG_CTS_HOLD	= 1 << 0,
	PORT_FLAG_DSR_HOLD	= 1 << 1,
	PORT_FLAG_RLSD_HOLD	= 1 << 2,
};

typedef struct
{
	UINT16	flags;			// size 0x02, offset 0x00
	UINT16	in_queue_size;	// size 0x02, offset 0x02
	UINT16	out_queue_size;	// size 0x02, offset 0x04
	UINT16	mtu_size;		// size 0x02, offset 0x06
} tPORT_STATUS; // size 0x08

/*******************************************************************************
 * external globals
 */

/*******************************************************************************
 * functions
 */

void RFCOMM_Init(void);
int RFCOMM_CreateConnection(UINT16 uuid, UINT8 scn, BOOLEAN is_server,
                            UINT16 mtu, BD_ADDR bd_addr, UINT16 *p_handle,
                            tPORT_CALLBACK *p_mgmt_cb);
int RFCOMM_RemoveConnection(UINT16 handle);

int PORT_SetEventCallback(UINT16 port_handle, tPORT_CALLBACK *p_port_cb);
int PORT_SetDataCallback(UINT16 port_handle, tPORT_DATA_CALLBACK *p_port_cb);
int PORT_SetEventMask(UINT16 port_handle, UINT32 mask);
int PORT_CheckConnection(UINT16 handle, BD_ADDR bd_addr, UINT16 *p_lcid);
int PORT_SetState(UINT16 handle, tPORT_STATE *p_settings);
int PORT_GetState(UINT16 handle, tPORT_STATE *p_settings);
int PORT_Control(UINT16 handle, UINT8 signal);
int PORT_FlowControl(UINT16 handle, BOOLEAN enable);
int PORT_GetModemStatus(UINT16 handle, UINT8 *p_signal);
int PORT_ClearError(UINT16 handle, UINT16 *p_errors, tPORT_STATUS *p_status);
int PORT_SendError(UINT16 handle, UINT8 errors);
int PORT_GetQueueStatus(UINT16 handle, tPORT_STATUS *p_status);
int PORT_Purge(UINT16 handle, UINT8 purge_flags);
int PORT_ReadData(UINT16 handle, char *p_data, UINT16 max_len, UINT16 *p_len);
int PORT_Read(UINT16 handle, BT_HDR **pp_buf);
int PORT_Write(UINT16 handle, BT_HDR *p_buf);
int PORT_WriteData(UINT16 handle, char *p_data, UINT16 max_len, UINT16 *p_len);
int PORT_Test(UINT16 handle, UINT8 *p_data, UINT16 len);
UINT8 PORT_SetTraceLevel(UINT8 new_level);

#ifdef __cplusplus
	}
#endif

#endif // BTE_RFCOMM_PORT_API_H
