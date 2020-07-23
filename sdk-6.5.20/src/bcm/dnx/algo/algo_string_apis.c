/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR

#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include <soc/sand/sand_signals.h>
#include "src/soc/dnx/dbal/dbal_internal.h"

shr_error_e
dnx_cint_algo_l3_rif_allocate_generic(
    int unit,
    int rif_id,
    char *rif_result_type_name,
    char *table_name)
{
    dbal_enum_value_result_type_eedb_rif_basic_e rif_result_type;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, table_name, &table_id));
    SHR_IF_ERR_EXIT(dbal_result_type_string_to_id(unit, table_id, rif_result_type_name, (uint32 *) (&rif_result_type)));
    SHR_IF_ERR_EXIT(dnx_algo_l3_rif_allocate_generic(unit, rif_id, rif_result_type, table_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_cint_algo_l3_fec_allocate(
    int unit,
    int *fec_index,
    uint32 flags,
    char *hierarchy_name,
    char *fec_resource_type_name)
{
    dbal_field_types_basic_info_t *field_info;
    dbal_enum_value_field_hierarchy_level_e hierarchy = DBAL_NOF_ENUM_HIERARCHY_LEVEL_VALUES;
    dbal_enum_value_field_fec_resource_type_e fec_resource_type = DBAL_NOF_ENUM_FEC_RESOURCE_TYPE_VALUES;
    uint8 enum_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, DBAL_FIELD_HIERARCHY_LEVEL, &field_info));

    for (enum_index = 0; enum_index < field_info->nof_enum_values; enum_index++)
    {
        if (sal_strcasecmp(hierarchy_name, field_info->enum_val_info[enum_index].name) == 0)
        {
            hierarchy = enum_index;
            break;
        }
    }
    if (enum_index == field_info->nof_enum_values)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "%s field %s enum val wasn't found \n", field_info->name, hierarchy_name);
    }

    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, DBAL_FIELD_FEC_RESOURCE_TYPE, &field_info));

    for (enum_index = 0; enum_index < field_info->nof_enum_values; enum_index++)
    {
        if (sal_strcasecmp(fec_resource_type_name, field_info->enum_val_info[enum_index].name) == 0)
        {
            fec_resource_type = enum_index;
            break;
        }
    }
    if (enum_index == field_info->nof_enum_values)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "%s field %s enum val wasn't found \n", field_info->name, fec_resource_type_name);
    }

    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_allocate(unit, fec_index, flags, hierarchy, fec_resource_type));

exit:
    SHR_FUNC_EXIT;
}
