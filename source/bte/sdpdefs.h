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

#define UINT_DESC_TYPE						1
#define TEXT_STR_DESC_TYPE					4

#define UUID_PROTOCOL_RFCOMM				0x0003
#define UUID_PROTOCOL_OBEX					0x0008
#define UUID_PROTOCOL_L2CAP					0x0100

#define UUID_SERVCLASS_PUBLIC_BROWSE_GROUP	0x1002

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
