/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
/*
 * Example of a simple-router IPMC scenario
 * Test Scenario
 *
 * ./bcm.user
 * cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vlan_translate.c
 * cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_optimised.c
 * cint
 * dnx_ip_route_optimised(0,200,201);
 * exit;
 *
 * ETH-RIF packet
 * tx 1 psrc = 200 data = 01005e020202000007000100810000cc0800450000350000000080009743c0800101e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 */


/**
 * Utility function: create an ARP with
 * result type ETPS_RIF_ARP_AC_1TAG_STAT
 */
int create_arp_ac_1tag_stat(
        int unit,
        bcm_mac_t arp_next_hop_mac,
        int *encap_id,
        int vlan)
{
    int rv;
    uint32 flags2 = 0;
    char *proc_name;
    proc_name = "create_arp_ac_1tag_stat";
    printf("%s(): Enter. arp_nex_hop_mac %02X:%02X:%02X:%02X:%02X:%02X encap_id 0x%04X vlan %d\r\n",
            proc_name, arp_next_hop_mac[0], arp_next_hop_mac[1], arp_next_hop_mac[2],
            arp_next_hop_mac[3], arp_next_hop_mac[4], arp_next_hop_mac[5], *encap_id, vlan);

    flags2 |= BCM_L3_FLAGS2_EGRESS_STAT_ENABLE | BCM_L3_FLAGS2_VLAN_TRANSLATION;
    printf("%s(): Going to call l3__egress_only_encap__create_inner() with encap_id 0x%08X, flags2 0x%08X\n",
                proc_name, *encap_id, flags2);
    rv = l3__egress_only_encap__create_inner(unit, 0, encap_id, arp_next_hop_mac, vlan,
                                             0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n",proc_name);
        return rv;
    }

    /*
     * for arp+ac, bcm_vlan_port_translation_set is the API which sets the AC part in
     * the ARP+AC entry in EEDB
     */
    printf("%s(): Call bcm_vlan_port_translation_set()\n",proc_name);
    bcm_vlan_port_translation_t lif_translation_info;
    bcm_vlan_port_translation_t_init(&lif_translation_info);
    int arp_outlif;
    int vlan_edit_profile = 7;
    bcm_gport_t arp_ac_gport;
    int inner_vlan = 0;

    arp_outlif = BCM_L3_ITF_VAL_GET(*encap_id);
    BCM_GPORT_SUB_TYPE_L3_VLAN_TRANSLATION_SET(arp_ac_gport, arp_outlif);
    BCM_GPORT_VLAN_PORT_ID_SET(arp_ac_gport, arp_ac_gport);

    /*
    * Perform vlan editing:
    */
    rv = vlan_translate_ive_eve_translation_set(unit, arp_ac_gport,         /* lif  */
                                                        0x9100,               /* outer_tpid */
                                                        0x8100,               /* inner_tpid */
                                                        bcmVlanActionArpVlanTranslateAdd,  /* outer_action */
                                                        bcmVlanActionNone,  /* inner_action */
                                                        vlan,                 /* new_outer_vid */
                                                        inner_vlan,           /* new_inner_vid */
                                                        vlan_edit_profile,    /* vlan_edit_profile */
                                                        0,                    /* tag_format - must be untag! */
                                                        FALSE                 /* is_ive */
                                                        );

    /*
    * Check that the VLAN Edit info was set correctly:
    */
    bcm_vlan_port_translation_t lif_translation_info_get;
    lif_translation_info_get.flags = BCM_VLAN_ACTION_SET_EGRESS;
    lif_translation_info_get.gport = arp_ac_gport;

    lif_translation_info_get.new_outer_vlan = 0xFFFFFFFF;
    lif_translation_info_get.new_inner_vlan = 0xFFFFFFFF;
    lif_translation_info_get.vlan_edit_class_id = 0xFFFFFFFF;

    rv = bcm_vlan_port_translation_get(unit, &lif_translation_info_get);

    if (rv != BCM_E_NONE) {
        printf("%s(): Error, lif_translation_info_get return error = %d\n", proc_name, rv);
        return rv;
    }

    if (lif_translation_info_get.new_outer_vlan != vlan) {
        printf("%s(): Error, get.new_outer_vlan = %d  set.new_outer_vlan = %d\n", proc_name,
                lif_translation_info_get.new_outer_vlan, vlan);

        return BCM_E_PARAM;
    }

    if (lif_translation_info_get.new_inner_vlan != inner_vlan) {
        printf("%s(): Error, get.new_inner_vlan = %d  set.new_inner_vlan = %d\n", proc_name,
                lif_translation_info_get.new_inner_vlan, inner_vlan);

        return BCM_E_PARAM;
    }

    if (lif_translation_info_get.vlan_edit_class_id != vlan_edit_profile) {
        printf("%s(): Error, get.vlan_edit_class_id = %d  set.vlan_edit_class_id = %d\n", proc_name,
                lif_translation_info_get.vlan_edit_class_id, vlan_edit_profile);

        return BCM_E_PARAM;
    }

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

/*
 * Add IPv6 Host entry <vrf, addr> --> intf
 * - addr: IPv6 address 16 8-bit values
 * - vrf: VRF ID
 * - intf: egress object created using create_l3_egress (FEC)
 *
 */
int
add_host_ipv6_dest(
    int unit,
    bcm_ip6_t addr,
    int vrf,
    int intf,
    int encap_id,
    int destination)
{
    int rc;
    bcm_l3_host_t host;
    bcm_l3_host_t_init(host);

    host.l3a_ip6_addr = addr;
    host.l3a_vrf = vrf;
    host.l3a_intf = (intf != 0) ? intf : encap_id;
    host.encap_id = (intf != 0) ? encap_id : 0;
    host.l3a_flags = BCM_L3_IP6;
    if (destination)
    {
        host.l3a_port_tgid = destination;
    }
    rc = bcm_l3_host_add(unit, host);
    if (rc != BCM_E_NONE)
    {
        printf("bcm_l3_host_add failed: %x \n", rc);
    }
    return rc;
}


/*
 * packet will be routed from in_port to out-port
 *
 * Route:
 * packet to send:
 *  - in port = in_port
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x7fffff00-0x7fffff0f except 0x7fffff03
 * expected:
 *  - out port = out_port
 *  - vlan = 100.
 *  - DA = {0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}
 *  TTL decremented
 *
 */
/*
 * example:
 int unit = 0;
 int in_port = 200;
 int out_port = 201;
 */
int
dnx_ip_route_optimised(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    int intf_in = cint_ip_route_info.intf_in;           /* Incoming packet ETH-RIF */
    int intf_out = cint_ip_route_info.intf_out;         /* Outgoing packet ETH-RIF */
    int vrf = 1;
    int encap_id = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x384;
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC 00:0c:00:02:00:00*/
    bcm_mac_t intf_out_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x01 };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_ip6_t ipv6_host = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xff, 0x10};
    bcm_gport_t gport;
    uint32 host = 0x7fffff02;
    l3_ingress_intf_init(&ingress_rif);
    int vlan = cint_ip_route_info.intf_out;
    /*
     * used to pass fec in host.l3a_intf
     */
    int encoded_fec;

    char *err_proc_name;
    char *proc_name;

    proc_name = "dnx_basic_example";
    printf("%s(): Enter. in_port %d out_port %d\r\n", proc_name, in_port, out_port);

    intf_out_mac_address[0] = 0x00;
    intf_out_mac_address[1] = 0x12;
    intf_out_mac_address[2] = 0x34;
    intf_out_mac_address[3] = 0x56;
    intf_out_mac_address[4] = 0x78;
    intf_out_mac_address[5] = 0x9a;

    printf("encap_id 0x%08X\r\n", encap_id);

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, in_port, intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n", proc_name);
        return rv;
    }
    /** Configure low latency processing on packets coming on in_port */
    rv = bcm_port_control_set(unit, in_port, bcmPortControlLowLatencyPacketProcessing, 1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_port_control_set\n", proc_name);
        return rv;
    }

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set out_port %d\n", proc_name,out_port);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties.
     */
    rv = intf_eth_rif_create_vrf(unit, intf_in, 0, intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n", proc_name,intf_in);
        return rv;
    }

    rv = intf_eth_rif_create_vrf(unit, intf_out, 0, intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n", proc_name);
        return rv;
    }

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = intf_in;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create\n", proc_name);
        return rv;
    }

    /*
     * 5. Create ARP and set its properties
     */
    rv = create_arp_ac_1tag_stat(unit, arp_next_hop_mac, &encap_id, vlan);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create_arp_plus_ac_type\n", proc_name);
        return rv;
    }

    /*
     * 6. Add host entry
     */
    if (BCM_GPORT_IS_SET(out_port)) {
       gport = out_port;
    }
    else
    {
       BCM_GPORT_LOCAL_SET(gport, out_port);
    }
    printf("%s(): Adding IPv4 host entry with IP = 0x%08x\r\n",proc_name, host);
    rv = add_host_ipv4(unit, host, vrf, intf_out, encap_id /* arp id */ , gport);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv4(), \n",proc_name);
        return rv;
    }
    rv = add_host_ipv6_dest(unit, ipv6_host, vrf, intf_out, encap_id, gport);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv6_dest(), \n",proc_name);
        return rv;
    }

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}
