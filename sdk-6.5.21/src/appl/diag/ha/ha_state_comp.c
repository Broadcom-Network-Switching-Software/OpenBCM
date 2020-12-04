/*! \file wb_state_comp.c
 *
 * This file is the test of the warm boot state
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/ltsw/ha.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <bcma/io/bcma_io_file.h>

static int
ha_write_f(void *ctx, void *buf, size_t len)
{
    bcma_io_file_handle_t *fh = (bcma_io_file_handle_t *)ctx;
    return bcma_io_file_write(*fh, buf, 1, len);
}

static int
ha_read_f(void *ctx, void *buf, size_t len)
{
    bcma_io_file_handle_t *fh = (bcma_io_file_handle_t *)ctx;
    return bcma_io_file_read(*fh, buf, 1, len);
}

static int
ha_unit_state_dump(int unit, const char *state_path)
{
    int rv;
    char out_file[256];
    const char *actual_path;
    int len;
    bcma_io_file_handle_t fh;

    if (state_path) {
        actual_path = state_path;
    } else {
        actual_path = "./";
    }
    len = snprintf(out_file, sizeof(out_file), "%s/dump_bcm_ha_%d",
                   actual_path, unit);
    if ((unsigned)len >= sizeof(out_file) || len < 0) {
        LOG_WARN(BSL_LS_APPL_HA,
                 (BSL_META("Invalid path string for HA state file\n")));
    }
    fh = bcma_io_file_open(out_file, "wb");
    if (fh == NULL) {
        LOG_WARN(BSL_LS_APPL_HA,
                 (BSL_META("Failed to open file %s\n"),
                  out_file));
        return SHR_E_IO;
    }
    rv = soc_ltsw_ha_mem_content_dump(unit, &ha_write_f, &fh);
    bcma_io_file_close(fh);
    return rv;
}

static int
ha_unit_state_comp(int unit, const char *state_path)
{
    int rv;
    char out_file[256];
    const char *actual_path;
    int len;
    bcma_io_file_handle_t fh;

    if (state_path) {
        actual_path = state_path;
    } else {
        actual_path = "./";
    }
    len = snprintf(out_file, sizeof(out_file), "%s/dump_bcm_ha_%d",
                   actual_path, unit);
    if ((unsigned)len >= sizeof(out_file) || len < 0) {
        LOG_WARN(BSL_LS_APPL_HA,
                 (BSL_META("Invalid path string for HA state file\n")));
    }
    fh = bcma_io_file_open(out_file, "rb");
    if (fh == NULL) {
        LOG_WARN(BSL_LS_APPL_HA,
                 (BSL_META("Failed to open file %s\n"),
                  out_file));
        return SHR_E_IO;
    }
    cli_out("Compare HA state for unit %d - %s: ", unit, out_file);
    rv = soc_ltsw_ha_mem_content_comp(unit, &ha_read_f, &fh);
    cli_out("%s\n", rv < 0 ? "FAILED" : "PASSED");
    bcma_io_file_close(fh);
    bcma_io_file_delete(out_file);
    return rv;
}

int appl_ha_unit_state_dump(int unit, const char *state_path)
{
    int rv;

    rv = ha_unit_state_dump(unit, state_path);
    if (SHR_FAILURE(rv)) {
        cli_out("Failed to dump HA memory for unit %d - %s\n",
                unit, shr_errmsg(rv));
        return rv;
    }
    return SHR_E_NONE;
}

int appl_ha_unit_state_comp(int unit, const char *state_path)
{
    int rv;

    rv = ha_unit_state_comp(unit, state_path);
    if (SHR_FAILURE(rv)) {
        cli_out("Failed to compare HA memory for unit %d - %s\n",
                unit, shr_errmsg(rv));
        return rv;
    }
    return SHR_E_NONE;
}

