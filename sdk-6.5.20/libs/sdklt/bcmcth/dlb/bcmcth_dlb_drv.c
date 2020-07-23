/*! \file bcmcth_dlb_drv.c
 *
 * BCMCTH Dynamic Load Balance Driver APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmcth/bcmcth_dlb_drv.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_DLB

/*******************************************************************************
 * Local definitions
 */

/*! Device specific attach function type. */
typedef bcmcth_dlb_drv_t *(*bcmcth_dlb_drv_get_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_cth_dlb_drv_get },
static struct {
    bcmcth_dlb_drv_get_f drv_get;
} dlb_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

static bcmcth_dlb_drv_t *dlb_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
* Public functions
 */

int
bcmcth_dlb_drv_init(int unit)
{
    bcmdrd_dev_type_t dev_type;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return SHR_E_UNIT;
    }

    /* Perform device-specific software setup */
    dlb_drv[unit] = dlb_drv_get[dev_type].drv_get(unit);

    return SHR_E_NONE;
}

int
bcmcth_dlb_ecmp_init(int unit, bool warm)
{
    bcmcth_dlb_ecmp_info_t *ecmp_info = NULL;

    SHR_FUNC_ENTER(unit);

    if (dlb_drv[unit] && dlb_drv[unit]->ecmp_init) {
        SHR_IF_ERR_EXIT
            (bcmcth_dlb_ecmp_info_get(unit, &ecmp_info));
        SHR_NULL_CHECK(ecmp_info->ha_ent_arr, SHR_E_INTERNAL);
        SHR_IF_ERR_EXIT
            (dlb_drv[unit]->ecmp_init(unit, warm, ecmp_info->ha_ent_arr));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_dlb_ecmp_cleanup(int unit)
{
    if (dlb_drv[unit] && dlb_drv[unit]->ecmp_cleanup) {
        return dlb_drv[unit]->ecmp_cleanup(unit);
    }

    return SHR_E_NONE;
}

int
bcmcth_dlb_grp_cnt_get(int unit, int *grp_cnt)
{
    if (dlb_drv[unit] && dlb_drv[unit]->grp_cnt_get) {
        return dlb_drv[unit]->grp_cnt_get(unit, grp_cnt);
    }

    return SHR_E_NONE;
}

int
bcmcth_dlb_ecmp_set(int unit,
                    bcmcth_dlb_ecmp_t *lt_entry,
                    bool flow_set_init)
{
    if (dlb_drv[unit] && dlb_drv[unit]->ecmp_set) {
        return dlb_drv[unit]->ecmp_set(unit, lt_entry, flow_set_init);
    }

    return SHR_E_NONE;
}

int
bcmcth_dlb_ecmp_del(int unit,
                    bcmcth_dlb_ecmp_t *lt_entry)
{
    if (dlb_drv[unit] && dlb_drv[unit]->ecmp_del) {
        return dlb_drv[unit]->ecmp_del(unit, lt_entry);
    }

    return SHR_E_NONE;
}

int
bcmcth_dlb_ecmp_pctl_set(int unit,
                         bcmcth_dlb_ecmp_pctl_t *lt_entry)
{
    if (dlb_drv[unit] && dlb_drv[unit]->ecmp_pctl_set) {
        return dlb_drv[unit]->ecmp_pctl_set(unit, lt_entry);
    }

    return SHR_E_NONE;
}

int
bcmcth_dlb_ecmp_pctl_del(int unit,
                         bcmcth_dlb_ecmp_pctl_t *lt_entry)
{
    if (dlb_drv[unit] && dlb_drv[unit]->ecmp_pctl_del) {
        return dlb_drv[unit]->ecmp_pctl_del(unit, lt_entry);
    }

    return SHR_E_NONE;
}

int
bcmcth_dlb_ecmp_info_recover(int unit,
                             bcmcth_dlb_ecmp_entry_info_t *entry_info)
{
    if (dlb_drv[unit] && dlb_drv[unit]->ecmp_recover) {
        return dlb_drv[unit]->ecmp_recover(unit, entry_info);
    }

    return SHR_E_NONE;
}

/*!
 * \brief Set the DLB group information in hw.
 *
 * \param [in] unit Unit number.
 * \param [in] blk_ptr DLB_TRUNK LT block pointer.
 * \param [in] lt_entry DLB_TRUNK LT entry.
 * \param [in] flow_set_init Flag indicating allocation
 *             of flow set resources to DLB group.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL if call fails.
 */
int
bcmcth_dlb_trunk_set(int unit,
                     bcmcth_dlb_trunk_blk_t *blk_ptr,
                     bcmcth_dlb_trunk_t *lt_entry,
                     bool flow_set_init)
{
    if (dlb_drv[unit] && dlb_drv[unit]->trunk_set) {
        return dlb_drv[unit]->trunk_set(
                              unit,
                              blk_ptr,
                              lt_entry,
                              flow_set_init);
    }

    return SHR_E_NONE;
}

/*!
 * \brief Delete the DLB group information in hw.
 *
 * \param [in] unit Unit number.
 * \param [in] blk_ptr DLB_TRUNK LT block pointer.
 * \param [in] dlb_trunk DLB_TRUNK LT entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL if call fails.
 */

int
bcmcth_dlb_trunk_del(int unit,
                    bcmcth_dlb_trunk_blk_t *blk_ptr,
                    bcmcth_dlb_trunk_t *lt_entry)
{
    if (dlb_drv[unit] && dlb_drv[unit]->trunk_del) {
        return dlb_drv[unit]->trunk_del(unit, blk_ptr, lt_entry);
    }

    return SHR_E_NONE;
}

/*!
 * \brief Set the DLB_TRUNK_PORT_CONTROL hw entry
 *
 * \param [in] unit Unit number.
 * \param [in] entry DLB_TRUNK_PORT_CONTROL LT entry
 *
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL if call fails.
 */
int
bcmcth_dlb_trunk_pctl_set(int unit,
                         bcmcth_dlb_trunk_pctl_t *lt_entry)
{
    if (dlb_drv[unit] && dlb_drv[unit]->trunk_pctl_set) {
        return dlb_drv[unit]->trunk_pctl_set(unit, lt_entry);
    }

    return SHR_E_NONE;
}

/*!
 * \brief Set the DLB Trunk group count.
 *
 * \param [in] unit Unit number.
 * \param [out] grp_cnt Group count.
 * \param [out] flow_set_blk_cnt total block count for flow set.
 *
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL if call fails.
 */
int
bcmcth_dlb_trunk_param_get(int unit, int *grp_cnt, int *flow_set_blk_cnt)
{
    if (dlb_drv[unit] && dlb_drv[unit]->trunk_grp_param_get) {
        return dlb_drv[unit]->trunk_grp_param_get(unit, grp_cnt, flow_set_blk_cnt);
    }

    return SHR_E_NONE;
}

int
bcmcth_dlb_ecmp_stats_set(int unit,
                          bcmcth_dlb_ecmp_stats_t *lt_entry)
{
    if (dlb_drv[unit] && dlb_drv[unit]->ecmp_stats_set) {
        return dlb_drv[unit]->ecmp_stats_set(unit, lt_entry);
    }

    return SHR_E_NONE;
}

int
bcmcth_dlb_ecmp_stats_get(int unit,
                          bcmcth_dlb_ecmp_stats_t *lt_entry)
{
    if (dlb_drv[unit] && dlb_drv[unit]->ecmp_stats_set) {
        return dlb_drv[unit]->ecmp_stats_get(unit, lt_entry);
    }

    return SHR_E_NONE;
}
