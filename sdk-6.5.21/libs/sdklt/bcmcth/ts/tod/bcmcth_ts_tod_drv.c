/*! \file bcmcth_ts_tod_drv.c
 *
 * ToD driver APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <bcmcth/bcmcth_ts_drv.h>
#include <bcmcth/bcmcth_ts_tod_drv.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmissu/issu_api.h>
#include <bcmmgmt/bcmmgmt_sysm.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TIMESYNC

/*******************************************************************************
 * Private Functions
 */

static int
tod_update(int unit,
           bcmltd_sid_t ltid,
           ts_tod_entry_t *entry)
{
    bcmcth_ts_tod_drv_t *tod_drv = bcmcth_ts_tod_drv_get(unit);

    if (tod_drv && tod_drv->tod_update) {
        return tod_drv->tod_update(unit, ltid, entry);
    }
    return SHR_E_NONE;
}

static int
tod_auto(int unit,
         bcmltd_sid_t ltid,
         ts_tod_entry_t *entry)

{
    bcmcth_ts_tod_drv_t *tod_drv = bcmcth_ts_tod_drv_get(unit);

    if (tod_drv && tod_drv->tod_auto) {
        return tod_drv->tod_auto(unit, ltid, entry);
    }
    return SHR_E_NONE;
}

static int
tod_get(int unit,
        bcmltd_sid_t ltid,
        ts_tod_entry_oper_t *entry)
{
    bcmcth_ts_tod_drv_t *tod_drv = bcmcth_ts_tod_drv_get(unit);

    if (tod_drv && tod_drv->tod_get) {
        return tod_drv->tod_get(unit, ltid, entry);
    }
    return SHR_E_NONE;
}

static int
tod_entry_lookup(int unit,
                 bcmltd_sid_t sid,
                 const bcmltd_op_arg_t *op_arg,
                 ts_tod_entry_oper_t *oper)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (tod_get(unit, sid, oper));
exit:
    SHR_FUNC_EXIT();
}

static int
tod_entry_update(int unit,
                 ts_tod_entry_t *entry)
{
    SHR_FUNC_ENTER(unit);
    if (entry->hw_update) {
        SHR_IF_ERR_EXIT
            (tod_auto(unit, entry->ltid, entry));
    } else {
        SHR_IF_ERR_EXIT
            (tod_auto(unit, entry->ltid, entry));
        SHR_IF_ERR_EXIT
            (tod_update(unit, entry->ltid, entry));
    }

exit:
    SHR_FUNC_EXIT();
}
/*******************************************************************************
* Public functions
 */
int
bcmcth_ts_tod_update(int unit,
                     ts_tod_entry_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (tod_entry_update(unit, entry));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ts_tod_lookup(int unit,
                     bcmltd_sid_t ltid,
                     const bcmltd_op_arg_t *op_arg,
                     ts_tod_entry_oper_t *oper)

{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (tod_entry_lookup(unit, ltid, op_arg, oper));

    exit:
        SHR_FUNC_EXIT();
}
