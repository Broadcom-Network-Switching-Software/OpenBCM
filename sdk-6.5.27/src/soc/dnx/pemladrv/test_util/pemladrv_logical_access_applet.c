
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include "../pemladrv_logical_access.h"
#include "../pemladrv_meminfo_access.h"
#include "../pemladrv_cfg_api_defines.h"

#ifdef BCM_DNX_SUPPORT
#define BSL_LOG_MODULE BSL_LS_SOCDNX_ADAPTERDNX
#endif

extern ApiInfo api_info[MAX_NOF_UNITS];

int
init_pem_applets_db(
    const int unit)
{
    int mem_ndx, cache_ndx, core;
    unsigned int cache_entry;
    int nof_previously_allocated_mems = api_info[unit].applet_info.meminfo_curr_array_size;

    for (mem_ndx = 0; mem_ndx < nof_previously_allocated_mems; ++mem_ndx)
    {
        SHR_FREE(api_info[unit].applet_info.meminfo_array_for_applet[mem_ndx]);
    }
    SHR_FREE(api_info[unit].applet_info.meminfo_array_for_applet);

    for (core = 0; core < PEM_CFG_API_NOF_CORES_DYNAMIC; ++core)
    {
        for (cache_ndx = 0; cache_ndx < 2; ++cache_ndx)
        {
            for (cache_entry = 0; cache_entry < api_info[unit].applet_info.nof_pending_applets[core][cache_ndx];
                 ++cache_entry)
            {
                SHR_FREE(api_info[unit].applet_info.applet_mem_cache[core][cache_ndx][cache_entry]);
            }
            api_info[unit].applet_info.nof_pending_applets[core][cache_ndx] = 0;
        }
    }
    api_info[unit].applet_info.meminfo_curr_array_size = 0;
    api_info[unit].applet_info.currently_writing_applets_bit = 0;

    return 0;
}

void
read_data_from_applets_cache(
    const int unit,
    int entry,
    unsigned int core,
    int is_ingress0_egress1,
    uint32 *data,
    int data_size_in_bits)
{
    uint32 *entry_to_copy = api_info[unit].applet_info.applet_mem_cache[core][is_ingress0_egress1][entry];
    uint32 temp_dest_data = 0;
    int first_entry_of_data = (ADDRESS_SIZE_IN_BITS + BLOCK_ID_SIZE_IN_BITS) / (8 * sizeof(uint32));

    int shift_left_amount = 8 * sizeof(uint32) - BLOCK_ID_SIZE_IN_BITS;
    int shift_right_amount = (ADDRESS_SIZE_IN_BITS + BLOCK_ID_SIZE_IN_BITS) % (8 * sizeof(uint32));
    int source_ndx, dest_ndx, nof_bits_left_to_copy = data_size_in_bits;

    for (source_ndx = first_entry_of_data, dest_ndx = 0; nof_bits_left_to_copy > 0;
         ++dest_ndx, ++source_ndx, temp_dest_data = 0)
    {
        temp_dest_data = entry_to_copy[source_ndx];
        temp_dest_data >>= shift_right_amount;
        nof_bits_left_to_copy -= shift_left_amount;
        if (nof_bits_left_to_copy > 0)
            temp_dest_data |= (entry_to_copy[source_ndx + 1] << shift_left_amount);
        data[dest_ndx] = temp_dest_data;
        nof_bits_left_to_copy -= shift_right_amount;
    }
}

void
write_to_applets_cache(
    const int unit,
    int entry,
    int is_ingress0_egress1,
    uint32 *data,
    int data_size_in_bits,
    unsigned long long full_address)
{
    uint32 address, block_id;
    int core = get_core_from_memory_address(unit, full_address);
    uint32 *entry_to_write = api_info[unit].applet_info.applet_mem_cache[core][is_ingress0_egress1][entry];

    get_address_and_block_id(full_address, &address, &block_id);

    set_bits(&block_id, 0, 0, BLOCK_ID_SIZE_IN_BITS, entry_to_write);

    set_bits(&address, 0, BLOCK_ID_SIZE_IN_BITS, ADDRESS_SIZE_IN_BITS, entry_to_write);
    set_bits(data, 0, BLOCK_ID_SIZE_IN_BITS + ADDRESS_SIZE_IN_BITS, data_size_in_bits, entry_to_write);

}

void
build_phy_mem_struct_for_applets_flush(
    const int unit,
    phy_mem_t * mem,
    const int core,
    const int is_ingress0_egress1,
    const int is_mem0_reg1)
{
    mem->mem_address =
        (is_mem0_reg1 ==
         1) ? (api_info[unit].applet_info.applet_reg_info[core][is_ingress0_egress1].
               address) : (api_info[unit].applet_info.applet_mem_info[core][is_ingress0_egress1].address);
    mem->block_identifier =
        (is_mem0_reg1 ==
         1) ? (api_info[unit].applet_info.applet_reg_info[core][is_ingress0_egress1].
               block_id) : (api_info[unit].applet_info.applet_mem_info[core][is_ingress0_egress1].block_id);
    mem->mem_width_in_bits =
        (is_mem0_reg1 ==
         1) ? (api_info[unit].applet_info.applet_reg_info[core][is_ingress0_egress1].
               length_in_bits) : (api_info[unit].applet_info.applet_mem_info[core][is_ingress0_egress1].length_in_bits);
}

shr_error_e
wait_for_trigger_reg(
    const int unit,
    const int is_ingress0_egress1)
{
    unsigned int reg_val = 0;
    phy_mem_t reg_info = { 0 };
    int sub_unit_idx = 0;

    SHR_FUNC_INIT_VARS(unit);

    reg_info.is_reg = 1;
    build_phy_mem_struct_for_applets_flush(unit, &reg_info, 0, is_ingress0_egress1, 1);
    do
    {

#ifdef BCM_DNX_SUPPORT
        SHR_IF_ERR_EXIT(pem_read(unit, sub_unit_idx, &reg_info, reg_info.is_reg, &reg_val));
#endif
        reg_val >>= APPLET_REG_AMOUNT_OF_PACKETS_SIZE_IN_BITS;
    }
    while (reg_val != 0);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
start_writing_applets(
    const int unit)
{
    int applets_bit_state = api_info[unit].applet_info.currently_writing_applets_bit;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(wait_for_trigger_reg(unit, 0));

    SHR_IF_ERR_EXIT(wait_for_trigger_reg(unit, 1));

    assert(0 == applets_bit_state);
    api_info[unit].applet_info.currently_writing_applets_bit = 1;

    if (applets_bit_state == 0)
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "applets_bit_state %d", applets_bit_state);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flush_applets_to_single_mem(
    const int unit,
    const int core,
    const int is_ingress0_egress1)
{
    unsigned int nof_entries_to_write = api_info[unit].applet_info.nof_pending_applets[core][is_ingress0_egress1];
    unsigned int entry_ndx;
    unsigned int applet_reg_val;
    phy_mem_t reg_info, mem_info;
    int sub_unit_idx = PEM_CFG_CORE_ID_TO_SUB_UNIT_ID(core);

    SHR_FUNC_INIT_VARS(unit);

    reg_info.is_reg = 1;
    mem_info.is_reg = 0;
    reg_info.reserve = is_ingress0_egress1;
    mem_info.reserve = is_ingress0_egress1;

    if (nof_entries_to_write == 0)
        SHR_EXIT();

    build_phy_mem_struct_for_applets_flush(unit, &reg_info, core, is_ingress0_egress1, 1);
    build_phy_mem_struct_for_applets_flush(unit, &mem_info, core, is_ingress0_egress1, 0);

    for (entry_ndx = 0; entry_ndx < nof_entries_to_write; ++entry_ndx)
    {

#ifdef BCM_DNX_SUPPORT
        SHR_IF_ERR_EXIT(pem_write
                        (unit, sub_unit_idx, &mem_info, mem_info.is_reg,
                         api_info[unit].applet_info.applet_mem_cache[core][is_ingress0_egress1][entry_ndx], 0));
#endif
        mem_info.mem_address++;
    }

    applet_reg_val = ((1 << 7) | nof_entries_to_write);

#ifdef BCM_DNX_SUPPORT
    SHR_IF_ERR_EXIT(pem_write(unit, sub_unit_idx, &reg_info, reg_info.is_reg, &applet_reg_val, 0));
#endif
    api_info[unit].applet_info.nof_pending_applets[core][is_ingress0_egress1] = 0;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flush_applets(
    const int unit)
{
    int applets_bit_state = api_info[unit].applet_info.currently_writing_applets_bit;
    int core;

    SHR_FUNC_INIT_VARS(unit);

    assert(1 == applets_bit_state);
    if (applets_bit_state == 0)
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "cannot flush applets as none written");

    for (core = 0; core < PEM_CFG_API_NOF_CORES_DYNAMIC; ++core)
    {
        SHR_IF_ERR_EXIT(flush_applets_to_single_mem(unit, core, 0));
        SHR_IF_ERR_EXIT(flush_applets_to_single_mem(unit, core, 1));
    }

    api_info[unit].applet_info.currently_writing_applets_bit = 0;

exit:
    SHR_FUNC_EXIT;
}

int
is_mem_belong_to_ingress0_or_egress1(
    phy_mem_t * mem,
    int min_ndx,
    int max_ndx)
{
    return 0;
}
