/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~MPLS LSR~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_mpls_lsr_context.c
 * Purpose: Example of the MPLS LSR using VRF/IN-RIF as lookup context. 
 *
 * Explanation:
 *  *   For incoming packet, the Ethernet header is terminated.
 *  *   Labels are swapped and forwarded to egress port with new lable and Ethernet encapsulations.
 *  *   Different from legacy LSR lookup, There are 2 types of look up: 
    *      -  MPLS Label + VRF
    *      -  MPLS Label + IN-RIF
 * Calling sequence:
 *   -  Add soc property: 
 *        - mpls_termination_label_index_enable=1
 *        - mpls_context:
 *           - VRF context:      mpls_context=vrf
 *           - IN-RIF context :  mpls_context=interface
 *   
 *   -  Add ILM (ingress label mapping) entry .
 *      - For VRF context:
 *         -  set the vpn parameter as VRF value.
 *      - For In-RIF context:
 *         -  set the flags with BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF
 *         -  set the ingress_if as IN-RIF

*/


uint16   lsr_vrf = 2000;



int 
mpls_lsr_l3_intf_create(int unit, int vid, int vrf) {
    create_l3_intf_s intf;
    int rv;

    intf.vsi = vid;
    intf.my_global_mac = mpls_lsr_info_1.my_mac;
    intf.my_lsb_mac = mpls_lsr_info_1.my_mac;
    intf.skip_mymac = 1;
    intf.vrf = vrf;
    intf.vrf_valid = 1;
    rv = l3__intf_rif__create(unit, &intf);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create");
    }

    return rv;
}

int
mpls_lsr_config_init(int unit){
    int rv = 0;
    bcm_pbmp_t pbmp,ubmp;
    bcm_vlan_t vlan;

    vlan = mpls_lsr_info_1.in_vid;
    rv = bcm_vlan_create(unit, vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with vlan %d continue \n", vlan);
        print rv;
    }

    /* add vlan to pwe_port */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, mpls_lsr_info_1.in_port);

    rv = bcm_vlan_port_add(unit, vlan, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_add, vlan=%d, \n", vlan);
        print rv;
        return rv;
    }

    /*create L3 interface*/
    mpls_lsr_l3_intf_create(unit,vlan,lsr_vrf); 
    mpls_lsr_l3_intf_create(unit,mpls_lsr_info_1.eg_vid,lsr_vrf);


    /*create ILM entry*/
    bcm_mpls_egress_label_t label_array[2];

    bcm_mpls_egress_label_t_init(&label_array[0]);
    bcm_mpls_egress_label_t_init(&label_array[1]);

    label_array[0].exp = 0; /* Set it statically 0*/ 
    label_array[0].flags = (BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);  
    label_array[0].flags =BCM_MPLS_EGRESS_LABEL_ACTION_VALID;  
    label_array[0].action = BCM_MPLS_EGRESS_ACTION_SWAP;

    label_array[0].label = mpls_lsr_info_1.eg_label;
    label_array[0].ttl = 20;

    rv = bcm_mpls_tunnel_initiator_create(unit,0,1,label_array);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    printf("mpls tunnel id:0x%x",label_array.tunnel_id);

    create_l3_egress_s l3eg;

    sal_memcpy(l3eg.next_hop_mac_addr, mpls_lsr_info_1.next_hop_mac, 6);
    l3eg.vlan   = mpls_lsr_info_1.eg_vid;
    l3eg.out_gport   = mpls_lsr_info_1.eg_port;
    l3eg.out_tunnel_or_rif = label_array[0].tunnel_id;

    rv = l3__egress__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, in l3__egress__create rv=%d \n", rv);
        return rv;
    }
    printf("egress FEC ID=0x%x\n",l3eg.fec_id);

    mpls_lsr_info_1.egress_intf = l3eg.fec_id;

    return rv;

}

 int
mpls_add_switch_entry_with_context(int unit, int in_label, int eg_label,  bcm_if_t egress_intf,int vrf,int rif)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;
    

    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_SWAP;
    /* TTL decrement has to be present */
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    /* Uniform: inherit TTL and EXP, 
     * in general valid options: 
     * both present (uniform) or none of them (Pipe)
     */
    entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;
    /* incomming label */
    BCM_MPLS_INDEXED_LABEL_SET(entry.label,in_label,1);
    
    /* egress attribures*/
    entry.egress_label.label = eg_label;
    entry.egress_if = egress_intf;
    if(rif) {
       entry.ingress_if = rif;
       entry.flags |= BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF;
    } else {
       entry.vpn = vrf;
    }
    entry.port = mpls_lsr_info_1.in_port; /* This is relevant only in case of mpls context port */
        
    rv = bcm_mpls_tunnel_switch_create(unit,&entry);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    return rv;
}

int
mpls_lsr_context_with_default(int unit,int in_port, int out_port, int use_vrf){
    int rv;

    mpls_lsr_init(in_port,out_port,0x11,0x22,5000,8000,100,200,0);

    rv = mpls_lsr_config_init(unit);
    if(rv){
        printf("mpls_lsr_with_vrf_context_example failed rv=%d",rv);
    }

    rv = mpls_add_switch_entry_with_context(unit,mpls_lsr_info_1.in_label,mpls_lsr_info_1.eg_label,mpls_lsr_info_1.egress_intf,
                                            use_vrf?lsr_vrf:0,
                                            use_vrf?0:100);
    if(rv){
        printf("mpls_lsr_with_vrf_context_example failed rv=%d",rv);
    }

    return rv;
}
