/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * In order to add a new callback:
 * 1. Add the callback here and to dbal_formula_cb.h file
 *      a. The callback structure (dnx_dbal_formula_offset_cb) can be found at dbal_structures.h
 *      b. It receives the unit and the entry handle id as input, and outputs the calculated offset
 *      c. In order to access the entry's fields (keys and results), use the following macros:
 *          DBAL_FORMULA_CB_GET_FULL_KEY_SIZE(unit, entry_handle_id, size) - full key size
 *          DBAL_FORMULA_CB_GET_KEY_FIELD_SIZE(unit, entry_handle_id, field_id, size) - specific key size
 *          DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, entry_handle_id, key_value) - equivalent to formula KEY
 *          DBAL_FORMULA_CB_GET_KEY_VALUE(unit, entry_handle_id, field_id, entry_value) - specific key value
 *          DBAL_FORMULA_CB_GET_FULL_RESULT_SIZE(unit, entry_handle_id, result_size) - equivalent to formula RESULT
 *          DBAL_FORMULA_CB_GET_RESULT_FIELD_SIZE(unit, entry_handle_id, field_id, size) - specific result field size
 *      d. The entry_handle provided is already allocated and linked to the relevant entry
 * 2. During init stage, the dbal will scan the .xmls and will register the callbacks to their matching tables
 * 3. Upon entry's access, the callback will be called
 * 4. For reference, see example_formula_cb() below (and in the .h file). EXAMPLE_TABLE_REGISTER_GROUP field DESTINATION_ENCODING uses it
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

#include <sal/core/libc.h>
#include <sal/core/alloc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <sal/types.h>
#include <shared/bsl.h>
#include <shared/util.h>
#include <shared/shrextend/shrextend_error.h>
#include <bcm/error.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include "dbal_formula_cb.h"
#include "dbal_internal.h"

shr_error_e
instance_multiplied_by_result_field_size_cb(
    int unit,
    void *entry_handle,
    dbal_fields_e current_mapped_field_id,
    uint32 *offset)
{

    uint32 instance_idx;
    int32 size;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);

    DBAL_FORMULA_CB_GET_RESULT_FIELD_SIZE(unit, entry_handle, current_mapped_field_id, size);
    
    *offset = instance_idx * size;

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
example_formula_cb(
    int unit,
    void *entry_handle,
    dbal_fields_e current_mapped_field_id,
    uint32 *offset)
{
    uint32 default_ac_prof;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, entry_handle, DBAL_FIELD_DEFAULT_AC_PROF, default_ac_prof);
    *offset = default_ac_prof * 3;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
imb_fc_lane_in_core_to_internal_lane_in_ethu_cb(
    int unit,
    void *entry_handle,
    dbal_fields_e current_mapped_field_id,
    uint32 *offset)
{
    uint32 core_id, lane_in_core, global_lane, first_lane;
    int ethu_id, pm_index, index;
    bcm_pbmp_t pm_phys;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, entry_handle, DBAL_FIELD_CORE_ID, core_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, entry_handle, DBAL_FIELD_LANE_IN_CORE, lane_in_core);

    
    global_lane = lane_in_core + dnx_data_nif.phys.nof_phys_per_core_get(unit) * core_id;

    
    index = global_lane / dnx_data_nif.eth.phy_map_granularity_get(unit);
    ethu_id = dnx_data_nif.eth.phy_map_get(unit, index)->ethu_index;

    
    pm_index = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->pms[0];
    pm_phys = dnx_data_nif.eth.pm_properties_get(unit, pm_index)->phys;
    _SHR_PBMP_FIRST(pm_phys, first_lane);

    
    *offset = global_lane - first_lane;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
imb_fc_lane_in_core_to_ethu_type_index_cb(
    int unit,
    void *entry_handle,
    dbal_fields_e current_mapped_field_id,
    uint32 *offset)
{
    uint32 core_id, lane_in_core, global_lane;
    int ethu_id, type_index, index;
    int nof_cdums = 0;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, entry_handle, DBAL_FIELD_CORE_ID, core_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, entry_handle, DBAL_FIELD_LANE_IN_CORE, lane_in_core);

    
    global_lane = lane_in_core + dnx_data_nif.phys.nof_phys_per_core_get(unit) * core_id;

    
    index = global_lane / dnx_data_nif.eth.phy_map_granularity_get(unit);
    ethu_id = dnx_data_nif.eth.phy_map_get(unit, index)->ethu_index;

    
    type_index = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type_index;

    
    if (dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type == imbDispatchTypeImb_cdu)
    {
        nof_cdums =
            (core_id ==
             0) ? (dnx_data_nif.eth.cdum_nof_get(unit) /
                   dnx_data_device.general.nof_cores_get(unit)) : dnx_data_nif.eth.cdum_nof_get(unit);
    }
    *offset = type_index - nof_cdums;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
imb_fc_ethu_id_to_ethu_type_index_cb(
    int unit,
    void *entry_handle,
    dbal_fields_e current_mapped_field_id,
    uint32 *offset)
{
    uint32 core_id, ethu_id_in_core, nof_ethus_per_core;
    int ethu_id, type_index;
    int nof_cdums = 0;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, entry_handle, DBAL_FIELD_CORE_ID, core_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, entry_handle, DBAL_FIELD_ETHU_ID, ethu_id_in_core);

    
    nof_ethus_per_core = dnx_data_nif.eth.ethu_nof_get(unit) / dnx_data_device.general.nof_cores_get(unit);
    ethu_id = core_id * nof_ethus_per_core + ethu_id_in_core;

    
    type_index = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type_index;

    
    if (dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type == imbDispatchTypeImb_cdu)
    {
        nof_cdums =
            (core_id ==
             0) ? (dnx_data_nif.eth.cdum_nof_get(unit) /
                   dnx_data_device.general.nof_cores_get(unit)) : dnx_data_nif.eth.cdum_nof_get(unit);
    }
    *offset = type_index - nof_cdums;

exit:
    SHR_FUNC_EXIT;
}

offset_cb_from_str_t formula_offset_cb_map[] = {
    {"example", example_formula_cb}
    ,
    {"lane_in_core_to_internal_lane_in_ethu", imb_fc_lane_in_core_to_internal_lane_in_ethu_cb}
    ,
    {"lane_in_core_to_ethu_type_index", imb_fc_lane_in_core_to_ethu_type_index_cb}
    ,
    {"ethu_id_to_ethu_type_index", imb_fc_ethu_id_to_ethu_type_index_cb}
    ,
    {"instance_multiplied_by_result_field_size_cb", instance_multiplied_by_result_field_size_cb}
    ,
    {DBAL_FORMULA_CB_LAST_CB, NULL}
    ,                                     
};




shr_error_e
illegal_value_cb_example(
    int unit,
    uint32 value,
    uint8 *is_illegal)
{
    SHR_FUNC_INIT_VARS(unit);

    (*is_illegal) = 1;

    if (value == 0 || value == 3)
    {
        (*is_illegal) = 0;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_clu_logical_port_illegal_value_get(
    int unit,
    uint32 value,
    uint8 *is_illegal)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    dnx_algo_port_info_s port_info;
    int is_valid;

    SHR_FUNC_INIT_VARS(unit);

    (*is_illegal) = 1;

    
    SHR_IF_ERR_EXIT(dnx_algo_port_is_valid_get(unit, value, &is_valid));
    if (is_valid == TRUE)
    {
        
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, value, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, TRUE, TRUE))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, value, &ethu_info));
            if (ethu_info.imb_type == imbDispatchTypeImb_clu)
            {
                (*is_illegal) = 0;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_field_types_init_illegal_value_cb_assign(
    int unit,
    dbal_field_types_defs_e field_type,
    dbal_field_types_basic_info_t * field_types_info)
{

    SHR_FUNC_INIT_VARS(unit);

    switch (field_type)
    {
        case DBAL_FIELD_TYPE_DEF_FIELD_WITH_ILLEGAL_VALUES_CB:
            field_types_info->illegal_value_cb = illegal_value_cb_example;
            break;
        case DBAL_FIELD_TYPE_DEF_CLU_LOGICAL_PORT:
            field_types_info->illegal_value_cb = dbal_clu_logical_port_illegal_value_get;
            break;

        default:
            break;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}
