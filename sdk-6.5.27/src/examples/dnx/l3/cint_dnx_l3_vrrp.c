/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */


/*
 * This function configure IPv4 and IPv6 L3 forwarding entries using the same VSI.
 * The My MAC process should be done using the VRRP where each of the IP version should use a different VRID
 */
int
l3_vrrp_protocol_field_test(
    int unit,
    int in_port,
    int out_port)
{
    int intf_in = 15;           /* Incoming packet ETH-RIF */
    int intf_out = 100;         /* Outgoing packet ETH-RIF */
    char error_msg[200]={0,};
    int vrf = 1;
    int encap_id = 0x1384;         /* ARP-Link-layer (needs to be higher than 8192 for Jer Plus) */
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_gport_t gport;
    l3_ingress_intf ing_rif;
    l3_ingress_intf_init(&ing_rif);
    uint32 route = 0x7fffff00;
    uint32 mask = 0xfffffff0;
    uint32 fec_fwd_flags2;
    bcm_ip6_t ipv6_route = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x10 };
    bcm_ip6_t ipv6_mask =  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0 };
    l3_ingress_intf_init(&ingress_rif);
    int vlan = 100;
    int fec;
    int vrid_4 = 7;
    int vrid_6 = 8;
    /*
    * used to pass fec in host.l3a_intf
    */
    int encoded_fec;

    char *proc_name;

    proc_name = "l3_vrrp_protocol_field_test";

    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &fec), "");

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port, intf_in), "");
    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    snprintf(error_msg, sizeof(error_msg), "intf_out out_port %d", out_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port), error_msg);

    /*
     * 3. Create ETH-RIF and set its properties
     */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", intf_in);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_in, intf_in_mac_address), error_msg);

    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_out, intf_out_mac_address), "intf_out");
    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ing_rif.vrf = vrf;
    ing_rif.intf_id = intf_in;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ing_rif), "intf_out");

    /*
     * 5. Create ARP and set its properties
     */
    BCM_IF_ERROR_RETURN_MSG(create_arp_plus_ac_type(unit, 0, arp_next_hop_mac, &encap_id, vlan), "");

    /*
     * 6. Create FEC and set its properties
     * only in case the host format is not "no-fec"
     */
    fec_fwd_flags2 = (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) ? BCM_L3_FLAGS2_FWD_ONLY : 0;

    BCM_GPORT_LOCAL_SET(gport, out_port);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, fec, 0, encap_id, gport, 0, fec_fwd_flags2, 0, 0, &encoded_fec),
        "create egress object FEC only");

    /*
     * 7. Add forwarding entries to the IPv4 and IPv6 entry
     */

    BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, route, mask, vrf, fec), "");

    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, ipv6_route, ipv6_mask, vrf, fec), "");

    /*
     * 8. Add VRRP EXEM entries for the IPv4 and IPv6 with the same VSI but with different VRID
     */

    /* only IPv4, DMAC: 00:00:5e:00:01:07 */
    snprintf(error_msg, sizeof(error_msg), "VRID %d", vrid_4);
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_vrrp_config_add(unit, BCM_L3_VRRP_EXTENDED | BCM_L3_VRRP_IPV4, intf_in, vrid_4), error_msg);

    /* only IPv6, DMAC: 00:00:5e:00:02:08 */
    snprintf(error_msg, sizeof(error_msg), "VRID %d", vrid_6);
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_vrrp_config_add(unit, BCM_L3_VRRP_EXTENDED | BCM_L3_VRRP_IPV6, intf_in, vrid_6), error_msg);

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;

}


int fec;

/*  After adding vrrp mapping, use this function to map all traffic that
 *  has recieved on in_port, been l2 terminated and has vlan == in_vlan.
 *  Those packets will be forwarded to out_port.
 */
int create_traffic_mapping(int unit, int in_port, int out_port, int in_vlan, int ipv6_distinct){
    int rv;
    int ing_intf_in;
    int ing_intf_out;
    int out_vlan = 100;
    int vrf = 0;
    int encap_id;
    int route;
    int mask;
    bcm_ip6_t route6 = {0x01,0x00,0x16,0x00,0x35,0x00,0x70,0x00,0x00,0x00,0xdb,0x07,0x00,0x00,0x00,0x00};
    bcm_ip6_t mask6 = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac */
    int vrrp_flags;
    uint32 fec_flags2 = 0;

    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    /* create ingress router interface */
    rv = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
        return rv;
    }

    if (ipv6_distinct) {
        rv = vlan__open_vlan_per_mc(unit, in_vlan - 1, 0x1);
        if (rv != BCM_E_NONE) {
            printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
            return rv;
        }
    }

    rv = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_vlan_gport_add\n");
        return rv;
    }

    create_l3_intf_s intf;
    sal_memset(&intf, 0, sizeof(intf));
    intf.vsi = in_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;

    rv = dnx_utils_l3__intf_rif__create(unit, &intf);
    ing_intf_in = intf.rif;
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
        return rv;
    }

    if (ipv6_distinct) {
        intf.vsi = in_vlan  - 1;
        rv = dnx_utils_l3__intf_rif__create(unit, &intf);
        ing_intf_in = intf.rif;
        if (rv != BCM_E_NONE) {
            printf("Error, l3__intf_rif__create\n");
            return rv;
        }
    }

    /* create egress router interface */
    rv = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, out_vlan, in_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", in_port, out_vlan);
      return rv;
    }

    intf.vsi = out_vlan;

    rv = dnx_utils_l3__intf_rif__create(unit, &intf);
    ing_intf_out = intf.rif;
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
        return rv;
    }

    /* create egress object (ARP) */
    l3_egress_s l3eg2;
    dnx_utils_l3_egress_s_init(unit,0,&l3eg2);
    sal_memcpy(l3eg2.next_hop_mac_addr, next_hop_mac , 6);
    l3eg2.allocation_flags = BCM_L3_EGRESS_ONLY;
    l3eg2.vlan = out_vlan;
    l3eg2.arp_encap_id = encap_id;
    rv = dnx_utils_l3_egress_create(unit, &l3eg2);
    if (rv != BCM_E_NONE) {
        printf("Error, dnx_utils_l3_egress_create\n");
        return rv;
    }

    encap_id = l3eg2.arp_encap_id;
    /* create FEC object */
    dnx_utils_l3_egress_s_init(unit,0,&l3eg2);
    l3eg2.allocation_flags = BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID;
    l3eg2.out_tunnel_or_rif = ing_intf_out;
    l3eg2.out_gport = out_port;
    l3eg2.fec_id = fec;
    l3eg2.l3_flags2 = fec_flags2;
    l3eg2.arp_encap_id = encap_id;
    rv = dnx_utils_l3_egress_create(unit, &l3eg2);
    if (rv != BCM_E_NONE) {
        printf("Error, dnx_utils_l3_egress_create\n");
        return rv;
    }

    fec = l3eg2.fec_id;

    printf("created FEC-id =0x%08x, \n", fec);
    printf("next hop mac at encap-id %08x, \n", encap_id);

    /* add IPV4 route point to FEC -> out_port */
    route = 0x7fffff00;
    mask  = 0xfffffff0;

    rv = add_route_ipv4(unit, route, mask , vrf, fec);
    if (rv != BCM_E_NONE) {
        printf("Error, add_route_ipv4 \n");
        return rv;
    }
    print_route("add entry ", route,mask,vrf);
    printf("---> egress-object=0x%08x, port=%d, \n", fec, out_port);

    /* add IPV6 route point to FEC -> out_port */
    rv = add_route_ipv6(unit, route6, mask6, vrf, fec);
    if (rv != BCM_E_NONE) {
        printf("Error, add_route_ipv6 \n");
        return rv;
    }

    return rv;
}
