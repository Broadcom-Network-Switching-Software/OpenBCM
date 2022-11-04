/**
 * \file bcm_int/dnx/switch/switch_svtag.h
 *
 * Load balancing definitions, for DNX, mainly related to BCM APIs
 *
 * Purpose:
 *   Definitions for logical and physical LB-related operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef SWITCH_SVTAG_H_INCLUDED
/* { */
#define SWITCH_SVTAG_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif
/*
 * Includes
 * {
 */
#include <bcm/switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_switch.h>
#include <bcm_int/dnx/port/port_esem.h>
#include <bcm_int/dnx/algo/port_pp/algo_port_pp.h>
/*
 * }
 */

/*
 * Defines
 * {
 */

/*
 * }
 */
/*
 * Macros
 * {
 */

/*
 * }
 */
/*
 * Typedefs
 * {
 */
/*
 * This union holds one of the fields according to user use/device configuration
 */
typedef union
{
    bcm_switch_svtag_egress_traverse_cb egress_traverse_cb; /** cb use for traverse egress API*/
    bcm_switch_svtag_encapsulation_traverse_cb encapsulation_traverse_cb; /** cb use for traverse encapsulation API*/
} switch_svtag_traverse_cb_t;
/*
 * }
 */
/*
 * Prototypes
 * {
 */

/**
 * \brief -
 *  Init the SVTAG.
 *
 * \param [in] unit - the unit ID.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  None
 */
shr_error_e dnx_switch_svtag_init(
    int unit);

/**
 * \brief
 * This function returns whether an SVTAG ESEM commands is present inside the ESEM commands group.
 * \param [in] unit  - The unit number.
 * \param [in] esem_cmd_profile - The profile ID for the available ESEM commands.
 * \param [out] app_db_id - the app_db_id is the HW lookup key type, e.g TM_PORT, OUTLIF, in case the esem command has SVTAG configuration,
 *                          else, zero
 * \param [out] found - indicate if an SVTAG ESEM command is present between the ESEM commands group.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *  None
 *
 * \see
 *  None
 */
shr_error_e dnx_switch_svtag_is_present_in_esem_cmd(
    int unit,
    uint32 esem_cmd_profile,
    int *app_db_id,
    uint8 *found);

/**
 * \brief- check if there is an SVTAG entry for the given port
 *
 * \param [in] unit  - The unit number.
 * \param [in] port  - port
 * \param [out] svtag_port_exists - Boolean, true in case the port has an SVTAG entry, false otherwise
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  None
 */
shr_error_e dnx_switch_svtag_egress_entry_port_exist_check(
    int unit,
    bcm_gport_t port,
    uint8 *svtag_port_exists);

/**
 * \brief- Sets SVTAG over ESEM command for the given gport, include hw setting
 *
 * \param [in] unit  - The unit number.
 * \param [in] gport  - gport
 * \param [in] enable - Boolean, true in case of adding SVTAG configuration, false in case of removing SVTAG configuration
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  None
 */
shr_error_e dnx_switch_svtag_esem_cmd_set(
    int unit,
    bcm_gport_t gport,
    uint8 enable);

/**
 * \brief- Sets SVTAG over ESEM command for the given trunk
 *
 * \param [in] unit  - The unit number.
 * \param [in] port  - trunk gport
 * \param [in] enable - Boolean, true in case of adding ESEM command SVTAG configuration,
 *                      false in case of removing ESEM command SVTAG configuration
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  None
 */
shr_error_e dnx_switch_svtag_trunk_egress_lookup_enable_set(
    int unit,
    bcm_gport_t port,
    uint8 enable);

/**
 * \brief- Get indication for SVTAG over ESEM command for the given trunk
 *
 * \param [in] unit  - The unit number.
 * \param [in] port  - trunk gport
 * \param [out] enable - Boolean, true in case the ESEM command has SVTAG configuration,
 *                      false otherwise
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  None
 */
shr_error_e dnx_switch_svtag_trunk_egress_lookup_enable_get(
    int unit,
    bcm_gport_t port,
    uint8 *enable);

/**
 * \brief- Exchange ESEM command with SVTAG settings according to the parameters
 *
 * \param [in] unit  - The unit number.
 * \param [in] gport  - gport
 * \param [in] old_esem_cmd - The ESEM command used now by the gport
 * \param [in] enable - Boolean, enable/disable SVTAG configuration
 * \param [in] flags- DNX_PORT_ESEM_* flags
 * \param [out] new_esem_cmd- pointer to the ESEM command allocated
 * \param [out] exchange_and_set_hw- boolean, specify if allocation exchange and HW setting is necessary
 * \param [out] esem_cmd_data - the data of the command to be used for HW setting
 * \param [out] is_first - Indicate that if new_esem_cmd is used for the first time.
 * \param [out] is_last - Indicate that if old_esem_cmd is not in use now.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  None
 */
shr_error_e dnx_switch_svtag_esem_cmd_exchange(
    int unit,
    bcm_gport_t gport,
    int old_esem_cmd,
    uint8 enable,
    uint32 flags,
    int *new_esem_cmd,
    uint8 *exchange_and_set_hw,
    dnx_esem_cmd_data_t * esem_cmd_data,
    uint8 *is_first,
    uint8 *is_last);

/**
 * \brief - enable/disable the MACSEC state of the PP port
 *
 * \param [in] unit - Relevant unit
 * \param [in] gport - gport logical port
 * \param [in] enable - enable/disable indication
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_switch_svtag_macsec_set(
    int unit,
    bcm_port_t gport,
    int enable);

/**
 * \brief
 *  This function sets the SVTAG settings for specific port according to the enable flag
 * \param [in] unit - the relevant unit.
 * \param [in] gport - gport.
 * \param [in] enable - Bool flag for enable or disable svtag.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *  * None
 * \see
 *  * None
 */
shr_error_e dnx_switch_svtag_port_set(
    int unit,
    bcm_gport_t gport,
    uint8 enable);

/**
 * \brief
 *  This function retrieve the SVTAG status(enable/disable) for specific port
 * \param [in] unit - the relevant unit.
 * \param [in] gport - gport.
 * \param [out] enable - Boolean flag indicating enabled or disabled SVATG.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *  * None
 * \see
 *  * None
 */
shr_error_e dnx_switch_svtag_port_get(
    int unit,
    bcm_gport_t gport,
    uint8 *enable);

/**
 * \brief
 *  Set svtag PFC fc mapping.
 * \param [in] unit - the relevant unit.
 * \param [in] value - 1: enable, 0: disable.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *  * None
 * \see
 *  * None
 */
shr_error_e dnx_switch_svtag_pfc_flow_control_mapping_set(
    int unit,
    int value);

/**
 * \brief
 *  Get svtag PFC fc mapping.
 * \param [in] unit - the relevant unit.
 * \param [out] value - 1: enable, 0: disable.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *  * None
 * \see
 *  * None
 */
shr_error_e dnx_switch_svtag_pfc_flow_control_mapping_get(
    int unit,
    int *value);

/*
 * }
 */
/* } */
#endif /* SWITCH_SVTAG_H_INCLUDED */
