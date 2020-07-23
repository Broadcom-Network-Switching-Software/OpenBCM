/*! \file pfc.c
 *
 * BCM level APIs for pfc.
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
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/pfc.h>
#include <bcm_int/ltsw/mbcm/pfc.h>

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
 * \brief Initialize the pfc module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_pfc_init(int unit)

{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_pfc_init(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach the pfc module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_pfc_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_pfc_detach(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable/diable PFC rx.
 *
 * \param [in] unit        Unit Number.
 * \param [in] lport       Logical port ID.
 * \param [in] pfc_enable  PFC enable, 0: disable, 1: enable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_pfc_rx_config_set(int unit, int lport, int pfc_enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_pfc_rx_config_set(unit, lport, pfc_enable));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable/diable PFC tx.
 *
 * \param [in] unit        Unit Number.
 * \param [in] lport       Logical port ID.
 * \param [in] pfc_enable  PFC enable, 0: disable, 1: enable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_pfc_tx_config_set(int unit, int lport, int pfc_enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_pfc_tx_config_set(unit, lport, pfc_enable));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief diable port pfc.
 *
 * \param [in] unit        Unit Number.
 * \param [in] port        port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_pfc_port_pfc_disable(
    int unit,
    bcm_port_t port)
{
    int pg;
    bcmi_ltsw_cosq_device_info_t device_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    for (pg = 0; pg < device_info.num_pg; pg++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_pfc_port_pg_pfc_set(unit, port, pg, 0));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the port priority group configuration.
 *
 * \param [in] unit        Unit Number.
 * \param [in] gport       generic port.
 * \param [in] priority_group  priority group id.
 * \param [in] prigrp_config  tructure describes port PG configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_port_priority_group_config_set(
    int unit,
    bcm_gport_t gport,
    int priority_group,
    bcm_port_priority_group_config_t *prigrp_config)
{
    int local_port, pfc_enable;
    bcmi_ltsw_cosq_device_info_t device_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    if ((priority_group < 0) ||
        (priority_group >= device_info.num_pg)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (prigrp_config == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (prigrp_config->shared_pool_xoff_enable ||
        prigrp_config->shared_pool_discard_enable ||
        prigrp_config->priority_enable_vector_mask) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    pfc_enable = prigrp_config->pfc_transmit_enable ? true : false;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_pfc_port_pg_pfc_set(unit, local_port, priority_group, pfc_enable));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the port priority group configuration.
 *
 * \param [in] unit        Unit Number.
 * \param [in] gport       generic port.
 * \param [in] priority_group  priority group id.
 * \param [out] prigrp_config  tructure describes port PG configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_port_priority_group_config_get(
    int unit,
    bcm_gport_t gport,
    int priority_group,
    bcm_port_priority_group_config_t *prigrp_config)
{
    int local_port, pfc_enable;
    bcmi_ltsw_cosq_device_info_t device_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    if ((priority_group < 0) ||
        (priority_group >= device_info.num_pg)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (prigrp_config == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_pfc_port_pg_pfc_get(unit, local_port, priority_group, &pfc_enable));

    prigrp_config->pfc_transmit_enable = pfc_enable;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the PFC-related profile ID for a given port.
 *
 * \param [in] unit        Unit Number.
 * \param [in] lport       Logical port ID.
 * \param [in] profile_id  profile ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_pfc_config_profile_id_set(int unit, int lport, int profile_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_pfc_config_profile_id_set(unit, lport, profile_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set Recovery action to be performed in PFC deadlock recovery state.
 *
 * \param [in] unit Unit Number.
 * \param [in] action 0-Transmit Packets (Default), 1-Drop.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_pfc_deadlock_recovery_action_set(int unit, int action)
{
    SHR_FUNC_ENTER(unit);

    if(action < 0 || action > 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_pfc_deadlock_recovery_action_set(unit, action));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get Recovery action to be performed in PFC deadlock recovery state.
 *
 * \param [in] unit Unit Number.
 * \param [out] action 0-Transmit Packets (Default), 1-Drop.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_pfc_deadlock_recovery_action_get(int unit, int *action)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_pfc_deadlock_recovery_action_get(unit, action));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an entire PFC class (RX) profile.
 *
 * \param [in] unit           Unit Number.
 * \param [in] profile_id     Profile ID.
 * \param [in] max_count      Maximum number of elements in config array.
 * \param [out] config_array  PFC class config array.
 * \param [out] count         actual size of array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_cosq_pfc_class_config_profile_get(
    int unit,
    int profile_id,
    int max_count,
    bcm_cosq_pfc_class_map_config_t *config_array,
    int *count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_pfc_class_config_profile_get(unit, profile_id, max_count,
                                                config_array, count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set an entire PFC class (RX) profile.
 *
 * \param [in] unit         Unit Number.
 * \param [in] profile_id   Profile ID.
 * \param [in] count        The length of array
 *                          It needs to be fixed at 8. All PFC priorities should
 *                          be configured at once, and the config array must be
 *                          arranged in ascending order of PFC priorities.
 *                          0, 1, ... , 7.
 * \param [in] config_array PFC class config array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_cosq_pfc_class_config_profile_set(
    int unit,
    int profile_id,
    int count,
    bcm_cosq_pfc_class_map_config_t *config_array)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_pfc_class_config_profile_set(unit, profile_id, count,
                                                config_array));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a PFC-tx priority to PG mapping profile.
 *
 * \param [in] unit           Unit Number.
 * \param [in] profile_id     Profile ID.
 * \param [in] array_max      Maximum number of elements in PG array.
 * \param [out] pg_array      PG array, indexed by PFC priority.
 * \param [out] count         actual size of array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_cosq_priority_group_pfc_priority_mapping_profile_get(
    int unit,
    int profile_id,
    int array_max,
    int *pg_array,
    int *array_count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_pfc_priority_mapping_profile_get(unit, profile_id, array_max,
                                                    pg_array, array_count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set a PFC-tx priority to PG mapping profile.
 *
 * \param [in] unit          Unit Number.
 * \param [in] profile_id    Profile ID.
 * \param [in] array_count   actual size of array.
 * \param [in] pg_array      PG array, indexed by PFC priority.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_cosq_priority_group_pfc_priority_mapping_profile_set(
    int unit,
    int profile_id,
    int array_count,
    int *pg_array)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_pfc_priority_mapping_profile_set(unit, profile_id, array_count,
                                                    pg_array));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get number of optimized PFC groups on a port.
 *
 * \param [in] unit           Unit Number.
 * \param [in] port           logical port number.
 * \param [out] count         number of optimized PFC groups supported.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_cosq_port_optimized_pfc_group_num_get(
    int unit,
    bcm_port_t port,
    int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_pfc_group_num_get(unit, port, arg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get config for the given (PFC priority, port)
 *        for the PFC Deadlock feature.
 *
 * \param [in] unit          Unit Number.
 * \param [in] port          Port ID.
 * \param [in] pfc_priority  PFC Priority.
 * \param [in] type          PFC deadlock control type.
 * \param [in] arg           arg.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_cosq_pfc_deadlock_control_get(
    int unit,
    bcm_port_t port,
    int pfc_priority,
    bcm_cosq_pfc_deadlock_control_t type,
    int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_pfc_deadlock_control_get(unit, port, pfc_priority, type, arg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set config for the given (PFC priority, port)
 *        for the PFC Deadlock feature.
 *
 * \param [in] unit          Unit Number.
 * \param [in] port          Port ID.
 * \param [in] pfc_priority  PFC Priority.
 * \param [in] type          PFC deadlock control type.
 * \param [in] arg           arg.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_cosq_pfc_deadlock_control_set(
    int unit,
    bcm_port_t port,
    int pfc_priority,
    bcm_cosq_pfc_deadlock_control_t type,
    int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_pfc_deadlock_control_set(unit, port, pfc_priority, type, arg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get bitmap of Enabled(Admin) and Current Deadlock ports status for a
 *        given priority in PFC deadlock feature.
 *
 * \param [in] unit           Unit Number.
 * \param [in] priority       PFC priority.
 * \param [out] info          Info for a given priority.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_cosq_pfc_deadlock_info_get(
    int unit,
    int priority,
    bcm_cosq_pfc_deadlock_info_t *pfc_deadlock_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_pfc_deadlock_info_get(unit, priority, pfc_deadlock_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the current Deadlock status for the given Port.
 *
 * \param [in] unit             Unit Number.
 * \param [in] gport            gport id.
 * \param [out] deadlock_status Deatlock status for the given port
 *                              bit[0]->PFC priority 0 ...
 *                              bit[7] -> PFC priority 7
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_cosq_pfc_deadlock_queue_status_get(
    int unit,
    bcm_gport_t gport,
    uint8_t *deadlock_status)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_pfc_deadlock_queue_status_get(unit, gport, deadlock_status));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Register pfc deadlock recovery callback in PFC deadlock feature.
 *
 * \param [in] unit           Unit Number.
 * \param [in] callback       callback function.
 * \param [in] userdata                         .
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_cosq_pfc_deadlock_recovery_event_register(
    int unit,
    bcm_cosq_pfc_deadlock_recovery_event_cb_t callback,
    void *userdata)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_pfc_deadlock_recovery_event_register(unit, callback, userdata));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Unregister pfc deadlock recovery callback in PFC deadlock feature.
 *
 * \param [in] unit           Unit Number.
 * \param [in] callback       callback function.
 * \param [in] userdata       user data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_cosq_pfc_deadlock_recovery_event_unregister(
    int unit,
    bcm_cosq_pfc_deadlock_recovery_event_cb_t callback,
    void *userdata)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_pfc_deadlock_recovery_event_unregister(unit, callback, userdata));

exit:
    SHR_FUNC_EXIT();
}

