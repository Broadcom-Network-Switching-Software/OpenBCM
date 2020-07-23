/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_vswitch_port_based_pwe.c
 * Purpose: Example of creating a PWE which is identified by port+mpls_label.
 *
 * Attachment circuit (AC): Ethernet port attached to the service based on port-vlan-vlan, 
 *                          connect from access side. 
 * PWE: Virtual circuit attached to the service based on VC-label. Connect to the MPLS 
 *      network side.
 * 
 *   configuration tips:
 *   1. add soc property:
 *       pwe_termination_port_mode_enable =1
 *   2. enable the feature for relevant pp_port
 *   bcm_port_control_set(unit,pp_port,bcmPortControlPWETerminationPortModeEnable,1);
 *   3. call bcm_mpls_port_add() with BCM_MPLS_PORT_MATCH_LABEL_PORT flag and valid mpls_port.port parameter
 */ 
int port_based_pwe_run_with_defaults(int unit, int acP, int pweP)
{
   
    port_based_pwe_enable = 1;

   bcm_port_control_set(unit,pweP,bcmPortControlPWETerminationPortModeEnable ,1); 

    vswitch_vpls_run_with_defaults_dvapi(unit,acP,pweP);
   
    return BCM_E_NONE;
}

int port_based_pwe_update_match_label(int unit,int label)
{
    bcm_mpls_port_t  gport;
    int rv = 0;
   
   bcm_mpls_port_t_init(&gport);
   gport.mpls_port_id = network_port_id;

   rv = bcm_mpls_port_get(unit,vswitch_vpls_shared_info_1.vpn,&gport);
   if (rv != BCM_E_NONE) {
        printf("port_based_pwe_update_match_label(), Error, bcm_mpls_port_get\n");
        return rv;
   }

   if (mpls_termination_label_index_enable) {
       BCM_MPLS_INDEXED_LABEL_SET(gport.match_label, label, vswitch_vpls_info_2.access_index_3);
   } else {
       gport.match_label = label;
   }
   gport.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_REPLACE;

   rv = bcm_mpls_port_add(unit,vswitch_vpls_shared_info_1.vpn,&gport);
   if (rv != BCM_E_NONE) {
        printf("port_based_pwe_update_match_label(), Error, bcm_mpls_port_add\n");
        return rv;
   }
   return rv;
}


