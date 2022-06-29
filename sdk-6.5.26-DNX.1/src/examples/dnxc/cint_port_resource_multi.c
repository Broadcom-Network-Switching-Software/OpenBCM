/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:        cint_port_resource_multi.c
 *
 * Purpose:
 *
 * Notes:
 *              1) Relevant functions for user:
 *                  - cint_port_resource_multi_speed_set
 *
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
 *
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
 *
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
    int fabric_pam4_to_nrz_speed_transition[array_size] = {0};
    uint32 flags = 0;
    bcm_port_t port;

    for (port_idx = 0; port_idx < array_size; port_idx++)
    {

        bcm_port_resource_t_init(&port_resources[port_idx]);

        port_resources[port_idx].port = ports_array[port_idx];
        port_resources[port_idx].speed = speeds_array[port_idx];

        rv = cint_fabric_speed_transition_pam4_nrz_get(unit, port_resources[port_idx].port, port_resources[port_idx].speed, &fabric_pam4_to_nrz_speed_transition[port_idx]);
        if (BCM_FAILURE(rv))
        {
            printf("cint_fabric_speed_transition_pam4_nrz_get failed.\n");
            return rv;
        }

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
        /** If we go from PAM4 to NRZ, we need to disable link tranining.
         * This is needed in order to disable TX_PAM4_PRECODER_ENABLE in the next step.
         * It should not be enabled for NRZ.
         */
        port_resources[port_idx].link_training = fabric_pam4_to_nrz_speed_transition[port_idx] ? 0 : port_resources[port_idx].link_training;

        /*
         * For now phy_lane_config won't be implemented as control variable
         */
        port_resources[port_idx].phy_lane_config = BCM_PORT_RESOURCE_DEFAULT_REQUEST;

        /*
         * Get default values for the ports given the params passed
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

    
    /*
     * Relevant for fabric ports only
     */
    for (port_idx = 0; port_idx < array_size; port_idx++)
    {
        if (fabric_pam4_to_nrz_speed_transition[port_idx])
        {
            rv = bcm_port_phy_control_set(unit, ports_array[port_idx], BCM_PORT_PHY_CONTROL_TX_PAM4_PRECODER_ENABLE, 0);
            if (BCM_FAILURE(rv))
            {
                printf("bcm_port_phy_control_set failed to disable TX_PAM4_PRECODER_ENABLE for port %d.\n", ports_array[port_idx]);
                return rv;
            }

            /*
             * We had to disable LT to remove the PAM4 precoder
             * If LT is not as user requested - update it
             */
            if (port_resources[port_idx].link_training != link_training_array[port_idx])
            {
                port_resources[port_idx].link_training  = link_training_array[port_idx];
                /*
                 * Get default values for the ports given the params passed. Link training might be passed here as -1/BCM_PORT_RESOURCE_DEFAULT_REQUEST
                 */
                rv = bcm_port_resource_default_get(unit, ports_array[port_idx], flags, &port_resources[port_idx]);
                if (BCM_FAILURE(rv))
                {
                    printf("bcm_port_resource_default_get failed.\n");
                    return rv;
                }

                rv = bcm_port_resource_set(unit, port_resources[port_idx].port, port_resources[port_idx]);
                if (BCM_FAILURE(rv))
                {
                    printf("bcm_port_resource_multi_set failed.\n");
                    return rv;
                }
            }
        }
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
 *
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
    int pam4_to_nrz_speed_transition;

    /*
     * Check if we are moving from PAM4 to NRZ speed.
     * Getting this information from the first Fabric port.
     */
    BCM_PBMP_ITER(pbmp, port)
    {
        rv = cint_fabric_speed_transition_pam4_nrz_get(unit, port, speed, &pam4_to_nrz_speed_transition);
        if (BCM_FAILURE(rv))
        {
            printf("cint_port_resource_multi_params_set failed.\n");
            return rv;
        }
        break;
    }

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
            /** If we go from PAM4 to NRZ, we need to disable link tranining.
             * This is needed in order to disable TX_PAM4_PRECODER_ENABLE in the next step.
             * It should not be enabled for NRZ.
             */
            link_training_ar[port_idx] = (pam4_to_nrz_speed_transition && link_training) ? 0 : link_training;
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

    if (pam4_to_nrz_speed_transition)
    {
        printf("Disable TX PAM4 precoder.\n");
        port_idx = 0;
        BCM_PBMP_ITER(pbmp, port)
        {
            /** Disable TX_PAM4_PRECODER */
            rv = bcm_port_phy_control_set(unit, port, BCM_PORT_PHY_CONTROL_TX_PAM4_PRECODER_ENABLE, 0);
            if (BCM_FAILURE(rv))
            {
                printf("bcm_port_phy_control_set failed to disable TX_PAM4_PRECODER_ENABLE for port %d.\n", port);
                return rv;
            }
            /** Prepare link training to be enabled back if needed. */
            link_training_ar[port_idx] = link_training;
            port_idx++;
        }
        if (link_training)
        {
            /** Enable back link training. */
            rv = cint_port_resource_multi_params_set(unit, ports_array, speed_ar,  fec_ar , link_training_ar , count);
            if (BCM_FAILURE(rv))
            {
                printf("bcm_port_resource_multi_set failed.\n");
                return rv;
            }
        }
    }
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
    printf("cint_all_fabric_port_resource_multi_params_set: PASS - unit %d.\n", unit);
    return BCM_E_NONE;
}


/*
 * Function:
 *      cint_fabric_speed_transition_pam4_nrz_get
 *
 * Purpose:
 *      This cint is checking if we are moving from PAM4 to NRZ speed.
 * Parameters:
 *      unit                                    (IN) - unit #
 *      port                                    (IN) - port #
 *      new_speed                               (IN) - new_speed # new target speed
 *      is_transition                           (OUT) - is_transition # indicates of there is transition.
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 *
 */
int
cint_fabric_speed_transition_pam4_nrz_get(
    int unit,
    bcm_port_t port,
    int new_speed,
    int *is_transition)
{
    int rv;
    int is_current_speed_pam4 = 0;
    int is_new_speed_nrz = 0;
    bcm_port_resource_t resource;
    bcm_port_config_t config;
    int is_dnx, fabric_port_base;

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_device_member_get failed to get is_dnx\n", unit);
        return rv;
    }

    *is_transition = 0;

    rv = bcm_port_config_get(unit,&config);
    if (BCM_FAILURE(rv)) {
        printf("Error, in bcm_port_config_get, rv=%d, \n", rv);
        return rv;
    }

    if (is_dnx && BCM_GPORT_IS_LOCAL_FABRIC(port))
    {
        fabric_port_base = *(dnxc_data_get(unit, "port", "general", "fabric_port_base", NULL));
        port = fabric_port_base + BCM_GPORT_LOCAL_FABRIC_GET(port);
    }

    if (BCM_PBMP_MEMBER(config.sfi, port)) {
        bcm_port_resource_t_init(&resource);
        rv = bcm_port_resource_get(unit, port, &resource);
        if (BCM_FAILURE(rv))
        {
            printf("bcm_port_resource_get failed for port %d.\n", port);
            return rv;
        }

        is_current_speed_pam4 = (resource.speed > 28125) ? 1 : 0;
        is_new_speed_nrz = (new_speed <= 28125) ? 1 : 0;

        if(is_current_speed_pam4 && is_new_speed_nrz)
        {
            *is_transition = 1;
        }
        else
        {
            *is_transition = 0;
        }
    }

    return BCM_E_NONE;
}
