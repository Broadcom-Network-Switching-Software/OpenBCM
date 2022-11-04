/*
 ABSTRACT:
 SRv6 Transit for L2-IPv6-SRH-IPv4 with Standard SID When incoming ipv6 hop limit = 0/1 pkts need to be dropped.

 EXPECTS: for VXLT1 lookup,
          Set 1 Ingress pkt.IPv6hlimit = 2: packets will be routed based on END.T function and received at port 1
          Set 3 Ingress pkt.IPv6hlimit = 1: packets will be dropped
          Set 4 Ingress pkt.IPv6hlimit = 0: packets will be dropped

        network port     ---- TD3 ---- access port

 IPv4 in IPv6 with SRH header in GSID format    Transit via Host table

 Note:  the script runs on SDK-6.5.26 or later SDK releases

 Test 1) Transit
 Input to network, output on access

 */

cint_reset();

#include "srv6_common_functions.c"

int unit = 0;
int rv;
bcm_mac_t src_mac = "00:00:00:00:33:00";
bcm_ip6_t dip6 = {0,0x18,0,1,0,2,0,0,0,4,0,5,0,6,3,0xf0};
bcm_ip6_t sid1 = {0,0x18,0,1,0,2,0,0,0,4,0,5,0,6,3,0xf1};

bcm_mac_t payload_dst_mac = "00:00:00:00:11:00";
bcm_mac_t payload_src_mac = "00:00:00:00:22:00";
bcm_vlan_t vid_acc = 20;
bcm_vlan_t vid_network_1 = 30;
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

/* Only test TUNNEL_ADAPT_1(VLAN_XLATE_1) table */
int tunnel_adapt_tbl_valid[3] = {1, 0, 0};
int srh_action[3] = {BCM_SWITCH_SRV6_SRH_ACTION_TRANSIT, 0, 0};
int dip_pfx_len[3] = {BCM_SWITCH_SRV6_DIP_PREFIX_96BIT, 0, 0};

/* Global switch control setting */
rv = do_srv6_decap_global_setting(unit, tunnel_adapt_tbl_valid, srh_action, NULL, dip_pfx_len, NULL, NULL);
printf("SRv6 global setting ...... %s\n", bcm_errmsg(rv));

rv = port_srv6_enable(unit, port_network_1, TRUE);
printf("Port %d SRv6 setting ...... %s\n", port_network_1, bcm_errmsg(rv));

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


uint32 flow_handle;
uint32 match_sip_flow_option;

print bcm_flow_handle_get(0,"SRV6",&flow_handle);
print bcm_flow_option_id_get(0,flow_handle,
            "ADAPT_1_LOOKUP_DIP_ASSIGN_SVP_L3_IIF_AND_TUNNEL_TERM",
             &match_sip_flow_option);

bcm_flow_match_config_t info;
bcm_flow_match_config_t_init(&info);
info.flow_handle = flow_handle;
info.flow_option = match_sip_flow_option;
info.criteria = BCM_FLOW_MATCH_CRITERIA_DIP;
info.dip6 = dip6;
info.intf_id = intf_id_network_1;
info.tunnel_action = bcmFlowTunnelSrv6ActionTransitStdSid;
info.valid_elements = BCM_FLOW_MATCH_DIP_V6_VALID |
                      BCM_FLOW_MATCH_TNL_ACTION_VALID |
                      BCM_FLOW_MATCH_IIF_VALID;
print bcm_flow_match_add(unit, &info, 0, NULL);


print prepare_port_for_tx(unit, port_acc);
print prepare_port_for_tx(unit, port_network_1);
print prepare_packet_trace_for_tx(unit);

printf("Sending a packet with Hop limit = 3, should see a copy-to-cpu packet\n");
char *pkt_data = "0000000033002021222324258100001e86dd6040000100bc2b0300040003000200010001000200030004001800010002000000040005000603f20406040202000000001800010002000000040005000603f0001800010002000000040005000603f1001800010002000000040005000603f245380084000000004006520c101a0b0a0a0a03031000200000000001000000005000000000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f5051525354555657585900029ea9d3fc";
print send_packet(unit, port_network_1, pkt_data);
bshell(0, "sleep 2");
bshell(0, "cstat set 5 rx 9 RIPD6");

printf("Sending a packet with Hop limit = 1, should see a packet drop\n");
pkt_data = "0000000033002021222324258100001e86dd6040000100bc2b0100040003000200010001000200030004001800010002000000040005000603f20406040202000000001800010002000000040005000603f0001800010002000000040005000603f1001800010002000000040005000603f245380084000000004006520c101a0b0a0a0a03031000200000000001000000005000000000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f5051525354555657585900029ea9d3fc";
print send_packet(unit, port_network_1, pkt_data);
printf("Sending a packet with Hop limit = 0, should see a packet drop\n");
pkt_data = "0000000033002021222324258100001e86dd6040000100bc2b0000040003000200010001000200030004001800010002000000040005000603f20406040202000000001800010002000000040005000603f0001800010002000000040005000603f1001800010002000000040005000603f245380084000000004006520c101a0b0a0a0a03031000200000000001000000005000000000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f5051525354555657585900029ea9d3fc";
print send_packet(unit, port_network_1, pkt_data);

bshell(0, "sleep 2");
bshell(0, "cstat 5");


/* Expected output
[bcmPW.0]
[bcmPW.0]Packet[1]: data[0000]: {000000001100} {000000002200} 8100 0014
[bcmPW.0]Packet[1]: data[0010]: 86dd 6040 0001 00bc 2b02 0004 0003 0002
[bcmPW.0]Packet[1]: data[0020]: 0001 0001 0002 0003 0004 0018 0001 0002
[bcmPW.0]Packet[1]: data[0030]: 0000 0004 0005 0006 03f1 0406 0401 0200
[bcmPW.0]Packet[1]: data[0040]: 0000 0018 0001 0002 0000 0004 0005 0006
[bcmPW.0]Packet[1]: data[0050]: 03f0 0018 0001 0002 0000 0004 0005 0006
[bcmPW.0]Packet[1]: data[0060]: 03f1 0018 0001 0002 0000 0004 0005 0006
[bcmPW.0]Packet[1]: data[0070]: 03f2 4538 0084 0000 0000 4006 520c 101a
[bcmPW.0]Packet[1]: data[0080]: 0b0a 0a0a 0303 1000 2000 0000 0001 0000
[bcmPW.0]Packet[1]: data[0090]: 0000 5000 0000 0000 0000 0001 0203 0405
[bcmPW.0]Packet[1]: data[00a0]: 0607 0809 0a0b 0c0d 0e0f 1011 1213 1415
[bcmPW.0]Packet[1]: data[00b0]: 1617 1819 1a1b 1c1d 1e1f 2021 2223 2425
[bcmPW.0]Packet[1]: data[00c0]: 2627 2829 2a2b 2c2d 2e2f 3031 3233 3435
[bcmPW.0]Packet[1]: data[00d0]: 3637 3839 3a3b 3c3d 3e3f 4041 4243 4445
[bcmPW.0]Packet[1]: data[00e0]: 4647 4849 4a4b 4c4d 4e4f 5051 5253 5455
[bcmPW.0]Packet[1]: data[00f0]: 5657 5859 0002 9ea9 d3fc 6ed6 613c
[bcmPW.0]Packet[1]: length 254 (254). rx-port 1. cos 0. prio_int 0. vlan 20. reason 0x20. Outer tagged.
[bcmPW.0]Packet[1]: dest-port 20. dest-mod 0. src-port 1. src-mod 0. opcode 2.  matched 0. classification-tag 0.
[bcmPW.0]Packet[1]: reasons: L2Move
Sending a packet with Hop limit = 1, should see a packet drop
Packet from data=<>, length=254
int $$ = 0 (0x0)
Sending a packet with Hop limit = 0, should see a packet drop
Packet from data=<>, length=254
int $$ = 0 (0x0)
Sleeping for 2 seconds

+------------------Programmable Statistics Counters[Port  ce1]------------------+
| Type | No. |       Value       |                Enabled For                   |
+-------------------------------------------------------------------------------+
|  RX  | 0(R)|                  2| RIPD4 RIPD6 RPORTD PDISC                     |
|      |     |                   | RDROP RxVlanMismatch RxVlanMemberMismatch RxTpidMismatch RxPrivateVlanMismatch                                              |
|      | 9   |                  2| RIPD6                                        |
+-------------------------------------------------------------------------------+
*/
