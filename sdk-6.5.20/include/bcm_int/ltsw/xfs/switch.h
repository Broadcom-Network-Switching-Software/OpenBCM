/*! \file switch.h
 *
 * Switch control function declare APIs for XFS devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_XFS_SC_H
#define BCMINT_XFS_SC_H

#include <bcm/switch.h>

/*!
 * \brief Configure a port based control.
 *
 * \param [in] unit Unit number
 * \param [in] port Port number.
 * \param [in] type Control type.
 * \param [in] arg Control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Entry not found.
 */
extern int
bcmint_xfs_scp_set(int unit, int port, bcm_switch_control_t type, int arg);

/*!
 * \brief Retrieve a port based control setting.
 *
 * \param [in] unit Unit number
 * \param [in] port Port number.
 * \param [in] type Control type.
 * \param [out] arg Control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Entry not found.
 */
extern int
bcmint_xfs_scp_get(int unit, int port, bcm_switch_control_t type, int *arg);

/*!
 * \brief Configure a device-wide control.
 *
 * \param [in] unit Unit number
 * \param [in] type Control type.
 * \param [in] arg Control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Entry not found.
 */
extern int
bcmint_xfs_sc_set(int unit, bcm_switch_control_t type, int arg);


/*!
 * \brief Retrieve a device-wide control setting.
 *
 * \param [in] unit Unit number
 * \param [in] type Control type.
 * \param [out] arg Control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Entry not found.
 */
extern int
bcmint_xfs_sc_get(int unit, bcm_switch_control_t type, int *arg);

/*!
 * \brief Add a protocol control entry.
 *
 * \param [in] unit Unit number
 * \param [in] options Options flags.
 * \param [in] match Protocol match structure.
 * \param [in] action Packet control action.
 * \param [in] priority Entry priority.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmint_xfs_spc_add(int unit, uint32_t options,
                   bcm_switch_pkt_protocol_match_t *match,
                   bcm_switch_pkt_control_action_t *action,
                   int priority);

/*!
 * \brief Get action and priority for a protocol control entry.
 *
 * \param [in] unit Unit number
 * \param [in] match Protocol match structure.
 * \param [out] action Packet control action.
 * \param [out] priority Entry priority.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmint_xfs_spc_get(int unit,
                   bcm_switch_pkt_protocol_match_t *match,
                   bcm_switch_pkt_control_action_t *action,
                   int *priority);

/*!
 * \brief Delete a protocol control entry.
 *
 * \param [in] unit Unit number
 * \param [in] match Protocol match structure.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmint_xfs_spc_delete(int unit, bcm_switch_pkt_protocol_match_t *match);

/*!
 * \brief Add an entry into packet integrity check table.
 *
 * \param [in] unit Unit number.
 * \param [in] option Options flag.
 * \param [in] match Pointer to a packet integrity match attribute structure.
 * \param [in] action Pointer to a packet control action structure.
 * \param [in] priority Entry priority.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_xfs_switch_pkt_integrity_check_add(int unit, uint32 options,
                                          bcm_switch_pkt_integrity_match_t *match,
                                          bcm_switch_pkt_control_action_t *action,
                                          int priority);

/*!
 * \brief Get an entry from packet integrity check table.
 *
 * \param [in] unit Unit number.
 * \param [in] match Pointer to a packet integrity match attribute structure.
 * \param [out] action Pointer to a packet control action structure.
 * \param [out] priority Entry priority.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_xfs_switch_pkt_integrity_check_get(int unit,
                                          bcm_switch_pkt_integrity_match_t *match,
                                          bcm_switch_pkt_control_action_t *action,
                                          int *priority);

/*!
 * \brief Delete an entry from packet integrity check table.
 *
 * \param [in] unit Unit number.
 * \param [in] match Pointer to a packet integrity match structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_xfs_switch_pkt_integrity_check_delete(int unit,
                                             bcm_switch_pkt_integrity_match_t *match);

/*!
 * \brief Configure a switch drop event monitor.
 *
 * This function configures monitor for a given switch drop event. The caller must specify
 * a switch drop event and the associated actions in the monitor structure.
 *
 * \param [in] unit Unit number.
 * \param [in] monitor Pointer to a switch drop event monitor structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_xfs_switch_drop_event_mon_set(int unit, bcm_switch_drop_event_mon_t *monitor);

/*!
 * \brief Get a switch drop event monitor configuration.
 *
 * This function get monitor configuration for a given switch drop event. The caller must specify
 * a drop event in the monitor structure. The associated actions will be returned.
 *
 * \param [in] unit Unit number.
 * \param [inout] monitor Pointer to a switch drop event monitor structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_xfs_switch_drop_event_mon_get(int unit, bcm_switch_drop_event_mon_t *monitor);

/*!
 * \brief Configure a switch trace event monitor.
 *
 * This function configures monitor for a given switch trace event. The caller must specify a
 * switch trace event and the associated actions in the monitor structure.
 *
 * \param [in] unit Unit number.
 * \param [in] monitor Pointer to a switch trace event monitor structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_xfs_switch_trace_event_mon_set(int unit, bcm_switch_trace_event_mon_t *monitor);


/*!
 * \brief Get a switch trace event monitor configuration.
 *
 * This function get monitor configuration for a given switch trace event. The caller must specify
 * a trace event in the monitor structure. The associated actions will be returned.
 *
 * \param [in] unit Unit number.
 * \param [inout] monitor Pointer to a switch trace event monitor structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_xfs_switch_trace_event_mon_get(int unit, bcm_switch_trace_event_mon_t *monitor);

/*!
 * \brief Covert a drop event enum value to Logical Table symbol string.
 *
 * \param [in] unit Unit number.
 * \param [in] drop_event Drop event enum value.
 * \param [out] table Logical Table name.
 * \param [out] symbol Logical Table symbol string.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid switch drop event enum value.
 * \retval SHR_E_NOT_FOUND Symbol string not found for the specified drop event.
 */
extern int
bcmint_xfs_switch_drop_event_to_symbol(int unit, bcm_pkt_drop_event_t drop_event,
                                      const char **table, const char **symbol);

/*!
 * \brief Covert a trace event enum value to Logical Table symbol string.
 *
 * \param [in] unit Unit number.
 * \param [in] trace_event Trace event enum value.
 * \param [out] table Logical Table name.
 * \param [out] symbol Logical Table symbol string.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid switch trace event enum value.
 * \retval SHR_E_NOT_FOUND Symbol string not found for the specified trace event.
 */
extern int
bcmint_xfs_switch_trace_event_to_symbol(int unit, bcm_pkt_trace_event_t trace_event,
                                       const char **table, const char **symbol);

/*!
 * \brief Convert an EP Recirculate drop event value to Logical Table symbol string.
 *
 * \param [in] unit Unit number.
 * \param [in] drop_event Drop event value.
 * \param [out] symbol Logical Table symbol string.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmint_xfs_switch_ep_recirc_drop_event_to_symbol(int unit,
                                                 int drop_event,
                                                 const char **symbol);
/*!
 * \brief Convert an EP Recirculate Logical Table symbol string to drop event value.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol Logical Table symbol string.
 * \param [out] drop_event Drop event value.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmint_xfs_switch_ep_recirc_symbol_to_drop_event(int unit,
                                                 const char *symbol,
                                                 int *drop_event);

/*!
 * \brief Get the number of EP Recirculate trace events.
 *
 * \param [in] unit Unit number.
 * \param [out] num Number of trace events.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmint_xfs_switch_ep_recirc_trace_event_num_get(int unit, int *num);

/*!
 * \brief Convert an EP Recirculate trace event enum value to Logical Table symbol string.
 *
 * \param [in] unit Unit number.
 * \param [in] trace_event Trace event enum value.
 * \param [out] symbol Logical Table symbol string.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmint_xfs_switch_ep_recirc_trace_event_to_symbol(int unit,
                                                  bcm_pkt_trace_event_t trace_event,
                                                  const char **symbol);

/*!
 * \brief Convert an EP Recirculate Logical Table symbol string to trace event enum value.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol Logical Table symbol string.
 * \param [out] trace_event Trace event enum value.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmint_xfs_switch_ep_recirc_symbol_to_trace_event(int unit,
                                                  const char *symbol,
                                                  bcm_pkt_trace_event_t *trace_event);

/*!
 * \brief Get an EP Recirculate Logical table symbol string from trace event index.
 *
 * \param [in] unit Unit number.
 * \param [in] index Trace event index.
 * \param [out] symbol Logical Table symbol string.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmint_xfs_switch_ep_recirc_trace_event_symbol_iterate_get(int unit,
                                                           int index,
                                                           const char **symbol);

/*!
 * \brief Switch module initialization.
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Entry not found.
 */
extern int
bcmint_xfs_switch_init(int unit);

/*!
 * \brief Switch module detach.
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Entry not found.
 */
extern int
bcmint_xfs_switch_detach(int unit);

#endif /* BCMINT_XFS_SC_H */
