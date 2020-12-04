/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SHR_SW_STATE_UTILS_H
#define _SHR_SW_STATE_UTILS_H

#include <soc/types.h>
#include <shared/swstate/sw_state.h>
#include <shared/swstate/layout/sw_state_layout.h> 

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

#define SW_STATE_NODE_ID_CHECK(unit, node_id) \
    do{ \
        if (node_id >= SW_STATE_LAYOUT_NOF_MAX_LAYOUT_NODES) {\
            _SOC_EXIT_WITH_ERR(BCM_E_FULL, \
                       (BSL_META_U(unit, \
                          "Unit:%d sw state ERROR: the ds_layout_nodes array is full.\n"), unit)); \
        }\
    } while(0)

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

int shr_sw_state_ds_layout_node_t_clear(shr_sw_state_ds_layout_node_t *node); 

int shr_sw_state_ds_layout_node_set(int unit, 
                                    int node_id, 
                                    char* name, 
                                    int size, 
                                    int nof_pointer,
                                    char* type, 
                                    int array_size_0, 
                                    int array_size_1); 
int shr_sw_state_ds_layout_add_child(int unit, int parent_node_id, int child_node_id); 
int shr_sw_state_ds_layout_add_brother(int unit, int older_brother_node_id, int younger_brother_node_id); 
int shr_sw_state_ds_layout_update_offset(int unit, int node_id, int offset); 

int shr_sw_state_utils_init(int unit);
int shr_sw_state_utils_deinit(int unit);

int shr_sw_state_scache_ptr_get(int unit, soc_scache_handle_t handle, shr_sw_state_scache_oper_t oper,
                             int flags, uint32 *size, uint8 **scache_ptr, int *already_exists);

int shr_sw_state_scache_sync(int unit, soc_scache_handle_t handle, int offset, int size);




#endif  /* _SHR_SW_STATE_UTILS_H */
