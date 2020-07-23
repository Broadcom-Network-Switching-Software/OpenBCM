/*! \file int.c
 *
 * INT management.
 * This file contains the management for INT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#if defined(INCLUDE_INT)

#include <bcm/int.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/int.h>
#include <bcm_int/ltsw/int_ifa.h>
#include <bcm_int/ltsw/int_ioam.h>
#include <bcm_int/ltsw/int_dp.h>
#include <bcm_int/ltsw/index_table_mgmt.h>
#include <bcm_int/ltsw/mbcm/int.h>
#include <bcm_int/ltsw/feature.h>

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_INT

#define BCMI_LTSW_INT_ID_ZERO    0

typedef struct ltsw_int_info_s {
    /* INT initialized flag. */
    int initialized;

    /*! INT module lock. */
    sal_mutex_t int_lock;
} ltsw_int_info_t;

static ltsw_int_info_t ltsw_int_info[BCM_MAX_NUM_UNITS];
#define INT_INFO(unit) (&ltsw_int_info[unit])

/*! Cause a routine to return SHR_E_INIT if INT subsystem is not initialized. */
#define INT_INIT_CHECK(unit) \
    do { \
        if (INT_INFO(unit)->initialized == 0) { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while (0) \

/*! INT mutex lock. */
#define INT_LOCK(unit) \
    do { \
        if (INT_INFO(unit)->int_lock) { \
            sal_mutex_take(INT_INFO(unit)->int_lock, SAL_MUTEX_FOREVER); \
        } else { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        }\
    } while (0)

/*! INT mutex unlock. */
#define INT_UNLOCK(unit) \
    do { \
        if (INT_INFO(unit)->int_lock) { \
            sal_mutex_give(INT_INFO(unit)->int_lock); \
        } else { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while (0)

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Clean up INT module software database.
 *
 * \param [in]  unit Unit number.
 */
static void
ltsw_int_info_cleanup(int unit)
{
    if (ltsw_int_info[unit].int_lock) {
        sal_mutex_destroy(ltsw_int_info[unit].int_lock);
        ltsw_int_info[unit].int_lock = NULL;
    }
}

/*!
 * \brief Initialize Multicast module software database.
 *
 * \param [in]  unit          Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
static int
ltsw_int_info_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_int_info[unit].int_lock == NULL) {
        ltsw_int_info[unit].int_lock
            = sal_mutex_create("bcmLtswIntMutex");
       SHR_NULL_CHECK(ltsw_int_info[unit].int_lock, SHR_E_MEMORY);
    }

exit:
    if (SHR_FUNC_ERR()) {
        ltsw_int_info_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a vetor match for INT.
 *
 * \param [in] unit Unit number.
 * \param [in] options Add options
 * \param [in] vetor_match Vector match criteria.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_vector_match_add(int unit, uint32 options,
                         bcmi_ltsw_int_vector_match_t *vector_match)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_NULL_CHECK(vector_match, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_vector_match_add(unit, options, vector_match));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the vector match for INT.
 *
 * \param [in] unit Unit number.
 * \param [out] vector_match Vector match criteria.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_vector_match_get(int unit, bcmi_ltsw_int_vector_match_t *vector_match)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_NULL_CHECK(vector_match, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_vector_match_get(unit, vector_match));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete the vector match for INT.
 *
 * \param [in] unit Unit number.
 * \param [in] vector_match Vector match criteria.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_vector_match_delete(int unit,
                             bcmi_ltsw_int_vector_match_t *vector_match)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_NULL_CHECK(vector_match, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_vector_match_del(unit, vector_match));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all vector matches for INT.
 *
 * \param [in] unit Unit number.
 * \param [in] type Vector match type.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_vector_match_delete_all(int unit,
                                 bcmi_ltsw_int_vector_match_type_t type)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_vector_match_del_all(unit, type));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all vector matches for IFA.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn A pointer to the callback function to call for each vector match.
 * \param [in] cb A pointer to the callback function to call for construct ifa info from int.
 * \param [in] user_data Pointer to user data to supply in the callback.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_vector_match_ifa_traverse(
    int unit,
    bcm_int_ifa_vector_match_traverse_cb trav_fn,
    bcmi_ltsw_int_ifa_vector_match_from_int_cb_f cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_vector_match_ifa_traverse(unit, trav_fn, cb, user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all vector matches for IOAM.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn A pointer to the callback function to call for each vector match.
 * \param [in] cb A pointer to the callback function to call for construct ioam info from int.
 * \param [in] user_data Pointer to user data to supply in the callback.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_vector_match_ioam_traverse(
    int unit,
    bcm_int_ioam_vector_match_traverse_cb trav_fn,
    bcmi_ltsw_int_ioam_vector_match_from_int_cb_f cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_vector_match_ioam_traverse(unit, trav_fn, cb,
                                                  user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all vector matches for INT-DP.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn A pointer to the callback function to call for each vector match.
 * \param [in] cb A pointer to the callback function to call for construct int-dp info from int.
 * \param [in] user_data Pointer to user data to supply in the callback.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_vector_match_dp_traverse(
    int unit,
    bcm_int_dp_vector_match_traverse_cb trav_fn,
    bcmi_ltsw_int_dp_vector_match_from_int_cb_f cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_vector_match_dp_traverse(unit, trav_fn, cb, user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Translate INT action profile create options to shared (table management) flags.
 *
 * \param [in] unit Unit number.
 * \param [in] options INT action table create options.
 * \param [out] shr_flags Shared flags.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_action_profile_options_to_shr(
    int unit,
    uint32_t options,
    uint32_t *shr_flags)
{
    uint32 flag;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(shr_flags, SHR_E_PARAM);

    /*
     * Reserve entry Zero in INT action profile table. This reserved entry
     * will not be allocated. The purpose of this entry is that
     * any object points to this entry will be treated
     * as default.
     */
    flag = BCMI_LTSW_IDX_TBL_OP_SKIP_INDEX_ZERO;
    flag |= BCMI_LTSW_IDX_TBL_OP_MATCH_DISABLE;

    if (options & BCMI_LTSW_INT_ACTION_PROFILE_OPTIONS_REPLACE) {
       flag |= BCMI_LTSW_IDX_TBL_OP_REPLACE;
    }

    if (options & BCMI_LTSW_INT_ACTION_PROFILE_OPTIONS_WITH_ID) {
       flag |= BCMI_LTSW_IDX_TBL_OP_WITH_ID;
    }

    *shr_flags = flag;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the IFA action profile.
 *
 * \param [in] unit Unit number.
 * \param [in] options Create options.
 * \param [in/out] profile_id Action profile ID.
 * \param [in] profile Action profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_action_profile_create(
    int unit,
    uint32 options,
    int *profile_id,
    bcmi_ltsw_int_action_profile_t *profile)
{
    uint32_t shr_flags;

    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_NULL_CHECK(profile_id, SHR_E_PARAM);
    SHR_NULL_CHECK(profile, SHR_E_PARAM);

    if (options & BCMI_LTSW_INT_ACTION_PROFILE_OPTIONS_WITH_ID) {
        if (*profile_id == BCMI_LTSW_INT_ID_ZERO) {
            /* Index 0 is reserved. */
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    /* Convert INT action profile specific create options to table management flags. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_int_action_profile_options_to_shr(unit, options, &shr_flags));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_action_profile_create(unit, shr_flags, profile_id,
                                             profile));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the INT action profile.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Action profile ID.
 * \param [out] profile Action profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_action_profile_get(
    int unit,
    int profile_id,
    bcmi_ltsw_int_action_profile_t *profile)
{
    uint32_t shr_flags;

    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_NULL_CHECK(profile, SHR_E_PARAM);

    /* Convert INT action profile specific create options to table management flags. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_int_action_profile_options_to_shr(unit, 0, &shr_flags));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_action_profile_get(unit, shr_flags, profile_id,
                                          profile));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the INT action profile.
 *
 * \param [in] unit Unit number.
 * \param [in] type Action profile type.
 * \param [in] profile_id Action profile ID.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_action_profile_destroy(
    int unit,
    bcmi_ltsw_int_action_profile_type_t type,
    int profile_id)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_action_profile_del(unit, 0, type, profile_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all action profiles for IFA.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn A pointer to the callback function to call for each action profile.
 * \param [in] cb A pointer to the callback function to call for construct ifa info from int.
 * \param [in] user_data Pointer to user data to supply in the callback.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_action_profile_ifa_traverse(
    int unit,
    bcm_int_ifa_action_profile_traverse_cb trav_fn,
    bcmi_ltsw_int_ifa_action_profile_from_int_cb_f cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_action_profile_ifa_traverse(unit, trav_fn, cb,
                                                   user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all action profiles for IOAM.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn A pointer to the callback function to call for each action profile.
 * \param [in] cb A pointer to the callback function to call for construct ifa info from int.
 * \param [in] user_data Pointer to user data to supply in the callback.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_action_profile_ioam_traverse(
    int unit,
    bcm_int_ioam_action_profile_traverse_cb trav_fn,
    bcmi_ltsw_int_ioam_action_profile_from_int_cb_f cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_action_profile_ioam_traverse(unit, trav_fn, cb,
                                                    user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all action profiles for INT-DP.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn A pointer to the callback function to call for each action profile.
 * \param [in] cb A pointer to the callback function to call for construct ifa info from int.
 * \param [in] user_data Pointer to user data to supply in the callback.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_action_profile_dp_traverse(
    int unit,
    bcm_int_dp_action_profile_traverse_cb trav_fn,
    bcmi_ltsw_int_dp_action_profile_from_int_cb_f cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_action_profile_dp_traverse(unit, trav_fn, cb,
                                                  user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Translate INT opaque data profile create options to
 *             shared (table management) flags.
 *
 * \param [in] unit Unit number.
 * \param [in] options Create options.
 * \param [out] shr_flags Shared flags.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_opaque_data_profile_options_to_shr(
    int unit,
    uint32_t options,
    uint32_t *shr_flags)
{
    uint32 flag;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(shr_flags, SHR_E_PARAM);

    /*
     * Reserve entry Zero in INT opaque data profile table. This reserved entry
     * will not be allocated. The purpose of this entry is that
     * any object points to this entry will be treated
     * as default.
     */
    flag = BCMI_LTSW_IDX_TBL_OP_SKIP_INDEX_ZERO;
    flag |= BCMI_LTSW_IDX_TBL_OP_MATCH_DISABLE;

    if (options & BCM_INT_OPAQUE_DATA_PROFILE_OPTIONS_REPLACE) {
       flag |= BCMI_LTSW_IDX_TBL_OP_REPLACE;
    }

    if (options & BCM_INT_OPAQUE_DATA_PROFILE_OPTIONS_WITH_ID) {
       flag |= BCMI_LTSW_IDX_TBL_OP_WITH_ID;
    }

    *shr_flags = flag;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the opaque data profile for INT metadata construction.
 *
 * \param [in] unit Unit Number.
 * \param [in] options Create options.
 * \param [in/out] profile_id Opaque data profile ID.
 * \param [in] profile Opaque data profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_opaque_data_profile_create(
    int unit,
    uint32 options,
    int *profile_id,
    bcm_int_opaque_data_profile_t *profile)
{
    uint32_t shr_flags;

    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_NULL_CHECK(profile_id, SHR_E_PARAM);
    SHR_NULL_CHECK(profile, SHR_E_PARAM);

    if (options & BCM_INT_OPAQUE_DATA_PROFILE_OPTIONS_WITH_ID) {
        if (*profile_id == BCMI_LTSW_INT_ID_ZERO) {
            /* Index 0 is reserved. */
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    /* Convert INT opaque data profile specific create options to table management flags. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_int_opaque_data_profile_options_to_shr(unit, options,
                                                     &shr_flags));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_opaque_data_profile_create(unit, shr_flags, profile_id,
                                                  profile));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the opaque data profile for INT metadata construction.
 *
 * \param [in] unit Unit Number
 * \param [in] profile_id Opaque data profile ID.
 * \param [out] profile Opaque data profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_opaque_data_profile_get(
    int unit,
    int profile_id,
    bcm_int_opaque_data_profile_t *profile)
{
    uint32_t shr_flags;

    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_NULL_CHECK(profile, SHR_E_PARAM);

    /* Convert INT opaque data profile specific create options to table management flags. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_int_opaque_data_profile_options_to_shr(unit, 0, &shr_flags));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_opaque_data_profile_get(unit, shr_flags, profile_id,
                                               profile));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the opaque data profile for INT metadata construction.
 *
 * \param [in] unit Unit Number
 * \param [in] profile_id Opaque data profile ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_opaque_data_profile_destroy(
    int unit,
    int profile_id)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_opaque_data_profile_del(unit, 0, profile_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all opaque data profiles INT metadata construction.
 *
 * \param [in] unit Unit number.
 * \param [in] cb A pointer to the callback function to call for each opaque data profile.
 * \param [in] user_data Pointer to user data to supply in the callback.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_opaque_data_profile_traverse(
    int unit,
    bcm_int_opaque_data_profile_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_opaque_data_profile_traverse(unit, cb, user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set various configurations for INT cosq stat.
 *
 * \param [in] unit Unit Number
 * \param [in] config Cosq stat configurations.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_cosq_stat_config_set(
    int unit,
    bcm_int_cosq_stat_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_NULL_CHECK(config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_cosq_stat_config_set(unit, config));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get various configurations for INT cosq stat.
 *
 * \param [in] unit Unit Number
 * \param [out] config Cosq stat configurations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_cosq_stat_config_get(
    int unit,
    bcm_int_cosq_stat_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_NULL_CHECK(config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_cosq_stat_config_get(unit, config));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Translate INT metadata profile create options to shared (table management) flags.
 *
 * \param [in] unit Unit number.
 * \param [in] options INT metadata table create options.
 * \param [out] shr_flags Shared flags.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_metadata_profile_options_to_shr(
    int unit,
    uint32_t options,
    uint32_t *shr_flags)
{
    uint32 flag;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(shr_flags, SHR_E_PARAM);

    /*
     * Reserve entry Zero in INT metadata profile table. This reserved entry
     * will not be allocated. The purpose of this entry is that
     * any object points to this entry will be treated
     * as default.
     */
    flag = BCMI_LTSW_IDX_TBL_OP_SKIP_INDEX_ZERO;
    flag |= BCMI_LTSW_IDX_TBL_OP_MATCH_DISABLE;

    if (options & BCM_INT_METADATA_PROFILE_OPTIONS_WITH_ID) {
       flag |= BCMI_LTSW_IDX_TBL_OP_WITH_ID;
    }

    *shr_flags = flag;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the metadata profile.
 *
 * \param [in] unit Unit Number
 * \param [in] options Create options
 * \param [in/out] profile_id Metadata profile ID
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_metadata_profile_create(
    int unit,
    uint32 options,
    int *profile_id)
{
    uint32_t shr_flags;

    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_NULL_CHECK(profile_id, SHR_E_PARAM);

    if (options & BCM_INT_METADATA_PROFILE_OPTIONS_WITH_ID) {
        if (*profile_id == BCMI_LTSW_INT_ID_ZERO) {
            /* Index 0 is reserved. */
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    /* Convert INTmetadata profile specific create options to table management flags. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_int_metadata_profile_options_to_shr(unit, options, &shr_flags));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_metadata_profile_create(unit, shr_flags, profile_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the metadata profile.
 *
 * \param [in] unit Unit Number
 * \param [in] profile_id Metadata profile ID
 * \param [in] array_size Metadata field array size
 * \param [out] entry_array Metadata field entry array
 * \param [out] field_count Metadata field count
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_metadata_profile_get(
    int unit,
    int profile_id,
    int array_size,
    bcm_int_metadata_field_entry_t *entry_array,
    int *field_count)
{
    uint32_t shr_flags;

    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_NULL_CHECK(field_count, SHR_E_PARAM);

    if (array_size < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (array_size > 0) {
        SHR_NULL_CHECK(entry_array, SHR_E_PARAM);
    }

    /* Convert INT metadata profile specific create options to table management flags. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_int_metadata_profile_options_to_shr(unit, 0, &shr_flags));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_metadata_profile_get(unit, shr_flags, profile_id,
                                            array_size, entry_array,
                                            field_count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the metadata profile.
 *
 * \param [in] unit Unit Number
 * \param [in] profile_id Metadata profile ID
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */

static int
ltsw_int_metadata_profile_destroy(
    int unit,
    int profile_id)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_metadata_profile_del(unit, 0, profile_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all metadata profiles.
 *
 * \param [in] unit Unit number.
 * \param [in] cb A pointer to the callback function to call for each metadata profile.
 * \param [in] user_data Pointer to user data to supply in the callback.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_metadata_profile_traverse(
    int unit,
    bcm_int_metadata_profile_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_metadata_profile_traverse(unit, cb, user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add the metadata field select and construct to the metadata profile.
 *
 * \param [in] unit Unit Number
 * \param [in] profile_id Metadata profile ID
 * \param [in] field_select Metadata field select
 * \param [in] field_construct Metadata field construct
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_metadata_field_add(
    int unit,
    int profile_id,
    bcm_int_metadata_field_entry_t *field_entry)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_NULL_CHECK(field_entry, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_metadata_field_add(unit, profile_id, field_entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete the metadata field from the metadata profile.
 *
 * \param [in] unit Unit Number
 * \param [in] profile_id Metadata profile ID
 * \param [in] field_entry Metadata field entry
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_metadata_field_delete(
    int unit,
    int profile_id,
    bcm_int_metadata_field_entry_t  *field_entry)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_NULL_CHECK(field_entry, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_metadata_field_delete(unit, profile_id, field_entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all the metadata field from the metadata profile.
 *
 * \param [in] unit Unit Number
 * \param [in] profile_id Metadata profile ID
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_metadata_field_delete_all(
    int unit,
    int profile_id)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_metadata_field_delete_all(unit, profile_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the field select.
 *
 * \param [in] unit Unit Number
 * \param [in] field Metadata field
 * \param [out] field_select_info Metadata field select information
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_metadata_field_select_info_get(
    int unit,
    bcm_int_metadata_field_t field,
    bcm_int_metadata_field_select_info_t *field_select_info)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_NULL_CHECK(field_select_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_metadata_field_select_info_get(unit, field,
                                                      field_select_info));

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

int
bcmint_int_md_field_info_get(
    int unit,
    bcmint_int_md_field_info_t **field_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_md_field_info_get(unit, field_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Init INT module.
 *
 * \param [in] unit              Unit number.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval SHR_E_MEMORY          Out of memory.
 */
int
bcm_ltsw_int_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (INT_INFO(unit)->initialized) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_int_detach(unit));
    }

    /* Initialize INT module software database. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_int_info_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_init(unit));

    INT_INFO(unit)->initialized = TRUE;

    /* Initialize IFA software database. */
    if (ltsw_feature(unit, LTSW_FT_INT_IFA)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_int_ifa_init(unit));
    }

    /* Initialize IOAM software database. */
    if (ltsw_feature(unit, LTSW_FT_INT_IOAM)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_int_ioam_init(unit));
    }

    /* Initialize INT-DP software database. */
    if (ltsw_feature(unit, LTSW_FT_INT_DP)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_int_dp_init(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach INT module.
 *
 * \param [in] unit              Unit number
 *
 * \retval SHR_E_NONE            No errors.
 */
int
bcm_ltsw_int_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (INT_INFO(unit)->initialized == FALSE) {
        SHR_EXIT();
    }

    /* De-initialize IFA module software database. */
    if (ltsw_feature(unit, LTSW_FT_INT_IFA)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_int_ifa_deinit(unit));
    }

    /* De-initialize IOAM module software database. */
    if (ltsw_feature(unit, LTSW_FT_INT_IOAM)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_int_ioam_deinit(unit));
    }

    /* De-initialize INT-DP module software database. */
    if (ltsw_feature(unit, LTSW_FT_INT_DP)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_int_dp_deinit(unit));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_detach(unit));

    INT_INFO(unit)->initialized = FALSE;

    ltsw_int_info_cleanup(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Specify INT switch control behaviors.
 *
 * \param [in] unit Unit number.
 * \param [in] control The desired configuration parameter to modify.
 * \param [in] value The value with which to set the parameter.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_int_control_set(
    int unit,
    bcmi_ltsw_int_control_t control,
    uint32_t value)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_int_control_set(unit, control, value));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve INT switch control behaviors.
 *
 * \param [in] unit Unit number.
 * \param [in] control The desired configuration parameter to retrieve.
 * \param [out] value Pointer to where the retrieved value will be written.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_int_control_get(
    int unit,
    bcmi_ltsw_int_control_t control,
    uint32_t *value)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_int_control_get(unit, control, value));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a match for INT.
 *
 * \param [in] unit Unit number.
 * \param [in] options Add options.
 * \param [in] vector_match Vector match criteria.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_int_vector_match_add(int unit, uint32 options,
                               bcmi_ltsw_int_vector_match_t *vector_match)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_vector_match_add(unit, options, vector_match));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the vector match for INT.
 *
 * \param [in] unit Unit number.
 * \param [out] vector_match Vector match criteria.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_int_vector_match_get(int unit,
                               bcmi_ltsw_int_vector_match_t *vector_match)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_vector_match_get(unit, vector_match));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete the vector match for INT.
 *
 * \param [in] unit Unit number.
 * \param [in] vector_match Vector match criteria.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_int_vector_match_delete(int unit,
                                  bcmi_ltsw_int_vector_match_t *vector_match)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_vector_match_delete(unit, vector_match));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all vector matches for IFA.
 *
 * \param [in] unit Unit number.
 * \param [in] type Vector match type.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_int_vector_match_delete_all(int unit,
                                      bcmi_ltsw_int_vector_match_type_t type)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_vector_match_delete_all(unit, type));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all vector matches for IFA.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn A pointer to the callback function to call for each vector match.
 * \param [in] cb A pointer to the callback function to call for construct ifa info from int.
 * \param [in] user_data Pointer to user data to supply in the callback.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_int_vector_match_ifa_traverse(
    int unit,
    bcm_int_ifa_vector_match_traverse_cb trav_fn,
    bcmi_ltsw_int_ifa_vector_match_from_int_cb_f cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_vector_match_ifa_traverse(unit, trav_fn, cb, user_data));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all vector matches for IOAM.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn A pointer to the callback function to call for each vector match.
 * \param [in] cb A pointer to the callback function to call for construct ioam info from int.
 * \param [in] user_data Pointer to user data to supply in the callback.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_int_vector_match_ioam_traverse(
    int unit,
    bcm_int_ioam_vector_match_traverse_cb trav_fn,
    bcmi_ltsw_int_ioam_vector_match_from_int_cb_f cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_vector_match_ioam_traverse(unit, trav_fn, cb, user_data));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all vector matches for INT-DP.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn A pointer to the callback function to call for each vector match.
 * \param [in] cb A pointer to the callback function to call for construct int-dp info from int.
 * \param [in] user_data Pointer to user data to supply in the callback.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_int_vector_match_dp_traverse(
    int unit,
    bcm_int_dp_vector_match_traverse_cb trav_fn,
    bcmi_ltsw_int_dp_vector_match_from_int_cb_f cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_vector_match_dp_traverse(unit, trav_fn, cb, user_data));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the INT action profile.
 *
 * \param [in] unit Unit number.
 * \param [in] options Create options
 * \param [in/out] profile_id Action profile ID
 * \param [in] profile Action profile
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_int_action_profile_create(
    int unit,
    uint32 options,
    int *profile_id,
    bcmi_ltsw_int_action_profile_t *profile)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_action_profile_create(unit, options, profile_id, profile));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the INT action profile.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Action profile ID
 * \param [out] profile Action profile
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_int_action_profile_get(
    int unit,
    int profile_id,
    bcmi_ltsw_int_action_profile_t *profile)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_action_profile_get(unit, profile_id, profile));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the INT action profile.
 *
 * \param [in] unit Unit number.
 * \param [in] type Action profile type
 * \param [in] profile_id Action profile ID
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_int_action_profile_destroy(
    int unit,
    bcmi_ltsw_int_action_profile_type_t type,
    int profile_id)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_action_profile_destroy(unit, type, profile_id));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all action profiles for IFA.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn A pointer to the callback function to call for each action profile.
 * \param [in] cb A pointer to the callback function to call for construct ifa info from int.
 * \param [in] user_data Pointer to user data to supply in the callback.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_int_action_profile_ifa_traverse(
    int unit,
    bcm_int_ifa_action_profile_traverse_cb trav_fn,
    bcmi_ltsw_int_ifa_action_profile_from_int_cb_f cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_action_profile_ifa_traverse(unit, trav_fn, cb, user_data));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all action profiles for IOAM.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn A pointer to the callback function to call for each action profile.
 * \param [in] cb A pointer to the callback function to call for construct ioam info from int.
 * \param [in] user_data Pointer to user data to supply in the callback.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_int_action_profile_ioam_traverse(
    int unit,
    bcm_int_ioam_action_profile_traverse_cb trav_fn,
    bcmi_ltsw_int_ioam_action_profile_from_int_cb_f cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_action_profile_ioam_traverse(unit, trav_fn, cb, user_data));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all action profiles for INT-DP.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn A pointer to the callback function to call for each action profile.
 * \param [in] cb A pointer to the callback function to call for construct int-dp info from int.
 * \param [in] user_data Pointer to user data to supply in the callback.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_int_action_profile_dp_traverse(
    int unit,
    bcm_int_dp_action_profile_traverse_cb trav_fn,
    bcmi_ltsw_int_dp_action_profile_from_int_cb_f cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_action_profile_dp_traverse(unit, trav_fn, cb, user_data));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the opaque data profile for INT metadata construction.
 *
 * \param [in] unit Unit Number
 * \param [in] options Create options
 * \param [in/out] profile_id Opaque data profile ID
 * \param [in] profile Opaque data profile
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_int_opaque_data_profile_create(
    int unit,
    uint32 options,
    int *profile_id,
    bcm_int_opaque_data_profile_t *profile)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_opaque_data_profile_create(unit, options, profile_id,
                                             profile));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the opaque data profile for INT metadata construction.
 *
 * \param [in] unit Unit Number
 * \param [in] profile_id Opaque data profile ID
 * \param [out] profile Opaque data profile
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_int_opaque_data_profile_get(
    int unit,
    int profile_id,
    bcm_int_opaque_data_profile_t *profile)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_opaque_data_profile_get(unit, profile_id, profile));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the opaque data profile for INT metadata construction.
 *
 * \param [in] unit Unit Number
 * \param [in] profile_id Opaque data profile ID
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_int_opaque_data_profile_destroy(
    int unit,
    int profile_id)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_opaque_data_profile_destroy(unit, profile_id));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all the opaque data profiles for INT metadata construction.
 *
 * \param [in] unit Unit Number
 * \param [in] cb A pointer to the callback function to call for each opaque data profile
 * \param [in] user_data Pointer to user data to supply in the callback
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_int_opaque_data_profile_traverse(
    int unit,
    bcm_int_opaque_data_profile_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_opaque_data_profile_traverse(unit, cb, user_data));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set various configurations for INT cosq stat.
 *
 * \param [in] unit Unit Number
 * \param [in] config Cosq stat configurations
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_int_cosq_stat_config_set(
    int unit,
    bcm_int_cosq_stat_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_cosq_stat_config_set(unit, config));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get various configurations for INT cosq stat.
 *
 * \param [in] unit Unit Number
 * \param [out] config Cosq stat configurations
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_int_cosq_stat_config_get(
    int unit,
    bcm_int_cosq_stat_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_cosq_stat_config_get(unit, config));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the metadata profile.
 *
 * \param [in] unit Unit number.
 * \param [in] options Create options
 * \param [in/out] profile_id Metadata profile ID
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_int_metadata_profile_create(
    int unit,
    uint32 options,
    int *profile_id)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_metadata_profile_create(unit, options, profile_id));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the metadata profile.
 *
 * \param [in] unit Unit Number
 * \param [in] profile_id Metadata profile ID
 * \param [in] array_size Metadata field array size
 * \param [out] field_select_array Metadata field select array
 * \param [out] field_construct_array Metadata field construct array
 * \param [out] field_count Metadata field count
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_int_metadata_profile_get(
    int unit,
    int profile_id,
    int array_size,
    bcm_int_metadata_field_entry_t *entry_array,
    int *field_count)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_metadata_profile_get(unit, profile_id, array_size,
                                       entry_array, field_count));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the metadata profile.
 *
 * \param [in] unit Unit Number
 * \param [in] profile_id Metadata profile ID
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_int_metadata_profile_destroy(
    int unit,
    int profile_id)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_metadata_profile_destroy(unit, profile_id));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all metadata profiles for IFA.
 *
 * \param [in] unit Unit number.
 * \param [in] cb A pointer to the callback function to call for each metadata profile.
 * \param [in] user_data Pointer to user data to supply in the callback.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_int_metadata_profile_traverse(
    int unit,
    bcm_int_metadata_profile_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_metadata_profile_traverse(unit, cb, user_data));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add the metadata field to the metadata profile.
 *
 * \param [in] unit Unit Number
 * \param [in] profile_id Metadata profile ID
 * \param [in] field_entry Metadata field entry
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_int_metadata_field_add(
    int unit,
    int profile_id,
    bcm_int_metadata_field_entry_t *field_entry)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_metadata_field_add(unit, profile_id, field_entry));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete the metadata field from the metadata profile.
 *
 * \param [in] unit Unit Number
 * \param [in] profile_id Metadata profile ID
 * \param [in] field_entry Metadata field entry
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_int_metadata_field_delete(
    int unit,
    int profile_id,
    bcm_int_metadata_field_entry_t  *field_entry)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_metadata_field_delete(unit, profile_id, field_entry));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all the metadata field from the metadata profile.
 *
 * \param [in] unit Unit Number
 * \param [in] profile_id Metadata profile ID
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_int_metadata_field_delete_all(
    int unit,
    int profile_id)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_metadata_field_delete_all(unit, profile_id));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the metadata field select information.
 *
 * \param [in] unit Unit Number
 * \param [in] field Metadata field
 * \param [out] field_select_info Metadata field select information
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_int_metadata_field_select_info_get(
    int unit,
    bcm_int_metadata_field_t field,
    bcm_int_metadata_field_select_info_t *field_select_info)
{
    SHR_FUNC_ENTER(unit);

    INT_INIT_CHECK(unit);

    INT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_int_metadata_field_select_info_get(unit, field,
                                                 field_select_info));

    INT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}
#endif /* INCLUDE_INT */

