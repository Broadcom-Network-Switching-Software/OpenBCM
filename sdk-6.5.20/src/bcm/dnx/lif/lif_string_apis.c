/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LIF

#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr_api.h>
#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include <soc/sand/sand_signals.h>

shr_error_e
dnx_cint_lif_lib_allocate(
    int unit,
    uint32 flags,
    int *global_lif,
    lif_mngr_local_inlif_str_info_t * inlif_str_info,
    lif_mngr_local_outlif_str_info_t * outlif_str_info)
{
    lif_mngr_local_inlif_info_t inlif_info = { 0 };
    lif_mngr_local_outlif_info_t outlif_info = { 0 };

    lif_mngr_local_inlif_info_t *inlif_info_ptr = NULL;
    lif_mngr_local_outlif_info_t *outlif_info_ptr = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (inlif_str_info != NULL)
    {
        SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, inlif_str_info->table_name, &inlif_info.dbal_table_id));
        SHR_IF_ERR_EXIT(dbal_result_type_string_to_id
                        (unit, inlif_info.dbal_table_id, inlif_str_info->result_type_name,
                         (uint32 *) (&inlif_info.dbal_result_type)));
        inlif_info.core_id = inlif_str_info->core_id;
        inlif_info.local_inlif = inlif_str_info->local_inlif;
        inlif_info_ptr = &inlif_info;
    }

    if (outlif_str_info != NULL)
    {
        sal_memset(&outlif_info, 0, sizeof(outlif_info));
        SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, outlif_str_info->table_name, &outlif_info.dbal_table_id));
        SHR_IF_ERR_EXIT(dbal_result_type_string_to_id
                        (unit, outlif_info.dbal_table_id, outlif_str_info->result_type_name,
                         (uint32 *) (&outlif_info.dbal_result_type)));
        SHR_IF_ERR_EXIT(dnx_lif_mngr_phase_string_to_id
                        (unit, outlif_str_info->outlif_phase_name, &outlif_info.outlif_phase));

        outlif_info.local_lif_flags = outlif_str_info->local_lif_flags;
        outlif_info.local_outlif = outlif_str_info->local_outlif;
        outlif_info_ptr = &outlif_info;
    }

    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, flags, global_lif, inlif_info_ptr, outlif_info_ptr));

    if (inlif_str_info != NULL)
    {
        inlif_str_info->local_inlif = inlif_info_ptr->local_inlif;
    }

    if (outlif_str_info != NULL)
    {
        outlif_str_info->local_outlif = outlif_info_ptr->local_outlif;
    }

exit:
    SHR_FUNC_EXIT;
}
