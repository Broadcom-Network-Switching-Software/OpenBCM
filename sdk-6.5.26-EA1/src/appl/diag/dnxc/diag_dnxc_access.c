
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
#ifdef BCM_ACCESS_SUPPORT
#include <soc/access/access.h>
#endif /* BCM_ACCESS_SUPPORT */

#include <soc/sand/shrextend/shrextend_debug.h>

#include <appl/diag/sand/diag_sand_utils.h>

#define APPL_ACCESS_MAX_MEM_UINT32S 256

#ifdef BCM_ACCESS_SUPPORT
/*
 * Find a memory whose name is mem_name.
 * Return local regmem ID and array index.
 * On success, stores the local regmem ID at out_mem_id and array index in array_index and returns 0. On failure, returns -1.
 *
 * For dump and write commands a memory array index may be added after the memory name in square brackets like: mem_name[3].
 * The function will fail for memory arrays for which an array index
 * is not specified when array_index is non-NULL.
 * The function will fail for non array memories for which an array index is specified.
 * For non array memories, array_index may be NULL; If not Null 0 will be returned in it.
 */

int
access_parse_memory_name(
    access_runtime_info_t * runtime_info,
    char *mem_name,
    unsigned *array_index,
    access_local_regmem_id_t * out_mem_id)
{
    const access_device_type_info_t *device_info = runtime_info->device_type_info; /** constant access information for the device type */
    access_local_regmem_id_t mem_i = -1;
    const access_device_regmem_t *rm_info = NULL, *mem_aliased_to;
    char *close_bracket, /** the close bracket for the array index in mem_name */
       *open_bracket,   /** the open bracket for the array index in mem_name */
        tmp[80];
    unsigned arr_ind;

    if (mem_name == NULL || *mem_name == '\0')
    {
        return -1;
    }

    /** Do not destroy input string */
    sal_strncpy_s(tmp, mem_name, sizeof(tmp));

    /** check if an array index is specified */
    if ((((open_bracket = sal_strchr(tmp, '[')) != NULL) &&
         ((close_bracket = sal_strchr(open_bracket + 1, ']')) != NULL) && (close_bracket - open_bracket > 1)) ||
        (((open_bracket = sal_strchr(tmp, '(')) != NULL) &&
         ((close_bracket = sal_strchr(open_bracket + 1, ')')) != NULL) && (close_bracket - open_bracket > 1)))
    {
        if (array_index == NULL)
        {
            return -1;
        }
        /** handle memory array, handle initial spaces after '[' */
        for (close_bracket = open_bracket + 1; *close_bracket == ' '; ++close_bracket)
        {
        }
        if (*close_bracket < '0' || *close_bracket > '9')
        {
            return -1; /** error: bad array index character */
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
        {                                                         /** verify closing ']' */
            return -1; /** error: bad array index character */
        }
        *array_index = arr_ind;
        close_bracket = sal_strchr(close_bracket + 1, '.');
        *open_bracket = '\0';
    }
    else
    {
        arr_ind = 0xffffffff;
        close_bracket = sal_strchr(tmp, '.');
        if (array_index != NULL)
        {
            *array_index = 0;
        }
    }

    if (close_bracket != NULL)
    {
        *close_bracket++ = '\0';
    }

    /** loop over all memories */
    for (mem_i = device_info->nof_regs; mem_i < device_info->nof_regs + device_info->nof_mems; mem_i++)
    {
        /** Memory to access */
        rm_info = device_info->local_regs + mem_i;
        /** find by name */
        if (sal_strcasecmp(tmp, access_global_names[rm_info->name_index]) == 0)
        {
            break;
        }
    }
    if (mem_i == device_info->nof_regs + device_info->nof_mems)
    {                                                             /** the specified memory is not found */
        return -1;
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
    else if (array_index != NULL && mem_aliased_to->nof_array_indices != 1)
    {
        /** no array index specified, but this is a memory array */
        cli_out("ERROR: an array index was not specified for a memory array\n");
        return -1;
    }

    *out_mem_id = rm_info - device_info->local_regs;

    return 0;
}

/*
 * Print a one line summary for matching memories
 * If substr_match is NULL, match all memories.
 * If substr_match is non-NULL, match any memories whose name
 * or user-friendly name contains that substring.
 */
STATIC void
access_dnxc_mem_list_summary(
    access_runtime_info_t * runtime_info,
    const char *substr_match,
    uint32 flags_to_be_set,
    uint32 flags_to_be_clear)
{
    int unit = runtime_info->unit;
    const access_device_type_info_t *device_info = runtime_info->device_type_info;  /** Constant access information for the device type */
    const access_device_block_t *local_block;
    const access_device_regmem_t *rm_info;
    int nof_instances, dlen;
    int found = 0;
    char *dstr;
    access_local_regmem_id_t mem_i;

    bsl_log_start(BSL_APPL_SHELL, bslSeverityNormal, unit, "");
    /** Loop over all memories */
    for (mem_i = device_info->nof_regs; mem_i < device_info->nof_regs + device_info->nof_mems; mem_i++)
    {
        /** Memory to access */
        rm_info = device_info->local_regs + mem_i;
        /** Filter by flags */
        if ((rm_info->flags & flags_to_be_set) != flags_to_be_set)
        {
            continue;
        }
        if (substr_match != NULL && strcaseindex(access_global_names[rm_info->name_index], substr_match) == NULL)
        {
            continue;
        }

        local_block = device_info->blocks + rm_info->local_block_id;
        nof_instances = local_block->nof_instances;

        dlen = strlen(rm_info->description);
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
                    access_global_names[rm_info->name_index], rm_info->u.mem.mem_nof_elements);
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

    if (found)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                    "Flags: (r)eadonly, (w)riteonly, (s)orted, (c)ounter\n" "       A(lias), (W)ide\n");
    }
    else if (substr_match != NULL)
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "No memory found with the substring '%s' in its name.\n"), substr_match));
    }
    bsl_log_end(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "");
}
#endif /* BCM_ACCESS_SUPPORT */

#ifdef BCM_ACCESS_SUPPORT
/** List the tables, or fields of a table entry */
cmd_result_t
access_cmd_dnxc_mem_list(
    access_runtime_info_t * runtime_info,
    args_t * a)
{
    int unit = runtime_info->unit;
    const access_device_type_info_t *device_info = runtime_info->device_type_info;  /** Constant access information for the device type */
    const access_device_block_t *local_block;
    const access_device_regmem_t *rm_info = NULL;
    const access_device_regmem_t *rm_info_alias;
    access_local_regmem_id_t regmem_alias;      /** The register or memory to access */
    const access_device_field_t *field_info;
    int have_entry, i, dw, disabled, minidx, maxidx;
    uint32 entry[APPL_ACCESS_MAX_MEM_UINT32S], flags;
    char *tab, *s;
    access_local_regmem_id_t regmem_i;
    access_local_block_id_t block_i;
    access_local_field_id_t fld;
    uint32 flags_to_be_set = 0, flags_to_be_clear = 0;
    const uint8 *block_instances_flags;

    tab = ARG_GET(a);

    
    if ((tab != NULL) && (sal_strcasecmp(tab, "cache-table") == 0))
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current enabled cached memories: \n");
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                    "This command does not support on this device.\n");
        return CMD_FAIL;
#if 0
        if (soc_mem_iterate(unit, diag_sand_mem_list_cache, NULL) < 0)
        {
            LOG_ERROR(BSL_LS_APPL_SOCMEM, (BSL_META_U(unit, "ERROR: Get all cache tables error.\n")));
            return CMD_FAIL;
        }
        return CMD_OK;
#endif
    }

    
    if ((tab != NULL) && (sal_strcasecmp(tab, "ser-action") == 0))
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current all SER action: \n");
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                    "This command does not support on this device.\n");
        return CMD_FAIL;
#if 0
        if (diag_sand_mem_list_ser_action(unit) < 0)
        {
            LOG_ERROR(BSL_LS_APPL_SOCMEM, (BSL_META_U(unit, "ERROR: List SER action error.\n")));
            return CMD_FAIL;
        }
        return CMD_OK;
#endif
    }

    if ((tab != NULL) && (sal_strcasecmp(tab, "dynamic-table") == 0))
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current dynamic memories: \n");
        flags_to_be_set |= ACCESS_REGMEM_FLAG_ASIC_CHANGABLE;
        tab = NULL;
    }
    if ((tab != NULL) && (sal_strcasecmp(tab, "read-write") == 0))
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current read and write memories: \n");
        flags_to_be_clear |= ACCESS_REGMEM_FLAG_RO | ACCESS_REGMEM_FLAG_WO;
        tab = NULL;
    }
    if ((tab != NULL) && (sal_strcasecmp(tab, "init-on-read") == 0))
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current init on read memories: \n");
        flags_to_be_set |= ACCESS_REGMEM_FLAG_INIT_ON_READ;
        tab = NULL;
    }
    if ((tab != NULL) && (sal_strcasecmp(tab, "clear-bits-on-write") == 0))
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current clear bits on write memories: \n");
        flags_to_be_set |= ACCESS_REGMEM_FLAG_CLEAR_BITS_ON_WRITE;
        tab = NULL;
    }
    if ((tab != NULL) && (sal_strcasecmp(tab, "counter") == 0))
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current counter memories: \n");
        flags_to_be_set |= ACCESS_REGMEM_FLAG_COUNTER;
        tab = NULL;
    }
    if ((tab != NULL) && (sal_strcasecmp(tab, "alias") == 0))
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current alias memories: \n");
        flags_to_be_set |= ACCESS_REGMEM_FLAG_IS_ALIAS;
        tab = NULL;
    }
    if ((tab != NULL) && (sal_strcasecmp(tab, "type-1st-bar") == 0))
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                    "Current memories with access type 1st-bar: \n");
        flags_to_be_set |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_1ST_BAR;
        flags_to_be_clear |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK & ~ACCESS_REGMEM_FLAG_ACCESS_TYPE_1ST_BAR;
        tab = NULL;
    }
    if ((tab != NULL) && (sal_strcasecmp(tab, "type-2nd-bar") == 0))
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                    "Current memories with access type 2nd-bar: \n");
        flags_to_be_set |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_2ND_BAR;
        flags_to_be_clear |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK & ~ACCESS_REGMEM_FLAG_ACCESS_TYPE_2ND_BAR;
        tab = NULL;
    }
    if ((tab != NULL) && (sal_strcasecmp(tab, "wide") == 0))
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current wide memories: \n");
        flags_to_be_set |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_WIDE;
        flags_to_be_clear |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK & ~ACCESS_REGMEM_FLAG_ACCESS_TYPE_WIDE;
        tab = NULL;
    }
    if ((tab != NULL) && (sal_strcasecmp(tab, "access-per-port") == 0))
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current access per port memories: \n");
        flags_to_be_set |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_PER_PORT;
        flags_to_be_clear |= ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK & ~ACCESS_REGMEM_FLAG_ACCESS_TYPE_PER_PORT;
        tab = NULL;
    }
    if ((tab != NULL) && (sal_strcasecmp(tab, "no-protection") == 0))
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current no protected memories: \n");
        flags_to_be_set |= ACCESS_REGMEM_FLAG_HAS_NO_PROTECTION;
        flags_to_be_clear |= ACCESS_REGMEM_FLAG_CONTENT_PROTECTION_MASK & ~ACCESS_REGMEM_FLAG_HAS_NO_PROTECTION;
        tab = NULL;
    }
    if ((tab != NULL) && (sal_strcasecmp(tab, "has-ecc") == 0))
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current memories with ecc: \n");
        flags_to_be_set |= ACCESS_REGMEM_FLAG_HAS_ECC;
        flags_to_be_clear |= ACCESS_REGMEM_FLAG_CONTENT_PROTECTION_MASK & ~ACCESS_REGMEM_FLAG_HAS_ECC;
        tab = NULL;
    }
    if ((tab != NULL) && (sal_strcasecmp(tab, "has-parity") == 0))
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current memories with parity: \n");
        flags_to_be_set |= ACCESS_REGMEM_FLAG_HAS_PARITY;
        flags_to_be_clear |= ACCESS_REGMEM_FLAG_CONTENT_PROTECTION_MASK & ~ACCESS_REGMEM_FLAG_HAS_PARITY;
        tab = NULL;
    }

    if (!tab)
    {
        access_dnxc_mem_list_summary(runtime_info, NULL, flags_to_be_set, flags_to_be_clear);
        return CMD_OK;
    }

    if (access_parse_memory_name(runtime_info, tab, 0, &regmem_i) < 0)
    {
        if ((s = strchr(tab, '.')) != NULL)
        {
            *s = 0;
        }
        /** Assume we have a partial name of a memory */
        access_dnxc_mem_list_summary(runtime_info, tab, flags_to_be_set, flags_to_be_clear);
        return CMD_OK;
    }

    bsl_log_start(BSL_APPL_SHELL, bslSeverityNormal, unit, "");

    /** Memory to access */
    rm_info = device_info->local_mems + regmem_i;

    flags = rm_info->flags;

    dw = (rm_info->width_in_bits + 31) / 32;

    if ((s = ARG_GET(a)) == 0)
    {
        have_entry = 0;
    }
    else
    {
        for (i = 0; i < dw; i++)
        {
            if (s == 0)
            {
                LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Not enough data specified (%d words needed)\n"), dw));
                return CMD_FAIL;
            }
            entry[i] = parse_integer(s);
            s = ARG_GET(a);
        }
        if (s)
        {
            LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Extra data specified (ignored)\n")));
        }
        have_entry = 1;
    }
    local_block = device_info->blocks + rm_info->local_block_id;
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Memory: %s.%s",
                access_global_names[rm_info->name_index],
                access_global_block_types[local_block->block_type].block_name);

    if ((rm_info->flags & ACCESS_REGMEM_FLAG_IS_ALIAS) != 0)
    {
        regmem_alias = rm_info->u.mem.alias_regmem;
        rm_info_alias = device_info->local_regs + regmem_alias;
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " alias to %s \n",
                    access_global_names[rm_info_alias->name_index]);
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

    minidx = 0;
    maxidx = rm_info->u.mem.mem_nof_elements;
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Entries: %u with indices %d-%d (0x%x-0x%x)",
                rm_info->u.mem.mem_nof_elements, minidx, maxidx - 1, minidx, maxidx - 1);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ", each %d bytes %d words\n",
                (rm_info->width_in_bits + 7) / 8, dw);

    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Width in bits: %d\n", rm_info->width_in_bits);

    s = rm_info->description;
    if (s && *s)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Description: %s\n", s);
    }

    for (fld = rm_info->fields_index; fld < rm_info->fields_index + rm_info->nof_fields; fld++)
    {
        /** Field to access */
        field_info = device_info->local_fields + fld;
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "  %s<%u",
                    access_global_names[field_info->name_index], field_info->start_bit + field_info->size_in_bits - 1);
        if (field_info->size_in_bits > 1)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ":%u", field_info->start_bit);
        }
        if (have_entry)
        {
            uint32 fval[APPL_ACCESS_MAX_MEM_UINT32S];
            char tmp[132];

            access_local_field_get(runtime_info, fld, entry, fval);
            format_long_integer(tmp, fval, (field_info->size_in_bits + 31) / 32);
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "> = %s\n", tmp);
        }
        else
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ">\n");
        }
    }
    bsl_log_end(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "");
    return CMD_OK;
}
#endif /* BCM_ACCESS_SUPPORT */
