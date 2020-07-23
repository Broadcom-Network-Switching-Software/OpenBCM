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
#include <bcma/bcmpc/bcma_bcmpccmd.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicd.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicx.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_dev.h>
#include <bcma/bcmpc/bcma_bcmpccmd.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd.h>
#include <bcma/cint/bcma_cint_cmd.h>
#include <bcma/ha/bcma_ha.h>
#include <bcma/ha/bcma_hacmd.h>
#include <bcma/sys/bcma_sys_conf_sdk.h>
#include <bcma/fwm/bcma_fwmcmd.h>

#include "version.h"

/*******************************************************************************
 * Local variables
 */

/* System configuration structure */
static bcma_sys_conf_t sys_conf, *isc;

/*******************************************************************************
 * Private CLI commands
 */

static int
drd_init(bcma_sys_conf_t *sc);

static int
clicmd_probe(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    drd_init(isc);

    return BCMA_CLI_CMD_OK;
}

static bcma_cli_command_t shcmd_probe = {
    .name = "probe",
    .func = clicmd_probe,
    .desc = "Probe for devices.",
};

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
    bcma_bslcmd_add_cmds(sc->dsh);

    /* Add CLI commands for tables operation */
    bcma_bcmltcmd_add_cmds(sc->cli);
    /* Add CLI commands for tables application */
    bcma_bcmltcmd_add_appl_cmds(sc->cli);

    /* Add support for CLI command completion */
    bcma_sys_conf_clirlc_init();

    /* Add local probe command to debug shell */
    bcma_cli_add_command(sc->dsh, &shcmd_probe, 0);

    /* Add base driver commands to debug shell */
    bcma_bcmbdcmd_add_cmicd_cmds(sc->dsh);
    bcma_bcmbdcmd_add_cmicx_cmds(sc->dsh);
    bcma_bcmbdcmd_add_dev_cmds(sc->dsh);

    /* Add Port Control commands to debug shell */
    bcma_bcmpccmd_add_phy_cmds(sc->dsh);
    /* Add CLI commands for Port Control */
    bcma_bcmpccmd_add_cmds(sc->cli);

    /* Add CLI commands for packet I/O driver */
    bcma_bcmpktcmd_add_cmds(sc->cli);

    /* Add HA commands to debug shell */
    bcma_hacmd_add_cmds(sc->dsh);

    /* Add Firmware Management commands to debug shell */
    bcma_fwmcmd_add_cmds(sc->dsh);

    /* Add CLI commands for BCMLT C interpreter (CINT) */
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
ha_init(bcma_sys_conf_t *sc, bool warm_boot, int ha_instance)
{
    int rv;
    int unit;

    rv = bcma_ha_mem_init(-1, 0, warm_boot, ha_instance);
    if (SHR_FAILURE(rv)) {
        printf ("Failed to create generic HA memory\n");
        return rv;
    }

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (!bcmdrd_dev_exists(unit)) {
            continue;
        }
        rv = bcma_ha_mem_init(unit, 0, warm_boot, ha_instance);
        if (SHR_FAILURE(rv)) {
            printf("Failed to create HA memory for unit %d (%d)\n", unit, rv);
        }
        if (warm_boot) {
            char buf[80];
            rv = bcma_ha_mem_name_get(unit, sizeof(buf), buf);
            if (SHR_FAILURE(rv)) {
                printf("Failed to get HA file for unit %d (%d)\n", unit, rv);
            } else {
                printf("Warm boot - using HA file %s for unit %d\n",
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
            rv = bcma_ha_mem_name_get(unit, sizeof(buf), buf);
            if (SHR_FAILURE(rv)) {
                printf("Failed to get HA file for unit %d (%d)\n", unit, rv);
            } else {
                printf("Warm exit - keeping HA file %s for unit %d\n",
                       buf, unit);
            }
        }
        bcma_ha_mem_cleanup(unit, keep_ha_file);
    }

    bcma_ha_mem_cleanup(-1, keep_ha_file);

    return SHR_E_NONE;
}

static int
sdk_init(const char *config_file, bool warm_boot,
         bcmmgmt_issu_info_t *issu_info, bool debug_shell,
         int ha_instance)
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
        printf("bcma_bslmgmt_init failed (%s)\n", shr_errmsg(rv));
        return rv;
    }

    /* Initialize CLI */
    rv = cli_init(isc);
    if (SHR_FAILURE(rv)) {
        printf("cli_init failed (%s)\n", shr_errmsg(rv));
        return rv;
    }

    /* Initialize test component */
    if (bcma_sys_conf_test_init(isc) < 0) {
        return SHR_E_FAIL;
    }

    if (debug_shell) {
        /* Debug shell main loop */
        bcma_cli_cmd_loop(isc->dsh);
    }

    /* Initialize DRD */
    rv = drd_init(isc);
    if (SHR_FAILURE(rv)) {
        printf("drd_init failed (%s)\n", shr_errmsg(rv));
        return rv;
    }

    /* Initialize HA */
    rv = ha_init(isc, warm_boot, ha_instance);
    if (SHR_FAILURE(rv)) {
        printf("ha_init failed (%s)\n", shr_errmsg(rv));
        return rv;
    }

    /* ISSU upgrade start */
    rv = bcmmgmt_issu_start(warm_boot, issu_info);
    if (SHR_FAILURE(rv)) {
        printf("bcmmgmt_issu_start failed (%s)\n", shr_errmsg(rv));
        return rv;
    }

    /* Default init sequence */
    rv = bcmmgmt_init(warm_boot, warm_boot ? NULL : config_file);
    if (SHR_FAILURE(rv)) {
        printf("bcmmgmt_init failed (%s)\n", shr_errmsg(rv));
        bcmmgmt_issu_done();
        return rv;
    }

    /* ISSU upgrade done */
    rv = bcmmgmt_issu_done();
    if (SHR_FAILURE(rv)) {
        printf("bcmmgmt_issu_done failed (%s)\n", shr_errmsg(rv));
        return rv;
    }

    /* Auto-run CLI script */
    bcma_sys_conf_rcload_run(isc);

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
        printf("bcmmgmt_shutdown failed (%s)\n", shr_errmsg(rv));
        return rv;
    }

    /* Clean up HA file */
    rv = ha_cleanup(isc, keep_ha_file);
    if (SHR_FAILURE(rv)) {
        printf("ha_cleanup failed (%s)\n", shr_errmsg(rv));
        return rv;
    }

    /* Remove devices from DRD */
    bcma_sys_conf_drd_cleanup(isc);

    /* Cleanup test component */
    bcma_sys_conf_test_cleanup(isc);

    /* Clean up CLI */
    bcma_sys_conf_cli_cleanup(isc);

    /* Clean up system log */
    rv = bcma_bslmgmt_cleanup();
    if (SHR_FAILURE(rv)) {
        printf("bcma_bslmgmt_cleanup failed (%s)\n", shr_errmsg(rv));
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
    bcmmgmt_issu_info_t issu_info, *issu_info_p = NULL;
    bool debug_shell = false;
    bool keep_ha_file = false;
    bool warm_boot = false;
    int ha_instance = -1;

    issu_info.start_ver = NULL;
    issu_info.current_ver = NULL;

    while (1) {
        int ch;

        ch = getopt(argc, argv, "dhkl:s:uv:wy:g:");
        if (ch == -1) {
            /* No more options */
            break;
        }

        switch (ch) {
        case 'd':
            debug_shell = true;
            break;

        case 'g':
            ha_instance = sal_atoi(optarg);
            break;
        case 'h':
            printf("Usage:\n");
            printf("%s [options]\n"
                   "-d          Enter debug shell before starting SDK.\n"
                   "-g <inst>   Specify HA file specific instance.\n"
                   "-h          Show this help text.\n"
                   "-k          Keep high-availability file.\n"
                   "-w          Run in warm-boot mode.\n"
                   "-u          Enable version upgrade in warm-boot mode.\n"
                   "-s <ver>    Specify start version for upgrade.\n"
                   "-v <ver>    Specify target version for upgrade.\n"
                   "-y <file>   Specify YAML configuration file.\n"
                   "-l <file>   Specify CLI auto-run script file.\n"
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

        case 'u':
            issu_info_p = &issu_info;
            break;

        case 's':
            issu_info.start_ver = optarg;
            break;

        case 'v':
            issu_info.current_ver = optarg;
            break;

        case 'l':
            bcma_sys_conf_rcload_set(optarg);
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
    sdk_init(config_file, warm_boot, issu_info_p, debug_shell, ha_instance);

    /* CLI main loop */
    sdk_run();

    /* Clean up SDK */
    sdk_cleanup(keep_ha_file);

    exit(0);
}
