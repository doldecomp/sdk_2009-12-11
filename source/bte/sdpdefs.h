#ifndef BTE_SDP_DEFS_H
#define BTE_SDP_DEFS_H

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/include/sdpdefs.h
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
 * macros
 */

#define BLUETOOTH_DI_SPECIFICATION	0x0103

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

// main
enum
{
	ATTR_ID_SERVICE_RECORD_HDL			= 0,
	ATTR_ID_SERVICE_CLASS_ID_LIST		= 1,

	ATTR_ID_SERVICE_ID					= 3,
	ATTR_ID_PROTOCOL_DESC_LIST			= 4,
	ATTR_ID_BROWSE_GROUP_LIST			= 5,
	ATTR_ID_LANGUAGE_BASE_ATTR_ID_LIST	= 6,

	ATTR_ID_BT_PROFILE_DESC_LIST		= 9,
	ATTR_ID_DOCUMENTATION_URL			= 10,
	ATTR_ID_CLIENT_EXE_URL				= 11,

	ATTR_ID_ADDITION_PROTO_DESC_LISTS	= 13,
};

// language
enum
{
	ATTR_ID_SERVICE_NAME				= 256,
	ATTR_ID_SERVICE_DESCRIPTION			= 257,
	ATTR_ID_PROVIDER_NAME				= 258,
};

// device identification
enum
{
	ATTR_ID_SPECIFICATION_ID	= 512,
	ATTR_ID_VENDOR_ID			= 513,
	ATTR_ID_PRODUCT_ID			= 514,
	ATTR_ID_PRODUCT_VERSION		= 515,
	ATTR_ID_PRIMARY_RECORD		= 516,
	ATTR_ID_VENDOR_ID_SOURCE	= 517,
};

enum
{
	DI_VENDOR_ID_SOURCE_BTSIG	= 1,
};

// hid profile
enum
{
	ATTR_ID_HID_DEVICE_RELNUM			= 512,
	ATTR_ID_HID_PARSER_VERSION			= 513,
	ATTR_ID_HID_DEVICE_SUBCLASS			= 514,
	ATTR_ID_HID_COUNTRY_CODE			= 515,
	ATTR_ID_HID_VIRTUAL_CABLE			= 516,
	ATTR_ID_HID_RECONNECT_INITIATE		= 517,
	ATTR_ID_HID_DESCRIPTOR_LIST			= 518,

	ATTR_ID_HID_SDP_DISABLE				= 520,
	ATTR_ID_HID_BATTERY_POWER			= 521,
	ATTR_ID_HID_REMOTE_WAKE				= 522,

	ATTR_ID_HID_LINK_SUPERVISION_TO		= 524,
	ATTR_ID_HID_NORMALLY_CONNECTABLE	= 525,
};

enum
{
	UUID_PROTOCOL_RFCOMM	= 3,
	UUID_PROTOCOL_OBEX		= 8,
	UUID_PROTOCOL_L2CAP		= 256,
};

enum
{
	UUID_SERVCLASS_PUBLIC_BROWSE_GROUP	= 0x1002,

	UUID_SERVCLASS_SERIAL_PORT			= 0x1101,
	UUID_SERVCLASS_LAN_ACCESS_USING_PPP	= 0x1102,
	UUID_SERVCLASS_DIALUP_NETWORKING	= 0x1103,

	UUID_SERVCLASS_FAX					= 0x1111,

	UUID_SERVCLASS_HUMAN_INTERFACE		= 0x1124,

	UUID_SERVCLASS_PNP_INFORMATION		= 0x1200,
};

enum
{
	UINT_DESC_TYPE			= 1,
	TWO_COMP_INT_DESC_TYPE	= 2,
	UUID_DESC_TYPE			= 3,
	TEXT_STR_DESC_TYPE		= 4,
	BOOLEAN_DESC_TYPE		= 5,
	DATA_ELE_SEQ_DESC_TYPE	= 6,
	DATA_ELE_ALT_DESC_TYPE	= 7,
	URL_DESC_TYPE			= 8,
};

enum
{
	SIZE_ONE_BYTE,
	SIZE_TWO_BYTES,
	SIZE_FOUR_BYTES,
	SIZE_EIGHT_BYTES,
	SIZE_SIXTEEN_BYTES,
	SIZE_IN_NEXT_BYTE,
	SIZE_IN_NEXT_WORD,
	SIZE_IN_NEXT_LONG,
};

#ifdef __cplusplus
	}
#endif

#endif // BTE_SDP_DEFS_H
