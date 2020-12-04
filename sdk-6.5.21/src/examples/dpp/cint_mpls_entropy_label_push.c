/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This example demonstrate how to insert entropy label for MPLS encapsulation.
 * JERICHO supports entropy label inserting under 2 different scenario:
 * a) Inerting EL/ELI at BoS location. The relevant scenario is ingress LSR;
 * b) Inserting EL/ELI at non-BoS location. The relevant scenario is ingress PE in L2/L3 VPN.
 *
 * Configuration tips:
 * a) Insert EL/ELI at BoS: configure the mpls initiator, setting the flags of first label with BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE|BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE
 * b) Insert EL/ELI at non-BoS, etc, right outer the first innermost label, there are 2 scenarios:
 *     i.  The 1st MPLS tunnel  carry 2 labels:
 *                i-1. mpls_label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE|BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE;
 *                i-2. bcm_mpls_tunnel_initiator_create()unit,intf,label_num,mpls_label_array to create the 1st MPLS tunnel.
 *                i-3. bcm_mpls_tunnel_initiator_create() to create the 2nd MPLS tunnel.
 *     ii. The 1st MPLS tunnel carry only 1 label, and second label is carried by 2nd MPLS tunnel.
 *                ii-1. bcm_mpls_tunnel_initiator_create() to create the 1st MPLS tunnel.
 *                ii-2. mpls_label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE|BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE;
 *                ii-3. bcm_mpls_tunnel_initiator_create(unit,intf,label_num,mpls_label_array) to create the 2nd MPLS tunnel.
 *

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
 * cint ../../src/examples/dpp/internal/cint_mpls_four_separate_encapsulation.c
 * cint ../../src/examples/dpp/cint_ip_route.c
 * cint ../../src/examples/dpp/cint_mpls_entropy_label_push.c
 * cint
 * mpls_entropy_label_inserting_example(0,201,203,0); 
 * exit;
 *
 * cint
 * ip_to_mpls_tunnel_with_entropy_label_on_2_mpls_enc(0,201,203,2,2,1);
 * exit;
 *
 * tx 1 psrc=201 data=0x0000000000110000e7ec428e81000064080045000054000000008000a7910a0a0a0a7fffff052008011800000000000300040000000a0000000a0000000100010203040500000001020304050000000300040000000a0000000a000000010001020304050000
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 201, Destination port: 203 
 *  Data: 0x000000000022000000000011810000c8884700fa003c00bb8014007d003c003e8014000070140000010045000054000000007f00a8910a0a0a0a7fffff052008011800000000000300040000000a0000000a0000000100010203040500000001020304050000000300040000000a0000000a000000010001020304050000 
 *
 * cint
 * ip_to_mpls_tunnel_with_entropy_label_on_2_mpls_enc(0,201,203,2,2,0);
 * exit;
 *
 * tx 1 psrc=201 data=0x0000000000110000e7ec428e81000064080045000054000000008000a7910a0a0a0a7fffff052008011800000000000300040000000a0000000a0000000100010203040500000001020304050000000300040000000a0000000a000000010001020304050000
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 201, Destination port: 203 
 *  Data: 0x000000000022000000000011810000c8884700fa003c00bb8014007d003c0000703c01111000003e811445000054000000007f00a8910a0a0a0a7fffff052008011800000000000300040000000a0000000a0000000100010203040500000001020304050000000300040000000a0000000a000000010001020304050000 
 *
 * cint
 * ip_to_mpls_tunnel_with_entropy_label_on_2_mpls_enc(0,201,203,2,1,0);
 * exit;
 *
 * tx 1 psrc=201 data=0x0000000000110000e7ec428e81000064080045000054000000008000a7910a0a0a0a7fffff052008011800000000000300040000000a0000000a0000000100010203040500000001020304050000000300040000000a0000000a000000010001020304050000
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 201, Destination port: 203 
 *  Data: 0x000000000022000000000011810000c8884700bb8014007d003c0000703c01111000003e811445000054000000007f00a8910a0a0a0a7fffff052008011800000000000300040000000a0000000a0000000100010203040500000001020304050000000300040000000a0000000a000000010001020304050000 
 *
 * cint
 * ip_to_mpls_tunnel_with_entropy_label_on_2_mpls_enc(0,201,203,1,1,0);
 * exit;
 *
 * tx 1 psrc=201 data=0x0000000000110000e7ec428e81000064080045000054000000008000a7910a0a0a0a7fffff052008011800000000000300040000000a0000000a0000000100010203040500000001020304050000000300040000000a0000000a000000010001020304050000
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 201, Destination port: 203 
 *  Data: 0x000000000022000000000011810000c8884700bb80140000701401111000003e811445000054000000007f00a8910a0a0a0a7fffff052008011800000000000300040000000a0000000a0000000100010203040500000001020304050000000300040000000a0000000a000000010001020304050000 
 *
 * cint
 * ip_to_mpls_tunnel_with_entropy_label_on_1_mpls_enc(0,201,203,2,0);
 * exit;
 *
 * tx 1 psrc=201 data=0x0000000000110000e7ec428e81000064080045000054000000008000a7910a0a0a0a7fffff052008011800000000000300040000000a0000000a0000000100010203040500000001020304050000000300040000000a0000000a000000010001020304050000
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 201, Destination port: 203 
 *  Data: 0x000000000022000000000011810000c88847007d003c0000703c01111000003e811445000054000000007f00a8910a0a0a0a7fffff052008011800000000000300040000000a0000000a0000000100010203040500000001020304050000000300040000000a0000000a000000010001020304050000 
 *
 * cint
 * ip_to_mpls_tunnel_with_entropy_label_on_2_mpls_enc(0,201,203,1,2,0);
 * exit;
 *
 * tx 1 psrc=201 data=0x0000000000110000e7ec428e81000064080045000054000000008000a7910a0a0a0a7fffff052008011800000000000300040000000a0000000a0000000100010203040500000001020304050000000300040000000a0000000a000000010001020304050000
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 201, Destination port: 203 
 *  Data: 0x000000000022000000000011810000c8884700fa003c00bb80140000701401111000003e811445000054000000007f00a8910a0a0a0a7fffff052008011800000000000300040000000a0000000a0000000100010203040500000001020304050000000300040000000a0000000a000000010001020304050000 
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
 * cint ../../src/examples/dpp/internal/cint_mpls_four_separate_encapsulation.c
 * cint ../../src/examples/dpp/cint_ip_route.c
 * cint ../../src/examples/dpp/cint_mpls_entropy_label_push.c
 * cint
 * mpls_entropy_label_inserting_example(0,200,201,0,1); ip_to_mpls_tunnel_with_entropy_label_add_ext_label_on_lsp2(0,5000); 
 * exit;
 *
 * cint
 * ip_to_mpls_tunnel_with_entropy_label_on_2_mpls_enc(0,200,201,2,2,1,1);
 * exit;
 *
 * cint
 * ip_to_mpls_tunnel_with_entropy_label_add_ext_label_on_lsp2(0,5000);
 * exit;
 *
 * tx 1 psrc=200 data=0x00000000001100002cad4f4781000064080045000054000000008000a7910a0a0a0a7fffff052008011800000000000300040000000a0000000a0000000100010203040500000001020304050000000300040000000a0000000a000000010001020304050000
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 200, Destination port: 201 
 *  Data: 0x000000000022000000000011810000c888470138803c00fa003c00bb8014007d003c003e8014000070140000010045000054000000007f00a8910a0a0a0a7fffff052008011800000000000300040000000a0000000a0000000100010203040500000001020304050000000300040000000a0000000a000000010001020304050000 
 *
 * cint
 * ip_to_mpls_tunnel_with_entropy_label_on_2_mpls_enc(0,200,201,2,2,0,1);
 * exit;
 *
 * cint
 * ip_to_mpls_tunnel_with_entropy_label_add_ext_label_on_lsp2(0,5000);
 * exit;
 *
 * tx 1 psrc=200 data=0x00000000001100002cad4f4781000064080045000054000000008000a7910a0a0a0a7fffff052008011800000000000300040000000a0000000a0000000100010203040500000001020304050000000300040000000a0000000a000000010001020304050000
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 200, Destination port: 201 
 *  Data: 0x000000000022000000000011810000c888470138803c00fa003c00bb8014007d003c0000703c01111000003e811445000054000000007f00a8910a0a0a0a7fffff052008011800000000000300040000000a0000000a0000000100010203040500000001020304050000000300040000000a0000000a000000010001020304050000 
 *
 * cint
 * ip_to_mpls_tunnel_with_entropy_label_on_2_mpls_enc(0,200,201,2,1,0,1);
 * exit;
 *
 * cint
 * ip_to_mpls_tunnel_with_entropy_label_add_ext_label_on_lsp2(0,5000);
 * exit;
 *
 * tx 1 psrc=200 data=0x00000000001100002cad4f4781000064080045000054000000008000a7910a0a0a0a7fffff052008011800000000000300040000000a0000000a0000000100010203040500000001020304050000000300040000000a0000000a000000010001020304050000
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 200, Destination port: 201 
 *  Data: 0x000000000022000000000011810000c888470138801400bb8014007d003c0000703c01111000003e811445000054000000007f00a8910a0a0a0a7fffff052008011800000000000300040000000a0000000a0000000100010203040500000001020304050000000300040000000a0000000a000000010001020304050000 
 *
 * cint
 * ip_to_mpls_tunnel_with_entropy_label_on_2_mpls_enc(0,200,201,1,1,0,1);
 * exit;
 *
 * cint
 * ip_to_mpls_tunnel_with_entropy_label_add_ext_label_on_lsp2(0,5000);
 * exit;
 *
 * tx 1 psrc=200 data=0x00000000001100002cad4f4781000064080045000054000000008000a7910a0a0a0a7fffff052008011800000000000300040000000a0000000a0000000100010203040500000001020304050000000300040000000a0000000a000000010001020304050000
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 200, Destination port: 201 
 *  Data: 0x000000000022000000000011810000c888470138801400bb80140000701401111000003e811445000054000000007f00a8910a0a0a0a7fffff052008011800000000000300040000000a0000000a0000000100010203040500000001020304050000000300040000000a0000000a000000010001020304050000 
 *
 * cint
 * ip_to_mpls_tunnel_with_entropy_label_on_2_mpls_enc(0,200,201,1,2,0,1);
 * exit;
 *
 * cint
 * ip_to_mpls_tunnel_with_entropy_label_add_ext_label_on_lsp2(0,5000);
 * exit;
 *
 * tx 1 psrc=200 data=0x00000000001100002cad4f4781000064080045000054000000008000a7910a0a0a0a7fffff052008011800000000000300040000000a0000000a0000000100010203040500000001020304050000000300040000000a0000000a000000010001020304050000
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 200, Destination port: 201 
 *  Data: 0x000000000022000000000011810000c888470138803c00fa003c00bb80140000701401111000003e811445000054000000007f00a8910a0a0a0a7fffff052008011800000000000300040000000a0000000a0000000100010203040500000001020304050000000300040000000a0000000a000000010001020304050000 
 */



 struct cint_mpls_entropy_label_s{
    int lsp_A1_label;
    int lsp_A2_label;
    int lsp_B1_label;
    int lsp_B2_label;

    bcm_if_t  lsp1_tunnel_id;
    bcm_if_t  lsp2_tunnel_id;

    bcm_if_t l3_fec_id;
    bcm_if_t arp_id;

};

cint_mpls_entropy_label_s entropy_label_info;

/*Inserting EL/ELI, with 2 MPLS encapsulation EES*/
int ip_to_mpls_tunnel_with_entropy_label_on_2_mpls_enc(int unit, int inP, int outP,int num_of_labels_lsp1,int num_of_labels_lsp2,int el_at_bos,int with_ext_label){
    int rv;

    create_l3_egress_s l3_egress;
    create_l3_egress_s l3_ingress;

    mpls__egress_tunnel_utils_s mpls_lsp1_tunnel_properties;
    mpls__egress_tunnel_utils_s mpls_lsp2_tunnel_properties;
    uint32 route,mask;
    bcm_if_t tunnel_id;

    /*Clear the tunnel first*/
    if (entropy_label_info.lsp1_tunnel_id){
        printf("clearing the lsp1 0x%x\n",entropy_label_info.lsp1_tunnel_id);
        bcm_mpls_tunnel_initiator_clear(unit,entropy_label_info.lsp1_tunnel_id);
    }

    if (entropy_label_info.lsp2_tunnel_id){
        printf("clearing the lsp2 0x%x\n",entropy_label_info.lsp2_tunnel_id);
        bcm_mpls_tunnel_initiator_clear(unit,entropy_label_info.lsp2_tunnel_id);
    }


    /*create the LL entry*/
    if (entropy_label_info.arp_id == 0) {
        l3_egress.vlan = label_info.out_vlan;
        l3_egress.next_hop_mac_addr = label_info.next_hop_mac;
        l3_egress.out_tunnel_or_rif = label_info.out_vlan;
        
        rv = l3__egress_only_encap__create(unit, &l3_egress);
        if (rv != BCM_E_NONE) {
          printf("Error, in l3__egress_only_encap__create\n");
          return rv;
        }
        printf("Created LL ID: 0x%08x\n",l3_egress.arp_encap_id);
        entropy_label_info.arp_id = l3_egress.arp_encap_id;
    }

    mpls_lsp2_tunnel_properties.label_in = entropy_label_info.lsp_B1_label;
    mpls_lsp2_tunnel_properties.force_flags_out = 1;
    mpls_lsp2_tunnel_properties.label_out = (num_of_labels_lsp2==1)? 0:entropy_label_info.lsp_B2_label;
    mpls_lsp2_tunnel_properties.next_pointer_intf = 0;
    mpls_lsp2_tunnel_properties.flags = (!el_at_bos && num_of_labels_lsp1<2)? BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE|BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE:0;
    mpls_lsp2_tunnel_properties.flags |= (with_ext_label)?BCM_MPLS_EGRESS_LABEL_WIDE:0;
    if (entropy_label_info.lsp2_tunnel_id){
        mpls_lsp2_tunnel_properties.flags |=BCM_MPLS_EGRESS_LABEL_WITH_ID;
        mpls_lsp2_tunnel_properties.tunnel_id = entropy_label_info.lsp2_tunnel_id;
    }

    printf("Trying to create lsp2 tunnel initiator\n");
    rv = mpls__create_tunnel_initiator__set(unit, &mpls_lsp2_tunnel_properties);
    if (rv != BCM_E_NONE) {
       printf("Error, in mpls__create_tunnel_initiator__set\n");
       return rv;
    }
    entropy_label_info.lsp2_tunnel_id=  mpls_lsp2_tunnel_properties.tunnel_id;
    printf("Created Tunnel ID: 0x%08x\n",entropy_label_info.lsp2_tunnel_id);

    mpls_lsp2_tunnel_properties.flags |= BCM_MPLS_EGRESS_LABEL_REPLACE | BCM_MPLS_EGRESS_LABEL_WITH_ID;
    mpls_lsp2_tunnel_properties.flags_out |= BCM_MPLS_EGRESS_LABEL_REPLACE | BCM_MPLS_EGRESS_LABEL_WITH_ID;
    mpls_lsp2_tunnel_properties.next_pointer_intf = entropy_label_info.arp_id;
    printf("Trying to conntect lsp2 to LL\n");
    rv = mpls__create_tunnel_initiator__set(unit, &mpls_lsp2_tunnel_properties);
    if (rv != BCM_E_NONE) {
       printf("Error, in mpls__create_tunnel_initiator__set\n");
       return rv;
    }

    /*lsp1 tunnel creating*/
    mpls_lsp1_tunnel_properties.label_in = entropy_label_info.lsp_A1_label;
    mpls_lsp1_tunnel_properties.force_flags_out = 1;
    mpls_lsp1_tunnel_properties.label_out = (num_of_labels_lsp1==1)?0:entropy_label_info.lsp_A2_label;
    mpls_lsp1_tunnel_properties.next_pointer_intf = entropy_label_info.lsp2_tunnel_id;
    /*Lsp1 carry 2 labels, EL flags is indicated here ,else the flag is indicated by lsp2*/
    if (el_at_bos) {
        mpls_lsp1_tunnel_properties.flags= BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE|BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE;
    }else if (num_of_labels_lsp1>1) {
        mpls_lsp1_tunnel_properties.flags_out = BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE|BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE;
    }
    if (entropy_label_info.lsp1_tunnel_id){
        mpls_lsp1_tunnel_properties.flags |=BCM_MPLS_EGRESS_LABEL_WITH_ID;
        mpls_lsp1_tunnel_properties.tunnel_id = entropy_label_info.lsp1_tunnel_id;
    }

    printf("Trying to create lsp1 tunnel initiator\n");
    rv = mpls__create_tunnel_initiator__set(unit, &mpls_lsp1_tunnel_properties);
    if (rv != BCM_E_NONE) {
       printf("Error, in mpls__create_tunnel_initiator__set\n");
       return rv;
    }
    entropy_label_info.lsp1_tunnel_id =  mpls_lsp1_tunnel_properties.tunnel_id;
    printf("Created lsp1 Tunnel ID: 0x%08x\n",entropy_label_info.lsp1_tunnel_id);

    if (entropy_label_info.l3_fec_id == 0) {

        l3_ingress.arp_encap_id = entropy_label_info.lsp1_tunnel_id;
        l3_ingress.out_gport = outP;
        rv = l3__egress_only_fec__create(unit, &l3_ingress);
        if (rv != BCM_E_NONE) {
          printf("Error, in l3__egress_only_fec__create\n");
          return rv;
        }
        printf("Created FEC ID: 0x%08x\n",l3_ingress.fec_id);
        
        route = 0x7fffff00;
        mask  = 0xfffffff0;
        
        rv = add_route(unit, route, mask , 0, l3_ingress.fec_id); 
        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, out port=%d in unit %d, \n", outP, unit);
        }
        
        entropy_label_info.l3_fec_id = l3_ingress.fec_id;

    }

    return rv;
}

int ip_to_mpls_tunnel_with_entropy_label_on_1_mpls_enc(int unit, int inP, int outP,int num_of_labels_lsp1,int el_at_bos){
    int rv;

    create_l3_egress_s l3_egress;
    create_l3_egress_s l3_ingress;

    mpls__egress_tunnel_utils_s mpls_lsp1_tunnel_properties;
    uint32 route,mask;
    bcm_if_t tunnel_id;

    /*Clear the tunnel first*/
    if (entropy_label_info.lsp1_tunnel_id){
        printf("clearing the lsp1 0x%x\n",entropy_label_info.lsp1_tunnel_id);
        bcm_mpls_tunnel_initiator_clear(unit,entropy_label_info.lsp1_tunnel_id);
    }

    if (entropy_label_info.lsp2_tunnel_id){
        printf("clearing the lsp2 0x%x\n",entropy_label_info.lsp2_tunnel_id);
        bcm_mpls_tunnel_initiator_clear(unit,entropy_label_info.lsp2_tunnel_id);
    }


    /*create the LL entry*/
    if (entropy_label_info.arp_id == 0) {
        l3_egress.vlan = label_info.out_vlan;
        l3_egress.next_hop_mac_addr = label_info.next_hop_mac;
        l3_egress.out_tunnel_or_rif = label_info.out_vlan;
        
        rv = l3__egress_only_encap__create(unit, &l3_egress);
        if (rv != BCM_E_NONE) {
          printf("Error, in l3__egress_only_encap__create\n");
          return rv;
        }
        printf("Created LL ID: 0x%08x\n",l3_egress.arp_encap_id);
        entropy_label_info.arp_id = l3_egress.arp_encap_id;
    }

    /*lsp1 tunnel creating*/
    mpls_lsp1_tunnel_properties.label_in = entropy_label_info.lsp_A1_label;
    mpls_lsp1_tunnel_properties.label_out = (num_of_labels_lsp1==1)?0:entropy_label_info.lsp_A2_label;
    mpls_lsp1_tunnel_properties.next_pointer_intf = entropy_label_info.arp_id;

    /*Lsp1 carry 2 labels, EL flags is indicated here ,else the flag is indicated by lsp2*/
    if (el_at_bos) {
        mpls_lsp1_tunnel_properties.flags= BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE|BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE;
    }else{
        mpls_lsp1_tunnel_properties.flags_out = BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE|BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE;
        mpls_lsp1_tunnel_properties.force_flags_out=1;
    }

    if (entropy_label_info.lsp1_tunnel_id){
        mpls_lsp1_tunnel_properties.flags |=BCM_MPLS_EGRESS_LABEL_WITH_ID;
        mpls_lsp1_tunnel_properties.tunnel_id = entropy_label_info.lsp1_tunnel_id;
    }

    printf("Trying to create lsp1 tunnel initiator\n");
    rv = mpls__create_tunnel_initiator__set(unit, &mpls_lsp1_tunnel_properties);
    if (rv != BCM_E_NONE) {
       printf("Error, in mpls__create_tunnel_initiator__set\n");
       return rv;
    }
    entropy_label_info.lsp1_tunnel_id =  mpls_lsp1_tunnel_properties.tunnel_id;
    printf("Created lsp1 Tunnel ID: 0x%08x\n",entropy_label_info.lsp1_tunnel_id);

    if (entropy_label_info.l3_fec_id == 0) {

        l3_ingress.arp_encap_id = entropy_label_info.lsp1_tunnel_id;
        l3_ingress.out_gport = outP;
        rv = l3__egress_only_fec__create(unit, &l3_ingress);
        if (rv != BCM_E_NONE) {
          printf("Error, in l3__egress_only_fec__create\n");
          return rv;
        }
        printf("Created FEC ID: 0x%08x\n",l3_ingress.fec_id);
        
        route = 0x7fffff00;
        mask  = 0xfffffff0;
        
        rv = add_route(unit, route, mask , 0, l3_ingress.fec_id); 
        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, out port=%d in unit %d, \n", outP, unit);
        }
        
        entropy_label_info.l3_fec_id = l3_ingress.fec_id;

    }

    return rv;
}

int
ip_to_mpls_tunnel_with_entropy_label_add_ext_label_on_lsp2(int unit, int label) {
    int rv;
    uint64 wide_data;
    int get_value;
    bcm_gport_t gport;

    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, entropy_label_info.lsp2_tunnel_id);

    /* Fill wide entry and configure outlif as having additional label */
    COMPILER_64_SET(wide_data, 0, label);
    rv = bcm_port_wide_data_set(unit, gport, BCM_PORT_WIDE_DATA_EGRESS/*flags*/, wide_data);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_wide_data_set\n");
        return rv;
    }

    rv = bcm_port_control_set(unit, gport, bcmPortControlMPLSEncapsulateAdditionalLabel, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_control_set\n");
        return rv;
    }

    rv = bcm_port_control_get(unit, gport, bcmPortControlMPLSEncapsulateAdditionalLabel, &get_value);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_control_get\n");
        return rv;
    }
    if (get_value != 1) {
        printf("Error, in bcm_port_control_get: value is not 1\n");
        return BCM_E_INTERNAL;
    }

    return rv;
}


int mpls_entropy_label_info_init(int unit){
  

    entropy_label_info.lsp_A1_label = 1000;
    entropy_label_info.lsp_A2_label = 2000;
    entropy_label_info.lsp_B1_label = 3000;
    entropy_label_info.lsp_B2_label = 4000;

}

/*
  * el_location: 
  * 0: insert EL/ELI between 1st tunnel and 2nd tunnel;
  * 1: insert EL/ELI at BoS location
*/

int mpls_entropy_label_inserting_example(int unit,int inP,int outP, int el_location, int with_ext_label){
    int rc = 0;

    four_level_mpls_encapsulation_info_init(unit);

    mpls_entropy_label_info_init(unit);

    four_level_mpls_l3_intf_init(unit,inP,outP);

    rc = ip_to_mpls_tunnel_with_entropy_label_on_2_mpls_enc(unit,inP,outP,2,2,el_location,with_ext_label);
    if(rc) {
        printf("\n ip_to_mpls_tunnel_with_entropy_label failed");
    }



    return rc;
}


