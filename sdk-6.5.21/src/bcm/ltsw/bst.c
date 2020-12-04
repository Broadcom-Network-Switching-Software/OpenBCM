/*! \file bst.c
 *
 * BCM level APIs for bst.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <bcm/error.h>
#include <bcm/cosq.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/bst.h>
#include <bcm_int/ltsw/mbcm/bst.h>

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <sal/sal_mutex.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmlt/bcmlt.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_COSQ

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Get the BST profile for CosQ objects.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   gport        GPORT ID.
 * \param [in]   cosq         COS queue.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 * \param [out]  profile      BST profile configuration.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
ltsw_cosq_bst_profile_get(int unit,
                          bcm_gport_t gport,
                          bcm_cos_queue_t cosq,
                          bcm_bst_stat_id_t bid,
                          bcm_cosq_bst_profile_t *profile)
{
    SHR_FUNC_ENTER(unit);

    if (NULL == profile) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_bst_profile_get(unit, gport, cosq, bid, profile));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the BST profile for CosQ objects.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   gport        GPORT ID.
 * \param [in]   cosq         COS queue.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 * \param [in]   profile      BST profile configuration.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
ltsw_cosq_bst_profile_set(int unit,
                          bcm_gport_t gport,
                          bcm_cos_queue_t cosq,
                          bcm_bst_stat_id_t bid,
                          bcm_cosq_bst_profile_t *profile)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_bst_profile_set(unit, gport, cosq, bid, profile));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a specific BST stat count.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   gport        GPORT ID.
 * \param [in]   cosq         COS queue.
 * \param [in]   buffer       buffer id.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 * \param [in]   options      BST stat read options.
 * \param [out]  pvalue       BST stat count.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
ltsw_cosq_bst_stat_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_buffer_id_t buffer,
    bcm_bst_stat_id_t bid,
    uint32_t options,
    uint64_t *pvalue)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pvalue, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_bst_stat_get(unit, gport, cosq, buffer, bid,
                                     options, pvalue));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear speficied BST stat.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   gport        GPORT ID.
 * \param [in]   cosq         COS queue.
 * \param [in]   buffer       buffer id.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
ltsw_cosq_bst_stat_clear(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_buffer_id_t buffer,
    bcm_bst_stat_id_t bid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_bst_stat_clear(unit, gport, cosq, buffer, bid));
exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public functions
 */

/*!
 * \brief Initialize the bst module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_bst_init(int unit)

{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_bst_init(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach the bst module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_bst_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_bst_detach(unit));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable BST tracking.
 *
 * \param [in] unit Unit Number.
 * \param [in] arg BST enable value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_bst_enable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_bst_enable_set(unit, arg));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get BST enable value.
 *
 * \param [in] unit Unit Number.
 * \param [out] arg BST enable value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_bst_enable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_bst_enable_get(unit, arg));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set BST resource usage tracking mode.
 *
 * \param [in] unit Unit Number.
 * \param [in] arg BST resource usage tracking mode value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_bst_tracking_mode_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_bst_tracking_mode_set(unit, arg));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get BST resource usage tracking mode.
 *
 * \param [in] unit Unit Number.
 * \param [out] arg BST resource usage tracking mode value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_bst_tracking_mode_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_bst_tracking_mode_get(unit, arg));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set BST resource usage SnapShot mode.
 *
 * \param [in] unit Unit Number.
 * \param [in] arg BST resource usage SnapShot mode value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_bst_snapshot_enable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_bst_snapshot_enable_set(unit, arg));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get BST resource usage SnapShot mode.
 *
 * \param [in] unit Unit Number.
 * \param [out] arg BST resource usage SnapShot mode value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_bst_snapshot_enable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_bst_snapshot_enable_get(unit, arg));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the BST profile for CosQ objects.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   gport        Device or logical port or GPORT ID.
 * \param [in]   cosq         Cosq object offset identifier.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 * \param [out]  profile      BST profile configuration.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_bst_profile_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_bst_stat_id_t bid,
    bcm_cosq_bst_profile_t *profile)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_cosq_bst_profile_get(unit, gport, cosq, bid, profile));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the BST profile for CosQ objects.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   gport        Device or logical port or GPORT ID.
 * \param [in]   cosq         Cosq object offset identifier.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 * \param [in]   profile      BST profile configuration.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_bst_profile_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_bst_stat_id_t bid,
    bcm_cosq_bst_profile_t *profile)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_cosq_bst_profile_set(unit, gport, cosq, bid, profile));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear the current statistic/count of specified BST profile.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   gport        Device or logical port or GPORT ID.
 * \param [in]   cosq         Cosq object offset identifier.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_bst_stat_clear(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_bst_stat_id_t bid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_cosq_bst_stat_clear(unit, gport, cosq, BCM_COSQ_BUFFER_ID_INVALID,
                                  bid));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear the current statistic/count of specified BST profile.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   id           Compound index containing gport, cosq, buffer.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_bst_stat_extended_clear(
    int unit,
    bcm_cosq_object_id_t *id,
    bcm_bst_stat_id_t bid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_cosq_bst_stat_clear(unit, id->port, id->cosq, id->buffer, bid));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the current statistic/count of specified BST profile.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   id           Compound index containing gport, cosq, buffer.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 * \param [in]   options      options to perform clear-on-read.
 * \param [out]  values       stat value to return.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_bst_stat_extended_get(
    int unit,
    bcm_cosq_object_id_t *id,
    bcm_bst_stat_id_t bid,
    uint32 options,
    uint64 *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_cosq_bst_stat_get(unit, id->port, id->cosq, id->buffer,
                                bid, options, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the current statistic/count of specified BST profile.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   gport        Device or logical port or GPORT ID.
 * \param [in]   cosq         Cosq object offset identifier.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 * \param [in]   options      options to perform clear-on-read.
 * \param [out]  values       stat value to return.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_bst_stat_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_bst_stat_id_t bid,
    uint32 options,
    uint64 *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_cosq_bst_stat_get(unit, gport, cosq, BCM_COSQ_BUFFER_ID_INVALID,
                                bid, options, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the current statistic/count of multiple specified BST profile.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   gport        Device or logical port or GPORT ID.
 * \param [in]   cosq         Cosq object offset identifier.
 * \param [in]   options      options to perform clear-on-read.
 * \param [in]   max_values   Number of elements in id_list and pvalue.
 * \param [in]   id_list      Array of BST stat ID list to identify the COSQ resource/object.
 * \param [out]  values       Array of stat value to return.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_bst_stat_multi_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 options,
    int max_values,
    bcm_bst_stat_id_t *id_list,
    uint64 *values)
{
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(id_list, SHR_E_PARAM);
    SHR_NULL_CHECK(values, SHR_E_PARAM);

    if ((max_values >= bcmBstStatIdMaxCount) || (max_values <= 0)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    for (i = 0; i < max_values; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_cosq_bst_stat_get(unit, gport, cosq,
                                    BCM_COSQ_BUFFER_ID_INVALID,
                                    id_list[i], options, &values[i]));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Sync the HW stats value to SW copy for all or given BST resource.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_bst_stat_sync(
    int unit,
    bcm_bst_stat_id_t bid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_bst_stat_sync(unit, bid));

exit:
    SHR_FUNC_EXIT();
}

