#ifndef BTE_XML_BLD_H
#define BTE_XML_BLD_H

/*******************************************************************************
 * headers
 */

#include "data_types.h"

/*******************************************************************************
 * functions
 */

#ifdef __cplusplus
	extern "C" {
#endif

UINT16 XML_BufAddTag(UINT8 **pp_buf, UINT8 const *prefix, UINT8 const *tag,
                     BOOLEAN start_tag, BOOLEAN has_attr);
UINT16 XML_BufAddAttribute(UINT8 **pp_buf, UINT8 const *prefix,
                           UINT8 const *attr_name, UINT8 const *attr_value,
                           UINT8 last_attr);
UINT16 XML_BufAddCharData(UINT8 **pp_buf, UINT8 const *charData);

#ifdef __cplusplus
	}
#endif

#endif // BTE_XML_BLD_H
