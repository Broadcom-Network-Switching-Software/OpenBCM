/*! \file switch.h
 *
 * MBCM switch control module interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef MBCM_LTSW_SC_H
#define MBCM_LTSW_SC_H

#include <bcm_int/ltsw/switch.h>
#include <bcm_int/ltsw/switch_int.h>

#include <bcmlt/bcmlt.h>

/*! Device-wide switch control set function prototype. */
typedef int (*mbcm_ltsw_sc_set_f)(int unit, bcm_switch_control_t type, int arg);

/*! Device-wide switch control get function prototype. */
typedef int (*mbcm_ltsw_sc_get_f)(int unit, bcm_switch_control_t type, int *arg);

/*! Port base switch control set function prototype. */
typedef int (*mbcm_ltsw_scp_set_f)(int unit, int port, bcm_switch_control_t type,
                                   int arg);

/*! Port base switch control get function prototype. */
typedef int (*mbcm_ltsw_scp_get_f)(int unit, int port, bcm_switch_control_t type,
                                   int *arg);

/*! Add switch protocol control entry */
typedef int (*mbcm_ltsw_spc_add_f)(int unit, uint32_t options,
                                   bcm_switch_pkt_protocol_match_t *match,
                                   bcm_switch_pkt_control_action_t *action,
                                   int priority);

/*! Get switch protocol control entry */
typedef int (*mbcm_ltsw_spc_get_f)(int unit,
                                   bcm_switch_pkt_protocol_match_t *match,
                                   bcm_switch_pkt_control_action_t *action,
                                   int *priority);

/*! Delete switch protocol control entry */
typedef int (*mbcm_ltsw_spc_del_f)(int unit,
                                   bcm_switch_pkt_protocol_match_t *match);

/*! Traverse switch protocol control entries. */
typedef int (*mbcm_ltsw_spc_trav_f)(int unit,
                                    bcm_switch_pkt_protocol_control_traverse_cb,
                                    void *user_data);

/*! Delete all switch protocol control entries. */
typedef int (*mbcm_ltsw_spc_del_all_f)(int unit);

/*! Add an entry into packet integrity check table. */
typedef int (*mbcm_ltsw_pkt_integrity_check_add_f)(int unit, uint32_t options,
                                                   bcm_switch_pkt_integrity_match_t *match,
                                                   bcm_switch_pkt_control_action_t *action,
                                                   int priority);

/*! Get an entry from packet integrity check table. */
typedef int (*mbcm_ltsw_pkt_integrity_check_get_f)(int unit,
                                                   bcm_switch_pkt_integrity_match_t *match,
                                                   bcm_switch_pkt_control_action_t *action,
                                                   int *priority);

/*! Delete an entry from packet integrity check table. */
typedef int (*mbcm_ltsw_pkt_integrity_check_delete_f)(int unit,
                                                      bcm_switch_pkt_integrity_match_t *match);

/*! Configure a switch drop event monitor. */
typedef int (*mbcm_ltsw_switch_drop_event_mon_set_f)(int unit,
                                                     bcm_switch_drop_event_mon_t *monitor);

/*! Get a switch drop event monitor configuration. */
typedef int (*mbcm_ltsw_switch_drop_event_mon_get_f)(int unit,
                                                     bcm_switch_drop_event_mon_t *monitor);

/*! Configure a switch trace event monitor. */
typedef int (*mbcm_ltsw_switch_trace_event_mon_set_f)(int unit,
                                                      bcm_switch_trace_event_mon_t *monitor);

/*! Get a switch trace event monitor configuration. */
typedef int (*mbcm_ltsw_switch_trace_event_mon_get_f)(int unit,
                                                      bcm_switch_trace_event_mon_t *monitor);

/*! Covert a drop event enum value to Logical Table symbol string. */
typedef int (*mbcm_ltsw_switch_drop_event_to_symbol_f)(int unit,
                                                       bcm_pkt_drop_event_t drop_event,
                                                       const char **table, const char **symbol);

/*! Covert a trace event enum value to Logical Table symbol string. */
typedef int (*mbcm_ltsw_switch_trace_event_to_symbol_f)(int unit,
                                                        bcm_pkt_trace_event_t trace_event,
                                                        const char **table, const char **symbol);

/*! Get the variant specific database. */
typedef int (*switch_db_get_f)(int unit, bcmint_switch_db_t *switch_db);

/*! Switch protocol packet control initialization. */
typedef int (*mbcm_ltsw_switch_protocol_init_f)(int unit);

/*! Switch protocol packet control operations handler. */
typedef int (*mbcm_ltsw_switch_protocol_handle_f)(int unit,
                                                  bcmlt_opcode_t opcode,
                                                  bcm_switch_pkt_protocol_match_t *match,
                                                  bcm_switch_pkt_control_action_t *action,
                                                  int *priority);

/*! Switch packet integrity check initialization. */
typedef int (*mbcm_ltsw_switch_pkt_integrity_check_init_f)(int unit);

/*! Switch packet integrity check operations handler. */
typedef int (*mbcm_ltsw_switch_pkt_integrity_handle_f)(int unit,
                                                       bcmlt_opcode_t opcode,
                                                       bcm_switch_pkt_integrity_match_t *match,
                                                       bcm_switch_pkt_control_action_t *action,
                                                       int *priority);

/*! Switch trace event monitor initialization. */
typedef int (*mbcm_ltsw_switch_mon_trace_event_init_f)(int unit);

/*! Switch module initialization. */
typedef int (*mbcm_ltsw_switch_init_f)(int unit);

/*! Switch module detach. */
typedef int (*mbcm_ltsw_switch_detach_f)(int unit);

/*! Create an EP Recirculate profile. */
typedef int (*mbcm_ltsw_switch_ep_recirc_profile_create_f)(int unit,
                                                           bcm_switch_ep_recirc_profile_t *recirc_profile,
                                                           int *recirc_profile_id);

/*! Destroy an EP Recirculate profile. */
typedef int (*mbcm_ltsw_switch_ep_recirc_profile_destroy_f)(int unit,
                                                            int recirc_profile_id);

/*! Get all EP Recirculate profile ids. */
typedef int (*mbcm_ltsw_switch_ep_recirc_profile_get_all_f)(int unit,
                                                            int *recirc_profile_id_array,
                                                            int *count);

/*! Get an EP Recirculate profile. */
typedef int (*mbcm_ltsw_switch_ep_recirc_profile_get_f)(int unit,
                                                        int recirc_profile_id,
                                                        bcm_switch_ep_recirc_profile_t *recirc_profile);

/*! Set drop event based EP Recirculate control. */
typedef int (*mbcm_ltsw_switch_ep_recirc_drop_event_control_set_f)(
    int unit,
    bcm_switch_ep_recirc_drop_event_control_t *drop_event_ctrl);


/*! Get drop event based EP Recirculate control. */
typedef int (*mbcm_ltsw_switch_ep_recirc_drop_event_control_get_f)(
    int unit,
    bcm_switch_ep_recirc_drop_event_control_t *drop_event_ctrl);

/*! Set trace event based EP Recirculate control. */
typedef int (*mbcm_ltsw_switch_ep_recirc_trace_event_control_set_f)(
    int unit,
    bcm_switch_ep_recirc_trace_event_control_t *trace_event_ctrl);

/*! Get trace event based EP Recirculate control. */
typedef int (*mbcm_ltsw_switch_ep_recirc_trace_event_control_get_f)(
    int unit,
    bcm_switch_ep_recirc_trace_event_control_t *trace_event_ctrl);

/*! Enable/disable an egress trace event to trigger EP Recirculate. */
typedef int (*mbcm_ltsw_switch_ep_recirc_trace_event_enable_set_f)(
    int unit,
    bcm_pkt_trace_event_t trace_event,
    int enable);

/*! Get all enabled egress trace event for EP Recirculate. */
typedef int (*mbcm_ltsw_switch_ep_recirc_trace_event_enable_get_all_f)(
    int unit,
    bcm_pkt_trace_event_t *trace_event_array,
    int *count);

/*! Get enabled status of an egress trace event for EP Recirculate. */
typedef int (*mbcm_ltsw_switch_ep_recirc_trace_event_enable_get_f)(
    int unit,
    bcm_pkt_trace_event_t trace_event,
    int *enable);

/* Set a mirror instance id to an EP recirculate profile entry. */
typedef int (*mbcm_ltsw_switch_ep_recirc_profile_mirror_instance_set_f)(
    int unit,
    int recirc_profile_id,
    int mirror_instance_id);

/* Get the mirror instance id from EP recirculate profile entries. */
typedef int (*mbcm_ltsw_switch_ep_recirc_profile_mirror_instance_get_f)(
    int unit,
    int *mirror_instance_id);

/*!
 * \brief Switch control module driver function vector.
 *
 * Provides driver function access.
 */
typedef struct mbcm_ltsw_sc_drv_s {

    /*! Set per port control. */
    mbcm_ltsw_sc_set_f sc_set;

    /*! Get per port control. */
    mbcm_ltsw_sc_get_f sc_get;

    /*! Set switch-wide control. */
    mbcm_ltsw_scp_set_f scp_set;

    /*! get switch-wide control. */
    mbcm_ltsw_scp_get_f scp_get;

    /*! Add switch protocol control entry */
    mbcm_ltsw_spc_add_f spc_add;

    /*! Get switch protocol control entry */
    mbcm_ltsw_spc_get_f spc_get;

    /*! Delete switch protocol control entry */
    mbcm_ltsw_spc_del_f spc_del;

    /*! Traverse switch protocol control entries. */
    mbcm_ltsw_spc_trav_f spc_trav;

    /*! Delete all switch protocol control entries. */
    mbcm_ltsw_spc_del_all_f spc_del_all;

    /*! Add an entry into packet integrity check table. */
    mbcm_ltsw_pkt_integrity_check_add_f switch_pkt_integrity_check_add;

    /*! Get an entry from packet integrity check table. */
    mbcm_ltsw_pkt_integrity_check_get_f switch_pkt_integrity_check_get;

    /*! Delete an entry from packet integrity check table. */
    mbcm_ltsw_pkt_integrity_check_delete_f switch_pkt_integrity_check_delete;

    /*! Configure a switch drop event monitor. */
    mbcm_ltsw_switch_drop_event_mon_set_f switch_drop_event_mon_set;

    /*! Get a switch drop event monitor configuration. */
    mbcm_ltsw_switch_drop_event_mon_get_f switch_drop_event_mon_get;

    /*! Configure a switch trace event monitor. */
    mbcm_ltsw_switch_trace_event_mon_set_f switch_trace_event_mon_set;

    /*! Get a switch trace event monitor configuration. */
    mbcm_ltsw_switch_trace_event_mon_get_f switch_trace_event_mon_get;

    /*! Covert a drop event enum value to Logical Table symbol string. */
    mbcm_ltsw_switch_drop_event_to_symbol_f switch_drop_event_to_symbol;

    /*! Covert a trace event enum value to Logical Table symbol string. */
    mbcm_ltsw_switch_trace_event_to_symbol_f switch_trace_event_to_symbol;

    /*! Get the variant specific database. */
    switch_db_get_f switch_db_get;

    /*! Switch protocol packet control initialization. */
    mbcm_ltsw_switch_protocol_init_f switch_protocol_init;

    /*! Switch protocol packet control operations handler. */
    mbcm_ltsw_switch_protocol_handle_f switch_protocol_handle;

    /*! Switch packet integrity check initialization. */
    mbcm_ltsw_switch_pkt_integrity_check_init_f switch_pkt_integrity_check_init;

    /*! Switch packet integrity check operations handler. */
    mbcm_ltsw_switch_pkt_integrity_handle_f switch_pkt_integrity_handle;

    /*! Switch trace event monitor initialization. */
    mbcm_ltsw_switch_mon_trace_event_init_f switch_mon_trace_event_init;

    /*! Switch module initialization. */
    mbcm_ltsw_switch_init_f switch_init;

    /*! Switch module detach. */
    mbcm_ltsw_switch_detach_f switch_detach;

    /*! Create an EP Recirculate profile. */
    mbcm_ltsw_switch_ep_recirc_profile_create_f switch_ep_recirc_profile_create;

    /*! Destroy an EP Recirculate profile. */
    mbcm_ltsw_switch_ep_recirc_profile_destroy_f switch_ep_recirc_profile_destroy;

    /*! Get all EP Recirculate profile ids. */
    mbcm_ltsw_switch_ep_recirc_profile_get_all_f switch_ep_recirc_profile_get_all;

    /*! Get an EP Recirculate profile. */
    mbcm_ltsw_switch_ep_recirc_profile_get_f switch_ep_recirc_profile_get;

    /*! Set drop event based EP Recirculate control. */
    mbcm_ltsw_switch_ep_recirc_drop_event_control_set_f switch_ep_recirc_drop_event_control_set;

    /*! Get drop event based EP Recirculate control. */
    mbcm_ltsw_switch_ep_recirc_drop_event_control_get_f switch_ep_recirc_drop_event_control_get;

    /*! Set trace event based EP Recirculate control. */
    mbcm_ltsw_switch_ep_recirc_trace_event_control_set_f switch_ep_recirc_trace_event_control_set;

    /*! Get trace event based EP Recirculate control. */
    mbcm_ltsw_switch_ep_recirc_trace_event_control_get_f switch_ep_recirc_trace_event_control_get;

    /*! Enable/disable an egress trace event to trigger EP Recirculate. */
    mbcm_ltsw_switch_ep_recirc_trace_event_enable_set_f switch_ep_recirc_trace_event_enable_set;

    /*! Get all enabled egress trace event for EP Recirculate. */
    mbcm_ltsw_switch_ep_recirc_trace_event_enable_get_all_f switch_ep_recirc_trace_event_enable_get_all;

    /*! Get enabled status of an egress trace event for EP Recirculate. */
    mbcm_ltsw_switch_ep_recirc_trace_event_enable_get_f switch_ep_recirc_trace_event_enable_get;

    /* Set a mirror instance id to an EP recirculate profile entry. */
    mbcm_ltsw_switch_ep_recirc_profile_mirror_instance_set_f switch_ep_recirc_profile_mirror_instance_set;

    /* Get the mirror instance id from EP recirculate profile entries. */
    mbcm_ltsw_switch_ep_recirc_profile_mirror_instance_get_f switch_ep_recirc_profile_mirror_instance_get;

} mbcm_ltsw_sc_drv_t;

/*!
 * \brief Hook a SC driver onto the unit.
 *
 * \param [in] unit Unit number.
 * \param [in] drv SC driver to set.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_sc_drv_set(int unit, mbcm_ltsw_sc_drv_t *drv);

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
mbcm_ltsw_scp_set(int unit, int port, bcm_switch_control_t type, int arg);

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
mbcm_ltsw_scp_get(int unit, int port, bcm_switch_control_t type, int *arg);

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
mbcm_ltsw_sc_set(int unit, bcm_switch_control_t type, int arg);

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
mbcm_ltsw_sc_get(int unit, bcm_switch_control_t type, int *arg);

/*!
 * \brief Add a protocol control entry.
 *
 * \param [in] unit Unit number
 * \param [in] options Options flags.
 * \param [in] match Protocol match structure.
 * \param [in] action Packet control action structure.
 * \param [in] priority Entry priority.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_spc_add(int unit, uint32_t options,
                  bcm_switch_pkt_protocol_match_t *match,
                  bcm_switch_pkt_control_action_t *action,
                  int priority);

/*!
 * \brief Get action and priority for a protocol control entry.
 *
 * \param [in] unit Unit number
 * \param [in] match Protocol match structure.
 * \param [out] action Packet control action structure.
 * \param [out] priority Entry priority.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_spc_get(int unit,
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
mbcm_ltsw_spc_delete(int unit, bcm_switch_pkt_protocol_match_t *match);

/*!
 * \brief Traverse switch protocol control entries.
 *
 * \param [in] unit Unit number
 * \param [in] cb Protocol control traverse callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_spc_traverse(int unit, bcm_switch_pkt_protocol_control_traverse_cb cb,
                       void *user_data);

/*!
 * \brief Delete all protocol control entries.
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_spc_delete_all(int unit);

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
mbcm_ltsw_switch_pkt_integrity_check_add(int unit, uint32 options,
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
mbcm_ltsw_switch_pkt_integrity_check_get(int unit,
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
mbcm_ltsw_switch_pkt_integrity_check_delete(int unit,
                                            bcm_switch_pkt_integrity_match_t *match);

/*!
 * \brief Configure a switch drop event monitor.
 *
 * This function configures monitor for a given drop event. The caller must specify
 * a switch drop event and the associated actions in the monitor structure.
 *
 * \param [in] unit Unit number.
 * \param [in] monitor Pointer to a switch drop event monitor structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
mbcm_ltsw_switch_drop_event_mon_set(int unit, bcm_switch_drop_event_mon_t *monitor);

/*!
 * \brief Get a switch drop event monitor configuration.
 *
 * This function get monitor configuration for a given drop event. The caller must specify
 * a drop event in the monitor structure. The associated actions will be returned.
 *
 * \param [in] unit Unit number.
 * \param [inout] monitor Pointer to a switch drop event monitor structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
mbcm_ltsw_switch_drop_event_mon_get(int unit, bcm_switch_drop_event_mon_t *monitor);

/*!
 * \brief Configure a switch trace event monitor.
 *
 * This function configures monitor for a given trace event. The caller must specify a
 * switch trace event and the associated actions in the monitor structure.
 *
 * \param [in] unit Unit number.
 * \param [in] monitor Pointer to a switch trace event monitor structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
mbcm_ltsw_switch_trace_event_mon_set(int unit, bcm_switch_trace_event_mon_t *monitor);


/*!
 * \brief Get a switch trace event monitor configuration.
 *
 * This function get monitor configuration for a given  trace event. The caller must specify
 * a trace event in the monitor structure. The associated actions will be returned.
 *
 * \param [in] unit Unit number.
 * \param [inout] monitor Pointer to a switch trace event monitor structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
mbcm_ltsw_switch_trace_event_mon_get(int unit, bcm_switch_trace_event_mon_t *monitor);

/*!
 * \brief Covert a drop event enum value to Logical Table symbol string.
 *
 * \param [in] unit Unit number.
 * \param [in] drop_event Drop event enum value.
 * \param [out] table Logical Table name.
 * \param [out] symbol Logical Table symbol string.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid drop event enum value.
 * \retval SHR_E_NOT_FOUND Symbol string not found for the specified drop event.
 */
extern int
mbcm_ltsw_switch_drop_event_to_symbol(int unit, bcm_pkt_drop_event_t drop_event,
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
 * \retval SHR_E_PARAM Invalid trace event enum value.
 * \retval SHR_E_NOT_FOUND Symbol string not found for the specified trace event.
 */
extern int
mbcm_ltsw_switch_trace_event_to_symbol(int unit, bcm_pkt_trace_event_t trace_event,
                                       const char **table, const char **symbol);

/*!
 * \brief Set a mirror instance id to an EP recirculate profile entry.
 *
 * \param [in] unit Unit number.
 * \param [in] recirc_profile_id EP Recirculate profile id.
 * \param [in] id Mirror instance id.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_switch_ep_recirc_profile_mirror_instance_set(int unit,
                                                       int recirc_profile_id,
                                                       int id);

/*!
 * \brief Get the variant specific database.
 *
 * \param [in] unit Unit Number.
 * \param [out] switch_db Switch database structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_switch_db_get(int unit, bcmint_switch_db_t *switch_db);

/*!
 * \brief Switch protocol packet control initialization.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_switch_protocol_init(int unit);

/*!
 * \brief Switch protocol packet control operations handler.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_switch_protocol_handle(int unit, bcmlt_opcode_t opcode,
                                 bcm_switch_pkt_protocol_match_t *match,
                                 bcm_switch_pkt_control_action_t *action,
                                 int *priority);


/*!
 * \brief Switch packet integrity check initialization.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_switch_pkt_integrity_check_init(int unit);

/*!
 * \brief Switch packet integrity check operations handler.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_switch_pkt_integrity_handle(int unit, bcmlt_opcode_t opcode,
                                      bcm_switch_pkt_integrity_match_t *match,
                                      bcm_switch_pkt_control_action_t *action,
                                      int *priority);

/*!
 * \brief Switch trace event monitor initialization.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_switch_mon_trace_event_init(int unit);

/*!
 * \brief Switch module init.
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
mbcm_ltsw_switch_init(int unit);

/*!
 * \brief Switch module detach.
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
mbcm_ltsw_switch_detach(int unit);

/*!
 * \brief Create an EP Recirculate profile.
 *
 * \param [in] unit Unit number.
 * \param [in/out] recirc_profile The EP Recirculate profile config structure.
 * \param [out] recirc_profile_id The EP Recirculate config profile id.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_switch_ep_recirc_profile_create(
    int unit,
    bcm_switch_ep_recirc_profile_t *recirc_profile,
    int *recirc_profile_id);

/*!
 * \brief Destroy an EP Recirculate profile.
 *
 * \param [in] unit Unit number.
 * \param [in] recirc_profile_id The EP Recirculate config profile id.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_switch_ep_recirc_profile_destroy(
    int unit,
    int recirc_profile_id);

/*!
 * \brief Get all EP Recirculate profile ids.
 *
 * If recirc_profile_id_array == NULL, count will be OUT for the number of
 * profiles; if the size cannot fit, it will only copy according to the size.
 *
 * \param [in] unit Unit number.
 * \param [out] recirc_profile_id_array EP Recirculate profile id array.
 * \param [in/out] count Count of the EP Recirculate profiles.
 *
 * \retval BCM_E_NONE on success and error code otherwise.
 */
extern int
mbcm_ltsw_switch_ep_recirc_profile_get_all(
    int unit,
    int *recirc_profile_id_array,
    int *count);

/*!
 * \brief Get an EP Recirculate profile.
 *
 * \param [in] unit Unit number.
 * \param [in] recirc_profile_id The EP Recirculate profile id.
 * \param [out] recirc_profile The EP Recirculate profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_switch_ep_recirc_profile_get(
    int unit,
    int recirc_profile_id,
    bcm_switch_ep_recirc_profile_t *recirc_profile);

/*!
 * \brief Set drop event based EP Recirculate control.
 *
 * \param [in] unit Unit number.
 * \param [in] drop_event_ctrl The recirculate drop event control.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_switch_ep_recirc_drop_event_control_set(
    int unit,
    bcm_switch_ep_recirc_drop_event_control_t *drop_event_ctrl);


/*!
 * \brief Get drop event based EP Recirculate control.
 *
 * \param [in] unit Unit number.
 * \param [out] drop_event_ctrl The recirculate drop event control.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_switch_ep_recirc_drop_event_control_get(
    int unit,
    bcm_switch_ep_recirc_drop_event_control_t *drop_event_ctrl);

/*!
 * \brief Set trace event based EP Recirculate control.
 *
 * \param [in] unit Unit number.
 * \param [in] trace_event_ctrl The recirculate trace event control.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_switch_ep_recirc_trace_event_control_set(
    int unit,
    bcm_switch_ep_recirc_trace_event_control_t *trace_event_ctrl);

/*!
 * \brief Get trace event based EP Recirculate control.
 *
 * \param [in] unit Unit number.
 * \param [out] trace_event_ctrl The recirculate trace event control.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_switch_ep_recirc_trace_event_control_get(
    int unit,
    bcm_switch_ep_recirc_trace_event_control_t *trace_event_ctrl);

/*!
 * \brief Enable/disable an egress trace event to trigger EP Recirculate.
 *
 * Users need to refer to related documents for the currently supported
 * trace event types.
 *
 * \param [in] unit Unit number.
 * \param [in] trace_event The trace event.
 * \param [in] enable The enabled status of the trace event.
 *
 * 
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_switch_ep_recirc_trace_event_enable_set(
    int unit,
    bcm_pkt_trace_event_t trace_event,
    int enable);

/*!
 * \brief Get all enabled egress trace event for EP Recirculate.
 *
 * If trace_event_array == NULL, count will be OUT for the number of
 * events; if the size cannot fit, it will only copy according to the size;
 *
 * \param [in] unit Unit number.
 * \param [out] trace_event_array Trace event array.
 * \param [in/out] count Count of the enabled trace events.
 *
 * \retval BCM_E_NONE on success and error code otherwise.
 */
extern int
mbcm_ltsw_switch_ep_recirc_trace_event_enable_get_all(
    int unit,
    bcm_pkt_trace_event_t *trace_event_array,
    int *count);

/*!
 * \brief Get enabled status of an egress trace event for EP Recirculate.
 *
 * Users need to refer to related documents for the currently supported
 * trace event types.
 *
 * \param [in] unit Unit number.
 * \param [in] trace_event The trace event.
 * \param [out] enable The enabled status of the trace event.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_switch_ep_recirc_trace_event_enable_get(
    int unit,
    bcm_pkt_trace_event_t trace_event,
    int *enable);

/*!
 * \brief Set a mirror instance id to an EP recirculate profile entry.
 *
 * \param [in] unit Unit number.
 * \param [in] recirc_profile_id EP Recirculate profile id.
 * \param [in] mirror_instance_id Mirror instance id.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_switch_ep_recirc_profile_mirror_instance_set(
    int unit,
    int recirc_profile_id,
    int mirror_instance_id);

/*!
 * \brief Get the mirror instance id from EP recirculate profile entries.
 *
 * \param [in] unit Unit number.
 * \param [out] mirror_instance_id Mirror instance id.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_switch_ep_recirc_profile_mirror_instance_get(
    int unit,
    int *mirror_instance_id);

#endif /* MBCM_LTSW_SC_H */
