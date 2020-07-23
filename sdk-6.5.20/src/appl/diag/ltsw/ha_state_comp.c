/*! \file wb_state_comp.c
 *
 * This file is the test of the warm boot state
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <shr/shr_ha.h>
#include <bcmdrd_config.h>

static int ha_store_fh;
static int ha_write_f(void *buf, size_t len)
{
    return write(ha_store_fh, buf, len);
}

static int ha_read_f(void *buf, size_t len)
{
    return read(ha_store_fh, buf, len);
}

static int ha_unit_state_dump(int unit, const char *state_path)
{
    int rv;
    char out_file[256];
    const char *actual_path;
    int len;

    if (state_path) {
        actual_path = state_path;
    } else {
        actual_path = "./";
    }
    len = snprintf(out_file, sizeof(out_file), "%s/dump_ha_%d",
                   actual_path, unit);
    if ((unsigned)len >= sizeof(out_file) || len < 0) {
        LOG_WARN(BSL_LS_APPL_HA,
                 (BSL_META("Invalid path string for HA state file\n")));
    }
    ha_store_fh = open(out_file, O_WRONLY | O_CREAT | O_TRUNC,
                       S_IRUSR | S_IWUSR | S_IRGRP);
    if (ha_store_fh == -1) {
        LOG_WARN(BSL_LS_APPL_HA,
                 (BSL_META("Failed to open file %s error=%d\n"),
                  out_file, errno));
        return SHR_E_IO;
    }
    rv = shr_ha_content_dump(unit, &ha_write_f);
    close(ha_store_fh);
    return rv;
}

static int ha_unit_state_comp(int unit, const char *state_path)
{
    int rv;
    char out_file[256];
    const char *actual_path;
    int len;

    if (state_path) {
        actual_path = state_path;
    } else {
        actual_path = "./";
    }
    len = snprintf(out_file, sizeof(out_file), "%s/dump_ha_%d",
                   actual_path, unit);
    if ((unsigned)len >= sizeof(out_file) || len < 0) {
        LOG_WARN(BSL_LS_APPL_HA,
                 (BSL_META("Invalid path string for HA state file\n")));
    }
    ha_store_fh = open(out_file, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP);
    if (ha_store_fh == -1) {
        LOG_WARN(BSL_LS_APPL_HA,
                 (BSL_META("Failed to open file %s error=%d\n"),
                  out_file, errno));
        return SHR_E_IO;
    }
    cli_out("Compare HA state for instance dev %d - %s:\n", unit, out_file);
    rv = shr_ha_content_comp(unit, &ha_read_f);
    close(ha_store_fh);
    unlink(out_file);
    return rv;
}

int appl_ltsw_ha_unit_state_dump(int unit, const char *state_path)
{
    int rv;

    rv = ha_unit_state_dump(unit, state_path);
    if (SHR_FAILURE(rv)) {
        cli_out("Failed to dump HA instance for DRD dev %d - %s\n",
                unit, shr_errmsg(rv));
        return rv;
    }

    rv = ha_unit_state_dump(unit + BCMDRD_CONFIG_MAX_UNITS, state_path);
    if (SHR_FAILURE(rv)) {
        cli_out("Failed to dump HA instance for BAE of DRD dev %d - %s\n",
                unit, shr_errmsg(rv));
        return rv;
    }
    return SHR_E_NONE;
}

int appl_ltsw_ha_unit_state_comp(int unit, const char *state_path)
{
    int rv;

    rv = ha_unit_state_comp(unit, state_path);
    if (SHR_FAILURE(rv)) {
        cli_out("Failed to compare HA instance for DRD dev %d - %s\n",
                unit, shr_errmsg(rv));
        return rv;
    }

    rv = ha_unit_state_comp(unit + BCMDRD_CONFIG_MAX_UNITS, state_path);
    if (SHR_FAILURE(rv)) {
        cli_out("Failed to compare HA instance for BAE of DRD dev %d - %s\n",
                unit, shr_errmsg(rv));
        return rv;
    }
    return SHR_E_NONE;
}

