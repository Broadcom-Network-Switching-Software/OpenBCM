/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~MPLS Check Version~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 *
 * File: cint_dnx_mpls_check_ver.c
 * Purpose: Verify various MPLS applications operate
 *
 * MPLS allows for various applications utilizing encapsulation, termination and/or tunneling.
 * This cint configures the relevant MPLS parameters.
 * Once the configuration has been completed, it can be tested via transmitting packets
 * with the relevant parameters and checking the received packets.
 * 
 * CINT Usage:
 *  1. Initialize MPLS parameters
 *
 * Basic example:
 *      BCM> cd ../../../../src
 *      BCM> cint examples/dnx/utility/cint_dnx_utils_global.c
 *      BCM> cint examples/dnx/l3/cint_dnx_ip_route_basic.c
 *      BCM> cint examples/dnx/utility/cint_dnx_utility_mpls.c
 *      BCM> cint examples/dnx/utility/cint_dnx_utils_l3.c
 *      BCM> cint examples/dnx/utility/cint_dnx_utils_vpls.c
 *      BCM> cint examples/dnx/mpls/cint_dnx_mpls_check_ver.c
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
    int in_rif;                              /* access RIF */
    int out_rif;                             /* out RIF */
    int vrf;                                 /*  VRF */
    bcm_mac_t my_mac;                        /* mac for Incoming L3 intf */
    bcm_mac_t next_hop_mac;                  /* next hop mac for outgoing station */
    bcm_ip_t dip_routing_into_tunnel;        /* dip routing into tunnel */
    bcm_ip6_t dip6_routing_into_tunnel;      /* dip6 routing into tunnel */
    bcm_ip_t dip_routing_into_php;           /* dip routing into php */
    bcm_ip_t dip_routing_after_termination;  /* dip routing after termination */
    bcm_ip_t dip_routing_with_el;            /* dip routing into tunnel with ELI */
    int multi_label_tunnel_label[4][4];      /*mpls tunnel label, for case 4-7*/
    int common_mpls_tunnel_label[2];         /*mpls tunnel label, for case 10,13*/
    int eli_mpls_tunnel_label[2];            /*eli mpls tunnel label, for case 15*/
    int   mpls_term_label[5];                /*mpls tunnel termination info*/
    int   mpls_term_tunnel_index[5];         /*mpls tunnel termination info*/
    int   mpls_term_with_eli[5];             /*mpls tunnel termination with entropy label*/
    int   mpls_term_is_bos[5];               /* indicated if this term label is BOS */
    int   frr_label;                         /*frr label for case 8,9*/
    int   term_by_frr_label;                 /*label terminated by FRR for case 8,9*/
    int lsr_incoming_label[6];               /*incoming label for forwarding*/
    int lsr_outgoing_label[6];               /*outgoing label for forwarding*/
    int lsr_incoming_label_to_php;           /*incoming label for PHP*/
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
    {32, 33, 34, 64, 42},
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
 *   in_port - traffic incoming port 
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

    dnx_utils_l3_eth_rif_s eth_rif_structure;

    dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_mpls_check_ver_info.in_rif, 0, 0, cint_mpls_check_ver_info.my_mac, cint_mpls_check_ver_info.vrf);
    rv = dnx_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
     printf("Error, dnx_utils_l3_eth_rif_create core_native_eth_rif\n");
     return rv;
    }

    dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_mpls_check_ver_info.out_rif, 0, 0, cint_mpls_check_ver_info.my_mac, cint_mpls_check_ver_info.vrf);
    rv = dnx_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
     printf("Error, dnx_utils_l3_eth_rif_create core_native_eth_rif\n");
     return rv;
    }

    return rv;
}

/**
 * MPLS tunnel adding
*/
int
mpls_check_ver_mpls_tunnel_configuration(
    int unit)
{
    int rv;
    int label_index,tunnel_num;
    int tunnel_ttl[4] = {20,40,60,64};
    mpls_util_mpls_tunnel_initiator_create_s mpls_encap_tunnel[4]; /* 1 ~ 4 labels */

    /** step 1: multiple label tunnel.
      * for case 4-7
      */
    for (tunnel_num = 0; tunnel_num < 4; tunnel_num++) {
        mpls_tunnel_initiator_create_s_init(mpls_encap_tunnel,tunnel_num+1);

        for (label_index = tunnel_num; label_index >= 0; label_index--)
        {
            mpls_encap_tunnel[label_index].label[0] = cint_mpls_check_ver_info.multi_label_tunnel_label[tunnel_num][label_index];
            mpls_encap_tunnel[label_index].num_labels = 1;
            mpls_encap_tunnel[label_index].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
            if (label_index == tunnel_num) {
                mpls_encap_tunnel[label_index].l3_intf_id = &cint_mpls_check_ver_info.out_arp_id;
            } else {
                mpls_encap_tunnel[label_index].l3_intf_id = &mpls_encap_tunnel[label_index+1].tunnel_id;
            }
            mpls_encap_tunnel[label_index].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
            mpls_encap_tunnel[label_index].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
            mpls_encap_tunnel[label_index].ttl = tunnel_ttl[label_index];
            mpls_encap_tunnel[label_index].encap_access = bcmEncapAccessTunnel1 + label_index;
        }

        rv = mpls_create_initiator_tunnels(unit, mpls_encap_tunnel, tunnel_num+1);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in mpls_create_initiator_tunnels\n");
            return rv;
        }

        cint_mpls_check_ver_info.multi_label_tunnel_id[tunnel_num] = mpls_encap_tunnel[0].tunnel_id;
    }

    /** step 2: common tunnel.
      * for case 10,13
      */
    mpls_tunnel_initiator_create_s_init(mpls_encap_tunnel,2);

    for (label_index = 1; label_index >= 0; label_index--)
    {
        mpls_encap_tunnel[label_index].label[0] = cint_mpls_check_ver_info.common_mpls_tunnel_label[label_index];
        mpls_encap_tunnel[label_index].num_labels = 1;
        mpls_encap_tunnel[label_index].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        if (label_index == 1) {
            mpls_encap_tunnel[label_index].l3_intf_id = &cint_mpls_check_ver_info.out_arp_id;
        } else {
            mpls_encap_tunnel[label_index].l3_intf_id = &mpls_encap_tunnel[label_index+1].tunnel_id;
        }
        mpls_encap_tunnel[label_index].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        mpls_encap_tunnel[label_index].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        mpls_encap_tunnel[label_index].ttl = tunnel_ttl[label_index];
        mpls_encap_tunnel[label_index].encap_access = bcmEncapAccessTunnel1 + label_index;
    }

    rv = mpls_create_initiator_tunnels(unit, mpls_encap_tunnel, 2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_create_initiator_tunnels\n");
        return rv;
    }

    cint_mpls_check_ver_info.common_mpls_tunnel_id = mpls_encap_tunnel[0].tunnel_id;

    /** step 3: common tunnel with ELI/EL.
      * for case 15
      */

    mpls_tunnel_initiator_create_s_init(mpls_encap_tunnel,2);

    for (label_index = 1; label_index >= 0; label_index--)
    {
        mpls_encap_tunnel[label_index].label[0] = cint_mpls_check_ver_info.eli_mpls_tunnel_label[label_index];
        mpls_encap_tunnel[label_index].num_labels = 1;
        mpls_encap_tunnel[label_index].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        if (label_index == 1) {
            mpls_encap_tunnel[label_index].l3_intf_id = &cint_mpls_check_ver_info.out_arp_id;
        } else {
            mpls_encap_tunnel[label_index].l3_intf_id = &mpls_encap_tunnel[label_index+1].tunnel_id;
            mpls_encap_tunnel[label_index].flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE | BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE;
        }
        mpls_encap_tunnel[label_index].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        mpls_encap_tunnel[label_index].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        mpls_encap_tunnel[label_index].ttl = tunnel_ttl[label_index+1];
        mpls_encap_tunnel[label_index].encap_access = bcmEncapAccessTunnel1 + label_index;
    }

    rv = mpls_create_initiator_tunnels(unit, mpls_encap_tunnel, 2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_create_initiator_tunnels\n");
        return rv;
    }

    cint_mpls_check_ver_info.eli_mpls_tunnel_id = mpls_encap_tunnel[0].tunnel_id;

    return rv;
}

/**
 * ARP entry
*/
int
mpls_check_ver_arp_configuration(
    int unit)
{
    int rv;

    dnx_utils_l3_arp_s arp_structure;

    /*
     * Configure ARP entry
     */
    dnx_utils_l3_arp_s_common_init(unit, 0, &arp_structure, 0, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION, cint_mpls_check_ver_info.next_hop_mac, cint_mpls_check_ver_info.out_rif);
    rv = dnx_utils_l3_arp_create(unit, &arp_structure);
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
 *   out_port - traffic outgoing port
*/
int
mpls_check_ver_fec_entry_configuration(
    int unit,
    int out_port)
{
    int rv;
    int tunnel_num = 0;
    dnx_utils_l3_fec_s fec_structure;

    /** step 1: multiple label tunnel.
     * for case 4-7
     */
    for (tunnel_num = 0; tunnel_num < 4; tunnel_num++) {
        dnx_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
        BCM_GPORT_LOCAL_SET(fec_structure.destination, out_port);
        fec_structure.tunnel_gport = cint_mpls_check_ver_info.multi_label_tunnel_id[tunnel_num];
        rv = dnx_utils_l3_fec_create(unit, &fec_structure);
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
    dnx_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    BCM_GPORT_LOCAL_SET(fec_structure.destination, out_port);
    fec_structure.tunnel_gport = cint_mpls_check_ver_info.common_mpls_tunnel_id;
    rv = dnx_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }
    cint_mpls_check_ver_info.common_tunnel_fec_id = fec_structure.l3eg_fec_id;

    /** step 3: common tunnel with ELI/EL.
     * for case 15
     */
    dnx_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    BCM_GPORT_LOCAL_SET(fec_structure.destination, out_port);
    fec_structure.tunnel_gport = cint_mpls_check_ver_info.eli_mpls_tunnel_id;
    rv = dnx_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }
    cint_mpls_check_ver_info.tunnel_with_eli_fec_id = fec_structure.l3eg_fec_id;

    /** step 4: RIF+ARP.
     * for case 1-4,8-9,11-12,14
     */
    dnx_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    BCM_GPORT_LOCAL_SET(fec_structure.destination, out_port);
    fec_structure.tunnel_gport = cint_mpls_check_ver_info.out_arp_id;
    rv = dnx_utils_l3_fec_create(unit, &fec_structure);
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
*/
int
mpls_check_ver_forward_entry_configuration(
    int unit)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;
    int index = 0;

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
    entry.egress_label.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    entry.egress_label.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    entry.flags |= BCM_MPLS_SWITCH_TTL_DECREMENT;

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
    int unit)
{
    bcm_mpls_tunnel_switch_t entry;
    int rv, i,label=0;

    /*
     * step 1: Create a stack of MPLS labels to be terminated 
     */
    for (i = 0; i < 5; i++)
    {
        bcm_mpls_tunnel_switch_t_init(&entry);

        entry.action = BCM_MPLS_SWITCH_ACTION_POP;
        entry.label = cint_mpls_check_ver_info.mpls_term_label[i];
        entry.vpn = cint_mpls_check_ver_info.vrf;

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
 *   in_port - traffic incoming port 
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

    rv = mpls_check_ver_arp_configuration(unit);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_check_ver_arp_configuration\n");
     return rv;
    }

    rv = mpls_check_ver_mpls_tunnel_configuration(unit);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_check_ver_mpls_tunnel_configuration\n");
     return rv;
    }

    rv = mpls_check_ver_fec_entry_configuration(unit,out_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_check_ver_fec_entry_configuration\n");
     return rv;
    }

    rv = mpls_check_ver_forward_entry_configuration(unit);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_check_ver_forward_entry_configuration\n");
     return rv;
    }

    rv = mpls_check_ver_mpls_termination_stack_configuration(unit);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_check_ver_mpls_termination_stack_configuration\n");
     return rv;
    }

    return rv;
}

