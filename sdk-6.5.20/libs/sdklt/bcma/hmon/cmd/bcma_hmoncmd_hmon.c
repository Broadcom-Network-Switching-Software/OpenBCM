/*! \file bcma_hmoncmd_hmon.c
 *
 *  CLI command for hmon command user interface.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcmbd/bcmbd_pvt.h>
#include <bcma/hmon/bcma_hmoncmd_hmon.h>
#include <bcma/io/bcma_io_file.h>
#include <bcmbd/bcmbd.h>
#include <shr/shr_pb.h>

#define BSL_LOG_MODULE  BSL_LS_APPL_TEST

#define TOTAL_HMON_ARGUMENT 5

/*!
 * \brief CLI 'hmon' command for debugging.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
static int
bcma_hmoncmd_hmon_temp_get(int unit, uint32_t equation)
{
    size_t count;
    size_t sensor_count;
    size_t ix;
    bcmbd_pvt_temp_rec_t *rptr = NULL;
    bcmbd_pvt_temp_rec_t *uptr = NULL;
    bcmbd_pvt_temp_rec_t *uptr_sec = NULL;
    int rv;

    /* Call bcmbd api to do actual work */
    count = 0;
    rv = bcmbd_pvt_temperature_get(unit, count, equation, rptr, &sensor_count);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    cli_out("\n%d temperature sensor detected. All values in Degree C.\n\n",
            (int) sensor_count);
    count = sensor_count;

    if (count != 0) {
        /* Base pointer for reference and free */
        if (equation == PVT_EQUATION_1) {
            rptr =  sal_alloc(sizeof(bcmbd_pvt_temp_rec_t) * count,
                              "bcmaHmonTemp");
            if (rptr == NULL) {
                return SHR_E_MEMORY;
            }

            sal_memset(rptr, 0, sizeof(bcmbd_pvt_temp_rec_t) * count);
            rv = bcmbd_pvt_temperature_get(unit, count, equation,
                                           rptr, &sensor_count);
            if (SHR_FAILURE(rv)) {
                if (rptr) {
                    sal_free(rptr);
                }

                return rv;
            }

            /* Utility pointer for iterating record */
            uptr = rptr;
            cli_out("Sensor ID\tCurrent\t\tHist Min\tHist Max\t\n");
            for (ix = 0; ix < count; ix++) {
                cli_out("%d\t\t%.1f\t\t%.1f\t\t%.1f\n",
                        (int) ix, uptr->current, uptr->min, uptr->max);
                uptr++;
            }

            if (rptr) {
                sal_free(rptr);
            }
        } else if (equation == PVT_EQUATION_2) {
            /* Needs 2 buffer for both linear and quadratic equations. */
            rptr =  sal_alloc(sizeof(bcmbd_pvt_temp_rec_t) * count * 2,
                              "bcmaHmonTemp");
            if (rptr == NULL) {
                return SHR_E_MEMORY;
            }

            sal_memset(rptr, 0, sizeof(bcmbd_pvt_temp_rec_t) * count);
            rv = bcmbd_pvt_temperature_get(unit, count, equation,
                                           rptr, &sensor_count);
            if (SHR_FAILURE(rv)) {
                if (rptr) {
                    sal_free(rptr);
                }
                return rv;
            }

            /* Utility pointer for iterating record */
            uptr = rptr;
            uptr_sec = rptr + count;
            cli_out("Sensor ID\tCurrent\t\tHist Min\tHist Max\tEquation\n");
            for (ix = 0; ix < count; ix++) {
                cli_out("%d\t\t%.1f\t\t%.1f\t\t%.1f\t\t%s\n",
                        (int) ix, uptr->current, uptr->min, uptr->max, "Linear");
                cli_out("\t\t%.1f\t\t%.1f\t\t%.1f\t\t%s\n",
                        uptr_sec->current, uptr_sec->min, uptr_sec->max, "Quadratic");
                uptr++;
                uptr_sec++;
            }

            if (rptr) {
                sal_free(rptr);
            }
        }
    }

    return SHR_E_NONE;
}

/*!
 * \brief CLI 'hmon' command for debugging.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
int
bcma_hmoncmd_hmon(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit;
    char *cmd;
    char *cmd_opt;
    bool detail = FALSE;

    /* Variables initialization */
    unit = cli->cmd_unit;

    /* Get first argument. */
    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Try to get second argument. */
    if ((cmd_opt = BCMA_CLI_ARG_GET(args))) {
        if (bcma_cli_parse_cmp("detail", cmd_opt, 0)) {
            detail = TRUE;
        }
    }

    if (bcma_cli_parse_cmp("temperature", cmd, 0)) {
        /* Call bcmbd api to do actual work */
        if (detail) {
            bcma_hmoncmd_hmon_temp_get(unit, PVT_EQUATION_2);
        } else {
            bcma_hmoncmd_hmon_temp_get(unit, PVT_EQUATION_1);
        }
    } else {
        return BCMA_CLI_CMD_USAGE;
    }

    return BCMA_CLI_CMD_OK;
}
