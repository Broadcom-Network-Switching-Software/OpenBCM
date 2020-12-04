/*! \file bcma_bcmbdcmd_cmicd_schan.c
 *
 * BCMBD CLI 'schan' command for CMICd
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_error.h>

#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmbd/bcmbd_cmicd.h>

#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/bcmbd/bcma_bcmbdcmd_cmicd_schan.h>

int
bcma_bcmbdcmd_cmicd_schan(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit;
    int rv;
    const char *arg;
    uint32_t data;
    uint32_t *schan_msg;
    int dwc_max = 0, dwc_write = 0;
    int bsize, idx;

    unit = cli->cmd_unit;
    if (!bcmdrd_dev_exists(unit)) {
        return BCMA_CLI_CMD_BAD_ARG;
    }

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Get the maximum buffer size required in the S-Channel operation. */
    rv = bcmbd_cmicd_schan_raw_op(unit, NULL, 0, 0, &dwc_max);
    if (SHR_FAILURE(rv)) {
        return BCMA_CLI_CMD_FAIL;
    }

    bsize = dwc_max * sizeof(uint32_t);
    schan_msg = sal_alloc(bsize, "bcmaCliSchanMsg");
    if (schan_msg == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }
    sal_memset(schan_msg, 0, bsize);

    while (arg && dwc_write < dwc_max) {
        if (bcma_cli_parse_uint32(arg, &data) < 0) {
            sal_free(schan_msg);
            return BCMA_CLI_CMD_BAD_ARG;
        }
        schan_msg[dwc_write++] = data;
        arg = BCMA_CLI_ARG_GET(args);
    }

    /* Successfully read all raw data to write. */
    rv = bcmbd_cmicd_schan_raw_op(unit, schan_msg, dwc_max, dwc_max, NULL);
    if (SHR_FAILURE(rv)) {
        cli_out("%sS-Channel operation failed: %s (%d)\n",
                BCMA_CLI_CONFIG_ERROR_STR, shr_errmsg(rv), rv);
    } else {
        for (idx = 0; idx < dwc_max; idx++) {
            cli_out("    0x%08"PRIx32, schan_msg[idx]);
            if (idx == (dwc_max - 1) || (idx & 3) == 3) {
                cli_out("\n");
            }
        }
    }

    sal_free(schan_msg);

    return SHR_SUCCESS(rv) ? BCMA_CLI_CMD_OK : BCMA_CLI_CMD_FAIL;
}
