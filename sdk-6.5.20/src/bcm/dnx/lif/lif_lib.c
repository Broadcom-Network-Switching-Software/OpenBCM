/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */



#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LIF




#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/dbal_dynamic.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/mdb/mdb_init.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/swstate/auto_generated/access/algo_lif_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>





#define LIF_LIB_LOCAL_OUTLIF_PHASE_MAP_BANK_SIZE      (dnx_data_lif.out_lif.physical_bank_size_get(unit))


#define LIF_LIB_LOCAL_OUTLIF_BANK_TO_PHASE_MAP_RATIO  (dnx_data_lif.out_lif.allocation_bank_size_get(unit) \
                                                        / LIF_LIB_LOCAL_OUTLIF_PHASE_MAP_BANK_SIZE)






#define LIF_LIB_LOCAL_OUTLIF_TO_PHASE_MAP_BANK(_local_outlif) (_local_outlif / LIF_LIB_LOCAL_OUTLIF_PHASE_MAP_BANK_SIZE)













static shr_error_e
dnx_lif_lib_outlif_bank_map_change(
    int unit,
    int local_outlif,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_info,
    int delete_0_add_1)
{
    int update_ll, update_data;
    uint32 entry_handle_id;
    int current_outlif_phase_map_bank, last_outlif_phase_map_bank;

    shr_error_e(*mdb_init_func_cb) (int, int, mdb_cluster_alloc_info_t *);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    update_ll = _SHR_IS_FLAG_SET(outlif_hw_info->flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_LL_BANK);
    update_data = _SHR_IS_FLAG_SET(outlif_hw_info->flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_DATA_BANK);
    mdb_init_func_cb = (delete_0_add_1) ? mdb_init_add_table_resources : mdb_init_delete_table_resources;
    
    if (update_data)
    {
        SHR_IF_ERR_EXIT(mdb_init_func_cb(unit, 1, &outlif_hw_info->data_bank_info));
    }

    if (update_ll)
    {
        SHR_IF_ERR_EXIT(mdb_init_func_cb(unit, 1, &outlif_hw_info->ll_bank_info));
    }

    

    
    if (_SHR_IS_FLAG_SET(outlif_hw_info->flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_POINTERS))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_7, &entry_handle_id));

        
        current_outlif_phase_map_bank =
            (LIF_LIB_LOCAL_OUTLIF_BANK_TO_PHASE_MAP_RATIO) * (LIF_LIB_LOCAL_OUTLIF_TO_PHASE_MAP_BANK(local_outlif) /
                                                              (LIF_LIB_LOCAL_OUTLIF_BANK_TO_PHASE_MAP_RATIO));
        last_outlif_phase_map_bank = current_outlif_phase_map_bank + LIF_LIB_LOCAL_OUTLIF_BANK_TO_PHASE_MAP_RATIO;

        for (; current_outlif_phase_map_bank < last_outlif_phase_map_bank; current_outlif_phase_map_bank++)
        {
            
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_KEY_0, current_outlif_phase_map_bank);

            
            if (delete_0_add_1)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE,
                                             outlif_hw_info->logical_phase);
            }
            else
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE,
                                             DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_FIRST);
            }

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_lif_lib_init_phase_mapping(
    int unit)
{
    uint32 entry_handle_id;
    uint8 current_logical_phase;
    uint8 current_physical_phase;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_10, &entry_handle_id));

    
    for (current_logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_FIRST;
         current_logical_phase < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT; current_logical_phase++)
    {
        
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_logical_phase_to_physical_phase(unit,
                                                                              current_logical_phase,
                                                                              &current_physical_phase));
        
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_2, current_physical_phase,
                                    current_logical_phase);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, current_physical_phase,
                                    current_logical_phase);

        
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_3,
                                    current_logical_phase, current_physical_phase);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_1, current_logical_phase,
                                    current_physical_phase);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_lif_lib_init_rif(
    int unit)
{
    dnx_algo_local_outlif_hw_info_t *outlif_hw_infos = NULL;
    int outrif_bank_starts[MDB_MAX_NOF_CLUSTERS];
    int nof_outlif_infos, current_outlif_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(outlif_hw_infos, sizeof(dnx_algo_local_outlif_hw_info_t) * MDB_MAX_NOF_CLUSTERS,
                       "outlif_hw_infos", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (dnx_data_l3.rif.nof_rifs_get(unit) > 0)
    {
        
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_rif_init(unit));

        
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_outrif_physical_bank_hw_info_get(unit, outrif_bank_starts,
                                                                               outlif_hw_infos, &nof_outlif_infos));

        for (current_outlif_info = 0; current_outlif_info < nof_outlif_infos; current_outlif_info++)
        {
            SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_bank_map_change(unit, outrif_bank_starts[current_outlif_info],
                                                               &outlif_hw_infos[current_outlif_info], 1));
        }
    }

exit:
    SHR_FREE(outlif_hw_infos);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_lif_lib_outlif_mdb_clusters_init(
    int unit)
{
    mdb_cluster_alloc_info_t init_clusters_info[MDB_MAX_NOF_CLUSTERS];
    int nof_init_clusters;

    SHR_FUNC_INIT_VARS(unit);

    
    sal_memset(init_clusters_info, 0, sizeof(init_clusters_info));

    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_mdb_bank_database_init(unit, init_clusters_info, &nof_init_clusters));

    SHR_IF_ERR_EXIT(mdb_init_add_table_resources(unit, nof_init_clusters, init_clusters_info));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_lif_lib_global_lif_type_get(
    int unit,
    uint32 global_lif,
    int *is_rif)
{
    int iop_mode_outlif_selection, system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);

    iop_mode_outlif_selection = dnx_data_lif.feature.feature_get(unit, dnx_data_lif_feature_iop_mode_outlif_selection);
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    if (((global_lif < dnx_data_l3.rif.nof_rifs_get(unit))
         && (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE))
        || ((global_lif < dnx_data_l3.rif.nof_rifs_get(unit)) && (iop_mode_outlif_selection == FALSE)))
    {
        (*is_rif) = 1;
    }
    else
    {
        (*is_rif) = 0;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_lif_lib_outlif_traffic_lock_bug_workaround(
    int unit)
{
    int outlif_physical_phase_by_size[DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT] = { 7, 0, 6, 1, 5, 2, 4, 3 };
    int outlif_phase_allocation_order[DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT];
    lif_mngr_outlif_phase_e outlif_phase_per_logical_phase[DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT] =
        { LIF_MNGR_OUTLIF_PHASE_RIF,
        LIF_MNGR_OUTLIF_PHASE_NATIVE_ARP,
        LIF_MNGR_OUTLIF_PHASE_VPLS_1,
        LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_2,
        LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_3,
        LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_4,
        LIF_MNGR_OUTLIF_PHASE_ARP,
        LIF_MNGR_OUTLIF_PHASE_AC
    };
    int allocated_outlifs[2][DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT];
    uint8 current_logical_phase, current_logical_phase_index;
    uint8 current_physical_phase, current_stack;
    lif_mngr_local_outlif_info_t local_outlif_info;

    int global_lifs[2], *global_lif_p;
    uint32 flags, entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    for (current_stack = 0; current_stack < 2; current_stack++)
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.eedb_bank_traffic_lock.
                        local_lifs.alloc(unit, current_stack, DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT));
    }

    
    for (current_logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_FIRST;
         current_logical_phase < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT; current_logical_phase++)
    {
        
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_logical_phase_to_physical_phase(unit,
                                                                              current_logical_phase,
                                                                              &current_physical_phase));
        outlif_phase_allocation_order[outlif_physical_phase_by_size[current_physical_phase]] = current_logical_phase;
    }

    
    for (current_stack = 0; current_stack < 2; current_stack++)
    {
        for (current_logical_phase_index = 0;
             current_logical_phase_index < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT; current_logical_phase_index++)
        {

            
            current_logical_phase = outlif_phase_allocation_order[current_logical_phase_index];

            sal_memset(&local_outlif_info, 0, sizeof(local_outlif_info));
            local_outlif_info.dbal_table_id = DBAL_TABLE_EEDB_DATA_ENTRY;
            local_outlif_info.dbal_result_type = DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_DIP6;
            local_outlif_info.outlif_phase = outlif_phase_per_logical_phase[current_logical_phase];

            local_outlif_info.local_lif_flags = DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_NEW_BANK_ALLOCATION
                | DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_DATA_IN_EEDB_BANKS;

            if (current_logical_phase == DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_FIRST)
            {
                
                global_lif_p = &global_lifs[current_stack];
                flags = 0;
            }
            else
            {
                global_lif_p = NULL;
                flags = LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF;
            }

            
            SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, flags, global_lif_p, NULL, &local_outlif_info));

            allocated_outlifs[current_stack][current_logical_phase] = local_outlif_info.local_outlif;
        }
    }

    
    DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_DIP6);

    
    for (current_stack = 0; current_stack < 2; current_stack++)
    {

        for (current_logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_FIRST;
             current_logical_phase < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT - 1; current_logical_phase++)
        {

            
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF,
                                       allocated_outlifs[current_stack][current_logical_phase]);

            
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER,
                                         INST_SINGLE, allocated_outlifs[current_stack][current_logical_phase + 1]);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

    
    for (current_stack = 0; current_stack < 2; current_stack++)
    {
        SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem
                        (unit, _SHR_CORE_ALL, global_lifs[current_stack], allocated_outlifs[current_stack][0]));
    }

    
    for (current_stack = 0; current_stack < 2; current_stack++)
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.eedb_bank_traffic_lock.
                        global_lif_destination.set(unit, current_stack, global_lifs[current_stack]));

        for (current_logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_FIRST;
             current_logical_phase < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT; current_logical_phase++)
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.eedb_bank_traffic_lock.
                            local_lifs.set(unit, current_stack, current_logical_phase,
                                           allocated_outlifs[current_stack][current_logical_phase]));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_lib_init(
    int unit)
{
    int is_rif = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (!sw_state_is_warm_boot(unit))
    {
        SHR_IF_ERR_EXIT(dnx_lif_lib_init_phase_mapping(unit));
        SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_mdb_clusters_init(unit));
        SHR_IF_ERR_EXIT(dnx_lif_lib_init_rif(unit));

        if (dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_eedb_bank_traffic_lock))
        {
            SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_traffic_lock_bug_workaround(unit));
        }

        
        if ((dnx_data_lif.out_lif.outlif_profile_width_get(unit)) == 0)
        {
            SHR_IF_ERR_EXIT(dnx_algo_local_outlif_eedb_data_banks_disable_set(unit, TRUE));
        }
    }

    dnx_lif_lib_global_lif_type_get(unit, 0, &is_rif);
    if (!is_rif)
    {
        
        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_update
                        (unit, DBAL_TABLE_GLOBAL_LIF_EM, DBAL_FIELD_GLOB_OUT_LIF, TRUE, 0, 0,
                         DBAL_PREDEF_VAL_MIN_VALUE, 0));

        
        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_update
                        (unit, DBAL_TABLE_GLOBAL_RIF_EM, DBAL_FIELD_GLOB_OUT_RIF, TRUE, 0, 0,
                         DBAL_PREDEF_VAL_MAX_VALUE, 0));

    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_lib_allocate(
    int unit,
    uint32 flags,
    int *global_lif,
    lif_mngr_local_inlif_info_t * inlif_info,
    lif_mngr_local_outlif_info_t * outlif_info)
{
    dnx_algo_local_outlif_hw_info_t outlif_hw_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_mngr_lif_allocate(unit, flags, global_lif, inlif_info, outlif_info, &outlif_hw_info));

    if ((outlif_info && _SHR_IS_FLAG_SET(outlif_hw_info.flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_HW))
        && !_SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_DONT_UPDATE_HW))
    {
        
        SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_bank_map_change(unit, outlif_info->local_outlif, &outlif_hw_info, 1));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_lib_inlif_allocate(
    int unit,
    lif_mngr_local_inlif_info_t * inlif_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, inlif_info, NULL));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_lib_outlif_allocate(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, NULL, outlif_info));
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_lif_lib_free(
    int unit,
    int global_lif,
    lif_mngr_local_inlif_info_t * inlif_info,
    int local_outlif)
{
    lif_mngr_local_outlif_info_t outlif_info, *outlif_info_ptr;
    dnx_algo_local_outlif_hw_info_t outlif_hw_info;
    SHR_FUNC_INIT_VARS(unit);

    if (local_outlif != LIF_MNGR_INVALID)
    {
        sal_memset(&outlif_info, 0, sizeof(outlif_info));
        outlif_info.local_outlif = local_outlif;
        outlif_info_ptr = &outlif_info;
    }
    else
    {
        outlif_info_ptr = NULL;
    }

    SHR_IF_ERR_EXIT(dnx_lif_mngr_lif_free(unit, global_lif, inlif_info, outlif_info_ptr, &outlif_hw_info));

    if (outlif_info_ptr && _SHR_IS_FLAG_SET(outlif_hw_info.flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_HW))
    {
        
        SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_bank_map_change(unit, outlif_info_ptr->local_outlif, &outlif_hw_info, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_lib_inlif_free(
    int unit,
    lif_mngr_local_inlif_info_t * inlif_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, LIF_MNGR_INVALID, inlif_info, LIF_MNGR_INVALID));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_lib_outlif_free(
    int unit,
    int local_outlif)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, LIF_MNGR_INVALID, NULL, local_outlif));
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_lif_lib_add_to_glem_verify(
    int unit,
    int core_id,
    uint32 global_lif,
    uint32 local_lif,
    uint8 is_replace)
{
    uint8 is_allocated;
    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORE_VALIDATE(unit, core_id, TRUE);

    LIF_MNGR_GLOBAL_OUT_LIF_VERIFY(unit, global_lif);

    if (!is_replace || local_lif != LIF_LIB_GLEM_KEEP_OLD_VALUE)
    {
        LIF_MNGR_LOCAL_OUT_LIF_VERIFY(unit, local_lif);
    }
    if (global_lif < dnx_data_l3.rif.nof_rifs_get(unit))
    {
        SHR_IF_ERR_EXIT(algo_l3_db.rif_alloc.rif_res_manager.is_allocated(unit, global_lif, &is_allocated));
    }
    else
    {
        SHR_IF_ERR_EXIT(lif_mngr_db.global_lif_allocation.
                        egress_global_lif.is_allocated(unit, global_lif, &is_allocated));
    }
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "global_lif is is not allocated, cannot write to GLEM.");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_lif_lib_add_to_glem_internal(
    int unit,
    int core_id,
    uint32 global_lif,
    uint32 local_lif,
    uint32 outlif_profile,
    uint8 is_replace)
{
    uint32 entry_handle_id;
    int is_rif = 0;
    uint8 entry_changed = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_lif_lib_add_to_glem_verify(unit, core_id, global_lif, local_lif, is_replace));

    SHR_IF_ERR_EXIT(dnx_lif_lib_global_lif_type_get(unit, global_lif, &is_rif));

    if (is_rif)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_RIF_EM, &entry_handle_id));
        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_RIF, global_lif);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        
        if ((!is_replace) || (outlif_profile != LIF_LIB_GLEM_KEEP_OLD_VALUE))
        {
            entry_changed = TRUE;
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_OUT_LIF_PROFILE,
                                         INST_SINGLE, outlif_profile);

        }
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_LIF_EM, &entry_handle_id));
        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, global_lif);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        
        if ((!is_replace) || (local_lif != LIF_LIB_GLEM_KEEP_OLD_VALUE))
        {
            entry_changed = TRUE;
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, INST_SINGLE, local_lif);
        }
        if ((!is_replace) || (outlif_profile != LIF_LIB_GLEM_KEEP_OLD_VALUE))
        {
            if (dnx_data_lif.out_lif.outlif_profile_width_get(unit))
            {
                entry_changed = TRUE;
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_OUT_LIF_PROFILE, INST_SINGLE,
                                             outlif_profile);
            }
        }
    }

    if (entry_changed == FALSE)
    {
        SHR_EXIT();
    }
    if ((dnx_data_lif.global_lif.glem_rif_optimization_enabled_get(unit)) && (is_rif))
    {
        
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));
    }
    else if (is_replace)
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_lib_add_to_glem(
    int unit,
    int core_id,
    uint32 global_lif,
    uint32 local_lif)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem_internal(unit, core_id, global_lif, local_lif, 0, 0));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_lib_replace_glem(
    int unit,
    int core_id,
    uint32 global_lif,
    uint32 local_lif,
    uint32 outlif_profile)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem_internal(unit, core_id, global_lif, local_lif, outlif_profile, 1));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_lif_lib_read_from_glem_verify(
    int unit,
    int core_id,
    uint32 global_lif,
    uint32 *local_lif,
    uint32 *outlif_profile)
{
    uint8 is_allocated;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(outlif_profile, _SHR_E_PARAM, "outlif_profile");
    DNXCMN_CORE_VALIDATE(unit, core_id, FALSE);
    LIF_MNGR_GLOBAL_OUT_LIF_VERIFY(unit, global_lif);
    if (global_lif >= dnx_data_l3.rif.nof_rifs_get(unit))
    {
        SHR_NULL_CHECK(local_lif, _SHR_E_PARAM, "local_lif");
    }
    if (global_lif < dnx_data_l3.rif.nof_rifs_get(unit))
    {
        SHR_IF_ERR_EXIT(algo_l3_db.rif_alloc.rif_res_manager.is_allocated(unit, global_lif, &is_allocated));
    }
    else
    {
        SHR_IF_ERR_EXIT(lif_mngr_db.global_lif_allocation.
                        egress_global_lif.is_allocated(unit, global_lif, &is_allocated));
    }
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "global_lif is is not allocated, cannot read from GLEM.");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_lib_read_from_glem(
    int unit,
    int core_id,
    uint32 global_lif,
    uint32 *local_lif_p,
    uint32 *outlif_profile_p)
{
    uint32 entry_handle_id;
    int is_rif;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_lif_lib_read_from_glem_verify(unit, core_id, global_lif, local_lif_p, outlif_profile_p));

    SHR_IF_ERR_EXIT(dnx_lif_lib_global_lif_type_get(unit, global_lif, &is_rif));

    
    if (is_rif)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_RIF_EM, &entry_handle_id));
        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_RIF, global_lif);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ERPP_OUT_LIF_PROFILE,
                                   INST_SINGLE, outlif_profile_p);

    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_LIF_EM, &entry_handle_id));
        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, global_lif);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, INST_SINGLE, local_lif_p);
        if (dnx_data_lif.out_lif.outlif_profile_width_get(unit))
        {
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ERPP_OUT_LIF_PROFILE, INST_SINGLE,
                                       outlif_profile_p);
        }
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_lif_lib_remove_from_glem_verify(
    int unit,
    int core_id,
    uint32 global_lif)
{
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORE_VALIDATE(unit, core_id, TRUE);

    LIF_MNGR_GLOBAL_OUT_LIF_VERIFY(unit, global_lif);

    if (global_lif < dnx_data_l3.rif.nof_rifs_get(unit))
    {
        SHR_IF_ERR_EXIT(algo_l3_db.rif_alloc.rif_res_manager.is_allocated(unit, global_lif, &is_allocated));
        if (is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "global_rif is allocated, cannot delete. First free rif allocation");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_lib_remove_from_glem(
    int unit,
    int core_id,
    uint32 global_lif)
{
    uint32 entry_handle_id;
    int core_index;
    shr_error_e rv;
    int is_rif = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_lif_lib_remove_from_glem_verify(unit, core_id, global_lif));

    SHR_IF_ERR_EXIT(dnx_lif_lib_global_lif_type_get(unit, global_lif, &is_rif));

    if (!is_rif)
    {
        DNXCMN_CORES_ITER(unit, core_id, core_index)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_LIF_EM, &entry_handle_id));
            
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, global_lif);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_index);

            rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
            if (rv == _SHR_E_NONE)
            {
                SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }
    if (is_rif)
    {
        
        DNXCMN_CORES_ITER(unit, core_id, core_index)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_RIF_EM, &entry_handle_id));

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_RIF, global_lif);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_index);
            SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


