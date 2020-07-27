/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : VxLAN access_to_network and network_to_access known multicast
 *
 * Usage    : BCM.0> cint acc_to_net_mc.c
 *
 * config   : BCM56275_A1-RIOT.bcm 
 *
 * Log file : acc_to_net_mc_log.txt
 *
 * Test Topology :
 *
 *                  +------------------------------+
 *                  |                              |
 *                  |                              |  net_port_1(to VTEP 1)
 *                  |                              +----------------------+
 *    access_port   |                              |
 * +----------------+          SWITCH              |
 *                  |                              |
 *                  |                              |
 *                  |                              |  net_port_2(to VTEP 2)
 *                  |                              +----------------------+
 *                  |                              |
 *                  +------------------------------+
 * Summary:
 * ========
 *   This cint example demostrates configuration of VXLAN access_to_network and
 *   network_to_access knowm multicast.
 *
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *    1) Step1 - Test Setup (Done in test_setup()):
 *    =============================================
 *      a) Select three ports out of which one port is used as access_port and the
 *         remaining four ports i.e net_port_1 and net_port_2 are used as network ports.
 *
 *      b) Configure access_port as ingress port and all others as egress ports.
 *
 *      c) All ports are configured in loopback mode and IFP rules are installed
 *         on all ports to copy all packets ingressing these ports to CPU.
 *
 *      d) On egress ports an additional action is installed to drop the packets
 *         to avoid loopback.
 *
 *      e) Start packet watcher.
 *
 *    2) Step2 - Configuration (Done in config_vxlan_access_to_network_multicast())
 *    =============================================================================
 *      a) Create a VXLAN VPN and a multicast group(dlf_group) of type "BCM_MULTICAST_TYPE_VXLAN".
 *         Configure this multicast group as the VXLAN VPN's unknown-unicast,unknown-multicast
 *         and broadcast group.
 *
 *      b) Configuire  one VXLAN access VP, i.e acc_vxlan_port and
 *         two VXLAN network VPs i.e, net_vxlan_port_1 and net_vxlan_port_2.
 *
 *      c) Configure another VXLAN network virtual port i.e vxlan_port_dlf for
 *         handling unknown-unicast, unknown-multicast and broadcast traffic.
 *
 *      d) Configure another VXLAN network virtual port i.e known_mc_vxlan_port
 *         for known-multicast for multicast mac "01:00:5e:01:01:01".
 *
 *      e) Create another multicast group(i.e known_mc_group) for multicast "01:00:5e:01:01:01"
 *         and add acc_vxlan_port and known_mc_vxlan_port to this "known_mc_group"
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *       Per VPN, a multicast IP/UDP tunnel is created for DLF traffic,
 *       only one multicast packet is sent out on the selected network port
 *       even if there are more than one network port.  In the following example,
 *       port "net_port_1" is used for the DLF.
 *       For the known multicast (IPMC group address 224.1.1.20), port "net_port_2" is used.
 *
 *       a) Multicast initiation
 *           Send the below packet on port "access_port".
 *
 *           01005E0101010000000011BB81000015
 *           08004500002E0000000040FF84C40A0A
 *           0A01E001011400010203040506070809
 *           0A0B0C0D0E0F10111213141516171819
 *
 *      b) Multicast termination
 *           Send the below packet on port "net_port_2".
 *
 *            01005E01011400000000000381000017
 *            080045000063000100004011D6C9C0A8
 *            0202E0010114FFFF2118004F00000800
 *            00000123450001005E01010100000000
 *            11CC080045000031000100004000D820
 *            C0A80101E00101010001020304050607
 *            08090A0B0C0D0E0F1011121314151617
 *            1819202122
 *
 *      c) Expected Result:
 *      ===================
 
 *       After step 3.a, verify that the below VXLAN encapsulated packet egress out of
 *       network net_port_2.
 *
 *       0100 5e01 0114 0000 0000 3333 8100 0017
 *       0800 4501 0060 0004 0000 1011 ab67 0a0a
 *       1402 e001 0114 ffff 2118 004c 0000 0800
 *       0000 0123 4500 0100 5e01 0101 0000 0000
 *       11bb 0800 4500 002e 0000 0000 40ff 84c4
 *       0a0a 0a01 e001 0114 0001 0203 0405 0607
 *       0809 0a0b 0c0d 0e0f 1011 1213 1415 1617
 *       1819 f9de ff52
 *
 *       After step 3.b, verify that below decapsulated packet egresses out of access_port.
 *
 *       0100 5e01 0101 0000 0000 11cc 8100 0015
 *       0800 4500 0031 0001 0000 4000 d820 c0a8
 *       0101 e001 0101 0001 0203 0405 0607 0809
 *       0a0b 0c0d 0e0f 1011 1213 1415 1617 1819
 *       2021 2270 4d38 c1
 */

cint_reset();
uint16 udp_dp = 8472;   /* UDP dst port used for OTV */
bcm_port_t access_port = 1;
bcm_port_t net_port_1 = 2;
bcm_port_t net_port_2 = 3;

/* FP Group/Rule Creation/Destruction*/
bcm_field_group_config_t group_config;
bcm_field_entry_t access_port_eid = BCM_FIELD_ENTRY_INVALID;
bcm_field_entry_t net_port_1_eid = BCM_FIELD_ENTRY_INVALID;
bcm_field_entry_t net_port_2_eid = BCM_FIELD_ENTRY_INVALID;

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
  int port_list[3], i;
  bcm_field_entry_t entry[3] = {BCM_FIELD_ENTRY_INVALID, BCM_FIELD_ENTRY_INVALID, BCM_FIELD_ENTRY_INVALID};
  if (BCM_E_NONE != portNumbersGet(unit, port_list, 3)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }

  access_port = port_list[0];
  net_port_1 = port_list[1];
  net_port_2 = port_list[2];

  BCM_IF_ERROR_RETURN(create_ifp_group(unit, &group_config));
  printf("Using port number:%d as access_port\n", access_port);
  if (BCM_E_NONE != ingress_port_setup(unit, access_port, &entry[0])) {
    printf("ingress_port_setup() failed for port %d\n", access_port);
    return -1;
  }
  access_port_eid = entry[0];
  for (i = 1; i <= 2; i++) {
    printf("Using port number:%d as net_port_%d\n", port_list[i], (i -1));
    if (BCM_E_NONE != egress_port_setup(unit, port_list[i], &entry[i])) {
      printf("egress_port_setup() failed for port %d\n", port_list[i]);
      return -1;
    }
  }
  net_port_1_eid = entry[1];
  net_port_2_eid = entry[2];


  bshell(unit, "pw start report +raw +decode");
  return BCM_E_NONE;
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

/* Create L3 interface */
int
create_l3_interface(int unit, int flags, bcm_mac_t local_mac, int vid, int vrf, bcm_if_t *intf_id)
{
    bcm_l3_intf_t l3_intf;
    bcm_error_t rv = BCM_E_NONE;
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags |= flags;
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
    l3_intf.l3a_vid = vid;
    l3_intf.l3a_vrf = vrf;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    *intf_id = l3_intf.l3a_intf_id;
    if (BCM_E_NONE != rv)
    {
      printf("bcm_l3_intf_create failed %s\n", bcm_errmsg(rv));
      return rv;
    }
    return rv;
}

int
create_l3_if_id(int unit, int flags, int intf_id, bcm_mac_t local_mac, int vid, int vrf)
{
    bcm_l3_intf_t l3_intf;
    bcm_error_t rv = BCM_E_NONE;
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags = BCM_L3_WITH_ID | flags;
    l3_intf.l3a_intf_id = intf_id;
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
    l3_intf.l3a_vid = vid;
    l3_intf.l3a_vrf = vrf;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    if (BCM_E_NONE != rv)
    {
      printf("bcm_l3_intf_create failed %s\n", bcm_errmsg(rv));
      return rv;
    }
    return rv;
}

/* Create L3 egress object */
int
l3_create_egr_obj(int unit, int flags, int l3_if, bcm_mac_t nh_mac, bcm_gport_t gport,
                   bcm_if_t *egr_obj_id)
{
    bcm_l3_egress_t l3_egress;
    bcm_error_t rv = BCM_E_NONE;
    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.flags  |= flags;
    sal_memcpy(l3_egress.mac_addr,  nh_mac, sizeof(nh_mac));
    l3_egress.intf  = l3_if;
    l3_egress.port = gport;
    rv = bcm_l3_egress_create(unit, 0, &l3_egress, egr_obj_id);
    if (BCM_E_NONE != rv)
    {
      printf("bcm_l3_egress_create failed %s\n", bcm_errmsg(rv));
      return rv;
    }
    return rv;
}

int
vxlan_create_egr_obj(int unit, uint32 flag, int l3_if, bcm_mac_t nh_mac, bcm_gport_t gport,
              int vid, bcm_if_t *egr_obj_id)
{
    bcm_l3_egress_t l3_egress;
    bcm_error_t rv = BCM_E_NONE;
    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.flags = BCM_L3_VXLAN_ONLY | flag;
    l3_egress.intf  = l3_if;
    sal_memcpy(l3_egress.mac_addr,  nh_mac, sizeof(nh_mac));
    l3_egress.vlan = vid;
    l3_egress.port = gport;
    rv = bcm_l3_egress_create(unit, flag, &l3_egress, egr_obj_id);
    if (BCM_E_NONE != rv)
    {
      printf("bcm_l3_egress_create failed %s\n", bcm_errmsg(rv));
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
    if (BCM_E_NONE != rv)
    {
      printf("bcm_l2_station_add failed %s\n", bcm_errmsg(rv));
      return rv;
    }
    return rv;
}

int
add_to_l2_table(int unit, bcm_mac_t mac, bcm_vpn_t vpn_id, int port, int stat)
{
    bcm_l2_addr_t l2_addr;
    bcm_error_t rv = BCM_E_NONE;
    bcm_l2_addr_t_init(&l2_addr, mac, vpn_id);
    if (stat)
    {
      l2_addr.flags = BCM_L2_STATIC;
    }
    l2_addr.port = port;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (BCM_E_NONE != rv)
    {
      printf("bcm_l2_addr_add failed %s\n", bcm_errmsg(rv));
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
    rv = bcm_switch_control_set(unit, bcmSwitchVxlanEntropyEnable, FALSE);
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

int
tunnel_initiator_setup(int unit, bcm_ip_t lip, bcm_ip_t rip, int dp, int sp, int ttl, int *tid)
{
    bcm_tunnel_initiator_t tnl_init;
    bcm_error_t rv = BCM_E_NONE;
    bcm_tunnel_initiator_t_init(&tnl_init);
    tnl_init.type  = bcmTunnelTypeVxlan;
    tnl_init.ttl = ttl;
    tnl_init.sip = lip;
    tnl_init.dip = rip;
    tnl_init.udp_dst_port = dp;
    tnl_init.udp_src_port = sp;
    rv = bcm_vxlan_tunnel_initiator_create(unit, &tnl_init);
    *tid = tnl_init.tunnel_id;
    if (BCM_E_NONE != rv)
    {
      printf("bcm_vxlan_tunnel_initiator_create failed %s\n", bcm_errmsg(rv));
      return rv;
    }
    return rv;
}

int
tunnel_terminator_setup(int unit, bcm_ip_t rip, bcm_ip_t lip, bcm_vlan_t net_vid,
                        int tunnel_init_id, int *term_id)
{
    bcm_tunnel_terminator_t tnl_term;
    bcm_error_t rv = BCM_E_NONE;
    bcm_tunnel_terminator_t_init(&tnl_term);
    tnl_term.type = bcmTunnelTypeVxlan;
    tnl_term.sip = rip;    /* For MC tunnel, Don't care */
    tnl_term.dip = lip;
    tnl_term.tunnel_id = tunnel_init_id;
    tnl_term.flags = BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
    if (net_vid != -1) {
        tnl_term.vlan = net_vid;  /* MC tunnel only - for Bud check */
    }
    rv = bcm_vxlan_tunnel_terminator_create(unit, &tnl_term);
    *term_id = tnl_term.tunnel_id;
    if (BCM_E_NONE != rv)
    {
      printf("bcm_vxlan_tunnel_terminator_create failed %s\n", bcm_errmsg(rv));
      return rv;
    }
    return rv;
}

int
create_vxlan_vpn(int unit, int vpn_id, int vnid, bcm_multicast_t bc,
                 bcm_multicast_t mc, bcm_multicast_t uuc)
{
    bcm_vxlan_vpn_config_t vpn_info;
    bcm_error_t rv = BCM_E_NONE;
    bcm_vxlan_vpn_config_t_init(&vpn_info);
    vpn_info.flags = BCM_VXLAN_VPN_ELAN | BCM_VXLAN_VPN_WITH_ID | BCM_VXLAN_VPN_WITH_VPNID;
    vpn_info.vpn  = vpn_id;
    vpn_info.vnid = vnid;
    vpn_info.broadcast_group         = bc;
    vpn_info.unknown_multicast_group = mc;
    vpn_info.unknown_unicast_group   = uuc;
    rv = bcm_vxlan_vpn_create(unit, &vpn_info);
    if (BCM_E_NONE != rv)
    {
      printf("bcm_vxlan_vpn_create failed %s\n", bcm_errmsg(rv));
      return rv;
    }
    return rv;
}

int
sdtag_create_vxlan_vpn(int unit, int vpn_id, int vnid, bcm_multicast_t bc,
              bcm_multicast_t mc, bcm_multicast_t uuc)
{
    bcm_vxlan_vpn_config_t vpn_info;
    bcm_error_t rv = BCM_E_NONE;
    bcm_vxlan_vpn_config_t_init(&vpn_info);
    /* Service tag will be removed going into UDP tunnel */
    vpn_info.flags = BCM_VXLAN_VPN_ELAN | BCM_VXLAN_VPN_WITH_ID | BCM_VXLAN_VPN_WITH_VPNID |
                     BCM_VXLAN_VPN_SERVICE_TAGGED | BCM_VXLAN_VPN_SERVICE_VLAN_DELETE;
    vpn_info.vpn  = vpn_id;
    vpn_info.vnid = vnid;
    vpn_info.broadcast_group         = bc;
    vpn_info.unknown_multicast_group = mc;
    vpn_info.unknown_unicast_group   = uuc;
    vpn_info.egress_service_tpid     = 0x8100;
    rv = bcm_vxlan_vpn_create(unit, &vpn_info);
    if (BCM_E_NONE != rv)
    {
      printf("bcm_vxlan_vpn_create failed %s\n", bcm_errmsg(rv));
      return rv;
    }
    return rv;
}

int
create_vxlan_acc_vp(int unit, bcm_vpn_t vpn, uint32 flags, bcm_gport_t port,
                    bcm_vxlan_port_match_t criteria,
                    bcm_if_t egr_obj, bcm_vlan_t vid, bcm_gport_t *vp)
{
    bcm_vxlan_port_t vxlan_port;
    bcm_error_t rv = BCM_E_NONE;

    bcm_vxlan_port_t_init(&vxlan_port);
    vxlan_port.flags = BCM_VXLAN_PORT_SERVICE_TAGGED | flags;
    vxlan_port.match_port =        port;
    vxlan_port.criteria =          criteria;
    vxlan_port.egress_if =         egr_obj;
    vxlan_port.match_vlan =        vid;
    rv = bcm_vxlan_port_add(unit, vpn, &vxlan_port);
    *vp = vxlan_port.vxlan_port_id;
    if (BCM_E_NONE != rv)
    {
      printf("bcm_vxlan_port_add failed %s\n", bcm_errmsg(rv));
      return rv;
    }
    return rv;
}

int
sdtag_create_vxlan_acc_vp(int unit, bcm_vpn_t vpn, uint32 flags, bcm_gport_t port,
                    bcm_vxlan_port_match_t criteria, bcm_if_t egr_obj,
                    bcm_vlan_t vid, bcm_vlan_t sdtag, bcm_gport_t *vp)
{
    bcm_vxlan_port_t vxlan_port;
    bcm_error_t rv = BCM_E_NONE;
    bcm_vxlan_port_t_init(&vxlan_port);
    vxlan_port.flags = BCM_VXLAN_PORT_SERVICE_TAGGED | flags;
    vxlan_port.criteria =          criteria;
    vxlan_port.match_port =        port;
    vxlan_port.egress_if =         egr_obj;
    vxlan_port.match_vlan =        vid;
    vxlan_port.egress_service_tpid = 0x8100;
    vxlan_port.egress_service_vlan = sdtag;
    rv = bcm_vxlan_port_add(unit, vpn, &vxlan_port);
    *vp = vxlan_port.vxlan_port_id;
    if (BCM_E_NONE != rv)
    {
      printf("bcm_vxlan_port_add failed %s\n", bcm_errmsg(rv));
      return rv;
    }
    return rv;
}

int
create_vxlan_net_vp(int unit, bcm_vpn_t vpn, uint32 flags, bcm_gport_t port,
                    bcm_vxlan_port_match_t criteria, bcm_if_t egr_obj,
                    bcm_gport_t tun_init, bcm_gport_t tun_term, bcm_gport_t *vp)
{
    bcm_vxlan_port_t vxlan_port;
    bcm_error_t rv = BCM_E_NONE;
    bcm_vxlan_port_t_init(&vxlan_port);
    vxlan_port.flags = BCM_VXLAN_PORT_NETWORK | BCM_VXLAN_PORT_EGRESS_TUNNEL |
                       BCM_VXLAN_PORT_SERVICE_TAGGED | flags;
    vxlan_port.match_port =        port;
    vxlan_port.criteria =          criteria;
    vxlan_port.egress_if =         egr_obj;
    vxlan_port.egress_tunnel_id =  tun_init;
    vxlan_port.match_tunnel_id =   tun_term;
    /* vpn_id parameter is not care for net VP */
    rv = bcm_vxlan_port_add(unit, vpn, &vxlan_port);
    *vp = vxlan_port.vxlan_port_id;
    if (BCM_E_NONE != rv)
    {
      printf("bcm_vxlan_port_add failed %s\n", bcm_errmsg(rv));
      return rv;
    }
    return rv;
}

bcm_error_t config_vxlan_access_to_network_multicast(int unit)
{
  bcm_error_t rv = BCM_E_NONE;
  /* VPN */
  bcm_vpn_t vpn_id = 0x7010;
  uint32 vnid = 0x12345;
  bcm_multicast_t dlf_group = 0;

  /* Used to create a dummy VPN that is used to create network VP */
  int rsvd_network_vpn = 0x7001;
  int rsvd_network_vpn_vnid = 0xeeee;

  /*
   * Access side
   */
  /* Access side MAC addresses are not really used */
  bcm_mac_t acc_dummy_mac = "00:00:01:00:00:01";

  bcm_vlan_t acc_vid = 21;
  bcm_if_t acc_intf_id = 1;
  bcm_gport_t acc_gport = BCM_GPORT_INVALID;
  bcm_if_t acc_egr_obj;
  int acc_vxlan_port;
  bcm_if_t acc_encap_id;

  /*
   * Network side
   */

  /* Tunnel */
  uint8 ttl = 16;

  uint16 udp_sp = 0xffff;
  int vrf = 0;

  /* Network 1 */

  bcm_vlan_t net_vid_1 = 22;
  bcm_if_t net_intf_id_1 = 2;
  bcm_gport_t net_gport_1 = BCM_GPORT_INVALID;
  bcm_if_t net_egr_obj_1;
  int net_vxlan_port_1;
  bcm_mac_t net_local_mac_1 = "00:00:00:00:22:22";
  bcm_mac_t net_remote_mac_1 = "00:00:00:00:00:02";
  bcm_ip_t tnl_local_ip_1  = 0x0a0a0a01;    /* 10.10.10.1 */
  bcm_ip_t tnl_remote_ip_1 = 0xC0A80101;    /* 192.168.1.1 */
  int tnl_uc_init_id_1, tnl_uc_term_id_1;

  /* Network 2 */

  bcm_vlan_t net_vid_2 = 23;
  bcm_if_t net_intf_id_2 = 3;
  bcm_gport_t net_gport_2 = BCM_GPORT_INVALID;
  bcm_if_t net_egr_obj_2;
  int net_vxlan_port_2;
  bcm_mac_t net_local_mac_2 = "00:00:00:00:33:33";
  bcm_mac_t net_remote_mac_2 = "00:00:00:00:00:03";
  bcm_ip_t tnl_local_ip_2  = 0x0a0a1402;    /* 10.10.20.2 */
  bcm_ip_t tnl_remote_ip_2 = 0xC0A80202;    /* 192.168.2.2 */
  int tnl_uc_init_id_2, tnl_uc_term_id_2;

  /*
   * DLF/BC tunnel and virtual port
   */
  bcm_mac_t dlf_mac = "01:00:5e:00:00:0A"; /* 224.0.0.10 */
  bcm_ip_t tnl_dlf_dip = 0xe000000A;       /* 224.0.0.10 */

  /* Assume the DLF/BC only forwards to network port 1 */
  bcm_if_t egr_obj_dlf;
  int vxlan_port_dlf;
  bcm_if_t encap_id_dlf;

  /* Still need to create two DLF tunnels, because the incoming DLF may arrive at any network ports */
  int tnl_dlf_init_id_1, tnl_dlf_term_id_1;
  int tnl_dlf_init_id_2, tnl_dlf_term_id_2;

  /* Unicast payload MAC */
  bcm_mac_t payload_da_1 = "00:00:00:00:11:aa";
  bcm_mac_t payload_sa   = "00:00:00:00:11:bb";
  bcm_mac_t payload_da_2 = "00:00:00:00:11:cc";


/* Global settings */
  rv = do_vxlan_global_setting(unit);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing do_vxlan_global_setting() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  /* Access port settings */
  rv = do_vxlan_access_port_settings(unit, access_port);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing do_vxlan_access_port_settings() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  /* Network port settings */
  rv = do_vxlan_net_port_settings(unit, net_port_1);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing do_vxlan_net_port_settings() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  rv = do_vxlan_net_port_settings(unit, net_port_2);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing do_vxlan_net_port_settings() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_port_gport_get(unit, access_port, &acc_gport);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_port_gport_get() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_port_gport_get(unit, net_port_1, &net_gport_1);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_port_gport_get() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_port_gport_get(unit, net_port_2, &net_gport_2);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_port_gport_get() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  /* VLAN settings */
  rv = vlan_create_add_port(unit, acc_vid, access_port);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing vlan_create_add_port() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  rv = vlan_create_add_port(unit, net_vid_1, net_port_1);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing vlan_create_add_port() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  rv = vlan_create_add_port(unit, net_vid_2, net_port_2);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing vlan_create_add_port() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  /* Multicast group for the segment MC/BC */
  rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_VXLAN, &dlf_group);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_multicast_create() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  /* Create VXLAN VPN */
  rv = sdtag_create_vxlan_vpn(unit, vpn_id, vnid, dlf_group, dlf_group, dlf_group);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing sdtag_create_vxlan_vpn() : %s\n", bcm_errmsg(rv));
    return rv;
  }
  /* Create VXLAN VPN for assigning Network-VP */
  rv = sdtag_create_vxlan_vpn(unit, rsvd_network_vpn, rsvd_network_vpn_vnid, dlf_group, dlf_group, dlf_group);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing sdtag_create_vxlan_vpn() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  /* L3 interface and egress object for access - note use a dummy MAC address */
  rv = create_l3_interface(unit, 0, acc_dummy_mac, acc_vid, vrf, &acc_intf_id);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing create_l3_interface() : %s\n", bcm_errmsg(rv));
    return rv;
  }
  rv = vxlan_create_egr_obj(unit, 0, acc_intf_id, acc_dummy_mac, acc_gport, acc_vid, &acc_egr_obj);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing vxlan_create_egr_obj() : %s\n", bcm_errmsg(rv));
    return rv;
  }
  /* Create VXLAN VP for access port */
  rv = sdtag_create_vxlan_acc_vp(unit, vpn_id, BCM_VXLAN_PORT_SERVICE_VLAN_ADD,
                      acc_gport, BCM_VXLAN_PORT_MATCH_PORT_VLAN,
                      acc_egr_obj, acc_vid, acc_vid, &acc_vxlan_port);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing sdtag_create_vxlan_acc_vp() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  /*
   * Network port 1
   */
  /* Tunnel Setup (UC Initiator & Terminator) */
  rv = tunnel_initiator_setup(unit, tnl_local_ip_1, tnl_remote_ip_1, udp_dp, udp_sp, ttl, &tnl_uc_init_id_1);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing tunnel_initiator_setup() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  rv = tunnel_terminator_setup(unit, tnl_remote_ip_1, tnl_local_ip_1, -1, tnl_uc_init_id_1, &tnl_uc_term_id_1);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing tunnel_terminator_setup() : %s\n", bcm_errmsg(rv));
    return rv;
  }
  /* L3 interface and egress object */
  rv = create_l3_interface(unit, 0, net_local_mac_1, net_vid_1, vrf, &net_intf_id_1);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing create_l3_interface() : %s\n", bcm_errmsg(rv));
    return rv;
  }
  rv = vxlan_create_egr_obj(unit, 0, net_intf_id_1, net_remote_mac_1, net_gport_1,
                 net_vid_1, &net_egr_obj_1);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing vxlan_create_egr_obj() : %s\n", bcm_errmsg(rv));
    return rv;
  }
  /* Create VXLAN VP */
  rv = create_vxlan_net_vp(unit, rsvd_network_vpn, 0, net_gport_1, BCM_VXLAN_PORT_MATCH_VN_ID,
                      net_egr_obj_1, tnl_uc_init_id_1, tnl_uc_term_id_1, &net_vxlan_port_1);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing create_vxlan_net_vp() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  /* Tunnel Setup (DLF/BC Initiator & Terminator) */
  rv = tunnel_initiator_setup(unit, tnl_local_ip_1, tnl_dlf_dip, udp_dp, udp_sp, ttl, &tnl_dlf_init_id_1);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing tunnel_initiator_setup() : %s\n", bcm_errmsg(rv));
    return rv;
  }
  rv = tunnel_terminator_setup(unit, tnl_remote_ip_1, tnl_dlf_dip, net_vid_1,
                          tnl_dlf_init_id_1, &tnl_dlf_term_id_1);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing tunnel_terminator_setup() : %s\n", bcm_errmsg(rv));
    return rv;
  }
  /*
   * Network port 2
   */
  /* Tunnel Setup (UC Initiator & Terminator) */
  rv = tunnel_initiator_setup(unit, tnl_local_ip_2, tnl_remote_ip_2, udp_dp, udp_sp, ttl, &tnl_uc_init_id_2);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing tunnel_initiator_setup() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  rv = tunnel_terminator_setup(unit, tnl_remote_ip_2, tnl_local_ip_2, -1, tnl_uc_init_id_2, &tnl_uc_term_id_2);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing tunnel_terminator_setup() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  /* L3 interface and egress object */
  rv = create_l3_interface(unit, 0, net_local_mac_2, net_vid_2, vrf, &net_intf_id_2);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing create_l3_interface() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  rv = vxlan_create_egr_obj(unit, 0, net_intf_id_2, net_remote_mac_2, net_gport_2, net_vid_2, &net_egr_obj_2);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing vxlan_create_egr_obj() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  /* Create VXLAN VP */
  rv = create_vxlan_net_vp(unit, rsvd_network_vpn, 0, net_gport_2, BCM_VXLAN_PORT_MATCH_VN_ID,
                net_egr_obj_2, tnl_uc_init_id_2, tnl_uc_term_id_2, &net_vxlan_port_2);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing create_vxlan_net_vp() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  /* Tunnel Setup (DLF/BC Initiator & Terminator) */
  rv = tunnel_initiator_setup(unit, tnl_local_ip_2, tnl_dlf_dip, udp_dp, udp_sp, ttl, &tnl_dlf_init_id_2);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing tunnel_initiator_setup() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  rv = tunnel_terminator_setup(unit, tnl_remote_ip_2, tnl_dlf_dip, net_vid_2,
                          tnl_dlf_init_id_2, &tnl_dlf_term_id_2);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing tunnel_terminator_setup() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  /*
   * Egress object for DLF/BC VXLAN VP - uses same interface as UC VXLAN network 1
   */
  rv = vxlan_create_egr_obj(unit, BCM_L3_IPMC, net_intf_id_1, dlf_mac, net_gport_1, net_vid_1, &egr_obj_dlf);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing vxlan_create_egr_obj() : %s\n", bcm_errmsg(rv));
    return rv;
  }
  /* Create DLF/BC VXLAN VP at network port 1 */
  rv = create_vxlan_net_vp(unit, rsvd_network_vpn, BCM_VXLAN_PORT_MULTICAST, net_gport_1,
                      BCM_VXLAN_PORT_MATCH_NONE, egr_obj_dlf,
                      tnl_dlf_init_id_1, tnl_dlf_term_id_1, &vxlan_port_dlf);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing create_vxlan_net_vp() : %s\n", bcm_errmsg(rv));
    return rv;
  }


  /* Station MAC set up */
  rv = add_to_l2_station(unit, net_local_mac_1, net_vid_1, BCM_L2_STATION_IPV4);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing add_to_l2_station() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  rv = add_to_l2_station(unit, net_local_mac_2, net_vid_2, BCM_L2_STATION_IPV4);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing add_to_l2_station() : %s\n", bcm_errmsg(rv));
    return rv;
  }
  /* DLF MC group set up - should contain all Access ports and Network DLF port */
  rv = bcm_multicast_vxlan_encap_get(unit, dlf_group, acc_gport, acc_vxlan_port, &acc_encap_id);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_multicast_vxlan_encap_get() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_multicast_egress_add(unit, dlf_group, acc_vxlan_port, acc_encap_id);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_multicast_egress_add() : %s\n", bcm_errmsg(rv));
    return rv;
  }
  rv = bcm_multicast_vxlan_encap_get(unit, dlf_group, net_gport_1, vxlan_port_dlf, &encap_id_dlf);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_multicast_vxlan_encap_get() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_multicast_egress_add(unit, dlf_group, vxlan_port_dlf, encap_id_dlf);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_multicast_egress_add() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  /*
   * Add unicast Payload L2 address to L2 table to avoid DLF flooding
  add_to_l2_table(unit, payload_sa, vpn_id, acc_vxlan_port, 0);
  add_to_l2_table(unit, payload_da_1, vpn_id, net_vxlan_port_1, 0);
  add_to_l2_table(unit, payload_da_2, vpn_id, net_vxlan_port_2, 0);
   */

  /*=====================================
   * Known multicast Setup - access <--> Network port 2
   *====================================*/
  bcm_multicast_t known_mc_group;
  bcm_mac_t known_mc_mac = "01:00:5e:01:01:01";
  bcm_mac_t known_mc_tnl_mac = "01:00:5e:01:01:14";
  bcm_ip_t known_mc_tnl_dip = 0xe0010114;       /* 224.1.1.20 */
  bcm_if_t known_mc_egr_obj;
  int known_mc_vxlan_port;
  bcm_if_t known_mc_encap;
  int known_mc_tnl_init_id, known_mc_tnl_term_id;

  /* Multicast group for the known MC group */
  rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_VXLAN, &known_mc_group);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_multicast_create() : %s\n", bcm_errmsg(rv));
    return rv;
  }


  /* Tunnel Setup (Known MC Initiator & Terminator) */
  rv = tunnel_initiator_setup(unit, tnl_local_ip_2, known_mc_tnl_dip, udp_dp, udp_sp, ttl, &known_mc_tnl_init_id);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing tunnel_initiator_setup() : %s\n", bcm_errmsg(rv));
    return rv;
  }
  rv = tunnel_terminator_setup(unit, tnl_remote_ip_2, known_mc_tnl_dip, -1,  /* net_vid_2, */
                          known_mc_tnl_init_id, &known_mc_tnl_term_id);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing tunnel_terminator_setup() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  /* Egress object for known MC VP - uses same interface as UC VXLAN network 2 */
  rv = vxlan_create_egr_obj(unit, BCM_L3_IPMC, net_intf_id_2, known_mc_tnl_mac, net_gport_2,
                 net_vid_2, &known_mc_egr_obj);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing vxlan_create_egr_obj() : %s\n", bcm_errmsg(rv));
    return rv;
  }
  /* Create known MC VP */
  rv = create_vxlan_net_vp(unit, rsvd_network_vpn, BCM_VXLAN_PORT_MULTICAST, net_gport_2,
                      BCM_VXLAN_PORT_MATCH_VN_ID, known_mc_egr_obj,
                      known_mc_tnl_init_id, known_mc_tnl_term_id, &known_mc_vxlan_port);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing create_vxlan_net_vp() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  rv = add_to_l2_station(unit, known_mc_tnl_mac, net_vid_2, BCM_L2_STATION_IPV4);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing add_to_l2_station() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  /* Known MC group replication - contain all access VPs & MC VP included in the MC group */
  rv = bcm_multicast_vxlan_encap_get(unit, known_mc_group, net_gport_2, known_mc_vxlan_port, &known_mc_encap);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_multicast_vxlan_encap_get() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_multicast_egress_add(unit, known_mc_group, known_mc_vxlan_port, known_mc_encap);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_multicast_egress_add() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_multicast_vxlan_encap_get(unit, known_mc_group, acc_gport, acc_vxlan_port, &acc_encap_id);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_multicast_vxlan_encap_get() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_multicast_egress_add(unit, known_mc_group, acc_vxlan_port, acc_encap_id);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_multicast_egress_add() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  /* Add MC group address to L2 table */
  bcm_l2_addr_t mc_addr;
  bcm_l2_addr_t_init(&mc_addr, known_mc_mac, vpn_id);
  mc_addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;
  mc_addr.l2mc_group = known_mc_group;
  rv = bcm_l2_addr_add (unit, &mc_addr);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing add_to_l2_station() : %s\n", bcm_errmsg(rv));
    return rv;
  }
  return BCM_E_NONE;
}
void verify(int unit)
{
  bcm_error_t rv=0;
  char   str[512];
  bshell(unit, "hm ieee");
  printf(" \n **************TEST 1  Multicast initiation access_port:(%d)  to net_port_2:(%d)**************\n", access_port, net_port_2);
  bshell(unit, "l2 show");
  /*
  *
  *01005E0101010000000011BB81000015
  *08004500002E0000000040FF84C40A0A
  *0A01E001011400010203040506070809
  *0A0B0C0D0E0F10111213141516171819
  */

  if (BCM_E_NONE != ingress_port_setup(unit, access_port, &access_port_eid)) {
    printf("ingress_port_setup() failed for port %d\n", access_port);
    return;
  }
  if (BCM_E_NONE != egress_port_setup(unit, net_port_2, &net_port_2_eid)) {
    printf("egress_port_setup() failed for port %d\n", net_port_2);
    return;
  }
  snprintf(str, 512, "tx 1 pbm=%d DATA=0x01005E0101010000000011BB8100001508004500002E0000000040FF84C40A0A0A01E0010114000102030405060708090A0B0C0D0E0F10111213141516171819; sleep 1", access_port);
  bshell(unit, str);

  bshell(unit, "l2 show");
  bshell(unit, "show c");
  printf(" \n **************TEST 2  Multicast termination net_port_2:(%d) to  access_port:(%d) **************\n", net_port_2, access_port);

   /*
   *       01005E01011400000000000381000017
   *       080045000063000100004011D6C9C0A8
   *       0202E0010114FFFF2118004F00000800
   *       00000123450001005E01010100000000
   *       11CC080045000031000100004000D820
   *       C0A80101E00101010001020304050607
   *       08090A0B0C0D0E0F1011121314151617
   *       1819202122
  */

  if (BCM_E_NONE != egress_port_setup(unit, access_port, &access_port_eid)) {
    printf("egress_port_setup() failed for port %d\n", access_port);
    return;
  }
  if (BCM_E_NONE != ingress_port_setup(unit, net_port_2, &net_port_2_eid)) {
    printf("ingress_port_setup() failed for port %d\n", net_port_2);
    return;
  }

  snprintf(str, 512, "tx 1 pbm=%d DATA=0x01005E01011400000000000381000017080045000063000100004011D6C9C0A80202E0010114FFFF2118004F0000080000000123450001005E0101010000000011CC080045000031000100004000D820C0A80101E0010101000102030405060708090A0B0C0D0E0F10111213141516171819202122; sleep 1;", net_port_2);
  bshell(unit, str);
  bshell(unit, "l2 show");
  bshell(unit, "show c");
  return;
}

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
  bshell(unit, "config show; a ; version; cancun stat");
  if (BCM_FAILURE((rv = test_setup(unit)))) {
    printf("test_setup() failed.\n");
    return -1;
  }

  if (BCM_FAILURE((rv = config_vxlan_access_to_network_multicast(unit)))) {
    printf("config_vxlan_access_to_network_multicast() Failed\n");
    return -1;
  }

  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
