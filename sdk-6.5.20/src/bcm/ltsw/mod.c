/*! \file mod.c
 *
 * Mirror-on-drop Module Emulator above SDKLT.
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/cosq.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/mod.h>
#include <bcm_int/ltsw/mbcm/mod.h>

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_COSQ

/*!
 * \brief MOD information data structure.
 */
typedef struct mod_info_s {

    /*! Module is initialized. */
    bool inited;

    /*! Module lock. */
    sal_mutex_t mutex;

} mod_info_t;

/*
 * Static global variable to hold MOD info.
 */
static mod_info_t mod_info[BCM_MAX_NUM_UNITS] = {{0}};

/* MOD info. */
#define MOD_INFO(unit) \
    (&mod_info[unit])

/* Check if MOD module is initialized. */
#define MOD_INIT_CHECK(unit) \
    do { \
        mod_info_t *mod = MOD_INFO(unit); \
        if (mod->inited == false) { \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT); \
        } \
    } while(0)

/* Take MOD lock. */
#define MOD_LOCK(unit) \
    sal_mutex_take(MOD_INFO(unit)->mutex, SAL_MUTEX_FOREVER)

/* Give MOD lock. */
#define MOD_UNLOCK(unit) \
    sal_mutex_give(MOD_INFO(unit)->mutex)

/******************************************************************************
 * Private functions
 */



/******************************************************************************
 * Public functions
 */

int
bcmi_ltsw_cosq_mod_detach(int unit)
{
    bool locked = false;
    SHR_FUNC_ENTER(unit);

    /* If not initialized, return success. */
    if (MOD_INFO(unit)->inited == false) {
        SHR_EXIT();
    }

    MOD_LOCK(unit);
    locked = true;

    MOD_INFO(unit)->inited = false;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_mod_detach(unit));

    MOD_UNLOCK(unit);
    locked = false;

    if (MOD_INFO(unit)->mutex) {
        sal_mutex_destroy(MOD_INFO(unit)->mutex);
        MOD_INFO(unit)->mutex = NULL;
    }

exit:
    if (locked) {
        MOD_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_cosq_mod_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (MOD_INFO(unit)->inited) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_mod_detach(unit));
    }

    if (MOD_INFO(unit)->mutex == NULL) {
        MOD_INFO(unit)->mutex = sal_mutex_create("MOD mutex");
        SHR_NULL_CHECK(MOD_INFO(unit)->mutex, SHR_E_MEMORY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_mod_init(unit));
    MOD_INFO(unit)->inited = true;
exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_cosq_mod_dest_add(int unit, bcmi_ltsw_cosq_mod_dest_t *mod_dest)
{
    int rv;
    SHR_FUNC_ENTER(unit);
    MOD_INIT_CHECK(unit);

    SHR_NULL_CHECK(mod_dest, SHR_E_PARAM);

    MOD_LOCK(unit);
    rv = mbcm_ltsw_cosq_mod_dest_add(unit, mod_dest);
    MOD_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_cosq_mod_dest_delete(int unit,
                               bcmi_ltsw_cosq_mod_dest_t *mod_dest)
{
    int rv;
    SHR_FUNC_ENTER(unit);
    MOD_INIT_CHECK(unit);

    MOD_LOCK(unit);
    rv = mbcm_ltsw_cosq_mod_dest_delete(unit, mod_dest);
    MOD_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_cosq_mod_dest_get(int unit, bcmi_ltsw_cosq_mod_dest_t *mod_dest)
{
    int rv;
    SHR_FUNC_ENTER(unit);
    MOD_INIT_CHECK(unit);

    SHR_NULL_CHECK(mod_dest, SHR_E_PARAM);

    MOD_LOCK(unit);
    rv = mbcm_ltsw_cosq_mod_dest_get(unit, mod_dest);
    MOD_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_cosq_mod_instance_id_get(int unit, int *instance_id)
{
    int rv;
    SHR_FUNC_ENTER(unit);
    MOD_INIT_CHECK(unit);

    SHR_NULL_CHECK(instance_id, SHR_E_PARAM);

    MOD_LOCK(unit);
    rv = mbcm_ltsw_cosq_mod_instance_id_get(unit, instance_id);
    MOD_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_cosq_mod_control_set(int unit, bcm_cosq_mod_control_t *control)
{
    int rv;
    SHR_FUNC_ENTER(unit);
    MOD_INIT_CHECK(unit);

    SHR_NULL_CHECK(control, SHR_E_PARAM);

    MOD_LOCK(unit);
    rv = mbcm_ltsw_cosq_mod_control_set(unit, control);
    MOD_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_cosq_mod_control_get(int unit, bcm_cosq_mod_control_t *control)
{
    int rv;
    SHR_FUNC_ENTER(unit);
    MOD_INIT_CHECK(unit);

    SHR_NULL_CHECK(control, SHR_E_PARAM);

    MOD_LOCK(unit);
    rv = mbcm_ltsw_cosq_mod_control_get(unit, control);
    MOD_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_cosq_mod_profile_create(int unit, uint32_t options, int *profile_id)
{
    int rv;
    SHR_FUNC_ENTER(unit);
    MOD_INIT_CHECK(unit);

    SHR_NULL_CHECK(profile_id, SHR_E_PARAM);

    MOD_LOCK(unit);
    rv = mbcm_ltsw_cosq_mod_profile_create(unit, options, profile_id);
    MOD_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_cosq_mod_profile_destroy(int unit, int profile_id)
{
    int rv;
    SHR_FUNC_ENTER(unit);
    MOD_INIT_CHECK(unit);

    MOD_LOCK(unit);
    rv = mbcm_ltsw_cosq_mod_profile_destroy(unit, profile_id);
    MOD_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_cosq_mod_profile_set(int unit, int profile_id,
                               bcm_cosq_mod_profile_t *profile)
{
    int rv;
    SHR_FUNC_ENTER(unit);
    MOD_INIT_CHECK(unit);

    SHR_NULL_CHECK(profile, SHR_E_PARAM);

    MOD_LOCK(unit);
    rv = mbcm_ltsw_cosq_mod_profile_set(unit, profile_id, profile);
    MOD_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_cosq_mod_profile_get(int unit, int profile_id,
                              bcm_cosq_mod_profile_t *profile)
{
    int rv;
    SHR_FUNC_ENTER(unit);
    MOD_INIT_CHECK(unit);

    SHR_NULL_CHECK(profile, SHR_E_PARAM);

    MOD_LOCK(unit);
    rv = mbcm_ltsw_cosq_mod_profile_get(unit, profile_id, profile);
    MOD_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_cosq_mod_stat_set(int unit, bcm_cosq_object_id_t *id,
                               bcm_cosq_mod_stat_t stat, uint64 value)
{
    int rv;
    SHR_FUNC_ENTER(unit);
    MOD_INIT_CHECK(unit);

    SHR_NULL_CHECK(id, SHR_E_PARAM);

    MOD_LOCK(unit);
    rv = mbcm_ltsw_cosq_mod_stat_set(unit, id, stat, value);
    MOD_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_cosq_mod_stat_get(int unit, bcm_cosq_object_id_t *id,
                               bcm_cosq_mod_stat_t stat, uint64 *value)
{
    int rv;
    SHR_FUNC_ENTER(unit);
    MOD_INIT_CHECK(unit);

    SHR_NULL_CHECK(id, SHR_E_PARAM);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    MOD_LOCK(unit);
    rv = mbcm_ltsw_cosq_mod_stat_get(unit, id, stat, value);
    MOD_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

