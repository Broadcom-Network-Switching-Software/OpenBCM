/*! \file bcma_bcmmgmt.c
 *
 * Application for MGMT component.
 *
 * These APIs initialize the SDK using a custom init sequence for the
 * purpose of supporting multiple configuration files.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <shr/shr_debug.h>

#include <bcmmgmt/bcmmgmt_sysm_default.h>

#include <bcma/io/bcma_io_file.h>
#include <bcma/bcmmgmt/bcma_bcmmgmt.h>

/*******************************************************************************
 * Local definitions
 */

/* Maximum length of string parameters from the command line */
#define MAX_YML_FILE_LEN        255

/* Get maximum value from public define */
#define MAX_NUM_CONFIG_FILES    BCMA_BCMMGMT_MAX_NUM_CONFIG_FILES

/*******************************************************************************
 * Local data
 */

/* YAML configuration files */
static char cfgyml_file[MAX_NUM_CONFIG_FILES][MAX_YML_FILE_LEN + 1];

/* Track if configuration files have been loaded */
static bool config_loaded;

/*******************************************************************************
 * Public functions
 */

int
bcma_bcmmgmt_cfgyml_file_set(const char *cfgfile, int cfgfile_idx)
{
    int idx;
    bcma_io_file_handle_t fh;

    if (cfgfile_idx < 0) {
        if (cfgfile == NULL) {
            /* Uninstall all files */
            sal_memset(cfgyml_file, 0, sizeof(cfgyml_file));
            return SHR_E_NONE;
        }
        /* Find first empty slot */
        for (idx = 0; idx < MAX_NUM_CONFIG_FILES; idx++) {
            if (cfgyml_file[idx][0] == '\0') {
                break;
            }
        }
        cfgfile_idx = idx;
    }
    if (cfgfile_idx >= MAX_NUM_CONFIG_FILES) {
        return SHR_E_FULL;
    }
    if (cfgfile) {
        if (cfgfile[0] == '\0' || sal_strlen(cfgfile) > MAX_YML_FILE_LEN) {
            return SHR_E_PARAM;
        }
        if ((fh = bcma_io_file_open(cfgfile, "r")) == NULL) {
            return SHR_E_NOT_FOUND;
        }
        bcma_io_file_close(fh);
        sal_strncpy(cfgyml_file[cfgfile_idx], cfgfile, MAX_YML_FILE_LEN);
    }
    return SHR_E_NONE;
}

const char *
bcma_bcmmgmt_cfgyml_file_get(const char *cfgfile)
{
    int idx;

    for (idx = 0; idx < MAX_NUM_CONFIG_FILES; idx++) {
        if (cfgfile == NULL && cfgyml_file[idx][0] != '\0') {
            return cfgyml_file[idx];
        }
        if (cfgfile == cfgyml_file[idx]) {
            /* Clear pointer to return next non-NULL entry */
            cfgfile = NULL;
        }
    }
    return NULL;
}

int
bcma_bcmmgmt_config_load(bool warm)
{
    int rv;
    const char *cfgfile;

    /* Core must be initialized before we can load configuration files */
    rv = bcmmgmt_core_init();
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Load all installed configuration files if cold boot */
    if (!warm && !config_loaded) {
        cfgfile = NULL;
        while ((cfgfile = bcma_bcmmgmt_cfgyml_file_get(cfgfile)) != NULL) {
            rv = bcmmgmt_core_config_load(cfgfile);
            if (SHR_FAILURE(rv)) {
                return rv;
            }
        }
        config_loaded = true;
    }

    return rv;
}

int
bcma_bcmmgmt_init(bool warm, bool core_only,
                  const char *default_cfgyml_file,
                  const bcmmgmt_issu_info_t *ver_info)
{
    int rv;

    /*
     * Core-only ISSU initialization is not permitted, as it will
     * leave the system in an undefined state.
     */
    if (ver_info && core_only) {
        return SHR_E_INIT;
    }

    /* Ignore configuration files if warm boot */
    if (!warm) {
        /* Use default configuration file if none were specified */
        if (bcma_bcmmgmt_cfgyml_file_get(NULL) == NULL) {
            rv = bcma_bcmmgmt_cfgyml_file_set(default_cfgyml_file, 0);
            if (SHR_FAILURE(rv) && rv != SHR_E_NOT_FOUND) {
                /* Allow file not found for the default configuration file */
                return SHR_E_FAIL;
            }
        }
    }

    /* Start ISSU upgrade */
    rv = bcmmgmt_issu_start(warm, ver_info);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Use custom init sequence for multiple configuration files */
    if (SHR_SUCCESS(rv)) {
        rv = bcma_bcmmgmt_start(warm, core_only);
    }

    /* End of ISSU upgrade */
    bcmmgmt_issu_done();

    if (SHR_FAILURE(rv)) {
        bcma_bcmmgmt_shutdown(true, false);
    }

    return rv;
}

int
bcma_bcmmgmt_shutdown(bool graceful, bool keep_netif)
{
    int rv = SHR_E_NONE;

    if (keep_netif) {
        /* Shutdown without cleaning up network interfaces */
        rv = bcmmgmt_shutdown_keep_netif(graceful);
    } else {
        /* Default shutdown */
        rv = bcmmgmt_shutdown(graceful);
    }
    if (SHR_SUCCESS(rv)) {
        config_loaded = false;
    }
    return rv;
}

int
bcma_bcmmgmt_start(bool warm, bool core_only)
{
    int rv;

    rv = bcma_bcmmgmt_config_load(warm);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Start System Manager */
    rv = bcmmgmt_core_start(warm, bcmmgmt_sysm_default_comp_list());
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Check if we are all done */
    if (core_only) {
        return SHR_E_NONE;
    }

    /* Attach all installed switch devices */
    return bcmmgmt_dev_attach_all(warm);
}

int
bcma_bcmmgmt_stop(void)
{
    int rv;

    /* Detach all installed switch devices */
    (void)bcmmgmt_dev_detach_all();

    /* Stop System Manager */
    rv = bcmmgmt_core_stop(true);

    if (SHR_SUCCESS(rv)) {
        config_loaded = false;
    }

    return rv;
}
