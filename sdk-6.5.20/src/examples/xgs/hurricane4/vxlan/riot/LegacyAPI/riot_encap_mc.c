/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : riot encap multicast flow
 *
 * Usage    : BCM.0> cint riot_encap_mc.c
 *
 * config   : BCM56275_A1-RIOT-LEGACY.bcm
 *
 * Log file : riot_encap_mc_log.txt
 *
 * Test Topology :
 *
 *                              +-----------------+
 *                              |                 |> access port 2
 *               access port 1  |       SWITCH    |> access port 3
 *   30.30.30.1 11:11:11        |                 |>-- Network port 1 VFI --> VxLAN Encap --> tunnel
 * 224.0.10.1 01:00:5e:00:10:01 +-----------------+
 *
 *						                                  VxLAN Tunnel SIP: 10.10.10.1 DIP: 224.0.0.10
 *                               											  SA: bb:bb	   DA: 01:00:5e:00:00:0A
 *									                                 		   VLAN: 11
 *
 * Summary:
 * ========
 *   This Cint example to show configuration of riot mc mode
 *
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup ():
 *   =============================================
 *      a) Select 3 ports as 3 access_ports, another 1 port1 as 1 network ports
 *
 *      b)
 *
 *      c) All ports are configured in loopback mode and IFP rules are installed
 *         on all ports to copy all packets ingressing these ports to CPU.
 *
 *      d) On egress ports an additional action is installed to drop the packets
 *         to avoid loopback.
 *
 *      e) Start packet watcher.
 *
 *   2) Step2 - Configuration ():
 *   ========================================================================
 *     a)
 *     b) run riot_encap_mc.c, to create riot VPN.
 *     c) Necessary SOC properties:
 *
 *       config add riot_enable=1
 *       config add riot_overlay_l3_intf_mem_size=6144
 *       config add riot_overlay_l3_egress_mem_size=24576
 *       config add l3_ecmp_levels=2
 *       config add riot_overlay_ecmp_resilient_hash_size=64
 *
 *     BCM56275_A1-RIOT-LEGACY.bcm have included the properties
 *   3) Step3 - Verification ():
 *   ===========================================

 *
 *       tx 1 PortBitMap=[access port 1 ] data=01005E0010010000001111118100000A0800450000320000000040FF53AD1E1E1E01E0000A01000000000000000000000000000000000000000000000000000000000000
 *
 *       Packet TX into access port 1
 *       01005E0010010000001111118100000A
 *       0800450000320000000040FF53AD1E1E
 *       1E01E0000A0100000000000000000000
 *       00000000000000000000000000000000
 *       00000000C847075F
 *
 *
 *       Expected Result:
 *       1. Packets out of network port 1
 *
 *       Route + VxLAN
 *       {01005e00000a} {00000000bbbb} 8100 000b
 *       0800 4500 0068 0000 0000 0a11 bc70 0a0a
 *       0a01 e000 000a ffff 2118 0054 0000 0800
 *       0000 1213 4500 0100 5e00 1001 0000 0000
 *       0201 8100 000c 0800 4500 0032 0000 0000
 *       3fff 54ad 1e1e 1e01 e000 0a01 0000 0000
 *       0000 0000 0000 0000 0000 0000 0000 0000
 *       0000 0000 0000 0000 0000 7cc2 6fc4
 *
 *       Switch + VxLAN
 *       {01005e00000a} {00000000bbbb} 8100 000b
 *       0800 4500 0068 0001 0000 0a11 bc6f 0a0a
 *       0a01 e000 000a ffff 2118 0054 0000 0800
 *       0000 0070 1000 0100 5e00 1001 0000 0011
 *       1111 8100 000a 0800 4500 0032 0000 0000
 *       40ff 53ad 1e1e 1e01 e000 0a01 0000 0000
 *       0000 0000 0000 0000 0000 0000 0000 0000
 *       0000 0000 0000 0000 0000 a0bb 69c9
 *
 *       2. Packet out of access port 2
 *
 *       {01005e001001} {000000000002} 8100 000d
 *       0800 4500 0032 0000 0000 3fff 54ad 1e1e
 *       1e01 e000 0a01 0000 0000 0000 0000 0000
 *       0000 0000 0000 0000 0000 0000 0000 0000
 *       0000 0000 80eb 9da9
 *
 *       3. Packet out of access port 3
 *
 *       {01005e001001} {000000111111} 8100 000a
 *       0800 4500 0032 0000 0000 40ff 53ad 1e1e
 *       1e01 e000 0a01 0000 0000 0000 0000 0000
 *       0000 0000 0000 0000 0000 0000 0000 0000
 *       0000 0000 c847 075f
 */



cint_reset();


/**************************   1 basic function begin*************************************/
/* Create vlan and add port to vlan */


int vlan_create_add_port(int unit, int vlan, int port)
{
    bcm_pbmp_t pbmp, upbmp;
    bcm_error_t rv = BCM_E_NONE;

    rv =  bcm_vlan_create(unit, vlan);
	if (BCM_E_NONE != rv)
	{
		printf("bcm_vlan_create failed %s\n", bcm_errmsg(rv));
		return rv;
	}
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    rv = bcm_vlan_port_add(unit, vlan, pbmp, upbmp);
	if (BCM_E_NONE != rv)
	{
		printf("bcm_vlan_port_add failed %s\n", bcm_errmsg(rv));
		return rv;
	}

	printf("vlan_create_add_port SUCCESS \n");

    return rv;
}

int vxlan_riot_system_config(int unit, uint16 udp_dst_port, int enable_hash)
{
    bcm_error_t rv = BCM_E_NONE;

    /* L4 Port for VxLAN */
    rv =  bcm_switch_control_set(unit, bcmSwitchVxlanUdpDestPortSet, udp_dst_port);
	if (BCM_E_NONE != rv)
	{
		printf("bcm_switch_control_set bcmSwitchVxlanUdpDestPortSet failed %s\n", bcm_errmsg(rv));
		return rv;
	}
    /* enable Hash */
    rv =  bcm_switch_control_set(unit, bcmSwitchVxlanEntropyEnable, enable_hash);
	if (BCM_E_NONE != rv)
	{
		printf("bcm_switch_control_set bcmSwitchVxlanEntropyEnable  failed %s\n", bcm_errmsg(rv));
		return rv;
	}
    /* Enable L3EgressMode */
    rv =  bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1);
	if (BCM_E_NONE != rv)
	{
		printf("bcm_switch_control_set  bcmSwitchL3EgressMode failed %s\n", bcm_errmsg(rv));
		return rv;
	}
    /* Enable L3IngressMode */
    rv =  bcm_switch_control_set(unit, bcmSwitchL3IngressMode, 1);
	if (BCM_E_NONE != rv)
	{
		printf("bcm_switch_control_set  bcmSwitchL3IngressMode failed %s\n", bcm_errmsg(rv));
		return rv;
	}
    /* Enable switch ingress intf map set */
    rv =  bcm_switch_control_set(unit, bcmSwitchL3IngressInterfaceMapSet, 1);
	if (BCM_E_NONE != rv)
	{
		printf("bcm_switch_control_set failed bcmSwitchL3IngressInterfaceMapSet %s\n", bcm_errmsg(rv));
		return rv;
	}
    /* Enable ipmc */
    rv =  bcm_ipmc_enable(unit, 1);
	if (BCM_E_NONE != rv)
	{
		printf("bcm_ipmc_enable failed %s\n", bcm_errmsg(rv));
		return rv;
	}

	printf("vxlan_riot_system_config SUCCESS \n");

    return rv;
}

/*
 * Routine to enable Vxlan Access Port Config
 */
int vxlan_access_port_config(int unit, bcm_port_t port)
{
    bcm_error_t rv = BCM_E_NONE;

    rv = bcm_port_control_set(unit, port, bcmPortControlVxlanEnable, 0x0);
	if (BCM_E_NONE != rv)
	{
		printf("bcm_port_control_set  bcmPortControlVxlanEnable failed %s\n", bcm_errmsg(rv));
		return rv;
	}
    rv = bcm_port_control_set(unit, port, bcmPortControlVxlanTunnelbasedVnId, 0x0);
	if (BCM_E_NONE != rv)
	{
		printf("bcm_port_control_set  bcmPortControlVxlanTunnelbasedVnId failed %s\n", bcm_errmsg(rv));
		return rv;
	}
    rv = bcm_vlan_control_port_set(unit, port, bcmVlanTranslateIngressEnable, 1);
	if (BCM_E_NONE != rv)
	{
		printf("bcm_port_control_set bcmVlanTranslateIngressEnable failed %s\n", bcm_errmsg(rv));
		return rv;
	}

	printf("vxlan_access_port_config SUCCESS \n");
    return rv;
}

/*
 * Routine to enable Vxlan Network Port Config
 */
int vxlan_network_port_config(int unit, bcm_port_t port)
{
    bcm_error_t rv = BCM_E_NONE;

    rv = bcm_port_control_set(unit, port, bcmPortControlVxlanEnable, 0x1);
	if (BCM_E_NONE != rv)
	{
		printf("bcm_port_control_set bcmPortControlVxlanEnable failed %s\n", bcm_errmsg(rv));
		return rv;
	}

    rv = bcm_port_control_set(unit, port, bcmPortControlVxlanTunnelbasedVnId, 0x0);
	if (BCM_E_NONE != rv)
	{
		printf("bcm_port_control_set bcmPortControlVxlanTunnelbasedVnId failed %s\n", bcm_errmsg(rv));
		return rv;
	}


	printf("vxlan_network_port_config SUCCESS \n");

    return rv;
}

/*
 * vxlan init and term tunnel create
 */
bcm_error_t vxlan_tunnel_init_term_create(int unit, bcm_ip_t sip_init, bcm_ip_t dip_init,
                                  bcm_ip_t sip_term, bcm_ip_t dip_term, int ttl,
                                  uint16 udp_src_port, uint16 udp_dst_port, bcm_gport_t *tnl_id)
{
  bcm_error_t rv = BCM_E_NONE;

  bcm_tunnel_initiator_t tnl_init;
  bcm_tunnel_initiator_t_init(&tnl_init);
  bcm_tunnel_terminator_t tnl_term;
  bcm_tunnel_terminator_t_init(&tnl_term);

  tnl_init.ttl = ttl;
  tnl_init.udp_src_port = udp_src_port;
  tnl_init.udp_dst_port = udp_dst_port;
  tnl_init.sip = sip_init;
  tnl_init.dip = dip_init;
  tnl_init.flags  = 0;
  tnl_init.type = bcmTunnelTypeVxlan;

  /* create vxlan tunnel init */
  BCM_IF_ERROR_RETURN(bcm_vxlan_tunnel_initiator_create(unit, &tnl_init));
  printf("bcm_vxlan_tunnel_initiator_create() %s\n", bcm_errmsg(rv));


  *tnl_id = tnl_init.tunnel_id;
  tnl_term.flags = BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
  tnl_term.tunnel_id  = *tnl_id;
  tnl_term.type = bcmTunnelTypeVxlan;
  tnl_term.sip = sip_term;
  tnl_term.dip = dip_term;
  tnl_term.sip_mask = 0xffffffff;
  tnl_term.dip_mask = 0xffffffff;

  /* create vxlan tunnel term */
  rv = bcm_vxlan_tunnel_terminator_create(unit, &tnl_term);
  printf("bcm_vxlan_tunnel_terminator_create() %s\n", bcm_errmsg(rv));

  return rv;
}

/*
 * Routine to Create a Virtual port Group
 */
int vxlan_vpn_group_create(int unit, bcm_vpn_t *vpn, bcm_multicast_t *mcast, uint32 vnid,
                       uint32 flags, uint32 egress_service_tpid, bcm_vlan_t egress_service_vlan)
{
    bcm_error_t rv = BCM_E_NONE;

    rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_VXLAN, mcast);
    printf("bcm_multicast_create() %s mc group=%d\n", bcm_errmsg(rv), *mcast);
    if (rv != BCM_E_NONE) {
        return rv;
    }

    /* to add Vxlan VPN - vxlan_vpn_add */
    bcm_vxlan_vpn_config_t vpn_t;
    bcm_vxlan_vpn_config_t_init(&vpn_t);

    vpn_t.broadcast_group = *mcast;
    vpn_t.unknown_multicast_group = *mcast;
    vpn_t.unknown_unicast_group = *mcast;
    vpn_t.vpn = *vpn;
    flags |= (BCM_VXLAN_VPN_ELAN | BCM_VXLAN_VPN_WITH_VPNID);
    vpn_t.vnid = vnid;
    vpn_t.flags = flags;
    if (egress_service_tpid != 0) {
        vpn_t.egress_service_tpid = egress_service_tpid;
        vpn_t.egress_service_vlan = egress_service_vlan;
    }

    rv = bcm_vxlan_vpn_create(unit, &vpn_t);
    *vpn = vpn_t.vpn;
    printf("bcm_vxlan_vpn_create() %s vpn=0x%x\n", bcm_errmsg(rv), *vpn);

    return rv;
}

/*
 * Routine to update vlan profile
 */
int vlan_vfi_profile_update(int unit, bcm_vlan_t vlan, bcm_vrf_t vrf, bcm_if_t intf)
{
    bcm_error_t rv = BCM_E_NONE;

    /* get vlan control structure */
    bcm_vlan_control_vlan_t vlan_profile;
    bcm_vlan_control_vlan_t_init(&vlan_profile);
    rv = bcm_vlan_control_vlan_get(unit, vlan, &vlan_profile);
    printf("bcm_vlan_control_vlan_get() %s\n", bcm_errmsg(rv));

    /* modify the profile */
    if (vrf != -1) {
        vlan_profile.vrf = vrf;
    }

    if (intf != BCM_IF_INVALID) {
        vlan_profile.ingress_if = intf;
    }

    /* update profile */
    rv = bcm_vlan_control_vlan_set(unit, vlan, vlan_profile);
    printf("bcm_vlan_control_vlan_set() %s\n", bcm_errmsg(rv));

    return rv;
}



int l3_egress_create(int unit, uint32 flags, bcm_if_t intf, bcm_gport_t port,
                     bcm_mac_t mac_addr, bcm_if_t *egr_obj_id)
{
    int rv = 0;
    bcm_l3_egress_t egr_t;
    bcm_l3_egress_t_init(&egr_t);
    egr_t.port = port;
    egr_t.intf = intf;
    egr_t.flags = flags;
    egr_t.mac_addr = mac_addr;

    rv = bcm_l3_egress_create(unit, 0, egr_t, egr_obj_id);
    printf("bcm_l3_egress_create() %s egr_obj_id=0x%x\n", bcm_errmsg(rv), *egr_obj_id);
}

int vxlan_port_create(int unit, bcm_vpn_t vpn, bcm_port_t gport, int flags,
                      bcm_if_t egress_if,
                      bcm_vxlan_port_match_t criteria, bcm_vlan_t match_vlan,
                      bcm_gport_t match_tunnel_id, bcm_gport_t egress_tunnel_id,
                      bcm_vlan_t egress_service_vlan, uint32 egress_service_tpid,
                      bcm_gport_t *vxlan_port)
{
    int rv = 0;

    bcm_vxlan_port_t port_t;
    bcm_vxlan_port_t_init(&port_t);

    port_t.flags = flags;

    if (criteria != BCM_VXLAN_PORT_MATCH_INVALID)
        port_t.criteria = criteria;

    if (match_tunnel_id != BCM_GPORT_INVALID)
        port_t.match_tunnel_id = match_tunnel_id;

    if (egress_tunnel_id != BCM_GPORT_INVALID)
        port_t.egress_tunnel_id = egress_tunnel_id;

    if (egress_service_vlan != BCM_VLAN_INVALID) {
        port_t.egress_service_vlan = egress_service_vlan;
        port_t.egress_service_tpid = egress_service_tpid;
    }

    if (gport != BCM_GPORT_INVALID)
        port_t.match_port = gport;

    if (match_vlan != BCM_VLAN_INVALID)
        port_t.match_vlan = match_vlan;

    port_t.egress_if = egress_if;

    rv = bcm_vxlan_port_add(unit, vpn, &port_t);
    printf("bcm_vxlan_port_add() %s\n", bcm_errmsg(rv));
    *vxlan_port = port_t.vxlan_port_id;

    return rv;
}

/*
 * Wrapper to add an L3 interface.
 */
int l3_intf_create(int unit, int flags, bcm_mac_t mac_addr, bcm_vlan_t vid, bcm_if_t *intf_id)
{
    int rv = 0;

    /* init strucuture and configure members */
    bcm_l3_intf_t intf_t;
    bcm_l3_intf_t_init(&intf_t);

    sal_memcpy(intf_t.l3a_mac_addr, mac_addr, sizeof(mac_addr));
    intf_t.l3a_vid = vid;
	intf_t.l3a_flags = flags;
	intf_t.l3a_intf_id = *intf_id;
    rv = bcm_l3_intf_create(unit, &intf_t);
    printf("bcm_l3_intf_create() %s\n", bcm_errmsg(rv));
    *intf_id = intf_t.l3a_intf_id;

    return rv;
}

/*
 * Routine to create L2 Station Entry
 */
int l2_station_add(int unit, bcm_mac_t mac_addr, bcm_vlan_t vlan, bcm_vlan_t vlan_mask, int flags, int *station_id)
{
    int rv = 0;

    /* vars */
    bcm_l2_station_t l2_t;
    bcm_l2_station_t_init(&l2_t);

    /* Parse input */
    l2_t.dst_mac = mac_addr;
    l2_t.vlan = vlan;
    l2_t.vlan_mask = vlan_mask;
    l2_t.dst_mac_mask = "FF:FF:FF:FF:FF:FF";
    l2_t.flags = flags;

    rv = bcm_l2_station_add(unit, station_id, &l2_t);
    printf("bcm_l2_station_add() %s\n", bcm_errmsg(rv));
    return rv;
}

/*
 * Wrapper for L3 Route Creation
 */
int l3_route_add(int unit, bcm_ip_t ip_addr, bcm_ip_t ip_mask, bcm_vrf_t vrf, bcm_if_t egr_obj)
{
    int rv = 0;

    bcm_l3_route_t route_t;
    bcm_l3_route_t_init(&route_t);

    route_t.l3a_subnet = ip_addr;
    route_t.l3a_ip_mask = ip_mask;
    route_t.l3a_intf = egr_obj;
    route_t.l3a_vrf = vrf;

    rv = bcm_l3_route_add(unit, &route_t);
    printf("bcm_l3_route_add() %s\n", bcm_errmsg(rv));

    return rv;
}

/*
 * Update termination tunnel
 */
int vxlan_tunnel_term_update(int unit, bcm_gport_t tnl_id, uint32 mflags)
{
	int rv = 0;

	bcm_tunnel_terminator_t tnl_info;
	bcm_tunnel_terminator_t_init(&tnl_info);
	tnl_info.tunnel_id = tnl_id;
	print bcm_vxlan_tunnel_terminator_get(unit, &tnl_info);
	tnl_info.multicast_flag = mflags;
	tnl_info.flags |= BCM_TUNNEL_TERM_TUNNEL_WITH_ID;

	rv = bcm_vxlan_tunnel_terminator_update(unit, &tnl_info);

	return rv;
}

/*
 *  Routine to build IPv4 packet
 *
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |Version|  IHL  |Type of Service|          Total Length         |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |         Identification        |Flags|      Fragment Offset    |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |  Time to Live |    Protocol   |         Header Checksum       |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                       Source Address                          |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                    Destination Address                        |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                    Options                    |    Padding    |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
void tx_ipv4_packet_from_cpu(int unit, int vlan_tag,
                             bcm_mac_t src_mac, bcm_mac_t dst_mac,
                             bcm_ip_t src_ip, bcm_ip_t dst_ip, bcm_port_t dest_port)
{
    bcm_pkt_t *pkt;
    int pkt_size = 68; /* including VLAN Tag if BCM_PKT_F_NO_VTAG = 0 */
    int flags = BCM_TX_CRC_APPEND;

    /* EtherType(2B) Version+IHL+TOS(2B) Total Length (2B) + Identification (2B)
       Flags+Fragment offset(2B) TTL(1B: TTL=0x10) Protocol(1B: UDP=0x11) Header Checksum(2B) */
    char ipv4_ether_type[2] = {0x08, 0x00};
    /* ipv4.total_length = 68 - 12 - 2 - 4 - 4 = 46; */
    char ipv4_hdr[24]    = {0x45, 0x00, 0x00, 0x2E,
                            0x00, 0x00, 0x00, 0x00,
                            0x40, 0xFF, 0xDA, 0x7E,
                            0xC0, 0xA8, 0x0A, 0x01,
                            0xC0, 0xA8, 0x14, 0x01};

    if (BCM_E_NONE != bcm_pkt_alloc(unit, pkt_size, flags, &pkt)) {
        printf("TX: Failed to allocate packets\n");
    }

    /* packet init */
    sal_memset(pkt->_pkt_data.data, 0x00, pkt_size);

    sal_memcpy(ipv4_hdr+12, &src_ip, 4);
    sal_memcpy(ipv4_hdr+16, &dst_ip, 4);

    BCM_PKT_HDR_DMAC_SET(&pkt, dst_mac);
    BCM_PKT_HDR_SMAC_SET(&pkt, src_mac);
    BCM_PKT_HDR_TPID_SET(&pkt, 0x8100);
    BCM_PKT_HDR_VTAG_CONTROL_SET(&pkt, vlan_tag);
    sal_memcpy(pkt->pkt_data->data+16, ipv4_ether_type, 2);
    sal_memcpy(pkt->pkt_data->data+18, ipv4_hdr, 24);
    BCM_PKT_PORT_SET(&pkt, dest_port, 0, 0);
    pkt->call_back = NULL;

    if (BCM_E_NONE != bcm_tx(unit, pkt, NULL)) {
        printf("bcm tx error\n");
    }

    bcm_pkt_free(unit, pkt);
}

/*
 *  Routine to build VxLAN packet
 *
 *    MAC_DA(6B) MAC_SA(6B) outer VLAN(4B, optional)
 *    Ethertype(2B, 0x0800) IPv4 header(20B, without options)
 *    UDP source port (2B) UDP dest port (2B)
 *    UDP length (2B) UDP checksum (2B)
 *
 *    VXLAN Header
 *
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   | FFFFIFFF (1)  | Reserved      |          Reserved             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |         VNID (3B)                             | Reserved      |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *    (1) F [7b] ?V Flag bits, currently reserved, it is configurable on TD2.
 *        I [1b] ?V Identifier bit, indicate the definition of this frame format
 *                 (always set to 1) and that the VN_ID is valid.
 *
 *    VN_ID [24b] ?V Virtual Network ID, Identifies the Virtual L2 Network.
 *    Reserved_1 [24b], Reserved_2 [8b] ?V These bits are currently undefined, configurable on TD2.
 */

void tx_vxlan_packet_from_cpu(int unit, bcm_mac_t ul_dst_mac, bcm_mac_t ul_src_mac,
                              bcm_ip_t ul_dip, bcm_ip_t ul_sip,
                              uint16 ul_udp_dst_port, uint16 ul_udp_src_port,
                              uint32 vnid, int ul_vlan_tag,
                              bcm_mac_t ol_dst_mac, bcm_mac_t ol_src_mac,
                              bcm_ip_t ol_dip, bcm_ip_t ol_sip,
                              int ol_vlan_tag, bcm_port_t dest_port)
{
    bcm_pkt_t *pkt;
    int pkt_size = 122; /* 68 ipv4 packet size + 54 vxlan tunnel header size */
    int flags = BCM_TX_CRC_APPEND;

    /*
       ul_ipv4_hdr: IPv4 header(20B, without options)
                    UDP source port (2B) UDP dest port (2B)
                    UDP length (2B) UDP checksum (2B)
     */

    char ul_ipv4_ether_type[2] = {0x08, 0x00};
    /* ipv4.total_length = 122 - 12 - 2 - 4 - 4 = 104 (0x68); */
    char ul_ipv4_hdr[28] = {0x45, 0x00, 0x00, 0x64,
                            0x00, 0x00, 0x00, 0x00,
                            0x3F, 0x11, 0x3F, 0x6A,
                            0x14, 0x14, 0x14, 0x01,
                            0x0A, 0x0A, 0x0A, 0x01,
                            0xFF, 0xFF, 0x21, 0x18,
                            0x00, 0x3A, 0x00, 0x00 };

    char vxlan_header[8] = {0x08, 0x00, 0x00, 0x00,
                            0x01, 0x23, 0x45, 0x00};

    /* EtherType(2B) Version+IHL+TOS(2B) Total Length (2B) + Identification (2B)
       Flags+Fragment offset(2B) TTL(1B: TTL=0x10) Protocol(1B: UDP=0x11) Header Checksum(2B) */
    char ol_ipv4_ether_type[2] = {0x08, 0x00};
    char ol_ipv4_hdr[24] = {0x45, 0x00, 0x00, 0x2A,
                            0x00, 0x00, 0x00, 0x00,
                            0x40, 0xFF, 0xDA, 0x82,
                            0xC0, 0xA8, 0x14, 0x01,
                            0xC0, 0xA8, 0x0A, 0x01,
                            0x00, 0x00, 0x00, 0x00};
    if (BCM_E_NONE != bcm_pkt_alloc(unit, pkt_size, flags, &pkt)) {
        printf("TX: Failed to allocate packets\n");
    }

    /* packet init */
    sal_memset(pkt->_pkt_data.data, 0x00, pkt_size);

    /* ul_mac */
    BCM_PKT_HDR_DMAC_SET(&pkt, ul_dst_mac);
    BCM_PKT_HDR_SMAC_SET(&pkt, ul_src_mac);
    BCM_PKT_HDR_TPID_SET(&pkt, 0x8100);
    BCM_PKT_HDR_VTAG_CONTROL_SET(&pkt, ul_vlan_tag);

    /* ul_ip */
    sal_memcpy(ul_ipv4_hdr+12, &ul_sip, 4);
    sal_memcpy(ul_ipv4_hdr+16, &ul_dip, 4);
    sal_memcpy(ul_ipv4_hdr+20, &ul_udp_src_port, 2);
    sal_memcpy(ul_ipv4_hdr+22, &ul_udp_dst_port, 2);

    sal_memcpy(pkt->pkt_data->data+16, ul_ipv4_ether_type, 2);
    sal_memcpy(pkt->pkt_data->data+18, ul_ipv4_hdr, 28);

    /* vxlan_header */
    int vnid_temp = vnid << 8;
    sal_memcpy(vxlan_header+4, &vnid_temp, 3);
    sal_memcpy(pkt->pkt_data->data+46, vxlan_header, 8);

    /* ol_dst_mac, ol_src_mac */
    sal_memcpy(pkt->pkt_data->data+54, ol_dst_mac, 6);
    sal_memcpy(pkt->pkt_data->data+60, ol_src_mac, 6);

    char ol_tpid[2] = {0x81, 0x00};
    sal_memcpy(pkt->pkt_data->data+66, ol_tpid, 2);
    ol_vlan_tag <<= 16;
    sal_memcpy(pkt->pkt_data->data+68, &ol_vlan_tag, 2);

    /* ol_ip */
    sal_memcpy(ol_ipv4_hdr+12, &ol_sip, 4);
    sal_memcpy(ol_ipv4_hdr+16, &ol_dip, 4);

    sal_memcpy(pkt->pkt_data->data+70, ol_ipv4_ether_type, 2);
    sal_memcpy(pkt->pkt_data->data+72, ol_ipv4_hdr, 24);

    BCM_PKT_PORT_SET(&pkt, dest_port, 0, 0);
    pkt->call_back = NULL;

    if (BCM_E_NONE != bcm_tx(unit, pkt, NULL)) {
        printf("bcm tx error\n");
    }

    bcm_pkt_free(unit, pkt);
}



/**************************1 basic function end*************************************/

/**************************2 global variable defination begin***********************/

bcm_error_t rv = BCM_E_NONE;
uint16 udp_dp = 8472;   /* UDP dst port used for OTV */

bcm_port_t access_port_1  = BCM_PORT_INVALID;
bcm_port_t access_port_2  = BCM_PORT_INVALID;
bcm_port_t access_port_3  = BCM_PORT_INVALID;
bcm_port_t network_port_1 = BCM_PORT_INVALID;


bcm_gport_t access_gport_1  = BCM_GPORT_INVALID;
bcm_gport_t access_gport_2  = BCM_GPORT_INVALID;
bcm_gport_t access_gport_3  = BCM_GPORT_INVALID;
bcm_gport_t network_gport_1 = BCM_GPORT_INVALID;

/* FP Group/Rule Creation/Destruction*/
bcm_field_group_config_t group_config;
bcm_field_entry_t access_port_1_eid = BCM_FIELD_ENTRY_INVALID;
bcm_field_entry_t access_port_2_eid = BCM_FIELD_ENTRY_INVALID;
bcm_field_entry_t access_port_3_eid = BCM_FIELD_ENTRY_INVALID;
bcm_field_entry_t network_port_1_eid = BCM_FIELD_ENTRY_INVALID;


/**************************2 global variable defination end ***********************/

/**************************3 test setup functions begin***********************/

bcm_error_t
create_ifp_group (int unit, bcm_field_group_config_t *group_config)
{
    bcm_error_t rv = BCM_E_NONE;
    /* FP group configuration and creation */
    bcm_field_group_config_t_init(group_config);
    BCM_FIELD_QSET_INIT(group_config->qset);
    BCM_FIELD_QSET_ADD(group_config->qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config->qset, bcmFieldQualifyInPort);
    group_config->mode = bcmFieldGroupModeAuto;
    rv = bcm_field_group_config_create(unit, group_config);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_group_config_create() : %s\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

bcm_error_t
create_ifp_to_cpu_rule (int unit, bcm_field_group_t gid, bcm_field_entry_t *eid, bcm_port_t port, int drop)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_port_t port_mask = 0xffffffff;
    /* FP entry configuration and creation */
    rv = bcm_field_entry_create(unit, gid, eid);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_entry_create() : %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_field_qualify_InPort(unit, *eid, port, port_mask);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_qualify_InPort() : %s\n", bcm_errmsg(rv));
        return rv;
    }
    /* FP entry actions configuration */
    if (1 == drop) {
        rv = bcm_field_action_add(unit, *eid, bcmFieldActionDrop, 0, 0);
        if(BCM_FAILURE(rv)) {
            printf("\nError in bcm_field_action_add() : %s\n", bcm_errmsg(rv));
            return rv;
        }
    }
    rv = bcm_field_action_add(unit, *eid, bcmFieldActionCopyToCpu, 0, 0);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_action_add() : %s\n", bcm_errmsg(rv));
        return rv;
    }
    /* Installing FP entry to FP TCAM */
    rv = bcm_field_entry_install(unit, *eid);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_entry_install() : %s\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

bcm_error_t
destroy_ifp_to_cpu_rule (int unit, bcm_field_entry_t eid)
{
    bcm_error_t rv = BCM_E_NONE;

    /* Destroying FP entry */
    rv = bcm_field_entry_destroy(unit, eid);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_entry_destroy() : %s\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}


/* This function is written so that hardcoding of port
   numbers in Cint scripts is removed. This function gives
   required number of ports
*/
bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{

  int i=0,port=0,rv=0;
  bcm_port_config_t configP;
  bcm_pbmp_t ports_pbmp;

  rv = bcm_port_config_get(unit,&configP);
  if(BCM_FAILURE(rv)) {
    printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
    return rv;
  }

  ports_pbmp = configP.e;
  for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
    if (BCM_PBMP_MEMBER(&ports_pbmp,i)&& (port < num_ports)) {
      port_list[port]=i;
      port++;
    }
  }

  if (( port == 0 ) || ( port != num_ports )) {
      printf("portNumbersGet() failed \n");
      return -1;
  }

  return BCM_E_NONE;

}

/*
 * Configures the port in loopback mode and discard setting to BCM_PORT_DISCARD_NONE.
 * Also add a rule to copy packets ingressing on the port to CPU.
 */
bcm_error_t ingress_port_setup(int unit, bcm_port_t port, bcm_field_entry_t *entry)
{
  int drop;
  bcm_error_t rv;
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_NONE));
  if (BCM_FIELD_ENTRY_INVALID != *entry)
  {
    rv = destroy_ifp_to_cpu_rule(unit, *entry);
    if(BCM_FAILURE(rv))
    {
        printf("\nError executing destroy_ifp_to_cpu_rule%s.\n",bcm_errmsg(rv));
        return rv;
    }
  }
  rv = create_ifp_to_cpu_rule(unit, group_config.group, entry, port, (drop = 0));
  if(BCM_FAILURE(rv))
  {
      printf("\nError executing create_ifp_to_cpu_rule(): %s.\n",bcm_errmsg(rv));
      return rv;
  }

  return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and discard setting to BCM_PORT_DISCARD_ALL.
 * Also add a rule to copy packets ingressing on the port to CPU and drop them
 * to avoid loopback.
 */

bcm_error_t egress_port_setup(int unit, bcm_port_t port, bcm_field_entry_t *entry)
{
  int drop;
  bcm_error_t rv;
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));
  if (BCM_FIELD_ENTRY_INVALID != *entry)
  {
    rv = destroy_ifp_to_cpu_rule(unit, *entry);
    if(BCM_FAILURE(rv))
    {
        printf("\nError executing destroy_ifp_to_cpu_rule%s.\n",bcm_errmsg(rv));
        return rv;
    }
  }
  rv = create_ifp_to_cpu_rule(unit, group_config.group, entry, port, (drop = 1));
  if(BCM_FAILURE(rv))
  {
      printf("\nError executing create_ifp_to_cpu_rule(): %s.\n",bcm_errmsg(rv));
      return rv;
  }

  return BCM_E_NONE;
}



/*   Select 3 ports and does ingess and egress port setting on respective ports.
 */
bcm_error_t test_setup(int unit)
{
  int port_list[4], i;
  bcm_field_entry_t entry[4] = {BCM_FIELD_ENTRY_INVALID, BCM_FIELD_ENTRY_INVALID, BCM_FIELD_ENTRY_INVALID,BCM_FIELD_ENTRY_INVALID};
  if (BCM_E_NONE != portNumbersGet(unit, port_list, 4)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }

  access_port_1 = port_list[0];
  access_port_2 = port_list[1];
  access_port_3 = port_list[2];
  network_port_1 = port_list[3];

  BCM_IF_ERROR_RETURN(create_ifp_group(unit, &group_config));
  /*
  printf("Using port number:%d as access_port_1\n", access_port_1);
  if (BCM_E_NONE != ingress_port_setup(unit, access_port_1, &entry[0])) {
    printf("ingress_port_setup() failed for port %d\n", access_port_1);
    return -1;
  }
  access_port_1_eid = entry[0];

  printf("Using port number:%d as access_port_2\n", access_port_2);
  if (BCM_E_NONE != ingress_port_setup(unit, access_port_2, &entry[1])) {
    printf("ingress_port_setup() failed for port %d\n", access_port_2);
    return -1;
  }
  access_port_2_eid = entry[1];

  for (i = 1; i <= 2; i++) {
    printf("Using port number:%d as network_port_%d\n", port_list[i+1], (i ));
    if (BCM_E_NONE != egress_port_setup(unit, port_list[i+1], &entry[i+1])) {
      printf("egress_port_setup() failed for port %d\n", port_list[i+1]);
      return -1;
    }
  }
  network_port_1_eid = entry[2];
  network_port_2_eid = entry[3];

*/
  bshell(unit, "pw start report +raw +decode");
  return BCM_E_NONE;
}

/**************************3 test setup functions end ***********************/

/**************************4 main functions begin ***********************/

/********************************************************************
*********************************************************************
*************** main script *********************
 */


bcm_error_t riot_encap_multicast(int unit )
{

	bcm_mac_t dummy_mac_addr    = "00:00:00:00:00:00";
	bcm_mac_t payload_smac      = "00:00:00:11:11:11";
	bcm_mac_t payload_dmac_mc   = "01:00:5e:00:10:01";
	bcm_mac_t acc_ol_mac        = "00:00:00:00:00:01";
	bcm_mac_t acc_ol_mac_1      = "00:00:00:00:00:02";
	bcm_mac_t acc_ol_nh_mac     = "00:00:00:11:11:11";
	bcm_mac_t net_ol_nh_mac     = "00:00:00:00:01:01";
	bcm_mac_t net_ol_mac        = "00:00:00:00:02:01";
	bcm_mac_t net_ul_mac        = "00:00:00:00:bb:bb";
	bcm_mac_t net_ul_mc_mac     = "01:00:5e:00:00:0A";

	bcm_vlan_t acc_vid          = 10;
	bcm_vlan_t acc_vid_1        = 13;
	bcm_vlan_t net_ul_vid       = 11;
	bcm_vlan_t net_ol_vid       = 12;
	int ttl                     = 0xA;
	bcm_vrf_t vrf               = 1;
	uint32 net_vpn_vnid         = 0x121345;
	uint32 acc_vpn_vnid         = 0x7010;
	uint16 udp_src_port         = 0xFFFF;
	uint16 udp_dst_port         = 0x2118;      /* 8472 */
	bcm_ip_t tunnel_remote_ip   = 0x14141401;  /* 20.20.20.1 */
	bcm_ip_t tunnel_local_ip    = 0x0a0a0a01;  /* 10.10.10.1 */
	bcm_ip_t payload_sip        = 0x1e1e1e01;  /* 30.30.30.1 */
	bcm_ip_t payload_dip_mc     = 0xE0000a01;  /* 224.0.10.1 */
	bcm_ip_t tunnel_remote_mc   = 0xE000000a;  /* 224.0.0.10 */
	bcm_if_t acc_ing_obj        = -1;
	bcm_if_t net_ing_obj        = -1;
	bcm_multicast_t ol_mc_group = -1;
	uint32 flags;
	int rv = 0;
	bcm_vlan_control_vlan_t vlan_info = {0};

	BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, access_port_1, &access_gport_1));
	BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, network_port_1, &network_gport_1));
	BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, access_port_2, &access_gport_2));

	printf("==== switch control set for vxlan ====\n");
	BCM_IF_ERROR_RETURN(vxlan_riot_system_config(unit, udp_dst_port, FALSE));

	printf("==== port control and vlan control set for vxlan port ====\n");
	BCM_IF_ERROR_RETURN(vxlan_access_port_config(unit, access_port_1));
	BCM_IF_ERROR_RETURN(vxlan_network_port_config(unit, network_port_1));

	printf("Enable ipmc\n");
	BCM_IF_ERROR_RETURN(bcm_ipmc_enable(unit, 1));

	printf("==== create vlan, add vlan port ====\n");
	BCM_IF_ERROR_RETURN(vlan_create_add_port(unit, acc_vid, access_port_1));
	BCM_IF_ERROR_RETURN(vlan_create_add_port(unit, net_ul_vid, network_port_1));
	BCM_IF_ERROR_RETURN(vlan_create_add_port(unit, acc_vid_1, access_port_2));

	/*
	 * Create vxlan VPNs/VFIs, Riot will routing between them
	 */
	printf("Access side VFI\n");
	bcm_vpn_t vpn_acc;
	bcm_multicast_t mcast_acc;
	flags = BCM_VXLAN_VPN_SERVICE_TAGGED;
	BCM_IF_ERROR_RETURN(vxlan_vpn_group_create(unit, &vpn_acc, &mcast_acc, acc_vpn_vnid, flags, 0, 0));
	printf("vpn_acc=0x%x\n", vpn_acc);

	printf("Network side VFI\n");
	bcm_vpn_t vpn_net;
	bcm_multicast_t mcast_net;
	uint32 net_tpid = 0x8100;
	flags = BCM_VXLAN_VPN_SERVICE_TAGGED |
	        BCM_VXLAN_VPN_SERVICE_VLAN_ADD;  /* BCM_VXLAN_VPN_SERVICE_VLAN_DELETE */
	BCM_IF_ERROR_RETURN(vxlan_vpn_group_create(unit, &vpn_net, &mcast_net, net_vpn_vnid, flags, net_tpid, net_ol_vid));
	printf("vpn_net=0x%x\n", vpn_net);

	/*
	 * Create Access vxlan VP
	 */
	printf("==== Create vxlan l3 egress - access UL with no UL l3 intf create required ====\n");
	bcm_if_t acc_egr_obj_ul;
	BCM_IF_ERROR_RETURN(l3_egress_create(unit, BCM_L3_VXLAN_ONLY, BCM_IF_INVALID, access_port_1,
	                 dummy_mac_addr, &acc_egr_obj_ul));

	printf("==== Create vxlan port - access vp ====\n");
	/* If BCM_VXLAN_PORT_MATCH_PORT_VLAN, then must enable bcmVlanTranslateIngressEnable */
	bcm_gport_t acc_vp;
	flags = BCM_VXLAN_PORT_SERVICE_TAGGED | BCM_VXLAN_PORT_SERVICE_VLAN_ADD;
	BCM_IF_ERROR_RETURN(vxlan_port_create(unit, vpn_acc, access_gport_1, 0 /* flags */,
	                       acc_egr_obj_ul,
	                       BCM_VXLAN_PORT_MATCH_PORT,  /* BCM_VXLAN_PORT_MATCH_PORT_VLAN */
	                       acc_vid, BCM_GPORT_INVALID, BCM_GPORT_INVALID,
	                       acc_vid, 0x8100,
	                       &acc_vp));
	printf("vxlan_port_create()  acc_vp=0x%x\n", acc_vp);

	/*
	 * Enable IPMC on access VP
	 */
	BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, acc_vp, bcmPortControlIP4Mcast, 1));


	/*
	 * Routing interface (access)
	 */
	printf("==== Create vxlan l3 egress - access OL ====\n");
	bcm_if_t acc_intf_ol = 2;/*the l3intf id must bigger  than or equal to 2 */

	BCM_IF_ERROR_RETURN(l3_intf_create(unit, BCM_L3_WITH_ID, acc_ol_mac, vpn_acc, &acc_intf_ol));
	printf("l3_intf_create() acc_intf_ol=0x%x\n", acc_intf_ol);

	bcm_if_t acc_egr_obj_ol;
	BCM_IF_ERROR_RETURN(l3_egress_create(unit, BCM_L3_IPMC, acc_intf_ol, acc_vp, dummy_mac_addr, &acc_egr_obj_ol));
	printf("l3_egress_create() acc_egr_obj_ol=0x%x\n", acc_egr_obj_ol);

	/*
	 * Create L3 ingress intf at access
	 */
	bcm_l3_ingress_t l3_ingress;
	bcm_l3_ingress_t_init(&l3_ingress);
	l3_ingress.flags = BCM_L3_INGRESS_WITH_ID;
	l3_ingress.vrf = vrf;
	acc_ing_obj = acc_intf_ol;

	//BCM_IF_ERROR_RETURN(bcm_l3_ingress_create(unit, &l3_ingress, &acc_ing_obj));
	rv = (bcm_l3_ingress_create(unit, &l3_ingress, &acc_ing_obj));

	if (BCM_E_NONE != rv ) {
		printf("bcm_l3_ingress_create() failed rv = %d\n", rv);
		return rv;
	}

	printf("bcm_l3_ingress_create()  acc_ing_obj=0x%x\n", acc_ing_obj);

    bcm_vlan_control_vlan_t_init(&vlan_info);
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_get(unit, vpn_acc, &vlan_info));
    vlan_info.ingress_if = acc_ing_obj;
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_set(unit, vpn_acc, vlan_info));


	printf("==== update VFI table vrf field ====\n");
	BCM_IF_ERROR_RETURN(vlan_vfi_profile_update(unit, vpn_acc, vrf, acc_ing_obj));



	/*
	 * Create network MC VP
	 */
	printf("==== Create vxlan l3 egress - network UL ====\n");
	bcm_if_t net_intf_ul;
	BCM_IF_ERROR_RETURN(l3_intf_create(unit, 0, net_ul_mac, net_ul_vid, &net_intf_ul));

	printf("l3_intf_create() net_intf_ul=0x%x\n", net_intf_ul);

	bcm_if_t net_egr_obj_ul;
	BCM_IF_ERROR_RETURN(l3_egress_create(unit, BCM_L3_VXLAN_ONLY | BCM_L3_IPMC, net_intf_ul,
	                 network_port_1, net_ul_mc_mac, &net_egr_obj_ul));
	printf("l3_egress_create() net_egr_obj_ul=0x%x\n", net_egr_obj_ul);

	printf("==== VxLAN tunnel setup ====\n");
	bcm_gport_t tnl_mc_id;
	BCM_IF_ERROR_RETURN(vxlan_tunnel_init_term_create(unit, tunnel_local_ip, tunnel_remote_mc,
	                                          tunnel_remote_ip, tunnel_remote_mc, ttl,
	                                          udp_src_port, udp_dst_port, &tnl_mc_id));
	printf("tnl_mc_id = 0x%x\n", tnl_mc_id);

	printf("==== Create vxlan port - network vp ====\n");
	bcm_gport_t net_vp;

	flags = BCM_VXLAN_PORT_NETWORK | BCM_VXLAN_PORT_EGRESS_TUNNEL |
	        BCM_VXLAN_PORT_SERVICE_TAGGED |
	        BCM_VXLAN_PORT_MULTICAST;
	BCM_IF_ERROR_RETURN(vxlan_port_create(unit, vpn_net, -1 /* port */, flags,
	                  net_egr_obj_ul,
	                  BCM_VXLAN_PORT_MATCH_VN_ID, BCM_VLAN_INVALID,
	                  tnl_mc_id, tnl_mc_id,
	                  BCM_VLAN_INVALID, 0 /* egress_service_tpid */,
	                  &net_vp));
	printf("vxlan_port_create()  net_vp=0x%x\n", net_vp);

	/*
	 * Routing interface (network)
	 */
	printf("==== Create Riot routing L3 egress - network OL over the network VP ====\n");
	bcm_if_t net_intf_ol;
	BCM_IF_ERROR_RETURN(l3_intf_create(unit, 0,net_ol_mac, vpn_net, &net_intf_ol));
	printf("l3_intf_create() net_intf_ol=0x%x\n", net_intf_ol);

	bcm_if_t net_egr_obj_ol;
	BCM_IF_ERROR_RETURN(l3_egress_create(unit, BCM_L3_IPMC, net_intf_ol, net_vp, dummy_mac_addr, &net_egr_obj_ol));
	printf("l3_egress_create() net_egr_obj_ol=0x%x\n", net_egr_obj_ol);

	/*
	 * Create L3 ingress intf at Network
	 */
	//bcm_l3_ingress_t_init(&l3_ingress);
	//l3_ingress.flags = BCM_L3_INGRESS_REPLACE;
	//l3_ingress.vrf = vrf;
	//net_ing_obj = net_intf_ol;
	//rv = bcm_l3_ingress_create(unit, &l3_ingress, &net_ing_obj);
	//printf("bcm_l3_ingress_create() %s net_ing_obj=0x%x\n", bcm_errmsg(rv), net_ing_obj);

	//printf("==== update VFI table vrf field ====\n");
	//vlan_vfi_profile_update(unit, vpn_net, vrf, net_ing_obj);

	/*
	 * Create IPMC
	 */
	printf("===create a l3 mc group - OL===\n");
	BCM_IF_ERROR_RETURN(bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L3, &ol_mc_group));

	/*
	 * Two copies will egress network port:
	 *  - L2 switched into the VXLAN tunnel
	 *  - L3 routed into the VXLAN tunnel
	 */
	bcm_if_t encap_id;
	BCM_IF_ERROR_RETURN(bcm_multicast_l3_encap_get(unit, ol_mc_group, network_gport_1, net_egr_obj_ol, &encap_id));
	BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, ol_mc_group, network_gport_1, encap_id));

	BCM_IF_ERROR_RETURN(bcm_multicast_egress_object_encap_get(unit, ol_mc_group, net_egr_obj_ul, &encap_id));
	BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, ol_mc_group, network_gport_1, encap_id));

	/*
	 * Access port 1 - routing interface on Network VFI
	 */

	/* Create Access vxlan VP 1 */
	printf("==== Create vxlan l3 egress - access UL with no UL l3 intf create required ====\n");
	bcm_if_t acc_egr_obj_ul_1;
	BCM_IF_ERROR_RETURN(l3_egress_create(unit, BCM_L3_VXLAN_ONLY, BCM_IF_INVALID, access_port_2,
	                 dummy_mac_addr, &acc_egr_obj_ul_1));

	printf("==== Create vxlan port - access vp 1 ====\n");
	bcm_gport_t acc_vp_1;
	flags = BCM_VXLAN_PORT_SERVICE_TAGGED | BCM_VXLAN_PORT_SERVICE_VLAN_ADD;
	BCM_IF_ERROR_RETURN(vxlan_port_create(unit, vpn_net, access_gport_2, flags,
	                       acc_egr_obj_ul_1,
	                       BCM_VXLAN_PORT_MATCH_PORT,  /* BCM_VXLAN_PORT_MATCH_PORT_VLAN */
	                       acc_vid_1, BCM_GPORT_INVALID, BCM_GPORT_INVALID,
	                       acc_vid_1, 0x8100, &acc_vp_1));
	printf("vxlan_port_create()  acc_vp_1=0x%x\n",  acc_vp_1);

	printf("==== Create vxlan l3 egress - access OL ====\n");
	bcm_if_t acc_intf_ol_1;
	BCM_IF_ERROR_RETURN(l3_intf_create(unit, 0, acc_ol_mac_1, vpn_net, &acc_intf_ol_1));
	printf("l3_intf_create() acc_intf_ol_1=0x%x\n", acc_intf_ol_1);

	bcm_if_t acc_egr_obj_ol_1;
	BCM_IF_ERROR_RETURN(l3_egress_create(unit, BCM_L3_IPMC, acc_intf_ol_1, acc_vp_1, dummy_mac_addr, &acc_egr_obj_ol_1));
	printf("l3_egress_create() acc_egr_obj_ol_1=0x%x\n", acc_egr_obj_ol_1);

	BCM_IF_ERROR_RETURN(bcm_multicast_egress_object_encap_get(unit, ol_mc_group, acc_egr_obj_ol_1, &encap_id));
	BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, ol_mc_group, access_gport_2, encap_id));

	/*
	 * Access port 2 - L2 switching interface
	 */
	BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, access_port_3, &access_gport_3));
	BCM_IF_ERROR_RETURN(bcm_vlan_gport_add(unit, acc_vid, access_gport_3, 0));

	BCM_IF_ERROR_RETURN(bcm_multicast_l2_encap_get(unit, ol_mc_group, access_gport_3, acc_vid, &encap_id));
	BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, ol_mc_group, access_gport_3, encap_id));

	/*
	 * Add IPMC entry
	 */
	printf("Add IPMC entry\n");
	int modid;
	BCM_IF_ERROR_RETURN(bcm_stk_modid_get(unit, &modid));

	bcm_ipmc_addr_t ipmc;
	bcm_ipmc_addr_t_init(&ipmc);
	ipmc.flags = 0;
	ipmc.s_ip_addr = payload_sip;
	ipmc.mc_ip_addr = payload_dip_mc;
	ipmc.vrf = vrf;
	ipmc.port_tgid = access_port_1;
	ipmc.mod_id = modid;
	ipmc.group = ol_mc_group;
	BCM_IF_ERROR_RETURN(bcm_ipmc_add(unit, &ipmc));
	printf("riot decap mc  SUCCESS \n");

	return BCM_E_NONE;

}


 /**************************4 main functions end ***********************/


 /**************************5 verify functions begin ***********************/

 /*
  * send the pkt to verify the script
 */
 void verify(int unit)
 {
	 char	   str[512];
	 bshell(unit, "hm ieee");
	 printf(" \n****************************\n\n\n\n\n");

	 /************************************************************************************************************/
	 printf(" \n **************TEST 1  Multicast **************\n");

	 if (BCM_E_NONE != ingress_port_setup(unit, access_port_1  , &access_port_1_eid )) {
	 printf("ingress_port_setup() failed for port %d\n", access_port_1);
	 return ;
	 }


	 if (BCM_E_NONE != egress_port_setup(unit, access_port_2, &access_port_2_eid)) {
	 printf("egress_port_setup() failed for port %d\n", access_port_2);
	 return ;
	 }

	 if (BCM_E_NONE != egress_port_setup(unit, access_port_3, &access_port_3_eid)) {
	 printf("egress_port_setup() failed for port %d\n", access_port_3);
	 return ;
	 }

	 if (BCM_E_NONE != egress_port_setup(unit, network_port_1, &network_port_1_eid)) {
	 printf("egress_port_setup() failed for port %d\n", network_port_1);
	 return ;
	 }


	snprintf(str, 512, "tx 1 PortBitMap=%d data=01005E0010010000001111118100000A0800450000320000000040FF53AD1E1E1E01E0000A01000000000000000000000000000000000000000000000000000000000000", access_port_1);
	bshell(unit, str);
	bshell(0, "sleep 1");
	bshell(0, "l2 show ");
	bshell(0, "sleep 1");
	bshell(0, " show c");

	printf(" \n **************destroy the ifp rules************** \n");
	destroy_ifp_to_cpu_rule(unit,access_port_1_eid);
	destroy_ifp_to_cpu_rule(unit,access_port_2_eid);
	destroy_ifp_to_cpu_rule(unit,access_port_3_eid);
	destroy_ifp_to_cpu_rule(unit,network_port_1_eid);

	return ;


 }

 /**************************5 verify functions end ***********************/



 ////////////////////////////////////////////////////////////////////////////////////////
 /*
  * This function does the following
  * a)test setup
  * b)actual configuration (Done in config_vxlan_access_to_network_multicast())
  * c)demonstrates the functionality(done in verify()).
  */

 bcm_error_t execute()
 {
   bcm_error_t rv;
   int unit =0;


   printf("**************************** basic information begin****************************\n", bcm_errmsg(rv));
   bshell(unit, "config show; a ; version; cancun stat");

   printf("**************************** basic information end****************************\n\n", bcm_errmsg(rv));
   printf("**************************** test setup begin****************************\n", bcm_errmsg(rv));

   if (BCM_FAILURE((rv = test_setup(unit)))) {
	 printf("test_setup() failed.\n");
	 return -1;
   }
   printf("**************************** test setup end****************************\n\n", bcm_errmsg(rv));
   printf("****************************run main script begin****************************\n", bcm_errmsg(rv));

   if (BCM_FAILURE((rv = riot_encap_multicast(unit)))) {
	 printf("vxlan_access_to_access_multicast() Failed\n");
	 return -1;
   }
   printf("****************************run main script end****************************\n\n", bcm_errmsg(rv));
   printf("****************************Verify begin****************************\n", bcm_errmsg(rv));

   verify(unit);

   printf("****************************Verify end****************************\n", bcm_errmsg(rv));
   return BCM_E_NONE;
 }

 const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
 if (!sal_strcmp(auto_execute, "YES")) {
   print execute();
 }
