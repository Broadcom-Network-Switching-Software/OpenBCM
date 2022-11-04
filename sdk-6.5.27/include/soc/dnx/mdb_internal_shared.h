
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef MDB_H_INTERNAL_SHARED_INCLUDED

#define MDB_H_INTERNAL_SHARED_INCLUDED

#include <soc/mem.h>

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dbal/dbal_structures.h>
#ifdef ADAPTER_SERVER_MODE
#include <soc/dnx/dnx_data/auto_generated/dnx_data_adapter.h>
#endif
#include <soc/dnx/mdb_global.h>

#include <libs/kaps/include/kaps_errors.h>
#include <libs/kaps/include/kaps_default_allocator.h>
#include <libs/kaps/include/kaps_device.h>
#include <libs/kaps/include/kaps_db.h>
#include <libs/kaps/include/kaps_ad.h>
#include <libs/kaps/include/kaps_instruction.h>
#include <libs/kaps/include/kaps_key.h>
#include <libs/kaps/include/kaps_xpt.h>

#ifdef ADAPTER_SERVER_MODE

#define MDB_ADAPTER_MEMORY_ID_INVALID (255)
#endif

#define MDB_INIT_CLUSTER_FLAG_2ND_INTERFACES          0x01
#define MDB_INIT_CLUSTER_FLAG_FLEX                    0x02
#define MDB_INIT_CLUSTER_FLAG_480_BPU                 0x04
#define MDB_INIT_CLUSTER_FLAG_960_BPU                 0x08
#define MDB_INIT_CLUSTER_FLAG_FIRST_INTF_CLUSTER      0x10

#define MDB_EM_TEST_MAX_NOF_DBAL_LOGICAL_TABLES 2

#define MDB_ENTRY_REMAINDER_CALC(entry_index,entries_size,row_width) ((entry_index * entries_size) % row_width)

#define MDB_MAX_DIRECT_PAYLOAD_SIZE_32    UTILEX_DIV_ROUND_UP(MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES-1), SAL_UINT32_NOF_BITS)

#define MDB_MAX_EM_PAYLOAD_SIZE_32 UTILEX_DIV_ROUND_UP(DNX_DATA_MAX_MDB_PDBS_MAX_PAYLOAD_SIZE,SAL_UINT32_NOF_BITS)

#define MDB_EM_TABLE_USE_ALL_ASPECT_RATIOS(_dbal_physical_table_id)  ((dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, _dbal_physical_table_id)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM) \
                                                        && ((dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, _dbal_physical_table_id)->db_subtype == MDB_EM_TYPE_EXEM)     \
                                                             || (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, _dbal_physical_table_id)->db_subtype == MDB_EM_TYPE_PPMC)))

#define MDB_INIT_IS_FEC_TABLE(_mdb_physical_table_id) ((dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, _mdb_physical_table_id)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT) \
                                                      && (dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, _mdb_physical_table_id)->db_subtype == MDB_DIRECT_TYPE_FEC))

#define MDB_DH_IS_TABLE_SUPPORT_FLEX(unit, _mdb_physical_table_id) ((dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, _mdb_physical_table_id)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM) && \
                                                   dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, _mdb_physical_table_id)->row_width == dnx_data_mdb.dh.cluster_row_width_bits_get(unit))

#define MDB_DH_SERIAL_CLUSTER_NUMBER_FOR_MACRO_TYPE(unit, macro_type, macro_idx, cluster_idx) (((macro_idx)*dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->nof_clusters)+cluster_idx)

#define MDB_APP_ID_ITER_ALL                         (SAL_UINT32_MAX)

#define MDB_KAPS_KEY_WIDTH_IN_BYTES(unit)       BITS2BYTES(dnx_data_mdb.kaps.key_width_in_bits_get(unit))

#define MDB_KAPS_KEY_WIDTH_IN_UINT32(unit)      BITS2WORDS(dnx_data_mdb.kaps.key_width_in_bits_get(unit))

typedef enum
{

    MDB_DH_INIT_FIRST_IF_USED = 0,

    MDB_DH_INIT_SECOND_IF_USED = 1,

    MDB_DH_INIT_MAX_AVAILABLE_INTERFACES = 2
} mdb_dh_init_interface_instance_in_macro_e;

typedef enum
{

    MDB_EM_ENCODING_TABLE_ENTRY,

    MDB_EM_ENCODING_DH_ENTRY_120,

    MDB_EM_ENCODING_DH_ENTRY_240,

    MDB_EM_NOF_ENCODING_REQUESTS
} mdb_em_encoding_request_e;

typedef enum
{
    MDB_EM_ENCODING_TYPE_ONE,
    MDB_EM_ENCODING_TYPE_HALF,
    MDB_EM_ENCODING_TYPE_QUARTER,
    MDB_EM_ENCODING_TYPE_EIGHTH,
    MDB_EM_ENCODING_TYPE_EMPTY,
    MDB_EM_NOF_ENCODING_TYPES
} mdb_em_encoding_types_e;

typedef enum
{

    MDB_TEST_BRIEF,

    MDB_TEST_CAPACITY,

    MDB_TEST_CAPACITY_MIXED,

    MDB_TEST_ITERATOR,

    MDB_TEST_SER,

    MDB_TEST_HITBIT_BASIC,

    MDB_TEST_HITBIT_CAPACITY,

    MDB_TEST_HASH_PACKING,

    MDB_TEST_TABLE_CLEAR,

    MDB_TEST_TABLE_SCAN_LOCK,

    MDB_TEST_TABLE_INSTANCE,

    MDB_NOF_TEST_MODES
} mdb_test_mode_e;

typedef struct mdb_dh_macro_interface_info_s
{

    uint8 flags;
    int nof_cluster_per_entry;
    int first_alloc_cluster;
    uint32 nof_cluster_allocated;
    uint32 alloc_bit_map;
} mdb_dh_macro_interface_info_t;

typedef struct mdb_dh_macro_info_s
{
    uint32 macro_index;
    uint32 cluster_index[8];
    mdb_macro_types_e macro_type;
    mdb_dh_macro_interface_info_t intf[MDB_DH_INIT_MAX_AVAILABLE_INTERFACES];
} mdb_dh_macro_info_t;

extern const int mdb_lpm_ad_size_enum_to_bits[MDB_NOF_LPM_AD_SIZES];

shr_error_e mdb_arm_init(
    int unit);

shr_error_e mdb_arm_deinit(
    int unit);

shr_error_e mdb_dbal_table_is_mdb(
    int unit,
    dbal_tables_e dbal_logical_table_id,
    uint8 *is_mdb);

shr_error_e mdb_direct_address_mapping_alloc(
    int unit);

shr_error_e mdb_direct_address_mapping_set(
    int unit,
    mdb_physical_table_e mdb_physical_table_id,
    int physical_address_start,
    int physical_address_end,
    int cluster_index);

shr_error_e mdb_direct_address_mapping_unset(
    int unit,
    mdb_physical_table_e mdb_physical_table_id,
    int physical_address_start,
    int physical_address_end);

shr_error_e mdb_direct_address_mapping_get(
    int unit,
    mdb_physical_table_e mdb_physical_table_id,
    int physical_address,
    int *cluster_index);

shr_error_e mdb_direct_init(
    int unit);

shr_error_e mdb_direct_table_get_basic_size(
    int unit,
    mdb_physical_table_e mdb_physical_table_id,
    int *basic_size);

shr_error_e mdb_direct_table_get_row_width(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int *row_width);

shr_error_e mdb_direct_table_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode,
    int delete_cmd,
    int full_time);

shr_error_e mdb_dh_em_way_index_get(
    int unit,
    mdb_physical_table_e mdb_physical_table_id,
    mdb_macro_types_e macro_type,
    uint8 macro_idx,
    uint8 cluster_idx,
    uint32 *way_index);

shr_error_e mdb_dh_get_cluster_granularity(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_idx,
    int cluster_idx,
    int *cluster_granularity);

shr_error_e mdb_dh_set_bpu_setting(
    int unit);

int mdb_em_age_supported_internal(
    int unit,
    mdb_physical_table_e mdb_physical_table_id);

int mdb_em_hitbit_supported_internal(
    int unit,
    mdb_physical_table_e mdb_physical_table_id);

shr_error_e mdb_em_calc_vmv_regs(
    int unit,
    mdb_em_encoding_request_e encoding_request,
    mdb_em_encoding_request_e encoding_nof_bits_request,
    dbal_physical_tables_e dbal_table_id,
    uint8 esem_cmd_indication,
    soc_reg_above_64_val_t vmv_encoding_data,
    soc_reg_above_64_val_t vmv_size_data);

shr_error_e mdb_em_get_addr_bits(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 *addr_bits);

int mdb_em_get_entry_encoding_is_valid(
    int unit,
    uint32 row_width,
    uint32 max_key_app_id_size,
    uint32 key_size,
    uint32 payload_size,
    uint32 app_id_size,
    uint32 vmv_size,
    uint32 entry_addr_bits,
    mdb_em_entry_encoding_e entry_encoding);

shr_error_e mdb_em_get_key_size(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    uint32 *key_size);

shr_error_e mdb_em_hw_entry_count_decrease(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id);

shr_error_e mdb_em_hw_entry_count_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 *count);

shr_error_e mdb_em_init_overrides(
    int unit);

shr_error_e mdb_em_init_shadow_db(
    int unit);

shr_error_e mdb_em_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    mdb_test_mode_e mode,
    int delete_cmd,
    int full_time);

shr_error_e mdb_em_vmv_calculation_by_entry(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint8 *vmv_size,
    uint8 *vmv_value);

shr_error_e mdb_em_vmv_calculation_by_full_vmv(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 full_vmv_value,
    uint8 *vmv_value,
    uint8 *vmv_size);

shr_error_e mdb_eedb_address_mapping_alloc(
    int unit);

shr_error_e mdb_eedb_address_mapping_set(
    int unit,
    mdb_physical_table_e mdb_physical_table_id,
    int logical_address_start,
    int logical_address_end,
    int cluster_index);

shr_error_e mdb_eedb_address_mapping_unset(
    int unit,
    mdb_physical_table_e mdb_physical_table_id,
    int logical_address_start,
    int logical_address_end);

shr_error_e mdb_eedb_address_mapping_get(
    int unit,
    int logical_address,
    mdb_physical_table_e * mdb_physical_table_id_data,
    int *cluster_index_data,
    mdb_physical_table_e * mdb_physical_table_id_ll,
    int *cluster_index_ll);

shr_error_e mdb_eedb_cluster_type_to_rows(
    int unit,
    mdb_macro_types_e macro_type,
    int *cluster_rows);

shr_error_e mdb_eedb_hitbit_init(
    int unit);

mdb_eedb_mem_type_e mdb_eedb_table_to_mem_type(
    int unit,
    mdb_physical_table_e mdb_physical_table_id);

shr_error_e mdb_eedb_table_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode,
    int delete_cmd,
    int full_time);

dbal_enum_value_field_mdb_eedb_phase_e mdb_eedb_table_to_phase(
    int unit,
    mdb_physical_table_e mdb_physical_table_id);

shr_error_e mdb_eedb_translate(
    int unit,
    mdb_eedb_mem_type_e eedb_mem_type,
    dbal_enum_value_field_mdb_eedb_phase_e phase,
    mdb_physical_table_e * mdb_physical_table_id);

shr_error_e mdb_eedb_dynamic_bank_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode);

shr_error_e mdb_eedb_dynamic_cluster_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode);

shr_error_e mdb_eedb_phase_bank_to_index(
    int unit,
    int eedb_bank_index,
    dbal_enum_value_field_mdb_eedb_phase_e phase,
    uint32 *phase_bank_index);

shr_error_e mdb_get_macro_allocation(
    int unit,
    mdb_physical_table_e mdb_table_id,
    int macro_idx,
    mdb_macro_types_e macro_type,
    uint32 *macro_cluster_allocation);

shr_error_e mdb_init_get_cluster_assoc(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_idx,
    int cluster_idx,
    mdb_physical_table_e * mdb_physical_table_id);

shr_error_e mdb_init_set_cluster_assoc(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_idx,
    int cluster_idx,
    mdb_physical_table_e mdb_physical_table_id);

shr_error_e mdb_lpm_dbal_to_db(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint8 *db_idx);

shr_error_e mdb_lpm_deinit(
    int unit,
    int no_sync_flag);

shr_error_e mdb_lpm_dump_hw(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id);

shr_error_e mdb_lpm_init(
    int unit);

shr_error_e mdb_lpm_iterator_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id);

shr_error_e mdb_lpm_ser_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id);

shr_error_e mdb_lpm_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode,
    int delete_cmd,
    int full_time);

shr_error_e mdb_lpm_xpt_big_kaps_enabled(
    int unit,
    uint8 db_id,
    int *big_kaps_enabled);

kaps_status mdb_lpm_xpt_command(
    void *xpt,
    enum kaps_cmd cmd,
    enum kaps_func func,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint32_t nbytes,
    uint8_t * bytes);

shr_error_e mdb_lpm_xpt_entry_search(
    int unit,
    int core,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry);

kaps_status mdb_lpm_xpt_hb_read(
    void *xpt,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint8_t * bytes);

kaps_status mdb_lpm_xpt_hb_write(
    void *xpt,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint8_t * bytes);

int mdb_lpm_xpt_physical_table_is_bb(
    int unit,
    mdb_physical_table_e mdb_physical_table_id);

kaps_status mdb_lpm_xpt_register_read(
    void *xpt,
    uint32_t offset,
    uint32_t nbytes,
    uint8_t * bytes);

kaps_status mdb_lpm_xpt_register_write(
    void *xpt,
    uint32_t offset,
    uint32_t nbytes,
    uint8_t * bytes);

kaps_status mdb_lpm_xpt_search(
    void *xpt,
    uint8_t * key,
    enum kaps_search_interface search_interface,
    struct kaps_search_result *kaps_result);

shr_error_e mdb_parse_xml_em_vmv(
    int unit);

shr_error_e mdb_verify_macro_cluster_allocation(
    int unit,
    uint32 cluster_allocation);

#endif
