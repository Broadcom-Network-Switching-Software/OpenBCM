/*
 * Purpose: VPLS basic examples
 */

/*
 * Test Scenarios
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_multicast.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_vlan_translate.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../src/./examples/dnx/vpls/cint_vpls_basic.c
 * cint
 * int port1 = 200;
 * int port2 = 201;
 * vpls_basic_example(0,port1,port2,1,0);
 *
 *  Scenario configured in this cint:
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *   Basic VPLS Termination
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  - MPLS label is terminated with the PWE label
 *  - Lookup in MAC table and forwarded/flooded to access side
 *  - Source MAC learnt with PWE and FEC
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *   Basic VPLS encapsulation
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  - Switch into a PWE over MPLS core.
 *  - Exit with encapsulation of PWE and MPLS labels
 *
 * 1: pwe -> ac unknown
 * tx 1 psrc=203 data=0x000c0002000100000000cd1d8100001e884700d0504000d8014000110000011200010203040591000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x0011000001120001020304059100012c81000190ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Data: 0x00110000011200010203040591000064810000c8ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * 2: ac -> pwe unknown
 * tx 1 psrc=200 data=0x00110000011200050403020191000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x0011000001120005040302019100012c81000190ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 * Data: 0x00000000cd1d000c000200018100001e884700d050fe00d801fe00110000011200050403020191000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 * Data: 0x00000000cd1d000c000200018100001e88470115c0fe00d811fe00110000011200050403020191000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * 3: pwe -> ac known
 * tx 1 psrc=203 data=0x000c0002000100000000cd1d8100001e884700d0504000d8014000050403020100010203040591000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00050403020100010203040591000064810000c8ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * 4: ac -> pwe known
 * tx 1 psrc=200 data=0x00010203040500050403020191000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000000cd1d000c000200018100001e884700d050fe00d801fe00010203040500050403020191000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * vpls_with_mpls_ecmp(0,port1,port2,0);
 * 
 *  Scenario configured in this cint:
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *   Basic VPLS Termination
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  - MPLS label is terminated with the PWE label
 *  - Lookup in MAC table and forwarded/flooded to access side
 *  - Source MAC learnt with PWE and ECMP group
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *   Basic VPLS encapsulation
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  - Switch into a PWE over MPLS core.
 *  - Exit with encapsulation of PWE label and a MPLS label chosen by ECMP
 *
 * pwe -> ac unknown
 * tx 1 psrc=203 data=0x000c0002000100000000cd1d8100001e884700d0504000d8014000110000011200010203040591000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00110000011200010203040591000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * 2: ac -> pwe
 * tx 1 psrc=202 data=0x000102030405000504030201910000050800450000350000000080003340b8afcfd77fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x09000000cd1d000c000200018100001e884700d0e0fe00d801fe000102030405000504030201910000050800450000350000000080003340b8afcfd77fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 */

uint8 make_before_break = 0; /* configuring two mpls ports with same vc label */
bcm_mpls_port_control_channel_type_t vccv_type = 0;
int default_cpu_port = 0;
int has_cw = 0; /* control word */
int encap_access_pwe = -1;
int encap_access_mpls_tunnel = -1;

l2_port_properties_s pwe_port_2;
l2_port_properties_s ac_port_2;
mpls_port_add_s pwe_encap_2;
mpls_port_add_s pwe_term_2;
mpls_util_mpls_tunnel_initiator_create_s mpls_encap_tunnel_1[10];
mpls_util_arp_s arps[10];
mpls_util_fec_s fecs[10];

vpn = 5;

int station_id; /* For MPLS ignore mymac */

int
init_vpls_params(
    int unit,
    int extend_example)
{
    int rv = BCM_E_NONE;
    int estimate_encap_size_required = *dnxc_data_get(unit, "dev_init", "general", "egress_estimated_bta_btr_hw_config", NULL);
    int first_fec_in_hier;

    bcm_mac_t mac5 = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x02 };
    bcm_mac_t mac6 = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x2d };
    bcm_mac_t mac7 = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x10 };
    bcm_mac_t mac8 = { 0x00, 0x11, 0x00, 0x00, 0x01, 0x22 };

    /*
     * l2, mac, arp, fec parameters
     */
    pwe_port.intf = 30;
    pwe_port.mac_addr = mac1;
    pwe_port.next_hop = mac2;
    pwe_port.port = 200;
    pwe_port.arp = 9001;
    pwe_port.stat_id = 0;
    pwe_port.stat_pp_profile = 0;

    ac_port.intf = 5;
    ac_port.mac_addr = mac3;
    ac_port.next_hop = mac4;
    ac_port.port = 201;
    ac_port.vlan_port_id = 0;
    ac_port.stat_id = 0;
    ac_port.stat_pp_profile = 0;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &first_fec_in_hier);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    ac_port.encap_fec_id = first_fec_in_hier;

    /*
     * mpls encapsulation tunnel parameters
     */
    mpls_encap_tunnel_1[0].label[0] = 3333;
    mpls_encap_tunnel_1[0].num_labels = 1;
    mpls_encap_tunnel_1[0].encap_access = (encap_access_mpls_tunnel != -1) ? encap_access_mpls_tunnel : bcmEncapAccessTunnel2;
    mpls_encap_tunnel_1[0].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_encap_tunnel_1[0].tunnel_id = 16386;

    /*
     * mpls tunnel termination parameters
     */
    vpls_mpls_tunnel_switch_create_s_init(mpls_term_tunnel);
    mpls_term_tunnel[0].action = BCM_MPLS_SWITCH_ACTION_POP;
    mpls_term_tunnel[0].label = mpls_encap_tunnel_1[0].label[0];

    /*
     * PWE tunnel to be encapsulated parameters
     */
    mpls_port_add_s_init(&pwe_encap);
    pwe_encap.flags |= BCM_MPLS_PORT_EGRESS_TUNNEL;
    if (has_cw)
    {
        pwe_encap.flags |= BCM_MPLS_PORT_CONTROL_WORD;
    }
    pwe_encap.flags2 = BCM_MPLS_PORT2_EGRESS_ONLY;
    pwe_encap.label = 3456;
    pwe_encap.label_flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    pwe_encap.mpls_port_id = 0;
    pwe_encap.encap_access = (encap_access_pwe != -1) ? encap_access_pwe: bcmEncapAccessTunnel1;
    pwe_encap.encap_id = 0;
    pwe_encap.nwk_group_valid = 1;
    pwe_encap.pwe_nwk_group_id = 1;

    /** estimate_encap_size feature add restrictions on the global lif id allocation. */
    /** Therefore, if required, it is better to allocate without id, otherwise, use WITH_ID */
    if (!estimate_encap_size_required)
    {
        pwe_encap.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_ENCAP_WITH_ID;
        pwe_encap.mpls_port_id = 18888;
        pwe_encap.encap_id = 18888;
    }

    /*
     * PWE tunnel to be terminated parameters 
     */
    mpls_port_add_s_init(&pwe_term);
    pwe_term.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    pwe_term.encap_id = pwe_encap.encap_id;
    if (has_cw)
    {
        pwe_term.flags |= BCM_MPLS_PORT_CONTROL_WORD;
    }
    pwe_term.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
    pwe_term.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY;
    pwe_term.match_label = pwe_encap.label;
    pwe_term.mpls_port_id = 9999;
    pwe_term.nwk_group_valid = 1;
    pwe_term.pwe_nwk_group_id = 1;
    pwe_term.vccv_type = vccv_type;

    if (extend_example)
    {
        /*
         * l2, mac, arp, fec parameters
         */
        pwe_port_2.intf = 40;
        pwe_port_2.mac_addr = mac5;
        pwe_port_2.next_hop = mac6;
        pwe_port_2.arp = 9002;
        pwe_port_2.stat_id = 0;
        pwe_port_2.stat_pp_profile = 0;

        ac_port_2.intf = 6;
        ac_port_2.mac_addr = mac7;
        ac_port_2.next_hop = mac8;
        ac_port_2.vlan_port_id = 0;
        ac_port_2.stat_id = 0;
        ac_port_2.stat_pp_profile = 0;
    
        ac_port_2.encap_fec_id = first_fec_in_hier + 0x10;

        /*
         * MPLS tunnel parameters
         */
        mpls_encap_tunnel_1[1].label[0] = 4444;
        mpls_encap_tunnel_1[1].num_labels = 1;
        mpls_encap_tunnel_1[1].encap_access = (encap_access_mpls_tunnel != -1) ? encap_access_mpls_tunnel : bcmEncapAccessTunnel2;
        mpls_encap_tunnel_1[1].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        mpls_encap_tunnel_1[1].tunnel_id = 16388;

        /*
         * mpls tunnel termination parameters
         */
        mpls_term_tunnel[1].action = BCM_MPLS_SWITCH_ACTION_POP;
        mpls_term_tunnel[1].label = mpls_encap_tunnel_1[1].label[0];

        /*
         * PWE tunnel to be encapsulated parameters
         */
        mpls_port_add_s_init(&pwe_encap_2);
        pwe_encap_2.flags |= BCM_MPLS_PORT_EGRESS_TUNNEL;
        if (has_cw)
        {
            pwe_encap_2.flags |= BCM_MPLS_PORT_CONTROL_WORD;
        }
        pwe_encap_2.flags2 = BCM_MPLS_PORT2_EGRESS_ONLY;
        pwe_encap_2.label = 3457;
        pwe_encap_2.label_flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        pwe_encap_2.mpls_port_id = 0;
        pwe_encap_2.encap_access = (encap_access_pwe != -1) ? encap_access_pwe: bcmEncapAccessTunnel1;
        pwe_encap_2.encap_id = 0;
        pwe_encap_2.nwk_group_valid = 1;
        pwe_encap_2.pwe_nwk_group_id = 1;

        /** estimate_encap_size feature add restrictions on the global lif id allocation. */
        /** Therefore, if required, it is better to allocate without id, otherwise, use WITH_ID */
        if (!estimate_encap_size_required)
        {
            pwe_encap_2.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_ENCAP_WITH_ID;
            pwe_encap_2.mpls_port_id = 18889;
            pwe_encap_2.encap_id = 18889;
        }

        /*
         * PWE tunnel to be terminated parameters
         */
        mpls_port_add_s_init(&pwe_term_2);
        pwe_term_2.criteria = BCM_MPLS_PORT_MATCH_LABEL;
        pwe_term_2.encap_id = pwe_encap_2.encap_id;
        if (has_cw)
        {
            pwe_term_2.flags |= BCM_MPLS_PORT_CONTROL_WORD;
        }
        pwe_term_2.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
        pwe_term_2.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY;
        pwe_term_2.match_label = make_before_break ? pwe_term.match_label : pwe_encap_2.label;
        pwe_term_2.mpls_port_id = 10015;
        pwe_term_2.nwk_group_valid = 1;
        pwe_term_2.pwe_nwk_group_id = 1;
        pwe_term_2.vccv_type = vccv_type;
    }

    params_set = 1;

    return rv;
}

/*
 * Create L2 VPN (VSI and MC group)
 */
int
vpls_vswitch_create(
    int unit,
    int vpn)
{
    int rv = BCM_E_NONE;

    printf("Create VSI\n");
    rv = bcm_vswitch_create_with_id(unit, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_dnx_vswitch_create_with_id\n");
        return rv;
    }

    printf("Create MC group\n");
    rv = multicast__open_mc_group(unit, &vpn, 0);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in multicast__open_mc_group\n");
        return rv;
    }

    return rv;
}


int
vpls_add_access_port(
    int unit,
    l2_port_properties_s * port,
    bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;

    /*
     * add port, according to port_vlan_vlan
     */
    bcm_vlan_port_t_init(&vlan_port);

    /*
     * set port attribures, key <port-vlan>
     */
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = port->port;
    vlan_port.match_vlan = port->intf;
    vlan_port.vsi = 0;
    vlan_port.flags = 0;
    if (is_contain_wide_data)
    {
        vlan_port.flags |= BCM_VLAN_PORT_INGRESS_WIDE;
    }

    if (port->nwk_group_valid)
    {
        vlan_port.ingress_network_group_id = port->ac_nwk_group_id;
        vlan_port.egress_network_group_id = port->ac_nwk_group_id;
    }

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    port->vlan_port_id = vlan_port.vlan_port_id;
    port->encap_id = vlan_port.encap_id;

    if (vpn)
    {
        rv = bcm_vswitch_port_add(unit, vpn, vlan_port.vlan_port_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vswitch_port_add\n");
            return rv;
        }
    }

    /*
     * Add port to VLAN membership
     */
    rv = bcm_vlan_gport_add(unit, port->intf, port->port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_gport_add \n");
        return rv;
    }

    return rv;
}

/* Switch PWE1 from mpls tunnel (3333) to the other (4444) that was prevously configured for PWE2.
   This configures only the multicast traffic to go throught the new tunnel, unicast traffic is not effected. */
int
switch_pwe_tunnel(int unit) {
    int rv = BCM_E_NONE;
    bcm_mpls_port_t mpls_port;

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.mpls_port_id = pwe_encap.mpls_port_id;
    rv = bcm_mpls_port_get(unit, 0, &mpls_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_port_get\n");
        return rv;
    }

    mpls_port.flags |= BCM_MPLS_PORT_REPLACE | BCM_MPLS_PORT_WITH_ID;
    mpls_port.flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;
    mpls_port.egress_tunnel_if = mpls_encap_tunnel_1[1].tunnel_id;
    rv = bcm_mpls_port_add(unit, 0, &mpls_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_port_get\n");
        return rv;
    }

    return rv;
}

int
vpls_add_core_port(
    int unit,
    l2_port_properties_s * port)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;

    /*
     * add port, according to port_vlan_vlan
     */
    bcm_vlan_port_t_init(&vlan_port);

    /*
     * set port attribures, key <port-vlan>
     */
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = port->port;
    vlan_port.match_vlan = port->intf;
    vlan_port.vsi = port->intf;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    /*
     * Set Out-Port default properties 
     */
    rv = out_port_set(unit, port->port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set intf_out, port=%d\n", port->port);
        return rv;
    }

    /*
     * Add port to VLAN membership
     */
    rv = bcm_vlan_gport_add(unit, port->intf, port->port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_gport_add \n");
        return rv;
    }

    return rv;
}

/*
 * Basic VPLS scenario
 */
int
vpls_basic_example(
    int unit,
    int port1,
    int port2,
    int extend_example,
    int no_learning)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    proc_name = "vpls_basic_example";
    int ac_outer_vlan_1 = 100;
    int ac_inner_vlan_1 = 200;
    int ac_outer_vlan_2 = 300;
    int ac_inner_vlan_2 = 400;
    int flags = 0;

    /* Init parameters */
    init_vpls_params(unit,extend_example);

    if (has_cw) {
        int control_word = 0xffffffff;
        rv = bcm_switch_control_set (unit, bcmSwitchMplsPWControlWord, control_word);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in bcm_switch_control_set %d\n", proc_name);
            return rv;
        }
    }

    ac_port.port = port1;
    pwe_port.port = port2;

    /*
     * create VPN and multicast group
     */
    rv = vpls_vswitch_create(unit, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vpls_vswitch_create \n", proc_name);
        return rv;
    }

    pwe_term.forwarding_domain = vpn;

    /*
     * Configure AC and PWE ports
     */
    rv = vpls_add_access_port(unit, ac_port, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_add_access_port ac_port\n", proc_name);
        return rv;
    }

    rv = vpls_add_core_port(unit, pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_add_core_port pwe_port\n", proc_name);
        return rv;
    }

    /*
     * Configure L3 interfaces
     */
    rv = vpls_create_l3_interfaces(unit, &pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_l3_interfaces\n", proc_name);
        return rv;
    }

    /*
     * Configure an ARP entries
     */
    rv = vpls_create_arp_entry(unit, &pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_arp_entry\n", proc_name);
        return rv;
    }

    mpls_encap_tunnel_1[0].l3_intf_id = &pwe_port.arp;
    if (extend_example) {
        /* In this exmaple, use same ARP entry as MPLS tunnel 1 */
        mpls_encap_tunnel_1[1].l3_intf_id = &pwe_port.arp;
    }

    /*
     * Configure MPLS tunnels
     */
    rv = mpls_create_initiator_tunnels(unit, mpls_encap_tunnel_1, (extend_example ? 2:1));
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in mpls_create_initiator_tunnels\n", proc_name);
        return rv;
    }

    /*
     * Configure fec entry
     */
    if (*dnxc_data_get(unit, "headers", "system_headers", "system_headers_mode", NULL) == 0)
    {
        flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
    }
    rv = vpls_create_fec_entry(unit, ac_port.encap_fec_id, mpls_encap_tunnel_1[0].tunnel_id, pwe_port.port, flags);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_fec_entry\n", proc_name);
        return rv;
    }
    pwe_term.egress_tunnel_if = ac_port.encap_fec_id;

    /*
     * Configure a termination label for the ingress flow
     */
    rv = vpls_create_termination_stack(unit, mpls_term_tunnel);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_termination_stack\n", proc_name);
        return rv;
    }

    /*
     * configure PWE tunnel - egress flow, set next pointer to MPLS tunnel
     */
    pwe_encap.egress_tunnel_if = mpls_encap_tunnel_1[0].tunnel_id;
    rv = vpls_mpls_port_add_encapsulation(unit, &pwe_encap);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_mpls_port_add_encapsulation\n", proc_name);
        return rv;
    }
    pwe_term.encap_id = pwe_encap.encap_id;

    /*
     * configure PWE tunnel - ingress flow
     */
    rv = vpls_mpls_port_add_termination(unit, &pwe_term);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_mpls_port_add_termination\n", proc_name);
        return rv;
    }

    if (no_learning)
    {
        /* add l2 addresses to be defined as static - no learning needed */
        rv = vpls_l2_addr_add(unit, vpn, ac_port.mac_addr, pwe_term.mpls_port_id, ac_port.stat_id, ac_port.stat_pp_profile);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in vpls_l2_addr_add\n", proc_name);
            return rv;
        }

        rv = vpls_l2_addr_add(unit, vpn, ac_port.next_hop, ac_port.vlan_port_id, pwe_port.stat_id, pwe_port.stat_pp_profile);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in vpls_l2_addr_add\n", proc_name);
            return rv;
        }
    } else {
        /* Add AC and PWE to multicast group */
        rv = multicast__gport_encap_add(unit, vpn, pwe_port.port, pwe_encap.mpls_port_id, egress_mc);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error, in multicast__gport_encap_add for AC\n", proc_name);
            return rv;
        }

        rv = multicast__gport_encap_add(unit, vpn, ac_port.port, ac_port.vlan_port_id, egress_mc);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error, in multicast__gport_encap_add for AC\n", proc_name);
            return rv;
        }
    }

    /* Create EVE on egress AC */
    rv = vlan_translate_ive_eve_translation_set(unit, ac_port.vlan_port_id,         /* lif  */
                                                       0x9100,                     /* outer_tpid */
                                                       0x8100,                     /* inner_tpid */
                                                       bcmVlanActionReplace,           /* outer_action */
                                                       bcmVlanActionAdd,           /* inner_action */
                                                       ac_outer_vlan_1,            /* new_outer_vid */
                                                       ac_inner_vlan_1,            /* new_inner_vid */
                                                       5,                          /* vlan_edit_profile */
                                                       4,                          /* tag_format - here is stag */
                                                       FALSE                       /* is_ive */
                                                       );
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress vlan editing\n", proc_name);
        return rv;
    }

    if (extend_example) {

        ac_port_2.port = port1;
        pwe_port_2.port = port2;

        /*
         * Configure AC and PWE ports
         */
        rv = vpls_add_access_port(unit, ac_port_2, vpn);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in vpls_add_access_port ac_port_2\n", proc_name);
            return rv;
        }

        rv = vpls_add_core_port(unit, pwe_port_2);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in vpls_add_core_port pwe_port\n", proc_name);
            return rv;
        }

        /*
         * Configure L3 interfaces
         */
        rv = vpls_create_l3_interfaces(unit, &pwe_port_2);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in vpls_create_l3_interfaces\n", proc_name);
            return rv;
        }

        /*
         * Configure fec entry, use same MPLS tunnel PWE 1
         */
        rv = vpls_create_fec_entry(unit, ac_port_2.encap_fec_id, mpls_encap_tunnel_1[1].tunnel_id, pwe_port_2.port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in vpls_create_fec_entry\n", proc_name);
            return rv;
        }

        pwe_term_2.egress_tunnel_if = ac_port_2.encap_fec_id;

        /*
         * configure PWE tunnel - egress flow, set next pointer to MPLS tunnel
         */
        pwe_encap_2.egress_tunnel_if = mpls_encap_tunnel_1[1].tunnel_id;
        rv = vpls_mpls_port_add_encapsulation(unit, &pwe_encap_2);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in vpls_mpls_port_add_encapsulation\n", proc_name);
            return rv;
        }
        pwe_term_2.encap_id = pwe_encap_2.encap_id;

        /*
         * configure PWE tunnel - ingress flow 
         */
        pwe_term_2.forwarding_domain = vpn;
        rv = vpls_mpls_port_add_termination(unit, &pwe_term_2);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in vpls_mpls_port_add_termination\n", proc_name);
            return rv;
        }

        if (no_learning)
        {
            /* add l2 addresses to be defined as static - no learning needed */
            rv = vpls_l2_addr_add(unit, vpn, ac_port_2.mac_addr, pwe_term_2.mpls_port_id, ac_port_2.stat_id, ac_port_2.stat_pp_profile);
            if (rv != BCM_E_NONE)
            {
                printf("%s(): Error, in vpls_l2_addr_add\n", proc_name);
                return rv;
            }

            rv = vpls_l2_addr_add(unit, vpn, ac_port_2.next_hop, ac_port_2.vlan_port_id, pwe_port_2.stat_id, pwe_port_2.stat_pp_profile);
            if (rv != BCM_E_NONE)
            {
                printf("%s(): Error, in vpls_l2_addr_add\n", proc_name);
                return rv;
            }
        } else {
            /* Add AC and PWE to multicast group */
            rv = multicast__gport_encap_add(unit, vpn, pwe_port_2.port, pwe_encap_2.mpls_port_id, egress_mc);
            if (rv != BCM_E_NONE) {
                printf("%s(): Error, in multicast__gport_encap_add for PWE\n", proc_name);
                return rv;
            }

            rv = multicast__gport_encap_add(unit, vpn, ac_port_2.port, ac_port_2.vlan_port_id, egress_mc);
            if (rv != BCM_E_NONE) {
                printf("%s(): Error, in multicast__gport_encap_add for AC\n", proc_name);
                return rv;
            }
        }

        /* Create EVE on egress AC */
        rv = vlan_translate_ive_eve_translation_set(unit, ac_port_2.vlan_port_id,         /* lif  */
                                                           0x9100,                     /* outer_tpid */
                                                           0x8100,                     /* inner_tpid */
                                                           bcmVlanActionReplace,           /* outer_action */
                                                           bcmVlanActionAdd,           /* inner_action */
                                                           ac_outer_vlan_2,            /* new_outer_vid */
                                                           ac_inner_vlan_2,            /* new_inner_vid */
                                                           6,                          /* vlan_edit_profile */
                                                           4,                          /* tag_format - here is stag */
                                                           FALSE                       /* is_ive */
                                                           );
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, create egress vlan editing\n", proc_name);
            return rv;
        }
    }

    /* Configure TTL1 trap to control plane used for VCCV type 3 */
    if (vccv_type == bcmMplsPortControlChannelTtl) {
        int trap_code;
        bcm_rx_trap_t trap_type;
        trap_type = bcmRxTrapMplsTunnelTerminationTtl1;
        rv =  bcm_rx_trap_type_create(unit, 0/*flags*/, trap_type, &trap_code);
        if (rv != BCM_E_NONE) {
           printf("%s(): Error, in bcm_rx_trap_type_create\n", proc_name);
           return rv;
        }

        bcm_rx_trap_config_t trap_config;
        bcm_rx_trap_config_t_init(&trap_config);
        trap_config.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
        trap_config.trap_strength = 7;
        trap_config.dest_port = default_cpu_port;

        rv = bcm_rx_trap_set(unit, trap_code, trap_config);
        if (rv != BCM_E_NONE) {
           printf("%s(): Error, in bcm_rx_trap_set\n", proc_name);
           return rv;
        }
    }

    return rv;
}

/*
 * VPLS with ECMP pointing to multiple FECs(each to a different MPLS tunnel)
 */
int
vpls_with_mpls_ecmp(
    int unit,
    int port1,
    int port2,
    int no_learning)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    proc_name = "vpls_with_mpls_ecmp";
    int i;
    int fec_ids[10];
    int ecmp_group = 100;

    /* Init parameters */
    init_vpls_params(unit, 0);

    /* Init params for 10 ARP entries */
    mpls_util_arp_s_init(arps, 10);
    for (i=0; i < 10; i++)
    {
        arps[i].arp = 0;
        arps[i].next_hop = pwe_port.next_hop;
        arps[i].intf = &pwe_port.intf;
        pwe_port.next_hop[0]++;
    }

    /* Init params for 10 MPLS tunnels */
    for (i=0; i < 10; i++)
    {
        mpls_encap_tunnel_1[i].label[0] = 3333+i;
        mpls_encap_tunnel_1[i].num_labels = 1;
        mpls_encap_tunnel_1[i].encap_access = bcmEncapAccessTunnel2;
        mpls_encap_tunnel_1[i].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        mpls_encap_tunnel_1[i].l3_intf_id = &arps[i].arp;
    }

    /* Init params for 10 FEC entries */
    mpls_util_fec_s_init(fecs, 10);
    for (i=0; i < 10; i++)
    {
        fecs[i].fec_id = 0;
        fecs[i].port = &pwe_port.port;
        fecs[i].tunnel_gport = &mpls_encap_tunnel_1[i].tunnel_id;
        fecs[i].tunnel_gport2 = &MPLS_UTIL_INVALID_GPORT;
    }

    ac_port.port = port1;
    pwe_port.port = port2;

    /*
     * create VPN and multicast group
     */
    rv = vpls_vswitch_create(unit, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vpls_vswitch_create \n", proc_name);
        return rv;
    }

    pwe_term.forwarding_domain = vpn;

    /*
     * Configure AC and PWE ports
     */
    rv = vpls_add_access_port(unit, ac_port, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_add_access_port ac_port\n", proc_name);
        return rv;
    }

    rv = vpls_add_core_port(unit, pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_add_core_port pwe_port\n", proc_name);
        return rv;
    }

    /*
     * Configure L3 interfaces
     */
    rv = vpls_create_l3_interfaces(unit, &pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_l3_interfaces\n", proc_name);
        return rv;
    }

    /*
     * Configure an ARP entries
     */
    rv = mpls_create_arp_entries(unit, arps, 10);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in mpls_create_arp_entries\n", proc_name);
        return rv;
    }

    /*
     * Configure MPLS tunnels
     */
    rv = mpls_create_initiator_tunnels(unit, mpls_encap_tunnel_1, 10);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in mpls_create_initiator_tunnels\n", proc_name);
        return rv;
    }

    /*
     * Configure 10 fec entries
     */
    rv = mpls_create_fec_entries(unit, fecs, 10);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in mpls_create_fec_entries\n", proc_name);
        return rv;
    }

    /* Save FEC ids */
    for (i=0; i < 10; i++)
    {
        fec_ids[i] = fecs[i].fec_id;
    }

    /* create an ECMP, containing the FEC entries */
    bcm_l3_egress_ecmp_t ecmp;
    ecmp.flags = BCM_L3_WITH_ID;
    ecmp.ecmp_intf = ecmp_group;
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DISABLED;
    rv = bcm_l3_egress_ecmp_create(unit, ecmp, 10, fec_ids);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, bcm_l3_egress_ecmp_create failed\n", proc_name);
        return rv;
    }

    /* Set PWE learnt egress if to ECMP */
    pwe_term.egress_tunnel_if = ecmp_group;

    /*
     * Configure a termination label for the ingress flow
     */
    rv = vpls_create_termination_stack(unit, mpls_term_tunnel);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_termination_stack\n", proc_name);
        return rv;
    }

    /*
     * configure PWE tunnel - egress flow
     */
    pwe_encap.egress_tunnel_if = mpls_encap_tunnel_1[0].tunnel_id;
    rv = vpls_mpls_port_add_encapsulation(unit, &pwe_encap);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_mpls_port_add_encapsulation\n", proc_name);
        return rv;
    }
    pwe_term.encap_id = pwe_encap.encap_id;

    /*
     * configure PWE tunnel - ingress flow
     */
    rv = vpls_mpls_port_add_termination(unit, &pwe_term);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_mpls_port_add_termination\n", proc_name);
        return rv;
    }

    if (no_learning)
    {
        /* add l2 addresses to be defined as static - no learning needed */
        rv = vpls_l2_addr_add(unit, vpn, ac_port.mac_addr, pwe_term.mpls_port_id, ac_port.stat_id, ac_port.stat_pp_profile);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in vpls_l2_addr_add\n", proc_name);
            return rv;
        }

        rv = vpls_l2_addr_add(unit, vpn, ac_port.next_hop, ac_port.vlan_port_id, pwe_port.stat_id, pwe_port.stat_pp_profile);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in vpls_l2_addr_add\n", proc_name);
            return rv;
        }
    } else {
        /* Add AC and PWE to multicast group */
        rv = multicast__gport_encap_add(unit, vpn, pwe_port.port, pwe_encap.mpls_port_id, egress_mc);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error, in multicast__gport_encap_add for AC\n", proc_name);
            return rv;
        }

        rv = multicast__gport_encap_add(unit, vpn, ac_port.port, ac_port.vlan_port_id, egress_mc);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error, in multicast__gport_encap_add for AC\n", proc_name);
            return rv;
        }
    }

    return rv;
}

int
init_vpws_multi_device_params(
    int tx_unit,
    int rx_unit)
{
    int rv = BCM_E_NONE;
    int estimate_encap_size_required = *dnxc_data_get(rx_unit, "dev_init", "general", "egress_estimated_bta_btr_hw_config", NULL);
    int first_fec_in_hier;

    /*
     * l2, mac, arp, fec parameters
     */
    pwe_port.intf = 30;
    pwe_port.mac_addr = mac1;
    pwe_port.next_hop = mac2;
    pwe_port.port = 200;
    pwe_port.arp = 9001;
    pwe_port.stat_id = 0;
    pwe_port.stat_pp_profile = 0;

    ac_port.intf = 5;
    ac_port.mac_addr = mac3;
    ac_port.next_hop = mac4;
    ac_port.port = 201;
    ac_port.vlan_port_id = 0;
    ac_port.stat_id = 0;
    ac_port.stat_pp_profile = 0;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(tx_unit, 0, 0, &first_fec_in_hier);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    ac_port.encap_fec_id = first_fec_in_hier;

    /*
     * mpls encapsulation tunnel parameters
     */
    mpls_encap_tunnel_1[0].label[0] = 3333;
    mpls_encap_tunnel_1[0].num_labels = 1;
    mpls_encap_tunnel_1[0].encap_access = bcmEncapAccessTunnel2;
    mpls_encap_tunnel_1[0].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_encap_tunnel_1[0].tunnel_id = 16386;

    /*
     * mpls tunnel termination parameters
     */
    vpls_mpls_tunnel_switch_create_s_init(mpls_term_tunnel);
    mpls_term_tunnel[0].action = BCM_MPLS_SWITCH_ACTION_POP;
    mpls_term_tunnel[0].label = mpls_encap_tunnel_1[0].label[0];

    /*
     * PWE tunnel to be encapsulated parameters
     */
    mpls_port_add_s_init(&pwe_encap);
    pwe_encap.flags |= BCM_MPLS_PORT_EGRESS_TUNNEL;
    if (has_cw)
    {
        pwe_encap.flags |= BCM_MPLS_PORT_CONTROL_WORD;
    }
    pwe_encap.flags2 = BCM_MPLS_PORT2_EGRESS_ONLY;
    pwe_encap.label = 3456;
    pwe_encap.label_flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    pwe_encap.mpls_port_id = 0;
    pwe_encap.encap_access = bcmEncapAccessTunnel1;
    pwe_encap.encap_id = 0;
    pwe_encap.nwk_group_valid = 1;
    pwe_encap.pwe_nwk_group_id = 1;

    /** estimate_encap_size feature add restrictions on the global lif id allocation. */
    /** Therefore, if required, it is better to allocate without id, otherwise, use WITH_ID */
    if (!estimate_encap_size_required)
    {
        pwe_encap.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_ENCAP_WITH_ID;
        pwe_encap.mpls_port_id = 18888;
        pwe_encap.encap_id = 18888;
    }

    /*
     * PWE tunnel to be terminated parameters 
     */
    mpls_port_add_s_init(&pwe_term);
    pwe_term.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    pwe_term.encap_id = pwe_encap.encap_id;
    pwe_term.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
    if (has_cw)
    {
        pwe_term.flags |= BCM_MPLS_PORT_CONTROL_WORD;
    }
    pwe_term.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY | BCM_MPLS_PORT2_CROSS_CONNECT;
    pwe_term.match_label = pwe_encap.label;
    pwe_term.mpls_port_id = 9999;
    pwe_term.nwk_group_valid = 1;
    pwe_term.pwe_nwk_group_id = 1;
    pwe_term.forwarding_domain = 0; /* P2P */

    params_set = 1;

    return rv;
}

int
vpws_multi_device_main(
    int tx_unit,
    int rx_unit,
    int acP,
    int pweP)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    proc_name = "vpws_multi_device_main";

    /* Init parameters */
    init_vpws_multi_device_params(tx_unit, rx_unit);

    ac_port.port = acP;
    pwe_port.port = pweP;

    if (has_cw) {
        int control_word = 0xffffffff;
        rv = bcm_switch_control_set (rx_unit, bcmSwitchMplsPWControlWord, control_word);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in bcm_switch_control_set %d\n", proc_name);
            return rv;
        }
    }

    /*
     * create VPN and multicast group
     */
    rv = vpls_vswitch_create(tx_unit, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vpls_vswitch_create \n", proc_name);
        return rv;
    }

    /*
     * Configure AC and PWE ports
     */
    rv = vpls_add_access_port(tx_unit, ac_port, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_add_access_port ac_port\n", proc_name);
        return rv;
    }

    rv = vpls_add_core_port(rx_unit, pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_add_core_port pwe_port\n", proc_name);
        return rv;
    }

    /*
     * Configure L3 interfaces
     */
    rv = vpls_create_l3_interfaces(rx_unit, &pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_l3_interfaces\n", proc_name);
        return rv;
    }

    /*
     * Configure an ARP entries
     */
    rv = vpls_create_arp_entry(rx_unit, &pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_arp_entry\n", proc_name);
        return rv;
    }

    mpls_encap_tunnel_1[0].l3_intf_id = &pwe_port.arp;

    /*
     * Configure MPLS tunnels
     */
    rv = mpls_create_initiator_tunnels(rx_unit, mpls_encap_tunnel_1, 1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in mpls_create_initiator_tunnels\n", proc_name);
        return rv;
    }

    /*
     * Configure fec entry
     */
    rv = vpls_create_fec_entry(tx_unit, ac_port.encap_fec_id, mpls_encap_tunnel_1[0].tunnel_id, pwe_port.port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_fec_entry\n", proc_name);
        return rv;
    }
    pwe_term.egress_tunnel_if = ac_port.encap_fec_id;

    /*
     * Configure a termination label for the ingress flow
     */
    rv = vpls_create_termination_stack(rx_unit, mpls_term_tunnel);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_termination_stack\n", proc_name);
        return rv;
    }

    /*
     * configure PWE tunnel - egress flow, set next pointer to MPLS tunnel
     */
    pwe_encap.egress_tunnel_if = mpls_encap_tunnel_1[0].tunnel_id;
    rv = vpls_mpls_port_add_encapsulation(rx_unit, &pwe_encap);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_mpls_port_add_encapsulation\n", proc_name);
        return rv;
    }
    pwe_term.encap_id = pwe_encap.encap_id;

    /*
     * configure PWE tunnel - ingress flow
     */
    rv = vpls_mpls_port_add_termination(rx_unit, &pwe_term);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_mpls_port_add_termination\n", proc_name);
        return rv;
    }

    /*
     * Cross connect AC to pwe
     */
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init (gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = ac_port.vlan_port_id;
    BCM_GPORT_FORWARD_PORT_SET(gports.port2, ac_port.encap_fec_id);
    gports.encap2 = pwe_encap.encap_id;
    printf("bcm_vswitch_cross_connect_add( gport1 = %d, gport2 = %d, encap1 = %d, encap2 = %d )\n", gports.port1,
           gports.port2, gports.encap1, gports.encap2);

    rv = bcm_vswitch_cross_connect_add(tx_unit, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_vswitch_cross_connect_add", proc_name);
        return rv;
    }

    /*
     * Cross connect pwe to AC
     */
    bcm_vswitch_cross_connect_t_init (gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = pwe_term.mpls_port_id;
    gports.port2 = acP;
    gports.encap2 = ac_port.encap_id;
    printf("bcm_vswitch_cross_connect_add( gport1 = %d, gport2 = %d, encap1 = %d, encap2 = %d )\n", gports.port1,
           gports.port2, gports.encap1, gports.encap2);

    rv = bcm_vswitch_cross_connect_add(rx_unit, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_vswitch_cross_connect_add", proc_name);
        return rv;
    }

    return rv;
}

/**
 * Port Ingore Mymac enabel/disable for MPLS
 * INPUt:
 * unit   - device unit
 * enable - enable/disable
*/
int port_based_ingore_mymac_mpls(int unit,int port, int enable)
{
    int rv;

    if (enable)
    {
        rv = bcm_switch_l3_protocol_group_set(unit,BCM_SWITCH_L3_PROTOCOL_GROUP_MPLS,2);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_switch_l3_protocol_group_set\n");
            return rv;
        }

        rv = bcm_switch_control_set(unit,bcmSwitchL2StationExtendedMode,2);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_switch_control_set\n");
            return rv;
        }

        bcm_l2_station_t station;

        bcm_l2_station_t_init(&station);
        station.src_port=port;
        station.src_port_mask=0xffff;
        station.flags = BCM_L2_STATION_MPLS|BCM_L2_STATION_EXTENDED;
        rv = bcm_l2_station_add(unit,&station_id,&station);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_l2_station_add\n");
            return rv;
        }
    }
    else
    {
        rv = bcm_l2_station_delete(unit, station_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_l2_station_delete\n");
            return rv;
        }

        rv = bcm_switch_control_set(unit,bcmSwitchL2StationExtendedMode,0);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_switch_control_set\n");
            return rv;
        }

        rv = bcm_switch_l3_protocol_group_set(unit,0,2);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_switch_l3_protocol_group_set\n");
            return rv;
        }
    }

    return rv;
}
