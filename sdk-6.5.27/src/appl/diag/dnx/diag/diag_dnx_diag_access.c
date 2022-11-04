
/** \file diag_dnx_diag_access.c
 *
 * diagnostic pack for fabric
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#include <appl/diag/system.h>
#include <appl/diag/shell.h>
#ifdef BCM_ACCESS_SUPPORT
#include <soc/access/access.h>
#include <soc/access/auto_generated/common_enum.h>
#endif /* BCM_ACCESS_SUPPORT */

#include <soc/drv.h>
#include <soc/property.h>

#define BSL_LOG_MODULE BSL_LS_SOC_ACCESS
#include <soc/sand/shrextend/shrextend_debug.h>

#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/bslcons.h>
#include <appl/diag/bslfile.h>
#include <sal/appl/pci.h>
#include <shared/bitop.h>

#include <appl/diag/sand/diag_sand_access.h>

#include <sal/appl/sal.h>
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_access.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/dnxc/diag_dnxc_diag.h>
#include <appl/diag/dnxc/diag_dnxc_diag_access.h>
#include <appl/diag/dnx/diag_dnx_diag.h>
#ifdef BCM_ACCESS_SUPPORT
#include <bcm_int/dnx/gtimer/gtimer_access.h>
#endif
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnxc/dnxc_time.h>
#include <soc/counter.h>
#include <shared/utilex/utilex_bitstream.h>

/*
 * Includes related to DATA
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ecgm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_graphical.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stat.h>

/*
 * }
 */

/*
 * Local includes
 * {
 */

/*
 * }
 */

#ifdef BCM_ACCESS_SUPPORT
/*************
 *  DEFINES  *
 *************/
typedef struct access_dnx_counter_register_s
{
    char *reg_name;
    access_regmem_id_t reg_arr[DNX_DIAG_COUNTER_REG_NOF_REGS];
    unsigned reg_index;         /* register index, should be 0 when not a register array. */
    access_field_id_t cnt_field[DNX_DIAG_COUNTER_REG_NOF_REGS];
    access_field_id_t overflow_field[DNX_DIAG_COUNTER_REG_NOF_REGS];
    int core_field_map[DNX_DIAG_COUNTER_REG_NOF_REGS];  /* The core ID which the register's field map to */
    uint32 fld_num;
    uint32 is_err;
} access_dnx_counter_register_t;

typedef struct access_dnx_counter_graphical_block_s
{
    access_dnx_counter_register_t *counter_table;       /* pointer to one counter table of one block */
    char block_name[DNX_DIAG_BLOCK_NAME_LENGTH];
    int num_entries;            /* Indicate how many counters are there in this counter table */
    uint32 is_err;
} access_dnx_counter_graphical_block_t;

#define ACCESS_DNX_GRAPHIC_COUNTER_CACHE_MAX_NUM    1024

typedef struct access_dnx_graphic_counter_reg_cache_s
{
    int cache_num;
    access_regmem_id_t reg[ACCESS_DNX_GRAPHIC_COUNTER_CACHE_MAX_NUM];
    int port[ACCESS_DNX_GRAPHIC_COUNTER_CACHE_MAX_NUM];
    int index[ACCESS_DNX_GRAPHIC_COUNTER_CACHE_MAX_NUM];
    access_reg_above_64_val_t reg_value[ACCESS_DNX_GRAPHIC_COUNTER_CACHE_MAX_NUM];
} access_dnx_graphic_counter_reg_cache_t;

extern shr_error_e sh_dnx_counter_convert_to_dnx_time_units(
    diag_dnxc_time_unit_e time_units,
    dnxc_time_unit_e * dnx_time_units);

extern shr_error_e sh_dnx_packet_dropped_reason_print(
    int unit,
    int cdsp,
    sh_sand_control_t * sand_control);

/**
 * \brief
 *   To check how many blocks are loaded in the counter_block_group
 * \param [in] unit - Unit #
 * \param [in] blk_group - to specify which block group to check
 * \param [out] nof_valid_block - a pointer to return number of valid blocks
 * \return
 *      Standard error handling
 */
static shr_error_e
access_counter_graphical_valid_block_get(
    int unit,
    access_dnx_counter_graphical_block_t * blk_group,
    int *nof_valid_block)
{
    int i;

    for (i = 0; i < DNX_DATA_MAX_GRAPHICAL_DIAG_NUM_OF_INDEX; i++)
    {
        /*
         * block is valid if it has at least one counter register
         */
        if (0 == blk_group[i].num_entries)
        {
            break;
        }
    }
    *nof_valid_block = i;

    return _SHR_E_NONE;
}

/**
 * \brief
 *   To calculate how many rows to print out in one counter graphical section
 * \param [in] unit - Unit #
 * \param [in] blk_group_left - a pointer to specify the left block group data
 * \param [in] blk_group_right - a pointer to specify the right block group data
 * \param [in] nof_block_left - to specify number of blocks in the left block group
 * \param [in] nof_block_right - to specify number of blocks in the right block group
 * \param [out] row_of_section - a pointer to return row of this block section, which is
 *              MAX(row of left, row of right), row of left/right is number of all the counters
 *              in all the blocks + number of parting lines which is number of blocks - 1.
 * \return
 *      Standard error handling
 */
static shr_error_e
access_counter_graphical_section_row_get(
    int unit,
    access_dnx_counter_graphical_block_t * blk_group_left,
    access_dnx_counter_graphical_block_t * blk_group_right,
    int nof_block_left,
    int nof_block_right,
    int *row_of_section)
{
    int i;
    int row_of_group_left = 0, row_of_group_right = 0;

    if (nof_block_left > DNX_DATA_MAX_GRAPHICAL_DIAG_NUM_OF_INDEX
        || nof_block_right > DNX_DATA_MAX_GRAPHICAL_DIAG_NUM_OF_INDEX)
    {
        LOG_CLI(("Invalid parameter: nof_block_left is %d, nof_block_right is %d \n", nof_block_left, nof_block_right));
        return _SHR_E_PARAM;
    }

    /*
     * calculate the row of the left group
     */
    for (i = 0; i < nof_block_left; i++)
    {
        if (0 != i)
        {
            /*
             * add one row for the parting line
             */
            row_of_group_left++;
        }
        row_of_group_left += blk_group_left[i].num_entries;
    }
    /*
     * calculate the row of the right group
     */
    for (i = 0; i < nof_block_right; i++)
    {
        if (0 != i)
        {
            /*
             * add one row for the parting line
             */
            row_of_group_right++;
        }
        row_of_group_right += blk_group_right[i].num_entries;
    }
    /*
     * the row of section is the max between row_of_group_left and row_of_group_right
     */
    *row_of_section = (row_of_group_left > row_of_group_right) ? row_of_group_left : row_of_group_right;

    return _SHR_E_NONE;
}

/**
 * \brief
 *   To calculate how many relevant registers are in the reg entry via very simple logic
 * \param [in] unit - Unit #
 * \param [in] relevant_block_reg_entry - the relevant counter registry structure
 * \param [out] nof_regs - number of relevant registers for the entry.
 * \return
 *      Standard error handling
 */
static shr_error_e
access_counter_graphical_nof_regs_in_entry_get(
    int unit,
    access_dnx_counter_register_t * relevant_block_reg_entry,
    int *nof_regs)
{

    /*
     * By default 1
     * Change to MAX only if flag is given
     */
    *nof_regs = relevant_block_reg_entry->fld_num;

    return _SHR_E_NONE;
}
/**
 * \brief
 *   Returns the soc_info id od all the relevant reg blocks for the provided registers in the counter reg entry
 * \param [in] unit - Unit #
 * \param [in] graphical_block_group - pointer to the relvant graphic block structure
 * \param [in] nof_graphical_blocks - number of blocks
 * \param [out] relevant_reg_block_ids - array of relevant block ids
 * \param [out] nof_valid_blocks_total - size of relevant_reg_block_ids.
 * \note
 * The function assumes that the registers in all entries are from the same blocks
 *  so the reg blocks are retrieved only from the regs in the first entry!
 *  The index is accumulated not zeroed!
 *
 * \return
 *      Standard error handling
 */
static shr_error_e
access_counter_graphical_relevant_reg_blocks_get(
    int unit,
    access_dnx_counter_graphical_block_t * graphical_block_group,
    int nof_graphical_blocks,
    int *relevant_reg_block_ids,
    int *nof_valid_blocks_total)
{
    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit);
    const access_device_type_info_t *device_info = runtime_info->device_type_info;
    const access_device_regmem_t *rm_info;
    access_local_regmem_id_t regmem_i;
    int graphical_block_idx, reg_index, nof_regs = 1;
    access_dnx_counter_register_t relevant_block_reg_entry;
    int i, find;
    SHR_FUNC_INIT_VARS(unit);

    for (graphical_block_idx = 0; graphical_block_idx < nof_graphical_blocks; graphical_block_idx++)
    {
        /**
         * So far all entries in the block registry descriptors are symetric if comes to flags and reg_blocks
         * This is time optimization!. A fool iteration over all entires can be done as well
         */
        relevant_block_reg_entry = graphical_block_group[graphical_block_idx].counter_table[0];

        SHR_IF_ERR_EXIT(access_counter_graphical_nof_regs_in_entry_get(unit, &relevant_block_reg_entry, &nof_regs));

        for (reg_index = 0; reg_index < nof_regs; reg_index++)
        {
            /**
             * Skip irrelevant for the device registers
             */
            if (!access_regmem_is_valid(unit, relevant_block_reg_entry.reg_arr[reg_index]))
            {
                continue;
            }

            regmem_i = access_return_local_regmem_id(runtime_info, relevant_block_reg_entry.reg_arr[reg_index]);
            rm_info = device_info->local_regs + regmem_i;
            /**
             * Find if the block id exsited in the array 
             */
            find = FALSE;
            for (i = 0; i < *nof_valid_blocks_total; i++)
            {
                if (relevant_reg_block_ids[i] == rm_info->local_block_id)
                {
                    find = TRUE;
                    break;
                }
            }
            if (!find)
            {
                relevant_reg_block_ids[*nof_valid_blocks_total] = rm_info->local_block_id;;
                *nof_valid_blocks_total += 1;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   To get the counter value and overflow value for graphical.
 * \param [in] unit - Unit #
 * \param [in] interval - indicate the interval of g-timer (second)
 * \param [in] reg - The register id of counter
 * \param [in] reg_value - The value of register
 * \param [in] cnt_field - The field of counter
 * \param [in] ovf_field - The field of overflow
 * \param [in] cnt_field_len - The field length of counter.
 * \param [out] fld_value - The counter value
 * \param [out] ovf_value - The overflow field value
 * \return
 *      Standard error handling
 */
shr_error_e
access_dnx_get_graphica_cnt_and_ovf_value(
    int unit,
    uint32 interval,
    access_regmem_id_t reg,
    access_reg_above_64_val_t reg_value,
    access_field_id_t cnt_field,
    access_field_id_t ovf_field,
    uint32 cnt_field_len,
    access_reg_above_64_val_t fld_value,
    uint32 *ovf_value)
{

    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit);
    const access_device_type_info_t *device_info = runtime_info->device_type_info;
    const access_device_regmem_t *rm_info;
    const access_device_block_t *local_block = NULL;
    access_local_regmem_id_t local_reg;
    int is_gtimer_en;

    SHR_FUNC_INIT_VARS(unit);

    is_gtimer_en = FALSE;
    if (interval)
    {
        /*
         * get the local device info for calling the local device function 
         */
        SHR_IF_ERR_EXIT(access_regmem_global2local(runtime_info, reg, &local_reg));
        rm_info = device_info->local_regs + local_reg;
        local_block = device_info->blocks + rm_info->local_block_id;
        SHR_IF_ERR_EXIT(dnx_gtimer_access_block_type_is_supported(unit, local_block->block_type, &is_gtimer_en));
    }

    sal_memset(fld_value, 0, sizeof(access_reg_above_64_val_t));
    SHR_IF_ERR_EXIT(access_field_get(unit, cnt_field, reg_value, fld_value));

    if (ovf_field != ACCESS_INVALID_FIELD_ID)
    {
        SHR_IF_ERR_EXIT(access_field32_get(unit, ovf_field, reg_value, ovf_value));
    }
    else
    {
        /*
         * There are no explict ovf field for this counter, suppose msb is ovf bit
         */
        *ovf_value = utilex_bitstream_test_bit(fld_value, cnt_field_len - 1);
        if (is_gtimer_en)
        {
            SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit(fld_value, cnt_field_len - 1));
        }
    }

    if (is_gtimer_en)
    {
        /*
         * gtimer is enable, ignore overflow status
         */
        *ovf_value = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
access_dnx_counter_graphic_get_reg_value(
    int unit,
    access_regmem_id_t reg,
    int port,
    int index,
    access_dnx_graphic_counter_reg_cache_t * reg_cache,
    access_reg_above_64_val_t reg_value)
{
    int ii;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(reg_cache, _SHR_E_PARAM, "reg_cache");

    sal_memset(reg_value, 0, sizeof(access_reg_above_64_val_t));
    for (ii = 0; ii < reg_cache->cache_num; ii++)
    {
        if ((reg == reg_cache->reg[ii]) && (port == reg_cache->port[ii]) && (index == reg_cache->index[ii]))
        {
            break;
        }
    }
    if (ii < reg_cache->cache_num)
    {
        /*
         * get counter register value from cache
         */
        SHR_BITCOPY_RANGE(reg_value, 0, reg_cache->reg_value[ii], 0, DIAG_ACCESS_MAX_REG_UINT32S * 32);
    }
    else
    {
        /*
         * read counter register and update cache
         */
        SHR_IF_ERR_EXIT(access_regmem(unit, 0, reg, port, index, 0, reg_value));
        if (reg_cache->cache_num < ACCESS_DNX_GRAPHIC_COUNTER_CACHE_MAX_NUM)
        {
            SHR_BITCOPY_RANGE(reg_cache->reg_value[reg_cache->cache_num], 0, reg_value, 0,
                              DIAG_ACCESS_MAX_REG_UINT32S * 32);
            reg_cache->reg[reg_cache->cache_num] = reg;
            reg_cache->port[reg_cache->cache_num] = port;
            reg_cache->index[reg_cache->cache_num] = index;

            reg_cache->cache_num++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   to get per core counter value.
 * \param [in] unit - Unit #
 * \param [in] counter_table - counter table to read
 * \param [in] reg_idx_cou_tbl - register index in counter table
 * \param [in] blk_instance_id - block instance ID
 * \param [in] reg_arr_idx - array index of the counter register
 * \return
 *      return core ID which associate with the HW reg's block
 * \note
 */
static bcm_core_t
access_dnx_counter_graphic_counter_reg_core_id_get(
    int unit,
    access_dnx_counter_register_t * counter_table,
    int reg_idx_cou_tbl,
    int blk_instance_id,
    int reg_arr_idx)
{
    int nof_blk_instances, reg_numels, total_instances;
    int nof_cores, inst_per_core, nof_cores_per_blk;
    bcm_core_t core_id = 0;
    uint32 first_array_idx;
    access_regmem_id_t reg;
    int rv;
    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit);
    const access_device_type_info_t *device_info = runtime_info->device_type_info;      /* constant access information
                                                                                         * for the device type */
    access_local_block_id_t local_block_id;
    access_local_regmem_id_t local_reg;
    const access_device_regmem_t *rm_info;

    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    reg = counter_table->reg_arr[reg_idx_cou_tbl];
    /*
     * get the local device info for calling the local device function 
     */
    rv = access_regmem_global2local(runtime_info, reg, &local_reg);
    if (SHR_FAILURE(rv))
    {
        /*
         * The log is tracked by test case AT_Dnx_port_diag_cou_g_cmds_error_monitor, may need to
         * update test case if update the log.
         */
        LOG_CLI(("sh_dnx_counter_graphic_counter_reg_core_id_get: global2local failed regmem = %d\n", reg));
        return 0;
    }
    rm_info = device_info->local_regs + local_reg;
    local_block_id = rm_info->local_block_id;

    nof_blk_instances = device_info->blocks[local_block_id].nof_instances;

    /*
     * core_field_map store the mapping between register field and core ID
     */
    if (counter_table->core_field_map[reg_idx_cou_tbl] != INVALID_CORE)
    {
        nof_cores_per_blk = nof_cores / nof_blk_instances;
        core_id = nof_cores_per_blk * blk_instance_id + counter_table->core_field_map[reg_idx_cou_tbl];
    }
    else
    {
        if (device_info->blocks[local_block_id].block_type == BLOCK_NBU ||
            device_info->blocks[local_block_id].block_type == BLOCK_NBUTX)
        {
            core_id = dnx_data_nif.eth.ethu_per_core_get(unit, blk_instance_id)->core_id;
        }
        else

        {
            reg_numels = rm_info->nof_array_indices;
            first_array_idx = rm_info->first_array_index;
            /*
             * counter_table.reg_index stores the field "arr_i" from graphical xml file, need
             * init the variable to get core ID correctly if "arr_i" is set.
             */
            if (counter_table->reg_index > 0)
            {
                reg_arr_idx = 0;
                first_array_idx = 0;
                reg_numels = 1;
            }

            total_instances = nof_blk_instances * reg_numels;
            inst_per_core = total_instances / nof_cores;

            if (first_array_idx)
            {
                reg_arr_idx = reg_arr_idx - first_array_idx;
            }

            if (total_instances > nof_cores)
            {
                core_id = (blk_instance_id * reg_numels + reg_arr_idx) / inst_per_core;
            }
            else
            {
                core_id = (blk_instance_id * reg_numels + reg_arr_idx) % nof_cores;
            }
        }
    }

    if (core_id >= nof_cores || core_id < 0)
    {
        core_id = 0;
        /*
         * The log is tracked by test case AT_Dnx_port_diag_cou_g_cmds_error_monitor, may need to
         * update test case if update the log.
         */
        LOG_CLI(("sh_dnx_counter_graphic_counter_reg_core_id_get: get an invalid core ID %d\n", core_id));
    }

    return core_id;
}

/**
 * \brief
 *   to get per core counter value.
 * \param [in] unit - Unit #
 * \param [in] counter_table - counter table to read
 * \param [in] reg_idx_cou_tbl - register index in counter table
 * \param [in] interval - interval of display graphical option
 * \param [in] reg_port - register port which associate the counter register
 * \param [in] reg_arr_idx - array index of the counter register
 * \param [in] reg_cache - graphic counter register value cache
 * \param [out] fld_value_u64 - register field value per core
 * \param [out] overflow - counter register overflow indication
 * \return
 *      Standard error handling
 * \note
 */
static shr_error_e
access_dnx_counter_graphic_per_core_counter_regs_read(
    int unit,
    access_dnx_counter_register_t * counter_table,
    int reg_idx_cou_tbl,
    uint32 interval,
    int reg_port,
    int reg_arr_idx,
    access_dnx_graphic_counter_reg_cache_t * reg_cache,
    uint64 *fld_value_u64,
    uint32 *overflow)
{
    access_field_id_t reg_field, reg_overflow_field;
    access_reg_above_64_val_t temp_fld_value;
    uint32 temp_overflow = 0, field_len;
    access_reg_above_64_val_t reg_value;
    uint64 temp_fld_value_u64;
    bcm_core_t core_id;
    access_regmem_id_t reg;

    SHR_FUNC_INIT_VARS(unit);

    reg = counter_table->reg_arr[reg_idx_cou_tbl];
    reg_field = counter_table->cnt_field[reg_idx_cou_tbl];
    reg_overflow_field = counter_table->overflow_field[reg_idx_cou_tbl];
    field_len = access_get_field_size(unit, reg_field);

    SHR_IF_ERR_EXIT(access_dnx_counter_graphic_get_reg_value(unit, reg, reg_port, reg_arr_idx, reg_cache, reg_value));

    sal_memset(temp_fld_value, 0, sizeof(access_reg_above_64_val_t));
    SHR_IF_ERR_EXIT(access_field_get(unit, reg_field, reg_value, temp_fld_value));

    /** some counter may are 128 bits. need to verify that it didn't exceed 64 bits value */
    SHR_IF_ERR_EXIT(access_dnx_get_graphica_cnt_and_ovf_value
                    (unit, interval, reg, reg_value, reg_field,
                     reg_overflow_field, field_len, temp_fld_value, &temp_overflow));

    COMPILER_64_ZERO(temp_fld_value_u64);
    COMPILER_64_SET(temp_fld_value_u64, temp_fld_value[1], temp_fld_value[0]);

    core_id =
        access_dnx_counter_graphic_counter_reg_core_id_get(unit, counter_table, reg_idx_cou_tbl, reg_port, reg_arr_idx);

    COMPILER_64_ADD_64(fld_value_u64[core_id], temp_fld_value_u64);

    if (temp_overflow)
    {
        overflow[core_id] = temp_overflow;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   to get per core counter value.
 * \param [in] unit - Unit #
 * \param [in] nof_block - number of blocks in the counter block group
 * \param [in] blk_group - a pointer to an array which contains counter block group information
 * \param [in] options - options set (cdsp, non_zero, in_hex, and interval) for graphical printing
 * \param [in] cnt_idx - counter index within one block
 * \param [in] blk_idx - block index within block group
 * \param [in] reg_cache - graphic counter register value cache
 * \param [out] fld_value - counter value per core
 * \param [out] overflow - overflow indication per core
 * \param [out] field_len - the length of counter register's field
 * \param [out] row_valid - the length of counter register's field
 * \return
 *      Standard error handling
 * \note
 */
static shr_error_e
access_dnx_counter_graphic_per_core_counters_get(
    int unit,
    int nof_block,
    access_dnx_counter_graphical_block_t * blk_group,
    sh_dnx_counter_graphical_options_t options,
    int *cnt_idx,
    int *blk_idx,
    access_dnx_graphic_counter_reg_cache_t * reg_cache,
    access_reg_above_64_val_t * fld_value,
    uint32 *overflow,
    int *field_len,
    int *row_valid)
{
    uint64 fld_value_u64[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    int instance_id, nof_instances, reg_numels;
    access_dnx_counter_register_t counter_table;
    int reg_idx_cou_tbl, nof_regs = 1;
    int reg_arr_idx, core_idx;
    uint32 first_array_idx;
    access_field_id_t cnt_field;
    access_regmem_id_t reg;
    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit);
    const access_device_type_info_t *device_info = runtime_info->device_type_info;      /* constant access information
                                                                                         * for the device type */
    access_local_block_id_t local_block_id;
    access_local_regmem_id_t local_reg;
    const access_device_regmem_t *rm_info;
    SHR_FUNC_INIT_VARS(unit);

    if (*blk_idx >= nof_block)
    {
        SHR_EXIT();
    }

    counter_table = blk_group[*blk_idx].counter_table[*cnt_idx];
    SHR_IF_ERR_EXIT(access_counter_graphical_nof_regs_in_entry_get(unit, &counter_table, &nof_regs));

    for (core_idx = 0; core_idx < DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES; core_idx++)
    {
        overflow[core_idx] = FALSE;
        sal_memset(fld_value[core_idx], 0, sizeof(access_reg_above_64_val_t));
        COMPILER_64_ZERO(fld_value_u64[core_idx]);
    }

    for (reg_idx_cou_tbl = 0; reg_idx_cou_tbl < nof_regs; reg_idx_cou_tbl++)
    {
        reg = counter_table.reg_arr[reg_idx_cou_tbl];
        cnt_field = counter_table.cnt_field[reg_idx_cou_tbl];

        if (ACCESS_INVALID_REGMEM_ID == reg || ACCESS_INVALID_FIELD_ID == cnt_field)
        {
            /*
             * The log is tracked by test case AT_Dnx_port_diag_cou_g_cmds_error_monitor, may need to
             * update test case if update the log.
             */
            LOG_CLI(("ERROR! invalid register in block entry %s\n", counter_table.reg_name));
            continue;
        }
        if (!access_regmem_is_valid(unit, reg))

        {
            /*
             * The log is tracked by test case AT_Dnx_port_diag_cou_g_cmds_error_monitor, may need to
             * update test case if update the log.
             */
            LOG_CLI(("ERROR! invalid register or field in block entry %s reg %d field %d\n",
                     counter_table.reg_name, reg, cnt_field));
            continue;
        }
        /*
         * get the local device info for calling the local device function 
         */
        SHR_IF_ERR_EXIT(access_regmem_global2local(runtime_info, reg, &local_reg));
        rm_info = device_info->local_regs + local_reg;
        local_block_id = rm_info->local_block_id;

        *row_valid = 1;
        *field_len = access_get_field_size(unit, cnt_field);
        nof_instances = device_info->blocks[local_block_id].nof_instances;
        reg_numels = rm_info->nof_array_indices;
        first_array_idx = rm_info->first_array_index;
        /*
         * counter_table.reg_index stores the field "arr_i" from graphical xml file, need use
         * reg_idnex from "arr_i" to read HW register.
         */
        if (counter_table.reg_index > 0)
        {
            reg_numels = 1;
            first_array_idx = counter_table.reg_index;
        }

        for (instance_id = 0; instance_id < nof_instances; instance_id++)
        {
            for (reg_arr_idx = first_array_idx; reg_arr_idx < reg_numels + first_array_idx; reg_arr_idx++)
            {
                SHR_IF_ERR_EXIT(access_dnx_counter_graphic_per_core_counter_regs_read
                                (unit, &counter_table, reg_idx_cou_tbl, options.interval, instance_id,
                                 reg_arr_idx, reg_cache, fld_value_u64, overflow));
            }
        }
    }

    /** set the fld value */
    for (core_idx = 0; core_idx < DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES; core_idx++)
    {
        fld_value[core_idx][0] = COMPILER_64_LO(fld_value_u64[core_idx]);
        fld_value[core_idx][1] = COMPILER_64_HI(fld_value_u64[core_idx]);
    }

    if (*field_len > 64)
    {
        *field_len = 64;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   to check whether all cores counters value are 0.
 * \param [in] unit - Unit #
 * \param [in] fld_value - counter value per core
 * \return
 *      Return 1 if all counters are 0 in all cores, otherwise, return 0
 * \note
 */
static int
access_dnx_counter_graphic_all_cores_counters_zero(
    int unit,
    access_reg_above_64_val_t * fld_value)
{
    int nof_cores, core_id;

    nof_cores = dnx_data_device.general.nof_cores_get(unit);

    for (core_id = 0; core_id < nof_cores; core_id++)
    {
        if (!SHR_BITNULL_RANGE(fld_value[core_id], 0, DIAG_ACCESS_MAX_REG_UINT32S * 32))
        {
            return FALSE;
        }
    }

    return TRUE;
}

/**
 * \brief
 *   to check whether one of core counter value is 0.
 * \param [in] unit - Unit #
 * \param [in] fld_value - counter value per core
 * \return
 *      Return 1 if one of core counter are 0, otherwise, return 0
 * \note
 */
static int
access_dnx_counter_graphic_one_core_counter_zero(
    int unit,
    access_reg_above_64_val_t * fld_value)
{
    int nof_cores, core_id;

    nof_cores = dnx_data_device.general.nof_cores_get(unit);

    for (core_id = 0; core_id < nof_cores; core_id++)
    {
        if (SHR_BITNULL_RANGE(fld_value[core_id], 0, DIAG_ACCESS_MAX_REG_UINT32S * 32))
        {
            return TRUE;
        }
    }

    return FALSE;
}

/**
 * \brief
 *   to check whether one of core counter value is overflow.
 * \param [in] unit - Unit #
 * \param [in] overflow - counter overflow status per core
 * \return
 *      Return 1 if one of core counter are 0, otherwise, return 0
 * \note
 */
static int
access_dnx_counter_graphic_one_core_counter_overflow(
    int unit,
    uint32 *overflow)
{
    int nof_cores, core_id;

    nof_cores = dnx_data_device.general.nof_cores_get(unit);

    for (core_id = 0; core_id < nof_cores; core_id++)
    {
        if (overflow[core_id])
        {
            return TRUE;
        }
    }

    return FALSE;
}
/**
 * \brief
 *   to print per core counter value.
 * \param [in] unit - Unit #
 * \param [in] nof_block - number of blocks in the counter block group
 * \param [in] blk_group - a pointer to an array which contains counter block group information
 * \param [in] options - options set (cdsp, non_zero, in_hex, and interval) for graphical printing
 * \param [in] prt_ctr - print control pointer
 * \param [in] cnt_idx - counter index within one block
 * \param [in] blk_idx - block index within block group
 * \param [in] is_parting_line - parting line flag
 * \param [in] fld_value - counter value per core
 * \param [in] overflow - overflow indication per core
 * \param [in] field_len - the length of counter register's field
 * \param [in] row_valid - the length of counter register's field
 * \return
 *      Standard error handling
 * \note
 */
static shr_error_e
access_dnx_counter_graphic_per_core_counters_print(
    int unit,
    int nof_block,
    access_dnx_counter_graphical_block_t * blk_group,
    sh_dnx_counter_graphical_options_t options,
    prt_control_t * prt_ctr,
    int *cnt_idx,
    int *blk_idx,
    int *is_parting_line,
    access_reg_above_64_val_t * fld_value,
    uint32 *overflow,
    int field_len,
    int row_valid)
{
    char value_str[PRT_COLUMN_WIDTH_BIG];
    int overflow_sum, overflow_final;
    int nof_cores = 0, core_idx;

    SHR_FUNC_INIT_VARS(unit);

    nof_cores = dnx_data_device.general.nof_cores_get(unit);

    if (*blk_idx >= nof_block)
    {
        /*
         * skip all the cells when exhausting all the blocks
         */
        PRT_CELL_SKIP((options.cdsp ? nof_cores + 2 : 3));
    }
    else if (TRUE == *is_parting_line)
    {
        /*
         * set one parting line between 2 blocks with one block group
         */
        PRT_CELL_SET("----");
        PRT_CELL_SET("----------------------------------");
        if (options.cdsp)
        {
            DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
            {
                PRT_CELL_SET("---------------");
            }
        }
        else
        {
            PRT_CELL_SET("---------------");
        }
    }
    else
    {
        /*
         * set block name only for the first counter within one block
         */
        if (0 == *cnt_idx)
        {
            PRT_CELL_SET("%s", blk_group[*blk_idx].block_name);
        }
        else
        {
            PRT_CELL_SKIP(1);
        }

        /*
         * print graphical counter on both cores separately
         */
        if (options.cdsp)
        {
            if (!row_valid)
            {
                PRT_CELL_SKIP(nof_cores + 1);
            }
            /*
             * option NonZero enabled and counter on one core at least is zero
             */
            else if (options.non_zero && (access_dnx_counter_graphic_one_core_counter_zero(unit, fld_value)))
            {
                /*
                 * counters on all cores are zero
                 */
                if (access_dnx_counter_graphic_all_cores_counters_zero(unit, fld_value))
                {
                    PRT_CELL_SKIP(nof_cores + 1);
                }
                else
                {
                    /*
                     * counter on one of core is zero
                     */
                    PRT_CELL_SET("%s", blk_group[*blk_idx].counter_table[*cnt_idx].reg_name);
                    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
                    {
                        if (SHR_BITNULL_RANGE(fld_value[core_idx], 0, DIAG_ACCESS_MAX_REG_UINT32S * 32))
                        {
                            PRT_CELL_SKIP(1);
                        }
                        else
                        {
                            format_value_to_str(fld_value[core_idx], field_len, options.in_hex, value_str);
                            if (overflow[core_idx] == TRUE)
                            {
                                PRT_CELL_SET("%s%s", value_str, "(ovf)");
                            }
                            else
                            {
                                PRT_CELL_SET("%s", value_str);
                            }
                        }
                    }
                }
            }
            /*
             * option NonZero disabled or none of counter is zero
             */
            else
            {
                /*
                 * set counter name
                 */
                PRT_CELL_SET("%s", blk_group[*blk_idx].counter_table[*cnt_idx].reg_name);

                DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
                {
                    format_value_to_str(fld_value[core_idx], field_len, options.in_hex, value_str);
                    if (overflow[core_idx] == TRUE)
                    {
                        PRT_CELL_SET("%s%s", value_str, "(ovf)");
                    }
                    else
                    {
                        PRT_CELL_SET("%s", value_str);
                    }
                }

            }
        }
        /*
         * print graphical counter with option cdsp disabled
         */
        else
        {
            if (!row_valid)
            {
                PRT_CELL_SKIP(2);
            }
            else if (options.non_zero && (access_dnx_counter_graphic_all_cores_counters_zero(unit, fld_value) ||
                                          ((options.core_id != BCM_CORE_ALL) &&
                                           SHR_BITNULL_RANGE(fld_value[options.core_id], 0,
                                                             DIAG_ACCESS_MAX_REG_UINT32S * 32))))
            {
                PRT_CELL_SKIP(2);
            }
            else
            {
                /*
                 * set counter name
                 */
                PRT_CELL_SET("%s", blk_group[*blk_idx].counter_table[*cnt_idx].reg_name);
                if (options.core_id != BCM_CORE_ALL)
                {
                    format_value_to_str(fld_value[options.core_id], field_len, options.in_hex, value_str);
                    overflow_final = overflow[options.core_id];
                }
                else
                {
                    sum_value_to_str(unit, fld_value, nof_cores, field_len, options.in_hex, value_str, &overflow_sum);
                    overflow_final = access_dnx_counter_graphic_one_core_counter_overflow(unit, overflow)
                        || overflow_sum;
                }
                if (overflow_final)
                {
                    PRT_CELL_SET("%s%s", value_str, "(ovf)");
                }
                else
                {
                    PRT_CELL_SET("%s", value_str);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   to update block index, counter index and flag to step to next counter block group.
 * \param [in] unit - Unit #
 * \param [in] nof_block - number of blocks in the counter block group
 * \param [in] blk_group - a pointer to an array which contains counter block group information
 * \param [in] cnt_idx - counter index within one block
 * \param [in] blk_idx - block index within block group
 * \param [in] is_parting_line - parting line flag
 * \return
 *      Standard error handling
 * \note
 */
static shr_error_e
access_dnx_counter_graphic_blk_cnt_idx_update(
    int unit,
    int nof_block,
    access_dnx_counter_graphical_block_t * blk_group,
    int *cnt_idx,
    int *blk_idx,
    int *is_parting_line)
{
    SHR_FUNC_INIT_VARS(unit);

    if (TRUE == *is_parting_line)
    {
        /*
         * unmarked parting_line flag
         */
        *is_parting_line = FALSE;
    }
    else
    {
        /*
         * increase counter index only when drawing non_parting_line row
         */
        (*cnt_idx)++;
    }
    /*
     * increase block index, reset counter index and set parting_line flag when one block is finished
     */
    if (*cnt_idx >= blk_group[*blk_idx].num_entries)
    {
        if (*blk_idx < nof_block)
        {
            (*blk_idx)++;
        }
        *cnt_idx = 0;
        *is_parting_line = TRUE;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   to print out half an row (half left or half right), given counter index and block index
 *   in one counter block group.
 * \param [in] unit - Unit #
 * \param [in] nof_block - number of blocks in the counter block group
 * \param [in] blk_group - a pointer to an array which contains counter block group information
 * \param [in] options - options set (cdsp, non_zero, in_hex, and interval) for graphical printing
 * \param [in] prt_ctr - print control pointer
 * \param [in] cnt_idx - counter index within one block
 * \param [in] blk_idx - block index within block group
 * \param [in] is_parting_line - parting line flag
 * \param [in] reg_cache - graphic counter register value cache
 * \return
 *      Standard error handling
 * \note
 */
static shr_error_e
access_dnx_counter_graphic_half_row_print(
    int unit,
    int nof_block,
    access_dnx_counter_graphical_block_t * blk_group,
    sh_dnx_counter_graphical_options_t options,
    prt_control_t * prt_ctr,
    int *cnt_idx,
    int *blk_idx,
    int *is_parting_line,
    access_dnx_graphic_counter_reg_cache_t * reg_cache)
{
    access_reg_above_64_val_t fld_value[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    uint32 overflow[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    int field_len = 0, row_valid = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get per core counters in the counter block group.
     */
    SHR_IF_ERR_EXIT(access_dnx_counter_graphic_per_core_counters_get
                    (unit, nof_block, blk_group, options, cnt_idx, blk_idx,
                     reg_cache, fld_value, overflow, &field_len, &row_valid));

    /*
     * Print per core counters in the counter block group according to print options.
     */
    SHR_IF_ERR_EXIT(access_dnx_counter_graphic_per_core_counters_print
                    (unit, nof_block, blk_group, options, prt_ctr, cnt_idx, blk_idx,
                     is_parting_line, fld_value, overflow, field_len, row_valid));

    /*
     * Update block index, counter index and flag to step to next counter block group.
     */
    SHR_IF_ERR_EXIT(access_dnx_counter_graphic_blk_cnt_idx_update
                    (unit, nof_block, blk_group, cnt_idx, blk_idx, is_parting_line));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Below is the sketch of counter graphic
 * --------------------------------------------------------------------------
 * | Block |    Counter      |  Value  | Block |    Counter       |  Value  |
 * --------------------------------------------------------------------------   --------
 * | blk_1 | blk_1_cnt_1     |  00000  | blk_5 | blk_5_cnt_1      | 00000   |
 * |       | blk_1_cnt_2     |  00000  |       | blk_5_cnt_2      | 00000   | <--section_1
 * |       |                 |         |       | blk_5_cnt_3      | 00000   |
 * --------------------------------------------------------------------------   --------
 * | blk_2 | blk_2_cnt_1     |  00000  | blk_6 | blk_6_cnt_1      | 00000   |
 * |       | blk_2_cnt_2     |  00000  |       | blk_6_cnt_2      | 00000   |
 * |-----------------------------------|       | blk_6_cnt_3      | 00000   | <--section_2
 * | blk_3 | blk_3_cnt_1     |  00000  |       | blk_6_cnt_4      | 00000   |
 * |       | blk_3_cnt_2     |  00000  |       |                  |         |
 * --------------------------------------------------------------------------   --------
 * | blk_4 | blk_4_cnt_1     |  00000  | blk_7 | blk_7_cnt_1      | 00000   |
 * |       | blk_4_cnt_2     |  00000  |       | blk_7_cnt_2      | 00000   |
 * |       | blk_4_cnt_3     |  00000  |       | blk_7_cnt_3      | 00000   |
 * |       | blk_4_cnt_4     |  00000  |------------------------------------| <--section_3
 * |       | blk_4_cnt_5     |  00000  | blk_8 | blk_8_cnt_1      | 00000   |
 * |       | blk_4_cnt_6     |  00000  |       |                  |         |
 * --------------------------------------------------------------------------   --------
 * |                                   |                                    |
 * | <------      Left      ------>    | <------      Right      ------>    |
 */
/**
 * \brief
 *   to print out one section of the couter graphic which contains x blocks
 *   in left side and y blocks in right side.
 *   the entire counter graphic consists of serval sections.
 * \param [in] unit - Unit #
 * \param [in] blk_grp_left - a pointer to an array which contains counter informantion
 *             on the base of block for left side of this section.
 * \param [in] blk_grp_right - a pointer to an array which contains counter informantion
 *             on the base of block for right side of this section.
 * \param [in] options - options set (cdsp, non_zero, in_hex, and interval) for graphical printing
 * \param [in] prt_ctr - print control pointer
 * \param [in] reg_cache - graphic counter register value cache
 * \return
 *      Standard error handling
 * \note
 *  Function assumes that the devices has no more than 2 cores
 */
static shr_error_e
access_dnx_counter_graphic_section_print(
    int unit,
    access_dnx_counter_graphical_block_t * blk_grp_left,
    access_dnx_counter_graphical_block_t * blk_grp_right,
    sh_dnx_counter_graphical_options_t options,
    prt_control_t * prt_ctr,
    access_dnx_graphic_counter_reg_cache_t * reg_cache)
{
    int i_total = 0;
    int iblk_left = 0, icnt_left = 0;
    int iblk_right = 0, icnt_right = 0;
    int is_parting_line_left = FALSE, is_parting_line_right = FALSE;
    int nof_blk_left, nof_blk_right, row_of_section;
    int idx = 0, nof_block_total = 0;
    int block_ids[ACCESS_NOF_GLOBAL_BLOCK_TYPES];
    uint32 nof_clocks;
    dnxc_time_t time_given_usec;
    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit);
    const access_device_type_info_t *device_info = runtime_info->device_type_info;
    access_block_id_t block_type;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * calculate number of blocks in the left and right block group
     */
    SHR_IF_ERR_EXIT(access_counter_graphical_valid_block_get(unit, blk_grp_left, &nof_blk_left));
    SHR_IF_ERR_EXIT(access_counter_graphical_valid_block_get(unit, blk_grp_right, &nof_blk_right));
    /*
     * calculate row of this counter graphical section
     */
    SHR_IF_ERR_EXIT(access_counter_graphical_section_row_get
                    (unit, blk_grp_left, blk_grp_right, nof_blk_left, nof_blk_right, &row_of_section));

    if (options.interval)
    {
        time_given_usec.time_units = options.time_units;
        COMPILER_64_SET(time_given_usec.time, 0, options.interval);
        SHR_IF_ERR_EXIT_WITH_LOG(dnxc_time_time_to_clock_cycles_get(unit, &time_given_usec, &nof_clocks),
                                 "Failed to get clock cycles %s%s%s\n", EMPTY, EMPTY, EMPTY);
        /*
         * Get all the block instances for the blocks in counter graphical
         */
        sal_memset(block_ids, 0, ACCESS_NOF_GLOBAL_BLOCK_TYPES * sizeof(int));
        SHR_IF_ERR_EXIT(access_counter_graphical_relevant_reg_blocks_get
                        (unit, blk_grp_left, nof_blk_left, block_ids, &nof_block_total));
        SHR_IF_ERR_EXIT(access_counter_graphical_relevant_reg_blocks_get
                        (unit, blk_grp_right, nof_blk_right, block_ids, &nof_block_total));

        /*
         * Config and triger the gtimer for the blocks
         */
        for (idx = 0; idx < nof_block_total; idx++)
        {
            block_type = device_info->blocks[block_ids[idx]].block_type;
            SHR_IF_ERR_EXIT_WITH_LOG(dnx_gtimer_access_set(unit, nof_clocks, block_type, -1),
                                     "Failed to config gtimer in block(%s%s%s)\n",
                                     access_global_block_types[block_type].block_name, EMPTY, EMPTY);
        }
        for (idx = 0; idx < nof_block_total; idx++)
        {
            block_type = device_info->blocks[block_ids[idx]].block_type;
            SHR_IF_ERR_EXIT_WITH_LOG(dnx_gtimer_access_wait(unit, block_type, -1),
                                     "Failed to wait gtimer in block(%s%s%s)\n",
                                     access_global_block_types[block_type].block_name, EMPTY, EMPTY);
        }
    }

    for (i_total = 0; i_total < row_of_section; i_total++)
    {
        if (row_of_section - 1 == i_total)
        {
            /*
             * the last row drawed with underscore
             */
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        }
        else
        {
            /*
             * the other rows drawed with non_line
             */
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        }
        /*
         * print the half left row
         */
        SHR_IF_ERR_EXIT(access_dnx_counter_graphic_half_row_print
                        (unit, nof_blk_left, blk_grp_left, options, prt_ctr, &icnt_left, &iblk_left,
                         &is_parting_line_left, reg_cache));
        /*
         * print the half right row
         */
        SHR_IF_ERR_EXIT(access_dnx_counter_graphic_half_row_print
                        (unit, nof_blk_right, blk_grp_right, options, prt_ctr, &icnt_right, &iblk_right,
                         &is_parting_line_right, reg_cache));

    }

    if (options.interval)
    {
        /*
         * Clear the gtimer for the blocks
         */
        for (idx = 0; idx < nof_block_total; idx++)
        {
            block_type = device_info->blocks[block_ids[idx]].block_type;
            SHR_IF_ERR_EXIT_WITH_LOG(dnx_gtimer_access_clear(unit, block_type, -1),
                                     "Failed to clear gtimer in block(%s%s%s)\n",
                                     access_global_block_types[block_type].block_name, EMPTY, EMPTY);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   to get counter register for graphical display
 * \param [in] unit - Unit #
 * \param [in] blk - display graphical block enum
 * \param [in] working_block - block structure
 * \return
 *      void
 * \note
 *  Function assumes that the devices has no more than 2 cores
 */
static void
access_dnx_counter_register_get(
    int unit,
    dnx_diag_graphical_block blk,
    access_dnx_counter_graphical_block_t * working_block)
{
    int entry;
    int arr_index;
    access_dnx_counter_register_t *table;

    for (entry = 0; entry < dnx_data_graphical.diag.num_of_block_entries_get(unit); entry++)
    {
        if (dnx_data_graphical.diag.counters_get(unit, blk, entry)->regs[0] == INVALIDr)
        {
            break;
        }
        table = &working_block->counter_table[entry];
        table->reg_name = dnx_data_graphical.diag.counters_get(unit, blk, entry)->name;
        table->reg_index = dnx_data_graphical.diag.counters_get(unit, blk, entry)->arr_i;
        table->fld_num = dnx_data_graphical.diag.counters_get(unit, blk, entry)->fld_num;
        table->is_err = dnx_data_graphical.diag.counters_get(unit, blk, entry)->is_err;
        for (arr_index = 0; arr_index < dnx_data_graphical.diag.counters_get(unit, blk, entry)->fld_num; arr_index++)
        {
            table->reg_arr[arr_index] = dnx_data_graphical.diag.counters_get(unit, blk, entry)->regs[arr_index];
            table->cnt_field[arr_index] = dnx_data_graphical.diag.counters_get(unit, blk, entry)->flds[arr_index];
            table->overflow_field[arr_index] =
                dnx_data_graphical.diag.counters_get(unit, blk, entry)->overflow_field[arr_index];
            table->core_field_map[arr_index] =
                dnx_data_graphical.diag.counters_get(unit, blk, entry)->core_fld_map[arr_index];
        }
        working_block->num_entries++;
    }
}

/**
 * \brief
 *   For DNX device only
 *   To start gtimer of the blocks specified by block index array
 * \param [in] unit - Unit #
 * \param [in] nof_block - number of blocks contained in block_ids array
 * \param [in] block_ids - an array to contain the blockes
 * \param [in] inst_ids - an array to contain the instance of block_ids
 * \param [in] interval - indicate how long gtimer keeps enabled
 * \param [in] time_units - indicate which time units are depicted by 'interval' parameter
 * \return
 *      Standard error handling
 */
shr_error_e
access_dnx_diag_block_gtimer_start(
    int unit,
    int nof_block,
    int *block_ids,
    int *inst_ids,
    int interval,
    diag_dnxc_time_unit_e time_units)
{
    int rv = _SHR_E_NONE;
    uint32 nof_clocks;
    int i, is_supported;
    dnxc_time_t time_given_usec;
    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit);
    const access_device_type_info_t *device_info = runtime_info->device_type_info;
    access_block_id_t block_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_counter_convert_to_dnx_time_units(time_units, &time_given_usec.time_units));

    COMPILER_64_SET(time_given_usec.time, 0, interval);
    rv = dnxc_time_time_to_clock_cycles_get(unit, &time_given_usec, &nof_clocks);
    SHR_IF_ERR_EXIT_WITH_LOG(rv, "Failed(%d) to get clock cycles %s%s\n", rv, EMPTY, EMPTY);
    /*
     * Config gtimer for all the blocks
     */
    for (i = 0; i < nof_block; i++)
    {
        block_type = device_info->blocks[block_ids[i]].block_type;
        /** verify if block supports gtimer */
        SHR_IF_ERR_EXIT(dnx_gtimer_access_block_type_is_supported(unit, block_type, &is_supported));
        if (is_supported == FALSE)
        {
            continue;
        }
        rv = dnx_gtimer_access_set(unit, nof_clocks, block_type, inst_ids[i]);
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "Failed(%d) config gtimer in block(%s%s)\n", rv,
                                 access_global_block_types[block_type].block_name, EMPTY);
    }

    /*
     * Wait gtimer trigger to indicate counting finished
     */
    for (i = 0; i < nof_block; i++)
    {
        block_type = device_info->blocks[block_ids[i]].block_type;
        /** verify if block supports gtimer */
        SHR_IF_ERR_EXIT(dnx_gtimer_access_block_type_is_supported(unit, block_type, &is_supported));
        if (is_supported == FALSE)
        {
            continue;
        }
        rv = dnx_gtimer_access_wait(unit, block_type, inst_ids[i]);

        SHR_IF_ERR_EXIT_WITH_LOG(rv, "Failed(%d) wait gtimer in block(%s%s)\n", rv,
                                 access_global_block_types[block_type].block_name, EMPTY);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   For DNX device only
 *   To stop gtimer of the blocks specified by block index array
 * \param [in] unit - Unit #
 * \param [in] nof_block - number of blocks contained in block_ids array
 * \param [in] block_ids - an array to contain the blockes
 * \param [in] inst_ids - an array to contain the instance of block_ids
 * \return
 *      Standard error handling
 */
shr_error_e
access_dnx_diag_block_gtimer_stop(
    int unit,
    int nof_block,
    int *block_ids,
    int *inst_ids)
{
    int i, is_supported;
    int rv = _SHR_E_NONE;
    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit);
    const access_device_type_info_t *device_info = runtime_info->device_type_info;
    access_block_id_t block_type;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Stop gtimer
     */
    for (i = 0; i < nof_block; i++)
    {
        block_type = device_info->blocks[block_ids[i]].block_type;
        /** verify if block supports gtimer */
        SHR_IF_ERR_EXIT(dnx_gtimer_access_block_type_is_supported(unit, block_type, &is_supported));
        if (is_supported == FALSE)
        {
            continue;
        }

        rv = dnx_gtimer_access_clear(unit, block_type, inst_ids[i]);
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "Failed(%d) clear gtimer in block(%s%s)\n", rv,
                                 access_global_block_types[block_type].block_name, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */

/**
 * \brief
 *   to display the counter graphic in which counters are placed
 *   in packet-flow-going sequence
 * \param [in] unit - Unit #
 * \param [in] cdsp - indicate if counters were displayed per core
 * \param [in] core_id - indicate if counters were displayed only specific core
 * \param [in] non_zero - indicate to display non_zero counters only
 * \param [in] in_hex - indicate to display counters in hex format
 * \param [in] interval - indicate the interval of g-timer
 * \param [in] time_units - indicate which time units are depicted by 'interval' parameter
 * \param [in] sand_control - pointer to framework control structure
 * \return
 *      Standard error handling
 * \note
 *  Function assumes that the devices has no more than 4 cores
 */
shr_error_e
access_dnx_diag_counter_graphical_print(
    int unit,
    int cdsp,
    int core_id,
    int non_zero,
    int in_hex,
    uint32 interval,
    diag_dnxc_time_unit_e time_units,
    sh_sand_control_t * sand_control)
{
    int core_idx, rv;
    sh_dnx_counter_graphical_options_t options;
    int row, index;
    access_dnx_counter_graphical_block_t group_left[DNX_DATA_MAX_GRAPHICAL_DIAG_NUM_OF_INDEX];
    access_dnx_counter_graphical_block_t group_right[DNX_DATA_MAX_GRAPHICAL_DIAG_NUM_OF_INDEX];
    int left_blks, right_blks;
    dnx_diag_graphical_block blk;
    int field_col_id, temp_col_id;
    access_dnx_graphic_counter_reg_cache_t *reg_cache;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    reg_cache = sal_alloc(sizeof(access_dnx_graphic_counter_reg_cache_t), "reg_cache");
    SHR_NULL_CHECK(reg_cache, _SHR_E_MEMORY, "reg_cache");
    sal_memset(reg_cache, 0, sizeof(access_dnx_graphic_counter_reg_cache_t));
    options.cdsp = cdsp;
    options.non_zero = non_zero;
    options.in_hex = in_hex;
    options.interval = interval;
    options.core_id = core_id;
    SHR_IF_ERR_EXIT(sh_dnx_counter_convert_to_dnx_time_units(time_units, &options.time_units));
    /*
     * Print the head of counter graphical
     */
    SHR_IF_ERR_EXIT(sh_dnx_diag_counter_graphical_head_tail_print(unit, cdsp, 1));
    /*
     * add column for counter graphical
     */
    PRT_TITLE_SET("");
    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &field_col_id, "   ");
    PRT_COLUMN_ADDX(PRT_XML_CHILD, field_col_id, &temp_col_id, "   Counter  ");
    if (cdsp)
    {
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
        {
            PRT_COLUMN_ADDX(PRT_XML_CHILD, field_col_id, &temp_col_id, " Value(core_%d) ", core_idx);
        }
    }
    else
    {
        if (options.core_id != BCM_CORE_ALL)
        {
            PRT_COLUMN_ADDX(PRT_XML_CHILD, field_col_id, &temp_col_id, " Value(core_%d) ", options.core_id);
        }
        else
        {
            PRT_COLUMN_ADDX(PRT_XML_CHILD, field_col_id, &temp_col_id, "   Value   ");
        }
    }
    PRT_COLUMN_ADDX(PRT_XML_CHILD, field_col_id, &temp_col_id, "   ");
    PRT_COLUMN_ADDX(PRT_XML_CHILD, field_col_id, &temp_col_id, "   Counter  ");
    if (cdsp)
    {
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
        {
            PRT_COLUMN_ADDX(PRT_XML_CHILD, field_col_id, &temp_col_id, " Value(core_%d) ", core_idx);
        }
    }
    else
    {
        if (options.core_id != BCM_CORE_ALL)
        {
            PRT_COLUMN_ADDX(PRT_XML_CHILD, field_col_id, &temp_col_id, " Value(core_%d) ", options.core_id);
        }
        else
        {
            PRT_COLUMN_ADDX(PRT_XML_CHILD, field_col_id, &temp_col_id, "   Value   ");
        }
    }

    for (row = 0; row < dnx_data_graphical.diag.num_of_rows_get(unit); row++)
    {
        sal_memset(group_left, 0,
                   sizeof(access_dnx_counter_graphical_block_t) * dnx_data_graphical.diag.num_of_index_get(unit));
        sal_memset(group_right, 0,
                   sizeof(access_dnx_counter_graphical_block_t) * dnx_data_graphical.diag.num_of_index_get(unit));
        /** get left */
        left_blks = 0;
        for (index = 0; index < dnx_data_graphical.diag.num_of_index_get(unit); index++)
        {
            if (dnx_data_graphical.diag.blocks_left_get(unit, row, index)->valid)
            {
                group_left[left_blks].counter_table = NULL;
                SHR_ALLOC_ERR_EXIT(group_left[left_blks].counter_table,
                                   (sizeof(access_dnx_counter_register_t) *
                                    dnx_data_graphical.diag.num_of_block_entries_get(unit)), "diag graphical left",
                                   "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
                sal_snprintf(group_left[left_blks].block_name, DNX_DIAG_BLOCK_NAME_LENGTH, "%s",
                             dnx_data_graphical.diag.blocks_left_get(unit, row, index)->name);
                blk = dnx_data_graphical.diag.blocks_left_get(unit, row, index)->block;
                access_dnx_counter_register_get(unit, blk, &group_left[left_blks]);
                left_blks++;
            }
        }
        /** get right */
        right_blks = 0;
        for (index = 0; index < dnx_data_graphical.diag.num_of_index_get(unit); index++)
        {
            if (dnx_data_graphical.diag.blocks_right_get(unit, row, index)->valid)
            {
                group_right[right_blks].counter_table = NULL;
                SHR_ALLOC_ERR_EXIT(group_right[right_blks].counter_table,
                                   (sizeof(access_dnx_counter_register_t) *
                                    dnx_data_graphical.diag.num_of_block_entries_get(unit)), "signal_key", "%s%s%s\r\n",
                                   EMPTY, EMPTY, EMPTY);
                sal_snprintf(group_right[right_blks].block_name, DNX_DIAG_BLOCK_NAME_LENGTH, "%s",
                             dnx_data_graphical.diag.blocks_right_get(unit, row, index)->name);
                blk = dnx_data_graphical.diag.blocks_right_get(unit, row, index)->block;
                access_dnx_counter_register_get(unit, blk, &group_right[right_blks]);
                right_blks++;
            }
        }
        /** print out counter graphical section 1 */
        rv = access_dnx_counter_graphic_section_print(unit, group_left, group_right, options, prt_ctr, reg_cache);
        /** release counter table memory */
        for (index = 0; index < left_blks; index++)
        {
            sal_free(group_left[index].counter_table);
        }
        for (index = 0; index < right_blks; index++)
        {
            sal_free(group_right[index].counter_table);
        }
        SHR_IF_ERR_EXIT(rv);
    }

    PRT_COMMITX;

    /*
     * print reasons for dropped packets in block CGM, RQP and PQP
     */
    SHR_IF_ERR_EXIT(sh_dnx_packet_dropped_reason_print(unit, cdsp, sand_control));
    /*
     * Print the tail of counter graphical
     */
    SHR_IF_ERR_EXIT(sh_dnx_diag_counter_graphical_head_tail_print(unit, cdsp, 0));

exit:
    if (reg_cache != NULL)
    {
        sal_free(reg_cache);
        reg_cache = NULL;
    }
    PRT_FREE;
    SHR_FUNC_EXIT;
}

#endif /* BCM_ACCESS_SUPPORT */

