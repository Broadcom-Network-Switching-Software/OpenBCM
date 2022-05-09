/*
 * $Id: cint_dnx_system_vswitch.c
 *
 * System resource example:
 * In the following CINT we will try to provide an example of System resources in local mode.
 *    The focus of the CINT is on AC-LIFs (bcm_vlan_port_t object in BCM API).
 * Similiar principles are apply also to other LIF types such as bcm_mpls_port_t
 * (see cint_dnx_system_vswitch_vpls.c).
 * Each AC-LIF allocates two objects: An Ingress LIF entry and an Egress LIF entry.
 * In case of 1:1 protection also a L2 FEC object that stores
 * the protection pairs is also being created.
 *
 * The following CINT provides some basic examples of AC-LIf metro vswitch allocation schemes when
 *  working in local mode.
 *  1. VLAN P2P unprotected: see system_vlan_port_p2p_example
 *  2. VLAN MP unprotection: see system_vlan_port_mp_example
 *  3. VLAN P2P/MP protected: see system_protected_vlan_port_example
 *
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_multicast.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_vlan_translate.c
 * cint ../../src/./examples/dnx/cint_dnx_system_vswitch.c
 * cint
 * system_vlan_port_mp_example(1,1811939528,1811939529,2,1811940552);
 * exit;
 *
 * MP system managment, configured mac: Send from unit1, port1 to unit2, port2
 *   tx 1 psrc=200 data=0x0000000000110001020304058100000affff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 2 should be:
 *   Data: 0x0000000000110001020304058100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * MP system managment, learned SA: Send from unit2, port2 to unit1, port1
 *   tx 1 psrc=200 data=0x0001020304050000000000118100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 1 should be:
 *   Data: 0x0001020304050000000000118100000affff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * MP system managment, expect flood: Send from unit2, port2
 *   tx 1 psrc=200 data=0x0000000011220000276708208100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 1 should be:
 *   Data: 0x0000000011220000276708208100000affff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *   Data: 0x0000000011220000276708208100000affff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * MP system managment, learned SA: Send from unit1, second_port1 to unit2, port2
 *   tx 1 psrc=201 data=0x0000276708200000e62719498100000affff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 2 should be:
 *   Data: 0x0000276708200000e62719498100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * MP system managment, configured mac: Send from unit1, second_port1 to unit1, port1
 *   tx 1 psrc=201 data=0x00000000002200001de4bbd68100000affff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 1 should be:
 *   Data: 0x00000000002200001de4bbd68100000affff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * MP system managment, configured mac: Send from unit1, port1 to unit1, second_port1
 *   tx 1 psrc=200 data=0x00000000003300001de4bbd68100000affff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 1 should be:
 *   Data: 0x00000000003300001de4bbd68100000affff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * MP system managment, configured mac: Send from unit2, port2 to unit1, second_port1
 *   tx 1 psrc=200 data=0x0000000000330000b122c89b8100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 1 should be:
 *   Data: 0x0000000000330000b122c89b8100000affff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * MP system managment, learned SA: Send from unit1, port1 to unit2, port2
 *   tx 1 psrc=200 data=0x0000b122c89b0000db17ab548100000affff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 2 should be:
 *   Data: 0x0000b122c89b0000db17ab548100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
*/

bcm_failover_t failover_id = 0;


/*
 * Create L2 VPN (VSI and MC group)
 */
int
system_vswitch_create(
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

/*
 * bcm_vlan_port_create: for unprotected case.
 */
int
system_vlan_port_create(
    int unit,
    bcm_port_t port,
    bcm_vlan_t vlan,
    int vsi,
    bcm_gport_t *gport,
    bcm_vlan_port_t *vp)
{
    int rv;
    bcm_vlan_action_set_t action;

    bcm_vlan_port_t_init(vp);
    vp->port = port;
    vp->vsi = 0; /* will be populated when the gport is added to service, using vswitch_port_add */
    vp->criteria = BCM_VLAN_PORT_MATCH_NONE;
    if (*gport!=0) {
        vp->flags = BCM_VLAN_PORT_WITH_ID;
        BCM_GPORT_SUB_TYPE_LIF_SET(vp->vlan_port_id, 0, *gport);
        BCM_GPORT_VLAN_PORT_ID_SET(vp->vlan_port_id, vp->vlan_port_id);
    }
    rv = bcm_vlan_port_create(unit, vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    if(verbose >= 1) {
        printf("created vlan_port:0x%08x in unit %d \n",vp->vlan_port_id, unit);
    }

    *gport = vp->vlan_port_id;

    /* Add ingress AC match */
    bcm_port_match_info_t port_match_info;
    bcm_port_match_info_t_init(&port_match_info);

    port_match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
    port_match_info.match = BCM_PORT_MATCH_PORT_CVLAN; /* 0x8100 */
    port_match_info.port = port;
    port_match_info.match_vlan = vlan;

    rv = bcm_port_match_add(unit, vp->vlan_port_id, &port_match_info);
    if (rv != BCM_E_NONE) {
       printf("Error, in bcm_port_match_add\n");
       return rv;
    }

    /* Add port to vlan membership */
    rv = bcm_vlan_gport_add(unit, vlan, port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_gport_add\n");
        return rv;
    }

    /* Add AC to VPN */
    if (vsi > 0) {
        rv = bcm_vswitch_port_add(unit, vsi, *gport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_port_add\n");
            return rv;
        }
    }

    /* Create EVE on egress AC */
    rv = vlan_translate_ive_eve_translation_set(unit, vp->vlan_port_id,           /* lif  */
                                                      0x8100,                     /* outer_tpid */
                                                      0x9100,                     /* inner_tpid */
                                                      bcmVlanActionReplace,       /* outer_action */
                                                      bcmVlanActionNone,          /* inner_action */
                                                      vlan,                       /* new_outer_vid */
                                                      0,                          /* new_inner_vid */
                                                      3,                          /* vlan_edit_profile */
                                                      8,                          /* tag_format - here is ctag */
                                                      FALSE                       /* is_ive */
                                                      );
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress vlan editing\n");
        return rv;
    }

    return rv;
}

/*
 * Multipoint Example:
 *  devices : unit1 and unit2
 *
 *  physical port:
 *      - unit1: port1,second_port1
 *      - unit2: port2
 *
 *  gports (LIFs):
 *      - unit1:
 *          - gport1:       on <port2,vlan1=10>
 *          - second_gport1 on <second_port1,v1=10>
 *      - unit2:
 *          - gport2        on <port2,vlan2=30>
 *
 * << NOTE: port parameters should be given as system-gport >>
 *
 */
int system_vlan_port_mp_example(
    int unit1,
    int port1,
    int second_port1,
    int unit2,
    int port2)
{
    int rv = BCM_E_NONE;
    bcm_vlan_t vpn = 6000;

    bcm_vlan_port_t vp1;
    bcm_vlan_port_t_init(&vp1);
    bcm_vlan_port_t second_vp1;
    bcm_vlan_port_t_init(&second_vp1);
    bcm_vlan_port_t vp2;
    bcm_vlan_port_t_init(&vp2);
    int vlan1 = 10;
    bcm_gport_t gport1= 4098;
    int vlan2 = 30;
    bcm_gport_t gport2= 4098;
    bcm_gport_t second_gport1= 4099;

    bcm_mac_t mac1  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
    bcm_mac_t mac2  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x22};
    bcm_mac_t mac3  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33};

    char *proc_name;
    proc_name = "system_vlan_port_mp_example";

    /*
     * Create VPN and multicast group
     */
    rv = system_vswitch_create(unit1, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vpls_vswitch_create \n", proc_name);
        return rv;
    }

    rv = system_vswitch_create(unit2, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vpls_vswitch_create \n", proc_name);
        return rv;
    }

    /*
     * Create ACs
     */
    /* create gport1 in unit1 attach to vsi */
    rv = system_vlan_port_create(unit1, port1, vlan1, vpn, &gport1, &vp1);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, system_vlan_port_create port 1\n", proc_name);
        return rv;
    }

    /* create second_gport1 in unit1 */
    rv = system_vlan_port_create(unit1, second_port1, vlan1, vpn, &second_gport1, &second_vp1);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, system_vlan_port_create second port 1\n", proc_name);
        return rv;
    }

    /* create gport2 (different gport) in unit2 */
    rv = system_vlan_port_create(unit2, port2, vlan2, vpn, &gport2, &vp2);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, system_vlan_port_create port 2\n", proc_name);
        return rv;
    }

    /* Add ACs to multicast group */
    /* unit1 */
    rv = multicast__add_multicast_entry(unit1, vpn, &port1, &vp1.encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, in multicast__add_multicast_entry for port1\n", proc_name);
        return rv;
    }

    rv = multicast__add_multicast_entry(unit1, vpn, &second_port1, &second_vp1.encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, in multicast__add_multicast_entry for second_port1\n", proc_name);
        return rv;
    }

    rv = multicast__add_multicast_entry(unit1, vpn, &port2, &vp2.encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, in multicast__add_multicast_entry for port2\n", proc_name);
        return rv;
    }

    /* unit2 */
    rv = multicast__add_multicast_entry(unit2, vpn, &port1, &vp1.encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, in multicast__add_multicast_entry for port1\n", proc_name);
        return rv;
    }

    rv = multicast__add_multicast_entry(unit2, vpn, &second_port1, &second_vp1.encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, in multicast__add_multicast_entry for second_port1\n", proc_name);
        return rv;
    }

    rv = multicast__add_multicast_entry(unit2, vpn, &port2, &vp2.encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, in multicast__add_multicast_entry for port2\n", proc_name);
        return rv;
    }

    /*
     * Add l2 addresses to be defined as static
     */
    /* On unit 1 */
    /* mac1 to unit2 port2 */
    bcm_l2_addr_t l2addr;
    bcm_l2_addr_t_init(&l2addr, mac1, vpn);
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = port2;
    l2addr.encap_id = vp2.encap_id;

    rv = bcm_l2_addr_add(unit1, l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in bcm_l2_addr_add\n", proc_name);
        return rv;
    }

    /* mac2 to unit1 port1 */
    bcm_l2_addr_t_init(&l2addr, mac2, vpn);
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = port1;
    l2addr.encap_id = vp1.encap_id;

    rv = bcm_l2_addr_add(unit1, l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in bcm_l2_addr_add\n", proc_name);
        return rv;
    }

    /* mac3 to unit1 second_port1 */
    bcm_l2_addr_t_init(&l2addr, mac3, vpn);
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = second_port1;
    l2addr.encap_id = second_vp1.encap_id;

    rv = bcm_l2_addr_add(unit1, l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in bcm_l2_addr_add\n", proc_name);
        return rv;
    }

    /* On unit 2 */
    /* mac2 to unit1 port1 */
    bcm_l2_addr_t_init(&l2addr, mac2, vpn);
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = port1;
    l2addr.encap_id = vp1.encap_id;

    rv = bcm_l2_addr_add(unit2, l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in bcm_l2_addr_add\n", proc_name);
        return rv;
    }

    /* mac3 to unit1 second_port1 */
    bcm_l2_addr_t_init(&l2addr, mac3, vpn);
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = second_port1;
    l2addr.encap_id = second_vp1.encap_id;

    rv = bcm_l2_addr_add(unit2, l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in bcm_l2_addr_add\n", proc_name);
        return rv;
    }

    return rv;
}

/* 
 * P2P Example:
 *  devices : unit1 and unit2
 *
 *  physical port:
 *      - unit1: port1
 *      - unit2: port2
 *
 *  gports (LIFs):
 *      - unit1: gport1 on <port1,vlan1=102>
 *      - unit2: gport2 on <port2,vlan2=201>
 *
 * << NOTE: port parameters should be given as system-gport >>
 *
 */
int system_vlan_port_p2p_example(
    int unit1,
    int port1,
    int unit2,
    int port2)
{
    int rv = BCM_E_NONE;

    bcm_vlan_port_t vp1;
    bcm_vlan_port_t_init(&vp1);
    bcm_vlan_port_t vp2;
    bcm_vlan_port_t_init(&vp2);
    int vlan1 = 102;
    bcm_gport_t gport1= 4098;
    int vlan2 = 201;
    bcm_gport_t gport2= 4098;

    char *proc_name;
    proc_name = "system_vlan_port_p2p_example";

    /*
     * Create ACs
     */
    /* create gport1 in unit1 */
    rv = system_vlan_port_create(unit1, port1, vlan1, 0, &gport1, &vp1);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, system_vlan_port_create port 1\n", proc_name);
        return rv;
    }

    /* create gport2 in unit2 */
    rv = system_vlan_port_create(unit2, port2, vlan2, 0, &gport2, &vp2);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, system_vlan_port_create port 2\n", proc_name);
        return rv;
    }

    /*
     * Cross connect AC to AC, unit1 -> unit2
     */
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init (gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = gport1;
    gports.port2 = port2;
    gports.encap2 = vp2.encap_id;
    printf("bcm_vswitch_cross_connect_add( gport1 = %d, gport2 = %d, encap1 = %d, encap2 = %d )\n", gports.port1,
           gports.port2, gports.encap1, gports.encap2);

    rv = bcm_vswitch_cross_connect_add(unit1, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_vswitch_cross_connect_add", proc_name);
        return rv;
    }

    /*
     * Cross connect AC to AC, unit2 -> unit1
     */
    bcm_vswitch_cross_connect_t_init (gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = gport2;
    gports.port2 = port1;
    gports.encap2 = vp1.encap_id;
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
 * Multipoint Example:
 *  devices : unit1 and unit2
 *
 *  physical port:
 *      - unit1: port1
 *      - unit2: port2, port3
 *
 *  two gports
 *  - gport1
 *      - not protected
 *      - resources:
 *          - unit1: LIF
 *  - gport2
 *      - protected
 *      - resources:
 *          - unit1: primary_fec, secondary_fec
 *          - unit2: lif_a, lif_b
 *
 * << NOTE: port parameters should be given as system-gport >>
 */
int system_protected_vlan_port_example(
    int unit1,
    int port1,
    int unit2,
    int port2,
    int port3,
    int is_mp)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    proc_name = "system_protected_vlan_port_example";

    int vpn;
    bcm_vlan_port_t vp1;
    bcm_vlan_port_t_init(&vp1);
    bcm_vlan_port_t vp2_a;
    bcm_vlan_port_t_init(&vp2_a);
    bcm_vlan_port_t vp2_b;
    bcm_vlan_port_t_init(&vp2_b);
    int gport;

    /* unit1 AC */
    int vlan1 = 10;
    bcm_gport_t gport1_lif = 4098;

    /* unit2 AC */
    int vlan2 = 20;
    int vlan3 = 30;
    bcm_gport_t gport2_lif_a = 4098;
    bcm_gport_t gport2_lif_b = 4099;

    bcm_mac_t mac1  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
    bcm_mac_t mac2  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x22};
    bcm_mac_t mac3  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33};

    /* FECs on unit1 */
    bcm_gport_t primary_fec;
    bcm_gport_t secondary_fec;
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit1, 0, 0, &primary_fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    secondary_fec = primary_fec + 1;

    if (is_mp) {
        vpn = 7000;
        /*
         * Create VPN and multicast group
         */
        rv = system_vswitch_create(unit1, vpn);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, vpls_vswitch_create \n", proc_name);
            return rv;
        }

        rv = system_vswitch_create(unit2, vpn);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, vpls_vswitch_create \n", proc_name);
            return rv;
        }
    } else {
        vpn = 0;
    }

    /* create gport1 in unit1 without protection  */
    rv = system_vlan_port_create(unit1, port1, vlan1, vpn, &gport1_lif, &vp1);
    if (rv != BCM_E_NONE) {
      printf("%s(): Error, system_vlan_port_create port 1\n", proc_name);
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
     * Configure fec entry
     */
    /* protection path */
    BCM_L3_ITF_SET(gport,BCM_L3_ITF_TYPE_LIF, gport2_lif_b);
    rv = l3__egress_only_fec__create_inner(unit1, secondary_fec, gport, 0, port3, 0, 0, failover_id, 0, NULL);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n", proc_name);
        return rv;
    }

    /* primary path */
    BCM_L3_ITF_SET(gport,BCM_L3_ITF_TYPE_LIF, gport2_lif_a);
    rv = l3__egress_only_fec__create_inner(unit1, primary_fec, gport, 0, port2, 0, 0, failover_id, 1, NULL);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n", proc_name);
        return rv;
    }

    /*
     * Create AC on unit2
     */
    /* create gport2.LIFa in unit2 */
    rv = system_vlan_port_create(unit2, port2, vlan2, vpn, &gport2_lif_a, &vp2_a);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, system_vlan_port_create port 2\n", proc_name);
        return rv;
    }

    /* create gport2.LIFb in unit2 */
    rv = system_vlan_port_create(unit2, port3, vlan3, vpn, &gport2_lif_b, &vp2_b);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, system_vlan_port_create port 3\n", proc_name);
        return rv;
    }

    if (is_mp) {
        /* Add ACs to multicast group */
        /* unit1 */
        rv = multicast__add_multicast_entry(unit1, vpn, &port1, &vp1.encap_id, 1, 0);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error, in multicast__add_multicast_entry for port1\n", proc_name);
            return rv;
        }

        rv = multicast__add_multicast_entry(unit1, vpn, &port2, &vp2_a.encap_id, 1, 0);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error, in multicast__add_multicast_entry for port2\n", proc_name);
            return rv;
        }

        rv = multicast__add_multicast_entry(unit1, vpn, &port3, &vp2_b.encap_id, 1, 0);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error, in multicast__add_multicast_entry for port3\n", proc_name);
            return rv;
        }

        /* unit2 */
        rv = multicast__add_multicast_entry(unit2, vpn, &port1, &vp1.encap_id, 1, 0);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error, in multicast__add_multicast_entry for port1\n", proc_name);
            return rv;
        }

        rv = multicast__add_multicast_entry(unit2, vpn, &port2, &vp2_a.encap_id, 1, 0);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error, in multicast__add_multicast_entry for port2\n", proc_name);
            return rv;
        }

        rv = multicast__add_multicast_entry(unit2, vpn, &port3, &vp2_b.encap_id, 1, 0);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error, in multicast__add_multicast_entry for port3\n", proc_name);
            return rv;
        }

        /*
         * Add l2 addresses to be defined as static
         */
        bcm_l2_addr_t l2addr;
        bcm_l2_addr_t_init(&l2addr, mac2, vpn);
        l2addr.flags = BCM_L2_STATIC;
        BCM_GPORT_FORWARD_PORT_SET(l2addr.port, primary_fec);
        rv = bcm_l2_addr_add(unit1, l2addr);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in bcm_l2_addr_add\n", proc_name);
            return rv;
        }
    } else {
        /*
         * Cross connect AC to FEC gport, unit1 -> unit2
         */
        bcm_vswitch_cross_connect_t gports;
        bcm_vswitch_cross_connect_t_init (gports);
        gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
        gports.port1 = gport1_lif;
        BCM_GPORT_FORWARD_PORT_SET(gports.port2, primary_fec);
        rv = bcm_vswitch_cross_connect_add(unit1, &gports);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, bcm_vswitch_cross_connect_add", proc_name);
            return rv;
        }

        /*
         * Cross connect gport2_lif_a --> gport1_lif
         */
        bcm_vswitch_cross_connect_t_init (gports);
        gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
        gports.port1 = gport2_lif_a;
        gports.port2 = port1;
        gports.encap2 = vp1.encap_id;
        rv = bcm_vswitch_cross_connect_add(unit2, &gports);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, bcm_vswitch_cross_connect_add", proc_name);
            return rv;
        }

        /*
         * Cross connect gport2_lif_b --> gport1_lif
         */
        gports.port1 = gport2_lif_b;
        rv = bcm_vswitch_cross_connect_add(unit2, &gports);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, bcm_vswitch_cross_connect_add", proc_name);
            return rv;
        }
    }

    return rv;
}
