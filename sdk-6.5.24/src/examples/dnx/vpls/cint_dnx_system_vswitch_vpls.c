/*
 * $Id: cint_dnx_system_vswitch_vpls.c
 *
 * System resource example:
 * In the following CINT we will try to provide an example of System resources in local mode.
 *    The focus of the CINT is on PWE-LIFs (bcm_mpls_port_t object in BCM API).
 * Similiar principles are apply also to other LIF types such as bcm_vlan_port_t
 * (see cint_dnx_system_vswitch.c).
 * Each PWE-LIF allocates two objects: An Ingress LIF entry and an Egress LIF entry.
 * In case of 1:1 protection also a L2 FEC object that stores
 * the protection pairs is also being created.
 *
 * The following CINT provides some basic examples of PWE-LIf VPLS vswitch allocation schemes when
 *  working in local mode.
 *  1. MPLS P2P unprotected: see system_mpls_port_p2p_example
 *  2. MPLS MP unprotection: see system_mpls_port_mp_example
 *  3. MPLS P2P protected: see system_protected_mpls_port_p2p_example
 *  4. MPLS MP protected: see system_protected_mpls_port_mp_example
 *
 *
 * Test Scenario
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
 * cint ../../src/./examples/dnx/vpls/cint_dnx_system_vswitch_vpls.c
 * cint
 * system_mpls_port_mp_example(1,1811939529,1811939531,2,1811940552);
 * exit;
 *
 * MP system managment, configured mac: Send from unit1, port1 to unit2, port2
 *   tx 1 psrc=201 data=0x000c0002000100000000cd1d9100000a884700064040000c9140000000000a1100010203040591000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 2 should be:
 *   Data: 0x00000000cd1d000c000200018100000a88470006503f000ca13f000000000a1100010203040591000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * MP system managment, expect flood: Send from unit2, port2
 *   tx 1 psrc=200 data=0x000c0002000100000000cd1d9100000a884700065040000ca14000000000112200010203040591000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 1 should be:
 *   Data: 0x00000000112200010203040591000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * MP system managment, learned SA: Send from unit1, second_port1 to unit2, port2
 *   tx 1 psrc=203 data=0x0001020304050005040302019100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20

 * Received packets on unit 2 should be:
 *   Data: 0x00000000cd1d000c000200018100000a8847000650fe000ca1fe0001020304050005040302019100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * MP system managment, learned SA: Send from unit2, port2 to unit1, second_port1
 *   tx 1 psrc=200 data=0x000c0002000100000000cd1d9100000a884700065040000ca14000050403020100010203040591000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 1 should be:
 *   Data: 0x00050403020100010203040591000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20

 * MP system managment, configured mac: Send from unit1, second_port1 to unit1, port1
 *   tx 1 psrc=203 data=0x000000000a220005040302019100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 1 should be:
 *   Data: 0x00000000cd1d000c000200018100000a8847000640fe000c91fe000000000a220005040302019100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000

 * MP system managment, configured mac: Send from unit1, port1 to unit1, second_port1
 *   tx 1 psrc=201 data=0x000c0002000100000000cd1d9100000a884700064040000c9140000000000a3300010203040591000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 1 should be:
 *   Data: 0x000000000a3300010203040591000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20

 * MP system managment, learnt mac: Send from unit2, port2 to unit1, port1
 *   tx 1 psrc=200 data=0x000c00020001000080d260f09100000a884700065040000ca140000102030405000080d260f091000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 */

l2_port_properties_s ac__port;
l2_port_properties_s pwe__port_1;
mpls_tunnel_switch_create_s mpls__term_1[MAX_NOF_TUNNELS];
mpls_tunnel_initiator_create_s mpls__encap_1[MAX_NOF_TUNNELS];
mpls_port_add_s pwe__encap_1;
mpls_port_add_s pwe__term_1;

l2_port_properties_s pwe__port_2;
mpls_tunnel_switch_create_s mpls__term_2[MAX_NOF_TUNNELS];
mpls_tunnel_initiator_create_s mpls__encap_2[MAX_NOF_TUNNELS];
mpls_port_add_s pwe__encap_2;
mpls_port_add_s pwe__term_2;

bcm_failover_t failover_id = 0;

/*
 * Convert system port to local port
*/
int
multi_dev_system_port_to_local(
    int unit,
    int sysport,
    int *local_port)
{
    int rv = BCM_E_NONE;

    rv = bcm_port_local_get(unit, sysport, local_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_dnx_port_local_get fail core_port\n");
        return rv;
    }

    return rv;
}

int
init_vpls_params(
    int unit1,
    int unit2)
{
    int rv = BCM_E_NONE;

    vpn = 6000;


    /* UNIT 1 */

    /*
     * PWE and AC parameters
     */
    pwe__port_1.intf = 10;
    pwe__port_1.mac_addr = mac1;
    pwe__port_1.next_hop = mac2;
    pwe__port_1.arp = 9001;

    ac__port.intf = 30;
    ac__port.mac_addr = mac3;
    ac__port.next_hop = mac4;
    ac__port.vlan_port_id = 0;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit1, 0, 0, &pwe__port_1.encap_fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    /*
     * mpls encapsulation tunnel parameters
     */
    vpls_mpls_tunnel_initiator_create_s_init(mpls__encap_1);
    mpls__encap_1[0].label[0] = 100;
    mpls__encap_1[0].num_labels = 1;
    mpls__encap_1[0].encap_access = bcmEncapAccessTunnel2;
    mpls__encap_1[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

    /*
     * mpls tunnel termination parameters
     */
    vpls_mpls_tunnel_switch_create_s_init(mpls__term_1);
    mpls__term_1[0].action = BCM_MPLS_SWITCH_ACTION_POP;
    mpls__term_1[0].label = mpls__encap_1[0].label[0];

    /*
     * PWE tunnel to be encapsulated parameters
     */
    mpls_port_add_s_init(&pwe__encap_1);
    pwe__encap_1.flags |= BCM_MPLS_PORT_EGRESS_TUNNEL;
    pwe__encap_1.flags2 = BCM_MPLS_PORT2_EGRESS_ONLY | BCM_MPLS_PORT2_ALLOC_SYMMETRIC;
    pwe__encap_1.label = 201;
    pwe__encap_1.label_flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    pwe__encap_1.mpls_port_id = 0;
    pwe__encap_1.encap_access = bcmEncapAccessTunnel1;
    pwe__encap_1.encap_id = 0;
    pwe__encap_1.nwk_group_valid = 1; /* split horizon */
    pwe__encap_1.pwe_nwk_group_id = 1;

    /*
     * PWE tunnel to be terminated parameters 
     */
    mpls_port_add_s_init(&pwe__term_1);
    pwe__term_1.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    pwe__term_1.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
    pwe__term_1.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY;
    pwe__term_1.match_label = pwe__encap_1.label;
    pwe__term_1.forwarding_domain = vpn;
    pwe__term_1.nwk_group_valid = 1; /* split horizon */
    pwe__term_1.pwe_nwk_group_id = 1;

    /* UNIT 2 */

    /*
     * PWE parameters
     */
    pwe__port_2.intf = 10;
    pwe__port_2.mac_addr = mac1;
    pwe__port_2.next_hop = mac2;
    pwe__port_2.arp = 9001;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit2, 0, 0, &pwe__port_2.encap_fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    pwe__port_2.encap_fec_id = pwe__port_2.encap_fec_id + 10;

    /*
     * mpls encapsulation tunnel parameters
     */
    vpls_mpls_tunnel_initiator_create_s_init(mpls__encap_2);
    mpls__encap_2[0].label[0] = 101;
    mpls__encap_2[0].num_labels = 1;
    mpls__encap_2[0].encap_access = bcmEncapAccessTunnel2;
    mpls__encap_2[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

    /*
     * mpls tunnel termination parameters
     */
    vpls_mpls_tunnel_switch_create_s_init(mpls__term_2);
    mpls__term_2[0].action = BCM_MPLS_SWITCH_ACTION_POP;
    mpls__term_2[0].label = mpls__encap_2[0].label[0];

    /*
     * PWE tunnel to be encapsulated parameters
     */
    mpls_port_add_s_init(&pwe__encap_2);
    pwe__encap_2.flags |= BCM_MPLS_PORT_EGRESS_TUNNEL;
    pwe__encap_2.flags2 = BCM_MPLS_PORT2_EGRESS_ONLY | BCM_MPLS_PORT2_ALLOC_SYMMETRIC;
    pwe__encap_2.label = 202;
    pwe__encap_2.label_flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    pwe__encap_2.mpls_port_id = 0;
    pwe__encap_2.encap_access = bcmEncapAccessTunnel1;
    pwe__encap_2.encap_id = 0;

    /*
     * PWE tunnel to be terminated parameters 
     */
    mpls_port_add_s_init(&pwe__term_2);
    pwe__term_2.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    pwe__term_2.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
    pwe__term_2.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY;
    pwe__term_2.match_label = pwe__encap_2.label;
    pwe__term_2.forwarding_domain = vpn;
    pwe__term_2.nwk_group_valid = 1; /* split horizon */
    pwe__term_2.pwe_nwk_group_id = 1;

    params_set = 1;

    return rv;
}

/*
 * Scheme is:
 * unit1 -> MPLS_PORT1 pwe1 (gport1) and VLAN_PORT2 ac1 (second_gport1)
 * uint2 -> MPLS_PORT2 pwe2 (gport2)
 */
int system_mpls_port_mp_example(
    int unit1,
    int port1,
    int second_port1,
    int unit2,
    int port2)
{
    int rv = BCM_E_NONE;
    bcm_mac_t pwe_mac1  = {0x00, 0x00, 0x00, 0x00, 0x0a, 0x11};
    bcm_mac_t pwe_mac2  = {0x00, 0x00, 0x00, 0x00, 0x0a, 0x22};
    bcm_mac_t ac_mac  = {0x00, 0x00, 0x00, 0x00, 0x0a, 0x33};

    char *proc_name;
    proc_name = "system_mpls_port_mp_example";

    /* Init parameters */
    init_vpls_params(unit1,unit2);

    multi_dev_system_port_to_local(unit1,port1,&pwe__port_1.port);
    multi_dev_system_port_to_local(unit1,second_port1,&ac__port.port);
    multi_dev_system_port_to_local(unit2,port2,&pwe__port_2.port);

    /*
     * create VPN and multicast group
     */
    rv = vpls_vswitch_create(unit1, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vpls_vswitch_create \n", proc_name);
        return rv;
    }

    rv = vpls_vswitch_create(unit2, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vpls_vswitch_create \n", proc_name);
        return rv;
    }

    /*
     * Configure AC and PWE ports
     */
    /* AC port on unit1 */
    rv = vpls_configure_port_properties(unit1, ac__port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_configure_port_properties ac__port\n", proc_name);
        return rv;
    }

    /* PWE port on unit1 */
    rv = vpls_configure_port_properties(unit1, pwe__port_1, 1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_configure_port_properties pwe_port\n", proc_name);
        return rv;
    }

    /* PWE port on unit2 */
    rv = vpls_configure_port_properties(unit2, pwe__port_2, 1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_configure_port_properties pwe_port\n", proc_name);
        return rv;
    }

    /*
     * Configure L3 interfaces
     */
    rv = vpls_create_l3_interfaces(unit1, &pwe__port_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_l3_interfaces\n", proc_name);
        return rv;
    }

    rv = vpls_create_l3_interfaces(unit2, &pwe__port_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_l3_interfaces\n", proc_name);
        return rv;
    }

    /*
     * Configure an ARP entries
     */
    rv = vpls_create_arp_entry(unit1, &pwe__port_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_arp_entry\n", proc_name);
        return rv;
    }
    mpls__encap_1[0].l3_intf_id = pwe__port_1.arp;

    rv = vpls_create_arp_entry(unit2, &pwe__port_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_arp_entry\n", proc_name);
        return rv;
    }
    mpls__encap_2[0].l3_intf_id = pwe__port_2.arp;

    /*
     * Configure a push entry
     */
    rv = vpls_create_mpls_tunnel(unit1, mpls__encap_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_mpls_tunnel\n", proc_name);
        return rv;
    }
    pwe__encap_1.egress_tunnel_if = mpls__encap_1[0].tunnel_id;

    rv = vpls_create_mpls_tunnel(unit2, mpls__encap_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_mpls_tunnel\n", proc_name);
        return rv;
    }
    pwe__encap_2.egress_tunnel_if = mpls__encap_2[0].tunnel_id;

    /*
     * Configure fec entry
     */
    /* unit 1 */
    rv = vpls_create_fec_entry(unit1, pwe__port_1.encap_fec_id, mpls__encap_1[0].tunnel_id, pwe__port_1.port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_fec_entry\n", proc_name);
        return rv;
    }
    pwe__term_1.egress_tunnel_if = pwe__port_1.encap_fec_id;

    rv = vpls_create_fec_entry(unit1, pwe__port_2.encap_fec_id, mpls__encap_2[0].tunnel_id, port2, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_fec_entry\n", proc_name);
        return rv;
    }

    /* unit 2 */
    rv = vpls_create_fec_entry(unit2, pwe__port_2.encap_fec_id, mpls__encap_2[0].tunnel_id, pwe__port_2.port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_fec_entry\n", proc_name);
        return rv;
    }
    pwe__term_2.egress_tunnel_if = pwe__port_2.encap_fec_id;

    rv = vpls_create_fec_entry(unit2, pwe__port_1.encap_fec_id, mpls__encap_1[0].tunnel_id, port1, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_fec_entry\n", proc_name);
        return rv;
    }

    /*
     * Configure a termination label for the ingress flow
     */
    rv = vpls_create_termination_stack(unit1, mpls__term_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_termination_stack\n", proc_name);
        return rv;
    }

    rv = vpls_create_termination_stack(unit2, mpls__term_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_termination_stack\n", proc_name);
        return rv;
    }

    /*
     * configure PWE tunnel - egress flow
     */
    rv = vpls_mpls_port_add_encapsulation(unit1, &pwe__encap_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_mpls_port_add_encapsulation\n", proc_name);
        return rv;
    }
    pwe__term_1.encap_id = pwe__encap_1.encap_id;
    pwe__term_1.mpls_port_id = pwe__encap_1.encap_id;

    rv = vpls_mpls_port_add_encapsulation(unit2, &pwe__encap_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_mpls_port_add_encapsulation\n", proc_name);
        return rv;
    }
    pwe__term_2.encap_id = pwe__encap_2.encap_id;
    pwe__term_2.mpls_port_id = pwe__encap_2.encap_id;

    /*
     * configure PWE tunnel - ingress flow
     */
    rv = vpls_mpls_port_add_termination(unit1, &pwe__term_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_mpls_port_add_termination\n", proc_name);
        return rv;
    }

    rv = vpls_mpls_port_add_termination(unit2, &pwe__term_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_mpls_port_add_termination\n", proc_name);
        return rv;
    }

    /* Configure ac port */
    rv = vpls_add_access_port(unit1, &ac__port, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_add_access_port ac_port\n", proc_name);
        return rv;
    }

    /*
     * Add l2 addresses to be defined as static
     */
    /* MACT entry to AC, on both units */
    bcm_l2_addr_t l2addr;
    bcm_l2_addr_t_init(&l2addr, ac_mac, vpn);
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = second_port1;/* system port */
    l2addr.encap_id = ac__port.encap_id;

    rv = bcm_l2_addr_add(unit1, l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in bcm_l2_addr_add\n", proc_name);
        return rv;
    }

    rv = bcm_l2_addr_add(unit2, l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in bcm_l2_addr_add\n", proc_name);
        return rv;
    }

    /* MACT entry to unit1's pwe on unit1 */
    rv = vpls_l2_addr_add(unit1, vpn, pwe_mac2, pwe__term_1.mpls_port_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_l2_addr_add\n", proc_name);
        return rv;
    }

    /* MACT entry to unit2's pwe on unit1 */
    bcm_l2_addr_t_init(&l2addr, pwe_mac1, vpn);
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = port2; /* system port */
    l2addr.encap_id = pwe__encap_2.encap_id;

    rv = bcm_l2_addr_add(unit1, l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in bcm_l2_addr_add\n", proc_name);
        return rv;
    }

    /* Add AC and PWE to multicast group */
    rv = multicast__add_multicast_entry(unit1, vpn, &port1, &pwe__encap_1.encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, in multicast__add_multicast_entry for PWE 1\n", proc_name);
        return rv;
    }

    rv = multicast__add_multicast_entry(unit1, vpn, &port2, &pwe__encap_2.encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, in multicast__add_multicast_entry for PWE 2\n", proc_name);
        return rv;
    }

    rv = multicast__add_multicast_entry(unit1, vpn, &second_port1, &ac__port.encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, in multicast__add_multicast_entry for AC\n", proc_name);
        return rv;
    }

    rv = multicast__add_multicast_entry(unit2, vpn, &port1, &pwe__encap_1.encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, in multicast__add_multicast_entry for PWE 1\n", proc_name);
        return rv;
    }

    rv = multicast__add_multicast_entry(unit2, vpn, &port2, &pwe__encap_2.encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, in multicast__add_multicast_entry for PWE 2\n", proc_name);
        return rv;
    }

    rv = multicast__add_multicast_entry(unit2, vpn, &second_port1, &ac__port.encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, in multicast__add_multicast_entry for AC\n", proc_name);
        return rv;
    }

    /* Split-Horizon need to be taken from ETPS TOS , need to configure native ac as well  */
    bcm_vlan_port_t vp;
    int nwk_grp_from_esem;
    nwk_grp_from_esem = *dnxc_data_get(unit1, "esem", "feature", "etps_properties_assignment", NULL);

    if (nwk_grp_from_esem)
    {
        bcm_vlan_port_t_init(&vp);
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vp.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
        vp.egress_network_group_id = 1;
        vp.port = pwe__encap_1.mpls_port_id;
        vp.vsi = vpn;
        rv = bcm_vlan_port_create(unit1, &vp);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): ERROR: bcm_vlan_port_create default_native_ac\n", proc_name);
            return rv;
        }
    }

    nwk_grp_from_esem = *dnxc_data_get(unit2, "esem", "feature", "etps_properties_assignment", NULL);

    if (nwk_grp_from_esem)
    {
        bcm_vlan_port_t_init(&vp);
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vp.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
        vp.egress_network_group_id = 0; /* don't set split horizon for unit1 PWE to unit2 PWE */
        vp.port = pwe__encap_2.mpls_port_id;
        vp.vsi = vpn;
        rv = bcm_vlan_port_create(unit2, &vp);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): ERROR: bcm_vlan_port_create default_native_ac\n", proc_name);
            return rv;
        }
    }

    return rv;
}

/*
 * P2P connection VLAN-Port unit1 <-> MPLS-Port unit2
 */
int system_mpls_port_p2p_example(
    int unit1,
    int port1,
    int unit2,
    int port2)
{
    int rv = BCM_E_NONE;
    bcm_mac_t pwe_mac1  = {0x00, 0x00, 0x00, 0x00, 0x0a, 0x11};
    bcm_mac_t pwe_mac2  = {0x00, 0x00, 0x00, 0x00, 0x0a, 0x22};
    bcm_mac_t ac_mac  = {0x00, 0x00, 0x00, 0x00, 0x0a, 0x33};

    char *proc_name;
    proc_name = "system_mpls_port_p2p_example";

    /* Init parameters */
    init_vpls_params(unit1,unit2);
    vpn = 0;
    pwe__term_2.forwarding_domain = vpn;
    pwe__term_2.flags2 |= BCM_MPLS_PORT2_CROSS_CONNECT;

    multi_dev_system_port_to_local(unit1,port1,&ac__port.port);
    multi_dev_system_port_to_local(unit2,port2,&pwe__port_2.port);

    /*
     * Configure AC and PWE ports
     */
    /* AC port on unit1 */
    rv = vpls_configure_port_properties(unit1, ac__port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_configure_port_properties ac__port\n", proc_name);
        return rv;
    }

    /* PWE port on unit2 */
    rv = vpls_configure_port_properties(unit2, pwe__port_2, 1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_configure_port_properties pwe_port\n", proc_name);
        return rv;
    }

    /*
     * Configure L3 interfaces
     */
    rv = vpls_create_l3_interfaces(unit2, &pwe__port_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_l3_interfaces\n", proc_name);
        return rv;
    }

    /*
     * Configure an ARP entries
     */
    rv = vpls_create_arp_entry(unit2, &pwe__port_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_arp_entry\n", proc_name);
        return rv;
    }
    mpls__encap_2[0].l3_intf_id = pwe__port_2.arp;

    /*
     * Configure a push entry
     */
    rv = vpls_create_mpls_tunnel(unit2, mpls__encap_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_mpls_tunnel\n", proc_name);
        return rv;
    }
    pwe__encap_2.egress_tunnel_if = mpls__encap_2[0].tunnel_id;

    /*
     * Configure fec entry
     */
    rv = vpls_create_fec_entry(unit2, pwe__port_2.encap_fec_id, mpls__encap_2[0].tunnel_id, port2, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_fec_entry\n", proc_name);
        return rv;
    }
    pwe__term_2.egress_tunnel_if = pwe__port_2.encap_fec_id;

    rv = vpls_create_fec_entry(unit1, pwe__port_2.encap_fec_id, mpls__encap_2[0].tunnel_id, port2, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_fec_entry\n", proc_name);
        return rv;
    }

    /*
     * Configure a termination label for the ingress flow
     */
    rv = vpls_create_termination_stack(unit2, mpls__term_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_termination_stack\n", proc_name);
        return rv;
    }

    /*
     * configure PWE tunnel - egress flow
     */
    rv = vpls_mpls_port_add_encapsulation(unit2, &pwe__encap_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_mpls_port_add_encapsulation\n", proc_name);
        return rv;
    }
    pwe__term_2.encap_id = pwe__encap_2.encap_id;
    pwe__term_2.mpls_port_id = pwe__encap_2.encap_id;

    /*
     * configure PWE tunnel - ingress flow
     */
    rv = vpls_mpls_port_add_termination(unit2, &pwe__term_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_mpls_port_add_termination\n", proc_name);
        return rv;
    }

    /* Configure ac port */
    rv = vpls_add_access_port(unit1, &ac__port, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_add_access_port ac_port\n", proc_name);
        return rv;
    }

    /*
     * Cross connect AC to pwe, unit1 -> unit2
     */
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init (gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = ac__port.vlan_port_id;
    BCM_GPORT_FORWARD_PORT_SET(gports.port2, pwe__port_2.encap_fec_id);
    gports.encap2 = pwe__encap_2.encap_id;
    printf("bcm_vswitch_cross_connect_add( gport1 = %d, gport2 = %d, encap1 = %d, encap2 = %d )\n", gports.port1,
           gports.port2, gports.encap1, gports.encap2);

    rv = bcm_vswitch_cross_connect_add(unit1, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_vswitch_cross_connect_add", proc_name);
        return rv;
    }

    /*
     * Cross connect pwe to AC, unit2 -> unit1
     */
    bcm_vswitch_cross_connect_t_init (gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = pwe__term_2.mpls_port_id;
    gports.port2 = port1;
    gports.encap2 = ac__port.encap_id;
    printf("bcm_vswitch_cross_connect_add( gport1 = %d, gport2 = %d, encap1 = %d, encap2 = %d )\n", gports.port1,
           gports.port2, gports.encap1, gports.encap2);

    rv = bcm_vswitch_cross_connect_add(unit2, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_vswitch_cross_connect_add", proc_name);
        return rv;
    }

    return rv;
}

/*
 * VLAN-Port unit1 primary FEC -> MPLS-Port 1 unit2
 *                 secondary FEC -> MPLS-Port 2 unit2
 */
int system_protected_mpls_port_mp_example(
    int unit1,
    int port1,
    int unit2,
    int port2,
    int port3)
{
    int rv = BCM_E_NONE;
    bcm_mac_t pwe_mac1  = {0x00, 0x00, 0x00, 0x00, 0x0a, 0x11};
    bcm_gport_t gport;

    char *proc_name;
    proc_name = "system_protected_mpls_port_mp_example";

    /* Init parameters */
    init_vpls_params(unit1,unit2);
    pwe__encap_2.nwk_group_valid = 1; /* split horizon */
    pwe__encap_2.pwe_nwk_group_id = 1;

    /* FECs are protected */
    pwe__port_2.encap_fec_id = pwe__port_1.encap_fec_id + 1;

    /* Ac on unit1, pwe 1 and pwe 2 are on unit2 */
    multi_dev_system_port_to_local(unit1,port1,&ac__port.port);
    multi_dev_system_port_to_local(unit2,port2,&pwe__port_1.port);
    multi_dev_system_port_to_local(unit2,port3,&pwe__port_2.port);

    /*
     * create VPN and multicast group
     */
    rv = vpls_vswitch_create(unit1, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vpls_vswitch_create \n", proc_name);
        return rv;
    }

    rv = vpls_vswitch_create(unit2, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vpls_vswitch_create \n", proc_name);
        return rv;
    }

    /*
     * Configure AC and PWE ports
     */
    /* AC port on unit1 */
    rv = vpls_configure_port_properties(unit1, ac__port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_configure_port_properties ac__port\n", proc_name);
        return rv;
    }

    /* PWE ports on unit2 */
    rv = vpls_configure_port_properties(unit2, pwe__port_1, 1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_configure_port_properties pwe_port\n", proc_name);
        return rv;
    }

    rv = vpls_configure_port_properties(unit2, pwe__port_2, 1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_configure_port_properties pwe_port\n", proc_name);
        return rv;
    }

    /*
     * Configure L3 interfaces
     */
    rv = vpls_create_l3_interfaces(unit2, &pwe__port_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_l3_interfaces\n", proc_name);
        return rv;
    }

    /*
     * Configure failover object
     */
    rv = bcm_failover_create(unit1, BCM_FAILOVER_FEC, &failover_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in bcm_failover_create\n", proc_name);
        return rv;
    }

    /*
     * Configure an ARP entries
     */
    rv = vpls_create_arp_entry(unit2, &pwe__port_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_arp_entry\n", proc_name);
        return rv;
    }
    /* Both tunnel use same ARP */
    mpls__encap_1[0].l3_intf_id = pwe__port_1.arp;
    mpls__encap_2[0].l3_intf_id = pwe__port_1.arp;

    /*
     * Configure a push entry
     */
    rv = vpls_create_mpls_tunnel(unit2, mpls__encap_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_mpls_tunnel\n", proc_name);
        return rv;
    }
    /* Both PWE points to same LSP tunnel */
    pwe__encap_1.egress_tunnel_if = mpls__encap_1[0].tunnel_id;
    pwe__encap_2.egress_tunnel_if = mpls__encap_1[0].tunnel_id;

    /*
     * Configure a termination label for the ingress flow
     */
    rv = vpls_create_termination_stack(unit2, mpls__term_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_termination_stack\n", proc_name);
        return rv;
    }

    /*
     * configure PWE tunnel - egress flow
     */
    rv = vpls_mpls_port_add_encapsulation(unit2, &pwe__encap_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_mpls_port_add_encapsulation\n", proc_name);
        return rv;
    }
    pwe__term_1.encap_id = pwe__encap_1.encap_id;
    pwe__term_1.mpls_port_id = pwe__encap_1.encap_id;

    rv = vpls_mpls_port_add_encapsulation(unit2, &pwe__encap_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_mpls_port_add_encapsulation\n", proc_name);
        return rv;
    }
    pwe__term_2.encap_id = pwe__encap_2.encap_id;
    pwe__term_2.mpls_port_id = pwe__encap_2.encap_id;

    /*
     * configure PWE tunnel - ingress flow
     */
    rv = vpls_mpls_port_add_termination(unit2, &pwe__term_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_mpls_port_add_termination\n", proc_name);
        return rv;
    }

    rv = vpls_mpls_port_add_termination(unit2, &pwe__term_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_mpls_port_add_termination\n", proc_name);
        return rv;
    }

    /*
     * Configure fec entry
     */
    /* protection path */
    BCM_L3_ITF_SET(gport,BCM_L3_ITF_TYPE_LIF, pwe__encap_2.encap_id);
    rv = l3__egress_only_fec__create_inner(unit1, pwe__port_2.encap_fec_id, gport, 0, port3, 0, 0, failover_id, 0, NULL);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n", proc_name);
        return rv;
    }

    /* primary path */
    BCM_L3_ITF_SET(gport,BCM_L3_ITF_TYPE_LIF, pwe__encap_1.encap_id);
    rv = l3__egress_only_fec__create_inner(unit1, pwe__port_1.encap_fec_id, gport, 0, port2, 0, 0, failover_id, 1, NULL);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n", proc_name);
        return rv;
    }

    /* Configure ac port */
    rv = vpls_add_access_port(unit1, &ac__port, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_add_access_port ac_port\n", proc_name);
        return rv;
    }

    /*
     * Add l2 addresses to be defined as static
     */
    /* MACT entry to unit2's pwe on unit1 */
    BCM_GPORT_FORWARD_PORT_SET(gport, pwe__port_1.encap_fec_id);
    rv = vpls_l2_addr_add(unit1, vpn, pwe_mac1, gport, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_l2_addr_add\n", proc_name);
        return rv;
    }

    /* Add AC and PWE to multicast group */
    rv = multicast__add_multicast_entry(unit1, vpn, &port2, &pwe__encap_1.encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, in multicast__add_multicast_entry for PWE 1\n", proc_name);
        return rv;
    }

    rv = multicast__add_multicast_entry(unit1, vpn, &port3, &pwe__encap_2.encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, in multicast__add_multicast_entry for PWE 2\n", proc_name);
        return rv;
    }

    rv = multicast__add_multicast_entry(unit1, vpn, &port1, &ac__port.encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, in multicast__add_multicast_entry for AC\n", proc_name);
        return rv;
    }

    rv = multicast__add_multicast_entry(unit2, vpn, &port2, &pwe__encap_1.encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, in multicast__add_multicast_entry for PWE 1\n", proc_name);
        return rv;
    }

    rv = multicast__add_multicast_entry(unit2, vpn, &port3, &pwe__encap_2.encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, in multicast__add_multicast_entry for PWE 2\n", proc_name);
        return rv;
    }

    rv = multicast__add_multicast_entry(unit2, vpn, &port1, &ac__port.encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, in multicast__add_multicast_entry for AC\n", proc_name);
        return rv;
    }

    /* Split-Horizon need to be taken from ETPS TOS , need to configure native ac as well  */
    bcm_vlan_port_t vp;
    int nwk_grp_from_esem;
    nwk_grp_from_esem = *dnxc_data_get(unit2, "esem", "feature", "etps_properties_assignment", NULL);

    if (nwk_grp_from_esem)
    {
        bcm_vlan_port_t_init(&vp);
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vp.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
        vp.egress_network_group_id = 1;
        vp.port = pwe__encap_1.mpls_port_id;
        vp.vsi = vpn;
        rv = bcm_vlan_port_create(unit2, &vp);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): ERROR: bcm_vlan_port_create default_native_ac\n", proc_name);
            return rv;
        }

        bcm_vlan_port_t_init(&vp);
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vp.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
        vp.egress_network_group_id = 1;
        vp.port = pwe__encap_2.mpls_port_id;
        vp.vsi = vpn;
        rv = bcm_vlan_port_create(unit2, &vp);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): ERROR: bcm_vlan_port_create default_native_ac\n", proc_name);
            return rv;
        }
    }

    return rv;
}

int system_protected_mpls_port_p2p_example(
    int unit1,
    int port1,
    int unit2,
    int port2,
    int port3)
{
    int rv = BCM_E_NONE;
    bcm_mac_t pwe_mac1  = {0x00, 0x00, 0x00, 0x00, 0x0a, 0x11};
    bcm_gport_t gport;

    char *proc_name;
    proc_name = "system_protected_mpls_port_p2p_example";

    /* Init parameters */
    init_vpls_params(unit1,unit2);

    vpn = 0;
    pwe__term_1.forwarding_domain = vpn;
    pwe__term_1.flags2 |= BCM_MPLS_PORT2_CROSS_CONNECT;
    pwe__term_2.forwarding_domain = vpn;
    pwe__term_2.flags2 |= BCM_MPLS_PORT2_CROSS_CONNECT;

    /* FECs are protected */
    pwe__port_2.encap_fec_id = pwe__port_1.encap_fec_id + 1;

    /* Ac on unit1, pwe 1 and pwe 2 are on unit2 */
    multi_dev_system_port_to_local(unit1,port1,&ac__port.port);
    multi_dev_system_port_to_local(unit2,port2,&pwe__port_1.port);
    multi_dev_system_port_to_local(unit2,port3,&pwe__port_2.port);

    /*
     * Configure AC and PWE ports
     */
    /* AC port on unit1 */
    rv = vpls_configure_port_properties(unit1, ac__port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_configure_port_properties ac__port\n", proc_name);
        return rv;
    }

    /* PWE ports on unit2 */
    rv = vpls_configure_port_properties(unit2, pwe__port_1, 1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_configure_port_properties pwe_port\n", proc_name);
        return rv;
    }

    rv = vpls_configure_port_properties(unit2, pwe__port_2, 1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_configure_port_properties pwe_port\n", proc_name);
        return rv;
    }

    /*
     * Configure L3 interfaces
     */
    rv = vpls_create_l3_interfaces(unit2, &pwe__port_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_l3_interfaces\n", proc_name);
        return rv;
    }

    /*
     * Configure failover object
     */
    rv = bcm_failover_create(unit1, BCM_FAILOVER_FEC, &failover_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in bcm_failover_create\n", proc_name);
        return rv;
    }

    /*
     * Configure an ARP entries
     */
    rv = vpls_create_arp_entry(unit2, &pwe__port_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_arp_entry\n", proc_name);
        return rv;
    }
    /* Both tunnel use same ARP */
    mpls__encap_1[0].l3_intf_id = pwe__port_1.arp;
    mpls__encap_2[0].l3_intf_id = pwe__port_1.arp;

    /*
     * Configure a push entry
     */
    rv = vpls_create_mpls_tunnel(unit2, mpls__encap_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_mpls_tunnel\n", proc_name);
        return rv;
    }
    /* Both PWE points to same LSP tunnel */
    pwe__encap_1.egress_tunnel_if = mpls__encap_1[0].tunnel_id;
    pwe__encap_2.egress_tunnel_if = mpls__encap_1[0].tunnel_id;

    /*
     * Configure a termination label for the ingress flow
     */
    rv = vpls_create_termination_stack(unit2, mpls__term_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_termination_stack\n", proc_name);
        return rv;
    }

    /*
     * configure PWE tunnel - egress flow
     */
    rv = vpls_mpls_port_add_encapsulation(unit2, &pwe__encap_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_mpls_port_add_encapsulation\n", proc_name);
        return rv;
    }
    pwe__term_1.encap_id = pwe__encap_1.encap_id;
    pwe__term_1.mpls_port_id = pwe__encap_1.encap_id;

    rv = vpls_mpls_port_add_encapsulation(unit2, &pwe__encap_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_mpls_port_add_encapsulation\n", proc_name);
        return rv;
    }
    pwe__term_2.encap_id = pwe__encap_2.encap_id;
    pwe__term_2.mpls_port_id = pwe__encap_2.encap_id;

    /*
     * configure PWE tunnel - ingress flow
     */
    rv = vpls_mpls_port_add_termination(unit2, &pwe__term_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_mpls_port_add_termination\n", proc_name);
        return rv;
    }

    rv = vpls_mpls_port_add_termination(unit2, &pwe__term_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_mpls_port_add_termination\n", proc_name);
        return rv;
    }

    /*
     * Configure fec entry
     */
    /* protection path */
    BCM_L3_ITF_SET(gport,BCM_L3_ITF_TYPE_LIF, pwe__encap_2.encap_id);
    rv = l3__egress_only_fec__create_inner(unit1, pwe__port_2.encap_fec_id, gport, 0, port3, 0, 0, failover_id, 0, NULL);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n", proc_name);
        return rv;
    }

    /* primary path */
    BCM_L3_ITF_SET(gport,BCM_L3_ITF_TYPE_LIF, pwe__encap_1.encap_id);
    rv = l3__egress_only_fec__create_inner(unit1, pwe__port_1.encap_fec_id, gport, 0, port2, 0, 0, failover_id, 1, NULL);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n", proc_name);
        return rv;
    }

    /* Configure ac port */
    rv = vpls_add_access_port(unit1, &ac__port, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_add_access_port ac_port\n", proc_name);
        return rv;
    }

    /*
     * Cross connect AC to pwe, unit1 -> unit2
     */
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init (gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = ac__port.vlan_port_id;
    BCM_GPORT_FORWARD_PORT_SET(gports.port2, pwe__port_1.encap_fec_id);
    printf("bcm_vswitch_cross_connect_add( gport1 = %d, gport2 = %d, encap1 = %d, encap2 = %d )\n", gports.port1,
           gports.port2, gports.encap1, gports.encap2);

    rv = bcm_vswitch_cross_connect_add(unit1, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_vswitch_cross_connect_add", proc_name);
        return rv;
    }

    /*
     * Cross connect pwe to AC, unit2 -> unit1
     */
    bcm_vswitch_cross_connect_t_init (gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = pwe__term_1.mpls_port_id;
    gports.port2 = port1;
    gports.encap2 = ac__port.encap_id;
    printf("bcm_vswitch_cross_connect_add( gport1 = %d, gport2 = %d, encap1 = %d, encap2 = %d )\n", gports.port1,
           gports.port2, gports.encap1, gports.encap2);

    rv = bcm_vswitch_cross_connect_add(unit2, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_vswitch_cross_connect_add", proc_name);
        return rv;
    }

    bcm_vswitch_cross_connect_t_init (gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = pwe__term_2.mpls_port_id;
    gports.port2 = port1;
    gports.encap2 = ac__port.encap_id;
    printf("bcm_vswitch_cross_connect_add( gport1 = %d, gport2 = %d, encap1 = %d, encap2 = %d )\n", gports.port1,
           gports.port2, gports.encap1, gports.encap2);

    rv = bcm_vswitch_cross_connect_add(unit2, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_vswitch_cross_connect_add", proc_name);
        return rv;
    }

    return rv;
}
