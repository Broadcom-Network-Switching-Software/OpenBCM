/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * socdiag framer symbols
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_SYMTAB

#include <shared/bsl.h>
#include <shared/error.h>
#include <appl/diag/system.h>
#include <soc/sand/shrextend/shrextend_debug.h>

#ifndef NO_FRAMER_LIB_BUILD
#include "diag_framer_symbols.h"
#include "diag_framer_access.h"
#include "global_define.h"
#include "framer_sal_j2x_info.h"

#define FRAMER_SYMBOL_INST_IDENTIFY_BY_ADDR  0

#define FRAMER_SYMBOL_FORMAT_MAX      (2)
#define FRAMER_SYMBOL_SURFIX_MAX      (64)
static REG_INFO_S *sym_pool[FRAMER_SYMBOL_DWORD_MAX] = { NULL };
static int sym_count;
static int sym_debug = 0;

typedef struct framer_blk_map_s
{
    framer_blk_t blk;
    int blk_no;
    int blk_num;
    int schan_num;
} framer_blk_map_t;

static framer_blk_map_t framer_blk_map[] = {
    {FRAMER_BLK_FLFR, 0, 414, 414},     /* 0 */
    {FRAMER_BLK_FLFR, 1, 415, 415},     /* 1 */
    {FRAMER_BLK_FLFR, 2, 416, 416},     /* 2 */
    {FRAMER_BLK_FLFR, 3, 417, 417},     /* 3 */
    {FRAMER_BLK_FLFT, 0, 418, 418},     /* 4 */
    {FRAMER_BLK_FLFT, 1, 419, 419},     /* 5 */
    {FRAMER_BLK_FFOA, -1, 405, 405},    /* 6 */
    {FRAMER_BLK_FFOB, -1, 406, 406},    /* 7 */
    {FRAMER_BLK_FDMX, 0, 408, 408},     /* 8 */
    {FRAMER_BLK_FDMX, 1, 409, 409},     /* 9 */
    {FRAMER_BLK_FAWR, -1, 407, 407},    /* 10 */
    {FRAMER_BLK_FAWT, -1, 437, 437},    /* 11 */
    {FRAMER_BLK_FBSW, -1, 400, 400},    /* 12 */
    {FRAMER_BLK_FOSW, -1, 428, 428},    /* 13 */
    {FRAMER_BLK_FPMR, -1, 433, 433},    /* 14 */
    {FRAMER_BLK_FPMT, -1, 434, 434},    /* 15 */
    {FRAMER_BLK_FLOTN, -1, 420, 420},   /* 16 */
    {FRAMER_BLK_FFLXO, -1, 440, 440},   /* 17 */
    {FRAMER_BLK_FDMXC, -1, 431, 431},   /* 18 */
    {FRAMER_BLK_FDMXBI, -1, 450, 450},  /* 19 */
    {FRAMER_BLK_FDMXA, -1, 429, 429},   /* 20 */
    {FRAMER_BLK_FDMXT, -1, 439, 439},   /* 21 */
    {FRAMER_BLK_FMXC, -1, 432, 432},    /* 22 */
    {FRAMER_BLK_FMXBI, -1, 445, 445},   /* 23 */
    {FRAMER_BLK_FMXBJ, -1, 446, 446},   /* 24 */
    {FRAMER_BLK_FMXAI, -1, 430, 430},   /* 25 */
    {FRAMER_BLK_FMXT, -1, 447, 447},    /* 26 */
    {FRAMER_BLK_FMX, 0, 443, 443},      /* 27 */
    {FRAMER_BLK_FMX, 1, 444, 444},      /* 28 */
    {FRAMER_BLK_FRA, 0, 448, 448},      /* 29 */
    {FRAMER_BLK_FRA, 1, 449, 449},      /* 30 */
    {FRAMER_BLK_FMAP, -1, 423, 423},    /* 31 */
    {FRAMER_BLK_FDMP, -1, 436, 436},    /* 32 */
    {FRAMER_BLK_FOAM, 0, 424, 424},     /* 33 */
    {FRAMER_BLK_FOAM, 1, 425, 425},     /* 34 */
    {FRAMER_BLK_FSAR, -1, 435, 435},    /* 35 */
    {FRAMER_BLK_FGMAP, -1, 422, 422},   /* 36 */
    {FRAMER_BLK_FMACR, -1, 441, 441},   /* 37 */
    {FRAMER_BLK_FGDMP, -1, 421, 421},   /* 38 */
    {FRAMER_BLK_FMACT, -1, 442, 442},   /* 39 */
    {FRAMER_BLK_FECRL, -1, 403, 403},   /* 40 */
    {FRAMER_BLK_FDCRL, -1, 401, 401},   /* 41 */
    {FRAMER_BLK_FECRS, -1, 404, 404},   /* 42 */
    {FRAMER_BLK_FDCRS, -1, 402, 402},   /* 43 */
    {FRAMER_BLK_FPTPR, -1, 412, 412},   /* 44 */
    {FRAMER_BLK_FPTPT, -1, 413, 413},   /* 45 */
    {FRAMER_BLK_FODUO, 0, 426, 426},    /* 46 */
    {FRAMER_BLK_FODUO, 1, 427, 427},    /* 47 */
    {FRAMER_BLK_FMXAJ, -1, 451, 451},   /* 48 */
    {FRAMER_BLK_FMXBK, -1, 452, 452},   /* 49 */
    {FRAMER_BLK_FDMXBJ, -1, 438, 438},  /* 50 */
    {FRAMER_BLK_FECPB, -1, 453, 453},   /* 51 */
    {FRAMER_BLK_FICPB, -1, 454, 454},   /* 52 */
};

static int framer_blk_cnt = COUNTOF(framer_blk_map);

#define FRAMER_BLK_ID_INVALID(_blk_id) \
        ((_blk_id) < 0 || (_blk_id) >= framer_blk_cnt)

static int framer_symbol_debug = 0;

typedef struct framer_symbol_module_s
{
    FRAMER_MODULE_INST_S *mi;
    uint32 flags;
    uint32 mlen;
    uint32 prefix_len;
    uint32 prefix_len_removed;
    framer_symbol_inst_t inst;
} framer_symbol_module_t;

static void
framer_symbol_print(
    int unit,
    uint32 flags,
    framer_symbol_t * sym)
{
    int blk_id;
    const char *blk_name[] = FRAMER_BLK_NAME_STR;
    int midx, widx, mlast, wlast;
    int i;
    uint32 width, depth;
    REG_INFO_S **sym_info = sym->sinfo;

    SHR_FUNC_INIT_VARS(unit);

    cli_out("Symbol:\n");
    cli_out("%d: {%s %s %s %s %s %s %s %s}\n",
            unit, flags & FRAMER_REGSFILE_F_REG ? "REG" : "",
            flags & FRAMER_REGSFILE_F_MEM ? "MEM" : "",
            flags & FRAMER_REGSFILE_F_FMT ? "FMT" : "",
            flags & FRAMER_REGSFILE_F_RAW ? "RAW" : "",
            flags & FRAMER_REGSFILE_F_CTR ? "CTR" : "",
            flags & FRAMER_REGSFILE_F_ALM ? "ALM" : "",
            flags & FRAMER_REGSFILE_F_DBG ? "DBG" : "", flags & FRAMER_REGSFILE_F_PFX ? "PFX" : "");
    cli_out("     %s(%d) {%s %s %s %s %s %s %s %s %s %s %s %s}\n",
            sym->sname, sym->sname_len,
            sym->flags & FRAMER_SYMBOL_F_REG ? "REG" : "",
            sym->flags & FRAMER_SYMBOL_F_MEM ? "MEM" : "",
            sym->flags & FRAMER_SYMBOL_F_ARRAY ? "ARRAY" : "",
            sym->flags & FRAMER_SYMBOL_F_WIDTH ? "WIDTH" : "",
            sym->flags & FRAMER_SYMBOL_F_INSTANCE ? "INSTANCE" : "",
            sym->flags & FRAMER_SYMBOL_F_INIT_ON_READ ? "INIT_ON_READ" : "",
            sym->flags & FRAMER_SYMBOL_F_INTR ? "INTR" : "",
            sym->flags & FRAMER_SYMBOL_F_RO ? "RO" : "",
            sym->flags & FRAMER_SYMBOL_F_WO ? "WO" : "",
            sym->flags & FRAMER_SYMBOL_F_COUNTER ? "COUNTER" : "",
            sym->flags & FRAMER_SYMBOL_F_RST ? "RST" : "", sym->flags & FRAMER_SYMBOL_F_ALARM ? "ALARM" : "");
    if (sym->alias[0] != '\0')
    {
        cli_out("     Alias: %s\n", sym->alias);
    }
    if ((flags & FRAMER_REGSFILE_F_RAW) == 0)
    {
        cli_out("     base_addr = 0x%08x, offset = 0x%04x (count = %d)\n", sym->base_addr, sym->offset, sym->count);
        for (i = 0; i < sym->count; i++)
        {
            blk_id = FRAMER_BLK_ID_GET(sym->base_addr + sym_info[i]->offset_addr);
            if (sym->blk_no < 0)
            {
                cli_out("          %d: offset = 0x%04x, blk_id = %d (%s)",
                        i, sym_info[i]->offset_addr, blk_id,
                        FRAMER_BLK_ID_INVALID(blk_id) ? "UNKNOWN" : blk_name[sym->blk]);
            }
            else
            {
                cli_out("          %d: offset = 0x%04x, blk_id = %d (%s%d)",
                        i, sym_info[i]->offset_addr, blk_id,
                        FRAMER_BLK_ID_INVALID(blk_id) ? "UNKNOWN" : blk_name[sym->blk], sym->blk_no);
            }
            if (sym->flags & FRAMER_SYMBOL_F_MEM)
            {
                depth = sym_info[i]->ext & MASK_20_BIT;
                width = ((sym_info[i]->ext >> SHIFT_BIT_20) & MASK_10_BIT) + 1;
                cli_out(" {Deep:%d(%d), Width:%db(%db)}", depth, sym->depth, width, sym->width);
            }
            cli_out("\n");
        }
        FRAMER_SYMBOL_M_GET(sym->index, midx);
        FRAMER_SYMBOL_W_GET(sym->index, widx);
        FRAMER_SYMBOL_M_GET(sym->last, mlast);
        FRAMER_SYMBOL_W_GET(sym->last, wlast);
        cli_out("     midx = %3d, widx = %3d, mlast = %3d, wlast = %3d\n", midx, widx, mlast, wlast);
        if (sym->flags & FRAMER_SYMBOL_F_INSTANCE)
        {
            cli_out("     instance: <%d/%d>(%d blocks, each %d)\n",
                    sym->inst->inst, sym->inst->inst_max + 1,
                    (sym->inst->inst_max + 1) / sym->inst->inst_per_blk, sym->inst->inst_per_blk);
        }
    }
SHR_VOID_FUNC_EXIT}

static void
framer_symbol_parser_print(
    int unit,
    framer_symbol_parser_t * sym_parser)
{
    const char *blk_name[] = FRAMER_BLK_NAME_STR;
    SHR_FUNC_INIT_VARS(unit);

    cli_out("Parser:\n");
    cli_out("%d: %s {%s %s %s %s %s %s}\n",
            unit, sym_parser->sname,
            sym_parser->flags & FRAMER_SYMBOL_PARSER_F_BLOCK ? "BLK" : "",
            sym_parser->flags & FRAMER_SYMBOL_PARSER_F_ARRAY ? "ARRAY" : "",
            sym_parser->flags & FRAMER_SYMBOL_PARSER_F_BASE_ADDR ? "BASE_ADDR" : "",
            sym_parser->flags & FRAMER_SYMBOL_PARSER_F_OFFSET ? "OFFSET" : "",
            sym_parser->flags & FRAMER_SYMBOL_PARSER_F_INDEX ? "INDEX" : "",
            sym_parser->flags & FRAMER_SYMBOL_PARSER_F_EXACT_MATCH ? "EXACT_MATCH" : "");
    if (sym_parser->flags & FRAMER_SYMBOL_PARSER_F_ARRAY)
    {
        cli_out("     aidx = %d, aidx_max = %d\n", sym_parser->aidx, sym_parser->aidx_max);
    }
    if (sym_parser->flags & FRAMER_SYMBOL_PARSER_F_BLOCK)
    {
        if (sym_parser->blk_no < 0)
        {
            cli_out("     blk = %d, blk_no = %d (%s)\n",
                    sym_parser->blk, sym_parser->blk_no, blk_name[sym_parser->blk]);
        }
        else
        {
            cli_out("     blk = %d, blk_no = %d (%s%d)\n",
                    sym_parser->blk, sym_parser->blk_no, blk_name[sym_parser->blk], sym_parser->blk_no);
        }
    }
    if (sym_parser->flags & FRAMER_SYMBOL_PARSER_F_BASE_ADDR)
    {
        cli_out("     base_addr = 0x%x\n", sym_parser->base_addr);
    }
    if (sym_parser->flags & FRAMER_SYMBOL_PARSER_F_OFFSET)
    {
        cli_out("     offset = 0x%x\n", sym_parser->offset);
    }
    if (sym_parser->flags & FRAMER_SYMBOL_PARSER_F_INDEX)
    {
        cli_out("     index = %d\n", sym_parser->index);
    }
SHR_VOID_FUNC_EXIT}

static int
framer_symbol_handle(
    int unit,
    uint32 flags,
    framer_symbol_cb_f cb,
    framer_symbol_t * sym,
    REG_INFO_S * reg,
    void *user_data)
{
    int widx, wlast;

    SHR_FUNC_INIT_VARS(unit);

    FRAMER_SYMBOL_W_GET(sym->index, widx);
    FRAMER_SYMBOL_W_GET(sym->last, wlast);

    if (sym->index == 0 || widx == 0)
    {
        sal_memset(sym_pool, 0, sizeof(sym_pool));
        sym_count = 0;
    }
    if (sym_count >= FRAMER_SYMBOL_DWORD_MAX)
    {
        cli_out("# WARNING: the field width of register is out of range!!!\n");
        SHR_IF_ERR_EXIT(_SHR_E_FULL);
    }
    sym_pool[sym_count++] = reg;
    if ((sym->flags & FRAMER_SYMBOL_F_WIDTH) && (wlast == 0))
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }
    sym->sinfo = &sym_pool[0];
    sym->count = sym_count;
    sym->offset = sym->sinfo[0]->offset_addr;
    if (sym->flags & FRAMER_SYMBOL_F_MEM)
    {
        sym->depth = sym->sinfo[0]->ext & MASK_20_BIT;
        sym->width = ((sym->sinfo[0]->ext >> SHIFT_BIT_20) & MASK_10_BIT) + 1;
    }
    if (framer_symbol_debug)
    {
        framer_symbol_print(unit, flags, sym);
    }
    if (cb)
    {
        SHR_IF_ERR_EXIT_NO_MSG(cb(unit, flags, sym, user_data));
    }
exit:
    SHR_FUNC_EXIT;
}

int
framer_symbol_blk_invalid(
    int unit,
    int blk_id)
{
    return FRAMER_BLK_ID_INVALID(blk_id) ? 1 : 0;
}

int
framer_symbol_blk_iter(
    int unit,
    uint32 flags,
    framer_symbol_blk_cb_f cb,
    void *user_data)
{
    int i;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & FRAMER_REGSFILE_F_RAW)
    {
        if (cb)
        {
            SHR_IF_ERR_EXIT_NO_MSG(cb(unit, flags, 0, -1, -1, -1, -1, user_data));
        }
    }
    else
    {
        for (i = 0; i < framer_blk_cnt; i++)
        {
            if (cb)
            {
                SHR_IF_ERR_CONT(cb(unit, flags, i,
                                   framer_blk_map[i].blk,
                                   framer_blk_map[i].blk_no,
                                   framer_blk_map[i].blk_num, framer_blk_map[i].schan_num, user_data));
            }
        }
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    }
exit:
    SHR_FUNC_EXIT;
}

int
framer_symbol_blk_name2id(
    int unit,
    char *blk_name,
    int *blk,
    int *blk_no)
{
    int i, blk_no_tmp, found = 0;
    uint32 blk_name_len, blk_name_str_len, dflen;
    const char *blk_name_str[] = FRAMER_BLK_NAME_STR;

    SHR_FUNC_INIT_VARS(unit);
    if (blk_name == NULL || blk_name[0] == '\0')
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }
    dflen = SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH;
    blk_name_len = sal_strnlen(blk_name, dflen);
    for (i = 0; i < framer_blk_cnt; i++)
    {
        blk_name_str_len = sal_strnlen(blk_name_str[framer_blk_map[i].blk], dflen);
        if (sal_strncasecmp(blk_name_str[framer_blk_map[i].blk], blk_name, blk_name_str_len) == 0)
        {
            if (blk_name_str_len < blk_name_len && isint(&blk_name[blk_name_str_len]))
            {
                blk_no_tmp = parse_integer(&blk_name[blk_name_str_len]);
                if (blk_no_tmp == framer_blk_map[i].blk_no)
                {
                    *blk = framer_blk_map[i].blk;
                    *blk_no = framer_blk_map[i].blk_no;
                    found = 1;
                    break;
                }
            }
            else if (blk_name_str_len == blk_name_len)
            {
                *blk = framer_blk_map[i].blk;
                if (framer_blk_map[i].blk_no < 0)
                {
                    *blk_no = framer_blk_map[i].blk_no;
                }
                else
                {
                    *blk_no = -1;
                }
                found = 1;
                break;
            }
        }
    }
    if (!found)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT;
}

int
framer_symbol_blk_num2name_id(
    int unit,
    int blk_num,
    char **blk_name,
    int *blk,
    int *blk_no)
{
    char *blk_name_str[] = FRAMER_BLK_NAME_STR;

    SHR_FUNC_INIT_VARS(unit);
    if (blk_num < 0 || blk_num >= framer_blk_cnt)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }

    if (blk_name)
    {
        *blk_name = blk_name_str[framer_blk_map[blk_num].blk];
    }
    if (blk)
    {
        *blk = framer_blk_map[blk_num].blk;
    }
    if (blk_no)
    {
        *blk_no = framer_blk_map[blk_num].blk_no;
    }

exit:
    SHR_FUNC_EXIT;
}

int
framer_symbol_array2index(
    int unit,
    char *array,
    int *index)
{
    SHR_FUNC_INIT_VARS(unit);
    if (array == NULL || array[0] == '\0')
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }
    if (isint(&array[0]))
    {
        *index = parse_integer(&array[0]);
    }
    else
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT;
}

int
framer_symbol_str2data(
    int unit,
    char *str,
    int data_count,
    uint32 *data)
{
    SHR_FUNC_INIT_VARS(unit);
    if (str == NULL || str[0] == '\0')
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }
    if (data == NULL)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_MEMORY);
    }
    if (data_count <= 1)
    {
        *data = parse_integer(str);
    }
    else
    {
        parse_long_integer(data, data_count, str);
    }

exit:
    SHR_FUNC_EXIT;
}

int
framer_symbol_name2sym_parser(
    int unit,
    uint32 flags,
    char *name,
    framer_symbol_parser_t * sym_parse)
{
    char *begin = NULL, *dot = NULL, *bracket = NULL, *minus = NULL;
    char *array = NULL, *block = NULL, *base_addr = NULL, *offset = NULL, *index = NULL;
    int aidx = 0, aidx_max = 0, rv;
    uint32 len, dflen;

    SHR_FUNC_INIT_VARS(unit);

    if (!name || !sym_parse)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }
    sal_memset(sym_parse, 0, sizeof(*sym_parse));
    /*
     * MODULE__REGISTER[ARRAY].BLOCK[BASE_ADDR][OFFSET]
     * MODULE__MEMORY[ARRAY].BLOCK[BASE_ADDR][OFFSET][INDEX]
     */
    dflen = SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH;
    len = sal_strnlen(name, dflen);
    sal_strncpy(sym_parse->sname, name, len);
    begin = &sym_parse->sname[0];
    dot = sal_strnchr(sym_parse->sname, '.', len);
    if (dot)
    {
        *dot = '\0';
        block = dot + 1;
        bracket = sal_strnchr(block, '[', len - (block - begin));
        if (bracket)
        {
            *bracket = '\0';
            base_addr = bracket + 1;

            bracket = sal_strnchr(base_addr, ']', len - (base_addr - begin));
            if (bracket)
            {
                *bracket = '\0';
                bracket++;

                bracket = sal_strnchr(bracket, '[', len - (bracket - begin));
                if (bracket)
                {
                    *bracket = '\0';
                    offset = bracket + 1;

                    bracket = sal_strnchr(offset, ']', len - (offset - begin));
                    if (bracket)
                    {
                        *bracket = '\0';
                        bracket++;

                        bracket = sal_strnchr(bracket, '[', len - (bracket - begin));
                        if (bracket)
                        {
                            *bracket = '\0';
                            index = bracket + 1;

                            bracket = sal_strnchr(index, ']', len - (index - begin));
                            if (bracket)
                            {
                                *bracket = '\0';
                                bracket++;
                            }
                        }
                    }
                }
            }
        }
    }
    bracket = sal_strnchr(begin, '[', len);
    if (bracket)
    {
        *bracket = '\0';
        array = bracket + 1;

        bracket = sal_strnchr(array, ']', len - (array - begin));
        if (bracket)
        {
            *bracket = '\0';
            bracket++;
        }
    }
    if (array)
    {
        if (array[0] != '\0')
        {
            minus = sal_strnchr(array, '-', sal_strnlen(array, len));
            if (minus)
            {
                *minus = '\0';
                minus++;
            }
            rv = framer_symbol_array2index(unit, array, &aidx);
            if (rv == _SHR_E_NOT_FOUND)
            {
                aidx = 0;
            }
            sym_parse->aidx = aidx;
            if (minus)
            {
                rv = framer_symbol_array2index(unit, minus, &aidx_max);
                if (rv == _SHR_E_NOT_FOUND)
                {
                    aidx_max = -1;
                }
            }
            else
            {
                aidx_max = aidx;
            }
            sym_parse->aidx_max = aidx_max;
            sym_parse->flags |= FRAMER_SYMBOL_PARSER_F_ARRAY;
        }
    }
    if (block)
    {
        if (block[0] != '\0' && block[0] != '*')
        {
            SHR_IF_ERR_EXIT_NO_MSG(framer_symbol_blk_name2id(unit, block, &sym_parse->blk, &sym_parse->blk_no));
            sym_parse->flags |= FRAMER_SYMBOL_PARSER_F_BLOCK;
        }
    }
    if (base_addr)
    {
        sym_parse->base_addr = parse_integer(base_addr);
        sym_parse->flags |= FRAMER_SYMBOL_PARSER_F_BASE_ADDR;
    }
    if (offset)
    {
        sym_parse->offset = parse_integer(offset);
        sym_parse->flags |= FRAMER_SYMBOL_PARSER_F_OFFSET;
    }
    if (index)
    {
        sym_parse->index = parse_integer(index);
        sym_parse->flags |= FRAMER_SYMBOL_PARSER_F_INDEX;
    }

exit:
    SHR_FUNC_EXIT;
}

int
framer_symbol_mask(
    int unit,
    framer_symbol_t * sym,
    int count,
    uint32 *mask)
{
    int i, j, w, msb, lsb, width, dw;
    uint32 fwidth, fmask;
    REG_INFO_S **sym_info = NULL;
    int wmsb, wlsb, msbm, lsbm;

    SHR_FUNC_INIT_VARS(unit);

    if (sym == NULL || mask == NULL)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_MEMORY);
    }
    sym_info = sym->sinfo;
    if (sym->flags & FRAMER_SYMBOL_F_REG)
    {
        for (w = 0; w < sym->count && w < count; w++)
        {
            for (i = 0; i < sym_info[w]->bits_num; i++)
            {
                msb = sym_info[w]->reg_bits[i].msb;
                lsb = sym_info[w]->reg_bits[i].lsb;
                fwidth = msb - lsb + 1;
                fmask = fwidth == 32 ? 0xffffffff : (((0x1 << fwidth) - 1) << lsb);
                mask[w] |= fmask;
            }
        }
    }
    else if (sym->flags & FRAMER_SYMBOL_F_MEM)
    {
        w = 0;
        width = ((sym_info[w]->ext >> SHIFT_BIT_20) & MASK_10_BIT) + 1;
        dw = (width + 32 - 1) / 32;
        for (i = 0; i < sym_info[w]->bits_num; i++)
        {
            msb = sym_info[w]->reg_bits[i].msb;
            lsb = sym_info[w]->reg_bits[i].lsb;
            wmsb = msb / 32;
            wlsb = lsb / 32;
            msbm = msb % 32;
            lsbm = lsb % 32;
            for (j = wlsb; j <= wmsb && j < dw && j < count; j++)
            {
                if (wlsb == wmsb)
                {
                    msb = msbm;
                    lsb = lsbm;
                }
                else if (j == wlsb)
                {
                    msb = 31;
                    lsb = lsbm;
                }
                else if (j == wmsb)
                {
                    msb = msbm;
                    lsb = 0;
                }
                else
                {
                    msb = 31;
                    lsb = 0;
                }
                fwidth = msb - lsb + 1;
                fmask = fwidth == 32 ? 0xffffffff : (((0x1 << fwidth) - 1) << lsb);
                mask[j] |= fmask;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
framer_symbol_field_modify(
    int unit,
    uint32 flags,
    framer_symbol_t * sym,
    framer_symbol_field_t * fld,
    void *user_data)
{
    uint32 fwidth, fdw, j;
    uint32 *value = NULL;
    uint32 len, dflen;
    framer_symbol_field_modify_ud_t *ud = (framer_symbol_field_modify_ud_t *) user_data;
    uint32 *data = (uint32 *) ud->cookie;

    SHR_FUNC_INIT_VARS(unit);

    dflen = SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH;
    for (j = 0; j < ud->count; j++)
    {
        len = sal_strnlen(fld->fldname, dflen);
        if (len == sal_strnlen(ud->param[j].fld, dflen) && sal_strncasecmp(fld->fldname, ud->param[j].fld, len) == 0)
        {
            break;
        }
    }
    if (j >= ud->count)
    {
        SHR_EXIT();
    }

    fwidth = fld->msb - fld->lsb + 1;
    fdw = (fwidth + 32 - 1) / 32;
    value = sal_alloc(sizeof(uint32) * fdw, "value");
    SHR_NULL_CHECK(value, _SHR_E_MEMORY, "value is NULL!");
    sal_memset(value, 0, sizeof(uint32) * fdw);
    SHR_IF_ERR_EXIT(framer_symbol_str2data(unit, ud->param[j].data, fdw, value));
    if (sym_debug)
    {
        cli_out("\t%s<%d:%d> = 0x", fld->fldname, fld->msb, fld->lsb);
        for (j = 0; j < fdw; j++)
        {
            cli_out("%08x ", value[j]);
        }
        cli_out("\n");
    }

    SHR_IF_ERR_EXIT(framer_field_set(data, sym->count, fld->lsb, fwidth, value));
    if (sym_debug)
    {
        for (j = 0; j < sym->count; j++)
        {
            cli_out("data[%d] = 0x%08x\n", j, data[j]);
        }
    }
exit:
    if (value)
    {
        sal_free(value);
        value = NULL;
    }
    SHR_FUNC_EXIT;
}

int
framer_symbol_field_print(
    int unit,
    uint32 flags,
    framer_symbol_t * sym,
    framer_symbol_field_t * fld,
    void *user_data)
{
    uint32 fwidth, fdw, i;
    uint32 *value = NULL;
    int first_4bit_count;
    framer_symbol_field_print_ud_t *ud = (framer_symbol_field_print_ud_t *) user_data;

    SHR_FUNC_INIT_VARS(unit);

    fwidth = fld->msb - fld->lsb + 1;
    fdw = (fwidth + 32 - 1) / 32;
    value = sal_alloc(sizeof(uint32) * fdw, "FldValue");
    SHR_NULL_CHECK(value, _SHR_E_MEMORY, "value is NULL!");
    sal_memset(value, 0, sizeof(uint32) * fdw);
    if (sym_debug)
    {
        cli_out("fwidth = %d, fdw = %d\n", fwidth, fdw);
    }
    SHR_IF_ERR_EXIT(framer_field_get(ud->data, ud->dw, fld->lsb, fwidth, value));
    first_4bit_count = ((fwidth - 1) % 32) / 4 + 1;
    if (fwidth == 1)
    {
        cli_out("%s%s<%d> = 0x", ud->pfx, fld->fldname, fld->lsb);
    }
    else
    {
        cli_out("%s%s<%d:%d> = 0x", ud->pfx, fld->fldname, fld->msb, fld->lsb);
    }
    if (fdw == 1)
    {
        cli_out("%x", value[0]);
    }
    else
    {
        for (i = 0; i < fdw; i++)
        {
            if (i == 0)
            {
                switch (first_4bit_count)
                {
                    case 1:
                        cli_out("%01x", value[i]);
                        break;
                    case 2:
                        cli_out("%02x", value[i]);
                        break;
                    case 3:
                        cli_out("%03x", value[i]);
                        break;
                    case 4:
                        cli_out("%04x", value[i]);
                        break;
                    case 5:
                        cli_out("%05x", value[i]);
                        break;
                    case 6:
                        cli_out("%06x", value[i]);
                        break;
                    case 7:
                        cli_out("%07x", value[i]);
                        break;
                    case 8:
                        cli_out("%08x", value[i]);
                        break;
                    default:
                        cli_out("%x", value[i]);
                        break;
                }
            }
            else
            {
                cli_out("%08x", value[i]);
            }
        }
    }
    cli_out("%s\n", ud->sfx ? ud->sfx : "");
exit:
    if (value)
    {
        sal_free(value);
        value = NULL;
    }
    SHR_FUNC_EXIT;
}

int
framer_symbol_field_iter(
    int unit,
    uint32 flags,
    framer_symbol_t * sym,
    framer_symbol_field_cb_f cb,
    void *user_data)
{
    REG_INFO_S **sym_info = sym->sinfo;
    int i, j, w;
    int msb, lsb, msb_last, lsb_last, msb_cal;
    int w_next, i_next;
    uint32 len, dflen;
    uint32 resetval, width, mask;
    framer_symbol_field_t fld;

    SHR_FUNC_INIT_VARS(unit);

    msb = lsb = -1;
    lsb_last = -1;
    if (flags & FRAMER_SYMBOL_FIELD_F_DBG)
    {
        SHR_IF_ERR_EXIT(framer_field_dbg_enable(1));
    }
    if (flags & FRAMER_SYMBOL_FIELD_F_DEFAULT_VALUE)
    {
        fld.rcnt = 0;
    }
    for (w = 0; w < sym->count && sym_info[w]; w++)
    {
        msb_last = msb;
        for (i = 0; i < sym_info[w]->bits_num; i++)
        {
            if (sym_info[w]->reg_bits[i].bit_name == NULL)
            {
                cli_out("# WARNING: field %d is missed in regsfile!!!\n", i);
                continue;
            }
            msb = sym_info[w]->reg_bits[i].msb + msb_last + 1;
            lsb = sym_info[w]->reg_bits[i].lsb + msb_last + 1;
            if (flags & FRAMER_SYMBOL_FIELD_F_DEFAULT_VALUE)
            {
                width = msb - lsb + 1;
                mask = width >= 32 ? 0xFFFFFFFF : ((0x1 << width) - 1);
                resetval = (sym_info[w]->default_value >> sym_info[w]->reg_bits[i].lsb) & mask;
                fld.reset_value[fld.rcnt++] = resetval;
            }
            if ((i + 1) < sym_info[w]->bits_num)
            {
                w_next = w;
                i_next = i + 1;
            }
            else if ((w + 1) < sym->count && sym_info[w + 1])
            {
                w_next = w + 1;
                i_next = 0;
            }
            else
            {
                w_next = -1;
                i_next = -1;
            }
            if (w_next >= 0 && i_next >= 0 && sym_info[w_next]->reg_bits[i_next].bit_name)
            {
                dflen = SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH;
                len = sal_strnlen(sym_info[w]->reg_bits[i].bit_name, dflen);
                if (sal_strncmp(sym_info[w]->reg_bits[i].bit_name, sym_info[w_next]->reg_bits[i_next].bit_name, len) ==
                    0)
                {
                    if (lsb_last < 0)
                    {
                        lsb_last = lsb;
                    }
                    continue;
                }
            }
            if (lsb_last >= 0)
            {
                lsb = lsb_last;
                lsb_last = -1;
            }
            j = 0;
            while (sym_info[w]->reg_bits[i].bit_name[j])
            {
                fld.fldname[j] = sal_toupper(sym_info[w]->reg_bits[i].bit_name[j]);
                j++;
            }
            fld.fldname[j] = '\0';
            fld.msb = msb;
            fld.lsb = lsb;
            if (cb)
            {
                SHR_IF_ERR_EXIT_NO_MSG(cb(unit, flags, sym, &fld, user_data));
            }
            if (flags & FRAMER_SYMBOL_FIELD_F_DEFAULT_VALUE)
            {
                fld.rcnt = 0;
            }
        }
        msb_cal = (w + 1) * 32 - 1;
        if (msb < msb_cal)
        {
            msb = msb_cal;
        }
    }
    if (flags & FRAMER_SYMBOL_FIELD_F_DBG)
    {
        SHR_IF_ERR_EXIT(framer_field_dbg_enable(0));
    }

exit:
    SHR_FUNC_EXIT;
}

static int
framer_symbol_module_iter(
    int unit,
    uint32 flags,
    framer_symbol_module_t * sm,
    framer_symbol_cb_f cb,
    void *user_data)
{
    int i, j, rv, found = 0;
    FRAMER_MODULE_INST_S *mi = sm->mi;
    FRAMER_MODULE_S *module = mi->module;
    FRAMER_C *inst_name = mi->inst_name;
    REG_INFO_S *regs = module->regs;
    REG_INFO_S *reg = NULL;
    framer_symbol_t sym;
    int prefix_len, prefix_len_removed, name_len;
    int underline_pos[FRAMER_SYMBOL_FORMAT_MAX];
    int index[FRAMER_SYMBOL_FORMAT_MAX];
    char surfix[FRAMER_SYMBOL_NAME_MAX_STR];
    int blk_id;
    uint32 mlen, dflen;
    int has_instance = 0, has_alias = 0, alias_len = 0;
    char *inst_prefix = "";

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&sym, 0, sizeof(sym));

    dflen = SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH;
    mlen = sal_strnlen(inst_name, dflen);
    if ((flags & FRAMER_REGSFILE_F_RAW) == 0)
    {
        if ((sm->inst.inst_max + 1) > 1)
        {
            has_instance = 1;
        }
        if (mlen != sm->mlen)
        {
            has_alias = 1;
            alias_len = sm->mlen;
        }
    }

    /*
     * <MODULE>__<REGISTER/MEMORY>
     */
    prefix_len = sm->prefix_len;
    prefix_len_removed = sm->prefix_len_removed;

    sym.base_addr = mi->module_addr;

    i = 0;
    mlen -= prefix_len_removed;
    if (has_alias)
    {
        alias_len -= prefix_len_removed;
    }
    while (i < mlen && (prefix_len + i) < FRAMER_SYMBOL_NAME_MAX_STR)
    {
        sym.sname[prefix_len + i] = sal_toupper(inst_name[prefix_len_removed + i]);
        if (has_alias && i < alias_len)
        {
            sym.alias[i] = sym.sname[prefix_len + i];
        }
        i++;
    }
    sym.sname[prefix_len + i] = '_';
    i++;
    sym.sname[prefix_len + i] = '_';
    i++;
    prefix_len += i;

    if (has_alias)
    {
        if (has_instance && sm->inst.inst_per_blk > 1)
        {
            sym.alias[alias_len] = '_';
            alias_len++;
            sal_snprintf(&sym.alias[alias_len], FRAMER_SYMBOL_NAME_MAX_STR - alias_len, "%s%d", inst_prefix,
                         sm->inst.inst % sm->inst.inst_per_blk);
            alias_len = sal_strnlen(sym.alias, dflen);
        }
        sym.alias[alias_len] = '_';
        alias_len++;
        sym.alias[alias_len] = '_';
        alias_len++;
    }

    for (j = 0; j < module->reg_num; j++)
    {
        reg = &regs[j];
        for (i = 0; i < FRAMER_SYMBOL_FORMAT_MAX; i++)
        {
            underline_pos[i] = -1;
            index[i] = -1;
        }
        sal_memset(surfix, 0, sizeof(surfix));
        name_len = sal_strnlen(reg->reg_name, dflen);
        if ((prefix_len + name_len) >= FRAMER_SYMBOL_NAME_MAX_STR)
        {
            cli_out("WARNING: the register [%s] is out of range!!!\n", reg->reg_name);
        }
        i = 0;
        while (i < name_len)
        {
            sym.sname[prefix_len + i] = sal_toupper(reg->reg_name[i]);
            if (has_alias)
            {
                sym.alias[alias_len + i] = sym.sname[prefix_len + i];
            }
            if ((flags & FRAMER_REGSFILE_F_RAW) == 0)
            {
                /*
                 * Find out the last underline position
                 */
                if (sym.sname[prefix_len + i] == '_')
                {
                    underline_pos[0] = underline_pos[1];
                    underline_pos[1] = i;
                }
            }
            i++;
        }
        sym.sname[prefix_len + i] = '\0';
        if (has_alias)
        {
            sym.alias[alias_len + i] = '\0';
        }

        sym.sname_len = prefix_len + i;
        sym.flags = 0;
        sym.index = 0;
        sym.last = 0;
        sym.sinfo = NULL;
        sym.count = 0;
        sym.inst = NULL;
        if (flags & FRAMER_REGSFILE_F_RAW)
        {
            sym.blk = -1;
            sym.blk_no = -1;
        }
        else
        {
            /*
             * Skip AER (Address Extension Register) registers.
             */
            if (FRAMER_AER_REG(sym.base_addr + reg->offset_addr))
            {
                continue;
            }
            blk_id = FRAMER_BLK_ID_GET(sym.base_addr + reg->offset_addr);
            if (framer_symbol_blk_invalid(unit, blk_id))
            {
                continue;
            }
            sym.blk = framer_blk_map[blk_id].blk;
            sym.blk_no = framer_blk_map[blk_id].blk_no;
        }
        if (has_instance)
        {
            sym.flags |= FRAMER_SYMBOL_F_INSTANCE;
            sym.inst = &sm->inst;
        }
        sal_strncpy(surfix, sym.sname, sym.sname_len);
        for (i = 0; i < FRAMER_SYMBOL_FORMAT_MAX; i++)
        {
            if (underline_pos[i] > 0)
            {
                surfix[prefix_len + underline_pos[i]] = '\0';
            }
        }
        for (i = 0; i < FRAMER_SYMBOL_FORMAT_MAX; i++)
        {
            if (underline_pos[i] > 0 && (underline_pos[i] + 1) <= name_len && (underline_pos[i] + 2) <= name_len)
            {
                /*
                 * M: multi-index, same to register array.
                 */
                if (surfix[prefix_len + underline_pos[i] + 1] == 'M' &&
                    surfix[prefix_len + underline_pos[i] + 2] != '\0' &&
                    isint(&surfix[prefix_len + underline_pos[i] + 2]))
                {
                    index[i] = parse_integer(&surfix[prefix_len + underline_pos[i] + 2]);
                    if (index[i] >= 0)
                    {
                        FRAMER_SYMBOL_M_SET(sym.index, index[i]);
                        sym.flags |= FRAMER_SYMBOL_F_ARRAY;
                        if ((j + 1) < module->reg_num)
                        {
                            if (sal_strncmp(reg->reg_name, regs[j + 1].reg_name, underline_pos[i]) == 0
                                && regs[j + 1].reg_name[underline_pos[i]] == '_')
                            {
                                if (sal_strnlen(reg->reg_name, dflen) + 1 < sal_strnlen(regs[j + 1].reg_name, dflen))
                                {
                                    FRAMER_SYMBOL_M_SET(sym.last, 1);
                                }
                            }
                            else
                            {
                                FRAMER_SYMBOL_M_SET(sym.last, 1);
                            }
                        }
                        else
                        {
                            FRAMER_SYMBOL_M_SET(sym.last, 1);
                        }
                    }
                }
                else
                    /*
                     * W: width-mode, which the field width of register is greater than 32 bits.
                     * Should concatenate them into one register.
                     */
                if (surfix[prefix_len + underline_pos[i] + 1] == 'W' &&
                        surfix[prefix_len + underline_pos[i] + 2] != '\0' &&
                        isint(&surfix[prefix_len + underline_pos[i] + 2]))
                {
                    index[i] = parse_integer(&surfix[prefix_len + underline_pos[i] + 2]);
                    if (index[i] >= 0)
                    {
                        FRAMER_SYMBOL_W_SET(sym.index, index[i]);
                        sym.flags |= FRAMER_SYMBOL_F_WIDTH;
                        if ((j + 1) < module->reg_num)
                        {
                            if (sal_strncmp(reg->reg_name, regs[j + 1].reg_name, underline_pos[i]) == 0
                                && regs[j + 1].reg_name[underline_pos[i]] == '_')
                            {
                                if (sal_strnlen(reg->reg_name, dflen) + 1 < sal_strnlen(regs[j + 1].reg_name, dflen))
                                {
                                    FRAMER_SYMBOL_W_SET(sym.last, 1);
                                }
                            }
                            else
                            {
                                FRAMER_SYMBOL_W_SET(sym.last, 1);
                            }
                        }
                        else
                        {
                            FRAMER_SYMBOL_W_SET(sym.last, 1);
                        }
                    }
                }
                else
                    /*
                     * The useless underline.
                     */
                {
                    underline_pos[i] = -1;
                }
            }
        }
        for (i = 0; i < FRAMER_SYMBOL_FORMAT_MAX; i++)
        {
            if (underline_pos[i] > 0)
            {
                sym.sname_len = prefix_len + underline_pos[i];
                if (has_alias)
                {
                    sym.alias[alias_len + underline_pos[i]] = '\0';
                }
                else
                {
                    sym.sname[prefix_len + underline_pos[i]] = '\0';
                }
                break;
            }
        }
        if (reg->reg_type == REG_TYPE_TRW || reg->reg_type == REG_TYPE_TRO || reg->reg_type == REG_TYPE_TWO)
        {
            sym.flags |= FRAMER_SYMBOL_F_MEM;
        }
        else
        {
            sym.flags |= FRAMER_SYMBOL_F_REG;
        }
        if (reg->reg_type == REG_TYPE_ALM)
        {
            sym.flags |= FRAMER_SYMBOL_F_ALARM;
        }
        if (reg->reg_type == REG_TYPE_ALM || reg->reg_type == REG_TYPE_CCR || reg->reg_type == REG_TYPE_CVR)
        {
            sym.flags |= FRAMER_SYMBOL_F_INIT_ON_READ;
        }
        if (reg->reg_type == REG_TYPE_INTS_YUSHUI || reg->reg_type == REG_TYPE_INTR)
        {
            sym.flags |= FRAMER_SYMBOL_F_INTR;
        }
        if (reg->reg_type == REG_TYPE_RO || reg->reg_type == REG_TYPE_ROP || reg->reg_type == REG_TYPE_TRO)
        {
            sym.flags |= FRAMER_SYMBOL_F_RO;
        }
        if (reg->reg_type == REG_TYPE_PLS ||
            reg->reg_type == REG_TYPE_WO || reg->reg_type == REG_TYPE_WOP || reg->reg_type == REG_TYPE_TWO)
        {
            sym.flags |= FRAMER_SYMBOL_F_WO;
        }
        if (reg->reg_type == REG_TYPE_CCR ||
            reg->reg_type == REG_TYPE_CCW ||
            reg->reg_type == REG_TYPE_CVR ||
            reg->reg_type == REG_TYPE_CVW ||
            reg->reg_type == REG_TYPE_CCWK ||
            reg->reg_type == REG_TYPE_CVWK || reg->reg_type == REG_TYPE_CCRK || reg->reg_type == REG_TYPE_CVRK)
        {
            sym.flags |= FRAMER_SYMBOL_F_COUNTER;
        }
        if (reg->reg_type == REG_TYPE_RST ||
            reg->reg_type == REG_TYPE_RST_ROOT_F ||
            reg->reg_type == REG_TYPE_RST_ROOT_L ||
            reg->reg_type == REG_TYPE_RST_GLB_F ||
            reg->reg_type == REG_TYPE_RST_GLB_L || reg->reg_type == REG_TYPE_RST_CTRL)
        {
            sym.flags |= FRAMER_SYMBOL_F_RST;
        }
        rv = framer_symbol_handle(unit, flags, cb, &sym, reg, user_data);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv >= 0)
        {
            found++;
        }
    }

    SHR_SET_CURRENT_ERR(found);
exit:
    SHR_FUNC_EXIT;
}

static int
framer_symbol_module_process(
    int unit,
    uint32 flags,
    int seq,
    FRAMER_MODULE_INST_S * mi,
    void *list_head,
    framer_symbol_module_t * sm)
{
    int i, name_len, len, dflen, inst;
    int prefix_len = 0, prefix_len_removed = 0;
    int blk_id, blk_id_tmp;
    int underline_pos[FRAMER_SYMBOL_FORMAT_MAX];
#if FRAMER_SYMBOL_INST_IDENTIFY_BY_ADDR == 0
    int index[FRAMER_SYMBOL_FORMAT_MAX];
#endif
    struct mi_list_s
    {
        FRAMER_MODULE_INST_S *mi;
        struct mi_list_s *next;
    }  *mi_list = NULL, *mi_list_head = list_head;
    FRAMER_MODULE_INST_S *minst = NULL;
    char *seperator = "__";

    SHR_FUNC_INIT_VARS(unit);

    sm->mi = mi;
    sm->flags = 0;
    sm->inst.inst = 0;
    sm->inst.inst_max = 0;
    sm->inst.inst_per_blk = 0;

    for (i = 0; i < FRAMER_SYMBOL_FORMAT_MAX; i++)
    {
        underline_pos[i] = -1;
#if FRAMER_SYMBOL_INST_IDENTIFY_BY_ADDR == 0
        index[i] = -1;
#endif
    }
    dflen = SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH;
    name_len = sal_strnlen(mi->inst_name, dflen);

    i = 0;
    while (i < name_len)
    {
        if ((flags & FRAMER_REGSFILE_F_PFX) == 0)
        {
            len = sal_strnlen(seperator, dflen);
            if (sal_strncmp(&mi->inst_name[i], seperator, len) == 0)
            {
                prefix_len_removed = i + len;
            }
        }
        if ((flags & FRAMER_REGSFILE_F_RAW) == 0)
        {
            if (mi->inst_name[i] == '_')
            {
                underline_pos[0] = underline_pos[1];
                underline_pos[1] = i;
            }
        }
        i++;
    }

    /*
     * The last underline
     */
    i = 1;
    if (underline_pos[i] > 0 && (underline_pos[i] + 1) <= name_len && (underline_pos[i] + 2) <= name_len)
    {
#if FRAMER_SYMBOL_INST_IDENTIFY_BY_ADDR == 0
        /*
         * M: multi-index.
         */
        if (mi->inst_name[prefix_len + underline_pos[i] + 1] == 'M' &&
            mi->inst_name[prefix_len + underline_pos[i] + 2] != '\0' &&
            isint(&mi->inst_name[prefix_len + underline_pos[i] + 2]))
        {
            index[i] = parse_integer(&mi->inst_name[prefix_len + underline_pos[i] + 2]);
            if (index[i] >= 0)
            {
                blk_id = FRAMER_BLK_ID_GET(mi->module_addr);
                sm->inst.inst = index[i];
                mi_list = mi_list_head;
                while (mi_list)
                {
                    minst = mi_list->mi;
                    /*
                     * MODULE_M0
                     */
                    if (sal_strncasecmp(mi->inst_name, minst->inst_name, prefix_len + underline_pos[i] + 1 + 1) == 0)
                    {
                        inst = parse_integer(&minst->inst_name[prefix_len + underline_pos[i] + 2]);
                        if (inst > sm->inst.inst_max)
                        {
                            sm->inst.inst_max = inst;
                        }
                        blk_id_tmp = FRAMER_BLK_ID_GET(minst->module_addr);
                        if (blk_id_tmp == blk_id)
                        {
                            sm->inst.inst_per_blk++;
                        }
                    }
                    mi_list = mi_list->next;
                }
            }
            name_len = underline_pos[i];
        }
        else
            /*
             * The useless underline.
             */
        {
            underline_pos[i] = -1;
        }
#else
        blk_id = FRAMER_BLK_ID_GET(mi->module_addr);
        inst = 0;
        mi_list = mi_list_head;
        while (mi_list)
        {
            minst = mi_list->mi;
            /*
             * MODULE_M0
             */
            len = sal_strnlen(minst->module->module_name, dflen);
            if (sal_strncmp(mi->module->module_name, minst->module->module_name, len) == 0)
            {
                if (minst->module_addr == mi->module_addr)
                {
                    sm->inst.inst = inst;
                }
                if (inst > sm->inst.inst_max)
                {
                    sm->inst.inst_max = inst;
                }
                blk_id_tmp = FRAMER_BLK_ID_GET(minst->module_addr);
                if (blk_id_tmp == blk_id)
                {
                    sm->inst.inst_per_blk++;
                }
                inst++;
            }
            mi_list = mi_list->next;
        }
        /*
         * M: multi-index.
         */
        if (mi->inst_name[prefix_len + underline_pos[i] + 1] == 'M' &&
            mi->inst_name[prefix_len + underline_pos[i] + 2] != '\0' &&
            isint(&mi->inst_name[prefix_len + underline_pos[i] + 2]))
        {
            name_len = underline_pos[i];
        }
        else
            /*
             * The useless underline.
             */
        {
            underline_pos[i] = -1;
        }
#endif
    }
    sm->mlen = name_len;
    sm->prefix_len = prefix_len;
    sm->prefix_len_removed = prefix_len_removed;

    SHR_IF_ERR_EXIT(_SHR_E_NONE);
exit:
    SHR_FUNC_EXIT;
}

int
framer_symbol_iter(
    int unit,
    uint32 flags,
    framer_symbol_cb_f cb,
    void *user_data)
{
    int i, rv = 0, found = 0;
    FRAMER_CHIP_S *sys_info = NULL;
    FRAMER_MODULE_INST_S *mi = NULL;
    framer_symbol_module_t sm;
    struct mi_list_s
    {
        FRAMER_MODULE_INST_S *mi;
        struct mi_list_s *next;
    }  *mi_list = NULL, *mi_list_head = NULL, *mi_list_next = NULL, *mi_list_prev = NULL;

    SHR_FUNC_INIT_VARS(unit);

    rv = framer_sal_j2x_get_sys_info(unit, &sys_info);
    if (rv != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

    for (i = 0; i < sys_info->reg_module_num; i++)
    {
        mi = &sys_info->module_inst[i];
        mi_list = sal_alloc(sizeof(*mi_list), "ModInstNode");
        SHR_NULL_CHECK(mi_list, _SHR_E_MEMORY, "mi_list is NULL!");
        sal_memset(mi_list, 0, sizeof(*mi_list));
        mi_list->mi = mi;
        if (mi_list_head == NULL)
        {
            mi_list_head = mi_list;
        }
        else
        {
            mi_list_prev = mi_list_next = mi_list_head;
            while (mi_list_next)
            {
                if (mi_list->mi->module_addr < mi_list_next->mi->module_addr)
                {
                    mi_list->next = mi_list_next;
                    if (mi_list_next == mi_list_head)
                    {
                        mi_list_head = mi_list;
                    }
                    else
                    {
                        mi_list_prev->next = mi_list;
                    }
                    break;
                }
                mi_list_prev = mi_list_next;
                mi_list_next = mi_list_next->next;
            }
            if (mi_list_next == NULL && mi_list_prev && mi_list_prev->next == NULL)
            {
                mi_list_prev->next = mi_list;
                mi_list->next = NULL;
            }
        }
    }

    i = 0;
    mi_list = mi_list_head;
    while (mi_list)
    {
        mi = mi_list->mi;
        SHR_IF_ERR_EXIT(framer_symbol_module_process(unit, flags, i, mi, mi_list_head, &sm));
        rv = framer_symbol_module_iter(unit, flags, &sm, cb, user_data);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv >= 0)
        {
            found += rv;
        }
        mi_list = mi_list->next;
        i++;
    }
exit:
    if (framer_symbol_debug)
    {
        cli_out("Total %d module instances iterated.\n", i);
    }
    mi_list = mi_list_head;
    while (mi_list)
    {
        mi_list_next = mi_list->next;
        sal_free(mi_list);
        mi_list = mi_list_next;
    }
    SHR_SET_CURRENT_ERR(rv < 0 ? rv : found);
    SHR_FUNC_EXIT;
}

static int
framer_symbol_filter_out(
    int unit,
    uint32 flags,
    framer_symbol_t * sym,
    framer_symbol_parser_t * sym_parse)
{
    int blk, blk_no, midx, i;
    uint32 dflen, snlen, salen, splen, reglen;
    int name_match = 0;
    char *regname = NULL;
    uint32 all_symbols = FRAMER_REGSFILE_F_REG | FRAMER_SYMBOL_F_MEM;
    SHR_FUNC_INIT_VARS(unit);

    if (sym == NULL)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }
    if (sym_parse == NULL)
    {
        if ((flags & all_symbols) == FRAMER_REGSFILE_F_REG)
        {
            if ((sym->flags & FRAMER_SYMBOL_F_REG) == 0)
            {
                SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
            }
        }
        if ((flags & all_symbols) == FRAMER_REGSFILE_F_MEM)
        {
            if ((sym->flags & FRAMER_SYMBOL_F_MEM) == 0)
            {
                SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
            }
        }
        if (flags & FRAMER_REGSFILE_F_CTR)
        {
            if ((sym->flags & FRAMER_SYMBOL_F_COUNTER) == 0)
            {
                SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
            }
        }
        if (flags & FRAMER_REGSFILE_F_ALM)
        {
            if ((sym->flags & FRAMER_SYMBOL_F_ALARM) == 0)
            {
                SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
            }
        }
    }
    else
    {
        dflen = SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH;
        snlen = sal_strnlen(sym->sname, dflen);
        salen = sal_strnlen(sym->alias, dflen);
        splen = sal_strnlen(sym_parse->sname, dflen);
        if (sal_strncasecmp(sym_parse->sname, "*", splen) == 0 ||
            sal_strncasecmp(sym_parse->sname, sym->sname, splen) == 0 ||
            sal_strncasecmp(sym_parse->sname, sym->alias, splen) == 0 ||
            (splen != snlen && sal_strncasecmp(sym_parse->sname, sym->sname, splen) == 0) ||
            (splen != salen && sal_strncasecmp(sym_parse->sname, sym->alias, splen) == 0))
        {
            name_match = 1;
        }
        if (name_match == 0)
        {
            regname = salen ? sym->alias : sym->sname;
            reglen = salen ? salen : snlen;
            if ((flags & FRAMER_REGSFILE_F_EXT) && (splen < reglen))
            {
                i = 0;
                while (regname && (regname[i] != '\0') && (reglen - i >= splen))
                {
                    if (sal_strncasecmp(&regname[i], sym_parse->sname, splen) == 0)
                    {
                        name_match = 1;
                        break;
                    }
                    i++;
                }
            }
        }
        if (name_match == 0)
        {
            SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
        }
        if (splen == salen || splen == snlen)
        {
            sym_parse->flags |= FRAMER_SYMBOL_PARSER_F_EXACT_MATCH;
        }
        if (sym_parse->flags & FRAMER_SYMBOL_PARSER_F_BLOCK)
        {
            if (flags & FRAMER_REGSFILE_F_RAW)
            {
                blk = -1;
                blk_no = -1;
            }
            else
            {
                blk = sym_parse->blk;
                blk_no = sym_parse->blk_no;
            }
            if (sym->blk != blk || (blk_no >= 0 && sym->blk_no != blk_no))
            {
                SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
            }
        }
        if (sym_parse->flags & FRAMER_SYMBOL_PARSER_F_ARRAY)
        {
            FRAMER_SYMBOL_M_GET(sym->index, midx);
            if (sym_parse->aidx_max == -1)
            {
                if (midx < sym_parse->aidx)
                {
                    SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
                }
            }
            else
            {
                if (midx < sym_parse->aidx || midx > sym_parse->aidx_max)
                {
                    SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
                }
            }
        }
        if (sym_parse->flags & FRAMER_SYMBOL_PARSER_F_BASE_ADDR)
        {
            if (sym_parse->base_addr != sym->base_addr)
            {
                SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
            }
        }
        if (sym_parse->flags & FRAMER_SYMBOL_PARSER_F_OFFSET)
        {
            if (sym_parse->offset != sym->sinfo[0]->offset_addr)
            {
                SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
            }
        }
    }

    SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NONE);

exit:
    SHR_FUNC_EXIT;
}

static int
framer_symbol_filter_cb(
    int unit,
    uint32 flags,
    framer_symbol_t * sym,
    void *user_data)
{
    int rv = _SHR_E_NOT_FOUND;
    framer_symbol_filter_t *filter = (framer_symbol_filter_t *) user_data;
    char *name = filter->name;
    framer_symbol_parser_t sym_parse;

    SHR_FUNC_INIT_VARS(unit);

    if (!name)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }
    SHR_IF_ERR_EXIT_NO_MSG(framer_symbol_filter_out(unit, flags, sym, NULL));
    SHR_IF_ERR_EXIT_NO_MSG(framer_symbol_name2sym_parser(unit, flags, name, &sym_parse));
    SHR_IF_ERR_EXIT_NO_MSG(framer_symbol_filter_out(unit, flags, sym, &sym_parse));
    if (flags & FRAMER_REGSFILE_F_DBG)
    {
        framer_symbol_print(unit, flags, sym);
        framer_symbol_parser_print(unit, &sym_parse);
    }
    if (sym_parse.flags & FRAMER_SYMBOL_PARSER_F_EXACT_MATCH)
    {
        rv = filter->cb(unit, flags | FRAMER_REGSFILE_F_EM, sym, filter->user_data);
    }
    else
    {
        rv = filter->cb(unit, flags, sym, filter->user_data);
    }
    if (SHR_SUCCESS(rv))
    {
        filter->stat.success++;
    }
    else if (rv == _SHR_E_NOT_FOUND)
    {
        filter->stat.skipped++;
    }
    else
    {
        filter->stat.failure++;
        if (filter->flags & FRAMER_FILTER_DBG)
        {
            framer_symbol_print(unit, flags, sym);
        }
        if (filter->flags & FRAMER_FILTER_CONT_ON_ERR)
        {
            rv = _SHR_E_NONE;
        }
    }
    SHR_IF_ERR_EXIT_NO_MSG(rv);

exit:
    SHR_FUNC_EXIT;
}

void
framer_symbol_filter_t_init(
    framer_symbol_filter_t * filter)
{
    if (filter)
    {
        sal_memset(filter, 0, sizeof(framer_symbol_filter_t));
    }
}

int
framer_symbol_filter(
    int unit,
    uint32 flags,
    framer_symbol_filter_t * filter)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);
    rv = framer_symbol_iter(unit, flags, framer_symbol_filter_cb, filter);
    if (filter->summary)
    {
        cli_out("SUMMARY: %s -> {success = %d, failure = %d, skipped = %d}\n",
                filter->summary, filter->stat.success, filter->stat.failure, filter->stat.skipped);
    }
    if (filter->flags & FRAMER_FILTER_LOOP)
    {
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT_NO_MSG(filter->stat.failure ? _SHR_E_FAIL : filter->stat.success == 0 ? _SHR_E_NOT_FOUND : rv);
    }
exit:
    SHR_FUNC_EXIT;
}
#endif /* NO_FRAMER_LIB_BUILD */
