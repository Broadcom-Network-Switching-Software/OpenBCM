/*~~~~~~~~~Multi-Dev MPLS Segment Routing~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_sand_mpls_segment_routing.c
 * Purpose: Demo MPLS Segment Routing JR2 interiop JR1 mode with multi device system
 *
 * Multi-Device system require the ingress/egress configuration separately and relatively.
 * This cint demo how to configurer the MPLS SR service under multi-device system.
 *  1. Create egress push 6 labels
 *  2. Create In-RIF and host entry on ingress device
 *  3. Send MPLS packet to verify the result
 *
 *  IPv4oEth1---------->MPLSoMPLSoMPLSoMPLSoMPLSoMPLSoEth1
 *
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_mpls.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_sand_multi_device_ipv4_uc.c
 * cint ../../src/examples/sand/cint_sand_mpls_segment_routing.c
 * cint
 * initialize_required_system_tools(2);
 * multi_dev_mpls_segment_routing_example(1,2,1811939529,1811940042,0);
 * exit;
 *
 * tx 1 psrc=201 data=0x00000000001100008569fc8f81000064080045000035000000008000b1bf0a0000057fffff05000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 2 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x000000000022000000000011810000c88847010010ff010020140100301401004014010050140100611445000035000000007f00b2bf0a0000057fffff05000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 */



  struct cint_mpls_sr_info_s
 {
     int dip_into_tunnel;                  /* dip into mpls tunnel from push command*/
     int encap_label [6];                   /*  mpls tunnel label */
     int pipe_mode_exp_set;        /* exp mode in pipe */
     int exp;                                       /*  exp in pipe */
     int ttl;                                         /*  ttl in pipe */
     int fec_id;                  /*FEC ID for arp, for case 1,2,3,4*/
     int egr_mpls_tunnel_encap_id[3];     /* encap_id for mpls tunnel, for case 2*/
     int arp_encap_id;
 };


  cint_mpls_sr_info_s cint_mpls_sr_info=
 {
     /*
         * dip_into_tunnel
         */
     0x7FFFFF05, /* 127.255.255.5 */

     /*
         * encap labels,
         */
     {0x1001,0x1002,0x1003,0x1004,0x1005,0x1006}, /*d1,c3,c1,c2,b1,b2*/


     /*
         * uniform or pipe mode for exp and ttl
         */
     0,         /*pipe_mode_exp_set*/
     5,         /*exp*/
     20         /*TTL*/

 };

int jr_outer_most_tunnel_if = 0;

/**
 * Initiation for system parameter
 * INPUT:
 *   ingress_unit- traffic incoming unit
 *   egress_unit- traffic ougtoing unit
 *   in_sys_port- traffic incoming port
 *   out_sys_port- traffic outgoing port
*/
 int
mpls_segment_routing_init(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{
    int rv,is_qax;

    rv = multi_dev_ipv4_uc_init(ingress_unit,egress_unit,in_sys_port,out_sys_port);
    if (rv != BCM_E_NONE)
    {
         printf("Error, in multi_dev_ipv4_uc_init\n");
         return rv;
    }

    /*
        * FEC
        */
    rv = is_qumran_ax(ingress_unit, &is_qax);
    if (rv != BCM_E_NONE) {
        printf("Error, in is_qumran_ax \n");
        return rv;
    }
    if (is_qax) {
        BCM_L3_ITF_SET(cint_mpls_sr_info.fec_id, BCM_L3_ITF_TYPE_FEC, 0x5000);
    } else {
        BCM_L3_ITF_SET(cint_mpls_sr_info.fec_id, BCM_L3_ITF_TYPE_FEC, 0x10000);
    }

    /*
        * mpls tunnel
        */
    BCM_L3_ITF_SET(cint_mpls_sr_info.egr_mpls_tunnel_encap_id[0],BCM_L3_ITF_TYPE_LIF, 0x2004);
    BCM_L3_ITF_SET(cint_mpls_sr_info.egr_mpls_tunnel_encap_id[1],BCM_L3_ITF_TYPE_LIF, 0x2005);
    BCM_L3_ITF_SET(cint_mpls_sr_info.egr_mpls_tunnel_encap_id[2],BCM_L3_ITF_TYPE_LIF, 0x2006);

    /*
        * ARP
        */
    BCM_L3_ITF_SET(cint_mpls_sr_info.arp_encap_id,BCM_L3_ITF_TYPE_LIF, 0x3000);

    return rv;
}

/**
 * ARP adding
 * INPUT:
 *   in_port    - traffic incoming port
 *   out_port - traffic outgoing port
*/
 int
mpls_segment_routing_arp_configuration(
    int unit,
    int out_port)
{
    int rv;

    sand_utils_l3_arp_s arp_structure;

    /* Configure ARP entry  */
    if (!is_device_or_above(unit, JERICHO2)) {
        sand_utils_l3_arp_s_common_init(unit, 0, &arp_structure, cint_mpls_sr_info.arp_encap_id, 0, BCM_L3_FLAGS2_EGRESS_WIDE, cint_ipv4_route_info.next_hop_mac, cint_ipv4_route_info.out_rif);
    } else {
        sand_utils_l3_arp_s_common_init(unit, 0, &arp_structure, cint_mpls_sr_info.arp_encap_id, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION, cint_ipv4_route_info.next_hop_mac, cint_ipv4_route_info.out_rif);
    }
    rv = sand_utils_l3_arp_create(unit, &arp_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
        return rv;
    }

    return rv;
}



/*
 * l3 FEC replace
 */
int
mpls_segment_routing_fec_configuration(
    int unit,
    int out_sys_port)
{
    int rv;
    sand_utils_l3_fec_s fec_structure;

    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    fec_structure.destination  = out_sys_port;
    fec_structure.tunnel_gport = cint_mpls_sr_info.egr_mpls_tunnel_encap_id[0];
    fec_structure.allocation_flags =   BCM_L3_WITH_ID | BCM_L3_INGRESS_ONLY;
    fec_structure.fec_id = cint_mpls_sr_info.fec_id;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in sand_utils_l3_fec_create\n");
        return rv;
    }
    return rv;

}

/*
 * l3 ARP replace
 */
int
mpls_segment_routing_arp_update(
    int unit)
{
    int rv;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;       /* not intersting */
    uint32 flags = BCM_L3_WITH_ID | BCM_L3_REPLACE | BCM_L3_EGRESS_ONLY | BCM_L3_KEEP_DSTMAC | BCM_L3_KEEP_VLAN;

    bcm_l3_egress_t_init(&l3eg);
    rv = bcm_l3_egress_get(unit, cint_mpls_sr_info.arp_encap_id, &l3eg);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_egress_get\n");
        return rv;
    }

    l3eg.encap_id = cint_mpls_sr_info.arp_encap_id;
    l3eg.intf = jr_outer_most_tunnel_if;

    rv = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid_null);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_l3_egress_create\n");
        return rv;
    }
    printf("Created ARP-id = 0x%08x, allocation_flags = 0x%08x , flags = 0x%08x, flags2 = 0x%08x \n", l3eg.encap_id, flags, l3eg.flags, l3eg.flags2);

    return rv;

}

/**
 * MPLS tunnel creating for label push
*/
int
mpls_segment_routing_encap_entry_init(
    int unit,
    int num_of_labels,
    bcm_mpls_egress_label_t * label_array)
{
    int rv = BCM_E_NONE;
    int i;

    for (i = 0; i < num_of_labels; i++)
    {
        bcm_mpls_egress_label_t_init(&label_array[i]);
        /*EXP Configuration*/
        if((!is_device_or_above(unit,ARAD_PLUS)) || cint_mpls_sr_info.pipe_mode_exp_set ) {
            if (is_device_or_above(unit, JERICHO2)) {
                label_array[i].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
            } else {
                label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET  ;
            }
            label_array[i].exp = cint_mpls_sr_info.exp;
        } else {
            if (is_device_or_above(unit, JERICHO2)) {
                label_array[i].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
            } else {
                label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY  ;
            }
        }

        /*TTL Configuration*/
        label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        if (is_device_or_above(unit, JERICHO2)) {

            label_array[i].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        } else {
            label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
        }
        label_array[i].ttl = cint_mpls_sr_info.ttl;


        if (!is_device_or_above(unit, JERICHO2)) {
            label_array[i].action = BCM_MPLS_EGRESS_ACTION_PUSH;
            label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
        }
        label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID;
    }
    return rv;

}

/*
  * MPLS tunnel creating for label push
  * B1oB2oC1oC2oC3oD1
  */
int
mpls_segment_routing_encap_configuration(
    int unit,
    uint32 force_flags)
{
    int rv, i;
    int is_jr2_or_above,is_jr_plus_or_above,is_jr_or_above;
    int num_of_labels=2;
    bcm_mpls_egress_label_t label_array[2];

    is_jr2_or_above = is_device_or_above(unit, JERICHO2);

    if (!is_jr2_or_above) {
        rv = bcm_switch_control_set(unit, bcmSwitchMplsPipeTunnelLabelExpSet, cint_mpls_sr_info.pipe_mode_exp_set);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_switch_control_set\n");
            return rv;
        }
    }
    /*
        * Step1, configure C3 and D1 labels for JR2
        */
    if (is_jr2_or_above) {

        rv= mpls_segment_routing_encap_entry_init(unit,num_of_labels,label_array);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in mpls_segment_routing_encap_entry_init\n");
            return rv;
        }


        for (i = 0; i < num_of_labels; i++){
            label_array[i].flags |= force_flags;
            label_array[i].l3_intf_id = cint_mpls_sr_info.arp_encap_id;
            label_array[i].tunnel_id = cint_mpls_sr_info.egr_mpls_tunnel_encap_id[2];
            if (is_jr2_or_above) {
                label_array[i].encap_access = bcmEncapAccessTunnel2;
            }
        }
        label_array[0].label = cint_mpls_sr_info.encap_label[1];
        label_array[1].label = cint_mpls_sr_info.encap_label[0];
        rv = bcm_mpls_tunnel_initiator_create(unit, 0, num_of_labels, label_array);

        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_mpls_tunnel_initiator_create\n");
            return rv;
        }
        cint_mpls_sr_info.egr_mpls_tunnel_encap_id[2]=label_array[0].tunnel_id;

    }

    /*
        * Step 2, configure C1,C2 labels
        */
    rv= mpls_segment_routing_encap_entry_init(unit,num_of_labels,label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_segment_routing_encap_entry_init\n");
        return rv;
    }
    for (i = 0; i < num_of_labels; i++){

        label_array[i].flags |= force_flags;
        if (!is_jr2_or_above) {
            label_array[i].l3_intf_id = cint_ipv4_route_info.out_rif;
            label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_WIDE;
        } else {
            label_array[i].l3_intf_id = cint_mpls_sr_info.egr_mpls_tunnel_encap_id[2];
        }
        label_array[i].tunnel_id = cint_mpls_sr_info.egr_mpls_tunnel_encap_id[1];
        if (is_jr2_or_above) {
            label_array[i].encap_access = bcmEncapAccessTunnel1;
        }
    }
    label_array[0].label = cint_mpls_sr_info.encap_label[3];
    label_array[1].label = cint_mpls_sr_info.encap_label[2];

    rv = bcm_mpls_tunnel_initiator_create(unit, 0, num_of_labels, label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }
    cint_mpls_sr_info.egr_mpls_tunnel_encap_id[1]=label_array[0].tunnel_id;
    jr_outer_most_tunnel_if = label_array[0].tunnel_id;


    /*
        * Step 3, configure b1,b2 labels
        */
    rv= mpls_segment_routing_encap_entry_init(unit,num_of_labels,label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_segment_routing_encap_entry_init\n");
        return rv;
    }
    for (i = 0; i < num_of_labels; i++){

        label_array[i].flags |= force_flags;
        label_array[i].l3_intf_id = cint_mpls_sr_info.egr_mpls_tunnel_encap_id[1];
        label_array[i].tunnel_id = cint_mpls_sr_info.egr_mpls_tunnel_encap_id[0];
        if (is_jr2_or_above) {
            label_array[i].encap_access = bcmEncapAccessNativeArp;
        }

    }
    label_array[0].label = cint_mpls_sr_info.encap_label[5];
    label_array[1].label = cint_mpls_sr_info.encap_label[4];
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, num_of_labels, label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }
    cint_mpls_sr_info.egr_mpls_tunnel_encap_id[0]=label_array[0].tunnel_id;

    /*
        * Step 4, onfigure C3 and D1 for JR1
        */
    if (!is_jr2_or_above) {

        int c3_gport,d1_gport;
        uint64 wide_data;
        /*Configure C3 label*/
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(c3_gport, cint_mpls_sr_info.egr_mpls_tunnel_encap_id[1]);

        COMPILER_64_SET(wide_data, 0, cint_mpls_sr_info.encap_label[1]);

        rv = bcm_port_wide_data_set(unit, c3_gport, BCM_PORT_WIDE_DATA_EGRESS, wide_data);
        if (rv != BCM_E_NONE) {
            printf("Error (%d), in bcm_port_wide_data_set\n", rv);
            return rv;
        }

        rv = bcm_port_control_set(unit, c3_gport, bcmPortControlMPLSEncapsulateAdditionalLabel, 1);
        if (rv != BCM_E_NONE) {
            printf("Error (%d), in bcm_port_control_set\n", rv);
            return rv;
        }

        /* Configure D1 label*/
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(d1_gport, cint_mpls_sr_info.arp_encap_id);

        COMPILER_64_SET(wide_data, 0, cint_mpls_sr_info.encap_label[0]);

        rv = bcm_port_wide_data_set(unit, d1_gport, BCM_PORT_WIDE_DATA_EGRESS, wide_data);
        if (rv != BCM_E_NONE) {
            printf("Error (%d), in bcm_port_wide_data_set\n", rv);
            return rv;
        }

        rv = bcm_port_control_set(unit, d1_gport, bcmPortControlMPLSEncapsulateAdditionalLabel, 1);
        if (rv != BCM_E_NONE) {
            printf("Error (%d), in bcm_port_control_set\n", rv);
            return rv;
        }
    }

    return rv;
}


 /**
  * Port, RIF and ARP configuration for mpls application
 */
 int
mpls_segment_routing_l2_l3_configuration(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{
    int rv;

    rv = mpls_segment_routing_init(ingress_unit,egress_unit,in_sys_port,out_sys_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, mpls_segment_routing_init\n");
        return rv;
    }

    /*
        * step 1.ingress L2/L3 configuration(Port,In-RIF)
        */
    rv = multi_dev_ingress_port_configuration(ingress_unit,in_sys_port,cint_ipv4_route_info.in_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multi_dev_ingress_port_configuration\n");
        return rv;
    }

    rv = multi_dev_ipv4_uc_l3_intf_configuration(ingress_unit,in_sys_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multi_dev_ipv4_uc_l3_intf_configuration\n");
        return rv;
    }


    /*
        * step 2. egress L2/L3 configuration(Port, Out-RIF, ARP)
        */
    rv = multi_dev_egress_port_configuration(egress_unit,out_sys_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multi_dev_egress_port_configuration\n");
        return rv;
    }

    rv = multi_dev_ipv4_uc_l3_intf_configuration(egress_unit,cint_ipv4_route_info.out_local_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multi_dev_ipv4_uc_l3_intf_configuration\n");
        return rv;
    }

    rv = mpls_segment_routing_arp_configuration(egress_unit,cint_ipv4_route_info.out_local_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, mpls_segment_routing_arp_configuration\n");
        return rv;
    }

    return rv;

}

/**
 * MPLS segment routing
 * Max pushing label number: 6
 * Applicable for JR1 and above
*/
 int
 multi_dev_mpls_segment_routing_example(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port,
    int with_entropy)
{

    int rv = BCM_E_NONE;
    int tunnel_force_flags = 0;
    if (with_entropy)
    {
         tunnel_force_flags= BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE |BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE;
    }

    /*
        * step 1: create in and out rif
        */
    rv = mpls_segment_routing_l2_l3_configuration(ingress_unit, egress_unit,in_sys_port,out_sys_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in mpls_segment_routing_l2_l3_configuration\n", rv);
        return rv;
    }

    /*
        * step 2: tunnel configuration
        */
    rv =  mpls_segment_routing_encap_configuration(egress_unit,tunnel_force_flags);
    if (rv != BCM_E_NONE)
    {
         printf("Error (%d), in mpls_segment_routing_encap_configuration\n", rv);
         return rv;
    }

    /*
        * step 3: update tunnel point to ARP,FEC point to tunnel
        */
    if (!is_device_or_above(egress_unit, JERICHO2)) {

        rv = mpls_segment_routing_arp_update(egress_unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in mpls_segment_routing_arp_update \n");
            return rv;
        }
    }

    rv = mpls_segment_routing_fec_configuration(ingress_unit, out_sys_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_segment_routing_fec_configuration \n");
        return rv;
    }

    /*
        * step 4: push over IPv4
        */
    rv = add_host_ipv4(ingress_unit, cint_mpls_sr_info.dip_into_tunnel, cint_ipv4_route_info.vrf, cint_mpls_sr_info.fec_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in add_host_ipv4\n", rv);
        return rv;
    }
    return rv;

}



