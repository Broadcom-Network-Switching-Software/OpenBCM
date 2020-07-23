
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef MDB_H_INTERNAL_INCLUDED

#define MDB_H_INTERNAL_INCLUDED

#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/swstate/auto_generated/types/mdb_kaps_types.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sbusdma_desc.h>

#define MDB_ENTRY_DATA_SIZE_IN_BITS            128
#define MDB_ENTRY_DATA_SIZE_IN_UINT32          BITS2WORDS(MDB_ENTRY_DATA_SIZE_IN_BITS)

#define MDB_ENTRY_DATA_TOTAL_SIZE_IN_BITS      (MDB_ENTRY_DATA_SIZE_IN_BITS + 8)
#define MDB_ENTRY_BANK_SIZE_IN_BITS            544
#if (MDB_ENTRY_BANK_SIZE_IN_BITS % SAL_UINT32_NOF_BITS) != 0
#error "MDB_ENTRY_BANK_SIZE_IN_BITS assumes to be divided by SAL_UINT32_NOF_BITS"
#endif
#define MDB_ENTRY_BANK_SIZE_UINT32             (MDB_ENTRY_BANK_SIZE_IN_BITS/SAL_UINT32_NOF_BITS)
#if (MDB_ENTRY_DATA_SIZE_IN_BITS % SAL_UINT32_NOF_BITS) != 0
#error "MDB_ENTRY_DATA_SIZE_IN_BITS assumes to be divided by SAL_UINT32_NOF_BITS"
#endif
#define MDB_ENTRY_DATA_SIZE_UINT32             BITS2WORDS(MDB_ENTRY_DATA_SIZE_IN_BITS)
#define MDB_ENTRY_DATA_PER_BANK                (MDB_ENTRY_BANK_SIZE_IN_BITS/MDB_ENTRY_DATA_SIZE_IN_BITS)

#define MDB_EM_ADAPTER_SIZE                      (4096)

#define MDB_LPM_INSTRUCTIONS_ID  0

#define MDB_LPM_KAPS_CFG_SMALL_KAPS_ENABLE 100

#ifndef ADAPTER_SERVER_MODE
#define MDB_EEDB_ACCESS_UNLOCK(_unit,_expr)                                                             \
do                                                                                                       \
{                                                                                                        \
    int _macro_rv = 0;                                                                                   \
    int _attempt_counter = 0;                                                                            \
    int _enable_feature = dnx_data_mdb.feature.feature_get(_unit, dnx_data_mdb_feature_eedb_bank_traffic_lock); \
    uint32 error_check_en = FALSE;                                                                       \
    uint32 timeout_check_en = FALSE;                                                                     \
    if (_enable_feature)                                                                                 \
    {                                                                                                    \
        error_check_en = SOC_FEATURE_GET(unit, soc_feature_schan_err_check);                             \
        SOC_FEATURE_CLEAR(unit, soc_feature_schan_err_check);                                            \
        timeout_check_en = SOC_FEATURE_GET(unit, soc_feature_schan_hw_timeout);                          \
        SOC_FEATURE_CLEAR(unit, soc_feature_schan_hw_timeout);                                           \
        SHR_IF_ERR_EXIT(dnx_ire_packet_start(_unit));                                                    \
    }                                                                                                    \
    do                                                                                                   \
    {                                                                                                    \
        _macro_rv = _expr;                                                                               \
        _attempt_counter++;                                                                              \
    } while (_enable_feature && (_attempt_counter < 10));                                                \
    if (_enable_feature)                                                                                 \
    {                                                                                                    \
        if (error_check_en)                                                                              \
        {                                                                                                \
            SOC_FEATURE_SET(unit, soc_feature_schan_err_check);                                          \
        }                                                                                                \
        if (timeout_check_en)                                                                            \
        {                                                                                                \
            SOC_FEATURE_SET(unit, soc_feature_schan_hw_timeout);                                         \
        }                                                                                                \
        SHR_IF_ERR_EXIT(dnx_ire_packet_end(_unit));                                                      \
    } else                                                                                               \
    {                                                                                                    \
        SHR_IF_ERR_EXIT(_macro_rv);                                                                      \
    }                                                                                                    \
} while (0);
#else
#define MDB_EEDB_ACCESS_UNLOCK(_unit,_expr)                                                             \
do                                                                                                       \
{                                                                                                        \
    SHR_IF_ERR_EXIT(_expr);                                                                              \
} while (0);
#endif

typedef enum
{
    MDB_EM_DH_120_ENTRY_ENCODING_ONE = 3,
    MDB_EM_DH_120_ENTRY_ENCODING_HALF = 2,
    MDB_EM_DH_120_ENTRY_ENCODING_QUARTER = 1,
    MDB_EM_DH_120_ENTRY_ENCODING_EMPTY = 0,
    MDB_EM_DH_120_NOF_ENTRY_ENCODINGS
} mdb_em_dh_120_entry_encoding_e;

typedef enum
{
    MDB_EM_DH_240_ENTRY_ENCODING_ONE = 4,
    MDB_EM_DH_240_ENTRY_ENCODING_HALF = 3,
    MDB_EM_DH_240_ENTRY_ENCODING_QUARTER = 2,
    MDB_EM_DH_240_ENTRY_ENCODING_EIGHTH = 1,
    MDB_EM_DH_240_ENTRY_ENCODING_EMPTY = 0,
    MDB_EM_DH_240_NOF_ENTRY_ENCODINGS
} mdb_em_dh_240_entry_encoding_e;

typedef enum
{
    MDB_DIRECT_REMAINDER_TYPE_0B,
    MDB_DIRECT_REMAINDER_TYPE_30B,
    MDB_DIRECT_REMAINDER_TYPE_60B,
    MDB_DIRECT_REMAINDER_TYPE_90B,
    MDB_DIRECT_REMAINDER_TYPE_120B,
    MDB_DIRECT_REMAINDER_TYPE_150B,
    MDB_DIRECT_REMAINDER_TYPE_180B,
    MDB_DIRECT_REMAINDER_TYPE_210B,
    MDB_NOF_DIRECT_REMAINDER_TYPES
} mdb_direct_remainder_types_e;

typedef enum
{
    MDB_DIRECT_ROW_WIDTH_TYPE_120B,
    MDB_DIRECT_ROW_WIDTH_TYPE_240B,
    MDB_NOF_DIRECT_ROW_WIDTH_TYPES
} mdb_direct_row_width_types_e;

typedef enum
{

    MDB_DIRECT_ENTRY_HIT_BIT_INDEX_1 = 0,

    MDB_DIRECT_ENTRY_HIT_BIT_INDEX_2 = 1,

    MDB_DIRECT_ENTRY_NOF_HIT_BIT_INDEXS = 2
} mdb_direct_entry_hit_bit_index_e;

typedef enum
{

    MDB_ABK_BANK_A = 0,

    MDB_ABK_BANK_B = 1,

    MDB_NOF_ABK_BANKS = 2
} mdb_abk_banks_e;

extern shr_error_e dbal_tables_table_by_mdb_phy_get(
    int unit,
    dbal_physical_tables_e phy_db_id,
    uint32 app_db_id,
    dbal_tables_e * table_id);

extern shr_error_e dbal_tables_physical_table_get(
    int unit,
    dbal_tables_e table_id,
    int physical_tbl_index,
    dbal_physical_tables_e * physical_table_id);

shr_error_e mdb_direct_cache_init(
    int unit);

shr_error_e mdb_direct_cache_alloc(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_index,
    int cluster_index);

shr_error_e mdb_direct_cache_set(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_index,
    int cluster_index,
    int row_index,
    uint32 src_offset_bits,
    uint32 *src_array);

shr_error_e mdb_direct_cache_get(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_index,
    int cluster_index,
    int row_index,
    uint32 dst_offset_bits,
    uint32 *dst_array);

shr_error_e mdb_direct_calc_macro_mem_blk(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_index,
    soc_mem_t * mem_entry,
    soc_reg_t * overflow_reg,
    soc_mem_t mem_hitbits[MDB_NOF_ABK_BANKS],
    int *blk);

shr_error_e mdb_parse_xml_direct_physical_map(
    int unit);

shr_error_e mdb_lpm_xpt_init(
    int unit,
    void **xpt);

shr_error_e mdb_lpm_xpt_deinit(
    int unit,
    void *xpt);

shr_error_e mdb_lpm_xpt_dump_hw(
    int unit,
    mdb_kaps_ip_db_id_e small_kaps_db,
    dbal_enum_value_field_mdb_physical_table_e big_kaps_ads,
    dbal_enum_value_field_mdb_physical_table_e big_kaps_bb);

shr_error_e mdb_lpm_uint8_to_uint32(
    int unit,
    int nof_bytes,
    uint8 *uint8_data,
    uint32 *uint32_data);

shr_error_e mdb_lpm_uint32_to_uint8(
    int unit,
    int nof_bytes,
    uint32 *uint32_data,
    uint8 *uint8_data);

shr_error_e mdb_eedb_calc_capacity(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int *capacity);

mdb_eedb_mem_type_e mdb_eedb_table_to_mem_type(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id);

shr_error_e mdb_get_dh_block(
    int unit,
    uint32 global_macro_index,
    int *blk);

shr_error_e mdb_arm_image_load_default(
    int unit);

shr_error_e mdb_num_valid_tables(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id_arr[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    int *tables_num);

shr_error_e mdb_parse_xml_update_fec_address(
    int unit);

#endif
