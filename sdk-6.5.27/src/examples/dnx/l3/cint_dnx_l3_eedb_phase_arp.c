 /* 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
/*
 *
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
 *    TTL decremented
 *
 */
/*
 * example:
 int unit = 0;
 int in_port = 200;
 int out_port = 201;
 */
int dnx_l3_eedb_phase_arp(
        int unit,
        int in_port,
        int out_port)
{
    char error_msg[200]={0,};
    int intf_in = 15;           /* Incoming packet ETH-RIF */
    int intf_out = 100;         /* Outgoing packet ETH-RIF */
    int fec = 0;
    int vrf = 1;
    int vlan = 100;
    int encoded_fec;
    int encap_id = 8193;
    int fec_flags2 = 0;
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x01 };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_gport_t gport;
    l3_ingress_intf_init(&ingress_rif);
    uint32 ipv4_dip = 0x7fffff02;
    char *proc_name;
    proc_name = "dnx_l3_eedb_phase_arp";
    /*
     * 1. Get a default first hierarchy FEC
     */
    BCM_IF_ERROR_RETURN_MSG(get_default_fec_id(unit,&fec), "");
    /*
     * 2. Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port, intf_in), "intf_in");
    /*
     * 3. Set Out-Port default properties
     */
    snprintf(error_msg, sizeof(error_msg), "intf_out out_port %d", out_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port), error_msg);
    /*
     * 4. Create ETH-RIF and set its properties
     */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", intf_in);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_in, intf_in_mac_address), error_msg);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_out, intf_out_mac_address), "intf_out");
    /*
     * 5. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = intf_in;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "intf_out");
    /*
    * 6. Create ARP and set its properties
    * Change the EEDB phase
    */
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;
    bcm_l3_egress_t_init(&l3eg);

    l3eg.mac_addr = arp_next_hop_mac;
    l3eg.encap_id = encap_id;
    /** set EEDB phase to AC*/
    l3eg.encap_access = bcmEncapAccessAc;
    l3eg.vlan = vlan;
    l3eg.flags = 0;
    l3eg.flags2 = 0;

    BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &l3egid_null), "create egress object");
    encap_id = l3eg.encap_id;
    /*
     * 7. Create FEC and set its properties
     */
    printf("%s(): Create main FEC and set its properties.\r\n",proc_name);
    BCM_GPORT_LOCAL_SET(gport, out_port);

    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, fec, 0, encap_id, gport, 0,
                                           fec_flags2, 0, 0, &encoded_fec), "create egress object FEC only");
    /*
     * 8. Add IPv4 route entry
     */
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, ipv4_dip, 0xffffffff, vrf, fec), "");

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}
