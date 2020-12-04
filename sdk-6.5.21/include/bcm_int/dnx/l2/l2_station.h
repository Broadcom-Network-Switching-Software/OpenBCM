/**
 * \file bcm_int/dnx/l2/l2_station.h 
 * Internal DNX L2 APIs 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef L2_STATION_H_INCLUDED
/*
 * {
 */
#define L2_STATION_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */

/*
 * }
 */

/*
 * DEFINES
 * {
 */

/*
 * }
 */

/**
 * \brief - Set EXEM based multiple my mac system mode.
 * Current supported modes are either VSI or source port.
 *  
 * \param [in] unit - Number of hardware unit used.
 * \param [in] arg - system mode:
 * * 0 - multiple my mac VSI mode (default)
 * * 1 - multiple my mac source port mode  
 * \return
 *   shr_error_e  
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l2_station_multi_my_mac_exem_mode_set(
    int unit,
    int arg);

/**
 * \brief - Get EXEM based multiple my mac system mode.
 * Current supported modes are either VSI or source port.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] arg - system mode:
 * * 0 - multiple my mac VSI mode (default)
 * * 1 - multiple my mac source port mode
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l2_station_multi_my_mac_exem_mode_get(
    int unit,
    int *arg);

/*
 * }
 */
#endif /* L2_STATION_H_INCLUDED */
