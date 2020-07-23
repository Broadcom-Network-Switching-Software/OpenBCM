/*! \file bcma_ha_mem_state.c
 *
 * This file is the test of the warm boot state
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmha/bcmha_mem_diag.h>
#include <bcmlt/bcmlt.h>

#include <bcma/io/bcma_io_file.h>
#include <bcma/ha/bcma_ha.h>

/*******************************************************************************
 * Local data
 */

static bcma_io_file_handle_t ha_store_fh;

/*******************************************************************************
 * Private functions
 */

static int
ha_write_f(void *buf, size_t len)
{
    return bcma_io_file_write(ha_store_fh, buf, 1, len);
}

static int
ha_read_f(void *buf, size_t len)
{
    return bcma_io_file_read(ha_store_fh, buf, 1, len);
}

/*******************************************************************************
 * Public functions
 */

int
bcma_ha_mem_state_dump(int unit, const char *state_path)
{
    int rv;
    char out_file[256];
    const char *actual_path;
    int len;

    if (state_path) {
        actual_path = state_path;
    } else {
        actual_path = ".";
    }
    len = sal_snprintf(out_file, sizeof(out_file), "%s/dump_ha_%d",
                       actual_path, unit);
    if ((unsigned)len >= sizeof(out_file) || len < 0) {
        LOG_ERROR(BSL_LS_APPL_HA,
                  (BSL_META_U(unit,
                              "Invalid path string for HA state file\n")));
        return SHR_E_PARAM;
    }
    ha_store_fh = bcma_io_file_open(out_file, "wb");
    if (ha_store_fh == NULL) {
        LOG_ERROR(BSL_LS_APPL_HA,
                  (BSL_META_U(unit,
                              "Failed to open file %s\n"),
                   out_file));
        return SHR_E_IO;
    }
    rv = bcmha_mem_content_dump(unit, &ha_write_f);
    bcma_io_file_close(ha_store_fh);
    return rv;
}

int
bcma_ha_mem_state_comp(int unit, const char *state_path)
{
    int rv;
    char out_file[256];
    const char *actual_path;
    int len;

    if (state_path) {
        actual_path = state_path;
    } else {
        actual_path = ".";
    }
    len = sal_snprintf(out_file, sizeof(out_file), "%s/dump_ha_%d",
                       actual_path, unit);
    if ((unsigned)len >= sizeof(out_file) || len < 0) {
        LOG_ERROR(BSL_LS_APPL_HA,
                  (BSL_META_U(unit,
                              "Invalid path string for HA state file\n")));
        return SHR_E_PARAM;
    }
    ha_store_fh = bcma_io_file_open(out_file, "rb");
    if (ha_store_fh == NULL) {
        LOG_WARN(BSL_LS_APPL_HA,
                 (BSL_META_U(unit,
                             "Failed to open file %s\n"),
                  out_file));
        return SHR_E_IO;
    }
    rv = bcmha_mem_content_comp(unit, &ha_read_f);
    bcma_io_file_close(ha_store_fh);
    bcma_io_file_delete(out_file);
    return rv;
}

int
bcma_ha_mem_state_dump_all(const char *state_path)
{
    int rv;
    int unit;

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (!bcmdrd_dev_exists(unit)) {
            continue;
        }
        if (!bcmlt_device_attached(unit)) {
            rv = bcma_ha_mem_state_dump(unit, state_path);
            if (SHR_FAILURE(rv)) {
                LOG_WARN(BSL_LS_APPL_HA,
                         (BSL_META_U(unit,
                                     "Failed to save HA content (%s)\n"),
                          shr_errmsg(rv)));
            }
        }
    }
    return SHR_E_NONE;
}

int
bcma_ha_mem_state_comp_all(const char *state_path)
{
    int rv;
    int rc = SHR_E_NONE;
    int unit;

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (!bcmdrd_dev_exists(unit)) {
            continue;
        }
        if (bcmlt_device_attached(unit)) {
            rv = bcma_ha_mem_state_comp(unit, state_path);
            if (SHR_FAILURE(rv)) {
                rc = rv;
                LOG_WARN(BSL_LS_APPL_HA,
                         (BSL_META_U(unit,
                                     "HA content does not match (%d)\n"), rv));
            }
        }
    }
    return rc;
}
