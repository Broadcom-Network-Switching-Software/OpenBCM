/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _SHR_SW_STATE_UTILS_H
#define _SHR_SW_STATE_UTILS_H

#include <soc/types.h>
#include <shared/swstate/sw_state.h>

/* scache operations */
typedef enum shr_sw_state_scache_oper_e {
    socSwStateScacheRetreive,
    socSwStateScacheCreate,
    socSwStateScacheRealloc
} shr_sw_state_scache_oper_t;

typedef enum
{
    SHR_SW_STATE_SCACHE_HANDLE_DATA_BLOCK,
    SHR_SW_STATE_SCACHE_HANDLE_JOURNAL
} SHR_SW_STATE_SCACHE_HANDLES;


/*
 *           ------ 
 *          | node |
 *          -------\
 *            |     \-----------------------------|
 *            v                                   v
 *          -------     -------     ------- 
 *          | node |--->| node |--->| node |--> ...
 *          -------     -------     ------- 
 * 
 * ex:
 *          -------
 *          | dpp |
 *          -------
 *            |    \-------------------|
 *            v                        v
 *          -------     -------     --------- 
 *          | bcm  |--->| soc  |--->| shared |
 *          -------     -------     --------- 

 * Note: layout is saved in sw state header (shr_sw_state_data_block_header)
 *       offsets are relative to shr_sw_state_data_block_header address
 *       layout node: an element of a struct
 *       children of a node: they are the element of parent node's struct 
 *       root: *sw_state 
 */


/**********************************************/
/***********      Functions     ***************/
/**********************************************/

uint8 sw_state_is_flag_on(
    uint32 flags,
    uint32 flag);

uint8 dnxc_sw_state_alloc_during_test_set(
    int unit,
    uint8 flag);

uint8 dnxc_sw_state_alloc_during_test_get(
    int unit);


int shr_sw_state_utils_init(int unit);
int shr_sw_state_utils_deinit(int unit);

int shr_sw_state_scache_ptr_get(int unit, soc_scache_handle_t handle, shr_sw_state_scache_oper_t oper,
                             int flags, uint32 *size, uint8 **scache_ptr, int *already_exists);

int shr_sw_state_scache_sync(int unit, soc_scache_handle_t handle, int offset, int size);




#endif  /* _SHR_SW_STATE_UTILS_H */
