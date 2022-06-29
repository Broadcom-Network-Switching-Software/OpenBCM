
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#include <hashtable.h>

void validate_read_from_lpm_em_designated_cache(
    const unsigned int offset,
    const unsigned int *virtual_db_data_array,
    const FieldBitRange * field_bit_range_arr,
    pemladrv_mem_t * mem_access,
    int field_id,
    int field_index,
    int field_width);

void validate_read_from_memory(
    int unit,
    phy_mem_t * mem,
    int is_mem,
    void *entry_data);

void validate_write_from_memory(
    int unit,
    phy_mem_t * mem,
    int is_mem,
    void *entry_data);
