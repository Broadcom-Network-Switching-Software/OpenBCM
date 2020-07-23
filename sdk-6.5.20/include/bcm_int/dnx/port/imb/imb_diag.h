/** \file imb_diag.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef IMB_CDU_DIAG_H_INCLUDED
/*
 * {
 */
#define IMB_CDU_DIAG_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * INCLUDE FILES:
 * {
 */

#include <shared/shrextend/shrextend_error.h>
#include <bcm/port.h>
#include <soc/dnxc/dnxc_port.h>
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
 * Functions
 * {
 */
/**We can't measure the serdes freq directly, but we can measure the sync_eth counter, 
and reconstruct the serdes freq from it by multiplying it with the blocks dividers. 
In PML and PMH GSMII the dividers are:
 VCO                                                   Sync_eth counter
  _        --> PM synce_div --> NBIL/H synce div -->         _
_| |_                                                      _| |_
So 
VCO = Fsynce * PMH_40_PML_20 * PM_1_7_11
and we need to do:
SerDes_rate = VCO/Oversample = Fsynce * PMH_40_PML_20 * PM_1_7_11 / Oversample

In PMH which is not GSMII the dividers are:
Serdes freq                                            Sync_eth counter
  _         --> PM synce_div --> NBIL/H synce div -->        _
_| |_                                                      _| |_
So we need to do:
SerDes_rate = Fsynce * PMH_40_PML_20 * PM_1_7_11 */
/**
 * \brief - provide the nif serdes rate measurment
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] is_rx - not used for NIF serdes measure
 * \param [out] phy_measure - phy measure output
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_diag_nif_phy_measure_get(
    int unit,
    bcm_port_t port,
    int is_rx,
    soc_dnxc_port_phy_measure_t * phy_measure);

/**
 * \brief - provide the fabric PHY measurment
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] is_rx - indicate the FIFO used to measure
 * \param [out] phy_measure - phy measure output
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_diag_fabric_phy_measure_get(
    int unit,
    bcm_port_t port,
    int is_rx,
    soc_dnxc_port_phy_measure_t * phy_measure);
/*
 * }
 */
#endif/*_IMB_CDU_DIAG_H_INCLUDED_*/
