/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/*
 * Example of a simple-router IPMC priorities scenario
 *
 * Test Scenario
 *
 * Test Scenario - start
 *
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/ipmc/cint_dnx_ipmc_route_basic.c
  cint ../../../../src/examples/dnx/ipmc/cint_dnx_ipmc_route_priorities.c
  cint
  basic_ipmc_priorities_example(0,0xCC,200,201,202);
  exit;
 *
 *  ETH-RIF packet
  tx 1 psrc=200 data=0x01005e020202000007000100810000cc0800450000350000000080009743c0800101e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be:
 *  Source port: 0, Destination port: 0
 *  Data: 0x5e02020200123456789b81000101080045000035000000007f009843c0800101e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *  Source port: 0, Destination port: 0
 *  Data: 0x5e02020200123456789a81000100080045000035000000007f009843c0800101e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 *  ETH-RIF packet
  tx 1 psrc=200 data=0x01005e020202000007000100810000cc0800450000350000000080009742c0800102e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be:
 *  Source port: 0, Destination port: 0
 *  Data: 0x5e02020200123456789d81000103080045000035000000007f009842c0800102e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *  Source port: 0, Destination port: 0
 *  Data: 0x5e02020200123456789c81000102080045000035000000007f009842c0800102e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 *  ETH-RIF packet
  tx 1 psrc=200 data=0x01005e020202000007000100810000cc0800450000350000000080009642c0800202e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be:
 *  Source port: 0, Destination port: 0
 *  Data: 0x5e02020200123456789f81000105080045000035000000007f009742c0800202e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *  Source port: 0, Destination port: 0
 *  Data: 0x5e02020200123456789e81000104080045000035000000007f009742c0800202e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 *  ETH-RIF packet
  tx 1 psrc=200 data=0x01005e020202000007000100810000cc0800450000350000000080009641c0810202e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be:
 *  Source port: 0, Destination port: 0
 *  Data: 0x5e0202020012345678a181000107080045000035000000007f009741c0810202e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *  Source port: 0, Destination port: 0
 *  Data: 0x5e0202020012345678a081000106080045000035000000007f009741c0810202e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 *  ETH-RIF packet
  tx 1 psrc=200 data=0x01005e020203000007000100810000cc0800450000350000000080009640c0810202e0020203000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be:
 *  Source port: 0, Destination port: 0
 *  Data: 0x5e0202030012345678a381000109080045000035000000007f009740c0810202e0020203000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *  Source port: 0, Destination port: 0
 *  Data: 0x5e0202030012345678a281000108080045000035000000007f009740c0810202e0020203000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 *  ETH-RIF packet
  tx 1 psrc=200 data=0x01005e030203000007000100810000cc0800450000350000000080009640c0810202e0030203000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be:
 *  Source port: 0, Destination port: 0
 *  Data: 0x01005e0302030012345678a48100010a080045000035000000007f009740c0810202e0030203000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *  Source port: 0, Destination port: 0
 *  Data: 0x01005e0302030012345678a58100010b080045000035000000007f009740c0810202e0030203000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 * Test Scenario - end
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Example of a simple-router IPv6 IPMC priorities scenario
 *
 * Test Scenario
 *
 * Test Scenario - start
 *
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/ipmc/cint_dnx_ipmc_route_basic.c
  cint ../../../../src/examples/dnx/ipmc/cint_dnx_ipmc_route_priorities.c
  cint
  basic_ipmc_ipv6_priorities_example(0,0xCC,200,201,202);
  exit;
 *
 * ETH-RIF packet to hit unmasked TCAM entry
  tx 1 psrc=200 data=0x333300001111000007000100810000cc86dd600000000049068000000000111100000000000022221111ffff0000000011110000000000001111000102030405060708090a
 *
 * ETH-RIF packet to hit TCAM entry with partially masked SIP
  tx 1 psrc=200 data=0x333300001111000007000100810000cc86dd600000000049068000000000111100000000000022221112ffff0000000011110000000000001111000102030405060708090a
 *
 * ETH-RIF packet to hit LEM entry
  tx 1 psrc=200 data=0x333300001111000007000100810000cc86dd600000000049068000000000111200000000000022221113ffff0000000011110000000000001111000102030405060708090a
 *
 * ETH-RIF packet to hit LPM entry with masked in-LIF
  tx 1 psrc=200 data=0x333300001111000007000100810000cd86dd600000000049068000000000111200000000000022221113ffff0000000011110000000000001111000102030405060708090a
 *
 * ETH-RIF packet to hit LPM entry with partially masked DIP
  tx 1 psrc=200 data=0x333300001112000007000100810000cd86dd600000000049068000000000111200000000000022221113ffff0000000011110000000000001112000102030405060708090a
 *
 * ETH-RIF packet to hit LPM default entry
  tx 1 psrc=200 data=0x333300001112000007000100810000cd86dd600000000049068000000000111200000000000022221113ffff0000000022220000000000001112000102030405060708090a
 * Test Scenario - end
 *
 */

int
basic_ipmc_priorities_example(
    int unit,
    int vid_in,
    int in_port,
    int out_port1,
    int out_port2)
{
    cint_ipmc_info.intf_in = vid_in;
    cint_ipmc_info.vrf = vid_in;
    cint_ipmc_info.in_port = in_port;
    uint32 all_valid = 0xFFFFFFFF;
    uint32 none_valid = 0x0;
    uint32 sip_part_mask = 0xFFFF0000;
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);
    bcm_gport_t mc_gport;
    int mc_group[6] = {6000, 6001, 6002, 6003, 6004, 6005};
    int i;
    int fec_id;

    cint_ipmc_info.out_ports[0] = out_port1;
    cint_ipmc_info.out_ports[1] = out_port2;

    /*
     * 1. Set In-Port to In ETh-RIF
     * 2. Set Out-Port
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, cint_ipmc_info.in_port, cint_ipmc_info.intf_in), "intf_in");

    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port1), "");

    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port2), "");

    /*
     * 2. Create ETH-RIF and set its properties
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_ipmc_info.intf_in, cint_ipmc_info.intf_in_mac_address), "intf_in");

    /*
     * 3. Set Incoming ETH-RIF properties
     */
    ingr_itf.intf_id = cint_ipmc_info.intf_in;
    ingr_itf.vrf = cint_ipmc_info.vrf;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf), "intf_out");

    /*
     * 4. Create a multicast group and its replications
     */
    for (i = 0; i < 6; i++)
    {
        BCM_IF_ERROR_RETURN_MSG(create_multicast_group_and_replications(unit, mc_group[i], cint_ipmc_info.out_ports), "");
        cint_ipmc_info.intf_out+=2;
    }

    /*
     * 5. Add IPv4 IPMC entries
     */
    BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, cint_ipmc_info.mc_dip, all_valid, cint_ipmc_info.sip, all_valid, cint_ipmc_info.intf_in,
                       cint_ipmc_info.vrf, mc_group[0], 0, 0), "to LPM table (full prefix)");
    if (*(dnxc_data_get(unit, "l3", "fwd", "tcam_entry_support", NULL)))
    {
        BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, cint_ipmc_info.mc_dip, all_valid, cint_ipmc_info.sip, 0xffffff00,
            cint_ipmc_info.intf_in, cint_ipmc_info.vrf, mc_group[1], 0, BCM_IPMC_TCAM), "to TCAM table");
    }
    BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, cint_ipmc_info.mc_dip, all_valid, cint_ipmc_info.sip, 0xffff0000, 0,
                       cint_ipmc_info.vrf, mc_group[2], 0, 0), "to LPM table (prefix on SIP)");
    if (*(dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL)))
    {
        BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, cint_ipmc_info.mc_dip, all_valid, cint_ipmc_info.sip, none_valid,
                           cint_ipmc_info.intf_in, cint_ipmc_info.vrf, mc_group[3], 0, 0), "to LEM table");
    }
    BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, cint_ipmc_info.mc_dip, 0xffffff00, cint_ipmc_info.sip, none_valid, 0,
                       cint_ipmc_info.vrf, mc_group[4], 0, 0), "to LPM table (prefix on G)");

    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &fec_id), "");

    BCM_GPORT_MCAST_SET(mc_gport, mc_group[5]);
    int fec_flags2 = 0;
    if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 = BCM_L3_FLAGS2_FWD_ONLY;
    }
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, fec_id, cint_ipmc_info.intf_in, 0, mc_gport, 0, fec_flags2),
        "create egress object FEC inner, fecs[0]");
    BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, cint_ipmc_info.mc_dip, none_valid, cint_ipmc_info.sip, none_valid, 0,
                       cint_ipmc_info.vrf, 0, fec_id, 0), "to LPM table (default)");

    return BCM_E_NONE;
}

/*
 * Example of a simple-router IPv6 IPMC priorities scenario
 *
 * Test Scenario
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/ipmc/cint_dnx_ipmc_route_basic.c
  cint ../../../../src/examples/dnx/ipmc/cint_dnx_ipmc_route_priorities.c
  cint
  basic_ipmc_ipv6_priorities_example(0,0xCC,200,201,202);
  exit;
 * Test Scenario - end
 */
int
basic_ipmc_ipv6_priorities_example(
    int unit,
    int vid_in,
    int in_port,
    int out_port1,
    int out_port2)
{
    cint_ipmc_info.intf_in = vid_in;
    cint_ipmc_info.vrf = vid_in;
    cint_ipmc_info.in_port = in_port;
    bcm_ip6_t dip_addr = {0xFF, 0xFF, 0, 0, 0, 0, 0x11, 0x11, 0, 0, 0, 0, 0, 0, 0x11, 0x11};
    bcm_ip6_t sip_addr = {0, 0, 0, 0, 0x11, 0x11, 0, 0, 0, 0, 0, 0, 0x22, 0x22, 0x11, 0x11};
    bcm_ip6_t all_valid = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    bcm_ip6_t none_valid = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    bcm_ip6_t part_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0, 0, 0, 0, 0, 0};
    int mc_group[7] = {6000, 6001, 6002, 6003, 6004, 6005, 6006};
    int i;
    int fec_id;
    bcm_gport_t mc_gport;
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);
    uint32 flags = 0;

    cint_ipmc_info.out_ports[0] = out_port1;
    cint_ipmc_info.out_ports[1] = out_port2;
    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &fec_id), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, cint_ipmc_info.intf_in, cint_ipmc_info.in_port, 0), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, cint_ipmc_info.intf_in+1, cint_ipmc_info.in_port, 0), "");

    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, cint_ipmc_info.out_ports[0]), "");

    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, cint_ipmc_info.out_ports[1]), "");

    /*
     * 2. Create ETH-RIF and set its properties
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_ipmc_info.intf_in, cint_ipmc_info.intf_in_mac_address), "intf_in");
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_ipmc_info.intf_in + 1, cint_ipmc_info.intf_in_mac_address), "intf_in");

    /*
     * 3. Set Incoming ETH-RIF properties
     */
    ingr_itf.intf_id = cint_ipmc_info.intf_in;
    ingr_itf.vrf = cint_ipmc_info.vrf;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf), "intf_out");
    ingr_itf.intf_id = cint_ipmc_info.intf_in + 1;
    ingr_itf.vrf = cint_ipmc_info.vrf;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf), "intf_out");

    /*
     * 4. Create a multicast group and its replications
     */
    for (i = 0; i < 7; i++)
    {
        BCM_IF_ERROR_RETURN_MSG(create_multicast_group_and_replications(unit, mc_group[i], cint_ipmc_info.out_ports), "");
        cint_ipmc_info.intf_out+=2;
    }

    BCM_GPORT_MCAST_SET(mc_gport, mc_group[6]);
    int fec_flags2 = 0;
    if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 = BCM_L3_FLAGS2_FWD_ONLY;
    }
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, fec_id, 0, 0, mc_gport, 0, fec_flags2),
        "create egress object FEC inner, fecs[0]");

    /*
     * 5. Add IPv6 IPMC entries
     */
    
    if (*(dnxc_data_get(unit, "l3", "fwd", "ipmc_config_cmprs_vrf_group", NULL)))
    {
        bcm_ipmc_addr_t data;
        bcm_ipmc_addr_t config;
        /**   
         * a. LEM C_VRFG + LPM full prefix
         * b. TCAM C_VRFG + LPM full prefix
         * c. TCAM C_VRFG + LPM partial SIP
         * d. TCAM C_VRFG + LPM fully masked SIP
         * e. TCAM C_VRFG partially masked + LPM fully masked SIP
         * f. TCAM C_VRFG (VRF + partial DIP) + LPM fully masked SIP
         * g. TCAM C_VRF (VRF + masked DIP) + LPM fully masked SIP
         */
        /** LEM C_VRFG + LPM full prefix */
        BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc(unit, dip_addr, all_valid, sip_addr, all_valid, 0, cint_ipmc_info.vrf,
                        cint_ipmc_info.compressed_sip, 0xFFFF, mc_group[0], 0, 0, 0), "1 to TCAM + LPM table");
        dip_addr[15]++;
        cint_ipmc_info.compressed_sip++;

        /** TCAM C_VRFG + LPM full prefix */
        BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc(unit, dip_addr, all_valid, sip_addr, all_valid, 0, cint_ipmc_info.vrf,
                        cint_ipmc_info.compressed_sip, 0xFFFF, mc_group[1], 0, 0, BCM_IPMC_TCAM), "2 to TCAM + LPM table");

        /** TCAM C_VRFG + LPM partial SIP */
        bcm_ipmc_addr_t_init(&data);
        data.s_ip6_addr = sip_addr;
        data.s_ip6_mask = part_mask;
        data.mc_ip_addr = cint_ipmc_info.compressed_sip;
        data.mc_ip_mask = 0xFFFF;
        data.group = mc_group[2];
        data.flags = BCM_IPMC_IP6;
        BCM_IF_ERROR_RETURN_MSG(bcm_ipmc_add(unit, &data), "");

        /** TCAM C_VRFG + LPM fully masked SIP */
        bcm_ipmc_addr_t_init(&data);
        data.s_ip6_addr = sip_addr;
        data.s_ip6_mask = none_valid;
        data.mc_ip_addr = cint_ipmc_info.compressed_sip;
        data.mc_ip_mask = 0xFFFF;
        data.group = mc_group[3];
        data.flags = BCM_IPMC_IP6;
        BCM_IF_ERROR_RETURN_MSG(bcm_ipmc_add(unit, &data), "");
        cint_ipmc_info.compressed_sip++;
        dip_addr[15]++;

        /** TCAM C_VRFG partially masked + LPM fully masked SIP */
        BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc(unit, dip_addr, all_valid, sip_addr, none_valid, 0, cint_ipmc_info.vrf,
                        cint_ipmc_info.compressed_sip, 0xFF00, mc_group[4], 0, 0, BCM_IPMC_TCAM), "5 to TCAM + LPM table");
        cint_ipmc_info.compressed_sip += 0x100;

        /** TCAM C_VRFG (VRF + partial DIP) + LPM fully masked SIP */
        BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc(unit, dip_addr, part_mask, sip_addr, none_valid, 0, cint_ipmc_info.vrf,
                        cint_ipmc_info.compressed_sip, 0xFF00, mc_group[5], 0, 0, BCM_IPMC_TCAM), "6 to TCAM + LPM table");
        cint_ipmc_info.compressed_sip += 0x100;

        /** TCAM C_VRF (VRF + masked DIP) + LPM fully masked SIP */
        BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc(unit, dip_addr, none_valid, sip_addr, none_valid, 0, cint_ipmc_info.vrf,
                        cint_ipmc_info.compressed_sip, 0xFF00, mc_group[6], 0, 0, BCM_IPMC_TCAM), "7 to TCAM + LPM table");
    }
    else
    {
        BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc(unit, dip_addr, all_valid, sip_addr, all_valid, cint_ipmc_info.intf_in, cint_ipmc_info.vrf,
                        cint_ipmc_info.compressed_sip, 0xFFFF, mc_group[0], 0, flags, 0), "1 to LPM table");
        cint_ipmc_info.compressed_sip++;
        BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc(unit, dip_addr, all_valid, sip_addr, all_valid, 0, cint_ipmc_info.vrf, cint_ipmc_info.compressed_sip, 0xFFFF,
                        mc_group[1], 0, flags, 0), "2 to LPM table");
        cint_ipmc_info.compressed_sip++;
        BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc(unit, dip_addr, all_valid, sip_addr, part_mask, 0, cint_ipmc_info.vrf, cint_ipmc_info.compressed_sip, 0xFFFF,
                        mc_group[2], 0, flags, 0), "3 to LPM table");
        BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc(unit, dip_addr, all_valid, sip_addr, none_valid, 0, cint_ipmc_info.vrf, 0, 0,
                            mc_group[3], 0, flags, 0), "5 to LPM table");
        dip_addr[15]++;
        BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc(unit, dip_addr, all_valid, none_valid, none_valid, cint_ipmc_info.intf_in,
                        cint_ipmc_info.vrf, 0, 0, mc_group[4], 0, 0, 0), "to LEM table");

        BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc(unit, dip_addr, part_mask, none_valid, none_valid, 0, cint_ipmc_info.vrf, 0, 0, mc_group[5], 0, 0, 0),
            "to LPM table (prefix on G)");
        BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc(unit, dip_addr, none_valid, none_valid, none_valid, 0, cint_ipmc_info.vrf, 0, 0, 0, fec_id, 0, 0),
            "to LPM table default");
    }
    return BCM_E_NONE;
}
