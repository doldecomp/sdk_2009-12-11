#ifndef BTE_RFC_DEFS_H
#define BTE_RFC_DEFS_H

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/include/rfcdefs.h
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

/*******************************************************************************
 * macros
 */

#define PORT_MAX_RFC_PORTS		31

#define RFCOMM_DEFAULT_MTU		127

#define RFCOMM_MAX_DLCI			61

#define RFCOMM_MIN_OFFSET		5
#define RFCOMM_DATA_OVERHEAD	(RFCOMM_MIN_OFFSET + 1)

#define RFCOMM_PN_CONV_LAYER_TYPE_1 0
#define RFCOMM_PN_CONV_LAYER_CBFC_R 0xE0

#define RFCOMM_K_MAX                7 /* Max value of K for credit based flow control */

#define RFCOMM_MX_DLCI                  0

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

#endif // BTE_RFC_DEFS_H
