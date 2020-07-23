/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : L3 IPv6 route
 *
 *  Usage    : BCM.0> cint l3_ipv6_route.c
 *
 *  config   : l3_ipv6_route_config.bcm
 *
 *  Log file : l3_ipv6_route_log.txt
 *
 *  Test Topology : None
 *
 *  This script can verify below L3 APIs:
 *  ========
 *   bcm_l3_route_add
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate L3 IPv6 route traffic configuration.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Create vlan and add port to vlan. Enable L3 feature per port.
 *
 *    2) Step2 - Configuration (Done in l3_route_uc_create())
 *    ======================================================
 *      a) Create egress objects used for unicast route destionation.
 *      b) Create a route.
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Verify the result after sens a packet.
 *      b) Expected Result:
 *         Packet raw data (66):
 *         0000: 0000 0000 fe02 0000 0000 0002 8100 000c
 *         0010: 86dd 6335 5555 0022 06fe 3ffe 0000 0000
 *         0020: 0000 0000 0000 0000 0001 3ffe 0000 0000
 *         0030: 0000 0000 0000 0000 0002 3030 3131 0000
 *         0040: 3232 0000 3333 5000 0107 3dd9 0000 0001
 *         0050: 0203 0405 0607 0809 0a0b 0c0d 2335 7d7f
 */
cint_reset();

int unit = 0;
bcm_port_t port_in = 1, port_out = 2, port_out2 = 3;
bcm_vlan_t vlan_in = 11, vlan_out = 12, vlan_out2 = 13;
bcm_vrf_t vrf = 1;
bcm_mac_t router_mac_in = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
bcm_mac_t router_mac_out = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
bcm_mac_t router_mac_out2 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x03};
bcm_mac_t dst_mac = {0x00, 0x00, 0x00, 0x00, 0xfe, 0x02};
bcm_mac_t dst_mac2 = {0x00, 0x00, 0x00, 0x00, 0xfe, 0x03};
bcm_ip6_t dip6 = {0x3F, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
bcm_ip6_t mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xff};
bcm_ip6_t dip6_64 = {0x3F, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
bcm_ip6_t mask_64 = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
bcm_pbmp_t pbmp, ubmp;

void test_setup()
{
    /* Enable IPv4/6 on PORT */
    print bcm_port_control_set(unit, port_in, bcmPortControlIP6, 1);
    /* Create ingress vlan and add port to vlan */
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_in);
    print bcm_vlan_create(unit, vlan_in);
    print bcm_vlan_port_add(unit, vlan_in, pbmp, ubmp);

    /* Create egress vlan and add port to vlan */
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_out);
    BCM_PBMP_PORT_ADD(pbmp, 0); /* Added CPU port to same vlan. */
    print bcm_vlan_create(unit, vlan_out);
    print bcm_vlan_port_add(unit, vlan_out, pbmp, ubmp);

    /* Create egress vlan2 and add port to vlan */
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_out2);
    BCM_PBMP_PORT_ADD(pbmp, 0); /* Added CPU port to same vlan. */
    print bcm_vlan_create(unit, vlan_out2);
    print bcm_vlan_port_add(unit, vlan_out2, pbmp, ubmp);

    /* Set the default VLAN for the port */
    print bcm_port_untagged_vlan_set(unit, port_in, vlan_in);
    print bcm_port_untagged_vlan_set(unit, port_out, vlan_out);
    print bcm_port_untagged_vlan_set(unit, port_out2, vlan_out2);
}

void l3_route_uc_create()
{
    /* Create L3 interface */
    bcm_l3_intf_t l3_intf_in, l3_intf_out, l3_intf_out2;

    bcm_l3_intf_t_init(&l3_intf_in);
    sal_memcpy(l3_intf_in.l3a_mac_addr, router_mac_in, sizeof(router_mac_in));
    l3_intf_in.l3a_vid = vlan_in;
    print bcm_l3_intf_create(unit, &l3_intf_in);

    bcm_l3_intf_t_init(&l3_intf_out);
    sal_memcpy(l3_intf_out.l3a_mac_addr, router_mac_out, sizeof(router_mac_out));
    l3_intf_out.l3a_vid = vlan_out;
    print bcm_l3_intf_create(unit, &l3_intf_out);

    bcm_l3_intf_t_init(&l3_intf_out2);
    sal_memcpy(l3_intf_out2.l3a_mac_addr, router_mac_out2, sizeof(router_mac_out2));
    l3_intf_out2.l3a_vid = vlan_out2;
    print bcm_l3_intf_create(unit, &l3_intf_out2);

    /* Create L3 ingress interface */
    bcm_l3_ingress_t l3_ingress;
    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.flags = BCM_L3_INGRESS_WITH_ID;
    bcm_if_t ingress_if = 0x1b2;
    l3_ingress.vrf = vrf;
    print bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if);

    /* Config vlan_id to l3_iif mapping */
    bcm_vlan_control_vlan_t vlan_ctrl;
    bcm_vlan_control_vlan_t_init(&vlan_ctrl);
    bcm_vlan_control_vlan_get(unit, vlan_in, &vlan_ctrl);
    vlan_ctrl.ingress_if = ingress_if;
    print bcm_vlan_control_vlan_set(unit, vlan_in, vlan_ctrl);

    /* Create L3 egress object */
    bcm_l3_egress_t l3_egress;
    bcm_if_t egr_obj_id;
    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr, dst_mac, sizeof(dst_mac));
    l3_egress.port = port_out;
    l3_egress.intf = l3_intf_out.l3a_intf_id;
    print bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_id);
    print egr_obj_id;

    bcm_l3_egress_t l3_egress2;
    bcm_if_t egr_obj_id2;
    bcm_l3_egress_t_init(&l3_egress2);
    sal_memcpy(l3_egress2.mac_addr, dst_mac2, sizeof(dst_mac2));
    l3_egress2.intf = l3_intf_out2.l3a_intf_id;
    l3_egress2.port = port_out2;

    l3_egress2.mtu = 1500;
    print bcm_l3_egress_create(unit, 0, &l3_egress2, &egr_obj_id2);
    print egr_obj_id2;

    /* Install the route for DIP (DEFIP_128) */
    bcm_l3_route_t route_info;
    bcm_l3_route_t_init(&route_info);
    route_info.l3a_flags = BCM_L3_IP6;
    route_info.l3a_intf = egr_obj_id;
    route_info.l3a_ip6_net = dip6;
    route_info.l3a_ip6_mask = mask;
    route_info.l3a_vrf = vrf;
    print bcm_l3_route_add(unit, &route_info);

    /* Install the route for DIP (DEFIP_64) */
    bcm_l3_route_t_init(&route_info);
    route_info.l3a_flags = BCM_L3_IP6;
    route_info.l3a_intf = egr_obj_id2;
    route_info.l3a_ip6_net = dip6_64;
    route_info.l3a_ip6_mask = mask_64;
    route_info.l3a_vrf = vrf;
    print bcm_l3_route_add(unit, &route_info);

    bcm_l3_route_t_init(&route_info);
    route_info.l3a_flags = BCM_L3_IP6;
    route_info.l3a_ip6_net = dip6;
    route_info.l3a_ip6_mask = mask;
    route_info.l3a_vrf = vrf;
    print bcm_l3_route_get(unit, &route_info);

    bcm_l3_route_t_init(&route_info);
    route_info.l3a_flags = BCM_L3_IP6;
    route_info.l3a_ip6_net = dip6_64;
    route_info.l3a_ip6_mask = mask_64;
    route_info.l3a_vrf = vrf;
    print bcm_l3_route_get(unit, &route_info);

    /* Config my station */
    bcm_l2_station_t l2_station;
    int station_id;
    bcm_l2_station_t_init(&l2_station);
    sal_memcpy(l2_station.dst_mac, router_mac_in, sizeof(router_mac_in));
    l2_station.dst_mac_mask = "ff:ff:ff:ff:ff:ff";
    l2_station.flags = BCM_L2_STATION_IPV4 | BCM_L2_STATION_IPV6;
    print bcm_l2_station_add(unit, &station_id, &l2_station);
}

void verify()
{
    /* Traffic */
    bshell(unit, "pw start");
    bshell(unit, "pw report all");

    /* Set port_in to mac loopback */
    print bcm_port_loopback_set(unit, port_in, BCM_PORT_LOOPBACK_MAC);
    print bcm_port_loopback_set(unit, port_out, BCM_PORT_LOOPBACK_MAC);
    print bcm_port_loopback_set(unit, port_out2, BCM_PORT_LOOPBACK_MAC);

    /* Added a L2 address to trap loopbacked packet on port_out to CPU */
    bcm_l2_addr_t addr;
    bcm_l2_addr_t_init(&addr, dst_mac, vlan_out);
    addr.port=0;
    addr.flags |= BCM_L2_STATIC;
    print bcm_l2_addr_add(unit, &addr);

    bcm_l2_addr_t addr2;
    bcm_l2_addr_t_init(&addr2, dst_mac2, vlan_out2);
    addr2.port=0;
    addr2.flags |= BCM_L2_STATIC;
    print bcm_l2_addr_add(unit, &addr2);
    /* Send pacekt to port_in */
    bshell(unit, "tx 1 pbm=cd0 data=0x000000000001002A1077770086dd63355555002206ff3ffe00000000000000000000000000013ffe0000000000000000000000000002303031310000323200003333500001073dd90000000102030405060708090a0b0c0d23357d7f");

    bshell(unit, "sleep 5");
    bshell(unit, "show c");
}

test_setup();
l3_route_uc_create();
verify();


