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

/*
 * }
 */
/* } */
#endif /* SWITCH_SVTAG_H_INCLUDED */
