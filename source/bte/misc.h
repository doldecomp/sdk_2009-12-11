#ifndef BTE_MISC_H
#define BTE_MISC_H

// Declarations that I haven't figured out where to put yet

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

/*******************************************************************************
 * external globals
 */

/*******************************************************************************
 * functions
 */

// bte_main.c

#if !defined(NDEBUG)
void __btu_interrupt_handler_debug_msg(BOOLEAN enable);
#endif

void bta_usb_close_evt(INT8 param_1);

#ifdef __cplusplus
	}
#endif

#endif // BTE_MISC_H
