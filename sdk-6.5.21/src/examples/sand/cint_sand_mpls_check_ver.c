/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~MPLS Check Version~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_sand_mpls_check_ver.c
 * Purpose: Verify various MPLS applications operate
 *
 * MPLS allows for various applications utilizing encapsulation, termination and/or tunneling. 
 * This cint configures the relevant MPLS parameters.
 * Once the configuration has been completed, it can be tested via transmitting packets
 * with the relevant parameters and checking the received packets.
 * Note: Test must be run with FRR, entropy label indicator and termination label index enabled
 * 
 * CINT Usage:
 *  1.  Initialize MPLS parameters
 *
 * Basic example:
 *      BCM> cd ../../../../src
 *      BCM> cint examples/sand/utility/cint_sand_utils_global.c
 *      BCM> cint examples/sand/cint_ip_route_basic.c
 *      BCM> cint examples/sand/utility/cint_sand_utils_mpls.c
 *      BCM> cint examples/sand/utility/cint_sand_utils_l3.c
 *      BCM> cint examples/dnx/utility/cint_dnx_utils_vpls.c
 *      BCM> cint examples/sand/cint_sand_mpls_check_ver.c
 *      BCM> cint
 *      cint> print mpls_check_ver_example(0, 200, 201);
 * 
 * The above script runs the configuration of the system, packets are transmitted via the TCL Packet Launcher.
 * Below is a list of all relevant configurations and desicions:
 *  0    1o5000o34o33o32oE           ->  1o8001oE                            (3 label termiantion)
 *  1    2o5000o33o32oE              ->  2o8001oE                            (2 label termiantion)
 *  2    3o5000o32oE                 ->  3o8001oE                            (1 label termiantion)
 *  3    4o5000oE                    ->  4o8001oE                            (Simple LSR)
 *  4    5o4441oE                    ->  5o4001o100oE                        (1 label encapsualtion)
 *  5    6o4442oE                    ->  6o4002o200o201oE                    (2 label encapsualtion)
 *  6    7o4443oE                    ->  7o4003o300o301o302oE                (3 label encapsualtion)
 *  7    8o4444oE                    ->  8o4004o400o401o402o403oE            (4 label encapsualtion)
 *  8    9o5005o129o128oE            ->  9o5006oE                            (FRR termination)
 *  9    10o5005o129oE               ->  10o5006oE                           (FRR, not FRR termination)
 *  10   11o127.255.255.4oE          ->  11o127.255.255.4o444o4444oE         (IP routing into tunnel)
 *  11   12o127.255.3.4o500oE        ->  12o127.255.3.4oE                    (PHP)
 *  12   13o127.255.255.3o64o32oE    ->  13o127.255.255.3oE                  (IP routing after termination)
 *  13   14o127.255.255.4o64o32oE    ->  14o127.255.255.4o444o4444oE         (IP routing after termination into tunnel)
 *  14   15o127.255.255.3o32o7o42oE  ->  15o127.255.255.3oE                  (ELI+ENT termiantion)
 *  15   16o127.255.255.16oE         ->  16o127.255.255.16o703488o7o77o777oE (ELI+ENT encapsualation)
 */


 struct cint_mpls_check_ver_info_s 
{
    int in_rif;                             /* access RIF */
    int out_rif;                            /* out RIF */
    int vrf;                            /*  VRF */
    bcm_mac_t my_mac;                       /* mac for Incoming L3 intf */
    bcm_mac_t next_hop_mac;                 /* next hop mac for outgoing station */
    bcm_ip_t dip_routing_into_tunnel;        /* dip routing into tunnel */
    bcm_ip6_t dip6_routing_into_tunnel;      /* dip6 routing into tunnel */
    bcm_ip_t dip_routing_into_php;           /* dip routing into php */
    bcm_ip_t dip_routing_after_termination;  /* dip routing after termination */
    bcm_ip_t dip_routing_with_el;            /* dip routing into tunnel with ELI */
    int multi_label_tunnel_label[4][4];      /*mpls tunnel label, for case 4-7*/
    int common_mpls_tunnel_label[2];      /*mpls tunnel label, for case 10,13*/
    int eli_mpls_tunnel_label[2];      /*eli mpls tunnel label, for case 15*/
    mpls_tunnel_switch_create_s   mpls_term_tunnel[5];  /*mpls tunnel termination info*/
    int   mpls_term_tunnel_index[5];        /*mpls tunnel termination info*/
    int   mpls_term_with_eli[5];        /*mpls tunnel termination with entropy label*/
    int   mpls_term_is_bos[5];          /* indicated if this term label is BOS */
    int   frr_label;                         /*frr label for case 8,9*/
    int   term_by_frr_label;                 /*label terminated by FRR for case 8,9*/
    int lsr_incoming_label[6];              /*incoming label for forwarding*/
    int lsr_outgoing_label[6];              /*outgoing label for forwarding*/
    int lsr_incoming_label_to_php;          /*incoming label for PHP*/

    int multi_label_tunnel_id[4];            /*mpls tunnel id, for case 4-7*/
    int common_mpls_tunnel_id;               /*mpls tunnel id, for case 10,13*/
    int eli_mpls_tunnel_id;                  /*mpls tunnel id, for case 15*/
    int out_arp_id;                          /*Next hop intf ID*/ 
    int routing_to_rif_fec_id;               /*FEC ID for routing into IP*/
    int common_tunnel_fec_id;                /*FEC ID for routing into common tunnel*/
    int tunnel_with_eli_fec_id;              /*FEC ID for routing into mpls tunnel with ELI*/
    int multi_label_tunnel_fec_id[4];        /*mpls tunnel fec id, for case 4-7*/

};
 
 
 cint_mpls_check_ver_info_s cint_mpls_check_ver_info=
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
    * dip_routing_into_tunnel
    */
    0x7FFFFF04, /* 127.255.255.4 */
    /*
    * dip6_routing_into_tunnel
    */
    {0x02,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03},
    /*
    * dip_routing_into_php
    */
    0x7FFF0304, /* 127.255.3.4 */
    /*
    * dip_routing_after_termination
    */
    0x7FFFff03, /* 127.255.255.3 */
    /*
    * dip_routing_with_el
    */
    0x7FFFff10, /* 127.255.255.16 */
    /*
    * multi_label_tunnel_label,
    */
    {
        {100,  0,  0,  0},
        {200,201,  0,  0},
        {300,301,302,  0},
        {400,401,402,403},
    },
    /*
    * common_mpls_tunnel_label,
    */
    {444,4444   },
    /*
    * eli_mpls_tunnel_label,
    */
    {77,777   },

    /*
    * mpls_term_tunnel,
    */
    {
        {BCM_MPLS_SWITCH_ACTION_POP,32},
        {BCM_MPLS_SWITCH_ACTION_POP,33},
        {BCM_MPLS_SWITCH_ACTION_POP,34},
        {BCM_MPLS_SWITCH_ACTION_POP,64},
        {BCM_MPLS_SWITCH_ACTION_POP,42},
    },
    /*
     * mpls_term_tunnel_index,
     */
    {1, 2, 3, 2, 1},
    /*
     * mpls_term_with_eli,
     */
    {0, 0, 0, 0, 1},
    /*
     * mpls_term_is_bos, entropy is BOS
     */
    {0, 0, 0, 1, 1},
    /*
     * frr_label,
     */
    128,
    /*
     * term_by_frr_label,
     */
    129,
    /*
    * lsr_incoming_label,
    */
    {5000,4441,4442,4443,4444,5005},

    /*
    * lsr_incoming_label,
    */
    {8001,4001,4002,4003,4004,5006},

    /*
    * lsr_incoming_label_to_php,
    */
    500,       

};



 
/**
 * L3 intf init
 * INPUT:
 *   in_port    - traffic incoming port 
 *   out_port - traffic outgoing port
*/
 int
 mpls_check_ver_l3_intf_configuration(
    int unit,
    int in_port,
    int out_port)
{
    int rv;

    rv = in_port_intf_set(unit, in_port, cint_mpls_check_ver_info.in_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }
    rv = in_port_intf_set(unit, out_port, cint_mpls_check_ver_info.in_rif);
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

    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_mpls_check_ver_info.in_rif, 0, 0, cint_mpls_check_ver_info.my_mac, cint_mpls_check_ver_info.vrf);
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
     printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
     return rv;
    }

    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_mpls_check_ver_info.out_rif, 0, 0, cint_mpls_check_ver_info.my_mac, cint_mpls_check_ver_info.vrf);
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
 mpls_check_ver_mpls_tunnel_configuration(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    int label_index,tunnel_num;
    bcm_mpls_egress_label_t label_array[4];
    int tunnel_ttl[4] = {20,40,60,64};
    int is_jr2;
    int start_index;

    is_jr2 = is_device_or_above(unit, JERICHO2);

    if (!is_jr2) {
        rv = bcm_switch_control_set(unit, bcmSwitchMplsPipeTunnelLabelExpSet, 1);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_switch_control_set\n");
            return rv;
        }
    }
    /** step 1: multiple label tunnel.
      * for case 4-7
      */
    for (tunnel_num = 0;tunnel_num < 4; tunnel_num++) {
        for (label_index = 0; label_index < tunnel_num+1; label_index++)
        {
            bcm_mpls_egress_label_t_init(&label_array[label_index]);
        
            /** Labels with the biggest member index*/
            label_array[label_index].label = cint_mpls_check_ver_info.multi_label_tunnel_label[tunnel_num][label_index];
            label_array[label_index].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
            if (is_jr2){
                label_array[label_index].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
                label_array[label_index].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
            } else {
                label_array[label_index].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
                label_array[label_index].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
            }


            label_array[label_index].action = BCM_MPLS_EGRESS_ACTION_PUSH;
            label_array[label_index].ttl = tunnel_ttl[label_index];
            /*Only outermost entry carry arp*/
            if (label_index == tunnel_num) {
                label_array[label_index].l3_intf_id = cint_mpls_check_ver_info.out_arp_id;
            }
        
        
        }
        /** Create the mpls tunnel with one call.*/
        rv = sand_mpls_tunnel_initiator_create(unit, 0, tunnel_num+1, label_array);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_mpls_tunnel_initiator_create\n");
            return rv;
        }

        cint_mpls_check_ver_info.multi_label_tunnel_id[tunnel_num] = label_array[0].tunnel_id;

    }

    /** step 2: common tunnel.
      * for case 10,13
      */
    for (label_index = 0; label_index < 2; label_index++)
    {
        bcm_mpls_egress_label_t_init(&label_array[label_index]);
    
        /** Labels with the biggest member index*/
        label_array[label_index].label = cint_mpls_check_ver_info.common_mpls_tunnel_label[label_index];
        label_array[label_index].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        if (is_jr2){
            label_array[label_index].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
            label_array[label_index].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        } else {
            label_array[label_index].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
            label_array[label_index].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
        }


        label_array[label_index].action = BCM_MPLS_EGRESS_ACTION_PUSH;
        label_array[label_index].ttl = tunnel_ttl[label_index];
    
        /*Only outermost entry carry arp*/
        if (label_index == 1) {
            label_array[label_index].l3_intf_id = cint_mpls_check_ver_info.out_arp_id;
        }
    
    
    }

    /** Create the mpls tunnel with one call.*/
    rv = sand_mpls_tunnel_initiator_create(unit, 0, 2, label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    cint_mpls_check_ver_info.common_mpls_tunnel_id = label_array[0].tunnel_id;

    /** step 3: common tunnel with ELI/EL.
      * for case 15
      */

    /** In Non_JR2 device, 
     * ELI is encapsulated by general EEDB entry
     */
    if (is_jr2) {
        bcm_mpls_egress_label_t_init(&label_array[0]);
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE | BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE;
        start_index = 0;
    } else {
        bcm_mpls_egress_label_t_init(&label_array[0]);
        
        bcm_switch_control_set(unit,bcmSwitchSetMplsEntropyLabelOffset,0xabc);
        label_array[0].label = 7;

        label_array[0].flags = (BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);  
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE;
        
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
       
        label_array[0].action = BCM_MPLS_EGRESS_ACTION_PUSH;
        label_array[0].ttl = tunnel_ttl[0];
        start_index = 1;
    }

    for (label_index = start_index; label_index < start_index+2; label_index++)
    {
        if (label_index > 0) {
           bcm_mpls_egress_label_t_init(&label_array[label_index]);
        }
    
        /** Labels with the biggest member index*/
        label_array[label_index].label = cint_mpls_check_ver_info.eli_mpls_tunnel_label[label_index-start_index];
        label_array[label_index].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;  
        
        if (is_jr2){
            label_array[label_index].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
            label_array[label_index].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        } else {
            label_array[label_index].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
            label_array[label_index].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
        }

        label_array[label_index].action = BCM_MPLS_EGRESS_ACTION_PUSH;
        if (start_index == 0) {
          label_array[label_index].ttl = tunnel_ttl[label_index+1];
        } else {
          label_array[label_index].ttl = tunnel_ttl[label_index];
        }
        /*Only outermost entry carry arp*/
        if (label_index == start_index+1) {
            label_array[label_index].l3_intf_id = cint_mpls_check_ver_info.out_arp_id;
        }
    
    
    }

    /** Create the mpls tunnel with one call.*/
    rv = sand_mpls_tunnel_initiator_create(unit, 0, 2+start_index, label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    cint_mpls_check_ver_info.eli_mpls_tunnel_id = label_array[0].tunnel_id;


    return rv;
}

/**
 * ARP adding
 * INPUT:
 *   in_port    - traffic incoming port 
 *   out_port - traffic outgoing port
*/
 int
 mpls_check_ver_arp_configuration(
    int unit,
    int in_port,
    int out_port)
{
    int rv;

    sand_utils_l3_arp_s arp_structure;

    /*
     * Configure native ARP entry      */
    sand_utils_l3_arp_s_common_init(unit, 0, &arp_structure, 0, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION, cint_mpls_check_ver_info.next_hop_mac, cint_mpls_check_ver_info.out_rif);
    rv = sand_utils_l3_arp_create(unit, &arp_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
        return rv;
    }
    cint_mpls_check_ver_info.out_arp_id = arp_structure.l3eg_arp_id;

    return rv;
}


/**
 * FEC entry adding
 * INPUT:
 *   in_port    - traffic incoming port 
 *   out_port - traffic outgoing port
*/
 int
 mpls_check_ver_fec_entry_configuration(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    int tunnel_num = 0;
    sand_utils_l3_fec_s fec_structure;

    /** step 1: multiple label tunnel.
     * for case 4-7
     */
    for (tunnel_num = 0; tunnel_num < 4; tunnel_num++) {
        sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
        BCM_GPORT_LOCAL_SET(fec_structure.destination, out_port);
        fec_structure.tunnel_gport = cint_mpls_check_ver_info.multi_label_tunnel_id[tunnel_num];
        rv = sand_utils_l3_fec_create(unit, &fec_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC only\n");
            return rv;
        }
        cint_mpls_check_ver_info.multi_label_tunnel_fec_id[tunnel_num] = fec_structure.l3eg_fec_id;
    }

    /** step 2: common tunnel.
     * for case 10,13
     */
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    BCM_GPORT_LOCAL_SET(fec_structure.destination, out_port);
    fec_structure.tunnel_gport = cint_mpls_check_ver_info.common_mpls_tunnel_id;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }
    cint_mpls_check_ver_info.common_tunnel_fec_id = fec_structure.l3eg_fec_id;

    /** step 3: common tunnel with ELI/EL.
     * for case 15
     */
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    BCM_GPORT_LOCAL_SET(fec_structure.destination, out_port);
    fec_structure.tunnel_gport = cint_mpls_check_ver_info.eli_mpls_tunnel_id;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }
    cint_mpls_check_ver_info.tunnel_with_eli_fec_id = fec_structure.l3eg_fec_id;

    /** step 4: RIF+ARP.
     * for case 1-4,8-9,11-12,14
     */
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    BCM_GPORT_LOCAL_SET(fec_structure.destination, out_port);
    fec_structure.tunnel_gport = cint_mpls_check_ver_info.out_arp_id;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }
    cint_mpls_check_ver_info.routing_to_rif_fec_id = fec_structure.l3eg_fec_id;

    return rv;
}

/**
 * forward entry adding(ILM, IPv4/IPv6 Route)
 * INPUT:
 *   in_port    - traffic incoming port 
 *   out_port - traffic outgoing port
*/
 int
 mpls_check_ver_forward_entry_configuration(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;
    int index = 0;
    int is_jr2;
    int is_jr = 0;

    is_jr = is_device_or_above(unit, JERICHO);
    is_jr2 = is_device_or_above(unit, JERICHO2);

    /** step 1: 5000---->8000+L3 intf.
     * for case 1-4
     */
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_SWAP;
    entry.label = cint_mpls_check_ver_info.lsr_incoming_label[0];
    entry.egress_label.label = cint_mpls_check_ver_info.lsr_outgoing_label[0];
    entry.flags |= BCM_MPLS_SWITCH_TTL_DECREMENT;
    BCM_L3_ITF_SET(entry.egress_if,BCM_L3_ITF_TYPE_FEC,cint_mpls_check_ver_info.routing_to_rif_fec_id);
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_mpls_tunnel_switch_create\n", rv);
        return rv;
    }

    /** step 2: multiple label encap.
     * for case 5-8
     */
    for (index = 1;index < 5; index++) {
        bcm_mpls_tunnel_switch_t_init(&entry);
        entry.action = BCM_MPLS_SWITCH_ACTION_SWAP;
        entry.label = cint_mpls_check_ver_info.lsr_incoming_label[index];
        entry.egress_label.label = cint_mpls_check_ver_info.lsr_outgoing_label[index];
        entry.flags |= BCM_MPLS_SWITCH_TTL_DECREMENT;
        BCM_L3_ITF_SET(entry.egress_if,BCM_L3_ITF_TYPE_FEC,cint_mpls_check_ver_info.multi_label_tunnel_fec_id[index-1]);
        rv = bcm_mpls_tunnel_switch_create(unit, &entry);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_mpls_tunnel_switch_create\n", rv);
            return rv;
        }
    }

    /** step 3:LSR for FRR traffic
     * for case 9
     */
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_SWAP;
    entry.label = cint_mpls_check_ver_info.lsr_incoming_label[5];
    entry.egress_label.label = cint_mpls_check_ver_info.lsr_outgoing_label[5];
    entry.flags |= BCM_MPLS_SWITCH_TTL_DECREMENT;
    BCM_L3_ITF_SET(entry.egress_if,BCM_L3_ITF_TYPE_FEC,cint_mpls_check_ver_info.routing_to_rif_fec_id);
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_mpls_tunnel_switch_create\n", rv);
        return rv;
    }

    /** step 4:PHP
     * for case 12
     */
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_PHP;
    entry.label = cint_mpls_check_ver_info.lsr_incoming_label_to_php;
    if (is_jr2) {
        entry.egress_label.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        entry.egress_label.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
        entry.flags |= BCM_MPLS_SWITCH_TTL_DECREMENT;
    } else {
        entry.flags |= BCM_MPLS_SWITCH_TTL_DECREMENT|BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;
    }
    /**
     * JR2 could parse the next protocol by egress parser,
     * JR need explicitly indication for next protocol
     */
    if (!is_jr2) {
       entry.flags |= BCM_MPLS_SWITCH_NEXT_HEADER_IPV4 | BCM_MPLS_SWITCH_NEXT_HEADER_IPV6;
    }
    BCM_L3_ITF_SET(entry.egress_if,BCM_L3_ITF_TYPE_FEC,cint_mpls_check_ver_info.routing_to_rif_fec_id);
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_mpls_tunnel_switch_create\n", rv);
        return rv;
    }

    /** step 5:routing after termination
     * for case 13,15
     */
    rv = add_host_ipv4(unit, cint_mpls_check_ver_info.dip_routing_after_termination, cint_mpls_check_ver_info.vrf, cint_mpls_check_ver_info.routing_to_rif_fec_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in add_host_ipv4\n", rv);
        return rv;
    }

    /** step 6:routing into tunnel
     * for case 11,14
     */
    rv = add_host_ipv4(unit, cint_mpls_check_ver_info.dip_routing_into_tunnel, cint_mpls_check_ver_info.vrf, cint_mpls_check_ver_info.common_tunnel_fec_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in add_host_ipv4\n", rv);
        return rv;
    }

    /* ARAD PLUS and below need encap_id != 0, which is not relevant */
    if (is_jr) {
        rv = add_host_ipv6(unit, cint_mpls_check_ver_info.dip6_routing_into_tunnel, cint_mpls_check_ver_info.vrf, cint_mpls_check_ver_info.common_tunnel_fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in add_host_ipv6\n", rv);
            return rv;
        }
    }

    /** step 7:routing into tunnel with eli
     * for case 16
     */
    rv = add_host_ipv4(unit, cint_mpls_check_ver_info.dip_routing_with_el, cint_mpls_check_ver_info.vrf, cint_mpls_check_ver_info.tunnel_with_eli_fec_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in add_host_ipv4\n", rv);
        return rv;
    }

    return rv;
}

/**
 * MPLS termination adding
 * INPUT:
 *   in_port    - traffic incoming port 
 *   out_port - traffic outgoing port
*/
 int
 mpls_check_ver_mpls_termination_stack_configuration(
    int unit,
    int in_port,
    int out_port)
{
    bcm_mpls_tunnel_switch_t entry;
    int rv, i,label=0;
    int is_jr;
    int is_arad = 0;

    is_jr = is_device_or_above(unit, JERICHO);

    /*
     * step 1: Create a stack of MPLS labels to be terminated 
     */
    for (i = 0; i < 5; i++)
    {
        bcm_mpls_tunnel_switch_t_init(&entry);

        entry.action = cint_mpls_check_ver_info.mpls_term_tunnel[i].action;
        entry.label = cint_mpls_check_ver_info.mpls_term_tunnel[i].label;
        entry.vpn = cint_mpls_check_ver_info.vrf;
        if (soc_property_get(unit, "mpls_termination_label_index_enable", 0))
        {
            label = entry.label;
            BCM_MPLS_INDEXED_LABEL_SET(&entry.label, &label, cint_mpls_check_ver_info.mpls_term_tunnel_index[i]);
        }
        /*
         * For JR below device, ELI need to terminated by explicit flag*/
        rv = is_arad_only(unit, &is_arad);
        if (rv != BCM_E_NONE) {
          printf("Error, in is_arad_only: %s\n", bcm_errmsg(rv));
        }

        if (!is_jr && cint_mpls_check_ver_info.mpls_term_with_eli[i]) {
            entry.flags |= BCM_MPLS_SWITCH_ENTROPY_ENABLE;
            if (is_arad) {
                entry.flags |= BCM_MPLS_SWITCH_NEXT_HEADER_IPV4 | BCM_MPLS_SWITCH_NEXT_HEADER_IPV6;
            }
        }
        /*for arad only, BOS is part of SEM key*/
        if (is_arad && cint_mpls_check_ver_info.mpls_term_is_bos[i]) {
            entry.flags |= BCM_MPLS_SWITCH_NEXT_HEADER_IPV4 | BCM_MPLS_SWITCH_NEXT_HEADER_IPV6;
        }

        rv = bcm_mpls_tunnel_switch_create(unit, &entry);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_mpls_tunnel_switch_create\n");
            return rv;
        }
    }

    /*Only JR below device support FRR,*/
    if (!is_jr) {

        /* 
         * step 2:Enable Port for FRR */
        rv = bcm_port_control_set(unit, in_port, bcmPortControlMplsFRREnable, 1);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_control_set\n");
            return rv;
        }


        /*
         * step 3: FRR label for case 8 
         */
        bcm_mpls_tunnel_switch_t_init(&entry);

        entry.action = BCM_MPLS_SWITCH_ACTION_POP;
        entry.label = cint_mpls_check_ver_info.frr_label;
        entry.vpn = cint_mpls_check_ver_info.vrf;
        entry.flags = BCM_MPLS_SWITCH_FRR|BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL;
        entry.second_label = cint_mpls_check_ver_info.term_by_frr_label;
        if (soc_property_get(unit, "mpls_termination_label_index_enable", 0))
        {
            label = entry.label;
            BCM_MPLS_INDEXED_LABEL_SET(&entry.label, &label, 1);
            label = entry.second_label;
            BCM_MPLS_INDEXED_LABEL_SET(&entry.second_label, &label, 2);
        }

        rv = bcm_mpls_tunnel_switch_create(unit, &entry);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_mpls_tunnel_switch_create\n");
            return rv;
        }

        /*
         * step 4: FRR label for case 9
         */
        bcm_mpls_tunnel_switch_t_init(&entry);

        entry.action = BCM_MPLS_SWITCH_ACTION_POP;
        entry.label = cint_mpls_check_ver_info.term_by_frr_label;
        entry.vpn = cint_mpls_check_ver_info.vrf;
        if (soc_property_get(unit, "mpls_termination_label_index_enable", 0))
        {
            label = entry.label;
            BCM_MPLS_INDEXED_LABEL_SET(&entry.label, &label, 1);
        }

        rv = bcm_mpls_tunnel_switch_create(unit, &entry);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_mpls_tunnel_switch_create\n");
            return rv;
        }
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
 mpls_check_ver_example(
    int unit,
    int in_port,
    int out_port)
{
    int rv;

    rv = mpls_check_ver_l3_intf_configuration(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_check_ver_l3_intf_configuration\n");
     return rv;
    }

    rv = mpls_check_ver_arp_configuration(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_check_ver_arp_configuration\n");
     return rv;
    }

    rv = mpls_check_ver_mpls_tunnel_configuration(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_check_ver_mpls_tunnel_configuration\n");
     return rv;
    }

    rv = mpls_check_ver_fec_entry_configuration(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_check_ver_fec_entry_configuration\n");
     return rv;
    }

    rv = mpls_check_ver_forward_entry_configuration(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_check_ver_forward_entry_configuration\n");
     return rv;
    }

    rv = mpls_check_ver_mpls_termination_stack_configuration(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_check_ver_mpls_termination_stack_configuration\n");
     return rv;
    }

    return rv;
}


