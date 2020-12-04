/*! \file bcmbd_config.c
 *
 * Base driver data structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_assert.h>

#include <shr/shr_debug.h>

#include <bcmbd/bcmbd.h>
#include <bcmbd/bcmbd_internal.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmcfg/bcmcfg_init.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmlrd/bcmlrd_client.h>

#include <bcmltd/chip/bcmltd_id.h>

/* Log source for this component */
#define BSL_LOG_MODULE  BSL_LS_BCMBD_INIT

/*******************************************************************************
* Local functions
 */

static int
config_param_get(int unit, const char *tbl_name, const char *fld_name,
                 uint64_t *val)
{
    int rv;
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    if (!tbl_name || !fld_name || !val) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (!BCMDRD_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    /* Return error if BCMCFG is not ready */
    if (!bcmcfg_ready(unit)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (bcmlrd_table_field_name_to_idx(unit, tbl_name, fld_name, &sid, &fid));

    rv = bcmcfg_field_get(unit, sid, fid, val);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Config param %s.%s "
                            "(sid %"PRIu32", fid %"PRIu32") = "
                            "0x%"PRIx64" (%s)\n"),
                 tbl_name, fld_name, sid, fid, *val, shr_errmsg(rv)));
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
* Public functions
 */

uint32_t
bcmbd_config_get(int unit, uint32_t def_val,
                 const char *tbl_name, const char *fld_name)
{
    uint64_t val64;

    if (SHR_SUCCESS(config_param_get(unit, tbl_name, fld_name, &val64))) {
        return (uint32_t)val64;
    }
    return def_val;
}

uint64_t
bcmbd_config64_get(int unit, uint64_t def_val,
                   const char *tbl_name, const char *fld_name)
{
    uint64_t val64;

    if (SHR_SUCCESS(config_param_get(unit, tbl_name, fld_name, &val64))) {
        return val64;
    }
    return def_val;
}
