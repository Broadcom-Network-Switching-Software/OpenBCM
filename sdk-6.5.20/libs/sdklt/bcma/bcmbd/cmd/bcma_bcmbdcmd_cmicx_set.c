/*! \file bcma_bcmbdcmd_cmicx_set.c
 *
 * CLI 'set' command for CMICx
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcma/cli/bcma_cli_ctrlc.h>

#include <bcma/bcmbd/bcma_bcmbd_cmicx.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicx_set.h>

#if BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS == 1

/*
 * Private data for symbol iterator.
 */
typedef struct cmicx_iter_s {
    int unit;
    bcma_bcmbd_id_t *sid;
    int ctoks_num;
    bcma_cli_tokens_t *ctoks;
    int invalid_syms;
} cmicx_iter_t;

/*
 * Get or Set the data for a symbol -- symbol iterator function
 */
static int
iter_op(const bcmdrd_symbol_t *symbol, bcmdrd_sid_t symbol_id, void *vptr)
{
    int rv;
    uint32_t and_masks[BCMDRD_MAX_PT_WSIZE];
    uint32_t or_masks[BCMDRD_MAX_PT_WSIZE];
    cmicx_iter_t *cmicx_iter = (cmicx_iter_t *)vptr;
    bcma_bcmbd_id_t sid;
    int ctoks_num = cmicx_iter->ctoks_num;
    bcma_cli_tokens_t *ctoks = cmicx_iter->ctoks;
    const char **fnames;
    int unit = cmicx_iter->unit;
    const bcmdrd_symbols_t *symbols = bcmdrd_dev_symbols_get(unit, 0);

    if (bcma_cli_ctrlc_break()) {
        return BCMA_CLI_CMD_INTR;
    }

    if (!bcmdrd_dev_sym_valid(unit, symbol_id)) {
        cmicx_iter->invalid_syms++;
        return 0;
    }

    assert(symbols);

    fnames = symbols->field_names;

    sal_memcpy(&sid, cmicx_iter->sid, sizeof(sid));

    /* Copy the address in for this symbol */
    sal_strlcpy(sid.addr.name, symbol->name, sizeof(sid.addr.name));
    sid.addr.name32 = symbol->addr;

    /* These CSTs contain the data and/or field assignments */
    rv = bcma_bcmbd_encode_fields_from_tokens(symbol, fnames, ctoks_num, ctoks,
                                              BCMDRD_MAX_PT_WSIZE,
                                              and_masks, or_masks);
    if (rv < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcma_bcmbd_cmicx_symop(unit, symbol, symbol_id, &sid,
                                and_masks, or_masks);
    if (rv < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    return 0;
}

static int
cmicx_set(bcma_cli_t *cli, bcma_cli_args_t *args,
          bcma_cli_tokens_t *ctoks, int ctoks_num)
{
    int rv;
    int unit;
    const char *arg = NULL;
    bcma_bcmbd_parse_info_t parse_info, *pi = &parse_info;
    const bcmdrd_symbols_t *symbols;
    bcma_bcmbd_id_t sid;
    bcmdrd_symbols_iter_t iter;
    cmicx_iter_t cmicx_iter;

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

    /* Crack the identifier */
    if (pi->identifier == NULL) {
        return bcma_cli_parse_error("symbol", NULL);
    }
    if (bcma_bcmbd_parse_id(pi->identifier, &sid, 0) < 0) {
        return bcma_cli_parse_error("identifier", pi->identifier);
    }

    if (pi->data_start == 0) {
        return bcma_cli_parse_error("data", NULL);
    }

    sid.flags = pi->cmd_flags;

    sal_memset(&cmicx_iter, 0, sizeof(cmicx_iter));
    cmicx_iter.unit = unit;
    cmicx_iter.ctoks_num = ctoks_num;
    cmicx_iter.ctoks = &ctoks[pi->data_start];
    cmicx_iter.sid = &sid;

    sal_memset(&iter, 0, sizeof(iter));
    iter.pflags = pi->sym_pflags;
    iter.aflags = pi->sym_aflags;
    iter.name = sid.addr.name;
    iter.symbols = symbols;
    iter.function = iter_op;
    iter.vptr = &cmicx_iter;

    /* Iterate over all matching symbols */
    rv = bcmdrd_symbols_iter(&iter);
    if (rv > 0) {
        /* Adjust for disabled symbols */
        rv -= cmicx_iter.invalid_syms;
    }
    if (rv == 0) {
        cli_out("no matching symbols\n");
    }

    return (rv < 0) ? rv : BCMA_CLI_CMD_OK;
}

#endif /* BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS */

int
bcma_bcmbdcmd_cmicx_set(bcma_cli_t *cli, bcma_cli_args_t *args)
{
#if BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS == 0
    return BCMA_CLI_CMD_NO_SYM;
#else
    int rv;
    int ctoks_num = BCMA_CLI_CONFIG_ARGS_CNT_MAX;
    bcma_cli_tokens_t *ctoks;

    ctoks = sal_alloc(ctoks_num * sizeof(bcma_cli_tokens_t),
                      "bcmaCliSetCtoks");
    if (ctoks == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    rv = cmicx_set(cli, args, ctoks, ctoks_num);

    sal_free(ctoks);

    return rv;
#endif /* BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS */
}
