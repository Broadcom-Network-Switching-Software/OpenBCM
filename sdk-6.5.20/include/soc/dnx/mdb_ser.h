
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef MDB_SER_H_INCLUDED

#define MDB_SER_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/dnx/mdb.h>
#include <soc/dnx/utils/dnx_ire_packet_utils.h>

#define MDB_SER_ENTRY_SIZE_IN_BITS          128
#define MDB_SER_ENTRY_DATA_SIZE_IN_BITS     120
#define MDB_SER_ENTRY_ECC_SIZE_IN_BITS      8
#define MDB_SER_ENTRY_DATA_SIZE_IN_U8       15
#define MDB_SER_ENTRY_SIZE_IN_U8            16
#define MDB_SER_ENTRY_SIZE_IN_U32           4

#define MDB_SER_ADDR_OFFSET_OFFSET(unit,macro_type) (dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->nof_address_bits)
#define MDB_SER_ADDR_OFFSET_BITS                    (2)
#define MDB_SER_ADDR_OFFSET_MASK(unit,macro_type)   ((1 << MDB_SER_ADDR_OFFSET_OFFSET(unit,macro_type)) - 1)

#define MDB_SER_CLUSTER_OFFSET_NONE         0

void mdb_ser_print_cluster_entry(
    int unit,
    uint32 *entry_data,
    uint32 ecc,
    char *print_msg);

shr_error_e mdb_phy_table_db_info_dump(
    int unit,
    int detail);

shr_error_e mdb_phy_mem_2_access_mem(
    int unit,
    soc_mem_t phy_mem,
    soc_mem_t * access_mem);

shr_error_e mdb_macro_map_info_dump(
    int unit);

shr_error_e mdb_dh_macro_cluster_entry_get(
    int unit,
    mdb_macro_types_e macro_type,
    uint32 macro_idx,
    uint32 cluster_idx,
    uint32 row_idx,
    uint32 *entry_data,
    uint32 *ecc);

shr_error_e mdb_dh_macro_cluster_entry_set(
    int unit,
    mdb_macro_types_e macro_type,
    uint32 macro_idx,
    uint32 cluster_idx,
    uint32 row_idx,
    uint32 *entry_data,
    uint32 ecc);

shr_error_e mdb_dh_physical_mem_entry_get(
    int unit,
    soc_mem_t phy_mem,
    int blk,
    uint32 array_idx,
    uint32 entry_idx,
    uint32 *entry_data);

shr_error_e mdb_dh_physical_mem_entry_set(
    int unit,
    soc_mem_t phy_mem,
    int blk,
    uint32 array_idx,
    uint32 entry_idx,
    uint32 *entry_data);

shr_error_e mdb_dh_macro_cluster_chg_dump(
    int unit,
    mdb_macro_types_e macro_type,
    uint32 macro_idx,
    uint32 cluster_idx);

shr_error_e mdb_phy_table_cluster_chg_dump(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_phy_table_id);

shr_error_e mdb_ser_ecc_1b_interrupt_data_correct(
    int unit,
    int blk,
    soc_mem_t mem,
    uint32 array_idx,
    uint32 entry_idx);

shr_error_e mdb_ser_ecc_2b_interrupt_data_correct(
    int unit,
    int blk,
    soc_mem_t mem,
    uint32 array_idx,
    uint32 entry_idx);

char *mdb_get_db_type_str(
    dbal_enum_value_field_mdb_db_type_e mdb_db_type);

char *mdb_physical_table_to_string(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_phy_table_id);

shr_error_e mdb_physical_table_string_to_id(
    int unit,
    char *str,
    dbal_enum_value_field_mdb_physical_table_e * mdb_phy_table_id);

shr_error_e mdb_ser_map_mem_to_macro_index(
    int unit,
    int blk,
    soc_mem_t mem,
    mdb_macro_types_e * macro_type,
    uint32 *macro_index);

shr_error_e mdb_ser_map_access_mem_to_macro_index(
    int unit,
    int blk,
    soc_mem_t access_mem,
    mdb_macro_types_e * macro_type,
    uint32 *macro_index);

shr_error_e mdb_ser_map_cluster_to_dbal_phy_table(
    int unit,
    mdb_macro_types_e macro_type,
    uint32 macro_index,
    uint32 cluster_index,
    dbal_physical_tables_e * dbal_physical_table_id);

shr_error_e mdb_dh_macro_cluster_cache_check(
    int unit,
    mdb_macro_types_e macro_type,
    uint32 macro_idx,
    uint32 cluster_idx);

shr_error_e mdb_phy_table_cluster_cache_check(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_phy_table_id);

int mdb_ser_eedb_array_write(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    void *entry_data);

int mdb_ser_eedb_array_read_flags(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    void *entry_data,
    int flags);

#endif
