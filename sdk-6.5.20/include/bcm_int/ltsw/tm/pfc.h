/* \file pfc.h
 *
 * PFC headfiles to declare internal APIs for TM devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_LTSW_TM_PFC_H
#define BCMINT_LTSW_TM_PFC_H

#include <bcm/cosq.h>
#include <bcm/types.h>

#include <sal/sal_types.h>

/*
 * \brief Get pfc recovery action
 *
 * \param [in] unit Unit Number.
 * \param [in/out] action recovery action.
 * \param [in/out] sym_val recovery action in symbol.
 * \param [in] get_sym get sym_val.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*cosq_pfc_recovery_action_get_t) (
    int unit,
    int *action,
    const char **sym_val,
    bool get_sym);

/*
 * \brief Get pfc deadlock detection countdown timer.
 *
 * \param [in] unit Unit Number.
 * \param [in/out] timer countdown timer.
 * \param [in/out] sym_val countdown timer in symbol.
 * \param [in] get_sym get sym_val.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*cosq_pfc_detection_timer_get_t) (
    int unit,
    int *timer,
    const char **sym_val,
    bool get_sym);

/*
 * \brief pfc driver structure.
 */
typedef struct tm_ltsw_pfc_chip_driver_s {

    /* Get pfc recovery action. */
    cosq_pfc_recovery_action_get_t cosq_pfc_recovery_action_get;

    /* Get pfc deadlock detection countdown timer. */
    cosq_pfc_detection_timer_get_t cosq_pfc_detection_timer_get;

} tm_ltsw_pfc_chip_driver_t;

/*
 * \brief PFC Driver callback registration
 *
 * \param [in] unit Unit number.
 * \param [in] drv  Driver pointer.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
tm_ltsw_pfc_chip_driver_register(int unit, tm_ltsw_pfc_chip_driver_t* drv);

/*
 * \brief PFC Driver callback deregistration
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
tm_ltsw_pfc_chip_driver_deregister(int unit);

/*
 * \brief PFC Driver get.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
tm_ltsw_pfc_chip_driver_get(int unit, tm_ltsw_pfc_chip_driver_t **drv);

/*
 * \brief Get an entire PFC class (RX) profile
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_id Profile ID.
 * \param [in] max_count Maximum number of elements in config array.
 * \param [out] config_array PFC class config array.
 * \param [out] count actual size of array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_pfc_class_config_profile_get(
    int unit,
    int profile_id,
    int max_count,
    bcm_cosq_pfc_class_map_config_t *config_array,
    int *count);

/*
 * \brief Set an entire PFC class (RX) profile
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_id Profile ID.
 * \param [in] count The length of array.
 * \param [out] config_array PFC class config array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_pfc_class_config_profile_set(
    int unit,
    int profile_id,
    int count,
    bcm_cosq_pfc_class_map_config_t *config_array);

/*
 * \brief Get a PFC-tx priority to PG mapping profile
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_id Profile ID.
 * \param [in] array_max Maximum number of elements in PG array.
 * \param [out] pg_array PG array, indexed by PFC priority.
 * \param [out] array_count actual size of array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_pfc_priority_mapping_profile_get(
    int unit,
    int profile_id,
    int array_max,
    int *pg_array,
    int *array_count);

/*
 * \brief Set a PFC-tx priority to PG mapping profile
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_id Profile ID.
 * \param [in] array_count actual size of array.
 * \param [out] pg_array PG array, indexed by PFC priority.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_pfc_priority_mapping_profile_set(
    int unit,
    int profile_id,
    int array_count,
    int *pg_array);

/*
 * \brief Initialize the pfc module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_pfc_init(int unit);

/*
 * \brief Detach the pfc module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_pfc_detach(int unit);

/*
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
tm_ltsw_pfc_rx_config_set(
    int unit,
    int lport,
    int pfc_enable);

/*
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
tm_ltsw_pfc_tx_config_set(
    int unit,
    int lport,
    int pfc_enable);

/*!
 * \brief enable port pg pfc.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port ID.
 * \param [in] pg priority group id.
 * \param [in] pfc_enable pfc enable state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_pfc_port_pg_pfc_set(
    int unit,
    bcm_port_t port,
    int pg,
    int pfc_enable);

/*!
 * \brief Get per port pg pfc enable state.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port ID.
 * \param [in] pg priority group id.
 * \param [out] pfc_enable pfc enable state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_pfc_port_pg_pfc_get(
    int unit,
    bcm_port_t port,
    int pg,
    int *pfc_enable);

/*
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
tm_ltsw_pfc_config_profile_id_set(
    int unit,
    int lport,
    int profile_id);

/*
 * \brief Set Recovery action to be performed in PFC deadlock recovery state.
 *
 * \param [in] unit Unit Number.
 * \param [in] action 0-Transmit Packets (Default), 1-Drop.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_pfc_deadlock_recovery_action_set(
    int unit,
    int action);

/*
 * \brief Get Recovery action to be performed in PFC deadlock recovery state.
 *
 * \param [in] unit Unit Number.
 * \param [out] action 0-Transmit Packets (Default), 1-Drop.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_pfc_deadlock_recovery_action_get(
    int unit,
    int *action);

/*
 * \brief Get config for the given (PFC priority, port) for the PFC Deadlock feature.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port ID.
 * \param [in] pfc_priority PFC Priority.
 * \param [in] type PFC deadlock control type.
 * \param [out] arg pointer to value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_pfc_deadlock_control_get(
    int unit,
    bcm_port_t port,
    int pfc_priority,
    bcm_cosq_pfc_deadlock_control_t type,
    int *arg);

/*
 * \brief Set config for the given (PFC priority, port) for the PFC Deadlock feature.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port ID.
 * \param [in] pfc_priority PFC Priority.
 * \param [in] type PFC deadlock control type.
 * \param [in] arg value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_pfc_deadlock_control_set(
    int unit,
    bcm_port_t port,
    int pfc_priority,
    bcm_cosq_pfc_deadlock_control_t type,
    int arg);

/*
 * \brief Get bitmap of Enabled(Admin) and Current Deadlock ports status for a given priority in PFC deadlock feature.
 *
 * \param [in] unit Unit Number.
 * \param [in] pfc_priority PFC Priority.
 * \param [out] pfc_deadlock_info Info for a given priority.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_pfc_deadlock_info_get(
    int unit,
    int pfc_priority,
    bcm_cosq_pfc_deadlock_info_t *pfc_deadlock_info);

/*
 * \brief Get the current Deadlock status for the given Port.
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport id.
 * \param [out] deadlock_status Deatlock status for the given port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_pfc_deadlock_queue_status_get(
    int unit,
    bcm_gport_t gport,
    uint8_t *deadlock_status);

/*
 * \brief Register pfc deadlock recovery callback in PFC deadlock feature.
 *
 * \param [in] unit Unit Number.
 * \param [in] callback callback function.
 * \param [in] userdata user data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_pfc_deadlock_recovery_event_register(
    int unit,
    bcm_cosq_pfc_deadlock_recovery_event_cb_t callback,
    void *userdata);

/*
 * \brief Unregister pfc deadlock recovery callback in PFC deadlock feature.
 *
 * \param [in] unit Unit Number.
 * \param [in] callback callback function.
 * \param [in] userdata user data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_pfc_deadlock_recovery_event_unregister(
    int unit,
    bcm_cosq_pfc_deadlock_recovery_event_cb_t callback,
    void *userdata);

#endif /* BCMINT_LTSW_TM_PFC_H */
