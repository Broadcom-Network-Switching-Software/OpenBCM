/*! \file cosq_stat.c
 *
 * BCM level APIs for cosq_stat.
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
#include <bcm_int/ltsw/cosq.h>
#include <bcm_int/ltsw/cosq_stat.h>
#include <bcm_int/ltsw/mbcm/cosq_stat.h>

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
 * \brief Get MMU statistics on a per port per cosq basis.
 *
 * \param [in]   unit                Unit number.
 * \param [in]   gport               GPORT ID.
 * \param [in]   cosq                COS queue.
 * \param [in]   stat                Statistic to be retrieved.
 * \param [in]   sync_mode           Sync mode.
 * \param [out]  value               Returned statistic value.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
ltsw_cosq_stat_get(int unit,
                   bcm_gport_t gport,
                   bcm_cos_queue_t cosq,
                   bcm_cosq_stat_t stat,
                   int sync_mode,
                   uint64_t *value)
{
    int rv;

    if (NULL == value) {
        return SHR_E_PARAM;
    }

    rv = mbcm_ltsw_cosq_stat_get(unit, gport, cosq, stat, sync_mode, value);
    if (rv == SHR_E_NOT_FOUND) {
        *value = 0;
        rv = SHR_E_NONE;
    }

    return rv;
}

/*!
 * \brief Set MMU statistics on a per port per cosq basis.
 *
 * \param [in]   unit                Unit number.
 * \param [in]   gport               GPORT ID.
 * \param [in]   cosq                COS queue.
 * \param [in]   stat                Statistic to be set.
 * \param [in]   arg                 Statistic value to be set.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
ltsw_cosq_stat_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t value)
{
    return mbcm_ltsw_cosq_stat_set(unit, gport, cosq, stat, value);
}

/******************************************************************************
 * Public functions
 */

/*!
 * \brief Initialize the cosq_stat module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_stat_init(int unit)

{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_stat_init(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach the cosq_stat module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_stat_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_stat_detach(unit));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear the cosq stat per port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Logic port id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_stat_clear(
    int unit,
    int port)
{
    return mbcm_ltsw_cosq_stat_clear(unit, port);
}

/*!
 * \brief Get MMU statistics on a per port per cosq basis.
 *
 * \param [in]   unit                Unit number.
 * \param [in]   gport               GPORT ID.
 * \param [in]   cosq                COS queue.
 * \param [in]   stat                Statistic to be retrieved.
 * \param [out]  arg                 Returned statistic value.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_stat_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t *value)
{
    int sync_mode = 0;

    return ltsw_cosq_stat_get(unit, gport, cosq, stat, sync_mode, value);
}

/*!
 * \brief Set MMU statistics on a per port per cosq basis.
 *
 * \param [in]   unit                Unit number.
 * \param [in]   gport               GPORT ID.
 * \param [in]   cosq                COS queue.
 * \param [in]   stat                Statistic to be set.
 * \param [in]   arg                 Statistic value to be set.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_stat_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t value)
{
    return ltsw_cosq_stat_set(unit, gport, cosq, stat, value);
}

/*!
 * \brief Get MMU debug counter match configuration for a certain MMU debug counter
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number, used to deduce buffer ID.
 * \param [in] stat MMU debug counter whose match configuration to be retrieved.
 * \param [out] match Returned MMU debug counter match configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_cosq_debug_stat_match_get(
    int unit,
    bcm_port_t port,
    bcm_cosq_stat_t stat,
    bcm_cosq_debug_stat_match_t *match)
{
    return mbcm_ltsw_cosq_stat_debug_stat_match_get(unit, port, stat, match);
}

/*!
 * \brief Set MMU debug counter match configuration for a certain MMU debug counter
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number, used to deduce buffer ID.
 * \param [in] stat MMU debug counter whose match configuration to be set.
 * \param [in] match MMU debug counter match configuration to be set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_cosq_debug_stat_match_set(
    int unit,
    bcm_port_t port,
    bcm_cosq_stat_t stat,
    bcm_cosq_debug_stat_match_t *match)
{
    return mbcm_ltsw_cosq_stat_debug_stat_match_set(unit, port, stat, match);
}

/*!
 * \brief Get MMU statistics on a per port per cosq basis.
 *        The software cached count is synced with the hardware value.
 *
 * \param [in]   unit                Unit number.
 * \param [in]   gport               GPORT ID.
 * \param [in]   cosq                COS queue.
 * \param [in]   stat                Statistic to be retrieved.
 * \param [out]  arg                 Returned statistic value.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_stat_sync_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t *value)
{
    int sync_mode = 1;

    return ltsw_cosq_stat_get(unit, gport, cosq, stat, sync_mode, value);
}

/*!
 * \brief Get multiple MMU statistics on a per port per cosq basis.
 *        The software cached count is synced with the hardware value.
 *
 * \param [in]   unit       Unit number.
 * \param [in]   gport      GPORT ID.
 * \param [in]   cosq       COS queue.
 * \param [in]   nstat      Number of elements in stat array.
 * \param [in]   stat_arr   Array of statistics types defined in bcm_cos_queue_t.
 * \param [out]  value_arr  Array of collected statistics values.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_stat_sync_multi_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int nstat,
    bcm_cosq_stat_t *stat_arr,
    uint64_t *value_arr)
{
    if ((nstat <= 0) || (NULL == stat_arr) || (NULL == value_arr)) {
        return SHR_E_PARAM;
    }

    return mbcm_ltsw_cosq_stat_sync_multi_get(unit, gport, cosq, nstat, stat_arr,
                                              value_arr);
}

