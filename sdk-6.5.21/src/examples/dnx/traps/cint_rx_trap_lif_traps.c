/* $Id:
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_rx_trap_lif_traps.c
 * Purpose: Shows an example for configuration of lif traps.
 *
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_lif_traps.c
 *
 * Main Function:
 *      cint_rx_trap_lif_traps_main(unit, lif_type, lif_gport, rif_intf, action_gport);
 * Destroy Function:
 *      cint_rx_trap_lif_traps_destroy(unit, lif_type, lif_gport, rif_intf);
 *
 *
 * Example Config:
 * cint ../../../../src/examples/dnx/traps/cint_utils_rx.c
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_ingress_traps.c
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_lif_traps.c
 * cint
 * bcm_gport_t action_gport;
 * cint_rx_trap_ingress_traps_main(0,0,bcmRxTrapUserDefine,NULL);
 * BCM_GPORT_TRAP_SET(action_gport, cint_rx_trap_ingress_traps_trap_id, 7, 0);
 * cint_rx_trap_lif_traps_main(0, bcmRxTrapLifTypeInLif, lif_gport, NULL, action_gport);
 */

/**
* \brief
*  Function that sets-up the desired lif trap for a specified LIF.
* \par DIRECT INPUT:
*   \param [in] unit           - unit Id
*   \param [in] lif_type  - lif type
*   \param [in] lif_gport  - lif gport
*   \param [in] action_gport  - action gport encoding either trap or mirror(only for egress)
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
cint_rx_trap_lif_traps_lif_set(
    int unit,
    bcm_rx_trap_lif_type_t lif_type,
    bcm_gport_t lif_gport,
    bcm_gport_t action_gport)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_lif_config_t lif_config;

    lif_config.lif_type = lif_type;
    lif_config.lif_gport = lif_gport;
    lif_config.action_gport = action_gport;
    
    rv = bcm_rx_trap_lif_set(unit, 0, &lif_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_lif_set\n");
        return rv;
    }

    return rv;
}

/**
* \brief
*  Function that sets-up the desired lif trap for a specified RIF.
* \par DIRECT INPUT:
*   \param [in] unit           - unit Id
*   \param [in] lif_type  - lif type
*   \param [in] rif_intf  - RIF interface
*   \param [in] action_gport  - action gport encoding either trap or mirror(only for egress)
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
cint_rx_trap_lif_traps_rif_set(
    int unit,
    bcm_rx_trap_lif_type_t lif_type,
    bcm_if_t rif_intf,
    bcm_gport_t action_gport)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_lif_config_t lif_config;

    lif_config.lif_type = lif_type;
    lif_config.rif_intf = rif_intf;
    lif_config.action_gport = action_gport;
    
    rv = bcm_rx_trap_lif_set(unit, 0, &lif_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_lif_set\n");
        return rv;
    }

    return rv;
}

/**
* \brief
*  Function that sets-up the desired trap for a specified LIF.
* \par DIRECT INPUT:
*   \param [in] unit           - unit Id
*   \param [in] lif_type  - lif type
*   \param [in] lif_gport  - lif gport
*   \param [in] rif_intf  - rif interface
*   \param [in] action_gport  - action gport encoding either trap or mirror(only for egress)
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
cint_rx_trap_lif_traps_main(
    int unit,
    bcm_rx_trap_lif_type_t lif_type,
    bcm_gport_t lif_gport,
    bcm_if_t rif_intf,
    bcm_gport_t action_gport)
{
    int rv = BCM_E_PARAM;

    if ((lif_type == bcmRxTrapLifTypeInLif) || (lif_type == bcmRxTrapLifTypeOutLif))
    {
        rv = cint_rx_trap_lif_traps_lif_set(unit, lif_type, lif_gport, action_gport);
    }
    else if ((lif_type == bcmRxTrapLifTypeInRif) || (lif_type == bcmRxTrapLifTypeOutRif))
    {
        rv = cint_rx_trap_lif_traps_rif_set(unit, lif_type, rif_intf, action_gport);
    }

    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_rx_trap_lif_traps_main\n");
    }

    return rv;
}

/**
* \brief
*  Function that clears configuration for a specified LIF.
* \par DIRECT INPUT:
*   \param [in] unit           - unit Id
*   \param [in] lif_type  - lif type
*   \param [in] lif_gport  - lif gport
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
cint_rx_trap_lif_traps_lif_clear(
    int unit,
    bcm_rx_trap_lif_type_t lif_type,
    bcm_gport_t lif_gport)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_lif_config_t lif_config;

    lif_config.lif_type = lif_type;
    lif_config.lif_gport = lif_gport;
    lif_config.action_gport = BCM_GPORT_INVALID;
    
    rv = bcm_rx_trap_lif_set(unit, 0, &lif_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_lif_set\n");
        return rv;
    }

    return rv;
}

/**
* \brief
*  Function that clears configuration for a specified RIF.
* \par DIRECT INPUT:
*   \param [in] unit           - unit Id
*   \param [in] lif_type  - lif type
*   \param [in] rif_intf  - RIF interface
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
cint_rx_trap_lif_traps_rif_clear(
    int unit,
    bcm_rx_trap_lif_type_t lif_type,
    bcm_if_t rif_intf)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_lif_config_t lif_config;

    lif_config.lif_type = lif_type;
    lif_config.rif_intf = rif_intf;
    lif_config.action_gport = BCM_GPORT_INVALID;
    
    rv = bcm_rx_trap_lif_set(unit, 0, &lif_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_lif_set\n");
        return rv;
    }

    return rv;
}

/**
* \brief
*  Function that gets and compares action gports.
* \par DIRECT INPUT:
*   \param [in] unit                - unit Id
*   \param [in] lif_type            - lif type
*   \param [in] lif_gport           - lif gport
*   \param [in] rif_intf            - rif interface
*   \param [in] set_action_gport    - Set trap action gport to compare against.
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
cint_rx_trap_lif_traps_get_compare(
    int unit,
    bcm_rx_trap_lif_type_t lif_type,
    bcm_gport_t lif_gport,
    bcm_if_t rif_intf,
    bcm_gport_t set_action_gport)
{

    int rv = BCM_E_NONE;
    bcm_rx_trap_lif_config_t lif_config;

    lif_config.lif_type = lif_type;
    lif_config.rif_intf = rif_intf;
    lif_config.lif_gport = lif_gport;

    rv = bcm_rx_trap_lif_get(unit, &lif_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_lif_set\n");
        return rv;
    }

    if(set_action_gport != lif_config.action_gport)
    {
        printf("Set Acton Gport(0x%08X) is not equal to Get Action Gport(0x%08X)\n",set_action_gport, lif_config.action_gport);
        return BCM_E_PARAM;
    }

    return rv;

}

/**
* \brief
*  Function that clears configuration for a specified LIF.
* \par DIRECT INPUT:
*   \param [in] unit           - unit Id
*   \param [in] lif_type  - lif type
*   \param [in] lif_gport  - lif gport
*   \param [in] rif_intf  - rif interface
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
cint_rx_trap_lif_traps_destroy(
    int unit,
    bcm_rx_trap_lif_type_t lif_type,
    bcm_gport_t lif_gport,
    bcm_if_t rif_intf)
{
    int rv = BCM_E_PARAM;

    if ((lif_type == bcmRxTrapLifTypeInLif) || (lif_type == bcmRxTrapLifTypeOutLif))
    {
        rv = cint_rx_trap_lif_traps_lif_clear(unit, lif_type, lif_gport);
    }
    else if ((lif_type == bcmRxTrapLifTypeInRif) || (lif_type == bcmRxTrapLifTypeOutRif))
    {
        rv = cint_rx_trap_lif_traps_rif_clear(unit, lif_type, rif_intf);
    }

    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_rx_trap_lif_traps_destroy\n");
    }

    return rv;
}

