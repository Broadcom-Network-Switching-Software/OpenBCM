/*! \file bcma_bcmbdcmd_cmicx_list.c
 *
 * CLI 'list' command for CMICx
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_pb.h>
#include <shr/shr_pb_format.h>

#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_sym_dump.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_pt_ser.h>

#include <bcma/cli/bcma_cli_ctrlc.h>

#include <bcma/bcmbd/bcma_bcmbd_cmicx.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicx_list.h>

#if BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS == 1

typedef struct sym_info_s {
    int unit;
    uint32_t flags;
    const char *view;
} sym_info_t;

#if BCMDRD_CONFIG_INCLUDE_FIELD_INFO == 1

static int
view_filter(const bcmdrd_symbol_t *symbol, const char **fnames,
            const char *encoding, void *cookie)
{
    char *view = (char *)cookie;
    char *ptr;

    /* Do not filter if no (or unknown) encoding */
    if (encoding == NULL || *encoding != '{' || view == NULL) {
        return 0;
    }

    /* Do not filter if encoding cannot be parsed */
    if ((ptr = sal_strchr(encoding, '}')) == NULL) {
        return 0;
    }

    /* Do not filter if view (partially) matches */
    ptr++;
    if (sal_strcmp(view, "*") == 0 ||
        sal_strncasecmp(ptr, view, sal_strlen(view)) == 0) {
        return 0;
    }

    /* No match - filter this field */
    return 1;
}

#endif /* BCMDRD_CONFIG_INCLUDE_FIELD_INFO */

static int
print_ser_en_ctrl(shr_pb_t *pb, const bcmdrd_symbols_t *symbols,
               bcmdrd_pt_ser_en_ctrl_t *ser_ec)
{
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    bcmdrd_sym_field_info_t field_info, *finfo = &field_info;

    bcmdrd_sym_info_get(symbols, ser_ec->sid, sinfo);
    bcmdrd_sym_field_info_get(symbols, ser_ec->sid, ser_ec->fid, finfo);
    shr_pb_printf(pb, "%s.%s", sinfo->name, finfo->name);
    if (ser_ec->fmask != 0) {
        shr_pb_printf(pb, "(mask=0x%"PRIx32")", ser_ec->fmask);
    }

    return 0;
}

static int
print_valid_tbl_inst(int unit, bcmdrd_sid_t sid, shr_pb_t *pb)
{
    int num_inst, inst;
    SHR_BITDCLNAME(ibmp, BCMDRD_CONFIG_MAX_PORTS);

    num_inst = bcmdrd_pt_num_tbl_inst(unit, sid);
    if (num_inst <= 1) {
        return 0;
    }

    shr_pb_printf(pb, "%-13s ", "Instance:");

    SHR_BITCLR_RANGE(ibmp, 0, BCMDRD_CONFIG_MAX_PORTS);
    for (inst = 0; inst < num_inst; inst++) {
        if (bcmdrd_pt_index_valid(unit, sid, inst, -1)) {
            SHR_BITSET(ibmp, inst);
        }
    }
    shr_pb_format_bit_list(pb, ibmp, BCMDRD_CONFIG_MAX_PORTS);

    shr_pb_printf(pb, "\n");

    return 0;
}

/*
 * Prints the information for a single symbol
 */
static int
print_sym(const bcmdrd_symbol_t *s, bcmdrd_sid_t symbol_id, void *vptr)
{
    sym_info_t *si = (sym_info_t *)vptr;
    uint32_t mask, flags, acc_type, blk_info;
    uint32_t size, min, max, step;
    int blktype, idx;
    int unit = si->unit;
    const char *flagstr;
    const bcmdrd_symbols_t *symbols = bcmdrd_dev_symbols_get(unit, 0);
    bcmdrd_pt_ser_info_t ser_info, *seri = &ser_info;
    bcmdrd_pt_ser_en_ctrl_t ser_en_ctrl, *ser_ec = &ser_en_ctrl;
    int rv;
    shr_pb_t *pb;
    uint32_t addr;
    const bcmdrd_block_t *pblk;
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);
    bool is_mem;

    if (bcma_cli_ctrlc_break()) {
        return BCMA_CLI_CMD_INTR;
    }

    if (!bcmdrd_dev_sym_valid(unit, symbol_id)) {
        return 0;
    }

    if (si->flags & BCMA_BCMBD_CMD_IDF_RAW) {
        cli_out("%s\n", s->name);
        return 0;
    }

    pb = shr_pb_create();

    size = BCMDRD_SYMBOL_INDEX_SIZE_GET(s->index);
    min = BCMDRD_SYMBOL_INDEX_MIN_GET(s->index);
    max = BCMDRD_SYMBOL_INDEX_MAX_GET(s->index);
    step = BCMDRD_SYMBOL_INDEX_STEP_GET(s->index);

    /* Extract dual-pipe access type from flags */
    flags = s->flags;
    acc_type = BCMDRD_SYM_INFO_ACCTYPE(s->info);
    blk_info = BCMDRD_SYM_INFO_BLKTYPES(s->info);

    is_mem = ((flags & BCMDRD_SYMBOL_FLAG_MEMORY) == BCMDRD_SYMBOL_FLAG_MEMORY);

    /* Max index may depend on current chip configuration */
    if (is_mem) {
        max = bcmdrd_chip_mem_maxidx(bcmdrd_dev_chip_info_get(unit),
                                     symbol_id, max);
    }

    shr_pb_printf(pb, "%-13s %s", "Name:", s->name);
#if  BCMDRD_CONFIG_INCLUDE_ALIAS_NAMES == 1
    if (s->ufname) {
        shr_pb_printf(pb, ", %s", s->ufname);
    }
    if (s->alias) {
        shr_pb_printf(pb, ", %s", s->alias);
    }
#endif
    shr_pb_printf(pb, "\n");
    addr = s->addr;
    pblk = bcmdrd_chip_block_types_match(cinfo, blk_info, NULL);
    if (pblk) {
        addr = bcmbd_block_port_addr(unit, pblk->blknum, is_mem ? -1 : 0,
                                     s->addr, 0);
    }
    shr_pb_printf(pb, "%-13s 0x%08"PRIx32, "Offset:", s->addr);
    if (addr != s->addr) {
        shr_pb_printf(pb, " (0x%08"PRIx32")", addr);
    }
    shr_pb_printf(pb, "\n");
    if (acc_type) {
        shr_pb_printf(pb, "%-13s %"PRIu32"\n", "Access:", acc_type);
    }
    if (is_mem) {
        shr_pb_printf(pb, "%-13s %"PRIu32" bytes (%"PRIu32" words)\n",
                      "Size:", size, BCMDRD_BYTES2WORDS(size));
    } else {
        const uint32_t *resetval;

        shr_pb_printf(pb, "%-13s %d-bit\n", "Size:", BCMDRD_BYTES2BITS(size));

        resetval = bcmdrd_sym_reg_resetval_get(symbols, s);
        if (resetval) {
            int wsize;

            wsize = BCMDRD_BYTES2WORDS(size);
            shr_pb_printf(pb, "%-13s ", "Reset value:");
            shr_pb_format_uint32(pb, NULL, resetval, wsize, wsize << 5);
            shr_pb_printf(pb, "\n");

            shr_pb_printf(pb, "%-13s ", "Reset mask:");
            shr_pb_format_uint32(pb, NULL, resetval + wsize, wsize, wsize << 5);
            shr_pb_printf(pb, "\n");
        }
    }

    shr_pb_printf(pb, "%-13s ", "Flags:");

    for (mask = 1; mask; mask <<= 1) {
        if (flags & mask) {
            flagstr = bcma_bcmbd_symflag_type2name(mask);
            if (flagstr != NULL) {
                shr_pb_printf(pb, "%s,", flagstr);
            }
        }
    }

    shr_pb_printf(pb, "(0x%"PRIx32")\n", flags);

    shr_pb_printf(pb, "%-13s ", "Blocks:");

    for (idx=0; idx < BCMA_BCMBD_MAX_BLKTYPES_PER_SYM; idx++) {
        blktype = bcma_bcmbd_info_block_type_get(unit, idx, blk_info);
        if (blktype < 0) {
            break;
        }
        flagstr = bcma_bcmbd_block_type2name(unit, blktype);
        if (flagstr != NULL) {
            shr_pb_printf(pb, "%s,", flagstr);
        }
    }

    shr_pb_printf(pb, "(0x%"PRIx32")\n", blk_info);

    if (is_mem || (max - min) > 0) {
        shr_pb_printf(pb, "%-13s %"PRIu32"-%"PRIu32"", "Index:", min, max);
        if (step > 1) {
            shr_pb_printf(pb, " (step 0x%"PRIx32")", step);
        }
        shr_pb_printf(pb, "\n");
    }

    /* Instance information */
    print_valid_tbl_inst(unit, symbol_id, pb);

    /* MOR information */
    shr_pb_printf(pb, "%-13s ", "MOR clocks:");
    shr_pb_printf(pb, "%d\n", bcmbd_mor_clks_read_get(unit, symbol_id));

    /* SER information */
    shr_pb_printf(pb, "%-13s ", "ECC/SER:");
    rv = bcmdrd_pt_ser_info_get(unit, symbol_id, seri);
    if (rv == SHR_E_NONE) {
        shr_pb_printf(pb, "yes\n");
        shr_pb_printf(pb, "\t%-15s %s\n",
                      "Memory type:", bcma_bcmbd_ser_type2name(seri->type));
        shr_pb_printf(pb, "\t%-15s %d\n", "XOR banks:", seri->num_xor_banks);
        shr_pb_printf(pb, "\t%-15s %s\n",
                      "Drop packet:", seri->drop_pkt ? "true" : "false");
        /* Display control register/field for packet drop if any */
        if (SHR_SUCCESS(
            bcmdrd_pt_ser_drop_pkt_ctrl_get(unit, symbol_id, ser_ec))) {
            shr_pb_printf(pb, "\t%-15s ", "");
            print_ser_en_ctrl(pb, symbols, ser_ec);
            shr_pb_printf(pb, "\n");
        }
        shr_pb_printf(pb, "\t%-15s %s\n",
                      "Response:", bcma_bcmbd_ser_resp_type2name(seri->resp));
    } else {
        shr_pb_printf(pb, "n/a\n");
    }
    /* SER control register/field information */
    for (idx = 0; idx < BCMDRD_SER_EN_TYPE_MAX; idx++) {
        rv = bcmdrd_pt_ser_en_ctrl_get(unit, idx, symbol_id, ser_ec);
        if (rv != SHR_E_NONE) {
            continue;
        }
        shr_pb_printf(pb, "\t%-15s ", bcma_bcmbd_ser_en_type2name(idx));
        print_ser_en_ctrl(pb, symbols, ser_ec);
        shr_pb_printf(pb, "\n");
    }

#if BCMDRD_CONFIG_INCLUDE_FIELD_INFO == 1
    if (s->fields && (si->flags & BCMA_BCMBD_CMD_IDF_NONZERO) == 0) {
        bcmdrd_symbol_filter_cb_f filter_cb;
        uint32_t sym_flags = BCMDRD_SYM_DUMP_BIT_RANGE;

        assert(symbols);

        if (si->flags & BCMA_BCMBD_CMD_IDF_COMPACT) {
            sym_flags |= BCMDRD_SYM_DUMP_SINGLE_LINE;
        }
        filter_cb = (si->view) ? view_filter : bcmdrd_symbol_field_filter;
        shr_pb_printf(pb, "%-13s %"PRIu32"\n",
                      "Fields:", bcmdrd_sym_field_info_count(s->fields));
        bcmdrd_sym_dump_fields(pb, s, symbols->field_names, NULL, sym_flags,
                               filter_cb, (void *)si->view, NULL, NULL);
    }
#endif

    cli_out("%s\n", shr_pb_str(pb));
    shr_pb_destroy(pb);
    return 0;
}

static int
cmicx_list(bcma_cli_t *cli, bcma_cli_args_t *args,
           bcma_cli_tokens_t *ctoks, int ctoks_num)
{
    int rv;
    int unit;
    const char *name = NULL;
    const char *arg = NULL;
    bcma_bcmbd_parse_info_t parse_info, *pi = &parse_info;
    const bcmdrd_symbols_t *symbols;
    bcmdrd_symbols_iter_t iter;
    sym_info_t sym_info;

    unit = cli->cmd_unit;
    if (!bcmdrd_dev_exists(unit)) {
        return BCMA_CLI_CMD_BAD_ARG;
    }
    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return BCMA_CLI_CMD_NO_SYM;
    }

    /* Parse all of our input arguments for options */
    if ((arg = bcma_bcmbd_parse_args(args, ctoks, ctoks_num)) != NULL) {
        /* Error in argument */
        return bcma_cli_parse_error("", arg);
    }

    /* Look through our arguments */
    if ((rv = bcma_bcmbd_parse_cmd(unit, ctoks, ctoks_num, pi)) < 0) {
        return rv;
    }

    name = pi->identifier;
    if (name == NULL) {
        return bcma_cli_parse_error("symbol", NULL);
    }

    sal_memset(&sym_info, 0, sizeof(sym_info));
    sym_info.unit = unit;
    sym_info.view = pi->view;
    sym_info.flags = pi->cmd_flags;

    sal_memset(&iter, 0, sizeof(iter));
    iter.pflags = pi->sym_pflags;
    iter.aflags = pi->sym_aflags;
    iter.symbols = symbols;
    iter.function = print_sym;
    iter.vptr = &sym_info;

    /*
     * By default we list all symbols with the input name as a
     * substring.
     */
    iter.matching_mode = BCMDRD_SYMBOLS_ITER_MODE_STRSTR;

    /*
     * The user can specify explicitly the type of matching with the
     * first character.
     */
    if (sal_strcmp(name, "*") != 0) {
        switch (name[0]) {
        case '^':
            iter.matching_mode = BCMDRD_SYMBOLS_ITER_MODE_START;
            name++;
            break;
        case '*':
            iter.matching_mode = BCMDRD_SYMBOLS_ITER_MODE_STRSTR;
            name++;
            break;
        case '@':
            iter.matching_mode = BCMDRD_SYMBOLS_ITER_MODE_EXACT;
            name++;
            break;
        default:
            break;
        }
    }
    iter.name = name;

    /* Iterate over all matching symbols */
    if ((rv = bcmdrd_symbols_iter(&iter)) == 0) {
        cli_out("no matching symbols\n");
    }

    return rv < 0 ? rv : BCMA_CLI_CMD_OK;
}

#endif /* BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS */

int
bcma_bcmbdcmd_cmicx_list(bcma_cli_t *cli, bcma_cli_args_t *args)
{
#if BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS == 0
    return BCMA_CLI_CMD_NO_SYM;
#else
    int rv;
    int ctoks_num = 5;
    bcma_cli_tokens_t *ctoks;

    ctoks = sal_alloc(ctoks_num * sizeof(bcma_cli_tokens_t),
                      "bcmaCliListCtoks");
    if (ctoks == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    rv = cmicx_list(cli, args, ctoks, ctoks_num);

    sal_free(ctoks);

    return rv;
#endif /* BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS */
}
