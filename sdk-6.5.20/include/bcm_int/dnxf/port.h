/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        port.h
 * Purpose:     PORT internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DNXF_PORT_H_
#define   _BCM_INT_DNXF_PORT_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif

/* Attributes taht can be controlled on BCM88750*/
#define _BCM_DNXF_PORT_ATTRS      \
   (BCM_PORT_ATTR_ENABLE_MASK      | \
    BCM_PORT_ATTR_SPEED_MASK       | \
    BCM_PORT_ATTR_LINKSCAN_MASK    | \
    BCM_PORT_ATTR_LOOPBACK_MASK)

int dnxf_port_init(int unit);
int dnxf_port_deinit(int unit);

/**
 * \brief - provide the fabric serdes rate measurment
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
int
dnxf_port_phy_measure_get(
    int unit,
    bcm_port_t port,
    int is_rx,
    soc_dnxc_port_phy_measure_t * phy_measure);
#endif /*_BCM_INT_DNXF_PORT_H_*/
