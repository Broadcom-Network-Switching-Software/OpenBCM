/*! \file vlan.h
 *
 * Vlan headfiles to declare internal APIs for XFS devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef XFS_LTSW_VLAN_H
#define XFS_LTSW_VLAN_H

#include <bcm/vlan.h>
#include <bcm/types.h>
#include <bcm_int/ltsw/vlan.h>
#include <bcm_int/ltsw/flexctr.h>
#include <bcm_int/ltsw/profile_mgmt.h>
#include <sal/sal_types.h>
#include <bcmltd/bcmltd_lt_types.h>

/*!
 * \brief Initialize VLAN module.
 *
 * \param [in] unit      Unit Number.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
extern int
xfs_ltsw_vlan_init(int unit);

/*!
 * \brief De-initialize VLAN module.
 *
 * \param [in] unit      Unit Number.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
extern int
xfs_ltsw_vlan_detach(int unit);

/*!
 * \brief Initialize VLAN outer TPID information.
 *
 * \param [in] unit      Unit Number.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
extern int
xfs_ltsw_vlan_otpid_init(int unit);

/*!
 * \brief De-initialize VLAN outer TPID information.
 *
 * \param [in] unit      Unit Number.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
extern int
xfs_ltsw_vlan_otpid_detach(int unit);

/*!
 * \brief Add VLAN outer TPID.
 *
 * \param [in] unit      Unit Number.
 * \param [in] index     Entry index.
 * \param [in] tpid      Outer TPID value.
 *
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
extern int
xfs_ltsw_vlan_otpid_add(int unit, int index, uint16_t tpid);

/*!
 * \brief Set global inner TPID.
 *
 * This function is used to set global inner TPID.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  tpid                 TPID to be set.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
xfs_ltsw_vlan_itpid_set(int unit, uint16_t tpid);

/*!
 * \brief Get global inner TPID.
 *
 * This function is used to get global inner TPID.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] tpid                 Inner TPID.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
xfs_ltsw_vlan_itpid_get(int unit, uint16_t *tpid);

/*!
 * \brief Add member ports into a specified vlan.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  vid           VLAN ID.
 * \param [in]  ing_pbmp      Ingress member ports.
 * \param [in]  egr_pbmp      Egress member ports.
 * \param [in]  egr_ubmp      Untagged member ports.
 * \param [in]  flags         BCM_VLAN_PORT_XXX flags.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
xfs_ltsw_vlan_port_add(int unit,
                       bcm_vlan_t vid,
                       bcm_pbmp_t ing_pbmp,
                       bcm_pbmp_t egr_pbmp,
                       bcm_pbmp_t egr_ubmp,
                       int flags);

/*!
 * \brief Get member ports from a specified vlan.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  vid           VLAN ID.
 * \param [out] ing_pbmp      Ingress member ports.
 * \param [out] egr_pbmp      Egress member ports.
 * \param [out] egr_ubmp      Untagged member ports.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
xfs_ltsw_vlan_port_get(int unit,
                       bcm_vlan_t vid,
                       bcm_pbmp_t *ing_pbmp,
                       bcm_pbmp_t *egr_pbmp,
                       bcm_pbmp_t *egr_ubmp);

/*!
 * \brief Remove member ports from a specified vlan.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  vid           VLAN ID.
 * \param [in]  ing_pbmp      Ingress member ports.
 * \param [in]  egr_pbmp      Egress member ports.
 * \param [in]  egr_ubmp      Untagged member ports.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
xfs_ltsw_vlan_port_remove(int unit,
                          bcm_vlan_t vid,
                          bcm_pbmp_t ing_pbmp,
                          bcm_pbmp_t egr_pbmp,
                          bcm_pbmp_t egr_ubmp);

/*!
 * \brief Set the state of VLAN membership check on a given port.
 *
 * This function is used to set the state of VLAN membership check on a given
 * port.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port number.
 * \param [in] disable           Disable or enable.
 * \param [in] flags             Direction flags.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_port_check_set(int unit, bcm_port_t port,
                             int disable, uint16_t flags);

/*!
 * \brief Set per VLAN configuration.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vlan            VLAN ID.
 * \param [in]   valid_fields    Valid fields for VLAN control structure,
 *                               BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 * \param [in]   control         VLAN control structure.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
extern int
xfs_ltsw_vlan_control_vlan_set(int unit,
                               bcm_vlan_t vid,
                               uint32_t valid_fields,
                               bcm_vlan_control_vlan_t *control);

/*!
 * \brief Get per VLAN configuration.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vlan            VLAN ID.
 * \param [in]   valid_fields    Valid fields for VLAN control structure,
 *                               BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 * \param [out]  control         VLAN control structure.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
extern int
xfs_ltsw_vlan_control_vlan_get(int unit,
                               bcm_vlan_t vid,
                               uint32_t valid_fields,
                               bcm_vlan_control_vlan_t *control);

/*!
 * \brief Set miscellaneous port-specific VLAN options.
 *
 * This function is used to set miscellaneous port-specific VLAN options.
 *
 * \param [in]  unit             Unit number.
 * \param [in]  port             PORT ID.
 * \param [in]  type             Control port type.
 * \param [in]  arg              Arg vlaue which is dependent on control type.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_control_port_set(int unit,
                               bcm_port_t port,
                               bcm_vlan_control_port_t type,
                               int arg);

/*!
 * \brief Get miscellaneous port-specific VLAN options.
 *
 * This function is used to get miscellaneous port-specific VLAN options.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  port            PORT ID.
 * \param [in]  type            Control port type.
 * \param [out] arg             Arg vlaue which is dependent on control type.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
extern int
xfs_ltsw_vlan_control_port_get(int unit,
                               bcm_port_t port,
                               bcm_vlan_control_port_t type,
                               int *arg);

/*!
 * \brief Add an entry to ingress VLAN translation table.
 *
 * This function is used to add an entry to ingress VLAN translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] cfg               Ingress xlate configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_ing_xlate_add(int unit,
                            bcm_vlan_translate_key_t key_type,
                            bcm_gport_t port,
                            bcm_vlan_t outer_vlan,
                            bcm_vlan_t inner_vlan,
                            bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg);

/*!
 * \brief Update an entry in ingress VLAN translation table.
 *
 * This function is used to update an entry in ingress VLAN translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] cfg               Ingress xlate configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_ing_xlate_update(int unit,
                               bcm_vlan_translate_key_t key_type,
                               bcm_gport_t port,
                               bcm_vlan_t outer_vlan,
                               bcm_vlan_t inner_vlan,
                               bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg);

/*!
 * \brief Get an entry from ingress VLAN translation table.
 *
 * This function is used to get an entry from ingress VLAN translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [out] cfg              Ingress xlate configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_ing_xlate_get(int unit,
                            bcm_vlan_translate_key_t key_type,
                            bcm_gport_t port,
                            bcm_vlan_t outer_vlan,
                            bcm_vlan_t inner_vlan,
                            bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg);

/*!
 * \brief Delete an entry from ingress VLAN translation table.
 *
 * This function is used to delete an entry from ingress VLAN translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_ing_xlate_delete(int unit,
                               bcm_vlan_translate_key_t key_type,
                               bcm_gport_t port,
                               bcm_vlan_t outer_vlan,
                               bcm_vlan_t inner_vlan);

/*!
 * \brief Traverse over all entries in ingress translation table.
 *
 * This function is used to traverse over all ingress translation entries and
 * call given callback with action structure.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Call back function.
 * \param [in] user_data         User data to a call back.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_ing_xlate_traverse(int unit,
                                 bcmi_ltsw_vlan_ing_xlate_traverse_cb cb,
                                 void *user_data);

/*!
 * \brief Add an action entry to ingress VLAN translation table.
 *
 * This function is used to add an action entry to ingress VLAN translation
 * table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] action            VLAN Actions.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_ing_xlate_action_add(int unit,
                                   bcm_vlan_translate_key_t key_type,
                                   bcm_gport_t port,
                                   bcm_vlan_t outer_vlan,
                                   bcm_vlan_t inner_vlan,
                                   bcm_vlan_action_set_t *action);

/*!
 * \brief Get an action entry from ingress VLAN translation table.
 *
 * This function is used to get an action entry from ingress VLAN translation
 * table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] action            VLAN Actions.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_ing_xlate_action_get(int unit,
                                   bcm_vlan_translate_key_t key_type,
                                   bcm_gport_t port,
                                   bcm_vlan_t outer_vlan,
                                   bcm_vlan_t inner_vlan,
                                   bcm_vlan_action_set_t *action);

/*!
 * \brief Delete an action entry from ingress VLAN translation table.
 *
 * This function is used to delete an action entry from ingress VLAN translation
 * table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] action            VLAN Actions.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_ing_xlate_action_delete(int unit,
                                      bcm_vlan_translate_key_t key_type,
                                      bcm_gport_t port,
                                      bcm_vlan_t outer_vlan,
                                      bcm_vlan_t inner_vlan);

/*!
 * \brief Traverse over all entries in ingress translation table.
 *
 * This function is used to traverse over all ingress translation entries and
 * call given callback with action structure.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Call back function.
 * \param [in] user_data         User data to a call back.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_ing_xlate_action_traverse(
    int unit,
    bcm_vlan_translate_action_traverse_cb cb,
    void *user_data);

/*!
 * \brief Add an action entry to egress VLAN translation table.
 *
 * This function is used to add an action entry to egress VLAN translation
 * table.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] action            VLAN Actions.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_egr_xlate_action_add(int unit,
                                   bcm_gport_t port,
                                   bcm_vlan_t outer_vlan,
                                   bcm_vlan_t inner_vlan,
                                   bcm_vlan_action_set_t *action);

/*!
 * \brief Get an action entry from egress VLAN translation table.
 *
 * This function is used to get an action entry from egress VLAN translation
 * table.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] action            VLAN Actions.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_egr_xlate_action_get(int unit,
                                   bcm_gport_t port,
                                   bcm_vlan_t outer_vlan,
                                   bcm_vlan_t inner_vlan,
                                   bcm_vlan_action_set_t *action);

/*!
 * \brief Delete an action entry from egress VLAN translation table.
 *
 * This function is used to delete an action entry from egress VLAN translation
 * table.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] action            VLAN Actions.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_egr_xlate_action_delete(int unit,
                                      bcm_gport_t port,
                                      bcm_vlan_t outer_vlan,
                                      bcm_vlan_t inner_vlan);

/*!
 * \brief Traverse over all entries in egress translation table.
 *
 * This function is used to traverse over all egress translation entries and
 * call given callback with action structure.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Call back function.
 * \param [in] user_data         User data to a call back.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_egr_xlate_action_traverse(
    int unit,
    bcm_vlan_translate_egress_action_traverse_cb cb,
    void *user_data);

/*!
 * \brief Set force VLAN port type into VLAN table.
 *
 * This function is used to set force VLAN port type into VLAN table.
 *
 * \param [in] unit              Unit number.
 * \param [in] vlan              VLAN ID.
 * \param [in] port_type         Force VLAN port type -
 *                               BCM_PORT_FORCE_VLAN_*_PORT.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_force_port_set(int unit, int vlan, int port_type);

/*!
 * \brief Get force VLAN port type from VLAN table.
 *
 * This function is used to get force VLAN port type from VLAN table.
 *
 * \param [in] unit              Unit number.
 * \param [in] vlan              VLAN ID.
 * \param [out]port_type         Force VLAN port type -
 *                               BCM_PORT_FORCE_VLAN_*_PORT.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_force_port_get(int unit, int vlan, int *port_type);

/*!
 * \brief Set VLAN actions per port.
 *
 * This function is used to set VLAN actions per port.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port number.
 * \param [in] action            VLAN actions.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_port_action_set(int unit,
                              bcm_port_t port,
                              bcm_vlan_action_set_t *action);
/*!
 * \brief Get VLAN actions per port.
 *
 * This function is used to get VLAN actions per port.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port number.
 * \param [out]action            VLAN actions.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_port_action_get(int unit,
                              bcm_port_t port,
                              bcm_vlan_action_set_t *action);

/*!
 * \brief Reset VLAN actions per port.
 *
 * This function is used to reset VLAN actions per port.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port number.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_port_action_reset(int unit, bcm_port_t port);

/*!
 * \brief Allocate and configure a VLAN.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vid             VLAN ID.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
extern int
xfs_ltsw_vlan_create(int unit, bcm_vlan_t vid);

/*!
 * \brief Deallocate VLAN from Switch.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vid             VLAN ID.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
extern int
xfs_ltsw_vlan_destroy(int unit, bcm_vlan_t vid);

/*!
 * \brief Attach counter entries to the given vlan.
 *
 * This function is used to attach flex counter to the given vlan.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [in] info              Counter information.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_stat_attach(int unit, bcm_vlan_t vid,
                          bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Detach counter entries to the given vlan.
 *
 * This function is used to detach flex counter to the given vlan.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [in] info              Counter information.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_stat_detach(int unit, bcm_vlan_t vid,
                          bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Get counter entries of the given vlan.
 *
 * This function is used to get flex counter to the given vlan.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [out] info             Counter information.
 * \param [out] num_info         Number of counter information.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_stat_get(int unit,
                       bcm_vlan_t vid,
                       bcmi_ltsw_flexctr_counter_info_t *info,
                       uint32_t *num_info);

/*!
 * \brief Attach flex-counter into a given ingress VLAN translation table.
 *
 * This function is used to attach the count ID and object value into a given
 * ingress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          VLAN translation key type.
 * \param [in] Port              GPORT (global port) identifier.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] config            Flex counter configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_ing_xlate_flexctr_attach(int unit,
                                       bcm_vlan_translate_key_t key_type,
                                       bcm_gport_t port,
                                       bcm_vlan_t outer_vlan,
                                       bcm_vlan_t inner_vlan,
                                       bcm_vlan_flexctr_config_t *config);

/*!
 * \brief Detach flex-counter from a given ingress vlan translation table.
 *
 * This function is used to detach the count ID and object value from a given
 * ingress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          VLAN translation key type.
 * \param [in] Port              GPORT (global port) identifier.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] config            Flex counter configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_ing_xlate_flexctr_detach(int unit,
                                       bcm_vlan_translate_key_t key_type,
                                       bcm_gport_t port,
                                       bcm_vlan_t outer_vlan,
                                       bcm_vlan_t inner_vlan,
                                       bcm_vlan_flexctr_config_t *config);

/*!
 * \brief Get flex-counter from a given ingress vlan translation table.
 *
 * This function is used to get the count ID and object value from a given
 * ingress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          VLAN translation key type.
 * \param [in] Port              GPORT (global port) identifier.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] array_size        Size of allocated entries in config array.
 * \param [out]config_array      Array of flex counter configurations.
 * \param [out]count             Number of elements in config array. If
 *                               array_size is 0, config array will be ignored
 *                               and count will be filled with an actual number.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_ing_xlate_flexctr_get(int unit,
                                    bcm_vlan_translate_key_t key_type,
                                    bcm_gport_t port,
                                    bcm_vlan_t outer_vlan,
                                    bcm_vlan_t inner_vlan,
                                    int array_size,
                                    bcm_vlan_flexctr_config_t *config_array,
                                    int *count);

/*!
 * \brief Attach flex-counter into a given egress VLAN translation table.
 *
 * This function is used to attach the count ID and object value into a given
 * egress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] config            Flex counter configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_egr_xlate_flexctr_attach(int unit,
                                       bcm_gport_t port,
                                       bcm_vlan_t outer_vlan,
                                       bcm_vlan_t inner_vlan,
                                       bcm_vlan_flexctr_config_t *config);

/*!
 * \brief Detach flex-counter from a given egress vlan translation table.
 *
 * This function is used to detach the count ID and object value from a given
 * egress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] config            Flex counter configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_egr_xlate_flexctr_detach(int unit,
                                       bcm_gport_t port,
                                       bcm_vlan_t outer_vlan,
                                       bcm_vlan_t inner_vlan,
                                       bcm_vlan_flexctr_config_t *config);

/*!
 * \brief Get flex-counter from a given egress vlan translation table.
 *
 * This function is used to get the count ID and object value from a given
 * egress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] port_class        Port class.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] array_size        Size of allocated entries in config array.
 * \param [out]config_array      Array of flex counter configurations.
 * \param [out]count             Number of elements in config array. If
 *                               array_size is 0, config array will be ignored
 *                               and count will be filled with an actual number.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_egr_xlate_flexctr_get(int unit,
                                    bcm_gport_t port,
                                    bcm_vlan_t outer_vlan,
                                    bcm_vlan_t inner_vlan,
                                    int array_size,
                                    bcm_vlan_flexctr_config_t *config_array,
                                    int *count);

/*!
 * \brief Add a Gport to the specified vlan. Adds WLAN/VLAN/NIV/Extender ports to
 *        broadcast, multicast and unknown unicast groups.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vlan            VLAN ID.
 * \param [in]   port            Gport id.
 * \param [in]   flags           BCM_VLAN_PORT_XXX.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
xfs_ltsw_vlan_gport_add(int unit, bcm_vlan_t vlan, bcm_gport_t port, int flags);

/*!
 * \brief Delete a Gport from the specified vlan with flag.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vlan            VLAN ID.
 * \param [in]   port            Gport id.
 * \param [out]  flags           BCM_VLAN_PORT_XXX.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
xfs_ltsw_vlan_gport_delete(int unit, bcm_vlan_t vlan, bcm_gport_t port, int flags);

/*!
 * \brief Delete all Gports from the specified vlan.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vlan            VLAN ID.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
xfs_ltsw_vlan_gport_delete_all(int unit, bcm_vlan_t vlan);

/*!
  * \brief Get a virtual or physical port from the specified VLAN.
  *
  * \param [in]   unit            Unit number.
  * \param [in]   vlan            VLAN ID.
  * \param [in]   port            Gport id.
  * \param [out]  flags           BCM_VLAN_PORT_XXX.
  *
  * \retval SHR_E_NONE      No errors.
  * \retval !SHR_E_NONE     Failure.
  */
extern int
xfs_ltsw_vlan_gport_get(int unit, bcm_vlan_t vlan,
                        bcm_gport_t port, int* flags);

/*!
  * \brief Get all virtual and physical ports from the specified VLAN.
  *
  * \param [in]   unit            Unit number.
  * \param [in]   vlan            VLAN ID.
  * \param [in]   array_max       Max array size.
  * \param [out]  gport_array     Gport id array.
  * \param [out]  flags_array     BCM_VLAN_PORT_XXX array.
  * \param [out]  array_size      Output array size.
  *
  * \retval SHR_E_NONE      No errors.
  * \retval !SHR_E_NONE     Failure.
  */
extern int
xfs_ltsw_vlan_gport_get_all(int unit, bcm_vlan_t vlan, int array_max,
                            bcm_gport_t *gport_array, int *flags_array,
                            int* array_size);

/*!
 * \brief Get miscellaneous VLAN-specific chip options.
 *
 * \param [in]    unit    Unit number.
 * \param [in]    type    A value from bcm_vlan_control_t enumeration list.
 * \param [out]   arg     State whose meaning is dependent on 'type'.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
extern int
xfs_ltsw_vlan_control_get(int unit, bcm_vlan_control_t type, int *arg);

/*!
 * \brief Set miscellaneous VLAN-specific chip options.
 *
 * \param [in]   unit     Unit number.
 * \param [in]   type     A value from bcm_vlan_control_t enumeration list.
 * \param [in]   arg      State whose meaning is dependent on 'type'.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
extern int
xfs_ltsw_vlan_control_set(int unit, bcm_vlan_control_t type, int arg);

/*!
 * \brief Update all VLAN information to L2 station.
 *
 * This function is used to update all VLAN information to L2 station, and it is
 * only called by VLAN or L2 STATION during init.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Callback function.
 * \param [in] user_data         User data of callback.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_l2_station_update_all(int unit,
                                    bcmi_ltsw_vlan_l2_station_update_cb cb,
                                    void *user_data);

/*!
 * \brief Update all VLAN information to L3 interface.
 *
 * This function is used to update all VLAN information to L3 interface, and it
 * is only called by L3 INTF module during init.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Callback function.
 * \param [in] user_data         User data of callback.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_l3_intf_update_all(int unit,
                                 bcmi_ltsw_vlan_l3_intf_update_cb cb,
                                 void *user_data);

/*!
 * \brief Add an entry to the VLAN Translation table, which assigns
 *        VLAN actions for packets matching within the VLAN range(s).
 *
 * \param [in]      unit                   Unit number.
 * \param [in]      port                   Port Id.
 * \param [in]      outer_vlan_low   Outer VLAN ID Low.
 * \param [in]      outer_vlan_high  Outer VLAN ID High.
 * \param [in]      inner_vlan_low   Inner VLAN ID Low.
 * \param [in]      inner_vlan_high  Inner VLAN ID High.
 * \param [in]      action                 Action for outer and inner tag.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
xfs_ltsw_vlan_translate_action_range_add(int unit,
                                         bcm_gport_t port,
                                         bcm_vlan_t outer_vlan_low,
                                         bcm_vlan_t outer_vlan_high,
                                         bcm_vlan_t inner_vlan_low,
                                         bcm_vlan_t inner_vlan_high,
                                         bcm_vlan_action_set_t *action);

/*!
 * \brief Delete an entry from the VLAN Translation table
 *        for the specified VLAN range(s).
 *
 * \param [in]      unit             Unit number.
 * \param [in]      port             Port Id.
 * \param [in]      outer_vlan_low   Outer VLAN ID Low.
 * \param [in]      outer_vlan_high  Outer VLAN ID High.
 * \param [in]      inner_vlan_low   Inner VLAN ID Low.
 * \param [in]      inner_vlan_high  Inner VLAN ID High.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
xfs_ltsw_vlan_translate_action_range_delete(int unit,
                                            bcm_gport_t port,
                                            bcm_vlan_t outer_vlan_low,
                                            bcm_vlan_t outer_vlan_high,
                                            bcm_vlan_t inner_vlan_low,
                                            bcm_vlan_t inner_vlan_high);

/*!
 * \brief Delete all VLAN range entries from the VLAN Translation table.
 *
 * \param [in]      unit             Unit number.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
xfs_ltsw_vlan_translate_action_range_delete_all(int unit);

/*!
 * \brief Traverses over VLAN Translation table and call provided callback
 *        with valid entries.
 *
 * \param [in]      unit             Unit number.
 * \param [in]      cb               Call back function.
 * \param [in]      user_data        Pointer to user specific data.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
xfs_ltsw_vlan_translate_action_range_traverse(int unit,
    bcm_vlan_translate_action_range_traverse_cb cb, void *user_data);

/*!
 * \brief Set per VLAN block configuration.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   vid       VLAN ID.
 * \param [in]   block     VLAN specific traffic blocking structure.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
extern int
xfs_ltsw_vlan_block_set(int unit, bcm_vlan_t vid, bcm_vlan_block_t *block);

/*!
 * \brief Get per VLAN block configuration.
 *
 * \param [in]   unit       Unit number.
 * \param [in]   vid        VLAN ID.
 * \param [out]  block      VLAN specific traffic blocking structure.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
extern int
xfs_ltsw_vlan_block_get(int unit, bcm_vlan_t vid, bcm_vlan_block_t *block);

/*!
 * \brief Delete VLAN profile entry per index.
 *
 * This function is used to delete VLAN profile entry per index.
 *
 * \param [in] unit              Unit number.
 * \param [in] profile_hdl       Profile handle.
 * \param [in] index             Profile entry index.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_profile_delete(int unit,
                             bcmi_ltsw_profile_hdl_t profile_hdl,
                             int index);

/*!
 * \brief Update VLAN range index when trunk member changes.
 *
 * This function is used to update VLAN range index when trunk member changes.
 *
 * \param [in]  unit              Unit Number.
 * \param [in]  gport             Original trunk member gport.
 * \param [in]  num_joining       Number of joining ports in trunk.
 * \param [in]  joining_arr       Joining ports in trunk.
 * \param [in]  num_leaving       Number of leaving ports in trunk.
 * \param [in]  leaving_arr       Leaving ports in trunk.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
xfs_ltsw_vlan_range_update(int unit, bcm_gport_t gport,
                           int num_joining, bcm_gport_t *joining_arr,
                           int num_leaving, bcm_gport_t *leaving_arr);

/*!
 * \brief Get port bitmap of reserved default VLAN flooding group.
 *
 * This function is used to get port pbmp of default VLAN flooding group.
 *
 * \param [in]   unit               Unit Number.
 * \param [in]   pbmp               Port bitmap.
 *
 * \retval SHR_E_NONE               No errors.
 * \retval !SHR_E_NONE              Failure.
 */
extern int
xfs_ltsw_vlan_def_flood_port_get(int unit, bcm_pbmp_t *pbmp);

/*
 * \brief Update vlan xlate pipe sw info.
 *
 * \param [in] unit              Unit number.
 * \param [in] pg                Port group.
 * \param [in] ingress           Ingress/egress.
 * \param [in] pipe              Pipe.
 * \param [in] op                Op code: Insert/Delete.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_vlan_xlate_pipe_update(int unit, bcm_port_t pg, int ingress,
                                int pipe, bcmi_ltsw_vlan_table_opcode_t op);

/*!
 * \brief Set STG into a specified VLAN entry.
 *
 * \param [in] unit      Unit Number.
 * \param [in] vid       VLAN ID.
 * \param [in] ingress   Ingress or egress.
 * \param [in] stg       STG index.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
extern int
xfs_ltsw_vlan_stg_set(int unit, bcm_vlan_t vid, int ingress, bcm_stg_t stg);

/*!
 * \brief Get STG from a specified VLAN entry.
 *
 * \param [in] unit      Unit Number.
 * \param [in] vid       VLAN ID.
 * \param [in] ingress   Ingress or egress.
 * \param [out]stg       STG index.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
extern int
xfs_ltsw_vlan_stg_get(int unit, bcm_vlan_t vid, int ingress, bcm_stg_t *stg);

/*!
 * \brief Notify VLAN membership check information for L2 tunnel case.
 *
 * \param [in] unit      Unit Number.
 * \param [in] vid       VLAN ID.
 * \param [in] add       Add or delete.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
extern int
xfs_ltsw_vlan_check_info_notify(int unit, bcm_vlan_t vid, int add);

/*!
 * \brief Display VLAN software structure information.
 *
 * \param [in] unit      Unit Number.
 * \param [in] ingress   Ingress/Egress.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
extern int
xfs_ltsw_vlan_info_dump(int unit);
#endif /* XFS_LTSW_VLAN_H */
