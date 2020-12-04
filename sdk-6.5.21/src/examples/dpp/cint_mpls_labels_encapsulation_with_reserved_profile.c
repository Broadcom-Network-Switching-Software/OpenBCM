/*~~~~~~~~~~~~~~~~~~~~~~~7 MPLS labels ENCAPSULATION~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_mpls_labels_encapsulation_with_reserved_profile.c
 * Purpose: Example of up to 7 MPLS label encapsulation with reserved mpls profile.
 *
  *In this solution,
 *        J+ supports 2 mpls EEDB in one EES,
 *        they are MPLS-tunnel1 and MPLS-tunnel2, each of them could encap up to 2 mpls labels.To support 7 labels
 *        encapsulation, a Data entry is added into the EES, and reserved MPLS profile is used to encap 3 MPLS 
 *        lables using the data stored in the Data entry.
 *         Output packet carry 7 MPLS labels:IPv4oL1oL2oL3oL4oL5oL6oL7oEth.
 *        - The Labels (L1,L2) are encapsulated by  mpls-tunnel-1, Labels(L6,L7)  are encapsulated by mpls-tunnel-2, 
 *           Labels(L3,L4,L5) are encapsulated by reserved mpls profile
 *        - The (L3,L4,L5) tunnel is created with bcm_mpls_tunnel_initiator_create(), setting flag 
 *           BCM_MPLS_EGRESS_LABEL_PRESERVE,
 *        - The (L6,L7) tunnel is created with bcm_mpls_tunnel_initiator_create(), setting spl_label_push_type 
 *           as bcmMplsSpecialLabelPushMeaningfulXXX to indicate how to use the reserved mpls label entry
 *        - The (L1,L2) tunnel is created with bcm_mpls_tunnel_initiator_create(), setting encap_access as 
 *           bcmEncapAccessTunnel1, indicatint it should use the bank phase number 1.

 *
 *   The EEDB entries link list just like:
 *    MPLS tunnel 1(L1,L2)--->MPLS tunnel 2(L6,L7)---->Data entry(L3,L4,L5)--->Link layer
 */
/*
 * Test Scenario 
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dpp/cint_queue_tests.c
 * cint ../../src/examples/dpp/utility/cint_utils_multicast.c
 * cint ../../src/examples/dpp/utility/cint_utils_vlan.c
 * cint ../../src/examples/dpp/utility/cint_utils_l3.c
 * cint ../../src/examples/dpp/utility/cint_utils_l2.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_mpls.c
 * cint ../../src/examples/dpp/utility/cint_utils_mpls_port.c
 * cint ../../src/examples/dpp/cint_port_tpid.c
 * cint ../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
 * cint ../../src/examples/dpp/cint_l2_mact.c
 * cint ../../src/examples/dpp/cint_vswitch_metro_mp.c
 * cint ../../src/examples/dpp/cint_qos.c
 * cint ../../src/examples/dpp/cint_ip_route.c
 * cint ../../src/examples/dpp/cint_mpls_labels_encapsulation_with_reserved_profile.c
 * cint
 * reserved_mpls_label_encapsulation_example(0,200,203); 
 * exit;
 *
 * tx 1 psrc=200 data=0x0000000000110000000011008100006408004500001400000000c800b6ae141414140a0a0a0a
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 200, Destination port: 203 
 *  Data: 0x000000000022000000000011810000c888470777703c066660140555501404444014033330140222203c011111644500001400000000c700b7ae141414140a0a0a0a000000000000000000000000000000000000000000000000 
 *
 * cint
 * reserved_mpls_label_encapsulation_3rd_mpls_tunnel_label_update(0,0x33333,0x44444,0x55555);
 * exit;
 *
 * tx 1 psrc=200 data=0x0000000000110000000011008100006408004500001400000000c800b6ae141414140a0a0a0a
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 200, Destination port: 203 
 *  Data: 0x000000000022000000000011810000c888470777703c066660145555501444444014333330140222203c011111644500001400000000c700b7ae141414140a0a0a0a000000000000000000000000000000000000000000000000 
 *
 * cint
 * reserved_mpls_label_encapsulation_3rd_mpls_tunnel_label_update(0,0x3333,0x4444,0x5555);
 * exit;
 *
 * cint
 * reserved_mpls_label_encapsulation_2nd_tunnel_update(0,8);
 * exit;
 *
 * tx 1 psrc=200 data=0x0000000000110000000011008100006408004500001400000000c800b6ae141414140a0a0a0a
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 200, Destination port: 203 
 *  Data: 0x000000000022000000000011810000c888470777703c0666601404444014033330140222203c011111644500001400000000c700b7ae141414140a0a0a0a000000000000000000000000000000000000000000000000 
 *
 * cint
 * reserved_mpls_label_encapsulation_2nd_tunnel_update(0,9);
 * exit;
 *
 * tx 1 psrc=200 data=0x0000000000110000000011008100006408004500001400000000c800b6ae141414140a0a0a0a
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 200, Destination port: 203 
 *  Data: 0x000000000022000000000011810000c888470777703c0666601405555014044440140222203c011111644500001400000000c700b7ae141414140a0a0a0a000000000000000000000000000000000000000000000000 
 *
 * cint
 * reserved_mpls_label_encapsulation_2nd_tunnel_update(0,7);
 * exit;
 *
 * tx 1 psrc=200 data=0x0000000000110000000011008100006408004500001400000000c800b6ae141414140a0a0a0a
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 200, Destination port: 203 
 *  Data: 0x000000000022000000000011810000c888470777703c0666601405555014033330140222203c011111644500001400000000c700b7ae141414140a0a0a0a000000000000000000000000000000000000000000000000 
 *
 * cint
 * reserved_mpls_label_encapsulation_2nd_tunnel_update(0,4);
 * exit;
 *
 * tx 1 psrc=200 data=0x0000000000110000000011008100006408004500001400000000c800b6ae141414140a0a0a0a
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 0, Destination port: 0 
 *  Data: 0x000000000022000000000011810000c888470777703c06666014033330140222203c011111644500001400000000c700b7ae141414140a0a0a0a000000000000000000000000000000000000000000000000 
 *
 * cint
 * reserved_mpls_label_encapsulation_2nd_tunnel_update(0,5);
 * exit;
 *
 * tx 1 psrc=200 data=0x0000000000110000000011008100006408004500001400000000c800b6ae141414140a0a0a0a
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 200, Destination port: 203 
 *  Data: 0x000000000022000000000011810000c888470777703c06666014044440140222203c011111644500001400000000c700b7ae141414140a0a0a0a000000000000000000000000000000000000000000000000 
 *
 * cint
 * reserved_mpls_label_encapsulation_2nd_tunnel_update(0,6);
 * exit;
 *
 * tx 1 psrc=200 data=0x0000000000110000000011008100006408004500001400000000c800b6ae141414140a0a0a0a
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 200, Destination port: 203 
 *  Data: 0x000000000022000000000011810000c888470777703c06666014055550140222203c011111644500001400000000c700b7ae141414140a0a0a0a000000000000000000000000000000000000000000000000 
 *
 * cint
 * reserved_mpls_label_encapsulation_update_tunnel_linklist(0,1);
 * exit;
 *
 * tx 1 psrc=200 data=0x0000000000110000000011008100006408004500001400000000c800b6ae141414140a0a0a0a
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 200, Destination port: 203 
 *  Data: 0x000000000022000000000011810000c888470777703c06666014055551144500001400000000c700b7ae141414140a0a0a0a000000000000000000000000000000000000000000000000 
 */


struct reserved_mpls_label_encapsulation_info_s{
    int Label_L1;
    int Label_L2;
    int Label_L3;
    int Label_L4;
    int Label_L5;
    int Label_L6;
    int Label_L7;

    int in_vlan; 
    int out_vlan;

    int mpls_tunnel_id[3];
    int link_layer_encap_id;

    int egress_fec_id;

    bcm_mac_t mac_address;  /* my-MAC */
    bcm_mac_t next_hop_mac; /* next_hop_mac1 */

    int out_port;
    int in_port;

};

reserved_mpls_label_encapsulation_info_s mpls_info;

int reserved_mpls_label_encapsulation_info_init(int unit,int in_port,int out_port){

    uint8 mac_address[6]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x11 };  /* my-MAC */
    uint8 next_hop_mac[6]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x22 }; /* next_hop_mac1 */


    mpls_info.Label_L1=   0x1111;
    mpls_info.Label_L2=   0x2222;
    mpls_info.Label_L3=   0x3333;
    mpls_info.Label_L4=   0x4444;
    mpls_info.Label_L5=   0x5555;
    mpls_info.Label_L6=   0x6666;
    mpls_info.Label_L7=   0x7777;

    mpls_info.in_vlan= 100;
    mpls_info.out_vlan= 200;

    mpls_info.out_port = out_port;
    mpls_info.in_port = in_port;

    sal_memcpy(mpls_info.mac_address, mac_address, 6);
    sal_memcpy(mpls_info.next_hop_mac, next_hop_mac, 6);

    return 0;
}

int l3_intf_init(int unit, int in_port, int out_port)
{
    int rc;
    bcm_pbmp_t pbmp, ubmp;
    create_l3_intf_s intf;

    /* Create the VLAN (in_lif/in_vsi here)*/
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, in_port);

    mpls_info.out_port = out_port;
    mpls_info.in_port = in_port;

    rc = bcm_vlan_destroy(unit, mpls_info.in_vlan);
    if (rc != BCM_E_NONE) {
        printf("Error, in bcm_vlan_destroy, vlan=%d, \n",  mpls_info.in_vlan);
    }

    rc = bcm_vlan_create(unit, mpls_info.in_vlan);
    if (rc != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create, vlan=%d, \n",  mpls_info.in_vlan);
        return rc;
    }
    rc  = bcm_vlan_port_add(unit, mpls_info.in_vlan, pbmp, ubmp);
    if (rc != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_add, vlan=%d, \n",  mpls_info.in_vlan);
        return rc;
    }

    intf.vsi = mpls_info.in_vlan;;
    intf.my_global_mac = mpls_info.mac_address;
    intf.my_lsb_mac = mpls_info.mac_address;
    rc = l3__intf_rif__create(unit, &intf);
    if (rc != BCM_E_NONE) {
        printf("Error, in l3__intf_rif__create\n");
        return rv;
    }

    /* Create the VLAN out_lif/out_vsi*/
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, out_port);
    rc = bcm_vlan_destroy(unit, mpls_info.out_vlan);
    if (rc != BCM_E_NONE) {
        printf("Error, in bcm_vlan_destroy, vlan=%d, \n", mpls_info.out_vlan);
    }

    rc =  bcm_vlan_create(unit, mpls_info.out_vlan);
    if (rc != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create, vlan=%d, \n", mpls_info.out_vlan);
        return rc;
    }

    rc = bcm_vlan_port_add(unit, mpls_info.out_vlan, pbmp, ubmp);
    if (rc != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_add, vlan=%d, \n", mpls_info.out_vlan);
        return rc;
    }
    intf.vsi = mpls_info.out_vlan;;
    intf.my_global_mac = mpls_info.mac_address;
    intf.my_lsb_mac = mpls_info.mac_address;
    rc = l3__intf_rif__create(unit, &intf);
    if (rc != BCM_E_NONE) {
        printf("Error, in l3__intf_rif__create\n");
        return rc;
    }

    return rc;
}

/*
  * Configure the IPv4 service,routing into MPLS tunnel list , mpls tunnel 1----> mpls tunnel 2---->Data entry--->link layer
  */

int reserved_mpls_label_encapsulation_example(int unit, int in_port, int out_port){
    int rc;
    create_l3_egress_s create_l3_egress_encap_params;
    mpls__egress_tunnel_utils_s mpls_tunnel_properties;
    uint64 wide_data;
    int gport;

    rc = reserved_mpls_label_encapsulation_info_init(unit,in_port,out_port);
    if (rc !=0 ) {
       printf("reserved_mpls_label_encapsulation_info_init rc=%d",rc);
    }

    rc = l3_intf_init(unit,in_port,out_port);
    if (rc !=0 ) {
       printf("l3_intf_init failed rc=%d",rc);
    }

    create_l3_egress_encap_params.l3_flags2 = 0;
    create_l3_egress_encap_params.arp_encap_id = 0;
    create_l3_egress_encap_params.vlan = mpls_info.out_vlan;
    create_l3_egress_encap_params.next_hop_mac_addr = mpls_info.next_hop_mac;
    rc = l3__egress_only_encap__create(unit, &create_l3_egress_encap_params);
    if (rc != BCM_E_NONE) {
        printf("Error, in l3__egress_only_encap__create\n");
        return rc;
    }
    mpls_info.link_layer_encap_id = create_l3_egress_encap_params.arp_encap_id;

    /*create the 3rd mpls tunnel with data entry, carrying L3,L4,L5 label,pointing to the above link layer entry*/
    mpls_tunnel_properties.tunnel_id = 0;
    mpls_tunnel_properties.next_pointer_intf =  mpls_info.link_layer_encap_id;
    mpls_tunnel_properties.label_in =  mpls_info.Label_L3;
    mpls_tunnel_properties.label_out=  mpls_info.Label_L4;
    mpls_tunnel_properties.label_out_2=  mpls_info.Label_L5;
    mpls_tunnel_properties.egress_action = BCM_MPLS_EGRESS_ACTION_PUSH;
    mpls_tunnel_properties.encap_access = bcmEncapAccessTunnel3;
    mpls_tunnel_properties.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT|BCM_MPLS_EGRESS_LABEL_ACTION_VALID|BCM_MPLS_EGRESS_LABEL_PRESERVE;
    rc = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
    if (rc != BCM_E_NONE) {
        printf("Error, in mpls__create_tunnel_initiator__set\n");
        return rc;
    }
    mpls_info.mpls_tunnel_id[2]= mpls_tunnel_properties.tunnel_id;


    /*create the 2nd mpls tunnel using second mpls tunnel, carrying L6,L7 label*/
    mpls_tunnel_properties.tunnel_id = 0;
    mpls_tunnel_properties.next_pointer_intf =  mpls_info.mpls_tunnel_id[2];
    mpls_tunnel_properties.label_in =  mpls_info.Label_L6;
    mpls_tunnel_properties.label_out= mpls_info.Label_L7;
    mpls_tunnel_properties.label_out_2= 0;

    mpls_tunnel_properties.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_tunnel_properties.spl_label_push_type = bcmMplsSpecialLabelPushMeaningful123;
    mpls_tunnel_properties.encap_access = bcmEncapAccessTunnel1;

    rc = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
    if (rc != BCM_E_NONE) {
        printf("Error, in mpls__create_tunnel_initiator__set\n");
        return rc;
    }
    mpls_info.mpls_tunnel_id[1]= mpls_tunnel_properties.tunnel_id;

    /*create the 1st mpls tunnel using second mpls tunnel, carrying L1,L2 label*/
    mpls_tunnel_properties.tunnel_id = 0;
    mpls_tunnel_properties.next_pointer_intf =  mpls_info.mpls_tunnel_id[1];
    mpls_tunnel_properties.label_in =  mpls_info.Label_L1;
    mpls_tunnel_properties.label_out= mpls_info.Label_L2;
    mpls_tunnel_properties.ttl = 100;

    mpls_tunnel_properties.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_tunnel_properties.encap_access = bcmEncapAccessNativeArp;
    mpls_tunnel_properties.spl_label_push_type = 0;

    rc = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
    if (rc != BCM_E_NONE) {
        printf("Error, in mpls__create_tunnel_initiator__set\n");
        return rc;
    }
    mpls_info.mpls_tunnel_id[0]= mpls_tunnel_properties.tunnel_id;


    create_l3_egress_s l3_egress_fec;

    l3_egress_fec.out_gport = out_port;
    l3_egress_fec.arp_encap_id = mpls_info.mpls_tunnel_id[0];
    rc = l3__egress_only_fec__create(unit, &l3_egress_fec);
    if (rc != BCM_E_NONE) {
        printf("Error, l3__egress_only_fec__create\n");
        return rc;
    }
    mpls_info.egress_fec_id = l3_egress_fec.fec_id;

    rc = add_route(unit,0x0a0a0a0a,0xffffff00,0,l3_egress_fec.fec_id);
    if (rc != BCM_E_NONE) {
        printf("Error, add_route\n");
        return rc;
    }

    return rc;
}

/*
 * Update the encap list, use 3 level tunnel or 2 level tunnel.
 */

int  reserved_mpls_label_encapsulation_update_tunnel_linklist(int unit,int with2level){
    int rc = 0;
    create_l3_egress_s l3_egress_fec;

    l3_egress_fec.fec_id = mpls_info.egress_fec_id;
    l3_egress_fec.allocation_flags = BCM_L3_REPLACE | BCM_L3_WITH_ID;
    l3_egress_fec.out_gport = mpls_info.out_port;
    l3_egress_fec.arp_encap_id = with2level?mpls_info.mpls_tunnel_id[1]:mpls_info.mpls_tunnel_id[0];
    rc = l3__egress_only_fec__create(unit, &l3_egress_fec);
    if (rc != BCM_E_NONE) {
        printf("Error, l3__egress_only_fec__create\n");
        return rc;
    }

    return rc;
}


/*
 * Update the MPLS-tunnel2,change the reserved MPLS label operation type.
 */
int  reserved_mpls_label_encapsulation_2nd_tunnel_update(int unit,bcm_mpls_special_label_push_type_t label_bmp){
    int rc = 0;
    bcm_mpls_egress_label_t  tLabelArray[2];
    int label_count;
    create_l3_egress_s create_l3_egress_encap_params;

    bcm_mpls_egress_label_t_init(&tLabelArray[0]);
    bcm_mpls_egress_label_t_init(&tLabelArray[1]);

    uint8 ucArrIndex = 0;

    rc = bcm_mpls_tunnel_initiator_get(0,mpls_info.mpls_tunnel_id[1],2,tLabelArray,&label_count);
    if (rc) {
        printf("\n bcm_mpls_tunnel_initiator_get failed");
    }
    tLabelArray[ucArrIndex].flags |= BCM_MPLS_EGRESS_LABEL_REPLACE | BCM_MPLS_EGRESS_LABEL_WITH_ID;
    tLabelArray[ucArrIndex+1].flags |= BCM_MPLS_EGRESS_LABEL_REPLACE | BCM_MPLS_EGRESS_LABEL_WITH_ID;

    tLabelArray[ucArrIndex].spl_label_push_type = label_bmp;
    tLabelArray[ucArrIndex].l3_intf_id = mpls_info.mpls_tunnel_id[2];

    rc = bcm_mpls_tunnel_initiator_create(0,0,2,tLabelArray);
    if (rc) {
        printf("\n bcm_mpls_tunnel_initiator_create failed");
        return rc;
    }

    return rc;
}

/*
 * Update the MPLS tunnel with Data entry, change the label value.
 */
int reserved_mpls_label_encapsulation_3rd_mpls_tunnel_label_update(int unit, int label1,int label2,int label3){
    int rc = 0;
    bcm_mpls_egress_label_t  tLabelArray[3];
    create_l3_egress_s create_l3_egress_encap_params;
    bcm_mpls_egress_label_t_init(&tLabelArray[0]);

    /*Update the mpls label for data entry*/
    tLabelArray[0].flags |= BCM_MPLS_EGRESS_LABEL_REPLACE | BCM_MPLS_EGRESS_LABEL_WITH_ID|BCM_MPLS_EGRESS_LABEL_PRESERVE;
    tLabelArray[0].label = label1;
    tLabelArray[1].label = label2;
    tLabelArray[2].label = label3;


    tLabelArray[0].tunnel_id = mpls_info.mpls_tunnel_id[2];

    tLabelArray[0].l3_intf_id =  mpls_info.link_layer_encap_id;

    rc = bcm_mpls_tunnel_initiator_create(0,0,3,tLabelArray);
    
    return rc;

}


/*Get MPLS 3rd tunnel to check*/
int reserved_mpls_label_encapsulation_3rd_mpls_tunnel_label_get_check(int unit, int in_port, int out_port)
{
    int rc;
    create_l3_egress_s create_l3_egress_encap_params;
    mpls__egress_tunnel_utils_s mpls_tunnel_properties;
    int gport;

    rc = reserved_mpls_label_encapsulation_info_init(unit,in_port,out_port);
    if (rc !=0 ) {
       printf("reserved_mpls_label_encapsulation_info_init rc=%d",rc);
    }
    mpls_info.Label_L1=   500011;
    mpls_info.Label_L2=   500012;
    mpls_info.Label_L3=   500013;
    mpls_info.Label_L4=   500014;
    mpls_info.Label_L5=   500015;
    mpls_info.Label_L6=   500016;
    mpls_info.Label_L7=   500017;

    rc = l3_intf_init(unit,in_port,out_port);
    if (rc !=0 ) {
       printf("l3_intf_init failed rc=%d",rc);
    }

    create_l3_egress_encap_params.l3_flags2 = 0;
    create_l3_egress_encap_params.arp_encap_id = 0;
    create_l3_egress_encap_params.vlan = mpls_info.out_vlan;
    create_l3_egress_encap_params.next_hop_mac_addr = mpls_info.next_hop_mac;
    rc = l3__egress_only_encap__create(unit, &create_l3_egress_encap_params);
    if (rc != BCM_E_NONE) {
        printf("Error, in l3__egress_only_encap__create\n");
        return rc;
    }
    mpls_info.link_layer_encap_id = create_l3_egress_encap_params.arp_encap_id;

    /*create the 3rd mpls tunnel with data entry, carrying L3,L4,L5 label,pointing to the above link layer entry*/
    mpls_tunnel_properties.tunnel_id = 0;
    mpls_tunnel_properties.next_pointer_intf =  mpls_info.link_layer_encap_id;
    mpls_tunnel_properties.label_in =  mpls_info.Label_L3;
    mpls_tunnel_properties.label_out=  mpls_info.Label_L4;
    mpls_tunnel_properties.label_out_2=  mpls_info.Label_L5;
    mpls_tunnel_properties.egress_action = BCM_MPLS_EGRESS_ACTION_PUSH;
    mpls_tunnel_properties.encap_access = bcmEncapAccessTunnel3;
    mpls_tunnel_properties.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT|BCM_MPLS_EGRESS_LABEL_ACTION_VALID|BCM_MPLS_EGRESS_LABEL_PRESERVE;
    rc = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
    if (rc != BCM_E_NONE) {
        printf("Error, in mpls__create_tunnel_initiator__set\n");
        return rc;
    }
    mpls_info.mpls_tunnel_id[2]= mpls_tunnel_properties.tunnel_id;


    /*create the 2nd mpls tunnel using second mpls tunnel, carrying L6,L7 label*/
    mpls_tunnel_properties.tunnel_id = 0;
    mpls_tunnel_properties.next_pointer_intf =  mpls_info.mpls_tunnel_id[2];
    mpls_tunnel_properties.label_in =  mpls_info.Label_L6;
    mpls_tunnel_properties.label_out= mpls_info.Label_L7;
    mpls_tunnel_properties.label_out_2= 0;

    mpls_tunnel_properties.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_tunnel_properties.spl_label_push_type = bcmMplsSpecialLabelPushMeaningful123;
    mpls_tunnel_properties.encap_access = bcmEncapAccessTunnel1;

    rc = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
    if (rc != BCM_E_NONE) {
        printf("Error, in mpls__create_tunnel_initiator__set\n");
        return rc;
    }
    mpls_info.mpls_tunnel_id[1]= mpls_tunnel_properties.tunnel_id;

    /*create the 1st mpls tunnel using second mpls tunnel, carrying L1,L2 label*/
    mpls_tunnel_properties.tunnel_id = 0;
    mpls_tunnel_properties.next_pointer_intf =  mpls_info.mpls_tunnel_id[1];
    mpls_tunnel_properties.label_in =  mpls_info.Label_L1;
    mpls_tunnel_properties.label_out= mpls_info.Label_L2;
    mpls_tunnel_properties.ttl = 100;

    mpls_tunnel_properties.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_tunnel_properties.encap_access = bcmEncapAccessNativeArp;
    mpls_tunnel_properties.spl_label_push_type = 0;

    rc = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
    if (rc != BCM_E_NONE) {
        printf("Error, in mpls__create_tunnel_initiator__set\n");
        return rc;
    }
    mpls_info.mpls_tunnel_id[0]= mpls_tunnel_properties.tunnel_id;

    /*Get check*/
    int label_max=10;
    bcm_mpls_egress_label_t label_array[10];
    int label_count=0;
    bcm_mpls_egress_label_t_init(&label_array[0]);
    bcm_mpls_egress_label_t_init(&label_array[1]);
    bcm_mpls_egress_label_t_init(&label_array[2]);
    bcm_mpls_egress_label_t_init(&label_array[3]);
    bcm_mpls_egress_label_t_init(&label_array[4]);
    bcm_mpls_egress_label_t_init(&label_array[5]);
    bcm_mpls_egress_label_t_init(&label_array[6]);
    bcm_mpls_egress_label_t_init(&label_array[7]);
    bcm_mpls_egress_label_t_init(&label_array[8]);
    bcm_mpls_egress_label_t_init(&label_array[9]);
    rc = bcm_mpls_tunnel_initiator_get(unit ,mpls_info.mpls_tunnel_id[2],label_max, label_array, &label_count);
    if (rc != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_get\n");
        return rc;
    }
    if (label_count != 3) {
        printf("Error label count of 3rd mpls tunnel\n");
        return rc;        
    }
    if ((label_array[0].label != mpls_info.Label_L3)
        || (label_array[1].label != mpls_info.Label_L4)
        || (label_array[2].label != mpls_info.Label_L5)) {
        printf("Error label from get operation\n");
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

