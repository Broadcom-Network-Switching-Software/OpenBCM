/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_SYMTAB

#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <shared/error.h>
#include <appl/diag/shell.h>
#include <soc/sand/shrextend/shrextend_debug.h>

#ifndef NO_FRAMER_LIB_BUILD
#include "diag_framer_symbols.h"
#include "diag_framer_access.h"

#define FRAMER_SYMBOL_DISPLAY_F_BRIEF           (0x1 << 0)
#define FRAMER_SYMBOL_DISPLAY_F_DATA_NO_SPACE   (0x1 << 1)
#define FRAMER_SYMBOL_DISPLAY_F_DBG_INFO        (0x1 << 6)
#define FRAMER_SYMBOL_DISPLAY_F_CFG_SYM         (0x1 << 7)

#define FRAMER_SYMBOL_F_NON_CFG \
        (FRAMER_SYMBOL_F_INIT_ON_READ | \
         FRAMER_SYMBOL_F_INTR | \
         FRAMER_SYMBOL_F_RO | \
         FRAMER_SYMBOL_F_WO | \
         FRAMER_SYMBOL_F_COUNTER | \
         FRAMER_SYMBOL_F_RST | \
         FRAMER_SYMBOL_F_ALARM)

char cmd_reg_get_usage[] =
    "Usages:\n\t"
    "Get the register data and parse field values.\n\t"
    "\n"
    "Options:\n\t"
    "Get [raw|chg|ctr|alm|dbg|pfx|brief] <REGISTER>[ARRAY][.<COPY>[BASE_ADDR][OFFSET]]\n\t"
    "raw - Get in raw format.\n\t"
    "chg - Get changed registers.\n\t"
    "ctr - Get counter registers.\n\t"
    "alm - Get alarm registers.\n\t"
    "dbg - Get registers in debug format.\n\t"
    "dbginfo - Get registers in debug format and print field debug info.\n\t"
    "pfx - Get registers in prefix format.\n\t"
    "brief - Get registers in brief format.\n\t"
    "cfg - Display configuration registers.\n\t"
    "\n"
    "Examples:\n\t"
    "Get chg *\n\t"
    "Get chg *.FBSW\n\t"
    "Get B66SW\n\t"
    "Get B66SW__FULL_B66SW\n\t"
    "Get B66SW__FULL_B66SW.FBSW\n\t"
    "Get ctr *\n\t"
    "Get ctr *.FBSW\n\t"
    "Get chg ctr *\n\t"
    "Get chg ctr *.FBSW\n\t"
    "Get alm *\n\t"
    "Get alm *.FBSW\n\t"
    "Get chg alm *\n\t"
    "Get chg alm *.FBSW\n\t"
    "Get brief B66SW__FULL_B66SW.FBSW[0x30000][0x9e]\n\t"
    "Get dbg brief B66SW__FULL_B66SW.FBSW[0x30000][0x9e]\n\t" "\n";

char cmd_reg_set_usage[] =
    "Usages:\n\t"
    "Set the register data.\n\t"
    "\n"
    "Options:\n\t"
    "Set [raw|pfx] <REGISTER>[ARRAY][.<COPY>[BASE_ADDR][OFFSET]] <DATA|0xHHHH>\n\t"
    "raw - Set in raw format.\n\t"
    "pfx - Set in prefix format.\n\t"
    "dbginfo - Set register and print debug info.\n\t"
    "\n"
    "Examples:\n\t"
    "Set OHIF_OTN_1__INS_PKT_DA_SA.FBSW 0x000000000000000000010000\n\t"
    "Set OHIF_OTN_1__INS_PKT_DA_SA.FBSW[0x30500][0x12] 0x000000000000000000010000\n\t" "\n";

char cmd_reg_modify_usage[] =
    "Usages:\n\t"
    "Modify specified field value for a register.\n\t"
    "\n"
    "Options:\n\t"
    "Modreg [raw|chg|ctr|alm|dbg|pfx|brief] <REGISTER>[ARRAY][.<COPY>[BASE_ADDR][OFFSET]] <FIELD>=<VALUE|0xHHHH>[ ...]\n\t"
    "raw - Modify register in raw format.\n\t"
    "pfx - Modify register in prefix format.\n\t"
    "dbginfo - Modify register in prefix format and print field debug info.\n\t"
    "\n"
    "Examples:\n\t"
    "Modreg OHIF_OTN_1__INS_PKT_DA_SA.FBSW INS_PKT_DA=0xabde INS_PKT_SA=0x1234\n\t"
    "Modreg OHIF_OTN_1__INS_PKT_DA_SA.FBSW[0x30500][0x12] INS_PKT_DA=0xabde INS_PKT_SA=0x1234\n\t" "\n";

typedef struct diag_reg_s
{
    uint32 flags;
    void *cookie;
} diag_reg_t;

static int
getreg_print_entry(
    int unit,
    uint32 flags,
    framer_symbol_t * sym,
    void *user_data)
{
    int w;
    char *regname = NULL;
    const char *blk_name[] = FRAMER_BLK_NAME_STR;
    int midx;
    uint32 *data = NULL;
    diag_reg_t *dr = (diag_reg_t *) user_data;
    uint32 rflags = dr->flags;
    framer_symbol_field_print_ud_t ud;
    uint32 fflags = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (rflags & FRAMER_SYMBOL_DISPLAY_F_CFG_SYM)
    {
        if (sym->flags & FRAMER_SYMBOL_F_NON_CFG)
        {
            SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
        }
    }

    data = sal_alloc(sizeof(uint32) * sym->count, "RegData");
    SHR_NULL_CHECK(data, _SHR_E_MEMORY, "data is NULL!");

    sal_memset(data, 0, sizeof(uint32) * sym->count);
    SHR_IF_ERR_EXIT(framer_reg_raw_read(unit, sym->base_addr, sym->offset, sym->count, data));
    if (flags & FRAMER_REGSFILE_F_CHG)
    {
        for (w = 0; w < sym->count; w++)
        {
            if (data[w] != sym->sinfo[w]->default_value)
            {
                break;
            }
        }
        if (w >= sym->count)
        {
            SHR_EXIT();
        }
    }

    FRAMER_SYMBOL_M_GET(sym->index, midx);

    if (sym->alias[0] != '\0')
    {
        regname = &sym->alias[0];
    }
    else
    {
        regname = &sym->sname[0];
    }
    cli_out("%s", regname);
    if (sym->flags & FRAMER_SYMBOL_F_ARRAY)
    {
        cli_out("[%d]", midx);
    }
    if (sym->blk_no < 0)
    {
        cli_out(".%s", sym->blk < 0 ? FRAMER_BLK_NAME_RAW : blk_name[sym->blk]);
    }
    else
    {
        cli_out(".%s%d", sym->blk < 0 ? FRAMER_BLK_NAME_RAW : blk_name[sym->blk], sym->blk_no);
    }
    cli_out("[0x%x][0x%x] = 0x", sym->base_addr, sym->offset);
    for (w = sym->count - 1; w >= 0; w--)
    {
        cli_out("%08x", data[w]);
        if ((rflags & FRAMER_SYMBOL_DISPLAY_F_DATA_NO_SPACE) == 0)
        {
            cli_out(" ");
        }
    }
    cli_out("\n");
    if ((rflags & FRAMER_SYMBOL_DISPLAY_F_BRIEF) == 0)
    {
        if (rflags & FRAMER_SYMBOL_DISPLAY_F_DBG_INFO)
        {
            fflags |= FRAMER_SYMBOL_FIELD_F_DBG;
        }
        ud.pfx = "\t";
        ud.sfx = NULL;
        ud.dw = sym->count;
        ud.data = data;
        SHR_IF_ERR_EXIT(framer_symbol_field_iter(unit, fflags, sym, framer_symbol_field_print, &ud));
    }
exit:
    if (data)
    {
        sal_free(data);
        data = NULL;
    }
    SHR_FUNC_EXIT;
}

/*
 * Gets a memory value or register from the SOC.
 * Syntax: getreg [<regtype>] <offset|symbol>
 */

cmd_result_t
cmd_reg_get(
    int unit,
    args_t * a)
{
    int rv = CMD_OK;
    char *name, *extra_args = NULL;
    uint32 flags = 0, rflags = 0;
    uint32 dflen;
    diag_reg_t dr;
    framer_symbol_filter_t filter;

    SHR_FUNC_INIT_VARS(unit);

    if (0 == sh_check_attached(ARG_CMD(a), unit))
    {
        SHR_IF_ERR_EXIT(CMD_FAIL);
    }
    name = ARG_GET(a);
    dflen = SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH;
    for (;;)
    {
        if (name && !sal_strncasecmp(name, "raw", sal_strnlen(name, dflen)))
        {
            flags |= FRAMER_REGSFILE_F_RAW;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "hex", sal_strnlen(name, dflen)))
        {
            flags |= FRAMER_REGSFILE_F_HEX;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "chg", sal_strnlen(name, dflen)))
        {
            flags |= FRAMER_REGSFILE_F_CHG;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "ctr", sal_strnlen(name, dflen)))
        {
            flags |= FRAMER_REGSFILE_F_CTR;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "alm", sal_strnlen(name, dflen)))
        {
            flags |= FRAMER_REGSFILE_F_ALM;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "dbg", sal_strnlen(name, dflen)))
        {
            flags |= FRAMER_REGSFILE_F_DBG;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "dbginfo", sal_strnlen(name, dflen)))
        {
            flags |= FRAMER_REGSFILE_F_DBG;
            rflags |= FRAMER_SYMBOL_DISPLAY_F_DBG_INFO;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "pfx", sal_strnlen(name, dflen)))
        {
            flags |= FRAMER_REGSFILE_F_PFX;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "brief", sal_strnlen(name, dflen)))
        {
            rflags |= FRAMER_SYMBOL_DISPLAY_F_BRIEF;
            rflags |= FRAMER_SYMBOL_DISPLAY_F_DATA_NO_SPACE;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "cfg", sal_strnlen(name, dflen)))
        {
            rflags |= FRAMER_SYMBOL_DISPLAY_F_CFG_SYM;
            name = ARG_GET(a);
        }
        else
        {
            break;
        }
    }
    if (name == NULL)
    {
        SHR_IF_ERR_EXIT_NO_MSG(CMD_USAGE);
    }
    dr.flags = rflags;
    dr.cookie = NULL;
    framer_symbol_filter_t_init(&filter);
    filter.name = name;
    filter.flags = FRAMER_FILTER_DBG | FRAMER_FILTER_CONT_ON_ERR;
    filter.cb = getreg_print_entry;
    filter.user_data = &dr;
    rv = framer_symbol_filter(unit, FRAMER_REGSFILE_F_REG | flags, &filter);
    if (rv < 0)
    {
        if (rv == _SHR_E_NOT_FOUND)
        {
            cli_out("No register found with the substring '%s' in its name.\n", name);
        }
        SHR_IF_ERR_EXIT_NO_MSG(CMD_FAIL);
    }
    if (ARG_CNT(a) > 0)
    {
        extra_args = ARG_CUR(a);
        if (extra_args && extra_args[0] == '#')
        {
            ARG_DISCARD(a);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static int
setreg_entry(
    int unit,
    uint32 flags,
    framer_symbol_t * sym,
    void *user_data)
{
    int w, midx;
    diag_reg_t *dr = (diag_reg_t *) user_data;
    uint32 rflags = dr->flags;
    char *data_str = (char *) dr->cookie;
    const char *blk_name[] = FRAMER_BLK_NAME_STR;
    char *regname = NULL;
    uint32 *data = NULL;

    SHR_FUNC_INIT_VARS(unit);

    data = sal_alloc(sizeof(uint32) * sym->count, "RegData");
    SHR_NULL_CHECK(data, _SHR_E_MEMORY, "data is NULL!");
    sal_memset(data, 0, sizeof(uint32) * sym->count);

    SHR_IF_ERR_EXIT(framer_symbol_str2data(unit, data_str, sym->count, data));

    if (rflags & FRAMER_SYMBOL_DISPLAY_F_DBG_INFO)
    {
        FRAMER_SYMBOL_M_GET(sym->index, midx);

        if (sym->alias[0] != '\0')
        {
            regname = &sym->alias[0];
        }
        else
        {
            regname = &sym->sname[0];
        }
        cli_out("Set %s", regname);
        if (sym->flags & FRAMER_SYMBOL_F_ARRAY)
        {
            cli_out("[%d]", midx);
        }
        if (sym->blk_no < 0)
        {
            cli_out(".%s", sym->blk < 0 ? FRAMER_BLK_NAME_RAW : blk_name[sym->blk]);
        }
        else
        {
            cli_out(".%s%d", sym->blk < 0 ? FRAMER_BLK_NAME_RAW : blk_name[sym->blk], sym->blk_no);
        }
        cli_out(" %s\n", data_str);
        for (w = 0; w < sym->count; w++)
        {
            cli_out("   data[%d] = 0x%08x\n", w, data[w]);
        }
    }

    SHR_IF_ERR_EXIT(framer_reg_raw_write(unit, sym->base_addr, sym->offset, sym->count, data));

exit:
    if (data)
    {
        sal_free(data);
        data = NULL;
    }
    SHR_FUNC_EXIT;
}

/*
 * Sets a register value to the SOC.
 * Syntax: setreg [<regtype>] <symbol> <0xHHHH>
 */

cmd_result_t
cmd_reg_set(
    int unit,
    args_t * a)
{
    int rv = CMD_OK;
    char *name;
    uint32 flags = 0;
    uint32 dflen;
    framer_symbol_filter_t filter;
    diag_reg_t dr;
    uint32 rflags = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (0 == sh_check_attached(ARG_CMD(a), unit))
    {
        SHR_IF_ERR_EXIT(CMD_FAIL);
    }
    name = ARG_GET(a);
    dflen = SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH;
    for (;;)
    {
        if (name && !sal_strncasecmp(name, "raw", sal_strnlen(name, dflen)))
        {
            flags |= FRAMER_REGSFILE_F_RAW;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "pfx", sal_strnlen(name, dflen)))
        {
            flags |= FRAMER_REGSFILE_F_PFX;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "dbginfo", sal_strnlen(name, dflen)))
        {
            rflags |= FRAMER_SYMBOL_DISPLAY_F_DBG_INFO;
            name = ARG_GET(a);
        }
        else
        {
            break;
        }
    }
    if (name == NULL)
    {
        SHR_IF_ERR_EXIT_NO_MSG(CMD_USAGE);
    }
    dr.flags = rflags;
    dr.cookie = ARG_GET(a);
    framer_symbol_filter_t_init(&filter);
    filter.name = name;
    filter.flags = FRAMER_FILTER_DBG | FRAMER_FILTER_CONT_ON_ERR;
    filter.cb = setreg_entry;
    filter.user_data = &dr;
    rv = framer_symbol_filter(unit, FRAMER_REGSFILE_F_REG | flags, &filter);
    if (rv < 0)
    {
        SHR_IF_ERR_EXIT_NO_MSG(CMD_FAIL);
    }

exit:
    SHR_FUNC_EXIT;
}

static int
modreg_entry(
    int unit,
    uint32 flags,
    framer_symbol_t * sym,
    void *user_data)
{
    int i;
    char *regname = NULL;
    const char *blk_name[] = FRAMER_BLK_NAME_STR;
    int midx;
    uint32 *data = NULL;
    diag_reg_t *dr = (diag_reg_t *) user_data;
    uint32 rflags = dr->flags;
    uint32 fflags = 0;
    framer_symbol_field_modify_ud_t *ud = (framer_symbol_field_modify_ud_t *) dr->cookie;

    SHR_FUNC_INIT_VARS(unit);

    data = sal_alloc(sizeof(uint32) * sym->count, "RegData");
    SHR_NULL_CHECK(data, _SHR_E_MEMORY, "data is NULL!");
    sal_memset(data, 0, sizeof(uint32) * sym->count);

    SHR_IF_ERR_EXIT(framer_reg_raw_read(unit, sym->base_addr, sym->offset, sym->count, data));

    if (rflags & FRAMER_SYMBOL_DISPLAY_F_DBG_INFO)
    {
        FRAMER_SYMBOL_M_GET(sym->index, midx);

        if (sym->alias[0] != '\0')
        {
            regname = &sym->alias[0];
        }
        else
        {
            regname = &sym->sname[0];
        }

        cli_out("Modreg %s", regname);
        if (sym->flags & FRAMER_SYMBOL_F_ARRAY)
        {
            cli_out("[%d]", midx);
        }
        if (sym->blk_no < 0)
        {
            cli_out(".%s", sym->blk < 0 ? FRAMER_BLK_NAME_RAW : blk_name[sym->blk]);
        }
        else
        {
            cli_out(".%s%d", sym->blk < 0 ? FRAMER_BLK_NAME_RAW : blk_name[sym->blk], sym->blk_no);
        }
        for (i = 0; i < ud->count; i++)
        {
            cli_out(" %s=%s", ud->param[i].fld, ud->param[i].data);
        }
        cli_out("\n");
    }

    ud->cookie = data;
    if (rflags & FRAMER_SYMBOL_DISPLAY_F_DBG_INFO)
    {
        fflags |= FRAMER_SYMBOL_FIELD_F_DBG;
    }
    SHR_IF_ERR_EXIT(framer_symbol_field_iter(unit, fflags, sym, framer_symbol_field_modify, ud));
    SHR_IF_ERR_EXIT(framer_reg_raw_write(unit, sym->base_addr, sym->offset, sym->count, data));

exit:
    if (data)
    {
        sal_free(data);
        data = NULL;
    }
    SHR_FUNC_EXIT;
}

/*
 * Modifies a register value to the SOC.
 * Syntax: modreg [<regtype>] <symbol> <fld=0xHHHH>
 */

cmd_result_t
cmd_reg_modify(
    int unit,
    args_t * a)
{
    int i, rv = CMD_OK;
    char *name;
    uint32 flags = 0;
    uint32 dflen;
    framer_symbol_filter_t filter;
    framer_symbol_field_modify_ud_t ud = { NULL, 0, NULL };
    diag_reg_t dr;
    uint32 rflags = 0;
    char *equal = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (0 == sh_check_attached(ARG_CMD(a), unit))
    {
        SHR_IF_ERR_EXIT(CMD_FAIL);
    }

    name = ARG_GET(a);
    dflen = SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH;
    for (;;)
    {
        if (name && !sal_strncasecmp(name, "raw", sal_strnlen(name, dflen)))
        {
            flags |= FRAMER_REGSFILE_F_RAW;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "pfx", sal_strnlen(name, dflen)))
        {
            flags |= FRAMER_REGSFILE_F_PFX;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "dbginfo", sal_strnlen(name, dflen)))
        {
            flags |= FRAMER_REGSFILE_F_DBG;
            rflags |= FRAMER_SYMBOL_DISPLAY_F_DBG_INFO;
            name = ARG_GET(a);
        }
        else
        {
            break;
        }
    }
    if (name == NULL)
    {
        SHR_IF_ERR_EXIT_NO_MSG(CMD_USAGE);
    }
    framer_symbol_filter_t_init(&filter);
    filter.name = name;
    if (filter.name == NULL)
    {
        SHR_IF_ERR_EXIT_NO_MSG(CMD_USAGE);
    }
    ud.count = ARG_CNT(a);
    if (ud.count == 0)
    {
        SHR_IF_ERR_EXIT_NO_MSG(CMD_USAGE);
    }
    ud.param = sal_alloc(sizeof(*(ud.param)) * ud.count, "ud.param");
    SHR_NULL_CHECK(ud.param, CMD_FAIL, "ud.param is NULL!");

    i = 0;
    while (ARG_CNT(a) > 0)
    {
        ud.param[i].fld = ARG_GET(a);
        equal = sal_strnchr(ud.param[i].fld, '=', sal_strnlen(ud.param[i].fld, dflen));
        SHR_NULL_CHECK(equal, CMD_USAGE, "The format of ud.param[i].fld is incorrect!\n");
        *equal = '\0';
        ud.param[i].data = equal + 1;
        i++;
    }
    dr.flags = rflags;
    dr.cookie = &ud;
    filter.flags = FRAMER_FILTER_DBG | FRAMER_FILTER_CONT_ON_ERR;
    filter.cb = modreg_entry;
    filter.user_data = &dr;
    rv = framer_symbol_filter(unit, FRAMER_REGSFILE_F_REG | flags, &filter);
    if (rv < 0)
    {
        SHR_IF_ERR_EXIT_NO_MSG(CMD_FAIL);
    }

exit:
    if (ud.param)
    {
        sal_free(ud.param);
        ud.param = NULL;
    }
    SHR_FUNC_EXIT;
}
#endif /* NO_FRAMER_LIB_BUILD */
