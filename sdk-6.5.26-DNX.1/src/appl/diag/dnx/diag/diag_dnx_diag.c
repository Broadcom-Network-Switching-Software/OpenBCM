/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:    diag_dnx_diag.c
 * Purpose:    Routines for handling misc diagnostic
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAG

/*************
 * INCLUDES  *
 *************/
/** allow drv.h include excplictly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
/** allow soc/mcm/memregs.h include excplictly */
#ifdef _SOC_MEMREGS_H
#undef _SOC_MEMREGS_H
#endif
#include <soc/drv.h> /**SOC_REG_INFO*/
#include <soc/mcm/memregs.h> /** needed for SOC_REG_NAME */
#include <soc/field.h>
#include <soc/types.h>
#include <soc/dnx/drv.h>

#include <sal/appl/sal.h>
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_access.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/dnxc/diag_dnxc_diag.h>
#include <appl/diag/dnx/diag_dnx_diag.h>
#include <bcm_int/dnx/gtimer/gtimer.h>
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
#ifdef BCM_IPROC_SUPPORT
#include <soc/iproc.h>
#endif

/*
 * Local includes
 * {
 */

extern sh_sand_enum_t diag_dnxc_diag_time_units[];
/*
 * }
 */
/*************
 *  MACROES  *
 *************/
/** Maximum index of block */
#define DNX_DIAG_BLOCK_MAX_INDEX 28
/** a flag which indicates that register has replication register in CDUM */
#define DNX_DIAG_COUNTER_REG_FLAGS_MULTI_REGS (0x1)

#define COUNTER_GRAPHIC_BLOCK_SET(_block, _counter_table, _blk_name)              \
{                                                                                 \
    _block->counter_table = _counter_table;                                     \
    sal_snprintf(_block->block_name, DNX_DIAG_BLOCK_NAME_LENGTH, "%s", _blk_name);         \
    _block->num_entries = sizeof(_counter_table)/sizeof(sh_dnx_counter_register_t); \
}

#define DIAG_DNX_GTIMER_OPTIONS  \
     /** name type desc default ext */ \
    {"block", SAL_FIELD_TYPE_STR, "Block name, empty means all", "", NULL, NULL, SH_SAND_ARG_FREE}

/*************
 *  DEFINES  *
 *************/
typedef struct sh_dnx_counter_graphical_options_s
{
    int cdsp;
    int core_id;
    int non_zero;
    int in_hex;
    uint32 interval;
    dnxc_time_unit_e time_units;
} sh_dnx_counter_graphical_options_t;

typedef struct sh_dnx_counter_register_s
{
    char *reg_name;
    soc_reg_t reg_arr[DNX_DIAG_COUNTER_REG_NOF_REGS];
    unsigned reg_index;         /* register index, should be 0 when not a register array. */
    soc_field_t cnt_field[DNX_DIAG_COUNTER_REG_NOF_REGS];
    soc_field_t overflow_field[DNX_DIAG_COUNTER_REG_NOF_REGS];
    int core_field_map[DNX_DIAG_COUNTER_REG_NOF_REGS];  /* The core ID which the register's field map to */
    uint32 fld_num;
    uint32 is_err;
} sh_dnx_counter_register_t;

typedef struct sh_dnx_counter_graphical_block_s
{
    sh_dnx_counter_register_t *counter_table;   /* pointer to one counter table of one block */
    char block_name[DNX_DIAG_BLOCK_NAME_LENGTH];
    int num_entries;            /* Indicate how many counters are there in this counter table */
    uint32 is_err;
} sh_dnx_counter_graphical_block_t;

#define SH_DNX_GRAPHIC_COUNTER_CACHE_MAX_NUM    1024
typedef struct sh_dnx_graphic_counter_reg_cache_s
{
    int cache_num;
    soc_reg_t reg[SH_DNX_GRAPHIC_COUNTER_CACHE_MAX_NUM];
    int port[SH_DNX_GRAPHIC_COUNTER_CACHE_MAX_NUM];
    int index[SH_DNX_GRAPHIC_COUNTER_CACHE_MAX_NUM];
    soc_reg_above_64_val_t reg_value[SH_DNX_GRAPHIC_COUNTER_CACHE_MAX_NUM];
} sh_dnx_graphic_counter_reg_cache_t;

/*************
 * GLOBALS   *
 *************/

/*************
 * FUNCTIONS *
 *************/
static shr_error_e
sh_dnx_counter_convert_to_dnx_time_units(
    diag_dnxc_time_unit_e time_units,
    dnxc_time_unit_e * dnx_time_units)
{
    SHR_FUNC_INIT_VARS(NO_UNIT);

    switch (time_units)
    {
        case DIAG_DNXC_TIME_UNIT_SEC:
            *dnx_time_units = DNXC_TIME_UNIT_SEC;
            break;
        case DIAG_DNXC_TIME_UNIT_MILISEC:
            *dnx_time_units = DNXC_TIME_UNIT_MILISEC;
            break;
        case DIAG_DNXC_TIME_UNIT_USEC:
            *dnx_time_units = DNXC_TIME_UNIT_USEC;
            break;
        case DIAG_DNXC_TIME_UNIT_NSEC:
            *dnx_time_units = DNXC_TIME_UNIT_NSEC;
            break;
        default:
            *dnx_time_units = DNXC_TIME_UNIT_INVALID;
            break;
    }

    SHR_FUNC_EXIT;
}

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
counter_graphical_valid_block_get(
    int unit,
    sh_dnx_counter_graphical_block_t * blk_group,
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
counter_graphical_section_row_get(
    int unit,
    sh_dnx_counter_graphical_block_t * blk_group_left,
    sh_dnx_counter_graphical_block_t * blk_group_right,
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
counter_graphical_nof_regs_in_entry_get(
    int unit,
    sh_dnx_counter_register_t * relevant_block_reg_entry,
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
counter_graphical_relevant_reg_blocks_get(
    int unit,
    sh_dnx_counter_graphical_block_t * graphical_block_group,
    int nof_graphical_blocks,
    int *relevant_reg_block_ids,
    int *nof_valid_blocks_total)
{
    int graphical_block_idx, block, reg_index, nof_regs = 1;
    sh_dnx_counter_register_t relevant_block_reg_entry;
    SHR_FUNC_INIT_VARS(unit);

    for (graphical_block_idx = 0; graphical_block_idx < nof_graphical_blocks; graphical_block_idx++)
    {
        /**
         * So far all entries in the block registry descriptors are symetric if comes to flags and reg_blocks
         * This is time optimization!. A fool iteration over all entires can be done as well
         */
        relevant_block_reg_entry = graphical_block_group[graphical_block_idx].counter_table[0];

        SHR_IF_ERR_EXIT(counter_graphical_nof_regs_in_entry_get(unit, &relevant_block_reg_entry, &nof_regs));

        for (reg_index = 0; reg_index < nof_regs; reg_index++)
        {
            /**
             * Skip irrelevant for the device registers
             */
            if (!soc_reg_field_valid(unit, relevant_block_reg_entry.reg_arr[reg_index],
                                     relevant_block_reg_entry.cnt_field[reg_index]))
            {
                continue;
            }
            /**
             * SOC_INFO holds the array of all relevant block ids for each register.
             * Iteration is only to filter the disabled ones
             */
            SOC_BLOCKS_ITER(unit, block, SOC_REG_INFO(unit, relevant_block_reg_entry.reg_arr[reg_index]).block)
            {
                if (!SOC_INFO(unit).block_valid[block])
                {
                    continue;
                }

                relevant_reg_block_ids[*nof_valid_blocks_total] = block;
                *nof_valid_blocks_total += 1;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   to print out the reasons for dropped and rejected packers in block
 *   CGM, RQP and PQP.
 * \param [in] unit - Unit #
 * \param [in] cdsp - indicate if reasons were displayed per core
 * \param [in] sand_control - pointer to framework control structure
 * \return
 *      Standard error handling
 * \note
 *  Function assumes that the devices has no more than 2 cores
 */
static shr_error_e
sh_dnx_packet_dropped_reason_print(
    int unit,
    int cdsp,
    sh_sand_control_t * sand_control)
{
    int nof_cores = 0, core_idx = 0, nof_cores_to_iter;
    int i_reason = 0, i_core = 0;
    int nof_reason = 0;
    int i_blk, blks;
    dnx_counter_packet_dropped_reason_t *dropped_reasons = NULL;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    nof_cores_to_iter = cdsp ? nof_cores : 1;

    blks = dnx_data_graphical.diag.drop_reason_info_get(unit)->key_size[0];
    SHR_ALLOC_ERR_EXIT(dropped_reasons,
                       (blks * sizeof(dnx_counter_packet_dropped_reason_t) * nof_cores),
                       "dropped reasons", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    sal_memset(dropped_reasons, 0, blks * sizeof(dnx_counter_packet_dropped_reason_t) * nof_cores);
    for (i_blk = 0; i_blk < blks; i_blk++)
    {
        dnx_dropped_reason_get_f dropped_reason_cb = NULL;

        dropped_reason_cb =
            (dnx_dropped_reason_get_f) dnx_data_graphical.diag.drop_reason_get(unit, i_blk)->drop_reason_cb;
        if (dropped_reason_cb == NULL)
        {
            continue;
        }
        dropped_reason_cb(unit, dnx_data_graphical.diag.drop_reason_get(unit, i_blk)->regs,
                          dnx_data_graphical.diag.drop_reason_get(unit, i_blk)->flds, cdsp,
                          &dropped_reasons[i_blk * nof_cores]);
    }

    for (i_blk = 0; i_blk < blks; i_blk++)
    {
        for (i_core = 0; i_core < nof_cores; i_core++)
        {
            nof_reason += dropped_reasons[i_blk * nof_cores + i_core].nof_reason;
        }
    }
    /** EXIT when no reason to print in all the blocks (CGM, RQP and PQP) */
    if (nof_reason == 0)
    {
        SHR_EXIT();
    }

    /*
     * Draw table to print reasons for the dropped packets
     */
    PRT_TITLE_SET("Reason for dropped and rejected packets");
    PRT_COLUMN_ADD("Block ");
    if (cdsp && nof_cores > 1)
    {
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
        {
            PRT_COLUMN_ADD("Reason(core_%d)                                                                         ",
                           core_idx);
        }
    }
    else
    {
        PRT_COLUMN_ADD
            ("Reason                                                                                                                                                  ");
    }

    for (i_blk = 0; i_blk < blks; i_blk++)
    {
        nof_reason = 0;
        for (i_core = 0; i_core < nof_cores_to_iter; i_core++)
        {
            nof_reason =
                nof_reason >
                dropped_reasons[i_core + i_blk * nof_cores].nof_reason ? nof_reason : dropped_reasons[i_core +
                                                                                                      i_blk *
                                                                                                      nof_cores].nof_reason;
        }
        if (nof_reason <= 0)
        {
            continue;
        }
        for (i_reason = 0; i_reason < nof_reason; i_reason++)
        {
            /*
             * Add new row
             */
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            /*
             * Print block name for each block
             */
            if (i_reason == 0)
            {
                PRT_CELL_SET("%s", dropped_reasons[i_blk * nof_cores].block_name);
            }
            else
            {
                PRT_CELL_SKIP(1);
            }
            for (i_core = 0; i_core < nof_cores_to_iter; i_core++)
            {
                if (i_reason < dropped_reasons[i_core + i_blk * nof_cores].nof_reason)
                {
                    PRT_CELL_SET("%s", dropped_reasons[i_core + i_blk * nof_cores].reason_buffer[i_reason].buffer);
                }
                else
                {
                    PRT_CELL_SKIP(1);
                }
            }
        }
        /*
         * Print partion line between block RQP and PQP
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("------");
        if (cdsp && nof_cores > 1)
        {
            DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
            {
                PRT_CELL_SET("---------------------------------------------------------------------------------------");
            }
        }
        else
        {
            PRT_CELL_SET
                ("------------------------------------------------------------------------------------------"
                 "---------------------------------------------------------------------------");
        }
    }

    PRT_COMMITX;

exit:
    for (i_blk = 0; i_blk < blks; i_blk++)
    {
        for (i_core = 0; i_core < nof_cores; i_core++)
        {
            if (dropped_reasons[i_core + i_blk * nof_cores].reason_buffer != NULL)
            {
                sal_free(dropped_reasons[i_core + i_blk * nof_cores].reason_buffer);

            }
        }
    }

    sal_free(dropped_reasons);
    PRT_FREE;
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
sh_dnx_get_graphica_cnt_and_ovf_value(
    int unit,
    uint32 interval,
    soc_reg_t reg,
    soc_reg_above_64_val_t reg_value,
    soc_field_t cnt_field,
    soc_field_t ovf_field,
    uint32 cnt_field_len,
    soc_reg_above_64_val_t fld_value,
    uint32 *ovf_value)
{
    soc_reg_info_t reg_info;
    soc_block_type_t block_type;
    int is_gtimer_en;

    SHR_FUNC_INIT_VARS(unit);

    is_gtimer_en = FALSE;
    if (interval)
    {
        reg_info = SOC_REG_INFO(unit, reg);
        block_type = SOC_REG_FIRST_BLK_TYPE(reg_info.block);
        SHR_IF_ERR_EXIT(dnx_gtimer_block_type_is_supported(unit, block_type, &is_gtimer_en));
    }

    SOC_REG_ABOVE_64_CLEAR(fld_value);
    soc_reg_above_64_field_get(unit, reg, reg_value, cnt_field, fld_value);
    if (SOC_REG_FIELD_VALID(unit, reg, ovf_field))
    {
        *ovf_value = soc_reg_above_64_field32_get(unit, reg, reg_value, ovf_field);
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
sh_dnx_counter_graphic_get_reg_value(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    sh_dnx_graphic_counter_reg_cache_t * reg_cache,
    soc_reg_above_64_val_t reg_value)
{
    int ii;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(reg_cache, _SHR_E_PARAM, "reg_cache");

    SOC_REG_ABOVE_64_CLEAR(reg_value);
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
        SOC_REG_ABOVE_64_COPY(reg_value, reg_cache->reg_value[ii]);
    }
    else
    {
        /*
         * read counter register and update cache
         */
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, port, index, reg_value));
        if (reg_cache->cache_num < SH_DNX_GRAPHIC_COUNTER_CACHE_MAX_NUM)
        {
            SOC_REG_ABOVE_64_COPY(reg_cache->reg_value[reg_cache->cache_num], reg_value);
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
sh_dnx_counter_graphic_counter_reg_core_id_get(
    int unit,
    sh_dnx_counter_register_t * counter_table,
    int reg_idx_cou_tbl,
    int blk_instance_id,
    int reg_arr_idx)
{
    int nof_blk_instances, reg_numels, total_instances;
    int nof_cores, inst_per_core, nof_cores_per_blk;
    bcm_core_t core_id = 0;
    uint32 first_array_idx;
    soc_reg_t reg;

    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    reg = counter_table->reg_arr[reg_idx_cou_tbl];
    nof_blk_instances = SOC_REG_BLOCK_NOF_INSTANCES(unit, reg);
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
        {
            reg_numels = SOC_REG_NUMELS(unit, reg);
            first_array_idx = SOC_REG_FIRST_ARRAY_INDEX(unit, reg);

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
sh_dnx_counter_graphic_per_core_counter_regs_read(
    int unit,
    sh_dnx_counter_register_t * counter_table,
    int reg_idx_cou_tbl,
    uint32 interval,
    int reg_port,
    int reg_arr_idx,
    sh_dnx_graphic_counter_reg_cache_t * reg_cache,
    uint64 *fld_value_u64,
    uint32 *overflow)
{
    soc_field_t reg_field, reg_overflow_field;
    soc_reg_above_64_val_t temp_fld_value;
    uint32 temp_overflow = 0, field_len;
    soc_reg_above_64_val_t reg_value;
    uint64 temp_fld_value_u64;
    bcm_core_t core_id;
    soc_reg_t reg;

    SHR_FUNC_INIT_VARS(unit);

    reg = counter_table->reg_arr[reg_idx_cou_tbl];
    reg_field = counter_table->cnt_field[reg_idx_cou_tbl];
    reg_overflow_field = counter_table->overflow_field[reg_idx_cou_tbl];
    field_len = soc_reg_field_length(unit, reg, reg_field);

    SHR_IF_ERR_EXIT(sh_dnx_counter_graphic_get_reg_value(unit, reg, reg_port, reg_arr_idx, reg_cache, reg_value));

    SOC_REG_ABOVE_64_CLEAR(temp_fld_value);
    soc_reg_above_64_field_get(unit, reg, reg_value, reg_field, temp_fld_value);

    /** some counter may are 128 bits. need to verify that it didn't exceed 64 bits value */
    SHR_IF_ERR_EXIT(sh_dnx_get_graphica_cnt_and_ovf_value
                    (unit, interval, reg, reg_value, reg_field,
                     reg_overflow_field, field_len, temp_fld_value, &temp_overflow));

    COMPILER_64_ZERO(temp_fld_value_u64);
    COMPILER_64_SET(temp_fld_value_u64, temp_fld_value[1], temp_fld_value[0]);

    core_id =
        sh_dnx_counter_graphic_counter_reg_core_id_get(unit, counter_table, reg_idx_cou_tbl, reg_port, reg_arr_idx);

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
sh_dnx_counter_graphic_per_core_counters_get(
    int unit,
    int nof_block,
    sh_dnx_counter_graphical_block_t * blk_group,
    sh_dnx_counter_graphical_options_t options,
    int *cnt_idx,
    int *blk_idx,
    sh_dnx_graphic_counter_reg_cache_t * reg_cache,
    soc_reg_above_64_val_t * fld_value,
    uint32 *overflow,
    int *field_len,
    int *row_valid)
{
    uint64 fld_value_u64[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    int instance_id, nof_instances, reg_numels;
    sh_dnx_counter_register_t counter_table;
    int reg_idx_cou_tbl, nof_regs = 1;
    int reg_arr_idx, core_idx;
    uint32 first_array_idx;
    soc_field_t cnt_field;
    soc_reg_t reg;

    SHR_FUNC_INIT_VARS(unit);

    if (*blk_idx >= nof_block)
    {
        SHR_EXIT();
    }

    counter_table = blk_group[*blk_idx].counter_table[*cnt_idx];
    SHR_IF_ERR_EXIT(counter_graphical_nof_regs_in_entry_get(unit, &counter_table, &nof_regs));

    for (core_idx = 0; core_idx < DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES; core_idx++)
    {
        overflow[core_idx] = FALSE;
        SOC_REG_ABOVE_64_CLEAR(fld_value[core_idx]);
        COMPILER_64_ZERO(fld_value_u64[core_idx]);
    }

    for (reg_idx_cou_tbl = 0; reg_idx_cou_tbl < nof_regs; reg_idx_cou_tbl++)
    {
        reg = counter_table.reg_arr[reg_idx_cou_tbl];
        cnt_field = counter_table.cnt_field[reg_idx_cou_tbl];

        if (INVALIDr == reg)
        {
            continue;
        }
        if (!soc_reg_field_valid(unit, reg, cnt_field))
        {
            continue;
        }

        *row_valid = 1;
        *field_len = soc_reg_field_length(unit, reg, cnt_field);
        nof_instances = SOC_REG_BLOCK_NOF_INSTANCES(unit, reg);
        reg_numels = SOC_REG_NUMELS(unit, reg);
        first_array_idx = SOC_REG_FIRST_ARRAY_INDEX(unit, reg);

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
                SHR_IF_ERR_EXIT(sh_dnx_counter_graphic_per_core_counter_regs_read
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
sh_dnx_counter_graphic_all_cores_counters_zero(
    int unit,
    soc_reg_above_64_val_t * fld_value)
{
    int nof_cores, core_id;

    nof_cores = dnx_data_device.general.nof_cores_get(unit);

    for (core_id = 0; core_id < nof_cores; core_id++)
    {
        if (!SOC_REG_ABOVE_64_IS_ZERO(fld_value[core_id]))
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
sh_dnx_counter_graphic_one_core_counter_zero(
    int unit,
    soc_reg_above_64_val_t * fld_value)
{
    int nof_cores, core_id;

    nof_cores = dnx_data_device.general.nof_cores_get(unit);

    for (core_id = 0; core_id < nof_cores; core_id++)
    {
        if (SOC_REG_ABOVE_64_IS_ZERO(fld_value[core_id]))
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
sh_dnx_counter_graphic_one_core_counter_overflow(
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
sh_dnx_counter_graphic_per_core_counters_print(
    int unit,
    int nof_block,
    sh_dnx_counter_graphical_block_t * blk_group,
    sh_dnx_counter_graphical_options_t options,
    prt_control_t * prt_ctr,
    int *cnt_idx,
    int *blk_idx,
    int *is_parting_line,
    soc_reg_above_64_val_t * fld_value,
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
            else if (options.non_zero && (sh_dnx_counter_graphic_one_core_counter_zero(unit, fld_value)))
            {
                /*
                 * counters on all cores are zero
                 */
                if (sh_dnx_counter_graphic_all_cores_counters_zero(unit, fld_value))
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
                        if (SOC_REG_ABOVE_64_IS_ZERO(fld_value[core_idx]))
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
            else if (options.non_zero && (sh_dnx_counter_graphic_all_cores_counters_zero(unit, fld_value) ||
                                          ((options.core_id != BCM_CORE_ALL) &&
                                           SOC_REG_ABOVE_64_IS_ZERO(fld_value[options.core_id]))))
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
                    overflow_final = sh_dnx_counter_graphic_one_core_counter_overflow(unit, overflow) || overflow_sum;
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
sh_dnx_counter_graphic_blk_cnt_idx_update(
    int unit,
    int nof_block,
    sh_dnx_counter_graphical_block_t * blk_group,
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
sh_dnx_counter_graphic_half_row_print(
    int unit,
    int nof_block,
    sh_dnx_counter_graphical_block_t * blk_group,
    sh_dnx_counter_graphical_options_t options,
    prt_control_t * prt_ctr,
    int *cnt_idx,
    int *blk_idx,
    int *is_parting_line,
    sh_dnx_graphic_counter_reg_cache_t * reg_cache)
{
    soc_reg_above_64_val_t fld_value[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    uint32 overflow[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    int field_len = 0, row_valid = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get per core counters in the counter block group.
     */
    SHR_IF_ERR_EXIT(sh_dnx_counter_graphic_per_core_counters_get
                    (unit, nof_block, blk_group, options, cnt_idx, blk_idx,
                     reg_cache, fld_value, overflow, &field_len, &row_valid));

    /*
     * Print per core counters in the counter block group according to print options.
     */
    SHR_IF_ERR_EXIT(sh_dnx_counter_graphic_per_core_counters_print
                    (unit, nof_block, blk_group, options, prt_ctr, cnt_idx, blk_idx,
                     is_parting_line, fld_value, overflow, field_len, row_valid));

    /*
     * Update block index, counter index and flag to step to next counter block group.
     */
    SHR_IF_ERR_EXIT(sh_dnx_counter_graphic_blk_cnt_idx_update
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
sh_dnx_counter_graphic_section_print(
    int unit,
    sh_dnx_counter_graphical_block_t * blk_grp_left,
    sh_dnx_counter_graphical_block_t * blk_grp_right,
    sh_dnx_counter_graphical_options_t options,
    prt_control_t * prt_ctr,
    sh_dnx_graphic_counter_reg_cache_t * reg_cache)
{
    int i_total = 0;
    int iblk_left = 0, icnt_left = 0;
    int iblk_right = 0, icnt_right = 0;
    int is_parting_line_left = FALSE, is_parting_line_right = FALSE;
    int nof_blk_left, nof_blk_right, row_of_section;
    int idx = 0, nof_block_total = 0;
    int block_ids[BLOCK_MAX_INDEX];
    uint32 nof_clocks;
    dnxc_time_t time_given_usec;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * calculate number of blocks in the left and right block group
     */
    SHR_IF_ERR_EXIT(counter_graphical_valid_block_get(unit, blk_grp_left, &nof_blk_left));
    SHR_IF_ERR_EXIT(counter_graphical_valid_block_get(unit, blk_grp_right, &nof_blk_right));
    /*
     * calculate row of this counter graphical section
     */
    SHR_IF_ERR_EXIT(counter_graphical_section_row_get
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
        sal_memset(block_ids, 0, BLOCK_MAX_INDEX * sizeof(int));
        SHR_IF_ERR_EXIT(counter_graphical_relevant_reg_blocks_get
                        (unit, blk_grp_left, nof_blk_left, block_ids, &nof_block_total));
        SHR_IF_ERR_EXIT(counter_graphical_relevant_reg_blocks_get
                        (unit, blk_grp_right, nof_blk_right, block_ids, &nof_block_total));

        /*
         * Config and triger the gtimer for the blocks
         */
        for (idx = 0; idx < nof_block_total; idx++)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(dnx_gtimer_set(unit, nof_clocks, SOC_BLOCK_TYPE(unit, block_ids[idx]),
                                                    SOC_BLOCK_NUMBER(unit, block_ids[idx])),
                                     "Failed to config gtimer in block(%s%s%s)\n", SOC_BLOCK_NAME(unit, block_ids[idx]),
                                     EMPTY, EMPTY);
        }
        for (idx = 0; idx < nof_block_total; idx++)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(dnx_gtimer_wait(unit, SOC_BLOCK_TYPE(unit, block_ids[idx]),
                                                     SOC_BLOCK_NUMBER(unit, block_ids[idx])),
                                     "Failed to wait gtimer in block(%s%s%s)\n", SOC_BLOCK_NAME(unit, block_ids[idx]),
                                     EMPTY, EMPTY);
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
        SHR_IF_ERR_EXIT(sh_dnx_counter_graphic_half_row_print
                        (unit, nof_blk_left, blk_grp_left, options, prt_ctr, &icnt_left, &iblk_left,
                         &is_parting_line_left, reg_cache));
        /*
         * print the half right row
         */
        SHR_IF_ERR_EXIT(sh_dnx_counter_graphic_half_row_print
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
            SHR_IF_ERR_EXIT_WITH_LOG(dnx_gtimer_clear(unit, SOC_BLOCK_TYPE(unit, block_ids[idx]),
                                                      SOC_BLOCK_NUMBER(unit, block_ids[idx])),
                                     "Failed to clear gtimer in block(%s%s%s)\n", SOC_BLOCK_NAME(unit, block_ids[idx]),
                                     EMPTY, EMPTY);
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
sh_dnx_counter_register_get(
    int unit,
    dnx_diag_graphical_block blk,
    sh_dnx_counter_graphical_block_t * working_block)
{
    int entry;
    int arr_index;
    sh_dnx_counter_register_t *table;

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
 *   to display the head or tail counter graphic
 * \param [in] unit - Unit #
 * \param [in] cdsp - indicate if counters were displayed per core
 * \param [in] head - indicate to print head or tail graphic
 * \return
 *      Standard error handling
 */
static shr_error_e
sh_dnx_diag_counter_graphical_head_tail_print(
    int unit,
    int cdsp,
    int head)
{
    char *printout_buffer_1[DNX_DIAG_GRAPHICAL_PRINT_BUFFER_LENGTH];
    char *printout_buffer_2[DNX_DIAG_GRAPHICAL_PRINT_BUFFER_LENGTH];
    int nof_cores;

    SHR_FUNC_INIT_VARS(unit);

    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    *printout_buffer_1 = "                                    ";
    *printout_buffer_2 = "               ";

    if (cdsp && (nof_cores > 1))
    {
        if (nof_cores < 4)
        {
            *printout_buffer_1 = "                                              ";
            *printout_buffer_2 = "                       ";
        }
        else
        {
            *printout_buffer_1 = "                                                                     ";
            *printout_buffer_2 = "                                       ";
        }
    }

    LOG_CLI(("%s||%s                 %s/\\ \n", *printout_buffer_1, *printout_buffer_2, *printout_buffer_2));
    if (head)
    {
        LOG_CLI(("%s||%sNetwork Interface%s||\n", *printout_buffer_1, *printout_buffer_2, *printout_buffer_2));
    }
    else
    {
        LOG_CLI(("%s||%s Fabric Interface%s||\n", *printout_buffer_1, *printout_buffer_2, *printout_buffer_2));
    }
    LOG_CLI(("%s\\/%s                 %s||\n", *printout_buffer_1, *printout_buffer_2, *printout_buffer_2));

    SHR_FUNC_EXIT;
}

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
sh_dnx_diag_counter_graphical_print(
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
    sh_dnx_counter_graphical_block_t group_left[DNX_DATA_MAX_GRAPHICAL_DIAG_NUM_OF_INDEX];
    sh_dnx_counter_graphical_block_t group_right[DNX_DATA_MAX_GRAPHICAL_DIAG_NUM_OF_INDEX];
    int left_blks, right_blks;
    dnx_diag_graphical_block blk;
    int field_col_id, temp_col_id;
    sh_dnx_graphic_counter_reg_cache_t *reg_cache;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    reg_cache = sal_alloc(sizeof(sh_dnx_graphic_counter_reg_cache_t), "reg_cache");
    SHR_NULL_CHECK(reg_cache, _SHR_E_MEMORY, "reg_cache");
    sal_memset(reg_cache, 0, sizeof(sh_dnx_graphic_counter_reg_cache_t));
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
                   sizeof(sh_dnx_counter_graphical_block_t) * dnx_data_graphical.diag.num_of_index_get(unit));
        sal_memset(group_right, 0,
                   sizeof(sh_dnx_counter_graphical_block_t) * dnx_data_graphical.diag.num_of_index_get(unit));
        /** get left */
        left_blks = 0;
        for (index = 0; index < dnx_data_graphical.diag.num_of_index_get(unit); index++)
        {
            if (dnx_data_graphical.diag.blocks_left_get(unit, row, index)->valid)
            {
                group_left[left_blks].counter_table = NULL;
                SHR_ALLOC_ERR_EXIT(group_left[left_blks].counter_table,
                                   (sizeof(sh_dnx_counter_register_t) *
                                    dnx_data_graphical.diag.num_of_block_entries_get(unit)), "diag graphical left",
                                   "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
                sal_snprintf(group_left[left_blks].block_name, DNX_DIAG_BLOCK_NAME_LENGTH, "%s",
                             dnx_data_graphical.diag.blocks_left_get(unit, row, index)->name);
                blk = dnx_data_graphical.diag.blocks_left_get(unit, row, index)->block;
                sh_dnx_counter_register_get(unit, blk, &group_left[left_blks]);
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
                                   (sizeof(sh_dnx_counter_register_t) *
                                    dnx_data_graphical.diag.num_of_block_entries_get(unit)), "signal_key", "%s%s%s\r\n",
                                   EMPTY, EMPTY, EMPTY);
                sal_snprintf(group_right[right_blks].block_name, DNX_DIAG_BLOCK_NAME_LENGTH, "%s",
                             dnx_data_graphical.diag.blocks_right_get(unit, row, index)->name);
                blk = dnx_data_graphical.diag.blocks_right_get(unit, row, index)->block;
                sh_dnx_counter_register_get(unit, blk, &group_right[right_blks]);
                right_blks++;
            }
        }
        /** print out counter graphical section 1 */
        rv = sh_dnx_counter_graphic_section_print(unit, group_left, group_right, options, prt_ctr, reg_cache);
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

/**
 * \brief
 *   For DNX device only
 *   To start gtimer of the blocks specified by block index array
 * \param [in] unit - Unit #
 * \param [in] nof_block - number of blocks contained in block_ids array
 * \param [in] block_ids - an array to contain the blockes
 * \param [in] interval - indicate how long gtimer keeps enabled
 * \param [in] time_units - indicate which time units are depicted by 'interval' parameter
 * \return
 *      Standard error handling
 */
shr_error_e
sh_dnx_diag_block_gtimer_start(
    int unit,
    int nof_block,
    int *block_ids,
    int interval,
    diag_dnxc_time_unit_e time_units)
{
    int i, is_supported;
    int rv = _SHR_E_NONE;
    uint32 nof_clocks;
    dnxc_time_t time_given_usec;

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
        /** verify if block supports gtimer */
        SHR_IF_ERR_EXIT(dnx_gtimer_block_type_is_supported(unit, SOC_BLOCK_TYPE(unit, block_ids[i]), &is_supported));
        if (is_supported == FALSE)
        {
            continue;
        }
        rv = dnx_gtimer_set(unit, nof_clocks, SOC_BLOCK_TYPE(unit, block_ids[i]), SOC_BLOCK_NUMBER(unit, block_ids[i]));
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "Failed(%d) config gtimer in block(%s%s)\n", rv,
                                 SOC_BLOCK_NAME(unit, block_ids[i]), EMPTY);
    }

    /*
     * Wait gtimer trigger to indicate counting finished
     */
    for (i = 0; i < nof_block; i++)
    {
        /** verify if block supports gtimer */
        SHR_IF_ERR_EXIT(dnx_gtimer_block_type_is_supported(unit, SOC_BLOCK_TYPE(unit, block_ids[i]), &is_supported));
        if (is_supported == FALSE)
        {
            continue;
        }
        rv = dnx_gtimer_wait(unit, SOC_BLOCK_TYPE(unit, block_ids[i]), SOC_BLOCK_NUMBER(unit, block_ids[i]));

        SHR_IF_ERR_EXIT_WITH_LOG(rv, "Failed(%d) wait gtimer in block(%s%s)\n", rv, SOC_BLOCK_NAME(unit, block_ids[i]),
                                 EMPTY);
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
 * \return
 *      Standard error handling
 */
shr_error_e
sh_dnx_diag_block_gtimer_stop(
    int unit,
    int nof_block,
    int *block_ids)
{
    int i, is_supported;
    int rv = _SHR_E_NONE;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Stop gtimer
     */
    for (i = 0; i < nof_block; i++)
    {
        /** verify if block supports gtimer */
        SHR_IF_ERR_EXIT(dnx_gtimer_block_type_is_supported(unit, SOC_BLOCK_TYPE(unit, block_ids[i]), &is_supported));
        if (is_supported == FALSE)
        {
            continue;
        }

        rv = dnx_gtimer_clear(unit, SOC_BLOCK_TYPE(unit, block_ids[i]), SOC_BLOCK_NUMBER(unit, block_ids[i]));
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "Failed(%d) clear gtimer in block(%s%s)\n", rv, SOC_BLOCK_NAME(unit, block_ids[i]),
                                 EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file diag_dnx_diag.h */
shr_error_e
dnx_cgm_drop_reason_to_str(
    int unit,
    soc_reg_t reg,
    soc_field_t field,
    int cdsp,
    dnx_counter_packet_dropped_reason_t * reason)
{
    int nof_cores = 0, nof_cores_to_iter;
    int i_fld = 0, i_core = 0;
    int nof_reason = 0;
    soc_field_info_t *fldinfo;
    soc_reg_info_t *reginfo;
    soc_reg_above_64_val_t reg_value[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    soc_reg_above_64_val_t fld_value[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    SHR_FUNC_INIT_VARS(unit);

    reginfo = &SOC_REG_INFO(unit, reg);
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    nof_cores_to_iter = cdsp ? nof_cores : 1;
    sal_memset(reason, 0, sizeof(dnx_counter_packet_dropped_reason_t) * nof_cores);

    for (i_core = 0; i_core < nof_cores; i_core++)
    {
        nof_reason = dnx_data_ingr_congestion.info.nof_dropped_reasons_cgm_get(unit);
        SHR_ALLOC_ERR_EXIT(reason[i_core].reason_buffer,
                           (nof_reason * sizeof(reason_buffer_t)),
                           "alloc reason buffer", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        sal_memset(reason[i_core].reason_buffer, 0, nof_reason * sizeof(reason_buffer_t));
    }

    for (i_core = 0; i_core < nof_cores; i_core++)
    {
        SOC_REG_ABOVE_64_CLEAR(reg_value[i_core]);
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, i_core, 0, reg_value[i_core]));
        sal_snprintf(reason[i_core].block_name, DNX_DIAG_BLOCK_NAME_LENGTH, "CGM");
    }
    if (field != INVALIDf)
    {
        for (i_core = 0; i_core < nof_cores; i_core++)
        {
            SOC_REG_ABOVE_64_CLEAR(fld_value[i_core]);
            soc_reg_above_64_field_get(unit, reg, reg_value[i_core], field, fld_value[i_core]);
        }
        for (i_core = 0; i_core < nof_cores; i_core++)
        {
            if (!SOC_REG_ABOVE_64_IS_ZERO(fld_value[i_core]))
            {
                break;
            }
        }
        if (i_core == nof_cores)
        {
            SHR_EXIT();
        }
        if (!cdsp)
        {
            for (i_core = nof_cores - 1; i_core >= 1; i_core--)
            {
                SOC_REG_ABOVE_64_OR(fld_value[i_core - 1], fld_value[i_core]);
            }
        }
        for (i_core = 0; i_core < nof_cores_to_iter; i_core++)
        {
            if (!SOC_REG_ABOVE_64_IS_ZERO(fld_value[i_core]))
            {
                sal_snprintf(reason[i_core].reason_buffer[reason[i_core].nof_reason++].buffer,
                             DNX_DIAG_GRAPHICAL_PRINT_BUFFER_LENGTH, "%s", SOC_FIELD_NAME(unit, field));
            }
        }
    }
    else
    {
        for (i_fld = 0; i_fld < reginfo->nFields; i_fld++)
        {
            fldinfo = &reginfo->fields[i_fld];
            for (i_core = 0; i_core < nof_cores; i_core++)
            {
                SOC_REG_ABOVE_64_CLEAR(fld_value[i_core]);
                soc_reg_above_64_field_get(unit, reg, reg_value[i_core], fldinfo->field, fld_value[i_core]);
            }
            for (i_core = 0; i_core < nof_cores; i_core++)
            {
                if (!SOC_REG_ABOVE_64_IS_ZERO(fld_value[i_core]))
                {
                    break;
                }
            }
            if (i_core == nof_cores)
            {
                continue;
            }
            if (!cdsp)
            {
                for (i_core = nof_cores - 1; i_core >= 1; i_core--)
                {
                    SOC_REG_ABOVE_64_OR(fld_value[i_core - 1], fld_value[i_core]);
                }
            }
            for (i_core = 0; i_core < nof_cores_to_iter; i_core++)
            {
                if (!SOC_REG_ABOVE_64_IS_ZERO(fld_value[i_core]))
                {
                    sal_snprintf(reason[i_core].reason_buffer[reason[i_core].nof_reason++].buffer,
                                 DNX_DIAG_GRAPHICAL_PRINT_BUFFER_LENGTH, "%s", SOC_FIELD_NAME(unit, fldinfo->field));
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/** see header file diag_dnx_diag.h */
shr_error_e
dnx_rqp_drop_reason_to_str(
    int unit,
    soc_reg_t reg,
    soc_field_t field,
    int cdsp,
    dnx_counter_packet_dropped_reason_t * reason)
{
    int field_len, nof_cores, nof_cores_to_iter;
    int i_core, i_bit;
    int nof_reason = 0;
    soc_reg_above_64_val_t reg_value[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    soc_reg_above_64_val_t fld_value[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    uint32 bit_mask = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * To get reasons from register ECGM_RQP_DISCARD_REASONS for block RQP, and
     * then put these reasons to reason[].reason_buffer_rqp for printing later
     */
    field_len = soc_reg_field_length(unit, reg, field);
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    nof_cores_to_iter = cdsp ? nof_cores : 1;
    sal_memset(reason, 0, sizeof(dnx_counter_packet_dropped_reason_t) * nof_cores);

    for (i_core = 0; i_core < nof_cores; i_core++)
    {
        nof_reason = dnx_data_ecgm.info.nof_dropped_reasons_rqp_get(unit);
        SHR_ALLOC_ERR_EXIT(reason[i_core].reason_buffer,
                           (nof_reason * sizeof(reason_buffer_t)),
                           "alloc reason buffer", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        sal_memset(reason[i_core].reason_buffer, 0, nof_reason * sizeof(reason_buffer_t));
    }

    for (i_core = 0; i_core < nof_cores; i_core++)
    {
        SOC_REG_ABOVE_64_CLEAR(reg_value[i_core]);
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, i_core, 0, reg_value[i_core]));
        SOC_REG_ABOVE_64_CLEAR(fld_value[i_core]);
        soc_reg_above_64_field_get(unit, reg, reg_value[i_core], field, fld_value[i_core]);
        sal_snprintf(reason[i_core].block_name, DNX_DIAG_BLOCK_NAME_LENGTH, "RQP");
    }

    for (i_core = 0; i_core < nof_cores; i_core++)
    {
        if (!SOC_REG_ABOVE_64_IS_ZERO(fld_value[i_core]))
        {
            break;
        }
    }
    if (i_core < nof_cores)
    {
        if (!cdsp)
        {
            for (i_core = nof_cores - 1; i_core >= 1; i_core--)
            {
                SOC_REG_ABOVE_64_OR(fld_value[i_core - 1], fld_value[i_core]);
            }
        }
        for (i_bit = 0; i_bit < field_len; i_bit++)
        {
            bit_mask = 0x1 << (i_bit % 32);
            for (i_core = 0; i_core < nof_cores_to_iter; i_core++)
            {
                if (fld_value[i_core][i_bit / 32] & bit_mask)
                {
                    sal_snprintf(reason[i_core].reason_buffer[reason[i_core].nof_reason++].buffer,
                                 DNX_DIAG_GRAPHICAL_PRINT_BUFFER_LENGTH, "%s",
                                 dnx_data_ecgm.info.dropped_reason_rqp_get(unit, i_bit)->reason);
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/** see header file diag_dnx_diag.h */
shr_error_e
dnx_pqp_drop_reason_to_str(
    int unit,
    soc_reg_t reg,
    soc_field_t field,
    int cdsp,
    dnx_counter_packet_dropped_reason_t * reason)
{
    int field_len, nof_cores, nof_cores_to_iter;
    int i_core, i_bit;
    int nof_reason = 0;
    soc_reg_above_64_val_t reg_value[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    soc_reg_above_64_val_t fld_value[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    uint32 bit_mask = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * To get reasons from register ECGM_RQP_DISCARD_REASONS for block RQP, and
     * then put these reasons to reason[].reason_buffer_rqp for printing later
     */
    field_len = soc_reg_field_length(unit, reg, field);
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    nof_cores_to_iter = cdsp ? nof_cores : 1;
    sal_memset(reason, 0, sizeof(dnx_counter_packet_dropped_reason_t) * nof_cores);
    for (i_core = 0; i_core < nof_cores; i_core++)
    {
        nof_reason = dnx_data_ecgm.info.nof_dropped_reasons_pqp_get(unit);
        SHR_ALLOC_ERR_EXIT(reason[i_core].reason_buffer,
                           (nof_reason * sizeof(reason_buffer_t)),
                           "alloc reason buffer", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        sal_memset(reason[i_core].reason_buffer, 0, nof_reason * sizeof(reason_buffer_t));
    }

    for (i_core = 0; i_core < nof_cores; i_core++)
    {
        SOC_REG_ABOVE_64_CLEAR(reg_value[i_core]);
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, i_core, 0, reg_value[i_core]));
        SOC_REG_ABOVE_64_CLEAR(fld_value[i_core]);
        soc_reg_above_64_field_get(unit, reg, reg_value[i_core], field, fld_value[i_core]);
        sal_snprintf(reason[i_core].block_name, DNX_DIAG_BLOCK_NAME_LENGTH, "PQP");
    }
    for (i_core = 0; i_core < nof_cores; i_core++)
    {
        if (!SOC_REG_ABOVE_64_IS_ZERO(fld_value[i_core]))
        {
            break;
        }
    }
    if (i_core < nof_cores)
    {
        if (!cdsp)
        {
            for (i_core = nof_cores - 1; i_core >= 1; i_core--)
            {
                SOC_REG_ABOVE_64_OR(fld_value[i_core - 1], fld_value[i_core]);
            }
        }
        for (i_bit = 0; i_bit < field_len; i_bit++)
        {
            bit_mask = 0x1 << (i_bit % 32);
            for (i_core = 0; i_core < nof_cores_to_iter; i_core++)
            {
                if (fld_value[i_core][i_bit / 32] & bit_mask)
                {
                    sal_snprintf(reason[i_core].reason_buffer[reason[i_core].nof_reason++].buffer,
                                 DNX_DIAG_GRAPHICAL_PRINT_BUFFER_LENGTH, "%s",
                                 dnx_data_ecgm.info.dropped_reason_pqp_get(unit, i_bit)->reason);
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/** see header file diag_dnx_diag.h */
shr_error_e
dnx_erpp_drop_reason_to_str(
    int unit,
    soc_reg_t reg,
    soc_field_t field,
    int cdsp,
    dnx_counter_packet_dropped_reason_t * reason)
{
    int nof_cores, nof_cores_to_iter;
    int i_core;
    soc_reg_above_64_val_t reg_value[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    uint32 fld_value[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    SHR_FUNC_INIT_VARS(unit);

    /*
     * To get reasons from register PQP_VISIBILITY_PACKET_DEBUG.VISIBILITY_ERPP_DROP
     */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    nof_cores_to_iter = cdsp ? nof_cores : 1;
    sal_memset(reason, 0, sizeof(dnx_counter_packet_dropped_reason_t) * nof_cores);
    for (i_core = 0; i_core < nof_cores; i_core++)
    {
        SHR_ALLOC_ERR_EXIT(reason[i_core].reason_buffer,
                           (sizeof(reason_buffer_t)), "alloc reason buffer", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        sal_memset(reason[i_core].reason_buffer, 0, sizeof(reason_buffer_t));
    }

    for (i_core = 0; i_core < nof_cores; i_core++)
    {
        SOC_REG_ABOVE_64_CLEAR(reg_value[i_core]);
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, i_core, 0, reg_value[i_core]));
        fld_value[i_core] = soc_reg_above_64_field32_get(unit, reg, reg_value[i_core], field);
        sal_snprintf(reason[i_core].block_name, DNX_DIAG_BLOCK_NAME_LENGTH, "PQP");
    }
    for (i_core = 0; i_core < nof_cores; i_core++)
    {
        if (fld_value[i_core] != 0)
        {
            break;
        }
    }
    if (i_core < nof_cores)
    {
        if (!cdsp)
        {
            for (i_core = nof_cores - 1; i_core >= 1; i_core--)
            {
                fld_value[i_core - 1] |= fld_value[i_core];
            }
        }
        for (i_core = 0; i_core < nof_cores_to_iter; i_core++)
        {
            if (fld_value[i_core])
            {
                reason[i_core].nof_reason = 1;
                sal_snprintf(reason[i_core].reason_buffer->buffer,
                             DNX_DIAG_GRAPHICAL_PRINT_BUFFER_LENGTH, "%s", "ERPP drop");
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/** see header file diag_dnx_diag.h */
shr_error_e
dnx_fdr_drop_reason_to_str(
    int unit,
    soc_reg_t reg,
    soc_field_t field,
    int cdsp,
    dnx_counter_packet_dropped_reason_t * reason)
{
    int nof_cores, nof_pipes;
    int i_core, i_pipe;
    uint64 reg_value, fld_value;
    SHR_FUNC_INIT_VARS(unit);

    /**
     * Initialize variables
     */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    nof_pipes = dnx_data_fabric.pipes.nof_pipes_get(unit);

    sal_memset(reason, 0, sizeof(dnx_counter_packet_dropped_reason_t) * nof_cores);
    for (i_core = 0; i_core < nof_cores; i_core++)
    {
        reason[i_core].reason_buffer = sal_alloc(sizeof(reason_buffer_t), "alloc reason buffer");
        sal_memset(reason[i_core].reason_buffer, 0, sizeof(reason_buffer_t));
        reason[i_core].nof_reason = 0;
    }

    for (i_core = 0; i_core < nof_cores; i_core++)
    {
        for (i_pipe = 0; i_pipe < nof_pipes; i_pipe++)
        {
            /**
             * Collect counters for all cores and pipes
             */
            COMPILER_64_ZERO(reg_value);
            COMPILER_64_ZERO(fld_value);
            SHR_IF_ERR_EXIT(soc_reg64_get(unit, reg, i_core, i_pipe, &reg_value));
            fld_value = soc_reg64_field_get(unit, reg, reg_value, field);

            if (fld_value)
            {
                if (cdsp)
                {
                    reason[i_core].nof_reason = 1;
                    sal_snprintf(reason[i_core].block_name, DNX_DIAG_BLOCK_NAME_LENGTH, "FDR");
                    sal_snprintf(reason[i_core].reason_buffer->buffer,
                                 DNX_DIAG_GRAPHICAL_PRINT_BUFFER_LENGTH, "Empty MC ID");
                    break;
                }
                /**
                 * If not using CDSP then the reason parameters must be printed in the first element of the reason array, so after finding drops we can exit the function
                 */
                else
                {
                    reason[0].nof_reason = 1;
                    sal_snprintf(reason[0].block_name, DNX_DIAG_BLOCK_NAME_LENGTH, "FDR");
                    sal_snprintf(reason[0].reason_buffer->buffer,
                                 DNX_DIAG_GRAPHICAL_PRINT_BUFFER_LENGTH, "Empty MC ID");
                    SHR_EXIT();
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   to get counter block according to block name
 * \param [in] unit - Unit #
 * \param [in] name - block name #
 * \param [out] block - counter block
 * \return
 *      Standard error handling
 */
static shr_error_e
dnx_diag_counter_block_get(
    int unit,
    const char *name,
    dnx_diag_graphical_block * block)
{
    int row, index;
    int strnlen_name;
    dnx_diag_graphical_block blk = dnx_graphical_block_count;
    int found = 0;
    SHR_FUNC_INIT_VARS(unit);

    strnlen_name = sal_strnlen(name, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    for (row = 0; row < dnx_data_graphical.diag.num_of_rows_get(unit); row++)
    {
        for (index = 0; index < dnx_data_graphical.diag.num_of_index_get(unit); index++)
        {
            /** check left */
            if (dnx_data_graphical.diag.blocks_left_get(unit, row, index)->valid)
            {
                if (!sal_strncasecmp
                    (dnx_data_graphical.diag.blocks_left_get(unit, row, index)->name, name, strnlen_name))
                {
                    blk = dnx_data_graphical.diag.blocks_left_get(unit, row, index)->block;
                    break;
                }
            }
            /** check right */
            if ((dnx_data_graphical.diag.blocks_right_get(unit, row, index)->valid) &&
                (!sal_strncasecmp
                 (dnx_data_graphical.diag.blocks_right_get(unit, row, index)->name, name, strnlen_name)))
            {
                blk = dnx_data_graphical.diag.blocks_right_get(unit, row, index)->block;
                break;
            }
        }
        if (blk != dnx_graphical_block_count)
        {
            /** found block */
            found = 1;
            break;
        }
    }
    if (found)
    {
        *block = blk;
    }
    else
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "Not found block(%s%s%s)\n", name, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   to get counter value
 * \param [in] unit - Unit #
 * \param [in] reg_entry - a pointer to a counter register structure
 * \param [out] counter - counter register value
 * \return
 *      Standard error handling
 */
static shr_error_e
dnx_diag_counter_value_get(
    int unit,
    sh_dnx_counter_register_t * reg_entry,
    uint64 *counter)
{
    int instance_id, nof_instances, reg_numels;
    int cnt_reg_index, reg_idx, nof_regs = 1;
    uint64 temp_fld_value_u64, fld_value_u64;
    soc_reg_above_64_val_t temp_fld_value;
    soc_reg_above_64_val_t reg_value;
    uint32 overflow = FALSE;
    uint32 first_array_idx;
    int field_len = 0;

    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO(fld_value_u64);
    SHR_IF_ERR_EXIT(counter_graphical_nof_regs_in_entry_get(unit, reg_entry, &nof_regs));
    for (reg_idx = 0; reg_idx < nof_regs; reg_idx++)
    {
        if (INVALIDr == reg_entry->reg_arr[reg_idx])
        {
            continue;
        }
        if (!soc_reg_field_valid(unit, reg_entry->reg_arr[reg_idx], reg_entry->cnt_field[reg_idx]))
        {
            continue;
        }

        nof_instances = SOC_REG_BLOCK_NOF_INSTANCES(unit, reg_entry->reg_arr[reg_idx]);
        reg_numels = SOC_REG_NUMELS(unit, reg_entry->reg_arr[reg_idx]);
        first_array_idx = SOC_REG_FIRST_ARRAY_INDEX(unit, reg_entry->reg_arr[reg_idx]);

        /** We can still use field "arr_i" to show multi row for one register with muti NUMELS **/
        if (reg_entry->reg_index > 0)
        {
            reg_numels = 1;
            first_array_idx = reg_entry->reg_index;
        }

        for (instance_id = 0; instance_id < nof_instances; instance_id++)
        {
            for (cnt_reg_index = first_array_idx; cnt_reg_index < reg_numels + first_array_idx; cnt_reg_index++)
            {
                /** read counter register and set counter value */
                SOC_REG_ABOVE_64_CLEAR(reg_value);
                field_len = soc_reg_field_length(unit, reg_entry->reg_arr[reg_idx], reg_entry->cnt_field[reg_idx]);
                if ((SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, reg_entry->reg_arr[reg_idx]).block, SOC_BLK_FDT)) ||
                    (SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, reg_entry->reg_arr[reg_idx]).block, SOC_BLK_FDA)))
                {
                    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg_entry->reg_arr[reg_idx], 0, instance_id, reg_value));
                }
                else
                {
                    SHR_IF_ERR_EXIT(soc_reg_above_64_get
                                    (unit, reg_entry->reg_arr[reg_idx], instance_id, cnt_reg_index, reg_value));
                }
                SOC_REG_ABOVE_64_CLEAR(temp_fld_value);
                soc_reg_above_64_field_get(unit, reg_entry->reg_arr[reg_idx],
                                           reg_value, reg_entry->cnt_field[reg_idx], temp_fld_value);
                /** some counter may are 128 bits. need to verify that it didn't exceed 64 bits value */
                SHR_IF_ERR_EXIT(sh_dnxc_get_cnt_and_ovf_value
                                (unit, 1, reg_entry->reg_arr[reg_idx],
                                 reg_value, reg_entry->cnt_field[reg_idx], field_len, temp_fld_value, &overflow));
                COMPILER_64_ZERO(temp_fld_value_u64);
                COMPILER_64_SET(temp_fld_value_u64, temp_fld_value[1], temp_fld_value[0]);
                COMPILER_64_ADD_64(fld_value_u64, temp_fld_value_u64);
            }
        }
    }

    /** set the fld value */
    COMPILER_64_COPY(*counter, fld_value_u64);
exit:
    SHR_FUNC_EXIT;
}

/** see header file diag_dnx_diag.h */
shr_error_e
dnx_diag_get_counter_specific(
    int unit,
    const char *name,
    const char *counterName,
    uint64 *counter)
{
    dnx_diag_graphical_block blk = dnx_graphical_block_count;
    int entry;
    int arr_index;
    sh_dnx_counter_register_t table;
    int found = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(name, _SHR_E_PARAM, "");
    SHR_NULL_CHECK(counterName, _SHR_E_PARAM, "");
    SHR_NULL_CHECK(counter, _SHR_E_PARAM, "");
    /** get block */
    SHR_IF_ERR_EXIT(dnx_diag_counter_block_get(unit, name, &blk));

    for (entry = 0; entry < dnx_data_graphical.diag.num_of_block_entries_get(unit); entry++)
    {
        if (dnx_data_graphical.diag.counters_get(unit, blk, entry)->regs[0] == INVALIDr)
        {
            break;
        }
        if (!sal_strncmp(dnx_data_graphical.diag.counters_get(unit, blk, entry)->name,
                         counterName, sal_strnlen(dnx_data_graphical.diag.counters_get(unit, blk, entry)->name,
                                                  SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)))
        {
            table.reg_name = dnx_data_graphical.diag.counters_get(unit, blk, entry)->name;
            table.reg_index = dnx_data_graphical.diag.counters_get(unit, blk, entry)->arr_i;
            table.fld_num = dnx_data_graphical.diag.counters_get(unit, blk, entry)->fld_num;
            table.is_err = dnx_data_graphical.diag.counters_get(unit, blk, entry)->is_err;
            for (arr_index = 0; arr_index < dnx_data_graphical.diag.counters_get(unit, blk, entry)->fld_num;
                 arr_index++)
            {
                table.reg_arr[arr_index] = dnx_data_graphical.diag.counters_get(unit, blk, entry)->regs[arr_index];
                table.cnt_field[arr_index] = dnx_data_graphical.diag.counters_get(unit, blk, entry)->flds[arr_index];
                table.overflow_field[arr_index] =
                    dnx_data_graphical.diag.counters_get(unit, blk, entry)->overflow_field[arr_index];
            }
            SHR_IF_ERR_EXIT(dnx_diag_counter_value_get(unit, &table, counter));
            found = 1;
            break;
        }
    }
    if (!found)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "Not found counter name(%s) in block(%s%s)\n", counterName, name,
                                 EMPTY);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   to print out one section of the couter description and register which contains x blocks
 * \param [in] unit - Unit #
 * \param [in] blk  - counter block
 * \param [in] blkName - counter block name
 * \param [in] prt_ctr - print control pointer
 * \return
 *      Standard error handling
 */
static shr_error_e
sh_dnx_diag_desc_section_print(
    int unit,
    dnx_diag_graphical_block blk,
    const char *blkName,
    prt_control_t * prt_ctr)
{
    int entry, arr_index;
    soc_reg_t reg;
    soc_field_t field;
    char value_str[PRT_COLUMN_WIDTH_BIG] = { 0 };
    char *str;
    int str_i;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(blkName, _SHR_E_PARAM, "");
    SHR_NULL_CHECK(prt_ctr, _SHR_E_PARAM, "");
    for (entry = 0; entry < dnx_data_graphical.diag.num_of_block_entries_get(unit); entry++)
    {
        if (dnx_data_graphical.diag.counters_get(unit, blk, entry)->regs[0] == INVALIDr)
        {
            break;
        }
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", entry);
        PRT_CELL_SET("%s", blkName);
        PRT_CELL_SET("%s", dnx_data_graphical.diag.counters_get(unit, blk, entry)->name);

        str_i = 0;
        str = &value_str[0];
        for (arr_index = 0; arr_index < dnx_data_graphical.diag.counters_get(unit, blk, entry)->fld_num; arr_index++)
        {
            reg = dnx_data_graphical.diag.counters_get(unit, blk, entry)->regs[arr_index];
            if (0 == arr_index)
            {
                str_i += sal_snprintf(str, PRT_COLUMN_WIDTH_BIG - str_i, "%s", SOC_REG_NAME(unit, reg));
            }
            else
            {
                str_i += sal_snprintf(str, PRT_COLUMN_WIDTH_BIG - str_i, " | %s", SOC_REG_NAME(unit, reg));
            }
            str = &value_str[str_i];
        }
        PRT_CELL_SET("%s", value_str);
        str_i = 0;
        str = &value_str[0];
        for (arr_index = 0; arr_index < dnx_data_graphical.diag.counters_get(unit, blk, entry)->fld_num; arr_index++)
        {
            field = dnx_data_graphical.diag.counters_get(unit, blk, entry)->flds[arr_index];
            if (0 == arr_index)
            {
                str_i += sal_snprintf(str, PRT_COLUMN_WIDTH_BIG - str_i, "%s", SOC_FIELD_NAME(unit, field));
            }
            else
            {
                str_i += sal_snprintf(str, PRT_COLUMN_WIDTH_BIG - str_i, " | %s", SOC_FIELD_NAME(unit, field));

            }
            str = &value_str[str_i];
        }
        PRT_CELL_SET("%s", value_str);
        PRT_CELL_SET("%s", dnx_data_graphical.diag.counters_get(unit, blk, entry)->doc);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   to display the description and register name in the internal blocks in virable format specified by option
 * \param [in] unit - Unit #
 * \param [in] args -
 * \param [in] sand_control -
 * \return
 *      Standard error handling
 */
shr_error_e
sh_dnx_diag_desc_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *match_n;
    int block_is_present = FALSE;
    int object_col_id;
    dnx_diag_graphical_block blk;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(NO_UNIT);

    /*
     * Get parameters
     */
    SH_SAND_IS_PRESENT("block", block_is_present);
    SH_SAND_GET_STR("block", match_n);

    PRT_TITLE_SET("BLOCK COUNTERS DESCRIPTION");
    /*
     * add column for displaying counters per block
     */
    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &object_col_id, "No.");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Block");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Counter");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Register");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Field");
    PRT_COLUMN_ADDX_FLEX(PRT_FLEX_ASCII, PRT_XML_ATTRIBUTE, object_col_id, NULL, "Description");

    if (!block_is_present)
    {
        /** no block present */
        SHR_CLI_EXIT(_SHR_E_NONE, "No block specified for block register display%s%s%s\n", EMPTY, EMPTY, EMPTY);
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_diag_counter_block_get(unit, match_n, &blk));
        SHR_IF_ERR_EXIT(sh_dnx_diag_desc_section_print(unit, blk, match_n, prt_ctr));
    }
    PRT_COMMIT;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   gtimer start
 * \param [in] unit - Unit #
 * \param [in] args -
 * \param [in] sand_control -
 * \return
 *      Standard error handling
 */
static shr_error_e
sh_dnx_gtimer_start_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *match_n;
    uint32 interval;
    diag_dnxc_time_unit_e time_units = DIAG_DNXC_TIME_UNIT_SEC;
    int nof_block = 0;
    int block_ids[BLOCK_MAX_INDEX];

    SHR_FUNC_INIT_VARS(NO_UNIT);

    /*
     * Get parameters
     */
    SH_SAND_GET_STR("block", match_n);
    SH_SAND_GET_UINT32("InTerVal", interval);
    SH_SAND_GET_ENUM("timeUnits", time_units);

    if (counter_block_name_match(unit, match_n, &nof_block, block_ids) != _SHR_E_NONE)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "no matched block found\n");
    }

    if (interval)
    {
        SHR_IF_ERR_EXIT(sh_dnx_diag_block_gtimer_start(unit, nof_block, block_ids, interval, time_units));
    }
    else
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "must be specify interval\n");
    }

    cli_out(">>>>Success<<<< \n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   gtimer stop
 * \param [in] unit - Unit #
 * \param [in] args -
 * \param [in] sand_control -
 * \return
 *      Standard error handling
 */
static shr_error_e
sh_dnx_gtimer_stop_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *match_n;
    int nof_block = 0;
    int block_ids[BLOCK_MAX_INDEX];

    SHR_FUNC_INIT_VARS(NO_UNIT);

    /*
     * Get parameters
     */
    SH_SAND_GET_STR("block", match_n);

    if (counter_block_name_match(unit, match_n, &nof_block, block_ids) != _SHR_E_NONE)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "no matched block found\n");
    }

    SHR_IF_ERR_EXIT(sh_dnx_diag_block_gtimer_stop(unit, nof_block, block_ids));

    cli_out(">>>>Success<<<< \n");

exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */

sh_sand_man_t sh_dnx_gtimer_start_man = {
    .brief = "Gtimer diagnostic command",
    .full = "Gtimer command, it enables gtimer blocks and waits gtimer trigger to indicate counting finished",
    .synopsis = "[<block_name>] [interval=<integer>] [timeunites=<time_unit>]",
    .examples = ""
            "CGM\n"
            "CGM interval=1\n"
            "CGM interval=1 timeunits=sec"
};

sh_sand_man_t sh_dnx_gtimer_stop_man = {
    .brief = "Gtimer diagnostic command",
    .full = "Gtimer command, it stops gtimer blocks",
    .synopsis = "[<block_name>]",
    .examples = ""
            "CGM"
};


/**
 * List of the supported commands, pointer to command function and command usage function.
 */

static sh_sand_option_t sh_dnx_gtimer_start_options[] = {
     /** name type desc default ext */ \
    DIAG_DNX_GTIMER_OPTIONS, \
    {"interval", SAL_FIELD_TYPE_UINT32, "interval for gtimer (in timeUnits)", "1", NULL}, \
    {"timeUnits", SAL_FIELD_TYPE_ENUM, "Time units to use]", "sec", (void *) diag_dnxc_diag_time_units}, \
    {NULL}
};

static sh_sand_option_t sh_dnx_gtimer_stop_options[] = {
     /** name type desc default ext */ \
    DIAG_DNX_GTIMER_OPTIONS, \
    {NULL}
};

/**
 * gtimer cmds
 */
sh_sand_cmd_t sh_dnx_gtimer_cmds[] = {
    /*keyword,  action,                   command, options,                       man                                     Flags                                      Conditional CB */
    {"start", sh_dnx_gtimer_start_cmd, NULL,    sh_dnx_gtimer_start_options,  &sh_dnx_gtimer_start_man},
    {"stop", sh_dnx_gtimer_stop_cmd, NULL,    sh_dnx_gtimer_stop_options, &sh_dnx_gtimer_stop_man},
    {NULL}
};

sh_sand_man_t sh_dnx_gtimer_man = {
    .brief = "Gtimer command"
};

/* *INDENT-ON* */
