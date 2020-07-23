/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        diag_sand_access.c
 * Purpose:     Diag shell direct access commands
 */

#include <shared/bsl.h>

#include <sal/appl/sal.h>
#include <sal/limits.h>
#include <soc/drv.h>
#include <soc/intr.h>
#include <soc/sand/sand_aux_access.h>

#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_access.h>

#ifdef BCM_DPP_SUPPORT
#include <soc/dpp/port_sw_db.h>
#endif

#ifdef BCM_IPROC_SUPPORT
#include <soc/iproc.h>
#endif

#ifdef BCM_DNX_SUPPORT
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#endif /* BCM_DNX_SUPPORT */

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/** see description in header file */
shr_error_e
access_reg_read(
    int unit,
    soc_reg_t reg,
    soc_block_t block_id,
    int port,
    int reg_index,
    uint32 *value,
    uint32 *addr_p)
{
    int blk;
    uint8 acc_type;
    uint32 addr = 0;
    soc_reg_info_t *reginfo = &SOC_REG_INFO(unit, reg);

    SHR_FUNC_INIT_VARS(unit);

    memset(value, 0, sizeof(uint32) * SOC_REG_ABOVE_64_MAX_SIZE_U32);

    switch (reginfo->regtype)
    {
        case soc_cpureg:
            addr = soc_reg_addr_get(unit, reg, block_id, reg_index, SOC_REG_ADDR_OPTION_NONE, &blk, &acc_type);
            *value = soc_pci_read(unit, addr);
            break;
#ifdef BCM_IPROC_SUPPORT
        case soc_iprocreg:
            block_id = CMIC_BLOCK(unit);
            addr = soc_reg_addr_get(unit, reg, block_id, reg_index, SOC_REG_ADDR_OPTION_NONE, &blk, &acc_type);
            soc_iproc_getreg(unit, addr, value);
            break;
#endif /* BCM_IPROC_SUPPORT */
#ifdef BCM_CMICM_SUPPORT
        case soc_mcsreg:
            blk = CMIC_BLOCK(unit);
            addr = soc_reg_addr_get(unit, reg, block_id, reg_index, SOC_REG_ADDR_OPTION_NONE, &blk, &acc_type);
            *value = soc_pci_mcs_read(unit, addr);
            break;
#endif /* BCM_CMICM_SUPPORT */
        case soc_genreg:
            addr =
                soc_reg_addr_get(unit, reg, SOC_BLOCK_PORT(unit, block_id), reg_index, SOC_REG_ADDR_OPTION_NONE, &blk,
                                 &acc_type);
            SHR_IF_ERR_EXIT_WITH_LOG(soc_reg_above_64_get(unit, reg, SOC_BLOCK_PORT(unit, block_id), reg_index, value),
                                     "ERROR: read from general register %s(%d).%s failed\n", SOC_REG_NAME(unit, reg),
                                     reg_index, SOC_BLOCK_NAME(unit, block_id));
            break;
        case soc_portreg:
            SHR_IF_ERR_EXIT_WITH_LOG(soc_reg_above_64_get(unit, reg, port, reg_index, value),
                                     "ERROR: read from port register %s(%d) port:%d failed\n",
                                     SOC_REG_NAME(unit, reg), reg_index, port);
            break;
        case soc_customreg:
            SHR_IF_ERR_EXIT_WITH_LOG(soc_custom_reg_above_64_get(unit, reg, port, reg_index, value),
                                     "ERROR: read from custom register %s(%d) port:%d failed\n",
                                     SOC_REG_NAME(unit, reg), reg_index, port);
            break;
        default:
            goto exit;
            break;
    }

    if (addr_p != NULL)
    {
        *addr_p = addr;
    }
exit:
    SHR_FUNC_EXIT;
}

static int
access_reg_filter_no_read_fields(
    int unit,
    soc_reg_t reg)
{
    int i;
    uint32 disallowed_fields[] = { INDIRECT_COMMAND_COUNTf, INDIRECT_COMMAND_WR_DATAf, INTERRUPT_REGISTER_TESTf,
        ECC_INTERRUPT_REGISTER_TESTf, NUM_SOC_FIELD
    };

    for (i = 0; disallowed_fields[i] != NUM_SOC_FIELD; i++)
    {
        if (SOC_REG_FIELD_VALID(unit, reg, disallowed_fields[i]))
        {
            return TRUE;
        }
    }
    return FALSE;
}

static shr_error_e
access_reg_properties_get(
    int unit,
    soc_reg_t reg,
    char *prop_str,
    void *property_h)
{
    shr_error_e rv;
    uint32 flags;

    soc_reg_info_t *reginfo = &SOC_REG_INFO(unit, reg);

    flags = reginfo->flags;

    sal_memset(prop_str, 0, PRT_COLUMN_WIDTH_BIG);
    sal_strcpy(prop_str + sal_strlen(prop_str), "reg:");
    sal_sprintf(prop_str + sal_strlen(prop_str), " %s", soc_regtypenames[reginfo->regtype]);
    if (flags & SOC_REG_FLAG_ARRAY)
    {
        sal_sprintf(prop_str + sal_strlen(prop_str), " array[%d]", reginfo->numels);
    }
    if (flags & SOC_REG_FLAG_RO)
    {
        sal_strcat(prop_str, " ro");
    }
    if (flags & SOC_REG_FLAG_WO)
    {
        sal_strcat(prop_str, " wo");
    }
    if (flags & SOC_REG_FLAG_COUNTER)
    {
        sal_strcat(prop_str, " counter");
    }
    if (flags & SOC_REG_FLAG_ED_CNTR)
    {
        sal_strcat(prop_str, " err/discard");
    }
    if (shr_access_reg_no_read_get(unit, reg))
    {
        sal_strcat(prop_str, " no_read");
    }
    else
    {   /* Registers with "no_read" are automatically no_wb, no need to mention it implicitly */
        if (shr_access_reg_no_wb_get(unit, reg))
        {
            sal_strcat(prop_str, " no_wb");
        }
    }
    if ((property_h != NULL) && (diag_sand_compare(property_h, prop_str) == FALSE))
        rv = _SHR_E_NOT_FOUND;
    else
        rv = _SHR_E_NONE;
    return rv;
}

static shr_error_e
access_mem_properties_get(
    int unit,
    soc_mem_t mem,
    char *prop_str,
    void *property_h)
{
    shr_error_e rv;
    uint32 flags;
    soc_mem_info_t *mem_info = &SOC_MEM_INFO(unit, mem);

    flags = mem_info->flags;

    sal_memset(prop_str, 0, PRT_COLUMN_WIDTH_BIG);

    sal_sprintf(prop_str + sal_strlen(prop_str), "mem:[%d]", soc_mem_index_max(unit, mem) + 1);

    if (flags & SOC_MEM_FLAG_IS_ARRAY)
    {
        sal_strcat(prop_str, " array");
        if (SOC_MEM_ARRAY_INFOP(unit, mem) != NULL)
            sal_sprintf(prop_str + sal_strlen(prop_str), "[%d]", SOC_MEM_NUMELS(unit, mem));
    }
    if (flags & SOC_MEM_FLAG_READONLY)
    {
        sal_strcat(prop_str, " ro");
    }
    if (flags & SOC_MEM_FLAG_WRITEONLY)
    {
        sal_strcat(prop_str, " wo");
    }
    if (flags & SOC_MEM_FLAG_SIGNAL)
    {
        sal_strcat(prop_str, " signal");
    }
    if (flags & SOC_MEM_FLAG_DEBUG)
    {
        sal_strcat(prop_str, " debug");
    }
    if (flags & SOC_MEM_FLAG_CACHABLE)
    {
        sal_strcat(prop_str, " cachable");
    }
    if (flags & SOC_MEM_FLAG_HASHED)
    {
        sal_strcat(prop_str, " hashed");
    }
    if (shr_access_mem_no_read_get(unit, mem))
    {
        sal_strcat(prop_str, " no_read");
    }
    else
    {   /* Registers with "no_read" are automatically no_wb, no need to mention it implicitly */
        if (shr_access_mem_no_wb_get(unit, mem))
        {
            sal_strcat(prop_str, " no_wb");
        }
    }

    if ((property_h != NULL) && (diag_sand_compare(property_h, prop_str) == FALSE))
        rv = _SHR_E_NOT_FOUND;
    else
        rv = _SHR_E_NONE;
    return rv;
}

static shr_error_e
access_obj_match(
    int dual_match,
    char *obj_name,
    soc_field_info_t * fields,
    int nFields,
    void *match_h,
    void *field_match_h,
    int *fld_match_list)
{
    shr_error_e rv = _SHR_E_NOT_FOUND;
    int i_fld;
    int reg_match, field_match = FALSE;

    reg_match = diag_sand_compare(match_h, obj_name);

    if (nFields > MAX_FIELDS_NUM)
    {
        cli_out("Number of field:%d exceeded maximum:%d\n", nFields, MAX_FIELDS_NUM);
        return _SHR_E_NOT_FOUND;
    }

    if ((dual_match == FALSE) && (reg_match == TRUE))
    {
        for (i_fld = 0; i_fld < nFields; i_fld++)
        {
            fld_match_list[i_fld] = TRUE;
        }
        rv = _SHR_E_NONE;
    }
    else
    {
        for (i_fld = 0; i_fld < nFields; i_fld++)
        {
            if (diag_sand_compare(field_match_h, SOC_FIELD_NAME(unit, fields[i_fld].field)) == TRUE)
            {
                field_match = TRUE;
                fld_match_list[i_fld] = TRUE;
            }
            else
            {
                fld_match_list[i_fld] = FALSE;
            }
        }

        if (dual_match == TRUE)
        {
            if ((reg_match == TRUE) && (field_match == TRUE))
            {
                rv = _SHR_E_NONE;
            }
        }
        else if (field_match == TRUE)
        {
            rv = _SHR_E_NONE;
        }
    }

    return rv;
}

static int
access_fields_get_ordered_list(
    soc_field_info_t * fields,
    int nFields,
    int *fld_index_l)
{
    int fldID = 0;
    int min_bit = 0;
    int i_fld;
    int fld_status[MAX_FIELDS_NUM];

    if ((fields == NULL) || (nFields == 0))
    {
        cli_out("No fields\n");
        return 0;
    }

    if (nFields > MAX_FIELDS_NUM)
    {
        cli_out("Number of field:%d exceeded maximum:%d\n", nFields, MAX_FIELDS_NUM);
        return 0;
    }

    for (i_fld = 0; i_fld < nFields; i_fld++)
        fld_status[i_fld] = FALSE;

    while (nFields != fldID)
    {
        int curID = -1;
        int fld_min_bit, cur_min_bit = SAL_INT32_MAX;
        for (i_fld = 0; i_fld < nFields; i_fld++)
        {
            fld_min_bit = fields[i_fld].bp;

            /*
             * Check if we don't have gaps in bits i.e. next min bit equal to previous max + 1
             */
            if ((fld_status[i_fld] == FALSE) && (fld_min_bit >= min_bit) && (fld_min_bit < cur_min_bit))
            {
                curID = i_fld;
                cur_min_bit = fld_min_bit;
            }
        }
        if ((cur_min_bit == SAL_INT32_MAX) || (curID == -1))
        {
            cli_out("Inconsistent next field for:%d\n", fldID);
            return 0;
        }
        else
        {
            fld_index_l[fldID++] = curID;
            fld_status[curID] = TRUE;
            min_bit = cur_min_bit;
        }
    }
    /*
     * Next minimal bit is here actual the size of the entire entry
     */
    return fields[fld_index_l[nFields - 1]].bp + fields[fld_index_l[nFields - 1]].len;
}

static int
access_fields_match_parse(
    char *match,
    char **field_match_p)
{
    int i_str;

    for (i_str = 0; i_str < sal_strlen(match); i_str++)
    {
        if (match[i_str] == '.')
        {
            match[i_str] = 0;
            *field_match_p = match + i_str + 1;
            return TRUE;
        }
    }

    *field_match_p = match;
    return FALSE;
}

static sh_sand_man_t access_block_man = {
    .brief = "Lists blocks of device",
    .full = "Block names to provide the list to be used for registers/memories per block filtering in list/read/write",
    .synopsis = NULL,
    .examples = "\n" "name=SCH",
};

static sh_sand_option_t access_block_options[] = {
    {"name", SAL_FIELD_TYPE_STR, "Full block name or its substring, filtering blocks to be listed", ""},
    {NULL}
};

static shr_error_e
access_block_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *match_n;
    int i_bl;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(NO_UNIT);

    SH_SAND_GET_STR("name", match_n);

    PRT_TITLE_SET("BLOCK LIST");
    /*
     * Prepare header, pay attention to put header items and content in the same order
     */
    PRT_COLUMN_ADD("Block");
    PRT_COLUMN_ADD("CMIC_ID");
    PRT_COLUMN_ADD("State");

    for (i_bl = 0; SOC_BLOCK_TYPE(unit, i_bl) >= 0; i_bl++)
    {
        if ((match_n) && sal_strcasestr(SOC_BLOCK_NAME(unit, i_bl), match_n) == NULL)
            continue;

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", SOC_BLOCK_NAME(unit, i_bl));
        PRT_CELL_SET("%d", SOC_BLOCK2OFFSET(unit, i_bl));
        if (!SOC_INFO(unit).block_valid[i_bl])
            PRT_CELL_SET("disabled");
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t access_property_man = {
    .brief = "Lists register number per property",
    .full = "Command scans all registers and prints the number per each property found",
    .synopsis = NULL,
    .examples = NULL    /* NULL will result in option less example */
};

static shr_error_e
access_property_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int i_type;
    soc_reg_t reg;
    int reg_num[soc_invalidreg];
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(NO_UNIT);

    memset(reg_num, 0, sizeof(int) * soc_invalidreg);
    PRT_TITLE_SET("REGISTER TYPES LIST");
    /*
     * Prepare header, pay attention to put header items and content in the same order
     */
    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD("Number");

    for (reg = 0; reg < NUM_SOC_REG; reg++)
    {
        if (!SOC_REG_IS_VALID(unit, reg))
            continue;

        reg_num[SOC_REG_INFO(unit, reg).regtype]++;
    }

    for (i_type = 0; i_type < soc_invalidreg; i_type++)
    {
        if (reg_num[i_type] == 0)
            continue;

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", soc_regtypenames[i_type]);
        PRT_CELL_SET("%d", reg_num[i_type]);
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t access_list_man = {
    .brief = "Show registers/memories lists per filtering criteria",
    .full =
        "Command allows to display or to save in XML file list of registers&memories filtered by one or more filters\n"
        "Entire list is very long and by default command without any option will return usage. If you want to have all, use"
        " \"all\" option."
        "Output includes register/memory name, property list, block list, reset value, size, address, optionally field list"
        "and description. Beware that sometimes output line exceeds screen width. In these case try either to increase your "
        "display size or apply more rigor filtering to decrease output volumes",
    .synopsis = "[name=<regular expr>] [block=<substring>] [fields] [all] [desc] [property=<regular expr>]",
    .examples =
        "prp=reg.*iproc field=no\n" "name=RIP block=SCH\n" "prp=mem address=0x00800000 desc\n"
        "all",
};

static sh_sand_option_t access_list_options[] = {
    {"name", SAL_FIELD_TYPE_STR, "Regular expression used to filter regs/mems and their fields", "", NULL, NULL,
     SH_SAND_ARG_FREE},
    {"address", SAL_FIELD_TYPE_UINT32, "Memory or Register address, acts as if mem/reg nam provides", "0xFFFFFFFF"},
    {"block", SAL_FIELD_TYPE_STR, "Block name or any substring of it to filter regs/mems", ""},
    {"all", SAL_FIELD_TYPE_BOOL, "Ignore filters and print all regs/mems", "No"},
    {"field", SAL_FIELD_TYPE_BOOL, "Include fields in regs/mems search and output", "Yes"},
    {"desc", SAL_FIELD_TYPE_BOOL, "Add description column in output", "No"},
    {"property", SAL_FIELD_TYPE_STR, "Name or name substring for regs/mems properties", ""},
    {NULL}
};

static shr_error_e
access_list_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *block_n, *match_n, *property_n, *field_match_n;
    int fld_index_list[MAX_FIELDS_NUM];
    int fld_match_list[MAX_FIELDS_NUM];
    soc_reg_t reg;
    soc_mem_t mem;
    uint32 address;

    int desc_flag, field_flag, all_flag;

    int i_fld;
    soc_field_info_t *fld;
    soc_reg_above_64_val_t fld_value;
    int dual_match;
    void *match_h = NULL, *property_h = NULL, *field_match_h = NULL;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(NO_UNIT);
    /*
     * Get parameters
     */
    SH_SAND_GET_STR("name", match_n);
    SH_SAND_GET_STR("block", block_n);
    SH_SAND_GET_STR("property", property_n);
    SH_SAND_GET_BOOL("field", field_flag);
    SH_SAND_GET_BOOL("desc", desc_flag);
    SH_SAND_GET_BOOL("all", all_flag);
    SH_SAND_GET_UINT32("address", address);

    if (all_flag == TRUE)
    {
        match_n = block_n = property_n = "";
    }
    else
    {
        if (ISEMPTY(match_n) && ISEMPTY(block_n) && ISEMPTY(property_n) && (address == SH_SAND_MAX_UINT32))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Use \"all\" option to see the entire list or use filtering option\n");
        }
    }

    utilex_str_to_upper(match_n);

    dual_match = access_fields_match_parse(match_n, &field_match_n);
    if ((dual_match == TRUE) && (field_flag == FALSE))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Dotted format cannot be used with field option disabled\n");
    }
    SHR_IF_ERR_EXIT(diag_sand_compare_init(match_n, &match_h));
    SHR_IF_ERR_EXIT(diag_sand_compare_init(field_match_n, &field_match_h));
    SHR_IF_ERR_EXIT(diag_sand_compare_init(property_n, &property_h));

    PRT_TITLE_SET("ACCESS LIST");
    /*
     * Prepare header, pay attention to put header items and content in the same order
     */
    PRT_COLUMN_ADD("Object");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Properties");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Blocks");
    PRT_COLUMN_ADD("Bits");
    PRT_COLUMN_ADD("Reset");
    PRT_COLUMN_ADD("Address");

    if (desc_flag == TRUE)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Description");
    }

    for (reg = 0; reg < NUM_SOC_REG; reg++)
    {
        soc_reg_info_t *reginfo;
        int reg_size;
        uint32 reg_addr;
        soc_reg_above_64_val_t reg_value;
        char value_str[PRT_COLUMN_WIDTH_BIG];
        char prop_str[PRT_COLUMN_WIDTH_BIG];
        char block_str[PRT_COLUMN_WIDTH_BIG];

        if (!SOC_REG_IS_VALID(unit, reg))
            continue;

        reginfo = &SOC_REG_INFO(unit, reg);

        reg_addr = SOC_REG_BASE(unit, reg) + reginfo->first_array_index;

        if (address != SH_SAND_MAX_UINT32)
        {
            int max_adr_offset = 0;
            if (reginfo->flags & SOC_REG_FLAG_ARRAY)
            {
                max_adr_offset = (reginfo->numels - 1) * SOC_REG_ELEM_SKIP(unit, reg);
            }
            if ((address < reg_addr) || (address > reg_addr + max_adr_offset))
                continue;
        }

        /*
         * Filter per register and field names 
         */
        if (access_obj_match(dual_match, SOC_REG_NAME(unit, reg), reginfo->fields, reginfo->nFields,
                             match_h, field_match_h, fld_match_list) != _SHR_E_NONE)
            continue;

        /*
         * Fill properties
         */
        if (access_reg_properties_get(unit, reg, prop_str, property_h) != _SHR_E_NONE)
            continue;
        /*
         * Fill blocks
         */
        if (diag_sand_reg_blocks_get(unit, reginfo, block_str, block_n) != _SHR_E_NONE)
            continue;

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", SOC_REG_NAME(unit, reg));
        PRT_CELL_SET("%s", prop_str);
        PRT_CELL_SET("%s", block_str);
        if ((reg_size = access_fields_get_ordered_list(reginfo->fields, reginfo->nFields, fld_index_list)) == 0)
        {
            cli_out("Problem with fields for register:%s\n", SOC_REG_NAME(unit, reg));
            continue;
        }

        PRT_CELL_SET("%02d:%02d", reginfo->fields[fld_index_list[0]].bp, reg_size - 1); /* Bits */
        SOC_REG_ABOVE_64_RST_VAL_GET(unit, reg, reg_value);
        diag_sand_value_to_str(reg_value, reg_size, value_str, PRT_COLUMN_WIDTH_BIG);
        PRT_CELL_SET("%s", value_str);  /* Reset */
        PRT_CELL_SET("%08x", reg_addr); /* Address */
        if (desc_flag == TRUE)
        {
            PRT_CELL_SET("%s", SOC_REG_DESC(unit, reg));
        }

        if (field_flag == TRUE)
        {
            for (i_fld = 0; i_fld < reginfo->nFields; i_fld++)
            {
                if (fld_match_list[fld_index_list[i_fld]] == FALSE)
                    continue;

                fld = &reginfo->fields[fld_index_list[i_fld]];
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET_SHIFT(1, "%s", SOC_FIELD_NAME(unit, fld->field));  /* Object */
                if (fld->flags & SOCF_RO)
                {
                    PRT_CELL_SET_SHIFT(1, "ro");        /* Properties */
                }
                else if (fld->flags & SOCF_WO)
                {
                    PRT_CELL_SET_SHIFT(1, "wo");        /* Properties */
                }
                else
                {
                    PRT_CELL_SET_SHIFT(1, "rw");        /* Properties */
                }
                PRT_CELL_SKIP(1);       /* Blocks */
                PRT_CELL_SET_SHIFT(1, "%02d:%02d", fld->bp, fld->bp + fld->len - 1);    /* Bits */

                SOC_REG_ABOVE_64_CLEAR(fld_value);
                SHR_BITCOPY_RANGE(fld_value, 0, reg_value, fld->bp, fld->len);
                diag_sand_value_to_str(fld_value, fld->len, value_str, PRT_COLUMN_WIDTH_BIG);
                PRT_CELL_SET_SHIFT(1, "%s", value_str); /* Reset */
            }
        }
    }

    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        soc_mem_info_t *meminfo;
        int mem_size;
        uint32 mem_addr;
        uint8 acc_type;
        char prop_str[PRT_COLUMN_WIDTH_BIG];
         /* coverity[stack_use_overflow : FALSE]  */
        char block_str[PRT_COLUMN_WIDTH_BIG];

        if (!SOC_MEM_IS_VALID(unit, mem))
            continue;

        meminfo = &SOC_MEM_INFO(unit, mem);
        /*
         * Init to print all fields 
         */
        for (i_fld = 0; i_fld < meminfo->nFields; i_fld++)
        {
            fld_match_list[i_fld] = TRUE;
        }

        if (address != SH_SAND_MAX_UINT32)
        {
            uint32 start, end;
            int array_index = 0;
            soc_mem_array_info_t *maip;

            start = soc_mem_addr_get(unit, mem, 0, SOC_MEM_BLOCK_ANY(unit, mem), 0, &acc_type);

            if ((maip = SOC_MEM_ARRAY_INFOP(unit, mem)) != NULL)
                array_index = (maip->numels - 1) + maip->first_array_index;

            end =
                soc_mem_addr_get(unit, mem, array_index, SOC_MEM_BLOCK_ANY(unit, mem), soc_mem_index_max(unit, mem),
                                 &acc_type);
            if ((address < start) || (address > end))
                continue;
        }
        /*
         * Filter per memory and field names 
         */
        if (access_obj_match(dual_match, SOC_MEM_NAME(unit, mem), meminfo->fields, meminfo->nFields,
                             match_h, field_match_h, fld_match_list) != _SHR_E_NONE)
            continue;

        mem_addr = soc_mem_addr_get(unit, mem, 0, SOC_MEM_BLOCK_ANY(unit, mem), 0, &acc_type);
        /*
         * Fill properties
         */
        if (access_mem_properties_get(unit, mem, prop_str, property_h) != _SHR_E_NONE)
            continue;
        /*
         * Fill blocks
         */
        if (diag_sand_mem_blocks_get(unit, mem, block_str, block_n) != _SHR_E_NONE)
            continue;

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", SOC_MEM_NAME(unit, mem));
        PRT_CELL_SET("%s", prop_str);
        PRT_CELL_SET("%s", block_str);

        if ((mem_size = access_fields_get_ordered_list(meminfo->fields, meminfo->nFields, fld_index_list)) == 0)
        {
            cli_out("Problem with field for memory:%s\n", SOC_MEM_NAME(unit, mem));
            continue;
        }

        PRT_CELL_SET("%02d:%02d", meminfo->fields[fld_index_list[0]].bp, mem_size - 1); /* Bits */

        PRT_CELL_SKIP(1);       /* Reset */
        PRT_CELL_SET("%08x", mem_addr); /* Address */
        if (desc_flag == TRUE)
        {
            PRT_CELL_SET("%s", SOC_MEM_DESC(unit, mem));
        }

        if (field_flag == TRUE)
        {
            for (i_fld = 0; i_fld < meminfo->nFields; i_fld++)
            {
                if (fld_match_list[fld_index_list[i_fld]] == FALSE)
                    continue;

                fld = &meminfo->fields[fld_index_list[i_fld]];
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET_SHIFT(1, "%s", SOC_FIELD_NAME(unit, fld->field));
                PRT_CELL_SKIP(2);       /* Properties/Blocks */
                PRT_CELL_SET_SHIFT(1, "%02d:%02d", fld->bp, fld->bp + fld->len - 1);
            }
        }
    }

    PRT_COMMITX;
exit:
    diag_sand_compare_close(match_h);
    diag_sand_compare_close(field_match_h);
    diag_sand_compare_close(property_h);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t access_read_man = {
    .brief = "Read registers/memories lists per filtering criteria",
    .full =
        "Command allows to display or to save in XML file value of registers&memories filtered by one or more filters\n"
        "Entire list is very long and by default command without any option will return usage. If you want to have all, use"
        "\"all\" option."
        "Output includes register/memory name, block name, array index for arrayed register, optionally fields list."
        " Beware that sometimes output line exceeds screen width. In these case try either to increase your "
        "display size or apply more rigor filtering to decrease output volumes",
    .synopsis = "[name=<regular expr>] [block=<substring>] [field] [all] [property=<regular expr>]",
    .examples = "name=VTT count=1 prp=mem\n"
        "name=RATE block=SCH wb count=8 print=on\n"
        "prp=mem.*signal name=PORT count=1 field\n"
        "name=VSI entry=0 count=1..2 index=0 field overwrite\n"
        "prp=mem block=ETPP count=1\n"
        "prp=reg.*port field separate\n" "block=IPPD count=1 changed prp=mem"
};

static sh_sand_option_t access_read_options[] = {
    {"name", SAL_FIELD_TYPE_STR, "Regular expression used to filter regs/mems and their fields", "", NULL, NULL,
     SH_SAND_ARG_FREE},
    {"block", SAL_FIELD_TYPE_STR, "Block name or any substring of it to filter regs/mems", ""},
    {"all", SAL_FIELD_TYPE_BOOL, "Ignore filters and print all regs/mems", "No", NULL, NULL, SH_SAND_ARG_QUIET},
    {"field", SAL_FIELD_TYPE_BOOL, "Include fields in regs/mems search and output", "No"},
    {"changed", SAL_FIELD_TYPE_BOOL, "Print only when value is different from reset one", "No"},
    {"property", SAL_FIELD_TYPE_STR, "Name or name substring for regs/mems properties/n"
     "<reg/mem/array/ro/wo regtype name:<cpu/general/iproc/custom/port>", ""},
    {"index", SAL_FIELD_TYPE_INT32, "Memory/Register Array Index", "-1"},
    {"entry", SAL_FIELD_TYPE_INT32, "Memory Entry Number", "-1"},
    {"count", SAL_FIELD_TYPE_INT32, "number of entries", "0"},
    {"cache", SAL_FIELD_TYPE_BOOL, "flag used to obtain non-cached memory value", "Yes"},
    {"overwrite", SAL_FIELD_TYPE_BOOL, "flag used to overwrite auxiliary no read", "No"},
    {"warmboot", SAL_FIELD_TYPE_BOOL, "used to read only warmboot meaningful objects", "No"},
    {"separate", SAL_FIELD_TYPE_BOOL, "Used to read mem/reg to separate files", "No"},
    {"print", SAL_FIELD_TYPE_BOOL, "Print the data to the screen/file", "Yes"},
    {"index_range", SAL_FIELD_TYPE_UINT32, "Memory/Register Array Index Range", "0-0", NULL, NULL,
     SH_SAND_ARG_QUIET | SH_SAND_ARG_RANGE},
    {NULL}
};

/** see description in header file */
shr_error_e
access_reg_print(
    prt_control_t * prt_ctr,
    int unit,
    soc_reg_t reg,
    char *block_name,
    int reg_index,
    uint32 address,
    soc_reg_above_64_val_t reg_value,
    int field_flag,
    int changed_flag,
    int *fld_match_list)
{
    int reg_size;
    int fld_index_list[MAX_FIELDS_NUM];
    char *value_str = NULL;
    char _value_str[PRT_COLUMN_WIDTH_BIG + 1];
    soc_reg_info_t *reginfo = &SOC_REG_INFO(unit, reg);
    soc_reg_above_64_val_t reset_value;
    int offset_index = 0;

    SHR_FUNC_INIT_VARS(NO_UNIT);
    sal_memset(_value_str, 0, PRT_COLUMN_WIDTH_BIG + 1);

    if ((reg_size = access_fields_get_ordered_list(reginfo->fields, reginfo->nFields, fld_index_list)) == 0)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Problem with fields for register:%s\n", SOC_REG_NAME(unit, reg));
    }

    /*
     * Each byte require 2 bytes in output string - each nibble has its character.
     */
    value_str = sal_alloc((reg_size * 2) + 1, "value_str");
    sal_memset(value_str, 0, (reg_size * 2) + 1);

    SOC_REG_ABOVE_64_RST_VAL_GET(unit, reg, reset_value);
    if (changed_flag && (diag_sand_value_same(reg_value, reset_value, reg_size) == TRUE))
        goto exit;

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", SOC_REG_NAME(unit, reg));
    PRT_CELL_SET("%s", block_name);
    PRT_CELL_SET("%d", reg_index);
    PRT_CELL_SET("%08x", address);
    diag_sand_value_to_str(reg_value, reg_size, value_str, (reg_size * 2) + 1);
    sal_strncpy(_value_str, value_str, PRT_COLUMN_WIDTH_BIG);
    PRT_CELL_SET("%s", _value_str);
    PRT_CELL_SET("%s", "reg");

    for (offset_index = 1; offset_index * (PRT_COLUMN_WIDTH_BIG / 2) < (reg_size * 2) + 1; offset_index++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET_SHIFT(1, "%s: part(%d)", SOC_REG_NAME(unit, reg), offset_index);   /* Object */
        PRT_CELL_SKIP(3); /** Block, Index and Address*/
        sal_strncpy(_value_str, value_str + (offset_index * (PRT_COLUMN_WIDTH_BIG / 16)), PRT_COLUMN_WIDTH_BIG);
        PRT_CELL_SET_SHIFT(1, "%s", _value_str);
        PRT_CELL_SKIP(1); /** Property */
    }

    if (field_flag == TRUE)
    {
        int i_fld;
        soc_field_info_t *fld;
        soc_reg_above_64_val_t fld_value;
        for (i_fld = 0; i_fld < reginfo->nFields; i_fld++)
        {
            if (fld_match_list[fld_index_list[i_fld]] == FALSE)
                continue;
            fld = &reginfo->fields[fld_index_list[i_fld]];
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET_SHIFT(1, "%s", SOC_FIELD_NAME(unit, fld->field));      /* Object */
            PRT_CELL_SKIP(3);   /* Block&Array Index&Address */
            SOC_REG_ABOVE_64_CLEAR(fld_value);
            SHR_BITCOPY_RANGE(fld_value, 0, reg_value, fld->bp, fld->len);
            diag_sand_value_to_str(fld_value, fld->len, value_str, (reg_size * 2) + 1);
            /*
             * Reducing 32 bits from the print sting, because of the shift.
             */
            sal_strncpy(_value_str, value_str, (PRT_COLUMN_WIDTH_BIG - 32));
            PRT_CELL_SET_SHIFT(1, "%s", _value_str);    /* Reset */

            for (offset_index = 1; offset_index * ((PRT_COLUMN_WIDTH_BIG - 32) / 2) < (fld->len * 2) + 1;
                 offset_index++)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET_SHIFT(1, "%s: part(%d)", SOC_FIELD_NAME(unit, fld->field), offset_index);  /* Object */
                PRT_CELL_SKIP(3); /** Block, Index and Address*/
                /*
                 * Reducing 32 bits from the print sting, because of the shift.
                 */
                sal_strncpy(_value_str, value_str + (offset_index * ((PRT_COLUMN_WIDTH_BIG - 32) / 16)),
                            (PRT_COLUMN_WIDTH_BIG - 32));
                PRT_CELL_SET_SHIFT(1, "%s", _value_str);
            }
        }
    }

exit:
    SHR_FREE(value_str);
    SHR_FUNC_EXIT;
}

static shr_error_e
access_mem_print(
    prt_control_t * prt_ctr,
    int unit,
    soc_mem_t mem,
    int mem_size,
    int block_id,
    int entry,
    int index,
    uint32 address,
    uint32 *mem_value,
    int field_flag,
    int changed,
    int *fld_index_list,
    int *fld_match_list)
{
    soc_mem_info_t *meminfo;
    char *value_str = NULL;
    char _value_str[PRT_COLUMN_WIDTH_BIG + 1];
    uint32 mem_null_value[SOC_MAX_MEM_WORDS] = { 0 };
    int offset_index;

    SHR_FUNC_INIT_VARS(NO_UNIT);
    sal_memset(_value_str, 0, PRT_COLUMN_WIDTH_BIG + 1);

    meminfo = &SOC_MEM_INFO(unit, mem);

    /*
     * If only changed memory should be shown compare to null one 
     */
    if (changed && diag_sand_value_same(mem_value, mem_null_value, mem_size) == TRUE)
        goto exit;

    /*
     * Each byte require 2 bytes in output string - each nibble has its character.
     */
    value_str = sal_alloc((mem_size * 2) + 1, "value_str");
    sal_memset(value_str, 0, (mem_size * 2) + 1);

    diag_sand_value_to_str(mem_value, mem_size, value_str, (mem_size * 2) + 1);
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", SOC_MEM_NAME(unit, mem));
    PRT_CELL_SET("%s", SOC_BLOCK_NAME(unit, block_id));

    if (meminfo->flags & SOC_MEM_FLAG_IS_ARRAY)
    {
        PRT_CELL_SET("%d(%d)", entry, index);
    }
    else
    {
        PRT_CELL_SET("%d", entry);
    }

    PRT_CELL_SET("%08x", address);
    sal_strncpy(_value_str, value_str, PRT_COLUMN_WIDTH_BIG);
    PRT_CELL_SET("%s", _value_str);
    PRT_CELL_SET("%s", "mem");

    for (offset_index = 1; offset_index * (PRT_COLUMN_WIDTH_BIG / 2) < (mem_size * 2) + 1; offset_index++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET_SHIFT(1, "%s: part(%d)", SOC_MEM_NAME(unit, mem), offset_index);   /* Object */
        PRT_CELL_SKIP(3); /** Block, Index and Address*/
        sal_strncpy(_value_str, value_str + (offset_index * (PRT_COLUMN_WIDTH_BIG / 16)), PRT_COLUMN_WIDTH_BIG);
        PRT_CELL_SET_SHIFT(1, "%s", _value_str);
        PRT_CELL_SKIP(1); /** Property */
    }

    if (field_flag == TRUE)
    {
        soc_field_info_t *fld;
        int i_fld;
        soc_reg_above_64_val_t fld_value;
        for (i_fld = 0; i_fld < meminfo->nFields; i_fld++)
        {
            if (fld_match_list[fld_index_list[i_fld]] == FALSE)
                continue;

            fld = &meminfo->fields[fld_index_list[i_fld]];
            SOC_REG_ABOVE_64_CLEAR(fld_value);
            SHR_BITCOPY_RANGE(fld_value, 0, mem_value, fld->bp, fld->len);

            /*
             * If only changed memory should be shown compare to null one filtered fields as well
             */
            if (changed && diag_sand_value_same(fld_value, mem_null_value, fld->len) == TRUE)
                continue;

            diag_sand_value_to_str(fld_value, fld->len, value_str, (mem_size * 2) + 1);

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET_SHIFT(1, "%s", SOC_FIELD_NAME(unit, fld->field));
            PRT_CELL_SKIP(3);   /* Properties&Blocks&Index */
            /*
             * Reducing 32 bits from the print sting, because of the shift.
             */
            sal_strncpy(_value_str, value_str, (PRT_COLUMN_WIDTH_BIG - 32));
            PRT_CELL_SET_SHIFT(1, "%s", _value_str);

            for (offset_index = 1; offset_index * ((PRT_COLUMN_WIDTH_BIG - 32) / 2) < (fld->len * 2) + 1;
                 offset_index++)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET_SHIFT(1, "%s: part(%d)", SOC_FIELD_NAME(unit, fld->field), offset_index);  /* Object */
                PRT_CELL_SKIP(3); /** Block, Index and Address*/
                /*
                 * Reducing 32 bits from the print sting, because of the shift.
                 */
                sal_strncpy(_value_str, value_str + (offset_index * ((PRT_COLUMN_WIDTH_BIG - 32) / 16)),
                            (PRT_COLUMN_WIDTH_BIG - 32));
                PRT_CELL_SET_SHIFT(1, "%s", _value_str);
            }
        }
    }
exit:
    SHR_FREE(value_str);
    SHR_FUNC_EXIT;
}

static shr_error_e
access_read_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *block_n, *match_n, *property_n, *field_match_n;
    int fld_index_list[MAX_FIELDS_NUM];
    int fld_match_list[MAX_FIELDS_NUM];
    soc_reg_t reg;
    soc_mem_t mem;

    int field_flag, all_flag;
    int object_col_id;
    void *match_h = NULL, *property_h = NULL, *field_match_h = NULL;
    int index, entry, count, wb, separate_flag, print_flag;
    uint32 address = 0;
    int cache, over, changed;
    int dual_match;
    char *folder_name;
    char help_filename[512 + 64];
    soc_reg_info_t *reginfo;
    soc_reg_above_64_val_t reg_value;
    int numels;
    char prop_str[PRT_COLUMN_WIDTH_BIG];
    char block_str[PRT_COLUMN_WIDTH_BIG];
    int port;
    pbmp_t bm;
    soc_mem_info_t *meminfo;
    uint8 acc_type;
    int i_bl, i_ind, i_entry;
    int index_start, index_num;
    uint32 *mem_value = NULL;
    int soc_error;
    int mem_size;
    int cur_entry, cur_count;
    uint32 index_range_start = 0, index_range_end = 0;
    uint8 has_index_range = FALSE;
    soc_regtype_t regtype;

#ifdef BCM_DNX_SUPPORT
    int is_master_port = 0;
#endif

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(NO_UNIT);
    /*
     * Get parameters
     */
    SH_SAND_GET_STR("name", match_n);
    SH_SAND_GET_STR("block", block_n);
    SH_SAND_GET_STR("property", property_n);
    SH_SAND_GET_BOOL("field", field_flag);
    SH_SAND_GET_BOOL("all", all_flag);
    SH_SAND_GET_BOOL("changed", changed);
    SH_SAND_GET_INT32("index", index);
    SH_SAND_GET_INT32("entry", entry);
    SH_SAND_GET_INT32("count", count);
    SH_SAND_GET_BOOL("cache", cache);
    SH_SAND_GET_BOOL("overwrite", over);
    SH_SAND_GET_BOOL("warmboot", wb);
    SH_SAND_GET_BOOL("separate", separate_flag);
    SH_SAND_GET_STR("folder", folder_name);
    SH_SAND_GET_BOOL("print", print_flag);
    SH_SAND_GET_UINT32_RANGE("index_range", index_range_start, index_range_end);
    SH_SAND_IS_PRESENT("index_range", has_index_range);

    sal_memset(help_filename, 0, sizeof(help_filename));

    if (all_flag == TRUE)
    {
        int is_present;
        match_n = block_n = property_n = "";
        SH_SAND_IS_PRESENT("name", is_present);
        if (is_present == TRUE)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "\"name\" filter along \"all\" has no effect\n");
        }
        SH_SAND_IS_PRESENT("block", is_present);
        if (is_present == TRUE)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "\"block\" filter along \"all\" has no effect\n");
        }
        SH_SAND_IS_PRESENT("property", is_present);
        if (is_present == TRUE)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "\"property\" filter along \"all\" has no effect\n");
        }
    }
    else
    {
        if (ISEMPTY(match_n) && ISEMPTY(block_n) && ISEMPTY(property_n))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Use \"all\" too read all regs/mems or use one of filtering options\n");
        }
    }

    utilex_str_to_upper(match_n);

    dual_match = access_fields_match_parse(match_n, &field_match_n);
    if (dual_match == TRUE)
    {
        field_flag = TRUE;
    }

    SHR_IF_ERR_EXIT(diag_sand_compare_init(match_n, &match_h));
    SHR_IF_ERR_EXIT(diag_sand_compare_init(field_match_n, &field_match_h));
    SHR_IF_ERR_EXIT(diag_sand_compare_init(property_n, &property_h));

    if (separate_flag != TRUE && print_flag == TRUE)
    {
        PRT_TITLE_SET("access read");
        /*
         * Prepare header, pay attention to put header items and content in the same order
         */
        PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &object_col_id, "Object");
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Block");
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Id");
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Address");
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Value");
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Property");
    }

    for (reg = 0; reg < NUM_SOC_REG; reg++)
    {

        if (!SOC_REG_IS_VALID(unit, reg))
            continue;

        if ((over == FALSE) && shr_access_reg_no_read_get(unit, reg))
            continue;

        if ((wb == TRUE) && shr_access_reg_no_wb_get(unit, reg))
            continue;

        /*
         * Check that there is no forbidden field in register 
         */
        if (access_reg_filter_no_read_fields(unit, reg))
            continue;

        reginfo = &SOC_REG_INFO(unit, reg);

        if (reginfo->flags & SOC_REG_FLAG_WO)
            continue;

        /*
         * Filter per register and field names 
         */
        if (access_obj_match(dual_match, SOC_REG_NAME(unit, reg), reginfo->fields, reginfo->nFields,
                             match_h, field_match_h, fld_match_list) != _SHR_E_NONE)
            continue;
        /*
         * Filter on properties
         */
        if (access_reg_properties_get(unit, reg, prop_str, property_h) != _SHR_E_NONE)
            continue;
        /*
         * Filter on blocks
         */
        if (diag_sand_reg_blocks_get(unit, reginfo, block_str, block_n) != _SHR_E_NONE)
            continue;

        if ((numels = SOC_REG_INFO(unit, reg).numels) == 0)
            numels = 1; /* Simulate index 0 for non arrayed registers, should not happen */

        if (separate_flag == TRUE && print_flag == TRUE)
        {
            if (!PRT_TITLE_IS_SET)
            {
                PRT_TITLE_SET("access read");
                /*
                 * Prepare header, pay attention to put header items and content in the same order
                 */
                PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &object_col_id, "Object");
                PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Block");
                PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Id");
                PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Address");
                PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Value");
                PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Property");
            }

            if (sal_strcmp(folder_name, ""))
            {
                sal_memset(help_filename, 0, sizeof(char) * 512);
                sal_strncat(help_filename, folder_name, 512);
                sal_strncat(help_filename, SOC_REG_NAME(unit, reg), 512);
                sal_strncat(help_filename, ".xml", 512);
            }
        }

        regtype = reginfo->regtype;
        if (soc_is_reg_flexe_core(unit, reg))
        {
            regtype = soc_genreg;
        }
        switch (regtype)
        {
            case soc_portreg:
                PBMP_ITER(PBMP_PORT_ALL(unit), port)
            {
#ifdef BCM_PETRA_SUPPORT
                int master_port;
                if (SOC_IS_DPP(unit))
                {
                    if (!IS_SFI_PORT(unit, port))
                    {
                        if (soc_port_sw_db_master_channel_get(unit, port, &master_port) < 0)
                        {
                            cli_out("Master get failed for:%s\n", SOC_REG_NAME(unit, reg));
                            continue;
                        }
                        if (port != master_port)
                            continue;
                    }
                }
#endif
#ifdef BCM_DNX_SUPPORT
                if (SOC_IS_DNX(unit))
                {
                    if (!IS_SFI_PORT(unit, port))
                    {
                        SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, port, &is_master_port));
                        if (is_master_port == FALSE)
                        {
                            continue;
                        }
                    }
                }
#endif /* BCM_DNX_SUPPORT */
                if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_PORT_TYPE(unit, port)))
                {
                    if (!ISEMPTY(block_n) && (sal_strcasestr(SOC_PORT_NAME(unit, port), block_n) == NULL))
                        continue;
                    if (access_reg_read(unit, reg, 0, port, 0, reg_value, NULL) == _SHR_E_NONE)
                    {
                        if (print_flag == TRUE)
                        {
                            access_reg_print(prt_ctr, unit, reg, SOC_PORT_NAME(unit, port), 0, SOC_REG_BASE(unit, reg),
                                             reg_value, field_flag, changed, fld_match_list);
                        }
                    }
                }
            }
                break;
            case soc_customreg:
                BCM_PBMP_ASSIGN(bm, SOC_INFO(unit).custom_reg_access.custom_port_pbmp);
                PBMP_ITER(bm, port)
                {
#ifdef BCM_PETRA_SUPPORT
                    int master_port;
                    if (SOC_IS_DPP(unit))
                    {
                        if (!IS_SFI_PORT(unit, port))
                        {
                            if (soc_port_sw_db_master_channel_get(unit, port, &master_port) < 0)
                            {
                                cli_out("Master get failed for:%s\n", SOC_REG_NAME(unit, reg));
                                continue;
                            }
                            if (port != master_port)
                                continue;
                        }
                    }
#endif
#ifdef BCM_DNX_SUPPORT
                    if (SOC_IS_DNX(unit))
                    {
                        if (!IS_SFI_PORT(unit, port))
                        {
                            SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, port, &is_master_port));
                            if (is_master_port == FALSE)
                            {
                                continue;
                            }
                        }
                    }
#endif /* BCM_DNX_SUPPORT */

                    if (!SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_PORT_TYPE(unit, port)))
                    {
                        continue;
                    }

                    if (!SOC_PORT_VALID(unit, port))
                    {
                        continue;
                    }

                    if (!ISEMPTY(block_n) && (sal_strcasestr(SOC_PORT_NAME(unit, port), block_n) == NULL))
                        continue;

                    if (access_reg_read(unit, reg, 0, port, 0, reg_value, NULL) == _SHR_E_NONE)
                    {
                        if (print_flag == TRUE)
                        {
                            access_reg_print(prt_ctr, unit, reg, SOC_PORT_NAME(unit, port), 0, SOC_REG_BASE(unit, reg),
                                             reg_value, field_flag, changed, fld_match_list);
                        }
                    }
                }
                break;
            case soc_iprocreg:
                break;
            default:
                for (i_bl = 0; SOC_BLOCK_INFO(unit, i_bl).type >= 0; i_bl++)
                {       /* 
                         * If block type dont match skip this block
                         */
                    if (!SOC_BLOCK_IS_TYPE(unit, i_bl, reginfo->block))
                        continue;
                    /*
                     * If block name filter was used and not matched skip this block
                     */
                    if (!ISEMPTY(block_n) && (sal_strcasestr(SOC_BLOCK_NAME(unit, i_bl), block_n) == NULL))
                        continue;
                    /*
                     * Non-arrayed registers has numels 1, so they will be index 0
                     */
                    if ((count == 0) || (count > numels))
                    {
                        index_num = numels;
                    }
                    else
                    {
                        index_num = count;
                    }
                    if ((index == -1) || (index > (numels + reginfo->first_array_index)))
                    {
                        index_start = reginfo->first_array_index;
                    }
                    else
                    {
                        index_start = index;
                    }

                    if ((index + index_num) > (numels + reginfo->first_array_index))
                    {
                        index_num = numels - index_start;
                    }

                    for (i_ind = index_start; i_ind < index_start + index_num; i_ind++)
                    {
                        if ((i_ind < index_range_start || i_ind > index_range_end) && has_index_range)
                            continue;
                        /*
                         * Put reg name for each block to keep consistency
                         */
                        if (access_reg_read(unit, reg, i_bl, 0, i_ind, reg_value, &address) == _SHR_E_NONE)
                        {
                            if (print_flag == TRUE)
                            {
                                access_reg_print(prt_ctr, unit, reg, SOC_BLOCK_NAME(unit, i_bl), i_ind, address,
                                                 reg_value, field_flag, changed, fld_match_list);
                            }
                        }
                    }
                }
                break;
        }
        if (separate_flag == TRUE && print_flag == TRUE)
        {
            if (sal_strcmp(folder_name, ""))
            {
                PRT_COMMITF(help_filename);
            }
            else
            {
                PRT_COMMITX;
            }
        }
    }

    if ((mem_value = sal_alloc(sizeof(uint32) * SOC_MAX_MEM_WORDS, "Memory for Access")) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "No memory for access command\n");
    }
    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {

        if (!SOC_MEM_IS_VALID(unit, mem))
            continue;

        /*
         * Check via auxiliary mcm db
         */
        if ((over == FALSE) && shr_access_mem_no_read_get(unit, mem))
            continue;

        if ((wb == TRUE) && shr_access_mem_no_wb_get(unit, mem))
            continue;

        meminfo = &SOC_MEM_INFO(unit, mem);

        if (meminfo->flags & SOC_MEM_FLAG_WRITEONLY)
            continue;

        /*
         * Filter per memory and field names 
         */
        if (access_obj_match(dual_match, SOC_MEM_NAME(unit, mem), meminfo->fields, meminfo->nFields,
                             match_h, field_match_h, fld_match_list) != _SHR_E_NONE)
            continue;
        /*
         * Fill properties
         */
        if (access_mem_properties_get(unit, mem, prop_str, property_h) != _SHR_E_NONE)
            continue;
        /*
         * Fill blocks
         */
        if (diag_sand_mem_blocks_get(unit, mem, block_str, block_n) != _SHR_E_NONE)
            continue;

        if (entry == -1)
        {
            cur_entry = soc_mem_index_min(unit, mem);
        }
        else
        {
            if (entry > soc_mem_index_max(unit, mem))
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "Entry:%d is out of range for %s\n", entry, SOC_MEM_UFNAME(unit, mem));
            }
            else
            {
                cur_entry = entry;
            }
        }

        if (count == 0)
        {
            cur_count = soc_mem_index_max(unit, mem) + 1 - cur_entry;
        }
        else
        {
            if (cur_entry + count > (soc_mem_index_max(unit, mem) + 1))
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "Count:%d is out of range for %s\n", count, SOC_MEM_UFNAME(unit, mem));
            }
            else
            {
                cur_count = count;
            }
        }

        if ((meminfo->flags & SOC_MEM_FLAG_IS_ARRAY) && (SOC_MEM_ARRAY_INFOP(unit, mem) != NULL))
        {
            if (index != -1)
            {
                if (index < SOC_MEM_FIRST_ARRAY_INDEX(unit, mem))
                {
                    cli_out("The given array index(%d) is smaller than the first memory array index(%d).\n", index,
                            SOC_MEM_FIRST_ARRAY_INDEX(unit, mem));
                    continue;
                }
                index_start = index;
                index_num = 1;
            }
            else
            {
                index_start = SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
                index_num = SOC_MEM_NUMELS(unit, mem);
            }
        }
        else
        {
            index_start = 0;
            index_num = 1;
        }

        if ((mem_size = access_fields_get_ordered_list(meminfo->fields, meminfo->nFields, fld_index_list)) == 0)
        {
            cli_out("Problem with field for memory:%s\n", SOC_MEM_NAME(unit, mem));
            continue;
        }

        SOC_MEM_BLOCK_ITER(unit, mem, i_bl)
        {
            /*
             * If block name filter was used and not matched skip this block
             */
            if (!ISEMPTY(block_n) && (sal_strcasestr(SOC_BLOCK_NAME(unit, i_bl), block_n) == NULL))
                continue;

            if (separate_flag == TRUE && print_flag == TRUE)
            {
                if (!PRT_TITLE_IS_SET)
                {
                    PRT_TITLE_SET("access read");
                    /*
                     * Prepare header, pay attention to put header items and content in the same order
                     */
                    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &object_col_id, "Object");
                    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Block");
                    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Id");
                    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Address");
                    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Value");
                    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Property");
                }

                if (sal_strcmp(folder_name, ""))
                {
                    sal_memset(help_filename, 0, sizeof(char) * 512);
                    sal_strncat(help_filename, folder_name, 512);
                    sal_strncat(help_filename, SOC_MEM_NAME(unit, mem), 512);
                    sal_strncat(help_filename, ".xml", 512);
                }
            }

            for (i_ind = index_start; i_ind < index_start + index_num; i_ind++)
            {
                if ((i_ind < index_range_start || i_ind > index_range_end) && has_index_range)
                    continue;

                for (i_entry = cur_entry; i_entry < cur_entry + cur_count; i_entry++)
                {
                    address = soc_mem_addr_get(unit, mem, i_ind, i_bl, i_entry, &acc_type);
                    if (cache == TRUE)
                    {
                        soc_error = soc_mem_array_read(unit, mem, i_ind, i_bl, i_entry, mem_value);
                    }
                    else
                    {
                        soc_error =
                            soc_mem_array_read_flags(unit, mem, i_ind, i_bl, i_entry, mem_value,
                                                     SOC_MEM_DONT_USE_CACHE);
                    }

                    if (soc_error < 0)
                    {
                        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Read ERROR: table %s.%s[%d]: %s\n", SOC_MEM_UFNAME(unit, mem),
                                     SOC_BLOCK_NAME(unit, i_bl), i_entry, soc_errmsg(soc_error));
                    }

                    if (print_flag == TRUE)
                    {
                        access_mem_print(prt_ctr, unit, mem, mem_size, i_bl, i_entry, i_ind, address, mem_value,
                                         field_flag, changed, fld_index_list, fld_match_list);
                    }
                }
            }
        }

        if (separate_flag == TRUE && print_flag == TRUE)
        {
            if (sal_strcmp(folder_name, ""))
            {
                PRT_COMMITF(help_filename);
            }
            else
            {
                PRT_COMMITX;
            }
        }
    }

    if (separate_flag != TRUE && print_flag == TRUE)
    {
        PRT_COMMITX;
    }
exit:
    if (mem_value != NULL)
    {
        sal_free(mem_value);
    }
    diag_sand_compare_close(match_h);
    diag_sand_compare_close(field_match_h);
    diag_sand_compare_close(property_h);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t access_write_man = {
    .brief = "Set registers/memories value.",
    .full = "Command allows to set a value to a register or memory field.\n"
        "Fields not specified in the list are left unaffected.\n",
    .synopsis =
        "[reg|mem] <REG/MEM Name> [block=<COPY>] [index=<INDEX>] [entry=<ENTRY>] [count=<ENTRYCOUNT>] [value=<FIELD>(<FIELDVALUE>),...| <DW0> .. <DWN>]\n",
    .examples =
        "mem TCAM_TCAM_BANK_COMMAND block=TCAM0 entry=0 count=1 value=TCAM_CPU_CMD_WR(3),TCAM_CPU_CMD_VALID(3),TCAM_CPU_CMD_RD(0),TCAM_CPU_CMD_CMP(0)\n"
        "mem IPPC_PMF_KBR_PASS_1 entry=0 count=1 value=\"0x0 0x0 0x0 0x0 0x1 0x0 0x0 0x0 0x0 0x0 0x0 0x5 0x0 0x0 0x0 0x0 0x8 0x0 0x0 0x11 0x3 0x0\"\n"
        "reg TCAM_TCAM_BANK_BLOCK_OWNER value=0 symbol=11\n"
        "mem TCAM_TCAM_ACTION entry=0 count=1 value=ACTION(0xFF) index=1 offset=4"
};

static sh_sand_option_t access_write_options[] = {
    {"name", SAL_FIELD_TYPE_STR, "Name of the mem/reg to be set", "", NULL, NULL, SH_SAND_ARG_FREE},
    {"value", SAL_FIELD_TYPE_STR, "New value of the mem/reg", ""},
    {"block", SAL_FIELD_TYPE_STR, "Specific block name of mem/reg to be set", ""},
    {"entry", SAL_FIELD_TYPE_STR, "Entry ID", ""},
    {"count", SAL_FIELD_TYPE_STR, "Number of entries to be set", ""},
    {"index", SAL_FIELD_TYPE_STR, "Memory array index", ""},
    {"offset", SAL_FIELD_TYPE_STR, "Offset", ""},
    {"symbol", SAL_FIELD_TYPE_STR, "Symbol", ""},
    {"register", SAL_FIELD_TYPE_BOOL, "Flag used to specify that we will set a value to a register", "No"},
    {"memory", SAL_FIELD_TYPE_BOOL, "Flag used to specify that we will set a value to a memory", "No"},
    {NULL}
};

static shr_error_e
access_write_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *name, *value, *block, *entry, *entrycount, *index, *symbol, *offset;
    char *h_value;
    char command[4095];
    uint8 reg, mem;
    int value_index;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    /*
     * Get parameters.
     */
    SH_SAND_GET_STR("name", name);
    SH_SAND_GET_STR("block", block);
    SH_SAND_GET_STR("entry", entry);
    SH_SAND_GET_STR("count", entrycount);
    SH_SAND_GET_STR("index", index);
    SH_SAND_GET_STR("value", value);
    SH_SAND_GET_STR("offset", offset);
    SH_SAND_GET_STR("symbol", symbol);
    SH_SAND_GET_BOOL("register", reg);
    SH_SAND_GET_BOOL("memory", mem);
    sal_memset(command, 0, 4095);

    if (reg == mem)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Specify reg or mem to be modified.\n");
    }

    if (reg == TRUE)
    {
        sal_memset(command, 0, 4095);
        if (ISEMPTY(name))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Set parameter \"name\" of the memory.\n");
        }
        if (ISEMPTY(value))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Set parameter \"value\" of the memory.\n");
        }
        sal_strcat(command, "s ");
        sal_strcat(command, name);

        if (ISEMPTY(offset))
        {
            sal_strcat(command, offset);
            sal_strcat(command, " ");
        }
        if (ISEMPTY(symbol))
        {
            sal_strcat(command, symbol);
            sal_strcat(command, " ");
        }

        if (!ISEMPTY(value))
        {
            char *s2;
            h_value = sal_strtok_r(value, ",", &s2);
            sal_strcat(command, " ");
            while (h_value != NULL)
            {
                for (value_index = 0; value_index < sal_strlen(h_value); value_index++)
                {
                    if (h_value[value_index] == '(')
                        h_value[value_index] = '=';
                    if (h_value[value_index] == ')')
                        h_value[value_index] = ' ';
                }
                sal_strcat(command, h_value);
                h_value = sal_strtok_r(NULL, ",", &s2);
            }
        }
        else
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Set parameters \"value\" of the memory.\n");
        }

        sh_process_command(unit, command);
    }

    if (mem == TRUE)
    {
        sal_memset(command, 0, 4095);
        if (ISEMPTY(name))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Set parameter \"name\" of the memory.\n");
        }
        if (ISEMPTY(entry))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Set parameter \"entry\" of the memory.\n");
        }
        if (ISEMPTY(entrycount))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Set parameter \"count\" of the memory.\n");
        }
        sal_strcat(command, "w ");
        sal_strcat(command, name);
        if (!ISEMPTY(block))
        {
            sal_strcat(command, ".");
            sal_strcat(command, block);
        }
        else if (!ISEMPTY(index))
        {
            sal_strcat(command, "[");
            sal_strcat(command, index);
            sal_strcat(command, "]");
        }
        sal_strcat(command, " ");
        sal_strcat(command, entry);
        sal_strcat(command, " ");
        sal_strcat(command, entrycount);
        sal_strcat(command, " ");

        if (!ISEMPTY(value))
        {
            h_value = strtok(value, ",");
            sal_strcat(command, " ");
            while (h_value != NULL)
            {
                for (value_index = 0; value_index < sal_strlen(h_value); value_index++)
                {
                    if (h_value[value_index] == '(')
                        h_value[value_index] = '=';
                    if (h_value[value_index] == ')')
                        h_value[value_index] = ' ';
                }
                sal_strcat(command, h_value);
                h_value = strtok(NULL, ",");
            }
        }
        else
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Set parameters \"value\" of the memory.\n");
        }

        sh_process_command(unit, command);
    }

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t access_modify_man = {
    .brief = "Modify registers/memories field value.",
    .full = "Command allows to modify register/memory field(s) value(s).\n"
        "Fields not specified in the list are left unaffected.\n",
    .synopsis =
        "[reg|mem] <REG/MEM Name> [block=<COPY>] [index=<INDEX>] [entry=<ENTRY>] [count=<ENTRYCOUNT>] [value=<FIELD>(<FIELDVALUE>),...]",
    .examples = "mem IPPC_PMF_KBR_PASS_1 entry=0 count=1 value=KEY_A_APP_DB(0x0)\n"
        "reg IPPD_ACTION_DISABLE value=ACR_VLAN_EDIT_CMD(1) block=IPPD0\n"
        "mem TCAM_TCAM_ACTION entry=0 count=1 value=ACTION(0xFF) index=1"
};

static sh_sand_option_t access_modify_options[] = {
    {"name", SAL_FIELD_TYPE_STR, "Name of the reg/mem to be set.", "", NULL, NULL, SH_SAND_ARG_FREE},
    {"register", SAL_FIELD_TYPE_BOOL, "Flag used to specify that we will set a value to a register.", "No"},
    {"memory", SAL_FIELD_TYPE_BOOL, "Flag used to specify that we will set a value to a memory.", "No"},
    {"value", SAL_FIELD_TYPE_STR, "Field and its new value.", ""},
    {"block", SAL_FIELD_TYPE_STR, "Block name of it to filter regs/mems.", ""},
    {"entry", SAL_FIELD_TYPE_STR, "Entry ID.", ""},
    {"count", SAL_FIELD_TYPE_STR, "Entres count.", ""},
    {"index", SAL_FIELD_TYPE_STR, "Memory Array Index.", ""},
    {NULL}
};

static shr_error_e
access_modify_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *name, *value, *block, *entry, *entrycount, *index;
    char *h_value;
    char command[1024];
    uint8 reg, mem;
    int value_index;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    /*
     * Get parameters
     */
    SH_SAND_GET_STR("name", name);
    SH_SAND_GET_STR("block", block);
    SH_SAND_GET_STR("entry", entry);
    SH_SAND_GET_STR("count", entrycount);
    SH_SAND_GET_STR("index", index);
    SH_SAND_GET_BOOL("register", reg);
    SH_SAND_GET_BOOL("memory", mem);
    SH_SAND_GET_STR("value", value);

    if (reg == mem)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Specify reg or mem will be modified.\n");
    }

    if (reg == TRUE)
    {
        sal_memset(command, 0, 1024);
        if (ISEMPTY(name))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Set parameter \"name\" of the memory.\n");
        }
        sal_strcat(command, "m ");
        sal_strcat(command, name);
        sal_strcat(command, " ");

        if (!ISEMPTY(value))
        {
            h_value = strtok(value, ",");
            sal_strcat(command, " ");
            while (h_value != NULL)
            {
                for (value_index = 0; value_index < sal_strlen(h_value); value_index++)
                {
                    if (h_value[value_index] == '(')
                        h_value[value_index] = '=';
                    if (h_value[value_index] == ')')
                        h_value[value_index] = ' ';
                }
                sal_strcat(command, h_value);
                h_value = strtok(NULL, ",");
            }
        }
        else
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Set parameters \"value\" of the memory.\n");
        }

        sh_process_command(unit, command);
    }

    if (mem == TRUE)
    {
        sal_memset(command, 0, 1024);
        if (ISEMPTY(name))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Set parameter \"name\" of the memory.\n");
        }
        if (ISEMPTY(entry))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Set parameter \"entry\" of the memory.\n");
        }
        if (ISEMPTY(entrycount))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Set parameter \"count\" of the memory.\n");
        }
        sal_strcat(command, "mod ");
        sal_strcat(command, name);
        if (!ISEMPTY(index))
        {
            sal_strcat(command, "[");
            sal_strcat(command, index);
            sal_strcat(command, "]");
        }
        if (!ISEMPTY(block))
        {
            sal_strcat(command, ".");
            sal_strcat(command, block);
        }
        sal_strcat(command, " ");
        sal_strcat(command, entry);
        sal_strcat(command, " ");
        sal_strcat(command, entrycount);
        sal_strcat(command, " ");

        if (!ISEMPTY(value))
        {
            char *s2;
            h_value = sal_strtok_r(value, ",", &s2);
            sal_strcat(command, " ");
            while (h_value != NULL)
            {
                for (value_index = 0; value_index < sal_strlen(h_value); value_index++)
                {
                    if (h_value[value_index] == '(')
                        h_value[value_index] = '=';
                    if (h_value[value_index] == ')')
                        h_value[value_index] = ' ';
                }
                sal_strcat(command, h_value);
                h_value = sal_strtok_r(NULL, ",", &s2);
            }
        }
        else
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Set parameters \"value\" of the memory.\n");
        }

        sh_process_command(unit, command);
    }

exit:
    SHR_FUNC_EXIT;
}

#ifdef ADAPTER_SERVER_MODE

#define ADAPTER_HIT_INFO_SIZE 17

#include <soc/dnx/adapter/adapter_reg_access.h>
static sh_sand_man_t access_hit_man = {
    .brief = "Obtain hit entries from adapter server and read the content",
    .full = "Command allows to display or to save in XML file value of registers&memories hitted in cmodle processing",
    .synopsis = "[name=<regular expr>] [block=<substring>] [field] [all] [property=<regular expr>]",
    .examples = "block=IPPD0 name=LBP changed\n" "read field name=LIF\n" "dbal clear\n"
        "all raw\n" "prp=mem name=TCAM block=IPPA0\n" "name=IPPB cache=no"
};

static sh_sand_option_t access_hit_options[] = {
    {"name", SAL_FIELD_TYPE_STR, "Regular expression used to filter regs/mems and their fields", "", NULL, NULL,
     SH_SAND_ARG_FREE},
    {"block", SAL_FIELD_TYPE_STR, "Block name or any substring of it to filter regs/mems", ""},
    {"all", SAL_FIELD_TYPE_BOOL, "Ignore filters and print all hit regs/mems", "No"},
    {"dbal", SAL_FIELD_TYPE_BOOL, "Ignore filters and print all hit regs/mems in different output, "
     "e.g. for DBAL usage", "No"},
    {"field", SAL_FIELD_TYPE_BOOL, "Include fields in regs/mems search and output", "No"},
    {"read", SAL_FIELD_TYPE_BOOL, "Read hit entries", "No"},
    {"changed", SAL_FIELD_TYPE_BOOL, "Print only when value is different from reset one", "No"},
    {"property", SAL_FIELD_TYPE_STR, "Name or name substring for regs/mems properties/n"
     "<reg/mem/array/ro/wo regtype name<cpu/general/iproc/custom/port>", ""},
    {"cache", SAL_FIELD_TYPE_BOOL, "flag used to obtain non-cached memory value", "Yes"},
    {"raw", SAL_FIELD_TYPE_BOOL, "Show raw numbers from adapter", "No"},
    {"clear", SAL_FIELD_TYPE_BOOL, "Flag used to delete the hit bit indication record.", "No"},
    {NULL}
};

static shr_error_e
access_hit_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *block_n, *match_n, *property_n, *field_match_n;
    int fld_index_list[MAX_FIELDS_NUM];
    int fld_match_list[MAX_FIELDS_NUM];

    int field_flag, all_flag, read_flag, raw_flag, dbal_flag;
    int object_col_id;
    void *match_h = NULL, *property_h = NULL, *field_match_h = NULL;
    uint32 address, base_address;
    int cache, changed;
    int dual_match;
    uint32 hit_data_length;
    char *hit_data;
    uint8 opcode;
    int cmic_block_id;
    int offset;
    int clear = 0;
    soc_regtype_t regtype;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("clear", clear);

    /*
     * First obtain buffer in order to see if we have anything to work with
     */
    SHR_IF_ERR_EXIT(adapter_read_hit_bits(unit, clear, &hit_data_length, &hit_data));

    if ((hit_data_length % ADAPTER_HIT_INFO_SIZE) != 0)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Bad hit buffer size:%d\n", hit_data_length);
    }
    /*
     * Get parameters
     */
    SH_SAND_GET_STR("name", match_n);
    SH_SAND_GET_STR("block", block_n);
    SH_SAND_GET_STR("property", property_n);
    SH_SAND_GET_BOOL("field", field_flag);
    SH_SAND_GET_BOOL("all", all_flag);
    SH_SAND_GET_BOOL("dbal", dbal_flag);
    SH_SAND_GET_BOOL("read", read_flag);
    SH_SAND_GET_BOOL("changed", changed);
    SH_SAND_GET_BOOL("cache", cache);
    SH_SAND_GET_BOOL("raw", raw_flag);

    if ((all_flag == TRUE) || (dbal_flag == TRUE))
    {
        match_n = block_n = property_n = "";
    }
    else
    {
        if (ISEMPTY(match_n) && ISEMPTY(block_n) && ISEMPTY(property_n))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Use \"all\" to see the entire hit list or use filtering option\n");
        }
    }

    utilex_str_to_upper(match_n);

    dual_match = access_fields_match_parse(match_n, &field_match_n);
    if (dual_match == TRUE)
    {
        field_flag = TRUE;
    }

    SHR_IF_ERR_EXIT(diag_sand_compare_init(match_n, &match_h));
    SHR_IF_ERR_EXIT(diag_sand_compare_init(field_match_n, &field_match_h));
    SHR_IF_ERR_EXIT(diag_sand_compare_init(property_n, &property_h));

    PRT_TITLE_SET("access hit");
    /*
     * Prepare header, pay attention to put header items and content in the same order
     */
    if (dbal_flag == TRUE)
    {
        rhlist_t *hit_list;
        shr_hit_entry_t *hit_entry;
        int flags = 0;
        if (changed == TRUE)
        {
            flags |= HIT_AVOID_REPEAT;
        }

        SHR_IF_ERR_EXIT(shr_access_hit_get(unit, &hit_list, flags));

        PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &object_col_id, "Object");
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Block");
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Address");
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Offset");
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Opcode");
        if (changed == TRUE)
        {
            PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Count");
        }

        RHITERATOR(hit_entry, hit_list)
        {
            char *opcode_n;
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", RHNAME(hit_entry));
            PRT_CELL_SET("%s", hit_entry->block_n);
            PRT_CELL_SET("%08x", hit_entry->address);
            PRT_CELL_SET("%d", hit_entry->offset);
            opcode_n = shr_access_hit_opcode_name(hit_entry->opcode);
            if (ISEMPTY(opcode_n))
            {
                PRT_CELL_SET("%s(%d)", "unsupported", hit_entry->opcode);
            }
            else
            {
                PRT_CELL_SET("%s", opcode_n);
            }
            if (changed == TRUE)
            {
                PRT_CELL_SET("%d", hit_entry->count + 1);
            }
        }
        PRT_COMMITX;
        SHR_EXIT();
    }
    else if (raw_flag == TRUE)
    {
        PRT_COLUMN_ADD("Opcode");
        PRT_COLUMN_ADD("Unit");
        PRT_COLUMN_ADD("CMIC Block");
        PRT_COLUMN_ADD("Address");
        PRT_COLUMN_ADD("Offset");
    }
    else
    {
        PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &object_col_id, "Object");
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Block");
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Index");
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Address");
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Value");
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Property");
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Opcode");
    }

    while (hit_data_length >= ADAPTER_HIT_INFO_SIZE)
    {
        int unit;
        opcode = *hit_data++;
        unit = bcm_ntohl(*((int *) hit_data));
        hit_data += sizeof(int);
        cmic_block_id = bcm_ntohl(*((int *) hit_data));
        hit_data += sizeof(int);
        base_address = bcm_ntohl(*((uint32 *) hit_data));
        hit_data += sizeof(uint32);
        offset = bcm_ntohl(*((int *) hit_data));
        hit_data += sizeof(int);
        hit_data_length -= ADAPTER_HIT_INFO_SIZE;

        if (raw_flag == TRUE)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%d", opcode);
            PRT_CELL_SET("0x%08x", unit);
            PRT_CELL_SET("0x%08x", cmic_block_id);
            PRT_CELL_SET("0x%08x", base_address);
            PRT_CELL_SET("0x%08x", offset);
            continue;
        }
        if ((opcode == UPDATE_TO_SOC_REG_READ) || (opcode == UPDATE_TO_SOC_REG_WRITE))
        {
            soc_reg_t reg;
            for (reg = 0; reg < NUM_SOC_REG; reg++)
            {
                soc_reg_info_t *reginfo;
                soc_reg_above_64_val_t reg_value;
                char prop_str[PRT_COLUMN_WIDTH_BIG];
                char block_str[PRT_COLUMN_WIDTH_BIG];
                int port;
                pbmp_t bm;
                int i_bl;

                if (!SOC_REG_IS_VALID(unit, reg))
                    continue;

                reginfo = &SOC_REG_INFO(unit, reg);

                if (base_address != SOC_REG_BASE(unit, reg))
                    continue;

                if ((shr_access_reg_present_in_block(unit, cmic_block_id, reginfo->block, &i_bl)) != _SHR_E_NONE)
                    continue;

                if ((offset < 0) || (offset >= SOC_REG_NUMELS(unit, reg)))
                {
                    cli_out("Block:0x%08x Offset:0x%08x Base:0x%08x Register:%s Invalid Offset", i_bl, offset,
                            base_address, SOC_REG_NAME(unit, reg));
                    continue;
                }
                else if ((i_bl < 0) || (i_bl >= SOC_MAX_NUM_BLKS))
                {
                    cli_out("Block:0x%08x Offset:0x%08x Base:0x%08x Register:%s Invalid Block", i_bl, offset,
                            base_address, SOC_REG_NAME(unit, reg));
                    continue;
                }

                /*
                 * Filter per register and field names 
                 */
                if (access_obj_match(dual_match, SOC_REG_NAME(unit, reg), reginfo->fields, reginfo->nFields,
                                     match_h, field_match_h, fld_match_list) != _SHR_E_NONE)
                    continue;
                /*
                 * Filter on properties
                 */
                if (access_reg_properties_get(unit, reg, prop_str, property_h) != _SHR_E_NONE)
                    continue;
                /*
                 * Filter on blocks
                 */
                if (diag_sand_reg_blocks_get(unit, reginfo, block_str, block_n) != _SHR_E_NONE)
                    continue;

                regtype = reginfo->regtype;
                if (soc_is_reg_flexe_core(unit, reg))
                {
                    regtype = soc_genreg;
                }
                switch (regtype)
                {
                    case soc_portreg:
                        PBMP_ITER(PBMP_PORT_ALL(unit), port)
                    {
                        if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_PORT_TYPE(unit, port)))
                        {
                            if (!ISEMPTY(block_n) && (sal_strcasestr(SOC_PORT_NAME(unit, port), block_n) == NULL))
                                continue;
                            if (read_flag)
                            {
                                access_reg_read(unit, reg, 0, port, reginfo->first_array_index, reg_value, NULL);
                            }
                            else
                            {
                                SOC_REG_ABOVE_64_CLEAR(reg_value);
                            }
                            access_reg_print(prt_ctr, unit, reg, SOC_PORT_NAME(unit, port), reginfo->first_array_index,
                                             SOC_REG_BASE(unit, reg), reg_value, field_flag, changed, fld_match_list);
                        }
                    }
                        break;
                    case soc_customreg:
                        BCM_PBMP_ASSIGN(bm, SOC_INFO(unit).custom_reg_access.custom_port_pbmp);
                        PBMP_ITER(bm, port)
                        {
                            if (!SOC_PORT_VALID(unit, port))
                            {
                                continue;
                            }

                            if (!ISEMPTY(block_n) && (sal_strcasestr(SOC_PORT_NAME(unit, port), block_n) == NULL))
                                continue;

                            if (read_flag)
                            {
                                access_reg_read(unit, reg, 0, port, reginfo->first_array_index, reg_value, NULL);
                            }
                            else
                            {
                                SOC_REG_ABOVE_64_CLEAR(reg_value);
                            }
                            access_reg_print(prt_ctr, unit, reg, SOC_PORT_NAME(unit, port), reginfo->first_array_index,
                                             SOC_REG_BASE(unit, reg), reg_value, field_flag, changed, fld_match_list);
                        }
                        break;
                    case soc_iprocreg:
                        break;
                    default:
                        /*
                         * If block name filter was used and not matched skip this block
                         */
                        if (!ISEMPTY(block_n) && (sal_strcasestr(SOC_BLOCK_NAME(unit, i_bl), block_n) == NULL))
                            continue;
                        /*
                         * Put reg name for each block to keep consistency
                         */
                        if (read_flag)
                        {
                            access_reg_read(unit, reg, i_bl, 0, offset + reginfo->first_array_index, reg_value,
                                            &address);
                        }
                        else
                        {
                            uint8 acc_type;
                            int blk;

                            SOC_REG_ABOVE_64_CLEAR(reg_value);
                            address =
                                soc_reg_addr_get(unit, reg, SOC_BLOCK_PORT(unit, i_bl),
                                                 offset + reginfo->first_array_index, SOC_REG_ADDR_OPTION_NONE, &blk,
                                                 &acc_type);
                        }
                        access_reg_print(prt_ctr, unit, reg, SOC_BLOCK_NAME(unit, i_bl),
                                         offset + reginfo->first_array_index, address, reg_value, field_flag, changed,
                                         fld_match_list);
                        break;
                }
                if (opcode == UPDATE_TO_SOC_REG_READ)
                {
                    PRT_CELL_SET("RD");
                }
                else
                {
                    PRT_CELL_SET("WR");
                }
            }
        }
        else if ((opcode == UPDATE_TO_SOC_MEM_READ) || (opcode == UPDATE_TO_SOC_MEM_WRITE))
        {       /* Opcode was for memory */
            soc_mem_t mem;
            for (mem = 0; mem < NUM_SOC_MEM; mem++)
            {
                soc_mem_info_t *meminfo;
                uint8 acc_type;
                uint32 mem_value[SOC_MAX_MEM_WORDS];
                int soc_error;
                int mem_size;
                char prop_str[PRT_COLUMN_WIDTH_BIG];
                char block_str[PRT_COLUMN_WIDTH_BIG];
                int i_entry, i_ind, i_bl;

                if (!SOC_MEM_IS_VALID(unit, mem))
                    continue;

                meminfo = &SOC_MEM_INFO(unit, mem);

                /*
                 * Check that base addresses match 
                 */
                if (base_address != meminfo->base)
                    continue;

                if ((shr_access_mem_present_in_block(unit, cmic_block_id, mem, &i_bl)) != _SHR_E_NONE)
                    continue;

                /*
                 * Filter per memory and field names 
                 */
                if (access_obj_match(dual_match, SOC_MEM_NAME(unit, mem), meminfo->fields, meminfo->nFields,
                                     match_h, field_match_h, fld_match_list) != _SHR_E_NONE)
                    continue;
                /*
                 * Fill properties
                 */
                if (access_mem_properties_get(unit, mem, prop_str, property_h) != _SHR_E_NONE)
                    continue;
                /*
                 * Fill blocks
                 */
                if (diag_sand_mem_blocks_get(unit, mem, block_str, block_n) != _SHR_E_NONE)
                    continue;

                if ((mem_size = access_fields_get_ordered_list(meminfo->fields, meminfo->nFields, fld_index_list)) == 0)
                {
                    cli_out("Problem with field for memory:%s\n", SOC_MEM_NAME(unit, mem));
                    continue;
                }

                if ((meminfo->flags & SOC_MEM_FLAG_IS_ARRAY) && (SOC_MEM_ARRAY_INFOP(unit, mem) != NULL))
                {
                    i_entry = offset % SOC_MEM_ELEM_SKIP_SAFE(unit, mem);
                    i_ind = offset / SOC_MEM_ELEM_SKIP_SAFE(unit, mem);

                    if (i_ind >= SOC_MEM_NUMELS_SAFE(unit, mem))
                    {
                        cli_out("Offset:0x%08x is too big for memory:%s(0x%08x)\n", offset, SOC_MEM_NAME(unit, mem),
                                base_address);
                        continue;
                    }
                }
                else
                {
                    i_entry = offset;
                    i_ind = 0;
                }

                address = soc_mem_addr_get(unit, mem, i_ind, i_bl, i_entry, &acc_type);
                if (read_flag)
                {
                    if (cache == TRUE)
                    {
                        soc_error = soc_mem_array_read(unit, mem, i_ind, i_bl, i_entry, mem_value);
                    }
                    else
                    {
                        soc_error =
                            soc_mem_array_read_flags(unit, mem, i_ind, i_bl, i_entry, mem_value,
                                                     SOC_MEM_DONT_USE_CACHE);
                    }
                }
                else
                {
                    memset(mem_value, 0, SOC_MAX_MEM_WORDS * 4);
                    soc_error = 0;
                }
                if (soc_error < 0)
                {
                    SHR_CLI_EXIT(_SHR_E_INTERNAL, "Read ERROR: Memory Entry %s:[0x%08x]: %s\n",
                                 SOC_MEM_UFNAME(unit, mem), address, soc_errmsg(soc_error));
                }

                access_mem_print(prt_ctr, unit, mem, mem_size, i_bl, i_entry, i_ind, address, mem_value, field_flag,
                                 changed, fld_index_list, fld_match_list);
                if (opcode == UPDATE_TO_SOC_MEM_READ)
                {
                    PRT_CELL_SET("RD");
                }
                else
                {
                    PRT_CELL_SET("WR");
                }
            }
        }
        else
        {
            cli_out("Bad opcode %d for address:0x%08x\n", opcode, base_address);
        }
    }

    PRT_COMMITX;
exit:
    diag_sand_compare_close(match_h);
    diag_sand_compare_close(field_match_h);
    diag_sand_compare_close(property_h);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

#endif

sh_sand_man_t sh_sand_access_man = {
    .brief = cmd_sand_access_desc,
    .full = cmd_sand_access_usage
};

sh_sand_cmd_t sh_sand_access_cmds[] = {
    {"blocks", access_block_cmd, NULL, access_block_options, &access_block_man}
    ,
    {"list", access_list_cmd, NULL, access_list_options, &access_list_man}
    ,
    {"read", access_read_cmd, NULL, access_read_options, &access_read_man}
    ,
#ifdef ADAPTER_SERVER_MODE
    {"hit", access_hit_cmd, NULL, access_hit_options, &access_hit_man, NULL, NULL, SH_CMD_SKIP_EXEC}
    ,
#endif
    {"property", access_property_cmd, NULL, NULL, &access_property_man}
    ,
    {"write", access_write_cmd, NULL, access_write_options, &access_write_man}
    ,
    {"modify", access_modify_cmd, NULL, access_modify_options, &access_modify_man}
    ,
    {NULL}
};

/*
 * This routine is for DPP/DFE only - For DNX/DNXF recursion starts from the top
 */
cmd_result_t
cmd_sand_access(
    int unit,
    args_t * args)
{
    cmd_result_t result;
    SH_SAND_VERIFY("ACCess", sh_sand_access_cmds, result);

    sh_sand_act(unit, args, sh_sand_access_cmds, sh_sand_sys_cmds, NULL);
    ARG_DISCARD(args);
    /*
     * Always return OK - we provide all help & usage from inside framework
     */
exit:
    return result;
}

char cmd_sand_intr_usage[] =
    "Usages:\n" "\tintr <enable/disable> <intr_id> [port]\n" "\t\tport - interrupt port, by default zero\n";

cmd_result_t
cmd_sand_intr(
    int unit,
    args_t * a)
{
    char *func, *c;
    int intr_id, port = 0, rc;
    soc_interrupt_db_t intr;

    if (!(func = ARG_GET(a)))
    {   /* Nothing to do */
        return (CMD_USAGE);     /* Print usage line */
    }

    if (!(c = ARG_GET(a)))
    {   /* Nothing to do */
        return (CMD_USAGE);     /* Print usage line */
    }

    intr_id = sal_ctoi(c, NULL);

    if ((c = ARG_GET(a)))
    {
        port = sal_ctoi(c, NULL);
    }

    rc = soc_interrupt_info_get(unit, intr_id, &intr);
    if (rc)
    {
        cli_out("unable to get interrupt: %d info \n", intr_id);
        return CMD_FAIL;
    }

    if (!sal_strcasecmp(func, "enable"))
    {
        rc = soc_interrupt_enable(unit, port, &intr);
        if (rc)
        {
            cli_out("soc_interrupt_enable failed");
            return CMD_FAIL;
        }
    }
    else if (!sal_strcasecmp(func, "disable"))
    {
        rc = soc_interrupt_disable(unit, port, &intr);
        if (rc)
        {
            cli_out("soc_interrupt_disable failed");
            return CMD_FAIL;
        }
    }
    else
    {
        return (CMD_USAGE);
    }

    return (CMD_OK);
}

/*
 * General shell style usage
 */
const char cmd_sand_access_usage[] = "Please use \"access(acc) list/read usage\" for help\n";

/*
 * General shell style description
 */
const char cmd_sand_access_desc[] = "List/Read/Modify registers&memories";
