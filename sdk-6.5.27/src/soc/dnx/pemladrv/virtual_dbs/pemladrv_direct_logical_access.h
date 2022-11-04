
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _PEMLADRV_DIRECT_LOGICAL_ACCESS_H_
#define _PEMLADRV_DIRECT_LOGICAL_ACCESS_H_

#ifdef BCM_DNX_SUPPORT
shr_error_e dnx_pem_direct_access_wrapper(
    int unit,
    int sub_unit_idx,
    int is_read,
    int is_mem,
    int block_identifier,
    uint32 address,
    uint32 size_in_bits,
    uint32 *entry_data);
#endif

unsigned int dnx_get_nof_direct_chunk_cols(
    const int core_id,
    LogicalDirectInfo * direct_info);
unsigned int dnx_pemladrv_get_nof_direct_chunk_rows(
    const int unit,
    const unsigned int nof_entries);

shr_error_e pemladrv_direct_write_inner(
    const int unit,
    const int core_id,
    table_id_t db_id,
    uint32 row_index,
    pemladrv_mem_t * data);

#endif
