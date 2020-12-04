/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : VPLAG on port extender virtual ports
 *
 * Usage    : BCM.0> cint vplag_port_extender.c
 *
 * config   : vplag_port_extender_config.bcm
 *
 * Log file : vplag_port_extender_log.txt
 *
 *
 *Topology:
 *=========
 *                       +-----------------------------------+
 *                       |                                   |
 *port_1 / VP LAG Port 1 |                                   |
 *+----------------------+                                   |
 *vif - 0xc / 0xabc (MC) |                                   |
 *                       |                                   |
 *                       |                                   |
 *port_2 / VP LAG Port 2 |                                   | port_4 - Physical port
 *+----------------------+             BCM 56470             +-----------------------+
 *vif - 0xd / 0xddd (MC) |                                   |
 *                       |                                   |
 *                       |                                   |
 * port_3 / Virtual Port |                                   |
 *+----------------------+                                   |
 *vif - 0xe / 0xeee (MC) |                                   |
 *                       |                                   |
 *                       +-----------------------------------+
 * Summary:
 * ========
 *   This cint example demostrates configuration of VPLAG on port extender Virtual ports.
 *
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *    1) Step1 - Test Setup (Done in test_setup()):
 *    =============================================
 *      a) Select four ports port_1, port_2, port_3 and port_4.
 *
 *      b) All ports are configured in loopback mode and IFP rules are installed
 *         on all ports to copy all packets ingressing these ports to CPU.
 *
 *      c) On egress ports an additional action is installed to drop the packets
 *         to avoid loopback.
 *
 *      d) Start packet watcher.
 *
 *    2) Step2 - Configuration (Done in configure_vplag_port_extender())
 *    ==================================================================
 *      a) Configure three unicast port extender VPS ext_port1c, ext_port2d and ext_port3e
 *         on physical ports port_1, port2 and port3 respectively.
 *
 *      b) Configure three multicast port extender VPS ext_port_mc1, ext_port_mc2 and ext_port_mc3
 *         on physical ports port_1, port2 and port3 respectively.
 *
 *      c) Configure a VPLAG with ext_port1c and ext_port2d as members.
 *
 *      d) Create vlan 2 and create a multicast group (vlan_mc_group) of type 
 *         BCM_MULTICAST_TYPE_EXTENDER
 *
 *      e) Configure vlan_mc_group as vlan 2's broadcast/unknown_multicast/unknown_unicast group.
 *
 *      f) Add multicast VPs, physical ports and VPLAG as members to the VLAN.
 *
 *      g) Create another multicast "l2mc_group" and add VPLAG and multicast VPs to this group.
 *
 *      f) Create a multicast entry(DA = "01:00:00:00:00:01") pointing to "l2mc_group"
 *
 *
 *    3) Step3 - Verification (Done in verify()):
 *    ===========================================
 *      a) Send known mcast pkt(SA = 00:00:00:00:00:0e, DA = 01:00:00:00:00:01, vlan = 2) to port 4
 *
 *      b) Send known mcast pkt(SA = 00:00:00:00:00:0f, DA = 01:00:00:00:00:01, vlan = 2) to port 4
 *
 *      c) Send unknown mcast pkt(SA = 00:00:00:00:00:0f, DA = 01:00:00:00:00:00, vlan = 2) to port 4
 *
 *      d) Expected Result:
 *      ===================
 *         After step 3.a, two copied are replicated one to VPLAG and another to multicast VP.
 *
 *         After step 3.b, two copied are replicated one to VPLAG(different port of VPLAG
 *         depending on hash result) and another to multicast VP.
 *
 *         After step 3.c, 4 copies are replicated. One each to three multicast VPs and another to
 *         one member of VPLAG(based on hash result)
 */

cint_reset();
bcm_port_t                 port_1 = 1;
bcm_port_t                 port_2 = 2;
bcm_port_t                 port_3 = 3;
bcm_port_t                 port_4 = 4;
/* FP Group/Rule Creation/Destruction*/
bcm_field_group_config_t group_config;
bcm_field_entry_t port_1_eid = BCM_FIELD_ENTRY_INVALID;
bcm_field_entry_t port_2_eid = BCM_FIELD_ENTRY_INVALID;
bcm_field_entry_t port_3_eid = BCM_FIELD_ENTRY_INVALID;
bcm_field_entry_t port_4_eid = BCM_FIELD_ENTRY_INVALID;

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
  int port_list[4], i;
  bcm_field_entry_t entry[4];
  if (BCM_E_NONE != portNumbersGet(unit, port_list, 4)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }
  for (i = 0; i <= 3; i++) {
    entry[i] = BCM_FIELD_ENTRY_INVALID;
  }
  port_1 = port_list[0];
  port_2 = port_list[1];
  port_3 = port_list[2];
  port_4 = port_list[3];

  BCM_IF_ERROR_RETURN(create_ifp_group(unit, &group_config));
  for (i = 0; i <= 2; i++) {
    printf("Using port number:%d as port_%d\n", port_list[i], (i+1));
    if (BCM_E_NONE != ingress_port_setup(unit, port_list[i], &entry[i])) {
      printf("ingress_port_setup() failed for port %d\n", port_list[i]);
      return -1;
    }
  }
  port_1_eid = entry[0];
  port_2_eid = entry[1];
  port_3_eid = entry[2];
  for (i = 3; i <= 3; i++) {
    printf("Using port number:%d as port_%d\n", port_list[i], (i + 1));
    if (BCM_E_NONE != egress_port_setup(unit, port_list[i], &entry[i])) {
      printf("egress_port_setup() failed for port %d\n", port_list[i]);
      return -1;
    }
  }
  port_4_eid = entry[3];

  bshell(unit, "pw start report +raw +decode");
  return BCM_E_NONE;
}


int
vis_uc_vp_create(int unit, bcm_gport_t gport, uint16 vif, bcm_gport_t *ext_port_id)
{
    bcm_extender_port_t ext_port_info;
    int rv;

    bcm_extender_port_t_init(&ext_port_info);
    ext_port_info.port = gport;
    ext_port_info.extended_port_vid = vif;
    rv = bcm_extender_port_add(unit, &ext_port_info);
    *ext_port_id = ext_port_info.extender_port_id;
    return rv;
}

/* Create NIV MC VP based on (port, VIF vector) */
int
vis_mc_vp_create(int unit, bcm_gport_t gport, uint16 vector, bcm_gport_t *ext_id)
{
    bcm_extender_port_t ext_mc_port_info;
    int rv;

    bcm_extender_port_t_init(&ext_mc_port_info);
    ext_mc_port_info.flags = BCM_EXTENDER_PORT_MULTICAST;
    ext_mc_port_info.port = gport;
    ext_mc_port_info.extended_port_vid = vector;
    rv = bcm_extender_port_add(unit, &ext_mc_port_info);
    *ext_id = ext_mc_port_info.extender_port_id;
    return rv;
}

int vplagcreate(int unit, int *tgid)
{
    int rc = BCM_E_NONE;
    int flags=0;
    flags=BCM_TRUNK_FLAG_VP;
    if((rc = bcm_trunk_create(unit, flags, tgid)) < 0)
    {
      printf("Error:%s\r\n",bcm_errmsg(rc));
    }
    return rc;
}
bcm_error_t configure_vplag_port_extender(int unit) 
{

  bcm_trunk_member_t         members[2];
  bcm_trunk_info_t           t_add_info;
  bcm_multicast_t            vlan_mc_group = 0;
  bcm_multicast_t            l2mc_group;
  bcm_l2_addr_t              l2addr;
  bcm_gport_t                gport_1, gport_2,gport_3,gport_4;
  bcm_gport_t                ext_port1a, ext_port1b, ext_port1c, ext_port2d, ext_port3e, ext_port_mc1, ext_port_mc2,ext_port_mc3;
  bcm_vlan_t                 vlan_id = 2;
  bcm_mac_t                  dmac = "01:00:00:00:00:01";
  bcm_if_t                   encap_id;
  int                        tgid1=1089;
  bcm_error_t                rv =  BCM_E_NONE;

  BCM_GPORT_LOCAL_SET(gport_1, port_1);
  BCM_GPORT_LOCAL_SET(gport_2, port_2);
  BCM_GPORT_LOCAL_SET(gport_3, port_3);
  BCM_GPORT_LOCAL_SET(gport_4, port_4);
  rv = bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE);
  if (BCM_FAILURE(rv))
  {
    printf("bcm_switch_control_set failed %s\n", bcm_errmsg(rv));
    return rv;
  }
  /* Enable vplag hash at egress side, it is useful to load balance mcast traffic on vplag */ 
  /*bshell(0, "modreg ING_CONFIG LBID_RTAG=3");*/
  rv = bcm_switch_control_set(unit, bcmSwitchLoadBalanceHashSelect, 3);
  if (BCM_FAILURE(rv))
  {
    printf("bcm_switch_control_set failed %s\n", bcm_errmsg(rv));
    return rv;
  }
  rv = bcm_switch_control_set(unit, bcmSwitchEtagEthertype, 0x8888);
  if (BCM_FAILURE(rv))
  {
    printf("bcm_switch_control_set failed %s\n", bcm_errmsg(rv));
    return rv;
  }
/* drop if packet doesn't have a VNTAG */
  rv = bcm_port_control_set(unit, port_1, bcmPortControlNonEtagDrop, 1);
  if (BCM_FAILURE(rv))
  {
    printf("bcm_port_control_set(port:%d)  bcmPortControlNonEtagDrop failed %s\n", port_1, bcm_errmsg(rv));
    return rv;
  }
  rv = bcm_port_control_set(unit, port_2, bcmPortControlNonEtagDrop, 1);
  if (BCM_FAILURE(rv))
  {
    printf("bcm_port_control_set(port:%d)  bcmPortControlNonEtagDrop failed %s\n", port_2, bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_port_control_set(unit, port_3, bcmPortControlNonEtagDrop, 1);
  if (BCM_FAILURE(rv))
  {
    printf("bcm_port_control_set(port:%d)  bcmPortControlNonEtagDrop failed %s\n", port_3, bcm_errmsg(rv));
    return rv;
  }
/*  delete VNTAG */
  rv = bcm_port_control_set(unit, port_4, bcmPortControlEtagDrop, 1);
  if (BCM_FAILURE(rv))
  {
    printf("bcm_port_control_set(port:%d)  bcmPortControlEtagDrop failed %s\n", port_4, bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_port_control_set(unit, port_4, bcmPortControlEtagEgressDelete , 1);
  if (BCM_FAILURE(rv))
  {
    printf("bcm_port_control_set(port:%d)  bcmPortControlEtagEgressDelete failed %s\n", port_4, bcm_errmsg(rv));
    return rv;
  }

/* Create NIV UC VP for vif 0xc on port1 */
  rv = vis_uc_vp_create(unit, gport_1, 0xc, &ext_port1c);
  if (BCM_FAILURE(rv))
  {
    printf("vis_uc_vp_create(port:%d) failed %s\n", port_1, bcm_errmsg(rv));
    return rv;
  }

/* Create NIV UC VP for vif 0xd on port2 */
  rv = vis_uc_vp_create(unit, gport_2, 0xd, &ext_port2d);
  if (BCM_FAILURE(rv))
  {
    printf("vis_uc_vp_create(port:%d) failed %s\n", port_2, bcm_errmsg(rv));
    return rv;
  }

/* Create NIV UC VP for vif 0xe on port3 */
  rv = vis_uc_vp_create(unit, gport_3, 0xe, &ext_port3e);
  if (BCM_FAILURE(rv))
  {
    printf("vis_uc_vp_create(port:%d) failed %s\n", port_3, bcm_errmsg(rv));
    return rv;
  }
/* Create NIV MC VP for vif vector  on port1 */
  rv = vis_mc_vp_create(unit, gport_1, 0xabc, &ext_port_mc1);
  if (BCM_FAILURE(rv))
  {
    printf("vis_mc_vp_create(port:%d) failed %s\n", port_1, bcm_errmsg(rv));
    return rv;
  }
/* Create NIV MC VP for vif vector  on port2 */
  rv = vis_mc_vp_create(unit, gport_2, 0xddd, &ext_port_mc2);
  if (BCM_FAILURE(rv))
  {
    printf("vis_mc_vp_create(port:%d) failed %s\n", port_2, bcm_errmsg(rv));
    return rv;
  }
/* Create NIV MC VP for vif vector  on port3 */
  rv = vis_mc_vp_create(unit, gport_3, 0xeee, &ext_port_mc3);
  if (BCM_FAILURE(rv))
  {
    printf("vis_mc_vp_create(port:%d) failed %s\n", port_3, bcm_errmsg(rv));
    return rv;
  }
  /* create VP_LAG*/
  rv = vplagcreate(unit, &tgid1);
  if (BCM_FAILURE(rv))
  {
    printf("vplagcreate() failed %s\n", bcm_errmsg(rv));
    return rv;
  }

  bcm_trunk_info_t_init(&t_add_info);
  t_add_info.dlf_index  = BCM_TRUNK_UNSPEC_INDEX;
  t_add_info.mc_index   = BCM_TRUNK_UNSPEC_INDEX;
  t_add_info.ipmc_index = BCM_TRUNK_UNSPEC_INDEX;
  t_add_info.psc = BCM_TRUNK_PSC_PORTFLOW;
  bcm_trunk_member_t_init(&members[0]);
  bcm_trunk_member_t_init(&members[1]);
  members[0].gport = ext_port1c;
  members[1].gport = ext_port2d;
  rv = bcm_trunk_set(unit, tgid1, &t_add_info, 2, members);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_trunk_set() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  /* DLF/Broadcast/Unknown MC configuration */
  /* Create and configure VLAN's broadcast, unknown multicast, and unknown unicast groups */
  rv = bcm_vlan_create(unit, vlan_id);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_vlan_create() : %s\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_EXTENDER, &vlan_mc_group);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_multicast_create() : %s\n", bcm_errmsg(rv));
    return rv;
  }
  bcm_vlan_control_vlan_t vlan_control;
  vlan_control.broadcast_group = vlan_mc_group;
  vlan_control.unknown_multicast_group = vlan_mc_group;
  vlan_control.unknown_unicast_group = vlan_mc_group;
  bcm_vlan_control_vlan_set(unit, vlan_id, vlan_control);
  rv = bcm_multicast_control_set(unit,vlan_mc_group,bcmMulticastVpTrunkResolve,1);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_multicast_control_set() : %s\n", bcm_errmsg(rv));
    return rv;
  }
  /* Add ports to VLAN (MC VPs and DGPPs) */
  rv = bcm_vlan_gport_add(unit, vlan_id, gport_1, 0);
  if (BCM_FAILURE(rv))
  {
    printf("bcm_vlan_gport_add failed %s\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_vlan_gport_add(unit, vlan_id, gport_2, 0);
  if (BCM_FAILURE(rv))
  {
    printf("bcm_vlan_gport_add failed %s\n", bcm_errmsg(rv));
    return rv;
  }
  rv = bcm_vlan_gport_add(unit, vlan_id, gport_3, 0);
  if (BCM_FAILURE(rv))
  {
    printf("bcm_vlan_gport_add failed %s\n", bcm_errmsg(rv));
    return rv;
  }
  rv = bcm_vlan_gport_add(unit, vlan_id, gport_4, 0);
  if (BCM_FAILURE(rv))
  {
    printf("bcm_vlan_gport_add failed %s\n", bcm_errmsg(rv));
    return rv;
  }
  rv = bcm_vlan_gport_add(unit, vlan_id, ext_port_mc1, 0);
  if (BCM_FAILURE(rv))
  {
    printf("bcm_vlan_gport_add failed %s\n", bcm_errmsg(rv));
    return rv;
  }
  rv = bcm_vlan_gport_add(unit, vlan_id, ext_port_mc2, 0);
  if (BCM_FAILURE(rv))
  {
    printf("bcm_vlan_gport_add failed %s\n", bcm_errmsg(rv));
    return rv;
  }
  rv = bcm_vlan_gport_add(unit, vlan_id, ext_port_mc3, 0);
  if (BCM_FAILURE(rv))
  {
    printf("bcm_vlan_gport_add failed %s\n", bcm_errmsg(rv));
    return rv;
  }
  /* Add vp_lag to this DLF group*/
  rv = bcm_vlan_gport_add(unit, vlan_id, ext_port1c, 0);
  if (BCM_FAILURE(rv))
  {
    printf("bcm_vlan_gport_add failed %s\n", bcm_errmsg(rv));
    return rv;
  }
  rv = bcm_vlan_gport_add(unit, vlan_id, ext_port2d, 0);
  if (BCM_FAILURE(rv))
  {
    printf("bcm_vlan_gport_add failed %s\n", bcm_errmsg(rv));
    return rv;
  }
  /* Create MC group */
  bcm_multicast_init(unit);
  l2mc_group = 0x1000004;
  rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_EXTENDER|BCM_MULTICAST_WITH_ID, &l2mc_group);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_multicast_create() : %s\n", bcm_errmsg(rv));
    return rv;
  } 
  printf("\n MCIndex=%x\r\n",l2mc_group);

  rv = bcm_multicast_control_set(unit,l2mc_group,bcmMulticastVpTrunkResolve,1);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_multicast_control_set() : %s\n", bcm_errmsg(rv));
    return rv;
  } /* Add vp_lag to the MC group */
  rv = bcm_multicast_extender_encap_get(unit,0,0,ext_port1c,&encap_id);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_multicast_extender_encap_get() : %s\n", bcm_errmsg(rv));
    return rv;
  }
  rv = bcm_multicast_egress_add(unit, l2mc_group,ext_port1c, encap_id);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_multicast_egress_add() : %s\n", bcm_errmsg(rv));
    return rv;
  }
  rv = bcm_multicast_extender_encap_get(unit,0,0,ext_port2d,&encap_id);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_multicast_extender_encap_get() : %s\n", bcm_errmsg(rv));
    return rv;
  }
  rv = bcm_multicast_egress_add(unit, l2mc_group,ext_port2d, encap_id);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_multicast_egress_add() : %s\n", bcm_errmsg(rv));
    return rv;
  }
  /* Add one Mcast ETag-MC1 to the MC group */
  rv = bcm_multicast_extender_encap_get(unit,0,0,ext_port_mc1,&encap_id);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_multicast_extender_encap_get() : %s\n", bcm_errmsg(rv));
    return rv;
  }
  rv = bcm_multicast_egress_add(unit, l2mc_group,ext_port_mc1, encap_id);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_multicast_egress_add() : %s\n", bcm_errmsg(rv));
    return rv;
  }
  /* Add Multicast address */
  bcm_l2_addr_t_init(l2addr, dmac, vlan_id);
  l2addr.l2mc_group = l2mc_group;
  rv = bcm_l2_addr_add(unit, &l2addr);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_l2_addr_add() : %s\n", bcm_errmsg(rv));
    return rv;
  }
  /* Replace "mod vlan_profile 1 1 l2_pfm=1" */
  rv = bcm_vlan_mcast_flood_set(unit, vlan_id, BCM_VLAN_MCAST_FLOOD_UNKNOWN);
  if(BCM_FAILURE(rv))
  {
    printf("\nError executing bcm_vlan_mcast_flood_set() : %s\n", bcm_errmsg(rv));
    return rv;
  }
  return rv;
}
void verify(int unit)
{
  char   str[512];
  bshell(unit, "hm ieee");
  bshell(unit, "l2 show");
  printf("Case:1 Send known mcast pkt to port_4(%d)\n", port_4);
  if (BCM_E_NONE != ingress_port_setup(unit, port_4, &port_4_eid)) {
    printf("ingress_port_setup() failed for port %d\n", port_4);
    return;
  }
  if (BCM_E_NONE != egress_port_setup(unit, port_1, &port_1_eid)) {
    printf("egress_port_setup() failed for port %d\n", port_1);
    return;
  }
  if (BCM_E_NONE != egress_port_setup(unit, port_2, &port_2_eid)) {
    printf("egress_port_setup() failed for port %d\n", port_2);
    return;
  }
  if (BCM_E_NONE != egress_port_setup(unit, port_3, &port_3_eid)) {
    printf("egress_port_setup() failed for port %d\n", port_3);
    return;
  }

  snprintf(str, 512, "tx 1 pbm=%d DATA=0x01000000000100000000000E81000002aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa; sleep 1;", port_4);
  bshell(unit, str);

  bshell(unit, "l2 show");
  bshell(unit, "show c");
  printf("Case:2 Send known mcast pkt with modified SA to port_4(%d)\n", port_4);
  snprintf(str, 512, "tx 1 pbm=%d DATA=0x01000000000100000000000f81000002aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa; sleep 1;", port_4);
  bshell(unit, str);
  bshell(unit, "l2 show");
  bshell(unit, "show c");

  printf("Case:3 Send unknown mcast pkt to port_4(%d)\n", port_4);
  snprintf(str, 512, "tx 1 pbm=%d DATA=0x01000000000000000000000f81000002aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa; sleep 1;", port_4);
  bshell(unit, str);
  bshell(unit, "l2 show");
  bshell(unit, "show c");
  return;
}

/*
 * This function does the following
 * a)test setup
 * b)actual configuration (Done in configure_vplag_port_extender())
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

  if (BCM_FAILURE((rv = configure_vplag_port_extender(unit)))) {
    printf("configure_vplag_port_extender() Failed\n");
    return -1;
  }

  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
