
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __HA_INTERNAL_H__
#define __HA_INTERNAL_H__
#if !defined(__KERNEL__) && (defined (LINUX) || defined(UNIX))

#include <bcm/types.h>
#include <soc/types.h>

#include <sal/core/sync.h>

#define HA_MEM_SIGNATURE           0xa75c9b3e
#define RESIZE_MEMORY_SIZE         0x1000000
#define HA_MAX_NUM_OF_MEM_SECTIONS 0x100
#define HA_FILE_NAME_SIZE 256

typedef enum
{
    ha_blk_free = 0,
    ha_blk_allocated = 1
} ha_mem_block_state_t;

typedef struct
{

    uint32 signature;

    uint16 blk_id;

    uint8 state;

    uint32 length;
} ha_mem_blk_hdr_t;

typedef struct
{

    ha_mem_blk_hdr_t *mem_start;

    ha_mem_blk_hdr_t *mem_end;

    ha_mem_blk_hdr_t *free_mem;

    uint32 blk_len;

    int file_handle;

    char file_name[HA_FILE_NAME_SIZE];

    int sys_page_size;

    uint8 new_file;

    sal_mutex_t mutex;

    uint8 initialized;
} ha_mgr_t;

int ha_mgr_find_block(
    int unit,
    uint16 blk_id,
    ha_mem_blk_hdr_t ** block);

int ha_mgr_sanity_check(
    int unit);

int ha_mgr_open_shared_mem(
    int unit,
    uint8 create_new_file);

#endif
#endif
