
/** \file diag_dnxc_access.c
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

#define APPL_ACCESS_MAX_MEM_UINT32S 256
#define APPL_ACCESS_MAX_REG_UINT32S 30

/** Defines for user input flags/options, used in the Getreg command */
#define REGMEM_PRINT_ONLY_HEX_VALUES 0x01
#define REGMEM_PRINT_RAW             0x02
#define REGMEM_PRINT_FIELDS          0x04
#define REGMEM_PRINT_CHG             0x08
#define REGMEM_PRINT_ADDR            0x10
#define REGMEM_PRINT_DISABLED_INST   0x20
#define DUMP_DISABLE_CACHE           0x40
#define DUMP_ADDR_ONLY               0x60

/* *INDENT-OFF* */
#ifdef BCM_ACCESS_SUPPORT
/*
 * Find a memory whose name is mem_name.
 * Return local regmem ID, array index and block instance number to access.
 * On success, stores the local regmem ID at out_mem_id, array index at array_index and instance number at inst_num and returns 0.
 * If the array index is not specified in the input string, -1 will be store in array_index.
 * If the instance number is not specified in the input string, ACCESS_ALL_BLOCK_INSTANCES will be stored in inst_num.
 * If the memory name was not found, returns 1.
 * On error/failure returns -1.
 *
 * For dump and write commands a memory array index may be added after the memory name in square brackets like: mem_name[3].
 * The function will fail for memory arrays for which an array index
 * is not specified when array_index is not NULL.
 * The function will fail for non array memories for which an array index is specified.
 * For non array memories, array_index may be NULL; If not NULL -1 will be returned in it.
 */
int
access_parse_memory_name(
    access_runtime_info_t *runtime_info,
    char *mem_name,
    uint16 *array_index,
    access_block_instance_num_t *inst_num,
    access_local_regmem_id_t *out_mem_id)
{
    const access_device_type_info_t *device_info = runtime_info->device_type_info; /** constant access information for the device type */
    access_local_regmem_id_t mem_i = -1;
    const access_device_regmem_t *rm_info = NULL, *mem_aliased_to;
    const access_device_block_t *local_block;
    char *close_bracket, /** the close bracket for the array index in reg_name */
       *open_bracket,   /** the open bracket for the array index in reg_name */
        tmp[80], *block_name, *inst_str;
    unsigned arr_ind;
    uint32 inst_number;

    if (mem_name == NULL || *mem_name == '\0')
    {
        cli_out("ERROR: must specify memory name\n");
        return -1;
    }

    /** Do not destroy input string */
    sal_strncpy_s(tmp, mem_name, sizeof(tmp));

    /** check if an array index is specified */
    if ((((open_bracket = sal_strnchr(tmp, '[', sizeof(tmp))) != NULL) &&
         ((close_bracket = sal_strnchr(open_bracket + 1, ']', sizeof(tmp))) != NULL) && (close_bracket - open_bracket > 1)) ||
        (((open_bracket = sal_strnchr(tmp, '(', sizeof(tmp))) != NULL) &&
         ((close_bracket = sal_strnchr(open_bracket + 1, ')',sizeof(tmp))) != NULL) && (close_bracket - open_bracket > 1)))
    {
        if (array_index == NULL)
        {
            cli_out("ERROR: missed array index number\n");
            return -1;
        }
        /** handle memory array, handle initial spaces after '[' */
        for (close_bracket = open_bracket + 1; *close_bracket == ' '; ++close_bracket)
        {
        }
        if (*close_bracket < '0' || *close_bracket > '9')
        {
            cli_out("ERROR: bad array index character\n");
            return -1;
        }
        /** read array index */
        arr_ind = 0;
        for (; *close_bracket >= '0' && *close_bracket <= '9'; ++close_bracket)
        {
            arr_ind = arr_ind * 10 + (*close_bracket - '0');
        }
        /** handle trailing spaces before ']' */
        for (; *close_bracket == ' '; ++close_bracket)
        {
        }
        if ((*close_bracket != ']') && (*close_bracket != ')'))
        {  /** verify closing ']' */
            cli_out("ERROR: bad array index character\n");
            return -1;
        }
        *array_index = arr_ind;
        close_bracket = sal_strnchr(close_bracket + 1, '.', sizeof(tmp));
        *open_bracket = '\0';
    }
    else
    {
        arr_ind = 0xffffffff;
        close_bracket = sal_strnchr(tmp, '.', sizeof(tmp));
        if (array_index != NULL)
        {
            *array_index = -1;
        }
    }
    if (close_bracket != NULL)
    {
        *close_bracket++ = '\0';
    }

    /** loop over all memories */
    for (rm_info = device_info->local_mems, mem_i = device_info->nof_mems; mem_i; mem_i--, rm_info++)
    {
        /** find by name */
        if (sal_strncasecmp
            (tmp, access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]], sizeof(tmp)) == 0)
        {
            break;
        }
    }
    if (mem_i == 0)
    {  /** the specified memory is not found */
        return 1;
    }

    if ((rm_info->flags & ACCESS_REGMEM_FLAG_IS_ALIAS) != 0)
    {
        mem_aliased_to = device_info->local_regs + rm_info->u.mem.alias_regmem;
    }
    else
    {
        mem_aliased_to = rm_info;
    }

    /** memory array related validation */
    if (arr_ind != 0xffffffff)
    {
        /** an array index was specified, verify that this is a memory array and that the index is in range */
        if (mem_aliased_to->nof_array_indices == 1 && arr_ind != 0)
        {
            cli_out("ERROR: an array index was specified for a memory which is not an array\n");
            return -1;
        }
        else if (arr_ind >= mem_aliased_to->first_array_index + mem_aliased_to->nof_array_indices
                 || arr_ind < mem_aliased_to->first_array_index)
        {
            cli_out("ERROR: array index %u is out of range, should be in the range %u-%u\n ",
                    arr_ind, mem_aliased_to->first_array_index,
                    mem_aliased_to->first_array_index + mem_aliased_to->nof_array_indices - 1);
            return -1;
        }
    }
    /** Check for instance number in the input string */
    if (close_bracket)
    {
        local_block = device_info->blocks + rm_info->local_block_id;
        block_name = access_global_block_types[local_block->block_type].block_name;
        for (inst_str = close_bracket; sal_tolower(*inst_str) == sal_tolower(*block_name); block_name++, inst_str++)
        {
            if (*inst_str == '\0')
            {
                cli_out("ERROR: must specify an instance number\n");
                return -1;
            }
        }
        if (*inst_str == '\0' || *block_name != '\0')
        {
            /** Reached the end of the block instance name or the whole block name did not matched */
            cli_out("\"%s\" is not a block instance of memory %s\n", close_bracket,
                    access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
            return -1;
        }
        if (sal_isdigit(*inst_str))
        {
            inst_number = parse_integer(inst_str);
        }
        else
        {
            cli_out("ERROR: unrecognized block instance %s\n", close_bracket);
            return -1;
        }
        /** Check that the inst number is not out of range*/
        if (inst_number >= local_block->nof_instances)
        {
            cli_out("Memory %s of block %s has %u instances, but given instance %u\n",
                    access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]],
                    access_global_block_types[local_block->block_type].block_name, local_block->nof_instances,
                    inst_number);
            return -1;
        }
        *inst_num = inst_number;
    }
    else
    {
        *inst_num = ACCESS_ALL_BLOCK_INSTANCES;
    }
    *out_mem_id = rm_info - device_info->local_regs;

    return 0;
}

/** Filter registers for "getreg *" to prevent reading registers where this operation will cause problems. */
static int
diag_dnxc_filter_regs_getreg_all(
    access_runtime_info_t *runtime_info,
    access_local_regmem_id_t regmem_i)
{
    const access_device_type_info_t *device_info = runtime_info->device_type_info; /** constant access information for the device type */
    const access_device_regmem_t *rm_info = device_info->local_regs + regmem_i;

    switch (rm_info->global_regmem_id)
    {
            /*
             * Added for iproc v14, found by testing
             */
        case rIPROCGENRES_TIM0_TIM_TIMERITCR:
        case rIPROCGENRES_TIM1_TIM_TIMERITCR:
        case rIPROCGENRES_TIM2_TIM_TIMERITCR:
        case rIPROCGENRES_TIM3_TIM_TIMERITCR:
        case rIPROCGENRES_TIM0_TIM_TIMERITOP:
        case rIPROCGENRES_TIM1_TIM_TIMERITOP:
        case rIPROCGENRES_TIM2_TIM_TIMERITOP:
        case rIPROCGENRES_TIM3_TIM_TIMERITOP:
        case rIPROCGENRES_TIM0_TIM_TIMER1LOAD:
        case rIPROCGENRES_TIM1_TIM_TIMER1LOAD:
        case rIPROCGENRES_TIM2_TIM_TIMER1LOAD:
        case rIPROCGENRES_TIM3_TIM_TIMER1LOAD:
        case rIPROCGENRES_TIM0_TIM_TIMER2LOAD:
        case rIPROCGENRES_TIM1_TIM_TIMER2LOAD:
        case rIPROCGENRES_TIM2_TIM_TIMER2LOAD:
        case rIPROCGENRES_TIM3_TIM_TIMER2LOAD:
        case rIPROCGENRES_TIM0_TIM_TIMER1MIS:
        case rIPROCGENRES_TIM1_TIM_TIMER1MIS:
        case rIPROCGENRES_TIM2_TIM_TIMER1MIS:
        case rIPROCGENRES_TIM3_TIM_TIMER1MIS:
        case rIPROCGENRES_TIM0_TIM_TIMER2MIS:
        case rIPROCGENRES_TIM1_TIM_TIMER2MIS:
        case rIPROCGENRES_TIM2_TIM_TIMER2MIS:
        case rIPROCGENRES_TIM3_TIM_TIMER2MIS:
        case rIPROCGENRES_TIM0_TIM_TIMER1RIS:
        case rIPROCGENRES_TIM1_TIM_TIMER1RIS:
        case rIPROCGENRES_TIM2_TIM_TIMER1RIS:
        case rIPROCGENRES_TIM3_TIM_TIMER1RIS:
        case rIPROCGENRES_TIM0_TIM_TIMER2RIS:
        case rIPROCGENRES_TIM1_TIM_TIMER2RIS:
        case rIPROCGENRES_TIM2_TIM_TIMER2RIS:
        case rIPROCGENRES_TIM3_TIM_TIMER2RIS:
        case rIPROCGENRES_TIM0_TIM_TIMER1BGLOAD:
        case rIPROCGENRES_TIM1_TIM_TIMER1BGLOAD:
        case rIPROCGENRES_TIM2_TIM_TIMER1BGLOAD:
        case rIPROCGENRES_TIM3_TIM_TIMER1BGLOAD:
        case rIPROCGENRES_TIM0_TIM_TIMER2BGLOAD:
        case rIPROCGENRES_TIM1_TIM_TIMER2BGLOAD:
        case rIPROCGENRES_TIM2_TIM_TIMER2BGLOAD:
        case rIPROCGENRES_TIM3_TIM_TIMER2BGLOAD:
        case rIPROCGENRES_TIM0_TIM_TIMER1CONTROL:
        case rIPROCGENRES_TIM1_TIM_TIMER1CONTROL:
        case rIPROCGENRES_TIM2_TIM_TIMER1CONTROL:
        case rIPROCGENRES_TIM3_TIM_TIMER1CONTROL:
        case rIPROCGENRES_TIM0_TIM_TIMER2CONTROL:
        case rIPROCGENRES_TIM1_TIM_TIMER2CONTROL:
        case rIPROCGENRES_TIM2_TIM_TIMER2CONTROL:
        case rIPROCGENRES_TIM3_TIM_TIMER2CONTROL:
        case rIPROCGENRES_TIM0_TIM_TIMER1INTCLR:
        case rIPROCGENRES_TIM1_TIM_TIMER1INTCLR:
        case rIPROCGENRES_TIM2_TIM_TIMER1INTCLR:
        case rIPROCGENRES_TIM3_TIM_TIMER1INTCLR:
        case rIPROCGENRES_TIM0_TIM_TIMER2INTCLR:
        case rIPROCGENRES_TIM1_TIM_TIMER2INTCLR:
        case rIPROCGENRES_TIM2_TIM_TIMER2INTCLR:
        case rIPROCGENRES_TIM3_TIM_TIMER2INTCLR:
        case rIPROCGENRES_TIM0_TIM_TIMER1VALUE:
        case rIPROCGENRES_TIM1_TIM_TIMER1VALUE:
        case rIPROCGENRES_TIM2_TIM_TIMER1VALUE:
        case rIPROCGENRES_TIM3_TIM_TIMER1VALUE:
        case rIPROCGENRES_TIM0_TIM_TIMER2VALUE:
        case rIPROCGENRES_TIM1_TIM_TIMER2VALUE:
        case rIPROCGENRES_TIM2_TIM_TIMER2VALUE:
        case rIPROCGENRES_TIM3_TIM_TIMER2VALUE:
        case rIPROCGENRES_TIM0_TIM_TIMERPCELLID0:
        case rIPROCGENRES_TIM1_TIM_TIMERPCELLID0:
        case rIPROCGENRES_TIM2_TIM_TIMERPCELLID0:
        case rIPROCGENRES_TIM3_TIM_TIMERPCELLID0:
        case rIPROCGENRES_TIM0_TIM_TIMERPCELLID1:
        case rIPROCGENRES_TIM1_TIM_TIMERPCELLID1:
        case rIPROCGENRES_TIM2_TIM_TIMERPCELLID1:
        case rIPROCGENRES_TIM3_TIM_TIMERPCELLID1:
        case rIPROCGENRES_TIM0_TIM_TIMERPCELLID2:
        case rIPROCGENRES_TIM1_TIM_TIMERPCELLID2:
        case rIPROCGENRES_TIM2_TIM_TIMERPCELLID2:
        case rIPROCGENRES_TIM3_TIM_TIMERPCELLID2:
        case rIPROCGENRES_TIM0_TIM_TIMERPCELLID3:
        case rIPROCGENRES_TIM1_TIM_TIMERPCELLID3:
        case rIPROCGENRES_TIM2_TIM_TIMERPCELLID3:
        case rIPROCGENRES_TIM3_TIM_TIMERPCELLID3:
        case rIPROCGENRES_TIM0_TIM_TIMERPERIPHID0:
        case rIPROCGENRES_TIM1_TIM_TIMERPERIPHID0:
        case rIPROCGENRES_TIM2_TIM_TIMERPERIPHID0:
        case rIPROCGENRES_TIM3_TIM_TIMERPERIPHID0:
        case rIPROCGENRES_TIM0_TIM_TIMERPERIPHID1:
        case rIPROCGENRES_TIM1_TIM_TIMERPERIPHID1:
        case rIPROCGENRES_TIM2_TIM_TIMERPERIPHID1:
        case rIPROCGENRES_TIM3_TIM_TIMERPERIPHID1:
        case rIPROCGENRES_TIM0_TIM_TIMERPERIPHID2:
        case rIPROCGENRES_TIM1_TIM_TIMERPERIPHID2:
        case rIPROCGENRES_TIM2_TIM_TIMERPERIPHID2:
        case rIPROCGENRES_TIM3_TIM_TIMERPERIPHID2:
        case rIPROCGENRES_TIM0_TIM_TIMERPERIPHID3:
        case rIPROCGENRES_TIM1_TIM_TIMERPERIPHID3:
        case rIPROCGENRES_TIM2_TIM_TIMERPERIPHID3:
        case rIPROCGENRES_TIM3_TIM_TIMERPERIPHID3:
        case rIPROCGENRES_WDT0_WDT_WDOGCONTROL:
        case rIPROCGENRES_WDT0_WDT_WDOGINTCLR:
        case rIPROCGENRES_WDT0_WDT_WDOGITCR:
        case rIPROCGENRES_WDT0_WDT_WDOGITOP:
        case rIPROCGENRES_WDT0_WDT_WDOGLOAD:
        case rIPROCGENRES_WDT0_WDT_WDOGLOCK:
        case rIPROCGENRES_WDT0_WDT_WDOGMIS:
        case rIPROCGENRES_WDT0_WDT_WDOGPCELLID0:
        case rIPROCGENRES_WDT0_WDT_WDOGPCELLID1:
        case rIPROCGENRES_WDT0_WDT_WDOGPCELLID2:
        case rIPROCGENRES_WDT0_WDT_WDOGPCELLID3:
        case rIPROCGENRES_WDT0_WDT_WDOGPERIPHID0:
        case rIPROCGENRES_WDT0_WDT_WDOGPERIPHID1:
        case rIPROCGENRES_WDT0_WDT_WDOGPERIPHID2:
        case rIPROCGENRES_WDT0_WDT_WDOGPERIPHID3:
        case rIPROCGENRES_WDT0_WDT_WDOGRIS:
        case rIPROCGENRES_WDT0_WDT_WDOGVALUE:
            return 1;
    }
    return 0;
}

/*
 * Print regmem information.
 * Return the fail of the function or SOC_E_NONE on success.
 * If "hex" is specified (REGMEM_PRINT_ONLY_HEX_VALUES), print only the value of the regmem
 * in hexadecimal format.
 * If "debug" is specified (REGMEM_PRINT_ADDR), print only the schan number,
 * the address and the value of the regmem.
 * If "addr" is specified (DUMP_ADDR_ONLY), print only the block name, block inst number and the address
 * If field_name is specified, print only the fields with the given name.
 * If there is no options specified, print the whole information for the register
 * (name, block, instance, address(for regs), minidx-maxidx(for mems), value and fields information).
 */
static int
access_dnxc_print_regmem(
    access_runtime_info_t *runtime_info,
    uint32 flags,
    access_local_regmem_id_t regmem_i,
    access_block_instance_num_t inst_i,
    unsigned array_index,
    int memory_first_index,
    int memory_last_index,
    char *field_name)
{
    int unit = runtime_info->unit;
    const access_device_type_info_t *device_info = runtime_info->device_type_info;  /** Constant access information for the device type */
    const access_device_regmem_t *rm_info = device_info->local_regs + regmem_i;
    const access_device_block_t *local_block = device_info->blocks + rm_info->local_block_id;
    const access_device_field_t *field_info;
    access_local_field_id_t fld;
    char buf[256], tmp[132];
    uint32 fval[APPL_ACCESS_MAX_MEM_UINT32S] = { 0 }, regmem_value[APPL_ACCESS_MAX_MEM_UINT32S] = { 0 },
     default_value[APPL_ACCESS_MAX_MEM_UINT32S] = { 0 }, nof_uint32s = (rm_info->width_in_bits + 31) / 32;
    int donot_print_regmem_name, first_printed_field, mem_index, i, entry_dw = (rm_info->width_in_bits + 31) / 32;

    SHR_FUNC_INIT_VARS(runtime_info->unit);

    if ((flags & REGMEM_PRINT_CHG) && !(rm_info->flags & ACCESS_REGMEM_FLAG_MEMORY))
    {
        SHR_IF_ERR_EXIT(access_get_local_regmem_default_value(runtime_info, regmem_i, default_value));
    }
    /** Loop over the indices, or execute just once for registers */
    for (mem_index = memory_first_index; mem_index <= memory_last_index; mem_index++)
    {
        SHR_IF_ERR_EXIT(access_local_regmem
                        (runtime_info, (flags & DUMP_DISABLE_CACHE) ? FLAG_ACCESS_DO_NOT_USE_CACHE : 0, regmem_i,
                         inst_i, array_index, mem_index, regmem_value));

        donot_print_regmem_name = (field_name != NULL && (rm_info->flags & ACCESS_REGMEM_FLAG_MEMORY) == 0)
            || ((flags & REGMEM_PRINT_CHG) && SHR_BITEQ_RANGE(regmem_value, default_value, 0, rm_info->width_in_bits));

        first_printed_field = 0;

        if (flags & REGMEM_PRINT_ONLY_HEX_VALUES)
        {
            /** Printing only the value of the regmem in hexadecimal format */
            if (rm_info->flags & ACCESS_REGMEM_FLAG_MEMORY)
            {
                for (i = 0; i < entry_dw; i++)
                {
                    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%08x\n", regmem_value[i]);
                }
            }
            else
            {
                format_long_integer(buf, regmem_value, nof_uint32s);
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%s\n", buf);
            }
        }
        else if (flags & REGMEM_PRINT_ADDR && (rm_info->flags & ACCESS_REGMEM_FLAG_MEMORY) == 0)
        {
            /** Printing only the schan number, the address and the value of the register */
            format_spec_long_integer(buf, regmem_value, nof_uint32s);
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "0x%02X 0x%04X: %s\n",
                        runtime_info->device_type_info->block_instances[inst_i].sbus_info.sbus_block_id,
                        rm_info->base_address + array_index * rm_info->array_skip, buf);
        }
        else
        {
            if (!donot_print_regmem_name)
            {
                if (flags & DUMP_ADDR_ONLY)
                {
                    if ((rm_info->flags & ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK) == ACCESS_REGMEM_FLAG_ACCESS_TYPE_PER_PORT)
                    {
                        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%s",
                                SOC_CONTROL(unit)->info.port_name[inst_i]);
                    }
                    else
                    {
                        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%s%u",
                                access_global_block_types[local_block->block_type].block_name, inst_i);
                    }
                }
                else
                {
                    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%s",
                                access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
                }
                if (rm_info->nof_array_indices != 1)
                {
                    if (rm_info->flags & ACCESS_REGMEM_FLAG_MEMORY)
                    {
                        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "[%u]", array_index);
                    }
                    else
                    {
                        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "(%u)", array_index);
                    }
                }
                if ((flags & DUMP_ADDR_ONLY) == 0)
                {
                    if ((rm_info->flags & ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK) == ACCESS_REGMEM_FLAG_ACCESS_TYPE_PER_PORT)
                    {
                        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ".%s",
                                SOC_CONTROL(unit)->info.port_name[inst_i]);
                    }
                    else
                    {
                        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ".%s%u",
                                access_global_block_types[local_block->block_type].block_name, inst_i);
                    }
                }
                if (rm_info->flags & ACCESS_REGMEM_FLAG_MEMORY)
                {
                    if (flags & DUMP_ADDR_ONLY)
                    {
                        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ".[%d]: 0x%x",
                                    mem_index, rm_info->base_address + mem_index + array_index * rm_info->array_skip);
                    }
                    else
                    {
                        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "[%u]", mem_index);
                        if ((flags & REGMEM_PRINT_CHG) == 0 && flags & REGMEM_PRINT_RAW)
                        {
                            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ": ", mem_index);
                        }
                        if (flags & REGMEM_PRINT_RAW)
                        {
                            for (i = 0; i < entry_dw; i++)
                            {
                                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "0x%08x ",
                                            regmem_value[i]);
                            }
                        }
                    }
                }
                else
                {
                    format_long_integer(buf, regmem_value, (rm_info->width_in_bits + 31) / 32);
                    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "[0x%x]=%s",
                                rm_info->base_address + array_index * rm_info->array_skip, buf);
                }
            }
            if (flags & REGMEM_PRINT_FIELDS && (flags & REGMEM_PRINT_RAW) == 0 && (flags & DUMP_ADDR_ONLY) == 0)
            {
                /** Printing the field content */
                if (!donot_print_regmem_name)
                {
                    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ": <");
                }
                if ((flags & REGMEM_PRINT_CHG) && !(rm_info->flags & ACCESS_REGMEM_FLAG_MEMORY))
                {
                    SHR_IF_ERR_EXIT(access_get_local_regmem_default_value(runtime_info, regmem_i, default_value));
                }
                for (fld = rm_info->fields_index; fld < rm_info->fields_index + rm_info->nof_fields; fld++)
                {
                    /** Field to access */
                    field_info = device_info->local_fields + fld;

                    if (field_name != NULL
                        && sal_strncasecmp(field_name,
                                          access_global_names[global_field_name_indices[field_info->global_field_id]],
                                          SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))
                    {
                        continue;
                    }
                    if (flags & REGMEM_PRINT_CHG)
                    {
                        /** Compare the default and the regmem values */
                        if (SHR_BITEQ_RANGE
                            (regmem_value, default_value, field_info->start_bit, field_info->size_in_bits))
                        {
                            continue;
                        }
                    }
                    if (first_printed_field == 0)
                    {
                        first_printed_field = 1;
                    }
                    else
                    {
                        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ", ", tmp);
                    }
                    SHR_IF_ERR_EXIT(access_local_field_get(runtime_info, fld, regmem_value, fval));

                    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%s",
                                access_global_names[global_field_name_indices[field_info->global_field_id]]);

                    format_long_integer(tmp, fval, (field_info->size_in_bits + 31) / 32);
                    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "=%s", tmp);
                }
                if (!donot_print_regmem_name)
                {
                    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ">");
                }
            }
            if (!donot_print_regmem_name || field_name != NULL)
            {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "\n");
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * Print a one line summary for all regmems whose name matches substr_match
 * (if not NULL) and match flags_to_be_set and flags_to_be_clear
 * If substr_match is NULL, match all regmems.
 * If substr_match is not NULL, match any regmems whose name
 * or user-friendly name contains that substring.
 */
static void
access_dnxc_regmem_list_summary(
    access_runtime_info_t *runtime_info,
    const char *substr_match,
    int print_block_names,
    uint32 flags_to_be_set,
    uint32 flags_to_be_clear,
    int is_mem)
{
    int unit = runtime_info->unit;
    const access_device_type_info_t *device_info = runtime_info->device_type_info;  /** Constant access information for the device type */
    const access_device_regmem_t *rm_info;
    const access_device_block_t *local_block = NULL;
    int nof_instances, dlen, found = 0;
    char *dstr;
    access_local_regmem_id_t regmem_i;

    if (is_mem)
    {
        /** Loop over all memories */
        for (rm_info = device_info->local_mems, regmem_i = device_info->nof_mems; regmem_i; regmem_i--, rm_info++)
        {
            /** Filter by flags */
            if ((rm_info->flags & flags_to_be_set) != flags_to_be_set)
            {
                continue;
            }
            if ((rm_info->flags & flags_to_be_clear) != 0)
            {
                continue;
            }
            
            if (substr_match != NULL
                && strcaseindex(access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]],
                                substr_match) == NULL)
            {
                continue;
            }
            if (local_block != device_info->blocks + rm_info->local_block_id)
            {
                local_block = device_info->blocks + rm_info->local_block_id;
                if (print_block_names)
                {
                    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "memories of block %s:\n",
                                access_global_block_types[local_block->block_type].block_name);
                }
            }

            nof_instances = local_block->nof_instances;

            dlen = sal_strnlen(rm_info->description, 39);
            if (dlen > 38)
            {
                dlen = 34;
                dstr = "...";
            }
            else
            {
                dstr = "";
            }
            if (!found)
            {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                            " %-6s  %-22s%5s/%-4s %s\n", "Flags", "Name", "Entry", "Copy", "Description");
                found = 1;
            }

            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                        " %c%c%c%c%c%c  %-22s %5d",
                        (rm_info->flags & ACCESS_REGMEM_FLAG_RO) ? 'r' : '-',
                        (rm_info->flags & ACCESS_REGMEM_FLAG_WO) ? 'w' : '-',
                        's',
                        (rm_info->flags & ACCESS_REGMEM_FLAG_COUNTER) ? 'c' : '-',
                        (rm_info->flags & ACCESS_REGMEM_FLAG_IS_ALIAS) ? 'A' : '-',
                        (rm_info->flags & ACCESS_REGMEM_FLAG_ACCESS_TYPE_WIDE) ? 'W' : '-',
                        access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]],
                        rm_info->u.mem.mem_nof_elements);
            if (nof_instances == 1)
            {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%5s %*.*s%s\n",
                            "", dlen, dlen, rm_info->description, dstr);
            }
            else
            {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "/%-4d %*.*s%s\n",
                            nof_instances, dlen, dlen, rm_info->description, dstr);
            }
        }
    }
    else
    {
        /** Loop over all registers */
        for (regmem_i = 0; regmem_i < device_info->nof_regs; regmem_i++)
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
            
            if (substr_match != NULL
                && strcaseindex(access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]],
                                substr_match) == NULL)
            {
                continue;
            }
            if (local_block != device_info->blocks + rm_info->local_block_id)
            {
                local_block = device_info->blocks + rm_info->local_block_id;
                if (print_block_names)
                {
                    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "registers of block %s:\n",
                                access_global_block_types[local_block->block_type].block_name);
                }
            }

            nof_instances = local_block->nof_instances;

            dlen = sal_strnlen(rm_info->description, 39);
            if (dlen > 38)
            {
                dlen = 34;
                dstr = "...";
            }
            else
            {
                dstr = "";
            }
            if (!found)
            {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                            " %-6s  %-22s%5s/%-4s %s\n", "Flags", "Name", "Entry", "Copy", "Description");
                found = 1;
            }

            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                        " %c%c%c%c  %-22s %5d",
                        (rm_info->flags & ACCESS_REGMEM_FLAG_RO) ? 'r' : '-',
                        (rm_info->flags & ACCESS_REGMEM_FLAG_WO) ? 'w' : '-',
                        's',
                        (rm_info->flags & ACCESS_REGMEM_FLAG_COUNTER) ? 'c' : '-',
                        access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]],
                        rm_info->nof_array_indices);
            if (nof_instances == 1)
            {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%5s %*.*s%s\n",
                            "", dlen, dlen, rm_info->description, dstr);
            }
            else
            {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "/%-4d %*.*s%s\n",
                            nof_instances, dlen, dlen, rm_info->description, dstr);
            }
        }
    }

    if (found)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                    "Flags: (r)eadonly, (w)riteonly, (s)orted, (c)ounter\n" "       A(lias), (W)ide\n");
    }
    else
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "No regmem found with the substring '%s' in its name.\n"), substr_match));
    }
    bsl_log_end(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "");
}

/*
 * Print all memories which are marked cache-able,
 */
static int
access_dnxc_regmem_list_table_cache(
    access_runtime_info_t *runtime_info)
{
    const access_device_type_info_t *device_info = runtime_info->device_type_info;  /** Constant access information for the device type */
    const access_device_regmem_t *rm_info;
    int unit = runtime_info->unit;
    access_local_regmem_id_t mem_index;
    const access_regmem_state_t *regmem_state = runtime_info->regmem_state;

    if (regmem_state == NULL)
    {
        return _SHR_E_UNAVAIL;
    }

    regmem_state = &runtime_info->regmem_state[device_info->nof_regs];

    /** Loop over  regmem */
    for (mem_index = 0; mem_index < device_info->nof_mems; ++mem_index, ++regmem_state)
    {
        if (regmem_state->flags & ACCESS_REGMEM_STATE_FLAG_CACHEABLE)
        {
            rm_info = device_info->local_mems + mem_index;
            /*
             * regmem cache-able 
             */
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "\n%s",
                        access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
        }

    }
    bsl_log_end(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "");

    return _SHR_E_NONE;
}

/** Print information for the given regmem, including its field */
static soc_error_t
access_diag_dnxc_list_regmem(
    access_runtime_info_t *runtime_info,
    const access_local_regmem_id_t regmem,
    args_t *a,
    char *entry_value_str)
{
    int unit = runtime_info->unit;
    const access_device_type_info_t *device_info = runtime_info->device_type_info;  /** Constant access information for the device type */
    const access_device_regmem_t *rm_info = device_info->local_regs + regmem;
    const access_device_block_t *local_block = NULL;
    const access_device_regmem_t *rm_info_alias;
    access_local_regmem_id_t regmem_alias;      /** The register or memory to access */
    const access_device_field_t *field_info;
    uint32 entry[APPL_ACCESS_MAX_MEM_UINT32S] = { 0 }, flags = rm_info->flags;
    int have_entry, i, disabled, maxidx, dw = (rm_info->width_in_bits + 31) / 32;
    access_local_block_id_t block_i;
    access_local_field_id_t fld;
    const uint8 *block_instances_flags;
    const access_regmem_state_t *regmem_state = runtime_info->regmem_state + regmem;

    SHR_FUNC_INIT_VARS(unit);

    if (entry_value_str == NULL)
    {
        have_entry = 0;
    }
    else
    {
        for (i = 0; i < dw; i++)
        {
            if (dw > APPL_ACCESS_MAX_MEM_UINT32S)
            {
                SHR_EXIT_WITH_LOG(SOC_E_PARAM, "The data %d is outside max memory entry/word %s%s\n", dw, EMPTY, EMPTY);
            }
            if (entry_value_str == 0)
            {
                SHR_EXIT_WITH_LOG(SOC_E_PARAM, "Not enough data specified (%d words needed) %s%s\n", dw, EMPTY, EMPTY);
            }
            entry[i] = parse_integer(entry_value_str);
            entry_value_str = ARG_GET(a);
        }
        if (entry_value_str)
        {
            LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Extra data specified (ignored)\n")));
        }
        have_entry = 1;
    }
    local_block = device_info->blocks + rm_info->local_block_id;
    if (flags & ACCESS_REGMEM_FLAG_MEMORY)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Memory: %s.%s",
                    access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]],
                    access_global_block_types[local_block->block_type].block_name);
        if ((flags & ACCESS_REGMEM_FLAG_IS_ALIAS) != 0)
        {
            regmem_alias = rm_info->u.mem.alias_regmem;
            rm_info_alias = device_info->local_regs + regmem_alias;
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " alias to %s \n",
                        access_global_names[rm_info_alias->global_regmem_id]);
        }
    }
    else
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Register: %s.%s",
                    access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]],
                    access_global_block_types[local_block->block_type].block_name);
    }

    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " address 0x%08x\n", rm_info->base_address);

    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Flags:");
    if ((flags & ACCESS_REGMEM_FLAG_NO_ACCESS) == ACCESS_REGMEM_FLAG_NO_ACCESS)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " no-CPU-access");
    }
    else
    {
        if (flags & ACCESS_REGMEM_FLAG_RO)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " read-only");
        }
        if (flags & ACCESS_REGMEM_FLAG_WO)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " write-only");
        }
    }
    if (flags & ACCESS_REGMEM_FLAG_ASIC_CHANGABLE)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " signal/dynamic");
    }
    if (flags & ACCESS_REGMEM_FLAG_INIT_ON_READ)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " init-on-read");
    }
    if (flags & ACCESS_REGMEM_FLAG_CLEAR_BITS_ON_WRITE)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " clear-bits-on-write");
    }
    if (flags & ACCESS_REGMEM_FLAG_INTERRUPT)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " interrupt-register");
    }
    if (flags & ACCESS_REGMEM_FLAG_COUNTER)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " counter");
    }
    if (flags & ACCESS_REGMEM_FLAG_IS_ALIAS)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " alias");
    }
    switch (flags & ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK)
    {
        case ACCESS_REGMEM_FLAG_ACCESS_TYPE_1ST_BAR:
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " type-1st-bar");
            break;
        case ACCESS_REGMEM_FLAG_ACCESS_TYPE_2ND_BAR:
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " type-2nd-bar");
            break;
        case ACCESS_REGMEM_FLAG_ACCESS_TYPE_WIDE:
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " wide");
            break;
        case ACCESS_REGMEM_FLAG_ACCESS_TYPE_PER_PORT:
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " access-per-port");
            break;
    }
    switch (flags & ACCESS_REGMEM_FLAG_CONTENT_PROTECTION_MASK)
    {
        case ACCESS_REGMEM_FLAG_HAS_NO_PROTECTION:
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " no-protection");
            break;
        case ACCESS_REGMEM_FLAG_HAS_ECC:
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " has-ecc");
            break;
        case ACCESS_REGMEM_FLAG_HAS_PARITY:
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " has-parity");
            break;
    }
    if (rm_info->nof_array_indices != 1)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "  array[%u-%u]",
                    rm_info->first_array_index, rm_info->first_array_index + rm_info->nof_array_indices - 1);
    }
    if (runtime_info->regmem_state && regmem_state->flags & ACCESS_REGMEM_STATE_FLAG_CACHED)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " cached");
    }

    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "\n");

    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Blocks: ");
    disabled = 0;
    local_block = device_info->blocks + rm_info->local_block_id;
    /** Check if a block instance is enabled */
    block_instances_flags = runtime_info->block_instance_flags + local_block->instances_start_index;
    for (block_i = 0; block_i < local_block->nof_instances; block_i++)
    {
        if ((block_instances_flags[block_i] & ACCESS_RUNTIME_BLOCK_INSTANCE_FLAG_DISABLED) == 0)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " %s%u",
                        access_global_block_types[local_block->block_type].block_name, block_i);
        }
        else
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " [%s]",
                        access_global_block_types[local_block->block_type].block_name);
            disabled += 1;
        }
    }
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " (%u instance%s", local_block->nof_instances,
                local_block->nof_instances == 1 ? "" : "s");
    if (disabled)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ", %d instance%s disabled", disabled,
                    disabled == 1 ? "" : "s");
    }
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ")\n");

    if (flags & ACCESS_REGMEM_FLAG_MEMORY)
    {
        maxidx = rm_info->u.mem.mem_nof_elements;
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Entries: %u with indices 0-%d (0x0-0x%x)",
                    rm_info->u.mem.mem_nof_elements, maxidx - 1, maxidx - 1);
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ", each %d bytes %d words\n",
                    (rm_info->width_in_bits + 7) / 8, dw);
    }
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Width in bits: %d\n", rm_info->width_in_bits);

    if (rm_info->description)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Description: %s\n", rm_info->description);
    }

    for (fld = rm_info->fields_index; fld < rm_info->fields_index + rm_info->nof_fields; fld++)
    {
        /** Field to access */
        field_info = device_info->local_fields + fld;
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "  %s<%u",
                    access_global_names[global_field_name_indices[field_info->global_field_id]],
                    field_info->start_bit + field_info->size_in_bits - 1);
        if (field_info->size_in_bits > 1)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ":%u", field_info->start_bit);
        }
        if (have_entry)
        {
            uint32 fval[APPL_ACCESS_MAX_MEM_UINT32S] = { 0 };
            char tmp[132];

            SHR_IF_ERR_EXIT(access_local_field_get(runtime_info, fld, entry, fval));
            format_long_integer(tmp, fval, (field_info->size_in_bits + 31) / 32);
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "> = %s\n", tmp);
        }
        else
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ">\n");
        }
    }
    bsl_log_end(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "");
exit:
    SHR_FUNC_EXIT;
}

/*
 * List memory information, or fields of a memory entry
 */
char cmd_dnxc_listmem_usage[] =
    "LISTmem [File=<name>] [Append=true|false]\n\t"
    "Parameters: <MEMORY> [<DATA> ...]\n\t"
    "Parameters: [flags] [<full or partial string of memories to list>]\n\t"
    "If <MEMORY> is '*', displays a reference list of all\n\t"
    "memories and their attributes.\n\t"
    "If MEMORY is given, displays the entry fields for that memories.\n\t"
    "Flags:\n\t"
    "   cache-table will cause only cached memories to be listed.\n\t"
    "   dynamic-table will cause only dynamic memories to be listed.\n\t"
    "   read-write will cause only memories supporting read and write to be listed.\n\t"
    "   init-on-read will cause only memories supporting init on read to be listed.\n\t"
    "   clear-bits-on-write will cause only memories supporting clear bits on write to be listed.\n\t"
    "   counter will cause only counter memories to be listed.\n\t"
    "   alias will cause only alias memories to be listed.\n\t"
    "   type-1st-bar will cause only type-1st-bar memories to be listed.\n\t"
    "   type-2nd-bar will cause only type-2nd-bar memories to be listed.\n\t"
    "   wide will cause only wide memories to be listed.\n\t"
    "   no-protection will cause only no-protection memories to be listed.\n\t"
    "   has-ecc will cause only memories with ecc to be listed.\n\t"
    "   has-parity will cause only memories with parity to be listed.\n\t"
    "   print-blocks will cause the block name to be printed before listing memories from the block.\n\t"
    "If DATA is given, decodes the data into entry fields.\n";

/** Implementation of the listmem shell command, listing memories */
cmd_result_t
access_cmd_dnxc_listmem(
    int unit,
    args_t *a)
{
    access_local_regmem_id_t regmem_i;
    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); /** Access information for the device */
    access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
    int rv = CMD_FAIL, append = FALSE, print_block_names = 0, parse_memory_name_rv;
    uint32 flags_to_be_set = 0, flags_to_be_clear = 0;
    int console_was_on = 0, console_disabled = 0, pushed_ctrl_c = 0;
    parse_table_t pt;
    jmp_buf ctrl_c;
    char *fname = "", *tab = NULL, *mem_name = NULL, *data = NULL;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "File", PQ_STRING, 0, &fname, 0);
    parse_table_add(&pt, "Append", PQ_BOOL, 0, &append, FALSE);

    if (!sh_check_attached(ARG_CMD(a), unit))
    {
        rv = CMD_FAIL;
        goto done;
    }
    if (!soc_property_get(unit, spn_MEMLIST_ENABLE, 1))
    {
        rv = CMD_FAIL;
        goto done;
    }
    if (parse_arg_eq(a, &pt) < 0)
    {
        rv = CMD_USAGE;
        goto done;
    }

    console_was_on = bslcons_is_enabled();

    if (fname[0] != 0)
    {
        /*
         * Catch control-C in case if using file output option. 
         */
#ifndef NO_CTRL_C
        if (setjmp(ctrl_c))
        {
            rv = CMD_INTR;
            goto done;
        }
#endif
        sh_push_ctrl_c(&ctrl_c);
        pushed_ctrl_c = TRUE;

        if (bslfile_is_enabled())
        {
            LOG_INFO(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit, "%s: Can't dump to file while logging is enabled\n"), ARG_CMD(a)));
            rv = CMD_FAIL;
            goto done;
        }

        if (bslfile_open((char *) fname, append) < 0)
        {
            LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "%s: Could not start log file\n"), ARG_CMD(a)));
            rv = CMD_FAIL;
            goto done;
        }

        bslcons_enable(FALSE);
        console_disabled = 1;
    }

    bsl_log_start(BSL_APPL_SHELL, bslSeverityNormal, unit, "");

    for (tab = ARG_GET(a); tab != NULL; tab = ARG_GET(a))
    {
        
        /*
         * add logic for selecting all cache-tables 
         * For the new access there is an update - we list not only memory names, but flags also
         * Can see "dynamic-table" listing
         */
        if (sal_strncasecmp(tab, "cache-table", sizeof("cache-table")) == 0)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current enabled cached memories: \n");
            if (access_dnxc_regmem_list_table_cache(runtime_info))
            {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                            "This cache does not support on this device.\n");
                return CMD_FAIL;
            }
            return CMD_OK;
        }

        if (sal_strncasecmp(tab, "dynamic-table", sizeof("dynamic-table")) == 0)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current dynamic memories: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_ASIC_CHANGABLE;
        }
        else if (sal_strncasecmp(tab, "read-write", sizeof("read-write")) == 0)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current read and write memories: \n");
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_RO | ACCESS_REGMEM_FLAG_WO;
        }
        else if (sal_strncasecmp(tab, "init-on-read", sizeof("init-on-read")) == 0)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current init on read memories: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_INIT_ON_READ;
        }
        else if (sal_strncasecmp(tab, "clear-bits-on-write", sizeof("clear-bits-on-write")) == 0)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                        "Current clear bits on write memories: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_CLEAR_BITS_ON_WRITE;
        }
        else if (sal_strncasecmp(tab, "counter", sizeof("counter")) == 0)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current counter memories: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_COUNTER;
        }
        else if (sal_strncasecmp(tab, "alias", sizeof("alias")) == 0)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current alias memories: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_IS_ALIAS;
        }
        else if (sal_strncasecmp(tab, "type-1st-bar", sizeof("type-1st-bar")) == 0)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                        "Current memories with access type 1st-bar: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_1ST_BAR;
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK & ~ACCESS_REGMEM_FLAG_ACCESS_TYPE_1ST_BAR;
        }
        else if (sal_strncasecmp(tab, "type-2nd-bar", sizeof("type-2nd-bar")) == 0)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                        "Current memories with access type 2nd-bar: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_2ND_BAR;
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK & ~ACCESS_REGMEM_FLAG_ACCESS_TYPE_2ND_BAR;
        }
        else if (sal_strncasecmp(tab, "wide", sizeof("wide")) == 0)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current wide memories: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_WIDE;
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK & ~ACCESS_REGMEM_FLAG_ACCESS_TYPE_WIDE;
        }
        else if (sal_strncasecmp(tab, "no-protection", sizeof("no-protection")) == 0)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current no protected memories: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_HAS_NO_PROTECTION;
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_CONTENT_PROTECTION_MASK & ~ACCESS_REGMEM_FLAG_HAS_NO_PROTECTION;
        }
        else if (sal_strncasecmp(tab, "has-ecc", sizeof("has-ecc")) == 0)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current memories with ecc: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_HAS_ECC;
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_CONTENT_PROTECTION_MASK & ~ACCESS_REGMEM_FLAG_HAS_ECC;
        }
        else if (sal_strncasecmp(tab, "has-parity", sizeof("has-parity")) == 0)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current memories with parity: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_HAS_PARITY;
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_CONTENT_PROTECTION_MASK & ~ACCESS_REGMEM_FLAG_HAS_PARITY;
        }
        else if (sal_strncasecmp(tab, "print-blocks", sizeof("print-blocks")) == 0)
        {
            print_block_names = 1;
        }
        else
        {
            mem_name = tab;
            tab = ARG_GET(a);
            if (tab != NULL)
            {
                cli_out("Received DATA after memory name %s\n", mem_name);
                data = tab;
                break;
            }
        }
    }

    if (!mem_name)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Must specify memory name or * for printing all appropriate memories.\n");
        rv = CMD_USAGE;
        goto done;
    }

    if (!sal_strncmp("*", mem_name, sizeof("*")))
    {
        access_dnxc_regmem_list_summary(runtime_info, NULL, print_block_names, flags_to_be_set, flags_to_be_clear, 1);
        rv = CMD_OK;
        goto done;
    }

    parse_memory_name_rv = access_parse_memory_name(runtime_info, mem_name, 0, &inst_i, &regmem_i);
    /** If access_parse_memory_name returns -1, there is an error/failure from the user input */
    if (parse_memory_name_rv < 0)
    {
        rv = CMD_FAIL;
        goto done;
    }
    /** If access_parse_memory_name returns 1, there is no exact match of memory name */
    if (parse_memory_name_rv == 1)
    {
        /** Assume we have a partial name of a memory */
        cli_out("Unknown memory %s: possible matches are:\n", mem_name);
        access_dnxc_regmem_list_summary(runtime_info, mem_name, print_block_names, flags_to_be_set, flags_to_be_clear,
                                        1);
        rv = CMD_OK;
        goto done;
    }

    /** If access_parse_memory_name returns 0, the memory is found */
    /** Memory to access */
    rv = access_diag_dnxc_list_regmem(runtime_info, regmem_i, a, data);

done:

    if (fname[0] != 0)
    {
        bslfile_close();
    }

    if (console_disabled && console_was_on)
    {
        bslcons_enable(TRUE);
    }

    if (pushed_ctrl_c)
    {
        sh_pop_ctrl_c();
    }

    parse_arg_eq_done(&pt);
    return rv;
}

/*
 * Find a register whose name is reg_name.
 * Return local regmem ID, array index and block instance number to access.
 * On success, stores the local regmem ID at out_reg_id, array index at array_index and instance number at inst_num and returns 0.
 * If the array index is not specified in the input string, -1 will be store in array_index.
 * If the instance number is not specified in the input string, ACCESS_ALL_BLOCK_INSTANCES will be stored in inst_num.
 * If the register name was not found, returns 1.
 * On error/failure, returns -1.
 *
 * For getreg and setreg commands a register array index may be added after the register name in square brackets like: reg_name[3].
 * The function will fail for register arrays for which an array index
 * is not specified when array_index is not NULL.
 * The function will fail for non array registers for which an array index is specified.
 * For non array registers, array_index may be NULL; If not Null -1 will be returned in it.
 */
int
access_parse_register_name(
    access_runtime_info_t *runtime_info,
    char *reg_name,
    uint16 *array_index,
    access_block_instance_num_t *inst_num,
    access_local_regmem_id_t *out_reg_id)
{
    int unit = runtime_info->unit;
    const access_device_type_info_t *device_info = runtime_info->device_type_info; /** constant access information for the device type */
    access_local_regmem_id_t reg_i = -1;
    const access_device_regmem_t *rm_info = NULL;
    const access_device_block_t *local_block;
    char *close_bracket, /** the close bracket for the array index in reg_name */
       *open_bracket,   /** the open bracket for the array index in reg_name */
        tmp[80], *block_name = NULL, *port_name = NULL, *inst_str;
    unsigned arr_ind, match = 0;
    uint32 inst_number;
    bcm_gport_t port;
    bcm_port_config_t port_config;

    if (reg_name == NULL || *reg_name == '\0')
    {
        cli_out("ERROR: must specify register name\n");
        return -1;
    }

    /** Do not destroy input string */
    sal_strncpy_s(tmp, reg_name, sizeof(tmp));

    /** check if an array index is specified */
    if ((((open_bracket = sal_strnchr(tmp, '[', sizeof(tmp))) != NULL) &&
         ((close_bracket = sal_strnchr(open_bracket + 1, ']', sizeof(tmp))) != NULL) && (close_bracket - open_bracket > 1)) ||
        (((open_bracket = sal_strnchr(tmp, '(', sizeof(tmp))) != NULL) &&
         ((close_bracket = sal_strnchr(open_bracket + 1,  ')', sizeof(tmp))) != NULL) && (close_bracket - open_bracket > 1)))
    {
        if (array_index == NULL)
        {
            cli_out("ERROR: missed array index number\n");
            return -1;
        }
        /** handle register array, handle initial spaces after '[' */
        for (close_bracket = open_bracket + 1; *close_bracket == ' '; ++close_bracket)
        {
        }
        if (*close_bracket < '0' || *close_bracket > '9')
        {
            cli_out("ERROR: bad array index character\n");
            return -1;
        }
        /** read array index */
        arr_ind = 0;
        for (; *close_bracket >= '0' && *close_bracket <= '9'; ++close_bracket)
        {
            arr_ind = arr_ind * 10 + (*close_bracket - '0');
        }
        /** handle trailing spaces before ']' */
        for (; *close_bracket == ' '; ++close_bracket)
        {
        }
        if ((*close_bracket != ']') && (*close_bracket != ')'))
        { /** verify closing ']' */
            cli_out("ERROR: bad array index character\n");
            return -1;
        }
        *array_index = arr_ind;
        close_bracket = sal_strnchr(close_bracket + 1, '.', sizeof(tmp));
        *open_bracket = '\0';
    }
    else
    {
        arr_ind = 0xffffffff;
        close_bracket = sal_strnchr(tmp, '.', sizeof(tmp));
        if (array_index != NULL)
        {
            *array_index = -1;
        }
    }
    if (close_bracket != NULL)
    {
        *close_bracket++ = '\0';
    }

    /** loop over all registers */
    for (reg_i = 0; reg_i < device_info->nof_regs; reg_i++)
    {
        /** register to access */
        rm_info = device_info->local_regs + reg_i;
        /** find by name */
        if (sal_strncasecmp
            (tmp, access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]], sizeof(tmp)) == 0)
        {
            break;
        }
    }
    if (reg_i == device_info->nof_regs)
    { /** the specified register is not found */
        return 1;
    }

    /** register array related validation */
    if (arr_ind != 0xffffffff)
    {
        /** an array index was specified, verify that this is a register array and that the index is in range */
        if (rm_info->nof_array_indices == 1 && arr_ind != 0)
        {
            cli_out("ERROR: an array index was specified for a register which is not an array\n");
            return -1;
        }
        else if (arr_ind >= rm_info->first_array_index + rm_info->nof_array_indices
                 || arr_ind < rm_info->first_array_index)
        {
            cli_out("ERROR: array index %u is out of range, should be in the range %u-%u\n ",
                    arr_ind, rm_info->first_array_index, rm_info->first_array_index + rm_info->nof_array_indices - 1);
            return -1;
        }
    }
    /** Check for instance number in the input string */
    if (close_bracket)
    {
        if ((rm_info->flags & ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK) == ACCESS_REGMEM_FLAG_ACCESS_TYPE_PER_PORT)
        {
            if (bcm_port_config_get(unit, &port_config) < 0) {
                cli_out("Error: Could not get port config for unit %d\n", unit);
                return -1;
            }
            BCM_PBMP_ITER(port_config.nif, port)
            {
                uint32 phy_port = 0;
                int block_inst_id = -1;
                access_local_block_id_t local_block_id;
                phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                block_inst_id = device_info->port_info[phy_port].blk; /* get the local block instance ID for phy_port */
                if (block_inst_id >= 0 && block_inst_id < device_info->nof_block_instances) { /* if found valid port_info data */
                    local_block_id = device_info->block_instances[block_inst_id].block_id;
                    if (rm_info->local_block_id == local_block_id || (rm_info->local_block_id == device_info->blocks_global2local_map[BLOCK_CDMAC] && local_block_id == device_info->blocks_global2local_map[BLOCK_CDPORT]) || (rm_info->local_block_id == device_info->blocks_global2local_map[BLOCK_CLMAC] && local_block_id == device_info->blocks_global2local_map[BLOCK_CLPORT]) || (rm_info->local_block_id == device_info->blocks_global2local_map[BLOCK_DC3MAC] && local_block_id == device_info->blocks_global2local_map[BLOCK_DC3PORT])) /* may need another blocktypes for future devices */
                    {
                        port_name = SOC_CONTROL(unit)->info.port_name[port];
                        if (sal_strcasecmp(port_name, close_bracket) == 0)
                        {
                            match = 1;
                            break;
                        }  
                    }
                }
            }
            if (!match)
            {
                /** The specified port is not valid */
                cli_out("\"%s\" is not a port of register %s\n", close_bracket,
                        access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
                return -1;
            }
            *inst_num = port;
        }
        else
        {
            local_block = device_info->blocks + rm_info->local_block_id;
            block_name = access_global_block_types[local_block->block_type].block_name;
            for (inst_str = close_bracket; sal_tolower(*inst_str) == sal_tolower(*block_name); block_name++, inst_str++)
            {
                if (*inst_str == '\0')
                {
                    cli_out("ERROR: must specify an instance number\n");
                    return -1;
                }
            }
            if (*inst_str == '\0' || *block_name != '\0')
            {
                /** Reached the end of the block instance name or the whole block name did not matched */
                cli_out("\"%s\" is not a block instance of register %s\n", close_bracket,
                        access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
                return -1;
            }
            if (sal_isdigit(*inst_str))
            {
                inst_number = parse_integer(inst_str);
            }
            else
            {
                cli_out("ERROR: unrecognized block instance %s\n", close_bracket);
                return -1;
            }
            /** Check that the inst number is not out of range*/
            if (inst_number >= local_block->nof_instances)
            {
                cli_out("Register %s of block %s has %u instances, but given instance %u\n",
                        access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]],
                        access_global_block_types[local_block->block_type].block_name, local_block->nof_instances,
                        inst_number);
                return -1;
            }
            *inst_num = inst_number;
        }
    }
    else
    {
        *inst_num = ACCESS_ALL_BLOCK_INSTANCES;
    }
    *out_reg_id = reg_i;

    return 0;
}

/**
 * List register information, or fields of a register entry
 */
char cmd_dnxc_listreg_usage[] =
    "Listreg [File=<name>] [Append=true|false]\n\t"
    "Usage: listreg [flags] regname [value]\n\t"
    "Flags:\n\t"
    "   read-write/r/w       display read-write\n\t"
    "   init-on-read         display init on read\n\t"
    "   clear-bits-on-write  display clear bits on write\n\t"
    "   counters/c           display counters\n\t"
    "   type-1st-bar         display type-1st-bar\n\t"
    "   type-2nd-bar         display type-2nd-bar\n\t"
    "   wide                 display wide\n\t"
    "   access-per-port      display accessed per port\n\t"
    "   no-protection        display no protection\n\t"
    "   has-ecc              display has ecc\n\t"
    "   has-parity           display has parity\n\t"
    "   print-blocks         display block name before listing registers from the block\n\t"
    "If reg name is '*', display all registers\n\t"
    "If reg name does not match a register name, a substring\n" "match is performed.  [] indicates register array.\n";

/** Implementation of the listreg shell command, listing registers */
cmd_result_t
access_cmd_dnxc_listreg(
    int unit,
    args_t *a)
{
    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); /** Access information for the device */
    access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
    char *fname = "", *tab = NULL, *reg_name = NULL, *value = NULL;
    access_local_regmem_id_t regmem_i;
    uint32 flags_to_be_set = 0, flags_to_be_clear = 0;
    int console_was_on = 0, console_disabled = 0, pushed_ctrl_c = 0;
    parse_table_t pt;
    jmp_buf ctrl_c;
    int rv = CMD_FAIL, append = FALSE, print_block_names = 0, parse_register_name_rv;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "File", PQ_STRING, 0, &fname, 0);
    parse_table_add(&pt, "Append", PQ_BOOL, 0, &append, FALSE);

    if (!sh_check_attached(ARG_CMD(a), unit))
    {
        rv = CMD_FAIL;
        goto done;
    }
    if (!soc_property_get(unit, spn_REGLIST_ENABLE, 1))
    {
        rv = CMD_FAIL;
        goto done;
    }
    if (parse_arg_eq(a, &pt) < 0)
    {
        rv = CMD_USAGE;
        goto done;
    }

    console_was_on = bslcons_is_enabled();

    if (fname[0] != 0)
    {
        /*
         * Catch control-C in case if using file output option. 
         */
#ifndef NO_CTRL_C
        if (setjmp(ctrl_c))
        {
            rv = CMD_INTR;
            goto done;
        }
#endif
        sh_push_ctrl_c(&ctrl_c);
        pushed_ctrl_c = TRUE;

        if (bslfile_is_enabled())
        {
            LOG_INFO(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit, "%s: Can't dump to file while logging is enabled\n"), ARG_CMD(a)));
            rv = CMD_FAIL;
            goto done;
        }

        if (bslfile_open((char *) fname, append) < 0)
        {
            LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "%s: Could not start log file\n"), ARG_CMD(a)));
            rv = CMD_FAIL;
            goto done;
        }

        bslcons_enable(FALSE);
        console_disabled = 1;
    }

    bsl_log_start(BSL_APPL_SHELL, bslSeverityNormal, unit, "");

    /** Parse options */
    for (tab = ARG_GET(a); tab != NULL; tab = ARG_GET(a))
    {
        if (sal_strncasecmp(tab, "read-write", sizeof("read-write")) == 0 || sal_strncasecmp(tab, "r/w", sizeof("r/w")) == 0)
        {       /** list read-write */
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current read and write registers: \n");
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_RO | ACCESS_REGMEM_FLAG_WO;
        }
        else if (sal_strncasecmp(tab, "init-on-read", sizeof("init-on-read")) == 0)
        {       /** list init-on-read */
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current init on read registers: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_INIT_ON_READ;
        }
        else if (sal_strncasecmp(tab, "clear-bits-on-write", sizeof("clear-bits-on-write")) == 0)
        {       /** list clear-bits-on-write */
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                        "Current clear bits on write registers: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_CLEAR_BITS_ON_WRITE;
        }
        else if (sal_strncasecmp(tab, "counter", sizeof("counter")) == 0 || sal_strncasecmp(tab, "c", sizeof("c")) == 0)
        {       /** list counter */
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current counter registers: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_COUNTER;
        }
        else if (sal_strncasecmp(tab, "alias", sizeof("alias")) == 0 || sal_strncasecmp(tab, "a", sizeof("a")) == 0)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current alias memories: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_IS_ALIAS;
        }
        else if (sal_strncasecmp(tab, "type-1st-bar", sizeof("type-1st-bar")) == 0)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                        "Current memories with access type 1st-bar: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_1ST_BAR;
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK & ~ACCESS_REGMEM_FLAG_ACCESS_TYPE_1ST_BAR;
        }
        else if (sal_strncasecmp(tab, "type-2nd-bar", sizeof("type-2nd-bar")) == 0)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                        "Current memories with access type 2nd-bar: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_2ND_BAR;
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK & ~ACCESS_REGMEM_FLAG_ACCESS_TYPE_2ND_BAR;
        }
        else if (sal_strncasecmp(tab, "wide", sizeof("wide")) == 0 || sal_strncasecmp(tab, "w", sizeof("w")) == 0)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current wide memories: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_WIDE;
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK & ~ACCESS_REGMEM_FLAG_ACCESS_TYPE_WIDE;
        }
        else if (sal_strncasecmp(tab, "access-per-port", sizeof("access-per-port")) == 0)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current access per port memories: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_PER_PORT;
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK & ~ACCESS_REGMEM_FLAG_ACCESS_TYPE_PER_PORT;
        }
        else if (sal_strncasecmp(tab, "no-protection", sizeof("no-protection")) == 0)
        {       /** list no-protection */
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current no protected registers: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_HAS_NO_PROTECTION;
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_CONTENT_PROTECTION_MASK & ~ACCESS_REGMEM_FLAG_HAS_NO_PROTECTION;
        }
        else if (sal_strncasecmp(tab, "has-ecc", sizeof("has-ecc")) == 0)
        {       /** list has-ecc */
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current registers with ecc: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_HAS_ECC;
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_CONTENT_PROTECTION_MASK & ~ACCESS_REGMEM_FLAG_HAS_ECC;
        }
        else if (sal_strncasecmp(tab, "has-parity", sizeof("has-parity")) == 0)
        {       /** list has-parity */
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current registers with parity: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_HAS_PARITY;
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_CONTENT_PROTECTION_MASK & ~ACCESS_REGMEM_FLAG_HAS_PARITY;
        }
        else if (sal_strncasecmp(tab, "print-blocks", sizeof("print-blocks")) == 0)
        {
            print_block_names = 1;
        }
        else
        {
            reg_name = tab;
            tab = ARG_GET(a);
            if (tab != NULL)
            {
                cli_out("Received value after register name %s\n", reg_name);
                value = tab;
                break;
            }
        }
    }

    if (!reg_name)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Must specify register name or * for printing all appropriate registers.\n");
        rv = CMD_USAGE;
        goto done;
    }
    /*
     * Symbolic name.
     * First check if the register is there as exact match.
     * If not, list all substring matches.
     */

    if (!sal_strncmp("*", reg_name, sizeof("*")))
    {
        access_dnxc_regmem_list_summary(runtime_info, NULL, print_block_names, flags_to_be_set, flags_to_be_clear, 0);
        rv = CMD_OK;
        goto done;
    }

    parse_register_name_rv = access_parse_register_name(runtime_info, reg_name, 0, &inst_i, &regmem_i);
    /** If access_parse_register_name returns -1, there is an error/failure from the user input */
    if (parse_register_name_rv < 0)
    {
        rv = CMD_FAIL;
        goto done;
    }

    /** If access_parse_register_name returns 1, there is no exact match of register name */
    if (parse_register_name_rv == 1)
    {
        /** Assume we have a partial name of a register */
        cli_out("Unknown register %s: possible matches are:\n", reg_name);
        access_dnxc_regmem_list_summary(runtime_info, reg_name, print_block_names, flags_to_be_set,
                                        flags_to_be_clear, 0);
        rv = CMD_OK;
        goto done;
    }

    /** If access_parse_register_name returns 0, the register is found */
    /** Register to access */
    rv = access_diag_dnxc_list_regmem(runtime_info, regmem_i, a, value);

done:

    if (fname[0] != 0)
    {
        bslfile_close();
    }

    if (console_disabled && console_was_on)
    {
        bslcons_enable(TRUE);
    }

    if (pushed_ctrl_c)
    {
        sh_pop_ctrl_c();
    }

    parse_arg_eq_done(&pt);
    return rv;
}

/*
 * List register information
 * Pedantic compiler allows only 509 chars
 */
char cmd_dnxc_getreg_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "Getreg [File=<name>] [Append=true|false]\n\t"
    "1. Parameters: [hex|raw|chg|debug|disabled] <REGISTER>[[array_index]][.block_instance]\n\t"
    "2. Parameters: [hex|raw|chg|disabled] direct dwc_read <nof words to read> cmic_blk <block_instance_sbus_id> <REGISTER_ADDRESS>\n\t"
    "3. Parameters: [hex|raw|chg|debug|disabled|flags] *\n\t"
    "4. Parameters: [hex|raw|chg|debug|disabled|flags] cmic_blk <block_instance_sbus_id>\n\t"
    "5. Parameters: [hex|raw|chg|debug|disabled|flags] <REGISTER>[[array_index]][.block_instance] <REGISTER_FIELD>\n\t"
    "Full documentation cannot be displayed with -pendantic compiler\n";
#else
    "1. Parameters: [hex|raw|chg|debug|disabled] <REGISTER>[[array_index]][.block_instance]\n\t"
    "2. Parameters: [hex|raw|chg|disabled] direct dwc_read <nof words to read> cmic_blk <block_instance_sbus_id> <REGISTER_ADDRESS>\n\t"
    "3. Parameters: [hex|raw|chg|debug|disabled|flags] *\n\t"
    "4. Parameters: [hex|raw|chg|debug|disabled|flags] cmic_blk <block_instance_sbus_id>\n\t"
    "5. Parameters: [hex|raw|chg|debug|disabled|flags] <REGISTER>[[array_index]][.block_instance] <REGISTER_FIELD>\n\t"
    "If hex is specified, dumps only a hex value (for Expect parsing).\n\t"
    "If raw is specified, no field decoding is done.\n\t"
    "If chg is specified, show only fields/regs changed from defaults.\n\t"
    "If debug is specified, show only the sbus block instance ID, the address and the value of the register\n\t"
    "If disabled is specified, try to access also the block instances that are disabled\n\t"
    "If block is specified, show all of the regs in the block.\n\t"
    "Flags:\n"
    "   read-write/r/w       display read-write\n"
    "   init-on-read         display init on read\n"
    "   clear-bits-on-write  display clear bits on write\n"
    "   counters/c           display counters\n"
    "   no-protection        display no protection\n"
    "   has-ecc              display has ecc\n"
    "   has-parity           display has parity\n"
    "   print-blocks         display block name before listing registers from the block\n"
    "\n\t"
    "For accessing sbus registers using their address and block instance number, use the 2nd form of the command (direct)\".\n";
#endif /* COMPILER_STRING_CONST_LIMIT */

/**
 * Implementation of the Getreg shell command, read and print a register, or multiple registers
 */
cmd_result_t
access_cmd_dnxc_getreg(
    int unit,
    args_t *a)
{
    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit);  /** Access information for the device */
    const access_device_type_info_t *device_info = runtime_info->device_type_info;  /** Constant access information for the device type */
    const access_device_block_t *local_block = NULL;
    const access_device_regmem_t *rm_info = NULL;
    access_local_regmem_id_t regmem_i;
    access_local_block_id_t block_i;
    access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
    uint16 array_index = -1;
    uint32 regaddr = 0, block_inst_sbus_id = -1, all_regs = 0, all_block_regs = 0;
    char *fname = "", *name = NULL, *reg_name = NULL, *block_name = NULL, *field_name = NULL;
    int rv = CMD_OK, append = FALSE, i, direct_mode = 0, dwc_read = -1, print_block_names = 0,
        parse_register_name_rv, first_array_ind, last_array_ind, first_inst_ind, last_inst_ind;
    uint32 flags_to_be_set = 0, flags_to_be_clear = 0, flags = 0;
    int console_was_on = 0, console_disabled = 0, pushed_ctrl_c = 0;
    parse_table_t pt;
    jmp_buf ctrl_c;
    const uint8 *block_instances_flags;
    bcm_gport_t port;
    bcm_port_config_t port_config;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "File", PQ_STRING, 0, &fname, 0);
    parse_table_add(&pt, "Append", PQ_BOOL, 0, &append, FALSE);

    if (!sh_check_attached(ARG_CMD(a), unit))
    {
        rv = CMD_FAIL;
        goto done;
    }
    if (parse_arg_eq(a, &pt) < 0)
    {
        rv = CMD_USAGE;
        goto done;
    }

    console_was_on = bslcons_is_enabled();

    if (fname[0] != 0)
    {
        /*
         * Catch control-C in case if using file output option. 
         */
#ifndef NO_CTRL_C
        if (setjmp(ctrl_c))
        {
            rv = CMD_INTR;
            goto done;
        }
#endif
        sh_push_ctrl_c(&ctrl_c);
        pushed_ctrl_c = TRUE;

        if (bslfile_is_enabled())
        {
            LOG_INFO(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit, "%s: Can't dump to file while logging is enabled\n"), ARG_CMD(a)));
            rv = CMD_FAIL;
            goto done;
        }

        if (bslfile_open((char *) fname, append) < 0)
        {
            LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "%s: Could not start log file\n"), ARG_CMD(a)));
            rv = CMD_FAIL;
            goto done;
        }

        bslcons_enable(FALSE);
        console_disabled = 1;
    }

    bsl_log_start(BSL_APPL_SHELL, bslSeverityNormal, unit, "");

    /*
     * If first arg is a register type or flag, take it and use the next argument
     * as the name or address.
     */
    for (name = ARG_GET(a); name != NULL; name = ARG_GET(a))
    {
        if (!sal_strncasecmp(name, "raw", sizeof("raw")))
        {
            flags |= REGMEM_PRINT_RAW;
        }
        else if (!sal_strncasecmp(name, "hex", sizeof("hex")))
        {
            flags |= REGMEM_PRINT_ONLY_HEX_VALUES;
        }
        else if (!sal_strncasecmp(name, "chg", sizeof("chg")))
        {
            flags |= REGMEM_PRINT_CHG;
        }
        else if (!sal_strncasecmp(name, "disabled", sizeof("disabled")))
        {
            flags |= REGMEM_PRINT_DISABLED_INST;
        }
        else if (!sal_strncasecmp(name, "cmic_blk", sizeof("cmic_blk")))
        {
            name = ARG_GET(a);
            if (name != NULL && isint(name))
            {
                block_inst_sbus_id = parse_integer(name);
            }
            else
            {
                cli_out("Error: must specify cmic_blk argument\n");
                rv = CMD_FAIL;
                goto done;
            }
        }
        else if (!sal_strncasecmp(name, "direct", sizeof("direct")))
        {
            direct_mode = 1;
        }
        else if (!sal_strncasecmp(name, "dwc_read", sizeof("dwc_read")))
        {
            name = ARG_GET(a);
            if (name != NULL && isint(name))
            {
                dwc_read = parse_integer(name);
            }
            else
            {
                cli_out("Error: must specify nof words to read\n");
                rv = CMD_FAIL;
                goto done;
            }
        }
        else if (!sal_strncasecmp(name, "*", sizeof("*")))
        {
            all_regs = 1;
        }
        else if (!sal_strncasecmp(name, "block", sizeof("block")))
        {
            block_name = ARG_GET(a);
            if (block_name == NULL)
            {
                cli_out("Error: must specify block name\n");
                rv = CMD_FAIL;
                goto done;
            }
            all_block_regs = 1;
        }
        else if (!sal_strncasecmp(name, "debug", sizeof("debug")))
        {
            flags |= REGMEM_PRINT_ADDR;
        }
        else if (!sal_strncasecmp(name, "read-write", sizeof("read-write")) || !sal_strncasecmp(name, "r/w", sizeof("r/w")))
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current read and write registers: \n");
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_RO | ACCESS_REGMEM_FLAG_WO;
        }
        else if (!sal_strncasecmp(name, "init-on-read", sizeof("init-on-read")))
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current init on read registers: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_INIT_ON_READ;
        }
        else if (!sal_strncasecmp(name, "clear-bits-on-write", sizeof("clear-bits-on-write")))
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                        "Current clear bits on write registers: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_CLEAR_BITS_ON_WRITE;
        }
        else if (!sal_strncasecmp(name, "counter", sizeof("counter")) || !sal_strncasecmp(name, "c", sizeof("c")))
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current counter registers: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_COUNTER;
        }
        else if (!sal_strncasecmp(name, "no-protection", sizeof("no-protection")))
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current no protected registers: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_HAS_NO_PROTECTION;
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_CONTENT_PROTECTION_MASK & ~ACCESS_REGMEM_FLAG_HAS_NO_PROTECTION;
        }
        else if (!sal_strncasecmp(name, "has-ecc", sizeof("has-ecc")))
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current registers with ecc: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_HAS_ECC;
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_CONTENT_PROTECTION_MASK & ~ACCESS_REGMEM_FLAG_HAS_ECC;
        }
        else if (!sal_strncasecmp(name, "has-parity", sizeof("has-parity")))
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current registers with parity: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_HAS_PARITY;
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_CONTENT_PROTECTION_MASK & ~ACCESS_REGMEM_FLAG_HAS_PARITY;
        }
        else if (!sal_strncasecmp(name, "print-blocks", sizeof("print-blocks")))
        {
            print_block_names = 1;
        }
        else if (!all_block_regs && !all_regs)
        {
            reg_name = name;
            name = ARG_GET(a);
            if (name != NULL)
            {
                field_name = name;
                name = ARG_GET(a);
            }
            if (name != NULL)
            {
                cli_out("Received unexpected argument %s after register/field name\n", name);
                rv = CMD_FAIL;
                goto done;
            }
        }
        else
        {
            cli_out("Received unexpected argument %s\n", name);
            rv = CMD_FAIL;
            goto done;
        }
    }

    if (1 == all_regs)
    {
         /** Loop over all registers */
        for (regmem_i = 0; regmem_i < device_info->nof_regs; regmem_i++)
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
            if (local_block != device_info->blocks + rm_info->local_block_id)
            {
                local_block = device_info->blocks + rm_info->local_block_id;
                if (print_block_names)
                {
                    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "registers of block %s:\n",
                                access_global_block_types[local_block->block_type].block_name);
                }
            }
            if (diag_dnxc_filter_regs_getreg_all(runtime_info, regmem_i))
            {
                continue;
            }

            /** Check if a block instance is enabled */
            block_instances_flags = runtime_info->block_instance_flags + local_block->instances_start_index;
            for (inst_i = 0; inst_i < local_block->nof_instances; inst_i++)
            {
                if ((block_instances_flags[inst_i] & ACCESS_RUNTIME_BLOCK_INSTANCE_FLAG_DISABLED))
                {
                    if (flags & REGMEM_PRINT_DISABLED_INST)
                    {
                        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                                    "Trying accessing disabled instance: %s%u\n",
                                    access_global_block_types[local_block->block_type].block_name, inst_i);
                    }
                    else
                    {
                        continue;
                    }
                }
                for (array_index = rm_info->first_array_index;
                     array_index < rm_info->nof_array_indices + rm_info->first_array_index; array_index++)
                {
                    if (access_dnxc_print_regmem(runtime_info, flags, regmem_i, inst_i, array_index, 0, 0, NULL) != SOC_E_NONE)
                    {
                        cli_out("Error while accessing the register\n");
                        rv = CMD_FAIL;
                    }
                }
            }
        }
        goto done;
    }

    if (1 == all_block_regs)
    {
        if (block_name == NULL)
        {
            cli_out("Error: must specify a block name\n");
            rv = CMD_FAIL;
            goto done;
        }
        for (block_i = 0; block_i < device_info->nof_blocks; block_i++)
        {
            local_block = device_info->blocks + block_i;

            if (!sal_strncasecmp(block_name, access_global_block_types[local_block->block_type].block_name,
                SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))
            {
                if (print_block_names)
                {
                    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "registers of block %s:\n",
                                access_global_block_types[local_block->block_type].block_name);
                }
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
                    if (diag_dnxc_filter_regs_getreg_all(runtime_info, regmem_i))
                    {
                        continue;
                    }

                    /** Check if a block instance is enabled */
                    block_instances_flags = runtime_info->block_instance_flags + local_block->instances_start_index;
                    for (inst_i = 0; inst_i < local_block->nof_instances; inst_i++)
                    {
                        if ((block_instances_flags[inst_i] & ACCESS_RUNTIME_BLOCK_INSTANCE_FLAG_DISABLED))
                        {
                            if (flags & REGMEM_PRINT_DISABLED_INST)
                            {
                                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                                            "Trying accessing disabled instance: %s%u\n",
                                            access_global_block_types[local_block->block_type].block_name, inst_i);
                            }
                            else
                            {
                                continue;
                            }
                        }
                        for (array_index = rm_info->first_array_index;
                             array_index < rm_info->nof_array_indices + rm_info->first_array_index; array_index++)
                        {
                            if (access_dnxc_print_regmem(runtime_info, flags, regmem_i, inst_i, array_index, 0, 0, NULL) != SOC_E_NONE)
                            {
                                cli_out("Error while accessing the register\n");
                                rv = CMD_FAIL;
                            }
                        }
                    }
                }
                goto done;
            }
        }
        cli_out("Error: Block %s not found\n", reg_name);
        rv = CMD_FAIL;
        goto done;
    }

    if (reg_name == NULL)
    {
        rv = CMD_USAGE;
        goto done;
    }

    if (isint(reg_name))
    {   /** Numerical address given */
        regaddr = parse_integer(reg_name);
        if (1 == direct_mode)
        {
            if (dwc_read == -1 || block_inst_sbus_id == -1)
            {
                cli_out("The direct argument also mode requires the dwc_read and cmic_blk arguments\n");
                rv = CMD_FAIL;
                goto done;
            }
            else if (dwc_read > 30)
            {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                            "dwc_read max size is 30, received %d\n", dwc_read);
                rv = CMD_FAIL;
                goto done;
            }
            else
            {
                uint32 get_data[30];
                if (access_direct_regmem(unit, flags, block_inst_sbus_id, regaddr, dwc_read, get_data) != SOC_E_NONE)
                {
                    cli_out("Failed to access an sbus register using its sbus block instance ID, address and size\n");
                    rv = CMD_FAIL;
                }
                else 
                {
                    for (i = 0; i < dwc_read; i++)
                    {
                        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "0x%x[%d]=0x%x\n",
                                regaddr + 4 * i, i, get_data[i]);
                    }
                }
                goto done;
            }
        }
        else
        {
            cli_out("Illegal register name %s\n", reg_name);
            rv = CMD_FAIL;
            goto done;
        }
    }
    else
    {
        /*
         * Symbolic name given, print all or some values ...
         */
        /** If access_parse_register_name returns -1, there is an error/failure from the user input */
        parse_register_name_rv = access_parse_register_name(runtime_info, reg_name, &array_index, &inst_i, &regmem_i);
        if (parse_register_name_rv < 0)
        {
            rv = CMD_FAIL;
            goto done;
        }
        /** If access_parse_register_name returns 1, there is no exact match of register name */
        if (parse_register_name_rv == 1)
        {
            /** Assume we have a partial name of a register */
            cli_out("Register %s was not found\n", reg_name);
            rv = CMD_FAIL;
            goto done;
        }

        /** If access_parse_register_name returns 0, the register is found */
        /** Register to access */
        rm_info = device_info->local_regs + regmem_i;

        local_block = device_info->blocks + rm_info->local_block_id;

        if (array_index == (uint16)(-1))
        {
            first_array_ind = rm_info->first_array_index;
            last_array_ind = rm_info->nof_array_indices + rm_info->first_array_index - 1;
        }
        else
        {
            first_array_ind = last_array_ind = array_index;
        }

        block_instances_flags = runtime_info->block_instance_flags + local_block->instances_start_index;

        if ((rm_info->flags & ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK) == ACCESS_REGMEM_FLAG_ACCESS_TYPE_PER_PORT)
        {
            if (inst_i == ACCESS_ALL_BLOCK_INSTANCES) 
            {
                rv = bcm_port_config_get(unit, &port_config);
                BCM_PBMP_ITER(port_config.nif, port)
                {
                    uint32 phy_port = 0;
                    int block_inst_id = -1;
                    access_local_block_id_t local_block_id;
                    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                    block_inst_id = device_info->port_info[phy_port].blk; /* get the local block instance ID for phy_port */
                    if (block_inst_id >= 0 && block_inst_id < device_info->nof_block_instances) { /* if found valid port_info data */
                        local_block_id = device_info->block_instances[block_inst_id].block_id;
                        if (rm_info->local_block_id == local_block_id || (rm_info->local_block_id == device_info->blocks_global2local_map[BLOCK_CDMAC] && local_block_id == device_info->blocks_global2local_map[BLOCK_CDPORT]) || (rm_info->local_block_id == device_info->blocks_global2local_map[BLOCK_CLMAC] && local_block_id == device_info->blocks_global2local_map[BLOCK_CLPORT]) || (rm_info->local_block_id == device_info->blocks_global2local_map[BLOCK_DC3MAC] && local_block_id == device_info->blocks_global2local_map[BLOCK_DC3PORT])) /* may need another blocktypes for future devices */
                        {
                            for (array_index = first_array_ind; array_index <= last_array_ind; array_index++)
                            {
                                flags |= REGMEM_PRINT_FIELDS;
                                if (access_dnxc_print_regmem(runtime_info, flags, regmem_i, port, array_index, 0, 0, field_name) != SOC_E_NONE)
                                {
                                    cli_out("Error while accessing the register\n");
                                    rv = CMD_FAIL;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                for (array_index = first_array_ind; array_index <= last_array_ind; array_index++)
                {
                    flags |= REGMEM_PRINT_FIELDS;
                    if (access_dnxc_print_regmem(runtime_info, flags, regmem_i, inst_i, array_index, 0, 0, field_name) != SOC_E_NONE)
                    {
                        cli_out("Error while accessing the register\n");
                        rv = CMD_FAIL;
                    }
                }
            }
        }
        else
        {
            if (inst_i == ACCESS_ALL_BLOCK_INSTANCES)
            {
                first_inst_ind = 0;
                last_inst_ind = local_block->nof_instances - 1;
            }
            else
            {
                first_inst_ind = last_inst_ind = inst_i;
            }
            for (inst_i = first_inst_ind; inst_i <= last_inst_ind; inst_i++)
            {
                /** Check if a block instance is enabled */
                if ((block_instances_flags[inst_i] & ACCESS_RUNTIME_BLOCK_INSTANCE_FLAG_DISABLED))
                {
                    if (flags & REGMEM_PRINT_DISABLED_INST)
                    {
                        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                                    "Trying accessing disabled instance: %s%u\n",
                                    access_global_block_types[local_block->block_type].block_name, inst_i);
                    }
                    else
                    {
                        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                                    "Block instance %s%u is disabled\n",
                                    access_global_block_types[local_block->block_type].block_name, inst_i);
                        continue;
                    }
                }
                for (array_index = first_array_ind; array_index <= last_array_ind; array_index++)
                {
                    flags |= REGMEM_PRINT_FIELDS;
                    if (access_dnxc_print_regmem(runtime_info, flags, regmem_i, inst_i, array_index, 0, 0, field_name) != SOC_E_NONE)
                    {
                        cli_out("Error while accessing the register\n");
                        rv = CMD_FAIL;
                    }
                }
            }
        }
    }
    bsl_log_end(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "");

done:

    if (fname[0] != 0)
    {
        bslfile_close();
    }

    if (console_disabled && console_was_on)
    {
        bslcons_enable(TRUE);
    }

    if (pushed_ctrl_c)
    {
        sh_pop_ctrl_c();
    }

    parse_arg_eq_done(&pt);
    return rv;
}

char cmd_dnxc_dump_usage[] = "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "Dump [options]\n"
#else
    "Dump [File=<name>] [Append=true|false]\n\t"
    "   1. Parameters: [raw|hex|chg|debug|addr|force|disabled]\n\t"
    "        <TABLE[[array_index]]>[.block_instance] [<index>] [<count>]\n\t"
    "   2. Parameters: [raw|hex|chg|debug|disable_cache|addr|force|disabled|flags] *\n\t"
    "   3. Parameters: [raw|hex|chg|debug|disable_cache|addr|force|disabled|flags] block <block_name>\n\t"
    "   4. Parameters: [raw|hex|chg|debug|addr|force|disabled]\n\t"
    "       <TABLE[[array_index]]>[.block_instance] [<index>] [<count>] <TABLE_FIELD>\n\t"
    "      If raw is specified, show raw memory words instead of fields.\n\t"
    "      If hex is specified, show hex data only (for Expect parsing).\n\t"
    "      If chg is specified, show only fields changed from defaults\n\t"
    "      If debug is specified, show only the sbus block instance ID, the address and the value of the memory\n\t"
    "      If disable_cache is specified, doesn't read from cached memory, if exist\n\t"
    "      If addr is specified, show block instance and address instead of memory name and index\n\t"
    "      If block is specified, show all of the memories in the block\n\t"
    "      If force is specified, attempt the operation even if the memory does not support it\n\t"
    "      If disabled is specified, try to access also the block instances that are disabled\n\t"
    "      Flags:\n"
    "       dynamic-table        display dynamic\n"
    "       read-write/r/w       display read-write\n"
    "       init-on-read         display init on read\n"
    "       clear-bits-on-write  display clear bits on write\n"
    "       counters/c           display counters\n"
    "       alias                display alias\n"
    "       type-1st-bar         display type-1st-bar\n"
    "       type-2nd-bar         display type-2nd-bar\n"
    "       wide                 display wide\n"
    "       no-protection        display no protection\n"
    "       has-ecc              display has ecc\n"
    "       has-parity           display has parity\n"
    "       print-blocks         display block name before listing memories from the block\n"
    "\n\t"
    "For accessing sbus memories using their address and block instance number, use the 5th form of the command.\n"
    "      (Use \"listmem\" command to show a list of valid tables)\n"
    "Dump MW [<START> [<COUNT>]]   (System memory, 32 bits)\n"
#endif /* COMPILER_STRING_CONST_LIMIT */
    ;

/**
 * Implementation of the Dump shell command, read and list specific memories
 */
cmd_result_t
access_cmd_dnxc_dump(
    int unit,
    args_t *a)
{
    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit);  /** Access information for the device */
    const access_device_type_info_t *device_info = runtime_info->device_type_info;  /** Constant access information for the device type */
    const access_device_block_t *local_block = NULL;
    const access_device_regmem_t *rm_info = NULL;
    access_local_regmem_id_t regmem_i;
    access_local_block_id_t block_i;
    access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
    char *arg1 = NULL, *fname = "", *mem_name = NULL, *block_name = NULL, *field_name = NULL;
    int rv = CMD_OK, force = 0, append = FALSE, print_block_names = 0,
        parse_memory_name_rv, index = -1, count = -1, first_array_ind, last_array_ind, first_inst_ind, last_inst_ind;
    parse_table_t pt;
    int console_was_on = 0, console_disabled = 0, pushed_ctrl_c = 0;
    jmp_buf ctrl_c;
    uint32 regaddr = -1, memory_index = -1, flags_to_be_set = 0, flags_to_be_clear = 0, flags =
        0, all_mems = 0, all_block_mems = 0, block_inst_sbus_id = -1;
    uint16 array_index = -1;
    const uint8 *block_instances_flags;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "File", PQ_STRING, 0, &fname, 0);
    parse_table_add(&pt, "Append", PQ_BOOL, 0, &append, FALSE);

    if (!sh_check_attached(ARG_CMD(a), unit))
    {
        goto done;
    }
    if (parse_arg_eq(a, &pt) < 0)
    {
        rv = CMD_USAGE;
        goto done;
    }

    console_was_on = bslcons_is_enabled();

    if (fname[0] != 0)
    {
        /*
         * Catch control-C in case if using file output option. 
         */
#ifndef NO_CTRL_C
        if (setjmp(ctrl_c))
        {
            rv = CMD_INTR;
            goto done;
        }
#endif
        sh_push_ctrl_c(&ctrl_c);
        pushed_ctrl_c = TRUE;

        if (bslfile_is_enabled())
        {
            LOG_INFO(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit, "%s: Can't dump to file while logging is enabled\n"), ARG_CMD(a)));
            rv = CMD_FAIL;
            goto done;
        }

        if (bslfile_open((char *) fname, append) < 0)
        {
            LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "%s: Could not start log file\n"), ARG_CMD(a)));
            rv = CMD_FAIL;
            goto done;
        }

        bslcons_enable(FALSE);
        console_disabled = 1;
    }
    bsl_log_start(BSL_APPL_SHELL, bslSeverityNormal, unit, "");

    for (arg1 = ARG_GET(a); arg1 != NULL; arg1 = ARG_GET(a))
    {
        if (!sal_strncasecmp(arg1, "raw", sizeof("raw")))
        {
            flags |= REGMEM_PRINT_RAW;
        }
        else if (!sal_strncasecmp(arg1, "hex", sizeof("hex")))
        {
            flags |= REGMEM_PRINT_ONLY_HEX_VALUES;
        }
        else if (!sal_strncasecmp(arg1, "chg", sizeof("chg")))
        {
            flags |= REGMEM_PRINT_CHG;
        }
        else if (!sal_strncasecmp(arg1, "debug", sizeof("debug")))
        {
            flags |= REGMEM_PRINT_ADDR;
        }
        else if (!sal_strncasecmp(arg1, "disable_cache", sizeof("disable_cache")))
        {
            flags |= DUMP_DISABLE_CACHE;
        }
        else if (!sal_strncasecmp(arg1, "addr", sizeof("addr")))
        {
            flags |= DUMP_ADDR_ONLY;
        }
        else if (!sal_strncasecmp(arg1, "block", sizeof("block")))
        {
            block_name = ARG_GET(a);
            if (block_name == NULL)
            {
                cli_out("Error: must specify block name\n");
                rv = CMD_FAIL;
                goto done;
            }
            all_block_mems = 1;
        }
        else if (!sal_strncasecmp(arg1, "force", sizeof("force")))
        {
            force = 1;
        }
        else if (!sal_strncasecmp(arg1, "disabled", sizeof("disabled")))
        {
            flags |= REGMEM_PRINT_DISABLED_INST;
        }
        else if (!sal_strncasecmp(arg1, "cmic_blk", sizeof("cmic_blk")))
        {
            arg1 = ARG_GET(a);
            if (arg1 != NULL && isint(arg1))
            {
                block_inst_sbus_id = parse_integer(arg1);
            }
            else
            {
                cli_out("Error: must specify cmic_blk argument\n");
                rv = CMD_FAIL;
                goto done;
            }
        }
        else if (!sal_strncasecmp(arg1, "*", sizeof("*")))
        {
            all_mems = 1;
        }
        else if (!sal_strncasecmp(arg1, "dynamic-table", sizeof("dynamic-table")))
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current dynamic memories: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_ASIC_CHANGABLE;
        }
        else if (!sal_strncasecmp(arg1, "read-write", sizeof("read-write")) || !sal_strncasecmp(arg1, "r/w", sizeof("r/w")))
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current read and write registers: \n");
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_RO | ACCESS_REGMEM_FLAG_WO;
        }
        else if (!sal_strncasecmp(arg1, "init-on-read", sizeof("init-on-read")))
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current init on read registers: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_INIT_ON_READ;
        }
        else if (!sal_strncasecmp(arg1, "clear-bits-on-write", sizeof("clear-bits-on-write")))
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                        "Current clear bits on write registers: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_CLEAR_BITS_ON_WRITE;
        }
        else if (!sal_strncasecmp(arg1, "counter", sizeof("counter")) || !sal_strncasecmp(arg1, "c", sizeof("c")))
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current counter registers: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_COUNTER;
        }
        else if (!sal_strncasecmp(arg1, "alias", sizeof("alias")))
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current alias memories: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_IS_ALIAS;
        }
        else if (!sal_strncasecmp(arg1, "type-1st-bar", sizeof("type-1st-bar")))
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                        "Current memories with access type 1st-bar: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_1ST_BAR;
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK & ~ACCESS_REGMEM_FLAG_ACCESS_TYPE_1ST_BAR;
        }
        else if (!sal_strncasecmp(arg1, "type-2nd-bar", sizeof("type-2nd-bar")))
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                        "Current memories with access type 2nd-bar: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_2ND_BAR;
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK & ~ACCESS_REGMEM_FLAG_ACCESS_TYPE_2ND_BAR;
        }
        else if (!sal_strncasecmp(arg1, "wide", sizeof("wide")))
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current wide memories: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_WIDE;
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK & ~ACCESS_REGMEM_FLAG_ACCESS_TYPE_WIDE;
        }
        else if (!sal_strncasecmp(arg1, "no-protection", sizeof("no-protection")))
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current no protected registers: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_HAS_NO_PROTECTION;
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_CONTENT_PROTECTION_MASK & ~ACCESS_REGMEM_FLAG_HAS_NO_PROTECTION;
        }
        else if (!sal_strncasecmp(arg1, "has-ecc", sizeof("has-ecc")))
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current registers with ecc: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_HAS_ECC;
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_CONTENT_PROTECTION_MASK & ~ACCESS_REGMEM_FLAG_HAS_ECC;
        }
        else if (!sal_strncasecmp(arg1, "has-parity", sizeof("has-parity")))
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current registers with parity: \n");
            flags_to_be_set |= ACCESS_REGMEM_FLAG_HAS_PARITY;
            flags_to_be_clear |= ACCESS_REGMEM_FLAG_CONTENT_PROTECTION_MASK & ~ACCESS_REGMEM_FLAG_HAS_PARITY;
        }
        else if (!sal_strncasecmp(arg1, "print-blocks", sizeof("print-blocks")))
        {
            print_block_names = 1;
        }
        else if (!sal_strncasecmp(arg1, "pcic", sizeof("pcic")))
        {
            diag_sand_pci_print_config(unit);
            goto done;
        }
        else if (!sal_strncasecmp(arg1, "mw", sizeof("mw")))
        {
            diag_sand_hostmem_w_print(unit, a);
            goto done;
        }
        else
        {
            if (all_mems || all_block_mems)
            {
                cli_out("Received argument after block name or '*'\n");
                rv = CMD_FAIL;
                goto done;
            }
            mem_name = arg1;
            arg1 = ARG_GET(a);
            if (arg1 != NULL)
            {
                if (isint(arg1))
                {
                    index = parse_integer(arg1);
                    arg1 = ARG_GET(a);
                    if (arg1 != NULL)
                    {
                        if (isint(arg1))
                        {
                            count = parse_integer(arg1) - 1;
                            arg1 = ARG_GET(a);
                            if (arg1 != NULL)
                            {
                                field_name = arg1;
                                arg1 = ARG_GET(a);
                                if (arg1 != NULL)
                                {
                                    cli_out("Error: found argument %s after field name\n", arg1);
                                    rv = CMD_FAIL;
                                    goto done;
                                }
                            }
                        }
                        else
                        {
                            cli_out("Error: count must be an integer value\n");
                            rv = CMD_FAIL;
                            goto done;
                        }
                    }
                }
                else
                {
                    field_name = arg1;
                    arg1 = ARG_GET(a);
                    if (arg1 != NULL)
                    {
                        cli_out("Error: found argument %s after field name\n", arg1);
                        rv = CMD_FAIL;
                        goto done;
                    }
                }
            }
        }
    }

    if (mem_name == NULL && !all_mems && !all_block_mems)
    {
        rv = CMD_USAGE;
        goto done;
    }

    if (1 == all_mems)
    {
         /** Loop over all momories */
        for (rm_info = device_info->local_mems, regmem_i = device_info->nof_mems; regmem_i; regmem_i--, rm_info++)
        {
            if ((rm_info->flags & ACCESS_REGMEM_FLAG_WO) && !force)
            {
                LOG_INFO(BSL_LS_APPL_COMMON,
                         (BSL_META_U(unit, "Memory %s is write-only.\n"),
                          access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]));
                continue;
            }
            if (rm_info->flags & ACCESS_REGMEM_FLAG_IS_ALIAS)
            {
                continue;
            }
            /** Filter by flags */
            if ((rm_info->flags & flags_to_be_set) != flags_to_be_set)
            {
                continue;
            }
            if ((rm_info->flags & flags_to_be_clear) != 0)
            {
                continue;
            }
            if (local_block != device_info->blocks + rm_info->local_block_id)
            {
                local_block = device_info->blocks + rm_info->local_block_id;
                if (print_block_names)
                {
                    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "memories of block %s:\n",
                                access_global_block_types[local_block->block_type].block_name);
                }
            }

            /** Check if a block instance is enabled */
            block_instances_flags = runtime_info->block_instance_flags + local_block->instances_start_index;
            for (inst_i = 0; inst_i < local_block->nof_instances; inst_i++)
            {
                if ((block_instances_flags[inst_i] & ACCESS_RUNTIME_BLOCK_INSTANCE_FLAG_DISABLED))
                {
                    if (flags & REGMEM_PRINT_DISABLED_INST)
                    {
                        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                                    "Trying accessing disabled instance: %s%u\n",
                                    access_global_block_types[local_block->block_type].block_name, inst_i);
                    }
                    else
                    {
                        continue;
                    }
                }
                for (array_index = rm_info->first_array_index;
                     array_index < rm_info->nof_array_indices + rm_info->first_array_index; array_index++)
                {
                    if (access_dnxc_print_regmem(runtime_info, flags, rm_info - device_info->local_regs, inst_i, array_index, 0, rm_info->u.mem.mem_nof_elements - 1, NULL) != SOC_E_NONE)
                    {
                        cli_out("Error while accessing the memory\n");
                        rv = CMD_FAIL;
                    }
                }
            }
        }
        goto done;
    }

    /*
     * Dumping all memories of a block 
     */
    if (1 == all_block_mems)
    {
        if (block_name == NULL)
        {
            cli_out("Error: must specify a block name\n");
            rv = CMD_FAIL;
            goto done;
        }
        for (block_i = 0; block_i < device_info->nof_blocks; block_i++)
        {
            local_block = device_info->blocks + block_i;

            if (!sal_strncasecmp(block_name, access_global_block_types[local_block->block_type].block_name,
                SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))
            {
                if (print_block_names)
                {
                    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "memories of block %s:\n",
                                access_global_block_types[local_block->block_type].block_name);
                }
                /** Loop over the memories of the block */
                for (regmem_i = local_block->memories_start;
                     regmem_i < local_block->memories_start + local_block->nof_memories; regmem_i++)
                {
                    /** Memory to access */
                    rm_info = device_info->local_regs + regmem_i;

                    if ((rm_info->flags & ACCESS_REGMEM_FLAG_WO) && !force)
                    {
                        LOG_INFO(BSL_LS_APPL_COMMON,
                                 (BSL_META_U(unit, "Memory %s is write-only.\n"),
                                  access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]));
                        continue;
                    }
                    if (rm_info->flags & ACCESS_REGMEM_FLAG_IS_ALIAS)
                    {
                        continue;
                    }
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
                            if (flags & REGMEM_PRINT_DISABLED_INST)
                            {
                                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                                            "Trying accessing disabled instance: %s%u\n",
                                            access_global_block_types[local_block->block_type].block_name, inst_i);
                            }
                            else
                            {
                                continue;
                            }
                        }
                        for (array_index = rm_info->first_array_index;
                             array_index < rm_info->nof_array_indices + rm_info->first_array_index; array_index++)
                        {
                            flags |= REGMEM_PRINT_FIELDS;
                            if (access_dnxc_print_regmem(runtime_info, flags, regmem_i, inst_i, array_index, 0, rm_info->u.mem.mem_nof_elements - 1, NULL) != SOC_E_NONE)
                            {
                                cli_out("Error while accessing the memory\n");
                                rv = CMD_FAIL;
                            }
                        }
                    }
                }
                goto done;
            }
        }
        cli_out("Error: Block %s not found\n", block_name);
        rv = CMD_FAIL;
        goto done;
    }

    if (isint(mem_name))
    {   /** Numerical address given */
        regaddr = parse_integer(mem_name);
        if (block_inst_sbus_id == -1)
        {
            cli_out("Error: cmic_blk was not specified\n");
            rv = CMD_FAIL;
            goto done;
        }
        if (access_find_regmem_by_address_and_by_sbus_block_id(runtime_info, ACCESS_REGMEM_FLAG_MEMORY, block_inst_sbus_id, regaddr, &regmem_i, &inst_i, &array_index, &memory_index) != SOC_E_NONE)
        {
            cli_out("Error while trying to find the memory by address and by sbus block id\n");
            rv = CMD_FAIL;
        }
    }
    else
    {
        /*
         * See if dumping a memory table 
         */
        /** If access_parse_memory_name returns -1, there is an error/failure from the user input */
        parse_memory_name_rv = access_parse_memory_name(runtime_info, mem_name, &array_index, &inst_i, &regmem_i);
        if (parse_memory_name_rv < 0)
        {
            rv = CMD_FAIL;
            goto done;
        }
        /** If access_parse_memory_name returns 1, there is no exact match of memory name */
        if (parse_memory_name_rv == 1)
        {
            /** Assume we have a partial name of a memory */
            cli_out("Memory %s was not found\n", mem_name);
            rv = CMD_FAIL;
            goto done;
        }
    }

    /** If access_parse_memory_name returns 0 or the memory is found by address and sbus_block_id */
    /** Memory to access */
    rm_info = device_info->local_regs + regmem_i;

    if ((rm_info->flags & ACCESS_REGMEM_FLAG_WO) && !force)
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "Error: Memory %s is write-only.\n"),
                  access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]));
        rv = CMD_FAIL;
        goto done;
    }

    if (index == -1)
    {
        index = 0;
    }
    if (count == -1)
    {
        if (field_name == NULL)
        {
            count = rm_info->u.mem.mem_nof_elements - 1;
        }
        else
        {
            count = 0;
        }

    }
    if (index < 0 || count < 0 || index >= rm_info->u.mem.mem_nof_elements
        || index + count >= rm_info->u.mem.mem_nof_elements)
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U
                  (unit, "Error: Memory index %d or count %d do not specify a legal sub range of indices 0..%u\n"),
                  index, count, rm_info->u.mem.mem_nof_elements));
        rv = CMD_FAIL;
        goto done;
    }

    local_block = device_info->blocks + rm_info->local_block_id;

    if (inst_i == ACCESS_ALL_BLOCK_INSTANCES)
    {
        first_inst_ind = 0;
        last_inst_ind = local_block->nof_instances - 1;
    }
    else
    {
        first_inst_ind = last_inst_ind = inst_i;
    }
    if (array_index == (uint16)(-1))
    {
        first_array_ind = rm_info->first_array_index;
        last_array_ind = rm_info->nof_array_indices + rm_info->first_array_index - 1;
    }
    else
    {
        first_array_ind = last_array_ind = array_index;
    }

    block_instances_flags = runtime_info->block_instance_flags + local_block->instances_start_index;

    flags |= REGMEM_PRINT_FIELDS;
    for (inst_i = first_inst_ind; inst_i <= last_inst_ind; inst_i++)
    {
        /** Check if a block instance is enabled */
        if ((block_instances_flags[inst_i] & ACCESS_RUNTIME_BLOCK_INSTANCE_FLAG_DISABLED))
        {
            if (flags & REGMEM_PRINT_DISABLED_INST)
            {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                            "Trying accessing disabled instance: %s%u",
                            access_global_block_types[local_block->block_type].block_name, inst_i);
            }
            else
            {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Block instance %s%u is disabled",
                            access_global_block_types[local_block->block_type].block_name, inst_i);
                continue;
            }
        }
        for (array_index = first_array_ind; array_index <= last_array_ind; array_index++)
        {
            if (access_dnxc_print_regmem(runtime_info, flags, regmem_i, inst_i, array_index, index, index + count, field_name) != SOC_E_NONE)
            {
                cli_out("Error while accessing the memory\n");
                rv = CMD_FAIL;
            }
        }
    }

    bsl_log_end(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "");
    goto done;

done:

    if (fname[0] != 0)
    {
        bslfile_close();
    }

    if (console_disabled && console_was_on)
    {
        bslcons_enable(TRUE);
    }

    if (pushed_ctrl_c)
    {
        sh_pop_ctrl_c();
    }

    parse_arg_eq_done(&pt);
    return rv;
}

/*
 * diag_dnxc_parse_and_set_regmem_field
 *
 *   Takes a string 'fields_str' containing "FIELD=value".
 *   Updates the field in the regmem value accordingly,
 *   leaving all other bits unmodified.
 *
 *   Returns CMD_FAIL on failure, CMD_OK on success.
 */

static int
diag_dnxc_parse_and_set_regmem_field(
    access_runtime_info_t *runtime_info,
    access_local_regmem_id_t regmem_i,
    uint32 *regmem_val,         /* value to write to the register */
    char *fields_str            /* "FILED=VALUE" string */
    )
{
    const access_device_type_info_t *device_info = runtime_info->device_type_info;  /** Constant access information for the device type */
    const access_device_regmem_t *rm_info = device_info->local_regs + regmem_i;
    const access_device_field_t *field_info;
    access_local_field_id_t fld;
    char *fval;
    uint32 fvalue[APPL_ACCESS_MAX_REG_UINT32S] = { 0 };

    fval = sal_strnchr(fields_str, '=', SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    if (fval == NULL)
    {
        cli_out("Missing value to assign to field.\n");
        return CMD_FAIL;
    }
    /*
     * Point fval to arg, NULL if none 
     */
    *fval = 0;  /* Now fmod holds only field name. */

    for (fld = rm_info->fields_index; fld < rm_info->fields_index + rm_info->nof_fields; fld++)
    {
        /** Field to access */
        field_info = device_info->local_fields + fld;

        if (!sal_strcasecmp(fields_str, access_global_names[global_field_name_indices[field_info->global_field_id]]))
        {
            break;
        }
    }
    if (fld == rm_info->fields_index + rm_info->nof_fields)
    {
        cli_out("No such field \"%s\" in regmem \"%s\".\n", fields_str,
                access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
        *fval = '=';
        return CMD_FAIL;
    }
    *fval++ = '=';
    parse_long_integer(fvalue, APPL_ACCESS_MAX_REG_UINT32S, fval);

    if (SHR_FAILURE(access_local_field_set(runtime_info, fld, regmem_val, fvalue)))
    {
        return CMD_FAIL;
    }

    return CMD_OK;
}

/*
 * Auxilliary routine to handle setreg and modreg.
 *      if block instance (inst_i) is specified, apply the command only for this instance or
 * if it is not specified, apply the command for all block instances.
 *      for register arrays - if array index (array_index) is specified, apply the command only for this index or
 * if it is not specified, apply the command for all indices.
 *      mod should be 0 for setreg, which takes either a value or a
 *              list of <field>=<value>, and in the latter case, sets all
 *              non-specified fields to zero.
 *      mod should be 1 for modreg, which does a read-modify-write of
 *              the register and permits value to be specified by a list
 *              of <field>=<value>[,...] only.
 * 
 *  Examples with modreg:
 *      modreg fe_mac1 lback=1        (modifies only lback field)
 *      modreg config ip_cfg=2,cpu_pri=4
 *      modreg config ip_cfg=2, cpu_pri=4
 */
static soc_error_t
diag_dnxc_do_setreg(
    int unit,
    args_t *a,
    int modify)
{
    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); /** Access information for the device */
    const access_device_type_info_t *device_info = runtime_info->device_type_info; /**  Constant access information for the device type */
    const access_device_regmem_t *rm_info = NULL;
    const access_device_block_t *local_block = NULL;
    access_local_regmem_id_t regmem_i = -1;
    access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
    int append = FALSE, parse_register_name_rv;
    int console_was_on = 0, console_disabled = 0, pushed_ctrl_c = 0, dwc_write = 0, direct_mode = 0,
        first_array_ind, last_array_ind, first_inst_ind, last_inst_ind, field_arguments_index = 0;
    parse_table_t pt;
    jmp_buf ctrl_c;
    char *fname = "", *name = NULL, *reg_name = NULL, *s = NULL, *next_field_str = NULL;
    uint32 data[30], regaddr = 0, regval[APPL_ACCESS_MAX_REG_UINT32S] = { 0 }, block_inst_sbus_id = -1;
    uint16 array_index = -1;
    bcm_gport_t port;
    bcm_port_config_t port_config;

    SHR_FUNC_INIT_VARS(unit);

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "File", PQ_STRING, 0, &fname, 0);
    parse_table_add(&pt, "Append", PQ_BOOL, 0, &append, FALSE);

    if (!sh_check_attached(ARG_CMD(a), unit))
    {
        SHR_SET_CURRENT_ERR(SOC_E_PARAM);
        SHR_EXIT();
    }
    if (parse_arg_eq(a, &pt) < 0)
    {
        SHR_SET_CURRENT_ERR(SOC_E_PARAM);
        SHR_EXIT();
    }

    console_was_on = bslcons_is_enabled();

    if (fname[0] != 0)
    {
        /*
         * Catch control-C in case if using file output option. 
         */
#ifndef NO_CTRL_C
        if (setjmp(ctrl_c))
        {
            SHR_SET_CURRENT_ERR(SOC_E_PARAM);
            SHR_EXIT();
        }
#endif
        sh_push_ctrl_c(&ctrl_c);
        pushed_ctrl_c = TRUE;

        if (bslfile_is_enabled())
        {
            LOG_INFO(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit, "%s: Can't dump to file while logging is enabled\n"), ARG_CMD(a)));
            SHR_SET_CURRENT_ERR(SOC_E_PARAM);
            SHR_EXIT();
        }

        if (bslfile_open((char *) fname, append) < 0)
        {
            LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "%s: Could not start log file\n"), ARG_CMD(a)));
            SHR_SET_CURRENT_ERR(SOC_E_PARAM);
            SHR_EXIT();
        }

        bslcons_enable(FALSE);
        console_disabled = 1;
    }

    bsl_log_start(BSL_APPL_SHELL, bslSeverityNormal, unit, "");

    for (name = ARG_GET(a); name != NULL; name = ARG_GET(a))
    {
        if (!sal_strncasecmp(name, "cmic_blk", sizeof("cmic_blk")))
        {
            name = ARG_GET(a);
            if (name != NULL && isint(name))
            {
                block_inst_sbus_id = parse_integer(name);
            }
            else
            {
                SHR_EXIT_WITH_LOG(SOC_E_PARAM,
                                  "ERROR: cmic_blk needs to be followed by an sbus block instance number %s%s%s\n",
                                  EMPTY, EMPTY, EMPTY);
            }
        }
        else if (!sal_strncasecmp(name, "direct", sizeof("direct")))
        {
            direct_mode = 1;
        }
        else if (!sal_strncasecmp(name, "data", sizeof("data")))
        {
            name = ARG_GET(a);
            while (name != NULL && 0 != sal_strcasecmp(name, "dataend"))
            {
                if (isint(name))
                {
                    data[dwc_write] = parse_integer(name);
                    dwc_write++;
                    if (dwc_write >= 30)
                    {
                        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "Error: Data is longer than 30 %s%s%s\n", EMPTY, EMPTY, EMPTY);
                    }
                }
                name = ARG_GET(a);
                if (name == NULL)
                {
                    break;
                }
            }
        }
        else
        {
            reg_name = name;
            break;
        }
    }

    if (reg_name == NULL || name == NULL)
    {
        SHR_SET_CURRENT_ERR(SOC_E_PARAM);
        SHR_EXIT();
    }
    /*
     * The value of direct_mode may be changed from 0 to 1
     */
    if (1 == direct_mode)
    {
        if (!isint(reg_name) || block_inst_sbus_id == -1)
        {
            SHR_EXIT_WITH_LOG(SOC_E_PARAM,
                              "ERROR: direct mode needs to both have cmic_blk and receive an address of the regmem %s%s%s\n",
                              EMPTY, EMPTY, EMPTY);
        }
        else if (modify)
        {
            SHR_EXIT_WITH_LOG(SOC_E_PARAM,
                              "ERROR: cannot modify direct registers as we do not know their fields %s%s%s\n", EMPTY,
                              EMPTY, EMPTY);
        }
        /** Numerical address given */
        regaddr = parse_integer(reg_name);
        name = ARG_GET(a);
        if (name == NULL)
        {
            SHR_EXIT_WITH_LOG(SOC_E_PARAM, "Missing value to assign to field %s%s%s\n", EMPTY, EMPTY, EMPTY);
            SHR_SET_CURRENT_ERR(SOC_E_PARAM);
            SHR_EXIT();
        }
        parse_long_integer(regval, APPL_ACCESS_MAX_REG_UINT32S, name);
        SHR_IF_ERR_EXIT(access_direct_regmem(unit, FLAG_ACCESS_IS_WRITE, block_inst_sbus_id, regaddr, dwc_write, data));
    }
    else if (isint(reg_name))
    {
        SHR_EXIT_WITH_LOG(SOC_E_PARAM,
                          "Accesing register by address also requires direct and cmic_blk arguments %s%s%s\n", EMPTY,
                          EMPTY, EMPTY);
    }

    /*
     * Symbolic name given, set all or some values ... 
     */
    parse_register_name_rv = access_parse_register_name(runtime_info, reg_name, &array_index, &inst_i, &regmem_i);

    /** If access_parse_register_name returns -1, there is an error/failure from the user input */
    if (parse_register_name_rv < 0)
    {
        SHR_SET_CURRENT_ERR(SOC_E_PARAM);
        SHR_EXIT();
    }

    /** If access_parse_register_name returns 1, there is no exact match of register name */
    if (parse_register_name_rv == 1)
    {
        /** Assume we have a partial name of a register */
        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "Unknown register %s%s%s\n", reg_name, EMPTY, EMPTY);
    }
    /** Register to access */
    rm_info = device_info->local_regs + regmem_i;

    local_block = device_info->blocks + rm_info->local_block_id;

    if (inst_i == ACCESS_ALL_BLOCK_INSTANCES)
    {
        first_inst_ind = 0;
        last_inst_ind = local_block->nof_instances - 1;
    }
    else
    {
        first_inst_ind = last_inst_ind = inst_i;
    }
    if (array_index == (uint16)(-1))
    {
        first_array_ind = rm_info->first_array_index;
        last_array_ind = rm_info->nof_array_indices + rm_info->first_array_index - 1;
    }
    else
    {
        first_array_ind = last_array_ind = array_index;
    }

    /** In modreg mode, read the current register value, and modify it. */
    /** In setreg mode, assume a starting value of zero and modify it. */
    if (!modify)
    {
        /*
         * Parse the register value
         */
        if ((s = ARG_GET(a)) == NULL)
        {
            SHR_EXIT_WITH_LOG(SOC_E_PARAM, "Syntax error: missing value %s%s%s\n", EMPTY, EMPTY, EMPTY);
        }
        if (isint(s))
        {       /* s is numeric */
            parse_long_integer(regval, APPL_ACCESS_MAX_REG_UINT32S, s);
            s = ARG_GET(a);
        }
        else
        {
            /** loop over remaining arguments which should be field values */
            for (; s != NULL; s = ARG_GET(a))
            {
                do
                {
                    /*
                     * Parse fields in the argument 
                     */
                    next_field_str = sal_strnchr(s, ',', SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
                    if (next_field_str)
                    {   /* Point to next field in this argument */
                        *next_field_str++ = 0;  /* Now s holds one field */
                    }
                    if (s[0] == 0)
                    {
                        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "Empty field specification %s%s%s\n", EMPTY, EMPTY, EMPTY);
                    }
                    SHR_IF_ERR_EXIT(diag_dnxc_parse_and_set_regmem_field(runtime_info, regmem_i, regval, s));
                    s = next_field_str;
                }
                while (next_field_str);
            }
        }
    }
    else
    {
        /** For modreg store the start of the field arguments to be parsed for every array index */
        field_arguments_index = ARG_GET_CURRENT_INDEX(a);
    }
    if ((rm_info->flags & ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK) == ACCESS_REGMEM_FLAG_ACCESS_TYPE_PER_PORT)
    {
        if (inst_i == ACCESS_ALL_BLOCK_INSTANCES) 
        {
            SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));
            BCM_PBMP_ITER(port_config.nif, port)
            {
                uint32 phy_port = 0;
                int block_inst_id = -1;
                access_local_block_id_t local_block_id;
                phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                block_inst_id = device_info->port_info[phy_port].blk; /* get the local block instance ID for phy_port */
                if (block_inst_id >= 0 && block_inst_id < device_info->nof_block_instances) { /* if found valid port_info data */
                    local_block_id = device_info->block_instances[block_inst_id].block_id;
                    if (rm_info->local_block_id == local_block_id || (rm_info->local_block_id == device_info->blocks_global2local_map[BLOCK_CDMAC] && local_block_id == device_info->blocks_global2local_map[BLOCK_CDPORT]) || (rm_info->local_block_id == device_info->blocks_global2local_map[BLOCK_CLMAC] && local_block_id == device_info->blocks_global2local_map[BLOCK_CLPORT]) || (rm_info->local_block_id == device_info->blocks_global2local_map[BLOCK_DC3MAC] && local_block_id == device_info->blocks_global2local_map[BLOCK_DC3PORT])) /* may need another blocktypes for future devices */
                    {
                        for (array_index = first_array_ind; array_index <= last_array_ind; array_index++)
                        {
                            if (modify)
                            {
                                SHR_IF_ERR_EXIT(access_local_regmem(runtime_info, 0, regmem_i, port, array_index, 0, regval));
                                /** Loop over the field arguments changing the read value */
                                ARG_SET_CURRENT_INDEX(a, field_arguments_index);
                                for (s = ARG_GET(a); s != NULL; s = ARG_GET(a))
                                {
                                    if (isint(s))
                                    {   /* s is numeric */
                                        if (modify)
                                        {
                                            SHR_EXIT_WITH_LOG(SOC_E_PARAM,
                                                            "ERROR: For modify must specify one or more fields in format <FIELD=VALUE> %s%s%s\n",
                                                            EMPTY, EMPTY, EMPTY);
                                        }
                                    }
                                    do
                                    {
                                        /*
                                        * Parse fields in the argument 
                                        */
                                        next_field_str = sal_strnchr(s, ',', SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
                                        if (next_field_str)
                                        {       /* Point to next field in this argument */
                                            *next_field_str = 0;        /* Now s holds one field */
                                        }
                                        if (s[0] == 0)
                                        {
                                            SHR_EXIT_WITH_LOG(SOC_E_PARAM, "Empty field specification %s%s%s\n", EMPTY, EMPTY, EMPTY);
                                        }
                                        else
                                        {
                                            SHR_IF_ERR_EXIT(diag_dnxc_parse_and_set_regmem_field(runtime_info, regmem_i, regval, s));
                                        }
                                        if (next_field_str)
                                        {
                                            *next_field_str++ = ',';
                                        }
                                        s = next_field_str;
                                    }
                                    while (next_field_str);
                                }
                            }
                            /** Write to the register */
                            SHR_IF_ERR_EXIT(access_local_regmem(runtime_info, FLAG_ACCESS_IS_WRITE, regmem_i, port, array_index, 0, regval));
                        }
                    } 
                }
            }
        }
        else
        {
            for (array_index = first_array_ind; array_index <= last_array_ind; array_index++)
            {
                if (modify)
                {
                    SHR_IF_ERR_EXIT(access_local_regmem(runtime_info, 0, regmem_i, inst_i, array_index, 0, regval));
                    /** Loop over the field arguments changing the read value */
                    ARG_SET_CURRENT_INDEX(a, field_arguments_index);
                    for (s = ARG_GET(a); s != NULL; s = ARG_GET(a))
                    {
                        if (isint(s))
                        {   /* s is numeric */
                            if (modify)
                            {
                                SHR_EXIT_WITH_LOG(SOC_E_PARAM,
                                                "ERROR: For modify must specify one or more fields in format <FIELD=VALUE> %s%s%s\n",
                                                EMPTY, EMPTY, EMPTY);
                            }
                        }
                        do
                        {
                            /*
                            * Parse fields in the argument 
                            */
                            next_field_str = sal_strnchr(s, ',', SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
                            if (next_field_str)
                            {       /* Point to next field in this argument */
                                *next_field_str = 0;        /* Now s holds one field */
                            }
                            if (s[0] == 0)
                            {
                                SHR_EXIT_WITH_LOG(SOC_E_PARAM, "Empty field specification %s%s%s\n", EMPTY, EMPTY, EMPTY);
                            }
                            else
                            {
                                SHR_IF_ERR_EXIT(diag_dnxc_parse_and_set_regmem_field(runtime_info, regmem_i, regval, s));
                            }
                            if (next_field_str)
                            {
                                *next_field_str++ = ',';
                            }
                            s = next_field_str;
                        }
                        while (next_field_str);
                    }
                }
                /** Write to the register */
                SHR_IF_ERR_EXIT(access_local_regmem(runtime_info, FLAG_ACCESS_IS_WRITE, regmem_i, inst_i, array_index, 0, regval));
            }
        }
    }
    else
    {
        /** Loop over instances and array indices to write the register to each one */
        for (inst_i = first_inst_ind; inst_i <= last_inst_ind; inst_i++)
        {
            for (array_index = first_array_ind; array_index <= last_array_ind; array_index++)
            {
                if (modify)
                {
                    SHR_IF_ERR_EXIT(access_local_regmem(runtime_info, 0, regmem_i, inst_i, array_index, 0, regval));
                    /** Loop over the field arguments changing the read value */
                    ARG_SET_CURRENT_INDEX(a, field_arguments_index);
                    for (s = ARG_GET(a); s != NULL; s = ARG_GET(a))
                    {
                        if (isint(s))
                        {   /* s is numeric */
                            if (modify)
                            {
                                SHR_EXIT_WITH_LOG(SOC_E_PARAM,
                                                "ERROR: For modify must specify one or more fields in format <FIELD=VALUE> %s%s%s\n",
                                                EMPTY, EMPTY, EMPTY);
                            }
                        }
                        do
                        {
                            /*
                            * Parse fields in the argument 
                            */
                            next_field_str = sal_strnchr(s, ',', SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
                            if (next_field_str)
                            {       /* Point to next field in this argument */
                                *next_field_str = 0;        /* Now s holds one field */
                            }
                            if (s[0] == 0)
                            {
                                SHR_EXIT_WITH_LOG(SOC_E_PARAM, "Empty field specification %s%s%s\n", EMPTY, EMPTY, EMPTY);
                            }
                            else
                            {
                                SHR_IF_ERR_EXIT(diag_dnxc_parse_and_set_regmem_field(runtime_info, regmem_i, regval, s));
                            }
                            if (next_field_str)
                            {
                                *next_field_str++ = ',';
                            }
                            s = next_field_str;
                        }
                        while (next_field_str);
                    }
                }
                /** Write to the register */
                SHR_IF_ERR_EXIT(access_local_regmem
                                (runtime_info, FLAG_ACCESS_IS_WRITE, regmem_i, inst_i, array_index, 0, regval));
            }
        }
    }
exit:
    if (fname[0] != 0)
    {
        bslfile_close();
    }

    if (console_disabled && console_was_on)
    {
        bslcons_enable(TRUE);
    }

    if (pushed_ctrl_c)
    {
        sh_pop_ctrl_c();
    }

    parse_arg_eq_done(&pt);
    SHR_FUNC_EXIT;
}

char cmd_dnxc_setreg_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "1. Parameters: <REGISTER>[[array_index]][.block_instance] <VALUE>\n\t"
    "<REGISTER> is symbolic name.\n"
    "2. Parameters: <REGISTER>[[array_index]][.block_instance] <FIELD>=<VALUE>[,...]\n\t"
    "<REGISTER> is symbolic name.\n\t"
    "<FIELD>=<VALUE>[,...] is a list of fields to set,\n\t"
    "for example: L3_ENA=0,CPU_PRI=1.\n\t"
    "<VALUE>\n\t" "Full documentation cannot be displayed with pendantic compiler\n"
    "3. Parameters: direct data <words to read> dataend cmic_blk <block_instance_sbus_id> <REGISTER_ADDRESS>\n";
#else
    "1. Parameters: <REGISTER>[[array_index]][.block_instance] <VALUE>\n\t"
    "<REGISTER> is symbolic name.\n"
    "2. Parameters: <REGISTER>[[array_index]][.block_instance] <FIELD>=<VALUE>[,...]\n\t"
    "<REGISTER> is symbolic name.\n\t"
    "<FIELD>=<VALUE>[,...] is a list of fields to affect,\n\t"
    "for example: L3_ENA=0,CPU_PRI=1.\n\t"
    "3. Parameters: cmic_blk <cmic_blk> <REGISTER_ADDRESS> "
    "<VALUE>\n\t"
    "Fields not specified in the list are set to zero.\n\t"
    "In case of register arrays specified by name(symbolically), specify idx to select array index.\n\t"
    "For accessing sbus register using his address and block instance number, use the 3nd form of the command (direct)\".\n";
#endif
/**
 * Implementation of the Setreg shell command, set register value
 */
cmd_result_t
access_cmd_dnxc_setreg(
    int unit,
    args_t *a)
{
    return diag_dnxc_do_setreg(unit, a, 0) == SOC_E_NONE ? CMD_OK : CMD_USAGE;
}

char cmd_dnxc_modreg_usage[] =
    "1. Parameters: <REGISTER>[[array_index]][.block_instance] <FIELD>=<VALUE>[,...]\n\t"
    "<REGISTER> is symbolic name.\n\t"
    "<FIELD>=<VALUE>[,...] is a list of fields to affect,\n\t"
    "for example: L3_ENA=0,CPU_PRI=1.\n\t"
    "2. Parameters: direct data <words to read> dataend cmic_blk <block_instance_sbus_id> <REGISTER_ADDRESS>\n\t"
    "Fields not specified in the list are left unaffected.\n\t"
    "In case of register arrays specified by name(symbolically), specify idx to select array index.\n\t"
    "For accessing sbus register using his address and block instance number, use the 3nd form of the command (direct)\".\n";

/**
 * Implementation of the Modreg shell command, Read/Modify/Write register
 */
cmd_result_t
access_cmd_dnxc_modreg(
    int unit,
    args_t *a)
{
    return diag_dnxc_do_setreg(unit, a, 1) == SOC_E_NONE ? CMD_OK : CMD_USAGE;
}

/*
 * Auxilliary routine to handle write and modify.
 *      if block instance (inst_i) is specified, apply the command only for this instance or
 * if it is not specified, apply the command for all block instances.
 *      for register arrays - if array index (array_index) is specified, apply the command only for this index or
 * if it is not specified, apply the command for all indices.
 *      mod should be 0 for write, which takes either a value or a
 *              list of <field>=<value>, and in the latter case, sets all
 *              non-specified fields to zero.
 *      mod should be 1 for modify, which does a read-modify-write of
 *              the register and permits value to be specified by a list
 *              of <field>=<value>[,...] only.
 */
static soc_error_t
diag_dnxc_writemem(
    int unit,
    args_t *a,
    int modify)
{
    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); /** Access information for the device */
    const access_device_type_info_t *device_info = runtime_info->device_type_info; /**  Constant access information for the device type */
    const access_device_regmem_t *rm_info = NULL, *mem_aliased_to;
    const access_device_block_t *local_block = NULL;
    access_local_regmem_id_t regmem_i = -1;
    access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
    int append = FALSE, i, start = 0, count, parse_memory_name_rv, entry_dw = 0,
        console_was_on = 0, console_disabled = 0, pushed_ctrl_c = 0, field_arguments_index = 0,
        first_array_ind, last_array_ind, first_inst_ind, last_inst_ind, no_cache = 0, force = 0;
    parse_table_t pt;
    jmp_buf ctrl_c;
    char *fname = "", *tab = NULL, *s = NULL, *mem_name = NULL, *next_field_str = NULL;
    uint32 entry[APPL_ACCESS_MAX_REG_UINT32S] = { 0 }, memory_index = -1;
    uint16 array_index = -1;

    SHR_FUNC_INIT_VARS(unit);

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "File", PQ_STRING, 0, &fname, 0);
    parse_table_add(&pt, "Append", PQ_BOOL, 0, &append, FALSE);

    if (!sh_check_attached(ARG_CMD(a), unit))
    {
        SHR_SET_CURRENT_ERR(SOC_E_PARAM);
        SHR_EXIT();
    }
    if (parse_arg_eq(a, &pt) < 0)
    {
        SHR_SET_CURRENT_ERR(SOC_E_PARAM);
        SHR_EXIT();
    }

    console_was_on = bslcons_is_enabled();

    if (fname[0] != 0)
    {
        /*
         * Catch control-C in case if using file output option. 
         */
#ifndef NO_CTRL_C
        if (setjmp(ctrl_c))
        {
            SHR_SET_CURRENT_ERR(SOC_E_PARAM);
            SHR_EXIT();;
        }
#endif
        sh_push_ctrl_c(&ctrl_c);
        pushed_ctrl_c = TRUE;

        if (bslfile_is_enabled())
        {
            LOG_INFO(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit, "%s: Can't dump to file while logging is enabled\n"), ARG_CMD(a)));
            SHR_SET_CURRENT_ERR(SOC_E_PARAM);
            SHR_EXIT();
        }

        if (bslfile_open((char *) fname, append) < 0)
        {
            LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "%s: Could not start log file\n"), ARG_CMD(a)));
            SHR_SET_CURRENT_ERR(SOC_E_PARAM);
            SHR_EXIT();
        }

        bslcons_enable(FALSE);
        console_disabled = 1;
    }

    bsl_log_start(BSL_APPL_SHELL, bslSeverityNormal, unit, "");

    for (tab = ARG_GET(a); tab != NULL; tab = ARG_GET(a))
    {
        if (!sal_strcasecmp(tab, "nocache"))
        {
            no_cache = 1;
        }
        if (!sal_strcasecmp(tab, "force"))
        {
            force = 1;
        }
        else
        {
            mem_name = tab;
            tab = ARG_GET(a);
            if (tab != NULL)
            {
                if (isint(tab))
                {
                    start = parse_integer(tab);
                    tab = ARG_GET(a);
                    if (tab != NULL)
                    {
                        if (isint(tab))
                        {
                            count = parse_integer(tab) - 1;
                        }
                        else
                        {
                            SHR_EXIT_WITH_LOG(SOC_E_PARAM, "Error: count must be an integer value %s%s%s\n", EMPTY,
                                              EMPTY, EMPTY);
                        }
                    }
                    else
                    {
                        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "Error: must specify the count %s%s%s\n", EMPTY, EMPTY, EMPTY);
                    }
                }
                else
                {
                    SHR_EXIT_WITH_LOG(SOC_E_PARAM, "Error: index must be an integer value %s%s%s\n", EMPTY, EMPTY,
                                      EMPTY);
                }
            }
            else
            {
                SHR_EXIT_WITH_LOG(SOC_E_PARAM, "Error: must specify an index %s%s%s\n", EMPTY, EMPTY, EMPTY);
            }
            break;
        }
    }

    if (mem_name == NULL)
    {
        SHR_SET_CURRENT_ERR(SOC_E_PARAM);
        SHR_EXIT();
    }

    /*
     * Symbolic name given
     */
    parse_memory_name_rv = access_parse_memory_name(runtime_info, mem_name, &array_index, &inst_i, &regmem_i);

    /** If accesss_parse_memory_name returns -1, there is an error/failure from the user input */
    if (parse_memory_name_rv < 0)
    {
        SHR_SET_CURRENT_ERR(SOC_E_PARAM);
        SHR_EXIT();
    }

    /** If accesss_parse_memory_name returns 1, there is no exact match of memory name */
    if (parse_memory_name_rv == 1)
    {
        /** Assume we have a partial name of a memory */
        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "Unknown memory %s%s%s\n", mem_name, EMPTY, EMPTY);
    }
    /** Memory to access */
    rm_info = device_info->local_regs + regmem_i;

    local_block = device_info->blocks + rm_info->local_block_id;

    if (inst_i == ACCESS_ALL_BLOCK_INSTANCES)
    {
        if (modify)
        {
            first_inst_ind = 0;
            last_inst_ind = local_block->nof_instances - 1;
        }
        else
        {
            first_inst_ind = last_inst_ind = ACCESS_ALL_BLOCK_INSTANCES;
        }
    }
    else
    {
        first_inst_ind = last_inst_ind = inst_i;
    }
    if (array_index == (uint16)(-1))
    {
        first_array_ind = rm_info->first_array_index;
        last_array_ind = rm_info->nof_array_indices + rm_info->first_array_index - 1;
    }
    else
    {
        first_array_ind = last_array_ind = array_index;
    }

    if (modify == 1 && (rm_info->flags & ACCESS_REGMEM_FLAG_WO) && !force)
    {
        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "ERROR: Memory %s is write-only %s%s\n",
                          access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]], EMPTY, EMPTY);
    }

    if ((rm_info->flags & ACCESS_REGMEM_FLAG_RO) && !force)
    {
        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "ERROR: Memory %s is read-only %s%s\n",
                          access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]], EMPTY, EMPTY);
    }

    if ((rm_info->flags & ACCESS_REGMEM_FLAG_IS_ALIAS) != 0)
    {
        mem_aliased_to = device_info->local_regs + rm_info->u.mem.alias_regmem;
    }
    else
    {
        mem_aliased_to = rm_info;
    }

    /** Check count and index range */
    if (start < 0 || count < 0 || start >= mem_aliased_to->u.mem.mem_nof_elements
        || start + count >= mem_aliased_to->u.mem.mem_nof_elements)
    {
        SHR_EXIT_WITH_LOG(SOC_E_PARAM,
                          "Error: Memory index %d or count %d do not specify a legal sub range of indices 0..%u\n",
                          start, count - 1, mem_aliased_to->u.mem.mem_nof_elements);
    }

    sal_memset(entry, 0, sizeof(entry));
    entry_dw = (mem_aliased_to->width_in_bits + 31) / 32;
    /*
     * If a list of fields were specified, generate the entry from them.
     * Otherwise, generate it by reading raw dwords from command line.
     */
    if (!modify)
    {
        if ((s = ARG_GET(a)) == NULL)
        {
            SHR_EXIT_WITH_LOG(SOC_E_PARAM, "Syntax error: missing value %s%s%s\n", EMPTY, EMPTY, EMPTY);
        }
        if (isint(s))
        {
            /*
             * List of numeric values 
             */
            for (i = 0; i < entry_dw; i++, s = ARG_GET(a))
            {
                if (s == NULL)
                {
                    SHR_EXIT_WITH_LOG(SOC_E_PARAM, "Not enough data values (have %d, need %d) %s\n", i, entry_dw,
                                      EMPTY);
                }
                entry[i] = parse_integer(s);
            }

            if (s != NULL)
            {
                LOG_INFO(BSL_LS_APPL_COMMON,
                         (BSL_META_U(unit, "Ignoring extra data on command line (only %d words needed)\n"), entry_dw));
            }
        }
        else
        {
            /** loop over remaining arguments which should be field values */
            for (; s != NULL; s = ARG_GET(a))
            {
                do
                {
                    /*
                     * Parse fields in the argument 
                     */
                    next_field_str = sal_strnchr(s, ',', SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
                    if (next_field_str)
                    {   /* Point to next field in this argument */
                        *next_field_str++ = 0;  /* Now s holds one field */
                    }
                    if (s[0] == 0)
                    {
                        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "Empty field specification %s%s%s\n", EMPTY, EMPTY, EMPTY);
                    }
                    SHR_IF_ERR_EXIT(diag_dnxc_parse_and_set_regmem_field(runtime_info, regmem_i, entry, s));
                    s = next_field_str;
                }
                while (next_field_str);
            }
        }
    }
    else
    {
        /** For modify store the start of the field arguments to be parsed for every index */
        field_arguments_index = ARG_GET_CURRENT_INDEX(a);
    }

    for (inst_i = first_inst_ind; inst_i <= last_inst_ind; inst_i != ACCESS_ALL_BLOCK_INSTANCES ? inst_i++ : last_inst_ind--)
    {
        for (array_index = first_array_ind; array_index <= last_array_ind; array_index++)
        {
            for (memory_index = start; memory_index <= start + count; memory_index++)
            {
                if (modify)
                {
                    SHR_IF_ERR_EXIT(access_local_regmem(runtime_info, 0, regmem_i, inst_i, array_index, memory_index, entry));
                    /** Loop over the field arguments changing the read value */
                    ARG_SET_CURRENT_INDEX(a, field_arguments_index);
                    for (s = ARG_GET(a); s != NULL; s = ARG_GET(a))
                    {
                        if (isint(s))
                        {   /* s is numeric */
                            SHR_EXIT_WITH_LOG(SOC_E_PARAM,
                                                  "ERROR: For modify must specify one or more fields in format <FIELD=VALUE> %s%s%s\n",
                                                  EMPTY, EMPTY, EMPTY);
                        }
                        do
                        {
                            /*
                             * Parse fields in the argument 
                             */
                            next_field_str = sal_strnchr(s, ',', SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
                            if (next_field_str)
                            {   /* Point to next field in this argument */
                                *next_field_str = 0;    /* Now s holds one field */
                            }
                            if (s[0] == 0)
                            {
                                SHR_EXIT_WITH_LOG(SOC_E_PARAM, "Empty field specification %s%s%s\n", EMPTY, EMPTY,
                                                  EMPTY);
                            }
                            SHR_IF_ERR_EXIT(diag_dnxc_parse_and_set_regmem_field
                                                (runtime_info, regmem_i, entry, s));
                            if (next_field_str)
                            {
                                *next_field_str++ = ',';
                            }
                            s = next_field_str;
                        }
                        while (next_field_str);
                    }
                }

                /** Write to the memory */
                SHR_IF_ERR_EXIT(access_local_regmem(runtime_info, no_cache ? FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_DO_NOT_USE_CACHE : FLAG_ACCESS_IS_WRITE, regmem_i, inst_i, array_index, memory_index, entry));
            }
        }
    }
exit:
    if (fname[0] != 0)
    {
        bslfile_close();
    }

    if (console_disabled && console_was_on)
    {
        bslcons_enable(TRUE);
    }

    if (pushed_ctrl_c)
    {
        sh_pop_ctrl_c();
    }

    parse_arg_eq_done(&pt);
    SHR_FUNC_EXIT;
}

char cmd_dnxc_writemem_usage[] =
    "WRITE [force] <TABLE>[.<COPY>] <ENTRY> <ENTRYCOUNT>\n\t"
    "        { <DW0> .. <DWN> | <FIELD>=<VALUE>[,...] }\n\t"
    "Number of <DW> must be a multiple of table entry size.\n\t"
    "If force is specified, attempt the operation even if the memory does not support it\n\t "
    "Writes value to the given indices of the specified memory (table).\n";

/**
 * Implementation of the Write shell command, Write entry(s) into a table
 */
cmd_result_t
access_cmd_dnxc_writemem(
    int unit,
    args_t *a)
{
    return diag_dnxc_writemem(unit, a, 0) == SOC_E_NONE ? CMD_OK : CMD_USAGE;
}

char cmd_dnxc_modifymem_usage[] =
    "MODIFY [force] <TABLE>[.<COPY>] <ENTRY> <ENTRYCOUNT>\n\t"
    "       <FIELD>=<VALUE>[,...]\n\t"
    "If force is specified, attempt the operation even if the memory does not support it\n\t "
    "Read-modify-write fields of memory (table) entries.\n";

/**
 * Implementation of the MODify shell command, Modify table entry by field names
 */
cmd_result_t
access_cmd_dnxc_modifymem(
    int unit,
    args_t *a)
{
    return diag_dnxc_writemem(unit, a, 1) == SOC_E_NONE ? CMD_OK : CMD_USAGE;
}

#endif /* BCM_ACCESS_SUPPORT */
