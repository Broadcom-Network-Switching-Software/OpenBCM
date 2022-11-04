/** \file imb_feu_phy_internal.h
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef IMB_FEU_PHY_INTERNAL_H_INCLUDED
/*
 * {
 */
#define IMB_FEU_PHY_INTERNAL_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * INCLUDE FILES:
 * {
 */

#include <bcm/port.h>
#include <bcm/types.h>
#include <soc/sand/shrextend/shrextend_debug.h>

/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * }
 */

/*
 * Structs
 * {
 */

/**
 * \brief - Check if the TDM slots needs to be
 *    doubled
 *
 * \param [in] unit - chip unit id
 * \param [in] logical_port - logical port
 * \param [in] adjust_port - the port whose TDM slot to be adjusted
 * \param [in] flags - Indicate to use single or double slot
 * \param [out] is_required - If doubling slots is required
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e imb_feu_phy_calendar_double_slot_is_required_get(
    int unit,
    bcm_port_t logical_port,
    bcm_port_t adjust_port,
    uint32 flags,
    int *is_required);

#endif/*_IMB_FEU_PHY_INTERNAL_H_INCLUDED_*/
