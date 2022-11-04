
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

#include <appl/diag/sand/diag_sand_access.h>

#include <sal/appl/sal.h>
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_access.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/dnxc/diag_dnxc_diag.h>
#include <appl/diag/dnxc/diag_dnxc_diag_access.h>
#include <soc/dnxc/dnxc_time.h>
#include <soc/counter.h>
#include <shared/utilex/utilex_bitstream.h>
/*
 * Includes related to DATA
 * {
 */

/*
 * }
 */

/*
 * Local includes
 * {
 */

#ifdef BCM_ACCESS_SUPPORT

extern sh_sand_enum_t diag_dnxc_diag_time_units[];

/*
 * }
 */

extern shr_error_e sh_dnx_diag_desc_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern shr_error_e sh_dnx_counter_convert_to_dnx_time_units(
    diag_dnxc_time_unit_e time_units,
    dnxc_time_unit_e * dnx_time_units);

extern shr_error_e access_dnx_diag_counter_graphical_print(
    int unit,
    int cdsp,
    int core_id,
    int non_zero,
    int in_hex,
    uint32 interval,
    diag_dnxc_time_unit_e time_units,
    sh_sand_control_t * sand_control);
shr_error_e access_dnx_diag_block_gtimer_start(
    int unit,
    int nof_block,
    int *block_ids,
    int *inst_ids,
    int interval,
    diag_dnxc_time_unit_e time_units);

shr_error_e access_dnx_diag_block_gtimer_stop(
    int unit,
    int nof_block,
    int *block_ids,
    int *inst_ids);

/* *INDENT-OFF* */
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
access_dnxc_diag_block_gtimer_start(
    int unit,
    int nof_block,
    int *block_ids,
    int *inst_ids,
    int interval,
    diag_dnxc_time_unit_e time_units)
{
    int rv = _SHR_E_NONE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_WITH_LOG(rv, "Failed(%d) sh_dnxc_diag_block_gtimer_start%s%s\n", rv, EMPTY, EMPTY);

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
access_dnxc_diag_block_gtimer_stop(
    int unit,
    int nof_block,
    int *block_ids,
    int *inst_ids)
{

    int rv = _SHR_E_NONE;
    SHR_FUNC_INIT_VARS(unit);
    
    SHR_IF_ERR_EXIT_WITH_LOG(rv, "Failed(%d) in sh_dnxc_diag_block_gtimer_stop%s%s\n", rv, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}


/**
 * \brief
 *   Check if the register match the blocks specified by "*match_n"
 * \param [in] unit - Unit #
 * \param [in] match_n - to specify block name that block should match with
 * \param [out] nof_block - to get number of blocks that this register matches with
 * \param [out] block_ids - an integer array to get all of block ID that this register matches with
 * \return
 *      Standard error handling
 */
static shr_error_e
access_counter_block_name_match(
    int unit,
    char *match_n,
    int *nof_block,
    int *block_ids,
    int *inst_ids)
{

    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit);  /** Access information for the device */
    const access_device_type_info_t *device_info = runtime_info->device_type_info;  /** Constant access information for the device type */
    const access_device_block_t *local_block = NULL;
    access_local_block_id_t block_i;
    int i, cmp_len;
    int inst_i;
    char block_name[BLOCK_NAME_LENGTH];
    shr_error_e rv = _SHR_E_NOT_FOUND;

    if (NULL == nof_block)
    {
        LOG_CLI(("Invalid parameter: nof_block\n"));
        return _SHR_E_PARAM;
    }
    if (NULL == block_ids)
    {
        LOG_CLI(("Invalid parameter: block_ids\n"));
        return _SHR_E_PARAM;
    }

    *nof_block = 0;
    cmp_len = sal_strnlen(match_n, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    /*
     * For the block name with index (0, 1, ....., 9), only the index specific block is matched
     * For the block name without index, all the blocks with any index are matched.
     */
    /*
     * exclude compare block which not specified
     */
    i = 0; 
    while ((cmp_len - i) > 0 && ASCII_CODE_OF_9 >= match_n[cmp_len - 1 - i] && match_n[cmp_len - 1 - i] >= ASCII_CODE_OF_0)
    {
        i++;
    }
    if ( i > 0 )
    {
        inst_i = sal_atoi(match_n + cmp_len - i);
    }
    else
    {
        /*block for all instance*/
        inst_i = -1;
    }
    
    sal_memset(block_name, 0, sizeof(block_name));
    sal_strncpy(block_name, match_n, cmp_len - i);
    
    for (block_i = 0; block_i < device_info->nof_blocks; block_i++)
    {
        local_block = device_info->blocks + block_i;

        if (!sal_strncasecmp(block_name, access_global_block_types[local_block->block_type].block_name,
            cmp_len))
        {
            if (*nof_block >= ACCESS_NOF_GLOBAL_BLOCK_TYPES)
            {
                LOG_CLI(("Number of block:%d exceeded maximum:%d\n", *nof_block, ACCESS_NOF_GLOBAL_BLOCK_TYPES));
                rv = _SHR_E_PARAM;
                break;
            }
            if (inst_i >= local_block->nof_instances)
            {
                LOG_CLI(("Invalid parameter: block instant exceeded maximum :%d\n", local_block->nof_instances));
                return _SHR_E_PARAM;
            }
            block_ids[*nof_block] = block_i;
            inst_ids[*nof_block] = inst_i;
            *nof_block += 1;
            rv = _SHR_E_NONE;
        }
    }

    return rv;
}

/**
 * \brief
 *   To get the overflow field for specific cnt
 * \param [in] unit - Unit #
 * \param [in] regmem_i - The register id of counter
 * \param [in] cnt_field - The field of counter
 * \return
 *      overflow field or INVALIDf
 */
static access_local_field_id_t
access_dnxc_get_overflow_field(
    int unit,
    access_local_regmem_id_t regmem_i,
    access_local_field_id_t cnt_field)
{
    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); 
    const access_device_type_info_t *device_info = runtime_info->device_type_info;
    const access_device_regmem_t *rm_info = device_info->local_regs + regmem_i;
    access_local_field_id_t rc = ACCESS_INVALID_FIELD_ID;
    const access_device_field_t *field_info, *cnt_filed_info;
    access_local_field_id_t fld;
    char over_field_name[256];
    char *over_field_str;

    cnt_filed_info = device_info->local_fields + cnt_field;
    sal_snprintf(over_field_name, sizeof(over_field_name), "%s_o", access_global_names[global_field_name_indices[cnt_filed_info->global_field_id]]);
    for (fld = rm_info->fields_index; fld < rm_info->fields_index + rm_info->nof_fields; fld++)
    {
        field_info = device_info->local_fields + fld;
        over_field_str = sal_strcasestr(access_global_names[global_field_name_indices[field_info->global_field_id]], over_field_name);
        if (over_field_str != NULL) 
        {
            rc = fld;
            break;
        }
    }


    return rc;
}

/**
 * \brief
 *   To get the counter value and overflow value.
 * \param [in] unit - Unit #
 * \param [in] interval - indicate the interval of g-timer (second)
 * \param [in] regmem_i - The register id of counter
 * \param [in] regmem_value - The value of register
 * \param [in] cnt_field - The field of counter
 * \param [in] cnt_field_len - The field length of counter.
 * \param [out] fld_value - The counter value
 * \param [out] ovf_value - The overflow field value
 * \return
 *      Standard error handling
 */
shr_error_e
access_dnxc_get_cnt_and_ovf_value(
    int unit,
    uint32 interval,
    access_local_regmem_id_t regmem_i,
    access_reg_above_64_val_t regmem_value,
    access_local_field_id_t cnt_field,
    uint32 cnt_field_len,
    access_reg_above_64_val_t fld_value,
    uint32 *ovf_value)
{
    access_local_field_id_t ovf_field;
    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); 
    int is_gtimer_en;

    SHR_FUNC_INIT_VARS(unit);

    is_gtimer_en = FALSE;

    sal_memset(fld_value, 0, sizeof(access_reg_above_64_val_t));
    SHR_IF_ERR_EXIT(access_local_field_get(runtime_info, cnt_field, regmem_value, fld_value));
    ovf_field = access_dnxc_get_overflow_field(unit, regmem_i, cnt_field);

    if (ovf_field != ACCESS_INVALID_FIELD_ID)
    {
        SHR_IF_ERR_EXIT(access_local_field_get(runtime_info, ovf_field, regmem_value, ovf_value));
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


/**
 * \brief
 *   to display the counters in the blocks specified by option "name"
 *   or the counters in all the blocks when setting option "all"
 * \param [in] unit - Unit #
 * \param [in] match_n - pointer to string to filter block name
 * \param [in] non_zero - indicate to display non_zero counters only
 * \param [in] in_hex - indicate to display counters in hex format
 * \param [in] interval - indicate how long gtimer keeps enabled in time_units
 * \param [in] time_units - indicate which time units are depicted 'interval' parameter
 * \param [in] is_error - indicate to display error counter only
 * \param [in] sand_control -
 * \return
 *      Standard error handling
 */
shr_error_e
access_dnxc_diag_counters_block_print(
    int unit,
    char *match_n,
    int non_zero,
    int in_hex,
    uint32 interval,
    diag_dnxc_time_unit_e time_units,
    int is_error,
    sh_sand_control_t * sand_control)
{
    int object_col_id;
    int idx = 0, nof_block = 0;
    int block_idx;
    int block_ids[ACCESS_NOF_GLOBAL_BLOCK_TYPES];
    int inst_ids[ACCESS_NOF_GLOBAL_BLOCK_TYPES];
    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit);  /** Access information for the device */
    const access_device_type_info_t *device_info = runtime_info->device_type_info;  /** Constant access information for the device type */
    const access_device_block_t *local_block = NULL;
    const access_device_regmem_t *rm_info = NULL;
    access_local_regmem_id_t regmem_i;
    access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
    const uint8 *block_instances_flags;
    uint16 array_index = -1;
    uint32 flags_to_be_set = 0, flags_to_be_clear = 0;
    uint32 fval[DIAG_ACCESS_MAX_REG_UINT32S] = { 0 }, regmem_value[DIAG_ACCESS_MAX_REG_UINT32S] = { 0 };
    const access_device_field_t *field_info;
    access_local_field_id_t fld;
    uint32 ovf_value;
    char value_str[PRT_COLUMN_WIDTH_BIG] = { 0 };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(NO_UNIT);

    PRT_TITLE_SET("BLOCK COUNTERS");

    /*
     * add column for displaying counters per block
     */
    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &object_col_id, "No.");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Block");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Counter");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Numels");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Value");

    sal_memset(block_ids, 0, ACCESS_NOF_GLOBAL_BLOCK_TYPES * sizeof(int));
    if (access_counter_block_name_match(unit, match_n, &nof_block, block_ids, inst_ids) != _SHR_E_NONE)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "no matched block found\n");
    }

    if (interval)
    {
        SHR_IF_ERR_EXIT(access_dnxc_diag_block_gtimer_start(unit, nof_block, block_ids, inst_ids, interval, time_units));
    }

    /*
     * filter out write-only registers
     */
    flags_to_be_clear |= ACCESS_REGMEM_FLAG_WO | ACCESS_REGMEM_FLAG_ACCESS_TYPE_PER_PORT;
    /*
     *  only display counter registers
     */
    flags_to_be_set |= ACCESS_REGMEM_FLAG_COUNTER;

    for (block_idx = 0; block_idx < nof_block; block_idx++)
    {
        local_block = device_info->blocks + block_ids[block_idx];

        /** Loop over the registers of the block */
        for (regmem_i = local_block->registers_start;
             regmem_i < local_block->registers_start + local_block->nof_registers; regmem_i++)
        {
            /** Register to access */
            rm_info = device_info->local_regs + regmem_i;

            /** Filter by flags */
            if ((rm_info->flags & flags_to_be_set) != flags_to_be_set)
            {
                continue;
            }
            if ((rm_info->flags & flags_to_be_clear) != 0)
            {
                continue;
            }

            /** Check if a block instance is enabled */
            block_instances_flags = runtime_info->block_instance_flags + local_block->instances_start_index;
            for (inst_i = 0; inst_i < local_block->nof_instances; inst_i++)
            {
                if ((block_instances_flags[inst_i] & ACCESS_RUNTIME_BLOCK_INSTANCE_FLAG_DISABLED))
                {
                    continue;
                }
                if (inst_ids[block_idx] != -1 && inst_ids[block_idx] != inst_i)
                {
                    continue;
                }
                for (array_index = rm_info->first_array_index;
                     array_index < rm_info->nof_array_indices + rm_info->first_array_index; array_index++)
                {

                    SHR_IF_ERR_EXIT(access_local_regmem(runtime_info, 0, regmem_i, inst_i, array_index, 0, regmem_value));
                    for (fld = rm_info->fields_index; fld < rm_info->fields_index + rm_info->nof_fields; fld++)
                    {
                        /** Field to access */
                        field_info = device_info->local_fields + fld;
                        if (!(field_info->flags & ACCESS_FIELD_FLAG_COUNTER_FEILD))
                        {
                            continue;
                        }

                        SHR_IF_ERR_EXIT(access_local_field_get(runtime_info, fld, regmem_value, fval));
                      
                        SHR_IF_ERR_EXIT(access_dnxc_get_cnt_and_ovf_value(unit, interval, regmem_i, regmem_value, fld,
                                                                      field_info->size_in_bits, fval, &ovf_value));
                        
                        if (non_zero && SHR_BITNULL_RANGE(fval, 0, DIAG_ACCESS_MAX_REG_UINT32S*32) && (ovf_value == 0))
                        {
                            continue;
                        }
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%d", idx++);
                        PRT_CELL_SET("%s%u", access_global_block_types[local_block->block_type].block_name, inst_i);
                        PRT_CELL_SET("%s", access_global_names[global_field_name_indices[field_info->global_field_id]]);
                        PRT_CELL_SET("%d", array_index);
                        /*
                         * when counter length is larger than 64 bits, to display in hex format
                         */
                        if (in_hex || (field_info->size_in_bits > 64))
                        {
                            format_value_hex_to_str(fval, (int)(field_info->size_in_bits), value_str);
                        }
                        else
                        {
                            format_value_dec_to_str(fval, (int)(field_info->size_in_bits), value_str);
                        }

                        if (ovf_value)
                        {
                            PRT_CELL_SET("%s(ovf)", value_str);
                        }
                        else
                        {
                            PRT_CELL_SET("%s", value_str);
                        }
                    }
                }
            }
        }
    }

    if (interval)
    {
        SHR_IF_ERR_EXIT(access_dnxc_diag_block_gtimer_stop(unit, nof_block, block_ids, inst_ids));
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}


/**
 * \brief
 *   to display the counters in the internal blocks in virable format specified by option
 * \param [in] unit - Unit #
 * \param [in] args -
 * \param [in] sand_control -
 * \return
 *      Standard error handling
 */
shr_error_e
access_dnxc_diag_counter_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *match_n;
    uint32 interval;
    diag_dnxc_time_unit_e time_units = DIAG_DNXC_TIME_UNIT_SEC;
    int block_is_present = FALSE;
    int non_zero, in_hex, graphical, is_error;

    SHR_FUNC_INIT_VARS(NO_UNIT);
    /*
     * Get parameters
     */
    SH_SAND_IS_PRESENT("block", block_is_present);
    SH_SAND_GET_STR("block", match_n);
    SH_SAND_GET_BOOL("GRaphical", graphical);

    if (block_is_present && graphical)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Option GRAPHICAL cannot be provided together with BLOCK option!\n");
    }
    SH_SAND_GET_BOOL("NonZero", non_zero);
    SH_SAND_GET_BOOL("hex", in_hex);
    SH_SAND_GET_UINT32("InTerVal", interval);
    SH_SAND_GET_ENUM("timeUnits", time_units);
    SH_SAND_GET_BOOL("ERROR", is_error);
    

    if (graphical)
    {

    }
    else
    {
        SHR_IF_ERR_EXIT(access_dnxc_diag_counters_block_print
                        (unit, match_n, non_zero, in_hex, interval, time_units, is_error, sand_control));
    }

exit:
    SHR_FUNC_EXIT;
}

#endif /* BCM_ACCESS_SUPPORT */

