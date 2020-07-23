/*! \file ha_internal.h
 * 
 *
 * HA internal class definition
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __HA_INTERNAL_H__
#define __HA_INTERNAL_H__
#if !defined(__KERNEL__) && (defined (LINUX) || defined(UNIX))

#include <bcm/types.h>
#include <soc/types.h>
/* for the mutex type */
#include <sal/core/sync.h>

#define HA_MEM_SIGNATURE           0xa75c9b3e
#define RESIZE_MEMORY_SIZE         0x1000000   /* 16MB */
#define HA_MAX_NUM_OF_MEM_SECTIONS 0x100
#define HA_FILE_NAME_SIZE 256



typedef enum {
    ha_blk_free = 0,
    ha_blk_allocated = 1
} ha_mem_block_state_t;


/*!
 * \brief memory block header type.
 *
 * Every memory block contains the following header structure.
 * The structure contains information required for the memory manager
 * as well as for version upgrade/downgrade
 */
typedef struct {
    /* Every block starts with signature */
    uint32 signature;
    /* unique block ID */
    uint16 blk_id;
    /* block state (allocated/free) */
    uint8 state;
    /* block length including the header */
    uint32 length;
} ha_mem_blk_hdr_t;

/*!
 * \brief memory manager struct.
 *
 * This is the memory manager struct. The purpose is that the
 * HA memory manager will be able to menage the allocated shared memory.
 */
typedef struct {
    /* pointer to the begining of the block */
    ha_mem_blk_hdr_t *mem_start;
    /* out of bounds block */  
    ha_mem_blk_hdr_t *mem_end;
    /* pointer to the free memory block */    
    ha_mem_blk_hdr_t *free_mem;
    /* Total allocated HA memory (free + allocated) */
    uint32 blk_len;
    /* shared file handle associated with the memory block */
    int fh;
    /* the HA shared file name - user provided */
    char file_name[HA_FILE_NAME_SIZE];
    /* system page size */
    int sys_page_size;
    /* indicates if created new file or opened existing file */
    uint8 new_file;
    /* protect shared HA memory resource */
    sal_mutex_t mutex;
    /* indicates if mem_init() method was called */
    uint8 initialized;
} ha_mgr_t;


/*
 * auxiliary functions used by the manager
 */

/* find block with specific block ID */
int ha_mgr_find_block(int unit, uint16 blk_id, ha_mem_blk_hdr_t **block);
/* sanity check of the entire HA memory */
int ha_mgr_sanity_check(int unit);
/* create/open shared memory file */
int ha_mgr_open_shared_mem(int unit, uint8 create_new_file);

#endif /* __KERNEL__ */
#endif   /* __HA_INTERNAL_H__  */
