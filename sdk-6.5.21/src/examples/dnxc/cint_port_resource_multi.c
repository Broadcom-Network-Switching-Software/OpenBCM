/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_port_resource_multi.c
 *
 * Purpose:
 *
 * Notes:
 *              1) Relevant functions for user:
 *                  - cint_port_resource_multi_speed_set
 */

/*
 * Function:
 *      cint_port_resource_multi_set
 *
 * Purpose:
 *      Set port resource values to many ports at once.
 *      This is done by the following sequence:
 *
 *      1) Get default parameters for all port resources that are not specifically given
 *      2) Use bcm_port_resource_multi_set to configure all ports at once
 *
 * Parameters:
 *      unit                                    (IN) - unit #
 *      ports_array                             (IN) - ports_array # array of ports which will be modified
 *      port_resources                          (IN) - port_resources # array of resources to be configured
 *      array_size                              (IN) - array_size # number of ports to be modified
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 */
int
cint_port_resource_multi_set(
    int unit,
    bcm_port_t * ports_array,
    bcm_port_resource_t * port_resources,
    int array_size)
{
    int rv;
    int port_idx;
    uint32 flags = 0;
    bcm_port_t port;

    /*
     * Set all ports at once
     */
    rv = bcm_port_resource_multi_set(unit, array_size, port_resources);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_resource_multi_set failed.\n");
        return rv;
    }

    printf("cint_port_resource_multi_set: PASS - unit %d.\n", unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      cint_port_resource_multi_params_set
 *
 * Purpose:
 *      Set port resource SPEED values to many ports at once.
 *      This is done by the following sequence:
 *
 *      1) Get default parameters for all port resources for the speed provided.
 *      2) Use bcm_port_resource_multi_set to configure all ports at once
 *
 * Parameters:
 *      unit                                    (IN) - unit #
 *      ports_array                             (IN) - ports_array # array of ports which will be modified
 *      speeds_array                            (IN) - speeds_array # arrayof speeds for each port to be set
 *      array_size                              (IN) - array_size # number of ports to be modified
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 */
int
cint_port_resource_multi_params_set(
    int unit,
    bcm_port_t * ports_array,
    int *speeds_array,
    bcm_port_phy_fec_t * fec_type_array,
    int *link_training_array,
    int array_size)
{
    int rv;
    int port_idx;
    bcm_port_resource_t port_resources[array_size];
    uint32 flags = 0;
    bcm_port_t port;

    for (port_idx = 0; port_idx < array_size; port_idx++)
    {
        bcm_port_resource_t_init(&port_resources[port_idx]);

        port_resources[port_idx].port = ports_array[port_idx];
        port_resources[port_idx].speed = speeds_array[port_idx];

        if (fec_type_array[port_idx] != BCM_PORT_RESOURCE_DEFAULT_REQUEST)
        {
            port_resources[port_idx].fec_type = fec_type_array[port_idx];
        }
        else
        {
            port_resources[port_idx].fec_type = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
        }

        if (link_training_array[port_idx] != BCM_PORT_RESOURCE_DEFAULT_REQUEST)
        {
            port_resources[port_idx].link_training = link_training_array[port_idx];
        }
        else
        {
            port_resources[port_idx].link_training = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
        }

        /*
         * For now phy_lane_config won't be implemented as control variable
         */
        port_resources[port_idx].phy_lane_config = BCM_PORT_RESOURCE_DEFAULT_REQUEST;

        /*
         * Get default values for the ports gicing the speed passed
         */
        rv = bcm_port_resource_default_get(unit, ports_array[port_idx], flags, &port_resources[port_idx]);
        if (BCM_FAILURE(rv))
        {
            printf("bcm_port_resource_default_get failed.\n");
            return rv;
        }
    }

    /*
     * Set all ports at once
     */
    rv = bcm_port_resource_multi_set(unit, array_size, port_resources);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_resource_multi_set failed.\n");
        return rv;
    }

    printf("cint_port_resource_multi_params_set: PASS - unit %d.\n", unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      cint_all_fabric_port_resource_multi_params_set
 *
 * Purpose:
 *      This cint is a wrapper to cint_port_resource_set.
 *      It sets configuration to all fabric ports on the device.
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
 */
int
cint_all_fabric_port_resource_multi_params_set(
    int unit,
    int speed,
    bcm_port_phy_fec_t fec,
    int link_training)
{
    int rv;
    int port_idx = 0;
    bcm_port_config_t config;
    bcm_port_t port;

    int count;
    bcm_pbmp_t pbmp, pbmp_detach_attach;;

    BCM_PBMP_CLEAR(pbmp);

    /*get sfi pbmp*/
    rv = bcm_port_config_get(unit,&config);
    if (BCM_FAILURE(rv)) {
        printf("Error, in bcm_port_config_get, rv=%d, \n", rv);
        return rv;
    }

    BCM_PBMP_ASSIGN(pbmp, config.sfi);
    BCM_PBMP_COUNT(pbmp, count);

    /* Create config arrays */
    bcm_port_t ports_array[count];
    int speed_ar[count];
    int fec_ar[count] ;
    int link_training_ar[count];
    bcm_port_resource_t port_resources[count];

    
    rv = bcm_port_detach(unit, pbmp, &pbmp_detach_attach);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_detach failed.\n");
        return rv;
    }
    sal_sleep(1);
    rv = bcm_port_probe(unit, pbmp, &pbmp_detach_attach);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_probe failed.\n");
        return rv;
    }
    sal_sleep(1);

    /*
     * Configure all ports and prepare arrays
     */
    BCM_PBMP_ITER(pbmp, port)
    {
        ports_array[port_idx] = port;
        speed_ar[port_idx] = speed;

        if (fec != BCM_PORT_RESOURCE_DEFAULT_REQUEST)
        {
            fec_ar[port_idx] = fec;
        }
        else
        {
            fec_ar[port_idx]= BCM_PORT_RESOURCE_DEFAULT_REQUEST;
        }

        if (link_training != BCM_PORT_RESOURCE_DEFAULT_REQUEST)
        {
            link_training_ar[port_idx] = link_training;
        }
        else
        {
            link_training_ar[port_idx]  = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
        }
        port_idx++;
    }

    rv = cint_port_resource_multi_params_set(unit, ports_array, speed_ar,  fec_ar , link_training_ar , count);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_resource_multi_set failed.\n");
        return rv;
    }
    sal_sleep(1);
    /*
     * Enable all ports again with new configuration
     */
    BCM_PBMP_ITER(pbmp, port)
    {
        rv = bcm_port_enable_set(unit, port, 1);
        if (BCM_FAILURE(rv))
        {
            printf("bcm_port_enable_set failed for port %d.\n", port);
            return rv;
        }
    }
    printf("cint_all_port_resource_multi_params_set: PASS - unit %d.\n", unit);
    return BCM_E_NONE;
}
