/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */



#ifndef  INCLUDE_LIF_MNGR_H_INCLUDED
#define  INCLUDE_LIF_MNGR_H_INCLUDED




#include <bcm_int/dnx/algo/lif_mngr/lif_mngr_api.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/lif_mngr_access.h>
#include <soc/dnx/mdb.h>








#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_DATA_BANK         SAL_BIT(18)


#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_LL_BANK           SAL_BIT(19)


#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_POINTERS           SAL_BIT(20)


#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_HW \
    (DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_LL_BANK | DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_DATA_BANK \
                | DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_POINTERS)


#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_NEW_BANK_ALLOCATION           SAL_BIT(21)


#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_DATA_IN_EEDB_BANKS            SAL_BIT(22)


#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_DONT_ALLOCATE_NEW_BANK               SAL_BIT(23)


#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_REPLACE                              SAL_BIT(24)


#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_RESERVE                              SAL_BIT(25)


#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_LINKED_LIST               SAL_BIT(26)


#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_NO_LINKED_LIST               SAL_BIT(27)


#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_IGNORE_PHASE_ALLOC_MISMATCH                 SAL_BIT(28)


#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_WITH_ID               SAL_BIT(29)


#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_RES_IGNORE_TAG               SAL_BIT(30)


#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_DONT_UPDATE_HW               SAL_BIT(31)






#define DNX_ALGO_LIF_MNGR_LOCAL_INLIF_REPLACE                              SAL_BIT(20)


#define DNX_ALGO_LIF_MNGR_LOCAL_INLIF_RESERVE                              SAL_BIT(21)


#define DNX_ALGO_LIF_MNGR_LOCAL_INLIF_RESERVE_FOR_SMALLER                  SAL_BIT(22)







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
            SHR_IF_ERR_EXIT(dnx_algo_lif_mngr_nof_inlifs_get(unit, &__nof_lifs, NULL)); \
            INTERNAL_LIF_MNGR_LIF_ID_VERIFY(_unit, __nof_lifs, \
                                    "Local inlif shared by cores", _lif_id) \
        }

#define LIF_MNGR_LOCAL_DPC_IN_LIF_VERIFY(_unit, _lif_id) \
        { \
            int __nof_lifs; \
            SHR_IF_ERR_EXIT(dnx_algo_lif_mngr_nof_inlifs_get(unit, NULL, &__nof_lifs)); \
            INTERNAL_LIF_MNGR_LIF_ID_VERIFY(_unit, __nof_lifs, \
                                    "Local inlif duplicated per core", _lif_id) \
        }

#define LIF_MNGR_LOCAL_OUT_LIF_VERIFY(_unit, _lif_id) \
            INTERNAL_LIF_MNGR_LIF_ID_VERIFY(_unit, dnx_data_lif.out_lif.nof_local_out_lifs_get(unit), \
                                    "Local outlif", _lif_id)


#define DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(_unit)      (dnx_data_lif.out_lif.allocation_bank_size_get(_unit))

#define DNX_ALGO_LOCAL_OUTLIF_NOF_BANKS(_unit)      ((dnx_data_lif.out_lif.nof_local_out_lifs_get(_unit) \
                                                             / DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(_unit)))

#define DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK          (-1)


#define EEDB_MAX_DATA_ENTRY_SIZE    MDB_NOF_CLUSTER_ROW_BITS


#define OUTLIF_BANK_FIRST_INDEX(_unit, _outlif_bank) (_outlif_bank * DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(_unit))


#define DNX_ALGO_OUTLIF_LOCAL_LIF_TO_BANK(_local_outlif) \
        ((_local_outlif) / DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit))

#define DNX_ALGO_OUTLIF_LOCAL_LIF_TO_OFFSET(_local_outlif) \
        ((_local_outlif) % DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit))

#define DNX_ALGO_OUTLIF_BANK_AND_OFFSET_TO_LOCAL_LIF(_bank, _offset) \
        ((_bank) * DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit) + (_offset))






typedef enum
{
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_FIRST,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_1 = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_FIRST,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_2,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_3,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_4,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_5,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_6,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_7,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_8,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT
} dnx_algo_local_outlif_logical_phase_e;


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










shr_error_e dnx_algo_local_outlif_next_bank_get(
    int unit,
    uint32 *next_bank);


shr_error_e dnx_algo_local_outlif_can_be_reused(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info,
    int *can_be_reused);


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
    int *global_lif,
    lif_mngr_local_inlif_info_t * inlif_info,
    lif_mngr_local_outlif_info_t * outlif_info,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_info);


shr_error_e dnx_lif_mngr_lif_free(
    int unit,
    int global_lif,
    lif_mngr_local_inlif_info_t * inlif_info,
    lif_mngr_local_outlif_info_t * outlif_info,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_info);


shr_error_e dnx_algo_lif_mngr_init(
    int unit);


shr_error_e dnx_algo_lif_mngr_deinit(
    int unit);


shr_error_e dnx_algo_lif_mngr_nof_inlifs_get(
    int unit,
    int *nof_inlif1_lifs,
    int *nof_inlif2_lifs);



shr_error_e dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_set(
    int unit,
    int local_in_lif,
    int core_id,
    dbal_tables_e table_id,
    uint32 result_type,
    uint32 fixed_entry_size);


shr_error_e dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get(
    int unit,
    int local_in_lif,
    int core_id,
    dbal_physical_tables_e physical_table_id,
    dbal_tables_e * table_id,
    uint32 *result_type,
    uint32 *fixed_entry_size);


shr_error_e dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_set(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info,
    int local_out_lif,
    dbal_tables_e table_id,
    uint32 result_type,
    uint32 fixed_entry_size);


shr_error_e dnx_lif_mngr_outlif_sw_info_get(
    int unit,
    int local_out_lif,
    dbal_tables_e * dbal_table_id,
    uint32 *result_type,
    lif_mngr_outlif_phase_e * outlif_phase,
    uint32 *fixed_entry_size,
    uint8 *has_ll,
    uint8 *is_count_profile);


shr_error_e dnx_lif_mngr_outlif_sw_info_is_count_profile_set(
    int unit,
    int local_out_lif);



shr_error_e dnx_algo_local_outlif_to_eedb_logical_phase(
    int unit,
    int local_outlif,
    dbal_tables_e dbal_table_id,
    lif_mngr_outlif_phase_e * outlif_phase);


shr_error_e dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_delete(
    int unit,
    int core_id,
    uint32 local_in_lif,
    dbal_tables_e table_id);


shr_error_e dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_delete(
    int unit,
    int local_out_lif);



shr_error_e dnx_lif_mngr_phase_string_to_id(
    int unit,
    char *outlif_phase_str,
    lif_mngr_outlif_phase_e * outlif_phase);


shr_error_e dnx_algo_local_outlif_mdb_bank_database_init(
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
    int *init_info);


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


shr_error_e dnx_algo_lif_mapping_local_to_global_get(
    int unit,
    uint32 flags,
    lif_mapping_local_lif_info_t * local_lif_info,
    int *global_lif);


shr_error_e dnx_lif_mngr_eedb_table_rt_has_linked_list(
    int unit,
    dbal_tables_e table_id,
    int res_type_idx,
    uint8 *has_link_list);


shr_error_e dnx_algo_local_outlif_phase_enum_to_logical_phase_num(
    int unit,
    lif_mngr_outlif_phase_e outlif_phase,
    dnx_algo_local_outlif_logical_phase_e * logical_phase);



#endif 
