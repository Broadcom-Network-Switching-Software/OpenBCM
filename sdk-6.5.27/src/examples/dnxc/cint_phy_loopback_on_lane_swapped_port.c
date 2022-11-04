/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:        cint_phy_loopback_on_lane_swapped_port.c
 *
 * Purpose:     Configuring a PHY and PHY Remote loopback on lane-swapped ports (lane to SerDes Tx mapping is not 1:1) is not supported.
 *              This cint provides a recommended example for configuring PHY loopback on such a port.
 *              The cint is suitable for fabric ports only.
 *
 * Notes:
 *              1) Relevant functions for user:
 *                  - cint_phy_loopback_on_lane_swapped_port_set
 *                  - cint_phy_loopback_on_lane_swapped_port_set_all
 *                  - cint_phy_remote_loopback_on_lane_swapped_port_set
 *                  - cint_phy_remote_loopback_on_lane_swapped_port_set_all
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
 * 
 *              4) The procedure cint_phy_loopback_on_lane_swapped_port_clear / cint_phy_loopback_on_lane_swapped_port_clear_all will be able to clear results from both
 *                 PHY and PHY REMOTE loopback CINTs.
 */
int MAX_NOF_LANES = 512;
int PHY_LOOPBACK_ON_ALL_PORTS  = -1;

/* holds originally configured <lane to SerDes Tx> mapping on loopback-ports octets or {0, 0, 0, 0, 0, 0, 0, 0} if no loopback is currently set in the octet */
bcm_port_lane_to_serdes_map_t lane_to_serdes_map_orig[MAX_NOF_LANES] = { {0, 0} };

int
cint_phy_loopback_on_lane_swapped_port_gport2port_get(
    int unit,
    bcm_port_t gport,
    bcm_port_t *port)
{
    uint32 fabric_port_base;

    if (BCM_GPORT_IS_LOCAL_FABRIC(gport))
    {
        fabric_port_base = *(dnxc_data_get(unit, "port", "general", "fabric_port_base", NULL));
        *port = fabric_port_base + BCM_GPORT_LOCAL_FABRIC_GET(gport);
    }
    else
    {
        *port = gport;
    }

    return BCM_E_NONE;
}

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
 *
 */
int
cint_phy_loopback_on_lane_swapped_port_port2lane_get(
    int unit,
    bcm_port_t port,
    int *lane)
{
    int is_dnx;

    BCM_IF_ERROR_RETURN_MSG(bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx), "failed to get is_dnx");

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
 *
 */
int
cint_phy_loopback_on_lane_swapped_port_lane2port_get(
    int unit,
    int lane,
    bcm_port_t * port)
{
    int is_dnx;

    BCM_IF_ERROR_RETURN_MSG(bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx), "failed to get is_dnx");

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
 *      nof_links                               (OUT) - nof_links #
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 *      Currently suitable for FE. Needs to be implemented for FAP
 *
 */
int
cint_phy_loopback_on_lane_swapped_port_nof_links_get(
    int unit,
    uint32 *nof_links)
{
    int is_dnx = 0;
    int not_all_cores_are_active = 0;
    int links_count = 0;
    int nof_cores = 0;

    /*
     * Init vars according to device type (DNX/DNXF)
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx), "failed to get is_dnx");

    /*
     * Get number of links
     */
    if (is_dnx)
    {
        *nof_links = *(dnxc_data_get(unit, "fabric", "links", "nof_links", NULL));
    }
    else
    {
        links_count = *(dnxc_data_get (unit, "port", "general", "nof_links", NULL));
        not_all_cores_are_active = *(dnxc_data_get(unit, "device", "general", "is_not_all_cores_active", NULL));
        nof_cores = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));

        /* 
         * In some DNXF cases not all cores may be active be active.
         * This will affect the number of links used.
         */
        if (not_all_cores_are_active)
        {
            *nof_links = links_count / nof_cores;
        }
        else
        {
            *nof_links = links_count;
        }
    }

    return BCM_E_NONE;

}

/*
 * Function:
 *      cint_phy_loopback_on_lane_swapped_port_nof_lanes_in_pm_get
 *
 * Purpose:
 *      Since not all device cores may be active in DNXF case, number of lanes in octed will change.
 *
 * Parameters:
 *      unit                                    (IN) - unit #
 *      nof_lanes_in_pm                      (OUT) - nof_lanes_in_pm #
 *
 * Returns:
 *      BCM_E_XXX
 *
 */
int
cint_phy_loopback_on_lane_swapped_port_nof_lanes_in_pm_get(
    int unit,
    int *nof_lanes_in_pm)
{
    int is_dnx = 0;

    /* Check if device family is DNX */
    BCM_IF_ERROR_RETURN_MSG(bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx), "failed to get is_dnx");

    *nof_lanes_in_pm = 8;

    if (!is_dnx)
    {
        int not_all_cores_are_active = *(dnxc_data_get(unit, "device", "general", "is_not_all_cores_active", NULL));
        int nof_cores = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));
        if (not_all_cores_are_active)
        {
            *nof_lanes_in_pm = *nof_lanes_in_pm / nof_cores;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      cint_phy_loopback_on_lane_swapped_port_is_flexible_link_mapping_supported_get
 *
 * Purpose:
 *      Tells if the device is supporting flexible link mapping.
 *
 * Parameters:
 *      unit                                    (IN) - unit #
 *      supported                               (OUT) - supported #
 *
 * Returns:
 *      BCM_E_XXX
 *
 */
int
cint_phy_loopback_on_lane_swapped_port_is_flexible_link_mapping_supported_get(
    int unit,
    int *supported)
{
    int rv;
    int is_dnx = 0;

    /* Check if device family is DNX */
    BCM_IF_ERROR_RETURN_MSG(bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx), "failed to get is_dnx");

    if (is_dnx)
    {
        *supported = 0;
    } else {
        *supported = *(dnxc_data_get(unit, "port", "lane_map", "is_flexible_link_mapping_supported", NULL));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      cint_phy_loopback_on_lane_swapped_port_ports_to_be_modified_get
 *
 * Purpose:
 *      Given one port, this procedure returns a PBMP of all ports on the same PM.
 *
 * Parameters:
 *      unit                                    (IN) - unit #
 *      loopback_port                           (IN) - loopback_port #
 *      ports_to_be_modified                    (OUT) - ports_to_be_modified
 *
 * Returns:
 *      BCM_E_XXX
 * 
 * Notes:
 *      When loopback_port is equal to PHY_LOOPBACK_ON_ALL_PORTS, retrned PBMP will be with all SFI ports on the device.
 *
 */
int
cint_phy_loopback_on_lane_swapped_port_ports_to_be_modified_get(
    int unit,
    bcm_port_t loopback_port,
    bcm_pbmp_t *ports_to_be_modified)
{
    int nof_lanes_in_pm = 0, loopback_lane = 0, first_lane_in_octet = 0, lane_of_loopback_port = 0;
    int pm_of_loopback_port = 0;
    int port = 0;
    uint32 nof_ports = 0;
    int is_flexible_link_mapping_supported = 0;
    bcm_port_config_t config;
    char error_msg[200]={0,};

    BCM_PBMP_CLEAR(*ports_to_be_modified);

    /* Return PBMP with all SFI ports. */
    if (loopback_port == PHY_LOOPBACK_ON_ALL_PORTS)
    {
        snprintf(error_msg, sizeof(error_msg), "unit %d.", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_config_get(unit, &config), error_msg);
        *ports_to_be_modified = config.sfi;
    }
    /* Return PBMP of all ports on the given loopback_port PM. */
    else
    {
        BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_nof_lanes_in_pm_get(unit, &nof_lanes_in_pm), "");

        snprintf(error_msg, sizeof(error_msg), "unit %d.", unit);
        BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_is_flexible_link_mapping_supported_get(unit, &is_flexible_link_mapping_supported), error_msg);

        if (is_flexible_link_mapping_supported)
        {
            /* 
             * In flexible link mapping case lane mapping is done per logical port and not per lane.
             * This means that ports on a same PM may not be sequential.
             * We need to:
             * 1. Get the lane of the input loopback_port
             * 2. Find the PM
             * 3. Go over all SFI ports and calculate the rest of the ports form the same PM.
             * 4. Return a PBMP with all ports from the same PM.
             */
            BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_nof_links_get(unit, &nof_ports), "");

            bcm_port_lane_to_serdes_map_t port_to_serdes_map[nof_ports] = { {0, 0} };
            snprintf(error_msg, sizeof(error_msg), "unit %d.", unit);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_lane_to_serdes_map_get(unit, BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE, nof_ports, port_to_serdes_map), error_msg);

            /* Get the physical lane of the loopback port */
            lane_of_loopback_port = port_to_serdes_map[loopback_port].serdes_rx_id;

            /* Get the PM for this lane */
            pm_of_loopback_port = lane_of_loopback_port / nof_lanes_in_pm;

            /* Find logical port for the lanes of the PM  */
            for (port = 0; port < nof_ports; port++)
            {
                /* Check if port is from the needed PM */
                if ((port_to_serdes_map[port].serdes_rx_id / nof_lanes_in_pm) == pm_of_loopback_port)
                {
                    /* Append to result pbmp */
                    BCM_PBMP_PORT_ADD(*ports_to_be_modified, port);
                }
            }
        }
        else
        {
            /*
             * get loopback_lane and first_lane in loopback-lane octet 
             */
            snprintf(error_msg, sizeof(error_msg), "unit %d, port %d.", unit, loopback_port);
            BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_port2lane_get(unit, loopback_port, &loopback_lane), error_msg);
            first_lane_in_octet = (loopback_lane / nof_lanes_in_pm) * nof_lanes_in_pm;

            for (lane_of_loopback_port = first_lane_in_octet; lane_of_loopback_port < (first_lane_in_octet + nof_lanes_in_pm); ++lane_of_loopback_port)
            {
                snprintf(error_msg, sizeof(error_msg), "unit %d, lane %d.", unit, lane_of_loopback_port);
                BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_lane2port_get(unit, lane_of_loopback_port, &port), error_msg);
                BCM_PBMP_PORT_ADD(*ports_to_be_modified, port);
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      cint_phy_loopback_on_lane_swapped_port_detach_and_save_resourse
 *
 * Purpose:
 *     Disable , store resource information and detach a list of ports.
 *
 * Parameters:
 *      unit                                    (IN) - unit #
 *      ports_to_be_modified                    (IN) - ports_to_be_modified #
 *      ports_resources                         (IN) - ports_resources #
 *      ports_enable_state                      (IN) - ports_enable_state #
 *
 * Returns:
 *      BCM_E_XXX
 *
 */
int
cint_phy_loopback_on_lane_swapped_port_detach_and_save_resourse(
    int unit,
    bcm_pbmp_t ports_to_be_modified,
    bcm_port_resource_t *ports_resources,
    int *ports_enable_state)
{
    char error_msg[200]={0,};
    int port_idx = 0;
    int port = 0;
    bcm_pbmp_t successfully_detached_pbmp;

    BCM_PBMP_CLEAR(successfully_detached_pbmp);

    BCM_PBMP_ITER(ports_to_be_modified, port)
    {
        /* Save resource information for every port in the bitmap. */
        snprintf(error_msg, sizeof(error_msg), "unit %d, port %d.", unit, port);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_resource_get(unit, port, &ports_resources[port_idx]), error_msg);

        /* Save enabled state information for every port in the bitmap. */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_get(unit, port, &ports_enable_state[port_idx]), error_msg);
        port_idx++;
    }

    /* Detach all ports from the bitmap. */
    snprintf(error_msg, sizeof(error_msg), "unit %d.", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_detach(unit, ports_to_be_modified, &successfully_detached_pbmp), error_msg);
    return BCM_E_NONE;
}

/*
 * Function:
 *      cint_phy_loopback_on_lane_swapped_port_probe_and_restore_resourse
 *
 * Purpose:
 *     Enable , restore resource information and probe a list of ports.
 *
 * Parameters:
 *      unit                                    (IN) - unit #
 *      ports_to_be_modified                    (IN) - ports_to_be_modified #
 *      ports_resources                         (IN) - ports_resources #
 *      ports_enable_state                      (IN) - ports_enable_state #
 *
 * Returns:
 *      BCM_E_XXX
 *
 */
int
cint_phy_loopback_on_lane_swapped_port_probe_and_restore_resourse(
    int unit,
    bcm_pbmp_t ports_to_be_modified,
    bcm_port_resource_t *ports_resources,
    int *ports_enable_state)
{
    char error_msg[200]={0,};
    int port_idx = 0;
    int port = 0;
    bcm_pbmp_t successfully_attached_pbmp;

    BCM_PBMP_CLEAR(successfully_attached_pbmp);

    /* Attach all ports from the bitmap. */
    snprintf(error_msg, sizeof(error_msg), "unit %d.", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_probe(unit, ports_to_be_modified, &successfully_attached_pbmp), error_msg);

    /*
     * restore resource configuration and enable all the enabled ports in the octet of the loopback port 
     */
    BCM_PBMP_ITER(ports_to_be_modified, port)
    {
        snprintf(error_msg, sizeof(error_msg), "unit %d, port %d.", unit, port);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_resource_set(unit, port, ports_resources[port_idx]), error_msg);

        if (ports_enable_state[port_idx])
        {
            BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, port, 1), error_msg);
        }
        port_idx++;
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
 *      loopback_port                           (IN) - port # of the port on which to enable the loopback
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 *      Since the sequence contains a detach step -
 *      Configurations configured on the originally-attached ports of the octet via API/SoC properties will not be restored.
 *      After the attach step the configuration of the originally-attached ports of the octet is set to be according to default.
 *      Exceptions: the resource configurations - speed, fec_type, phy_lane_config - will be restored.
 *
 */

int
cint_phy_loopback_on_lane_swapped_port_set(
    int unit,
    bcm_port_t loopback_port)
{
    char error_msg[200]={0,};
    bcm_port_loopback_t loopback_type = BCM_PORT_LOOPBACK_PHY;
    bcm_port_config_t config;
    bcm_port_t port;

    snprintf(error_msg, sizeof(error_msg), "unit %d.", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_config_get(unit, &config), error_msg);

    /*Convert gport to port*/
    BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_gport2port_get(unit, loopback_port, &port), error_msg);

    /* verify that given logical port is member of Fabric PBMP*/
    if (!BCM_PBMP_MEMBER(config.sfi, port))
    {
        printf("cint_phy_loopback_on_lane_swapped_port_set: failed - unit %d logical port %d is not part of SFI pbmp.\n", unit, port);
        return BCM_E_PORT;
    }

    snprintf(error_msg, sizeof(error_msg), "unit %d.", unit);
    BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_set_internal(unit, port, loopback_type), error_msg);
    return BCM_E_NONE;
}


/*
 * Function:
 *      cint_phy_loopback_on_lane_swapped_port_set_internal
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
 *      loopback_port                           (IN) - port # of the port on which to enable the loopback
 *                                                     PHY_LOOPBACK_ON_ALL_PORTS(-1) may be provided as loopback_port to execute procedure on all SFI ports.
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 *      Since the sequence contains a detach step -
 *      Configurations configured on the originally-attached ports of the octet via API/SoC properties will not be restored.
 *      After the attach step the configuration of the originally-attached ports of the octet is set to be according to default.
 *      Exceptions: the resource configurations - speed, fec_type, phy_lane_config - will be restored.
 *
 */ 
int
cint_phy_loopback_on_lane_swapped_port_set_internal(
    int unit,
    bcm_port_t loopback_port,
    bcm_port_loopback_t loopback_type)
{
    char error_msg[200]={0,};
    int port, lane;
    uint32 nof_ports = -1;

    BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_nof_links_get(unit, &nof_ports), "");

    bcm_port_resource_t ports_resources[nof_ports];
    bcm_pbmp_t ports_to_be_modified;
    int ports_enable_state[nof_ports] = { 0 };
    bcm_port_lane_to_serdes_map_t port_to_serdes_map[nof_ports] = { {0, 0} };

    /*
     * get default lane-mapping of all the lanes 
     */
    snprintf(error_msg, sizeof(error_msg), "unit %d.", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_lane_to_serdes_map_get(unit, BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE, nof_ports, port_to_serdes_map), error_msg);

    /*
     * Get a PBMP of all ports that need lane mapping allignment and will be modified.
     *  - If loopback is modified for a single port - result will be all ports of its PM
     *  - If loopback is modified on all ports - result will be all SFI ports.
     */
    snprintf(error_msg, sizeof(error_msg), "unit %d, port %d", unit, loopback_port);
    BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_ports_to_be_modified_get(unit, loopback_port, ports_to_be_modified), error_msg);

    /* 
     * We take the first port from the bitmap and get its corresponding lane.
     * This will help us to check bellow if this PM was already aligned.
     * 
     * Note: In flexible mapping case the returned lane will be the actual logical port.
     * This is expected and OK since we are indexing the lane to serdes map per port.
     */
    BCM_PBMP_ITER(ports_to_be_modified, port)
    {
        snprintf(error_msg, sizeof(error_msg), "unit %d, port %d.", unit, port);
        BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_port2lane_get(unit, port, &lane), error_msg);
        break;
    }
    /*
     * if we already configured one loopback in this octet - swap was already aligned -> nothing to do the check is if
     * the lane mapping was already stored.
     * 
     * Note: This step is skipped when we modify the loopback on all ports.
     */
    if ((lane_to_serdes_map_orig[lane].serdes_tx_id != 0 || lane_to_serdes_map_orig[lane + 1].serdes_tx_id != 0) && loopback_port != PHY_LOOPBACK_ON_ALL_PORTS)
    {
        snprintf(error_msg, sizeof(error_msg), "unit %d, port %d.", unit, loopback_port);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, loopback_port, loopback_type), error_msg);
    }
    else
    {
        /*
         * 1. get the original resource configuration and enable state of all the attached ports from the provided PBMP 
         * 2. detach all the attached ports rom the provided PBMP 
         */
        snprintf(error_msg, sizeof(error_msg), "unit %d, port %d.", unit, loopback_port);
        BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_detach_and_save_resourse(unit, ports_to_be_modified, ports_resources, ports_enable_state), error_msg);

        /*
         * 3. Keep the original TX lane mapping and
         * 4. Allign it with the corresponding RX lane.
         */
        BCM_PBMP_ITER(ports_to_be_modified, port)
        {
            snprintf(error_msg, sizeof(error_msg), "unit %d, port %d.", unit, port);
            BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_port2lane_get(unit, port, &lane), error_msg);

            lane_to_serdes_map_orig[lane].serdes_tx_id = port_to_serdes_map[lane].serdes_tx_id;
            if (port_to_serdes_map[lane].serdes_tx_id != BCM_PORT_LANE_TO_SERDES_NOT_MAPPED)
            {
                port_to_serdes_map[lane].serdes_tx_id = port_to_serdes_map[lane].serdes_rx_id;
            }
        }

        /* 5. Apply new lane mapping. */
        snprintf(error_msg, sizeof(error_msg), "unit %d, port %d.", unit, loopback_port);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_lane_to_serdes_map_set(unit, BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE, nof_ports, port_to_serdes_map), error_msg);

        /* 
         * 6. Attach all ports from the given PBMP.
         * 7. Restore original resource configuration and enabled state for the ports in the given PBMP 
         */
        BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_probe_and_restore_resourse(unit, ports_to_be_modified, ports_resources, ports_enable_state), error_msg);

        /* Set loopback for all ports or for a single port, based on the provided input. */
        if (loopback_port != PHY_LOOPBACK_ON_ALL_PORTS)
        {
            BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, loopback_port, loopback_type), error_msg);
        }
        else
        {
            BCM_PBMP_ITER(ports_to_be_modified, port)
            {
                snprintf(error_msg, sizeof(error_msg), "unit %d, port %d.", unit, port);
                BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, port, loopback_type), error_msg);
            }
        }
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
 *
 */
int
cint_phy_loopback_on_lane_swapped_port_set_all(
    int unit)
{
    char error_msg[200]={0,};
    /* Perform procedure for all SFI ports.*/
    int port = PHY_LOOPBACK_ON_ALL_PORTS;
    bcm_port_loopback_t loopback_type = BCM_PORT_LOOPBACK_PHY;

    snprintf(error_msg, sizeof(error_msg), "unit %d.", unit);
    BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_set_internal(unit, port, loopback_type), error_msg);

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
 * Notes:
 *      Since the sequence contains a detach step -
 *      Configurations configured on the originally-attached ports of the octet via API/SoC properties will not be restored.
 *      After the attach step the configuration of the originally-attached ports of the octet is set to be according to default.
 *      Exceptions: resource configuration - speed, fec_type, phy_lane_config - will be restored.
 *
 */
int
cint_phy_loopback_on_lane_swapped_port_clear(
    int unit,
    bcm_port_t loopback_port)
{
    char error_msg[200]={0,};
    bcm_port_config_t config;
    bcm_port_t port;

    snprintf(error_msg, sizeof(error_msg), "unit %d.", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_config_get(unit, &config), error_msg);

    /*Convert gport to port*/
    BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_gport2port_get(unit, loopback_port, &port), error_msg);

    /* verify that given logical port is member of Fabric PBMP*/
    if (!BCM_PBMP_MEMBER(config.sfi, port))
    {
        printf("cint_phy_loopback_on_lane_swapped_port_set: failed - unit %d logical port %d is not part of SFI pbmp.\n", unit, port);
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_clear_internal(unit, port), error_msg);

    return BCM_E_NONE;
}

int
cint_phy_loopback_on_lane_swapped_port_clear_internal(
    int unit,
    bcm_port_t loopback_port)
{
    char error_msg[200]={0,};
    int port, lane;
    int loopback;
    uint32 nof_ports = -1;

    BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_nof_links_get(unit, &nof_ports), "");

    bcm_port_resource_t ports_resources[nof_ports];
    bcm_pbmp_t ports_to_be_modified;
    int ports_enable_state[nof_ports] = { 0 };
    bcm_port_lane_to_serdes_map_t port_to_serdes_map[nof_ports] = { {0, 0} };

    /*
     * Get a PBMP of all ports that need lane mapping allignment and will be modified.
     *  - If loopback is modified for a single port - result will be all ports of its PM
     *  - If loopback is modified on all ports - result will be all SFI ports.
     */
    snprintf(error_msg, sizeof(error_msg), "unit %d, port %d.", unit, loopback_port);
    BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_ports_to_be_modified_get(unit, loopback_port, ports_to_be_modified), error_msg);

    if (loopback_port != PHY_LOOPBACK_ON_ALL_PORTS)
    {
        /*
        * Remove the loopback for the given port
        */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, loopback_port, BCM_PORT_LOOPBACK_NONE), error_msg);

        /*
         * If another PHY or PHY Remote loopback is configured in the octet - no need to restore the original lane mapping. lane mapping
         * will be restored only when last PHY or PHY Remote loopback of the octet is removed 
         */
        BCM_PBMP_ITER(ports_to_be_modified, port)
        {
            snprintf(error_msg, sizeof(error_msg), "unit %d, port %d.", unit, port);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_get(unit, port, &loopback), error_msg);
            if (loopback == BCM_PORT_LOOPBACK_PHY || loopback == BCM_PORT_LOOPBACK_PHY_REMOTE)
            {
                return BCM_E_NONE;
            } 
        }
    }
    else
    {
        /* Remove loopback on all ports form the given bitmap. */
        BCM_PBMP_ITER(ports_to_be_modified, port)
        {
            snprintf(error_msg, sizeof(error_msg), "unit %d, port %d.", unit, port);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_NONE), error_msg);
        }
    }

    /*
     * get default lane-mapping of all the ports 
     */
    snprintf(error_msg, sizeof(error_msg), "unit %d, port %d.", unit, loopback_port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_lane_to_serdes_map_get(unit, BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE, nof_ports, port_to_serdes_map), error_msg);

    /*
     * 1. get the original resource configuration and enable state of all the attached ports from the provided PBMP 
     * 2. detach all the attached ports rom the provided PBMP 
     */
    BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_detach_and_save_resourse(unit, ports_to_be_modified, ports_resources, ports_enable_state), error_msg);

    /*
     * 3. Restore original Tx lane mapping of the octet of the loopback port (for originally lane-mapped ports) 
     */
    BCM_PBMP_ITER(ports_to_be_modified, port)
    {
        snprintf(error_msg, sizeof(error_msg), "unit %d, port %d.", unit, port);
        BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_port2lane_get(unit, port, &lane), error_msg);

        if (port_to_serdes_map[lane].serdes_tx_id != BCM_PORT_LANE_TO_SERDES_NOT_MAPPED)
        {
            port_to_serdes_map[lane].serdes_tx_id = lane_to_serdes_map_orig[lane].serdes_tx_id;
            lane_to_serdes_map_orig[lane].serdes_tx_id = 0;
        }
    }

    snprintf(error_msg, sizeof(error_msg), "unit %d, port %d.", unit, loopback_port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_lane_to_serdes_map_set(unit, BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE, nof_ports, port_to_serdes_map), error_msg);

    /*
     * 4. Attach all originally-attached ports in the given PBMP 
     */
    BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_probe_and_restore_resourse(unit, ports_to_be_modified, ports_resources, ports_enable_state), error_msg);

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
 *
 */
int
cint_phy_loopback_on_lane_swapped_port_clear_all(
    int unit)
{
    char error_msg[200]={0,};
    int loopback_port = PHY_LOOPBACK_ON_ALL_PORTS;
    snprintf(error_msg, sizeof(error_msg), "unit %d.", unit);
    BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_clear_internal(unit, loopback_port), error_msg);

    return BCM_E_NONE;
}

/*
 * Function:
 *      cint_phy_remote_loopback_on_lane_swapped_port_set
 *
 * Purpose:
 *      configure PHY Remote loopback on a port that is lane-swapped (lane to SerDes Tx mapping is not 1:1).
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
 *      loopback_port                           (IN) - port # of the port on which to enable the loopback
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 *      Since the sequence contains a detach step -
 *      Configurations configured on the originally-attached ports of the octet via API/SoC properties will not be restored.
 *      After the attach step the configuration of the originally-attached ports of the octet is set to be according to default.
 *      Exceptions: the resource configurations - speed, fec_type, phy_lane_config - will be restored.
 *
 */

int
cint_phy_remote_loopback_on_lane_swapped_port_set(
    int unit,
    bcm_port_t loopback_port)
{
    char error_msg[200]={0,};
    bcm_port_loopback_t loopback_type = BCM_PORT_LOOPBACK_PHY_REMOTE;
    bcm_port_config_t config;
    bcm_port_t port;

    snprintf(error_msg, sizeof(error_msg), "unit %d.", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_config_get(unit, &config), error_msg);

    /*Convert gport to port*/
    BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_gport2port_get(unit, loopback_port, &port), error_msg);

    /* verify that given logical port is member of Fabric PBMP*/
    if (!BCM_PBMP_MEMBER(config.sfi, port))
    {
        printf("cint_phy_loopback_on_lane_swapped_port_set: failed - unit %d logical port %d is not part of SFI pbmp.\n", unit, port);
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_set_internal(unit, port, loopback_type), error_msg);
    return BCM_E_NONE;
}

/*
 * Function:
 *      cint_phy_remote_loopback_on_lane_swapped_port_set_all
 *
 * Purpose:
 *      Perform cint_phy_loopback_on_lane_swapped_port_set on all the fabric ports of the device
 *
 * Parameters:
 *      unit                                    (IN) - unit #
 *
 * Returns:
 *      BCM_E_XXX
 *
 */
int
cint_phy_remote_loopback_on_lane_swapped_port_set_all(
    int unit)
{
    char error_msg[200]={0,};
    /* Perform procedure for all SFI ports.*/
    int port = PHY_LOOPBACK_ON_ALL_PORTS;
    bcm_port_loopback_t loopback_type = BCM_PORT_LOOPBACK_PHY_REMOTE;

    snprintf(error_msg, sizeof(error_msg), "unit %d.", unit);
    BCM_IF_ERROR_RETURN_MSG(cint_phy_loopback_on_lane_swapped_port_set_internal(unit, port, loopback_type), error_msg);

    return BCM_E_NONE;
}