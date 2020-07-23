/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~MPLS Check Version~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_sand_mpls_ingress_push.c
 * Purpose: Verify ingress push command with EEI.
 * traffic process:
 *  1.  IPv4 packet route ,destination resolved as FEC(EEI+FEC);
 *  2.  EEI--->MPLS push command; FEC---->Out-LIf(MPLS tunnel EEDB)+out-Port;
 *  3.  Outgoing packet is encapsulated with MPLS stack, the inner most label comes from EEI;
 *
 * Test Scenario 
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_mpls.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../src/examples/sand/cint_sand_mpls_ingress_push.c
 * cint
 * mpls_ingress_push_l3_example(0,200,201);
 * exit;
 *
 *  1: IP Routing into EEI with MPLS Tunnel: payo127.255.255.4oE  ->  payo127.255.255.4o1000o400o401o402oE. 
 * tx 1 psrc=201 data=0x0000000000110000000000008100706408004500004c00000000403dfb71000000007fffff04000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 201, Destination port: 200 
 *  Data: 0x000000000022000000000011810000c88847001920140019101400190014003e813f4500004c000000003f3dfc71000000007fffff04000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b000000000000 
 */


 struct cint_mpls_ingress_push_info_s 
{
    int in_rif;                             /* access RIF */
    int out_rif;                            /* out RIF */
    int vrf;                                /*  VRF */
    bcm_mac_t my_mac;                       /* mac for Incoming L3 intf */
    bcm_mac_t next_hop_mac;                 /* next hop mac for outgoing station */
    bcm_ip_t dip_for_common_tunnel;         /* dip routing into tunnel */
    bcm_ip_t dip_for_tunnel_with_eli;       /* dip routing into tunnel with eli */
    int ingress_push_label;                 /*label pushed by eei*/
    int ingress_push_label_with_eli;        /*label pushed by eei with eli*/

    int mpls_tunnel_label[4];               /*mpls tunnel label, */

    int mpls_tunnel_id;                     /*mpls tunnel id,*/
    int out_arp_id;                          /*Next hop intf ID*/ 
    int mpls_tunnel_fec_id;                /*FEC ID for routing into common tunnel*/
    int eei_fec_id;                         /*FEC ID for EEI*/
    int eei_with_eli_fec_id;              /*FEC ID for EEI with ELI*/
};
 
 
 cint_mpls_ingress_push_info_s cint_mpls_ingress_push_info=
{
    /*
    * In-RIF,
    */
    100,
    /*
    * Out-RIF,
    */
    200,

    /*
    * vrf,
    */
    1,
    /*
    * my_mac | next_hop_mac 
    */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x11},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x22},

    /*
    * dip_for_common_tunnel
    */
    0x7FFFFF04, /* 127.255.255.4 */
    /*
    * dip_for_tunnel_with_eli
    */
    0x7FFF0304, /* 127.255.3.4 */
    /*
     * ingress_push_label
     */
     1000,
    /*
     * ingress_push_label_with_eli
     */
     1001,
    /*
    * mpls_tunnel_label,
    */
    {400,401,402,403},
    /*
    * out_arp_id,
    */
    0x22b2b,
};


/**
 * L2 port init
 * INPUT:
 *   in_port    - traffic incoming port 
 *   out_port - traffic outgoing port
*/
 int
 mpls_ingress_push_l2_port_configuration(
    int unit,
    int in_port,
    int out_port)
{
    int rv;

    rv = bcm_vlan_gport_add(unit, cint_mpls_ingress_push_info.in_rif, in_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_gport_add \n");
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, cint_mpls_ingress_push_info.out_rif, out_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_gport_add \n");
        return rv;
    }

    return rv;

}
 
/**
 * L3 intf init
 * INPUT:
 *   in_port    - traffic incoming port 
 *   out_port - traffic outgoing port
*/
 int
 mpls_ingress_push_l3_intf_configuration(
    int unit,
    int in_port,
    int out_port)
{
    int rv;

    rv = in_port_intf_set(unit, in_port, cint_mpls_ingress_push_info.in_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }
    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf(" Error, out_port_set intf_out out_port %d\n",out_port);
        return rv;
    }


    sand_utils_l3_eth_rif_s eth_rif_structure;

    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_mpls_ingress_push_info.in_rif, 0, 0, cint_mpls_ingress_push_info.my_mac, cint_mpls_ingress_push_info.vrf);
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
     printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
     return rv;
    }

    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_mpls_ingress_push_info.out_rif, 0, 0, cint_mpls_ingress_push_info.my_mac, cint_mpls_ingress_push_info.vrf);
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
     printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
     return rv;
    }

    return rv;

}

/**
 * MPLS tunnel adding
 * INPUT:
 *   in_port    - traffic incoming port 
 *   out_port - traffic outgoing port
*/
 int
 mpls_ingress_push_mpls_tunnel_configuration(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    int label_index;
    bcm_mpls_egress_label_t label_array[4];
    int tunnel_ttl[4] = {20,20,20,20};

    /** 
     * step 1: multiple label tunnel.
     */
    for (label_index = 0; label_index < 2; label_index++)
    {
        bcm_mpls_egress_label_t_init(&label_array[label_index]);
    
        /** Labels with the biggest member index*/
        label_array[label_index].label = cint_mpls_ingress_push_info.mpls_tunnel_label[label_index];
        label_array[label_index].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT; 
        if (is_device_or_above(unit, JERICHO2)){
            label_array[label_index].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
            label_array[label_index].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        } else {
            label_array[label_index].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
            label_array[label_index].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        }

        label_array[label_index].action = BCM_MPLS_EGRESS_ACTION_PUSH;
        label_array[label_index].ttl = tunnel_ttl[label_index];
        /*Only outermost entry carry arp*/
        if (label_index == 1) {
            label_array[label_index].l3_intf_id = cint_mpls_ingress_push_info.out_arp_id;
        }
    }
    /** 
     *Create the mpls tunnel with one call.
     */
    rv = sand_mpls_tunnel_initiator_create(unit, 0, 2, label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    cint_mpls_ingress_push_info.mpls_tunnel_id = label_array[0].tunnel_id;

    return rv;
}

/**
 * ARP adding
 * INPUT:
 *   in_port    - traffic incoming port 
 *   out_port - traffic outgoing port
*/
 int
 mpls_ingress_push_arp_configuration(
    int unit,
    int in_port,
    int out_port)
{
    int rv;

    sand_utils_l3_arp_s arp_structure;

    /*
     * Configure ARP entry      
     */
    sand_utils_l3_arp_s_common_init(unit, 0, &arp_structure, 0, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION, cint_mpls_ingress_push_info.next_hop_mac, cint_mpls_ingress_push_info.out_rif);
    rv = sand_utils_l3_arp_create(unit, &arp_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
        return rv;
    }
    cint_mpls_ingress_push_info.out_arp_id = arp_structure.l3eg_arp_id;

    return rv;
}


/**
 * FEC entry adding
 * INPUT:
 *   in_port    - traffic incoming port 
 *   out_port - traffic outgoing port
*/
 int
 mpls_ingress_push_fec_entry_configuration(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    int tunnel_num = 0;
    sand_utils_l3_fec_s fec_structure;
    bcm_failover_t failover_id_fec = 0;
    int is_jr2_mode;

    is_jr2_mode = is_device_or_above(unit, JERICHO2) ? *(dnxc_data_get(unit,  "headers", "system_headers", "system_headers_mode", NULL)) : 0;

    /*
     * Step1: create fec for mpls tunnel
     */
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    BCM_GPORT_LOCAL_SET(fec_structure.destination, out_port);
    fec_structure.tunnel_gport = cint_mpls_ingress_push_info.mpls_tunnel_id;
    fec_structure.flags = BCM_L3_2ND_HIERARCHY;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }
    cint_mpls_ingress_push_info.mpls_tunnel_fec_id = fec_structure.l3eg_fec_id;

    /*
     * Step2: create fec for eei
     */

    /*
     * JR, 1st level FEC does not support non-protection entry.
     */
    failover_id_fec = 0;
    if (!is_device_or_above(unit,JERICHO_B0)) {
        rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &failover_id_fec);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_failover_create\n");
            return rv;
        }
        rv = bcm_failover_set(unit, failover_id_fec, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_failover_set\n");
            return rv;
        }

    }

    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    BCM_GPORT_FORWARD_PORT_SET(fec_structure.destination,cint_mpls_ingress_push_info.mpls_tunnel_fec_id);
    fec_structure.mpls_action = is_jr2_mode ? BCM_MPLS_EGRESS_ACTION_SWAP_OR_PUSH : BCM_MPLS_EGRESS_ACTION_PUSH;
    fec_structure.mpls_label = cint_mpls_ingress_push_info.ingress_push_label;
    if (!is_device_or_above(unit,JERICHO_B0)) {
        fec_structure.failover_id = failover_id_fec;
    }
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }
    cint_mpls_ingress_push_info.eei_fec_id = fec_structure.l3eg_fec_id;


    return rv;
}

/**
 * forward entry adding(ILM, IPv4 Route)
 * INPUT:
 *   in_port    - traffic incoming port 
 *   out_port - traffic outgoing port
*/
 int
 mpls_ingress_push_forward_entry_configuration(
    int unit,
    int in_port,
    int out_port)
{
    int rv;

    /** 
     * step 1:routing to common push 
     */
    rv = add_host_ipv4(unit, cint_mpls_ingress_push_info.dip_for_common_tunnel, cint_mpls_ingress_push_info.vrf, cint_mpls_ingress_push_info.eei_fec_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in add_host_ipv4\n", rv);
        return rv;
    }

 
    return rv;
}

/**
 * Main entrance for mpls check ver configuration
 * INPUT:
 *   in_port	- traffic incoming port 
 *   out_port - traffic outgoing port
*/
 int
 mpls_ingress_push_l3_example(
    int unit,
    int in_port,
    int out_port)
{
    int rv;

    rv = mpls_ingress_push_l2_port_configuration(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_ingress_push_l2_port_configuration\n");
     return rv;
    }

    rv = mpls_ingress_push_l3_intf_configuration(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_ingress_push_l3_intf_configuration\n");
     return rv;
    }

    rv = mpls_ingress_push_arp_configuration(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_ingress_push_arp_configuration\n");
     return rv;
    }

    rv = mpls_ingress_push_mpls_tunnel_configuration(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_ingress_push_mpls_tunnel_configuration\n");
     return rv;
    }

    rv = mpls_ingress_push_fec_entry_configuration(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_ingress_push_fec_entry_configuration\n");
     return rv;
    }

    rv = mpls_ingress_push_forward_entry_configuration(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_ingress_push_forward_entry_configuration\n");
     return rv;
    }

    return rv;
}

