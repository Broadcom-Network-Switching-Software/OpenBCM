/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : VPLAG on VXLAN virtual ports
 *
 * Usage    : BCM.0> cint vplag_vxlan.c
 *
 * config   : vplag_vxlan_config.bcm
 *
 * Log file : vplag_vxlan_log.txt
 *
 * Test Topology :
 *                                  +-----------------------------------+
 *     port_acc1 / VP LAG 1 Port 1  |                                   |
 * +--------------------------------+                                   |
 *        VLAN ID - 21              |                                   |
 *                                  |                                   |
 *     port_acc2 / VP LAG 1 Port 2  |                                   |
 * +--------------------------------+                                   |
 *        VLAN ID - 22              |                                   |
 *                                  |                                   | port_network_1 - VP LAG 2 Port 1
 *     port_acc3 / VP LAG 1 Port 3  |                                   +---------------------------------+
 * +--------------------------------+                                   |         VLAN ID - 27
 *        VLAN ID - 23              |                                   |
 *                                  |             BCM SWITCH            |
 *     port_acc4 / VP LAG 1 Port 4  |                                   |
 * +--------------------------------+                                   |
 *        VLAN ID - 24              |                                   |
 *                                  |                                   |  port_network_2 - VP LAG 2 Port 2
 *     port_acc5 / VP LAG 1 Port 5  |                                   |
 * +--------------------------------+                                   +---------------------------------+
 *        VLAN ID - 25              |                                   |          VLAN ID - 28
 *                                  |                                   |
 *     port_acc6 / VP LAG 1 Port 6  |                                   |
 * +--------------------------------+                                   |
 *        VLAN ID - 26              |                                   |
 *                                  |                                   |
 *                                  +-----------------------------------+
 * Summary:
 * ========
 *   This cint example demostrates configuration of VPLAG on VXLAN Virtual ports.
 *
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *    1) Step1 - Test Setup (Done in test_setup()):
 *    =============================================
 *      a) Select eight ports out of which six ports(port_acc1, port_acc2,
 *         port_acc3, port_acc4, port_acc5, port_acc6 are  used as members
 *         of VPLAG on access side and remaining two ports(port_network_1,
 *         port_network_1) are used as members if VPLAG on network side.
 *
 *      b) Configure all six access side member ports as ingress and all others
 *         as egress ports.
 *
 *      c) All ports are configured in loopback mode and IFP rules are installed
 *         on all ports to copy all packets ingressing these ports to CPU.
 *
 *      d) On egress ports an additional action is installed to drop the packets
 *         to avoid loopback.
 *
 *      e) Start packet watcher.
 *
 *    2) Step2 - Configuration (Done in configure_vplag_vxlan())
 *    ==========================================================
 *      a) Create a VXLAN VPN and a multicast group(bcast_group) of type "BCM_MULTICAST_TYPE_VXLAN".
 *         Configure this multicast group as the VXLAN VPN's unknown-unicast,unknown-multicast
 *         and broadcast group.
 *
 *      b) Configuire 6 VXLAN access VPs, i.e vxlan_port_acc_list[0-5] and
 *         two VXLAN network VPs i.e, vxlan_port_network_1 and vxlan_port_network_2.
 *
 *      c) Create access side VPLAG(use flag "BCM_TRUNK_FLAG_VP" while creating trunk) with
 *          vxlan_port_acc_list[0-5] as members.
 *
 *      d) Create network side VPLAG with vxlan_port_network_1 and vxlan_port_network_2 as members.
 *
 *      e) Add 2 L2 entries pointing to network VPLAG.
 *         mac=00:00:00:00:cc:cc vlan=28688 GPORT=0x0 Trunk=2114 Static
 *         mac=00:00:00:00:aa:aa vlan=28688 GPORT=0x0 Trunk=2114 Static
 *
 *      f) Add one L2 entries pointing to access VPLAG.
 *         mac=00:00:00:00:bb:bb vlan=28688 GPORT=0x0 Trunk=2113 Static
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *
 *       a) Send known unicast packet(dmac:00:00:00:00:aa:aa,
 *          smac:00:00:00:00:00:01) on "port_acc1" 
 *
 *       b) Send known unicast packet(dmac:00:00:00:00:cc:cc,
 *          smac:00:00:00:00:00:11) on "port_acc1"
 *
 *       c) Send known unicast packet(dmac:00:00:00:00:bb:bb,
 *          smac:00:00:00:00:0b:0b) on "port_network_1"
 *
 *       d) Send 11 known unicast packet(dmac:00:00:00:00:0b:0b) 
 *          with incrementing SMAC on "port_acc1"
 *
 *       e) Expected Result:
 *       ===================
 
 *       After step 3.a, verify that VXLAN encapsulated packet egress out of
 *       one of port_network_1/port_network_2 and 00:00:00:00:00:01 is learned on 
 *       access side VPLAG.
 *       mac=00:00:00:00:00:01 vlan=28688 GPORT=0x0 Trunk=2113 Hit
 *
 *       After step 3.b, verify that VXLAN encapsulated packet egress out of
 *       one of port_network_1/port_network_2 and 00:00:00:00:00:11 is learnt on
 *       access side VPLAG.
 *       mac=00:00:00:00:00:11 vlan=28688 GPORT=0x0 Trunk=2113 Hit
 *
 *       After step 3.c, verify that decapsulated packet egress out of
 *       one of the members of access side VPLAG and 00:00:00:00:0b:0b is learnt on
 *       network side VPLAG.
 *       mac=00:00:00:00:0b:0b vlan=28688 GPORT=0x0 Trunk=2114 Hit 
 *
 *       After step 3.d, verify that VXLAN encapsulated packets egress out of
 *       network side VPLAG and they are distributed across ports "port_network_1" and
 *       "port_network_2".
 */
cint_reset();

bcm_port_t                 port_acc1 = 1;
bcm_port_t                 port_acc2 = 2;
bcm_port_t                 port_acc3 = 3;
bcm_port_t                 port_acc4 = 4;
bcm_port_t                 port_acc5 = 5;
bcm_port_t                 port_acc6 = 6;
bcm_port_t                 port_network_1 = 7;
bcm_port_t                 port_network_2 = 8;
uint8                      ttl = 16;
uint16 udp_dp = 8472;   /* UDP dst port used for OTV */
/* FP Group/Rule Creation/Destruction*/
bcm_field_group_config_t group_config;
bcm_field_entry_t port_acc1_eid = BCM_FIELD_ENTRY_INVALID;
bcm_field_entry_t port_acc2_eid = BCM_FIELD_ENTRY_INVALID;
bcm_field_entry_t port_acc3_eid = BCM_FIELD_ENTRY_INVALID;
bcm_field_entry_t port_acc4_eid = BCM_FIELD_ENTRY_INVALID;
bcm_field_entry_t port_acc5_eid = BCM_FIELD_ENTRY_INVALID;
bcm_field_entry_t port_acc6_eid = BCM_FIELD_ENTRY_INVALID;
bcm_field_entry_t port_network_1_eid = BCM_FIELD_ENTRY_INVALID;
bcm_field_entry_t port_network_2_eid = BCM_FIELD_ENTRY_INVALID;


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
bcm_error_t test_setup(int unit)
{
  int port_list[8], i;
  bcm_field_entry_t entry[8];
  if (BCM_E_NONE != portNumbersGet(unit, port_list, 8)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }
  for (i = 0; i < 8; i++) {
    entry[i] = BCM_FIELD_ENTRY_INVALID;
  }
  port_acc1 = port_list[0];
  port_acc2 = port_list[1];
  port_acc3 = port_list[2];
  port_acc4 = port_list[3];
  port_acc5 = port_list[4];
  port_acc6 = port_list[5];
  port_network_1 = port_list[6];
  port_network_2 = port_list[7];

  BCM_IF_ERROR_RETURN(create_ifp_group(unit, &group_config));
  for (i = 0; i <= 5; i++) {
    printf("Using port number:%d as port_acc%d\n", port_list[i], (i+1));
    if (BCM_E_NONE != ingress_port_setup(unit, port_list[i], &entry[i])) {
      printf("ingress_port_setup() failed for port %d\n", port_list[i]);
      return -1;
    }
  }
  port_acc1_eid = entry[0];
  port_acc2_eid = entry[1];
  port_acc3_eid = entry[2];
  port_acc4_eid = entry[3];
  port_acc5_eid = entry[4];
  port_acc6_eid = entry[5];
  for (i = 6; i <= 7; i++) {
    printf("Using port number:%d as port_network_%d\n", port_list[i], (i - 5));
    if (BCM_E_NONE != egress_port_setup(unit, port_list[i], &entry[i])) {
      printf("egress_port_setup() failed for port %d\n", port_list[i]);
      return -1;
    }
  }
  port_network_1_eid = entry[6];
  port_network_2_eid = entry[7];

  bshell(unit, "pw start report +raw +decode");
  return BCM_E_NONE;
}


int
create_l3_interface(int unit, int flags, bcm_mac_t local_mac, int vid, int vrf, bcm_if_t *intf_id)
{
    bcm_l3_intf_t               l3_intf;
    bcm_error_t                 rv = BCM_E_NONE;
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags |= flags;
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
    l3_intf.l3a_vid = vid;
    l3_intf.l3a_vrf = vrf;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    if (BCM_E_NONE != rv)
    {
      printf("bcm_l3_intf_create failed %s\n", bcm_errmsg(rv));
      return rv;
    }
    *intf_id = l3_intf.l3a_intf_id;
    return rv;
}

int vplag_vxlan_tunnel_initiator_create (int unit, bcm_ip_t dip, bcm_ip_t sip, bcm_vlan_t vlan, bcm_gport_t *pTunnel_id)
{
    bcm_tunnel_initiator_t      tnl_init;
    bcm_error_t                 rv = BCM_E_NONE;
    bcm_tunnel_initiator_t_init(&tnl_init);
    tnl_init.type  = bcmTunnelTypeVxlan;
    tnl_init.ttl = ttl;
    tnl_init.sip = sip;
    tnl_init.dip = dip;
    tnl_init.udp_dst_port = 8472;
    tnl_init.udp_src_port = 0xFFFF;
    if (vlan != 0)
    {
      tnl_init.vlan = vlan;
    }
    rv = bcm_vxlan_tunnel_initiator_create(unit, &tnl_init);
    if(BCM_FAILURE(rv))
    {
      printf("Error executing bcm_vxlan_tunnel_initiator_create(): %s \n", bcm_errmsg(rv));
      return rv;
    }
    *pTunnel_id = tnl_init.tunnel_id;
    return rv;
}

int vplag_vxlan_tunnel_terminator_create (int unit, bcm_ip_t sip, bcm_ip_t dip, bcm_gport_t tunnel_id, bcm_vlan_t vlan, bcm_gport_t *pTunnel_id)
{
    bcm_tunnel_terminator_t     tnl_term;
    bcm_error_t                 rv = BCM_E_NONE;
    bcm_tunnel_terminator_t_init(&tnl_term);
    tnl_term.type = bcmTunnelTypeVxlan;
    tnl_term.sip = sip;                                    /* Don't care for MC! */
    tnl_term.dip = dip;
    tnl_term.tunnel_id = tunnel_id;
    tnl_term.flags = BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
    if (vlan != 0)
    {
      tnl_term.vlan = vlan;                              /* For Bud check */
    }
    rv = bcm_vxlan_tunnel_terminator_create(unit, &tnl_term);
    if(BCM_FAILURE(rv))
    {
      printf("Error executing bcm_vxlan_tunnel_terminator_create(): %s \n", bcm_errmsg(rv));
      return rv;
    }
    *pTunnel_id = tnl_term.tunnel_id;
    return rv;
}

int vplag_vxlan_vpn_create (int unit, bcm_vpn_t vpn, bcm_multicast_t bcast_group, uint32 vnid)
{
    bcm_vxlan_vpn_config_t      vpn_info;
    bcm_error_t                 rv = BCM_E_NONE;
    bcm_vxlan_vpn_config_t_init(&vpn_info);
    vpn_info.flags = BCM_VXLAN_VPN_ELAN | BCM_VXLAN_VPN_WITH_ID | BCM_VXLAN_VPN_WITH_VPNID |
                     BCM_VXLAN_VPN_SERVICE_TAGGED | BCM_VXLAN_VPN_SERVICE_VLAN_DELETE;
    vpn_info.vpn = vpn;
    vpn_info.broadcast_group = bcast_group;
    vpn_info.unknown_multicast_group = bcast_group;
    vpn_info.unknown_unicast_group = bcast_group;
    vpn_info.vnid = vnid;
    vpn_info.egress_service_tpid = 0x8100;
    rv = bcm_vxlan_vpn_create(unit, &vpn_info);
    if(BCM_FAILURE(rv))
    {
      printf("Error executing bcm_vxlan_vpn_create = %s, vpn=0x%x \n", bcm_errmsg(rv), vpn_info.vpn);
      return rv;
    }
    return rv;
}

int vplag_vxlan_port_add (int unit, bcm_gport_t match_port, bcm_if_t egress_if, bcm_vxlan_port_match_t criteria,
                          bcm_vlan_t match_vlan, bcm_vlan_t match_inner_vlan, bcm_vpn_t vpn, bcm_gport_t *vxlan_port_id)
{
    bcm_vxlan_port_t            vxlan_port;
    bcm_error_t                 rv = BCM_E_NONE;

    bcm_vxlan_port_t_init(&vxlan_port);
    vxlan_port.flags = BCM_VXLAN_PORT_SERVICE_TAGGED | BCM_VXLAN_PORT_SERVICE_VLAN_ADD;
    vxlan_port.match_port = match_port;
    vxlan_port.egress_if = egress_if;
    vxlan_port.criteria = criteria;
    vxlan_port.egress_service_tpid = 0x8100;

    switch(vxlan_port.criteria)
    {
        case BCM_VXLAN_PORT_MATCH_PORT_VLAN:
            {
                vxlan_port.match_vlan = match_vlan;
                vxlan_port.egress_service_vlan = match_vlan;
            }
            break;
        case BCM_VXLAN_PORT_MATCH_PORT_INNER_VLAN:
            {
                vxlan_port.match_inner_vlan = match_inner_vlan;
                vxlan_port.egress_service_vlan = match_inner_vlan;
            }
            break;
        case BCM_VXLAN_PORT_MATCH_PORT_VLAN_STACKED:
            {
                vxlan_port.match_vlan = match_vlan;
                vxlan_port.match_inner_vlan = match_inner_vlan;
                vxlan_port.egress_service_vlan = match_vlan;
            }
            break;
        case BCM_VXLAN_PORT_MATCH_VLAN_PRI:
            {
                vxlan_port.match_vlan = match_vlan & 0xf000;
                vxlan_port.egress_service_vlan = match_vlan;
            }
            break;
        case BCM_VXLAN_PORT_MATCH_PORT:
            {
                vxlan_port.egress_service_vlan = match_vlan;
            }
            break;
    }
    rv = bcm_vxlan_port_add(unit, vpn, &vxlan_port);
    if(BCM_FAILURE(rv))
    {
      printf("Error executing bcm_vxlan_port_add() :%s\n", bcm_errmsg(rv));
      return rv;
    }
    *vxlan_port_id = vxlan_port.vxlan_port_id;
    return rv;
}

int vplag_vxlan_port_add_network (int unit, uint32 flags, bcm_gport_t match_port, bcm_if_t egress_if,
                      bcm_gport_t egress_tunnel_id, bcm_gport_t match_tunnel_id, bcm_vpn_t vpn, bcm_gport_t *vxlan_port_id)
{
    bcm_vxlan_port_t            vxlan_port;
    bcm_error_t                 rv = BCM_E_NONE;

    bcm_vxlan_port_t_init(&vxlan_port);
    vxlan_port.flags = flags;
    vxlan_port.match_port = match_port;
    vxlan_port.criteria = BCM_VXLAN_PORT_MATCH_VN_ID;
    vxlan_port.egress_if = egress_if;
    vxlan_port.egress_tunnel_id = egress_tunnel_id;
    vxlan_port.match_tunnel_id = match_tunnel_id;
    rv = bcm_vxlan_port_add(unit, vpn, &vxlan_port);
    if(BCM_FAILURE(rv))
    {
      printf("Error executing bcm_vxlan_port_add() :%s vpn: 0x%x\n", bcm_errmsg(rv), vpn);
      return rv;
    }
    *vxlan_port_id = vxlan_port.vxlan_port_id;
    return rv;
}

int vplag_l2_addr_add(int unit, uint32 flags, bcm_mac_t mac, bcm_vlan_t vid, int port)
{
    bcm_l2_addr_t               l2_addr;
    bcm_error_t                 rv = BCM_E_NONE;

    bcm_l2_addr_t_init(&l2_addr, mac, vid);
    l2_addr.flags = flags;
    sal_memcpy(l2_addr.mac, mac, sizeof(mac));
    l2_addr.vid = vid;
    l2_addr.port = port;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if(BCM_FAILURE(rv))
    {
      printf("Error executing bcm_l2_addr_add() :%s\n", bcm_errmsg(rv));
      return rv;
    }
    return rv;
}

int
add_to_l2_station(int unit, bcm_mac_t mac, bcm_vlan_t vid, int flags)
{
    bcm_l2_station_t l2_station;
    int station_id;
    bcm_error_t rv = BCM_E_NONE;

    bcm_l2_station_t_init(&l2_station);
    sal_memcpy(l2_station.dst_mac, mac, sizeof(mac));
    l2_station.flags   = flags;
    l2_station.dst_mac_mask = "ff:ff:ff:ff:ff:ff";
    l2_station.vlan         = vid;
    l2_station.vlan_mask    = 0xfff;
    rv = bcm_l2_station_add(unit, &station_id, &l2_station);
    if(BCM_FAILURE(rv))
    {
      printf("Error executing bcm_l2_station_add() :%s\n", bcm_errmsg(rv));
      return rv;
    }
    return rv;
}
/* Create vlan and add port to vlan */
int
vlan_create_add_port(int unit, int vid, int port)
{
    bcm_pbmp_t pbmp, upbmp;
    bcm_error_t rv = BCM_E_NONE;

    rv = bcm_vlan_create(unit, vid);
    if (BCM_E_NONE != rv)
    {
        printf("bcm_vlan_create failed %s\n", bcm_errmsg(rv));
        return rv;
    }
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    rv = bcm_vlan_port_add(unit, vid, pbmp, upbmp);
    if (BCM_E_NONE != rv)
    {
      printf("bcm_vlan_port_add failed %s\n", bcm_errmsg(rv));
      return rv;
    }
    return rv;
}
int
do_vxlan_global_setting(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    /* Enable L3 Egress Mode */
    rv = bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE);
    if (BCM_E_NONE != rv)
    {
      printf("bcm_switch_control_set failed %s\n", bcm_errmsg(rv));
      return rv;
    }
    /* Set UDP port for VXLAN */
    rv = bcm_switch_control_set(unit, bcmSwitchVxlanUdpDestPortSet, udp_dp);
    if (BCM_E_NONE != rv)
    {
      printf("bcm_switch_control_set bcmSwitchVxlanUdpDestPortSet failed %s\n", bcm_errmsg(rv));
      return rv;
    }
    /* Enable/Disable VXLAN Entropy */
    rv = bcm_switch_control_set(unit, bcmSwitchVxlanEntropyEnable, TRUE);
    if (BCM_E_NONE != rv)
    {
      printf("bcm_switch_control_set bcmSwitchVxlanEntropyEnable failed %s\n", bcm_errmsg(rv));
      return rv;
    }
    /* Enable/Disable VXLAN Tunnel lookup failure settings */
    rv = bcm_switch_control_set(unit, bcmSwitchVxlanTunnelMissToCpu, TRUE);
    if (BCM_E_NONE != rv)
    {
      printf("bcm_switch_control_set  bcmSwitchVxlanTunnelMissToCpu failed %s\n", bcm_errmsg(rv));
      return rv;
    }
    /* Enable/Disable VXLAN VN_ID lookup failure settings */
    rv = bcm_switch_control_set(unit, bcmSwitchVxlanVnIdMissToCpu, TRUE);
    if (BCM_E_NONE != rv)
    {
      printf("bcm_switch_control_set  bcmSwitchVxlanVnIdMissToCpu failed %s\n", bcm_errmsg(rv));
      return rv;
    }
    return BCM_E_NONE;
}

int
do_vxlan_access_port_settings(int unit, bcm_port_t a_port)
{
    bcm_error_t rv = BCM_E_NONE;
    /* Enable VLAN translation on access port */
    rv = bcm_vlan_control_port_set(unit, a_port, bcmVlanTranslateIngressEnable, TRUE);
    if (BCM_E_NONE != rv)
    {
      printf("bcm_vlan_control_port_set  bcmVlanTranslateIngressEnable failed %s\n", bcm_errmsg(rv));
      return rv;
    }
    /* Should disable Vxlan Processing on access port */
    rv = bcm_port_control_set(unit, a_port, bcmPortControlVxlanEnable, FALSE);
    if (BCM_E_NONE != rv)
    {
        printf("bcm_vlan_control_port_set  bcmPortControlVxlanEnable failed %s\n", bcm_errmsg(rv));
        return rv;
    }
    /* Should disable Tunnel Based Vxlan-VnId lookup */
    rv = bcm_port_control_set(unit, a_port, bcmPortControlVxlanTunnelbasedVnId, FALSE);
    if (BCM_E_NONE != rv)
    {
      printf("bcm_vlan_control_port_set  bcmPortControlVxlanTunnelbasedVnId failed %s\n", bcm_errmsg(rv));
      return rv;
    }
    return BCM_E_NONE;
}

int
do_vxlan_net_port_settings(int unit, bcm_port_t n_port)
{
    bcm_error_t rv = BCM_E_NONE;
    /* Enable VXLAN Processing on network port */
    rv =  bcm_port_control_set(unit, n_port, bcmPortControlVxlanEnable, TRUE);
    if (BCM_E_NONE != rv)
    {
      printf("bcm_port_control_set  bcmPortControlVxlanEnable failed %s\n", bcm_errmsg(rv));
      return rv;
    }
    /* Allow tunnel based VXLAN-VNID lookup */
    rv =  bcm_port_control_set(unit, n_port, bcmPortControlVxlanTunnelbasedVnId, FALSE);
                              /* Only BCM_VXLAN_PORT_MATCH_VN_ID at network port */
    if (BCM_E_NONE != rv)
    {
      printf("bcm_port_control_set  bcmPortControlVxlanTunnelbasedVnId failed %s\n", bcm_errmsg(rv));
      return rv;
    }
    return BCM_E_NONE;
}

int vplag_l3_egress_create(int unit, uint32 flags, bcm_if_t intf, bcm_mac_t mac_addr, bcm_port_t port, bcm_vlan_t vlan, bcm_if_t *egr_obj, int ipmc)
{
    bcm_l3_egress_t             l3_egress;
    bcm_error_t rv = BCM_E_NONE;

    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.flags = flags;
    l3_egress.intf = intf;
    sal_memcpy(l3_egress.mac_addr,  mac_addr, sizeof(l3_egress.mac_addr));
    l3_egress.vlan = vlan;
    l3_egress.port = port;
    if (ipmc)
    {
        rv = bcm_l3_egress_create(unit, BCM_L3_IPMC, &l3_egress, egr_obj);
    }
    else
    {
        rv = bcm_l3_egress_create(unit, 0, &l3_egress, egr_obj);
    }
    if (BCM_E_NONE != rv)
    {
      printf("bcm_l3_egress_create failed %s\n", bcm_errmsg(rv));
      return rv;
    }
    return rv;
}
bcm_error_t trunk_rtag7_config(int unit)
{
    int flags;
    bcm_error_t rv = BCM_E_NONE;

    rv = bcm_switch_control_get(unit, bcmSwitchHashControl, &flags);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_switch_control_get(bcmSwitchHashControl) : %s\n", bcm_errmsg(rv));
      return rv;
    }

    flags |= (BCM_HASH_CONTROL_TRUNK_NUC_ENHANCE |
              BCM_HASH_CONTROL_TRUNK_NUC_SRC |
              BCM_HASH_CONTROL_TRUNK_NUC_DST |
              BCM_HASH_CONTROL_TRUNK_NUC_MODPORT);
    rv = bcm_switch_control_set(unit, bcmSwitchHashControl, flags);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_switch_control_set(bcmSwitchHashControl) : %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* Only use L2 fields for hashing even for IP packets */
    rv = bcm_switch_control_get(unit,bcmSwitchHashSelectControl, &flags);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_switch_control_get(bcmSwitchHashSelectControl) : %s\n", bcm_errmsg(rv));
      return rv;
    }
    flags |=(BCM_HASH_FIELD0_DISABLE_IP4|
             BCM_HASH_FIELD1_DISABLE_IP4|
             BCM_HASH_FIELD0_DISABLE_IP6|
             BCM_HASH_FIELD1_DISABLE_IP6);
    rv = bcm_switch_control_set(unit, bcmSwitchHashSelectControl, flags);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_switch_control_set(bcmSwitchHashSelectControl) : %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* Block A - L2 packet field selection (RTAG7_HASH_FIELD_BMAP3) */
    rv = bcm_switch_control_set(unit, bcmSwitchHashL2Field0,
            BCM_HASH_FIELD_SRCMOD |
            BCM_HASH_FIELD_SRCPORT |
            BCM_HASH_FIELD_VLAN |
            BCM_HASH_FIELD_ETHER_TYPE |
            BCM_HASH_FIELD_MACDA_LO |
            BCM_HASH_FIELD_MACDA_MI |
            BCM_HASH_FIELD_MACDA_HI |
            BCM_HASH_FIELD_MACSA_LO |
            BCM_HASH_FIELD_MACSA_MI |
            BCM_HASH_FIELD_MACSA_HI);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_switch_control_set(bcmSwitchHashL2Field0) : %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* Block B - L2 packet field selection */
    rv = bcm_switch_control_set(unit, bcmSwitchHashL2Field1,
            BCM_HASH_FIELD_SRCMOD |
            BCM_HASH_FIELD_SRCPORT |
            BCM_HASH_FIELD_VLAN |
            BCM_HASH_FIELD_ETHER_TYPE |
            BCM_HASH_FIELD_MACDA_LO |
            BCM_HASH_FIELD_MACDA_MI |
            BCM_HASH_FIELD_MACDA_HI |
            BCM_HASH_FIELD_MACSA_LO |
            BCM_HASH_FIELD_MACSA_MI |
            BCM_HASH_FIELD_MACSA_HI);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_switch_control_set(bcmSwitchHashL2Field1) : %s\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashSeed0, 0x33333333);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_switch_control_set(bcmSwitchHashSeed0) : %s\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashSeed1, 0x55555555);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_switch_control_set(bcmSwitchHashSeed1) : %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* Configure HASH A and HASH B functions */
    rv = bcm_switch_control_set(unit, bcmSwitchHashField0Config,
            BCM_HASH_FIELD_CONFIG_CRC32LO); /* BCM_HASH_FIELD_CONFIG_CRC32LO */
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_switch_control_set(bcmSwitchHashField0Config) : %s\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashField1Config,
            BCM_HASH_FIELD_CONFIG_CRC16); /* BCM_HASH_FIELD_CONFIG_CRC32HI */
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_switch_control_set(bcmSwitchHashField1Config) : %s\n", bcm_errmsg(rv));
      return rv;
    }


    return BCM_E_NONE;
}

bcm_error_t configure_vplag_vxlan(int unit)
{
    bcm_trunk_member_t         member;
    bcm_trunk_member_t         trunk_member[10];
    bcm_l2_station_t           l2_station;
    bcm_trunk_info_t           t_info;
    bcm_multicast_t            bcast_group = 0;    /* For VXLAN access and network VPs */
    bcm_trunk_t                tid = 1;
    bcm_trunk_t                tid6 = 2;
    bcm_gport_t                gport;
    bcm_pbmp_t                 pbmp, upbmp;
    bcm_mac_t                  dummy_mac_addr = "00:00:00:01:00:01";
    bcm_mac_t                  mc_mac = "01:00:5e:00:00:0A";              /* 224.0.0.10 */
    bcm_mac_t                  payload_remote_mac1 = "00:00:00:00:aa:aa";
    bcm_mac_t                  payload_remote_mac2 = "00:00:00:00:cc:cc";
    bcm_mac_t                  payload_local_mac = "00:00:00:00:bb:bb";
    bcm_vrf_t                  vrf = 0;
    bcm_ip_t                   tnl_local_ip = 0x0a0a0a01;     /* 10.10.10.1 */
    bcm_ip_t                   tnl_remote_ip1 = 0xC0A80101;   /* 192.168.1.1 */
    bcm_ip_t                   tnl_remote_ip2 = 0xC0A80A01;   /* 192.168.10.1 */
    bcm_ip_t                   tnl_remote_ip_mc = 0xe000000A; /* 224.0.0.10 */
    bcm_if_t                   encap_id_2;
    bcm_if_t                   encap_id_mc1, encap_id_mc2;
    bcm_vpn_t                  vpn_id = 0x7010;
    uint32                     vnid = 0x12345;
    bcm_vpn_t                  rsvd_network_vpn = 0x7020;
    uint32                     rsvd_network_vpn_vnid = 0x33333;
    uint32                     flags;
    int                        tunnel_init_id1, tunnel_init_id2, tunnel_mc_init_id1, tunnel_mc_init_id2;
    int                        tunnel_term_id1, tunnel_term_id2, tunnel_mc_term_id1, tunnel_mc_term_id2;
    int                        vxlan_port_mc1, vxlan_port_mc2;
    int                        station_id;
    bcm_error_t                rv;
    int                        i, num_vps;
    int                        vp_lag_id_acc, vp_lag_id_network;
    bcm_port_t                 port_acc_list[6] = {port_acc1, port_acc2, port_acc3, port_acc4, port_acc5, port_acc6};
    bcm_port_t                 port_network_list[2] = {port_network_1, port_network_2};
    bcm_gport_t                gport_acc_list[6] = {BCM_GPORT_INVALID, BCM_GPORT_INVALID, BCM_GPORT_INVALID, BCM_GPORT_INVALID, BCM_GPORT_INVALID, BCM_GPORT_INVALID};
    bcm_gport_t                gport_network_list[2] = {BCM_GPORT_INVALID, BCM_GPORT_INVALID};
    bcm_vlan_t                 vid_acc_list[6] = {21, 22, 23, 24, 25, 26};
    bcm_vlan_t                 vid_network_list[2] = {27, 28};

    bcm_if_t                   egr_obj_acc_list[6];
    bcm_mac_t                  remote_mac_network_list[2] = {{0x00, 0x00, 0x00, 0x00, 0x02, 0x02}, {0x00, 0x00, 0x00, 0x00, 0x03, 0x03}};
    bcm_mac_t                  local_mac_network_list[2] = {{0x00, 0x00, 0x00, 0x00, 0x22, 0x22}, {0x00, 0x00, 0x00, 0x00, 0x33, 0x33}};

    bcm_if_t                   intf_id_network_list[2] = {7, 8};
    bcm_if_t                   egr_obj_network_list[2];
    bcm_if_t                   egr_obj_mc_list[2];
    bcm_if_t                   egr_obj_mc1, egr_obj_mc2;
    int                        vxlan_port_acc_list[6];
    int                        vxlan_port_network1 = 0, vxlan_port_network2 = 0;
    bcm_if_t                   encap_id_1_list[6];
    bcm_gport_t                acc_vplag_gport;
    bcm_if_t                   acc_vplag_encap_id;

    rv = trunk_rtag7_config(unit);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing trunk_rtag7_config() : %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* Global settings */
    rv = do_vxlan_global_setting(unit);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing do_vxlan_global_setting() : %s\n", bcm_errmsg(rv));
      return rv;
    }

    for (i = 0; i <= 5; i++)
    {
      /* Access port settings */
      rv = do_vxlan_access_port_settings(unit, port_acc_list[i]);
      if(BCM_FAILURE(rv))
      {
        printf("\nError executing do_vxlan_access_port_settings() : %s for port:(%d)\n", bcm_errmsg(rv), port_acc_list[i]);
        return rv;
      }
    }

    /* Network port settings */
    for (i = 0; i <= 1; i++)
    {
      rv = do_vxlan_net_port_settings(unit, port_network_list[i]);
      if(BCM_FAILURE(rv))
      {
        printf("\nError executing do_vxlan_net_port_settings() : %s for port:(%d)\n", bcm_errmsg(rv), port_network_list[i]);
        return rv;
      }
    }

    for (i = 0; i <= 5; i++)
    {
      rv = bcm_port_gport_get(unit, port_acc_list[i], &gport_acc_list[i]);
      if(BCM_FAILURE(rv))
      {
        printf("\nError executing bcm_port_gport_get() : %s for port %d\n", bcm_errmsg(rv), port_acc_list[i]);
        return rv;
      }
    }
    for (i = 0; i <= 1; i++)
    {
      rv = bcm_port_gport_get(unit, port_network_list[i], &gport_network_list[i]);
      if(BCM_FAILURE(rv))
      {
        printf("\nError executing bcm_port_gport_get() : %s for port %d\n", bcm_errmsg(rv), port_network_list[i]);
        return rv;
      }
    }

    /* VLAN settings - vid_acc_list[] */
    for (i = 0; i <= 5; i++)
    {
      rv = vlan_create_add_port(unit, vid_acc_list[i], port_acc_list[i]);
      if(BCM_FAILURE(rv))
      {
        printf("\nError executing vlan_create_add_port() : %s vlan:(%d) port:(%d)\n", bcm_errmsg(rv), vid_acc_list[i], port_acc_list[i]);
        return rv;
      }
    }

    /* VLAN settings - vid_network_list[] */
    for (i = 0; i <= 1; i++)
    {
      rv = vlan_create_add_port(unit, vid_network_list[i], port_network_list[i]);
      if(BCM_FAILURE(rv))
      {
        printf("\nError executing vlan_create_add_port() : %s vlan:(%d) port:(%d)\n", bcm_errmsg(rv), vid_network_list[i], port_network_list[i]);
        return rv;
      }
    }

    for (i = 0; i <= 5; i++)
    {
      rv = vplag_l3_egress_create(unit, 0, BCM_IF_INVALID, dummy_mac_addr, gport_acc_list[i], vid_acc_list[i], &egr_obj_acc_list[i], 0);
      if(BCM_FAILURE(rv))
      {
        printf("\nError executing vplag_l3_egress_create() : %s\n, vlan:(%d)", bcm_errmsg(rv), vid_acc_list[i]);
        return rv;
      }
    }

    for (i = 0; i <= 1; i++)
    {
      rv = create_l3_interface(unit, 0, local_mac_network_list[i], vid_network_list[i], vrf, &intf_id_network_list[i]);
      if(BCM_FAILURE(rv))
      {
        printf("\nError executing create_l3_interface() : %s\n", bcm_errmsg(rv));
        return rv;
      }
    }

    for (i = 0; i <= 1; i++)
    {
      rv = vplag_l3_egress_create(unit, BCM_L3_VXLAN_ONLY, intf_id_network_list[i], remote_mac_network_list[i], gport_network_list[i], vid_network_list[i], &egr_obj_network_list[i], 0);
      if(BCM_FAILURE(rv))
      {
        printf("\nError executing vplag_l3_egress_create() : %s\n", bcm_errmsg(rv));
        return rv;
      }
    }

    /* L3 Egress object for network port 1 - non-UC VXLAN VP 1 */
    /* L3 Egress object for network port 2 - non-UC VXLAN VP 2 */
    for (i = 0; i <= 1; i++)
    {
      rv = vplag_l3_egress_create(unit, BCM_L3_VXLAN_ONLY, intf_id_network_list[i], mc_mac, gport_network_list[i], vid_network_list[i], &egr_obj_mc_list[i], 1);
      if(BCM_FAILURE(rv))
      {
        printf("\nError executing vplag_l3_egress_create() : %s\n", bcm_errmsg(rv));
        return rv;
      }
    }

    /* Tunnel Initiator for UC tunnel */
    rv = vplag_vxlan_tunnel_initiator_create(unit, tnl_remote_ip1, tnl_local_ip, 0, &tunnel_init_id1);
    if(BCM_FAILURE(rv))
    {
      printf("Error executing vplag_vxlan_tunnel_initiator_create(): %s\n", bcm_errmsg(rv));
      return rv;
    }

    rv = vplag_vxlan_tunnel_initiator_create(unit, tnl_remote_ip2, tnl_local_ip, 0, &tunnel_init_id2);
    if(BCM_FAILURE(rv))
    {
      printf("Error executing vplag_vxlan_tunnel_initiator_create(): %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* Tunnel Initiator for Non-UC tunnel */
    rv = vplag_vxlan_tunnel_initiator_create(unit, tnl_remote_ip_mc, tnl_local_ip, vid_network_list[0], &tunnel_mc_init_id1);
    if(BCM_FAILURE(rv))
    {
      printf("Error executing vplag_vxlan_tunnel_initiator_create(): %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* Use same MC initiator ID for MC tunnel2 except the VLAN */
    rv = vplag_vxlan_tunnel_initiator_create(unit, tnl_remote_ip_mc, tnl_local_ip, vid_network_list[1], &tunnel_mc_init_id2);
    if(BCM_FAILURE(rv))
    {
      printf("Error executing vplag_vxlan_tunnel_initiator_create(): %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* Tunnel Terminator for UC tunnel */
    rv = vplag_vxlan_tunnel_terminator_create(unit, tnl_remote_ip1, tnl_local_ip, tunnel_init_id1, 0, &tunnel_term_id1);
    if(BCM_FAILURE(rv))
    {
      printf("Error executing vplag_vxlan_tunnel_terminator_create(): %s\n", bcm_errmsg(rv));
      return rv;
    }
    rv = vplag_vxlan_tunnel_terminator_create(unit, tnl_remote_ip2, tnl_local_ip, tunnel_init_id2, 0, &tunnel_term_id2);
    if(BCM_FAILURE(rv))
    {
      printf("Error executing vplag_vxlan_tunnel_terminator_create(): %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* Tunnel Terminator for Non-UC set up for MC tunnel 1 */
    rv = vplag_vxlan_tunnel_terminator_create(unit, tnl_remote_ip1, tnl_remote_ip_mc, tunnel_mc_init_id1, vid_network_list[0], &tunnel_mc_term_id1);
    if(BCM_FAILURE(rv))
    {
      printf("Error executing vplag_vxlan_tunnel_terminator_create(): %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* Use same MC initiator ID for MC tunnel2, except the vid */
    rv = vplag_vxlan_tunnel_terminator_create(unit, tnl_remote_ip2, tnl_remote_ip_mc, tunnel_mc_init_id2, vid_network_list[1], &tunnel_mc_term_id2);
    if(BCM_FAILURE(rv))
    {
      printf("Error executing vplag_vxlan_tunnel_terminator_create(): %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* Multicast group for non-UC */
    flags  = BCM_MULTICAST_TYPE_VXLAN;
    rv = bcm_multicast_create(unit, flags, &bcast_group);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_multicast_create() : %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* Create VXLAN VPN */
    rv = vplag_vxlan_vpn_create (unit, vpn_id, bcast_group, vnid);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing vplag_vxlan_vpn_create() : %s\n", bcm_errmsg(rv));
      return rv;
    }


    /* Create VXLAN VP for access port 1 to 6*/
    for(i = 0; i <= 5; i++)
    {
/*      if(i <= 3)
      {*/
        rv = vplag_vxlan_port_add(unit, gport_acc_list[i], egr_obj_acc_list[i],
            BCM_VXLAN_PORT_MATCH_PORT_VLAN, vid_acc_list[i], 0, vpn_id, &vxlan_port_acc_list[i]);

/*      }
      else
      {
        rv = vplag_vxlan_port_add(unit, gport_acc_list[i], egr_obj_acc_list[i],
            BCM_VXLAN_PORT_MATCH_PORT, vid_acc_list[i], 0, vpn_id, &vxlan_port_acc_list[i]);
      }*/
      if(BCM_FAILURE(rv))
      {
        printf("\nError executing vplag_vxlan_port_add() : %s i = %d\n", bcm_errmsg(rv), i);
        return rv;
      }
    }

    /* Create VXLAN VPN 2 */
    rv = vplag_vxlan_vpn_create (unit, rsvd_network_vpn, bcast_group, rsvd_network_vpn_vnid);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing vplag_vxlan_vpn_create() : %s\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_multicast_control_set(unit, bcast_group, bcmMulticastVpTrunkResolve, 1);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_multicast_control_set() : %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* Create VXLAN VP for network port 1 */
    flags = 0;
    flags = BCM_VXLAN_PORT_NETWORK | BCM_VXLAN_PORT_EGRESS_TUNNEL | BCM_VXLAN_PORT_SERVICE_TAGGED;
    rv = vplag_vxlan_port_add_network(unit, flags, gport_network_list[0], egr_obj_network_list[0],
                                        tunnel_init_id1, tunnel_term_id1, rsvd_network_vpn, &vxlan_port_network1);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing vplag_vxlan_port_add_network() : %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* Create VXLAN VP for network port 2 */
    rv = vplag_vxlan_port_add_network(unit, flags, gport_network_list[1], egr_obj_network_list[1],
                                        tunnel_init_id2, tunnel_term_id2, rsvd_network_vpn, &vxlan_port_network2);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing vplag_vxlan_port_add_network() : %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* Create non-UC VXLAN VP1 at network port 1 */
    flags = 0;
    flags = BCM_VXLAN_PORT_NETWORK | BCM_VXLAN_PORT_EGRESS_TUNNEL | BCM_VXLAN_PORT_MULTICAST | BCM_VXLAN_PORT_SERVICE_TAGGED;
    rv = vplag_vxlan_port_add_network(unit, flags, gport_network_list[0], egr_obj_mc_list[0],
                                         tunnel_mc_init_id1, tunnel_mc_term_id1, rsvd_network_vpn, &vxlan_port_mc1);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing vplag_vxlan_port_add_network() : %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* Create non-UC VXLAN VP2 for network port 2 */
    rv = vplag_vxlan_port_add_network(unit, flags, gport_network_list[1], egr_obj_mc_list[1],
                                                   tunnel_mc_init_id2, tunnel_mc_term_id2, rsvd_network_vpn, &vxlan_port_mc2);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing vplag_vxlan_port_add_network() : %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* MC group set up - MC group should contains all Access ports and Network non-UC port */

    rv = bcm_multicast_vxlan_encap_get(unit, bcast_group, gport_network_list[0], vxlan_port_mc1, &encap_id_mc1);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_multicast_vxlan_encap_get() : %s\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_multicast_egress_add(unit, bcast_group, vxlan_port_mc1, encap_id_mc1);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_multicast_egress_add() : %s\n", bcm_errmsg(rv));
      return rv;
    }
    rv = bcm_multicast_vxlan_encap_get(unit, bcast_group, gport_network_list[1], vxlan_port_mc2, &encap_id_mc2);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_multicast_vxlan_encap_get() : %s\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_multicast_egress_add(unit, bcast_group, vxlan_port_mc2, encap_id_mc2);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_multicast_egress_add() : %s\n", bcm_errmsg(rv));
      return rv;
    }


    /* ACCESS Trunk */
    rv = bcm_trunk_create(unit, BCM_TRUNK_FLAG_VP, &vp_lag_id_acc);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_trunk_create() : %s\n", bcm_errmsg(rv));
      return rv;
    }

    bcm_trunk_info_t_init(&t_info);
    /* Choose RTAG 7 for Load balancing */
    t_info.psc        = BCM_TRUNK_PSC_PORTFLOW;   /* BCM_TRUNK_PSC_RANDOMIZED */
    t_info.dlf_index  = -1;
    t_info.ipmc_index = -1;
    t_info.mc_index   = -1;
    num_vps = 0;
    trunk_member[num_vps].gport = vxlan_port_acc_list[num_vps];
    num_vps++;
    trunk_member[num_vps].gport = vxlan_port_acc_list[num_vps];
    num_vps++;
    trunk_member[num_vps].gport = vxlan_port_acc_list[num_vps];
    num_vps++;
    trunk_member[num_vps].gport = vxlan_port_acc_list[num_vps];
    num_vps++;
    trunk_member[num_vps].gport = vxlan_port_acc_list[num_vps];
    num_vps++;
    trunk_member[num_vps].gport = vxlan_port_acc_list[num_vps];
    num_vps++;
    if (num_vps) {
        rv = bcm_trunk_set(unit, vp_lag_id_acc, &t_info, num_vps, trunk_member);
        if(BCM_FAILURE(rv))
        {
          printf("\nError executing bcm_trunk_set() : %s\n", bcm_errmsg(rv));
          return rv;
        }
    }
    for (i = 0; i <= 5; i++)
    {
      rv = bcm_multicast_vxlan_encap_get(unit, bcast_group, gport_acc_list[i], vxlan_port_acc_list[i], &encap_id_1_list[i]);
      if(BCM_FAILURE(rv))
      {
        printf("\nError executing bcm_multicast_vxlan_encap_get() : %s\n", bcm_errmsg(rv));
        return rv;
      }
      rv = bcm_multicast_egress_add(unit, bcast_group, vxlan_port_acc_list[i], encap_id_1_list[i]);
      if(BCM_FAILURE(rv))
      {
        printf("\nError executing bcm_multicast_egress_add() : %s\n", bcm_errmsg(rv));
        return rv;
      }
    }
    bcm_gport_t vp_lag_acc_gport;
    BCM_GPORT_TRUNK_SET(vp_lag_acc_gport, vp_lag_id_acc);

    /* Network Trunk */
    rv = bcm_trunk_create(unit, BCM_TRUNK_FLAG_VP, &vp_lag_id_network);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_trunk_create() : %s\n", bcm_errmsg(rv));
      return rv;
    }

    bcm_trunk_info_t_init(&t_info);
    /* Choose RTAG 7 for Load balancing */
    t_info.psc        = BCM_TRUNK_PSC_PORTFLOW;   /* BCM_TRUNK_PSC_RANDOMIZED */
    t_info.dlf_index  = -1;
    t_info.ipmc_index = -1;
    t_info.mc_index   = -1;
    num_vps = 0;
    trunk_member[num_vps].gport = vxlan_port_network1;
    num_vps++;
    trunk_member[num_vps].gport = vxlan_port_network2;
    num_vps++;
    if (num_vps) {
        rv = bcm_trunk_set(unit, vp_lag_id_network, &t_info, num_vps, trunk_member);
        if(BCM_FAILURE(rv))
        {
          printf("\nError executing bcm_trunk_set() : %s\n", bcm_errmsg(rv));
          return rv;
        }
    }
    bcm_gport_t vp_lag_network_gport;
    BCM_GPORT_TRUNK_SET(vp_lag_network_gport, vp_lag_id_network);
    /* Station MAC set up */
    rv = add_to_l2_station(unit, local_mac_network_list[0], vid_network_list[0], BCM_L2_STATION_IPV4);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing add_to_l2_station() : %s\n", bcm_errmsg(rv));
      return rv;
    }

    rv = add_to_l2_station(unit, local_mac_network_list[1], vid_network_list[1], BCM_L2_STATION_IPV4);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing add_to_l2_station() : %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* Payload L2 address */
    rv = vplag_l2_addr_add (unit, BCM_L2_STATIC, payload_remote_mac1, vpn_id, /*vxlan_port_network1*/ vp_lag_network_gport);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing vplag_l2_addr_add() : %s\n", bcm_errmsg(rv));
      return rv;
    }

    rv = vplag_l2_addr_add (unit, BCM_L2_STATIC, payload_remote_mac2, vpn_id, /*vxlan_port_network2*/ vp_lag_network_gport);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing vplag_l2_addr_add() : %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* Network => Access */
    rv = vplag_l2_addr_add (unit, BCM_L2_STATIC, payload_local_mac, vpn_id, vp_lag_acc_gport);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing vplag_l2_addr_add() : %s\n", bcm_errmsg(rv));
      return rv;
    }
    return BCM_E_NONE;
}
void verify(int unit)
{
  bcm_error_t rv=0;
  char   str[512];
  bshell(unit, "hm ieee");
  bshell(unit, "l2 show");
  printf("Case:1 Send known ucast pkt to port_acc1(%d)\n", port_acc1);
  if (BCM_E_NONE != ingress_port_setup(unit, port_acc1, &port_acc1_eid)) {
    printf("ingress_port_setup() failed for port %d\n", port_acc1);
    return;
  }
  if (BCM_E_NONE != egress_port_setup(unit, port_network_1, &port_network_1_eid)) {
    printf("egress_port_setup() failed for port %d\n", port_network_1);
    return;
  }
  if (BCM_E_NONE != egress_port_setup(unit, port_network_2, &port_network_2_eid)) {
    printf("egress_port_setup() failed for port %d\n", port_network_2);
    return;
  }

  snprintf(str, 512, "tx 1 pbm=%d DATA=0x00000000AAAA0000000000018100001508004500002E0000000040FF55B31400000Fc0a80101000102030405060708090A0B0C0D0E0F101112131415161718196764056B; sleep 1;", port_acc1);
  bshell(unit, str);

  bshell(unit, "l2 show");
  bshell(unit, "show c");
  printf("Case:2 Send known ucast pkt to port_acc1(%d)\n", port_acc1);
  snprintf(str, 512, "tx 1 pbm=%d DATA=0x00000000CCCC0000000000118100001508004500002E0000000040FFA4191400000Fc0a80101000102030405060708090A0B0C0D0E0F101112131415161718196764056B; sleep 1;", port_acc1);
  bshell(unit, str);
  bshell(unit, "l2 show");
  bshell(unit, "show c");

  printf("Case:3 Send VXLAN pkt to port_network_1(%d)\n", port_network_1);
  if (BCM_E_NONE != ingress_port_setup(unit, port_network_1, &port_network_1_eid)) {
    printf("ingress_port_setup() failed for port %d\n", port_network_1);
    return;
  }
  if (BCM_E_NONE != ingress_port_setup(unit, port_network_2, &port_network_2_eid)) {
    printf("ingress_port_setup() failed for port %d\n", port_network_2);
    return;
  }
  if (BCM_E_NONE != egress_port_setup(unit, port_acc1, &port_acc1_eid)) {
    printf("egress_port_setup() failed for port %d\n", port_acc1);
    return;
  }
  if (BCM_E_NONE != egress_port_setup(unit, port_acc2, &port_acc2_eid)) {
    printf("egress_port_setup() failed for port %d\n", port_acc2);
    return;
  }
  if (BCM_E_NONE != egress_port_setup(unit, port_acc3, &port_acc3_eid)) {
    printf("egress_port_setup() failed for port %d\n", port_acc3);
    return;
  }
  if (BCM_E_NONE != egress_port_setup(unit, port_acc4, &port_acc4_eid)) {
    printf("egress_port_setup() failed for port %d\n", port_acc4);
    return;
  }
  if (BCM_E_NONE != egress_port_setup(unit, port_acc5, &port_acc5_eid)) {
    printf("egress_port_setup() failed for port %d\n", port_acc5);
    return;
  }
  if (BCM_E_NONE != egress_port_setup(unit, port_acc6, &port_acc6_eid)) {
    printf("egress_port_setup() failed for port %d\n", port_acc6);
    return;
  }

  snprintf(str, 512, "tx 1 pbm=%d DATA=0x0000000022220000000002028100001B080045000068000000004011A4D1C0A801010A0A0A01FFFF211800549798080000000123450000000000BBBB000000000B0B810000150800450000320000000040FFD766C0A8160BC0A80B0B000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1DCBFC8368; sleep 1;", port_network_1);

  bshell(unit, str);
  bshell(unit, "l2 show");
  bshell(unit, "show c");
  printf("Case:4 Send known ucast pkt to port_acc1(%d)\n", port_acc1);
  if (BCM_E_NONE != ingress_port_setup(unit, port_acc1, &port_acc1_eid)) {
    printf("ingress_port_setup() failed for port %d\n", port_acc1);
    return;
  }
  if (BCM_E_NONE != egress_port_setup(unit, port_network_1, &port_network_1_eid)) {
    printf("egress_port_setup() failed for port %d\n", port_network_1);
    return;
  }
  if (BCM_E_NONE != egress_port_setup(unit, port_network_2, &port_network_2_eid)) {
    printf("egress_port_setup() failed for port %d\n", port_network_2);
    return;
  }
  snprintf(str, 512, "tx 1 pbm=%d DATA=0x000000000B0B00000000000181000015000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F62D1F879; sleep 1;", port_acc1);
  bshell(unit, str);
  snprintf(str, 512, "tx 10 pbm=%d dm=0x0b0b sm=0x11 smi=1 vlan=21 Length=68; sleep 1;", port_acc1);
  bshell(unit, str);
  bshell(unit, "l2 show");
  bshell(unit, "show c");

  return;
}

/*
 * This function does the following
 * a)test setup
 * b)actual configuration (Done in configure_vplag_vxlan())
 * c)demonstrates the functionality(done in verify()).
 */

bcm_error_t execute()
{
  bcm_error_t rv;
  int unit = 0;
  bshell(unit, "config show; a ; version; cancun stat");
  if (BCM_FAILURE((rv = test_setup(unit)))) {
    printf("test_setup() failed.\n");
    return -1;
  }

  if (BCM_FAILURE((rv = configure_vplag_vxlan(unit)))) {
    printf("configure_vplag_vxlan() Failed\n");
    return -1;
  }

  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
