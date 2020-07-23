/*! \file obm.c
 *
 * BCM level APIs for obm.
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
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/obm.h>
#include <bcm_int/ltsw/mbcm/obm.h>

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


/******************************************************************************
 * Public functions
 */

/*!
 * \brief Initialize the obm module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_obm_init(int unit)

{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_obm_init(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach the obm module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_obm_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_obm_detach(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable obm port flow control setting.
 *
 * \param [in] unit        Unit Number.
 * \param [in] lport       Logical port ID.
 * \param [in] fc_type     Flow control type, 0: parse, 1: PFC.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_obm_port_fc_enable_set(int unit, int lport, int fc_type)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_obm_port_fc_enable_set(unit, lport, fc_type));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get port controls for OBM parsing
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port ID.
 * \param [in] type Parse type.
 * \param [out] arg arg.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_obm_port_control_get(
    int unit,
    bcm_port_t port,
    bcmi_ltsw_obm_port_control_t type,
    int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_obm_port_control_get(unit, port, type, arg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set port controls for OBM parsing
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port ID.
 * \param [in] type Parse type.
 * \param [in] arg arg.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_obm_port_control_set(
    int unit,
    bcm_port_t port,
    bcmi_ltsw_obm_port_control_t type,
    int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_obm_port_control_set(unit, port, type, arg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get pm port controls for OBM parsing
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port ID.
 * \param [in] type Parse type.
 * \param [out] arg arg.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_obm_pm_port_control_get(
    int unit,
    int pm_id,
    bcmi_ltsw_obm_port_control_t type,
    int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_obm_pm_port_control_get(unit, pm_id, type, arg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set pm port controls for OBM parsing
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port ID.
 * \param [in] type Parse type.
 * \param [in] arg arg.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_obm_pm_port_control_set(
    int unit,
    int pm_id,
    bcmi_ltsw_obm_port_control_t type,
    int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_obm_pm_port_control_set(unit, pm_id, type, arg));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the mapping of bcm_obm_code_point to bcm_obm_priority
 *
 * \param [in] unit Unit Number.
 * \param [in] gport gport for a given port.
 * \param [in] switch_obm_classifier_type type of switch_obm_classifier.
 * \param [in] array_max Maximum number of elements in switch_obm_classifier.
 * \param [out] switch_obm_classifier Array of bcm_switch_obm_classifier_t.
 * \param [out] array_count Number of elements in switch_obm_classifier.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_obm_classifier_mapping_multi_get(
    int unit,
    bcm_gport_t gport,
    bcm_switch_obm_classifier_type_t switch_obm_classifier_type,
    int array_max,
    bcm_switch_obm_classifier_t *switch_obm_classifier,
    int *array_count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_obm_classifier_mapping_multi_get(unit, gport,
            switch_obm_classifier_type, array_max,
            switch_obm_classifier, array_count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the mapping of bcm_obm_code_point to bcm_obm_priority
 *
 * \param [in] unit Unit Number.
 * \param [in] gport gport for a given port.
 * \param [in] switch_obm_classifier_type type of switch_obm_classifier.
 * \param [in] array_count Number of elements in switch_obm_classifier.
 * \param [in] switch_obm_classifier Array of bcm_switch_obm_classifier_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_obm_classifier_mapping_multi_set(
    int unit,
    bcm_gport_t gport,
    bcm_switch_obm_classifier_type_t switch_obm_classifier_type,
    int array_count,
    bcm_switch_obm_classifier_t *switch_obm_classifier)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_obm_classifier_mapping_multi_set(unit, gport,
            switch_obm_classifier_type,
            array_count, switch_obm_classifier));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get OBM traffic class for max watermark mode.
 *
 * \param [in] unit          Unit Number.
 * \param [in] port          Port ID.
 * \param [out]obm_wm_mode   OBM traffic class for max watermark mode selection.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_obm_port_max_usage_mode_get(
    int unit,
    bcm_port_t port,
    bcm_obm_max_watermark_mode_t *obm_wm_mode)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_obm_port_max_usage_mode_get(unit, port, obm_wm_mode));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set OBM traffic class for max watermark mode.
 *
 * \param [in] unit          Unit Number.
 * \param [in] port          Port ID.
 * \param [in] obm_wm_mode   OBM traffic class for max watermark mode selection.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_obm_port_max_usage_mode_set(
    int unit,
    bcm_port_t port,
    bcm_obm_max_watermark_mode_t obm_wm_mode)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_obm_port_max_usage_mode_set(unit, port, obm_wm_mode));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Determine which COS queue a given priority currently maps to.
 *
 * \param [in] unit                Unit Number.
 * \param [in] port                Port ID.
 * \param [in] priority_type       Lookup priority type.
 * \param [in] priority            Priority.
 * \param [out] obm_tc             OBM traffic class to be mapped.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_obm_port_pri_traffic_class_mapping_get(
    int unit,
    bcm_port_t port,
    bcm_obm_lookup_priority_type_t piority_type,
    int priority,
    bcm_obm_traffic_class_t *obm_tc)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_obm_port_pri_traffic_class_mapping_get
            (unit, port, piority_type, priority, obm_tc));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Determine which COS queue a given priority currently maps to.
 *
 * \param [in] unit                Unit Number.
 * \param [in] port                Port ID.
 * \param [in] priority_type       Lookup priority type.
 * \param [in] priority            Priority.
 * \param [in] obm_tc              OBM traffic class to be mapped.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_obm_port_pri_traffic_class_mapping_set(
    int unit,
    bcm_port_t port,
    bcm_obm_lookup_priority_type_t piority_type,
    int priority,
    bcm_obm_traffic_class_t obm_tc)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_obm_port_pri_traffic_class_mapping_set
            (unit, port, piority_type, priority, obm_tc));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get OBM flow control configuration.
 *
 * \param [in] unit                Unit Number.
 * \param [in] port                Port ID.
 * \param [in] obm_traffic_class   OBM traffic classes.
 * \param [in] max_pri_count       Max count.
 * \param [out] priority_list      priority array.
 * \param [out] pri_count          Count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_obm_traffic_class_pfc_priority_mapping_get(
    int unit,
    bcm_port_t port,
    bcm_obm_traffic_class_t obm_traffic_class,
    int max_pri_count,
    int *priority_list,
    int *pri_count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_obm_traffic_class_pfc_priority_mapping_get
            (unit, port, obm_traffic_class, max_pri_count,
             priority_list, pri_count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set OBM flow control configuration.
 *
 * \param [in] unit                Unit Number.
 * \param [in] port                Port ID.
 * \param [in] obm_traffic_class   OBM traffic classes.
 * \param [in] max_pri_count       Max count.
 * \param [in] priority_list       priority array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_obm_traffic_class_pfc_priority_mapping_set(
    int unit,
    bcm_port_t port,
    bcm_obm_traffic_class_t obm_traffic_class,
    int max_pri_count,
    int *priority_list)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_obm_traffic_class_pfc_priority_mapping_set
            (unit, port, obm_traffic_class, max_pri_count, priority_list));

exit:
    SHR_FUNC_EXIT();
}

