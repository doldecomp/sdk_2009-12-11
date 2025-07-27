#ifndef BTE_HCIDEFS_H
#define BTE_HCIDEFS_H

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/include/hcidefs.h
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

#define HCI_MAX_INQUIRY_LENGTH		0x30

#define HCI_PER_INQ_MIN_MIN_PERIOD	0x0002
#define HCI_PER_INQ_MAX_MIN_PERIOD	0xfffe
#define HCI_PER_INQ_MIN_MAX_PERIOD	0x0003
#define HCI_PER_INQ_MAX_MAX_PERIOD	0xffff

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

#endif // BTE_HCIDEFS_H
