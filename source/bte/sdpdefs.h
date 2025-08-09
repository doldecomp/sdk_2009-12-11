#ifndef BTE_SDP_DEFS_H
#define BTE_SDP_DEFS_H

/*******************************************************************************
 * headers
 */

/*******************************************************************************
 * macros
 */

#define ATTR_ID_BROWSE_GROUP_LIST			0x0005

#define ATTR_ID_SERVICE_NAME				0x0100
#define ATTR_ID_SERVICE_DESCRIPTION			0x0101
#define ATTR_ID_PROVIDER_NAME				0x0102

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
#define TEXT_STR_DESC_TYPE					4
#define DATA_ELE_SEQ_DESC_TYPE				6

#define UUID_PROTOCOL_RFCOMM				0x0003
#define UUID_PROTOCOL_OBEX					0x0008
#define UUID_PROTOCOL_L2CAP					0x0100

#define UUID_SERVCLASS_PUBLIC_BROWSE_GROUP	0x1002
#define UUID_SERVCLASS_SERIAL_PORT			0x1101
#define UUID_SERVCLASS_LAN_ACCESS_USING_PPP	0x1102
#define UUID_SERVCLASS_DIALUP_NETWORKING	0X1103
#define UUID_SERVCLASS_FAX					0X1111
#define UUID_SERVCLASS_HUMAN_INTERFACE		0x1124

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
