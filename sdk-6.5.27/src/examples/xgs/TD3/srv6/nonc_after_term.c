/*
ABSTRACT:
    Srv6 L3 tunnel (canonical) termination of L2-IPv6-SRH-IPv6-TCP and
    Non canonical SRH insertion with 3 SIDS (Inherit SGT/APPID)

        network port (1)    ---- TD3 ---- network port (5)

 Note:  the script runs on SDK-6.5.26 or later SDK releases

 Test 1) Termination and Non-Canonical encapsulation

 */
cint_reset();
#include "srv6_common_functions.c"

int unit = 0;
int rv;
bcm_mac_t remote_mac_network = "22:33:44:55:66:70";
bcm_mac_t local_mac_network_in = "20:20:00:00:02:00";
bcm_mac_t local_mac_network_out = "20:22:33:44:55:60";

bcm_vlan_t vid_network_from = 90;
bcm_vlan_t vid_network_to = 400;
bcm_if_t intf_id_network_in = 1;
bcm_if_t intf_id_network_out;
bcm_if_t l3_iif_id = 100;
bcm_vrf_t vrf = 10;

bcm_port_t port_network_from = 1;
bcm_port_t port_network_to = 5;
bcm_gport_t gport_network_to = BCM_GPORT_INVALID;

bcm_if_t egr_obj_network_out;
/* 0x715010dcffe6d9209ed59a4c */
bcm_ip6_t dip6 = {0x71,0x50,0x10,0xdc,0xff,0xe6,0xd9,0x20,0x9e,0xd5,0x9a,0x4c,0x00,0x00,0x00,0x00};
/* 0x001800010002000000040005000603f0 */
bcm_ip6_t sid1 = {0x00,0x18,0x00,0x01,0x00,0x02,0x00,0x00,0x00,0x04,0x00,0x05,0x00,0x06,0x03,0xf0};
uint32 flow_handle;
uint32 egr_flow_option;
uint32 intf_flow_option;

print bcm_flow_handle_get(unit, "SRV6",&flow_handle);
print bcm_flow_option_id_get(unit, flow_handle, "L3_ASSIGN_SRH_BASE_HDR_AND_OVERLAY_ACTIONS", &egr_flow_option);
print bcm_port_gport_get(unit, port_network_to, &gport_network_to);

/* Only test TUNNEL_ADAPT_1(VLAN_XLATE_1) table */
int tunnel_adapt_tbl_valid[3] = {1, 0, 0};
int srh_action[3] = {BCM_SWITCH_SRV6_SRH_ACTION_TERM, 0, 0};
int dip_pfx_len[3] = {BCM_SWITCH_SRV6_DIP_PREFIX_96BIT, 0, 0};

/* Global switch control setting */
rv = do_srv6_decap_global_setting(unit, tunnel_adapt_tbl_valid, srh_action, NULL, dip_pfx_len, NULL, NULL);
printf("Do SRv6 global setting for Termination...... %s\n", bcm_errmsg(rv));

/* VLAN settings */
rv = do_srv6_encap_global_setting(unit);
printf("Do SRv6 global setting for Encapsulation ...... %s\n", bcm_errmsg(rv));

rv = port_srv6_enable(unit, port_network_from, TRUE);
printf("Port %d SRv6 setting ...... %s\n", port_network_from, bcm_errmsg(rv));
rv = port_srv6_enable(unit, port_network_to, TRUE);
printf("Port %d SRv6 setting ...... %s\n", port_network_to, bcm_errmsg(rv));


rv = vlan_create_add_ports(unit, vid_network_from, 1, &port_network_from);
printf("Create network VLAN %d and add port %d for incoming packet...... %s\n", vid_network_from, port_network_from, bcm_errmsg(rv));
rv = vlan_create_add_ports(unit, vid_network_to, 1, &port_network_to);
printf("Create network VLAN %d and add port %d for outgoing packet...... %s\n", vid_network_to, port_network_to, bcm_errmsg(rv));
rv = vlan_create_remove_port(unit, 1, port_network_from);
printf("Remove port %d from default VLAN ...... %s\n", port_network_from, bcm_errmsg(rv));
rv = vlan_create_remove_port(unit, 1, port_network_to);
printf("Remove port %d from default VLAN ...... %s\n", port_network_to, bcm_errmsg(rv));

print bcm_switch_control_set(unit, bcmSwitchL3IngressMode, 1);
bcm_l3_ingress_t l3_ingress;
bcm_l3_ingress_t_init(&l3_ingress);
l3_ingress.flags = BCM_L3_INGRESS_REPLACE;
l3_ingress.vrf = vrf;
print bcm_l3_ingress_create(unit, &l3_ingress, &l3_iif_id);
printf("bcm_l3_ingress_create() L3_IIF index=%d\n", l3_iif_id);

/* Create incoming L3 interface */
rv = create_l3_intf_vrf(unit, local_mac_network_in, vid_network_from, &intf_id_network_in, NULL);
printf("Create a L3 interface for incoming network port ...... %s\n", bcm_errmsg(rv));

/* Create outgoing L3 interface */
rv = create_l3_interface(unit, local_mac_network_out, vid_network_to, vrf, 0, 0, &intf_id_network_out);
printf("Create a L3 interface for outgoing network port ...... %s\n", bcm_errmsg(rv));

/* Egress object for outgoing network port */
rv = create_egr_obj(unit, BCM_L3_KEEP_TTL, 0, flow_handle, egr_flow_option, intf_id_network_out,
                    remote_mac_network, gport_network_to, vid_network_to, &egr_obj_network_out);
printf("Create an egress object ..==> ID (0x%x %d).... %s\n", egr_obj_network_out, egr_obj_network_out, bcm_errmsg(rv));

/* Create tunnel terminator */
rv = flow_tunnel_term_create(unit, "ADAPT_1_LOOKUP_DIP_ASSIGN_SVP_L3_IIF_AND_TUNNEL_TERM", dip6, l3_iif_id);
printf("Create a Tunnel terminator ...... %s\n", bcm_errmsg(rv));

rv = add_to_l2_table(unit, local_mac_network_out, vid_network_to, port_network_from);
printf("Add L2 table for Station-Move...... %s\n", bcm_errmsg(rv));

/* L3 Host setup */
rv = add_to_l3_host6_table(unit, vrf, egr_obj_network_out, sid1);
printf("Create a L3 IPv6 Host entry ...... %s\n", bcm_errmsg(rv));


/* attach the srh and sid list to the above tunnel
 * The tunnel_id must come from above tunnel API
 */
bcm_ip6_t ip6_addr[3];

bcm_ip6_t ip6_seg0 = {0x00,0x18,0x00,0x01,0x00,0x02,0x00,0x00,0x00,0x04,0x00,0x05,0x16,0xc1,0xc4,0x8a};
bcm_ip6_t ip6_seg1 = {0x00,0x18,0x00,0x01,0x00,0x02,0x00,0x00,0x00,0x04,0x00,0x05,0x00,0x06,0x03,0xf1};
bcm_ip6_t ip6_seg2 = {0x00,0x18,0x00,0x01,0x00,0x02,0x00,0x00,0x00,0x04,0x00,0x05,0x00,0x06,0x03,0xf2};
ip6_addr[0]=ip6_seg0;
ip6_addr[1]=ip6_seg1;
ip6_addr[2]=ip6_seg2;

bcm_flow_tunnel_srv6_srh_t srh_info;
srh_info.next_hdr = 6;   /* IPv6, Next Hdr in the SRH */
srh_info.flags = BCM_FLOW_TUNNEL_SRV6_SRH_TYPE_STD_SID |
                 BCM_FLOW_TUNNEL_SRV6_SRH_DIP_AS_LAST_SID;
srh_info.egr_obj = egr_obj_network_out;
srh_info.valid_elements = BCM_FLOW_TUNNEL_SRV6_SRH_FLAGS_VALID |
                          BCM_FLOW_TUNNEL_SRV6_SRH_NEXT_HDR_VALID |
                          BCM_FLOW_TUNNEL_SRV6_SRH_EGR_OBJ_VALID;
print bcm_flow_tunnel_srv6_srh_add(unit, 0, &srh_info, 3, ip6_addr);

prepare_port_for_tx(unit, port_network_from);
prepare_port_for_tx(unit, port_network_to);
prepare_packet_trace_for_tx(unit);
char *pkt_data = "2020000002002021222324268100A05A86DD6040000100BC2B3E2195016258F56D100E09CF0FF2B1CEDC715010DCFFE6D9209ED59A4C16C1C48A2902040000000000715010DCFFE6D9209ED59A4C16C1C48A60C00014007C063FE8FD8994436604FA9E29C703003D1557001800010002000000040005000603F01000200000000001000000005000000000000000000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F404142434445464748494A4B4C4D4E4F505152535455565758595A5B5C5D5E5F606162636465000315077D6A";
print send_packet(unit, port_network_from, pkt_data);

/* Expected output
BCM.0> [bcmPW.0]
[bcmPW.0]Packet[1]: data[0000]: {223344556670} {202233445560} 8100 a190
[bcmPW.0]Packet[1]: data[0010]: 86dd 60c0 0014 00b4 2b3f e8fd 8994 4366
[bcmPW.0]Packet[1]: data[0020]: 04fa 9e29 c703 003d 1557 0018 0001 0002
[bcmPW.0]Packet[1]: data[0030]: 0000 0004 0005 16c1 c48a 0006 0402 0200
[bcmPW.0]Packet[1]: data[0040]: 0000 0000 0000 0000 0000 0018 0001 0000
[bcmPW.0]Packet[1]: data[0050]: 0000 0018 0001 0002 0000 0004 0005 0006
[bcmPW.0]Packet[1]: data[0060]: 03f1 0018 0001 0002 0000 0004 0005 16c1
[bcmPW.0]Packet[1]: data[0070]: c48a 1000 2000 0000 0001 0000 0000 5000
[bcmPW.0]Packet[1]: data[0080]: 0000 0000 0000 0001 0203 0405 0607 0809
[bcmPW.0]Packet[1]: data[0090]: 0a0b 0c0d 0e0f 1011 1213 1415 1617 1819
[bcmPW.0]Packet[1]: data[00a0]: 1a1b 1c1d 1e1f 2021 2223 2425 2627 2829
[bcmPW.0]Packet[1]: data[00b0]: 2a2b 2c2d 2e2f 3031 3233 3435 3637 3839
[bcmPW.0]Packet[1]: data[00c0]: 3a3b 3c3d 3e3f 4041 4243 4445 4647 4849
[bcmPW.0]Packet[1]: data[00d0]: 4a4b 4c4d 4e4f 5051 5253 5455 5657 5859
[bcmPW.0]Packet[1]: data[00e0]: 5a5b 5c5d 5e5f 6061 6263 6465 0003 1507
[bcmPW.0]Packet[1]: data[00f0]: 7d6a 4330 5c16
[bcmPW.0]Packet[1]: length 246 (246). rx-port 5. cos 2. prio_int 5. vlan 400. reason 0x20. Outer tagged.
[bcmPW.0]Packet[1]: dest-port 144. dest-mod 1. src-port 5. src-mod 0. opcode 2.  matched 0. classification-tag 0.
[bcmPW.0]Packet[1]: reasons: L2Move
*/

/*
Ethernet II
    Destination: 22:33:44:55:66:70 (22:33:44:55:66:70)
    Source: 20:22:33:44:55:60 (20:22:33:44:55:60)
802.1Q Virtual LAN
    .... 0001 1001 0000 = ID: 400
    Type: IPv6 (0x86dd)
Source Address: e8fd:8994:4366:4fa:9e29:c703:3d:1557
Destination Address: 18:1:2:0:4:5:16c1:c48a
Routing Header for IPv6 (Segment Routing)
    Next Header: IPv6 Hop-by-Hop Option (0)
    Length: 6
    Length: 56 bytes
    Type: Segment Routing (4)
    Segments Left: 2
    Last Entry: 2
    Flags: 0x00
    Tag: 0000
    Address[0]: ::18:1:0:0
    Address[1]: 18:1:2:0:4:5:6:3f1
    Address[2]: 18:1:2:0:4:5:16c1:c48a
*/

