/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_ramon_interop.c
 *
 * Purpose:
 *  
 * Ramon default parameters are optimized for a pure Ramon-Jericho2 system.
 * Ramon is backward compatible with DNX previous generation: Jericho.
 * This cint provides as an example of what should be configured when connecting Ramon to a previous generation device.
 */

/*
 * Function:
 *      cint_ramon_interop
 * Purpose:
 *      This is an example to configure specific Ramon ports in a Ramon-Jericho system.
 *      This function configures the resource API configurations: speed, FEC, link training (and default phy_lane_config according to them).
 *      Assuming:   link rates: 25000, FEC: RS FEC, cl72: enabled.
 *                  Assuming all other fabric configurations are the same in Ramon and Jericho.
 * Parameters:
 *      unit                                    (IN) - unit #
 *      legacy_ports                            (IN) - an array of Ramon ports to configure
 *      nof_legacy_ports                        (IN) - size of legacy_ports array
 * Returns:
 *      BCM_E_XXX
 */
int
cint_ramon_interop(
    int unit,
    bcm_port_t * legacy_ports,
    int nof_legacy_ports)
{
    int rv;
    bcm_port_t port;
    int port_index;
    bcm_port_resource_t port_resource_arr[nof_legacy_ports];
    int speed = 25000;
    bcm_port_phy_fec_t fec_type = bcmPortPhyFecRs206;
    int link_training = 1;

    /*
     * set the required resource configuration for all legacy ports in port_resource_arr
     */
    for (port_index = 0; port_index < nof_legacy_ports; ++port_index)
    {
        port = legacy_ports[port_index];
        bcm_port_resource_t_init(&port_resource_arr[port_index]);

        port_resource_arr[port_index].port = port;
        port_resource_arr[port_index].speed = speed;
        port_resource_arr[port_index].fec_type = fec_type;
        port_resource_arr[port_index].link_training = link_training;
        rv = bcm_port_resource_default_get(unit, port, 0, &port_resource_arr[port_index]);
        if (BCM_FAILURE(rv))
        {
            printf("bcm_port_resource_default_get failed unit %d, port %d.\n", unit, port);
            return rv;
        }
    }

    /*
     * configure all legacy ports
     */
    rv = bcm_port_resource_multi_set(unit, nof_legacy_ports, port_resource_arr);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_resource_multi_set failed unit %d.\n", unit);
        return rv;
    }

    printf("cint_ramon_interop: PASS - unit %d.\n", unit);
    return BCM_E_NONE;
}

/*
 * Function: 
 *      cint_ramon_interop_example
 * Purpose: 
 *      This is an example to configure Ramon unit in a Ramon-Jericho system.
 * Parameters: 
 *      unit                                    (IN) - unit #
 * Returns: 
 *      BCM_E_XXX 
 */
int
cint_ramon_interop_example(
    int unit)
{
    int rv;
    bcm_port_config_t port_config;
    bcm_port_t port;
    uint32 max_nof_ports = *(dnxc_data_get(unit, "port", "general", "nof_links", NULL));
    bcm_port_t legacy_ports[max_nof_ports];
    int nof_legacy_ports = 0;

    /*
     * Get all fabric ports
     */
    rv = bcm_port_config_get(unit, &port_config);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_config_get failed: unit %d.\n", unit);
        return rv;
    }

    BCM_PBMP_ITER(port_config.sfi, port)
    {
        legacy_ports[nof_legacy_ports] = port;
        ++nof_legacy_ports;
    }

    /*
     * Interop
     */
    rv = cint_ramon_interop(unit, legacy_ports, nof_legacy_ports);
    if (BCM_FAILURE(rv))
    {
        printf("cint_ramon_interop failed: unit %d.\n", unit);
        return rv;
    }

    printf("cint_ramon_interop_example: PASS - unit %d.\n", unit);
    return BCM_E_NONE;
}
