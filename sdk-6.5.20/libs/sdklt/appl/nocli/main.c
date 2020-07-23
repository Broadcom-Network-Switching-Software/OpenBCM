/*! \file main.c
 *
 * This example application demonstrates how the SDKLT can be
 * initialized using a minimal number of API calls.
 *
 * Upon initialization, the application will run until the the user
 * presses the ENTER key.
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

#include <bsl/bsl_ext.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmmgmt/bcmmgmt.h>

#include <bcma/sys/bcma_sys_probe.h>

#include "version.h"

/*******************************************************************************
 * Local variables
 */

/* System configuration structure */
static int log_level = BSL_SEV_WARN;

/*******************************************************************************
 * Private functions
 */

/* Simple output hook for core BSL configuration */
static int
bsl_out_hook(bsl_meta_t *meta, const char *format, va_list args)
{
    return vprintf(format, args);
}

/* Simple check hook for core BSL configuration */
static int
bsl_check_hook(bsl_packed_meta_t meta_pack)
{
    int source, severity;

    source = BSL_SOURCE_GET(meta_pack);
    severity = BSL_SEVERITY_GET(meta_pack);

    if (source == BSL_SRC_SHELL && severity <= BSL_SEV_INFO) {
        return 1;
    }
    if (severity <= log_level) {
        return 1;
    }
    return 0;
}

static int
sdk_init(const char *config_file)
{
    int rv;
    bsl_config_t bsl_config;

    /* Initialize system log output */
    bsl_config_t_init(&bsl_config);
    bsl_config.out_hook = bsl_out_hook;
    bsl_config.check_hook = bsl_check_hook;
    rv = bsl_init(&bsl_config);
    if (SHR_FAILURE(rv)) {
        printf("bsl_init failed (%d)\n", rv);
        return rv;
    }

    /* Probe for devices and initialize DRD */
    rv = bcma_sys_probe();
    printf("Found %d device%s\n", rv, (rv == 1) ? "" : "s");

    /* Default init sequence */
    rv = bcmmgmt_init(false, config_file);
    if (SHR_FAILURE(rv)) {
        printf("bcmmgmt_init failed (%d)\n", rv);
        return rv;
    }

    return 0;
}

static int
sdk_run(void)
{
    printf("\nSystem initialized and running.\nPress ENTER to exit...");
    fflush(stdout);
    getchar();
    return 0;
}

static int
sdk_cleanup(void)
{
    int rv;

    /* Shut down SDK */
    rv = bcmmgmt_shutdown(true);
    if (SHR_FAILURE(rv)) {
        printf("bcmmgmt_shutdown failed (%d)\n", rv);
        return rv;
    }

    /* Remove devices from DRD */
    bcma_sys_probe_cleanup();

    return 0;
}

/*******************************************************************************
 * Public functions
 */

int
main(int argc, char *argv[])
{
    const char *config_file = "config.yml";

    while (1) {
        int ch;

        ch = getopt(argc, argv, "hiy:");
        if (ch == -1) {
            /* No more options */
            break;
        }

        switch (ch) {
        case 'h':
            printf("Usage:\n");
            printf("%s [options]\n"
                   "-h          Show this help text.\n"
                   "-i          Show informational log messages.\n"
                   "-y <file>   Specify YAML configuration file.\n"
                   "\n", argv[0]);
            exit(-2);
            break;

        case 'i':
            log_level = BSL_SEV_INFO;
            break;

        case 'y':
            config_file = optarg;
            break;

        default:
            exit(-2);
            break;
        }
    }

    /* Initialize and display version string */
    version_init();
    version_signon();

    /* Initialize SDK */
    sdk_init(config_file);

    /* CLI main loop */
    sdk_run();

    /* Clean up SDK */
    sdk_cleanup();

    exit(0);
}
