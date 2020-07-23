/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*  
 * test 1: 
 * run: 
 * cint ../sand/utility/cint_sand_utils_mpls.c
 * cint utility/cint_utils_multicast.c  
 * cint cint_mpls_mc.c
 * cint
 * mpls_mc_test1(unit, <port_1>, <port_2>); 
 *  
 * run packet: 
 *      ethernet header with DA 0x11 and any SA
 *      vlan tag with vlan tag id 100
 *      mpls header with label 5000
 * from <port_1> 
 *  
 * the packet will arrive at <port_2> with: 
 *      ethernet header with DA 0x22 and SA 0x11
 *      vlan tag with vlan tag id 200
 *      mpls header with label_1 8000 and label_2 6000
 * after swap of label 5000 with label 6000 and push of label 8000 
 *  
 * test2: 
 * run: 
 * cint cint_mpls_mc.c
 * cint 
 * mpls_mc_test2(unit, <port_1>, <port_2>);  
 *  
 * run same packet (as for test 1)
 *  
 * the packet will arrive at <port_2> with: 
 *      ethernet header with DA 0x22 and SA 0x11
 *      vlan tag with vlan tag id 200
 *      mpls header with label_1 300 and label_2 6000
 * after swap of label 5000 with label 6000 and push of label 300 by tunnel 1
 *  
 * or: 
 *      ethernet header with DA 0x44 and SA 0x11
 *      vlan tag with vlan tag id 201
 *      mpls header with label_1 400, label_2 200 and lable_3 6000
 * after swap of label 5000 with label 6000 and push of labels 400,200 by tunnel 2 
 *  
 * run packet: 
 *      ethernet header with DA 0x33 and any SA
 *      vlan tag with vlan tag id 101
 *      mpls header with label 5001
 * from <port_1>
 * 
 * the packet will arrive at <port_2> with looking the same,
 * except for the last label, which will be 6001 instead of 6000 
 *  
 * test3: 
 * run: 
 * cint ../sand/utility/cint_sand_utils_global.c 
 * cint ../sand/utility/cint_sand_utils_mpls.c
 * cint cint_mpls_mc.c
 * cint 
 * mpls_mc_test3_explicit_flags_example(unit, <port_1>, <port_2>);  
 *  
 * run same packet (as for test 1). One change: above MPLS header add L3 (IPV4) header 
 * Test example the ability to use explicit flags to indicate ACTION on MPLS tunnel initator.
 * Also provides ability to indicate ILM to do NOP action. 
 *  
 * Expectation in test is that label manipulation will be done only at egress (SWAP, PHP, PUSH, SWAP & PUSH, PUSH & PUSH)
 * five packets will arrive at <port_2> with: 
 * MPLS SWAP: 
 *      ethernet header with DA 0x22 and SA 0x11
 *      vlan tag with vlan tag id 200
 *      after swap of label 5000 with label 6000
 * MPLS PHP (to IPV4): 
 *      ethernet header with DA 0x22 and SA 0x11
 *      vlan tag with vlan tag id 200
 *      l3 (ipv4) header: TTL, TOS not changed
 * MPLS PUSH: 
 *      ethernet header with DA 0x22 and SA 0x11
 *      vlan tag with vlan tag id 200
 *      mpls header with label_1 400, label_2 200 and label_3 5000 (no swap)
 * MPLS SWAP & PUSH
 *      ethernet header with DA 0x22 and SA 0x11
 *      vlan tag with vlan tag id 200
 *      mpls header with label_1 400, label_2 200 and label_3 6000 (swap)
 * MPLS PUSH & PUSH (4 tunnels) 
 *      ethernet header with DA 0x22 and SA 0x11
 *      vlan tag with vlan tag id 200
 *      mpls header with label_1 400, label_2 200 and label_3 800, label_4 600, label_5 5000 (no swap)
 */ 

struct mpls_mc_info_s {
   
    /* ingress interface attribures: */

    int in_port; /* phy port */

    /* outer vlan */
    int vid_1;
    int vid_2;

    bcm_mac_t my_mac_1;
    bcm_mac_t my_mac_2;

    /* incomming label */
    int in_label_1; 
    int in_label_2; 

    int swap_label_1;
    int swap_label_2;

    /* ingress objects, packets will be routed to */
    bcm_if_t l3_ingress_intf_1;
    bcm_if_t l3_ingress_intf_2; 
  
    /* egress attribures: */

    int eg_port; /* phy port */

    /* egress label*/
    int eg_label_1;
    int eg_label_2;

    int eg_vid_1;
    int eg_vid_2;

    bcm_mac_t next_hop_mac_1;
    bcm_mac_t next_hop_mac_2;

    bcm_if_t l3_egress_intf_1;
    bcm_if_t l3_egress_intf_2;

    bcm_if_t encap_id_1;
    bcm_if_t encap_id_2;

    int mc_group;
};

mpls_mc_info_s mpls_mc_info;

int exp = 0;

void
mpls_mc_init(
    int in_port, 
    int out_port, 
    int in_label_base, 
    int swap_label_base, 
    int out_label_base, 
    int in_vlan_base, 
    int out_vlan_base) {

    mpls_mc_info.in_port = in_port;
    mpls_mc_info.eg_port = out_port;

    uint8 mac_1[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
    uint8 next_mac_1[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x22};
    sal_memcpy(mpls_mc_info.my_mac_1, mac_1, 6);
    sal_memcpy(mpls_mc_info.next_hop_mac_1, next_mac_1, 6);

    uint8 mac_2[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33};
    uint8 next_mac_2[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x44};
    sal_memcpy(mpls_mc_info.my_mac_2, mac_2, 6);
    sal_memcpy(mpls_mc_info.next_hop_mac_2, next_mac_2, 6);

    mpls_mc_info.mc_group = 5000;

    /* incomming */ 
    mpls_mc_info.vid_1 = in_vlan_base++;
    mpls_mc_info.vid_2 = in_vlan_base++;

    mpls_mc_info.in_label_1 = in_label_base++;
    mpls_mc_info.in_label_2 = in_label_base++;

    /* eg atrributes */
    mpls_mc_info.eg_label_1 = out_label_base++;
    mpls_mc_info.eg_label_2 = out_label_base++;

    mpls_mc_info.eg_vid_1 = out_vlan_base++;
    mpls_mc_info.eg_vid_2 = out_vlan_base++;

    mpls_mc_info.swap_label_1 = swap_label_base++;
    mpls_mc_info.swap_label_2 = swap_label_base++;

    mpls_mc_info.encap_id_1 = 0;
    mpls_mc_info.encap_id_2 = 0;
}

/* add switch entry to perform swap
   swap in_label with eg_label
   and point to egress-object: egress_intf, returned by create_l3_egress
   In case eg_label = 0 sequence set action to be NOP (i.e. dont touch label)
*/
int
mpls_add_switch_entry(int unit, int in_label, int eg_label,  bcm_if_t egress_intf, int multicast)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;
    
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = (eg_label == BCM_MPLS_LABEL_INVALID) ? BCM_MPLS_SWITCH_ACTION_NOP : BCM_MPLS_SWITCH_ACTION_SWAP;
    /* TTL decrement has to be present */
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    /* Uniform: inherit TTL and EXP, 
       in general valid options: 
       both present (uniform) or none of them (Pipe)
    */
    if (!is_device_or_above(unit, JERICHO2)){
        /* in JR2 supported only for PHP and POP_DIRECT */
        entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL | BCM_MPLS_SWITCH_OUTER_EXP;
    }
    
    /* incomming label */
    entry.label = in_label;

    /* egress attribures*/
    entry.egress_label.label = eg_label;
    entry.egress_if = egress_intf;

    if (multicast) {
        /* add multicast group to entry */
        entry.flags |= BCM_MPLS_SWITCH_P2MP;
        entry.mc_group = mpls_mc_info.mc_group;
        printf("entry.mc_group:%08x\n", entry.mc_group);
    }

    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    return rv;
}

/* 
 * single_config:  if =1 then only 1 vlan, tunnel, ingress_intf, egress_obj and switch_entry will be done
 *                 if =0 then 2 of each will be done
 * l3_egress_push: if =1 then l3_egress_create will config push of eg_label 
 *                 if =0 then no mpls action will be done in l3_egress_create
 * multicast:      if =1 a multicast group is used when creating mpls_add_switch_entry 
 *                 if =0 no multicast group is used
 */
int 
mpls_mc_config(int unit, int in_port, int out_port, int single_config, int l3_egress_push, int multicast){

    int CINT_NO_FLAGS = 0;
    int rv;
    int tmp_itf;
    int ingress_intf;
    bcm_pbmp_t pbmp, ubmp;
    mpls__egress_tunnel_utils_s mpls_tunnel_properties;

    mpls_mc_init(in_port, out_port, 5000 /*in_label*/, 6000 /*swap_label*/, 8000 /*out_label*/, 100 /*vlan*/, 200 /*out_vlan*/);
  
    /* create 2 vlans - incomming,outgoing and add ports to them */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, mpls_mc_info.in_port);

    rv = bcm_vlan_create(unit, mpls_mc_info.vid_1);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_vlan_create with vid = %d\n", mpls_mc_info.vid_1);
        return rv;
    }
    rv = bcm_vlan_port_add(unit, mpls_mc_info.vid_1, pbmp, ubmp);
    if (BCM_FAILURE(rv)) {
        printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return CMD_FAIL;
    }
       
    if (!single_config) {
        rv = bcm_vlan_create(unit, mpls_mc_info.vid_2);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in bcm_vlan_create with vid = %d\n", mpls_mc_info.vid_2);
            return rv;
        }
        rv = bcm_vlan_port_add(unit, mpls_mc_info.vid_2, pbmp, ubmp);
        if (BCM_FAILURE(rv)) {
            printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
            return CMD_FAIL;
        }  
    }
    
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, mpls_mc_info.eg_port); 

    rv = bcm_vlan_create(unit,mpls_mc_info.eg_vid_1);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_vlan_create with vid = %d\n", mpls_mc_info.eg_vid_1);
        return rv;
    }
    rv = bcm_vlan_port_add(unit, mpls_mc_info.eg_vid_1, pbmp, ubmp);
    if (BCM_FAILURE(rv)) {
        printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return CMD_FAIL;
    }  

    if (!single_config) {
        rv = bcm_vlan_create(unit,mpls_mc_info.eg_vid_2);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in bcm_vlan_create with vid = %d\n", mpls_mc_info.eg_vid_2);
            return rv;
        }
        
        rv = bcm_vlan_port_add(unit, mpls_mc_info.eg_vid_2, pbmp, ubmp);
        if (BCM_FAILURE(rv)) {
            printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
            return CMD_FAIL;
        }  
    }

    /* set l2 termination for mpls routing for the in_vlan */
    create_l3_intf_s intf;
    intf.vsi = mpls_mc_info.vid_1;
    intf.my_global_mac = mpls_mc_info.my_mac_1;
    intf.my_lsb_mac = mpls_mc_info.my_mac_1;
    
    rv = l3__intf_rif__create(unit, &intf);
    mpls_mc_info.l3_ingress_intf_1 = intf.rif;
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create");
    }
    
    if (!single_config) {
        intf.vsi = mpls_mc_info.vid_2;
        intf.my_global_mac = mpls_mc_info.my_mac_2;
        intf.my_lsb_mac = mpls_mc_info.my_mac_2;

        rv = l3__intf_rif__create(unit, &intf);
        mpls_mc_info.l3_ingress_intf_1 = intf.rif;
        if (rv != BCM_E_NONE) {
            printf("Error, l3__intf_rif__create");
        }
    }

    /* create an ingress object, packets will be routed to */
    intf.vsi = mpls_mc_info.eg_vid_1;
    intf.my_global_mac = mpls_mc_info.my_mac_1;
    intf.my_lsb_mac = mpls_mc_info.my_mac_1;
    
    rv = l3__intf_rif__create(unit, &intf);
    mpls_mc_info.l3_ingress_intf_1 = intf.rif;
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create");
    }
    
    /* open MC-group */
    egress_mc = 0;
    int mc_group = mpls_mc_info.mc_group;
    rv = multicast__open_mc_group(unit, &mc_group, BCM_MULTICAST_TYPE_EGRESS_OBJECT);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group\n");
        return rv;
    }

    /* create egress object */
    create_l3_egress_s l3eg;
    sal_memcpy(l3eg.next_hop_mac_addr, mpls_mc_info.next_hop_mac_1 , 6);
    l3eg.allocation_flags = CINT_NO_FLAGS;
    l3eg.l3_flags = CINT_NO_FLAGS; /* Declare it's a second layer */
    l3eg.out_gport = mpls_mc_info.eg_port;
    l3eg.vlan = mpls_mc_info.eg_vid_1;
    l3eg.fec_id = mpls_mc_info.l3_egress_intf_1;
    l3eg.arp_encap_id = mpls_mc_info.encap_id_1;
    if (is_device_or_above(unit, JERICHO2)){
        /* JR2 seperate bcm_l3_egress_create for ARP and FEC */
        rv = l3__egress_only_encap__create(unit, &l3eg);
        if (rv != BCM_E_NONE) {
            printf("Error, in l3__egress_only_encap__create\n");
            return rv;
        }
        mpls_mc_info.encap_id_1 = l3eg.arp_encap_id; 
    }

    if(l3_egress_push) {
        /* JR2 does not support action PUSH, and BCM_MPLS_EGRESS_LABEL_ACTION_VALID is supported only for PHP */
        if (!is_device_or_above(unit, JERICHO2)){
            mpls_tunnel_properties.egress_action = BCM_MPLS_EGRESS_ACTION_PUSH;
            mpls_tunnel_properties.flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
        }
        mpls_tunnel_properties.label_in = mpls_mc_info.eg_label_1;
        mpls_tunnel_properties.label_out = 0;
        if (is_device_or_above(unit, JERICHO2)){
            mpls_tunnel_properties.next_pointer_intf = mpls_mc_info.encap_id_1;
        }
        else
        {
            mpls_tunnel_properties.next_pointer_intf = mpls_mc_info.l3_ingress_intf_1;
        }

        printf("Trying to create tunnel initiator\n");
        rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
        if (rv!= BCM_E_NONE) {
            printf("Error, in mpls__create_tunnel_initiator__set\n");
            return rv;
        }
        l3eg.arp_encap_id = 0;
    }

    l3eg.out_tunnel_or_rif = mpls_tunnel_properties.tunnel_id;
    if (is_device_or_above(unit, JERICHO2)){
        /* JR2 seperate bcm_l3_egress_create for ARP and FEC */
        rv = l3__egress_only_fec__create(unit, &l3eg);
        if (rv != BCM_E_NONE) {
            printf("Error, in l3__egress_only_fec__create\n");
            return rv;
        }
        mpls_mc_info.l3_egress_intf_1 = l3eg.fec_id;
    }
    else
    {
        rv = l3__egress__create(unit,&l3eg);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rv;
        }
        mpls_mc_info.l3_egress_intf_1 = l3eg.fec_id;
        mpls_mc_info.encap_id_1 = l3eg.arp_encap_id; 
    }

    if (!single_config) {
        create_l3_egress_s l3eg;
        sal_memcpy(l3eg.next_hop_mac_addr, mpls_mc_info.next_hop_mac_2 , 6);
        l3eg.allocation_flags = CINT_NO_FLAGS;
        l3eg.l3_flags = CINT_NO_FLAGS; /* Declare it's a second layer */
        l3eg.out_gport = mpls_mc_info.eg_port;
        l3eg.vlan = mpls_mc_info.eg_vid_1;
        l3eg.fec_id = mpls_mc_info.l3_egress_intf_2;
        l3eg.arp_encap_id = mpls_mc_info.encap_id_2;
        if (is_device_or_above(unit, JERICHO2)){
            /* JR2 seperate bcm_l3_egress_create for ARP and FEC */
            rv = l3__egress_only_encap__create(unit, &l3eg);
            if (rv != BCM_E_NONE) {
                printf("Error, in l3__egress_only_encap__create\n");
                return rv;
            }
            mpls_mc_info.encap_id_2 = l3eg.arp_encap_id;  
        }

        if(l3_egress_push) {
            if (is_device_or_above(unit, JERICHO2)){
                mpls_tunnel_properties.next_pointer_intf = mpls_mc_info.encap_id_2;
            }
            else
            {
                mpls_tunnel_properties.next_pointer_intf = mpls_mc_info.l3_ingress_intf_1;
            }
            mpls_tunnel_properties.label_in = mpls_mc_info.eg_label_2;
            rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
            if (rv != BCM_E_NONE) {
                printf("Error, in mpls__create_tunnel_initiator__set\n");
                return rv;
            }
        }
        l3eg.out_tunnel_or_rif = mpls_mc_info.l3_ingress_intf_1;

        if (is_device_or_above(unit, JERICHO2)){
            /* JR2 seperate bcm_l3_egress_create for ARP and FEC */
            rv = l3__egress_only_fec__create(unit, &l3eg);
            if (rv != BCM_E_NONE) {
                printf("Error, in l3__egress_only_fec__create\n");
                return rv;
            }
            mpls_mc_info.l3_egress_intf_2 = l3eg.fec_id;
        }
        else
        {
            rv = l3__egress__create(unit,&l3eg);
            if (rv != BCM_E_NONE) {
                printf("Error, l3__egress__create\n");
                return rv;
            }
            mpls_mc_info.l3_egress_intf_2 = l3eg.fec_id;
            mpls_mc_info.encap_id_2 = l3eg.arp_encap_id;  
        }
    }

    /* swap push-label with egress-label and point to egress object */
    rv = mpls_add_switch_entry(unit, mpls_mc_info.in_label_1, mpls_mc_info.swap_label_1 ,mpls_mc_info.l3_egress_intf_1, multicast);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_add_switch_entry 1\n");
        return rv;
    }

    if (!single_config) {
        rv = mpls_add_switch_entry(unit, mpls_mc_info.in_label_2, mpls_mc_info.swap_label_2 ,mpls_mc_info.l3_egress_intf_2, multicast);
        if (rv != BCM_E_NONE) {
            printf("Error, in mpls_add_switch_entry 2\n");
            return rv;
        }
    }

    return rv;
}

/* performs swap of in_label with swap_label and then push of push_label */
int 
mpls_mc_test1(int unit, int in_port, int out_port){

    int rv;

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__mpls_pipe_mode_exp_set\n");
        return rv;
    }

    rv = mpls_mc_config(unit, in_port, out_port, 1, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_mc_config\n");
        return rv;
    }

    return rv;
}

/* performs swap of in_label with swap_label and then creates a tunnel that pushes 1/2 labels */
int 
mpls_mc_test2(int unit, int in_port, int out_port){
    int rv;
    int egress_id, egress_id_base = 0x40000000;
    bcm_mpls_egress_label_t label_array[2];

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__mpls_pipe_mode_exp_set\n");
        return rv;
    }

    rv = mpls_mc_config(unit, in_port, out_port, 0, 0, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_mc_config\n");
        return rv;
    }

    /* set 2 mpls tunnels, one for each egress_intf */

    /* tunnel 1 is for 1 label */
    bcm_mpls_egress_label_t_init(&label_array[0]);

    label_array[0].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT; 
    if (is_device_or_above(unit, JERICHO2)) {
        label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    } else {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
    }
    if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
        if (is_device_or_above(unit, JERICHO2)) {
            label_array[0].exp = exp; 
            label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
        } else {
            label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
        }
    } else {
        if (is_device_or_above(unit, JERICHO2)) {
            label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        } else {
            label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        }
    }
    label_array[0].label = 300;
    label_array[0].ttl = 30;
    /* in JR2 l3_intf_id must be LIF type - pointing to arp */
    if (is_device_or_above(unit, JERICHO2)){
        label_array[0].l3_intf_id = mpls_mc_info.encap_id_1;
    } else {
        label_array[0].l3_intf_id = mpls_mc_info.l3_ingress_intf_1;
    }

    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, label_array);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    /* in JR2 no need to update ARP after MPLS creation */
    if (!is_device_or_above(unit, JERICHO2)){
        /* Connect Egress MPLS tunnel to encap_id (LL) */
        create_l3_egress_s l3eg;
        sal_memcpy(l3eg.next_hop_mac_addr, mpls_mc_info.next_hop_mac_1 , 6);
        l3eg.allocation_flags |=  BCM_L3_REPLACE;
        l3eg.out_tunnel_or_rif = label_array[0].tunnel_id;
        l3eg.vlan = mpls_mc_info.eg_vid_1;
        l3eg.arp_encap_id = mpls_mc_info.encap_id_1;

        rv = l3__egress_only_encap__create(unit,&l3eg);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress_only_encap__create\n");
            return rv;
        }

        mpls_mc_info.encap_id_1 = l3eg.arp_encap_id;
    }

    rv = multicast_mpls_encap_add(unit, mpls_mc_info.mc_group, mpls_mc_info.eg_port, label_array[0].tunnel_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast_mpls_encap_add intf = %d\n", label_array[0].tunnel_id);
        return rv;
    }  

    /* tunnel 2 is for 2 labels */
    bcm_mpls_egress_label_t_init(&label_array[0]);
    bcm_mpls_egress_label_t_init(&label_array[1]);

    label_array[0].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT; 
    if (is_device_or_above(unit, JERICHO2)) {
        label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    } else {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
    }
    if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
        if (is_device_or_above(unit, JERICHO2)) {
            label_array[0].exp = exp; 
            label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
        } else {
            label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
        }
    } else {
        if (is_device_or_above(unit, JERICHO2)) {
            label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        } else {
            label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        }
    }
    label_array[0].label = 200;
    label_array[0].ttl = 20;
    /* in JR2 l3_intf_id must be LIF type - pointing to arp */
    if (is_device_or_above(unit, JERICHO2)){
        label_array[0].l3_intf_id = mpls_mc_info.encap_id_1;
    } else {
        label_array[0].l3_intf_id = mpls_mc_info.l3_ingress_intf_1;
    }

    label_array[1].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT; 
    if (is_device_or_above(unit, JERICHO2)) {
        label_array[1].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    } else {
        label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
    }
    if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
         label_array[1].exp = label_array[0].exp;
        if (is_device_or_above(unit, JERICHO2)) {
            label_array[1].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
        } else {
            label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
        }
    } else {
        if (is_device_or_above(unit, JERICHO2)) {
            label_array[1].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        } else {
            label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        }
    }
    label_array[1].label = 400;
    /* in JR2 ttl and l3_intf_id values must be similar for both label1 and label2 on same EEDB entry */
    if(is_device_or_above(unit, JERICHO2))
    {
        label_array[1].ttl = label_array[0].ttl;
        label_array[1].l3_intf_id = label_array[0].l3_intf_id;
    }
    else
    {
        label_array[1].ttl = 40;
        label_array[1].l3_intf_id = mpls_mc_info.l3_ingress_intf_1;
    }

    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 2, label_array);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    /* in JR2 no need to update ARP after MPLS creation */
    if (!is_device_or_above(unit, JERICHO2)){
        /* Connect Egress MPLS tunnel to encap_id (LL) */
        create_l3_egress_s l3eg1;
        sal_memcpy(l3eg1.next_hop_mac_addr, mpls_mc_info.next_hop_mac_1 , 6);
        l3eg1.allocation_flags |= BCM_L3_REPLACE;
        l3eg1.out_tunnel_or_rif = label_array[0].tunnel_id;
        l3eg1.vlan = mpls_mc_info.eg_vid_1;
        l3eg1.arp_encap_id = mpls_mc_info.encap_id_1;

        rv = l3__egress_only_encap__create(unit,&l3eg1);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress_only_encap__create\n");
            return rv;
        }

        mpls_mc_info.encap_id_1 = l3eg1.arp_encap_id;
    }

    rv = multicast_mpls_encap_add(unit, mpls_mc_info.mc_group, mpls_mc_info.eg_port, label_array[0].tunnel_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast_mpls_encap_add intf = %d\n", label_array[0].tunnel_id);
        return rv;
    }  

    return rv;
}

int
mpls_mc_test3_explicit_flags_example_aux(int unit, int in_port, int out_port, int pipe_mode_exp, int expected){

   mpls_pipe_mode_exp_set = pipe_mode_exp;

   exp = expected;

   return mpls_mc_test3_explicit_flags_example(unit,in_port,out_port);
}


/* 
 * Test example the ability to use explicit flags to indicate ACTION on MPLS tunnel initator.
 * Also provides ability to indicate ILM to do NOP action                                   .
 * See comments at the start of the file for more information.                                                                                         .
 */  
int 
mpls_mc_test3_explicit_flags_example(int unit, int in_port, int out_port){
    int rv;
    int egress_id, egress_id_base = 0x40000000;
    bcm_mpls_egress_label_t label_array[2];
    bcm_if_t push_tunnel_intf;
    int CINT_NO_FLAGS = 0;    
    int tmp_itf;
    bcm_if_t ingress_intf;
    bcm_pbmp_t pbmp, ubmp;
    int is_jer_b0_or_jer_plus_or_qux;
    int mpls_termination_label_index_enable;
   	int mpls_termination_label_index_database_mode;

   	mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
   	mpls_termination_label_index_database_mode = soc_property_get(unit , spn_BCM886XX_MPLS_TERMINATION_DATABASE_MODE, 0);

    mpls__egress_tunnel_utils_s mpls_tunnel_properties1,mpls_tunnel_properties2;

    rv = is_jericho_b0_only(unit, &is_jer_b0_or_jer_plus_or_qux);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) is_jericho_b0_only\n", rv);
        return rv;
    }
    if (!is_jer_b0_or_jer_plus_or_qux) {
        rv = is_qumran_mx_b0_only(unit, &is_jer_b0_or_jer_plus_or_qux);
        if (rv != BCM_E_NONE) {
            printf("Failed(%d) is_qumran_mx_b0_only\n", rv);
            return rv;
        }
    }
    if (!is_jer_b0_or_jer_plus_or_qux) {
        rv = is_jericho_plus_only(unit, &is_jer_b0_or_jer_plus_or_qux);
        if (rv != BCM_E_NONE) {
            printf("Failed(%d) is_qumran_mx_b0_only\n", rv);
            return rv;
        }
    }

    is_jer_b0_or_jer_plus_or_qux |= is_device_or_above(unit,QUMRAN_UX);

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__mpls_pipe_mode_exp_set\n");
        return rv;
    }

    if ((mpls_termination_label_index_enable == 1) &&
        ((mpls_termination_label_index_database_mode == 22) || (mpls_termination_label_index_database_mode == 23))) {
    	rv = bcm_vlan_control_port_set(unit, in_port, bcmVlanPortDoubleLookupEnable, 1);

		if (rv != BCM_E_NONE) {
			printf("(%s) \n",bcm_errmsg(rv));
			return rv;
		}

		/* when a port is configured with  "bcmVlanPortDoubleLookupEnable" the VLAN domain must be unique in the device */
		rv = bcm_port_class_set(unit, in_port, bcmPortClassId, in_port);
		if (rv != BCM_E_NONE) {
			printf("Error, bcm_port_class_set %d\n", rv);
			return rv;
		}
    }

    mpls_mc_init(in_port, out_port, 5000 /*in_label*/, 6000 /*swap_label*/, 8000 /*out_label*/, 100 /*vlan*/, 200 /*out_vlan*/);
    mpls_mc_info.mc_group = 6293;
  
    /* create 2 vlans - incomming,outgoing and add ports to them */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, mpls_mc_info.in_port);

    rv = bcm_vlan_create(unit, mpls_mc_info.vid_1);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_vlan_create with vid = %d\n", mpls_mc_info.vid_1);
        return rv;
    }
    rv = bcm_vlan_port_add(unit, mpls_mc_info.vid_1, pbmp, ubmp);
    if (BCM_FAILURE(rv)) {
        printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return CMD_FAIL;
    }
       
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, mpls_mc_info.eg_port); 

    rv = bcm_vlan_create(unit,mpls_mc_info.eg_vid_1);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_vlan_create with vid = %d\n", mpls_mc_info.eg_vid_1);
        return rv;
    }
    rv = bcm_vlan_port_add(unit, mpls_mc_info.eg_vid_1, pbmp, ubmp);
    if (BCM_FAILURE(rv)) {
        printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return CMD_FAIL;
    }  

    /* set l2 termination for mpls routing for the in_vlan */
    create_l3_intf_s intf;
    intf.vsi = mpls_mc_info.vid_1;
    intf.my_global_mac = mpls_mc_info.my_mac_1;
    intf.my_lsb_mac = mpls_mc_info.my_mac_1;
    
    rv = l3__intf_rif__create(unit, &intf);
    mpls_mc_info.l3_ingress_intf_1 = intf.rif;
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create");
    }
    
    /* create an ingress object, packets will be routed to */
    intf.vsi = mpls_mc_info.eg_vid_1;
    
    rv = l3__intf_rif__create(unit, &intf);
    mpls_mc_info.l3_ingress_intf_1 = intf.rif;
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create");
    }
  
    /* open MC-group */
    egress_mc = 0;
	int mc_group = mpls_mc_info.mc_group;
    rv = multicast__open_mc_group(unit, &mc_group, BCM_MULTICAST_TYPE_EGRESS_OBJECT);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group\n");
        return rv;
    }

    /* create egress object */
    create_l3_egress_s l3eg1;
    sal_memcpy(l3eg1.next_hop_mac_addr, mpls_mc_info.next_hop_mac_1 , 6);
    l3eg1.allocation_flags = CINT_NO_FLAGS;
    l3eg1.l3_flags = CINT_NO_FLAGS; /* Declare it's a second layer */
    l3eg1.out_tunnel_or_rif = mpls_mc_info.l3_ingress_intf_1;
    l3eg1.out_gport = mpls_mc_info.eg_port;
    l3eg1.vlan = mpls_mc_info.eg_vid_1;
    l3eg1.fec_id = mpls_mc_info.l3_egress_intf_1;
    l3eg1.arp_encap_id = mpls_mc_info.encap_id_1;

    rv = l3__egress__create(unit,&l3eg1);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    mpls_mc_info.l3_egress_intf_1 = l3eg1.fec_id;
    mpls_mc_info.encap_id_1 = l3eg1.arp_encap_id;
	
    /* do not swap label (Action NOP) and point to egress object */
    rv = mpls_add_switch_entry(unit, mpls_mc_info.in_label_1, BCM_MPLS_LABEL_INVALID /* NOP */ , 0 , 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_add_switch_entry\n");
        return rv;
    }

    /* set 4 mpls egress tunnels, one for each action */

    /* tunnel 1 is for PHP action */
    
    /* in JR2 l3_intf_id must be LIF type - pointing to arp */
    if (is_device_or_above(unit, JERICHO2)){
        ingress_intf = mpls_mc_info.encap_id_1;
    } else {
        ingress_intf = mpls_mc_info.l3_ingress_intf_1;
    }

    mpls_tunnel_properties1.egress_action = BCM_MPLS_EGRESS_ACTION_PHP;
    mpls_tunnel_properties1.flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
    if (is_device_or_above(unit, JERICHO2)){
        mpls_tunnel_properties1.label_in = BCM_MPLS_LABEL_INVALID;
    } else {
        mpls_tunnel_properties1.label_in = 0;
    }
    mpls_tunnel_properties1.label_out = 0;
    mpls_tunnel_properties1.next_pointer_intf = ingress_intf;

    printf("Trying to create tunnel initiator\n");
    rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties1);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__create_tunnel_initiator__set\n");
        return rv;
    }

    ingress_intf = mpls_tunnel_properties1.tunnel_id;

    /* Connect Egress MPLS PHP to encap_id (LL) */
    create_l3_egress_s l3eg;
    sal_memcpy(l3eg.next_hop_mac_addr, mpls_mc_info.next_hop_mac_1 , 6);
    l3eg.allocation_flags |= BCM_L3_REPLACE;
    l3eg.out_tunnel_or_rif = ingress_intf;
    l3eg.vlan = mpls_mc_info.eg_vid_1;
    l3eg.arp_encap_id = mpls_mc_info.encap_id_1;

    rv = l3__egress_only_encap__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_encap__create\n");
        return rv;
    }

    mpls_mc_info.encap_id_1 = l3eg.arp_encap_id;

    /* Connect multicast group to MPLS PHP */
    rv = multicast_mpls_encap_add(unit, mpls_mc_info.mc_group, mpls_mc_info.eg_port, ingress_intf);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast_mpls_encap_add intf = %d\n", ingress_intf);
        return rv;
    }  

   /* tunnel 2 is for SWAP action */
   if (is_device_or_above(unit, JERICHO2)){
       ingress_intf = mpls_mc_info.encap_id_1;
   } else {
       ingress_intf = mpls_mc_info.l3_ingress_intf_1;
   }

   mpls_tunnel_properties2.tunnel_id = 0;
   mpls_tunnel_properties2.egress_action = BCM_MPLS_EGRESS_ACTION_SWAP;
   /* JR2 does not support action SWAP, and BCM_MPLS_EGRESS_LABEL_ACTION_VALID is supported only for PHP */
   if (!is_device_or_above(unit, JERICHO2)){
       mpls_tunnel_properties2.flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
   }
   mpls_tunnel_properties2.label_in = mpls_mc_info.swap_label_1;
   mpls_tunnel_properties2.label_out = 0;
   mpls_tunnel_properties2.next_pointer_intf = ingress_intf;

   rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties2);
   if (rv != BCM_E_NONE) {
       printf("Error, in mpls__create_tunnel_initiator__set\n");
       return rv;
   }

   ingress_intf = mpls_tunnel_properties2.tunnel_id;

    /* Connect Egress MPLS SWAP to encap_id (LL) */
    sal_memcpy(l3eg.next_hop_mac_addr, mpls_mc_info.next_hop_mac_1 , 6);
    l3eg.allocation_flags |= BCM_L3_REPLACE;
    l3eg.out_tunnel_or_rif = ingress_intf;
    l3eg.vlan = mpls_mc_info.eg_vid_1;
    l3eg.arp_encap_id = mpls_mc_info.encap_id_1;

    rv = l3__egress_only_encap__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_encap__create\n");
        return rv;
    }

    mpls_mc_info.encap_id_1 = l3eg.arp_encap_id;


   /* Connect multicast group to MPLS SWAP */
   rv = multicast_mpls_encap_add(unit, mpls_mc_info.mc_group, mpls_mc_info.eg_port, ingress_intf);
   if (rv != BCM_E_NONE) {
       printf("Error, in multicast_mpls_encap_add intf = %d\n", ingress_intf);
       return rv;
   }  

   /* tunnel 3 is for PUSH action */
   /* in JR2 l3_intf_id must be LIF type - pointing to arp */
   if (is_device_or_above(unit, JERICHO2)){
       ingress_intf = mpls_mc_info.encap_id_1;
   } else {
       ingress_intf = mpls_mc_info.l3_ingress_intf_1;
   }

   bcm_mpls_egress_label_t_init(&label_array[0]);
   bcm_mpls_egress_label_t_init(&label_array[1]);

   label_array[0].exp = exp; 
   label_array[0].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;  
   if(is_device_or_above(unit, JERICHO2))
   {
       label_array[0].egress_qos_model.egress_ttl= bcmQosEgressModelPipeMyNameSpace;
   }
   else
   {
       label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET | BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
   }
   if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
        if(is_device_or_above(unit, JERICHO2))
        {
            label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
        }
        else
        {
            label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
        }
   } else {
        if(is_device_or_above(unit, JERICHO2))
        {
            label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        }
        else
        {
            label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        }
   }
   label_array[0].label = 200;
   if(is_device_or_above(unit, JERICHO2)){
       label_array[0].action = BCM_MPLS_EGRESS_ACTION_SWAP;
       label_array[0].encap_access = bcmEncapAccessTunnel3;
    }
   else if (is_jer_b0_or_jer_plus_or_qux) {
       label_array[0].action = BCM_MPLS_EGRESS_ACTION_SWAP_OR_PUSH;
   }
   else {
       label_array[0].action = BCM_MPLS_EGRESS_ACTION_PUSH;
   }
   label_array[0].ttl = 20;
   if(is_device_or_above(unit, JERICHO2))
   {
       label_array[0].l3_intf_id = ingress_intf;
   }

   label_array[1].exp = exp; 
   label_array[1].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;  
   if(is_device_or_above(unit, JERICHO2))
   {
       label_array[1].egress_qos_model.egress_ttl= bcmQosEgressModelPipeMyNameSpace;
   }
   else
   {
       label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
   }
   if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
        if(is_device_or_above(unit, JERICHO2))
        {
            label_array[1].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
        }
        else
        {
            label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
        }
   } else {
        if(is_device_or_above(unit, JERICHO2))
        {
            label_array[1].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        }
        else
        {
            label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        }
   }
   label_array[1].label = 400;
    if(is_device_or_above(unit, JERICHO2)){
        label_array[1].ttl = label_array[0].ttl;
        label_array[1].action = label_array[0].action;
        label_array[1].l3_intf_id = label_array[0].l3_intf_id;
        label_array[1].encap_access = label_array[0].encap_access;
    } else {
        label_array[1].ttl = 40;
        if (is_jer_b0_or_jer_plus_or_qux) {
            label_array[1].action = BCM_MPLS_EGRESS_ACTION_SWAP_OR_PUSH;
        }
        else {
            label_array[1].action = BCM_MPLS_EGRESS_ACTION_PUSH;
        }
        label_array[1].l3_intf_id = ingress_intf;
    }

   rv = bcm_mpls_tunnel_initiator_create(unit, 0, 2, label_array);
   if (rv != BCM_E_NONE) {
       printf("Error, in bcm_mpls_tunnel_initiator_create\n");
       return rv;
   }
   ingress_intf = label_array[0].tunnel_id;
   push_tunnel_intf = label_array[0].tunnel_id; /* save that for later use */

    /* Connect Egress MPLS PUSH to encap_id (LL) */
    sal_memcpy(l3eg.next_hop_mac_addr, mpls_mc_info.next_hop_mac_1 , 6);
    l3eg.allocation_flags |= BCM_L3_REPLACE;
    l3eg.out_tunnel_or_rif = ingress_intf;
    l3eg.vlan = mpls_mc_info.eg_vid_1;
    l3eg.arp_encap_id = mpls_mc_info.encap_id_1;

    rv = l3__egress_only_encap__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_encap__create\n");
        return rv;
    }

    mpls_mc_info.encap_id_1 = l3eg.arp_encap_id;

   /* Connect multicast group to MPLS PUSH */
   rv = multicast_mpls_encap_add(unit, mpls_mc_info.mc_group, mpls_mc_info.eg_port, ingress_intf);
   if (rv != BCM_E_NONE) {
       printf("Error, in multicast_mpls_encap_add intf = %d\n", ingress_intf);
       return rv;
   }  

   /* tunnel 4 is for SWAP & PUSH actions */
   /* Use the same PUSH tunnels from tunnel 3 */
   /* On top of them operate a SWAP action */
   ingress_intf = label_array[0].tunnel_id;

   bcm_mpls_egress_label_t_init(&label_array[0]);
   bcm_mpls_egress_label_t_init(&label_array[1]);

   label_array[0].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;  
   
   /* JR2 does not support action SWAP, and BCM_MPLS_EGRESS_LABEL_ACTION_VALID is supported only for PHP */
   if (!is_device_or_above(unit, JERICHO2)){
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
   }
   label_array[0].label = mpls_mc_info.swap_label_1;
   label_array[0].action = BCM_MPLS_EGRESS_ACTION_SWAP;
   label_array[0].l3_intf_id = push_tunnel_intf; /* Connect SWAP action to MPLS tunnel push */

   rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, label_array);
   if (rv != BCM_E_NONE) {
       printf("Error, in bcm_mpls_tunnel_initiator_create\n");
       return rv;
   }
   ingress_intf = label_array[0].tunnel_id;

   /* Connect multicast group to MPLS SWAP */
   rv = multicast_mpls_encap_add(unit, mpls_mc_info.mc_group, mpls_mc_info.eg_port, ingress_intf);
   if (rv != BCM_E_NONE) {
       printf("Error, in multicast_mpls_encap_add intf = %d\n", ingress_intf);
       return rv;
   }  

   /* tunnel 4 is for SWAP & PUSH actions */
   /* Use the same PUSH tunnels from tunnel 3 */
   /* On top of them operate a SWAP action */
   ingress_intf = label_array[0].tunnel_id;

   bcm_mpls_egress_label_t_init(&label_array[0]);
   bcm_mpls_egress_label_t_init(&label_array[1]);

   
   label_array[0].exp = exp; 
   label_array[0].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;  
   if(is_device_or_above(unit, JERICHO2))
   {
       label_array[0].egress_qos_model.egress_ttl= bcmQosEgressModelPipeMyNameSpace;
   }
   else
   {
       label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
   }
   if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
       if(is_device_or_above(unit, JERICHO2))
        {
            label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
        }
        else
        {
            label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
        }
   } else {
        if(is_device_or_above(unit, JERICHO2))
        {
            label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        }
        else
        {
            label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        }
   }
   label_array[0].label = 600;
   if(is_device_or_above(unit, JERICHO2)){
       label_array[0].action = BCM_MPLS_EGRESS_ACTION_SWAP;
       label_array[0].l3_intf_id = push_tunnel_intf; /* Connect SWAP action to MPLS tunnel push */
    }
   else if (is_jer_b0_or_jer_plus_or_qux) {
       label_array[0].action = BCM_MPLS_EGRESS_ACTION_SWAP_OR_PUSH;
   }
   else {
       label_array[0].action = BCM_MPLS_EGRESS_ACTION_PUSH;
   }      
   label_array[0].ttl = 60;

   label_array[1].exp = exp; 
   label_array[1].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    if(is_device_or_above(unit, JERICHO2))
    {
        label_array[1].egress_qos_model.egress_ttl= bcmQosEgressModelPipeMyNameSpace;
    }
    else
    {
        label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
    }
    if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
        if(is_device_or_above(unit, JERICHO2))
        {
            label_array[1].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
        }
        else
        {
            label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
        }
    } else {
        if(is_device_or_above(unit, JERICHO2))
        {
            label_array[1].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        }
        else
        {    
            label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        }
   }
   label_array[1].label = 800;
    if(is_device_or_above(unit, JERICHO2)){
        label_array[1].action = BCM_MPLS_EGRESS_ACTION_SWAP;
        label_array[1].ttl = label_array[0].ttl;
        label_array[1].l3_intf_id = label_array[0].l3_intf_id;
    }
    else
    {
        if (is_jer_b0_or_jer_plus_or_qux) {
            label_array[1].action = BCM_MPLS_EGRESS_ACTION_SWAP_OR_PUSH;
        }
        else {
            label_array[1].action = BCM_MPLS_EGRESS_ACTION_PUSH;
        }   
        label_array[1].ttl = 40;
        label_array[1].l3_intf_id = push_tunnel_intf; /* Connect SWAP action to MPLS tunnel push */
    }

   rv = bcm_mpls_tunnel_initiator_create(unit, 0, 2, label_array);
   if (rv != BCM_E_NONE) {
       printf("Error, in bcm_mpls_tunnel_initiator_create\n");
       return rv;
   }
   ingress_intf = label_array[0].tunnel_id;

   /* Connect multicast group to MPLS PUSH & PUSH */
   rv = multicast_mpls_encap_add(unit, mpls_mc_info.mc_group, mpls_mc_info.eg_port, ingress_intf);
   if (rv != BCM_E_NONE) {
       printf("Error, in multicast_mpls_encap_add intf = %d\n", ingress_intf);
       return rv;
   }  

    return rv;
}

/*
 * add gport of type vlan-port to the multicast
 */
int multicast_mpls_encap_add(int unit, int mc_group_id, int sys_port, bcm_if_t egress_itf){

    int encap_id;
    int rv;

    rv = bcm_multicast_egress_object_encap_get(unit, mc_group_id, egress_itf, &encap_id);
    printf("mc_group_id = 0x%x, egress_itf = 0x%x, encap_id = 0x%x\n", mc_group_id, egress_itf, encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_egress_object_encap_get mc_group_id:  0x%08x   gport:  0x%08x \n", mc_group_id, egress_itf);
        return rv;
    }

    if (is_device_or_above(unit, JERICHO2)){
        /* in JR2 bcm_multicast_add replaces bcm_multicast_ingress_add */
        bcm_multicast_replication_t rep;

        bcm_multicast_replication_t_init(&rep);
        rep.port = sys_port;
        rep.encap1 = BCM_L3_ITF_VAL_GET(encap_id);
        
        rv = bcm_multicast_add(unit, mc_group_id, BCM_MULTICAST_INGRESS_GROUP, 1, &rep);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_multicast_add mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", mc_group_id, sys_port, encap_id);
            return rv;
        }
    }
    else
    {
        rv = bcm_multicast_ingress_add(unit, mc_group_id, sys_port, encap_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_multicast_ingress_add mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", mc_group_id, sys_port, encap_id);
            return rv;
        }
    }

    printf("encap_id:%08x\n", encap_id);
    printf("mc_group_id:%08x\n", mc_group_id);

    return rv;
}
