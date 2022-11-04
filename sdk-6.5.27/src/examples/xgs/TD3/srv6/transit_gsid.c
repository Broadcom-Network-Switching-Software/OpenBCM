/*
ABSTRACT:
  SRv6 Transit for L2-IPv6-SRH-IPv4 for lookup in L3_ENTRY with G-SID.

        network port     ---- TD3 ---- access port

 IPv4 in IPv6 with SRH header in GSID format    Transit via Host table

 Note:  the script runs on SDK-6.5.26 or later SDK releases

 Test 1) Transit
 Input to network, output on access

 */

cint_reset();

#include "srv6_common_functions.c"

int rv;
int unit = 0;
bcm_mac_t src_mac = "10:11:22:33:44:55";
bcm_ip6_t dip6 = {0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xfa,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
bcm_ip6_t sid1 = {0x12,0x34,0x56,0x78,0xa4,0xb4,0xa5,0xb5,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

bcm_mac_t payload_dst_mac = "00:00:00:00:11:00";
bcm_mac_t payload_src_mac = "00:00:00:00:22:00";

bcm_vlan_t vid_acc = 20;
bcm_vlan_t vid_network_1 = 200;
bcm_if_t intf_id_acc = 1;
bcm_if_t intf_id_network_1 = 2;
bcm_vrf_t vrf = 20;

bcm_port_t port_acc = 1;
bcm_port_t port_network_1 = 5;


bcm_gport_t gport_acc = BCM_GPORT_INVALID;
bcm_gport_t gport_network_1 = BCM_GPORT_INVALID;

print bcm_port_gport_get(unit, port_acc, &gport_acc);
print bcm_port_gport_get(unit, port_network_1, &gport_network_1);

bcm_if_t egr_obj_acc1;



/* Only test TUNNEL_ADAPT_2(MPLS_ENTRY_DOUBLE) table */
int tunnel_adapt_tbl_valid[3] = {0, 1, 0};
int srh_action[3] = {0, BCM_SWITCH_SRV6_SRH_ACTION_TRANSIT, 0};
int dip_pfx_len[3] = {0, BCM_SWITCH_SRV6_DIP_PREFIX_64BIT, 0};

/* Global switch control setting */
rv = do_srv6_decap_global_setting(unit, tunnel_adapt_tbl_valid, srh_action, NULL, dip_pfx_len, NULL, NULL);
printf("SRv6 global setting ...... %s\n", bcm_errmsg(rv));

rv = port_srv6_enable(unit, port_network_1, TRUE);
printf("Port %d SRv6 setting ...... %s\n", port_network_1, bcm_errmsg(rv));

/* Set SRV6_CSID_MODE.MODE = 2 */
rv = bcm_switch_control_set(unit, bcmSwitchSrv6SidMode, BCM_SWITCH_SRV6_CSID_MODE_GSID);
/* Set SRV6_GSID_CONFIG.PREFIX_1_SIZE = 1 */
rv = bcm_switch_control_set(unit, bcmSwitchSrv6GsidPrefix1, BCM_SWITCH_SRV6_GSID_PREFIX_32BIT);
printf("GSID setting ...... %s\n", bcm_errmsg(rv));

/* VLAN settings */
rv = vlan_create_add_ports(unit, vid_acc, 1, &port_acc);
printf("Create VLAN %d and add port %d ...... %s\n", vid_acc, port_acc, bcm_errmsg(rv));
rv = vlan_create_add_ports(unit, vid_network_1, 1, &port_network_1);
printf("Create VLAN %d and add port %d ...... %s\n", vid_network_1, port_network_1, bcm_errmsg(rv));
rv = vlan_create_remove_port(unit, 1, port_acc);
printf("Remove port %d from default VLAN ...... %s\n", port_acc, bcm_errmsg(rv));
rv = vlan_create_remove_port(unit, 1, port_network_1);
printf("Remove port %d from default VLAN ...... %s\n", port_network_1, bcm_errmsg(rv));

rv = add_to_l2_table(unit, payload_src_mac, vid_acc, port_network_1);
printf("Add a static L2 entry ...... %s\n", bcm_errmsg(rv));

/* Network port L3 interface */
rv = create_l3_intf_vrf(unit, src_mac, vid_network_1, &intf_id_network_1, &vrf);
printf("Create a L3 interface with VRF mapping for network port ...... %s\n", bcm_errmsg(rv));

/* Access port L3 interface */
rv = create_l3_interface(unit, payload_src_mac, vid_acc, 0, 0, 0, &intf_id_acc);
printf("Create a L3 interface for access port ...... %s\n", bcm_errmsg(rv));

/* Egress object for access */
rv = create_egr_obj(unit, 0, 0, 0, 0, intf_id_acc, payload_dst_mac, gport_acc, vid_acc, &egr_obj_acc1);
printf("Create an egress object ...... %s\n", bcm_errmsg(rv));

/* L3 host setup */
rv = add_to_l3_host6_table(unit, vrf, egr_obj_acc1, sid1);
printf("Create a L3 IPv6 Host entry ...... %s\n", bcm_errmsg(rv));


/* Flow API configuration */
uint32 flow_handle;
uint32 match_sip_flow_option;

print bcm_flow_handle_get(unit, "SRV6", &flow_handle);
print bcm_flow_option_id_get(unit, flow_handle,
             "ADAPT_2_LOOKUP_DIP_ASSIGN_SVP_L3_IIF_AND_TUNNEL_TERM",
             &match_sip_flow_option);

bcm_flow_match_config_t info;
bcm_flow_match_config_t_init(&info);
info.flow_handle = flow_handle;
info.flow_option = match_sip_flow_option;
info.criteria = BCM_FLOW_MATCH_CRITERIA_DIP;
info.dip6 = dip6;
info.intf_id = intf_id_network_1;
info.tunnel_action = bcmFlowTunnelSrv6ActionTransitGsid;
info.valid_elements = BCM_FLOW_MATCH_DIP_V6_VALID |
                      BCM_FLOW_MATCH_TNL_ACTION_VALID |
                      BCM_FLOW_MATCH_IIF_VALID;
print bcm_flow_match_add(0,&info,0,NULL);


print prepare_port_for_tx(unit, port_acc);
print prepare_port_for_tx(unit, port_network_1);
print prepare_packet_trace_for_tx(unit);
char *pkt_data = "1011223344552021222324258100A0C886DD6040000100BC2B6300040003000200010001000200030004123456789ABCDEFAAABBCCDD000000030406040202000000001800010002000000040005000603F0001800010002000000040005000603F1AB11AB22A6B6A7B7A4B4A5B5AABBCCDD45380084000000004006520C101A0B0A0A0A03031000200000000001000000005000000000000000000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F404142434445464748494A4B4C4D4E4F5051525354555657585900030C3CCA1A";
print send_packet(unit, port_network_1, pkt_data);

/* Expected packet output
[bcmPW.0]Packet[1]: data[0000]: {000000001100} {000000002200} 8100 a014
[bcmPW.0]Packet[1]: data[0010]: 86dd 6040 0001 00bc 2b62 0004 0003 0002
[bcmPW.0]Packet[1]: data[0020]: 0001 0001 0002 0003 0004 1234 5678 a4b4
[bcmPW.0]Packet[1]: data[0030]: a5b5 aabb ccdd 0000 0002 0406 0402 0200
[bcmPW.0]Packet[1]: data[0040]: 0000 0018 0001 0002 0000 0004 0005 0006
[bcmPW.0]Packet[1]: data[0050]: 03f0 0018 0001 0002 0000 0004 0005 0006
[bcmPW.0]Packet[1]: data[0060]: 03f1 ab11 ab22 a6b6 a7b7 a4b4 a5b5 aabb
[bcmPW.0]Packet[1]: data[0070]: ccdd 4538 0084 0000 0000 4006 520c 101a
[bcmPW.0]Packet[1]: data[0080]: 0b0a 0a0a 0303 1000 2000 0000 0001 0000
[bcmPW.0]Packet[1]: data[0090]: 0000 5000 0000 0000 0000 0001 0203 0405
[bcmPW.0]Packet[1]: data[00a0]: 0607 0809 0a0b 0c0d 0e0f 1011 1213 1415
[bcmPW.0]Packet[1]: data[00b0]: 1617 1819 1a1b 1c1d 1e1f 2021 2223 2425
[bcmPW.0]Packet[1]: data[00c0]: 2627 2829 2a2b 2c2d 2e2f 3031 3233 3435
[bcmPW.0]Packet[1]: data[00d0]: 3637 3839 3a3b 3c3d 3e3f 4041 4243 4445
[bcmPW.0]Packet[1]: data[00e0]: 4647 4849 4a4b 4c4d 4e4f 5051 5253 5455
[bcmPW.0]Packet[1]: data[00f0]: 5657 5859 0003 0c3c ca1a 8f0c dc71
[bcmPW.0]Packet[1]: length 254 (254). rx-port 1. cos 2. prio_int 5. vlan 20. reason 0x20. Outer tagged.
[bcmPW.0]Packet[1]: dest-port 20. dest-mod 0. src-port 1. src-mod 0. opcode 2.  matched 0. classification-tag 0.
[bcmPW.0]Packet[1]: reasons: L2Move
*/
