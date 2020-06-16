/* $Id:
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_rx_trap_1588_traps.c
 * Purpose: Shows an example for configuration of 1588 traps.
 *
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_1588_traps.c
 *
 * Main Function:
 *      cint_rx_trap_1588_traps_main(unit, trap_type, in_port, out_port, fwd_strength);
 * Destroy Function:
 *      cint_rx_trap_1588_traps_destroy(unit, trap_type);
 *
 *
 *
 * Example Config:
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dnx/traps/cint_utils_rx.c
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_1588_traps.c
 * cint
 * cint_rx_trap_1588_traps_main(0, bcmRxTrap1588Discard, 13, 0, 0);
 */

int use_default_profile = 0;

/**
* \brief
*  Function that sets-up the bcmRxTrap1588Discard configuration.
* \par DIRECT INPUT:
*   \param [in] unit           - unit Id
*   \param [in] in_port       - in port
*
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
cint_rx_trap_1588_traps_drop_set(
    int unit,
    bcm_port_t in_port)
{
    int rv = BCM_E_NONE, trap_id = -1, device_is_jericho2;
    bcm_port_timesync_config_t timesync_config;
    bcm_rx_trap_config_t trap_config;

    bcm_port_timesync_config_t_init(timesync_config);
    timesync_config.flags = BCM_PORT_TIMESYNC_DEFAULT | BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP;
    timesync_config.pkt_drop = 1;
    timesync_config.pkt_tocpu = 0;
    
    rv = bcm_port_timesync_config_set(unit, in_port, 1, &timesync_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_port_timesync_config_set\n");
        return rv;
    }

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, is_device_jericho2, device_is_jericho2\n");
        return rv;
    }

    if (!device_is_jericho2)
    {
        /* Set the trap */
        bcm_rx_trap_config_t_init(&trap_config);
        trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
        trap_config.trap_strength = 7;
        trap_config.dest_port = BCM_GPORT_BLACK_HOLE;
        rv = cint_utils_rx_trap_create_and_set(unit, 0, bcmRxTrap1588Discard, &trap_config, &trap_id);
        if (rv != BCM_E_NONE) 
        {
            printf("Error, in cint_utils_rx_trap_create_and_set \n");
            return rv;
        }     
    }

    return rv;
}

/**
* \brief
*  Function that sets-up the bcmRxTrap1588Accepted configuration.
* \par DIRECT INPUT:
*   \param [in] unit                - unit Id
*   \param [in] in_port            - in port
*   \param [in] fwd_strength    - trap forward strength
*
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
cint_rx_trap_1588_traps_fwd_set(
    int unit,
    bcm_port_t in_port,
    int fwd_strength)
{
    int rv = BCM_E_NONE, trap_id = -1;
    bcm_port_timesync_config_t timesync_config;
    bcm_rx_trap_config_t trap_config;

    bcm_port_timesync_config_t_init(timesync_config);
    timesync_config.flags = BCM_PORT_TIMESYNC_DEFAULT | BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP;
    timesync_config.pkt_drop = 0;
    timesync_config.pkt_tocpu = 0;
    
    rv = bcm_port_timesync_config_set(unit, in_port, 1, &timesync_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_port_timesync_config_set\n");
        return rv;
    }

    /* Set the trap */
    bcm_rx_trap_config_t_init(&trap_config);    
    trap_config.trap_strength = fwd_strength;
    rv = cint_utils_rx_trap_create_and_set(unit, 0, bcmRxTrap1588Accepted, &trap_config, &trap_id);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in cint_utils_rx_trap_create_and_set \n");
        return rv;
    } 
    
    return rv;
}

/**
* \brief
*  Function that sets-up the bcmRxTrap1588\User# configuration.
* \par DIRECT INPUT:
*   \param [in] unit                - unit Id
*   \param [in] trap_type        - bcmRxTrap1588XXX trap type
*   \param [in] in_port            - in port
*   \param [in] out_port          - out port
*   \param [in] fwd_strength    - trap forward strength
*
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
cint_rx_trap_1588_traps_cpu_set(
    int unit,
    bcm_rx_trap_t trap_type,
    bcm_port_t in_port,
    bcm_port_t out_port,
    int fwd_strength)
{
    int rv = BCM_E_NONE, trap_id = -1;
    bcm_port_timesync_config_t timesync_config;
    bcm_rx_trap_config_t trap_config;

    /* Set the trap */
    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;    
    trap_config.trap_strength = fwd_strength;
    trap_config.dest_port = out_port;
    rv = cint_utils_rx_trap_create_and_set(unit, 0, trap_type, &trap_config, &trap_id);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in cint_utils_rx_trap_create_and_set \n");
        return rv;
    } 

    bcm_port_timesync_config_t_init(timesync_config);
    timesync_config.flags = use_default_profile?BCM_PORT_TIMESYNC_DEFAULT:(BCM_PORT_TIMESYNC_DEFAULT | BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP);
    timesync_config.pkt_drop = 0;
    timesync_config.pkt_tocpu = 1;
    if (trap_type != bcmRxTrap1588)
    {
        timesync_config.user_trap_id = trap_id;
    }
    
    rv = bcm_port_timesync_config_set(unit, in_port, 1, &timesync_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_port_timesync_config_set\n");
        return rv;
    }
    
    return rv;
}

/**
* \brief
*  Set either accept or discard 1588 traps.
* \par DIRECT INPUT:
*   \param [in] unit                - unit Id
*   \param [in] trap_type        - bcmRxTrap1588XXX trap type
*   \param [in] in_port            - in port
*   \param [in] out_port          - out port
*   \param [in] fwd_strength    - trap forward strength
*
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
cint_rx_trap_1588_traps_main(
    int unit,
    bcm_rx_trap_t trap_type,
    bcm_port_t in_port,
    bcm_port_t out_port,
    int fwd_strength)
{
    int rv = BCM_E_NONE;

    switch (trap_type)
    {
        case bcmRxTrap1588Discard:
            rv = cint_rx_trap_1588_traps_drop_set(unit, in_port);
            if (rv != BCM_E_NONE) 
            {
                printf("Error, in cint_rx_trap_1588_traps_drop_set \n");
                return rv;
            }
            break;
        case bcmRxTrap1588Accepted:
            rv = cint_rx_trap_1588_traps_fwd_set(unit, in_port, fwd_strength);
            if (rv != BCM_E_NONE) 
            {
                printf("Error, in cint_rx_trap_1588_traps_fwd_set \n");
                return rv;
            }     
            break;            
        case bcmRxTrap1588: {}
        case bcmRxTrap1588User1: {}
        case bcmRxTrap1588User2: {}
        case bcmRxTrap1588User3: {}
        case bcmRxTrap1588User4: {}
        case bcmRxTrap1588User5:
            rv = cint_rx_trap_1588_traps_cpu_set(unit, trap_type, in_port, out_port, fwd_strength);
            if (rv != BCM_E_NONE) 
            {
                printf("Error, in cint_rx_trap_1588_traps_cpu_set \n");
                return rv;
            }     
            break;
        default:
            rv = BCM_E_PARAM;
            printf("Error, invalid trap type = %d. Trap type should be 1588 trap: bcmRxTrap1588XXX\n", trap_type);
            return rv;            
    }
    
    return rv;
}

/**
* \brief
*  Function that clears timesync configuration.
* \par DIRECT INPUT:
*   \param [in] unit           - unit Id
*   \param [in] in_port            - in port
*
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
cint_rx_trap_1588_traps_timesync_clear(
    int unit,
    bcm_port_t in_port)
{
    int rv = BCM_E_NONE;
    bcm_port_timesync_config_t timesync_config;

    bcm_port_timesync_config_t_init(timesync_config);
    timesync_config.flags = BCM_PORT_TIMESYNC_DEFAULT;
    rv = bcm_port_timesync_config_set(unit, in_port, 1, &timesync_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_port_timesync_config_set\n");
        return rv;
    }

    return rv;
}

/**
* \brief
*  Destroy either accept or discard 1588 traps.
* \par DIRECT INPUT:
*   \param [in] unit                - unit Id
*   \param [in] trap_type        - bcmRxTrap1588XXX trap type
*   \param [in] in_port            - in port
*
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
cint_rx_trap_1588_traps_destroy(
    int unit,
    bcm_rx_trap_t trap_type,
    bcm_port_t in_port)
{
    int rv = BCM_E_NONE, trap_id = -1, device_is_jericho2;

    rv = cint_rx_trap_1588_traps_timesync_clear(unit, in_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_rx_trap_1588_traps_timesync_clear\n");
        return rv;
    }

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, is_device_jericho2, device_is_jericho2\n");
        return rv;
    }    

    if ((trap_type != bcmRxTrap1588Discard) || ((trap_type == bcmRxTrap1588Discard) && !device_is_jericho2))
    {
        /* Destroy the trap */
        rv = bcm_rx_trap_type_get(unit, 0, trap_type, &trap_id);
        rv = bcm_rx_trap_type_destroy(unit, trap_id);
        if (rv != BCM_E_NONE) 
        {
            printf("Error, in bcm_rx_trap_type_destroy \n");
            return rv;
        }  
    }
    
    return rv;
}

