/*! \file bcma_bcmbdcmd_cmicx_geti.c
 *
 * CLI 'geti' command for CMICx
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd_miim.h>

#include <bcma/bcmbd/bcma_bcmbd_symlog.h>
#include <bcma/bcmbd/bcma_bcmbd_cmicx.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicx_geti.h>

/*
 * Get a CMIC register.
 */
static int
geti_cmic(bcma_cli_args_t *args)
{
    int rv, unit = args->unit;
    const char *arg;
    uint32_t addr;
    uint32_t data;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL || bcma_cli_parse_uint32(arg, &addr) < 0) {
        return bcma_cli_parse_error("address", arg);
    }

    addr &= ~3;
    rv = BCMDRD_DEV_READ32(unit, addr, &data);

    /* Log the operation. */
    bcma_bcmbd_symlog_cmic_set(unit, addr, BCMDRD_SYMBOL_FLAG_MEMMAPPED,
                               BCMLT_PT_OPCODE_GET, rv);

    cli_out("cmic[0x%"PRIx32"] = 0x%"PRIx32"\n", addr, data);
    return 0;
}

/*
 * Get an iProc register.
 */
static int
geti_iproc(bcma_cli_args_t *args)
{
    int rv, unit = args->unit;
    const char *arg;
    uint32_t addr;
    uint32_t data;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL || bcma_cli_parse_uint32(arg, &addr) < 0) {
        return bcma_cli_parse_error("address", arg);
    }

    addr &= ~3;
    rv = bcmdrd_hal_iproc_read(unit, addr, &data);

    /* Log the operation. */
    bcma_bcmbd_symlog_cmic_set(unit, addr, BCMDRD_SYMBOL_FLAG_IPROC,
                               BCMLT_PT_OPCODE_GET, rv);

    cli_out("iproc[0x%"PRIx32"] = 0x%"PRIx32"\n", addr, data);
    return 0;
}

/*
 * Get a chip register via S-channel.
 */
static int
geti_reg(bcma_cli_args_t *args)
{
    int unit = args->unit;
    const char *arg;
    int size;
    bcma_bcmbd_id_t sid;

    /* Crack the identifier */
    arg = BCMA_CLI_ARG_GET(args);
    if (bcma_bcmbd_parse_id(arg, &sid, 1) < 0) {
        return bcma_cli_parse_error("address", arg);
    }

    /* Default size is 1 (32 bits) */
    size = 1;

    /* Optional second argument is the size of the register */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg && (bcma_cli_parse_int(arg, &size) < 0)) {
        return bcma_cli_parse_error("size", arg);
    }

    /* Only 32 and 64 bit supported for registers */
    if (size != 1 && size != 2) {
        return bcma_cli_parse_error("size", arg);
    }

    sal_sprintf(sid.addr.name, "0x%04"PRIx32"%08"PRIx32"",
                sid.addr.ext32, sid.addr.name32);

    /* Output all matching registers */
    return bcma_bcmbd_cmicx_regops(unit, NULL, BCMDRD_INVALID_ID,
                                   &sid, size, NULL, NULL);
}

/*
 * Get a chip memory via S-channel.
 */
static int
geti_mem(bcma_cli_args_t *args)
{
    int unit = args->unit;
    const char *arg;
    int size;
    bcma_bcmbd_id_t sid;

    /* Crack the identifier */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL || bcma_bcmbd_parse_id(arg, &sid, 1) < 0) {
        return bcma_cli_parse_error("address", arg);
    }

    /* Default size is 1 (32 bits) */
    size = 1;

    /* Optional second argument is the memory size in words */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg && (bcma_cli_parse_int(arg, &size) < 0)) {
        return bcma_cli_parse_error("size", arg);
    }

    /*
     * Memory specifications can come in a couple of formats:
     *
     * MEM
     * MEM[i0,i1]
     * MEM.blockN[i0, i1]
     * MEM.block[b0, b1].[i0,i1]
     */
    return bcma_bcmbd_cmicx_memops(unit, NULL, BCMDRD_INVALID_ID,
                                   &sid, size, NULL, NULL);
}

/*
 * Get PHY register via MIIM/MDIO bus.
 */
static int
geti_miim(bcma_cli_args_t *args)
{
    int unit = args->unit;
    const char *arg;
    uint32_t regaddr, data;
    bcma_bcmbd_id_t sid;
    int i, devad;

    /* Crack the phy_id and addresses */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL || (bcma_bcmbd_parse_id(arg, &sid, 1) < 0) ||
        sid.addr.start >= 0) {
        return bcma_cli_parse_error("miim addr", arg);
    }

    if (sid.port.start < 0 && sid.port.end < 0) {
        sid.port.start = 0;
        sid.port.end = 0x1f;
    } else if (sid.port.end < 0) {
        sid.port.end = sid.port.start;
    }

    /* If present, treat block number as clause 45 devad */
    devad = 0;
    if (sid.block.start >= 0) {
        devad = sid.block.start;
    }

    for (i = sid.port.start; i <= sid.port.end; i++) {
        regaddr = i + (0x10000 * devad);
        if (bcmbd_miim_read(unit, sid.addr.name32, regaddr, &data) < 0) {
            cli_out("%s reading miim(0x%"PRIx32")[0x%x]\n",
                    BCMA_CLI_CONFIG_ERROR_STR, sid.addr.name32, i);
        } else {
            cli_out("miim(0x%"PRIx32")[0x%"PRIx32"] = 0x%"PRIx32"\n",
                    sid.addr.name32, regaddr, data);
        }
    }

    return 0;
}

static bcma_bcmbd_vect_t geti_vects[] =
{
    { "cmic",   geti_cmic,     },
    { "iproc",  geti_iproc,    },
    { "reg",    geti_reg,      },
    { "mem",    geti_mem,      },
    { "miim",   geti_miim,     },
    { 0, 0 },
};

int
bcma_bcmbdcmd_cmicx_geti(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit;

    unit = cli->cmd_unit;
    if (!bcmdrd_dev_exists(unit)) {
        return BCMA_CLI_CMD_BAD_ARG;
    }
    if (bcma_bcmbd_parse_vect(args, geti_vects, NULL) < 0) {
        bcma_cli_parse_error("type", BCMA_CLI_ARG_CUR(args));
    }
    return 0;
}
