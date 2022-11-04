/* $Id:
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: cint_rx_trap_etpp_recycle_and_2nd_pass_stamp.c
 * Purpose: Example for creating and setting an ETPP recycling and 2nd pass FHEI stamping + updating destination of 2nd pass packet.
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_etpp_recycle_and_2nd_pass_stamp.c
 *
 * Main Function:
 *      cint_rx_trap_etpp_recycle_and_2nd_pass_stamp_main(unit, out_port, &etpp_trap_id);
 * Destroy Function:
 *      cint_rx_trap_etpp_recycle_and_2nd_pass_stamp_destroy(unit, etpp_trap_id);
 *
 * Additional Notes:
 * Create an Ingress UD trap in range for Jer1 mode as well and set the actions to be FHEI stamping and
 * updating the destination of the packet to a supplied port.
 * Create an ETPP UD trap with action recycling which we supply the Ingress UD trap to be executed via Gport.
 *
 * Example Config:
 * Test Scenario - start
  cint ../../../../src/examples/dnx/traps/cint_rx_trap_etpp_recycle_and_2nd_pass_stamp.c
  cint
  int etpp_trap_id;
  cint_rx_trap_etpp_recycle_and_2nd_pass_stamp_main(0, 201, &etpp_trap_id);
 * Test Scenario - end
 */


/**
 * This number can be changed with any valid UD trap id.
 * This was chosen to be high enough to not be allocated soon by without ID allocation.
 */
int cint_rx_trap_etpp_recycle_and_2nd_pass_ingress_trap_id = 201;
int cint_rx_trap_etpp_recycle_and_2nd_pass_ingress_rcy_port = 221;

int
cint_rx_trap_etpp_recycle_and_2nd_pass_stamp_main(
    int unit,
    int out_port,
    int * etpp_trap_id)
{
    bcm_rx_trap_config_t trap_config;
    int trap_strength = 7, snoop_strength = 0;
    int egress_divided_to_erpp_and_etpp = *dnxc_data_get(unit, "pp", "stages", "is_erpp_supported", NULL);

    /**
     * Creating the Ingress trap for the 2nd pass of the packet.
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, &cint_rx_trap_etpp_recycle_and_2nd_pass_ingress_trap_id), "");

    bcm_rx_trap_config_t_init(&trap_config);

    /**
     * Setting the 2nd pass trap actions to stamp the FTMH
     * and forward the packet to to the desired destination port.
     */
    trap_config.flags = (BCM_RX_TRAP_TRAP | BCM_RX_TRAP_UPDATE_DEST);
    trap_config.dest_port = out_port;
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, cint_rx_trap_etpp_recycle_and_2nd_pass_ingress_trap_id, &trap_config), "");

    /**
     * Creating the ETPP Trap
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgTxUserDefine, etpp_trap_id), "");

    /**
     * Setting the ETPP Trap action to recycle the packet and trap it on the 2nd pass.
     */
    bcm_rx_trap_config_t_init(&trap_config);
    BCM_GPORT_TRAP_SET(trap_config.cpu_trap_gport, cint_rx_trap_etpp_recycle_and_2nd_pass_ingress_trap_id, trap_strength, snoop_strength);

    /*
     */
     if (egress_divided_to_erpp_and_etpp == 0)
     {
         trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
         trap_config.dest_port = cint_rx_trap_etpp_recycle_and_2nd_pass_ingress_rcy_port;
         trap_config.is_recycle_append_ftmh = 1;
     }

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, *etpp_trap_id, &trap_config), "");

    return BCM_E_NONE;
}


int
cint_rx_trap_etpp_recycle_and_2nd_pass_stamp_destroy(
    int unit,
    int etpp_trap_id)
{
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_destroy(unit, cint_rx_trap_etpp_recycle_and_2nd_pass_ingress_trap_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_destroy(unit, etpp_trap_id), "");

    return BCM_E_NONE;
}
