#ifndef BTE_GKI_TARGET_H
#define BTE_GKI_TARGET_H

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * include/gki_target.h
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

#ifndef PPC_TASK
# define PPC_TASK			2
#endif

#ifndef GKI_POOL_ID_2
# define GKI_POOL_ID_2		2
#endif

#ifndef GKI_POOL_ID_3
# define GKI_POOL_ID_3		3
#endif

#ifndef TICKS_PER_SEC
# define TICKS_PER_SEC		100
#endif

#ifndef GKI_MS_TO_TICKS
# define GKI_MS_TO_TICKS(x)	((x) / (1000 / TICKS_PER_SEC))
#endif

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

#endif // BTE_GKI_TARGET_H
