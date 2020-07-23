/*! \file bcma_bcmbdcmd_cmicd_seti.c
 *
 * CLI 'seti' command for CMICd
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
#include <bcmbd/bcmbd_miim.h>

#include <bcma/bcmbd/bcma_bcmbd_cmicd.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicd_seti.h>

static int
parse_multiword(const char *str, bcma_cli_tokens_t *ctok,
                uint32_t *words, int max_words)
{
    int idx;

    /* Parse string into word tokens */
    if (bcma_cli_tokenize(str, ctok, ":") < 0) {
        return -1;
    }

    /* Check array size */
    if (ctok->argc > max_words) {
        return -1;
    }

    /* Convert all tokens to integers */
    for (idx = 0; idx < ctok->argc; idx++) {
        /* This argument must be an integer */
        if (bcma_cli_parse_uint32(ctok->argv[idx], &words[idx]) < 0) {
            return -1;
        }
    }

    return 0;
}

/*
 * Set a CMIC register.
 */
static int
seti_cmic(bcma_cli_args_t *args)
{
    int unit = args->unit;
    const char *arg;
    uint32_t addr;
    uint32_t data;

    arg = BCMA_CLI_ARG_GET(args);
    if (bcma_cli_parse_uint32(arg, &addr) < 0) {
        return bcma_cli_parse_error("address", arg);
    }
    arg = BCMA_CLI_ARG_GET(args);
    if (bcma_cli_parse_uint32(arg, &data) < 0) {
        return bcma_cli_parse_error("data", arg);
    }

    addr &= ~3;
    BCMDRD_DEV_WRITE32(unit, addr, data);

    return 0;
}

/*
 * Set an iProc register
 */
static int
seti_iproc(bcma_cli_args_t *args)
{
    int unit = args->unit;
    const char *arg;
    uint32_t addr;
    uint32_t data;

    arg = BCMA_CLI_ARG_GET(args);
    if (bcma_cli_parse_uint32(arg, &addr) < 0) {
        return bcma_cli_parse_error("address", arg);
    }
    arg = BCMA_CLI_ARG_GET(args);
    if (bcma_cli_parse_uint32(arg, &data) < 0) {
        return bcma_cli_parse_error("data", arg);
    }

    addr &= ~3;
    bcmdrd_hal_iproc_write(unit, addr, data);

    return 0;
}

/*
 * Set a chip register via S-channel.
 */
static int
do_seti_reg(bcma_cli_args_t *args, bcma_cli_tokens_t *ctok)
{
    int unit = args->unit;
    const char *arg;
    uint32_t and_masks[2];
    uint32_t or_masks[2];
    bcma_bcmbd_id_t sid;
    int size;

    /* Register will be cleared */
    and_masks[0] = and_masks[1] = 0;

    /* Crack the identifier */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL || bcma_bcmbd_parse_id(arg, &sid, 1) < 0) {
        return bcma_cli_parse_error("address", arg);
    }

    /* 32 or 64 bit multiword value */
    arg = BCMA_CLI_ARG_GET(args);
    or_masks[0] = or_masks[1] = 0;
    if (arg == NULL ||
        parse_multiword(arg, ctok, or_masks, COUNTOF(or_masks)) < 0) {
        return bcma_cli_parse_error("data", arg);
    }

    /* Default size is the number of words specified */
    size = ctok->argc;

    /* Optional third argument is the size of the register */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg && bcma_cli_parse_int(arg, &size) < 0) {
        return bcma_cli_parse_error("size", arg);
    }

    return bcma_bcmbd_cmicd_regops(unit, NULL, BCMDRD_INVALID_ID,
                                   &sid, size, and_masks, or_masks);
}

/*
 * Set a chip register via S-channel.
 */
static int
seti_reg(bcma_cli_args_t *args)
{
    int rv;
    bcma_cli_tokens_t *ctok;

    ctok = sal_alloc(sizeof(bcma_cli_tokens_t), "bcmaCliSetiCtoks");
    if (ctok == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    rv = do_seti_reg(args, ctok);

    sal_free(ctok);

    return rv;
}

/*
 * Set a chip memory via S-channel.
 */
static int
do_seti_mem(bcma_cli_args_t *args, bcma_cli_tokens_t *ctok)
{
    int unit = args->unit;
    const char *arg;
    int size;
    bcma_bcmbd_id_t sid;
    uint32_t and_masks[8];
    uint32_t or_masks[8];

    /* Memory will be cleared */
    sal_memset(and_masks, 0, sizeof(and_masks));

    /* Crack the identifier */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL || bcma_bcmbd_parse_id(arg, &sid, 1) < 0) {
        return bcma_cli_parse_error("address", arg);
    }

    /* Second argument is the data */
    arg = BCMA_CLI_ARG_GET(args);
    sal_memset(or_masks, 0, sizeof(or_masks));
    if (arg == NULL ||
        parse_multiword(arg, ctok, or_masks, COUNTOF(or_masks)) < 0) {
        return bcma_cli_parse_error("data", arg);
    }

    /* Default size is the number of words specified */
    size = ctok->argc;

    /*
     * Optional third argument is the the memory size in words
     * Any words NOT specified in the data will be written as
     * zero (up to the size.)
     */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg && (bcma_cli_parse_int(arg, &size) < 0)) {
        return bcma_cli_parse_error("size", arg);
    }

    return bcma_bcmbd_cmicd_memops(unit, NULL, BCMDRD_INVALID_ID,
                                   &sid, size, and_masks, or_masks);
}

/*
 * Set a chip memory via S-channel.
 */
static int
seti_mem(bcma_cli_args_t *args)
{
    int rv;
    bcma_cli_tokens_t *ctok;

    ctok = sal_alloc(sizeof(bcma_cli_tokens_t), "bcmaCliSetiCtoks");
    if (ctok == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    rv = do_seti_mem(args, ctok);

    sal_free(ctok);

    return rv;
}

/*
 * Set PHY register via MIIM/MDIO bus.
 */
static int
seti_miim(bcma_cli_args_t *args)
{
    int unit = args->unit;
    const char *arg;
    uint32_t data;
    bcma_bcmbd_id_t sid;
    int i;

    /* Crack the phy_id and addresses */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL ||
        bcma_bcmbd_parse_id(arg, &sid, 1) < 0 || sid.addr.start >= 0) {
        return bcma_cli_parse_error("miim addr", arg);
    }

    if (sid.port.start < 0 && sid.port.end < 0) {
        sid.port.start = 0;
        sid.port.end = 0x1f;
    } else if (sid.port.end < 0) {
        sid.port.end = sid.port.start;
    }

    /* Get the data */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL || bcma_cli_parse_uint32(arg, &data) < 0) {
        return bcma_cli_parse_error("miim data", arg);
    }

    for (i = sid.port.start; i <= sid.port.end; i++) {
        if (bcmbd_miim_write(unit, sid.addr.name32, i, data) < 0) {
            cli_out("%s writing miim(0x%"PRIx32")[0x%x]\n",
                    BCMA_CLI_CONFIG_ERROR_STR, sid.addr.name32, i);
        }
    }

    return 0;
}

static bcma_bcmbd_vect_t seti_vects[] =
{
    { "cmic",   seti_cmic,     },
    { "iproc",  seti_iproc,    },
    { "reg",    seti_reg,      },
    { "mem",    seti_mem,      },
    { "miim",   seti_miim,     },
    { 0, 0 },
};

int
bcma_bcmbdcmd_cmicd_seti(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit;

    unit = cli->cmd_unit;
    if (!bcmdrd_dev_exists(unit)) {
        return BCMA_CLI_CMD_BAD_ARG;
    }
    if (bcma_bcmbd_parse_vect(args, seti_vects, NULL) < 0) {
        bcma_cli_parse_error("type", BCMA_CLI_ARG_CUR(args));
    }
    return 0;
}
