#ifndef BTE_GKI_H
#define BTE_GKI_H

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * gki/common/gki.h
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
 * macros
 */

#define TASK_MBOX_0	0

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef void TIMER_CBACK(void *p_tle);

#define TIMER_PARAM_TYPE	__typeof__(UINT32)

typedef struct
{
	void	*p_first;	// size 0x04, offset 0x00
	void	*p_last;	// size 0x04, offset 0x04
	UINT16	count;		// size 0x02, offset 0x08
	/* 2 bytes padding */
} BUFFER_Q; // size 0x0c

typedef struct _tle
{
	struct _tle			*p_next;	// size 0x04, offset 0x00
	struct _tle			*p_prev;	// size 0x04, offset 0x04
	TIMER_CBACK			*p_cback;	// size 0x04, offset 0x08
	INT32				ticks;		// size 0x04, offset 0x0c
	TIMER_PARAM_TYPE	param;		// size 0x04, offset 0x10
	UINT16				event;		// size 0x02, offset 0x14
	UINT8				in_use;		// size 0x01, offset 0x16
	/* 1 byte padding */
} TIMER_LIST_ENT; // size 0x18

typedef struct
{
	TIMER_LIST_ENT	*p_first;	// size 0x04, offset 0x00
	TIMER_LIST_ENT	*p_last;	// size 0x04, offset 0x04
	INT32			last_ticks;	// size 0x04, offset 0x08
} TIMER_LIST_Q; // size 0x0c

/*******************************************************************************
 * external globals
 */

/*******************************************************************************
 * functions
 */

UINT8 GKI_get_taskid(void);
void GKI_send_msg(UINT8, UINT8, void *);
void GKI_freebuf(void *);
void *GKI_getbuf(UINT16);
void GKI_add_to_timer_list(TIMER_LIST_Q *, TIMER_LIST_ENT *);
void GKI_init_timer_list(TIMER_LIST_Q *);
void GKI_remove_from_timer_list(TIMER_LIST_Q *, TIMER_LIST_ENT *);
void GKI_start_timer(UINT8, INT32, BOOLEAN);
void GKI_stop_timer(UINT8);
UINT16 GKI_update_timer_list(TIMER_LIST_Q *, INT32);
void *GKI_getfirst(BUFFER_Q *);
void GKI_disable(void);
void *GKI_getnext(void *);
void *GKI_dequeue(BUFFER_Q *);
void GKI_enable(void);
void *GKI_getpoolbuf(UINT8);
void GKI_enqueue(BUFFER_Q *, void *);
void GKI_enqueue_head(BUFFER_Q *, void *);
void *GKI_remove_from_queue(BUFFER_Q *, void *);
UINT16 GKI_get_pool_bufsize(UINT8);
UINT32 GKI_get_tick_count(void);

#ifdef __cplusplus
	}
#endif

#endif // BTE_GKI_H
