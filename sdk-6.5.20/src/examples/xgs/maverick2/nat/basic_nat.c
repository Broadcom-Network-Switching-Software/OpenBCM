/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : Basic NAT
 *
 *  Usage    : BCM.0> cint basic_nat.c
 *
 *  config   : basic_nat_config.bcm
 *
 *  Log file : basic_nat_log.txt
 *
 *  Test Topology :
 *
 *                   +------------------------------+  
 *                   |                              |  port_1
 *                   |                              +-----------------+
 *                   |                              |
 *   cpu_port        |                              |
 *  +----------------+          SWITCH              |
 *                   |                              |
 *                   |                              |
 *                   |                              |  port_2
 *                   |                              +-----------------+
 *                   |                              |
 *                   +------------------------------+
 *  Summary:
 *  ========
 *
 *    NAT or Network Address translation is used to avoid exposing the Internal or private IP addresses when the Host systems in an enterprise network or Datacenter 
 *    communicate with external internet servers . The main advantage of using NAT is to reduce the usage of the Public IP addresses allocated for an enterprise or Data center.
 *    NAT feature allows the capability to change the Src IP address , Destination IP address , Src TCP/UDP port number or Destination TCP/UDP port number for an IP packet
 *    undergoing L3  routing in the switch.
 *  
 *    Few terminology to understand here are 
 *    Outbound flow - packet routing  from private IP domain to Public IP Domain
 *    Inbound flow - packet routing  from Public IP domain to Private IP Domain 
 *    NAPT  - Network Address Port translation . Can also match on the L4 Port numbers for NAT and translate to a new Port number if required.
 *  
 *    Below is a Sample NAT Test scenario for Outbound   and Inbound flow with NAPT 
 *   
 *    The purpose of the test is to do NAPT , and change the private realm to public realm. The following tests are created to verify the chip behavior.
 *    Outbound:
 *      {src_ip=10.0.0.1, dst_ip=200.0.0.1, tcp, src_tcp_port=2000, dst_tcp_port=2828} --> {src_ip=100.0.0.1, dst_ip=200.0.0.1, tcp, src_tcp_port=5000, dst_tcp_port=2828}
 *
 *    Inbound:
 *      {src_ip=200.0.0.1, dst_ip=100.0.0.1, tcp, src_tcp_port=2828, dst_tcp_port=5000} --> {src_ip=200.0.0.1, dst_ip=10.0.0.1, tcp, src_tcp_port=2828, dst_tcp_port=2000}
 *
 *
 *    Detailed steps done in the CINT script:
 *    =====================================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select two ports and configure them in loopback mode.
 *      b) start Packet Watcher diag app (PW start)
 *
 *    2) Step2 - Configuration (Done in config_basic_nat())
 *    ============================================
 *      a) Enable L3EgressMode
 *      b) Create Vlan 100 and add port_1 and cpu_port to vlan 100.
 *      c) Create Vlan 200 and add port_2 and cpu_port to vlan 200.
 *      d) Configure NAT
 *      e) Configure NAT for inbound and outbound flows
 *
 *    3) Step3 - Verification(Done in verify())
 *    ==========================================
 *      a) Transmit the packet below to port_1 by tx command
 *         {src_ip=10.0.0.1, dst_ip=200.0.0.1, tcp, src_tcp_port=2000, dst_tcp_port=2828}
 *         0000000001000000020002008100006408004500006E000000004006A8880A000001C800000107D00B0C0000000000000000500000001FF30000000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F404142434445456F8564
 *      b) Expected Result:
 *      ===================
 *         Egress packet captured from packet watcher:
 *         src_ip=100.0.0.1(0x64000001), dst_ip=200.0.0.1(0xc8000001), tcp, src_tcp_port=5000(0x1388), dst_tcp_port=2828(0x0b0c)
 *         [bcmPW.0]
 *         [bcmPW.0]Packet[2]: data[0000]: {000000000201} {000000000200} 8100 00c8
 *         [bcmPW.0]Packet[2]: data[0010]: 0800 4500 006e 0000 0000 3f06 4f88 6400 
 *         [bcmPW.0]Packet[2]: data[0020]: 0001 c800 0001 1388 0b0c 0000 0000 0000 
 *         [bcmPW.0]Packet[2]: data[0030]: 0000 5000 0000 ba3a 0000 0001 0203 0405 
 *         [bcmPW.0]Packet[2]: data[0040]: 0607 0809 0a0b 0c0d 0e0f 1011 1213 1415 
 *         [bcmPW.0]Packet[2]: data[0050]: 1617 1819 1a1b 1c1d 1e1f 2021 2223 2425 
 *         [bcmPW.0]Packet[2]: data[0060]: 2627 2829 2a2b 2c2d 2e2f 3031 3233 3435 
 *         [bcmPW.0]Packet[2]: data[0070]: 3637 3839 3a3b 3c3d 3e3f 4041 4243 4445 
 *         [bcmPW.0]Packet[2]: data[0080]: 456f 8564 5087 f644 
 *         [bcmPW.0]Packet[2]: length 136 (136). rx-port 2. cos 0. prio_int 0. vlan 200. reason 0x1. Outer tagged.
 *         [bcmPW.0]Packet[2]: dest-port 200. dest-mod 0. src-port 2. src-mod 0. opcode 2.  matched 0. classification-tag 0.
 *         [bcmPW.0]Packet[2]: reasons: FilterMatch
 *      c) Transmit the packet below to port_2 by tx command
 *         {src_ip=200.0.0.1, dst_ip=100.0.0.1, tcp, src_tcp_port=2828(0x0b0c), dst_tcp_port=5000(0x1388)}
 *         000000000200000000000201810000C808004500006E0000000040064E88C8000001640000010B0C1388000000000000000050000000BA3A0000000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F404142434445E7C4880E
 *      d) Expected Result:
 *      ===================
 *         Egress packet captured from packet watcher 
 *         {src_ip=200.0.0.1, dst_ip=10.0.0.1, tcp, src_tcp_port=2828(0x0b0c), dst_tcp_port=2000(0x7D0)}
 *         [bcmPW.0]
 *         [bcmPW.0]Packet[4]: data[0000]: {000000000101} {000000000100} 8100 0064
 *         [bcmPW.0]Packet[4]: data[0010]: 0800 4500 006e 0000 0000 3f06 a988 c800 
 *         [bcmPW.0]Packet[4]: data[0020]: 0001 0a00 0001 0b0c 07d0 0000 0000 0000 
 *         [bcmPW.0]Packet[4]: data[0030]: 0000 5000 0000 1ff3 0000 0001 0203 0405 
 *         [bcmPW.0]Packet[4]: data[0040]: 0607 0809 0a0b 0c0d 0e0f 1011 1213 1415 
 *         [bcmPW.0]Packet[4]: data[0050]: 1617 1819 1a1b 1c1d 1e1f 2021 2223 2425 
 *         [bcmPW.0]Packet[4]: data[0060]: 2627 2829 2a2b 2c2d 2e2f 3031 3233 3435 
 *         [bcmPW.0]Packet[4]: data[0070]: 3637 3839 3a3b 3c3d 3e3f 4041 4243 4445 
 *         [bcmPW.0]Packet[4]: data[0080]: e7c4 880e cdf3 ab8c 
 *         [bcmPW.0]Packet[4]: length 136 (136). rx-port 1. cos 0. prio_int 0. vlan 100. reason 0x1. Outer tagged.
 *         [bcmPW.0]Packet[4]: dest-port 100. dest-mod 0. src-port 1. src-mod 0. opcode 2.  matched 0. classification-tag 0.
 *         [bcmPW.0]Packet[4]: reasons: FilterMatch
 */
 
cint_reset();

bcm_error_t         rv = BCM_E_NONE;
bcm_field_group_t   group = -1;
bcm_port_t          port_1 = 1;
bcm_port_t          port_2 = 2;
bcm_port_t          cpu_port = 0;

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
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU.
 */
bcm_error_t fp_copy_ingress_packet_to_cpu(int unit, bcm_port_t port)
{
  bcm_field_qset_t  qset;
  bcm_field_entry_t entry;

  if (group == -1)
  {
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));
  }

  BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
  return BCM_E_NONE;
}

int
vlan_create_add_ports(int unit, int vid, int count, bcm_port_t *ports)
{
    bcm_pbmp_t pbmp, upbmp;
    int i;
    bcm_error_t rv = BCM_E_NONE;
    rv  = bcm_vlan_create(unit, vid);
    if (BCM_E_NONE != rv)
    {
        printf("bcm_vlan_create failed %s\n", bcm_errmsg(rv));
        return rv;
    }
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);

    for (i = 0; i < count; i++) {
        BCM_PBMP_PORT_ADD(pbmp, ports[i]);
    }

    rv = bcm_vlan_port_add(unit, vid, pbmp, upbmp);
    if (BCM_E_NONE != rv)
    {
        printf("bcm_vlan_port_add failed %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

int intf_create(int unit, bcm_if_t id, bcm_vlan_t vid, bcm_mac_t mac, int realm)
{
	int                         rv;
	int                         sta_id;
	bcm_l2_station_t            sta;
	bcm_l3_intf_t               intf;

	bcm_l3_intf_t_init(&intf);
	intf.l3a_flags              = BCM_L3_WITH_ID;
	intf.l3a_intf_id            = id;
	intf.l3a_vid                = vid;
	intf.l3a_mac_addr           = mac;
	intf.l3a_vrf                = 0;
	intf.l3a_mtu                = 9216;
	/* Alternatively realm id can be specified with an ingress interface - see below */
	intf.l3a_nat_realm_id       = realm;

	rv = bcm_l3_intf_create(unit, &intf);
	if (rv != BCM_E_NONE) {
		printf("l3 intf %d create failed: %s\n", id, bcm_errmsg(rv));
		return rv;
	}

	/* Add station to L3 switch packets sent to the switch MAC address */
	bcm_l2_station_t_init(&sta);
	sta.dst_mac                 = mac;
	sta.dst_mac_mask            = "ff:ff:ff:ff:ff:ff";
	sta.flags                   = BCM_L2_STATION_IPV4 | BCM_L2_STATION_IPV6;

	rv = bcm_l2_station_add(unit, &sta_id, &sta);
	if (rv != BCM_E_NONE) {
		printf("l2 station create failed %s\n", bcm_errmsg(rv));
		return rv;
	}

	return BCM_E_NONE;
}

/* Create L3 egress object */
int
create_egr_obj(int unit, bcm_if_t l3_if, bcm_mac_t mac, bcm_gport_t gport,
               bcm_if_t *egr_if)
{
    bcm_error_t rv;
    bcm_l3_egress_t l3_egr;

    bcm_l3_egress_t_init(&l3_egr);
    l3_egr.intf = l3_if;
    l3_egr.port = gport;
    sal_memcpy(l3_egr.mac_addr, mac, sizeof(mac));
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egr, egr_if));

    return BCM_E_NONE;
}

/* Adding subnet address to defip table */
int
add_route(int unit, bcm_ip_t subnet_addr, bcm_ip_t subnet_mask, bcm_if_t egr_if)
{
    bcm_l3_route_t route;

    bcm_l3_route_t_init(&route);

    route.l3a_intf = egr_if;

    route.l3a_subnet = subnet_addr;
    route.l3a_ip_mask = subnet_mask;

    BCM_IF_ERROR_RETURN(bcm_l3_route_add(unit, &route));

    return BCM_E_NONE;
}

int nat_config(int unit, bcm_ip_t ip, int napt)
{
    bcm_l3_nat_ingress_t ing_nat;

    /* Enable NAT */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3NATEnable, TRUE));

    /* To dynamically configure NAT, trap NAT packet misses to the CPU */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3SNATMissToCpu, TRUE));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3DNATMissToCpu, TRUE));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3NatRealmCrossingIcmpToCpu, TRUE));

    /*
    * Populate the address pool
    * Addresses in the pool are publically available addresses. These addresses
    * are destined to this device and are not used for forwarding.
    * The pool address is looked up in the NAT resolution logic during ingress.
    */
    bcm_l3_nat_ingress_t_init(&ing_nat);
    ing_nat.flags   = BCM_L3_NAT_INGRESS_DNAT | BCM_L3_NAT_INGRESS_DNAT_POOL;
    if (napt) {
        ing_nat.flags |= BCM_L3_NAT_INGRESS_TYPE_NAPT;
    }

    ing_nat.ip_addr = ip;

    BCM_IF_ERROR_RETURN(bcm_l3_nat_ingress_add(unit, &ing_nat));

    return BCM_E_NONE;
}

int nat_inbound_create(int unit, bcm_l3_nat_id_t id, int napt,
		uint8 proto, bcm_if_t egr_obj,
		bcm_ip_t priv_ip, uint16 priv_l4_port,
		bcm_ip_t pub_ip, uint16 pub_l4_port)
{
	bcm_l3_nat_egress_t         egr_nat;
	bcm_l3_nat_ingress_t        ing_nat;
    int                         rv = 0;

	/*
	 * egr_nat is used for NAT packet modification in the egresss
	 * - dip_addr becomes the DIP in the packet
	 * - if NAPT, dst_port becomes the L4 destination port in the packet
	 */
	bcm_l3_nat_egress_t_init(&egr_nat);
	egr_nat.flags               = BCM_L3_NAT_EGRESS_DNAT | BCM_L3_NAT_EGRESS_WITH_ID;
	egr_nat.nat_id              = id;
	egr_nat.dip_addr            = priv_ip;
	egr_nat.dip_addr_mask       = 0xffffffff;
	if (napt) {
		egr_nat.flags          |= BCM_L3_NAT_EGRESS_NAPT;
		egr_nat.dst_port        = priv_l4_port;
	}
	BCM_IF_ERROR_RETURN(bcm_l3_nat_egress_add(0, &egr_nat));

	/*
	 * ing_nat is used for NAT packet resolution in the ingress
	 * - pub_ip is the address compared against the packet DIP to trigger DIP translation
	 * - if NAPT, ip_proto is the IP protocol compared against the packet IP protocol and
	 *   if matched, then the l4 port is compared against the destination port in the
	 *   packet L4 header
	 */
	bcm_l3_nat_ingress_t_init(&ing_nat);
	ing_nat.flags               = BCM_L3_NAT_INGRESS_DNAT;
	ing_nat.nat_id              = id;
	ing_nat.ip_addr             = pub_ip;
	ing_nat.nexthop             = egr_obj;
	if (napt) {
		ing_nat.flags          |= BCM_L3_NAT_INGRESS_TYPE_NAPT;
		ing_nat.l4_port         = pub_l4_port;
		ing_nat.ip_proto        = proto;
	}
	BCM_IF_ERROR_RETURN(bcm_l3_nat_ingress_add(0, &ing_nat));
	return BCM_E_NONE;
}

int nat_outbound_create(int unit, bcm_l3_nat_id_t id,  int napt, uint8 proto,
		bcm_ip_t priv_ip, uint16 priv_l4_port,
		bcm_ip_t pub_ip, uint16 pub_l4_port)
{
	bcm_l3_nat_egress_t         egr_nat;
	bcm_l3_nat_ingress_t        ing_nat;
    int                         rv=0;
	/*
	 * egr_nat is used for NAT packet modification in the egresss
	 * - sip_addr becomes the SIP in the packet
	 * - if NAPT, src_port becomes the L4 source port in the packet
	 */
	bcm_l3_nat_egress_t_init(&egr_nat);
	egr_nat.flags               = BCM_L3_NAT_EGRESS_SNAT | BCM_L3_NAT_EGRESS_WITH_ID;
	egr_nat.nat_id              = id;
	egr_nat.sip_addr            = pub_ip;
	egr_nat.sip_addr_mask       = 0xffffffff;
	if (napt) {
		egr_nat.flags          |= BCM_L3_NAT_EGRESS_NAPT;
		egr_nat.src_port        = pub_l4_port;
	}
	BCM_IF_ERROR_RETURN(bcm_l3_nat_egress_add(unit, &egr_nat));

	/*
	 * ing_nat is used for NAT packet resolution in the ingress
	 * - pub_ip is the address compared against the packet SIP to trigger SIP translation
	 * - if NAPT, ip_proto is the IP protocol compared against the packet IP protocol and
	 *   if matched, then the l4 port is compared against the source port in the
	 *   packet L4 header. In this example, IP proto is 6 (TCP).
	 */
	bcm_l3_nat_ingress_t_init(&ing_nat);
	ing_nat.flags               = 0;
	ing_nat.ip_addr             = priv_ip;
	ing_nat.nat_id              = id;
	if (napt) {
		ing_nat.flags          |= BCM_L3_NAT_INGRESS_TYPE_NAPT;
		ing_nat.l4_port         = priv_l4_port;
		ing_nat.ip_proto        = proto;
	}
	BCM_IF_ERROR_RETURN(bcm_l3_nat_ingress_add(unit, &ing_nat));
	return BCM_E_NONE;
}

/* configure basic nat */
bcm_error_t config_basic_nat(int unit)
{
    bcm_port_t vlan_ports[2];    
    bcm_if_t if_1 = 1;
    bcm_if_t if_2 = 2;
    bcm_gport_t gport_1;
    bcm_gport_t gport_2;
    bcm_if_t egr_obj_1;
    bcm_if_t egr_obj_2;
    bcm_mac_t mac_1 = "00:00:00:00:01:01";
    bcm_mac_t mac_2 = "00:00:00:00:02:01";    
    bcm_mac_t mac = "00:00:00:00:01:00";
    
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));

    vlan_ports[0] = cpu_port;
    vlan_ports[1] = port_1;
    
    rv = vlan_create_add_ports(unit, 100, 2, vlan_ports);
    if (BCM_FAILURE(rv)) {
        printf("Error executing vlan_create_add_ports(): %s.\n", bcm_errmsg(rv));
        return rv;
    }
    
    vlan_ports[0] = cpu_port;
    vlan_ports[1] = port_2;

    rv = vlan_create_add_ports(unit, 200, 2, vlan_ports);
    if (BCM_FAILURE(rv)) {
        printf("Error executing vlan_create_add_ports(): %s.\n", bcm_errmsg(rv));
        return rv;
    }   
    
    rv = intf_create(0, if_1, 100, mac, 1);
    if (BCM_FAILURE(rv)) {
        printf("Error in intf_create() : %s.\n", bcm_errmsg(rv));
        return rv;
    } 

    mac = "00:00:00:00:02:00";
    rv = intf_create(0, if_2, 200, mac, 2);
    if (BCM_FAILURE(rv)) {
        printf("Error in intf_create() : %s.\n", bcm_errmsg(rv));
        return rv;
    } 

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, port_1, &gport_1));
    rv = create_egr_obj(unit, if_1, mac_1, gport_1, &egr_obj_1);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring l3 egress object : %s.\n", bcm_errmsg(rv));
        return rv;
    } 
        
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, port_2, &gport_2));
    rv = create_egr_obj(unit, if_2, mac_2, gport_2, &egr_obj_2);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring l3 egress object : %s.\n", bcm_errmsg(rv));
        return rv;
    }    

    bcm_ip_t ip_1 = 0x64000000;
    bcm_ip_t ip_2 = 0xC8000000;    
    bcm_ip_t ip_mask = 0xFFFFFF00;
    
    rv = add_route(unit, ip_1, ip_mask, egr_obj_1);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring add_route() : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    
    rv = add_route(unit, ip_2, ip_mask, egr_obj_2);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring add_route() : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    
    int napt = TRUE;
    rv = nat_config(0, 0x64000001, napt);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring nat_config() : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    
    rv = nat_inbound_create(0, 3, napt,
            6, egr_obj_1,
            0x0a000001 /* priv_ip */, 2000,
            0x64000001 /* pub_ip */, 5000);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring nat_inbound_create() : %s.\n", bcm_errmsg(rv));
        return rv;
    }
            
    rv = nat_outbound_create(0, 4, napt, 6,
            0x0a000001 /* priv_ip */, 2000,
            0x64000001 /* pub_ip */, 5000) ;
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring nat_outbound_create() : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    
    return BCM_E_NONE;
}

bcm_error_t test_setup(int unit)
{
    int port_list[2];
    
    if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
    printf("portNumbersGet() failed\n");
        return -1;
    }
    
    port_1 = port_list[0];
    port_2 = port_list[1];    
    
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port_1, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port_2, BCM_PORT_LOOPBACK_MAC));

    bshell(0, "pw start");
    bshell(0, "pw report +raw");
    return BCM_E_NONE;
}
 
int _2_hex_char_2_one_int(char *c) {
    int value;
    if (*c >= '0' && *c <= '9')
	    value = (*c - '0');
    else if (*c >= 'a' && *c <= 'f')
	    value = (*c - 'a') + 10;
    else if (*c >= 'A' && *c <= 'F')
	    value = (*c - 'A') + 10;
	value = value * 16;   
	c++;
    if (*c >= '0' && *c <= '9')
	    value += (*c - '0');
    else if (*c >= 'a' && *c <= 'f')
	    value += (*c - 'a') + 10;
    else if (*c >= 'A' && *c <= 'F')
	    value += (*c - 'A') + 10;	 
    return value;
}

void
tx_packet_data_from_cpu(int unit, int port, int packet_count, int pkt_size, char *data)
{
    bcm_pkt_t *pkt;
    int flags = BCM_TX_CRC_APPEND; 
    int untagged = 0; 
    int i;
                
    if (BCM_E_NONE != bcm_pkt_alloc(unit, pkt_size, flags, &pkt)) {
        printf("TX: Failed to allocate packets\n");
    } 
    for (i=0; i < pkt_size; i++){
        pkt->_pkt_data.data[i] = _2_hex_char_2_one_int(data[i*2]);
	}
    BCM_PKT_PORT_SET(&pkt, port, untagged, 0);
    pkt->call_back = NULL;
    if (BCM_E_NONE != bcm_tx(unit, pkt, NULL)) {                            
            printf("bcm tx error\n");
    }    
    bcm_pkt_free(unit, pkt);
}
 
/* Simple wrapper function used to transmit test packet */
bcm_error_t
test_harness(int unit)
{
    bcm_error_t rv=0; 
    /* OutBound packet test */
    /*
    Ingress packet
    {src_ip=10.0.0.1, dst_ip=200.0.0.1, tcp, src_tcp_port=2000, dst_tcp_port=2828}
    0000000001000000020002008100006408004500006E000000004006A8880A000001C800000107D00B0C0000000000000000500000001FF30000000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F404142434445456F8564
    
    Egress packet
    src_ip=100.0.0.1(0x64000001), dst_ip=200.0.0.1(0xc8000001), tcp, src_tcp_port=5000(0x1388), dst_tcp_port=2828(0x0b0c)
    
    [bcmPW.0]
    [bcmPW.0]Packet[2]: data[0000]: {000000000201} {000000000200} 8100 00c8
    [bcmPW.0]Packet[2]: data[0010]: 0800 4500 006e 0000 0000 3f06 4f88 6400 
    [bcmPW.0]Packet[2]: data[0020]: 0001 c800 0001 1388 0b0c 0000 0000 0000 
    [bcmPW.0]Packet[2]: data[0030]: 0000 5000 0000 ba3a 0000 0001 0203 0405 
    [bcmPW.0]Packet[2]: data[0040]: 0607 0809 0a0b 0c0d 0e0f 1011 1213 1415 
    [bcmPW.0]Packet[2]: data[0050]: 1617 1819 1a1b 1c1d 1e1f 2021 2223 2425 
    [bcmPW.0]Packet[2]: data[0060]: 2627 2829 2a2b 2c2d 2e2f 3031 3233 3435 
    [bcmPW.0]Packet[2]: data[0070]: 3637 3839 3a3b 3c3d 3e3f 4041 4243 4445 
    [bcmPW.0]Packet[2]: data[0080]: 456f 8564 5087 f644 
    [bcmPW.0]Packet[2]: length 136 (136). rx-port 2. cos 0. prio_int 0. vlan 200. reason 0x1. Outer tagged.
    [bcmPW.0]Packet[2]: dest-port 200. dest-mod 0. src-port 2. src-mod 0. opcode 2.  matched 0. classification-tag 0.
    [bcmPW.0]Packet[2]: reasons: FilterMatch    
    */
    
    printf("==========  Test: OutBound packet test  ==========\n");
    tx_packet_data_from_cpu(unit, port_1, 1, 132, "0000000001000000020002008100006408004500006E000000004006A8880A000001C800000107D00B0C0000000000000000500000001FF30000000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F404142434445456F8564");
    bshell(unit, "sleep quiet 1;");
    /* Inbound packet test */
    /*
    Ingress packet
    
    {src_ip=200.0.0.1, dst_ip=100.0.0.1, tcp, src_tcp_port=2828(0x0b0c), dst_tcp_port=5000(0x1388)}
    000000000200000000000201810000C808004500006E0000000040064E88C8000001640000010B0C1388000000000000000050000000BA3A0000000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F404142434445E7C4880E
    
    Egress Packet 
    {src_ip=200.0.0.1, dst_ip=10.0.0.1, tcp, src_tcp_port=2828(0x0b0c), dst_tcp_port=2000(0x7D0)}

    [bcmPW.0]
    [bcmPW.0]Packet[4]: data[0000]: {000000000101} {000000000100} 8100 0064
    [bcmPW.0]Packet[4]: data[0010]: 0800 4500 006e 0000 0000 3f06 a988 c800 
    [bcmPW.0]Packet[4]: data[0020]: 0001 0a00 0001 0b0c 07d0 0000 0000 0000 
    [bcmPW.0]Packet[4]: data[0030]: 0000 5000 0000 1ff3 0000 0001 0203 0405 
    [bcmPW.0]Packet[4]: data[0040]: 0607 0809 0a0b 0c0d 0e0f 1011 1213 1415 
    [bcmPW.0]Packet[4]: data[0050]: 1617 1819 1a1b 1c1d 1e1f 2021 2223 2425 
    [bcmPW.0]Packet[4]: data[0060]: 2627 2829 2a2b 2c2d 2e2f 3031 3233 3435 
    [bcmPW.0]Packet[4]: data[0070]: 3637 3839 3a3b 3c3d 3e3f 4041 4243 4445 
    [bcmPW.0]Packet[4]: data[0080]: e7c4 880e cdf3 ab8c 
    [bcmPW.0]Packet[4]: length 136 (136). rx-port 1. cos 0. prio_int 0. vlan 100. reason 0x1. Outer tagged.
    [bcmPW.0]Packet[4]: dest-port 100. dest-mod 0. src-port 1. src-mod 0. opcode 2.  matched 0. classification-tag 0.
    [bcmPW.0]Packet[4]: reasons: FilterMatch    
    */

    printf("==========  Test: Inbound packet test  ==========\n");
    tx_packet_data_from_cpu(unit, port_2, 1, 132, "000000000200000000000201810000C808004500006E0000000040064E88C8000001640000010B0C1388000000000000000050000000BA3A0000000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F404142434445E7C4880E");

    return rv;
}
 
/* Run the test, save and possibly display error status */
void verify(int unit)
{
    bcm_error_t rv;

    BCM_IF_ERROR_RETURN(fp_copy_ingress_packet_to_cpu(unit, port_1));
    BCM_IF_ERROR_RETURN(fp_copy_ingress_packet_to_cpu(unit, port_2));    
    
    if (BCM_FAILURE(rv = test_harness(0))) {
        printf("Tx verification failed: %s\n", bcm_errmsg(rv));
    }
}

bcm_error_t execute()
{
    bcm_error_t rv;
    int unit =0;

    bshell(unit, "config show; a ; version; cancun stat");
    
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed\n");
        return -1;
    }

    if (BCM_FAILURE((rv = config_basic_nat(unit)))) {
        printf("config_basic_nat() failed\n");
        return -1;
    }

    verify(unit);
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print execute();
}
