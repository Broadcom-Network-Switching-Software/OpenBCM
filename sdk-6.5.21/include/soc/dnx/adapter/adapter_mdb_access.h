/** \file adapter_mdb_access.h
 * This file handles the communication with the adapter's server for MDB logical access
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. $
 */

#ifndef ADAPTER_MDB_ACCESS_H_INCLUDED

#define ADAPTER_MDB_ACCESS_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/dnx/dbal/dbal_structures.h>

shr_error_e adapter_mdb_access_lpm_write(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry);

shr_error_e adapter_mdb_access_lpm_delete(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry);

shr_error_e adapter_mdb_access_lpm_read(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry);

shr_error_e adapter_mdb_access_direct_table_write(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry);

shr_error_e adapter_mdb_access_direct_table_read(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry);

shr_error_e adapter_mdb_access_em_write(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry);

shr_error_e adapter_mdb_access_em_delete(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry);

shr_error_e adapter_mdb_access_em_read(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry);

shr_error_e adapter_mdb_access_eedb_write(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    int ll_exist,
    dbal_physical_entry_t * entry);

shr_error_e adapter_mdb_access_eedb_read(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry);

#endif
