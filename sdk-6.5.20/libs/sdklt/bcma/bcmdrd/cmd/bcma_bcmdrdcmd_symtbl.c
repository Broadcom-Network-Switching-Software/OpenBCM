/*! \file bcma_bcmdrdcmd_symtbl.c
 *
 * CLI 'symtbl' command for decode/encode the symbol-specific value format.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <sal/sal_assert.h>

#include <shr/shr_util.h>
#include <shr/shr_pb.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_sym_dump.h>

#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/cli/bcma_cli_token.h>

#include <bcma/bcmdrd/bcma_bcmdrdcmd_symtbl.h>


#if BCMDRD_CONFIG_INCLUDE_FIELD_INFO == 1

static int
symtbl_encode_field(const bcmdrd_symbol_t *sinfo, const char **fnames,
                    const char *field, char *value, uint32_t *data)
{
    int wsize, buf_wcnt;
    uint32_t val[BCMDRD_MAX_PT_WSIZE];
    bcmdrd_sym_field_info_t finfo;

    wsize = BCMDRD_BYTES2WORDS(BCMDRD_SYMBOL_INDEX_SIZE_GET(sinfo->index));
    buf_wcnt = COUNTOF(val);
    BCMDRD_SYM_FIELDS_ITER_BEGIN(sinfo->fields, finfo, fnames) {
        if (sal_strcasecmp(finfo.name, field)) {
            continue;
        }

        sal_memset(val, 0, sizeof(val));
        if (bcma_cli_parse_array_uint32(value, buf_wcnt, val) <= 0) {
            return -1;
        }

        if (sinfo->flags & BCMDRD_SYMBOL_FLAG_BIG_ENDIAN) {
            bcmdrd_field_be_set(data, wsize, finfo.minbit, finfo.maxbit, val);
        } else {
            bcmdrd_field_set(data, finfo.minbit, finfo.maxbit, val);
        }

        return 0;
    } BCMDRD_SYM_FIELDS_ITER_END();

    return -1;
}

static int
symtbl_encode(const bcmdrd_symbols_t *symbols, const bcmdrd_symbol_t *sinfo,
              bcma_cli_tokens_t *ctoks, int ctoks_num, shr_pb_t *pb)
{
    int idx, rv, wsize;
    uint32_t data[BCMDRD_MAX_PT_WSIZE];
    const char **fnames;

    fnames = symbols->field_names;
    sal_memset(data, 0, sizeof(data));

    for (idx = 0; idx < ctoks_num && ctoks[idx].argc == 2; idx++) {
        /* All tokens are treated as field=value */
        rv = symtbl_encode_field(sinfo, fnames,
                                 ctoks[idx].argv[0], ctoks[idx].argv[1], data);
        if (rv < 0) {
            return bcma_cli_parse_error("field", ctoks[idx].str);
        }
    }

    wsize = BCMDRD_BYTES2WORDS(BCMDRD_SYMBOL_INDEX_SIZE_GET(sinfo->index));
    for (idx = 0; idx < wsize; idx++) {
        shr_pb_printf(pb, " 0x%08"PRIx32, data[idx]);
    }
    shr_pb_printf(pb, "\n");
    cli_out("%s", shr_pb_str(pb));

    return BCMA_CLI_CMD_OK;
}

static int
symtbl_decode(const bcmdrd_symbols_t *symbols, const bcmdrd_symbol_t *sinfo,
              const bcma_cli_tokens_t *ctoks, int ctoks_num, shr_pb_t *pb)
{
    int idx, buf_wcnt, wcnt, widx;
    uint32_t flags = BCMDRD_SYM_DUMP_EXT_HEX;
    uint32_t data[BCMDRD_MAX_PT_WSIZE], *dptr;
    const char **fnames;
    const bcma_cli_tokens_t *ctok;
    bool is_single_value = false;

    widx = 0;
    sal_memset(data, 0, sizeof(data));
    for (idx = 0; idx < ctoks_num && ctoks[idx].argc; idx++) {
        ctok = &ctoks[idx];
        buf_wcnt = COUNTOF(data) - widx;
        dptr = data + widx;
        wcnt = bcma_cli_parse_array_uint32(ctok->argv[0], buf_wcnt, dptr);
        if (wcnt > 0) {
            if (wcnt > 1) {
                /* The raw data concatenates 32-bit hex values together. */
                if (widx) {
                    return bcma_cli_parse_error("field", ctok->str);
                }
                is_single_value = 1;
            } else {
                /* The raw data is a list of 32-bit hex values. */
                if (is_single_value) {
                    return bcma_cli_parse_error("field", ctok->str);
                }
                widx++;
            }
        } else if (sal_strcasecmp(ctok->argv[0], "nz") == 0) {
            /* Set the flag to skip fields with a value of zero. */
            flags |= BCMDRD_SYM_DUMP_SKIP_ZEROS;
        } else {
            return bcma_cli_parse_error("field", ctok->str);
        }
    }

    fnames = symbols->field_names;
    bcmdrd_sym_dump_fields(pb, sinfo, fnames, data, flags,
                           bcmdrd_symbol_field_filter, data, NULL, NULL);
    cli_out("%s", shr_pb_str(pb));

    return BCMA_CLI_CMD_OK;
}

static int
symtbl_exec(bcma_cli_t *cli, bcma_cli_args_t *args,
            bcma_cli_tokens_t *ctoks, int ctoks_num, shr_pb_t *pb)
{
    int unit;
    const char *arg;
    bool do_decode = 0;
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_symbol_t *sinfo;

    if (args->argc < 2) {
        return BCMA_CLI_CMD_FAIL;
    }

    unit = cli->cmd_unit;
    if (!bcmdrd_dev_exists(unit)) {
        return BCMA_CLI_CMD_BAD_ARG;
    }

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (!symbols) {
        return BCMA_CLI_CMD_FAIL;
    }

    /* Get the operation type: decode or encode. */
    arg = BCMA_CLI_ARG_GET(args);
    if (bcma_cli_parse_cmp("Decode", arg, 0)) {
        do_decode = 1;
    } else if (bcma_cli_parse_cmp("Encode", arg, 0)) {
        do_decode = 0;
    } else {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Get the given symbol info. */
    arg = BCMA_CLI_ARG_GET(args);
    sinfo = bcmdrd_symbols_find(arg, symbols, NULL);
    if (!sinfo) {
        return bcma_cli_parse_error("symbol", arg);
    }

    /* Tokenize the remaining args. */
    if ((arg = bcma_cli_parse_tokens(args, ctoks, ctoks_num)) != NULL) {
        return bcma_cli_parse_error("", arg);
    }

    /* Execute encoding or decoding. */
    if (do_decode) {
        return symtbl_decode(symbols, sinfo, ctoks, ctoks_num, pb);
    } else {
        return symtbl_encode(symbols, sinfo, ctoks, ctoks_num, pb);
    }
}

#endif /* BCMDRD_CONFIG_INCLUDE_FIELD_INFO */

int
bcma_bcmdrdcmd_symtbl(bcma_cli_t *cli, bcma_cli_args_t *args)
{
#if BCMDRD_CONFIG_INCLUDE_FIELD_INFO == 0
    return BCMA_CLI_CMD_NO_SYM;
#else
    int rv = BCMA_CLI_CMD_FAIL;
    int ctoks_num = BCMA_CLI_CONFIG_ARGS_CNT_MAX;
    bcma_cli_tokens_t *ctoks;
    shr_pb_t *pb;

    ctoks = sal_alloc(ctoks_num * sizeof(bcma_cli_tokens_t),
                      "bcmaCliSymtblCtoks");
    pb = shr_pb_create();

    if (ctoks && pb) {
        rv = symtbl_exec(cli, args, ctoks, ctoks_num, pb);
    }

    if (ctoks) {
        sal_free(ctoks);
    }
    shr_pb_destroy(pb);

    return rv;
#endif /* BCMDRD_CONFIG_INCLUDE_FIELD_INFO */
}
