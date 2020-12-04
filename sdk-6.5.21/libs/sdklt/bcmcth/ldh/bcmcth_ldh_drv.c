/*! \file bcmcth_ldh_drv.c
 *
 * BCMCTH Latency Distribution Histogram Driver APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd_ts.h>
#include <bcmcth/bcmcth_ldh_drv.h>

#include "bcmcth_ldh_internal.h"

/*******************************************************************************
 * Local definitions
 */

/*! Device specific attach function type. */
typedef bcmcth_ldh_drv_t *(*bcmcth_ldh_drv_get_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_cth_ldh_drv_get },
static struct {
    bcmcth_ldh_drv_get_f drv_get;
} ldh_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

static bcmcth_ldh_drv_t *ldh_drv[BCMDRD_CONFIG_MAX_UNITS];

static int
bcmcth_ldh_monitor_max_get(int unit, int *num)
{
    SHR_FUNC_ENTER(unit);

    if (ldh_drv[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_NULL_CHECK(num, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (ldh_drv[unit]->mon_max_get(unit, num));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_ldh_ctr_pool_max_get(int unit, int *num)
{
    SHR_FUNC_ENTER(unit);

    if (ldh_drv[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_NULL_CHECK(num, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (ldh_drv[unit]->ctr_pool_max_get(unit, num));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_ldh_ctr_entry_max_get(int unit, int *num)
{
    SHR_FUNC_ENTER(unit);

    if (ldh_drv[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_NULL_CHECK(num, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (ldh_drv[unit]->ctr_entry_max_get(unit, num));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
* Public functions
 */
int
bcmcth_ldh_drv_init(int unit)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    /* Perform device-specific software setup. */
    ldh_drv[unit] = ldh_drv_get[dev_type].drv_get(unit);

    if (ldh_drv[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_init(int unit)
{
    int max_num;
    bool ts_en = false;

    SHR_FUNC_ENTER(unit);

    if (ldh_drv[unit] == NULL) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmcth_ldh_control_init(unit));

    /* Misc configuration. */
    SHR_IF_ERR_EXIT
        (bcmcth_ldh_monitor_max_get(unit, &max_num));
    SHR_IF_ERR_EXIT
        (bcmcth_ldh_monitor_max_init(unit, max_num));

    SHR_IF_ERR_EXIT
        (bcmcth_ldh_ctr_pool_max_get(unit, &max_num));
    SHR_IF_ERR_EXIT
        (bcmcth_ldh_ctr_pool_max_init(unit, max_num));

    SHR_IF_ERR_EXIT
        (bcmcth_ldh_ctr_entry_max_get(unit, &max_num));
    SHR_IF_ERR_EXIT
        (bcmcth_ldh_ctr_entry_max_init(unit, max_num));

    /* Check if TimeSync timestamp counter is enabled. */
    SHR_IF_ERR_EXIT
        (bcmbd_ts_timestamp_enable_get(unit, BCMLDH_TS_INST, &ts_en));
    if (!ts_en) {
        SHR_ERR_EXIT(SHR_E_DISABLED);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (ldh_drv[unit] == NULL) {
        SHR_EXIT();
    }

    ldh_drv[unit] = NULL;

    SHR_IF_ERR_EXIT
        (bcmcth_ldh_control_cleanup(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_device_info_get(int unit, bcmcth_ldh_device_info_t *device_info)
{
    SHR_FUNC_ENTER(unit);

    if (ldh_drv[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_NULL_CHECK(device_info, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (ldh_drv[unit]->device_info_get(unit, device_info));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_ctr_pool_info_get(int unit,
                             bcmcth_ldh_pt_lt_mapping_t *mems,
                             uint32_t *num)
{
    SHR_FUNC_ENTER(unit);

    if (ldh_drv[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_NULL_CHECK(mems, SHR_E_MEMORY);
    SHR_NULL_CHECK(num, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (ldh_drv[unit]->ctr_pool_info_get(unit, &mems, num));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_ctr_data_get(int unit,
                        int pool_id,
                        int index,
                        uint32_t *packets,
                        uint32_t *bytes)
{
    SHR_FUNC_ENTER(unit);

    if (ldh_drv[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_NULL_CHECK(packets, SHR_E_MEMORY);
    SHR_NULL_CHECK(bytes, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (ldh_drv[unit]->ctr_data_get(unit,
                                     pool_id,
                                     index,
                                     packets,
                                     bytes));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_cur_histo_group_get(int unit, int mon_id, int *group)
{
    SHR_FUNC_ENTER(unit);

    if (ldh_drv[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_NULL_CHECK(group, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (ldh_drv[unit]->cur_histo_group_get(unit, mon_id, group));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_last_ctr_idx_get(int unit, int mon_id, int *index)
{
    SHR_FUNC_ENTER(unit);

    if (ldh_drv[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_NULL_CHECK(index, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (ldh_drv[unit]->last_ctr_idx_get(unit, mon_id, index));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_ctr_wrap_around_get(int unit, int mon_id, int *num)
{
    SHR_FUNC_ENTER(unit);

    if (ldh_drv[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_NULL_CHECK(num, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (ldh_drv[unit]->ctr_wrap_around_get(unit, mon_id, num));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_cur_histo_group_set(int unit, int mon_id, int group)
{
    SHR_FUNC_ENTER(unit);

    if (ldh_drv[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (ldh_drv[unit]->cur_histo_group_set(unit, mon_id, group));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_last_ctr_idx_set(int unit, int mon_id, int index)
{
    SHR_FUNC_ENTER(unit);

    if (ldh_drv[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (ldh_drv[unit]->last_ctr_idx_set(unit, mon_id, index));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_ctr_wrap_around_set(int unit, int mon_id, int num)
{
    SHR_FUNC_ENTER(unit);

    if (ldh_drv[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (ldh_drv[unit]->ctr_wrap_around_set(unit, mon_id, num));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_mon_enable_set(int unit, int mon_id, bool en)
{
    SHR_FUNC_ENTER(unit);

    if (ldh_drv[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (ldh_drv[unit]->mon_enable_set(unit, mon_id, en));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_mon_count_mode_set(int unit, int mon_id, int mode)
{
    SHR_FUNC_ENTER(unit);

    if (ldh_drv[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (ldh_drv[unit]->mon_count_mode_set(unit, mon_id, mode));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_mon_time_step_set(int unit, int mon_id, int step)
{
    SHR_FUNC_ENTER(unit);

    if (ldh_drv[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (ldh_drv[unit]->mon_time_step_set(unit, mon_id, step));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_mon_pool_id_set(int unit, int mon_id, int pool_id)
{
    SHR_FUNC_ENTER(unit);

    if (ldh_drv[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (ldh_drv[unit]->mon_pool_id_set(unit, mon_id, pool_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_mon_pool_mode_set(int unit, int mon_id, int mode)
{
    SHR_FUNC_ENTER(unit);

    if (ldh_drv[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (ldh_drv[unit]->mon_pool_mode_set(unit, mon_id, mode));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_mon_base_offset_set(int unit, int mon_id, int offset)
{
    SHR_FUNC_ENTER(unit);

    if (ldh_drv[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (ldh_drv[unit]->mon_base_offset_set(unit, mon_id, offset));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_mon_counter_incr_set(int unit, int mon_id, int incr)
{
    SHR_FUNC_ENTER(unit);

    if (ldh_drv[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (ldh_drv[unit]->mon_ctr_incr_set(unit, mon_id, incr));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_mon_group_max_set(int unit, int mon_id, int max)
{
    SHR_FUNC_ENTER(unit);

    if (ldh_drv[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (ldh_drv[unit]->mon_group_max_set(unit, mon_id, max));

exit:
    SHR_FUNC_EXIT();
}
