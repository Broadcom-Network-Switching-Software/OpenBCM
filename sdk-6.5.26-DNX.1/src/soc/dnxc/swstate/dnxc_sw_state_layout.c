
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */



#include <shared/bsl.h>
#include <soc/types.h>
#include <sal/core/libc.h>
#include <shared/util.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnxc_sw_state_layout.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>
#include <soc/dnxc/swstate/auto_generated/access/dnxc_module_ids_access.h>
#include <soc/dnxc/swstate/dnxc_sw_state_common.h>


#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#define LAYOUT_ALLOC_COPY_STRING(unit, param, node_id) {\
    data = param;\
    length = sal_strnlen(data, 2000)+1;\
    param = NULL;\
    DNX_SW_STATE_ALLOC(unit, node_id, param, char, length, DNXC_SW_STATE_NONE, "dnxc_sw_state_layout_alloc_string");\
    sal_strncpy(param, data, length);\
}


int dnxc_sw_state_node_id_verify(
    int unit,
    uint32 module_id,
    uint32 param_idx)
{
    SHR_FUNC_INIT_VARS(unit);

    if (module_id == DNXC_SW_STATE_LAYOUT_INVALID_MODULE_ID || module_id > NOF_MODULE_ID)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_EMPTY, "Trying to access invalid module id = %d and param index = %d \n%s", module_id, param_idx, EMPTY);
    }

    if (param_idx == DNXC_SW_STATE_LAYOUT_INVALID_PARAM_IDX)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_EMPTY, "Trying to access invalid module id = %d and param index = %d \n%s", module_id, param_idx, EMPTY);
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}


int dnxc_sw_state_layout_init_structure(
    int unit,
    uint32 node_id,
    dnxc_sw_state_layout_t * layout_struct,
    dnxc_sw_state_layout_t * layout_address,
    uint32 nof_params)
{
    char *data;
    int idx, length, node_ids = 0;
    SHR_FUNC_INIT_VARS(unit);

    DNX_SW_STATE_MEMCPY_BASIC(unit, node_id, layout_address, layout_struct, sizeof(*layout_struct) * DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(nof_params), DNXC_SW_STATE_NONE, "dnxc_sw_state_layout_init_structure");

    for (idx = 0; idx < DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(nof_params); idx++)
    {
        node_ids = node_id + idx;

        if (layout_address[idx].name != NULL){
            LAYOUT_ALLOC_COPY_STRING(unit, layout_address[idx].name, node_ids);
        }

        if (layout_address[idx].type != NULL){
            LAYOUT_ALLOC_COPY_STRING(unit, layout_address[idx].type, node_ids);
        }

        if (layout_address[idx].doc != NULL){
            LAYOUT_ALLOC_COPY_STRING(unit, layout_address[idx].doc, node_ids);
        }

        for (int dimension = 0 ; dimension < DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS; dimension++)
        {
            if (layout_address[idx].array_indexes[dimension].name != NULL){
                LAYOUT_ALLOC_COPY_STRING(unit, layout_address[idx].array_indexes[dimension].name, node_ids);
            }
        }
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}


int dnxc_sw_state_layout_node_array_size_update(
    int unit,
    uint32 node_id,
    dnxc_sw_state_layout_t * layout_address,
    uint32 nof_instances_to_alloc,
    uint8 dimension)
{
    uint32 param_idx = DNXC_SW_STATE_LAYOUT_INVALID;
    SHR_FUNC_INIT_VARS(unit);

    param_idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(node_id);
    
    if (param_idx > 0 && layout_address != NULL && dimension < (DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1))
    {
        if(dimension == DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS)
        {
            
            for (dimension = 0; dimension < DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS; dimension++)
            {
                if (layout_address[param_idx].array_indexes[dimension].num_elements == DNXC_SW_STATE_LAYOUT_INVALID
                    && layout_address[param_idx].array_indexes[dimension].index_type !=  DNXC_SWSTATE_ARRAY_INDEX_INVALID)
                {
                    layout_address[param_idx].array_indexes[dimension].num_elements = nof_instances_to_alloc;
                    break;
                }
            }
            
            layout_address[param_idx].labels |= DNXC_SW_STATE_LAYOUT_LABEL_ARRAY_INDEX_NOT_EQUAL_SIZE;
        }
        else if (layout_address[param_idx].array_indexes[dimension].num_elements != nof_instances_to_alloc
            && layout_address[param_idx].array_indexes[dimension].num_elements != DNXC_SW_STATE_LAYOUT_INVALID)
        {
            layout_address[param_idx].labels |= DNXC_SW_STATE_LAYOUT_LABEL_ARRAY_INDEX_NOT_EQUAL_SIZE;
        }
        else
        {
            layout_address[param_idx].array_indexes[dimension].num_elements = nof_instances_to_alloc;
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


int dnxc_sw_state_layout_node_sum_of_allocation_update(
    int unit,
    uint32 node_id,
    dnxc_sw_state_layout_t * layout_address,
    uint32 alloc_size,
    uint8 increment)
{
    uint32 param_idx = DNXC_SW_STATE_LAYOUT_INVALID;
    uint32 module_id = DNXC_SW_STATE_LAYOUT_INVALID;
    SHR_FUNC_INIT_VARS(unit);

    param_idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(node_id);
    module_id = DNXC_SW_STATE_LAYOUT_GET_MODULE_ID(node_id);
    if (layout_address != NULL)
    {
        dnxc_sw_state_node_id_verify(unit, module_id, param_idx);
        
        if (increment){
            if (layout_address[param_idx].sum_of_allocation!=DNXC_SW_STATE_LAYOUT_INVALID)
            {
                layout_address[param_idx].sum_of_allocation += alloc_size;
            }
            else
            {
                layout_address[param_idx].sum_of_allocation = alloc_size;
            }
        }
        else
        {
            layout_address[param_idx].sum_of_allocation -= alloc_size;
        }
    }
    else
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_EMPTY, "Trying to update the total size of module = %s and param_idx %d with layout NULL address.\n%s", dnxc_module_id_e_get_name(module_id), param_idx, EMPTY);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


int dnx_swstate_layout_node_last_child_index_get(
    int unit,
    dnxc_sw_state_layout_t * layout_address,
    uint32 param_idx,
    int * end_idx)
{
    int current_idx = 0;
    uint8 hasNext = TRUE, firstEntryInLastNode = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    current_idx = *end_idx = param_idx;
    while (hasNext)
    {
        if (layout_address[current_idx].last_child_index == DNXC_SW_STATE_LAYOUT_INVALID
            && *end_idx == param_idx)
        {
            hasNext = FALSE;
        }
        else if (layout_address[current_idx].last_child_index > *end_idx
                && layout_address[current_idx].last_child_index != DNXC_SW_STATE_LAYOUT_INVALID)
        {
            *end_idx = layout_address[current_idx].last_child_index;
            current_idx = *end_idx;
        }
        else if (layout_address[current_idx].last_child_index == DNXC_SW_STATE_LAYOUT_INVALID)
        {
            if (current_idx == layout_address[param_idx].last_child_index
                && firstEntryInLastNode)
            {
                firstEntryInLastNode = FALSE;
            }
            else if ((!firstEntryInLastNode && current_idx == layout_address[param_idx].last_child_index)
                    || current_idx == layout_address[param_idx].first_child_index)
            {
                hasNext = FALSE;
                break;
            }
            current_idx -= 1;
        }
        else if (layout_address[current_idx].last_child_index <= *end_idx
                    && (current_idx > layout_address[param_idx].last_child_index
                        || current_idx == layout_address[param_idx].last_child_index
                        || current_idx == param_idx))
        {
            hasNext = FALSE;
        }
    }

    *end_idx += 1;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


int dnxc_sw_state_layout_total_size_get(
    int unit,
    uint32 node_id,
    uint32 * total_size)
{
    uint32 param_idx = DNXC_SW_STATE_LAYOUT_INVALID;
    uint32 module_id = DNXC_SW_STATE_LAYOUT_INVALID;
    int index = 0, end_idx = 0;
    dnxc_sw_state_layout_t * layout_address = NULL;

    SHR_FUNC_INIT_VARS(unit);

    param_idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(node_id);
    module_id = DNXC_SW_STATE_LAYOUT_GET_MODULE_ID(node_id);
    layout_address = sw_state_layout_array[unit][module_id];

    SHR_NULL_CHECK(layout_address, _SHR_E_PARAM, "layout_address");
    dnxc_sw_state_node_id_verify(unit, module_id, param_idx);
    if (layout_address[param_idx].sum_of_allocation!=DNXC_SW_STATE_LAYOUT_INVALID)
    {
        *total_size += layout_address[param_idx].sum_of_allocation;
    }
    

    if(layout_address[param_idx].first_child_index != DNXC_SW_STATE_LAYOUT_INVALID
        && layout_address[param_idx].last_child_index != DNXC_SW_STATE_LAYOUT_INVALID)
    {
        dnx_swstate_layout_node_last_child_index_get(unit, layout_address, param_idx, &end_idx);
        for (index = layout_address[param_idx].first_child_index; index < end_idx; index++)
        {
            if (layout_address[index].sum_of_allocation!=DNXC_SW_STATE_LAYOUT_INVALID)
            {
                *total_size += layout_address[index].sum_of_allocation;
            }
            
        }
    }

    

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


int dnxc_sw_state_layout_flags_get(
    int unit,
    char *flag_str,
    uint32 node_id)
{
    dnxc_sw_state_layout_t * layout_address;
    uint32 param_idx = DNXC_SW_STATE_LAYOUT_INVALID;
    uint32 module_id = DNXC_SW_STATE_LAYOUT_INVALID;

    SHR_FUNC_INIT_VARS(unit);

    param_idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(node_id);
    module_id = DNXC_SW_STATE_LAYOUT_GET_MODULE_ID(node_id);
    dnxc_sw_state_node_id_verify(unit, module_id, param_idx);

    layout_address = sw_state_layout_array[unit][module_id];
    if (!_SHR_IS_FLAG_SET(layout_address[param_idx].labels, DNXC_SW_STATE_LAYOUT_LABEL_NULL)
            && layout_address[param_idx].labels!=0)
    {
        if (_SHR_IS_FLAG_SET(layout_address[param_idx].labels, DNXC_SW_STATE_LAYOUT_LABEL_ACCESS_PTR))
        {
            utilex_str_append(flag_str,"DNXC_SW_STATE_LAYOUT_LABEL_ACCESS_PTR,");
        }
        if (_SHR_IS_FLAG_SET(layout_address[param_idx].labels, DNXC_SW_STATE_LAYOUT_LABEL_COUNTER))
        {
            utilex_str_append(flag_str,"DNXC_SW_STATE_LAYOUT_LABEL_COUNTER,");
        }
        if (_SHR_IS_FLAG_SET(layout_address[param_idx].labels, DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_EXCEPTION))
        {
            utilex_str_append(flag_str,"DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_EXCEPTION,");
        }
        if (_SHR_IS_FLAG_SET(layout_address[param_idx].labels, DNXC_SW_STATE_LAYOUT_LABEL_ARRAY_RANGE_FUNCTIONS))
        {
            utilex_str_append(flag_str,"DNXC_SW_STATE_LAYOUT_LABEL_ARRAY_RANGE_FUNCTIONS,");
        }
        if (_SHR_IS_FLAG_SET(layout_address[param_idx].labels, DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_AFTER_INIT))
        {
            utilex_str_append(flag_str,"DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_AFTER_INIT,");
        }
        if (_SHR_IS_FLAG_SET(layout_address[param_idx].labels, DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP))
        {
            utilex_str_append(flag_str,"DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP,");
        }
        if (_SHR_IS_FLAG_SET(layout_address[param_idx].labels, DNXC_SW_STATE_LAYOUT_LABEL_GENERATE_FREE))
        {
            utilex_str_append(flag_str,"DNXC_SW_STATE_LAYOUT_LABEL_GENERATE_FREE,");
        }
        if (_SHR_IS_FLAG_SET(layout_address[param_idx].labels, DNXC_SW_STATE_LAYOUT_LABEL_GENERATE_IS_ALLOC))
        {
            utilex_str_append(flag_str,"DNXC_SW_STATE_LAYOUT_LABEL_GENERATE_IS_ALLOC,");
        }
        if (_SHR_IS_FLAG_SET(layout_address[param_idx].labels, DNXC_SW_STATE_LAYOUT_LABEL_ATTRIBUTE_PACKED))
        {
            utilex_str_append(flag_str,"DNXC_SW_STATE_LAYOUT_LABEL_ATTRIBUTE_PACKED,");
        }
        if (_SHR_IS_FLAG_SET(layout_address[param_idx].labels, DNXC_SW_STATE_LAYOUT_LABEL_UNION))
        {
            utilex_str_append(flag_str,"DNXC_SW_STATE_LAYOUT_LABEL_UNION,");
        }
        if (_SHR_IS_FLAG_SET(layout_address[param_idx].labels, DNXC_SW_STATE_LAYOUT_LABEL_LEAF_STRUCT))
        {
            utilex_str_append(flag_str,"DNXC_SW_STATE_LAYOUT_LABEL_LEAF_STRUCT,");
        }
        if (_SHR_IS_FLAG_SET(layout_address[param_idx].labels, DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION))
        {
            utilex_str_append(flag_str,"DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION,");
        }
        if (_SHR_IS_FLAG_SET(layout_address[param_idx].labels, DNXC_SW_STATE_LAYOUT_LABEL_EXPANDED_INTERFACE))
        {
           utilex_str_append(flag_str,"DNXC_SW_STATE_LAYOUT_LABEL_EXPANDED_INTERFACE,");
        }
        if (_SHR_IS_FLAG_SET(layout_address[param_idx].labels, DNXC_SW_STATE_LAYOUT_LABEL_DIFF_ALLOC_SIZES))
        {
            utilex_str_append(flag_str,"DNXC_SW_STATE_LAYOUT_LABEL_DIFF_ALLOC_SIZES,");
        }
        if (_SHR_IS_FLAG_SET(layout_address[param_idx].labels, DNXC_SW_STATE_LAYOUT_LABEL_ALLOW_WRITES_DURING_WB))
        {
           utilex_str_append(flag_str,"DNXC_SW_STATE_LAYOUT_LABEL_ALLOW_WRITES_DURING_WB,");
        }
        if (_SHR_IS_FLAG_SET(layout_address[param_idx].labels, DNXC_SW_STATE_LAYOUT_LABEL_ARRAY_INDEX_NOT_EQUAL_SIZE))
        {
            utilex_str_append(flag_str,"DNXC_SW_STATE_LAYOUT_LABEL_ARRAY_INDEX_NOT_EQUAL_SIZE,");
        }
    }
    else
    {
        utilex_str_append(flag_str,"NONE,");
    }
    utilex_str_white_spaces_remove(flag_str);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
