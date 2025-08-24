#ifndef BTE_UUSB_H
#define BTE_UUSB_H

/*******************************************************************************
 * headers
 */

#include "data_types.h"

/*******************************************************************************
 * macros
 */

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef enum
{
	UUSB_OP_NONE,
} tUUSB_IOCTL_OP;

typedef void tUUSB_CBACK(UINT8, INT8);

typedef struct
{
	char	pad0_[4];
	UINT16	at_0x04;
	UINT16	at_0x06;
	UINT16	at_0x08;
	char	pad1_[1];
	char	at_0x0b;
	char	pad2_[1]; // for stack
} tUUSB;

/*******************************************************************************
 * external globals
 */

extern UINT32 volatile wait4hci;

/*******************************************************************************
 * functions
 */

void UUSB_Unregister(void);

#ifdef __cplusplus
	}
#endif

#endif // BTE_UUSB_H
