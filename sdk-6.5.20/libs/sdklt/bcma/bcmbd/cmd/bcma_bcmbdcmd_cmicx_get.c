/*! \file bcma_bcmbdcmd_cmicx_get.c
 *
 * CLI 'get' command for CMICx
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcma/cli/bcma_cli_ctrlc.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/bcmbd/bcma_bcmbd_cmicx.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicx_get.h>

#if BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS == 1

/*
 * Private data for symbol iterator.
 */
typedef struct cmicx_iter_s {
    int unit;
    bcma_bcmbd_id_t *sid;
} cmicx_iter_t;

/*
 * Private data for fields filtering call-back function.
 */
typedef struct fields_filter_cookie_s {
    const bcma_cli_tokens_t *ctok_names;
    uint32_t pflags;
    uint32_t aflags;
} fields_filter_cookie_t;

/*
 * Fields filter function if field names or field flags are specified
 * in the 'get' command.
 */
static int
fields_filter(const bcmdrd_symbol_t *symbol, const char *name, uint32_t flags,
              uint32_t *data, void *cookie)
{
    int idx;
    fields_filter_cookie_t *filter = (fields_filter_cookie_t *)cookie;
    const bcma_cli_tokens_t *ctok = filter->ctok_names;

    /* Check flags which must be present */
    if (filter->pflags && ((flags & filter->pflags) != filter->pflags)) {
        /* Field does not match */
        return 1;
    }

    /* Check flags which must be absent */
    if (filter->aflags && ((flags & filter->aflags) != 0)) {
        /* Field does not match */
        return 1;
    }

    /* No more field names to check */
    if (ctok == NULL) {
        return 0;
    }

    /* Filtering fields names */
    for (idx = 0; idx < ctok->argc; idx++) {
        if (sal_strcasecmp(name, ctok->argv[idx]) == 0) {
            return 0;
        }
    }

    return 1;
}

/*
 * Get or Set the data for a symbol -- symbol iterator function
 */
static int
iter_op(const bcmdrd_symbol_t *symbol, bcmdrd_sid_t symbol_id, void *vptr)
{
    int rv;
    cmicx_iter_t *cmicx_iter = (cmicx_iter_t *)vptr;
    bcma_bcmbd_id_t sid;
    int unit = cmicx_iter->unit;

    if (bcma_cli_ctrlc_break()) {
        return BCMA_CLI_CMD_INTR;
    }

    if (!bcmdrd_dev_sym_valid(unit, symbol_id)) {
        return 0;
    }

    sal_memcpy(&sid, cmicx_iter->sid, sizeof(sid));

    /* Copy the address in for this symbol */
    sal_strlcpy(sid.addr.name, symbol->name, sizeof(sid.addr.name));
    sid.addr.name32 = symbol->addr;

    rv = bcma_bcmbd_cmicx_symop(unit, symbol, symbol_id, &sid, NULL, NULL);
    if (rv < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    return 0;
}

static int
cmicx_get(bcma_cli_t *cli, bcma_cli_args_t *args,
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
    fields_filter_cookie_t cookie;

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
        /* Match any symbol by default */
        pi->identifier = "*";
    }
    if (bcma_bcmbd_parse_id(pi->identifier, &sid, 0) < 0) {
        return bcma_cli_parse_error("identifier", pi->identifier);
    }

    sal_memset(&cookie, 0, sizeof(cookie));
    cookie.ctok_names = pi->fld_names;
    cookie.pflags = pi->fld_pflags;
    cookie.aflags = pi->fld_aflags;

    sid.flags = pi->cmd_flags;
    sid.ccb = fields_filter;
    sid.ccb_cookie = &cookie;

    sal_memset(&cmicx_iter, 0, sizeof(cmicx_iter));
    cmicx_iter.unit = unit;
    cmicx_iter.sid = &sid;

    sal_memset(&iter, 0, sizeof(iter));
    iter.pflags = pi->sym_pflags;
    iter.aflags = pi->sym_aflags;
    iter.name = sid.addr.name;
    iter.symbols = symbols;
    iter.function = iter_op;
    iter.vptr = &cmicx_iter;

    /* Iterate over all matching symbols */
    if ((rv = bcmdrd_symbols_iter(&iter)) == 0) {
        cli_out("no matching symbols\n");
    }

    return (rv < 0) ? rv : BCMA_CLI_CMD_OK;
}

#endif /* BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS */

int
bcma_bcmbdcmd_cmicx_get(bcma_cli_t *cli, bcma_cli_args_t *args)
{
#if BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS == 0
    return BCMA_CLI_CMD_NO_SYM;
#else
    int rv;
    int ctoks_num = 7;
    bcma_cli_tokens_t *ctoks;

    ctoks = sal_alloc(ctoks_num * sizeof(bcma_cli_tokens_t),
                      "bcmaCliGetCtoks");
    if (ctoks == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    rv = cmicx_get(cli, args, ctoks, ctoks_num);

    sal_free(ctoks);

    return rv;
#endif /* BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS */
}
