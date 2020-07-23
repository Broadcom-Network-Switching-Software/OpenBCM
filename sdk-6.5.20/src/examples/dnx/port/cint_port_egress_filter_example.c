/* $Id:
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~Traditional Bridge: Port Egress filter~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * File: cint_port_egress_filter_example.c
 * Purpose: Example of how to set the Port Egress filter for Unknown UC / Unknown MC / BC
 *
 * Default settings:
 * It is assumed diag_init is executed:
 *  All ports set with TPID 0x8100 and all packet formats (tagged/untagged) are accepted.
 *  PVID of all ports set to VID = 1, thus untagged packet is associated to VLAN 1.
 *  All VLANs are mapped to Default STG (0).
 *  STP state of all ports in default topology is set to forwarding.
 * 
 * Usage:
 *      port_eg_filter_set(unit,eg_port,disable_types): Set disable filter types on Egress port.
 *          disable_types according to port_control types: BCM_PORT_CONTROL_FILTER_DISABLE_*
 *      cint_rx_trap_erpp_traps_main(0, bcmRxTrapEgUnknownDa, NULL): egress unknow DA trap
 *
 * Specific example:
 * Filter Unknown DA on egress port 14
 *  port_eg_filter_example(unit,14,0);
 * Send Packet:
 * Unknown DA packet : VLAN 1 DA : 1
 * Expects flooding to ethernet ports except 14
 */

int
port_eg_filter_set(int unit, int egress_port, int disable_types)
{
    int rv = BCM_E_NONE;
    int value = 0;

    if (disable_types == 0)
    {
        value = BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_MC | BCM_PORT_CONTROL_FILTER_DISABLE_DA_BC;
    }
    else if (disable_types == 1)
    {
        value = BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_UC | BCM_PORT_CONTROL_FILTER_DISABLE_DA_BC;
    }
    else
    {
        value = BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_MC | BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_UC;
    }

    rv = bcm_port_control_set(unit, egress_port, bcmPortControlEgressFilterDisable, value);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_control_set, port=%d, \n", egress_port);
        return rv;
    }

    return rv;
}

/* An example of filter according to type */
/* type 0 - enable only Unknown DA , 1 - enable only Unknown MC, 2 - enable only BC */
int port_eg_filter_example_main(int unit, int eg_port, int type)
{
    int rv = BCM_E_NONE;

    rv = port_eg_filter_set(unit, eg_port, type);
    if (rv != BCM_E_NONE) {
        printf("Error, in port_eg_filter_set\n");
        return rv;
    }

    rv = cint_rx_trap_erpp_traps_main(unit, bcmRxTrapEgUnknownDa, NULL);
    if (rv != BCM_E_NONE) {
        printf("Error, in cint_rx_trap_erpp_traps_main for bcmRxTrapEgUnknownDa \n");
        return rv;
    }

    return rv;
}
