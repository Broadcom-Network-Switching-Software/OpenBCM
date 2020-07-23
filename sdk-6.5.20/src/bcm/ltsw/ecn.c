/*! \file ecn.c
 *
 * BCM level APIs for ECN.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/ecn.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/ecn.h>
#include <bcm_int/ltsw/ecn_int.h>
#include <bcm_int/ltsw/mbcm/ecn.h>
#include <bcm_int/ltsw/lt_intf.h>

#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>

#define BSL_LOG_MODULE BSL_LS_BCM_COSQ

/* ECN database. */
typedef struct ltsw_ecn_info_s {
    /* ECN module initialized flag. */
    int initialized;

    /* ECN module lock. */
    sal_mutex_t lock;
} ltsw_ecn_info_t;

static ltsw_ecn_info_t ltsw_ecn_info[BCM_MAX_NUM_UNITS];

#define ECN_INFO(unit) (&ltsw_ecn_info[unit])

#define ECN_INIT_CHECK(unit) \
    do { \
        if (ECN_INFO(unit)->initialized == 0) { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while(0)

#define ECN_LOCK(unit) \
    do { \
        if (ECN_INFO(unit)->lock) { \
            sal_mutex_take(ECN_INFO(unit)->lock, SAL_MUTEX_FOREVER); \
        } else { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while(0)

#define ECN_UNLOCK(unit) \
    do { \
        if (ECN_INFO(unit)->lock) { \
            sal_mutex_give(ECN_INFO(unit)->lock); \
        } else { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while(0)

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Free resources of ecn module.
 *
 * Free ECN module resources.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static
int ltsw_ecn_resource_free(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_ecn_deinit(unit));

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

/*!
 * \brief De-initialize the ECN module.
 *
 * Free ECN module resources.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_ecn_deinit(int unit)
{
    int lock = 0;

    SHR_FUNC_ENTER(unit);

    if (ECN_INFO(unit)->initialized == 0) {
        SHR_EXIT();
    }

    ECN_LOCK(unit);
    lock = 1;

    ECN_INFO(unit)->initialized = 0;

    SHR_IF_ERR_EXIT
        (ltsw_ecn_resource_free(unit));

exit:
    if (lock == 1) {
        ECN_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the ECN module.
 *
 * Initialize ECN database and ECN related logical tables.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_ecn_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_ecn_deinit(unit));

    if (ECN_INFO(unit)->lock == NULL) {
        ECN_INFO(unit)->lock = sal_mutex_create("bcmLtswEcnMutex");
        SHR_NULL_CHECK(ECN_INFO(unit)->lock, SHR_E_MEMORY);
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_ecn_init(unit));

    ECN_INFO(unit)->initialized = 1;

exit:
    if (SHR_FUNC_ERR()) {
        ltsw_ecn_resource_free(unit);

        if (ECN_INFO(unit)->lock != NULL) {
            sal_mutex_destroy(ECN_INFO(unit)->lock);
            ECN_INFO(unit)->lock = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief To set the default responsive value for Non-TCP packets.
 *
 * \param [in] unit Unit Number.
 * \param [in] value Default responsive value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcmi_ltsw_ecn_responsive_default_set(
    int unit,
    int value)
{
    SHR_FUNC_ENTER(unit);

    ECN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecn_responsive_default_set(unit, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief To get the default responsive value for Non-TCP packets.
 *
 * \param [in] unit Unit Number.
 * \param [out] value Returned default responsive value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcmi_ltsw_ecn_responsive_default_get(
    int unit,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    ECN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecn_responsive_default_get(unit, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief To set the latency-based ECN mode.
 *
 * \param [in] unit Unit Number.
 * \param [in] mode Latency-based ECN mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcmi_ltsw_ecn_latency_based_mode_set(
    int unit,
    bcm_switch_latency_ecn_mark_mode_t mode)
{
    int ecn_lock = 0;

    SHR_FUNC_ENTER(unit);

    ECN_INIT_CHECK(unit);

    ECN_LOCK(unit);
    ecn_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecn_latency_based_mode_set(unit, mode));

exit:
    if (ecn_lock == 1) {
        ECN_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief To get the latency-based ECN mode.
 *
 * \param [in] unit Unit Number.
 * \param [out] mode Latency-based ECN mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcmi_ltsw_ecn_latency_based_mode_get(
    int unit,
    bcm_switch_latency_ecn_mark_mode_t *mode)
{
    int ecn_lock = 0;

    SHR_FUNC_ENTER(unit);

    ECN_INIT_CHECK(unit);

    SHR_NULL_CHECK(mode, SHR_E_PARAM);

    ECN_LOCK(unit);
    ecn_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecn_latency_based_mode_get(unit, mode));

exit:
    if (ecn_lock == 1) {
        ECN_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief To set the threshold of latency-based ECN mode.
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Port number or GPORT identifier.
 * \param [in] cosq COS queue number.
 * \param [in] mode Latency-based ECN mode.
 * \param [in] threshold threshold.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcmi_ltsw_ecn_latency_based_threshold_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_switch_latency_ecn_mark_mode_t mode,
    uint32_t threshold)
{
    int ecn_lock = 0;

    SHR_FUNC_ENTER(unit);

    ECN_INIT_CHECK(unit);

    ECN_LOCK(unit);
    ecn_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecn_latency_based_threshold_set(unit, gport, cosq, mode,
                                                   threshold));

exit:
    if (ecn_lock == 1) {
        ECN_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief To get the threshold of latency-based ECN mode.
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Port number or GPORT identifier.
 * \param [in] cosq COS queue number.
 * \param [in] mode Latency-based ECN mode.
 * \param [out] threshold threshold.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcmi_ltsw_ecn_latency_based_threshold_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_switch_latency_ecn_mark_mode_t mode,
    uint32_t *threshold)
{
    int ecn_lock = 0;

    SHR_FUNC_ENTER(unit);

    ECN_INIT_CHECK(unit);

    ECN_LOCK(unit);
    ecn_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecn_latency_based_threshold_get(unit, gport, cosq, mode,
                                                   threshold));

exit:
    if (ecn_lock == 1) {
        ECN_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Resolve ECN map ID.
 *
 * \param [in] unit Unit Number.
 * \param [in] ecn_map_id ECN map ID.
 * \param [out] ecn_map_type ECN map type.
 * \param [out] prof_id ECN map profile ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcmi_ltsw_ecn_map_id_resolve(
    int unit,
    int ecn_map_id,
    int *ecn_map_type,
    uint32_t *prof_id)
{
    SHR_FUNC_ENTER(unit);

    ECN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecn_map_id_resolve(unit, ecn_map_id, ecn_map_type, prof_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Construct ECN map ID.
 *
 * \param [in] unit Unit Number.
 * \param [in] ecn_map_type ECN map type.
 * \param [in] prof_id ECN map profile ID.
 * \param [out] ecn_map_id ECN map ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcmi_ltsw_ecn_map_id_construct(
    int unit,
    int ecn_map_type,
    uint32_t prof_id,
    int *ecn_map_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ecn_map_id, SHR_E_PARAM);

    ECN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecn_map_id_construct(unit, ecn_map_type, prof_id, ecn_map_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create an ECN mapping profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] flags Flags.
 * \param [out] ecn_map_id ECN map ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_ecn_map_create(
    int unit,
    uint32_t flags,
    int *ecn_map_id)
{
    int ecn_lock = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ecn_map_id, SHR_E_PARAM);

    ECN_INIT_CHECK(unit);

    ECN_LOCK(unit);
    ecn_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecn_map_create(unit, flags, ecn_map_id));

exit:
    if (ecn_lock == 1) {
        ECN_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy an ECN map profile.
 *
 * \param [in] unit Unit number.
 * \param [in] ecn_map_id ECN map ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_ecn_map_destroy(
    int unit,
    int ecn_map_id)
{
    int ecn_lock = 0;

    SHR_FUNC_ENTER(unit);

    ECN_INIT_CHECK(unit);

    ECN_LOCK(unit);
    ecn_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecn_map_destroy(unit, ecn_map_id));

exit:
    if (ecn_lock == 1) {
        ECN_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Set an ECN mapping entry in an ECN mapping profile.
 *
 * \param [in] unit Unit number.
 * \param [in] options options, not used currently.
 * \param [in] ecn_map_id ECN map ID.
 * \param [in] ecn_map ECN mapping entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_ecn_map_set(
    int unit,
    uint32_t options,
    int ecn_map_id,
    bcm_ecn_map_t *ecn_map)
{
    int ecn_lock = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ecn_map, SHR_E_PARAM);

    ECN_INIT_CHECK(unit);

    ECN_LOCK(unit);
    ecn_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecn_map_set(unit, options, ecn_map_id, ecn_map));

exit:
    if (ecn_lock == 1) {
        ECN_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an ECN mapping entry from an ECN mapping profile.
 *
 * \param [in] unit Unit number.
 * \param [in] ecn_map_id ECN map ID.
 * \param [out] ecn_map Returned ECN mapping entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_ecn_map_get(
    int unit,
    int ecn_map_id,
    bcm_ecn_map_t *ecn_map)
{
    int ecn_lock = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ecn_map, SHR_E_PARAM);

    ECN_INIT_CHECK(unit);

    ECN_LOCK(unit);
    ecn_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecn_map_get(unit, ecn_map_id, ecn_map));

exit:
    if (ecn_lock == 1) {
        ECN_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief To set the mapped internal congestion notification (int_cn) value.
 *
 * \param [in] unit Unit number.
 * \param [in] map Internal congestion notification map.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_ecn_traffic_map_set(
    int unit,
    bcm_ecn_traffic_map_info_t *map)
{
    int ecn_lock = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(map, SHR_E_PARAM);

    ECN_INIT_CHECK(unit);

    ECN_LOCK(unit);
    ecn_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecn_traffic_map_set(unit, map));

exit:
    if (ecn_lock == 1) {
        ECN_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief To get the mapped internal congestion notification (int_cn) value.
 *
 * \param [in] unit Unit number.
 * \param [in/out] map Internal congestion notification map.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_ecn_traffic_map_get(
    int unit,
    bcm_ecn_traffic_map_info_t *map)
{
    int ecn_lock = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(map, SHR_E_PARAM);

    ECN_INIT_CHECK(unit);

    ECN_LOCK(unit);
    ecn_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecn_traffic_map_get(unit, map));

exit:
    if (ecn_lock == 1) {
        ECN_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief To set the ECN type mapping.
 *
 * \param [in] unit Unit number.
 * \param [in] ecn_type ECN type map structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_ecn_type_map_set(
    int unit,
    uint32_t flags,
    bcm_ecn_type_map_t *map)
{
    int ecn_lock = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(map, SHR_E_PARAM);

    ECN_INIT_CHECK(unit);

    ECN_LOCK(unit);
    ecn_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecn_type_map_set(unit, flags, map));

exit:
    if (ecn_lock == 1) {
        ECN_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief To get the ECN type mapping.
 *
 * \param [in] unit Unit number.
 * \param [in/out] ecn_type ECN type map structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_ecn_type_map_get(
    int unit,
    uint32_t flags,
    bcm_ecn_type_map_t *map)
{
    int ecn_lock = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(map, SHR_E_PARAM);

    ECN_INIT_CHECK(unit);

    ECN_LOCK(unit);
    ecn_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecn_type_map_get(unit, flags,  map));

exit:
    if (ecn_lock == 1) {
        ECN_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Assign the actions of the specified ECN traffic.
 *
 * \param [in] unit Unit number.
 * \param [in] ecn_config ECN traffic action configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_ecn_traffic_action_config_set(
    int unit,
    bcm_ecn_traffic_action_config_t *ecn_config)
{
    int ecn_lock = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ecn_config, SHR_E_PARAM);

    if (ecn_config->action_flags &
        ~(BCM_ECN_TRAFFIC_ACTION_ENQUEUE_WRED_RESPONSIVE |
          BCM_ECN_TRAFFIC_ACTION_ENQUEUE_MARK_ELIGIBLE |
          BCM_ECN_TRAFFIC_ACTION_DEQUEUE_CONGESTION_INT_CN_UPDATE |
          BCM_ECN_TRAFFIC_ACTION_DEQUEUE_NON_CONGESTION_INT_CN_UPDATE |
          BCM_ECN_TRAFFIC_ACTION_EGRESS_ECN_MARKING |
          BCM_ECN_TRAFFIC_ACTION_EGRESS_DROP |
          BCM_ECN_TRAFFIC_ACTION_ECN_COUNTER_ELIGIBLE |
          BCM_ECN_TRAFFIC_ACTION_DEQUEUE_LATENCY_INT_CN_UPDATE)) {

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    ECN_INIT_CHECK(unit);

    ECN_LOCK(unit);
    ecn_lock = 1;

    switch (ecn_config->action_type) {
    case BCM_ECN_TRAFFIC_ACTION_TYPE_ENQUEUE:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_ecn_action_enqueue_set(unit, ecn_config));
        break;

    case BCM_ECN_TRAFFIC_ACTION_TYPE_DEQUEUE:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_ecn_action_dequeue_set(unit, ecn_config));
        break;

    case BCM_ECN_TRAFFIC_ACTION_TYPE_EGRESS:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_ecn_action_egress_set(unit, ecn_config));
        break;

    case BCM_ECN_TRAFFIC_ACTION_TYPE_LATENCY_ECN_DEQUEUE:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_ecn_action_latency_dequeue_set(unit, ecn_config));
        break;

    default:
        SHR_ERR_EXIT(SHR_E_PARAM);

    }

exit:
    if (ecn_lock == 1) {
        ECN_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief To get the actions of the specified ECN traffic.
 *
 * \param [in] unit Unit number.
 * \param [in/out] ecn_config ECN traffic action configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_ecn_traffic_action_config_get(
    int unit,
    bcm_ecn_traffic_action_config_t *ecn_config)
{
    int ecn_lock = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ecn_config, SHR_E_PARAM);

    ECN_INIT_CHECK(unit);

    ECN_LOCK(unit);
    ecn_lock = 1;

    switch (ecn_config->action_type) {
    case BCM_ECN_TRAFFIC_ACTION_TYPE_ENQUEUE:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_ecn_action_enqueue_get(unit, ecn_config));
        break;

    case BCM_ECN_TRAFFIC_ACTION_TYPE_DEQUEUE:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_ecn_action_dequeue_get(unit, ecn_config));
        break;

    case BCM_ECN_TRAFFIC_ACTION_TYPE_EGRESS:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_ecn_action_egress_get(unit, ecn_config));
        break;

    case BCM_ECN_TRAFFIC_ACTION_TYPE_LATENCY_ECN_DEQUEUE:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_ecn_action_latency_dequeue_get(unit, ecn_config));
        break;

    default:
        SHR_ERR_EXIT(SHR_E_PARAM);

    }

exit:
    if (ecn_lock == 1) {
        ECN_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief To configure the value of responsive indication based on the IP protocol value.
 *
 * \param [in] unit Unit Number.
 * \param [in] ip_proto IP Protocol value.
 * \param [in] responsice Responsice indication.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_ecn_responsive_protocol_set(
    int unit,
    uint8 ip_proto,
    int value)
{
    SHR_FUNC_ENTER(unit);

    ECN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecn_responsive_protocol_set(unit, ip_proto, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief To get the value of responsive indication based on the IP protocol value.
 *
 * \param [in] unit Unit Number.
 * \param [in] ip_proto IP Protocol value.
 * \param [out] responsice Responsice indication.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_ecn_responsive_protocol_get(
    int unit,
    uint8 ip_proto,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    ECN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecn_responsive_protocol_get(unit, ip_proto, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all specified ECN type maps and call the supplied
 *          callback routine for each one.
 *
 * \param [in] unit Unit number.
 * \param [in] flags Indicates the operation by BCM_ECN_TYPE_MAP_XXX flags.
 * \param [in] cb User supplied callback function.
 * \param [in] user_data User supplied cookie used as parameter in callback function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_ecn_type_map_traverse(
    int unit,
    uint32 flags,
    bcm_ecn_type_map_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    ECN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecn_type_map_traverse(unit, flags, cb, user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief To set an ECN mapping info from a port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port gport.
 * \param [inout] ecn_map ECN mapping values.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_ecn_port_map_set(
    int unit,
    bcm_gport_t port,
    bcm_ecn_port_map_t *ecn_map)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ecn_map, SHR_E_PARAM);

    ECN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecn_port_map_set(unit, port, ecn_map));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief To get an ECN mapping info from a port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port gport.
 * \param [inout] ecn_map ECN mapping values.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_ecn_port_map_get(
    int unit,
    bcm_gport_t port,
    bcm_ecn_port_map_t *ecn_map)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ecn_map, SHR_E_PARAM);

    ECN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecn_port_map_get(unit, port, ecn_map));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_ecn_ctrl_set(int unit, const char *lt_name,
                      const char *lt_fld_name, int val)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecn_ctrl_set(unit, lt_name, lt_fld_name, val));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_ecn_ctrl_get(int unit, const char *lt_name,
                      const char *lt_fld_name, int *val)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecn_ctrl_get(unit, lt_name, lt_fld_name, val));

exit:
    SHR_FUNC_EXIT();
}

