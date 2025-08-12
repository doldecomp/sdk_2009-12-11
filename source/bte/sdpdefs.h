#ifndef BTE_SDP_DEFS_H
#define BTE_SDP_DEFS_H

/*******************************************************************************
 * headers
 */

/*******************************************************************************
 * macros
 */

#define ATTR_ID_SERVICE_RECORD_HDL			0x0000
#define ATTR_ID_SERVICE_CLASS_ID_LIST		0x0001
#define ATTR_ID_SERVICE_ID					0x0003
#define ATTR_ID_PROTOCOL_DESC_LIST			0x0004
#define ATTR_ID_BROWSE_GROUP_LIST			0x0005
#define ATTR_ID_LANGUAGE_BASE_ATTR_ID_LIST	0x0006
#define ATTR_ID_BT_PROFILE_DESC_LIST		0x0009
#define ATTR_ID_DOCUMENTATION_URL			0x000a
#define ATTR_ID_CLIENT_EXE_URL				0x000b
#define ATTR_ID_ADDITION_PROTO_DESC_LISTS	0x000d

#define ATTR_ID_SERVICE_NAME				0x0100
#define ATTR_ID_SERVICE_DESCRIPTION			0x0101
#define ATTR_ID_PROVIDER_NAME				0x0102

#define ATTR_ID_SPECIFICATION_ID			0x0200
#define ATTR_ID_VENDOR_ID					0x0201
#define ATTR_ID_PRODUCT_ID					0x0202
#define ATTR_ID_PRODUCT_VERSION				0x0203
#define ATTR_ID_PRIMARY_RECORD				0x0204
#define ATTR_ID_VENDOR_ID_SOURCE			0x0205

#define BLUETOOTH_DI_SPECIFICATION              0x0103  /* 1.3 */

#define ATTR_ID_HID_DEVICE_RELNUM			0x0200
#define ATTR_ID_HID_PARSER_VERSION			0x0201
#define ATTR_ID_HID_DEVICE_SUBCLASS			0x0202
#define ATTR_ID_HID_COUNTRY_CODE			0x0203
#define ATTR_ID_HID_VIRTUAL_CABLE			0x0204
#define ATTR_ID_HID_RECONNECT_INITIATE		0x0205
#define ATTR_ID_HID_DESCRIPTOR_LIST			0x0206
#define ATTR_ID_HID_SDP_DISABLE				0x0208
#define ATTR_ID_HID_BATTERY_POWER			0x0209
#define ATTR_ID_HID_REMOTE_WAKE				0x020a
#define ATTR_ID_HID_LINK_SUPERVISION_TO		0x020c
#define ATTR_ID_HID_NORMALLY_CONNECTABLE	0x020d

#define UINT_DESC_TYPE						1
#define TWO_COMP_INT_DESC_TYPE				2
#define UUID_DESC_TYPE						3
#define TEXT_STR_DESC_TYPE					4
#define BOOLEAN_DESC_TYPE					5
#define DATA_ELE_SEQ_DESC_TYPE				6
#define DATA_ELE_ALT_DESC_TYPE				7
#define URL_DESC_TYPE						8

#define  SIZE_ONE_BYTE                0
#define  SIZE_TWO_BYTES               1
#define  SIZE_FOUR_BYTES              2
#define  SIZE_EIGHT_BYTES             3
#define  SIZE_SIXTEEN_BYTES           4
#define  SIZE_IN_NEXT_BYTE            5
#define  SIZE_IN_NEXT_WORD            6
#define  SIZE_IN_NEXT_LONG            7

#define UUID_PROTOCOL_RFCOMM				0x0003
#define UUID_PROTOCOL_OBEX					0x0008
#define UUID_PROTOCOL_L2CAP					0x0100

#define UUID_SERVCLASS_PUBLIC_BROWSE_GROUP	0x1002
#define UUID_SERVCLASS_SERIAL_PORT			0x1101
#define UUID_SERVCLASS_LAN_ACCESS_USING_PPP	0x1102
#define UUID_SERVCLASS_DIALUP_NETWORKING	0X1103
#define UUID_SERVCLASS_FAX					0X1111
#define UUID_SERVCLASS_HUMAN_INTERFACE		0x1124
#define UUID_SERVCLASS_PNP_INFORMATION		0x1200
#define UUID_SERVCLASS_HDP_PROFILE			0x1400
#define UUID_SERVCLASS_HDP_SOURCE			0x1401

#define DI_VENDOR_ID_SOURCE_BTSIG			0x0001

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

/*******************************************************************************
 * external globals
 */

/*******************************************************************************
 * functions
 */

#ifdef __cplusplus
	}
#endif

#endif // BTE_SDP_DEFS_H
