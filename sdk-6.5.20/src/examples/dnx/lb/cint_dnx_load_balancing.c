/*
 * $Id: cint_dnx_load_balancing.c
 * Exp $
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Example of a symmetrical hashing configuration.
 * Two packets are sent for every symmetrical packet header type - ETH, IPv4, IPv6, UDP, TCP.
 * The values of the two main fields of the header are switched and the LB_Key hashing value should remain the same.
 *
 * ./bcm.user
 * cint src/examples/sand/cint_sand_utils_global.c
 * cint src/examples/sand/cint_ip_route_basic.c
 * cint src/examples/sand/cint_dnx_load_balancing.c
 * cint
 * cint_dnx_lb_symmetrical_hashing(0,200,201,202);
 * exit;
 *
 * ETH1 packet (DA and SA field values are switched)
 * tx 1 psrc = 200 data = 0x000007000100000c0002000081000010ffff000102030405060708090a0b0c0d0e0f
 * Received packet data: 0x00000000cd1d000007000100810000640000000102030405060708090a0b0c0d0e0f
 *
 * tx 1 psrc = 200 data = 0x000c000200000000070001008100000fffff000102030405060708090a0b0c0d0e0f
 * Received packet data: 0x00000000cd1d000c00020000810000640000000102030405060708090a0b0c0d0e0f
 *
 *
 * IPv4oETH1 packet (DIP and SIP field values are switched)
 * tx 1 psrc = 200 data = 0x000c000200000000070001008100000f080045000035000000008000d7f6c0800101a0500101000102030405060708090a0b0c0d0e0f
 * Received packet data = 0x00000000cd1d00123456789a81000064080045000035000000007f00d8f6c0800101a0500101000102030405060708090a0b0c0d0e0f
 *
 * tx 1 psrc = 200 data = 0x000007000100000c0002000081000010080045000035000000008000d7f6a0500101c0800101000102030405060708090a0b0c0d0e0f
 * Received packet data = 0x00000000cd1d00123456789a81000064080045000035000000007f00d8f6a0500101c0800101000102030405060708090a0b0c0d0e0f
 *
 *
 * IPv6oETH1 packet (DIP and SIP field values are switched)
 * tx 1 psrc = 200 data = 0x000c000200000000070001008100000f86dd60000000004906800000000000001234000000000000c0ab000000000000000012340000ff00ff13000102030405060708090a0b0c0d0e0f
 * Received packet data = 0x00000000cd1d00123456789a8100006486dd600000000049067f0000000000001234000000000000c0ab000000000000000012340000ff00ff13000102030405060708090a0b0c0d0e0f
 *
 * tx 1 psrc = 200 data = 0x000007000100000c000200008100001086dd6000000000490680000000000000000012340000ff00ff130000000000001234000000000000c0ab000102030405060708090a0b0c0d0e0f
 * Received packet data = 0x00000000cd1d00123456789a8100006486dd600000000049067f000000000000000012340000ff00ff130000000000001234000000000000c0ab000102030405060708090a0b0c0d0e0f
 *
 *
 * UDPoIPv4oETH1 packet (SRC_PORT and DST_PORT field values are switched)
 * tx 1 psrc = 200 data = 0x000007000100000c000200008100001008004500003d000000008011d7dda0500101c080010101f4025800290000000102030405060708090a0b0c0d0e0f
 * Received packet data = 0x00000000cd1d00123456789a8100006408004500003d000000007f11d8dda0500101c080010101f4025800290000000102030405060708090a0b0c0d0e0f
 *
 * tx 1 psrc = 200 data = 0x000c000200000000070001008100000f08004500003d000000008011d7ddc0800101a0500101025801f400290000000102030405060708090a0b0c0d0e0f
 * Received packet data = 0x00000000cd1d00123456789a8100006408004500003d000000007f11d8ddc0800101a0500101025801f400290000000102030405060708090a0b0c0d0e0f
 *
 *
 * TCPoIPv4oETH1 packet (SRC_PORT and DST_PORT field values are switched)
 * tx 1 psrc = 200 data = 0x000c000200000000070001008100000f080045000049000000008006d7dcc0800101a0500101025801f400000000000000000010001000000000000102030405060708090a0b0c0d0e0f
 * Received packet data = 0x00000000cd1d00123456789a81000064080045000049000000007f06d8dcc0800101a0500101025801f400000000000000000010001000000000000102030405060708090a0b0c0d0e0f
 *
 * tx 1 psrc = 200 data = 0x000007000100000c0002000081000010080045000049000000008006d7dca0500101c080010101f4025800000000000000000010001000000000000102030405060708090a0b0c0d0e0f
 * Received packet data = 0x00000000cd1d00123456789a81000064080045000049000000007f06d8dca0500101c080010101f4025800000000000000000010001000000000000102030405060708090a0b0c0d0e0f
 */

/******* Run example ******/

/*
 * Verify packets are symmetrically hashed using packet headers L4oIPvXoETH, IPvXoETH, ETH
 * packets will be routed from in_port1/in_port2 to out_port
 *
 * _________________________________
 * ETH1 packet1 to send:
 *  - in port = in_port1
 *  - Vlan ID = 15
 *  - DA = 00:0c:00:02:00:00
 *  - SA = 00:00:07:00:01:00
 * expected:
 *  - out port = out_port
 *  - vlan = 100
 *  - DA = 00:0c:00:02:00:00
 *  - SA = 00:12:34:56:78:9A
 *
 * ETH1 packet 2 to send:
 *  - in port = in_port2
 *  - Vlan ID = 16
 *  - DA = SA from packet1
 *  - SA = DA from packet1
 * expected:
 *  - out port = out_port
 *  - vlan = 100
 *  - DA = SA from packet1
 *  - SA = 00:12:34:56:78:9A
 * It is expected that the hash key (LB_Key) will be the same for the two packets.
 *
 * _________________________________
 * IPv4oETH1 packet1 to send:
 *  - in port = in_port1
 *  - Vlan ID = 15
 *  - DA = 00:0c:00:02:00:00
 *  - SA = 00:00:07:00:01:00
 *  - DIP = 160.80.1.1
 *  - SIP = 192.128.1.1
 * expected:
 *  - out port = out_port
 *  - vlan = 100
 *  - DA = 00:00:00:00:CD:1D
 *  - SA = 00:12:34:56:78:9A
 *  - DIP = 160.80.1.1
 *  - SIP = 192.128.1.1
 *
 * IPv4oETH1 packet2 to send:
 *  - in port = in_port2
 *  - Vlan ID = 16
 *  - DA = SA from packet1
 *  - SA = DA from packet1
 *  - DIP = SIP from packet1
 *  - SIP = DIP from packet1
 * expected:
 *  - out port = out_port
 *  - vlan = 100
 *  - DA = 00:00:00:00:CD:1D
 *  - SA = 00:12:34:56:78:9A
 *  - DIP = SIP from packet1
 *  - SIP = DIP from packet1
 * It is expected that the hash key (LB_Key) will be the same for the two packets.
 *
 * _________________________________
 * IPv6oETH1 packet1 to send:
 *  - in port = in_port1
 *  - Vlan ID = 15
 *  - DA = 00:0c:00:02:00:00
 *  - SA = 00:00:07:00:01:00
 *  - DIP = 0000:0000:0000:0000:1234:0000:ff00:FF13
 *  - SIP = 0000:0000:0000:1234:0000:0000:0000:c0ab
 * expected:
 *  - out port = out_port
 *  - vlan = 100
 *  - DA = 00:00:00:00:CD:1D
 *  - SA = 00:12:34:56:78:9A
 *  - DIP = 0000:0000:0000:0000:1234:0000:ff00:FF13
 *  - SIP = 0000:0000:0000:1234:0000:0000:0000:c0ab
 *
 * IPv6oETH1 packet2 to send:
 *  - in port = in_port2
 *  - Vlan ID = 16
 *  - DA = SA from packet1
 *  - SA = DA from packet1
 *  - DIP = SIP from packet1
 *  - SIP = DIP from packet1
 * expected:
 *  - out port = out_port
 *  - vlan = 100
 *  - DA = 00:00:00:00:CD:1D
 *  - SA = 00:12:34:56:78:9A
 *  - DIP = SIP from packet1
 *  - SIP = DIP from packet1
 * It is expected that the hash key (LB_Key) will be the same for the two packets.
 *
 * _________________________________
 *  UDPoIPv4oETH1 packet1 to send:
 *  - in port = in_port1
 *  - Vlan ID = 15
 *  - DA = 00:0c:00:02:00:00
 *  - SA = 00:00:07:00:01:00
 *  - DIP = 160.80.1.1
 *  - SIP = 192.128.1.1
 *  - Src_port = X (any)
 *  - Dst_port = Y (any)
 * expected:
 *  - out port = out_port
 *  - vlan = 100
 *  - DA = 00:00:00:00:CD:1D
 *  - SA = 00:12:34:56:78:9A
 *  - DIP = 160.80.1.1
 *  - SIP = 192.128.1.1
 *  - Src_port = X
 *  - Dst_port = Y
 *
 * UDPoIPv4oETH1 packet2 to send:
 *  - in port = in_port2
 *  - Vlan ID = 16
 *  - DA = SA from packet1
 *  - SA = DA from packet1
 *  - DIP = SIP from packet1
 *  - SIP = DIP from packet1
 *  - Src_port = Dst_port from packet1
 *  - Dst_port = Src_port from packet1
 * expected:
 *  - out port = out_port
 *  - vlan = 100
 *  - DA = 00:00:00:00:CD:1D
 *  - SA = 00:12:34:56:78:9A
 *  - DIP = SIP from packet1
 *  - SIP = DIP from packet1
 *  - Src_port = Dst_port from packet1
 *  - Dst_port = Src_port from packet1
 * It is expected that the hash key (LB_Key) will be the same for the two packets.
 *
 * _________________________________
 * TCPoIPv4oETH1 packet1 to send:
 *  - in port = in_port1
 *  - Vlan ID = 15
 *  - DA = 00:0c:00:02:00:00
 *  - SA = 00:00:07:00:01:00
 *  - DIP = 160.80.1.1
 *  - SIP = 192.128.1.1
 *  - Src_port = X (any)
 *  - Dst_port = Y (any)
 * expected:
 *  - out port = out_port
 *  - vlan = 100
 *  - DA = 00:00:00:00:CD:1D
 *  - SA = 00:12:34:56:78:9A
 *  - DIP = 160.80.1.1
 *  - SIP = 192.128.1.1
 *  - Src_port = X
 *  - Dst_port = Y
 *
 * TCPoIPv4oETH1 packet2 to send:
 *  - in port = in_port2
 *  - Vlan ID = 16
 *  - DA = SA from packet1
 *  - SA = DA from packet1
 *  - DIP = SIP from packet1
 *  - SIP = DIP from packet1
 *  - Src_port = Dst_port from packet1
 *  - Dst_port = Src_port from packet1
 * expected:
 *  - out port = out_port
 *  - vlan = 100
 *  - DA = 00:00:00:00:CD:1D
 *  - SA = 00:12:34:56:78:9A
 *  - DIP = SIP from packet1
 *  - SIP = DIP from packet1
 *  - Src_port = Dst_port from packet1
 *  - Dst_port = Src_port from packet1
 * It is expected that the hash key (LB_Key) will be the same for the two packets.
 */

int
cint_dnx_lb_symmetrical_hashing(
    int unit,
    int in_port1,
    int in_port2,
    int out_port)
{
    int rv = BCM_E_NONE;
    int intf_in1 = 15;           /* Incoming packet ETH-RIF */
    int intf_in2 = 16;           /* Incoming packet ETH-RIF */
    int intf_out = 100;         /* Outgoing packet ETH-RIF */
    int fec;
    int vrf = 1;
    int encap_id = 0x1384;
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    bcm_mac_t src_in_mac_address = { 0x00, 0x00, 0x07, 0x00, 0x01, 0x00 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_ip6_t dip_v6 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x13};
    bcm_ip6_t sip_v6 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xab};
    bcm_gport_t gport;
    bcm_l2_addr_t l2_addr;

    uint32 sip = 0xC0800101;
    uint32 dip = 0xA0500101;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, in_port1, intf_in1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }
    rv = in_port_intf_set(unit, in_port2, intf_in2);
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
        printf("Error, out_port_set intf_out out_port %d\n", out_port);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, intf_in1, intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in %d\n", intf_in1);
        return rv;
    }
    rv = intf_eth_rif_create(unit, intf_in2, src_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in %d\n", intf_in2);
        return rv;
    }
    rv = intf_eth_rif_create(unit, intf_out, intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }
    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = intf_in1;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = intf_in2;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    /*
     * 5. Create ARP and set its properties
     */
    rv = l3__egress_only_encap__create_inner(unit, 0, &encap_id, arp_next_hop_mac, intf_out, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only\n");
        return rv;
    }

    /*
     * 6. Create FEC and set its properties
     */
    printf("Create main FEC and set its properties.\r\n");
    BCM_GPORT_LOCAL_SET(gport, out_port);
    rv = l3__egress_only_fec__create(unit, fec, 0, encap_id, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    /*
     * 7. Add IPv4 host entries
     */
    int _l3_itf;
    BCM_L3_ITF_SET(&_l3_itf, BCM_L3_ITF_TYPE_FEC, fec);
    BCM_GPORT_LOCAL_SET(gport, out_port);
    rv = add_host_ipv4(unit, dip, vrf, _l3_itf, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv4(), \n");
        return rv;
    }
    rv = add_host_ipv4(unit, sip, vrf, _l3_itf, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv4(), \n");
        return rv;
    }

    /*
     * 8. Add IPV6 host entries
     */
    rv = add_host_ipv6(unit, dip_v6, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv6: vrf = %d, fec = %d, err_id = %d\n", vrf, fec, rv);
        return rv;
    }
    rv = add_host_ipv6(unit, sip_v6, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv6: vrf = %d, fec = %d, err_id = %d\n", vrf, fec, rv);
        return rv;
    }

    /*
     * 9. Add MACT entries
     */
    BCM_GPORT_FORWARD_PORT_SET(gport, fec);
    bcm_l2_addr_t_init(&l2_addr, intf_in_mac_address, intf_in1);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = gport;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_l2_addr_add dmac (vsi = %d)\n", intf_in1);
        return rv;
    }
    bcm_l2_addr_t_init(&l2_addr, src_in_mac_address, intf_in2);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = gport;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_l2_addr_add smac (vsi = %d)\n", intf_in2);
        return rv;
    }

    return rv;
}
