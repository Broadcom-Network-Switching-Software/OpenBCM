/* $Id: 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *  
 * File: cint_trap_utilities.c
 * Purpose: Utilities for traps configurations
 *
 * cint ../../../../src/examples/dnx/traps/cint_trap_utilities.c
 */

/**
* \brief  
*  Create ingress trap and configure trap action
* \par DIRECT INPUT: 
*   \param [in] unit      -  Unit Id
*   \param [in] trap_type   -  trap type to configure
*   \param [in] trap_create_flags   -  flags for trap creation (BCM_RX_TRAP_WITH_ID)
*   \param [in] trap_config  - trap action configuration
*   \param [in/out] trap_id  - id of created trap (when BCM_RX_TRAP_WITH_ID is set, supplied as input)
*/
int cint_utils_rx_trap_create_and_set(
int unit,
int trap_create_flags,
bcm_rx_trap_t trap_type,
bcm_rx_trap_config_t* trap_config,
int* trap_id)
{
    int rv = BCM_E_NONE;

    /* Create the trap */
    rv = bcm_rx_trap_type_create(unit, trap_create_flags, trap_type, trap_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error, in trap create, trap_id = %d \n", *trap_id);
        return rv;
    }

    /* Set the trap */
    rv = bcm_rx_trap_set(unit, (*trap_id), trap_config);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in bcm_rx_trap_set \n");
        return rv;
    } 

    return rv;    
}

/**
* \brief  
*  Create Egress application trap and configure trap action
* \par DIRECT INPUT: 
*   \param [in] unit      -  Unit Id
*   \param [in] trap_create_flags   -  flags for trap creation (BCM_RX_TRAP_WITH_ID)
*   \param [in] is_erpp_trap   -  if TRUE indicates trap is ERPP else ETPP
*   \param [in] appl_trap_type   -  application trap type to configure
*   \param [in] trap_config  - trap action configuration
*   \param [in/out] trap_id  - id of created trap (when BCM_RX_TRAP_WITH_ID is set, supplied as input)
*/
int cint_utils_rx_trap_appl_config(
int unit,
int trap_create_flags,
int is_erpp_trap,
bcm_rx_trap_t appl_trap_type,
bcm_rx_trap_config_t* trap_config,
int* trap_id)
{
    int rv = BCM_E_NONE, fwd_strength = trap_config->trap_strength, snp_strength = trap_config->snoop_strength;
    int ud_trap_type = (is_erpp_trap) ? bcmRxTrapEgUserDefine : bcmRxTrapEgTxUserDefine;
    bcm_gport_t trap_gport;

    /* Strengths should be zero for user-defined trap */
    trap_config->trap_strength = 0;
    trap_config->snoop_strength = 0;

    if (fwd_strength != 0)
    {
        /* Create the trap */
        rv = cint_utils_rx_trap_create_and_set(unit, trap_create_flags, ud_trap_type, trap_config, trap_id);
        if(rv != BCM_E_NONE)
        {
            printf("Error, in cint_utils_rx_trap_create_and_set \n");
            return rv;
        }
    }
    
    /* Encode trap action profile */
    BCM_GPORT_TRAP_SET(trap_gport, trap_id, fwd_strength, snp_strength);

    rv = bcm_rx_trap_action_profile_set(unit, 0, appl_trap_type, trap_gport);
    if(rv != BCM_E_NONE)
    {
        printf("Error, in bcm_dnx_rx_trap_action_profile_set \n");
        return rv;
    }    

    return rv;    
}

/**
* \brief  
*  Create Egress application trap and configure trap action
* \par DIRECT INPUT: 
*   \param [in] unit      -  Unit Id
*   \param [in] appl_trap_type   -  application trap type to destroy
*/
int cint_utils_rx_trap_appl_destroy(
int unit,
bcm_rx_trap_t appl_trap_type)
{
    int rv = BCM_E_NONE, ud_trap_id;
    bcm_gport_t trap_gport;

    rv = bcm_rx_trap_action_profile_get(unit, appl_trap_type, &trap_gport);
    if(rv != BCM_E_NONE)
    {
        printf("Error, in bcm_rx_trap_action_profile_get \n");
        return rv;
    }
    
    rv = bcm_rx_trap_action_profile_clear(unit, appl_trap_type);
    if(rv != BCM_E_NONE)
    {
        printf("Error, in bcm_rx_trap_action_profile_clear \n");
        return rv;
    }

    ud_trap_id = BCM_GPORT_TRAP_GET_ID(trap_gport);
    
    if (ud_trap_id != BCM_RX_TRAP_EG_TRAP_ID_DEFAULT && 
        ud_trap_id != BCM_RX_TRAP_EG_TX_TRAP_ID_DEFAULT &&
        ud_trap_id != BCM_RX_TRAP_EG_TX_TRAP_ID_DROP)
    {
        rv = bcm_rx_trap_type_destroy(unit, ud_trap_id);
        if(rv != BCM_E_NONE)
        {
            printf("Error, in bcm_rx_trap_type_destroy \n");
            return rv;
        }
    }
    
    return rv;    
}

/**
* \brief
*  Configuration of Ingress trap action to drop packet
* \par DIRECT INPUT:
*   \param [in] unit            - unit Id
*   \param [in] trap_create_flags   -  flags for trap creation (BCM_RX_TRAP_WITH_ID)
*   \param [in] trap_type       - BCM trap type
*   \param [in] fwd_strength    - forward strength
*   \param [in/out] trap_id  - id of created trap (when BCM_RX_TRAP_WITH_ID is set, supplied as input)
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
cint_utils_rx_trap_ingress_create_and_set_to_drop(
    int unit,
    int trap_create_flags,
    bcm_rx_trap_t trap_type,
    int fwd_strength,
    int *trap_id)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config = { 0 };

    trap_config.trap_strength = fwd_strength;

    /* Configure trap action to drop */
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    trap_config.dest_port = BCM_GPORT_BLACK_HOLE;

    /* Create the trap */
    rv = cint_utils_rx_trap_create_and_set(unit, trap_create_flags, trap_type, &trap_config, trap_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error, in cint_utils_rx_trap_create_and_set \n");
        return rv;
    }

    return rv;
}

/**
* \brief
*  Configuration of Ingress trap action to redirect packet to CPU
* \par DIRECT INPUT:
*   \param [in] unit            - unit Id
*   \param [in] trap_create_flags   -  flags for trap creation (BCM_RX_TRAP_WITH_ID)
*   \param [in] trap_type       - BCM trap type
*   \param [in] fwd_strength    - forward strength
*   \param [in/out] trap_id  - id of created trap (when BCM_RX_TRAP_WITH_ID is set, supplied as input)
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
cint_utils_rx_trap_ingress_create_and_set_to_cpu(
    int unit,
    int trap_create_flags,
    bcm_rx_trap_t trap_type,
    int fwd_strength,
    int *trap_id)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config = { 0 };

    trap_config.trap_strength = fwd_strength;

    /* Configure trap action to drop */
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    trap_config.dest_port = BCM_GPORT_LOCAL_CPU;

    /* Create the trap */
    rv = cint_utils_rx_trap_create_and_set(unit, trap_create_flags, trap_type, &trap_config, trap_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error, in cint_utils_rx_trap_create_and_set \n");
        return rv;
    }

    return rv;
}

/**
* \brief
*  Configuration of ERPP trap action to drop the packet
* \par DIRECT INPUT:
*   \param [in] unit            - unit Id
*   \param [in] trap_create_flags   -  flags for trap creation (BCM_RX_TRAP_WITH_ID)
*   \param [in] appl_trap_type       - application trap type
*   \param [in] fwd_strength    - forward strength
*   \param [in/out] trap_id  - id of created trap (when BCM_RX_TRAP_WITH_ID is set, supplied as input)
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
cint_utils_rx_trap_erpp_create_and_set_to_drop(
    int unit,
    int trap_create_flags,
    bcm_rx_trap_t appl_trap_type,
    int fwd_strength,
    int *trap_id)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config = { 0 };
    bcm_gport_t trap_gport;

    /* Configure trap action to drop */
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    trap_config.dest_port = BCM_GPORT_BLACK_HOLE;

    /* Create the trap */
    rv = cint_utils_rx_trap_create_and_set(unit, trap_create_flags, bcmRxTrapEgUserDefine, &trap_config, trap_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error, in cint_utils_rx_trap_create_and_set \n");
        return rv;
    }

    BCM_GPORT_TRAP_SET(trap_gport, trap_id, fwd_strength, 0);

    rv = bcm_rx_trap_action_profile_set(unit, 0, appl_trap_type, trap_gport);
    if(rv != BCM_E_NONE)
    {
        printf("Error, in bcm_dnx_rx_trap_action_profile_set \n");
        return rv;
    }    

    return rv;
}

/**
* \brief
*  Configuration of ERPP trap action to redirect packet to CPU
* \par DIRECT INPUT:
*   \param [in] unit            - unit Id
*   \param [in] trap_create_flags   -  flags for trap creation (BCM_RX_TRAP_WITH_ID)
*   \param [in] appl_trap_type       - application trap type
*   \param [in] fwd_strength    - forward strength
*   \param [in/out] trap_id  - id of created trap (when BCM_RX_TRAP_WITH_ID is set, supplied as input)
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
cint_utils_rx_trap_erpp_create_and_set_to_cpu(
    int unit,
    int trap_create_flags,
    bcm_rx_trap_t appl_trap_type,
    int fwd_strength,
    int *trap_id)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config = { 0 };
    bcm_gport_t trap_gport;

    /* Configure trap action to drop */
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    trap_config.dest_port = BCM_GPORT_LOCAL_CPU;

    /* Create the trap */
    rv = cint_utils_rx_trap_create_and_set(unit, trap_create_flags, bcmRxTrapEgUserDefine, &trap_config, trap_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error, in cint_utils_rx_trap_create_and_set \n");
        return rv;
    }

    BCM_GPORT_TRAP_SET(trap_gport, trap_id, fwd_strength, 0);

    rv = bcm_rx_trap_action_profile_set(unit, 0, appl_trap_type, trap_gport);
    if(rv != BCM_E_NONE)
    {
        printf("Error, in bcm_dnx_rx_trap_action_profile_set \n");
        return rv;
    }    

    return rv;
}

/**
* \brief
*  Configuration of ETPP trap action to drop the packet
* \par DIRECT INPUT:
*   \param [in] unit            - unit Id
*   \param [in] appl_trap_type       - application trap type
*   \param [in] fwd_strength    - forward strength
*   \param [out] trap_id  - id of drop action profile
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
cint_utils_rx_trap_etpp_create_and_set_to_drop(
    int unit,
    bcm_rx_trap_t appl_trap_type,
    int fwd_strength,
    int *trap_id)
{
    int rv = BCM_E_NONE;
    bcm_gport_t trap_gport;

    BCM_GPORT_TRAP_SET(trap_gport, BCM_RX_TRAP_EG_TX_TRAP_ID_DROP, fwd_strength, 0);

    rv = bcm_rx_trap_action_profile_set(unit, 0, appl_trap_type, trap_gport);
    if(rv != BCM_E_NONE)
    {
        printf("Error, in bcm_dnx_rx_trap_action_profile_set \n");
        return rv;
    }    

    return rv;
}

