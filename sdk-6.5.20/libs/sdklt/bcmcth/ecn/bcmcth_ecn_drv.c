/*! \file bcmcth_ecn_drv.c
 *
 * BCMCTH ECN Protocol Driver APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmcth/bcmcth_ecn_drv.h>

/*******************************************************************************
 * Local definitions
 */

/*! Device specific attach function type. */
typedef bcmcth_ecn_drv_t *(*bcmcth_ecn_drv_get_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_cth_ecn_drv_get },
static struct {
    bcmcth_ecn_drv_get_f drv_get;
} ecn_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

static bcmcth_ecn_drv_t *ecn_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
* Public functions
 */

int
bcmcth_ecn_drv_init(int unit)
{
    bcmdrd_dev_type_t dev_type;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return SHR_E_UNIT;
    }

    /* Perform device-specific software setup */
    ecn_drv[unit] = ecn_drv_get[dev_type].drv_get(unit);

    return SHR_E_NONE;
}

int
bcmcth_ecn_proto_info_get(int unit, bcmcth_ecn_proto_info_t *proto_info)
{
    if (ecn_drv[unit] && ecn_drv[unit]->proto_info_get) {
        return ecn_drv[unit]->proto_info_get(unit, proto_info);
    }

    if (proto_info) {
        sal_memset(proto_info, 0, sizeof(*proto_info));
    }

    return SHR_E_NONE;
}

int
bcmcth_ecn_exp_to_ecn_map_set(int unit, bcmcth_ecn_exp_to_ecn_map_t *lt_entry)
{
    if (ecn_drv[unit] && ecn_drv[unit]->exp_to_ecn_map_set) {
        return ecn_drv[unit]->exp_to_ecn_map_set(unit, lt_entry);
    }

    return SHR_E_NONE;
}
