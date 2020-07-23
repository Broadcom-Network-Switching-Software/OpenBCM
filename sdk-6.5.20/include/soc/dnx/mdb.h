
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
#include <soc/dnx/swstate/auto_generated/types/mdb_types.h>

#include <soc/dnx/swstate/auto_generated/access/mdb_kaps_access.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/swstate/sw_state.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>
#include <soc/dnx/swstate/auto_generated/types/mdb_types.h>
#include <soc/dnx/swstate/auto_generated/types/mdb_kaps_types.h>
#include <libs/kaps/include/kaps_errors.h>
#include <libs/kaps/include/kaps_default_allocator.h>
#include <libs/kaps/include/kaps_db.h>
#include <libs/kaps/include/kaps_ad.h>
#include <libs/kaps/include/kaps_instruction.h>
#include <libs/kaps/include/kaps_key.h>
#include <libs/kaps/include/kaps_xpt.h>

#define MDB_EEDB_PHASE_LOGICAL_ENTRIES_MAX_SIZE 8

#define MDB_DIRECT_BASIC_ENTRY_SIZE             30
#define MDB_NOF_CLUSTER_ROW_BITS                120
#define MDB_NOF_CLUSTER_ROW_UINT32              BITS2WORDS(MDB_NOF_CLUSTER_ROW_BITS)

#define MDB_NOF_CLUSTER_ROW_BYTE                BITS2BYTES(MDB_NOF_CLUSTER_ROW_BITS)

#define MDB_MAX_NOF_CLUSTERS     100

#define MDB_TEST_BRIEF_FACTOR                 (1024)

#define MDB_TEST_BRIEF_ENTRIES                (128)

#define MDB_DIRECT_INVALID_START_ADDRESS      (0xFFFFFFF)

#define MDB_INIT_CLUSTER_FLAG_2ND_INTERFACES          0x01
#define MDB_INIT_CLUSTER_FLAG_FLEX                    0x02
#define MDB_INIT_CLUSTER_FLAG_480_BPU                 0x04
#define MDB_INIT_CLUSTER_FLAG_960_BPU                 0x08
#define MDB_INIT_CLUSTER_FLAG_FIRST_INTF_CLUSTER      0x10

#define MDB_PAYLOAD_SIZE_TO_PAYLOAD_SIZE_TYPE(payload_size) ((payload_size / MDB_DIRECT_BASIC_ENTRY_SIZE) - 1)
#define MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_type) ((payload_type + 1) * MDB_DIRECT_BASIC_ENTRY_SIZE)

#define MDB_ENTRY_REMAINDER_CALC(entry_index,entries_size,row_width) ((entry_index * entries_size) % row_width)

#define MDB_DIRECT_PAYLOAD_SIZE_TO_MULTIPLIER(_payload_size) ((_payload_size / MDB_DIRECT_BASIC_ENTRY_SIZE))

#define MDB_MAX_DIRECT_PAYLOAD_SIZE_32    UTILEX_DIV_ROUND_UP(MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(DBAL_NOF_ENUM_MDB_DIRECT_PAYLOAD_VALUES-1), SAL_UINT32_NOF_BITS)

#define MDB_MAX_EM_PAYLOAD_SIZE_32 UTILEX_DIV_ROUND_UP(DNX_DATA_MAX_MDB_PDBS_MAX_PAYLOAD_SIZE,SAL_UINT32_NOF_BITS)

#define MDB_OUTLIF_AND_GRANULARITY_TO_PREFIX(unit, outlif, granularity) \
        ((outlif >> utilex_log2_round_up(dnx_data_mdb.dh.macro_type_info_get(unit, MDB_EEDB_BANK)->nof_rows * MDB_NOF_CLUSTER_ROW_BITS / granularity)) \
                & ((1 << dnx_data_mdb.eedb.bank_id_bits_get(unit)) - 1))

#define MDB_EM_TABLE_ENTRY_ENCODING_NOF_BITS     (3)

#define MDB_EM_TABLE_IS_EMP(mdb_physical_table) ((mdb_physical_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM) || (mdb_physical_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3) || (mdb_physical_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4))

#define MDB_DH_GET_GLOBAL_MACRO_INDEX(_unit,_macro_type,_macro_index) (dnx_data_mdb.dh.macro_type_info_get(_unit, _macro_type)->global_start_index + _macro_index)

#define MDB_INIT_GET_EM_EMCODING_NOF_BITS(unit, encod_req) (encod_req == MDB_EM_ENCODING_TABLE_ENTRY) ? MDB_EM_TABLE_ENTRY_ENCODING_NOF_BITS : ((encod_req == MDB_EM_ENCODING_DH_ENTRY_120) ? dnx_data_mdb.em.dh_120_entry_encoding_nof_bits_get(unit): dnx_data_mdb.em.dh_240_entry_encoding_nof_bits_get(unit))

#define MDB_KAPS_AD_WIDTH_IN_BYTES(unit)        BITS2BYTES(dnx_data_mdb.kaps.ad_width_in_bits_get(unit))

#define MDB_KAPS_KEY_WIDTH_IN_BYTES(unit)       BITS2BYTES(dnx_data_mdb.kaps.key_width_in_bits_get(unit))

#define MDB_KAPS_KEY_WIDTH_IN_UINT32(unit)      BITS2WORDS(dnx_data_mdb.kaps.key_width_in_bits_get(unit))

#define MDB_EM_TABLE_USE_ALL_ASPECT_RATIOS(mdb_table)  ((mdb_table == DBAL_PHYSICAL_TABLE_PPMC) || (mdb_table == DBAL_PHYSICAL_TABLE_SEXEM_1) || (mdb_table == DBAL_PHYSICAL_TABLE_SEXEM_2) || (mdb_table == DBAL_PHYSICAL_TABLE_SEXEM_3) || (mdb_table == DBAL_PHYSICAL_TABLE_LEXEM))

#define MDB_INIT_IS_FEC_TABLE(mdb_physical_table) ((mdb_physical_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1) \
                                                || (mdb_physical_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2) \
                                                || (mdb_physical_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3))

#define MDB_INIT_IS_EEDB_RELATED_TABLE(mdb_physical_table) ((mdb_physical_table >= DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_LL) && (mdb_physical_table <= DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA ))

#define MDB_DH_IS_TABLE_SUPPORT_FLEX(unit,mdb_table) ((dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_table)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM) && \
                                                   dnx_data_mdb.dh.dh_info_get(unit, mdb_table)->row_width == dnx_data_mdb.dh.cluster_row_width_bits_get(unit))

#define MDB_DH_SERIAL_CLUSTER_NUMBER_FOR_MACRO_TYPE(unit, macro_type, macro_idx, cluster_idx) (((macro_idx)*dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->nof_clusters)+cluster_idx)

#define MDB_MACRO_NOF_FEC_BANKS_GET(macro_type) ((macro_type == MDB_MACRO_A) ? 2 : 1)

#define MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit,bank_id) (dnx_data_mdb.direct.nof_fecs_in_super_fec_get(unit)*UTILEX_DIV_ROUND_UP(bank_id*dnx_data_l3.fec.nof_physical_fecs_per_bank_get(unit)*dnx_data_mdb.pdbs.pdb_info_get(unit,DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1)->row_width,dnx_data_l3.fec.super_fec_size_get(unit)))

#define MDB_EEDB_POINTER_FORMAT_17BIT_RES_1k     (0)
#define MDB_EEDB_POINTER_FORMAT_18BIT_RES_1k     (1)
#define MDB_EEDB_POINTER_FORMAT_19BIT_RES_2k     (2)
#define MDB_EEDB_POINTER_FORMAT_20BIT_RES_4k     (3)

#define MDB_NOF_PHASES_PER_EEDB_BANK                    (3)
#define MDB_NOF_PHASES_PER_EEDB_BANK_BITS               (2)
#define MDB_NOF_TYPE_BANK_SELECT_PER_EEDB_BANK_BITS     (1)
#define MDB_PHASE_BANK_SELECT_DEFAULT                   (0)

#define MDB_APP_ID_ITER_ALL                         (SAL_UINT32_MAX)

typedef struct kaps_key *mdb_kaps_key_t_p;

typedef enum
{

    MDB_DH_INIT_FIRST_IF_USED = 0,

    MDB_DH_INIT_SECOND_IF_USED = 1,

    MDB_DH_INIT_MAX_AVAILABLE_INTERFACES = 2
} mdb_dh_init_interface_instance_in_macro_e;

typedef enum
{
    MDB_INIT_CLUSTER_ADD,
    MDB_INIT_CLUSTER_DELETE,

    MDB_NOF_INIT_CLUSTER_OPERATIONS
} mdb_init_cluster_func_e;

typedef enum
{
    MDB_KAPS_KEY_PREFIX_IPV4_UC = 0,
    MDB_KAPS_KEY_PREFIX_IPV4_MC = 2,
    MDB_KAPS_KEY_PREFIX_IPV6_UC = 3,
    MDB_KAPS_KEY_PREFIX_IPV6_MC = 4,
    MDB_KAPS_KEY_PREFIX_FCOE = 6,
    MDB_NOF_KAPS_KEY_PREFIX
} mdb_kaps_key_prefix_e;

typedef enum
{
    MDB_EEDB_MEM_TYPE_PHY,
    MDB_EEDB_MEM_TYPE_LL,

    MDB_NOF_EEDB_MEM_TYPES
} mdb_eedb_mem_type_e;

typedef enum
{
    MDB_EM_ENTRY_ENCODING_ONE = 0,
    MDB_EM_ENTRY_ENCODING_HALF = 1,
    MDB_EM_ENTRY_ENCODING_QUARTER = 2,
    MDB_EM_ENTRY_ENCODING_EIGHTH = 3,
    MDB_EM_ENTRY_ENCODING_EMPTY = 7,
    MDB_EM_NOF_ENTRY_ENCODINGS
} mdb_em_entry_encoding_e;

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
    MDB_STEP_TABLE_ENTRY_SIZE_ONE = 0,
    MDB_STEP_TABLE_ENTRY_SIZE_HALF = 1,
    MDB_STEP_TABLE_ENTRY_SIZE_QUARTER = 2,
    MDB_STEP_TABLE_ENTRY_SIZE_EIGHTH = 3,
    MDB_STEP_TABLE_NOF_ENTRY_SIZES
} mdb_step_table_entry_size_e;

typedef enum
{

    MDB_EM_ENCODING_TABLE_ENTRY,

    MDB_EM_ENCODING_DH_ENTRY_120,

    MDB_EM_ENCODING_DH_ENTRY_240,

    MDB_EM_NOF_ENCODING_REQUESTS
} mdb_em_encoding_request_e;

typedef enum
{

    MDB_TEST_BRIEF,

    MDB_TEST_CAPACITY,

    MDB_TEST_CAPACITY_MIXED_EE,

    MDB_TEST_ITERATOR,

    MDB_TEST_SER,

    MDB_NOF_TEST_MODES
} mdb_test_mode_e;

typedef enum
{
    MDB_EEDB_TABLE_A,
    MDB_EEDB_TABLE_B,
    MDB_EEDB_TABLE_C,
    MDB_EEDB_TABLE_D,
    MDB_NOF_EEDB_TABLES
} mdb_eedb_table_e;

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

extern const char cmd_mdb_usage[];

extern const dbal_physical_tables_e mdb_direct_mdb_to_dbal[DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES];

shr_error_e mdb_pre_init_step(
    int unit);

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

shr_error_e mdb_em_get_addr_bits(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 *addr_bits);

shr_error_e mdb_em_get_min_entry_encoding(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 key_size,
    uint32 payload_size,
    uint32 app_id,
    int app_id_size,
    int vmv_size,
    mdb_em_entry_encoding_e * entry_encoding);

shr_error_e mdb_eedb_translate(
    int unit,
    mdb_eedb_mem_type_e eedb_mem_type,
    dbal_enum_value_field_mdb_eedb_phase_e phase,
    mdb_macro_types_e macro_type,
    dbal_enum_value_field_mdb_physical_table_e * mdb_physical_table_id);

shr_error_e mdb_eedb_cluster_type_to_rows(
    int unit,
    mdb_macro_types_e macro_type,
    int *cluster_rows);

dbal_enum_value_field_mdb_eedb_phase_e mdb_eedb_table_to_phase(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id);

shr_error_e mdb_init_eedb_phase(
    int unit,
    dbal_enum_value_field_mdb_eedb_phase_e phase_index,
    soc_reg_above_64_val_t type_bank_select,
    soc_reg_above_64_val_t phase_bank_select,
    soc_reg_above_64_val_t bank_id);

shr_error_e mdb_init_eedb_ll_db_atr(
    int unit,
    dbal_enum_value_field_mdb_eedb_phase_e phase_index);

shr_error_e mdb_init_set_cluster_assoc(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_idx,
    int cluster_idx,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id);

shr_error_e mdb_init_eedb(
    int unit);

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

shr_error_e mdb_em_get_key_size(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    uint32 *key_size);

shr_error_e mdb_direct_table_get_direct_payload_type(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id,
    dbal_enum_value_field_direct_payload_sizes_e * payload_type);

shr_error_e mdb_direct_table_get_basic_size(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id,
    int *basic_size);

shr_error_e mdb_direct_table_get_basic_size_using_logical_mdb_table(
    int unit,
    dbal_physical_tables_e logic_table_id,
    int *basic_size);

shr_error_e mdb_verify_macro_cluster_allocation(
    int unit,
    uint32 cluster_allocation);

shr_error_e mdb_em_set_vmv_regs(
    int unit,
    uint8 vmv_value,
    uint8 vmv_size,
    uint8 vmv_total_nof_bits,
    uint32 encoding_value,
    uint32 encoding_nof_bits,
    soc_reg_above_64_val_t vmv_encoding_data,
    soc_reg_above_64_val_t vmv_size_data);

shr_error_e mdb_direct_table_get_row_width(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int *row_width);

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

shr_error_e mdb_direct_table_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode,
    int full_time);

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

shr_error_e mdb_direct_init(
    int unit);

shr_error_e mdb_direct_address_mapping_alloc(
    int unit);

shr_error_e mdb_direct_address_mapping_verify(
    int unit,
    int physical_address_start,
    int physical_address_end);

shr_error_e mdb_direct_address_mapping_set(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id,
    int physical_address_start,
    int physical_address_end,
    int cluster_index);

shr_error_e mdb_direct_address_mapping_unset(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id,
    int physical_address_start,
    int physical_address_end);

shr_error_e mdb_direct_address_mapping_get(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id,
    int physical_address,
    int *cluster_index);

shr_error_e mdb_eedb_address_mapping_alloc(
    int unit);

shr_error_e mdb_eedb_address_mapping_set(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id,
    int logical_address_start,
    int logical_address_end,
    int cluster_index);

shr_error_e mdb_eedb_address_mapping_unset(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id,
    int logical_address_start,
    int logical_address_end);

shr_error_e mdb_eedb_address_mapping_get(
    int unit,
    int logical_address,
    dbal_enum_value_field_mdb_physical_table_e * mdb_physical_table_id_data,
    int *cluster_index_data,
    dbal_enum_value_field_mdb_physical_table_e * mdb_physical_table_id_ll,
    int *cluster_index_ll);

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

shr_error_e mdb_em_dbal_table_to_emp(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    dbal_enum_value_field_mdb_emp_table_e * mdb_emp_table_id);

shr_error_e mdb_em_delete_by_addr(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_index,
    int cluster_index,
    int cluster_addr);

shr_error_e mdb_em_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    mdb_test_mode_e mode,
    int delete_cmd,
    int full_time);

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

shr_error_e mdb_em_get_min_remaining_entry_count(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int *free_entry_count);

shr_error_e mdb_em_hw_entry_count_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int *count);

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

shr_error_e mdb_em_init_shadow_db(
    int unit);

shr_error_e mdb_em_init_overrides(
    int unit);

shr_error_e mdb_lpm_init(
    int unit);

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

shr_error_e mdb_lpm_entry_search(
    int unit,
    int core,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry);

shr_error_e mdb_lpm_entry_delete(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id_arr[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);

shr_error_e mdb_lpm_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode,
    int delete_cmd,
    int full_time);

shr_error_e mdb_lpm_ser_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id);

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

shr_error_e mdb_lpm_dump_hw(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id);

uint32 mdb_lpm_calculate_prefix_length(
    uint32 *payload_mask);

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

shr_error_e mdb_lpm_dbal_to_db(
    int unit,
    dbal_physical_tables_e physical_table,
    mdb_kaps_ip_db_id_e * db_idx);

shr_error_e mdb_lpm_get_nof_small_bbs(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int *nof_small_bbs);

shr_error_e mdb_lpm_prefix_len_to_mask(
    int unit,
    int prefix_length,
    dbal_physical_entry_t * entry);

shr_error_e mdb_lpm_deinit_app(
    int unit,
    int no_sync_flag);

shr_error_e mdb_lpm_big_kaps_enabled(
    int unit,
    mdb_kaps_ip_db_id_e db_id,
    int *big_kaps_enabled);

shr_error_e mdb_init_lpm_xpt_inner(
    int unit);

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

shr_error_e mdb_eedb_table_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode,
    int full_time);

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

shr_error_e mdb_eedb_entry_to_phase(
    int unit,
    uint32 entry_index,
    dbal_physical_tables_e * dbal_physical_table_id);

shr_error_e mdb_init_get_table_resources(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_cluster_alloc_info_t clusters[MDB_MAX_NOF_CLUSTERS],
    int *nof_valid_clusters);

shr_error_e mdb_init_get_cluster_assoc(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_idx,
    int cluster_idx,
    dbal_enum_value_field_mdb_physical_table_e * mdb_physical_table_id);

shr_error_e mdb_dh_set_bpu_setting_macro(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_idx);

shr_error_e mdb_dh_em_way_index_get(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id,
    mdb_macro_types_e macro_type,
    uint8 macro_idx,
    uint8 cluster_idx,
    uint32 *way_index);

shr_error_e mdb_eedb_dynamic_bank_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode);

shr_error_e mdb_eedb_dynamic_cluster_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode);

shr_error_e mdb_get_macro_allocation(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_table_id,
    int macro_idx,
    mdb_macro_types_e macro_type,
    uint32 *macro_cluster_allocation);

shr_error_e mdb_eedb_phase_bank_to_index(
    int unit,
    int eedb_bank_index,
    dbal_enum_value_field_mdb_eedb_phase_e phase,
    uint32 *phase_bank_index);

shr_error_e mdb_em_get_entry_encoding(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 key_size,
    uint32 payload_size,
    uint32 app_id,
    int app_id_size,
    mdb_em_entry_encoding_e * entry_encoding);

shr_error_e mdb_parse_xml_em_vmv(
    int unit);

shr_error_e mdb_init_em_age_nof_bits_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_em_entry_encoding_e entry_encoding,
    int *nof_age_bits);

void mdb_init_em_set_b_access_interface_regs(
    void);

shr_error_e mdb_init_eedb_deinit_bank(
    int unit,
    int bank_idx);

shr_error_e mdb_direct_get_payload_type(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    dbal_enum_value_field_mdb_direct_payload_e * payload_type);

shr_error_e mdb_em_calc_vmv_regs(
    int unit,
    mdb_em_encoding_request_e encoding_request,
    mdb_em_encoding_request_e encoding_nof_bits_request,
    dbal_physical_tables_e dbal_table_id,
    uint8 esem_cmd_indication,
    soc_reg_above_64_val_t vmv_encoding_data,
    soc_reg_above_64_val_t vmv_size_data);

shr_error_e mdb_table_is_allocated_get(
    int unit,
    dbal_physical_tables_e dbal_phy_table_id,
    uint8 *is_allocated);

shr_error_e mdb_dbal_table_is_mdb(
    int unit,
    dbal_tables_e dbal_logical_table_id,
    uint8 *is_mdb);

shr_error_e mdb_eedb_hitbit_init(
    int unit);

int dnx_mdb_init(
    int unit);

int dnx_mdb_deinit(
    int unit);

shr_error_e mdb_arm_init(
    int unit);

shr_error_e mdb_arm_deinit(
    int unit);

shr_error_e mdb_em_mact_age_refresh_mode_set(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_table,
    uint32 mode);

shr_error_e mdb_em_mact_age_refresh_mode_get(
    int unit,
    uint32 *mode);

shr_error_e mdb_em_vmv_calculation(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint8 *vmv_size,
    uint8 *vmv_value);

#endif
