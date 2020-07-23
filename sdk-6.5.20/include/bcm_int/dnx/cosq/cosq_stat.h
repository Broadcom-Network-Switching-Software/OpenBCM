/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file bcm_int/dnx/cosq/cosq_stat.h
 * Reserved.$ 
 */

#ifndef _DNX_COSQ_STAT_H_INCLUDED_
/*
 * { 
 */
#define _DNX_COSQ_STAT_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/cosq.h>

/*
 * Defines
 * {
 */

/*
 * }
 */

/**
* \brief
*   init the cosq stat configuration. set the range of itm queues to the maximum range
* \param [in] unit -unit id
* \param [in] itm_nof_commands -number of crps commands id that relevant for ingress itm
* \return
*    shr_error_e
* \remark
*   symetric core configuration
* \see
*   NONE
*/
shr_error_e dnx_cosq_stat_config_init(
    int unit,
    int itm_nof_commands);

/**
* \brief
*   Configure if to enable drop per DP.
*   Drop is neing made by trap mechanism.
*   This API just enable the trap mechanism that may drop packets with that DP.
*   In order to complete the drop configuration, one need to configure egress policer trap (enum: bcmRxTrapEgTxMetering)
* \param [in] unit        - unit id
* \param [in] flags
* \return
*   shr_error_e - Error Type
* \remark
* 
* \see
*   * None
*/
shr_error_e dnx_cosq_stat_discard_egress_meter_set(
    int unit,
    uint32 flags);

/**
* \brief
*   get from HW which DP drop is enabled and update the flags accordigly.
* \param [in] unit        - unit id
* \param [out] flags - flag BCM_COSQ_DISCARD_EGRESS is in, all others- out.
* \return
*   shr_error_e - Error Type
* \remark
* 
* \see
*   * None
*/
shr_error_e dnx_cosq_stat_discard_egress_meter_get(
    int unit,
    uint32 *flags);

/** } */
#endif /*_DNX_COSQ_STAT_H_INCLUDED_*/
