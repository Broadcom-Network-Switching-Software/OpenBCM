/** \file bcm_int/dnx/switch/switch_reflector.h
 * 
 * Internal DNX SWITCH-REFLECTOR APIs
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef SWITCH_REFLECTOR_H_INCLUDED
/* { */
#define SWITCH_REFLECTOR_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif
/**
 * \brief
 *  Init switch reflector module
 * \param [in] unit  - unit #.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
*/
shr_error_e dnx_switch_reflector_init(
    int unit);

/**
 * \brief
 *  Set reflecotor recycle priority.
 * \param [in] unit  - unit
 * \param [in] priority  - priority to set
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
*/
shr_error_e dnx_switch_reflector_rcy_priority_set(
    int unit,
    int priority);

/**
 * \brief
 *  Get reflecotor recycle priority.
 * \param [in] unit  - unit.
 * \param [out] priority  - hold priority
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
*/
shr_error_e dnx_switch_reflector_rcy_priority_get(
    int unit,
    int *priority);

/* } */
#endif /* SWITCH_REFLECTOR_H_INCLUDED */
