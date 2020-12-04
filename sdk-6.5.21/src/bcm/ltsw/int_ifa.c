/*! \file int_ifa.c
 *
 * IFA management.
 * This file contains the management for IFA.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#if defined(INCLUDE_INT)
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/int.h>
#include <bcm_int/control.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/int_ifa.h>
#include <bcm_int/ltsw/int.h>
#include <bcm_int/ltsw/index_table_mgmt.h>
#include <bcm_int/ltsw/types.h>

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_INT

typedef struct ltsw_int_ifa_info_s {
    /* IFA initialized flag. */
    int initialized;

    /*! IFA module lock. */
    sal_mutex_t int_ifa_lock;
} ltsw_int_ifa_info_t;

static ltsw_int_ifa_info_t ltsw_int_ifa_info[BCM_MAX_NUM_UNITS];
#define INT_IFA_INFO(unit) (&ltsw_int_ifa_info[unit])

/*! Cause a routine to return SHR_E_INIT if tunnel subsystem is not initialized. */
#define INT_IFA_INIT_CHECK(unit) \
    do { \
        if (INT_IFA_INFO(unit)->initialized == 0) { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while (0) \

/*! IFA mutex lock. */
#define INT_IFA_LOCK(unit) \
    do { \
        if (INT_IFA_INFO(unit)->int_ifa_lock) { \
            sal_mutex_take(INT_IFA_INFO(unit)->int_ifa_lock, SAL_MUTEX_FOREVER); \
        } else { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        }\
    } while (0)

/*! IFA mutex unlock. */
#define INT_IFA_UNLOCK(unit) \
    do { \
        if (INT_IFA_INFO(unit)->int_ifa_lock) { \
            sal_mutex_give(INT_IFA_INFO(unit)->int_ifa_lock); \
        } else { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while (0)

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Set INT vector match lookup key.
 *
 * \param [in] unit Unit number.
 * \param [in] int_ifa_vector_match IFA vector match criteria.
 * \param [out] int_vector_match INT vector match criteria.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_int_ifa_vector_match_to_int_key(
    int unit,
    bcm_int_ifa_vector_match_t *int_ifa_vector_match,
    bcmi_ltsw_int_vector_match_t *int_vector_match)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(int_ifa_vector_match, SHR_E_PARAM);
    SHR_NULL_CHECK(int_vector_match, SHR_E_PARAM);

    int_vector_match->priority = int_ifa_vector_match->priority;
    int_vector_match->base_hdr_ns_id = int_ifa_vector_match->gns;
    int_vector_match->base_hdr_ns_id_mask = int_ifa_vector_match->gns_mask;
    int_vector_match->md_hdr_temp_id = int_ifa_vector_match->lns;
    int_vector_match->md_hdr_temp_id_mask = int_ifa_vector_match->lns_mask;
    int_vector_match->request_vector = int_ifa_vector_match->request_vector;
    int_vector_match->request_vector_mask =
        int_ifa_vector_match->request_vector_mask;
    int_vector_match->type = bcmiIntVectorMatchTypeIfa;

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

/*!
 * \brief Get IFA vector match criteria.
 *
 * \param [in] unit Unit number.
 * \param [in] int_vector_match INT vector match criteria.
 * \param [out] int_ifa_vector_match IFA vector match criteria.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmi_ltsw_int_ifa_vector_match_from_int(
    int unit,
    bcmi_ltsw_int_vector_match_t *int_vector_match,
    bcm_int_ifa_vector_match_t *int_ifa_vector_match)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(int_ifa_vector_match, SHR_E_PARAM);
    SHR_NULL_CHECK(int_vector_match, SHR_E_PARAM);

    int_ifa_vector_match->priority = int_vector_match->priority;
    int_ifa_vector_match->gns = int_vector_match->base_hdr_ns_id;
    int_ifa_vector_match->gns_mask = int_vector_match->base_hdr_ns_id_mask;
    int_ifa_vector_match->lns = int_vector_match->md_hdr_temp_id;
    int_ifa_vector_match->lns_mask = int_vector_match->md_hdr_temp_id_mask;
    int_ifa_vector_match->request_vector = int_vector_match->request_vector;
    int_ifa_vector_match->request_vector_mask = int_vector_match->request_vector_mask;
    int_ifa_vector_match->metadata_profile_id = int_vector_match->metadata_profile_id;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get IFA action profile.
 *
 * \param [in] unit Unit number.
 * \param [in] int_action_profile INT action profile.
 * \param [out] int_ifa_action_profile IFA action profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmi_ltsw_int_ifa_action_profile_from_int(
    int unit,
    bcmi_ltsw_int_action_profile_t *int_action_profile,
    bcm_int_ifa_action_profile_t *int_ifa_action_profile)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(int_ifa_action_profile, SHR_E_PARAM);
    SHR_NULL_CHECK(int_action_profile, SHR_E_PARAM);

    int_ifa_action_profile->timestamp_mode =
        int_action_profile->timestamp_mode;
    int_ifa_action_profile->residence_time_format =
        int_action_profile->residence_time_format;
    int_ifa_action_profile->header_operation_mode =
        int_action_profile->header_operation_mode;
    int_ifa_action_profile->metadata_profile_id =
        int_action_profile->metadata_profile_id;

    if (int_action_profile->flags &
        BCMI_LTSW_INT_ACTION_PROFILE_FLAGS_IP_LENGTH_UPDATE)
    {
        int_ifa_action_profile->flags |=
            BCM_INT_IFA_ACTION_PROFILE_FLAGS_IP_LENGTH_UPDATE;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the IFA module.
 *
 * Initial IFA-realated data structure.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_int_ifa_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (INT_IFA_INFO(unit)->initialized) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_int_ifa_deinit(unit));
    }

    if (ltsw_int_ifa_info[unit].int_ifa_lock == NULL) {
        ltsw_int_ifa_info[unit].int_ifa_lock =
            sal_mutex_create("bcmLtswIntIfaMutex");
        SHR_NULL_CHECK(ltsw_int_ifa_info[unit].int_ifa_lock, SHR_E_MEMORY);
    }

    INT_IFA_INFO(unit)->initialized = 1;

    /* Set IFA default version. */
    if (!bcmi_warmboot_get(unit)) {
        /* Set IFA default version. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_int_control_set(unit, bcmiIntControlIfaVersion, 2));
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (ltsw_int_ifa_info[unit].int_ifa_lock != NULL) {
            sal_mutex_destroy(ltsw_int_ifa_info[unit].int_ifa_lock);
            ltsw_int_ifa_info[unit].int_ifa_lock = NULL;
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize the IFA module.
 *
 * Free IFA-realated data structure.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_int_ifa_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!INT_IFA_INFO(unit)->initialized) {
        SHR_EXIT();
    }

    sal_mutex_destroy(ltsw_int_ifa_info[unit].int_ifa_lock);
    ltsw_int_ifa_info[unit].int_ifa_lock = NULL;

    INT_IFA_INFO(unit)->initialized = 0;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a vector match for IFA.
 *
 * \param [in] unit Unit number.
 * \param [in] options Add options.
 * \param [in] vector_match Vector match criteria.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_int_ifa_vector_match_add(int unit, uint32 options,
                                  bcm_int_ifa_vector_match_t *vector_match)
{
    bcmi_ltsw_int_vector_match_t int_vector_match;
    uint32 int_options = 0;
    bool int_ifa_locked = false;

    SHR_FUNC_ENTER(unit);

    INT_IFA_INIT_CHECK(unit);

    SHR_NULL_CHECK(vector_match, SHR_E_PARAM);

    INT_IFA_LOCK(unit);
    int_ifa_locked = true;

    sal_memset(&int_vector_match, 0, sizeof(bcmi_ltsw_int_vector_match_t));
    /* Vector match key. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_int_ifa_vector_match_to_int_key(unit, vector_match,
                                              &int_vector_match));

    /* Vector match match value. */
    int_vector_match.metadata_profile_id = vector_match->metadata_profile_id;

    if (options & BCM_INT_IFA_VECTOR_MATCH_OPTIONS_REPLACE) {
        int_options |= BCMI_LTSW_INT_VECTOR_MATCH_OPTIONS_REPLACE;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_vector_match_add(unit, int_options, &int_vector_match));

exit:
    if (int_ifa_locked) {
        INT_IFA_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Get the vector match for IFA.
 *
 * \param [in] unit Unit number.
 * \param [out] vector_match Vectoer match criteria.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_int_ifa_vector_match_get(int unit,
                                  bcm_int_ifa_vector_match_t *vector_match)
{
    bcmi_ltsw_int_vector_match_t int_vector_match;
    bool int_ifa_locked = false;

    SHR_FUNC_ENTER(unit);

    INT_IFA_INIT_CHECK(unit);

    SHR_NULL_CHECK(vector_match, SHR_E_PARAM);

    INT_IFA_LOCK(unit);
    int_ifa_locked = true;

    sal_memset(&int_vector_match, 0, sizeof(bcmi_ltsw_int_vector_match_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_int_ifa_vector_match_to_int_key(unit, vector_match,
                                              &int_vector_match));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_vector_match_get(unit, &int_vector_match));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_ifa_vector_match_from_int(unit, &int_vector_match,
                                                 vector_match));

exit:
    if (int_ifa_locked) {
        INT_IFA_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete the vector match for IFA.
 *
 * \param [in] unit Unit number.
 * \param [in] vector_match Vector match criteria.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_int_ifa_vector_match_delete(int unit,
                                     bcm_int_ifa_vector_match_t *vector_match)
{
    bcmi_ltsw_int_vector_match_t int_vector_match;
    bool int_ifa_locked = false;

    SHR_FUNC_ENTER(unit);

    INT_IFA_INIT_CHECK(unit);

    SHR_NULL_CHECK(vector_match, SHR_E_PARAM);

    INT_IFA_LOCK(unit);
    int_ifa_locked = true;

    sal_memset(&int_vector_match, 0, sizeof(bcmi_ltsw_int_vector_match_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_int_ifa_vector_match_to_int_key(unit, vector_match,
                                              &int_vector_match));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_vector_match_delete(unit, &int_vector_match));

exit:
    if (int_ifa_locked == 1) {
        INT_IFA_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all vector matches for IFA.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_int_ifa_vector_match_delete_all(int unit)
{
    bool int_ifa_locked = false;

    SHR_FUNC_ENTER(unit);

    INT_IFA_INIT_CHECK(unit);

    INT_IFA_LOCK(unit);
    int_ifa_locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_vector_match_delete_all(unit,
                                               bcmiIntVectorMatchTypeIfa));

exit:
    if (int_ifa_locked == 1) {
        INT_IFA_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all vector matches for IFA.
 *
 * \param [in] unit Unit number.
 * \param [in] cb A pointer to the callback function to call for each vector match
 * \param [in] user_data Pointer to user data to supply in the callback
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_int_ifa_vector_match_traverse(
    int unit,
    bcm_int_ifa_vector_match_traverse_cb cb,
    void *user_data)
{
    bool int_ifa_locked = false;

    SHR_FUNC_ENTER(unit);

    INT_IFA_INIT_CHECK(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    INT_IFA_LOCK(unit);
    int_ifa_locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_vector_match_ifa_traverse(unit, cb,
             bcmi_ltsw_int_ifa_vector_match_from_int, user_data));

exit:
    if (int_ifa_locked == 1) {
        INT_IFA_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the IFA action profile.
 *
 * \param [in] unit Unit number.
 * \param [in] options Create options
 * \param [in/out] profile_id Action profile ID
 * \param [in] profile  Action profile
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_int_ifa_action_profile_create(
    int unit,
    uint32 options,
    int *profile_id,
    bcm_int_ifa_action_profile_t *profile)
{
    bool int_ifa_locked = false;
    bcmi_ltsw_int_action_profile_t int_profile;
    uint32 int_options = 0;

    SHR_FUNC_ENTER(unit);

    INT_IFA_INIT_CHECK(unit);

    SHR_NULL_CHECK(profile_id, SHR_E_PARAM);
    SHR_NULL_CHECK(profile, SHR_E_PARAM);

    sal_memset(&int_profile, 0, sizeof(bcmi_ltsw_int_action_profile_t));
    int_profile.timestamp_mode = profile->timestamp_mode;
    int_profile.residence_time_format = profile->residence_time_format;
    int_profile.header_operation_mode = profile->header_operation_mode;
    int_profile.metadata_profile_id = profile->metadata_profile_id;
    int_profile.type = bcmiIntActionProfileTypeIfa;

    if (profile->flags & BCM_INT_IFA_ACTION_PROFILE_FLAGS_IP_LENGTH_UPDATE)
    {
        int_profile.flags |=
            BCMI_LTSW_INT_ACTION_PROFILE_FLAGS_IP_LENGTH_UPDATE;
    }

    if (options & BCM_INT_IFA_ACTION_PROFILE_OPTIONS_WITH_ID) {
        int_options |= BCMI_LTSW_INT_ACTION_PROFILE_OPTIONS_WITH_ID;
    }

    if (options & BCM_INT_IFA_ACTION_PROFILE_OPTIONS_REPLACE) {
        int_options |= BCMI_LTSW_INT_ACTION_PROFILE_OPTIONS_REPLACE;
    }

    INT_IFA_LOCK(unit);
    int_ifa_locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_action_profile_create(unit, int_options, profile_id,
                                             &int_profile));

exit:
    if (int_ifa_locked) {
        INT_IFA_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the IFA action profile.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Action profile ID
 * \param [out] profile Action profile
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_int_ifa_action_profile_get(
    int unit, 
    int profile_id,
    bcm_int_ifa_action_profile_t *profile)
{
    bool int_ifa_locked = false;
    bcmi_ltsw_int_action_profile_t int_profile;

    SHR_FUNC_ENTER(unit);

    INT_IFA_INIT_CHECK(unit);

    SHR_NULL_CHECK(profile, SHR_E_PARAM);

    INT_IFA_LOCK(unit);
    int_ifa_locked = true;

    sal_memset(&int_profile, 0, sizeof(bcmi_ltsw_int_action_profile_t));
    int_profile.type = bcmiIntActionProfileTypeIfa;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_action_profile_get(unit, profile_id, &int_profile));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_ifa_action_profile_from_int(unit, &int_profile,
                                                   profile));

exit:
    if (int_ifa_locked) {
        INT_IFA_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the IFA action profile.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Action profile ID
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_int_ifa_action_profile_destroy(int unit, int profile_id)
{
    bool int_ifa_locked = false;

    SHR_FUNC_ENTER(unit);

    INT_IFA_INIT_CHECK(unit);

    INT_IFA_LOCK(unit);
    int_ifa_locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_action_profile_destroy(unit, bcmiIntActionProfileTypeIfa,
                                              profile_id));

exit:
    if (int_ifa_locked) {
        INT_IFA_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all IFA action profiles.
 *
 * \param [in] unit Unit number.
 * \param [in] cb A pointer to the callback function to call for each action profile
 * \param [in] user_data Pointer to user data to supply in the callback
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_int_ifa_action_profile_traverse(
    int unit,
    bcm_int_ifa_action_profile_traverse_cb cb,
    void *user_data)
{
    bool int_ifa_locked = false;

    SHR_FUNC_ENTER(unit);

    INT_IFA_INIT_CHECK(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    INT_IFA_LOCK(unit);
    int_ifa_locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_action_profile_ifa_traverse(unit, cb,
             bcmi_ltsw_int_ifa_action_profile_from_int, user_data));

exit:
    if (int_ifa_locked) {
        INT_IFA_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}
#endif /* INCLUDE_INT */

