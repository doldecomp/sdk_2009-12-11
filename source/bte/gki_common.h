#ifndef BTE_GKI_COMMON_H
#define BTE_GKI_COMMON_H

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * gki/common/gki_common.h
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
#include "gki_target.h"

#include "gki.h"

/*******************************************************************************
 * macros
 */

#define TASK_DEAD       0   /* b0000 */
#define TASK_READY      1   /* b0001 */

#define GKI_ERROR_BUF_SIZE_ZERO         0xFFF6
#define GKI_ERROR_BUF_SIZE_TOOBIG       0xFFF7
#define GKI_ERROR_FREEBUF_BUF_LINKED    0xFFFC
#define GKI_ERROR_FREEBUF_BAD_QID       0xFFFD
#define GKI_ERROR_BUF_CORRUPTED         0xFFFF
#define GKI_ERROR_SEND_MSG_BAD_DEST     0xFFFB
#define GKI_ERROR_SEND_MSG_BUF_LINKED   0xFFFA
#define GKI_ERROR_ENQUEUE_BUF_LINKED    0xFFF9
#define GKI_ERROR_DELETE_POOL_BAD_QID   0xFFF8

#define ALIGN_POOL(pl_size)	(((pl_size) + 3) / sizeof(UINT32) * sizeof(UINT32))
#define BUFFER_PADDING_SIZE	(sizeof(BUFFER_HDR_T) + sizeof(UINT32))
#define MAX_USER_BUF_SIZE   (0xffff - BUFFER_PADDING_SIZE)

#define BUF_STATUS_FREE		0
#define BUF_STATUS_UNLINKED 1
#define BUF_STATUS_QUEUED   2

#define MAGIC_NO			0xddbaddba

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef struct _buffer_hdr
{
	struct _buffer_hdr	*p_next;
	UINT8				q_id;
	UINT8				task_id;
	UINT8				status;
	UINT8				type;
} BUFFER_HDR_T;

typedef struct _free_queue
{
	BUFFER_HDR_T	*p_first;
	BUFFER_HDR_T	*p_last;
	UINT16			size;
	UINT16			total;
	UINT16			cur_cnt;
	UINT16			max_cnt;
} FREE_QUEUE_T;

typedef struct
{
    UINT8           bufpool0[(ALIGN_POOL(GKI_BUF0_SIZE) + BUFFER_PADDING_SIZE) * GKI_BUF0_MAX];
    UINT8           bufpool1[(ALIGN_POOL(GKI_BUF1_SIZE) + BUFFER_PADDING_SIZE) * GKI_BUF1_MAX];
    UINT8           bufpool2[(ALIGN_POOL(GKI_BUF2_SIZE) + BUFFER_PADDING_SIZE) * GKI_BUF2_MAX];
    UINT8           bufpool3[(ALIGN_POOL(GKI_BUF3_SIZE) + BUFFER_PADDING_SIZE) * GKI_BUF3_MAX];
    UINT8           bufpool4[(ALIGN_POOL(GKI_BUF4_SIZE) + BUFFER_PADDING_SIZE) * GKI_BUF4_MAX];
    UINT8           *OSStack[GKI_MAX_TASKS];
    UINT16          OSStackSize[GKI_MAX_TASKS];
    INT8            *OSTName[GKI_MAX_TASKS];
    UINT8           OSRdyTbl[GKI_MAX_TASKS];
    UINT16          OSWaitEvt[GKI_MAX_TASKS];
    UINT16          OSWaitForEvt[GKI_MAX_TASKS];
    UINT32          OSTicks;
    UINT32          OSIdleCnt;
    INT16           OSDisableNesting;
    INT16           OSLockNesting;
    INT16           OSIntNesting;
    INT32           OSTicksTilExp;
    INT32           OSNumOrigTicks;
    INT32           OSWaitTmr[GKI_MAX_TASKS];
    INT32           OSTaskTmr0[GKI_MAX_TASKS];
    INT32           OSTaskTmr0R[GKI_MAX_TASKS];
    INT32           OSTaskTmr1[GKI_MAX_TASKS];
    INT32           OSTaskTmr1R[GKI_MAX_TASKS];
    BUFFER_HDR_T    *OSTaskQFirst[GKI_MAX_TASKS][NUM_TASK_MBOX];
    BUFFER_HDR_T    *OSTaskQLast[GKI_MAX_TASKS][NUM_TASK_MBOX];
    FREE_QUEUE_T    freeq[GKI_NUM_TOTAL_BUF_POOLS];
    UINT8           *pool_start[GKI_NUM_TOTAL_BUF_POOLS];
    UINT8           *pool_end[GKI_NUM_TOTAL_BUF_POOLS];
    UINT16          pool_size[GKI_NUM_TOTAL_BUF_POOLS];
    void            *p_user_mempool;
    UINT16          pool_access_mask;
    UINT8           pool_list[GKI_NUM_TOTAL_BUF_POOLS];
    UINT8           curr_total_no_of_pools;
    BOOLEAN         timer_nesting;
} tGKI_COM_CB;

/*******************************************************************************
 * external globals
 */

/*******************************************************************************
 * functions
 */

#ifdef __cplusplus
	}
#endif

#endif // BTE_GKI_COMMON_H
