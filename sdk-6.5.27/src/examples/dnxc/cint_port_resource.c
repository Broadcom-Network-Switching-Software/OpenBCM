/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:        cint_port_resource_set.c
 *
 * Purpose:
 *
 * Notes:
 *              1) Relevant functions for user:
 *                  - cint_port_resource_set
 *                  - cint_fabric_port_resource_set
 *
 */

/*
 * Function:
 *      cint_port_resource_set
 *
 * Purpose:
 *      configure port resource values(speed, link_training, fec) to an array of ports:
 *
 *      1) Detach and reattach all of the provided ports.
 *      2) Get default port resource values for the proveded speed, fec and link_training.
 *      3) Set the port resource values to all the ports.
 *      4) Enable the prots.
 *
 * Parameters:
 *      unit                                    (IN) - unit #
 *      ports_array                             (IN) - ports_array # array of ports which will be modified
 *      speed                                   (IN) - speed # speed to be set
 *      fec                                     (IN) - fec # fec to be set
 *      link_training                           (IN) - link_training # link training vlaue to be set
 *      nof_ports                               (IN) - nof_ports # number of ports to be modified
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 *
 */
int
cint_port_resource_set(
    int unit,
    bcm_port_t * ports_array,
    int speed,
    bcm_port_phy_fec_t fec,
    int link_training,
    int nof_ports)
{
    char error_msg[200]={0,};
    int port_idx;
    bcm_port_resource_t port_resource;
    uint32 flags = 0;
    bcm_port_t port;
    bcm_pbmp_t pbmp, pbmp_detach_attach;

    BCM_PBMP_CLEAR(pbmp);
    /*
     * Convert to PBMP for convenience
     */
    for (port_idx = 0; port_idx < nof_ports; port_idx++)
    {
        BCM_PBMP_PORT_ADD(pbmp, ports_array[port_idx]);
    }

    /*
     * Detach and reattach all ports to avoid PLL conflicts when we set the speed
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_detach(unit, pbmp, &pbmp_detach_attach), "");
    sal_sleep(1);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_probe(unit, pbmp, &pbmp_detach_attach), "");
    sal_sleep(1);

    /*
     * Get a default port resource configuration for the speed, fec and link training values provided
     */
    bcm_port_resource_t_init(&port_resource);
    port_resource.speed = speed;
    /*
     * Configure link training and fec_type
     */
    port_resource.fec_type = fec;
    port_resource.link_training = link_training;

    BCM_IF_ERROR_RETURN_MSG(bcm_port_resource_default_get(unit, ports_array[0], flags, &port_resource), "");

    /*
     * Configure all ports
     */
    BCM_PBMP_ITER(pbmp, port)
    {
        port_resource.port = port;
        snprintf(error_msg, sizeof(error_msg), "failed for port %d.", port);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_resource_set(unit, port, port_resource), error_msg);
    }

    sal_sleep(1);
    /*
     * Enable all ports again with new configuration
     */
    /*
     * Configure all ports
     */
    BCM_PBMP_ITER(pbmp, port)
    {
        snprintf(error_msg, sizeof(error_msg), "failed for port %d.", port);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, port, 1), error_msg);
    }
    printf("cint_port_resource_set: PASS - unit %d.\n", unit);
    return BCM_E_NONE;
}


/*
 * Function:
 *      cint_fabric_port_resource_set
 *
 * Purpose:
 *      configure port resource values(speed, link_training, fec) to all fabric ports:
 *
 *      1) Detach and reattach all of the provided ports.
 *      2) Get default port resource values for the proveded speed, fec and link_training.
 *      3) Set the port resource values to all the ports.
 *      4) Enable the prots.
 *
 * Parameters:
 *      unit                                    (IN) - unit #
 *      speed                                   (IN) - speed # speed to be set
 *      fec                                     (IN) - fec # fec to be set
 *      link_training                           (IN) - link_training # link training vlaue to be set
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 *
 */
int
cint_fabric_port_resource_set(
    int unit,
    int speed,
    bcm_port_phy_fec_t fec,
    int link_training)
{
    char error_msg[200]={0,};
    bcm_port_resource_t port_resource;
    uint32 flags = 0;
    bcm_port_config_t config;
    bcm_port_t port;
    bcm_gport_t first_port;
    bcm_pbmp_t pbmp, pbmp_detach_attach;

    BCM_PBMP_CLEAR(pbmp);

    /*get sfi pbmp*/
    BCM_IF_ERROR_RETURN_MSG(bcm_port_config_get(unit,&config), "");

    BCM_PBMP_ASSIGN(pbmp, config.sfi);

    /*
     * Get a default port resource configuration for the speed, fec and link training values provided
     */
    bcm_port_resource_t_init(&port_resource);
    port_resource.speed = speed;
    /*
     * Configure link training and fec_type
     */
    port_resource.fec_type = fec;
    port_resource.link_training = link_training;

    /*
     * Configure all ports
     */
    BCM_PBMP_ITER(pbmp, port)
    {
        port_resource.phy_lane_config = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
        port_resource.port = port;
        BCM_IF_ERROR_RETURN_MSG(bcm_port_resource_default_get(unit, port, flags, &port_resource), "");

        snprintf(error_msg, sizeof(error_msg), "failed for port %d.", port);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_resource_set(unit, port, port_resource), error_msg);
    }

    sal_sleep(1);
    /*
     * Enable all ports again with new configuration
     */
    /*
     * Configure all ports
     */
    BCM_PBMP_ITER(pbmp, port)
    {
        snprintf(error_msg, sizeof(error_msg), "failed for port %d.", port);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, port, 1), error_msg);
    }
    printf("cint_port_resource_set: PASS - unit %d.\n", unit);
    return BCM_E_NONE;
}
