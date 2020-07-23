 /*! \file pfc.h
  *
  * PFC internal management header file.
  * This file contains the management for PFC.
  */
 /*
  *
  * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
  * 
  * Copyright 2007-2020 Broadcom Inc. All rights reserved.
  */

#ifndef BCMI_LTSW_PFC_H
#define BCMI_LTSW_PFC_H

/*!
 * \brief Initialize the pfc module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_pfc_init(int unit);

/*!
 * \brief Detach the pfc module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_pfc_detach(int unit);

/*!
 * \brief Enable/diable PFC rx.
 *
 * \param [in] unit Unit Number.
 * \param [in] lport Logical port ID.
 * \param [in] pfc_enable PFC enable, 0: disable, 1: enable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_pfc_rx_config_set(
    int unit,
    int lport,
    int pfc_enable);

/*!
 * \brief Enable/diable PFC tx.
 *
 * \param [in] unit Unit Number.
 * \param [in] lport Logical port ID.
 * \param [in] pfc_enable PFC enable, 0: disable, 1: enable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_pfc_tx_config_set(
    int unit,
    int lport,
    int pfc_enable);

/*!
 * \brief diable port pfc.
 *
 * \param [in] unit        Unit Number.
 * \param [in] port        port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_pfc_port_pfc_disable(
    int unit,
    bcm_port_t port);

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
extern int
bcmi_ltsw_port_priority_group_config_set(
    int unit,
    bcm_gport_t gport,
    int priority_group,
    bcm_port_priority_group_config_t *prigrp_config);

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
extern int
bcmi_ltsw_port_priority_group_config_get(
    int unit,
    bcm_gport_t gport,
    int priority_group,
    bcm_port_priority_group_config_t *prigrp_config);

/*!
 * \brief Set the PFC-related profile ID for a given port.
 *
 * \param [in] unit Unit Number.
 * \param [in] lport Logical port ID.
 * \param [in] profile_id profile ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_pfc_config_profile_id_set(
    int unit,
    int lport,
    int profile_id);

/*!
 * \brief Set Recovery action to be performed in PFC deadlock recovery state.
 *
 * \param [in] unit Unit Number.
 * \param [in] action 0-Transmit Packets (Default), 1-Drop.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_pfc_deadlock_recovery_action_set(
    int unit,
    int action);

/*!
 * \brief Get Recovery action to be performed in PFC deadlock recovery state.
 *
 * \param [in] unit Unit Number.
 * \param [out] action 0-Transmit Packets (Default), 1-Drop.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_pfc_deadlock_recovery_action_get(
    int unit,
    int *action);

#endif /* BCMI_LTSW_PFC_H */
