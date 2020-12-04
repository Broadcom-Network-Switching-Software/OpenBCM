/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_phy_loopback_on_lane_swapped_port.c
 *
 * Purpose:     Configuring a PHY loopback on lane-swapped ports (lane to SerDes Tx mapping is not 1:1) is not supported.
 *              This cint provides a recommended example for configuring PHY loopback on such a port.
 *              The cint is suitable for fabric ports only.
 *
 * Notes:
 *              1) Relevant functions for user:
 *                  - cint_phy_loopback_on_lane_swapped_port_set
 *                  - cint_phy_loopback_on_lane_swapped_port_set_all
 *                  - cint_phy_loopback_on_lane_swapped_port_clear
 *                  - cint_phy_loopback_on_lane_swapped_port_clear_all
 *
 *              2) The procedures in this cint perform bcm_port_detach on all the attached lanes of the octet of the loopback-port.
 *              Thus, after each such procedure, the configurations of the attached lanes in the octet will be set to default.
 *              It's the user's responsibility to restore the ports properties that were set via APIs/SoC properties.
 *              Exceptions: resource configuration - speed, fec_type, phy_lane_config - will be restored by the cint.
 *
 *              3) After calling cint_phy_loopback_on_lane_swapped_port_set
 *              user is expected not to change the lane-mapping (bcm_port_lane_to_serdes_map_set)
 *              of the octet of the port on which loopback was set.
 *              The lane-mapping of the octet can be changed only after calling cint_phy_loopback_on_lane_swapped_port_clear on all the loopback-ports of the octet.
 */

int MAX_NOF_LANES = 192;
int nof_lanes_in_octet = 8;

/* holds originally configured <lane to SerDes Tx> mapping on loopback-ports octets or {0,0,0,0,0,0,0,0} if no loopback is currently set in the octet */
bcm_port_lane_to_serdes_map_t lane_to_serdes_map_orig[MAX_NOF_LANES] = { {0, 0} };

/*
 * Function:
 *      cint_phy_loopback_on_lane_swapped_port_port2lane_get
 *
 * Purpose:
 *      convert logical port to lane
 *
 * Parameters:
 *      unit                                    (IN) - unit #
 *      port                                    (IN) - logical port #
 *      lane                                   (OUT) - lane #
 *
 * Returns:
 *      BCM_E_XXX
 */
int
cint_phy_loopback_on_lane_swapped_port_port2lane_get(
    int unit,
    bcm_port_t port,
    int *lane)
{
    int rv;
    int is_dnx;

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_device_member_get failed to get is_dnx\n", unit);
        return rv;
    }

    if (is_dnx)
    {
        if (BCM_GPORT_IS_LOCAL_FABRIC(port))
        {
            *lane = BCM_GPORT_LOCAL_FABRIC_GET(port);
        }
        else
        {
            uint32 fabric_port_base = *(dnxc_data_get(unit, "port", "general", "fabric_port_base", NULL));
            *lane = port - fabric_port_base;
        }
    }
    else
    {
        *lane = port;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      cint_phy_loopback_on_lane_swapped_port_lane2port_get
 *
 * Purpose:
 *      convert lane to logical port
 *
 * Parameters:
 *      unit                                    (IN) - unit #
 *      lane                                    (IN) - lane #
 *      port                                   (OUT) - logical port #
 *
 * Returns:
 *      BCM_E_XXX
 */
int
cint_phy_loopback_on_lane_swapped_port_lane2port_get(
    int unit,
    int lane,
    bcm_port_t * port)
{
    int rv;
    int is_dnx;

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_device_member_get failed to get is_dnx\n", unit);
        return rv;
    }

    if (is_dnx)
    {
        uint32 fabric_port_base = *(dnxc_data_get(unit, "port", "general", "fabric_port_base", NULL));
        *port = lane + fabric_port_base;
    }
    else
    {
        *port = lane;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      cint_phy_loopback_on_lane_swapped_port_nof_links_get
 *
 * Purpose:
 *      convert lane to logical port
 *
 * Parameters:
 *      unit                                    (IN) - unit #
 *      lane                                    (IN) - lane #
 *      port                                   (OUT) - logical port #
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 *      Currently suitable for FE. Needs to be implemented for FAP
 */
int
cint_phy_loopback_on_lane_swapped_port_nof_links_get(
    int unit,
    uint32 *nof_links)
{
    int rv;
    int is_dnx = 0;
    int is_dnxf = 0;

    /*
     * Init vars according to device type (DNX/DNXF)
     */
    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_device_member_get failed to get is_dnx\n", unit);
        return rv;
    }
    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNXF, &is_dnxf);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_device_member_get failed to get is_dnxf\n", unit);
        return rv;
    }

    /*
     * Get number of links
     */
    if (is_dnx)
    {
        *nof_links = *(dnxc_data_get(unit, "fabric", "links", "nof_links", NULL));
    }
    else if (is_dnxf)
    {
        *nof_links = *(dnxc_data_get(unit, "port", "general", "nof_links", NULL));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      cint_phy_loopback_on_lane_swapped_port_set
 *
 * Purpose:
 *      configure PHY loopback on a port that is lane-swapped (lane to SerDes Tx mapping is not 1:1).
 *      This is done by the following sequence:
 *
 *      1) Detach all attached ports of the octet
 *      2) All originally-mapped lanes of the octet are re-mapped to be non-swapped: lane to SerDes Tx -> 1:1
 *      3) Re-attach (probe) all originally-attached ports in the octet
 *      4) Restore resource configuration of the originally-attached ports
 *      5) Re-enable all originally-attached ports in the octet that were enabled
 *      6) Set loopback on the port
 *
 * Parameters:
 *      unit                                    (IN) - unit #
 *      loopback_port                           (IN) - port # of the port on which to enable PHY loopback
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 *      Since the sequence contains a detach step -
 *      Configurations configured on the originally-attached ports of the octet via API/SoC properties will not be restored.
 *      After the attach step the configuration of the originally-attached ports of the octet is set to be according to default.
 *      Exceptions: the resource configurations - speed, fec_type, phy_lane_config - will be restored.
 */
int
cint_phy_loopback_on_lane_swapped_port_set(
    int unit,
    bcm_port_t loopback_port)
{
    int rv;
    int port, lane, loopback_lane, first_lane_in_octet, lane_index_inside_octet;
    bcm_port_resource_t resource[nof_lanes_in_octet];
    int lane_is_enabled[nof_lanes_in_octet] = { 0 };
    bcm_port_config_t config;
    bcm_pbmp_t octet_ports_pbmp, successfully_detached_pbmp, successfully_attached_pbmp;
    uint32 nof_lanes = -1;

    rv = cint_phy_loopback_on_lane_swapped_port_nof_links_get(unit, &nof_lanes);
    if (BCM_FAILURE(rv))
    {
        return BCM_E_FAIL;
    }
    bcm_port_lane_to_serdes_map_t lane_to_serdes_map[nof_lanes] = { {0, 0} };

    BCM_PBMP_CLEAR(octet_ports_pbmp);
    BCM_PBMP_CLEAR(successfully_detached_pbmp);
    BCM_PBMP_CLEAR(successfully_attached_pbmp);

    /*
     * get default lane-mapping of all the lanes 
     */
    rv = bcm_port_lane_to_serdes_map_get(unit, BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE, nof_lanes, lane_to_serdes_map);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_lane_to_serdes_map_get failed - unit %d, port %d.\n", unit, loopback_port);
        return rv;
    }

    /*
     * get loopback_lane and first_lane in loopback-lane octet 
     */
    rv = cint_phy_loopback_on_lane_swapped_port_port2lane_get(unit, loopback_port, &loopback_lane);
    if (BCM_FAILURE(rv))
    {
        printf("cint_phy_loopback_on_lane_swapped_port_port2lane_get failed - unit %d, port %d.\n", unit,
               loopback_port);
        return rv;
    }
    first_lane_in_octet = (loopback_lane / nof_lanes_in_octet) * nof_lanes_in_octet;

    /*
     * if we already configured one loopback in this octet - swap was already aligned -> nothing to do the check is if
     * the lane mapping was already stored 
     */
    if ((lane_to_serdes_map_orig[first_lane_in_octet].serdes_tx_id != 0)
        || (lane_to_serdes_map_orig[first_lane_in_octet + 1].serdes_tx_id != 0))
    {
        rv = bcm_port_loopback_set(unit, loopback_port, BCM_PORT_LOOPBACK_PHY);
        if (BCM_FAILURE(rv))
        {
            printf("bcm_port_loopback_set failed - unit %d, port %d.\n", unit, loopback_port);
            return rv;
        }

        return BCM_E_NONE;
    }

    /*
     * 1. get the original resource configuration and enable state of all the attached ports in the octet of the
     * loopback port 2. detach all the attached ports 
     */
    rv = bcm_port_config_get(unit, &config);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_config_get failed - unit %d, port %d.\n", unit, loopback_port);
        return rv;
    }

    for (lane = first_lane_in_octet; lane < (first_lane_in_octet + nof_lanes_in_octet); ++lane)
    {
        rv = cint_phy_loopback_on_lane_swapped_port_lane2port_get(unit, lane, &port);
        if (BCM_FAILURE(rv))
        {
            printf("cint_phy_loopback_on_lane_swapped_port_lane2port_get failed - unit %d, lane %d.\n", unit, lane);
            return rv;
        }

        if (BCM_PBMP_MEMBER(config.sfi, port))
        {
            lane_index_inside_octet = lane % nof_lanes_in_octet;

            rv = bcm_port_resource_get(unit, port, &resource[lane_index_inside_octet]);
            if (BCM_FAILURE(rv))
            {
                printf("bcm_port_resource_get failed - unit %d, port %d.\n", unit, port);
                return rv;
            }

            rv = bcm_port_enable_get(unit, port, &lane_is_enabled[lane_index_inside_octet]);
            if (BCM_FAILURE(rv))
            {
                printf("bcm_port_enable_get failed - unit %d, port %d.\n", unit, port);
                return rv;
            }
            BCM_PBMP_PORT_ADD(octet_ports_pbmp, port);
        }
    }
    rv = bcm_port_detach(unit, octet_ports_pbmp, &successfully_detached_pbmp);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_detach failed - unit %d, port %d.\n", unit, loopback_port);
        return rv;
    }

    /*
     * 1. keep original Tx lane mapping of the octet, to be restored later 2. align Tx lane mapping to be 1:1 - only on 
     * originally lane-mapped ports 
     */
    for (lane = first_lane_in_octet; lane < (first_lane_in_octet + nof_lanes_in_octet); ++lane)
    {
        lane_to_serdes_map_orig[lane].serdes_tx_id = lane_to_serdes_map[lane].serdes_tx_id;
        if (lane_to_serdes_map[lane].serdes_tx_id != BCM_PORT_LANE_TO_SERDES_NOT_MAPPED)
        {
            lane_to_serdes_map[lane].serdes_tx_id = lane;
        }
    }

    rv = bcm_port_lane_to_serdes_map_set(unit, BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE, nof_lanes, lane_to_serdes_map);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_lane_to_serdes_map_set failed - unit %d, port %d.\n", unit, loopback_port);
        return rv;
    }

    /*
     * attach all originally-attached ports in the octet of the loopback port 
     */
    rv = bcm_port_probe(unit, octet_ports_pbmp, &successfully_attached_pbmp);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_probe failed - unit %d, port %d.\n", unit, loopback_port);
        return rv;
    }

    /*
     * restore resource configuration and enable all the enabled ports in the octet of the loopback port 
     */
    for (lane = first_lane_in_octet; lane < (first_lane_in_octet + nof_lanes_in_octet); ++lane)
    {
        rv = cint_phy_loopback_on_lane_swapped_port_lane2port_get(unit, lane, &port);
        if (BCM_FAILURE(rv))
        {
            printf("cint_phy_loopback_on_lane_swapped_port_lane2port_get failed - unit %d, lane %d.\n", unit, lane);
            return rv;
        }

        if (BCM_PBMP_MEMBER(config.sfi, port))
        {
            lane_index_inside_octet = lane % nof_lanes_in_octet;

            rv = bcm_port_resource_set(unit, port, &resource[lane_index_inside_octet]);
            if (BCM_FAILURE(rv))
            {
                printf("bcm_port_resource_set failed - unit %d, port %d.\n", unit, port);
                return rv;
            }

            if (lane_is_enabled[lane_index_inside_octet])
            {
                rv = bcm_port_enable_set(unit, port, 1);
                if (BCM_FAILURE(rv))
                {
                    printf("bcm_port_enable_set failed - unit %d, port %d.\n", unit, port);
                    return rv;
                }
            }
        }
    }

    rv = bcm_port_loopback_set(unit, loopback_port, BCM_PORT_LOOPBACK_PHY);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_loopback_set failed - unit %d, port %d.\n", unit, loopback_port);
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      cint_phy_loopback_on_lane_swapped_port_set_all
 *
 * Purpose:
 *      Perform cint_phy_loopback_on_lane_swapped_port_set on all the fabric ports of the device
 *
 * Parameters:
 *      unit                                    (IN) - unit #
 *
 * Returns:
 *      BCM_E_XXX
 */
int
cint_phy_loopback_on_lane_swapped_port_set_all(
    int unit)
{
    int rv;
    int port;
    bcm_port_config_t config;

    rv = bcm_port_config_get(unit, &config);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_config_get failed - unit %d.\n", unit);
        return rv;
    }

    BCM_PBMP_ITER(config.sfi, port)
    {
        rv = cint_phy_loopback_on_lane_swapped_port_set(unit, port);
        if (BCM_FAILURE(rv))
        {
            printf("cint_phy_loopback_on_lane_swapped_port_set_all: failed - unit %d.\n", unit);
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      cint_phy_loopback_on_lane_swapped_port_clear
 *
 * Purpose:
 *      after configuring PHY loopback on a port that is lane-swapped (lane to SerDes Tx mapping is not 1:1),
 *      this function disables the loopback and restores original lane mapping of the relevant octet.
 *      This is done by the following sequence:
 *
 *      1) Remove loopback on the port
 *      2) Detach all the attached ports of the octet
 *      3) Restore the original lane-mapping of all lane-mapped ports of the octet
 *      4) Re-attach (probe) all originally-attached ports in the octet
 *      5) Restore resource configuration of the originally-attached ports
 *      6) Re-enable all originally-attached ports in the octet that were enabled
 *
 * Parameters:
 *      unit                                    (IN) - unit #
 *      loopback_port                           (IN) - lane # of the port on which to enable PHY loopback
 *
 * Returns:
 *      BCM_E_XXX
 *
 *
 * Notes:
 *      Since the sequence contains a detach step -
 *      Configurations configured on the originally-attached ports of the octet via API/SoC properties will not be restored.
 *      After the attach step the configuration of the originally-attached ports of the octet is set to be according to default.
 *      Exceptions: resource configuration - speed, fec_type, phy_lane_config - will be restored.
 */
int
cint_phy_loopback_on_lane_swapped_port_clear(
    int unit,
    bcm_port_t loopback_port)
{
    int rv;
    int port, lane, loopback_lane, first_lane_in_octet, lane_index_inside_octet;
    bcm_port_resource_t resource[nof_lanes_in_octet];
    int lane_is_enabled[nof_lanes_in_octet] = { 0 };
    int loopback;
    bcm_port_config_t config;
    bcm_pbmp_t octet_ports_pbmp, successfully_detached_pbmp, successfully_attached_pbmp;
    uint32 nof_lanes = -1;

    rv = cint_phy_loopback_on_lane_swapped_port_nof_links_get(unit, &nof_lanes);
    if (BCM_FAILURE(rv))
    {
        return BCM_E_FAIL;
    }
    bcm_port_lane_to_serdes_map_t lane_to_serdes_map[nof_lanes] = { {0, 0}
    };

    BCM_PBMP_CLEAR(octet_ports_pbmp);
    BCM_PBMP_CLEAR(successfully_detached_pbmp);
    BCM_PBMP_CLEAR(successfully_attached_pbmp);

    /*
     * remove the loopback 
     */
    rv = bcm_port_loopback_set(unit, loopback_port, BCM_PORT_LOOPBACK_NONE);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_loopback_set failed - unit %d, port %d.\n", unit, loopback_port);
        return rv;
    }

    /*
     * get loopback_lane and first_lane in loopback-lane octet 
     */
    rv = cint_phy_loopback_on_lane_swapped_port_port2lane_get(unit, loopback_port, &loopback_lane);
    if (BCM_FAILURE(rv))
    {
        printf("cint_phy_loopback_on_lane_swapped_port_port2lane_get failed - unit %d, port %d.\n", unit,
               loopback_port);
        return rv;
    }
    first_lane_in_octet = (loopback_lane / nof_lanes_in_octet) * nof_lanes_in_octet;

    rv = bcm_port_config_get(unit, &config);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_config_get failed - unit %d, port %d.\n", unit, loopback_port);
        return rv;
    }

    /*
     * if another PHY loopback is configured in the octet - no need to restore the original lane mapping. lane mapping
     * will be restored only when last PHY loopback of the octet is removed 
     */
    for (lane = first_lane_in_octet; lane < (first_lane_in_octet + nof_lanes_in_octet); ++lane)
    {
        rv = cint_phy_loopback_on_lane_swapped_port_lane2port_get(unit, lane, &port);
        if (BCM_FAILURE(rv))
        {
            printf("cint_phy_loopback_on_lane_swapped_port_lane2port_get failed - unit %d, lane %d.\n", unit, lane);
            return rv;
        }

        if (BCM_PBMP_MEMBER(config.sfi, port))
        {
            rv = bcm_port_loopback_get(unit, port, &loopback);
            if (BCM_FAILURE(rv))
            {
                printf("bcm_port_loopback_get failed - unit %d, port %d.\n", unit, loopback_port);
                return rv;
            }
            if (loopback == BCM_PORT_LOOPBACK_PHY)
            {
                return BCM_E_NONE;
            }
        }
    }

    /*
     * get default lane-mapping of all the ports 
     */

    rv = bcm_port_lane_to_serdes_map_get(unit, BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE, nof_lanes, lane_to_serdes_map);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_lane_to_serdes_map_get failed - unit %d, port %d.\n", unit, loopback_port);
        return rv;
    }

    /*
     * 1. get the original resource configuration and enable state of all the attached ports in the octet of the
     * loopback port 2. detach all the attached ports 
     */
    for (lane = first_lane_in_octet; lane < (first_lane_in_octet + nof_lanes_in_octet); ++lane)
    {
        rv = cint_phy_loopback_on_lane_swapped_port_lane2port_get(unit, lane, &port);
        if (BCM_FAILURE(rv))
        {
            printf("cint_phy_loopback_on_lane_swapped_port_lane2port_get failed - unit %d, lane %d.\n", unit, lane);
            return rv;
        }
        if (BCM_PBMP_MEMBER(config.sfi, port))
        {
            lane_index_inside_octet = lane % nof_lanes_in_octet;

            rv = bcm_port_resource_get(unit, port, &resource[lane_index_inside_octet]);
            if (BCM_FAILURE(rv))
            {
                printf("bcm_port_resource_get failed - unit %d, port %d.\n", unit, port);
                return rv;
            }

            rv = bcm_port_enable_get(unit, port, &lane_is_enabled[lane_index_inside_octet]);
            if (BCM_FAILURE(rv))
            {
                printf("bcm_port_enable_get failed - unit %d, port %d.\n", unit, port);
                return rv;
            }
            BCM_PBMP_PORT_ADD(octet_ports_pbmp, port);
        }
    }

    rv = bcm_port_detach(unit, octet_ports_pbmp, &successfully_detached_pbmp);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_detach failed - unit %d, port %d.\n", unit, loopback_port);
        return rv;
    }

    /*
     * restore original Tx lane mapping of the octet of the loopback port (for originally lane-mapped ports) 
     */
    for (lane = first_lane_in_octet; lane < (first_lane_in_octet + nof_lanes_in_octet); ++lane)
    {
        if (lane_to_serdes_map[lane].serdes_tx_id != BCM_PORT_LANE_TO_SERDES_NOT_MAPPED)
        {
            lane_to_serdes_map[lane].serdes_tx_id = lane_to_serdes_map_orig[lane].serdes_tx_id;
            lane_to_serdes_map_orig[lane].serdes_tx_id = 0;
        }
    }

    rv = bcm_port_lane_to_serdes_map_set(unit, BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE, nof_lanes, lane_to_serdes_map);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_lane_to_serdes_map_set failed - unit %d, port %d.\n", unit, loopback_port);
        return rv;
    }

    /*
     * attach all originally-attached ports in the loopback port octet 
     */
    rv = bcm_port_probe(unit, octet_ports_pbmp, &successfully_attached_pbmp);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_probe failed - unit %d, port %d.\n", unit, loopback_port);
        return rv;
    }

    /*
     * restore resource configuration and enable all the originally-attached ports that were enabled, in the octet of
     * the loopback port 
     */
    for (lane = first_lane_in_octet; lane < (first_lane_in_octet + nof_lanes_in_octet); ++lane)
    {
        rv = cint_phy_loopback_on_lane_swapped_port_lane2port_get(unit, lane, &port);
        if (BCM_FAILURE(rv))
        {
            printf("cint_phy_loopback_on_lane_swapped_port_lane2port_get failed - unit %d, lane %d.\n", unit, lane);
            return rv;
        }

        if (BCM_PBMP_MEMBER(config.sfi, port))
        {
            lane_index_inside_octet = lane % nof_lanes_in_octet;

            rv = bcm_port_resource_set(unit, port, &resource[lane_index_inside_octet]);
            if (BCM_FAILURE(rv))
            {
                printf("bcm_port_resource_set failed - unit %d, port %d.\n", unit, port);
                return rv;
            }

            if (lane_is_enabled[lane_index_inside_octet])
            {
                rv = bcm_port_enable_set(unit, port, 1);
                if (BCM_FAILURE(rv))
                {
                    printf("bcm_port_enable_set failed - unit %d, port %d.\n", unit, port);
                    return rv;
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      cint_phy_loopback_on_lane_swapped_port_clear_all
 *
 * Purpose:
 *      Perform cint_phy_loopback_on_lane_swapped_port_clear on all the fabric ports of the device
 *
 * Parameters:
 *      unit                                    (IN) - unit #
 *
 * Returns:
 *      BCM_E_XXX
 */
int
cint_phy_loopback_on_lane_swapped_port_clear_all(
    int unit)
{
    int rv;
    int port;
    bcm_port_config_t config;

    rv = bcm_port_config_get(unit, &config);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_config_get failed - unit %d.\n", unit);
        return rv;
    }

    BCM_PBMP_ITER(config.sfi, port)
    {
        rv = cint_phy_loopback_on_lane_swapped_port_clear(unit, port);
        if (BCM_FAILURE(rv))
        {
            printf("cint_phy_loopback_on_lane_swapped_port_clear_all: failed - unit %d.\n", unit);
            return rv;
        }
    }

    return BCM_E_NONE;
}
