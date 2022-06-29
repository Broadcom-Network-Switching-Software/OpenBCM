/* $Id: 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved. 
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

int cint_rx_trap_etpp_trap_to_cpu_rcy_port = 221;

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
    int ingress_trap_id;
    bcm_rx_trap_config_t trap_config;
    int egress_divided_to_erpp_and_etpp = *dnxc_data_get(unit, "trap", "egress", "egress_divided_to_erpp_and_etpp", NULL);

    BCM_IF_ERROR_RETURN(bcm_rx_trap_type_get(unit, 0, bcmRxTrapDfltRedirectToCpuPacket, &ingress_trap_id));

    if (egress_divided_to_erpp_and_etpp == 1) {
        if (!sand_utils_is_jr2_system_header_mode(unit)) {
            /* Get the trap */
            BCM_IF_ERROR_RETURN(bcm_rx_trap_get(unit, ingress_trap_id, &trap_config));

            trap_config.flags |= BCM_RX_TRAP_BYPASS_FILTERS;

            /* Set the trap */
            BCM_IF_ERROR_RETURN(bcm_rx_trap_set(unit, ingress_trap_id, &trap_config));
        }
    }

    BCM_IF_ERROR_RETURN(bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgTxUserDefine, etpp_trap_id_p));

    bcm_rx_trap_config_t_init(&trap_config);
    BCM_GPORT_TRAP_SET(trap_config.cpu_trap_gport, ingress_trap_id, 15, 0);

   /*
    */
    if (egress_divided_to_erpp_and_etpp == 0)
    {
        trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
        trap_config.dest_port = cint_rx_trap_etpp_trap_to_cpu_rcy_port;
        trap_config.is_recycle_append_ftmh = 1;
    }

    BCM_IF_ERROR_RETURN(bcm_rx_trap_set(unit, *etpp_trap_id_p, &trap_config));

    return BCM_E_NONE;
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
    BCM_IF_ERROR_RETURN(bcm_rx_trap_type_destroy(unit, etpp_trap_id));

    return BCM_E_NONE;
}
