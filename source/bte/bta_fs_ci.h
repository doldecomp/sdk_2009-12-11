#ifndef BTE_BTA_FS_CI_H
#define BTE_BTA_FS_CI_H

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * bta/include/bta_fs_ci.h
 */

/******************************************************************************
 *
 *  Copyright (C) 2003-2012 Broadcom Corporation
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

#include "bt_types.h"
#include "data_types.h"

#include "bta_fs_co.h"

/*******************************************************************************
 * macros
 */

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef struct
{
    BT_HDR            hdr;
    int               fd;
    tBTA_FS_CO_STATUS status;
} tBTA_FS_CI_WRITE_EVT;

typedef struct
{
    BT_HDR            hdr;
    int               fd;
    UINT16            num_read;
    tBTA_FS_CO_STATUS status;
} tBTA_FS_CI_READ_EVT;

typedef struct
{
    BT_HDR            hdr;
    UINT32            file_size;
    int               fd;
    tBTA_FS_CO_STATUS status;
} tBTA_FS_CI_OPEN_EVT;

typedef struct
{
    BT_HDR            hdr;
    tBTA_FS_CO_STATUS status;
} tBTA_FS_CI_GETDIR_EVT;

/*******************************************************************************
 * external globals
 */

/*******************************************************************************
 * functions
 */

void bta_fs_ci_write(int fd, tBTA_FS_CO_STATUS status, UINT16 evt);
void bta_fs_ci_read(int fd, UINT16 num_bytes_read, tBTA_FS_CO_STATUS status,
                    UINT16 evt);
void bta_fs_ci_open(int fd, tBTA_FS_CO_STATUS status, UINT32 file_size,
                    UINT16 evt);
void bta_fs_ci_direntry(tBTA_FS_CO_STATUS status, UINT16 evt);

#ifdef __cplusplus
	}
#endif

#endif // BTE_BTA_FS_CI_H
