/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/alloc.h>
#include <shared/bsl.h>

#include <soc/types.h>
#include <soc/error.h>
#include <bcm/error.h>
#include <shared/error.h>
#include <soc/scache.h>


#include <shared/swstate/sw_state.h>
#include <shared/swstate/sw_state_defs.h>
#include <shared/swstate/sw_state_utils.h>

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE

/* in the future, scache init/deinit will be called from these functions */
int
shr_sw_state_utils_init(int unit){
    return 0;
}

int
shr_sw_state_utils_deinit(int unit){
    return 0;
}

/* 
 * this function does the interaction with scache module,
 * fetching\creating\modifying an scache buffer and return a pointer to it
 */
int
shr_sw_state_scache_ptr_get(int unit, soc_scache_handle_t handle, shr_sw_state_scache_oper_t oper,
                             int flags, uint32 *size, uint8 **scache_ptr, int *already_exists)
{
    int        rc = SOC_E_NONE;  
    uint32     allocated_size;
    int        alloc_size;

#ifdef BCM_WARM_BOOT_SUPPORT
    int        incr_size;
#endif

    SOC_INIT_FUNC_DEFS; /* added for _SOC_EXIT_WITH_ER */

    if (scache_ptr == NULL) {
     _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
               (BSL_META_U(unit,
                  "Unit:%d scache_ptr is null.\n"), unit));

          /* return(SOC_E_PARAM); */
    }

    if (oper == socSwStateScacheCreate) {
        if (size == NULL) {
         _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                   (BSL_META_U(unit,
                      "Unit:%d size is null.\n"), unit));
           /* return(SOC_E_PARAM); */
        }
        if (already_exists == NULL) {
         _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                   (BSL_META_U(unit,
                      "Unit:%d already_exist is null.\n"), unit));
          /* return(SOC_E_PARAM); */
        }

        SHR_SW_STATE_ALIGN_SIZE(*size);
        alloc_size = (*size) + SHR_SW_STATE_SCACHE_CONTROL_SIZE;

#ifdef BCM_WARM_BOOT_SUPPORT
        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if ((rc != SOC_E_NONE) && (rc != SOC_E_NOT_FOUND) ) {
            return(rc);
        }

        if (rc == SOC_E_NONE) { /* already exists */
            (*already_exists) = TRUE;
             _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                       (BSL_META_U(unit,
                          "Unit:%d scache already_exist.\n"), unit));
            /* return(SOC_E_PARAM); */
        }
        else { /* need to create */
            (*already_exists) = FALSE;
            rc = soc_scache_alloc(unit, handle, alloc_size);
            if (rc != SOC_E_NONE) {
                return(rc);
            }

            rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
            if (rc != SOC_E_NONE) {
                return(rc);
            }
            if ((*scache_ptr) == NULL) {
             _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                       (BSL_META_U(unit,
                          "Unit:%d Memory failure.\n"), unit));
             /* return(SOC_E_MEMORY); */
            }
        }

        if (alloc_size != allocated_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                       (BSL_META_U(unit,
                          "Unit:%d Allocation size is not enough.\n"), unit));
             /* return(SOC_E_INTERNAL); */
        }

#else /*BCM_WARM_BOOT_SUPPORT*/
        /* alloc buffer here instead of in scache */
        /*   (for buffers that store their original data on the buffer itself)*/
        *scache_ptr = sal_alloc(alloc_size, "scache buffer replacement");
        if ( *scache_ptr == NULL) {
             _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                   (BSL_META_U(unit,
                      "Unit:%d Memory allocation failure.\n"), unit));
            return SOC_E_MEMORY;
        }
        allocated_size = alloc_size;
#endif /*BCM_WARM_BOOT_SUPPORT*/
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    else if (oper == socSwStateScacheRetreive) {
        if (size == NULL) {
             _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                   (BSL_META_U(unit,
                      "Unit:%d Size is null.\n"), unit));
            /* return(SOC_E_PARAM); */
        }

        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if (rc != SOC_E_NONE) {
            return(rc);
        }
        (*size) = allocated_size;
        
        if (already_exists != NULL) {
            (*already_exists) = TRUE;
        }
    }
#endif /*BCM_WARM_BOOT_SUPPORT*/

    else if (oper == socSwStateScacheRealloc) {
        if (size == NULL) {
             _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                   (BSL_META_U(unit,
                      "Unit:%d Size is null.\n"), unit));
            /* return(SOC_E_PARAM); */
        }

#ifdef BCM_WARM_BOOT_SUPPORT
        /* get current size */
        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if (rc != SOC_E_NONE) {
            return(rc);
        }
#endif /*BCM_WARM_BOOT_SUPPORT*/

        /* allocate new size */
        SHR_SW_STATE_ALIGN_SIZE(*size);
        alloc_size = (*size) + SHR_SW_STATE_SCACHE_CONTROL_SIZE;
#ifdef BCM_WARM_BOOT_SUPPORT
        incr_size = alloc_size - allocated_size;

        rc = soc_scache_realloc(unit, handle, incr_size);
        if (rc != SOC_E_NONE) {
            return(rc);
        }

        /* get the new pointer */
        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if (rc != SOC_E_NONE) {
            return(rc);
        }

        if (already_exists != NULL) {
            (*already_exists) = TRUE;
        }
#else /*BCM_WARM_BOOT_SUPPORT*/
        /* remove old buffer (supplied by caller) */
        /* we can consider holding a linked list inside this utils module
           instead of geting the old pointer from the user */
        if (*scache_ptr==NULL) {
             _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                   (BSL_META_U(unit,
                      "Unit:%d scache_ptr is null.\n"), unit));
            return SOC_E_INTERNAL;
        }
        else{
            sal_free(*scache_ptr - SHR_SW_STATE_SCACHE_CONTROL_SIZE);
        }
        /* alloc buffer here instead of in scache */
        *scache_ptr = sal_alloc(alloc_size, "shr_sw_state_buffer_info");
        if (*scache_ptr == NULL) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
               (BSL_META_U(unit,
                  "Unit:%d Memory allocation failure.\n"), unit));
            /* return SOC_E_MEMORY; */
        }
        allocated_size = alloc_size;
#endif /*BCM_WARM_BOOT_SUPPORT*/
    }
    else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
           (BSL_META_U(unit,
              "Unit:%d Invalid parameter.\n"), unit));
       /* return(SOC_E_PARAM); */
    }

    /* Advance over scache control info */
    (*scache_ptr) += SHR_SW_STATE_SCACHE_CONTROL_SIZE;
    (*size) = (allocated_size - SHR_SW_STATE_SCACHE_CONTROL_SIZE); /* update size */

    return(rc);  

exit:
SOC_FUNC_RETURN; /* added for _SOC_EXIT_WITH_ERR */

}

int shr_sw_state_scache_sync(int unit, soc_scache_handle_t handle, int offset, int size)
{
    SOC_INIT_FUNC_DEFS;
#ifdef BCM_WARM_BOOT_SUPPORT
    _SOC_IF_ERR_EXIT(soc_scache_partial_commit(unit, handle, offset + SHR_SW_STATE_SCACHE_CONTROL_SIZE, size));
#endif
    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}





int shr_sw_state_ds_layout_node_t_clear(shr_sw_state_ds_layout_node_t *node) {
    SOC_INIT_FUNC_DEFS;
    sal_memset(node, 0x0, sizeof(shr_sw_state_ds_layout_node_t)); 


    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

int shr_sw_state_ds_layout_node_set(int unit, 
                                    int node_id, 
                                    char* name, 
                                    int size, 
                                    int nof_pointer,
                                    char* type, 
                                    int array_size_0, 
                                    int array_size_1) {

    shr_sw_state_ds_layout_node_t* ds_layout_node = &(shr_sw_state_data_block_header[unit]->ds_layout_nodes[node_id]);
    SOC_INIT_FUNC_DEFS;
    SW_STATE_NODE_ID_CHECK(unit, node_id); 

    /* clear the current node */
    /*init the data to be all zeros*/
    sal_memset(ds_layout_node, 0x0, sizeof(shr_sw_state_ds_layout_node_t));

    if (sal_strlen(name) < sizeof(ds_layout_node->name)) {
        sal_strncpy(ds_layout_node->name, name, sizeof(ds_layout_node->name)-1); 
    } 
    /* name size is higher than node name array.
       Make sure we have a null character (don't copy the last character), so it's stay 0 */
    else {
        sal_strncpy(ds_layout_node->name, name, sizeof(ds_layout_node->name) -1 ); 
    }

    if (sal_strlen(type) < sizeof(ds_layout_node->type)) {
        sal_strncpy(ds_layout_node->type, type, sizeof(ds_layout_node->type)-1); 
    } 
    /* type size (nof character) is higher than node type array.
       Make sure we have a null character (don't copy the last character), so it's stay 0 */
    else {
        sal_strncpy(ds_layout_node->type, type, sizeof(ds_layout_node->type) -1 ); 
    }
   ds_layout_node->size = size;                   
   ds_layout_node->nof_pointer = nof_pointer;     
   ds_layout_node->array_sizes[0] = array_size_0; 
   ds_layout_node->array_sizes[1] = array_size_1; 
                                                               
    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

int shr_sw_state_ds_layout_add_brother(int unit, int older_brother_node_id, int younger_brother_node_id) {

    shr_sw_state_ds_layout_node_t* ds_layout_older_brother_node; 

    SOC_INIT_FUNC_DEFS;


    SW_STATE_NODE_ID_CHECK(unit, older_brother_node_id);  
    SW_STATE_NODE_ID_CHECK(unit, younger_brother_node_id);

    /* get older brother node */
     ds_layout_older_brother_node = &(shr_sw_state_data_block_header[unit]->ds_layout_nodes[older_brother_node_id]);  
    /* set the younger brother */
     ds_layout_older_brother_node->next_brother_node_index = younger_brother_node_id;  

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

int shr_sw_state_ds_layout_add_child(int unit, int parent_node_id, int child_node_id) {

    shr_sw_state_ds_layout_node_t* ds_layout_parent_node; 

    SOC_INIT_FUNC_DEFS;


    SW_STATE_NODE_ID_CHECK(unit, parent_node_id); 
    SW_STATE_NODE_ID_CHECK(unit, child_node_id);  

    /* get parent node */
    ds_layout_parent_node = &(shr_sw_state_data_block_header[unit]->ds_layout_nodes[parent_node_id]);   

    /* set the elder child */
    if (ds_layout_parent_node->first_child_node_index == 0) {
        ds_layout_parent_node->first_child_node_index = child_node_id;  
    } 
    /* add brother to the last child */
    else {
        _SOC_IF_ERR_EXIT(shr_sw_state_ds_layout_add_brother(unit, ds_layout_parent_node->last_child_node_index, child_node_id)); 
    }
    /* set the last child */
    ds_layout_parent_node->last_child_node_index = child_node_id; 

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}



int shr_sw_state_ds_layout_update_offset(int unit, int node_id, int offset) {

    shr_sw_state_ds_layout_node_t* ds_layout_node; 

    SOC_INIT_FUNC_DEFS;

    SW_STATE_NODE_ID_CHECK(unit, node_id); 

    /* get node */
    ds_layout_node = &(shr_sw_state_data_block_header[unit]->ds_layout_nodes[node_id]); 
    /* update offset */
    ds_layout_node->offset = offset; 

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}



#undef _ERR_MSG_MODULE_NAME
