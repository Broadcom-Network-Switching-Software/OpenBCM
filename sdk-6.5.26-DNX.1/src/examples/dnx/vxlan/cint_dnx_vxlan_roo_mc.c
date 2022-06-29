/*
 *
 * *********************************************************************************

 * Network diagram
 *
 * We configure Router A
 *
 *           host2------| ROUTER E|  | ROUTER B|------host1
 *                      |_________|  |_________|
 * access_vlan_2, access_port_2 |     |access_vlan_1, access_port_1
 *                              |     |
 *                      host3-| |     |
 * access_vlan_1,access_port2 | |     |
 *       _____________________|_|_____|______________________________
 *      |                   |         |                             |
 *      |                   | ROUTER A| (intra DC IP)    DC FABRIC  |
 *      |   provider_vlan,  |_________|                             |
 *      |       provider_port2/       \provider_port1, provider_vlan|
 *      | native_vlan1, vni1 /        \ native_vlan1, vni1          |
 *      |            _______/___       \__________                  |
 *      |           |          |      |          |                  |
 *      |           | ROUTER C |      | ROUTER D |                  |
 *      |           |__________|      |__________|                  |

 *      |            /                          \                   |

 *      |         /                               \                 |

 *      |_____ /____________________________________\_______________|
 *       ____/___                                _____\__
 *      |  TOR1 |                               |  TOR2 |
 *      |_______|                               |_______|
 *      |       |                               |       |
 *      |       |                               |       |
 *      |_______|                               |_______|
 *      | VM:A0 |                               |       |
 *      |_______|                               |_______|
 *
 * Configuration:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_multicast.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_util_rch.c
 * cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../../../src/examples/dnx/vxlan/cint_vxlan_roo_basic.c
 * cint ../../../../src/examples/dnx/vxlan/cint_dnx_vxlan_roo_mc.c
 * int unit=0;
 * int access_port1 = 200;
 * int access_port2 = 201;
 * int provider_port1 = 202;
 * int provider_port2 = 203;
 * int add_routing_table_entry = 0;
 * cint_vxlan_roo_basic_info.tunnel_type = bcmTunnelTypeVxlan;
 * vxlan_roo_mc_run(unit, access_port1, access_port2, provider_port1, provider_port2);
 *
 * MC Traffic from access routerB to access router: routerE,
 *                                to overlay network: routerC, routerD
 *                                to host3
 *  Note: for now we only send to overlay network routerD
 *
 * Purpose: - check compatible MC at L3 to overlay and to native router
 *          - check fall to bridge
 *
 *    Send:
 *             ---------------------------------------------
 *        eth: |    DA       |     SA      |      VLAN      |
 *             ---------------------------------------------
 *             | provider_MC | routerB_mac |  access_vlan_1 |
 *             ---------------------------------------------
 *
 *                 ---------------------------
 *             ip: |   SIP     | DIP          |
 *                 ----------------------------
 *                 | host10_ip |  provider_MC |
 *                 ----------------------------
 *
 * Receive:  to access router: router E
 *
 *             ---------------------------------------------
 *        eth: |    DA       |     SA      |      VLAN      |
 *             ---------------------------------------------
 *             | provider_MC | routerA_mac |  access_vlan_2 |
 *             ---------------------------------------------
 *
 *                 ---------------------------
 *             ip: |   SIP     | DIP          |
 *                 ----------------------------
 *                 | host10_ip |  provider_MC |
 *                 ----------------------------
 *
 * Receive:  to host3 (fall to bridge)
 *
 *             --------------------------------------------
 *        eth: |    DA       |     SA      |     VLAN      |
 *             --------------------------------------------
 *             | provider_MC | routerB_mac | access_vlan_1 |
 *             --------------------------------------------
 *
 *                 ----------------------------
 *             ip: |   SIP     | DIP          |
 *                 ----------------------------
 *                 | host10_ip |  provider_MC |
 *                 ----------------------------
 *
 * Receive:  to router C, router D (overlay)
 *             --------------------------------------------
 *        eth: |    DA      |     SA      |      VLAN     |
 *             --------------------------------------------
 *             | provider_MC| routerA_mac | provider_vlan |
 *             --------------------------------------------
 *
 *                 ---------------------------
 *             ip: |   SIP     | DIP         |
 *                 ---------------------------
 *                 | routerA_ip| provider_MC |
 *                 ---------------------------
 *
 *                udp:
 *
 *                   vxlan:  ---------
 *                           |  VNI  |
 *                           ---------
 *                           |  vni  |
 *                           ---------
 *
 *                                 -------------------------------------
 *                     native eth: |    DA       |     SA      |   VLAN |
 *                                 -------------------------------------
 *                                 | provider_MC | routerA_mac |   vpn  |
 *                                 -------------------------------------
 *
 *                                     ---------------------------
 *                         native ip:  |   SIP     | DIP          |
 *                                     ----------------------------
 *                                     | host10_ip |  native_MC   |
 *                                     ----------------------------
 *
 * Packet flow  :
 * - get the VSI from port x vlan
 * - DA is identified as compatible MC, ethernet header is terminated
 * - get the RIF (= vsi)
 * - RIF is compatible MC
 * - packet is forwarded according to DIP and VRF (inRif.vrf)
 * - result is MC
 * - each MC member contains: outLif, outRif and port.
 * - for MC entry to routerE (routing): LL outLif, outRif and port.
 *   - DA is built from DIP (compatible MC)
 *   - SA = outRif.myMac
 *   - VLAN = VSI (=outRif)
 * - for MC entry to host3 (fall to bridge): LL outLif, outRif and port.
 *   - inRif == outRif so do fall to bridge
 *   - packet is bridged
 * - for MC entry to router C and D (overlay): vxlan outLif, outRif and port
 *   - vxlan tunnel is built using IP tunnel eedb entry.
 *   - vxlan header.vni is resovled from outVsi (outRif) -> vni
 *   - native is built from DIP (compatible MC)
 *   - native SA = outRif.myMac
 *   - VLAN = VSI (=outRif)
 *
 * MC traffic from overlay router C to access router B,
 *                                     access router E
 *                                     and provider router D (will be filtered)
 *  Purpose: - check tunnel is terminated,
 *           - check native MC
 *           - check fall to bridge and split horizon filter (for copy to provider router C)
 *  Note: this test is separated into 2 tests: 1 MC group to terminate the tunnel and send to access router B and access router E
 *                                             1 MC group to terminate the tunnel, fall back to bridge and split horizon filter
 *
 * Send:
 *             -----------------------------------
 *        eth: |    DA      |     SA      | VLAN |
 *             -----------------------------------
 *             | provider_MC| routerC_mac |  v2  |
 *             -----------------------------------
 *
 *                 ------------------------
 *             ip: |   SIP  | DIP         |
 *                 ------------------------
 *                 | tor1_ip| provider_MC |
 *                 ------------------------
 *
 *                udp:
 *
 *                   vxlan:  ---------
 *                           |  VNI  |
 *                           ---------
 *                           |  vni  |
 *                           ---------
 *
 *                                 -------------------------------------
 *                     native eth: |    DA       |     SA      |  VLAN |
 *                                 -------------------------------------
 *                                 | native_MC   | routerC_mac |  vpn  |
 *                                 -------------------------------------
 *
 *                                     ---------------------------
 *                         native ip:  |   SIP     | DIP          |
 *                                     ----------------------------
 *                                     | vmA0_ip   |  native_MC   |
 *                                     ----------------------------
 *
 * Recieve
*                  -------------------------------------
*      native eth: |    DA       |     SA      |  VLAN |
*                  -------------------------------------
*                  | native_MC   | routerA_mac |   v1  |
*                  -------------------------------------
*
*                      ---------------------------
*          native ip:  |   SIP     | DIP          |
*                      ----------------------------
*                      | vmA0_ip   |  native_MC   |
*                      ----------------------------
 *
 * - get the VSI from the VNI (vni -> vsi table in isem)
 * - DA is identified as compatible MC, ethernet header is terminated
 * - get the RIF (=vsi)
 * - RIF is compatible MC
 * - tunnel is terminated at TT (currently, tunnel is terminated at second pass: VTT program: if recycle port and IP MC packet, terminate the tunnel)
 * - get the inRif (=VSI)
 * - packet is forwarded according to MC
 * - RIF is compatible MC
 * - packet is forwarded according to DIP and VRF (inRif.vrf)
 * - result is MC
 * - MC members: outLif, outRif and port
 * - for MC entry to provider routerD, vxlan outLif, outRif and port
 *   - outRif = inRif ( = vsi (vsi come from VNI -> VSI mapping))
 *   - fall to bridge
 *   - ip tunnel inLif.fwrd_group = ip tunnel outLif. frwrd_group, filter the packet. (orientation filter, BUD to BUD is filtered)
 *  - for MC entries to access router E and access Router B, do routing
 *
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 **************************************************************************************************** */

struct cint_vxlan_roo_mc_s
{
    int access_port[2];                     /* incoming port from access side */
    int provider_port[2];                   /* incoming port from provider side */
    int access_eth_rif[2];                  /* access RIF */
    bcm_gport_t vlan_port_id[2];

    /* native header */
    bcm_ip_t  native_mc_dip;
    bcm_ip_t  native_mc_dip_mask;
    bcm_ip6_t native_mc_dip6;
    bcm_ip6_t native_mc_dip6_mask;
    int       native_ipmc;

    /* overlay header */
    bcm_ip_t  overlay_mc_dip;
    bcm_ip_t  overlay_mc_dip_mask;
    bcm_ip6_t overlay_mc_dip6;
    bcm_ip6_t overlay_mc_dip6_mask;
    bcm_mac_t overlay_mc_da;
    int       overlay_ipmc;

    int       recycle_port;
    int       recycle_entry_encap_id;
    int       mtu_valid;
    int       mtu;       /* link layer mtu size */
    int       ttl_valid;
    int       ttl;       /* ttl scope */
};

cint_vxlan_roo_mc_s g_vxlan_roo_mc =
{
    /* access_port */
    {200, 201},
    /* provider_port */
    {202, 203},
    /* access_eth_rif */
    {35, 40},
    /* vlan_port_id */
    {0, 0},
    /* native_mc_dip */
    0xE0000101,
    /* native_mc_dip_mask */
    0xFFFFFFFF,
    /* native_mc_dip6 */
    {0xFF, 0x34, 0x56, 0x78, 0x90, 0x23, 0x45, 0x67, 0x89, 0x01, 0x09, 0x87, 0x65, 0x43, 0x21, 0x01},
    /* native_mc_dip6_mask */
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    /* native_ipmc */
    5000,
    /* overlay_mc_dip */
    0xE0000202,
    /* overlay_mc_dip_mask */
    0xFFFFFFFF,
    /* overlay_mc_dip6 */
    {0xFF, 0xFF, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE},
    /* overlay_mc_dip6_mask */
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    /* overlay_mc_da */
    {0x01, 0x00, 0x5e, 0x00, 0x02, 0x02},
    /* overlay_ipmc */
    5001,
    /* recycle_port */
    40,
    /* recycle_entry_encap_id */
    0,
    /* mtu_valid */
    0,
    /* mtu */
    0,
    /* ttl_valid */
    0,
    /* ttl */
    0

};

int
vxlan_roo_mc_init(
    int unit,
    int is_leaf_node,
    int access_port1,
    int access_port2,
    int provider_port1,
    int provider_port2)
{
    int rv = BCM_E_NONE;

    g_vxlan_roo_mc.access_port[0] = access_port1;
    g_vxlan_roo_mc.access_port[1] = access_port2;
    g_vxlan_roo_mc.provider_port[0] = provider_port1;
    g_vxlan_roo_mc.provider_port[1] = provider_port2;

    cint_vxlan_roo_basic_info.tunnel_dip = g_vxlan_roo_mc.overlay_mc_dip;
    sal_memcpy(&(cint_vxlan_roo_basic_info.tunnel_dip6), g_vxlan_roo_mc.overlay_mc_dip6, 16);
    sal_memcpy(cint_vxlan_roo_basic_info.core_overlay_next_hop_mac, g_vxlan_roo_mc.overlay_mc_da, 6);
    cint_vxlan_roo_basic_info.term_bud = !is_leaf_node;

    return rv;
}

/*
 * Configure vlan, vlan port
 */
int
vxlan_roo_mc_configure_port_properties(
    int unit)
{
    int rv = BCM_E_NONE;
    int idx;

    /*
     * Access port
     */
    for (idx = 0; idx < 2; idx++)
    {
        /* Set <port, vlan> to LIF  */
        rv = vlan_port_add(unit, g_vxlan_roo_mc.access_port[idx], g_vxlan_roo_mc.access_eth_rif[idx], &g_vxlan_roo_mc.vlan_port_id[idx]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, vlan_port_add \n");
            return rv;
        }

        /* Create VSI */
        rv = bcm_vswitch_create_with_id(unit, g_vxlan_roo_mc.access_eth_rif[idx]);
        if (rv != BCM_E_NONE)
        {
            printf("Error: in bcm_dnx_vswitch_create_with_id\n");
            return rv;
        }

        /* Set LIF -> VSI(RIF) */
        rv = bcm_vswitch_port_add(unit, g_vxlan_roo_mc.access_eth_rif[idx], g_vxlan_roo_mc.vlan_port_id[idx]);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_port_add\n");
            return rv;
        }

        /* VLAN membership */
        rv = bcm_vlan_gport_add(unit, g_vxlan_roo_mc.access_eth_rif[idx], g_vxlan_roo_mc.access_port[idx], 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_vlan_gport_add \n");
            return rv;
        }
    }

    /*
     * Provider port
     */
    rv = in_port_intf_set(unit, g_vxlan_roo_mc.provider_port[0], cint_vxlan_roo_basic_info.core_overlay_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set provider_eth_rif \n");
        return rv;
    }

    return rv;
}

/*
 * Configure ip tunnel termination egress interfaces and ip tunnel encapsulation ingress interfaces
 */
int
vxlan_roo_mc_open_route_interfaces_access(
    int unit)
{
    int idx;
    int rv = BCM_E_NONE;
    dnx_utils_l3_eth_rif_s eth_rif_structure;

    for (idx = 0; idx < 2; idx++)
    {
        /* Set RIF -> My-Mac  and RIF -> VRF */
        dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, g_vxlan_roo_mc.access_eth_rif[idx], 0, BCM_L3_INGRESS_GLOBAL_ROUTE, cint_vxlan_roo_basic_info.access_eth_rif_mac, cint_vxlan_roo_basic_info.access_eth_vrf);
        eth_rif_structure.ttl_valid = g_vxlan_roo_mc.ttl_valid;
        eth_rif_structure.ttl = g_vxlan_roo_mc.ttl;
        eth_rif_structure.mtu_valid = g_vxlan_roo_mc.mtu_valid;
        eth_rif_structure.mtu = g_vxlan_roo_mc.mtu;
        rv = dnx_utils_l3_eth_rif_create(unit, &eth_rif_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, dnx_utils_l3_eth_rif_create access_eth_rif[%d]\n", idx);
            return rv;
        }
    }

    return rv;
}

/*
 * Configure ip tunnel termination ingress interfaces and ip tunnel encapsulation egress interfaces
 */
int
vxlan_roo_mc_open_route_interfaces_provider(
    int unit)
{
    int rv = BCM_E_NONE;
    dnx_utils_l3_eth_rif_s eth_rif_structure;

    l3_ingress_intf ingr_itf;
    l3_ingress_intf ingr_itf_overlay;
    l3_ingress_intf ingr_itf_native_overlay;

    /*
     * Overlay RIF and VRF
     */
    dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_vxlan_roo_basic_info.core_overlay_eth_rif, 0, BCM_L3_INGRESS_GLOBAL_ROUTE, cint_vxlan_roo_basic_info.core_overlay_eth_rif_mac, cint_vxlan_roo_basic_info.core_overlay_eth_vrf);
    eth_rif_structure.ttl_valid = g_vxlan_roo_mc.ttl_valid;
    eth_rif_structure.ttl = g_vxlan_roo_mc.ttl;
    eth_rif_structure.mtu_valid = g_vxlan_roo_mc.mtu_valid;
    eth_rif_structure.mtu = g_vxlan_roo_mc.mtu;
    rv = dnx_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, dnx_utils_l3_eth_rif_create provider_eth_rif\n");
        return rv;
    }

    /*
     * Native RIF and VRF
     */
    dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_vxlan_roo_basic_info.core_native_eth_rif, 0, BCM_L3_INGRESS_GLOBAL_ROUTE, cint_vxlan_roo_basic_info.core_native_eth_rif_mac, cint_vxlan_roo_basic_info.tunnel_termination_vrf);
    eth_rif_structure.ttl_valid = g_vxlan_roo_mc.ttl_valid;
    eth_rif_structure.ttl = g_vxlan_roo_mc.ttl;
    eth_rif_structure.mtu_valid = g_vxlan_roo_mc.mtu_valid;
    eth_rif_structure.mtu = g_vxlan_roo_mc.mtu;
    rv = dnx_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, dnx_utils_l3_eth_rif_create provider_eth_rif\n");
        return rv;
    }

    return rv;
}

/*
 * Set split horizon on ESEM entry
 */
int vxlan_roo_mc_split_horizon_set(int unit, int vpn)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;

    int nwk_grp_from_esem = *dnxc_data_get(unit, "esem", "feature", "etps_properties_assignment", NULL);

    if (nwk_grp_from_esem) {
        /* At ESEM for vsi, netork_domain -> VNI, outlif profile
           create a virtual AC which to configure the outlif profile in ESEM entry.
         */
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.vsi = vpn; /* vsi, part of the key in ESEM lookup */
        vlan_port.match_class_id = 0; /* network domain, part of the key in ESEM lookup */
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_VSI;
        vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
        vlan_port.egress_network_group_id = cint_vxlan_roo_basic_info.vxlan_network_group_id;
        rv = bcm_vlan_port_create(unit, vlan_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_port_create \n", proc_name);
            return rv;
        }
    }

    return rv;
}

/*
 * Create native multicast group and add vlan port and VXLAN tunnel to it
 */
int vxlan_roo_mc_add_native_mc(int unit)
{
    int rv;
    int idx;
    uint32 flags;
    bcm_if_t encap_rif[4];
    bcm_if_t encap_lif[4];
    bcm_if_t ifs[2];
    bcm_if_t rep_idx = 0x300100;
    bcm_multicast_replication_t reps[4];
    bcm_ip6_t sip = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    bcm_multicast_replication_t_init(reps[0]);
    bcm_multicast_replication_t_init(reps[1]);

    bcm_multicast_replication_t_init(reps[2]);
    bcm_multicast_replication_t_init(reps[3]);

    /* Create multicast group */
    egress_mc = 1;
    rv = multicast__open_mc_group(unit, &g_vxlan_roo_mc.native_ipmc, 0);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in multicast__open_mc_group\n");
        return rv;
    }

    /** add provider mc entries (entries with 2 CUDS)
     *  and add access mc entries (entries with 1 CUD) **/

    /* IDX 0: add access mc entry: to router B */
    /* IDX 1: add access mc entry: to router E */
    for (idx = 0; idx < 2; idx++)
    {
        rv = bcm_multicast_l3_encap_get(unit, -1 /* not used */, -1 /* not used */,
                                   g_vxlan_roo_mc.access_eth_rif[idx], &(encap_rif[idx]));
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_l3_encap_get \n");
            return rv;
        }
        rv = bcm_multicast_vlan_encap_get(unit, -1 /* not used */, -1 /* not used */,
                                     g_vxlan_roo_mc.vlan_port_id[idx], &(encap_lif[idx]));
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_vlan_encap_get \n");
            return rv;
        }
        BCM_GPORT_LOCAL_SET(reps[idx].port, g_vxlan_roo_mc.access_port[idx]);
        reps[idx].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID | BCM_MUTICAST_REPLICATION_ENCAP1_L3_INTF;
    }

    /* IDX 2: add provider mc entry: to router D */
    /* IDX 3: add provider mc entry: to router C */
    for (idx = 2; idx < 4; idx++)
    {
        /* cud outRif */
        rv = bcm_multicast_l3_encap_get(unit, -1 /* not used */, -1 /* not used */,
                                        cint_vxlan_roo_basic_info.core_native_eth_rif, &(encap_rif[idx]));
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_l3_encap_get \n");
            return rv;
        }
        /* cud outLif */
        encap_lif[idx] =  BCM_L3_ITF_VAL_GET(cint_vxlan_roo_basic_info.core_overlay_encap_tunnel_intf);
        BCM_GPORT_LOCAL_SET(reps[idx].port, g_vxlan_roo_mc.provider_port[idx - 2]);
        reps[idx].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID | BCM_MUTICAST_REPLICATION_ENCAP1_L3_INTF;
    }

    flags = BCM_MULTICAST_EGRESS_GROUP;
    for (idx = 0; idx < 4; idx++)
        {
        reps[idx].encap1 = rep_idx++;

        /*JR2 use PP DB to carry 2 pointers*/
        BCM_L3_ITF_SET(ifs[0], BCM_L3_ITF_TYPE_RIF, encap_rif[idx]);
        BCM_L3_ITF_SET(ifs[1], BCM_L3_ITF_TYPE_LIF, encap_lif[idx]);
        rv = bcm_multicast_encap_extension_create(unit, BCM_MULTICAST_ENCAP_EXTENSION_WITH_ID, &(reps[idx].encap1), 2, ifs);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_multicast_encap_extension_create \n");
            return rv;
        }
        printf("PPMC: CUD=0x%x  -->  ITF0=0x%x, ITF1=0x%x\n", reps[idx].encap1, ifs[0], ifs[1]);
    }

    rv = bcm_multicast_set(unit, g_vxlan_roo_mc.native_ipmc, flags, 4, &reps[0]);
    if (rv != BCM_E_NONE) {
       printf("Error, bcm_multicast_set \n");
       return rv;
    }

    if (verbose >=2) {

        printf("multicast entries added to 0x%x \n", g_vxlan_roo_mc.native_ipmc);

        printf("mc entry with 2 cuds: outLif: 0x%x outRif: 0x%x port: 0x%x added to 0x%x \n", reps[0].encap2, reps[0].encap1, reps[0].port, g_vxlan_roo_mc.native_ipmc);

        printf("mc entry with 2 cuds: outLif: 0x%x outRif: 0x%x port: 0x%x added to 0x%x \n", reps[1].encap2, reps[1].encap1, reps[1].port, g_vxlan_roo_mc.native_ipmc);

        printf("mc entry with 2 cuds: outLif: 0x%x outRif: 0x%x port: 0x%x added to 0x%x \n", reps[2].encap2, reps[2].encap1, reps[2].port, g_vxlan_roo_mc.native_ipmc);

        printf("mc entry with 2 cuds: outLif: 0x%x outRif: 0x%x port: 0x%x added to 0x%x \n", reps[3].encap2, reps[3].encap1, reps[3].port, g_vxlan_roo_mc.native_ipmc);

    }

    rv = multicast__open_fabric_mc_or_ingress_mc_for_egress_mc(unit, g_vxlan_roo_mc.native_ipmc, g_vxlan_roo_mc.native_ipmc);
    if (rv != BCM_E_NONE) {
       printf("Error, multicast__open_ingress_mc_group_for_egress_mc \n");
       return rv;
    }

    /* Add IPv4 IPMC */
    rv = add_ipv4_ipmc(unit, g_vxlan_roo_mc.native_mc_dip, g_vxlan_roo_mc.native_mc_dip_mask, 0, 0,
                       0, 0, g_vxlan_roo_mc.native_ipmc, 0,0);
    if (rv != BCM_E_NONE) {
        printf("Error, in add_ipv4_ipmc IPv4\n");
        return rv;
    }

    /* Add IPv6 IPMC */
    rv = add_ipv6_ipmc(unit, g_vxlan_roo_mc.native_mc_dip6, g_vxlan_roo_mc.native_mc_dip6_mask, sip, sip,
                       0, cint_vxlan_roo_basic_info.access_eth_vrf, 0, 0, g_vxlan_roo_mc.native_ipmc, 0, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in add_ipv6_ipmc\n");
        return rv;
    }

    /* IPv6 MC for 2nd pass (drop and continue) */
    rv = add_ipv6_ipmc(unit, g_vxlan_roo_mc.native_mc_dip6, g_vxlan_roo_mc.native_mc_dip6_mask, sip, sip,
                       0, cint_vxlan_roo_basic_info.core_overlay_eth_vrf, 0, 0, g_vxlan_roo_mc.native_ipmc, 0, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in add_ipv6_ipmc\n");
        return rv;
    }

    return rv;
}

/*
 * Create overlay multicast group
 *   For bud node, add local copy to provider and recycle copy to it
 *   For leaf node, add local copies to access
 */
int vxlan_roo_mc_add_overlay_mc(int unit, int is_leaf_node)
{
    int rv;
    uint32 flags;
    bcm_multicast_replication_t replications[2];
    bcm_ip6_t sip = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    /* Create multicast group */
    egress_mc = 0;
    rv = multicast__open_mc_group(unit, &g_vxlan_roo_mc.overlay_ipmc, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error: in multicast__open_mc_group\n");
        return rv;
    }

    if (!is_leaf_node) {
        bcm_multicast_replication_t_init(&replications[0]);
        replications[0].encap1 = cint_vxlan_roo_basic_info.core_overlay_eth_rif;
        replications[0].port = g_vxlan_roo_mc.provider_port[1];

        bcm_multicast_replication_t_init(&replications[1]);
        replications[1].encap1 = g_vxlan_roo_mc.recycle_entry_encap_id & 0x3fffff;
        replications[1].port = g_vxlan_roo_mc.recycle_port;

        rv = bcm_multicast_add(unit, g_vxlan_roo_mc.overlay_ipmc, BCM_MULTICAST_INGRESS_GROUP, 2, replications);
        if (rv != BCM_E_NONE)
        {
          printf("Error, in bcm_multicast_add mc_group_id:  0x%08x  port:  0x%08x  encap_id:  0x%08x \n", g_vxlan_roo_mc.overlay_ipmc, g_vxlan_roo_mc.recycle_port, g_vxlan_roo_mc.recycle_entry_encap_id);
          return rv;
        }
    }

    /* Add IPv4 IPMC */
    rv = add_ipv4_ipmc(unit, g_vxlan_roo_mc.overlay_mc_dip, g_vxlan_roo_mc.overlay_mc_dip_mask, 0, 0,
                       cint_vxlan_roo_basic_info.core_overlay_eth_rif, cint_vxlan_roo_basic_info.core_overlay_eth_vrf, g_vxlan_roo_mc.overlay_ipmc, 0,0);
    if (rv != BCM_E_NONE) {
        printf("Error, in add_ipv4_ipmc IPv4\n");
        return rv;
    }

    /* Add IPv6 IPMC */
    rv = add_ipv6_ipmc(unit, g_vxlan_roo_mc.overlay_mc_dip6, g_vxlan_roo_mc.overlay_mc_dip6_mask, sip, sip,
                       0, cint_vxlan_roo_basic_info.core_overlay_eth_vrf, 0, 0, g_vxlan_roo_mc.overlay_ipmc, 0, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in add_ipv6_ipmc\n");
        return rv;
    }

    return rv;
}

/* disable split horizon per lif orientation (default)
 */
int vxlan_roo_mc_disable_split_horizon(int unit) {

    int rv;

    bcm_switch_network_group_t inlif_forwarding_group = 1;

    bcm_switch_network_group_config_t group_config;

    bcm_switch_network_group_config_t_init(&group_config);

    group_config.dest_network_group_id = 1; /* outlif forwarding group */
    group_config.config_flags = 0; /* disable filter */

    rv = bcm_switch_network_group_config_set(unit, inlif_forwarding_group, &group_config);

    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_network_group_config_set\n");
    }

    return rv;
}

/*
 * The basic VXLAN example with parameters:
 * Unit - relevant unit
 * access_port - port where core host is connected to.
 * provider_port - port where DC Fabric router is connected to.
 */
int vxlan_roo_mc_example(
    int unit,
    int is_leaf_node,
    int access_port1,
    int access_port2,
    int provider_port1,
    int provider_port2)
{
    int rv;
    int term_tunnel_id;
    int encap_tunnel_id;
    bcm_gport_t vxlan_port_id;
    char *proc_name;
    uint32 ibch1_supported = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL);
    proc_name = "vxlan_roo_mc_example";

    /* Init parameters */
    rv = vxlan_roo_mc_init(unit, is_leaf_node, access_port1, access_port2, provider_port1, provider_port2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_roo_mc_init \n", proc_name);
        return rv;
    }

    /*
     * Build L2 VPN
     */
    rv = vxlan_roo_basic_vxlan_open_vpn(unit, cint_vxlan_roo_basic_info.vpn_id, cint_vxlan_roo_basic_info.vni);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_roo_basic_vxlan_open_vpn, vpn=%d, \n", proc_name, cint_vxlan_roo_basic_info.vpn_id);
        return rv;
    }

    /*
     * Set the split horizon
     */
    rv = vxlan_roo_mc_split_horizon_set(unit, cint_vxlan_roo_basic_info.vpn_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vxlan_split_horizon_set\n", proc_name);
        return rv;
    }

    /*
     * Configure port properties
     */
    rv = vxlan_roo_mc_configure_port_properties(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_roo_mc_configure_port_properties \n", proc_name);
        return rv;
    }

    /*
     * Create recycle entry
     */
    rv = create_recycle_entry_with_defaults(unit, &g_vxlan_roo_mc.recycle_entry_encap_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create_recycle_entry_with_defaults \n", proc_name);
        return rv;
    }

    /*
     * Configure ingress interfaces
     */
    rv = vxlan_roo_mc_open_route_interfaces_access(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_roo_mc_open_route_interfaces_access \n", proc_name);
        return rv;
    }

    /*
     * Configure egress interfaces
     */
    rv = vxlan_roo_mc_open_route_interfaces_provider(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_roo_mc_open_route_interfaces_provider \n", proc_name);
        return rv;
    }

    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    rv = vxlan_tunnel_terminator_create(unit, &term_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function vxlan_tunnel_terminator_create \n", proc_name);
        return rv;
    }

    /*
     *Configure ARP entries
     */
    rv = vxlan_roo_basic_create_arp_entries(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_roo_basic_create_arp_entries\n", proc_name);
        return rv;
    }

    /*
     * Create the tunnel initiator
     */
    rv = vxlan_tunnel_initiator_create(unit, cint_vxlan_roo_basic_info.core_overlay_arp_id, &(cint_vxlan_roo_basic_info.core_overlay_encap_tunnel_intf), &encap_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function tunnel_initiator_configure \n", proc_name);
        return rv;
    }

    /*
     * Configure fec entry
     */
    rv = vxlan_roo_basic_create_fec_entries(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_roo_basic_create_fec_entries\n", proc_name);
        return rv;
    }

    /*
     * Add VXLAN port properties and configure the tunnel terminator In-Lif and tunnel initiatior Out-Lif
     */
    /* leaf node tunnel termination in one pass */
    if (is_leaf_node) {
        rv = vxlan_port_add(unit, provider_port1, term_tunnel_id, &vxlan_port_id);
    } else {
        rv = vxlan_port_add(unit, g_vxlan_roo_mc.recycle_port, term_tunnel_id, &vxlan_port_id);
    }
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_port_add \n", proc_name);
        return rv;
    }

    cint_vxlan_roo_basic_info.vxlan_port_id = vxlan_port_id;

    /*
     * Configure native IPMC
     */
    rv = vxlan_roo_mc_add_native_mc(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_roo_mc_add_native_mc \n", proc_name);
        return rv;
    }

    /* configure recycle port for 2nd pass*/
    if (!is_leaf_node && !ibch1_supported) {
        rv = bcm_port_control_set(unit,g_vxlan_roo_mc.recycle_port, bcmPortControlOverlayRecycle, 1);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error, bcm_port_control_set\n", proc_name);
            return rv;
        }
    }

    /*
     * Configure overlay IPMC
     */
    rv = vxlan_roo_mc_add_overlay_mc(unit, is_leaf_node);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_roo_mc_add_overlay_mc \n", proc_name);
        return rv;
    }

    return rv;
}
