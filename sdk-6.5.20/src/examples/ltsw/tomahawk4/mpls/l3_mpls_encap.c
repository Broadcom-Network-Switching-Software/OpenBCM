/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : L3 MPLS encap
 *
 *  Usage    : BCM.0> cint l3_mpls_encap.c
 *
 *  config   : l3_mpls_encap_config.bcm
 *
 *  Log file : l3_mpls_encap_log.txt
 *
 *  Test Topology : None
 *
 *  This script can verify below L3 APIs:
 *  ========
 *   bcm_l3_route_add
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate L3 MPLS encap traffic configuration.
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
 *         0000: 00 00 00 00 fe 02 00 00 - 00 00 00 02 08 00 45 00
 *         0010: 00 3c 67 62 00 00 fe 01 - be 76 0a 3a 40 01 0a 3a
 *         0020: 42 73 00 00 55 0a 00 01 - 00 51 61 62 63 64 65 66
 *         0030: 67 68 69 6a 6b 6c 6d 6e - 6f 70 71 72 73 74 75 76
 *         0040: 77 61
 */
cint_reset();

int unit = 0;
int rv;
bcm_port_t port_in = 1, port_out = 2, port_out2 = 3;
bcm_vlan_t vlan_in = 11, vlan_out = 12, vlan_out2 = 13;
bcm_vrf_t vrf = 1;
bcm_mac_t router_mac_in = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
bcm_mac_t router_mac_out = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
bcm_mac_t router_mac_out2 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x03};
bcm_mac_t dst_mac = {0x00, 0x00, 0x00, 0x00, 0xfe, 0x02};
bcm_mac_t dst_mac2 = {0x00, 0x00, 0x00, 0x00, 0xfe, 0x03};
bcm_ip_t dip = 0x0a3a4273;
bcm_ip_t mask = 0xffffff00;
bcm_ip_t mask2 = 0xfffff000;
bcm_pbmp_t pbmp, ubmp;

void test_setup()
{
    /* Enable IPv4/6 on PORT */
    print bcm_port_control_set(unit, port_in, bcmPortControlIP4, 1);
    /* Enable MPLS on PORT */
    print bcm_port_control_set(unit, port_in, bcmPortControlMpls, 1);
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
    int num_labels = 2;
    bcm_mpls_egress_label_t label_array[2];

    label_array[0].label = 0x50;
    label_array[1].label = 0x500;
    /*
    bcm_mpls_tunnel_encap_t tunnel_encap;
    bcm_mpls_tunnel_encap_t_init(&tunnel_encap);
    tunnel_encap.num_labels = 2;
    tunnel_encap.label_array[0].label = 0x50;
    tunnel_encap.label_array[1].label = 0x500;
    rv = bcm_mpls_tunnel_encap_create(unit, 0, &tunnel_encap);
    if (BCM_FAILURE(rv)) {
        printf("ERROR: bcm_mpls_tunnel_encap_create() - %d : %s.\n", rv, bcm_errmsg(rv));
    }
    printf("tunnel idx = 0x%x. \n", tunnel_encap.tunnel_id);
    */

    bcm_l3_intf_t_init(&l3_intf_in);
    sal_memcpy(l3_intf_in.l3a_mac_addr, router_mac_in, sizeof(router_mac_in));
    l3_intf_in.l3a_vid = vlan_in;
    print bcm_l3_intf_create(unit, &l3_intf_in);

    bcm_l3_intf_t_init(&l3_intf_out);
    sal_memcpy(l3_intf_out.l3a_mac_addr, router_mac_out, sizeof(router_mac_out));
    l3_intf_out.l3a_vid = vlan_out;
    print bcm_l3_intf_create(unit, &l3_intf_out);

    rv = bcm_mpls_tunnel_initiator_set(unit, l3_intf_out.l3a_intf_id, num_labels, label_array);
    if (BCM_FAILURE(rv)) {
        printf("ERROR: bcm_mpls_tunnel_initiator_set() - %d : %s.\n", rv, bcm_errmsg(rv));
    }

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
    l3_egress.flags = BCM_L3_ROUTE_LABEL;
    l3_egress.port = port_out;
    l3_egress.intf = l3_intf_out.l3a_intf_id;
    l3_egress.mpls_label = 0x200;
    print bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_id);
    print egr_obj_id;

    bcm_if_t egr_obj_id2;
    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr, dst_mac2, sizeof(dst_mac2));
    l3_egress.intf = l3_intf_out2.l3a_intf_id;
    l3_egress.port = port_out2;
    l3_egress.mtu = 1500;
    print bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_id2);
    print egr_obj_id2;

    /* Install the route for DIP */
    bcm_l3_route_t route_info;
    bcm_l3_route_t_init(&route_info);
    route_info.l3a_intf = egr_obj_id2;
    route_info.l3a_subnet = dip;
    route_info.l3a_ip_mask = mask2;
    route_info.l3a_vrf = vrf;
    print bcm_l3_route_add(unit, &route_info);

    bcm_l3_route_t_init(&route_info);
    route_info.l3a_intf = egr_obj_id;
    route_info.l3a_subnet = dip;
    route_info.l3a_ip_mask = mask;
    route_info.l3a_vrf = vrf;
    print bcm_l3_route_add(unit, &route_info);

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
    addr.port = 0;
    addr.flags |= BCM_L2_STATIC;
    print bcm_l2_addr_add(unit, &addr);

    /* Send pacekt to port_in */
    bshell(unit, "tx 1 pbm=cd0 data=0x000000000001002a1077770008004500003c67620000ff01bd760a3a40010a3a42730000550a000100516162636465666768696a6b6c6d6e6f7071727374757677616263646566676869");

    bshell(unit, "sleep 5");
    bshell(unit, "show c");
}

test_setup();
l3_route_uc_create();
verify();



