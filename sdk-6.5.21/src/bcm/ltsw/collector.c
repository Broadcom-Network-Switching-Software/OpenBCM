/*! \file collector.c
 *
 * Collector Module Emulator above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>
#include <bcm/collector.h>

#include <bcm_int/control.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/collector.h>
#include <bcm_int/ltsw/collector_int.h>
#include <bcm_int/ltsw/mbcm/collector.h>

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_COLLECTOR

/* Collector control db. */
typedef struct ltsw_collector_info_s {
    /* Collector module initialized flag. */
    int initialized;

    /* Collector module lock. */
    sal_mutex_t lock;
} ltsw_collector_info_t;

static ltsw_collector_info_t ltsw_collector_info[BCM_MAX_NUM_UNITS];

#define COLLECTOR_INFO(unit) (&ltsw_collector_info[unit])

#define COLLECTOR_INIT_CHECK(unit) \
    do { \
        if (COLLECTOR_INFO(unit)->initialized == 0) { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while(0)

#define COLLECTOR_LOCK(unit) \
    do { \
        if (COLLECTOR_INFO(unit)->lock) { \
            sal_mutex_take(COLLECTOR_INFO(unit)->lock, SAL_MUTEX_FOREVER); \
        } else { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while(0)

#define COLLECTOR_UNLOCK(unit) \
    do { \
        if (COLLECTOR_INFO(unit)->lock) { \
            sal_mutex_give(COLLECTOR_INFO(unit)->lock); \
        } else { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while(0)

/******************************************************************************
 * Private functions
 */

/*******************************************************************************
 * Public cross-module functions
 */

/*!
 * \brief Increment/Decrement reference count for an export profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] id Export profile ID.
 * \param [in] update Reference count update (+/-).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_collector_export_profile_ref_count_update(
    int unit,
    int id,
    int update)
{
    bool collector_lock = false;

    SHR_FUNC_ENTER(unit);
    COLLECTOR_INIT_CHECK(unit);

    COLLECTOR_LOCK(unit);
    collector_lock = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_collector_export_profile_ref_count_update(unit, id,
                                                             update));

exit:
    if (collector_lock) {
        COLLECTOR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Increment/Decrement reference count for a collector.
 *
 * \param [in] unit Unit Number.
 * \param [in] id Collector ID.
 * \param [in] update Reference count update (+/-).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_collector_ref_count_update(
    int unit,
    bcm_collector_t id,
    int update)
{
    bool collector_lock = false;

    SHR_FUNC_ENTER(unit);
    COLLECTOR_INIT_CHECK(unit);

    COLLECTOR_LOCK(unit);
    collector_lock = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_collector_ref_count_update(unit, id, update));

exit:
    if (collector_lock) {
        COLLECTOR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Increment/Decrement reference count for a collector.
 *
 * \param [in] unit Unit Number.
 * \param [in] id Collector ID.
 * \param [in] update Reference count update (+/-).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_collector_user_update(
    int unit,
    bcm_collector_t id,
    bcmi_ltsw_collector_export_app_t user)
{
    bool collector_lock = false;

    SHR_FUNC_ENTER(unit);
    COLLECTOR_INIT_CHECK(unit);

    COLLECTOR_LOCK(unit);
    collector_lock = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_collector_user_update(unit, id, user));

exit:
    if (collector_lock) {
        COLLECTOR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Check if collector ID is used by other user.
 *
 * \param [in] unit Unit Number.
 * \param [in] id Collector ID.
 * \param [in] my_user The user type of calling app.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_collector_check_user_is_other(
    int unit,
    bcm_collector_t id,
    bcmi_ltsw_collector_export_app_t my_user)
{
    bool collector_lock = false;

    SHR_FUNC_ENTER(unit);
    COLLECTOR_INIT_CHECK(unit);

    COLLECTOR_LOCK(unit);
    collector_lock = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_collector_check_user_is_other(unit, id, my_user));

exit:
    if (collector_lock) {
        COLLECTOR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the internal ID (the ID that is programmed in LT) and the type of a collector.
 *
 * \param [in] unit Unit Number.
 * \param [in] id Collector ID.
 * \param [out] internal_id Internal ID of the collector.
 * \param [out] type Type of the collector.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_collector_internal_id_get(
    int unit,
    bcm_collector_t id,
    uint32_t *internal_id,
    bcmi_ltsw_collector_transport_type_t *type)
{
    bool collector_lock = false;

    SHR_FUNC_ENTER(unit);
    COLLECTOR_INIT_CHECK(unit);

    SHR_NULL_CHECK(internal_id, SHR_E_PARAM);
    SHR_NULL_CHECK(type, SHR_E_PARAM);

    COLLECTOR_LOCK(unit);
    collector_lock = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_collector_internal_id_get(unit, id, internal_id, type));

exit:
    if (collector_lock) {
        COLLECTOR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the external ID of a collector.
 *
 * \param [in] unit Unit Number.
 * \param [in] internal_id Internal ID of the collector.
 * \param [in] type Type of the collector.
 * \param [out] id External ID of the collector.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_collector_external_id_get(
    int unit,
    uint32_t internal_id,
    bcmi_ltsw_collector_transport_type_t type,
    bcm_collector_t *id)
{
    bool collector_lock = false;

    SHR_FUNC_ENTER(unit);
    COLLECTOR_INIT_CHECK(unit);

    SHR_NULL_CHECK(id, SHR_E_PARAM);

    COLLECTOR_LOCK(unit);
    collector_lock = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_collector_external_id_get(unit, internal_id, type, id));

exit:
    if (collector_lock) {
        COLLECTOR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public functions
 */

/*!
 * \brief De-initialize the Collector module.
 *
 * Free Collector module resources.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_collector_detach(int unit)
{
    bool collector_lock = false;

    SHR_FUNC_ENTER(unit);

    if (COLLECTOR_INFO(unit)->initialized == 0) {
        SHR_EXIT();
    }

    COLLECTOR_LOCK(unit);
    collector_lock = true;

    COLLECTOR_INFO(unit)->initialized = 0;

exit:
    if (collector_lock) {
        COLLECTOR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the Collector module.
 *
 * Initialize Collector database and Collector related logical tables.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_collector_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (COLLECTOR_INFO(unit)->initialized) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_collector_detach(unit));
    }

    if (COLLECTOR_INFO(unit)->lock == NULL) {
        COLLECTOR_INFO(unit)->lock = sal_mutex_create("bcmLtswCollectorMutex");
        SHR_NULL_CHECK(COLLECTOR_INFO(unit)->lock, SHR_E_MEMORY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_collector_init(unit));

    COLLECTOR_INFO(unit)->initialized = 1;

exit:
    if (SHR_FUNC_ERR()) {
        if (COLLECTOR_INFO(unit)->lock != NULL) {
            sal_mutex_destroy(COLLECTOR_INFO(unit)->lock);
            COLLECTOR_INFO(unit)->lock = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a collector export profile with given export profile info.
 *
 * \param [in] unit Unit Number.
 * \param [in] options Collector export profile create options.
 * \param [inout] id Collector ID.
 * \param [in] export_profile_info Collector export profile info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_collector_export_profile_create(
    int unit,
    uint32_t options,
    int *id,
    bcm_collector_export_profile_t *export_profile_info)
{
    bool collector_lock = false;

    SHR_FUNC_ENTER(unit);

    COLLECTOR_INIT_CHECK(unit);

    SHR_NULL_CHECK(id, SHR_E_PARAM);
    SHR_NULL_CHECK(export_profile_info, SHR_E_PARAM);

    COLLECTOR_LOCK(unit);
    collector_lock = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_collector_export_profile_create(unit, options, id,
                                                   export_profile_info));

exit:
    if (collector_lock) {
        COLLECTOR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a collector export profile information.
 *
 * \param [in] unit Unit Number.
 * \param [in] id Collector ID.
 * \param [out] export_profile_info Collector export profile info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_collector_export_profile_get(
    int unit,
    int id,
    bcm_collector_export_profile_t *export_profile_info)
{
    bool collector_lock = false;

    SHR_FUNC_ENTER(unit);

    COLLECTOR_INIT_CHECK(unit);

    SHR_NULL_CHECK(export_profile_info, SHR_E_PARAM);

    COLLECTOR_LOCK(unit);
    collector_lock = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_collector_export_profile_get(unit, id, export_profile_info));

exit:
    if (collector_lock) {
        COLLECTOR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the list of all collector export profile IDs configured
 *
 * \param [in] unit Unit Number.
 * \param [in] max_size Size of the collector list array.
 * \param [out] export_profile_ids_list Collector export profile ID list.
 * \param [out] list_size Number of elements in the list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_collector_export_profile_ids_get_all(
    int unit,
    int max_size,
    int *export_profile_ids_list,
    int *list_size)
{
    bool collector_lock = false;

    SHR_FUNC_ENTER(unit);

    COLLECTOR_INIT_CHECK(unit);

    SHR_NULL_CHECK(export_profile_ids_list, SHR_E_PARAM);
    SHR_NULL_CHECK(list_size, SHR_E_PARAM);

    COLLECTOR_LOCK(unit);
    collector_lock = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_collector_export_profile_ids_get_all(unit, max_size,
                                                        export_profile_ids_list,
                                                        list_size));

exit:
    if (collector_lock) {
        COLLECTOR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a collector export profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] id Collector expor profile ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_collector_export_profile_destroy(int unit, int id)
{
    bool collector_lock = false;

    SHR_FUNC_ENTER(unit);

    COLLECTOR_INIT_CHECK(unit);

    COLLECTOR_LOCK(unit);
    collector_lock = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_collector_export_profile_destroy(unit, id));

exit:
    if (collector_lock) {
        COLLECTOR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a collector with given collector information.
 *
 * \param [in] unit Unit Number.
 * \param [in] options Collector create options.
 * \param [inout] id Collector ID.
 * \param [in] collector_info Collector info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_collector_create(
    int unit,
    uint32 options,
    bcm_collector_t *id,
    bcm_collector_info_t *collector_info)
{
    bool collector_lock = false;

    SHR_FUNC_ENTER(unit);

    COLLECTOR_INIT_CHECK(unit);

    SHR_NULL_CHECK(id, SHR_E_PARAM);
    SHR_NULL_CHECK(collector_info, SHR_E_PARAM);

    COLLECTOR_LOCK(unit);
    collector_lock = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_collector_create(unit, options, id, collector_info));

exit:
    if (collector_lock) {
        COLLECTOR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a collector information.
 *
 * \param [in] unit Unit Number.
 * \param [in] id Collector ID.
 * \param [out] collector_info Collector info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_collector_get(
    int unit,
    bcm_collector_t id,
    bcm_collector_info_t *collector_info)
{
    bool collector_lock = false;

    SHR_FUNC_ENTER(unit);

    COLLECTOR_INIT_CHECK(unit);

    SHR_NULL_CHECK(collector_info, SHR_E_PARAM);

    COLLECTOR_LOCK(unit);
    collector_lock = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_collector_get(unit, id, collector_info));

exit:
    if (collector_lock) {
        COLLECTOR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the list of all collector IDs configured.
 *
 * \param [in] unit Unit Number.
 * \param [in] max_size Size of the collector list array.
 * \param [out] collector_list Collector ID list.
 * \param [out] list_size Number of elements in the list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_collector_get_all(
    int unit,
    int max_size,
    bcm_collector_t *collector_list,
    int *list_size)
{
    bool collector_lock = false;

    SHR_FUNC_ENTER(unit);

    COLLECTOR_INIT_CHECK(unit);

    SHR_NULL_CHECK(collector_list, SHR_E_PARAM);
    SHR_NULL_CHECK(list_size, SHR_E_PARAM);

    COLLECTOR_LOCK(unit);
    collector_lock = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_collector_get_all(unit, max_size, collector_list,
                                     list_size));

exit:
    if (collector_lock) {
        COLLECTOR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a collector.
 *
 * \param [in] unit Unit Number.
 * \param [in] id Collector ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_collector_destroy(
    int unit,
    bcm_collector_t id)
{
    bool collector_lock = false;

    SHR_FUNC_ENTER(unit);

    COLLECTOR_INIT_CHECK(unit);

    COLLECTOR_LOCK(unit);
    collector_lock = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_collector_destroy(unit, id));

exit:
    if (collector_lock) {
        COLLECTOR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

