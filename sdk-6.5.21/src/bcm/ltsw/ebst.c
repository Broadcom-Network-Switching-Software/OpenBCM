/*! \file ebst.c
 *
 * EBST Module Emulator above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm_int/control.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/ebst.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/mbcm/ebst.h>

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_COSQ

/*!
 * \brief EBST information data structure.
 */
typedef struct ebst_info_s {

    /*! Module is initialized. */
    bool inited;

    /*! Module lock. */
    sal_mutex_t mutex;

} ebst_info_t;

/*
 * Static global variable to hold EBST info.
 */
static ebst_info_t ebst_info[BCM_MAX_NUM_UNITS] = {{0}};

/* EBST info. */
#define EBST_INFO(unit) \
    (&ebst_info[unit])

/* Check if EBST module is initialized. */
#define EBST_INIT_CHECK(unit) \
    do { \
        ebst_info_t *ebst = EBST_INFO(unit); \
        if (ebst->inited == false) { \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT); \
        } \
    } while(0)

/* Take EBST lock. */
#define EBST_LOCK(unit) \
    sal_mutex_take(EBST_INFO(unit)->mutex, SAL_MUTEX_FOREVER)

/* Give EBST lock. */
#define EBST_UNLOCK(unit) \
    sal_mutex_give(EBST_INFO(unit)->mutex)

/******************************************************************************
 * Private functions
 */



/******************************************************************************
 * Public functions
 */

int
bcmi_ltsw_cosq_ebst_detach(int unit)
{
    bool locked = false;
    SHR_FUNC_ENTER(unit);

    /* If not initialized, return success. */
    if (EBST_INFO(unit)->inited == false) {
        SHR_EXIT();
    }

    EBST_INFO(unit)->inited = false;

    EBST_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_ebst_detach(unit));

    EBST_UNLOCK(unit);
    locked = false;

    if (EBST_INFO(unit)->mutex) {
        sal_mutex_destroy(EBST_INFO(unit)->mutex);
        EBST_INFO(unit)->mutex = NULL;
    }

exit:
    if (locked) {
        EBST_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_cosq_ebst_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (EBST_INFO(unit)->inited) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_ebst_detach(unit));
    }

    if (EBST_INFO(unit)->mutex == NULL) {
        EBST_INFO(unit)->mutex = sal_mutex_create("EBST mutex");
        SHR_NULL_CHECK(EBST_INFO(unit)->mutex, SHR_E_MEMORY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_ebst_init(unit));
    EBST_INFO(unit)->inited = true;
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_cosq_ebst_threshold_profile_get(
    int unit,
    bcm_cosq_object_id_t *object_id,
    bcm_bst_stat_id_t bid,
    bcm_cosq_ebst_threshold_profile_t *profile)
{
    SHR_FUNC_ENTER(unit);
    EBST_INIT_CHECK(unit);

    SHR_NULL_CHECK(object_id, SHR_E_PARAM);
    SHR_NULL_CHECK(profile, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_ebst_threshold_profile_get(unit, object_id, bid, profile));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_cosq_ebst_threshold_profile_set(
    int unit,
    bcm_cosq_object_id_t *object_id,
    bcm_bst_stat_id_t bid,
    bcm_cosq_ebst_threshold_profile_t *profile)
{
    SHR_FUNC_ENTER(unit);
    EBST_INIT_CHECK(unit);

    SHR_NULL_CHECK(object_id, SHR_E_PARAM);
    SHR_NULL_CHECK(profile, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_ebst_threshold_profile_set(unit, object_id, bid, profile));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_cosq_ebst_control_get(
    int unit,
    bcm_cosq_ebst_control_t type,
    int *arg)
{
    SHR_FUNC_ENTER(unit);
    EBST_INIT_CHECK(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_ebst_control_get(unit, type, arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_cosq_ebst_control_set(
    int unit,
    bcm_cosq_ebst_control_t type,
    int arg)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);
    EBST_INIT_CHECK(unit);

    if (arg < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    EBST_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_ebst_control_set(unit, type, arg));

    EBST_UNLOCK(unit);
    locked = false;

exit:
    if (locked) {
        EBST_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_cosq_ebst_monitor_get(
    int unit,
    bcm_cosq_object_id_t *object_id,
    bcm_bst_stat_id_t bid,
    bcm_cosq_ebst_monitor_t *monitor)
{
    SHR_FUNC_ENTER(unit);
    EBST_INIT_CHECK(unit);

    SHR_NULL_CHECK(object_id, SHR_E_PARAM);
    SHR_NULL_CHECK(monitor, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_ebst_monitor_get(unit, object_id, bid, monitor));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_cosq_ebst_monitor_set(
    int unit,
    bcm_cosq_object_id_t *object_id,
    bcm_bst_stat_id_t bid,
    bcm_cosq_ebst_monitor_t *monitor)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);
    EBST_INIT_CHECK(unit);

    SHR_NULL_CHECK(object_id, SHR_E_PARAM);
    SHR_NULL_CHECK(monitor, SHR_E_PARAM);

    EBST_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_ebst_monitor_set(unit, object_id, bid, monitor));

    EBST_UNLOCK(unit);
    locked = false;

exit:
    if (locked) {
        EBST_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_cosq_ebst_data_stat_info_get(
    int unit,
    bcm_cosq_object_id_t *object_id,
    bcm_cosq_ebst_data_stat_info_t *info)
{
    SHR_FUNC_ENTER(unit);
    EBST_INIT_CHECK(unit);

    SHR_NULL_CHECK(object_id, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_ebst_data_stat_info_get(unit, object_id, info));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_cosq_ebst_data_get(
    int unit,
    bcm_cosq_object_id_t *object_id,
    bcm_bst_stat_id_t bid,
    int array_size,
    bcm_cosq_ebst_data_entry_t *entry_array,
    int *count)
{
    SHR_FUNC_ENTER(unit);
    EBST_INIT_CHECK(unit);

    SHR_NULL_CHECK(entry_array, SHR_E_PARAM);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_ebst_data_get(unit, object_id, bid, array_size,
                                      entry_array, count));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_cosq_ebst_enable_set(
    int unit,
    int enable)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);
    EBST_INIT_CHECK(unit);

    if (enable != 0 && enable != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    EBST_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_ebst_enable_set(unit, enable));

    EBST_UNLOCK(unit);
    locked = false;

exit:
    if (locked) {
        EBST_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_cosq_ebst_enable_get(
    int unit,
    int *enable)
{
    SHR_FUNC_ENTER(unit);
    EBST_INIT_CHECK(unit);

    SHR_NULL_CHECK(enable, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_ebst_enable_get(unit, enable));

exit:
    SHR_FUNC_EXIT();
}

