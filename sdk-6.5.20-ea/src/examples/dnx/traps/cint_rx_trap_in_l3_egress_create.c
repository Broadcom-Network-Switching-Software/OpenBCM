/* $Id:
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_rx_trap_in_l3_egress_create.c
 * Purpose: Shows an example for configuration of FEC with port = trap_gport.
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_in_l3_egress_create.c
 *
 * Main Function:
 *      cint_rx_trap_in_l3_egress_create_main(unit);
 *
 *
 * Example Config:
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_in_l3_egress_create.c
 * cint
 * cint_rx_trap_programmable_traps_main(0);
 */


/* Check trap set and get inside fec in l3 egress create */
int cint_rx_trap_in_l3_egress_create_main(int unit)
{
    bcm_error_t rv;
    bcm_gport_t rcpu_gport;
    int trap_id;
    bcm_l3_egress_t nh_egress;
    bcm_l3_egress_t nh_egress1;
    bcm_if_t nh_fec;

    rv = bcm_rx_trap_type_create(unit,0,bcmRxTrapUserDefine,&trap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_trap_type_create \n");
        return rv;
    }

    BCM_GPORT_TRAP_SET(rcpu_gport,trap_id,3,0);
    bcm_l3_egress_t_init(&nh_egress);
    nh_egress.port=rcpu_gport;

    rv = bcm_l3_egress_create(unit,BCM_L3_INGRESS_ONLY ,&nh_egress, &nh_fec);
     if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l3_egress_create \n");
        return rv;
    }

    bcm_l3_egress_t_init(&nh_egress1);
    rv = bcm_l3_egress_get(unit,nh_fec, &nh_egress1);
     if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l3_egress_get \n");
        return rv;
    }

    if (nh_egress1.port != nh_egress.port) {
        printf("Error, in comparing get to set parameters \n");
        return -1;
    }

    return rv;
}
