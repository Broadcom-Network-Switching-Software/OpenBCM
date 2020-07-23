/*
 * ! \file mdb_init.h $Id$ Contains all the MDB initialize files includes.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef MDB_H_INIT_INCLUDED
/*
 * {
 */
#define MDB_H_INIT_INCLUDED

/*
 * Include files.
 * {
 */
#include <shared/swstate/sw_state.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>
#include <soc/dnx/swstate/auto_generated/types/mdb_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/mdb.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */
#define MDB_LPM_MAX_FMT           (11)
#define MDB_LPM_NOF_FMT_BITS      (4)
#define MDB_LPM_MIN_FMT           (1)
#define MDB_NOF_DIRECT_FMT        (32)


#define MDB_BPU_MODE_BITS                       (3)
#define MDB_BPU_MODE_LPM                        (0)
#define MDB_BPU_MODE_TCAM                       (1)
#define MDB_BPU_MODE_EM_120                     (2)
#define MDB_BPU_MODE_EM_240                     (3)
#define MDB_BPU_MODE_DIRECT_120                 (4)
#define MDB_BPU_MODE_DIRECT_240                 (5)

/*
 * }
 */

/*
 * MACROs
 * {
 */

#define MDB_DIRECT_ROW_SIZE_TO_DIVIDER(_row_size) ((_row_size / MDB_NOF_CLUSTER_ROW_BITS) - 1)

#define MDB_CALC_FEC_BANK_SIZE(unit,bank_id) (MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit,(bank_id+1)) - MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit,bank_id))

/*
 * }
 */

/*
 * ENUMs
 * {
 */

/*
 * }
 */

/*
 * Structures
 * {
 */

/*
 * }
 */
/*
 * Globals
 * {
 */

/*
 * }
 */
/*
 * Externs
 * {
 */

/*
 * Function headers
 * {
 */
shr_error_e mdb_init_add_table_resources(
    int unit,
    int nof_valid_clusters,
    mdb_cluster_alloc_info_t clusters[MDB_MAX_NOF_CLUSTERS]);
shr_error_e mdb_init_delete_table_resources(
    int unit,
    int nof_valid_clusters,
    mdb_cluster_alloc_info_t clusters[MDB_MAX_NOF_CLUSTERS]);
shr_error_e mdb_dh_init(
    int unit);

shr_error_e mdb_step_table_init(
    int unit);

shr_error_e mdb_dh_init_table(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id);

shr_error_e mdb_dh_init_table_em_encoding(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id);

shr_error_e mdb_init_update_vmv(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id);

shr_error_e mdb_init_dh_add_em_format(
    int unit,
    uint32 key_size,
    dbal_physical_tables_e dbal_physical_table_id);

shr_error_e mdb_dh_set_bpu_setting(
    int unit);

shr_error_e mdb_init_em_logical_table(
    int unit,
    dbal_tables_e dbal_table,
    const dbal_logical_table_t * dbal_logical_table);

shr_error_e mdb_init_logical_table(
    int unit,
    dbal_tables_e dbal_table,
    void *access_info);

/*
 * }
 */
#endif /* !MDB_H_INIT_INCLUDED */
