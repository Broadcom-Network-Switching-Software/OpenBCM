/*
 * **************************************************************************************************************************************************
 *
 * Network diagram
 *
 *                          --------------------------------------
 *                         |                                      |
 *                         |                                      |
 *                         |               ROUTER                 |
 *                         |                                      |
 *                         |   ---------               ------     |
 *                         |  |  VSIx   |             | VSIy |    |
 *                         |   ---------               ------     |
 *                         |    |     |                  |        |
 *                         |    | RIF1|              RIF2|        |
 *                         |____|_____|__________________|________|
 *                              |     |                  |
 *                              |     |                  |
 *                          host1    host2             host3
 *                         <10,100>  <20>             <30,300>
 * 
 *                         <x,y> - x is outer vlan
 *                                 y is inner vlan
 * 
 * Default values:
 * VLAN values are in diagramm
 * Host1   -   MAC: 00:00:11:11:11:11
 *             IP:  11.11.11.11
 * Host2   -   MAC: 00:00:22:22:22:22
 *             IP:  22.22.22.22
 * Host3   -   MAC: 00:00:33:33:33:33
 *             IP:  33.33.33.33
 * PORT1(RIF1) - MAC: 00:00:00:00:01:11
 * PORT2(RIF2) - MAC: 00:00:00:00:02:22
 * 
 * Test Scenario
 *
 * Test Scenario - start
  cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vlan_translate.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_route_over_ac.c
  cint
  route_over_ac_example(0,200,201);
  exit;
 *
 * host3 -> host1
  tx 1 psrc=201 data=0x0000000002220000333333339100001e8100012c080045000035000000008000b2413333333311111111000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x0000111111110000000001118100000a91000064080045000035000000007f00b3413333333311111111000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * host1 -> host2
  tx 1 psrc=200 data=0x0000000001110000111111119100000a81000064080045000035000000008000d4631111111122222222000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x00002222222200000000022281000014080045000035000000007f00d5631111111122222222000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Test Scenario - end
 *
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 **************************************************************************************************** */
int ROUTE_OVER_AC_NUM_OF_HOSTS = 3;

/*  Main Struct  */
struct route_over_ac_s
{
    int vlan_outer[ROUTE_OVER_AC_NUM_OF_HOSTS];              /* outer vlan  */
    int vlan_inner[ROUTE_OVER_AC_NUM_OF_HOSTS];              /* inner vlan  */
    int intf_1;                                              /* incoming packet ETH-RIF */
    int intf_2;                                              /* outgoing packet ETH-RIF */
    bcm_mac_t intf_1_mac_address;                            /* mac for in RIF */
    bcm_mac_t intf_2_mac_address;                            /* mac for out RIF */
    bcm_mac_t arp_next_hop_mac[ROUTE_OVER_AC_NUM_OF_HOSTS];  /* mac for out RIF */
    bcm_ip_t host[ROUTE_OVER_AC_NUM_OF_HOSTS];               /* host1, host2, host3 */
    bcm_if_t fec_id[ROUTE_OVER_AC_NUM_OF_HOSTS];             /* FEC id */
    int encap_id[ROUTE_OVER_AC_NUM_OF_HOSTS];                /* encapsulation ID(ARP) */
    bcm_gport_t out_ac[ROUTE_OVER_AC_NUM_OF_HOSTS];          /* out AC gport */
    int out_lif[ROUTE_OVER_AC_NUM_OF_HOSTS];                 /* out AC encap id */
    int vrf;                                                 /* vrf */
};

/* Global struct initialization*/
route_over_ac_s g_route_over_ac =
{
    /* vlan_outer */
    {10, 20, 30},
    /* vlan_inner */
    {100, 0, 300},
    /* intf_in | intf_out */
    11, 22,
    /* intf_1_mac_address */
    {0x00, 0x00, 0x00, 0x00, 0x01, 0x11},
    /* intf_2_mac_address */
    {0x00, 0x00, 0x00, 0x00, 0x02, 0x22},
    /* arp_next_hop_mac */
    {{0x00, 0x00, 0x11, 0x11, 0x11, 0x11}, {0x00, 0x00, 0x22, 0x22, 0x22, 0x22}, {0x00, 0x00, 0x33, 0x33, 0x33, 0x33}},
    /* host IP */
    {0x11111111, 0x22222222, 0x33333333},
    /* fec_id */
    {0,0,0},
    /* encap_id */
    {0x3006, 0x3008, 0x300a},
    /* out_ac */
    {0,0,0},
    /* out_lif */
    {0,0,0},
    /* vrf */
    10,
};

/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 **************************************************************************************************** */
/* Initialization of main struct
 * This function allow to re-write default values or to leave default values without changes
 *
 * INPUT:
 *   params: new values for g_route_over_ac
 */
int route_over_ac_init(int unit)
{
    int first_fec_in_hier;
    int i;

    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &first_fec_in_hier), "");

    for (i=0; i < ROUTE_OVER_AC_NUM_OF_HOSTS; i++) {
        g_route_over_ac.fec_id[i] = first_fec_in_hier + i;
    }

    return BCM_E_NONE;
}

int route_over_ac_configure_port(
    int unit,
    int port_1,
    int port_2)
{
    int i;
    bcm_vlan_port_t vlan_port;

    /*
     * Create AC, set Port to In ETh-RIF
     */
    for (i=0; i < ROUTE_OVER_AC_NUM_OF_HOSTS; i++) {
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
        vlan_port.flags = 0;
        vlan_port.port = (i == 0) ? port_1:port_2;
        vlan_port.match_vlan = g_route_over_ac.vlan_outer[i];
        vlan_port.match_inner_vlan = g_route_over_ac.vlan_inner[i];
        vlan_port.vsi = (i == 0) ? g_route_over_ac.intf_1:g_route_over_ac.intf_2;

        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");

        /* Save out ac and out lif for Vlan translation */
        g_route_over_ac.out_ac[i] = vlan_port.vlan_port_id;
        g_route_over_ac.out_lif[i] = vlan_port.encap_id;

        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, g_route_over_ac.vlan_outer[i], (i == 0) ? port_1:port_2, 0), "");
    }

    return BCM_E_NONE;
}


int route_over_ac_example(
    int unit,
    int port_1,
    int port_2)
{
    int i;
    char error_msg[200]={0,};
    l3_ingress_intf ingress_intf;
    bcm_gport_t gport;
    bcm_vlan_action_t outer_action;
    bcm_vlan_action_t inner_action;
    int flags2 = 0;

    /* Initializing global parameters*/
    BCM_IF_ERROR_RETURN_MSG(route_over_ac_init(unit), "");

    /*
     * Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(route_over_ac_configure_port(unit, port_1, port_2), "");

    /*
     * Create ETH-RIF and set its properties
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, g_route_over_ac.intf_1, g_route_over_ac.intf_1_mac_address), "intf_1");

    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, g_route_over_ac.intf_2, g_route_over_ac.intf_2_mac_address), "intf_2");

    /*
     * Set Incoming ETH-RIF properties
     */
    l3_ingress_intf_init(&ingress_intf);
    ingress_intf.vrf = g_route_over_ac.vrf;
    ingress_intf.intf_id = g_route_over_ac.intf_1;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_intf), "intf_1");

    l3_ingress_intf_init(&ingress_intf);
    ingress_intf.vrf = g_route_over_ac.vrf;
    ingress_intf.intf_id = g_route_over_ac.intf_2;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_intf), "intf_2");
    if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        flags2 = BCM_L3_FLAGS2_FWD_ONLY;
    }
    /*
     * Create ARP and set its properties
     */
    for (i=0; i < ROUTE_OVER_AC_NUM_OF_HOSTS; i++) {
        snprintf(error_msg, sizeof(error_msg), "create egress object ARP only: encap_id = %d", g_route_over_ac.encap_id[i]);
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, 0, g_route_over_ac.encap_id[i], g_route_over_ac.arp_next_hop_mac[i], 
                                           (i == 0) ? g_route_over_ac.intf_1:g_route_over_ac.intf_2), error_msg);
    }

    /*
     * Create FEC and set its properties
     */
    for (i=0; i < ROUTE_OVER_AC_NUM_OF_HOSTS; i++) {
        BCM_GPORT_LOCAL_SET(gport, (i == 0) ? port_1:port_2);
        snprintf(error_msg, sizeof(error_msg), "reate egress object FEC only: fec = %d", g_route_over_ac.fec_id[i]);
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, g_route_over_ac.fec_id[i], (i == 0) ? g_route_over_ac.intf_1:g_route_over_ac.intf_2, 
                                         g_route_over_ac.out_lif[i], gport, 0, flags2), error_msg);
    }
    /*
     * Add IPv4 host entries
     */

    for (i=0; i < ROUTE_OVER_AC_NUM_OF_HOSTS; i++) {
        if (!*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL))
        {
            BCM_GPORT_FORWARD_PORT_SET(gport, g_route_over_ac.fec_id[i]);
            BCM_IF_ERROR_RETURN_MSG(add_route_ipv4_encap_dest(unit, g_route_over_ac.host[i], 0xffffffff, g_route_over_ac.vrf, 0, g_route_over_ac.encap_id[i], gport), "");
        }
        else {
            BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, g_route_over_ac.host[i], g_route_over_ac.vrf, g_route_over_ac.fec_id[i], g_route_over_ac.encap_id[i], 0), "");
        }
    }

    /*
     * Add VLAN translation
     */
    outer_action = bcmVlanActionAdd;
    for (i=0; i < ROUTE_OVER_AC_NUM_OF_HOSTS; i++) {
        if (i == 1) {
            inner_action = bcmVlanActionNone;
        } else {
            inner_action = bcmVlanActionAdd;
        }
        BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(unit, g_route_over_ac.out_ac[i],          /* out ac  */
                                                           0x8100,                            /* outer_tpid */
                                                           0x9100,                            /* inner_tpid */
                                                           outer_action,                      /* outer_action */
                                                           inner_action,                      /* inner_action */
                                                           g_route_over_ac.vlan_outer[i],     /* new_outer_vid */
                                                           g_route_over_ac.vlan_inner[i],     /* new_inner_vid */
                                                           3+i,                               /* vlan_edit_profile */
                                                           0,                                 /* tag_format - here is untag */
                                                           FALSE                              /* is_ive */
                                                           ), "create egress vlan editing");
    }

    return BCM_E_NONE;
}
