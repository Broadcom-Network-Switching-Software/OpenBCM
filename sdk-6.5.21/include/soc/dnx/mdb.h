
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef MDB_H_INCLUDED

#define MDB_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



#include <soc/mem.h>

#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnx/mdb_global.h>

#include <soc/dnx/swstate/auto_generated/access/mdb_kaps_access.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/swstate/sw_state.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>
#include <soc/dnx/swstate/auto_generated/types/mdb_types.h>
#include <soc/dnx/swstate/auto_generated/types/mdb_kaps_types.h>







#define MDB_DIRECT_BASIC_ENTRY_SIZE             30
#define MDB_NOF_CLUSTER_ROW_BITS                120
#define MDB_NOF_CLUSTER_ROW_UINT32              BITS2WORDS(MDB_NOF_CLUSTER_ROW_BITS)



#define MDB_NOF_CLUSTER_ROW_BYTE                BITS2BYTES(MDB_NOF_CLUSTER_ROW_BITS)



#define MDB_MAX_NOF_CLUSTERS     100

#define MDB_PAYLOAD_SIZE_TO_PAYLOAD_SIZE_TYPE(payload_size) ((payload_size / MDB_DIRECT_BASIC_ENTRY_SIZE) - 1)
#define MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_type) ((payload_type + 1) * MDB_DIRECT_BASIC_ENTRY_SIZE)



#define MDB_OUTLIF_AND_GRANULARITY_TO_PREFIX(unit, outlif, granularity) \
        ((outlif >> utilex_log2_round_up(dnx_data_mdb.dh.macro_type_info_get(unit, MDB_EEDB_BANK)->nof_rows * MDB_NOF_CLUSTER_ROW_BITS / granularity)) \
                & ((1 << dnx_data_mdb.eedb.bank_id_bits_get(unit)) - 1))



#define MDB_DH_GET_GLOBAL_MACRO_INDEX(_unit,_macro_type,_macro_index) (dnx_data_mdb.dh.macro_type_info_get(_unit, _macro_type)->global_start_index + _macro_index)



#define MDB_MACRO_NOF_FEC_BANKS_GET(macro_type) ((macro_type == MDB_MACRO_A) ? 2 : 1)



#define MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit,bank_id) (dnx_data_mdb.direct.nof_fecs_in_super_fec_get(unit)*UTILEX_DIV_ROUND_UP(bank_id*dnx_data_l3.fec.nof_physical_fecs_per_bank_get(unit)*dnx_data_mdb.pdbs.pdb_info_get(unit,DBAL_PHYSICAL_TABLE_FEC_1)->row_width,dnx_data_l3.fec.super_fec_size_get(unit)))


#define MDB_CALC_FEC_BANK_SIZE(unit,bank_id) (MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit,(bank_id+1)) - MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit,bank_id))






typedef enum
{
    MDB_STEP_TABLE_ENTRY_SIZE_ONE = 0,
    MDB_STEP_TABLE_ENTRY_SIZE_HALF = 1,
    MDB_STEP_TABLE_ENTRY_SIZE_QUARTER = 2,
    MDB_STEP_TABLE_ENTRY_SIZE_EIGHTH = 3,
    MDB_STEP_TABLE_NOF_ENTRY_SIZES
} mdb_step_table_entry_size_e;



typedef enum
{
    MDB_EM_ENTRY_ENCODING_ONE = 0,
    MDB_EM_ENTRY_ENCODING_HALF = 1,
    MDB_EM_ENTRY_ENCODING_QUARTER = 2,
    MDB_EM_ENTRY_ENCODING_EIGHTH = 3,
    MDB_EM_ENTRY_ENCODING_EMPTY = 7,
    MDB_EM_NOF_ENTRY_ENCODINGS
} mdb_em_entry_encoding_e;







typedef struct mdb_cluster_alloc_info
{

    

    dbal_physical_tables_e dbal_physical_table_id;

    

    mdb_eedb_mem_type_e mdb_eedb_type;

    

    mdb_macro_types_e macro_type;

    

    uint8 macro_idx;

    

    uint8 cluster_idx;

    

    int macro_nof_rows;

    

    uint8 macro_row_width;

    

    int entry_size;

    

    int logical_start_address;
} mdb_cluster_alloc_info_t;













int dnx_mdb_init(
    int unit);



int dnx_mdb_deinit(
    int unit);



shr_error_e mdb_pre_init_step(
    int unit);


shr_error_e mdb_init_add_table_resources(
    int unit,
    int nof_valid_clusters,
    mdb_cluster_alloc_info_t clusters[MDB_MAX_NOF_CLUSTERS]);


shr_error_e mdb_init_get_table_resources(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_cluster_alloc_info_t clusters[MDB_MAX_NOF_CLUSTERS],
    int *nof_valid_clusters);


shr_error_e mdb_init_delete_table_resources(
    int unit,
    int nof_valid_clusters,
    mdb_cluster_alloc_info_t clusters[MDB_MAX_NOF_CLUSTERS]);


shr_error_e mdb_init_em_age_nof_bits_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_em_entry_encoding_e entry_encoding,
    int *nof_age_bits);


shr_error_e mdb_init_logical_table(
    int unit,
    dbal_tables_e dbal_table,
    void *access_info);


shr_error_e mdb_init_update_vmv(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id);



shr_error_e mdb_get_capacity(
    int unit,
    dbal_physical_tables_e dbal_physical_table,
    int *capacity);



shr_error_e mdb_get_capacity_estimate(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int key_size,
    int payload_size,
    int app_id,
    int app_id_size,
    int *capacity_estimate);



shr_error_e mdb_direct_table_get_basic_size_using_logical_mdb_table(
    int unit,
    dbal_physical_tables_e logic_table_id,
    int *basic_size);


shr_error_e mdb_direct_table_entry_add(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id_arr[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);


shr_error_e mdb_direct_table_entry_get(
    int unit,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);


shr_error_e mdb_direct_table_entry_delete(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id_arr[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);


shr_error_e mdb_direct_table_iterator_init(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator);


shr_error_e mdb_direct_table_iterator_get_next(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator,
    dbal_physical_entry_t * entry,
    uint8 *is_end);


shr_error_e mdb_direct_table_iterator_deinit(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator);


shr_error_e mdb_direct_table_clear(
    int unit,
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id);


shr_error_e mdb_direct_get_payload_type(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    dbal_enum_value_field_mdb_direct_payload_e * payload_type);



shr_error_e mdb_em_entry_add(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id_arr[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);



shr_error_e mdb_em_entry_get(
    int unit,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);



shr_error_e mdb_em_entry_delete(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id_arr[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);



shr_error_e mdb_em_prepare_entry(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    soc_mem_t mem,
    uint32 *key,
    uint32 key_size,
    uint32 *payload,
    uint32 payload_size,
    mdb_em_entry_encoding_e entry_encoding,
    uint32 entry_offset,
    soc_reg_above_64_val_t data);


shr_error_e mdb_em_iterator_init(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator);



shr_error_e mdb_em_iterator_get_next(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator,
    dbal_physical_entry_t * entry,
    uint8 *is_end);



shr_error_e mdb_em_iterator_deinit(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator);



shr_error_e mdb_em_table_clear(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id_arr[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id);


shr_error_e mdb_em_get_min_entry_encoding(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 key_size,
    uint32 payload_size,
    uint32 app_id,
    int app_id_size,
    int vmv_size,
    mdb_em_entry_encoding_e * entry_encoding);


shr_error_e mdb_em_get_entry_encoding(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 key_size,
    uint32 payload_size,
    uint32 app_id,
    int app_id_size,
    mdb_em_entry_encoding_e * entry_encoding);



shr_error_e mdb_em_get_vmv_size_value(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_em_entry_encoding_e entry_encoding,
    uint8 *vmv_size,
    uint8 *vmv_value);



shr_error_e mdb_em_set_vmv_size_value(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_em_entry_encoding_e entry_encoding,
    uint8 vmv_size,
    uint8 vmv_value);



shr_error_e mdb_em_get_min_remaining_entry_count(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int *free_entry_count);


shr_error_e mdb_em_mact_age_refresh_mode_set(
    int unit,
    mdb_physical_table_e mdb_table,
    uint32 mode);


shr_error_e mdb_em_mact_age_refresh_mode_get(
    int unit,
    uint32 *mode);


int mdb_lpm_db_kaps_sync(
    int unit);


shr_error_e mdb_lpm_entry_add(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id_arr[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);


shr_error_e mdb_lpm_entry_get(
    int unit,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);



shr_error_e mdb_lpm_entry_delete(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id_arr[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);



shr_error_e mdb_lpm_iterator_init(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator);



shr_error_e mdb_lpm_iterator_get_next(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator,
    dbal_physical_entry_t * entry,
    uint8 *is_end);



shr_error_e mdb_lpm_iterator_deinit(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator);



shr_error_e mdb_lpm_table_clear(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id_arr[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id);



shr_error_e mdb_lpm_get_current_capacity(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int *num_entries,
    int *num_entries_iter,
    int *capacity_estimate);


shr_error_e mdb_lpm_get_ip_capacity(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int *ipv4_capacity,
    int *ipv6_capacity);


shr_error_e mdb_eedb_table_entry_add(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id_arr[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);


shr_error_e mdb_eedb_table_entry_get(
    int unit,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);


shr_error_e mdb_eedb_table_entry_delete(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id_arr[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);


shr_error_e mdb_eedb_table_iterator_init(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator);


shr_error_e mdb_eedb_table_iterator_get_next(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator,
    dbal_physical_entry_t * entry,
    uint8 *is_end);


shr_error_e mdb_eedb_table_iterator_deinit(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator);


shr_error_e mdb_eedb_table_clear(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id_arr[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id);


shr_error_e mdb_eedb_table_data_granularity_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 *address_granularity);


shr_error_e mdb_eedb_table_nof_entries_per_cluster_type_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_macro_types_e macro_type,
    uint32 *nof_entries);


shr_error_e mdb_table_is_allocated_get(
    int unit,
    dbal_physical_tables_e dbal_phy_table_id,
    uint8 *is_allocated);


#endif 
