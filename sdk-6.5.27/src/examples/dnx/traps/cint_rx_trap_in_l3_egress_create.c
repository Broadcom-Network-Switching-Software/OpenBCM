/* $Id:
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: cint_rx_trap_in_l3_egress_create.c
 * Purpose: Shows an example for configuration of FEC with port = trap_gport.
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_in_l3_egress_create.c
 *
 * Main Function:
 *      cint_rx_trap_in_l3_egress_create_main(unit);
 *
 * Example Config:
 * Test Scenario - start
  cint ../../../../src/examples/dnx/traps/cint_rx_trap_in_l3_egress_create.c
  cint
  cint_rx_trap_programmable_traps_main(0);
 * Test Scenario - end
 */


/* Check trap set and get inside fec in l3 egress create */
int cint_rx_trap_in_l3_egress_create_main(int unit)
{
    bcm_gport_t rcpu_gport;
    int trap_id;
    bcm_l3_egress_t nh_egress;
    bcm_l3_egress_t nh_egress1;
    bcm_if_t nh_fec;

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit,0,bcmRxTrapUserDefine,&trap_id), "");

    BCM_GPORT_TRAP_SET(rcpu_gport,trap_id,3,0);
    bcm_l3_egress_t_init(&nh_egress);
    nh_egress.port=rcpu_gport;

    if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
    	nh_egress.flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit,BCM_L3_INGRESS_ONLY ,&nh_egress, &nh_fec), "");

    bcm_l3_egress_t_init(&nh_egress1);
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_get(unit,nh_fec, &nh_egress1), "");

    if (nh_egress1.port != nh_egress.port) {
        printf("Error, in comparing get to set parameters \n");
        return -1;
    }

    return BCM_E_NONE;
}
