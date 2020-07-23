/* $Id: 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *  
 * File: cint_rx_trap_etpp_trap_to_cpu.c
 * Purpose: Example for creating and setting an ETPP action profile which traps packet to CPU.
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_etpp_trap_to_cpu.c
 *
 * Main Function:
 *      cint_rx_trap_etpp_trap_to_cpu_main(unit, &etpp_trap_id);
 * Destroy Function:
 *      cint_rx_trap_etpp_trap_to_cpu_destroy(unit, etpp_trap_id);
 *
 * Additional Notes: 
 * Since the destination of the packet cannot be changed at ETPP we recycle the packet 
 * and configure an ingress trap to redirect the packet to CPU.
 * In the cint we use a preconfigured trap bcmRxTrapDfltRedirectToCpuPacket, 
 * to configure additional actions for ingress trap please create a user-define ingress trap bcmRxTrapUserDefine.
 *
 * Example Config:
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_etpp_trap_to_cpu.c
 * cint
 * int etpp_trap_id;
 * cint_rx_trap_etpp_trap_to_cpu_main(0, &etpp_trap_id);
 */

 /**
 * \brief  
 *  Main function for configuring ETPP trap id to redirect packet to CPU
 * \par DIRECT INPUT: 
 *   \param [in] unit      -  Unit Id
 *   \param [in] trap_id - etpp trap id
 * \par INDIRECT INPUT: 
 *   * None
 * \par DIRECT OUTPUT: 
 *   int -  Error Type
 * \par INDIRECT OUTPUT: 
 *   * None
 * \remark 
 *   * None
 * \see
 *   * None
 */
int cint_rx_trap_etpp_trap_to_cpu_main(
int unit,
int* etpp_trap_id_p)
{
    int rv = BCM_E_NONE, ingress_trap_id;
    bcm_rx_trap_config_t trap_config;

    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapDfltRedirectToCpuPacket, &ingress_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_type_get\n");
        return rv;
    }   

    if (is_device_or_above(unit, JERICHO2) && (soc_property_get(unit, "system_headers_mode", 1) == 0)) { 
        /* Get the trap */
        rv = bcm_rx_trap_get(unit, ingress_trap_id, &trap_config);
        if (rv != BCM_E_NONE) 
        {
            printf("Error, in bcm_rx_trap_get \n");
            return rv;
        } 

        trap_config.flags |= BCM_RX_TRAP_BYPASS_FILTERS;    
        
        /* Set the trap */
        rv = bcm_rx_trap_set(unit, ingress_trap_id, &trap_config);
        if (rv != BCM_E_NONE) 
        {
            printf("Error, in bcm_rx_trap_set \n");
            return rv;
        } 
    }

    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgTxUserDefine, etpp_trap_id_p);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_type_create\n");
        return rv;
    }      

    bcm_rx_trap_config_t_init(&trap_config);
    BCM_GPORT_TRAP_SET(trap_config.cpu_trap_gport, ingress_trap_id, 15, 0);

    rv = bcm_rx_trap_set(unit, *etpp_trap_id_p, &trap_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_set\n");
        return rv;
    }     

    return rv;    
}

/**
* \brief  
*  Destroy etpp trap
* \par DIRECT INPUT: 
*   \param [in] unit      -  Unit Id
*   \param [in] etpp_trap_id - ETPP trap id
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int -  Error Type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
int cint_rx_trap_etpp_trap_to_cpu_destroy(
int unit,
int etpp_trap_id)
{
    int rv = BCM_E_NONE;

    rv = bcm_rx_trap_type_destroy(unit, etpp_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error destroying etpp trap, in bcm_rx_trap_type_destroy\n");
        return rv;
    }
    
    return rv;
}
