/**
 * \file algo_ptp.h Internal DNX Managment APIs 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef ALGO_PTP_H_INCLUDED
/*
 * { 
 */
#define ALGO_PTP_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/** template manager defines */
#define DNX_PTP_PORT_TEMPLATE_NAME "ptp port profile"

/**
 * \brief
 *      callback print function for template manager. print the ptp port profile
 * \param [in] unit -unit id
 * \param [in] data -const pointer to profile data
 * \return
 *  void
 * \remark
 *   NONE
 * \see
 *   NONE
 */
void dnx_algo_ptp_tm_port_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Init function, init all ptp resources
 * 
 * \param [in] unit - Number of hardware unit used.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_ptp_init(
    int unit);

/*
 * } 
 */
#endif /* ALGO_PTP_H_INCLUDED */
