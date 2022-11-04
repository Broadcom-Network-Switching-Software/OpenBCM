/* $Id:
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: cint_rx_trap_1588_traps.c
 * Purpose: Shows an example for configuration of 1588 traps.
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_1588_traps.c
 *
 * Main Function:
 *      cint_rx_trap_1588_traps_main(unit, trap_type, in_port, out_port, fwd_strength);
 * Destroy Function:
 *      cint_rx_trap_1588_traps_destroy(unit, trap_type);
 *
 * Example Config:
 * Test Scenario - start
  cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
  cint ../../../../src/examples/dnx/traps/cint_utils_rx.c
  cint ../../../../src/examples/dnx/traps/cint_rx_trap_1588_traps.c
  cint
  cint_rx_trap_1588_traps_main(0, bcmRxTrap1588Discard, 13, 0, 0);
 * Test Scenario - end
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
    int trap_id = -1, is_dnx;
    bcm_port_timesync_config_t timesync_config;
    bcm_rx_trap_config_t trap_config;

    bcm_port_timesync_config_t_init(timesync_config);
    timesync_config.flags = BCM_PORT_TIMESYNC_DEFAULT | BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP;
    timesync_config.pkt_drop = 1;
    timesync_config.pkt_tocpu = 0;
    
    BCM_IF_ERROR_RETURN_MSG(bcm_port_timesync_config_set(unit, in_port, 1, &timesync_config), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx), "");
 
    if (!is_dnx)
    {
        /* Set the trap */
        bcm_rx_trap_config_t_init(&trap_config);
        trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
        trap_config.trap_strength = 7;
        trap_config.dest_port = BCM_GPORT_BLACK_HOLE;
        BCM_IF_ERROR_RETURN_MSG(cint_utils_rx_trap_create_and_set(unit, 0, bcmRxTrap1588Discard, &trap_config, &trap_id), "");
    }

    return BCM_E_NONE;
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
    int trap_id = -1;
    bcm_port_timesync_config_t timesync_config;
    bcm_rx_trap_config_t trap_config;

    bcm_port_timesync_config_t_init(timesync_config);
    timesync_config.flags = BCM_PORT_TIMESYNC_DEFAULT | BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP;
    timesync_config.pkt_drop = 0;
    timesync_config.pkt_tocpu = 0;
    
    BCM_IF_ERROR_RETURN_MSG(bcm_port_timesync_config_set(unit, in_port, 1, &timesync_config), "");

    /* Set the trap */
    bcm_rx_trap_config_t_init(&trap_config);    
    trap_config.trap_strength = fwd_strength;
    BCM_IF_ERROR_RETURN_MSG(cint_utils_rx_trap_create_and_set(unit, 0, bcmRxTrap1588Accepted, &trap_config, &trap_id), "");
    
    return BCM_E_NONE;
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
    int trap_id = -1;
    bcm_port_timesync_config_t timesync_config;
    bcm_rx_trap_config_t trap_config;

    /* Set the trap */
    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;    
    trap_config.trap_strength = fwd_strength;
    trap_config.dest_port = out_port;
    BCM_IF_ERROR_RETURN_MSG(cint_utils_rx_trap_create_and_set(unit, 0, trap_type, &trap_config, &trap_id), "");

    bcm_port_timesync_config_t_init(timesync_config);
    timesync_config.flags = use_default_profile?BCM_PORT_TIMESYNC_DEFAULT:(BCM_PORT_TIMESYNC_DEFAULT | BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP);
    timesync_config.pkt_drop = 0;
    timesync_config.pkt_tocpu = 1;
    if (trap_type != bcmRxTrap1588)
    {
        timesync_config.user_trap_id = trap_id;
    }
    
    BCM_IF_ERROR_RETURN_MSG(bcm_port_timesync_config_set(unit, in_port, 1, &timesync_config), "");
    
    return BCM_E_NONE;
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
    switch (trap_type)
    {
        case bcmRxTrap1588Discard:
            BCM_IF_ERROR_RETURN_MSG(cint_rx_trap_1588_traps_drop_set(unit, in_port), "");
            break;
        case bcmRxTrap1588Accepted:
            BCM_IF_ERROR_RETURN_MSG(cint_rx_trap_1588_traps_fwd_set(unit, in_port, fwd_strength), "");
            break;            
        case bcmRxTrap1588: {}
        case bcmRxTrap1588User1: {}
        case bcmRxTrap1588User2: {}
        case bcmRxTrap1588User3: {}
        case bcmRxTrap1588User4: {}
        case bcmRxTrap1588User5:
            BCM_IF_ERROR_RETURN_MSG(cint_rx_trap_1588_traps_cpu_set(unit, trap_type, in_port, out_port, fwd_strength), "");
            break;
        default:
            printf("Error, invalid trap type = %d. Trap type should be 1588 trap: bcmRxTrap1588XXX\n", trap_type);
            return BCM_E_PARAM;
    }
    
    return BCM_E_NONE;
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
    bcm_port_timesync_config_t timesync_config;

    bcm_port_timesync_config_t_init(timesync_config);
    timesync_config.flags = BCM_PORT_TIMESYNC_DEFAULT;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_timesync_config_set(unit, in_port, 1, &timesync_config), "");

    return BCM_E_NONE;
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
    int trap_id = -1, is_dnx;

    BCM_IF_ERROR_RETURN_MSG(cint_rx_trap_1588_traps_timesync_clear(unit, in_port), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx), "");

    if ((trap_type != bcmRxTrap1588Discard) || ((trap_type == bcmRxTrap1588Discard) && !is_dnx))
    {
        /* Destroy the trap */
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_get(unit, 0, trap_type, &trap_id), "");
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_destroy(unit, trap_id), "");
    }
    
    return BCM_E_NONE;
}

