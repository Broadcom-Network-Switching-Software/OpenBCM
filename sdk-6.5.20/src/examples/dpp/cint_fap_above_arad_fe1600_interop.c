/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FAP above Arad - FE1600 interop example:
 *  
 * For FAP in generation above Arad: its default parameters are optimized to work with FE3200.
 * FAPs above Arad (Jericho, Jericho plus, Kalia) are backward compatible with DNX legacy generation (FE1600).
 * This Cint provides an example of what should be configured on the FAP's side when connecting a FAP above Arad to FE1600 legacy device.
 */

/*
 * Function: 
 *      cint_fap_above_arad_fe1600_interop_link
 * Purpose: 
 *      Configure on the FAP: per link related configuration required when connecting to FE1600 legacy device.
 * Parameters: 
 *      unit                                    (IN) - FAP unit #
 *      port                                    (IN) - link #
 *      pcs                                     (IN) - encoding bcmPortPCS64b66bFec or bcmPortPCS64b66b.
 *      cl72                                    (IN) - Enable/disable cl72
 * Returns: 
 *      BCM_E_XXX 
 *
 * Important: disabling PCP in the FAP needs to be done via SOC property: fabric_pcp_enable=0
 */


int
cint_fap_above_arad_fe1600_interop_link(int unit,
                         bcm_port_t port, 
                         int speed, 
                         bcm_port_pcs_t pcs, 
                         int cl72)
{
    int rv;

    /*Disable link*/
    rv = bcm_port_enable_set(unit, port, 0);
    if (BCM_FAILURE(rv))
    {
        printf("Port disable failed unit %d, port %d.\n", unit, port);
        return rv;
    }

    /*Configure PCS*/
    rv = bcm_port_control_set(unit,  port, bcmPortControlPCS, pcs);
    if (BCM_FAILURE(rv))
    {
        printf("PCS configuration failed unit %d, port %d, $pcs.\n", unit, port);
        return rv;
    }

    /*If KR-FEC: Disable extracting congestion indication from LLFC cells*/
    if (pcs == bcmPortPCS64b66bFec) /*KR-FEC*/
    {
        rv = bcm_port_control_set(unit,  port, bcmPortControlLlfcCellsCongestionIndEnable, 0);
        if (BCM_FAILURE(rv))
        {
            printf("Disable extracting congestion indication from LLFC cells failed unit %d, port %d, $pcs.\n", unit, port);
            return rv;
        }
    }

    /*Speed*/
    rv = bcm_port_speed_set(unit,  port, speed);
    if (BCM_FAILURE(rv))
    {
        printf("Speed set failed unit %d, port %d, speed %d.\n", unit, port, speed);
        return rv;
    }

    /*CL72*/
    rv = bcm_port_phy_control_set(unit,  port, BCM_PORT_PHY_CONTROL_CL72, cl72);
    if (BCM_FAILURE(rv))
    {
        printf("CL72 configuration failed unit %d, port %d, cl72 %d.\n", unit, port, cl72);
        return rv;
    }

    /*Disable PCP - Packet Cell Packing*/
    /*Need to use the SOC property fabric_pcp_enable=0 for the FAP*/

    /*Enable link*/
    rv = bcm_port_enable_set(unit, port, 1);
    if (BCM_FAILURE(rv))
    {
        printf("Port enable failed unit %d, port %d.\n", unit, port);
        return rv;
    }

    printf("cint_fap_above_arad_fe1600_interop_link: PASS - unit %d, port %d.\n", unit, port);
    return BCM_E_NONE;
}

/*
 * Function: 
 *      cint_fap_above_arad_fe1600_interop
 * Purpose: 
 *      FAP configuration required when connecting [FAP above ARAD] to FE1600 legacy device.
 * Parameters: 
 *      unit                                    (IN) - FAP unit #
 *      legacy_ports, nof_legacy_ports          (IN) - array  (and size of array) of links that are connected to legacy device.
 *      pcs                                     (IN) - encoding bcmPortPCS64b66bFec or bcmPortPCS64b66b.
 *      cl72                                    (IN) - Enable/disable cl72
 * Returns: 
 *      BCM_E_XXX 
 */
int
cint_fap_above_arad_fe1600_interop(int unit,
                    bcm_port_t *legacy_ports, int nof_legacy_ports, 
                    int speed, 
                    bcm_port_pcs_t pcs, 
                    int cl72)
{
    bcm_port_t port;
    int i, rv;

    /*Configure per link related configurations: speed, pcs, cl72, pcp*/
    for (i = 0; i < nof_legacy_ports; i++)
    {
        port = legacy_ports[i];
        rv = cint_fap_above_arad_fe1600_interop_link(unit, port, speed, pcs, cl72);
        if (BCM_FAILURE(rv))
        {
            printf("cint_fap_above_arad_fe1600_interop_link failed unit %d, port %d, speed %d, pcs %d, cl72 %d.\n", unit, port, speed, pcs, cl72);
            return rv;
        }
    }

    printf("cint_fap_above_arad_fe1600_interop: PASS - unit %d.\n", unit);
    return BCM_E_NONE;
}

/*
 * Function: 
 *      cint_fap_above_arad_fe1600_interop_example
 * Purpose: 
 *      This is an example to [FAP above ARAD]-FE1600 system.
 *      Assuming:   link rates: 10312, pcs: KR-FEC, cl72: enabled.
 * Parameters: 
 *      unit                                    (IN) - FAP unit #
 * Returns: 
 *      BCM_E_XXX 
 */
int
cint_fap_above_arad_fe1600_interop_example(int unit)
{
    int speed = 10312;
    bcm_port_pcs_t pcs = bcmPortPCS64b66bFec;
    int cl72 = 1;
    bcm_port_t legacy_ports[SOC_DPP_DEFS_GET_NOF_FABRIC_LINKS(&unit)];
    int nof_legacy_ports;
    bcm_port_config_t port_config;
    int rv;
    bcm_port_t port;

    /*Get all fabric link ports*/
    rv = bcm_port_config_get(unit, &port_config);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_config_get failed: unit %d.\n", unit);
        return rv;
    }

    nof_legacy_ports = 0;
    BCM_PBMP_ITER(port_config.sfi, port)
    {
        legacy_ports[nof_legacy_ports] = port;
        nof_legacy_ports++;
    }

    /*Configure device and all links*/
    rv = cint_fap_above_arad_fe1600_interop(unit, legacy_ports,  nof_legacy_ports, speed, pcs, cl72);
    if (BCM_FAILURE(rv))
    {
        printf("cint_fap_above_arad_fe1600_interop failed unit %d, speed %d, pcs %d, cl72 %d.\n", unit, speed, pcs, cl72);
        return rv;
    }

    printf("cint_fap_above_arad_fe1600_interop_example: PASS - unit %d.\n", unit);
    return BCM_E_NONE;
}
