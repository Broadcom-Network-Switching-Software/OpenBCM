/*! \file bcmcth_vlan_drv.c
 *
 * BCMCTH VLAN driver APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmcth/bcmcth_vlan_drv.h>

/*******************************************************************************
 * Local definitions
 */
/*! Device specific attach function type. */
typedef bcmcth_vlan_drv_t *(*bcmcth_vlan_drv_get_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_cth_vlan_drv_get },
static struct {
    bcmcth_vlan_drv_get_f drv_get;
} vlan_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

static bcmcth_vlan_drv_t *vlan_drv[BCMDRD_CONFIG_MAX_UNITS];
/*******************************************************************************
 * Public Functions
 */
int
bcmcth_vlan_drv_init(int unit)
{
    bcmdrd_dev_type_t dev_type;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return SHR_E_UNIT;
    }

    /* Perform device-specific software setup */
    vlan_drv[unit] = vlan_drv_get[dev_type].drv_get(unit);

    return SHR_E_NONE;
}

int
bcmcth_vlan_xlate_port_grp_set(int unit)
{
    if (vlan_drv[unit] && vlan_drv[unit]->xlate_port_grp_set) {
        return vlan_drv[unit]->xlate_port_grp_set(unit);
    }

    return SHR_E_NONE;
}

int
bcmcth_vlan_ing_stg_profile_set(int unit,
                                uint32_t trans_id,
                                bcmltd_sid_t lt_id,
                                vlan_ing_stg_profile_t *lt_info)
{
    if (vlan_drv[unit] && vlan_drv[unit]->ing_stg_profile_set) {
        return vlan_drv[unit]->ing_stg_profile_set(unit, trans_id, lt_id, lt_info);
    }

    return SHR_E_NONE;
}

int
bcmcth_vlan_ing_egr_stg_member_profile_set(int unit,
                                           uint32_t trans_id,
                                           bcmltd_sid_t lt_id,
                                           vlan_ing_egr_stg_member_profile_t *lt_info)
{
    if (vlan_drv[unit] && vlan_drv[unit]->ing_egr_stg_member_profile_set) {
        return vlan_drv[unit]->ing_egr_stg_member_profile_set(unit, trans_id, lt_id, lt_info);
    }

    return SHR_E_NONE;
}

int
bcmcth_vlan_egr_stg_profile_set(int unit,
                                uint32_t trans_id,
                                bcmltd_sid_t lt_id,
                                vlan_egr_stg_profile_t *lt_info)
{
    if (vlan_drv[unit] && vlan_drv[unit]->egr_stg_profile_set) {
        return vlan_drv[unit]->egr_stg_profile_set(unit, trans_id, lt_id, lt_info);
    }

    return SHR_E_NONE;
}

