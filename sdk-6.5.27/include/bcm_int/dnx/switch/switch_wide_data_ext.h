/** \file bcm_int/dnx/switch/switch_wide_data_ext.h
 * 
 * Internal DNX SWITCH-WIDE-DATA-EXT APIs
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef SWITCH_WIDE_DATA_EXT_H_INCLUDED
#define SWITCH_WIDE_DATA_EXT_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/**
 * \brief
 *  Set global mode for wide data extension key type
 * \param [in] unit  - unit #.
 * \param [in] mode  - wide data extension mode.
 * \return
 *    shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
*/
shr_error_e dnx_switch_wide_data_ext_key_mode_set(
    int unit,
    bcm_switch_wide_data_extension_key_mode_t mode);

/**
 * \brief
 *  get global mode for wide data extension key type
 * \param [in] unit  - unit #.
 * \param [out] mode  - wide data extension mode.
 * \return
 *    shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
*/
shr_error_e dnx_switch_wide_data_ext_key_mode_get(
    int unit,
    bcm_switch_wide_data_extension_key_mode_t * mode);

/**
 * \brief
 *  switch wide data init
 * \param [in] unit  - unit #.
  * \return
 *    shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
*/
shr_error_e dnx_switch_wide_data_init(
    int unit);
#endif /* SWITCH_WIDE_DATA_EXT_H_INCLUDED */
