/*! \file bcmbd_ts.c
 *
 * BD TimeSync driver API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_chip.h>

#include <bcmbd/bcmbd_ts_internal.h>

/*******************************************************************************
 * Local definitions
 */

static const bcmbd_ts_drv_t *bd_ts_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Public functions
 */

int
bcmbd_ts_drv_init(int unit, const bcmbd_ts_drv_t *ts_drv)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    bd_ts_drv[unit] = ts_drv;

    return SHR_E_NONE;
}

int
bcmbd_ts_init(int unit)
{
    const bcmbd_ts_drv_t *ts_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    ts_drv = bd_ts_drv[unit];
    if (ts_drv && ts_drv->init) {
        return ts_drv->init(unit);
    }
    return SHR_E_NONE;
}

int
bcmbd_ts_cleanup(int unit)
{
    const bcmbd_ts_drv_t *ts_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    ts_drv = bd_ts_drv[unit];
    if (ts_drv && ts_drv->cleanup) {
        return ts_drv->cleanup(unit);
    }
    return SHR_E_NONE;
}

int
bcmbd_ts_timestamp_enable_set(int unit, int ts_inst, bool enable)
{
    const bcmbd_ts_drv_t *ts_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    ts_drv = bd_ts_drv[unit];
    if (ts_drv && ts_drv->timestamp_enable_set) {
        return ts_drv->timestamp_enable_set(unit, ts_inst, enable);
    }
    return SHR_E_UNAVAIL;
}

int
bcmbd_ts_timestamp_enable_get(int unit, int ts_inst, bool *enable)
{
    const bcmbd_ts_drv_t *ts_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    ts_drv = bd_ts_drv[unit];
    if (ts_drv && ts_drv->timestamp_enable_get) {
        return ts_drv->timestamp_enable_get(unit, ts_inst, enable);
    }
    return SHR_E_UNAVAIL;
}

int
bcmbd_ts_timestamp_raw_get(int unit, int ts_inst, uint64_t *ts_raw,
                           uint32_t *valid_bits, uint32_t *frac_bits)
{
    const bcmbd_ts_drv_t *ts_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    ts_drv = bd_ts_drv[unit];
    if (ts_drv && ts_drv->timestamp_get) {
        return ts_drv->timestamp_get(unit, ts_inst,
                                     ts_raw, valid_bits, frac_bits);
    }
    return SHR_E_UNAVAIL;
}

int
bcmbd_ts_timestamp_raw_set(int unit, int ts_inst, uint64_t ts_raw,
                           uint32_t valid_bits, uint32_t frac_bits)
{
    const bcmbd_ts_drv_t *ts_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    ts_drv = bd_ts_drv[unit];
    if (ts_drv && ts_drv->timestamp_set) {
        return ts_drv->timestamp_set(unit, ts_inst,
                                     ts_raw, valid_bits, frac_bits);
    }
    return SHR_E_UNAVAIL;
}

int
bcmbd_ts_timestamp_raw_offset_get(int unit, int ts_inst, uint64_t *ts_raw_ofs)
{
    const bcmbd_ts_drv_t *ts_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    ts_drv = bd_ts_drv[unit];
    if (ts_drv && ts_drv->timestamp_offset_get) {
        return ts_drv->timestamp_offset_get(unit, ts_inst, ts_raw_ofs);
    }
    return SHR_E_UNAVAIL;
}

int
bcmbd_ts_timestamp_raw_offset_set(int unit, int ts_inst,
                                  uint64_t ts_raw_ofs, uint64_t ts_raw_interval)
{
    const bcmbd_ts_drv_t *ts_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    ts_drv = bd_ts_drv[unit];
    if (ts_drv && ts_drv->timestamp_offset_set) {
        return ts_drv->timestamp_offset_set(unit, ts_inst,
                                            ts_raw_ofs, ts_raw_interval);
    }
    return SHR_E_UNAVAIL;
}

int
bcmbd_ts_timestamp_nanosec_get(int unit, int ts_inst, uint64_t *ts_ns)
{
    int rv;
    uint64_t ts_raw;
    uint32_t frac_bits;

    rv = bcmbd_ts_timestamp_raw_get(unit, ts_inst, &ts_raw, NULL, &frac_bits);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    if (ts_ns) {
        *ts_ns = ts_raw >> frac_bits;
    }
    return SHR_E_NONE;
}

int
bcmbd_ts_timestamp_nanosec_set(int unit, int ts_inst, uint64_t ts_ns)
{
    int rv;
    uint32_t valid_bits, frac_bits;
    uint64_t ts_raw;

    rv = bcmbd_ts_timestamp_raw_get(unit, ts_inst, NULL,
                                    &valid_bits, &frac_bits);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    ts_raw = ts_ns << frac_bits;
    return bcmbd_ts_timestamp_raw_set(unit, ts_inst, ts_raw,
                                      valid_bits, frac_bits);
}
