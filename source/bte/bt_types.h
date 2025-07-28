#ifndef BTE_BT_TYPES_H
#define BTE_BT_TYPES_H

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/include/bt_types.h
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

/*******************************************************************************
 * banner
 */

#define BT_EVT_TO_BTU_SP_DATA	0x1500
#define BT_EVT_TO_LM_HCI_ACL	0x2100

#define BT_PSM_RFCOMM			3

#define BTM_SEC_PROTO_RFCOMM	3

// clang-format off
#define UINT8_TO_STREAM(p, u8)								\
	do														\
	{														\
		*(p)++ = (UINT8)(u8);								\
	}														\
	while (FALSE)

#define UINT16_TO_STREAM(p, u16)							\
	do														\
	{														\
		*(p)++ = (UINT8)(u16);								\
		*(p)++ = (UINT8)((u16) >> 8);						\
	} while (FALSE)

#define UINT32_TO_STREAM(p, u32)							\
	do														\
	{														\
		*(p)++ = (UINT8)(u32);								\
		*(p)++ = (UINT8)((u32) >> 8);						\
		*(p)++ = (UINT8)((u32) >> 16);						\
		*(p)++ = (UINT8)((u32) >> 24);						\
	} while (FALSE)

#define ARRAY8_TO_STREAM(p, a)								\
	do														\
	{														\
		register int ijk;									\
															\
		for (ijk = 0; ijk < 8; ijk++)						\
			*(p)++ = (UINT8)(a)[(8 - 1) - ijk];				\
	} while (FALSE)

#define ARRAY16_TO_STREAM(p, a)								\
	do														\
	{														\
		register int ijk;									\
															\
		for (ijk = 0; ijk < 16; ijk++)						\
			*(p)++ = (UINT8)(a)[(16 - 1) - ijk];			\
	} while (FALSE)

#define BDADDR_TO_STREAM(p, a)								\
	do														\
	{														\
		register int ijk;									\
															\
		for (ijk = 0; ijk < BD_ADDR_LEN; ijk++)				\
			*(p)++ = (UINT8)(a)[(BD_ADDR_LEN - 1) - ijk];	\
	} while (FALSE)

#define DEVCLASS_TO_STREAM(p, a)							\
	do														\
	{														\
		register int ijk;									\
															\
		for (ijk = 0; ijk < DEV_CLASS_LEN; ijk++)			\
			*(p)++ = (UINT8)(a)[(DEV_CLASS_LEN - 1) - ijk];	\
	} while (FALSE)

#define LAP_TO_STREAM(p, a)									\
	do														\
	{														\
		register int ijk;									\
															\
		for (ijk = 0; ijk < LAP_LEN; ijk++)					\
			*(p)++ = (UINT8)(a)[(LAP_LEN - 1) - ijk];		\
	} while (FALSE)

#define ARRAY_TO_STREAM(p, a, len)							\
	do														\
	{														\
		register int ijk;									\
															\
		for (ijk = 0; ijk < len; ijk++)						\
			*(p)++ = (UINT8) a[ijk];						\
	} while (FALSE)

// clang-format on

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef struct
{
	UINT16	event;			// size 0x02, offset 0x00
	UINT16	len;			// size 0x02, offset 0x02
	UINT16	offset;			// size 0x02, offset 0x04
	UINT16	layer_specific;	// size 0x02, offset 0x06
} BT_HDR; // size 0x08

// only for bc; do not use (TODO: migrate uses to actual sizeof expression)
#define BT_HDR_SIZE			(sizeof(BT_HDR))

#define BD_ADDR_LEN			6
typedef UINT8 BD_ADDR[BD_ADDR_LEN];
typedef UINT8 *BD_ADDR_PTR;

#define LINK_KEY_LEN		16
typedef UINT8 LINK_KEY[LINK_KEY_LEN];

#define DEV_CLASS_LEN		3
typedef UINT8 DEV_CLASS[DEV_CLASS_LEN];
typedef UINT8 *DEV_CLASS_PTR;

#define BD_NAME_LEN			248 // or is 247 to keep the + 1?
typedef UINT8 BD_NAME[BD_NAME_LEN];
typedef UINT8 *BD_NAME_PTR;

#define LAP_LEN				3
typedef UINT8 LAP[LAP_LEN];

#define PIN_CODE_LEN		16
typedef UINT8 PIN_CODE[PIN_CODE_LEN];
typedef UINT8 *PIN_CODE_PTR;

#define BT_EVENT_MASK_LEN	8
typedef UINT8 BT_EVENT_MASK[BT_EVENT_MASK_LEN];

#define MAX_UUID_SIZE		16
typedef struct
{
	UINT16	len;	// size 0x02, offset 0x00
	union
	{
		UINT16	uuid16;					// size 0x02
		UINT32	uuid32;					// size 0x04
		UINT8	uuid128[MAX_UUID_SIZE];	// size 0x10
	} uu; // size 0x10, offset 0x04
} tBT_UUID; // size 0x14

typedef struct
{
	UINT8	qos_flags;			// size 0x01, offset 0x00
	UINT8	service_type;		// size 0x01, offset 0x01
	/* 2 bytes padding */
	UINT32	token_rate;			// size 0x04, offset 0x04
	UINT32	token_bucket_size;	// size 0x04, offset 0x08
	UINT32	peak_bandwidth;		// size 0x04, offset 0x0c
	UINT32	latency;			// size 0x04, offset 0x10
	UINT32	delay_variation;	// size 0x04, offset 0x14
} FLOW_SPEC; // size 0x18

#ifdef __cplusplus
	}
#endif

#endif // BTE_BT_TYPES_H
