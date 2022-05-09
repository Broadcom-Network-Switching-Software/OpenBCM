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
#include <shared/error.h>
#include <shared/alloc.h>
#include <sal/core/libc.h>
#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <soc/sand/shrextend/shrextend_debug.h>

#ifndef NO_FRAMER_LIB_BUILD
#include "diag_framer_symbols.h"
#include "diag_framer_access.h"

#define FRAMER_MEM_DWORD_MAX           128
#define FRAMER_MEM_DUMP_ENTRY_MAX      32

#define FRAMER_SYMBOL_DISPLAY_F_BRIEF           (0x1 << 0)
#define FRAMER_SYMBOL_DISPLAY_F_DATA_NO_SPACE   (0x1 << 1)
#define FRAMER_SYMBOL_DISPLAY_F_COMPACT         (0x1 << 2)
#define FRAMER_SYMBOL_DISPLAY_F_INSTANCE        (0x1 << 3)
#define FRAMER_SYMBOL_DISPLAY_F_ARRAY           (0x1 << 4)
#define FRAMER_SYMBOL_DISPLAY_F_WIDTH           (0x1 << 5)

static int dump_mem_debug = 0;
static int write_mem_debug = 0;
static int modify_mem_debug = 0;

char cmd_dump_usage[] =
    "Usages:\n\t"
    "Dump the format fields and values of memories.\n\t"
    "\n"
    "Options:\n\t"
    "DUMP [raw|chg|dbg|pfx|brief] <TABLE|*>[ARRAY][.<COPY>[BASE_ADDR][OFFSET][INDEX]] <ENTRY> <ENTRYCOUNT>\n\t"
    "raw - Display in raw format.\n\t"
    "chg - Display changed tables only.\n\t"
    "dbg - Display the command debug information.\n\t"
    "pfx - Display with prefix format.\n\t"
    "brief - Display in brief format.\n\t"
    "\n"
    "Examples:\n\t"
    "Dump * 0 1\n\t"
    "Dump *.FBSW\n\t"
    "Dump *.FBSW 0 1\n\t"
    "Dump chg *\n\t"
    "Dump chg dbg *\n\t"
    "Dump chg dbg pfx *\n\t"
    "Dump chg brief *\n\t"
    "Dump chg *.FBSW\n\t"
    "Dump chg *.FBSW 0 1\n\t"
    "Dump chg B66SW__B66SWRAM\n\t"
    "Dump B66SW__B66SWRAM\n\t"
    "Dump B66SW__B66SWRAM 0 1\n\t"
    "Dump B66SW__B66SWRAM[0].FBSW 0 2\n\t"
    "Dump B66SW__B66SWRAM[1].FBSW[0x30000][0x11][2]\n\t"
    "Dump B66SW__B66SWRAM[2].FBSW[0x30000][0x12][320]\n\t"
    "Dump pfx *\n\t"
    "Dump pfx *.FBSW\n\t"
    "Dump pfx FPB_B66_SW__B66SW__B66SWRAM[0].FBSW\n\t"
    "Dump pfx FPB_B66_SW__B66SW__B66SWRAM[0].FBSW[0x30000][0x10][2]\n\t"
    "Dump raw *\n\t" "Dump raw pfx *\n\t" "Dump raw pfx FPB_OPU25_MUX__OPU25_MUX_J__OPU25_DRD_CFG_M0\n\t" "\n";

char cmd_write_usage[] =
    "Usages:\n\t"
    "Write the memory data.\n\t"
    "\n"
    "Options:\n\t"
    "Write [raw|pfx] <TABLE>[ARRAY][.<COPY>[BASE_ADDR][OFFSET][INDEX]] <ENTRY> <ENTRYCOUNT> <DATA|0xHHHH>\n\t"
    "raw - Write in raw format.\n\t"
    "pfx - Write in prefix format.\n\t"
    "\n"
    "Examples:\n\t"
    "Write FLEXE_MUX2__WR_RULE_RAM.FMX1 0 1 0x123456789abcdeffedcba987654321\n\t"
    "Write FLEXE_MUX2__WR_RULE_RAM.FMX1 0 0 0x0\n\t"
    "Write FLEXE_MUX2__WR_RULE_RAM.FMX1[0x70000][0xb4][0] 0\n\t"
    "Write FLEXE_MUX2__WR_RULE_RAM.FMX1[0x70000][0xb4][960] 0x0\n\t"
    "Write FLEXE_MUX2__WR_RULE_RAM.FMX1[0x70000][0xb4][960] 0x123456789abcdeffedcba987654321\n\t" "\n";

char cmd_modify_usage[] =
    "Usages:\n\t"
    "MODify the memory field values.\n\t"
    "\n"
    "Options:\n\t"
    "MODify [raw|pfx] <TABLE>[ARRAY][.<COPY>[BASE_ADDR][OFFSET][INDEX]] <ENTRY> <ENTRYCOUNT> <FIELD>=<VALUE|0xHHHH>[ ...]\n\t"
    "raw - Write in raw format.\n\t"
    "pfx - Write in prefix format.\n\t"
    "\n"
    "Examples:\n\t"
    "MODify OPU25_MUX_J__OPU25_DRD_CFG 0 0 OPU25_DRD_CFG0=2\n\t"
    "MODify OPU25_MUX_J__OPU25_DRD_CFG 0 1 OPU25_DRD_CFG0=3\n\t"
    "MODify OPU25_MUX_J__OPU25_DRD_CFG[0] 0 1 OPU25_DRD_CFG0=4\n\t"
    "MODify OPU25_MUX_J__OPU25_DRD_CFG[0].FMXT[0x68200][0x58][0] OPU25_DRD_CFG0=5\n\t"
    "MODify GFP_MAP__MAP_GFP_TX_BASE_CFG.FGMAP 0 1 CORE_HEADER_SCRAM_EN=1 PFI_CFG=1 FDI_SEND_EN=1\n\t"
    "MODify GFP_MAP__MAP_GFP_TX_BASE_CFG.FGMAP[0x90000][0x0][0] CORE_HEADER_SCRAM_EN=1 PFI_CFG=1 FDI_SEND_EN=1\n\t"
    "\n";

char cmd_list_usage[] =
    "Usages:\n\t"
    "LIST the registers and memories.\n\t"
    "\n"
    "Options:\n\t"
    "LIST [reg|mem|ctr|alm|raw|pfx|dbg|comp|inst|array|width] <TABLE|*>[ARRAY][.<COPY>[BASE_ADDR][OFFSET][INDEX]]\n\t"
    "reg - LIST registers.\n\t"
    "mem - LIST memories.\n\t"
    "ctr - LIST counter registers.\n\t"
    "alm - LIST alarm registers.\n\t"
    "raw - LIST regisers/memories in raw format.\n\t"
    "pfx - LIST regisers/memories in prefix format.\n\t"
    "dbg - LIST regisers/memories in dbg format.\n\t"
    "comp - LIST regisers/memories in compact mode.\n\t"
    "inst - LIST regisers/memories with FRAMER_SYMBOL_F_INSTANCE flags.\n\t"
    "array - LIST regisers/memories with FRAMER_SYMBOL_F_ARRAY flags.\n\t"
    "width - LIST regisers/memories with FRAMER_SYMBOL_F_WIDTH flags.\n\t"
    "\n"
    "Examples:\n\t"
    "LIST *\n\t"
    "LIST *.FBSW\n\t"
    "LIST *.*[0xa00][0x12]\n\t"
    "LIST reg *\n\t"
    "LIST reg inst *\n\t"
    "LIST mem *.FBSW\n\t"
    "LIST ctr *.FBSW\n\t"
    "LIST alm *.FBSW\n\t"
    "LIST mem CALENA\n\t"
    "LIST comp *.FICPB\n\t"
    "LIST inst *.FBSW\n\t"
    "LIST array comp *.FICPB\n\t"
    "LIST width comp *.FICPB\n\t"
    "LIST B66SW\n\t"
    "LIST B66SW__B66SWRAM\n\t"
    "LIST B66SW__B66SWRAM[0].FBSW[0x30000][0x10][320]\n\t" "LIST dbg B66SW__B66SWRAM[0]\n\t" "\n";

typedef struct diag_mem_s
{
    int start_index;
    int count;
    void *cookie;
} diag_mem_t;

typedef struct diag_list_s
{
    int flags;
    int data_tmp;
    void *cookie;
} diag_list_t;

static int
dump_mem_print_entry(
    int unit,
    uint32 flags,
    framer_symbol_t * sym,
    void *user_data)
{
    int i, k, dw, start, count;
    uint32 width = 0, depth = 0;
    uint32 *data = NULL;
    diag_mem_t *dm = (diag_mem_t *) user_data;
    uint32 gflags = *(uint32 *) dm->cookie;
    const char *blk_name[] = FRAMER_BLK_NAME_STR;
    int midx;
    char *memname = NULL;
    framer_symbol_field_print_ud_t ud;

    SHR_FUNC_INIT_VARS(unit);

    start = dm->start_index;
    count = dm->count;

    depth = sym->depth;
    width = sym->width;
    dw = (width + 32 - 1) / 32;
    if (start >= depth)
    {
        start = 0;
        count = depth;
    }
    if (count == 0)
    {
        count = depth - start;
    }
    if (count > (depth - start))
    {
        count = depth - start;
    }
    if (dump_mem_debug)
    {
        cli_out("depth = %d, width = %d, dw = %d, start = %d, count = %d\n", depth, width, dw, start, count);
    }
    data = sal_alloc(sizeof(uint32) * dw, "MemData");
    SHR_NULL_CHECK(data, _SHR_E_MEMORY, "data is NULL!");

    sal_memset(data, 0, sizeof(uint32) * dw);

    for (k = start; k < start + count; k++)
    {
        SHR_IF_ERR_EXIT(framer_mem_raw_read(unit, sym->base_addr, sym->offset, k, width, data));
        if (flags & FRAMER_REGSFILE_F_CHG)
        {
            for (i = 0; i < dw; i++)
            {
                if (data[i] != 0)
                {
                    break;
                }
            }
            if (i >= dw)
            {
                continue;
            }
        }

        FRAMER_SYMBOL_M_GET(sym->index, midx);

        if (sym->alias[0] != '\0')
        {
            memname = &sym->alias[0];
        }
        else
        {
            memname = &sym->sname[0];
        }
        cli_out("%s", memname);
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
        cli_out("[0x%x][0x%x][%d] = 0x", sym->base_addr, sym->offset, k);
        for (i = dw - 1; i >= 0; i--)
        {
            cli_out("%08x", data[i]);
            if ((gflags & FRAMER_SYMBOL_DISPLAY_F_DATA_NO_SPACE) == 0)
            {
                cli_out(" ");
            }
        }
        cli_out("\n");
        if (count == 1 && (gflags & FRAMER_SYMBOL_DISPLAY_F_BRIEF) == 0)
        {
            ud.pfx = "\t";
            ud.sfx = NULL;
            ud.dw = dw;
            ud.data = data;
            SHR_IF_ERR_EXIT(framer_symbol_field_iter(unit, 0, sym, framer_symbol_field_print, &ud));
        }
    }

exit:
    if (data)
    {
        sal_free(data);
        data = NULL;
    }
    SHR_FUNC_EXIT;
}

cmd_result_t
cmd_dump(
    int unit,
    args_t * a)
{
    int rv = _SHR_E_NONE;
    char *name, *arg2, *arg3;
    uint32 flags = 0, gflags = 0;
    uint32 dflen;
    diag_mem_t dm;
    framer_symbol_filter_t filter;
    framer_symbol_parser_t sym_parse;

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
        else if (name && !sal_strncasecmp(name, "dbg", sal_strnlen(name, dflen)))
        {
            flags |= FRAMER_REGSFILE_F_DBG;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "pfx", sal_strnlen(name, dflen)))
        {
            flags |= FRAMER_REGSFILE_F_PFX;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "brief", sal_strnlen(name, dflen)))
        {
            gflags |= FRAMER_SYMBOL_DISPLAY_F_BRIEF;
            gflags |= FRAMER_SYMBOL_DISPLAY_F_DATA_NO_SPACE;
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
    SHR_IF_ERR_EXIT(framer_symbol_name2sym_parser(unit, flags, name, &sym_parse));
    framer_symbol_filter_t_init(&filter);
    filter.name = name;
    if (sym_parse.flags & FRAMER_SYMBOL_PARSER_F_INDEX)
    {
        dm.start_index = sym_parse.index;
        dm.count = 1;
    }
    else
    {
        arg2 = ARG_GET(a);
        arg3 = ARG_GET(a);
        if ((arg2) && (isint(arg2)))
        {
            dm.start_index = parse_integer(arg2);
            if ((arg3) && (isint(arg3)))
            {
                dm.count = parse_integer(arg3);
            }
            else
            {
                dm.count = 1;
            }
        }
        else
        {
            dm.start_index = 0;
            dm.count = FRAMER_MEM_DUMP_ENTRY_MAX;
        }
    }
    dm.cookie = &gflags;
    filter.flags = FRAMER_FILTER_DBG | FRAMER_FILTER_CONT_ON_ERR;
    filter.cb = dump_mem_print_entry;
    filter.user_data = &dm;
    rv = framer_symbol_filter(unit, FRAMER_REGSFILE_F_MEM | flags, &filter);
    if (rv < 0)
    {
        cli_out("No memory found with the substring '%s' in its name.\n", name);
        SHR_IF_ERR_EXIT(CMD_FAIL);
    }

exit:
    SHR_FUNC_EXIT;
}

static int
write_mem_entry(
    int unit,
    uint32 flags,
    framer_symbol_t * sym,
    void *user_data)
{
    int k, dw, start, count;
    uint32 width = 0, depth = 0;
    uint32 *data = NULL;
    diag_mem_t *dm = (diag_mem_t *) user_data;
    const char *blk_name[] = FRAMER_BLK_NAME_STR;
    int midx;
    char *memname = NULL;
    char *data_str = (char *) dm->cookie;

    SHR_FUNC_INIT_VARS(unit);

    start = dm->start_index;
    count = dm->count;
    if (write_mem_debug)
    {
        FRAMER_SYMBOL_M_GET(sym->index, midx);

        if (sym->alias[0] != '\0')
        {
            memname = &sym->alias[0];
        }
        else
        {
            memname = &sym->sname[0];
        }
        cli_out("Write %s", memname);
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
        cli_out(" %d %d %s\n", start, count, data_str);
    }

    depth = sym->depth;
    width = sym->width;
    dw = (width + 32 - 1) / 32;
    if (start >= depth)
    {
        start = 0;
        count = depth;
    }
    if (count == 0)
    {
        count = depth - start;
    }
    if (count > (depth - start))
    {
        count = depth - start;
        cli_out("WARNING: 'count' can't exceed %d\n", (depth - start));
    }
    if (write_mem_debug)
    {
        cli_out("depth = %d, width = %d, dw = %d, start = %d, count = %d\n", depth, width, dw, start, count);
    }
    data = sal_alloc(sizeof(uint32) * dw, "MemData");
    SHR_NULL_CHECK(data, _SHR_E_MEMORY, "data is NULL!");
    sal_memset(data, 0, sizeof(uint32) * dw);
    SHR_IF_ERR_EXIT(framer_symbol_str2data(unit, data_str, dw, data));

    for (k = start; k < start + count; k++)
    {
        SHR_IF_ERR_EXIT(framer_mem_raw_write(unit, sym->base_addr, sym->offset, k, width, data));
    }

exit:
    if (data)
    {
        sal_free(data);
        data = NULL;
    }
    SHR_FUNC_EXIT;
}

cmd_result_t
cmd_write(
    int unit,
    args_t * a)
{
    int rv = _SHR_E_NONE;
    char *name, *arg2, *arg3;
    uint32 flags = 0;
    uint32 dflen;
    diag_mem_t dm;
    framer_symbol_filter_t filter;
    framer_symbol_parser_t sym_parse;

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
        else
        {
            break;
        }
    }
    if (name == NULL)
    {
        SHR_IF_ERR_EXIT_NO_MSG(CMD_USAGE);
    }
    SHR_IF_ERR_EXIT(framer_symbol_name2sym_parser(unit, flags, name, &sym_parse));
    framer_symbol_filter_t_init(&filter);
    filter.name = name;
    if (sym_parse.flags & FRAMER_SYMBOL_PARSER_F_INDEX)
    {
        dm.start_index = sym_parse.index;
        dm.count = 1;
    }
    else
    {
        arg2 = ARG_GET(a);
        arg3 = ARG_GET(a);
        if ((arg2) && (isint(arg2)))
        {
            dm.start_index = parse_integer(arg2);
            if ((arg3) && (isint(arg3)))
            {
                dm.count = parse_integer(arg3);
            }
            else
            {
                dm.count = 1;
            }
        }
        else
        {
            dm.start_index = 0;
            dm.count = 0;
        }
    }
    if (ARG_CNT(a) != 1)
    {
        SHR_IF_ERR_EXIT_NO_MSG(CMD_USAGE);
    }
    dm.cookie = ARG_GET(a);
    filter.flags = FRAMER_FILTER_DBG | FRAMER_FILTER_CONT_ON_ERR;
    filter.cb = write_mem_entry;
    filter.user_data = &dm;
    rv = framer_symbol_filter(unit, FRAMER_REGSFILE_F_MEM | flags, &filter);
    if (rv < 0)
    {
        cli_out("No memory found with the substring '%s' in its name.\n", name);
        SHR_IF_ERR_EXIT(CMD_FAIL);
    }

exit:
    SHR_FUNC_EXIT;
}

static int
modify_mem_entry(
    int unit,
    uint32 flags,
    framer_symbol_t * sym,
    void *user_data)
{
    int i, k, dw, start, count;
    uint32 width = 0, depth = 0;
    uint32 *data = NULL;
    diag_mem_t *dm = (diag_mem_t *) user_data;
    const char *blk_name[] = FRAMER_BLK_NAME_STR;
    int midx;
    char *memname = NULL;
    framer_symbol_field_modify_ud_t *ud = (framer_symbol_field_modify_ud_t *) dm->cookie;

    SHR_FUNC_INIT_VARS(unit);

    start = dm->start_index;
    count = dm->count;
    if (modify_mem_debug)
    {
        FRAMER_SYMBOL_M_GET(sym->index, midx);

        if (sym->alias[0] != '\0')
        {
            memname = &sym->alias[0];
        }
        else
        {
            memname = &sym->sname[0];
        }
        cli_out("MODify %s", memname);
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
        cli_out(" %d %d", start, count);
        for (i = 0; i < ud->count; i++)
        {
            cli_out(" %s=%s", ud->param[i].fld, ud->param[i].data);
        }
        cli_out("\n");
    }

    depth = sym->depth;
    width = sym->width;
    dw = (width + 32 - 1) / 32;
    if (start >= depth)
    {
        start = 0;
        count = depth;
    }
    if (count == 0)
    {
        count = depth - start;
    }
    if (count > (depth - start))
    {
        count = depth - start;
        cli_out("WARNING: 'count' can't exceed %d\n", (depth - start));
    }
    if (modify_mem_debug)
    {
        cli_out("depth = %d, width = %d, dw = %d, start = %d, count = %d\n", depth, width, dw, start, count);
    }
    data = sal_alloc(sizeof(uint32) * dw, "MemData");
    SHR_NULL_CHECK(data, _SHR_E_MEMORY, "data is NULL!");
    sal_memset(data, 0, sizeof(uint32) * dw);

    for (k = start; k < start + count; k++)
    {
        SHR_IF_ERR_EXIT(framer_mem_raw_read(unit, sym->base_addr, sym->offset, k, width, data));
        if (count)
        {
            ud->cookie = data;
            SHR_IF_ERR_EXIT(framer_symbol_field_iter(unit, 0, sym, framer_symbol_field_modify, ud));
        }
        SHR_IF_ERR_EXIT(framer_mem_raw_write(unit, sym->base_addr, sym->offset, k, width, data));
    }
exit:
    if (data)
    {
        sal_free(data);
        data = NULL;
    }
    SHR_FUNC_EXIT;
}

cmd_result_t
cmd_modify(
    int unit,
    args_t * a)
{
    int i, rv = _SHR_E_NONE;
    char *name, *arg2, *arg3;
    uint32 flags = 0;
    uint32 dflen;
    diag_mem_t dm;
    framer_symbol_field_modify_ud_t ud = { NULL, 0, NULL };
    char *equal = NULL;
    framer_symbol_filter_t filter;
    framer_symbol_parser_t sym_parse;

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
        else
        {
            break;
        }
    }
    if (name == NULL)
    {
        SHR_IF_ERR_EXIT_NO_MSG(CMD_USAGE);
    }
    SHR_IF_ERR_EXIT(framer_symbol_name2sym_parser(unit, flags, name, &sym_parse));
    framer_symbol_filter_t_init(&filter);
    filter.name = name;
    if (sym_parse.flags & FRAMER_SYMBOL_PARSER_F_INDEX)
    {
        dm.start_index = sym_parse.index;
        dm.count = 1;
    }
    else
    {
        arg2 = ARG_GET(a);
        arg3 = ARG_GET(a);
        if ((arg2) && (isint(arg2)))
        {
            dm.start_index = parse_integer(arg2);
            if ((arg3) && (isint(arg3)))
            {
                dm.count = parse_integer(arg3);
            }
            else
            {
                dm.count = 1;
            }
        }
        else
        {
            dm.start_index = 0;
            dm.count = 0;
        }
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
    dm.cookie = &ud;
    filter.flags = FRAMER_FILTER_DBG | FRAMER_FILTER_CONT_ON_ERR;
    filter.cb = modify_mem_entry;
    filter.user_data = &dm;
    rv = framer_symbol_filter(unit, FRAMER_REGSFILE_F_MEM | flags, &filter);
    if (rv < 0)
    {
        cli_out("No memory found with the substring '%s' in its name.\n", name);
        SHR_IF_ERR_EXIT(CMD_FAIL);
    }

exit:
    if (ud.param)
    {
        sal_free(ud.param);
        ud.param = NULL;
    }
    SHR_FUNC_EXIT;
}

static int
list_entry(
    int unit,
    uint32 flags,
    framer_symbol_t * sym,
    void *user_data)
{
    char *symname = NULL;
    int midx = 0, mlast = 0;
    int w, dw = 0;
    uint32 width = 0, depth = 0;
    const char *blk_name[] = FRAMER_BLK_NAME_STR;
    REG_INFO_S **sym_info = sym->sinfo;
    diag_list_t *dl = (diag_list_t *) user_data;
    framer_symbol_field_print_ud_t ud;
    uint32 *data = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (sym->alias[0] != '\0')
    {
        symname = &sym->alias[0];
    }
    else
    {
        symname = &sym->sname[0];
    }
    if (dl->flags & FRAMER_SYMBOL_DISPLAY_F_INSTANCE)
    {
        if ((sym->flags & FRAMER_SYMBOL_F_INSTANCE) == 0)
        {
            SHR_IF_ERR_EXIT(_SHR_E_NOT_FOUND);
        }
    }
    if (dl->flags & FRAMER_SYMBOL_DISPLAY_F_ARRAY)
    {
        if ((sym->flags & FRAMER_SYMBOL_F_ARRAY) == 0)
        {
            SHR_IF_ERR_EXIT(_SHR_E_NOT_FOUND);
        }
    }
    if (dl->flags & FRAMER_SYMBOL_DISPLAY_F_WIDTH)
    {
        if ((sym->flags & FRAMER_SYMBOL_F_WIDTH) == 0)
        {
            SHR_IF_ERR_EXIT(_SHR_E_NOT_FOUND);
        }
    }
    if (sym->flags & FRAMER_SYMBOL_F_ARRAY)
    {
        FRAMER_SYMBOL_M_GET(sym->index, midx);
        FRAMER_SYMBOL_M_GET(sym->last, mlast);
        if (dl->flags & FRAMER_SYMBOL_DISPLAY_F_COMPACT)
        {
            if (midx == 0)
            {
                dl->data_tmp = sym->offset;
            }
            if (mlast == 0)
            {
                SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
            }
        }
    }
    cli_out("\t%s", symname);
    if (sym->flags & FRAMER_SYMBOL_F_ARRAY)
    {
        if (dl->flags & FRAMER_SYMBOL_DISPLAY_F_COMPACT)
        {
            cli_out("[]");
        }
        else
        {
            cli_out("[%d]", midx);
        }
    }
    if (sym->blk_no < 0)
    {
        cli_out(".%s", sym->blk < 0 ? FRAMER_BLK_NAME_RAW : blk_name[sym->blk]);
    }
    else
    {
        cli_out(".%s%d", sym->blk < 0 ? FRAMER_BLK_NAME_RAW : blk_name[sym->blk], sym->blk_no);
    }
    if (sym->flags & FRAMER_SYMBOL_F_ARRAY)
    {
        if (dl->flags & FRAMER_SYMBOL_DISPLAY_F_COMPACT)
        {
            cli_out("[0x%x][0x%x-0x%x]", sym->base_addr, dl->data_tmp, sym->offset);
        }
        else
        {
            cli_out("[0x%x][0x%x]", sym->base_addr, sym->offset);
        }
    }
    else
    {
        cli_out("[0x%x][0x%x]", sym->base_addr, sym->offset);
    }
    if (sym->flags & FRAMER_SYMBOL_F_REG)
    {
        dw = sym->count;
    }
    if (sym->flags & FRAMER_SYMBOL_F_MEM)
    {
        depth = sym->depth;
        width = sym->width;
        dw = (width + 32 - 1) / 32;
    }
    if (sym->flags & FRAMER_SYMBOL_F_MEM)
    {
        cli_out("[%d]", depth);
    }
    cli_out("\n");
    if (flags & FRAMER_REGSFILE_F_EM)
    {
        data = sal_alloc(sizeof(uint32) * dw, "DefaultData");
        SHR_NULL_CHECK(data, _SHR_E_MEMORY, "data is NULL!");
        sal_memset(data, 0, sizeof(uint32) * dw);
        if (sym->flags & FRAMER_SYMBOL_F_REG)
        {
            for (w = 0; w < dw; w++)
            {
                data[w] = sym_info[w]->default_value;
            }
        }
        ud.pfx = "\t\t";
        ud.sfx = sym->flags & FRAMER_SYMBOL_F_RO ? "  (RO)" : sym->flags & FRAMER_SYMBOL_F_WO ? "  (WO)" : "  (RW)";
        ud.dw = dw;
        ud.data = data;
        SHR_IF_ERR_EXIT(framer_symbol_field_iter(unit, 0, sym, framer_symbol_field_print, &ud));
    }

exit:
    if (data)
    {
        sal_free(data);
        data = NULL;
    }
    SHR_FUNC_EXIT;
}

cmd_result_t
cmd_list(
    int unit,
    args_t * a)
{
    int rv = _SHR_E_NONE;
    char *name;
    uint32 flags = 0;
    uint32 dflen;
    framer_symbol_filter_t filter;
    int not_found = 0, found = 0;
    diag_list_t dl;

    SHR_FUNC_INIT_VARS(unit);

    if (0 == sh_check_attached(ARG_CMD(a), unit))
    {
        SHR_IF_ERR_EXIT(CMD_FAIL);
    }
    sal_memset(&dl, 0, sizeof(dl));
    name = ARG_GET(a);
    dflen = SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH;
    flags = FRAMER_REGSFILE_F_EXT;
    for (;;)
    {
        if (name && !sal_strncasecmp(name, "raw", sal_strnlen(name, dflen)))
        {
            flags |= FRAMER_REGSFILE_F_RAW;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "reg", sal_strnlen(name, dflen)))
        {
            flags |= FRAMER_REGSFILE_F_REG;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "mem", sal_strnlen(name, dflen)))
        {
            flags |= FRAMER_REGSFILE_F_MEM;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "ctr", sal_strnlen(name, dflen)))
        {
            flags |= FRAMER_REGSFILE_F_REG | FRAMER_REGSFILE_F_CTR;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "alm", sal_strnlen(name, dflen)))
        {
            flags |= FRAMER_REGSFILE_F_REG | FRAMER_REGSFILE_F_ALM;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "pfx", sal_strnlen(name, dflen)))
        {
            flags |= FRAMER_REGSFILE_F_PFX;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "dbg", sal_strnlen(name, dflen)))
        {
            flags |= FRAMER_REGSFILE_F_DBG;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "comp", sal_strnlen(name, dflen)))
        {
            dl.flags |= FRAMER_SYMBOL_DISPLAY_F_COMPACT;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "inst", sal_strnlen(name, dflen)))
        {
            dl.flags |= FRAMER_SYMBOL_DISPLAY_F_INSTANCE;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "array", sal_strnlen(name, dflen)))
        {
            dl.flags |= FRAMER_SYMBOL_DISPLAY_F_ARRAY;
            name = ARG_GET(a);
        }
        else if (name && !sal_strncasecmp(name, "width", sal_strnlen(name, dflen)))
        {
            dl.flags |= FRAMER_SYMBOL_DISPLAY_F_WIDTH;
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
    if ((flags & (FRAMER_REGSFILE_F_REG | FRAMER_REGSFILE_F_MEM)) == 0 || (flags & FRAMER_REGSFILE_F_REG))
    {
        cli_out("Registers:\n");
        found++;
        framer_symbol_filter_t_init(&filter);
        filter.name = name;
        filter.flags = FRAMER_FILTER_DBG | FRAMER_FILTER_CONT_ON_ERR;
        filter.cb = list_entry;
        filter.user_data = &dl;
        rv = framer_symbol_filter(unit, FRAMER_REGSFILE_F_REG | flags, &filter);
        if (rv < 0)
        {
            not_found++;
        }
    }
    if ((flags & (FRAMER_REGSFILE_F_REG | FRAMER_REGSFILE_F_MEM)) == 0 || (flags & FRAMER_REGSFILE_F_MEM))
    {
        cli_out("Memories:\n");
        found++;
        framer_symbol_filter_t_init(&filter);
        filter.name = name;
        filter.flags = FRAMER_FILTER_DBG | FRAMER_FILTER_CONT_ON_ERR;
        filter.cb = list_entry;
        filter.user_data = &dl;
        rv = framer_symbol_filter(unit, FRAMER_REGSFILE_F_MEM | flags, &filter);
        if (rv < 0)
        {
            not_found++;
        }
    }
    if (not_found == found)
    {
        cli_out("No symbol found with the substring '%s' in its name.\n", name);
        SHR_IF_ERR_EXIT(CMD_FAIL);
    }
exit:
    SHR_FUNC_EXIT;
}
#endif /* NO_FRAMER_LIB_BUILD */
