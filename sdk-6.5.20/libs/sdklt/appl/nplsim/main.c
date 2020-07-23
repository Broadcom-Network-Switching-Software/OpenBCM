/*! \file main.c
 *
 * This example application demonstrates how the SDKLT can be
 * initialized using the application helper components supplied in the
 * $SDK/bcma directory.
 *
 * Upon initialization, the application will launch a command line
 * interface (CLI) from which the user can exercise the logical table
 * (LT) API for switch device management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include <shr/shr_debug.h>
#include <shr/shr_pb.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/bcmbd.h>
#include <bcmmgmt/bcmmgmt.h>
#include <bcmmgmt/bcmmgmt_sysm_default.h>
#include <bcmlt/bcmlt.h>

#include <bcma/bsl/bcma_bslmgmt.h>
#include <bcma/bsl/bcma_bslcmd.h>
#include <bcma/bcmlt/bcma_bcmltcmd.h>
#include <bcma/cint/bcma_cint_cmd.h>
#include <bcma/ha/bcma_ha.h>
#include <bcma/sys/bcma_sys_conf_sdk.h>

#include "version.h"

/*******************************************************************************
 * Local variables
 */

/* System configuration structure */
static bcma_sys_conf_t sys_conf, *isc;

/*******************************************************************************
 * Private functions
 */

static int
cli_init(bcma_sys_conf_t *sc)
{
    /* Use default CLI prompt prefix function */
    if (bcma_sys_conf_cli_prompt_prefix_default_set(sc) < 0) {
        return SHR_E_FAIL;
    }

    /* Initialize basic CLI */
    if (bcma_sys_conf_cli_init(sc) < 0) {
        return SHR_E_FAIL;
    }

    /* Enable CLI redirection in BSL output hook */
    bcma_bslmgmt_redir_hook_set(bcma_sys_conf_cli_redir_bsl);

    /* Add CLI commands for controlling the system log */
    bcma_bslcmd_add_cmds(sc->cli);

    /* Add CLI commands for tables operation */
    bcma_bcmltcmd_add_cmds(sc->cli);

    /* Add support for CLI command completion */
    bcma_sys_conf_clirlc_init();

    /* Add support for CINT shell */
    bcma_cintcmd_add_cint_cmd(sc->cli);

    return SHR_E_NONE;
}

static int
drd_init(bcma_sys_conf_t *sc)
{
    int ndev;
    int unit;
    shr_pb_t *pb;

    /* Probe/create devices */
    if ((ndev = bcma_sys_conf_drd_init(sc)) < 0) {
        return SHR_E_FAIL;
    }

    /* Log probe info */
    pb = shr_pb_create();
    shr_pb_printf(pb, "Found %d device%s.\n", ndev, (ndev == 1) ? "" : "s");
    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (bcmdrd_dev_exists(unit)) {
            shr_pb_printf(pb, "Unit %d: %s\n", unit, bcmdrd_dev_name(unit));
        }
    }
    LOG_INFO(BSL_LS_APPL_SHELL,
             (BSL_META("%s"), shr_pb_str(pb)));
    shr_pb_destroy(pb);

    return SHR_E_NONE;
}

static int
ha_init(bcma_sys_conf_t *sc, bool warm_boot)
{
    int rv;
    int unit;

    rv = bcma_ha_gen_open(DEFAULT_HA_GEN_FILE_SIZE, true, warm_boot);
    if (SHR_FAILURE(rv)) {
        printf ("Failed to create generic HA memory\n");
        return rv;
    }

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (!bcmdrd_dev_exists(unit)) {
            continue;
        }
        rv = bcma_ha_unit_open(unit, DEFAULT_HA_FILE_SIZE, true, warm_boot);
        if (SHR_FAILURE(rv)) {
            printf("Failed to create HA memory for unit %d (%d)\n", unit, rv);
        }
        if (warm_boot) {
            char buf[80];
            rv = bcma_ha_file_name_get(unit, buf, sizeof(buf));
            if (SHR_FAILURE(rv)) {
                printf("Failed to get HA file for unit %d (%d)\n", unit, rv);
            } else {
                printf("Warm boot - Using HA file %s for unit %d\n",
                       buf, unit);
            }
        }
    }
    return SHR_E_NONE;
}

static int
ha_cleanup(bcma_sys_conf_t *sc, bool keep_ha_file)
{
    int rv;
    int unit;

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (!bcmdrd_dev_exists(unit)) {
            continue;
        }
        if (keep_ha_file) {
            char buf[80];
            rv = bcma_ha_file_name_get(unit, buf, sizeof(buf));
            if (SHR_FAILURE(rv)) {
                printf("Failed to get HA file for unit %d (%d)\n", unit, rv);
            } else {
                printf("Warm exit - Keeping HA file %s for unit %d\n",
                       buf, unit);
            }
        }
        bcma_ha_unit_close(unit, keep_ha_file);
    }

    bcma_ha_gen_close(keep_ha_file);

    return SHR_E_NONE;
}

static int
sdk_init(const char *config_file, bool warm_boot)
{
    int rv;

    /* Initialize system configuration structure */
    if (!isc) {
        isc = &sys_conf;
        bcma_sys_conf_init(isc);
    }

    /* Initialize system log output */
    rv = bcma_bslmgmt_init();
    if (SHR_FAILURE(rv)) {
        printf("bcma_bslmgmt_init failed (%d)\n", rv);
        return rv;
    }

    /* Initialize CLI */
    rv = cli_init(isc);
    if (SHR_FAILURE(rv)) {
        printf("cli_init failed (%d)\n", rv);
        return rv;
    }

    /* Initialize DRD */
    rv = drd_init(isc);
    if (SHR_FAILURE(rv)) {
        printf("drd_init failed (%d)\n", rv);
        return rv;
    }

    /* Initialize HA */
    rv = ha_init(isc, warm_boot);
    if (SHR_FAILURE(rv)) {
        printf("ha_init failed (%d)\n", rv);
        return rv;
    }

    /* Default init sequence */
    rv = bcmmgmt_init(warm_boot, warm_boot ? NULL : config_file);
    if (SHR_FAILURE(rv)) {
        printf("bcmmgmt_init failed (%d)\n", rv);
        return rv;
    }

    return 0;
}

static int
sdk_run(void)
{
    /* Check for valid sys_conf structure */
    if (isc == NULL) {
        return -1;
    }

    /* CLI main loop */
    bcma_cli_cmd_loop(isc->cli);

    return 0;
}

static int
sdk_cleanup(bool keep_ha_file)
{
    int rv;

    /* Check for valid sys_conf structure */
    if (isc == NULL) {
        return -1;
    }

    /* Shut down SDK */
    rv = bcmmgmt_shutdown(true);
    if (SHR_FAILURE(rv)) {
        printf("bcmmgmt_shutdown failed (%d)\n", rv);
        return rv;
    }

    /* Clean up HA file */
    rv = ha_cleanup(isc, keep_ha_file);
    if (SHR_FAILURE(rv)) {
        printf("ha_cleanup failed (%d)\n", rv);
        return rv;
    }

    /* Remove devices from DRD */
    bcma_sys_conf_drd_cleanup(isc);

    /* Clean up CLI */
    bcma_sys_conf_cli_cleanup(isc);

    /* Clean up system log */
    rv = bcma_bslmgmt_cleanup();
    if (SHR_FAILURE(rv)) {
        printf("bcma_bslmgmt_cleanup failed (%d)\n", rv);
        return rv;
    }

    /* Release system configuration structure */
    bcma_sys_conf_cleanup(isc);

    return 0;
}

/*******************************************************************************
 * Public functions
 */

int
main(int argc, char *argv[])
{
    const char *config_file = "config.yml";
    bool keep_ha_file = false;
    bool warm_boot = false;

    /* Always connect to the model if not otherwise requested. */
    if (setenv("SDKLT_BM_SOCK", "9090", 0) < 0) {
        printf("Error setting SDKLT_BM_SOCK.\n");
        exit(-3);
    }

    while (1) {
        int ch;

        ch = getopt(argc, argv, "dhkwy:");
        if (ch == -1) {
            /* No more options */
            break;
        }

        switch (ch) {
        case 'h':
            printf("Usage:\n");
            printf("%s [options]\n"
                   "-h          Show this help text.\n"
                   "-k          Keep high-availability file.\n"
                   "-w          Run in warm-boot mode.\n"
                   "-y <file>   Specify YAML configuration file.\n"
                   "\n", argv[0]);
            exit(-2);
            break;

        case 'k':
            keep_ha_file = true;
            break;

        case 'w':
            warm_boot = true;
            break;

        case 'y':
            config_file = optarg;
            break;

        default:
            exit(-2);
            break;
        }
    }

    /* Extra arguments may indicate a syntax error as well */
    if (optind < argc) {
        printf("Unexpected argument: %s\n", argv[optind]);
        exit(-2);
    }

    /* Initialize and display version string */
    version_init();
    version_signon();

    /* Initialize SDK */
    sdk_init(config_file, warm_boot);

    /* CLI main loop */
    sdk_run();

    /* Clean up SDK */
    sdk_cleanup(keep_ha_file);

    exit(0);
}
