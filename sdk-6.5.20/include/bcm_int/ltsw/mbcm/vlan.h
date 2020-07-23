/*! \file vlan.h
 *
 * BCM VLAN module APIs and Structures used only internally.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_MBCM_VLAN_H
#define BCMI_LTSW_MBCM_VLAN_H

#include <bcm/vlan.h>
#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <bcm_int/ltsw/vlan.h>
#include <bcm_int/ltsw/vlan_int.h>
#include <bcm_int/ltsw/flexctr.h>
#include <bcm_int/ltsw/profile_mgmt.h>
#include <bcmltd/bcmltd_lt_types.h>

/*!
 * \brief Initialize VLAN module.
 *
 * \param [in] unit      Unit Number.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
typedef int (*vlan_init_f)(int unit);

/*!
 * \brief De-initialize VLAN module.
 *
 * \param [in] unit      Unit Number.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
typedef int (*vlan_detach_f)(int unit);

/*!
 * \brief Initialize VLAN outer TPID information.
 *
 * \param [in] unit      Unit Number.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
typedef int (*vlan_otpid_init_f)(int unit);

/*!
 * \brief De-initialize VLAN outer TPID information.
 *
 * \param [in] unit      Unit Number.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
typedef int (*vlan_otpid_detach_f)(int unit);

/*!
 * \brief De-initialize VLAN outer TPID information.
 *
 * \param [in] unit      Unit Number.
 * \param [in] index     Entry index.
 * \param [in] tpid      Outer TPID value.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
typedef int (*vlan_otpid_add_f)(int unit, int index, uint16_t tpid);

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
typedef int (*vlan_itpid_set_f)(int unit, uint16_t tpid);

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
typedef int (*vlan_itpid_get_f)(int unit, uint16_t *tpid);

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
typedef int (*vlan_port_add_f)(int unit,
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
typedef int (*vlan_port_get_f)(int unit,
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
typedef int (*vlan_port_remove_f)(int unit,
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
typedef int (*vlan_port_check_set_f)(int unit, bcm_port_t port,
                                     int disable, uint16_t flags);

/*!
 * \brief Allocate and configure a VLAN.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vid             VLAN ID.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
typedef int (*vlan_create_f)(int unit, bcm_vlan_t vid);

/*!
 * \brief Deallocate VLAN from Switch.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vid             VLAN ID.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
typedef int (*vlan_destroy_f)(int unit, bcm_vlan_t vid);

/*!
 * \brief Set vlan specific traffic forwarding parameters.
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
typedef int (*vlan_control_vlan_set_f)(int unit,
                                       bcm_vlan_t vlan,
                                       uint32_t valid_fields,
                                       bcm_vlan_control_vlan_t *control);

/*!
 * \brief Get vlan specific traffic forwarding parameters.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vlan            VLAN ID.
 * \param [in]   valid_fields    Valid fields for VLAN control structure,
 *                               BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 * \param [out]   control        VLAN control structure.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
typedef int (*vlan_control_vlan_get_f) (int unit,
                                        bcm_vlan_t vlan,
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
typedef int (*vlan_control_port_set_f)(int unit,
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
typedef int (*vlan_control_port_get_f)(int unit,
                                       bcm_port_t port,
                                       bcm_vlan_control_port_t type,
                                       int *arg);

/*!
 * \brief Configure an entry in ingress VLAN xlate table.
 *
 * This function is used to configure an entry in ingress VLAN xlate table.
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
typedef int (*vlan_ing_xlate_cfg_f) (int unit,
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
typedef int (*vlan_ing_xlate_delete_f) (int unit,
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
 * \param [in] cb                Callback function.
 * \param [in] user_data         User data to a call back.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
typedef int (*vlan_ing_xlate_trv_f) (int unit,
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
typedef int (*vlan_ing_xlate_action_add_f)(int unit,
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
typedef int (*vlan_ing_xlate_action_get_f)(int unit,
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
typedef int (*vlan_ing_xlate_action_delete_f)(int unit,
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
typedef int (*vlan_ing_xlate_action_trv_f)(
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
typedef int (*vlan_egr_xlate_action_add_f)(int unit,
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
typedef int (*vlan_egr_xlate_action_get_f)(int unit,
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
typedef int (*vlan_egr_xlate_action_delete_f)(int unit,
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
typedef int (*vlan_egr_xlate_action_trv_f)(
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
typedef int (*vlan_force_port_set_f)(int unit, int vlan, int port_type);

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
typedef int (*vlan_force_port_get_f)(int unit, int vlan, int *port_type);

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
typedef int (*vlan_port_action_set_f)(int unit,
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
typedef int (*vlan_port_action_get_f)(int unit,
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
typedef int (*vlan_port_action_reset_f)(int unit, bcm_port_t port);

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
typedef int (*vlan_stat_attach_f)(int unit, bcm_vlan_t vid,
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
typedef int (*vlan_stat_detach_f)(int unit, bcm_vlan_t vid,
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
typedef int (*vlan_stat_get_f)(int unit,
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
typedef int (*vlan_ing_xlate_flexctr_attach_f)(
    int unit,
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
typedef int (*vlan_ing_xlate_flexctr_detach_f)(
    int unit,
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
typedef int (*vlan_ing_xlate_flexctr_get_f)(
    int unit,
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
typedef int (*vlan_egr_xlate_flexctr_attach_f)(
    int unit,
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
typedef int (*vlan_egr_xlate_flexctr_detach_f)(
    int unit,
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
typedef int (*vlan_egr_xlate_flexctr_get_f)(
    int unit,
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
typedef int (*vlan_gport_add_f)(int unit, bcm_vlan_t vlan,
                                bcm_gport_t port, int flags);

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
typedef int (*vlan_gport_delete_f)(int unit, bcm_vlan_t vlan,
                                   bcm_gport_t port, int flags);

/*!
 * \brief Delete all Gports from the specified vlan.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vlan            VLAN ID.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
typedef int (*vlan_gport_delete_all_f)(int unit, bcm_vlan_t vlan);

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
typedef int (*vlan_gport_get_f)(int unit, bcm_vlan_t vlan,
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
typedef int (*vlan_gport_get_all_f)(int unit, bcm_vlan_t vlan,
                                    int array_max, bcm_gport_t *gport_array,
                                    int *flags_array, int* array_size);

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
typedef int (*vlan_control_get_f)(int unit, bcm_vlan_control_t type, int *arg);

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
typedef int (*vlan_control_set_f)(int unit, bcm_vlan_control_t type, int arg);

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
typedef int (*vlan_l2_station_update_all_f)(
    int unit,
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
typedef int (*vlan_l3_intf_update_all_f)(int unit,
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
typedef int (*vlan_translate_action_range_add_f)(int unit,
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
typedef int (*vlan_translate_action_range_delete_f)(int unit,
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
typedef int (*vlan_translate_action_range_delete_all_f)(int unit);

/*!
 * \brief Traverses over VLAN Translation table and call provided callback
 *        with valid entries.
 *
 * \param [in]     unit          Unit number.
 * \param [in]      cb           Call back function.
 * \param [in]      user_data    Pointer to user specific data.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
typedef int (*vlan_translate_action_range_traverse_f)(
              int                                         unit,
              bcm_vlan_translate_action_range_traverse_cb cb,
              void                                        *user_data);

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
typedef int (*vlan_block_set_f)(int unit, bcm_vlan_t vid, bcm_vlan_block_t *block);

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
typedef int (*vlan_block_get_f)(int unit, bcm_vlan_t vid, bcm_vlan_block_t *block);

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
typedef int (*vlan_profile_delete_f)(int unit,
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
typedef int (*vlan_range_update_f)(int unit, bcm_gport_t gport,
                         int num_joining, bcm_gport_t *joining_arr,
                         int num_leaving, bcm_gport_t *leaving_arr);

typedef int (*vlan_lt_db_get_f)(int unit, bcmint_vlan_lt_db_t *lt_db);


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
typedef int (*vlan_def_flood_port_get_f)(int unit, bcm_pbmp_t *pbmp);

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
typedef int (*vlan_stg_set_f)(int unit, bcm_vlan_t vid,
                              int ingress, bcm_stg_t stg);

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
typedef int (*vlan_stg_get_f)(int unit, bcm_vlan_t vid,
                            int ingress, bcm_stg_t *stg);

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
typedef int (*vlan_check_info_notify_f)(int unit, bcm_vlan_t vid, int add);

/*!
 * \brief Set VLAN mcast flood mode.
 *
 * This function is used to set vlan flood mode.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [in] mode              VLAN flood mode.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
typedef int (*vlan_mcast_flood_set_f)(int unit, bcm_vlan_t vid,
                                      bcm_vlan_mcast_flood_t mode);

/*!
 * \brief Get VLAN mcast flood mode.
 *
 * This function is used to get vlan flood mode.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [out] mode             VLAN flood mode.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
typedef int (*vlan_mcast_flood_get_f)(int unit, bcm_vlan_t vid,
                                      bcm_vlan_mcast_flood_t *mode);

/*!
 * \brief Add a VLAN cross connect entry.
 *
 * This function is used to add a VLAN cross connect entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] port_1            First port in the cross-connect.
 * \param [in] port_2            Second port in the cross-connect.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
typedef int (*vlan_cross_connect_add_f)(int unit,
                                        bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan,
                                        bcm_gport_t port_1, bcm_gport_t port_2);

/*!
 * \brief Delete a VLAN cross connect entry.
 *
 * This function is used to delete a VLAN cross connect entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
typedef int (*vlan_cross_connect_delete_f)(int unit,
                                        bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan);

/*!
 * \brief Delete all VLAN cross connect entries.
 *
 * This function is used to delete all VLAN cross connect entries.
 *
 * \param [in] unit              Unit number.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
typedef int (*vlan_cross_connect_delete_all_f)(int unit);

/*!
 * \brief Traverse VLAN cross connect entries.
 *
 * This function is used to traverse VLAN cross connect entries.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Callback function.
 * \param [in] user_data         User data to be passed to callback function.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
typedef int (*vlan_cross_connect_traverse_f)(int unit,
                                             bcm_vlan_cross_connect_traverse_cb cb,
                                             void *user_data);

/*!
 * \brief Set ingress vlan qos id.
 *
 * This function is used to set ingress vlan qos id.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [in] id                vlan qos id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
typedef int (*vlan_ing_qos_set_f)(int unit, bcm_vlan_t vid, int id);

/*!
 * \brief Get ingress vlan qos id.
 *
 * This function is used to get ingress vlan qos id.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [out] id               vlan qos id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
typedef int (*vlan_ing_qos_get_f)(int unit, bcm_vlan_t vid, int *id);

/*!
 * \brief Set egress vlan qos id.
 *
 * This function is used to set egress vlan qos id.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [in] id                vlan qos id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
typedef int (*vlan_egr_qos_set_f)(int unit, bcm_vlan_t vid, int id);

/*!
 * \brief Get egress vlan qos id.
 *
 * This function is used to get egress vlan qos id.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [out] id               vlan qos id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
typedef int (*vlan_egr_qos_get_f)(int unit, bcm_vlan_t vid, int *id);

/*!
 * \brief Set vlan switch control configurations.
 *
 * \param [in] unit              Unit number.
 * \param [in] type              The desired configuration parameter.
 * \param [in] value             The value to set the parameter.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
typedef int (*vlan_internal_control_set_f)(int unit,
                                           bcmi_ltsw_vlan_control_t type,
                                           int value);

/*!
 * \brief Get vlan switch control configurations.
 *
 * \param [in] unit              Unit number.
 * \param [in] type              The desired configuration parameter.
 * \param [in] value             The value pointer to get the parameter.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
typedef int (*vlan_internal_control_get_f)(int unit,
                                           bcmi_ltsw_vlan_control_t type,
                                           int *value);

/*!
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
typedef int (*vlan_xlate_pipe_update_f)(int unit, bcm_port_t pg, int ingress,
                                        int pipe, bcmi_ltsw_vlan_table_opcode_t op);

/*!
 * \brief Check if vlan xlate tables share same pt resource.
 *
 * This function is used to check if vlan xlate tables share same pt resource.
 *
 * \param [in]   unit                 Unit Number.
 * \param [in]   lt_id                VLAN LT ID.
 * \param [out]  share                share.
 *
 * \retval None
 */
typedef int (*vlan_xlate_table_share_f)(int unit,
                                        vlan_xlate_table_id_t lt_id,
                                        int *share);

/*!
 * \brief Display VLAN software structure information.
 *
 * \param [in] unit      Unit Number.
 * \param [in] ingress   Ingress/Egress.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
typedef int (*vlan_info_dump_f)(int unit);

/*!
 * \brief Get default vlan membership ports profile id.
 *
 * This function is used to get default vlan membership ports profile id.
 *
 * \param [in] unit              Unit number.
 * \param [in] type             Default vlan membership ports profile type.
 * \param [out] profile_id    Default vlan membership ports profile id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
typedef int (*vlan_mshp_default_profile_get_f)(int unit,
                                               bcmi_ltsw_profile_hdl_t type,
                                               int *profile_id);

/*!
 * \brief Traverse vlan table to get needed info.
 *
 * This function is used to traverse vlan table without lock.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Callback function.
 * \param [in] user_data         User data of callback.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
typedef int (*vlan_traverse_f)(int unit,
                               bcmi_ltsw_vlan_traverse_cb cb,
                               void *user_data);

/*!
 * \brief VLAN function vector driver.
 */
typedef struct mbcm_ltsw_vlan_drv_s {
    /*! Initialize VLAN Module. */
    vlan_init_f vlan_init;

    /*! De-initialize VLAN Module. */
    vlan_detach_f vlan_detach;

    /*! Initialize VLAN outer TPID information. */
    vlan_otpid_init_f vlan_otpid_init;

    /*! De-initialize VLAN outer TPID information. */
    vlan_otpid_detach_f vlan_otpid_detach;

    /*! Add VLAN outer TPID. */
    vlan_otpid_add_f vlan_otpid_add;

    /*! Initialize VLAN payload outer TPID information. */
    vlan_otpid_init_f vlan_payload_otpid_init;

    /*! De-initialize VLAN payload outer TPID information. */
    vlan_otpid_detach_f vlan_payload_otpid_detach;

    /*! Add VLAN payload outer TPID. */
    vlan_otpid_add_f vlan_payload_otpid_add;

    /*! Set global ingress inner TPID. */
    vlan_itpid_set_f vlan_itpid_set;

    /*! Get global ingress inner TPID. */
    vlan_itpid_get_f vlan_itpid_get;

    /*! Add ports to the specified vlan. */
    vlan_port_add_f vlan_port_add;

    /*! Retrieves a list of the member ports of an existing VLAN. */
    vlan_port_get_f vlan_port_get;

    /*! Remove ports from a specified vlan. */
    vlan_port_remove_f vlan_port_remove;

    /*! Set the state of VLAN membership check on a given port. */
    vlan_port_check_set_f vlan_port_check_set;

    /*! Allocate and configure a VLAN. */
    vlan_create_f vlan_create;

    /*! Deallocate VLAN from Switch. */
    vlan_destroy_f vlan_destroy;

    /*! Set vlan specific traffic forwarding parameters. */
    vlan_control_vlan_set_f vlan_control_vlan_set;

    /*! Get vlan specific traffic forwarding parameters. */
    vlan_control_vlan_get_f vlan_control_vlan_get;

    /*! Set port specific VLAN options. */
    vlan_control_port_set_f vlan_control_port_set;

    /*! Get port specific VLAN options. */
    vlan_control_port_get_f vlan_control_port_get;

    /*! Add an ingress VLAN translation entry. */
    vlan_ing_xlate_cfg_f vlan_ing_xlate_add;

    /*! Update an ingress VLAN translation entry. */
    vlan_ing_xlate_cfg_f vlan_ing_xlate_update;

    /*! Get an ingress VLAN translation entry. */
    vlan_ing_xlate_cfg_f vlan_ing_xlate_get;

    /*! Delete an ingress VLAN translation entry. */
    vlan_ing_xlate_delete_f vlan_ing_xlate_delete;

    /*! Traverse ingress VLAN translation entries. */
    vlan_ing_xlate_trv_f vlan_ing_xlate_traverse;

    /*! Add an ingress VLAN action translation entry. */
    vlan_ing_xlate_action_add_f vlan_ing_xlate_action_add;

    /*! Get an ingress VLAN action translation entry. */
    vlan_ing_xlate_action_get_f vlan_ing_xlate_action_get;

    /*! Delete an ingress VLAN action translation entry. */
    vlan_ing_xlate_action_delete_f vlan_ing_xlate_action_delete;

    /*! Traverse ingress VLAN action translation entries. */
    vlan_ing_xlate_action_trv_f vlan_ing_xlate_action_traverse;

    /*! Add an egress VLAN action translation entry. */
    vlan_egr_xlate_action_add_f vlan_egr_xlate_action_add;

    /*! Get an egress VLAN action translation entry. */
    vlan_egr_xlate_action_get_f vlan_egr_xlate_action_get;

    /*! Delete an egress VLAN action translation entry. */
    vlan_egr_xlate_action_delete_f vlan_egr_xlate_action_delete;

    /*! Traverse egress VLAN action translation entries. */
    vlan_egr_xlate_action_trv_f vlan_egr_xlate_action_traverse;

    /*! Set force VLNA port type. */
    vlan_force_port_set_f vlan_force_port_set;

    /*! Get force VLNA port type. */
    vlan_force_port_get_f vlan_force_port_get;

    /*! Set VLAN actions per port. */
    vlan_port_action_set_f vlan_port_action_set;

    /*! Get VLAN actions per port. */
    vlan_port_action_get_f vlan_port_action_get;

    /*! Reset VLAN actions per port. */
    vlan_port_action_reset_f vlan_port_action_reset;

    /*! Set egr VLAN actions per port. */
    vlan_port_action_set_f egr_vlan_port_action_set;

    /*! Get egr VLAN actions per port. */
    vlan_port_action_get_f egr_vlan_port_action_get;

    /*! Reset egr VLAN actions per port. */
    vlan_port_action_reset_f egr_vlan_port_action_reset;

    /*! Attach flex counter to the given vlan. */
    vlan_stat_attach_f vlan_stat_attach;

    /*! Detach flex counter to the given vlan. */
    vlan_stat_detach_f vlan_stat_detach;

    /*! Attach flex-counter into a given ingress VLAN translation table. */
    vlan_ing_xlate_flexctr_attach_f vlan_ing_xlate_flexctr_attach;

    /*! Detach flex-counter from a given ingress vlan translation table. */
    vlan_ing_xlate_flexctr_detach_f vlan_ing_xlate_flexctr_detach;

    /*! Get flex-counter from a given ingress vlan translation table. */
    vlan_ing_xlate_flexctr_get_f vlan_ing_xlate_flexctr_get;

    /*! Attach flex-counter into a given egress VLAN translation table. */
    vlan_egr_xlate_flexctr_attach_f vlan_egr_xlate_flexctr_attach;

    /*! Detach flex-counter from a given egress vlan translation table. */
    vlan_egr_xlate_flexctr_detach_f vlan_egr_xlate_flexctr_detach;

    /*! Get flex-counter from a given egress vlan translation table. */
    vlan_egr_xlate_flexctr_get_f vlan_egr_xlate_flexctr_get;

    /*! Get counter entries of the given vlan. */
    vlan_stat_get_f vlan_stat_get;

    /*! Add a Gport to the specified vlan. */
    vlan_gport_add_f vlan_gport_add;

    /*! Delete a Gport from the specified vlan. */
    vlan_gport_delete_f vlan_gport_delete;

    /*! Delete all Gports from the specified vlan. */
    vlan_gport_delete_all_f vlan_gport_delete_all;

    /*! Get a virtual or physical port from the specified VLAN. */
    vlan_gport_get_f vlan_gport_get;

    /*! Get all virtual and physical ports from the specified VLAN. */
    vlan_gport_get_all_f vlan_gport_get_all;

    /*! Get miscellaneous VLAN-specific chip options. */
    vlan_control_get_f vlan_control_get;

    /*! Set miscellaneous VLAN-specific chip options. */
    vlan_control_set_f vlan_control_set;

    /*! Update all VLAN information to L2 station. */
    vlan_l2_station_update_all_f vlan_l2_station_update_all;

    /*! Update all VLAN information to L3 interface. */
    vlan_l3_intf_update_all_f vlan_l3_intf_update_all;

    /*! Add VLAN xlate entry which matches VLAN ranges. */
    vlan_translate_action_range_add_f vlan_translate_action_range_add;

    /*! Delete VLAN xlate entry which matches VLAN ranges. */
    vlan_translate_action_range_delete_f vlan_translate_action_range_delete;

    /*! Delete all VLAN range entries from the VLAN Translation table.*/
    vlan_translate_action_range_delete_all_f vlan_translate_action_range_delete_all;

    /*! Traverses over VLAN Translation table and callback.*/
    vlan_translate_action_range_traverse_f vlan_translate_action_range_traverse;

    /*! Set per VLAN block configuration. */
    vlan_block_set_f vlan_block_set;

    /*! Get per VLAN block configuration. */
    vlan_block_get_f vlan_block_get;

    /*! Delete VLAN profile entry per index. */
    vlan_profile_delete_f vlan_profile_delete;

    /*! Update VLAN range index when trunk member changes. */
    vlan_range_update_f vlan_range_update;

    /*! Get VLAN perl npl version lt info. */
    vlan_lt_db_get_f vlan_lt_db_get;

    /*! Get port bitmap of reserved default VLAN flooding group. */
    vlan_def_flood_port_get_f vlan_def_flood_port_get;

    /*! Set STG into a specified VLAN entry. */
    vlan_stg_set_f vlan_stg_set;

    /*! Get STG from a specified VLAN entry. */
    vlan_stg_get_f vlan_stg_get;

    /* Notify VLAN membership check information for L2 tunnel case. */
    vlan_check_info_notify_f vlan_check_info_notify;

    /*! VLAN flood mode set. */
    vlan_mcast_flood_set_f vlan_mcast_flood_set;

    /*! VLAN flood mode get. */
    vlan_mcast_flood_get_f vlan_mcast_flood_get;

    /*! Add a vlan cross connect entry. */
    vlan_cross_connect_add_f vlan_cross_connect_add;

    /*! Delete a vlan cross connect entry. */
    vlan_cross_connect_delete_f vlan_cross_connect_delete;

    /*! Delete all vlan cross connect entries. */
    vlan_cross_connect_delete_all_f vlan_cross_connect_delete_all;

    /*! Traverse all vlan cross connect entries. */
    vlan_cross_connect_traverse_f vlan_cross_connect_traverse;

    /*! Set ingress vlan qos id. */
    vlan_ing_qos_set_f vlan_ing_qos_set;

    /*! Get ingress vlan qos id. */
    vlan_ing_qos_get_f vlan_ing_qos_get;

    /*! Set egress vlan qos id. */
    vlan_egr_qos_set_f vlan_egr_qos_set;

    /*! Get egress vlan qos id. */
    vlan_egr_qos_get_f vlan_egr_qos_get;

    /*! Set vlan switch control configurations. */
    vlan_internal_control_set_f vlan_internal_control_set;

    /*! Get vlan switch control configurations. */
    vlan_internal_control_get_f vlan_internal_control_get;

    /*! Update vlan xlate pipe sw info. */
    vlan_xlate_pipe_update_f vlan_xlate_pipe_update;

    /*! Check if vlan xlate tables share same pt resource. */
    vlan_xlate_table_share_f vlan_xlate_table_share;

    /*! Display VLAN software structure information. */
    vlan_info_dump_f vlan_info_dump;

    /*! Get default vlan membership ports profile id. */
    vlan_mshp_default_profile_get_f vlan_mshp_default_profile_get;

    /*! Traverse vlan table. */
    vlan_traverse_f vlan_traverse;
} mbcm_ltsw_vlan_drv_t;

/*!
 * \brief Set the VLAN driver of the device.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Vlan driver to set.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_vlan_drv_set(int unit, mbcm_ltsw_vlan_drv_t *drv);

/*!
 * \brief Initialize VLAN module.
 *
 * \param [in] unit      Unit Number.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
extern int
mbcm_ltsw_vlan_init(int unit);

/*!
 * \brief De-initialize VLAN module.
 *
 * \param [in] unit      Unit Number.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
extern int
mbcm_ltsw_vlan_detach(int unit);

/*!
 * \brief Initialize VLAN outer TPID information.
 *
 * \param [in] unit      Unit Number.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
extern int
mbcm_ltsw_vlan_otpid_init(int unit);

/*!
 * \brief De-initialize VLAN outer TPID information.
 *
 * \param [in] unit      Unit Number.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
extern int
mbcm_ltsw_vlan_otpid_detach(int unit);

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
mbcm_ltsw_vlan_otpid_add(int unit, int index, uint16_t tpid);

/*!
 * \brief Initialize the payload outer TPID information.
 *
 * This function is used to initialize the payload outer TPID information.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
mbcm_ltsw_vlan_payload_otpid_init(int unit);

/*!
 * \brief De-initialize payload outer TPID information.
 *
 * This function is used to de-initialize payload outer TPID information.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
mbcm_ltsw_vlan_payload_otpid_detach(int unit);

/*!
 * \brief Add a new payload outer TPID entry.
 *
 * This function is used to add a new payload outer TPID entry. If a same TPID
 * already exists, simplely increase the reference count of the cached entry.
 * Otherwise, add the entry to the cached table and write the new entry into LT.
 * Only four distinct TPID values are currently supported.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  tpid                 TPID to be added.
 * \param [out] index                Index where the the new TPID is added.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
mbcm_ltsw_vlan_payload_otpid_add(int unit, int index, uint16_t tpid);

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
mbcm_ltsw_vlan_itpid_set(int unit, uint16_t tpid);

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
mbcm_ltsw_vlan_itpid_get(int unit, uint16_t *tpid);

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
mbcm_ltsw_vlan_port_add(int unit,
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
mbcm_ltsw_vlan_port_get(int unit,
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
mbcm_ltsw_vlan_port_remove(int unit,
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
mbcm_ltsw_vlan_port_check_set(int unit, bcm_port_t port,
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
mbcm_ltsw_vlan_control_vlan_set(int unit, bcm_vlan_t vid,
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
mbcm_ltsw_vlan_control_vlan_get(int unit, bcm_vlan_t vid,
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
mbcm_ltsw_vlan_control_port_set(int unit,
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
mbcm_ltsw_vlan_control_port_get(int unit,
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
mbcm_ltsw_vlan_ing_xlate_add(int unit,
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
mbcm_ltsw_vlan_ing_xlate_update(int unit,
                                bcm_vlan_translate_key_t key_type,
                                bcm_gport_t port,
                                bcm_vlan_t outer_vlan,
                                bcm_vlan_t inner_vlan,
                                bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg);

/*!
 * \brief Get an entry from ingress VLAN translation table.
 *
 * This function is used to get an entry from ingress VLAN translation table.
 * If fld_bmp is not specified (zero), all fields will be retrieved.
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
mbcm_ltsw_vlan_ing_xlate_get(int unit,
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
mbcm_ltsw_vlan_ing_xlate_delete(int unit,
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
 * \param [in] cb                Callback function.
 * \param [in] user_data         User data to a call back.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
mbcm_ltsw_vlan_ing_xlate_traverse(int unit,
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
mbcm_ltsw_vlan_ing_xlate_action_add(int unit,
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
mbcm_ltsw_vlan_ing_xlate_action_get(int unit,
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
mbcm_ltsw_vlan_ing_xlate_action_delete(int unit,
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
mbcm_ltsw_vlan_ing_xlate_action_traverse(
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
mbcm_ltsw_vlan_egr_xlate_action_add(int unit,
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
mbcm_ltsw_vlan_egr_xlate_action_get(int unit,
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
mbcm_ltsw_vlan_egr_xlate_action_delete(int unit,
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
mbcm_ltsw_vlan_egr_xlate_action_traverse(
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
mbcm_ltsw_vlan_force_port_set(int unit, int vlan, int port_type);

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
mbcm_ltsw_vlan_force_port_get(int unit, int vlan, int *port_type);

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
mbcm_ltsw_vlan_port_action_set(int unit,
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
mbcm_ltsw_vlan_port_action_get(int unit,
                               bcm_port_t port,
                               bcm_vlan_action_set_t *action);

/*!
 * \brief Reset egr VLAN actions per port.
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
mbcm_ltsw_egr_vlan_port_action_reset(int unit, bcm_port_t port);

/*!
 * \brief Set egr VLAN actions per port.
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
mbcm_ltsw_egr_vlan_port_action_set(int unit,
                                   bcm_port_t port,
                                   bcm_vlan_action_set_t *action);
/*!
 * \brief Get egr VLAN actions per port.
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
mbcm_ltsw_egr_vlan_port_action_get(int unit,
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
mbcm_ltsw_vlan_port_action_reset(int unit, bcm_port_t port);

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
mbcm_ltsw_vlan_create(int unit, bcm_vlan_t vid);

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
mbcm_ltsw_vlan_destroy(int unit, bcm_vlan_t vid);

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
mbcm_ltsw_vlan_stat_attach(int unit, bcm_vlan_t vid,
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
mbcm_ltsw_vlan_stat_detach(int unit, bcm_vlan_t vid,
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
mbcm_ltsw_vlan_stat_get(int unit,
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
mbcm_ltsw_vlan_ing_xlate_flexctr_attach(int unit,
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
mbcm_ltsw_vlan_ing_xlate_flexctr_detach(int unit,
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
mbcm_ltsw_vlan_ing_xlate_flexctr_get(int unit,
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
mbcm_ltsw_vlan_egr_xlate_flexctr_attach(int unit,
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
mbcm_ltsw_vlan_egr_xlate_flexctr_detach(int unit,
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
mbcm_ltsw_vlan_egr_xlate_flexctr_get(int unit,
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
mbcm_ltsw_vlan_gport_add(int unit, bcm_vlan_t vlan, bcm_gport_t port, int flags);

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
mbcm_ltsw_vlan_gport_delete(int unit, bcm_vlan_t vlan,
                            bcm_gport_t port, int flags);

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
mbcm_ltsw_vlan_gport_delete_all(int unit, bcm_vlan_t vlan);

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
mbcm_ltsw_vlan_gport_get(int unit, bcm_vlan_t vlan,
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
mbcm_ltsw_vlan_gport_get_all(int unit, bcm_vlan_t vlan, int array_max,
                             bcm_gport_t *gport_array, int *flags_array,
                             int* array_size);

/*!
 * \brief Get miscellaneous VLAN-specific chip options.
 *
 * \param [in]    unit     Unit number.
 * \param [in]    type     A value from bcm_vlan_control_t enumeration list.
 * \param [out]   arg      State whose meaning is dependent on 'type'.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
extern int
mbcm_ltsw_vlan_control_get(int unit, bcm_vlan_control_t type, int *arg);

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
mbcm_ltsw_vlan_control_set(int unit, bcm_vlan_control_t type, int arg);

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
mbcm_ltsw_vlan_l2_station_update_all(int unit,
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
mbcm_ltsw_vlan_l3_intf_update_all(int unit,
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
mbcm_ltsw_vlan_translate_action_range_add(int unit,
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
mbcm_ltsw_vlan_translate_action_range_delete(int unit,
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
mbcm_ltsw_vlan_translate_action_range_delete_all(int unit);

/*!
 * \brief Traverses over VLAN Translation table and call provided callback
 *        with valid entries.
 *
 * \param  [in]     unit             Unit number.
 * \param [in]      cb               Call back function.
 * \param [in]      user_data        Pointer to user specific data.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
mbcm_ltsw_vlan_translate_action_range_traverse(int unit,
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
mbcm_ltsw_vlan_block_set(int unit, bcm_vlan_t vid, bcm_vlan_block_t *block);

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
mbcm_ltsw_vlan_block_get(int unit, bcm_vlan_t vid, bcm_vlan_block_t *block);

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
mbcm_ltsw_vlan_profile_delete(int unit,
                              bcmi_ltsw_profile_hdl_t profile_hdl,
                              int index);

/*!
 * \brief Update VLAN range index when trunk member changes.
 *
 * This function is used to update VLAN range index when trunk member changes.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  gport                Original trunk member gport.
 * \param [in]  num_joining          Number of joining ports in trunk.
 * \param [in]  joining_arr          Joining ports in trunk.
 * \param [in]  num_leaving          Number of leaving ports in trunk.
 * \param [in]  leaving_arr          Leaving ports in trunk.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
mbcm_ltsw_vlan_range_update(int unit, bcm_gport_t gport,
                         int num_joining, bcm_gport_t *joining_arr,
                         int num_leaving, bcm_gport_t *leaving_arr);

/*!
 * \brief Get per npl  LT info.
 *
 * This function is used to get vlan lt info based on NPL version.
 *
 * \param [in]    unit                 Unit Number.
 * \param [out]   lt_info              LT info.
 *
 * \retval None
 */
extern int
mbcm_ltsw_vlan_lt_db_get(int unit,
                         bcmint_vlan_lt_db_t *lt_info);

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
mbcm_ltsw_vlan_def_flood_port_get(int unit, bcm_pbmp_t *pbmp);

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
mbcm_ltsw_vlan_stg_set(int unit, bcm_vlan_t vid, int ingress, bcm_stg_t stg);

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
mbcm_ltsw_vlan_stg_get(int unit, bcm_vlan_t vid, int ingress, bcm_stg_t *stg);

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
mbcm_ltsw_vlan_check_info_notify(int unit, bcm_vlan_t vid, int add);

/*!
 * \brief Set VLAN mcast flood mode.
 *
 * This function is used to set vlan flood mode.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [in] mode              VLAN flood mode.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
mbcm_ltsw_vlan_mcast_flood_set(int unit,
                               bcm_vlan_t vid,
                               bcm_vlan_mcast_flood_t mode);

/*!
 * \brief Get VLAN mcast flood mode.
 *
 * This function is used to get vlan flood mode.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [out] mode             VLAN flood mode.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
mbcm_ltsw_vlan_mcast_flood_get(int unit,
                               bcm_vlan_t vid,
                               bcm_vlan_mcast_flood_t *mode);

/*!
 * \brief Add a VLAN cross connect entry.
 *
 * This function is used to add a VLAN cross connect entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] port_1            First port in the cross-connect.
 * \param [in] port_2            Second port in the cross-connect.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
mbcm_ltsw_vlan_cross_connect_add(int unit,
                                 bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan,
                                 bcm_gport_t port_1, bcm_gport_t port_2);

/*!
 * \brief Delete a VLAN cross connect entry.
 *
 * This function is used to delete a VLAN cross connect entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
mbcm_ltsw_vlan_cross_connect_delete(int unit,
                                    bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan);

/*!
 * \brief Delete all VLAN cross connect entries.
 *
 * This function is used to delete all VLAN cross connect entries.
 *
 * \param [in] unit              Unit number.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
mbcm_ltsw_vlan_cross_connect_delete_all(int unit);

/*!
 * \brief Traverse VLAN cross connect entries.
 *
 * This function is used to traverse VLAN cross connect entries.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Callback function.
 * \param [in] user_data         User data to be passed to callback function.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
mbcm_ltsw_vlan_cross_connect_traverse(int unit,
                                      bcm_vlan_cross_connect_traverse_cb cb,
                                      void *user_data);

/*!
 * \brief Set ingress vlan qos id.
 *
 * This function is used to set ingress vlan qos id.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [in] id                vlan qos id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
mbcm_ltsw_vlan_ing_qos_set(int unit, bcm_vlan_t vid, int id);

/*!
 * \brief Get ingress vlan qos id.
 *
 * This function is used to get ingress vlan qos id.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [out] id               vlan qos id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
mbcm_ltsw_vlan_ing_qos_get(int unit, bcm_vlan_t vid, int *id);

/*!
 * \brief Set egress vlan qos id.
 *
 * This function is used to set egress vlan qos id.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [in] id                vlan qos id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
mbcm_ltsw_vlan_egr_qos_set(int unit, bcm_vlan_t vid, int id);

/*!
 * \brief Get egress vlan qos id.
 *
 * This function is used to get egress vlan qos id.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [out] id               vlan qos id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
mbcm_ltsw_vlan_egr_qos_get(int unit, bcm_vlan_t vid, int *id);

/*!
 * \brief Set vlan switch control configurations.
 *
 * \param [in] unit              Unit number.
 * \param [in] type              The desired configuration parameter.
 * \param [in] value             The value to set the parameter.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
mbcm_ltsw_vlan_internal_control_set(int unit,
                                    bcmi_ltsw_vlan_control_t type,
                                    int value);

/*!
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
mbcm_ltsw_vlan_xlate_pipe_update(int unit, bcm_port_t pg, int ingress,
                                 int pipe, bcmi_ltsw_vlan_table_opcode_t op);
/*!
 * \brief Check if vlan xlate tables share same pt resource.
 *
 * This function is used to check if vlan xlate tables share same pt resource.
 *
 * \param [in]   unit                 Unit Number.
 * \param [in]   lt_id                VLAN LT ID.
 * \param [out]  share                share.
 *
 * \retval None
 */
extern int
mbcm_ltsw_vlan_xlate_table_share(int unit, vlan_xlate_table_id_t lt_id, int *share);

/*!
 * \brief Get vlan switch control configurations.
 *
 * \param [in] unit              Unit number.
 * \param [in] type              The desired configuration parameter.
 * \param [in] value             The value pointer to get the parameter.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
mbcm_ltsw_vlan_internal_control_get(int unit,
                                    bcmi_ltsw_vlan_control_t type,
                                    int *value);

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
mbcm_ltsw_vlan_info_dump(int unit);

/*!
 * \brief Get default vlan membership ports profile id.
 *
 * This function is used to get default vlan membership ports profile id.
 *
 * \param [in] unit              Unit number.
 * \param [in] type             Default vlan membership ports profile type.
 * \param [out] profile_id    Default vlan membership ports profile id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
mbcm_ltsw_vlan_mshp_default_profile_get(int unit,
                                        bcmi_ltsw_profile_hdl_t type,
                                        int *profile_id);
/*!
 * \brief Traverse vlan table to get needed info.
 *
 * This function is used to traverse vlan table without lock.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Callback function.
 * \param [in] user_data         User data of callback.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
mbcm_ltsw_vlan_traverse(int unit,
                        bcmi_ltsw_vlan_traverse_cb cb, void *user_data);
#endif /* BCMI_LTSW_MBCM_VLAN_H */
