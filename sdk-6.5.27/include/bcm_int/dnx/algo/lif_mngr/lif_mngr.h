/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef  INCLUDE_LIF_MNGR_H_INCLUDED
#define  INCLUDE_LIF_MNGR_H_INCLUDED

#include <bcm_int/dnx/algo/lif_mngr/lif_mngr_api.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/lif_mngr_access.h>
#include <soc/dnx/mdb.h>

#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_DATA_BANK         SAL_BIT(10)

#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_LL_BANK           SAL_BIT(11)

#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_POINTERS           SAL_BIT(12)

#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_HW \
    (DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_LL_BANK | DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_DATA_BANK \
                | DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_POINTERS)

#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_DATA_ENTRY_HW               SAL_BIT(13)

#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_NEW_BANK_ALLOCATION           SAL_BIT(14)

#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_DATA_IN_EEDB_BANKS            SAL_BIT(15)

#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_DONT_ALLOCATE_NEW_BANK               SAL_BIT(16)

#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_REPLACE                              SAL_BIT(17)

#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_RESERVE                              SAL_BIT(18)

#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_LINKED_LIST               SAL_BIT(19)

#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_NO_LINKED_LIST               SAL_BIT(20)

#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_IGNORE_PHASE_ALLOC_MISMATCH                 SAL_BIT(21)

#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_LL_IN_HW                 SAL_BIT(22)

#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_WITH_ID               SAL_BIT(23)

#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_RES_IGNORE_TAG               SAL_BIT(24)

#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_DONT_UPDATE_HW               SAL_BIT(25)

#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_REPLACE_ALLOW_LL_MISMIATCH               SAL_BIT(26)

#define DNX_ALGO_LIF_MNGR_LOCAL_INLIF_REPLACE                              SAL_BIT(18)

#define DNX_ALGO_LIF_MNGR_LOCAL_INLIF_RESERVE                              SAL_BIT(19)

#define DNX_ALGO_LIF_MNGR_LOCAL_INLIF_RESERVE_FOR_SMALLER                  SAL_BIT(20)

#define INTERNAL_LIF_MNGR_LIF_ID_VERIFY(_unit, _nof_lifs, _lif_name, _lif_id) \
            if ((int)_lif_id < 0)    \
            { \
                SHR_ERR_EXIT(_SHR_E_PARAM, "%s can't be negative: %d", _lif_name, _lif_id); \
            } \
            if (_lif_id >= _nof_lifs) \
            { \
                SHR_ERR_EXIT(_SHR_E_PARAM, "%s is too high. Given 0x%08X but maximum is 0x%08X", _lif_name, _lif_id, _nof_lifs - 1); \
            }

#define LIF_MNGR_GLOBAL_IN_LIF_VERIFY(_unit, _lif_id) \
            INTERNAL_LIF_MNGR_LIF_ID_VERIFY(_unit, dnx_data_lif.global_lif.nof_global_in_lifs_get(_unit), \
                                    "Global inlif", _lif_id)

#define LIF_MNGR_GLOBAL_OUT_LIF_VERIFY(_unit, _lif_id) \
            INTERNAL_LIF_MNGR_LIF_ID_VERIFY(_unit, dnx_data_lif.global_lif.nof_global_out_lifs_get(_unit), \
                                    "Global outlif", _lif_id)

#define LIF_MNGR_LOCAL_SBC_IN_LIF_VERIFY(_unit, _lif_id) \
        { \
            int __nof_lifs; \
            SHR_IF_ERR_EXIT(dnx_algo_lif_mngr_max_inlif_id_get(unit, &__nof_lifs, NULL)); \
            INTERNAL_LIF_MNGR_LIF_ID_VERIFY(_unit, __nof_lifs, \
                                    "Local inlif shared by cores", _lif_id) \
        }

#define LIF_MNGR_LOCAL_DPC_IN_LIF_VERIFY(_unit, _lif_id) \
        { \
            int __nof_lifs; \
            SHR_IF_ERR_EXIT(dnx_algo_lif_mngr_max_inlif_id_get(unit, NULL, &__nof_lifs)); \
            INTERNAL_LIF_MNGR_LIF_ID_VERIFY(_unit, __nof_lifs, \
                                    "Local inlif duplicated per core", _lif_id) \
        }

#define LIF_MNGR_LOCAL_OUT_LIF_VERIFY(_unit, _lif_id) \
            INTERNAL_LIF_MNGR_LIF_ID_VERIFY(_unit, dnx_data_lif.out_lif.nof_local_out_lifs_get(unit), \
                                    "Local outlif", _lif_id)

#define LIF_MNGR_DB_IS_INLIF(_unit, _physical_table_id) ((dnx_data_mdb.pdbs.dbal_pdb_info_get(_unit, _physical_table_id)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT) \
        && (dnx_data_mdb.pdbs.dbal_pdb_info_get(_unit, _physical_table_id)->db_subtype == MDB_DIRECT_TYPE_INLIF))

#define LIF_MNGR_DB_IS_INLIF_SBC(_unit, _physical_table_id) ((LIF_MNGR_DB_IS_INLIF(_unit, _physical_table_id)) \
        && (dnx_data_mdb.pdbs.dbal_pdb_info_get(_unit, _physical_table_id)->core_mode == DBAL_CORE_MODE_SBC))

#define LIF_MNGR_DB_IS_INLIF_DPC(_unit, _physical_table_id) ((LIF_MNGR_DB_IS_INLIF(_unit, _physical_table_id)) \
        && (dnx_data_mdb.pdbs.dbal_pdb_info_get(_unit, _physical_table_id)->core_mode == DBAL_CORE_MODE_DPC))

#define DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(_unit)      (dnx_data_lif.out_lif.allocation_bank_size_get(_unit))

#define DNX_ALGO_LOCAL_OUTLIF_NOF_BANKS(_unit)      ((dnx_data_lif.out_lif.nof_local_out_lifs_get(_unit) \
                                                             / DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(_unit)))

#define DNX_ALGO_LOCAL_OUTLIF_LL_CAPABLE_NOF_BANKS(_unit)      ((dnx_data_lif.out_lif.nof_local_out_lifs_get(_unit) \
                                                                / DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(_unit)) / 2)

#define DNX_ALGO_LOCAL_OUTLIF_MAX_LL_LOGICAL_ADDR               (1 << (dnx_data_lif.out_lif.outlif_pointer_size_get(unit) - 1))

#define DNX_ALGO_LOCAL_OUTLIF_NOF_DYNAMIC_CAPABLE_LL_OUTLIF_BANKS(_unit) (DNX_ALGO_LOCAL_OUTLIF_MAX_LL_LOGICAL_ADDR / (dnx_data_lif.out_lif.allocation_bank_size_get(_unit)))

#define DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK          (-1)

#define EEDB_MAX_DATA_ENTRY_SIZE    MDB_NOF_CLUSTER_ROW_BITS

#define OUTLIF_BANK_FIRST_INDEX(_unit, _outlif_bank) (_outlif_bank * DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(_unit))

#define DNX_ALGO_OUTLIF_LOCAL_LIF_TO_OFFSET(_local_outlif) \
        ((_local_outlif) % DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit))

#define DNX_ALGO_OUTLIF_BANK_AND_OFFSET_TO_LOCAL_LIF(_bank, _offset) \
        ((_bank) * DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit) + (_offset))

#define DNX_ALGO_OUTLIF_IS_POINTER_BIGGER_THAN_EEDB_RESOURCE (dnx_data_lif.out_lif.outlif_profile_width_get(unit) == 0)

typedef struct
{
    int nof_outlif_banks_in_external_memory;
    int nof_used_outlif_banks_in_external_memory;
    int nof_eedb_banks;
    int nof_used_eedb_banks;
    int nof_glem_entries;
    int nof_used_glem_entries;
} dnx_algo_local_outlif_stats_t;

typedef struct
{

    uint32 flags;

    mdb_cluster_alloc_info_t data_bank_info;

    mdb_cluster_alloc_info_t ll_bank_info;

    dnx_algo_local_outlif_logical_phase_e logical_phase;
} dnx_algo_local_outlif_hw_info_t;

typedef enum
{
    DNX_ALGO_OUTLIF_BANK_UNASSIGNED,
    DNX_ALGO_OUTLIF_BANK_ASSIGNED_TO_EEDB,
    DNX_ALGO_OUTLIF_BANK_ASSIGNED_TO_MDB
} dnx_algo_outlif_bank_assignment_mode_e;

typedef struct
{
    int granularity;
    dnx_algo_outlif_bank_assignment_mode_e assigned;
} dnx_algo_lif_local_outlif_resource_extra_info_t;

shr_error_e dnx_algo_local_outlif_next_bank_get(
    int unit,
    uint32 *next_bank);

shr_error_e dnx_algo_local_outlif_can_be_reused(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info,
    int *can_be_reused);

shr_error_e dnx_algo_local_outlif_to_outlif_bank(
    int unit,
    int local_outlif,
    uint32 *outlif_bank,
    int *outlif_in_bank);

shr_error_e dnx_algo_local_outlif_is_allocated_get(
    int unit,
    int local_outlif,
    uint8 *is_allocated);

shr_error_e dnx_algo_local_outlif_allocate_with_id(
    int unit,
    uint32 outlif_bank,
    int local_outlif,
    lif_mngr_local_outlif_info_t * outlif_info);

shr_error_e dnx_algo_local_outlif_free_several(
    int unit,
    uint32 outlif_bank,
    int local_outlif,
    int deallocation_size);

shr_error_e dnx_algo_local_inlif_can_be_reused(
    int unit,
    lif_mngr_local_inlif_info_t * inlif_info,
    int *can_be_reused,
    int *smaller_inlif);

shr_error_e dnx_algo_local_outlif_outrif_physical_bank_hw_info_get(
    int unit,
    int *outrif_bank_starts,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_infos,
    int *nof_outrifs_info);

shr_error_e dnx_lif_mngr_lif_allocate(
    int unit,
    uint32 flags,
    lif_mngr_global_lif_info_t * global_lif_info,
    lif_mngr_local_inlif_info_t * inlif_info,
    lif_mngr_local_outlif_info_t * outlif_info,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_info);

shr_error_e dnx_lif_mngr_lif_free(
    int unit,
    int global_lif,
    lif_mngr_local_inlif_info_t * inlif_info,
    lif_mngr_local_outlif_info_t * outlif_info,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_info,
    int dont_release_sw_info);

shr_error_e dnx_algo_lif_mngr_init(
    int unit);

shr_error_e dnx_algo_lif_mngr_deinit(
    int unit);

shr_error_e dnx_algo_lif_mngr_max_inlif_id_get(
    int unit,
    int *nof_sbc_lifs,
    int *nof_dpc_lifs);

shr_error_e dnx_algo_lif_mngr_nof_inlifs_for_hash_get(
    int unit,
    int *nof_inlifs);

shr_error_e dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_set(
    int unit,
    int local_in_lif,
    int core_id,
    dbal_tables_e table_id,
    uint32 result_type);

shr_error_e dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get(
    int unit,
    int local_in_lif,
    int core_id,
    dbal_physical_tables_e physical_table_id,
    dbal_tables_e * table_id,
    uint32 *result_type);

shr_error_e dnx_lif_mngr_outlif_sw_info_add(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info);

shr_error_e dnx_lif_mngr_outlif_sw_info_get(
    int unit,
    int local_out_lif,
    dbal_tables_e * dbal_table_id,
    uint32 *result_type,
    dnx_algo_local_outlif_logical_phase_e * logical_phase,
    uint32 *fixed_entry_size,
    uint8 *has_ll,
    uint8 *is_count_profile);

shr_error_e dnx_lif_mngr_outlif_sw_info_is_count_profile_set(
    int unit,
    int local_out_lif,
    dbal_tables_e dbal_table_id);

shr_error_e dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_delete(
    int unit,
    int core_id,
    uint32 local_in_lif,
    dbal_tables_e table_id);

shr_error_e dnx_lif_mngr_outlif_sw_info_delete(
    int unit,
    int local_out_lif,
    dbal_tables_e optional_dbal_table_id);

shr_error_e dnx_lif_mngr_phase_string_to_id(
    int unit,
    char *logical_phase_str,
    dnx_algo_local_outlif_logical_phase_e * logical_phase);

shr_error_e dnx_algo_local_outlif_db_init(
    int unit,
    mdb_cluster_alloc_info_t * init_clusters_info,
    int *nof_init_clusters);

shr_error_e dnx_algo_local_outlif_rif_init(
    int unit);

shr_error_e dnx_algo_local_outlif_logical_phase_to_physical_phase(
    int unit,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    uint8 *physical_phase);

shr_error_e dnx_algo_local_outlif_get_potential_nof_mdb_outlifs_for_entry(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info,
    int *nof_potential_outlifs);

shr_error_e dnx_algo_local_outlif_info_to_nof_outlifs_per_entry(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info,
    int *nof_outlifs_per_entry);

shr_error_e dnx_algo_local_outlif_mdb_clusters_disable_set(
    int unit,
    uint8 value);

shr_error_e dnx_algo_local_outlif_eedb_data_banks_disable_set(
    int unit,
    uint8 value);

shr_error_e dnx_algo_local_outlif_stats_get(
    int unit,
    dnx_algo_local_outlif_stats_t * outlif_stats);

shr_error_e dnx_algo_local_outlif_rif_init_info_get(
    int unit,
    dnx_algo_lif_local_outlif_resource_extra_info_t * init_info);

shr_error_e dnx_algo_global_lif_allocation_count_verify(
    int unit,
    uint32 direction_flags);

shr_error_e dnx_algo_global_lif_allocation_update_count(
    int unit,
    uint32 direction_flags,
    int diff);

shr_error_e dnx_algo_global_lif_allocation_lif_id_is_symmetric(
    int unit,
    int global_lif,
    uint8 *is_symmetric);

shr_error_e dnx_algo_global_lif_allocation_update_symmetric_indication(
    int unit,
    int global_lif,
    int is_symmetric);

shr_error_e dnx_algo_lif_mapping_local_to_global_get(
    int unit,
    uint32 flags,
    lif_mapping_local_lif_key_t * local_lif_info,
    int *global_lif);

shr_error_e dnx_lif_mngr_eedb_table_rt_has_linked_list(
    int unit,
    dbal_tables_e table_id,
    int res_type_idx,
    uint8 *has_link_list);

shr_error_e dnx_lif_lib_encap_access_to_logical_phase_convert(
    int unit,
    bcm_encap_access_t encap_access,
    dnx_algo_local_outlif_logical_phase_e * logical_phase);

shr_error_e dnx_lif_lib_logical_phase_to_encap_access_convert(
    int unit,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    bcm_encap_access_t * encap_access);

shr_error_e dnx_algo_global_out_lif_alloc_info_get(
    int unit,
    int global_lif,
    int direction,
    global_lif_alloc_info_t * alloc_info);

void dnx_global_lif_tag_set(
    int unit,
    global_lif_alloc_info_t * alloc_info,
    uint32 *tag);

#endif
