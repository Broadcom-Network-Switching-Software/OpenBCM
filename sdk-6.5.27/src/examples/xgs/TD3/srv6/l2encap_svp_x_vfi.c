/*
ABSTRACT:
  SRv6 L2 tunnel (switch) encapsulation with unicast payload (Insertion of 3 SIDs).
  Using SVP and VFI cross-connect

        network port     ---- TD3 ---- access port

 Note:  the script runs on SDK-6.5.26 or later SDK releases

 Test 1) Encapsulation
 Input to access, output on network

 */
cint_reset();
#include "srv6_common_functions.c"

int unit = 0;
int rv;
bcm_mac_t remote_mac_network_1 = "00:00:00:00:00:02";
bcm_mac_t local_mac_network = "00:00:00:00:22:22";

bcm_mac_t payload_remote_mac1 = "00:00:00:00:aa:aa";

bcm_vlan_t vid_acc = 21;
bcm_vlan_t vid_network_1 = 22;
bcm_if_t intf_id_network_1 = 2;
bcm_vrf_t vrf = 0;
int option;

bcm_port_t port_acc = 1;
bcm_port_t port_network_1 = 5;

bcm_gport_t gport_acc = BCM_GPORT_INVALID;
bcm_gport_t gport_network_1 = BCM_GPORT_INVALID;

print bcm_port_gport_get(unit, port_acc, &gport_acc);
print bcm_port_gport_get(unit, port_network_1, &gport_network_1);

bcm_if_t egr_obj_network1;
bcm_vpn_t vpn;
int acc_flow_port;
int net_flow_port;
bcm_ip6_t tnl_local_ip = {0x1,1,1,1,0x35,0,0x70,0,0,0,0x10,0x9,8,7,6,5};
bcm_ip6_t tnl_remote_ip1 = {0x2,2,2,2,0x36,0,0x70,0,0,0,0x1,0x2,3,4,5, 0xf0};
uint32 flow_handle;
uint32 egr_flow_option;
uint32 intf_flow_option;
uint32 dvp_flow_option;
uint32 tnl_flow_option;
uint32 encap_flow_option;

/* VLAN settings */
print bcm_vlan_control_port_set(unit, port_acc, bcmVlanTranslateIngressEnable, 1);
print bcm_switch_control_set(unit, bcmSwitchSrv6FunctionSelect1, BCM_SWITCH_SRV6_FUNCTION_OFFSET_64_SIZE_32BIT);
rv = do_srv6_encap_global_setting(unit);
printf("Do SRv6 global setting for Encapsulation ...... %s\n", bcm_errmsg(rv));

rv = vlan_create_add_ports(unit, vid_acc, 1, &port_acc);
printf("Create VLAN %d and add port %d ...... %s\n", vid_acc, port_acc, bcm_errmsg(rv));
rv = vlan_create_add_ports(unit, vid_network_1, 1, &port_network_1);
printf("Create VLAN %d and add port %d ...... %s\n", vid_network_1, port_network_1, bcm_errmsg(rv));
rv = vlan_create_remove_port(unit, 1, port_acc);
printf("Remove port %d from default VLAN ...... %s\n", port_acc, bcm_errmsg(rv));
rv = vlan_create_remove_port(unit, 1, port_network_1);
printf("Remove port %d from default VLAN ...... %s\n", port_network_1, bcm_errmsg(rv));

print bcm_flow_handle_get(unit, "SRV6",&flow_handle);
print bcm_flow_option_id_get(unit, flow_handle, "L2_MAC_ADDR_IPV6", &egr_flow_option);
print bcm_flow_option_id_get(unit, flow_handle, "L2_IPV6", &intf_flow_option);
/* L3 interfaces */
rv = create_l3_interface(unit, local_mac_network, vid_network_1, vrf, flow_handle,
                         intf_flow_option, &intf_id_network_1);
printf("Create a L3 interface for network port ...... %s\n", bcm_errmsg(rv));

/* Egress object for access */
rv = create_egr_obj(unit, 0, BCM_L3_FLOW_ONLY, flow_handle, egr_flow_option, intf_id_network_1,
                    remote_mac_network_1, gport_network_1, vid_network_1, &egr_obj_network1);
printf("Create an egress object ...... %s\n", bcm_errmsg(rv));

/* VPN create */
bcm_multicast_t mcast_net;
print flow_vpn_group_create(unit, &vpn, &mcast_net, BCM_FLOW_VPN_ELINE, 0, 0);

/* access VP creation*/
rv = create_flow_acc_vp(unit, vpn, 0, &acc_flow_port);
printf("Create Access VP ...... %s\n", bcm_errmsg(rv));

/* network VP creation */
rv = create_flow_acc_vp(unit, vpn, BCM_FLOW_PORT_NETWORK | BCM_FLOW_PORT_DVP_TABLE_EXTENSION, &net_flow_port);
printf("Create Access VP ...... %s\n", bcm_errmsg(rv));

/* L2 address setup */
rv = add_to_l2_table(unit, payload_remote_mac1, vpn, net_flow_port);
printf("Added VP port to L2 table ...... %s\n", bcm_errmsg(rv));

rv = add_to_l2_table(unit, local_mac_network, vid_network_1, port_acc);
printf("Add L2 table for Station-Move...... %s\n", bcm_errmsg(rv));


print bcm_flow_option_id_get(unit, flow_handle, "L2_EVXLT_KEY_TYPE_DVP_VFI", &dvp_flow_option);
print bcm_flow_option_id_get(unit, flow_handle, "L2_LOOKUP_DVP_VFI_ASSIGN_FUNCTION", &encap_flow_option);

/* access port side */
bcm_flow_match_config_t info;
bcm_flow_match_config_t_init(&info);

/* generic: port match criteria, flow_handle must be 0, error check */
/* port/vlan criteria */
info.criteria = BCM_FLOW_MATCH_CRITERIA_PORT_VLAN;
info.port = gport_acc;
info.flow_port = acc_flow_port;
info.vlan = vid_acc;
info.valid_elements = BCM_FLOW_MATCH_PORT_VALID |
                      BCM_FLOW_MATCH_VLAN_VALID |
                      BCM_FLOW_MATCH_FLOW_PORT_VALID;
print bcm_flow_match_add(0,&info,0,NULL);

/* network port side */
/* flow_port encap set. Set the flow type matching the tunnel */
bcm_flow_port_encap_t peinfo;
bcm_flow_port_encap_t_init(&peinfo);

peinfo.flow_handle = flow_handle;
peinfo.flow_option = dvp_flow_option;
peinfo.flow_port = net_flow_port;
peinfo.egress_if = egr_obj_network1;
peinfo.valid_elements = BCM_FLOW_PORT_ENCAP_PORT_VALID |
                        BCM_FLOW_PORT_ENCAP_EGRESS_IF_VALID;
print bcm_flow_port_encap_set(0,&peinfo,0,NULL);

/* encap add, VFI-->VNID */
bcm_flow_encap_config_t einfo;
uint32 vnid = 0x12345;
bcm_flow_encap_config_t_init(&einfo);

einfo.flow_handle = flow_handle;
einfo.flow_option = encap_flow_option;
einfo.criteria = BCM_FLOW_ENCAP_CRITERIA_VFI_DVP;
einfo.function = vnid;
einfo.vpn = vpn;
einfo.flow_port = net_flow_port;
einfo.valid_elements = BCM_FLOW_ENCAP_FUNCTION_VALID |
                       BCM_FLOW_ENCAP_FLOW_PORT_VALID |
                      BCM_FLOW_ENCAP_VPN_VALID;
print bcm_flow_encap_add(0,&einfo,0,NULL);

/* create a vxlan IP tunnel for the given flow port */
bcm_flow_tunnel_initiator_t tiinfo;
bcm_flow_tunnel_initiator_t_init(&tiinfo);
tiinfo.flow_handle = flow_handle;
tiinfo.valid_elements = BCM_FLOW_TUNNEL_INIT_DIP6_VALID |
                       BCM_FLOW_TUNNEL_INIT_SIP6_VALID |
                       BCM_FLOW_TUNNEL_INIT_DSCP_VALID |
                       BCM_FLOW_TUNNEL_INIT_TTL_VALID;

tiinfo.flow_port = net_flow_port;
tiinfo.type = bcmTunnelTypeEthSR6;
tiinfo.sip6 = tnl_local_ip;
tiinfo.dip6 = tnl_remote_ip1;
tiinfo.dscp_sel = bcmTunnelDscpAssign;
tiinfo.dscp = 9;
tiinfo.ttl = 0xff;
tiinfo.l3_intf_id = intf_id_network_1;
print bcm_flow_tunnel_initiator_create(0,&tiinfo,0,NULL);

/* attach the srh and sid list to the above tunnel
 * The tunnel_id must come from above tunnel API
 */
bcm_ip6_t ip6_addr[3];

bcm_ip6_t ip6_seg0 = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
bcm_ip6_t ip6_seg1 = {0x2,2,2,2,0x36,0,0x70,0,0,0,0x1,0x2,3,4,5,0xf1};
bcm_ip6_t ip6_seg2 = {0x2,2,2,2,0x36,0,0x70,0,0,0,0x1,0x2,3,4,5,0xf2};
ip6_addr[0]=ip6_seg0;
ip6_addr[1]=ip6_seg1;
ip6_addr[2]=ip6_seg2;

bcm_flow_tunnel_srv6_srh_t srh_info;
srh_info.next_hdr = 143;   /* ethernet, Next Hdr in the SRH */
srh_info.tag = 3;          /* Tag field in the SRH */
srh_info.flags = BCM_FLOW_TUNNEL_SRV6_SRH_CANONICAL |
                 BCM_FLOW_TUNNEL_SRV6_SRH_TYPE_STD_SID;
option = BCM_FLOW_TUNNEL_SRV6_SRH_OPTION_WITH_ID;
srh_info.tunnel_id = tiinfo.tunnel_id;
srh_info.valid_elements = BCM_FLOW_TUNNEL_SRV6_SRH_FLAGS_VALID |
                          BCM_FLOW_TUNNEL_SRV6_SRH_TAG_VALID;

/* 3 SIDs are supported only if the outer and inner header is untagged */
print bcm_flow_tunnel_srv6_srh_add(unit, option, &srh_info, 2, ip6_addr);


prepare_port_for_tx(unit, port_acc);
prepare_port_for_tx(unit, port_network_1);
prepare_packet_trace_for_tx(unit);
char *pkt_data = "00000000aaaa00000000bbbb81000015002e00000000000000001234569c1234569d1234569e1234569f123456a0123456a1123456a2123456a3123456a41234c5a7b74e";
print send_packet(unit, port_acc, pkt_data);

/* Expected output
[bcmPW.0]Packet[1]: data[0000]: {000000000002} {000000002222} 8100 0016
[bcmPW.0]Packet[1]: data[0010]: 86dd 6240 0000 006c 2bff 0101 0101 3500 
[bcmPW.0]Packet[1]: data[0020]: 7000 0000 1009 0807 0605 0202 0202 3600 
[bcmPW.0]Packet[1]: data[0030]: 7000 0000 0102 0304 05f0 8f04 0401 0100 
[bcmPW.0]Packet[1]: data[0040]: 0003 0202 0202 3600 7000 0000 0102 0304 
[bcmPW.0]Packet[1]: data[0050]: 05f1 0202 0202 3600 7000 0000 0102 0304 
[bcmPW.0]Packet[1]: data[0060]: 05f0 0000 0000 aaaa 0000 0000 bbbb 8100 
[bcmPW.0]Packet[1]: data[0070]: 0015 002e 0000 0000 0000 0000 1234 569c 
[bcmPW.0]Packet[1]: data[0080]: 1234 569d 1234 569e 1234 569f 1234 56a0 
[bcmPW.0]Packet[1]: data[0090]: 1234 56a1 1234 56a2 1234 56a3 1234 56a4 
[bcmPW.0]Packet[1]: data[00a0]: 1234 c5a7 b74e 6606 8f17 
[bcmPW.0]Packet[1]: length 170 (170). rx-port 5. cos 0. prio_int 0. vlan 22. reason 0x20. Outer tagged.
[bcmPW.0]Packet[1]: dest-port 22. dest-mod 0. src-port 5. src-mod 0. opcode 2.  matched 0. classification-tag 0.
[bcmPW.0]Packet[1]: reasons: L2Move
*/
