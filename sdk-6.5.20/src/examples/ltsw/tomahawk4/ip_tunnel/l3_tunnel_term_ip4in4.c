/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : L3 Ipv4 in Ipv4 Tunnel
 *
 *  Usage    : BCM.0> cint l3_tunnel_term_ip4in4.c
 *
 *  config   : l3_tunnel_term_ip4in4_config.bcm
 *
 *  Log file : l3_tunnel_term_ip4in4_log.txt
 *
 *  Test Topology : None
 *
 *  This script can verify below L3 IP Tunnel APIs:
 *  ========
 *   bcm_tunnel_terminator_add
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate L3 Ipv4 in Ipv4 Tunnel traffic configuration.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Create vlan and add port to vlan. Enable L3 feature per port.
 *
 *    2) Step2 - Configuration (Done in l3_tunnel_create())
 *    ======================================================
 *      a) Create ingress and egress objects used for tunnel terminator.
 *      b) Create routes for tunnel terminator.
 *      c) Add L2 MAC into l2 station table.
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Verify the result after send a packet.
 *      b) Expected Result:
 *         Packet raw data (72):
 *      0000: 0000 0000 0001 0000 0000 1111 8100 0015
 *      0010: 0800 4500 002e 0000 0000 3fff   74cc  0101
 *      0020: 0101 0202 0202 0000 0000 0000 0000 0000
 *      0030: 0000 0000 0000 0000 0000 0000 0000 0000
 *      0040: 8856 eef7
 */
cint_reset();

int unit = 0;
bcm_ip_t dip = 0x02020202; /* 2.2.2.2 */

bcm_ip_t tunnel_ip_remote = 0x0a0a0a0a;
bcm_ip_t tunnel_ip_local = 0x09090909;

bcm_mac_t router_mac = {0x00, 0x00, 0x00, 0x00, 0x11, 0x11};
bcm_mac_t tunnel_mac = {0x00, 0x00, 0x00, 0x00, 0x22, 0x22};
bcm_mac_t router_nh_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
bcm_mac_t tunnel_nh_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};

bcm_vrf_t   vrf = 0;
bcm_vlan_t  vid_out = 21;
bcm_vlan_t  vid_tunnel = 22;
int l3_egr_if = 1;

bcm_port_t      port_out = 2;
bcm_port_t      port_tunnel = 3;

bcm_gport_t     gport_out, gport_tunnel;
bcm_pbmp_t pbmp, ubmp;

/*
 * This function is pre-test setup.
 */
void test_setup()
{
    /* Enable IPv4/6 on PORT */
    print bcm_port_control_set(unit, port_tunnel, bcmPortControlIP4, 1);

    /* Create ingress vlan and add port to vlan */
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_out);
    print bcm_vlan_create(unit, vid_out);
    print bcm_vlan_port_add(unit, vid_out, pbmp, ubmp);

    /* Create egress vlan and add port to vlan */
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_tunnel);
    BCM_PBMP_PORT_ADD(pbmp, 0); /* Added CPU port to same vlan. */
    print bcm_vlan_create(unit, vid_tunnel);
    print bcm_vlan_port_add(unit, vid_tunnel, pbmp, ubmp);

    /* Set the default VLAN for the port */
    print bcm_port_untagged_vlan_set(unit, port_out, vid_out);
    print bcm_port_untagged_vlan_set(unit, port_tunnel, vid_tunnel);

    bcm_port_gport_get(unit, port_out, &gport_out);
    bcm_port_gport_get(unit, port_tunnel, &gport_tunnel);
}

void l3_tunnel_create()
{
    /* Create L3 interface */
    bcm_l3_intf_t egr_l3_intf;
    bcm_l3_intf_t_init(&egr_l3_intf);
    egr_l3_intf.l3a_flags = BCM_L3_WITH_ID;
    egr_l3_intf.l3a_intf_id = l3_egr_if;
    sal_memcpy(egr_l3_intf.l3a_mac_addr, router_mac, 6);
    egr_l3_intf.l3a_vid = vid_out;
    egr_l3_intf.l3a_vrf = vrf;
    print bcm_l3_intf_create(unit, &egr_l3_intf);

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
    bcm_vlan_control_vlan_get(unit, vid_tunnel, &vlan_ctrl);
    vlan_ctrl.ingress_if = ingress_if;
    print bcm_vlan_control_vlan_set(unit, vid_tunnel, vlan_ctrl);

    /* Create L3 egress object */
    bcm_if_t l3_egr_obj_id = 0;
    bcm_l3_egress_t l3_egr_obj;
    bcm_l3_egress_t_init(&l3_egr_obj);
    l3_egr_obj.flags = 0;
    l3_egr_obj.intf = l3_egr_if;
    sal_memcpy(l3_egr_obj.mac_addr, router_nh_mac, 6);
    l3_egr_obj.vlan = vid_out;
    l3_egr_obj.port = gport_out;
    print bcm_l3_egress_create(unit, 0, &l3_egr_obj, &l3_egr_obj_id);
    print l3_egr_obj_id;

    /* Add tunnel termination */
    bcm_tunnel_terminator_t tnl_term_info;
    print bcm_tunnel_terminator_t_init(&tnl_term_info);
    int tnl_term_flags = BCM_TUNNEL_TERM_USE_OUTER_DSCP |
                         BCM_TUNNEL_TERM_USE_OUTER_TTL |
                         BCM_TUNNEL_TERM_KEEP_INNER_DSCP;
    tnl_term_info.flags = tnl_term_flags;
    tnl_term_info.type = bcmTunnelTypeIp4In4; /* Choose Ipv4-in-Ipv4 tunnel type */
    tnl_term_info.vrf = BCM_L3_VRF_DEFAULT;
    tnl_term_info.vlan = ingress_if;
    tnl_term_info.sip = tunnel_ip_local;
    tnl_term_info.dip = tunnel_ip_remote;
    tnl_term_info.sip_mask = bcm_ip_mask_create(32);
    tnl_term_info.dip_mask = bcm_ip_mask_create(32);
    BCM_PBMP_PORT_SET(tnl_term_info.pbmp, port_tunnel);
    print bcm_tunnel_terminator_add(unit, &tnl_term_info);

    /* Add L3 Route */
    bcm_l3_route_t route;
    bcm_l3_route_t_init(&route);
    route.l3a_subnet = dip;
    route.l3a_ip_mask = bcm_ip_mask_create(32);
    route.l3a_intf = l3_egr_obj_id;
    route.l3a_vrf = vrf;
    print bcm_l3_route_add(unit, &route);

    /* Add L2 MAC into l2 station table to enable L3 lookup for MAC + VLAN */
    int station_id;
    bcm_l2_station_t station;
    bcm_mac_t mac_mask = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    bcm_l2_station_t_init(&station);
    station.flags = BCM_L2_STATION_IPV4;
    sal_memcpy(station.dst_mac, tunnel_mac, sizeof(tunnel_mac));
    sal_memcpy(station.dst_mac_mask, mac_mask, sizeof(mac_mask));
    station.vlan = vid_tunnel;
    station.vlan_mask = vid_tunnel ? 0xfff: 0x0;
    print bcm_l2_station_add(unit, &station_id, &station);

    bcm_l2_station_t_init(&station);
    station.flags = BCM_L2_STATION_IPV4 | BCM_L2_STATION_COPY_TO_CPU;
    sal_memcpy(station.dst_mac, router_nh_mac, sizeof(router_nh_mac));
    sal_memcpy(station.dst_mac_mask, mac_mask, sizeof(mac_mask));
    station.vlan = vid_out;
    station.vlan_mask = vid_out ? 0xfff: 0x0;
    print bcm_l2_station_add(unit, &station_id, &station);
}

/*
 * This function does the following.
 * Transmit the below packet on port_tunnel and verify
 */
void verify()
{
    char   str[512];
    uint64 in_pkt, out_pkt;

    /* Set ports to mac loopback */
    print bcm_port_loopback_set(unit, port_out, BCM_PORT_LOOPBACK_MAC);
    print bcm_port_loopback_set(unit, port_tunnel, BCM_PORT_LOOPBACK_MAC);

    /* Start traffic. */
    bshell(unit, "pw start");
    bshell(unit, "pw report all");

    /* Send pacekt to port_tunnel */
    snprintf(str, 512, "tx 1 pbm=%d data=000000002222000000000002810000160800450000420000000040045493090909090A0A0A0A4500002E0000000040FF73CC010101010202020200000000000000000000000000000000000000000000000000008856EEF7;sleep 5", port_tunnel);
    printf("%s\n", str);
    bshell(unit, str);
    bshell(unit, "show c");
}

test_setup();
l3_tunnel_create();
verify();
