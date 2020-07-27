/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

 /* Feature  : Virtual Port Link Aggregation Group.
 *
 *  Usage    : BCM.0> vplag_L3_multicast.c
 *
 *  config   :vplag_L3_multicast_config.bcm 
 *
 *  Log file : vplag_L3_multicast_log.txt
 *
 *  Test Topology :
 *
 *                   	   +-----------------------------------+
 *                         |                                   |
 *                         |                                   |
 *  Port 1 / VP LAG Port 1 |                                   | Port 3 - Physical port
 *  +----------------------+                                   +-----------------------+
 *  VFI - 0xc / 0xabc (MC) |                                   |      VLAN ID - 2 
 *       VLAN ID - 2       |                                   |
 *                         |             BCM 56770             |
 *                         |                                   |
 *  Port 2 / VP LAG Port 2 |                                   | Port 4 - Physical port
 *  +----------------------+                                   +-----------------------+
 *  VFI - 0xd / 0xddd (MC) |                                   |      VLAN ID - 2
 *       VLAN ID - 2       |                                   |
 *                         |                                   |
 *                         +-----------------------------------+
 *
 *  Summary:
 *  ========
 *  This cint example demostrates configuration of VPLAG port extender L3 multicast routing feature using virtual ports.
 *
 *  Detailed steps done in the CINT script:
 *  =======================================
 *  1) Step1 - Test Setup (Done in test_setup()):
 *  =============================================
 *  a) Select Four ports out of which one is ingress port and other three are egress ports for case 1.
 *  b) For case2 also select 1 ingress and 3 egress ports.
 *  c) All ports are configured in loopback mode.
 *  d) on egress ports an additional action drop is configured to drop the packets to avoid loopback.
 *  e) Start Packet watcher
 *         
 *  2) Step2 - Configuration (Done in vplag_execute()):
 *  ===================================================
 *  a) Create two VPLAG ports and create multicast group of type "BCM_EXTENDER_PORT_MULTICAST".
 *  b) VP_LAG Memeber - VP LAG Port 1 & VP LAG Port 2
 *  c) L3 Multicast Memeber - VP LAG Port 1 (MC), VP LAG Port 2 (MC) & Port 3 (Physical Port)
 *     BCM.0> multicast show
 *     Group 0x2000002 (L3)
 *     port xe0, encap id 400007
 *     port xe1, encap id 400008
 *     port xe2, encap id 10
 *  d)EXTENDER Multicast Memeber - VP LAG Port 1, VP LAG Port 1 (MC), VP LAG Port 2, VP LAG Port 2 (MC),
 *                            Port1, Port2, Port 3 & Port 4 (Physical Ports)
 *     BCM.0> multicast show
 *     Group 0xe000001 (EXTENDER)
 *     port xe0, encap id 400002
 *     port xe0, encap id 400004
 *     port xe1, encap id 400003
 *     port xe1, encap id 400005
 *     port xe0, encap id -1
 *     port xe1, encap id -1
 *     port xe2, encap id -1
 *     port xe3, encap id -1
 *  e) Adding L2 entries
 *     BCM.0> l2 show
 *      mac=aa:00:00:00:00:00 vlan=2 GPORT=0x0 modid=0 port=0/cpu0 Static CPU
 *      mac=02:00:00:00:00:00 vlan=2 GPORT=0x0 modid=0 port=0/cpu0 Static CPU
 *  3) Step3 - Verification (Done in verify())
 *  ==========================================
 *  Case 1:
 *  ******
 *  Send the below packet on port3 which is ingress in case 1 and packet receives on port1,2 and 4.
 * 
 *  0100 5e00 000a 0000 0000 0002 8100 0002
 *  0800 4500 002e 0000 0000 3fff 98c5 0101
 *  0101 e000 000a 0001 0203 0000 0000 0000
 *  0000 0000 0000 0000 0000 0000 0000 0000
 *  7414 0a3e
 *  b) Expected Result
 *     ===============
 * Two copies are replicated in this mcast group
 *  1. Port1 receive one copy, the etag VID is 0xabc(mcast)
 *        0100 5e00 000a 0000 0000 0002 8888 0000
 *        0abc 0000 8100 0002 0800 4500 002e 0000
 *        0000 3fff 98c5 0101 0101 e000 000a 0001
 *        0203 0000 0000 0000 0000 0000 0000 0000
 *        0000 0000 0000 0000 a059 83f7
 *  Port2 receive one copy, the etag VID is 0xddd(mcast)
 *        0100 5e00 000a 0000 0000 0002 8888 0000
 *        0ddd 0000 8100 0002 0800 4500 002e 0000
 *        0000 3fff 98c5 0101 0101 e000 000a 0001
 *        0203 0000 0000 0000 0000 0000 0000 0000
 *        0000 0000 0000 0000 5e0f eaf3
 *  Port1 receive etag VID 0xc, this packet belong to vplag
 *        0100 5e00 000a 0000 0000 0002 8888 0000
 *        000c 0000 8100 0002 0800 4500 002e 0000
 *        0000 3fff 98c5 0101 0101 e000 000a 0001
 *        0203 0000 0000 0000 0000 0000 0000 0000
 *        0000 0000 0000 0000 2878 2bf6
 *  Port4 receive following non VID packet
 *        0100 5e00 000a 0000 0000 0002 8100 0002
 *        0800 4500 002e 0000 0000 3fff 98c5 0101
 *        0101 e000 000a 0001 0203 0000 0000 0000
 *        0000 0000 0000 0000 0000 0000 0000 0000
 *        7414 0a3e
 *  BCM.0> l2 show
 *  mac=aa:00:00:00:00:00 vlan=2 GPORT=0x0 modid=0 port=0/cpu0 Static CPU
 *  mac=00:00:00:00:00:02 vlan=2 GPORT=0x3 modid=0 port=3/xe2 Hit
 *  mac=02:00:00:00:00:00 vlan=2 GPORT=0x0 modid=0 port=0/cpu0 Static CPU
 *
 * Case 2:
 *  ******
 * Send the below pkt to port 1
 *  0100 5e00 000a 0000 0000 0002 8888 0000
 *  000c 0000 8100 0002 0800 4500 002e 0000
 *  0000 3fff 98c5 0101 0202 e000 000a 0001
 *  0203 0000 0000 0000 0000 0000 0000 0000
 *  0000 0000 0000 0000 2878 2bf6
 * Expected result
 *  A new mac address is learned on VPLAG
 *   mac=00:00:00:00:00:02 vlan=2 GPORT=0x0 Trunk=2114 Hit
 */
 
/* reset c interpreter*/
cint_reset();
bcm_port_t egress_port_1;
bcm_port_t egress_port_2;
bcm_port_t ingress_port_1;
bcm_port_t egress_port_3;

/* This function populates the port_list_arr with the required
   number of ports
*/
bcm_error_t portNumbersGet(int unit, bcm_port_t *port_list, int num_ports)
{
    int i = 0, port = 0, rv = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit,&configP);
    if(BCM_FAILURE(rv)) {
        printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = configP.e;
    for (i = 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp,i)&& (port < num_ports)) {
            port_list[port] = i;
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
 * Configures the port in loopback mode
 */
bcm_error_t ingress_port_setup(int unit, bcm_port_t port)
{
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and sets the discard bit
 * in the PORT table
 */
bcm_error_t egress_port_setup(int unit, bcm_port_t port)
{
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));
    return BCM_E_NONE;
}

/*
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
    int port_list[4];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 4)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    egress_port_1 = port_list[0];
    egress_port_2 = port_list[1];
    ingress_port_1 = port_list[2];
    egress_port_3 = port_list[3];
    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port_1)) {
        printf("ingress_port_setup() failed");
        return -1;
    }
    
   if (BCM_E_NONE != egress_port_setup(unit, egress_port_1)) {
        printf("egress_port_setup() failed for port:%d", egress_port_1);
        return -1;
   }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port_2)) {
        printf("egress_port_setup() failed for port:%d", egress_port_2);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port_3)) {
        printf("egress_port_setup() failed for port:%d", egress_port_3);
        return -1;
    }

    return BCM_E_NONE;
}


int
vis_uc_vp_create(int unit, bcm_gport_t gport, uint16 vif, bcm_gport_t *ext_port_id)
{
    bcm_extender_port_t ext_port_info;
    bcm_error_t rv = BCM_E_NONE;
    bcm_extender_port_t_init(&ext_port_info);
    ext_port_info.port = gport;
    ext_port_info.extended_port_vid = vif;
    rv = bcm_extender_port_add(unit, &ext_port_info);
	if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_extender_port_add  %s.\n", bcm_errmsg (rv));
        return rv;
    }
    *ext_port_id = ext_port_info.extender_port_id;
    return rv;
}
/* Create NIV MC VP based on (port, VIF vector) */
int
vis_mc_vp_create(int unit, bcm_gport_t gport, uint16 vector, bcm_gport_t *ext_id)
{
    bcm_extender_port_t ext_mc_port_info;
    bcm_error_t rv = BCM_E_NONE;

    bcm_extender_port_t_init(&ext_mc_port_info);
    ext_mc_port_info.flags = BCM_EXTENDER_PORT_MULTICAST;
    ext_mc_port_info.port = gport;
    ext_mc_port_info.extended_port_vid = vector;
    rv = bcm_extender_port_add(unit, &ext_mc_port_info);
	if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_extender_port_add  %s.\n", bcm_errmsg (rv));
        return rv;
    }
    *ext_id = ext_mc_port_info.extender_port_id;
    return rv;
}

int vplag_create(int unit, int tgid,int withid)
{
    int         flags=0;
    bcm_error_t rv = BCM_E_NONE;

    flags=BCM_TRUNK_FLAG_VP;

    if(withid)
    {
        flags|= BCM_TRUNK_FLAG_WITH_ID;
    }

    rv=bcm_trunk_create(unit,flags,&tgid);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_trunk_create %s.\n", bcm_errmsg (rv));
        return rv;
    }

    return rv;
}
int vplag_l3_intf_create(int unit, bcm_if_t l3a_intf_id, bcm_vlan_t l3a_vid, bcm_mac_t mac_addr)
{
    bcm_l3_intf_t  l3_intf;
    bcm_error_t rv = BCM_E_NONE;
    
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags = (BCM_L3_ADD_TO_ARL | BCM_L3_WITH_ID);
    l3_intf.l3a_intf_id = l3a_intf_id;
    sal_memcpy(l3_intf.l3a_mac_addr, mac_addr, sizeof(l3_intf.l3a_mac_addr));
    l3_intf.l3a_vid = l3a_vid;
    rv=bcm_l3_intf_create(unit, &l3_intf);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_l3_intf_create %s.\n", bcm_errmsg (rv));
        return rv;
    }
	
    return BCM_E_NONE;
}
bcm_error_t vplag_execute(int unit)
{
    bcm_trunk_chip_info_t           ta_chip_info;
    bcm_trunk_member_t              members[2];
    bcm_trunk_info_t                t_add_info;
    bcm_multicast_t                 vlan_mc_group = 0;
    bcm_multicast_t                 l3_mc_group;
    bcm_ipmc_addr_t                 ipmc_addr;
    bcm_gport_t                     gport_1, gport_2,gport_3,gport_4;
    bcm_gport_t                     ext_port1a, ext_port1b, ext_port1c, ext_port2d, ext_port3e, ext_port_mc1, ext_port_mc2,
                                    ext_port_mc3;
    bcm_port_t                      port_1=egress_port_1, port_2=egress_port_2, port_3=ingress_port_1, port_4=egress_port_3;
    bcm_vlan_t                      vlan_id = 2;
    bcm_vlan_t                      R1_vid = 2;
    bcm_vlan_t                      R2_vid = 2;
    bcm_mac_t                       R1_mac = {0x02,0x0,0x0,0x0,0x0,0x00};
    bcm_mac_t                       R2_mac = {0xaa,0x0,0x0,0x0,0x0,0x00};
    bcm_mac_t                       stnmac={0,0,0,0,0,0};
    bcm_if_t                        encap;
    bcm_if_t                        R1_intf = 10;
    bcm_if_t                        R2_intf = 20;
    int                             tgid1=0;
    int                             tgid2=0;
    bcm_error_t rv = BCM_E_NONE;

    BCM_GPORT_LOCAL_SET(gport_1, port_1);
    BCM_GPORT_LOCAL_SET(gport_2, port_2);
    BCM_GPORT_LOCAL_SET(gport_3, port_3);
    BCM_GPORT_LOCAL_SET(gport_4, port_4);

    rv = bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcmSwitchL3EgressMode %s.\n", bcm_errmsg (rv));
        return rv;
    }
    
    rv = bcm_switch_control_set(unit, bcmSwitchEtagEthertype, 0x8888);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcmSwitchL3EgressMode %s.\n", bcm_errmsg (rv));
        return rv;
    }
    /* drop if packet doesn't have a VNTAG */
    rv =  bcm_port_control_set(unit, port_1, bcmPortControlNonEtagDrop, 1);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcmPortControlNonEtagDrop for egress_port_1 %s.\n", bcm_errmsg (rv));
        return rv;
    }
    rv = bcm_port_control_set(unit, port_2, bcmPortControlNonEtagDrop, 1);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcmPortControlNonEtagDrop for for egress_port_2 %s.\n", bcm_errmsg (rv));
        return rv;
    }
    /*  delete VNTAG */
    rv = bcm_port_control_set(unit, port_3, bcmPortControlEtagDrop, 1);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcmPortControlEtagDrop  for ingress_port_1 %s.\n", bcm_errmsg (rv));
        return rv;
    }
    rv =bcm_port_control_set(unit, port_3, bcmPortControlEtagEgressDelete , 1);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcmPortControlEtagEgressDelete for ingress_port_1 %s.\n", bcm_errmsg (rv));
        return rv;
    }
    rv =bcm_port_control_set(unit, port_4, bcmPortControlEtagDrop, 1);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcmPortControlEtagDrop for egress_port_3 %s.\n", bcm_errmsg (rv));
        return rv;
    }
    rv =bcm_port_control_set(unit, port_4, bcmPortControlEtagEgressDelete , 1);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcmPortControlEtagEgressDelete for egress_port_3 %s.\n", bcm_errmsg (rv));
        return rv;
    }

    /* Create NIV UC VP for vif 0xc on ingress_port_1 */
    rv = vis_uc_vp_create(unit, gport_1, 0xc, &ext_port1c);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing vis_uc_vp_create for vif egress_port_1 %s.\n", bcm_errmsg (rv));
        return rv;
    }
    /* Create NIV UC VP for vif 0xd on ingress_port_2 */
    rv = vis_uc_vp_create(unit, gport_2, 0xd, &ext_port2d);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing vis_uc_vp_create for vif egress_port_2 %s.\n", bcm_errmsg (rv));
        return rv;
    }
    /* Create NIV MC VP for vif vector 0xabc on port1 */
    rv = vis_mc_vp_create(unit, gport_1, 0xabc, &ext_port_mc1);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing vis_mc_vp_create for vif vector egress_port_1 %s.\n", bcm_errmsg (rv));
        return rv;
    }
    /* Create NIV MC VP for vif vector 0xddd on port2 */
    rv = vis_mc_vp_create(unit, gport_2, 0xddd, &ext_port_mc2);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing vis_mc_vp_create for vif vector egress_port_2 %s.\n", bcm_errmsg (rv));
        return rv;
    }
    bcm_trunk_chip_info_get(unit, &ta_chip_info);
    tgid2 = ta_chip_info.vp_id_min;
    tgid1 = ta_chip_info.vp_id_min+1;

    /* create VP_LAG */
    rv = vplag_create(unit, tgid2,1);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing vplag_create for tgid2  %s.\n", bcm_errmsg (rv));
        return rv;
    }
    rv = vplag_create(unit, tgid1,1);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing vplag_create for tgid1 %s.\n", bcm_errmsg (rv));
        return rv;
    }

    bcm_trunk_info_t_init(&t_add_info);
    t_add_info.dlf_index  = BCM_TRUNK_UNSPEC_INDEX;
    t_add_info.mc_index   = BCM_TRUNK_UNSPEC_INDEX;
    t_add_info.ipmc_index = BCM_TRUNK_UNSPEC_INDEX;
    t_add_info.psc = BCM_TRUNK_PSC_PORTFLOW;
    print bcm_trunk_member_t_init(&members[0]);
    print bcm_trunk_member_t_init(&members[1]);
    members[0].gport = ext_port1c;
    members[1].gport = ext_port2d;
    rv = bcm_trunk_set(0, tgid1, &t_add_info, 2, members);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_trunk_set %s.\n", bcm_errmsg (rv));
        return rv;
    }

    /* Create VLAN */ 
    rv = bcm_vlan_create(unit, vlan_id);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_create for vlan_id:%d %s.\n", vlan_id,bcm_errmsg (rv));
        return rv;
    }
    /* Create Extender multicast group */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_EXTENDER, &vlan_mc_group);
	if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_multicast_create for extender multicast group vlan_mc_group:%d %s.\n", vlan_mc_group,bcm_errmsg (rv));
        return rv;
    }

    bcm_vlan_control_vlan_t vlan_control;
    vlan_control.broadcast_group = vlan_mc_group;
    vlan_control.unknown_multicast_group = vlan_mc_group;
    vlan_control.unknown_unicast_group = vlan_mc_group;
    rv =bcm_vlan_control_vlan_set(unit, vlan_id, vlan_control);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_control_vlan_set for vlan_id:%d %s.\n", vlan_id,bcm_errmsg (rv));
        return rv;
    }     
    /* Set VP Trunk Resolve */
    rv = bcm_multicast_control_set(unit,vlan_mc_group,bcmMulticastVpTrunkResolve,1);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_multicast_control_set for bcm_multicast_control_set.\n", vlan_id,bcm_errmsg (rv));
        return rv;
    }
    /* Add ports to VLAN (MC VPs and DGPPs) */
    rv =bcm_vlan_gport_add(unit, vlan_id, gport_4, 0);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_gport_add for vlan_id:%d %s.\n", vlan_id,bcm_errmsg (rv));
        return rv;
    }
    rv =bcm_vlan_gport_add(unit, vlan_id, gport_3, 0);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_gport_add for vlan_id:%d gport_3:%d %s.\n", vlan_id,gport_3,bcm_errmsg (rv));
        return rv;
    }
    rv =bcm_vlan_gport_add(unit, vlan_id, gport_2, 0);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_gport_add for vlan_id:%d gport_2:%d %s.\n", vlan_id, gport_2,bcm_errmsg (rv));
        return rv;
    }
    rv =bcm_vlan_gport_add(unit, vlan_id, gport_1, 0);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_gport_add for vlan_id:%d gport_1:%d %s.\n", vlan_id,gport_1,bcm_errmsg (rv));
        return rv;
    }
    rv =bcm_vlan_gport_add(unit, vlan_id, ext_port_mc1, 0);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_gport_add for vlan_id:%d ext_port_mc1:%d %s.\n", vlan_id,ext_port_mc1,bcm_errmsg (rv));
        return rv;
    }
    rv =bcm_vlan_gport_add(unit, vlan_id, ext_port_mc2, 0);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_gport_add for vlan_id:%d ext_port_mc2:%d %s.\n", vlan_id,ext_port_mc2,bcm_errmsg (rv));
        return rv;
    }
    /*Add vp_lag to this DLF group*/
    rv =bcm_vlan_gport_add(unit, vlan_id, ext_port1c, 0);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_gport_add for vlan:%d and ext_port1c:%d %s.\n", vlan,ext_port1c,bcm_errmsg (rv));
        return rv;
    }
    rv =bcm_vlan_gport_add(unit, vlan_id, ext_port2d, 0);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_gport_add for vlan_id:%d and ext_port2d:%d %s.\n", vlan_id,ext_port2d,bcm_errmsg (rv));
        return rv;
    }

    /* Replace "mod vlan_profile 1 1 l2_pfm=1" */
    rv =bcm_vlan_mcast_flood_set(unit, vlan_id, BCM_VLAN_MCAST_FLOOD_UNKNOWN);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_mcast_flood_set for vlan_id:%d %s.\n", vlan_id, bcm_errmsg (rv));
        return rv;
    }

    /* Create L3 interface */
    rv = vplag_l3_intf_create (unit, R1_intf, R1_vid, R1_mac);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing vplag_l3_intf_create for R1_intf%d %s.\n", R1_intf,bcm_errmsg (rv));
        return rv;
    }
  
    rv = vplag_l3_intf_create (unit, R2_intf, R2_vid, R2_mac);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing vplag_l3_intf_create for R2_intf:%d %s.\n", R2_intf,bcm_errmsg (rv));
        return rv;
    }
    
    /* Create L3 multicast group */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L3, &l3_mc_group);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_multicast_create  %s.\n", bcm_errmsg (rv));
        return rv;
    }

    /* Add physical port 3 to this L3 mcgroup */
    rv = bcm_multicast_l3_encap_get(unit, l3_mc_group, gport_3, R1_intf, &encap);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_multicast_l3_encap_get for gport_3 %s.\n", bcm_errmsg (rv));
        return rv;
    }
    rv = bcm_multicast_egress_add(unit, l3_mc_group,gport_3,encap);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_multicast_egress_add for gport_3 %s.\n", bcm_errmsg (rv));
        return rv;
    }
    /* Add vplag to this L3 mcgroup */
    /* Add vplag member 1 */
    rv = bcm_multicast_l3_encap_get(unit, l3_mc_group, ext_port1c, R2_intf, &encap);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_multicast_l3_encap_get while adding vplag member1 %s.\n", bcm_errmsg (rv));
        return rv;
    }
    rv = bcm_multicast_egress_add(unit, l3_mc_group,ext_port1c,encap);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_multicast_egress_add for ext_port1c %s.\n", bcm_errmsg (rv));
        return rv;
    }
    /* Add vplag member 2 */
    rv = bcm_multicast_l3_encap_get(unit, l3_mc_group, ext_port2d, R2_intf, &encap);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_multicast_l3_encap_get while add vplag member2 %s.\n", bcm_errmsg (rv));
        return rv;
    }
    rv = bcm_multicast_egress_add(unit, l3_mc_group,ext_port2d ,encap);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_multicast_egress_add for ext_port2d %s.\n", bcm_errmsg (rv));
        return rv;
    }

    /* Set VP Trunk Resolve */
    rv = bcm_multicast_control_set(unit,l3_mc_group,bcmMulticastVpTrunkResolve,1);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_multicast_control_set %s.\n", bcm_errmsg (rv));
        return rv;
    }

    /* Add IP multicast entry */
    bcm_ipmc_addr_t_init(&ipmc_addr);
    ipmc_addr.mc_ip_addr = 0xE000000A;
    ipmc_addr.vid        = 1;                               /* Or, IPMC.L3_IIF */
    ipmc_addr.group      = l3_mc_group;
    ipmc_addr.flags      = BCM_IPMC_SOURCE_PORT_NOCHECK;
    rv = bcm_ipmc_add(unit, &ipmc_addr);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_ipmc_add %s.\n", bcm_errmsg (rv));
        return rv;
    }
    
    return BCM_E_NONE;
}


 
 
void verify(int unit)
{
    bcm_error_t rv=0;
    char   str[512];
    bshell(unit, "hm ieee");
	printf("Case:1 Send multicast pkt to ingress_port_1(%d)\n", ingress_port_1);
	BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit,ingress_port_1, BCM_PORT_DISCARD_NONE));
    snprintf(str, 512, "tx 1 pbm=%d data=0x01005e00000a0000000000028100000208004500002e000000003fff98c501010101e000000a000102030000000000000000000000000000000000000000000074140a3e; sleep 1;", ingress_port_1); 
	bshell(unit, str);
	bshell(unit, "l2 show");
	bshell(unit, "show c");
	printf("Case:2 Send VPLAG packet on VP_LAGPORT port_1(%d)\n", egress_port_1);
	BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit,ingress_port_1, BCM_PORT_DISCARD_ALL));
	BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit,egress_port_1, BCM_PORT_DISCARD_NONE));
       snprintf(str, 512, "tx 1  pbm=%d data=0x01005e00000a00000000000288880000000c00008100000208004500002e000000003fff98c501010202e000000a0002030000000000000000000000000000000000000000000028782bf6; sleep 1;",egress_port_1); 
	bshell(unit, str);
	bshell(unit, "l2 show");
	
}


bcm_error_t execute()
{
    bcm_error_t rv;
    int unit = 0;

    bshell(unit, "config show; a ; version; cancun stat");
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return rv;
    }
    if (BCM_FAILURE((rv = vplag_execute(unit)))) {
        printf("vplag_vlan_vp() failed.\n");
        return rv;
    }
    
    verify(unit);
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

