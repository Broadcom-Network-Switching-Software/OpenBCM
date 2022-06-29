/* $Id:
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: cint_rx_trap_etpp_keep_orig_fabric_hdr.c
 * Purpose: Example for creating and setting an ETPP recycling with keeping fabric headers and 2nd pass FHEI stamping + updating destination of 2nd pass packet.
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_etpp_keep_orig_fabric_hdr.c
 *
 * Main Function:
 *      cint_rx_trap_etpp_keep_orig_fabric_hdr_main(unit, out_port, &etpp_trap_id);
 * Destroy Function:
 *      cint_rx_trap_etpp_keep_orig_fabric_hdr_destroy(unit, etpp_trap_id);
 *
 * Additional Notes:
 * Create an Ingress UD trap in range for Jer1 mode as well and set the actions to be FHEI stamping and
 * updating the destination of the packet to a supplied port.
 * Create an ETPP UD trap with action keeping fabric headers and recycling which we supply the Ingress UD trap to be executed via Gport.
 *
 * Example Config:
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_etpp_keep_orig_fabric_hdr.c
 * cint_rx_trap_etpp_keep_orig_fabric_hdr_main(0, 201, &etpp_trap_id);
 */

int cint_rx_trap_etpp_keep_orig_fabric_hdr_ingress_trap_id;
int cint_rx_trap_etpp_keep_orig_fabric_hdr_rcy_port = 221;

/**
* \brief
*  Configuration of Forward recycling with keeping original system headers
*  and redirecting the packet to CPU on 2nd pass
* \par DIRECT INPUT:
*   \param [in] unit   -  unit id
*   \param [in] out_port   -  final CPU out port
*   \param [out] etpp_trap_id  - ETPP trap_id
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
int cint_rx_trap_etpp_keep_orig_fabric_hdr_main(
    int unit,
    int out_port,
    int * etpp_trap_id)
{
    bcm_rx_trap_config_t trap_config;
    int trap_strength = 15, snoop_strength = 0;
    int rv = BCM_E_NONE, ingress_trap_id;
    int egress_divided_to_erpp_and_etpp = *dnxc_data_get(unit, "trap", "egress", "egress_divided_to_erpp_and_etpp", NULL);

    /**
     * Creating the Ingress trap for the 2nd pass of the packet.
     */
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &ingress_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_type_create\n");
        return rv;
    }

    cint_rx_trap_etpp_keep_orig_fabric_hdr_ingress_trap_id = ingress_trap_id;
    bcm_rx_trap_config_t_init(&trap_config);

    /**
     * Setting the 2nd pass trap actions to stamp the FHEI
     * and forward the packet to to the desired destination port.
     */
    trap_config.flags = (BCM_RX_TRAP_TRAP | BCM_RX_TRAP_UPDATE_DEST);
    trap_config.dest_port = out_port;
    rv = bcm_rx_trap_set(unit, ingress_trap_id, &trap_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_set\n");
        return rv;
    }

    /**
     * Creating the ETPP Trap
     */
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgTxUserDefine, etpp_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_type_create\n");
        return rv;
    }

    /**
     * Setting the ETPP Trap action to recycle the packet and trap it on the 2nd pass.
     */
    bcm_rx_trap_config_t_init(&trap_config);
    BCM_GPORT_TRAP_SET(trap_config.cpu_trap_gport, ingress_trap_id, trap_strength, snoop_strength);
    trap_config.flags2 = BCM_RX_TRAP_FLAGS2_KEEP_FABRIC_HEADER;

    /** For united egress devices, need to add 64B in order to include IBCH needed for recycling */
    if (egress_divided_to_erpp_and_etpp == 0)
    {
        trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
        trap_config.dest_port = cint_rx_trap_etpp_keep_orig_fabric_hdr_rcy_port;
        trap_config.is_recycle_append_ftmh = 1;
    }

    rv = bcm_rx_trap_set(unit, *etpp_trap_id, &trap_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_set\n");
        return rv;
    }

    return rv;
}

/**
* \brief
*   Destroy cint_rx_trap_etpp_keep_orig_fabric_hdr_main trap configurations
* \par DIRECT INPUT:
*   \param [in] unit   -  unit id
*   \param [in] etpp_trap_id  - ETPP trap_id
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
int cint_rx_trap_etpp_keep_orig_fabric_hdr_destroy(
    int unit,
    int etpp_trap_id)
{
    int rv = BCM_E_NONE, ingress_trap_id;
    bcm_rx_trap_config_t trap_config;

    rv = bcm_rx_trap_get(unit, etpp_trap_id, &trap_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_get\n");
        return rv;
    }

    ingress_trap_id = BCM_GPORT_TRAP_GET_ID(trap_config.cpu_trap_gport);

    rv = bcm_rx_trap_type_destroy(unit, ingress_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_type_destroy\n");
        return rv;
    }

    rv = bcm_rx_trap_type_destroy(unit, etpp_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_type_destroy\n");
        return rv;
    }

    return rv;
}
