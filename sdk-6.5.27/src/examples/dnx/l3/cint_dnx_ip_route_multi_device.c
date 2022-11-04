/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Multi-Dev IPv4 UC~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * File: cint_dnx_ip_route_multi_device.c
 * Purpose: Demo IP route UC under multi device system
 *
 * Multi-Device system require the ingress/egress configuration separately and relatively.
 * This cint demo how to configurer the IPvx service under multi-device system.
 *  1. Create ARP and out-RIF configuration on egress device
 *  2. Create In-RIF and Forward configuration on ingress device
 *  3. Send IPv4 packet to verify the result

 * Example:
 * Test Scenario
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_l3.c
  cint ../../../../src/examples/dnx/l3/cint_ip_route_basic.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_multi_device.c
  cint
  multi_dev_ipv4_uc_example(0,0,1811939531,1811939530);
 * Send packets on unit 1 should be: 
  tx 1 psrc=203 data=0x0000000000110000000000008100706408004500004c00000000403dfb71000000007fffff04000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000
 *
 * Received packets on unit 2 should be: 
 *  Data: 0x00000022000000000011810000c808004500004c000000003f3dfc71000000007fffff04000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b000000000000 
 * Test Scenario - end
 */

struct cint_multi_dev_ip_route_info_s
{
    int in_rif;                             /* access RIF */
    int out_rif;                            /* out RIF */
    int vrf;                                /*  VRF */
    bcm_mac_t my_mac;                       /* mac for Incoming L3 intf */
    bcm_mac_t next_hop_mac;                 /* next hop mac for outgoing station */
    bcm_ip_t dip;                           /* dip */

    int arp_encap_id;                       /*Next hop intf ID*/
    int level1_fec_id;                      /*FEC ID for routing into IP*/

    /*
     * FEC usage
     * 0:1 level FEC with OutRIF+ARP+Dest (Format A)
     * 1:1 level FEC with Out-LIF+Dest (Format B)
     */
    int fec_mode;
    int in_local_port;
    int out_local_port;
    int mtu;
};

cint_multi_dev_ip_route_info_s cint_multi_dev_ip_route_info=
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
    * dip
    */
    0x7FFFFF04, /* 127.255.255.4 */

};

int default_mtu = 0;
int urpf_mode = bcmL3IngressUrpfDisable;
int l3_fec_cascade = 0;
int ip_route_fec_with_id = 0;
int lpm_raw_data_enabled =0;         /* use raw data payload in KAPs LPM */
int fec_id_used = 0;
int lpm_payload = 0x1234;            /* raw payload in Kaps*/
int lpm_raw_new_dst_port;

/**
 * Convert system port to local port
*/

int
multi_dev_system_port_to_local(int unit, int sysport, int *local_port){

    BCM_IF_ERROR_RETURN_MSG(bcm_port_local_get(unit, sysport, local_port), "core_port");

    return BCM_E_NONE;
}

/**
 * Main entrance for mpls check ver configuration
 * INPUT:
 *   ingress_unit- traffic incoming unit
 *   egress_unit- traffic ougtoing unit
 *   in_sys_port- traffic incoming port
 *   out_sys_port- traffic outgoing port
*/
int
multi_dev_ip_route_init(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{
    int local_port,is_local;
    int egress_trunk_id;
    int egress_trunk_group;
    int egress_trunk_pool;

    /*
     * 0: Convert the sysport to local port
     */

    if (!BCM_GPORT_IS_TRUNK(in_sys_port)) {
        multi_dev_system_port_to_local(ingress_unit,in_sys_port,&cint_multi_dev_ip_route_info.in_local_port);
        multi_dev_system_port_to_local(egress_unit,out_sys_port,&cint_multi_dev_ip_route_info.out_local_port);
    } else {
        cint_multi_dev_ip_route_info.in_local_port = in_sys_port;
        BCM_TRUNK_ID_GROUP_GET(egress_trunk_group,out_sys_port);
        BCM_TRUNK_ID_POOL_GET(egress_trunk_pool,out_sys_port);
        BCM_TRUNK_ID_SET(egress_trunk_id, egress_trunk_pool, egress_trunk_group);
        BCM_GPORT_TRUNK_SET(cint_multi_dev_ip_route_info.out_local_port, egress_trunk_id);
    }

    /*
     * 1.EncapID, use the minimal valid
     */
    BCM_L3_ITF_SET(cint_multi_dev_ip_route_info.arp_encap_id,BCM_L3_ITF_TYPE_LIF, 0x4000);

    /*
     *2.FEC ID
     */
    int fec_id;
    if (l3_fec_cascade == BCM_L3_2ND_HIERARCHY ) {
        BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(ingress_unit, 1, 0, &fec_id), "");
    } else if (l3_fec_cascade == BCM_L3_3RD_HIERARCHY) {
        BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(ingress_unit, 2, 0, &fec_id), "");
    } else {
        BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(ingress_unit, 0, 0, &fec_id), "");
    }

    BCM_L3_ITF_SET(cint_multi_dev_ip_route_info.level1_fec_id, BCM_L3_ITF_TYPE_FEC, fec_id);

    return BCM_E_NONE;
}

/**
 * port init
 * INPUT:
 *   in_port - traffic incoming port 
*/
int
multi_dev_ingress_port_configuration(
    int unit,
    int in_port,
    int in_rif)
{

    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port, in_rif), "");

    return BCM_E_NONE;
}

/**
 * port egress init
 * INPUT:
 *   out_port - traffic outgoing port
*/
int
multi_dev_egress_port_configuration(
    int unit,
    int out_port)
{
    char error_msg[200]={0,};

    /*
     * 1. Set Out-Port default properties, in case of ARP+AC no need
     */
    snprintf(error_msg, sizeof(error_msg), "out_port %d", out_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port), error_msg);

    return BCM_E_NONE;
}

/**
 * L3 intf init
 * INPUT:
 *   in_port - traffic incoming port
*/
int
multi_dev_ip_route_l3_intf_configuration(
    int unit)
{
    int rv;
    bcm_l3_intf_t l3_intf;
    dnx_utils_l3_eth_rif_s eth_rif_structure;

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_intf_id = cint_multi_dev_ip_route_info.in_rif;
    rv = bcm_l3_intf_get(unit,&l3_intf);
    if (rv != BCM_E_NONE) {
        dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_multi_dev_ip_route_info.in_rif, 0, 0, cint_multi_dev_ip_route_info.my_mac, cint_multi_dev_ip_route_info.vrf);
        if (default_mtu !=0) {
            eth_rif_structure.mtu_valid  = 1;
            eth_rif_structure.mtu  = default_mtu;
        }
        eth_rif_structure.urpf_mode = urpf_mode;
        BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_eth_rif_create(unit, &eth_rif_structure), "core_native_eth_rif");
    }

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_intf_id = cint_multi_dev_ip_route_info.out_rif;
    rv = bcm_l3_intf_get(unit,&l3_intf);
    if (rv != BCM_E_NONE) {
        dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_multi_dev_ip_route_info.out_rif, 0, 0, cint_multi_dev_ip_route_info.my_mac, cint_multi_dev_ip_route_info.vrf);
        if (default_mtu !=0) {
            eth_rif_structure.mtu_valid  = 1;
            eth_rif_structure.mtu  = default_mtu;
        }
        eth_rif_structure.urpf_mode = urpf_mode;
        BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_eth_rif_create(unit, &eth_rif_structure), "core_native_eth_rif");

    }

    return BCM_E_NONE;
}

/**
 * ARP adding
 * INPUT:
 *   in_port - traffic incoming port 
 *   out_port - traffic outgoing port
*/
int
multi_dev_ip_route_arp_configuration(
    int unit)
{

    dnx_utils_l3_arp_s arp_structure;

    /*
     * Configure ARP entry
     */
    if (cint_multi_dev_ip_route_info.mtu)
    {
        /*
         * For JR2, set l3eg.mtu as none-zero, so the result type of eedb_arp has mtu_profile field 
         */    
        dnx_utils_l3_arp_s_common_init(unit, 0, &arp_structure, cint_multi_dev_ip_route_info.arp_encap_id, 0, 0 , cint_multi_dev_ip_route_info.next_hop_mac, cint_multi_dev_ip_route_info.out_rif);
        arp_structure.mtu = cint_multi_dev_ip_route_info.mtu;
    }
    else
    {
        dnx_utils_l3_arp_s_common_init(unit, 0, &arp_structure, cint_multi_dev_ip_route_info.arp_encap_id, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION , cint_multi_dev_ip_route_info.next_hop_mac, cint_multi_dev_ip_route_info.out_rif); 
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_arp_create(unit, &arp_structure), "create AC egress object ARP only");

    return BCM_E_NONE;
}

/**
 * FEC entry adding
 * INPUT:
 *   in_port    - traffic incoming port 
 *   out_sys_port - traffic outgoing port
*/
int
multi_dev_ip_route_fec_entry_configuration(
    int unit,
    int out_sys_port)
{
    dnx_utils_l3_fec_s fec_structure;


    dnx_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    fec_structure.destination = out_sys_port;

    switch (cint_multi_dev_ip_route_info.fec_mode)
    {
        case 0:
            fec_structure.tunnel_gport = cint_multi_dev_ip_route_info.out_rif;
            fec_structure.tunnel_gport2 = cint_multi_dev_ip_route_info.arp_encap_id;
        break;

        case 1:
            fec_structure.tunnel_gport = cint_multi_dev_ip_route_info.arp_encap_id;
        break;
    }

    fec_structure.fec_id = cint_multi_dev_ip_route_info.level1_fec_id;
    fec_structure.allocation_flags = BCM_L3_WITH_ID;

    if (l3_fec_cascade == BCM_L3_2ND_HIERARCHY || l3_fec_cascade == BCM_L3_3RD_HIERARCHY) {
        fec_structure.flags |= l3_fec_cascade;
        printf("FEC's will be hierarchical\n");
    }

    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
       fec_structure.flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_fec_create(unit, &fec_structure), "create egress object FEC only");

    return BCM_E_NONE;
}

/**
 * forward entry adding(ILM, IPv4 Route)
 * INPUT:
 *   unit - traffic incoming unit 
*/
int
multi_dev_ip_route_forward_entry_configuration(
    int unit)
{
    BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, cint_multi_dev_ip_route_info.dip, cint_multi_dev_ip_route_info.vrf, cint_multi_dev_ip_route_info.level1_fec_id, 0, 0), "");

    return BCM_E_NONE;
}

/**
 * ingress side configuration
 * INPUT:
 *   in_port - traffic incoming port 
 *   out_sys_port - traffic outgoing port
*/
int
multi_dev_ip_route_ingress_configuration(
    int ingress_unit,
    int in_port,
    int out_sys_port)
{

    BCM_IF_ERROR_RETURN_MSG(multi_dev_ingress_port_configuration(ingress_unit,in_port, cint_multi_dev_ip_route_info.in_rif), "");

    BCM_IF_ERROR_RETURN_MSG(multi_dev_ip_route_l3_intf_configuration(ingress_unit), "");

    BCM_IF_ERROR_RETURN_MSG(multi_dev_ip_route_fec_entry_configuration(ingress_unit,out_sys_port), "");

    BCM_IF_ERROR_RETURN_MSG(multi_dev_ip_route_forward_entry_configuration(ingress_unit), "");

    return BCM_E_NONE;
}

/**
 * egress side configuration
 * INPUT:
 *   out_port - traffic outgoing port
*/
 int
 multi_dev_ip_route_egress_configuration(
    int egress_unit,
    int out_port)
{

    BCM_IF_ERROR_RETURN_MSG(multi_dev_egress_port_configuration(egress_unit,out_port), "");

    BCM_IF_ERROR_RETURN_MSG(multi_dev_ip_route_l3_intf_configuration(egress_unit), "");

    BCM_IF_ERROR_RETURN_MSG(multi_dev_ip_route_arp_configuration(egress_unit), "");

    return BCM_E_NONE;
}

/**
 * Main entrance for IPv4 UC on multi-device
 * INPUT:
 *   in_sys_port - traffic incoming port 
 *   out_sys_port - traffic outgoing port
*/
 int
 multi_dev_ipv4_uc_example(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{

    BCM_IF_ERROR_RETURN_MSG(multi_dev_ip_route_init(ingress_unit,egress_unit,in_sys_port,out_sys_port), "");

    BCM_IF_ERROR_RETURN_MSG(multi_dev_ip_route_ingress_configuration(ingress_unit,cint_multi_dev_ip_route_info.in_local_port,out_sys_port), "");

    BCM_IF_ERROR_RETURN_MSG(multi_dev_ip_route_egress_configuration(egress_unit,cint_multi_dev_ip_route_info.out_local_port), "");

    return BCM_E_NONE;
}

int
basic_example_multi_units (
    int ingress_unit,
    int egress_unit,
    int in_sysport,
    int out_sysport)
{
    int rv;
    int ing_intf_in;
    int ing_intf_out;
    int fec[2] = {0, 0};
    int flags = 0;
    int flags2 = 0;
    int in_vlan = 15;
    int out_vlan = 100;
    int vrf = 0;
    int host;
    int encap_id[2]={0x2002, 0x2004};
    int open_vlan = 1;
    int route;
    int mask;
    int result = 0;
    bcm_mac_t mac_address  = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };  /* my-MAC */
    bcm_mac_t next_hop_mac  = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d }; /* next_hop_mac1 */
    bcm_mac_t next_hop_mac2  = { 0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d }; /* next_hop_mac2 */
    int units_ids[2] = {ingress_unit, egress_unit};
    char error_msg[200]={0,};
    bcm_vlan_control_vlan_t control_vlan;
    dnx_utils_l3_eth_rif_s eth_rif_structure;
    dnx_utils_l3_arp_s l3_arp;
    dnx_utils_l3_fec_s l3_fec;
    dnx_utils_l3_route_ipv4_s route_ipv4_structure;
    int sysport_is_local = 0;
    int local_port = 0;
    bcm_gport_t dst_gport;

    if (lpm_raw_data_enabled) {
        vrf = 1;
        fec[1] = lpm_payload;
    }

    BCM_IF_ERROR_RETURN_MSG(multi_dev_ingress_port_configuration(ingress_unit, in_sysport, in_vlan), "for ingress_unit");

    BCM_IF_ERROR_RETURN_MSG(multi_dev_egress_port_configuration(egress_unit, out_sysport), "for egress_unit");

    rv = bcm_vlan_create(ingress_unit, in_vlan);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("Error, create vlan=%d, in unit %d \n", in_vlan, ingress_unit);
        return rv;
    }

    /* Set VLAN with MC */
    snprintf(error_msg, sizeof(error_msg), "fail get control vlan(%d)", in_vlan);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_control_vlan_get(ingress_unit, in_vlan, &control_vlan), error_msg);
    control_vlan.unknown_unicast_group = control_vlan.unknown_multicast_group = control_vlan.broadcast_group = 0x1;
    snprintf(error_msg, sizeof(error_msg), "fail set control vlan(%d)", in_vlan);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_control_vlan_set(ingress_unit, in_vlan, control_vlan), error_msg);

    rv = bcm_vlan_create(egress_unit, out_vlan);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("Error, create vlan=%d, in unit %d \n", out_vlan, egress_unit);
        return rv;
    }

    /* Set VLAN with MC */
    snprintf(error_msg, sizeof(error_msg), "fail get control vlan(%d)", out_vlan);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_control_vlan_get(egress_unit, out_vlan, &control_vlan), error_msg);
    control_vlan.unknown_unicast_group = control_vlan.unknown_multicast_group = control_vlan.broadcast_group = 0x1;
    snprintf(error_msg, sizeof(error_msg), "fail set control vlan(%d)", out_vlan);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_control_vlan_set(egress_unit, out_vlan, control_vlan), error_msg);

    /*** create ingress router interface ***/
    if(*dnxc_data_get(ingress_unit, "l3", "feature", "public_routing_support", NULL))
    {
        flags = (vrf == 0) ? BCM_L3_INGRESS_GLOBAL_ROUTE : 0;
    }
    else
    {
        flags = 0;
    }
    dnx_utils_l3_eth_rif_s_common_init(ingress_unit, 0, &eth_rif_structure, in_vlan, 0, flags, mac_address, vrf);
    eth_rif_structure.urpf_mode = urpf_mode;
    eth_rif_structure.mtu_valid  = 1;
    eth_rif_structure.mtu  = default_mtu;
    BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_eth_rif_create(ingress_unit, &eth_rif_structure), "core_native_eth_rif");
    ing_intf_in = eth_rif_structure.l3_rif;

    if(*dnxc_data_get(egress_unit, "l3", "feature", "public_routing_support", NULL))
    {
        flags = (vrf == 0) ? BCM_L3_INGRESS_GLOBAL_ROUTE : 0;
    }
    else
    {
        flags = 0;
    }
    /*** create egress router interface ***/
    dnx_utils_l3_eth_rif_s_common_init(egress_unit, 0, &eth_rif_structure, out_vlan, 0, flags, mac_address, vrf);
    eth_rif_structure.urpf_mode = urpf_mode;
    eth_rif_structure.mtu_valid  = 1;
    eth_rif_structure.mtu  = default_mtu;
    BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_eth_rif_create(egress_unit, &eth_rif_structure), "core_native_eth_rif");
    ing_intf_out = eth_rif_structure.l3_rif;

    /*** create egress object 1 ***/
    /* out_sysport unit is already first */
    /* Create ARP and set its properties */
    dnx_utils_l3_arp_s_common_init(egress_unit, 0, &l3_arp, encap_id[0], 0, 0, next_hop_mac, out_vlan);
    BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_arp_create(egress_unit, &l3_arp), "create egress object ARP only");
    encap_id[0] = l3_arp.l3eg_arp_id;

    /* Create FEC and set its properties */
    dnx_utils_l3_fec_s_init(ingress_unit, 0x0, &l3_fec);
    l3_fec.destination = out_sysport;
    l3_fec.tunnel_gport = ing_intf_out;
    l3_fec.tunnel_gport2 = encap_id[0];
    l3_fec.allocation_flags = 0;
    if (ip_route_fec_with_id != 0) {
        l3_fec.allocation_flags |= BCM_L3_WITH_ID;
        fec[0] = ip_route_fec_with_id;
    }
    l3_fec.fec_id = fec[0];
    BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_fec_create(ingress_unit, &l3_fec), "create egress object FEC only");
    fec[0] = l3_fec.l3eg_fec_id;

    if(verbose >= 1) {
        printf("created FEC-id =0x%08x, in unit %d \n", fec[0], ingress_unit);
        printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[0], ingress_unit);
    }

    if (!lpm_raw_data_enabled) {
        /*** add host point to FEC ***/
        /*In JR2, if public entries were used in host table,
          the user should consider move those entries to the LPM table */
        flags = 0;
        if (is_jericho2_kbp_ipv6_split_rpf_enabled(ingress_unit) 
            || (*dnxc_data_get(ingress_unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)))
        {
            flags2 = BCM_L3_FLAGS2_FWD_ONLY;
        }
        else
        {
            flags2 = 0;
        }
        host = 0x7fffff03;
        route = 0x7fffff03;
        mask = 0xffffffff;
        /* Units order does not matter */
        dnx_utils_l3_route_ipv4_s_common_init(ingress_unit, 0, &route_ipv4_structure, route, mask, vrf, flags, flags2, fec[0]);
        BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_route_ipv4_add(ingress_unit, &route_ipv4_structure), "");
    }

    /*** create egress object 2***/
    /* out_sysport unit is already first */
    /* Create ARP and set its properties */
    dnx_utils_l3_arp_s_common_init(egress_unit, 0, &l3_arp, encap_id[1], 0, 0, next_hop_mac2, out_vlan);
    BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_arp_create(egress_unit, &l3_arp), "create egress object ARP only");
    encap_id[1] = l3_arp.l3eg_arp_id;

    /* Create FEC and set its properties */
    dnx_utils_l3_fec_s_init(ingress_unit, 0x0, &l3_fec);
    l3_fec.destination = out_sysport;
    l3_fec.tunnel_gport = ing_intf_out;
    l3_fec.tunnel_gport2 = encap_id[1];
    l3_fec.fec_id = fec[1];
    l3_fec.allocation_flags = lpm_raw_data_enabled ? BCM_L3_WITH_ID : 0;
    BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_fec_create(ingress_unit, &l3_fec), "create egress object FEC only");
    fec[1] = l3_fec.l3eg_fec_id;
    fec_id_used = fec[1];

    if(verbose >= 1) {
        printf("created FEC-id =0x%08x, in unit %d \n", fec[1], ingress_unit);
        printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[1], ingress_unit);
    }

    /*** add route point to FEC2 ***/
    route = 0x7fffff00;
    mask  = 0xffff0000;
    flags2 = lpm_raw_data_enabled ? BCM_L3_FLAGS2_RAW_ENTRY : 0;
    if (is_jericho2_kbp_ipv4_split_rpf_enabled(ingress_unit))
    {
        flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    /* Units order does not matter*/
    flags = 0;
    if (is_jericho2_kbp_ipv4_split_rpf_enabled(ingress_unit) 
        || (*dnxc_data_get(ingress_unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)))
    {
        flags2 = BCM_L3_FLAGS2_FWD_ONLY;
    }
    else
    {
        flags2 = 0;
    }

    printf("fec used is  = 0x%x (%d)\n", fec[1], fec[1]);
    if (lpm_raw_data_enabled) {
        dnx_utils_l3_route_ipv4_s_common_init(ingress_unit, 0, &route_ipv4_structure, route, mask, vrf, flags, flags2, lpm_payload);
    } else {
        dnx_utils_l3_route_ipv4_s_common_init(ingress_unit, 0, &route_ipv4_structure, route, mask, vrf, flags, flags2, fec[1]);
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_route_ipv4_add(ingress_unit, &route_ipv4_structure), "");

    /* redirect the packet in the pmf acording to the raw payload received in the kaps*/
    if (lpm_raw_data_enabled) {
        if (!BCM_GPORT_IS_TRUNK(lpm_raw_new_dst_port)) {
            BCM_GPORT_LOCAL_SET(dst_gport, lpm_raw_new_dst_port);
        } else {
            dst_gport = lpm_raw_new_dst_port;
        }
        BCM_IF_ERROR_RETURN_MSG(dnx_kaps_raw_data_redirect(ingress_unit, lpm_payload, 0, 0, dst_gport), "");
    }

    return BCM_E_NONE;
}

/*
* Test Scenario
*
* Test Scenario - start
 cint ../../../../src/examples/dnx/utility/cint_dnx_utils_l3.c
 cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
 cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_multi_device.c
 cint
 mtu_check_setup(0,0,1811939528,1811939531,1500);
 exit;
*
 tx 1 psrc=200 data=0x00000000001100000700010081000064080045000079000000008000b17c0a0000057fffff04c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
*
*  Received packets on unit 0 should be:
*  Source port: 0, Destination port: 0
*  Data: 0x000000000022000000000011810000c8080045000079000000007f00b27c0a0000057fffff04c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
*
 cint
 mtu_check_setup_destroy(0);
 exit;
* Test Scenario - end
*/
int compressed_layer_type =  2 ;
/*
 * header configuration
 *   Create a compressed layer type
 */
int
mtu_check_header_code_enable (
    int unit,
    int header_code)
{
    bcm_switch_control_key_t  mtu_cfg_type;
    bcm_switch_control_info_t mtu_cfg_info;

    /* Creating a compressed layer type */
    mtu_cfg_type.type = bcmSwitchLinkLayerMtuFilter;
    mtu_cfg_type.index = header_code;

    mtu_cfg_info.value = compressed_layer_type;

    BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_indexed_set(unit, mtu_cfg_type, mtu_cfg_info), "");
    return BCM_E_NONE;
}

int
mtu_check_setup (
    int ing_unit,
    int egr_unit,
    int in_sysport,
    int out_sysport,
    int link_mtu)
{
    int unit;
    int i;
    int ipv4_layer_protocol;
    int header_code = bcmForwardingTypeIp4Ucast;

    default_mtu = link_mtu;
    for (i = 0 ; i < 2 ; i++)
    {
        if (i==0) {
            unit = ing_unit;
        } else {
            unit = egr_unit;
        }
        if(*(dnxc_data_get(unit, "trap", "etpp", "mtu_layer_to_compressed_layer_mapping_support", NULL)))
        {
            dnx_dbal_fields_enum_value_get(unit, "LAYER_TYPES", "IPV4", &ipv4_layer_protocol);

            /* Configure MTU filters */

            header_code = ipv4_layer_protocol;
            BCM_IF_ERROR_RETURN_MSG(mtu_check_header_code_enable(unit, header_code), "");
        }
    }


    /* Configure L3 interfaces */
    BCM_IF_ERROR_RETURN_MSG(multi_dev_ipv4_uc_example(ing_unit, egr_unit, in_sysport, out_sysport), "");

    return BCM_E_NONE;
}

int
mtu_check_setup_destroy (
    int unit)
{
    if (!(*(dnxc_data_get(unit, "trap", "etpp", "mtu_layer_to_compressed_layer_mapping_support", NULL))))
    {
        compressed_layer_type = bcmRxTrapMtuLayerTypeIpv4;
    }
    BCM_IF_ERROR_RETURN_MSG(mtu_check_etpp_rif_set(unit, 100/*out_rif*/, compressed_layer_type, 0/*mtu*/, 0/*is_set*/), "");
    return BCM_E_NONE;
}
