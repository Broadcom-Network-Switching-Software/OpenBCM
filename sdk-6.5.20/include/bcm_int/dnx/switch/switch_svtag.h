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
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SWITCH_SVTAG_H_INCLUDED
/* { */
#define SWITCH_SVTAG_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
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
 * \param [in] esem_cmd_data - Holds all the available ESEM commands.
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
    dnx_esem_cmd_data_t * esem_cmd_data,
    uint8 *found);
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

/*
 * }
 */
/* } */
#endif /* SWITCH_SVTAG_H_INCLUDED */
