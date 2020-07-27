/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

 /* Feature  : Virtual Port Link Aggregation Group.
 *
 *  Usage    : BCM.0> vplag_vlan_vp.c
 *
 *  config   : vplag_vlan_vp_config.bcm
 *
 *  Log file : vplag_vlan_vp_log.txt
 *
 *  Test Topology :
 *
 *                   +------------------------------+
 *                   |                              +
 *                   |                              |
 *                   |                              |
 *  ingress_port     |                              |  egress_port1
 *  +----------------+          BCM SWITCH          +-----------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |  egress_port2
 *                   |                              +-----------------+
 *                   |                              |
 *                   +------------------------------+
 *
 *  Summary:
 *  ========
 *  Virtual Port Link Aggregation Group (VPLAG) is a Data Center access network enhancement that
 *  facilitates grouping Virtual Machines (VMs) into virtual Link Aggregation Groups.
 *  This cint example demo the virtual port created based on vlan port, vplag 
 *
 *  Detailed steps done in the CINT script:
 *  =======================================
 *  1) Step1 - Test Setup (Done in test_setup()):
 *  =============================================
 *  a) Select one ingress port and two egress ports and configure them in Loopback mode.
 *         
 *  2) Step2 - Configuration (Done in vplag_vlan_vp()):
 *  =====================================================================
 *  a) Create layer 2 logical ports or VP for vlan 3, 4 and 100. 
 *  b) Enable hash trunk and L2 and hash seed switch controls.
 *  c) Enable VP multicast replication and create VP lag trunk GPORT.
 *  3) Step3 - Verification (Done in verify())
 *  ==========================================
 *  Send the vlan 3 pkt ingress from egress_port_1, the vlan 3 + SrcMac 0x12345 + VPLAG 1073 willbe *   learned  on L2 entry
 *  BCM.0> tx 1 pbm=egress_port_1 vlan=3
 *  BCM.0> l2 show 
 *  mac=00:00:00:00:00:20 vlan=100 GPORT=0x0 Trunk=1073 Hit
 *  BCM.0> 
 *  Send the vlan 100 pkts ingress from xe6, with same DstMac=0x12345, different SrcMac 0x1,0x3,...
 *  etc. we can see pkt hashed egress out from ports xe4,xe5.
 *  BCM.0> tx 1 pbm=ingress_port  vlan=100 dm=0x12345 sm=0x1
 *  BCM.0> tx 1 pbm=ingress_port  vlan=100 dm=0x12345 sm=0x3
 *  b) Expected Result
 *	===============
 *	We can see pkt hashed egress out from ports egress_port1 and egress_port2. This can be
 *	checked using "show counters".
 */

/* reset c interpreter*/
cint_reset();
bcm_port_t ingress_port;
bcm_port_t egress_port_1;
bcm_port_t egress_port_2;

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

    ingress_port = port_list[0];
    egress_port_1 = port_list[1];
    egress_port_2 = port_list[2];

    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
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

    return BCM_E_NONE;
}


bcm_error_t vplag_vlan_vp(int unit)
{


    bcm_error_t rv = BCM_E_NONE;
    bcm_port_t      p1 = egress_port_1;
    bcm_port_t      p2 = egress_port_2;
    bcm_port_t      p3 = ingress_port;
    bcm_gport_t gp1,gp2,gp3;
    int             vlan_id = 100;
    int             mgroup;
    bcm_vlan_port_t vlan_port1,vlan_port2;
    bcm_vlan_control_vlan_t vlan_ctrl;
    int flags;

    /*
     * hash seeds and l2 and nonuc switch controls
     */
    rv = vplag_add_switchcontrols(unit);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing switch controls %s\n",bcm_errmsg (rv));
        return rv;
    }
    rv = bcm_port_gport_get(unit, egress_port_1, &gp1);
	if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_port_gport_get for egress_port_1 %s.\n", bcm_errmsg (rv));
    }
    rv =bcm_port_gport_get(unit, egress_port_2, &gp2);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_port_gport_get for egress_port_2 %s.\n", bcm_errmsg (rv));
    }
    rv =bcm_port_gport_get(unit, ingress_port, &gp3);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_port_gport_get for ingress_port %s.\n", bcm_errmsg (rv));
    }

    rv =bcm_switch_control_port_set(unit,gp1,bcmSwitchL3EgressMode,TRUE);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing switch control for bcmSwitchL3EgressMode for gp1%s.\n", bcm_errmsg (rv));
    }
    rv =bcm_switch_control_port_set(unit,gp2,bcmSwitchL3EgressMode,TRUE);
	
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing switch control for bcmSwitchL3EgressMode for gp2 setting bcmSwitchL3EgressMode%s.\n", bcm_errmsg (rv));
    }

/* Enable VLAN translation. */
    rv =bcm_vlan_control_port_set(unit,gp1,bcmVlanTranslateIngressEnable,TRUE);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_control_port_set for gp1 setting bcmVlanTranslateIngressEnable%s.\n", bcm_errmsg (rv));
        return rv;
    }
    rv =bcm_vlan_control_port_set(unit,gp1,bcmVlanTranslateEgressEnable,TRUE);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_control_port_set for gp1 setting bcmVlanTranslateEgressEnable %s.\n", bcm_errmsg (rv));
        return rv;
    }
    rv =bcm_vlan_control_port_set(unit,gp1,bcmVlanTranslateIngressMissDrop, TRUE);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_control_port_set for gp1 setting bcmVlanTranslateIngressMissDrop %s.\n", bcm_errmsg (rv));
        return rv;
    }
    rv =bcm_vlan_control_port_set(unit,gp1,bcmVlanTranslateEgressMissDrop, TRUE);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_control_port_set for gp1 setting bcmVlanTranslateEgressMissDrop %s.\n", bcm_errmsg (rv));
        return rv;
    }
    rv =bcm_vlan_control_port_set(unit,egress_port_2,bcmVlanTranslateIngressEnable,TRUE);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_control_port_set for egress_port_2 setting bcmVlanTranslateIngressEnable %s.\n", bcm_errmsg (rv));
        return rv;
    }
    rv =bcm_vlan_control_port_set(unit,egress_port_2,bcmVlanTranslateEgressEnable,TRUE);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_control_port_set for egress_port_2 setting bcmVlanTranslateEgressEnable %s.\n", bcm_errmsg (rv));
        return rv;
    }
    rv =bcm_vlan_control_port_set(unit,egress_port_2,bcmVlanTranslateIngressMissDrop, TRUE);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_control_port_set for egress_port_2 setting bcmVlanTranslateIngressMissDrop %s.\n", bcm_errmsg (rv));
        return rv;
    }
    rv =bcm_vlan_control_port_set(unit,egress_port_2,bcmVlanTranslateEgressMissDrop, TRUE);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_control_port_set for egress_port_2 setting bcmVlanTranslateEgressMissDrop %s.\n", bcm_errmsg (rv));
        return rv;
    }

    bcm_vlan_port_t_init(&vlan_port1);
    vlan_port1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    //vlan_port1.vlan_port_id      = 0x44000001;
   //vlan_port1.flags             = BCM_VLAN_PORT_WITH_ID;
   vlan_port1.match_vlan = 3;
   vlan_port1.egress_vlan = vlan_id;
   vlan_port1.port = gp1;
   rv =bcm_vlan_port_create(unit,&vlan_port1);
   if (BCM_FAILURE (rv))
   {
        printf ("Error executing bcm_vlan_port_create for vlan_port1 %s.\n", bcm_errmsg (rv));
        return rv;
   }
   bcm_vlan_port_t_init(&vlan_port2);
   vlan_port2.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
   //vlan_port2.vlan_port_id      = 0x44000002;
   //vlan_port2.flags             = BCM_VLAN_PORT_WITH_ID;
   vlan_port2.match_vlan = 4;
   vlan_port2.egress_vlan = vlan_id;
   vlan_port2.port = gp2;
   rv =bcm_vlan_port_create(unit,&vlan_port2);
   if (BCM_FAILURE (rv))
   {
        printf ("Error executing bcm_vlan_port_create for vlan_port2 %s.\n", bcm_errmsg (rv));
        return rv;
   }
   rv =bcm_vlan_create(unit, vlan_id);
   if (BCM_FAILURE (rv))
   {
        printf ("Error executing bcm_vlan_create  %s.\n", bcm_errmsg (rv));
        return rv;
   }

   //flags = BCM_MULTICAST_TYPE_VLAN | BCM_MULTICAST_WITH_ID; 
   flags = BCM_MULTICAST_TYPE_VLAN; 
   rv =bcm_multicast_create(unit, flags, &mgroup);
   if (BCM_FAILURE (rv))
   {
        printf ("Error executing bcm_multicast_create %s.\n", bcm_errmsg (rv));
        return rv;
   }
   rv =bcm_multicast_control_set(unit,mgroup,bcmMulticastVpTrunkResolve,TRUE);
   if (BCM_FAILURE (rv))
   {
        printf ("Error executing bcm_multicast_control_set for bcmMulticastVpTrunkResolve %s.\n", bcm_errmsg (rv));
        return rv;
   }
   bcm_vlan_control_vlan_t_init(vlan_ctrl);
   vlan_ctrl.broadcast_group         = mgroup;
   vlan_ctrl.unknown_multicast_group = mgroup;
   vlan_ctrl.unknown_unicast_group   = mgroup;
   //vlan_ctrl.vp_mc_ctrl              = bcmVlanVPMcControlEnable;
   rv =bcm_vlan_control_vlan_set(unit, vlan_id, vlan_ctrl);
   if (BCM_FAILURE (rv))
   {
        printf ("Error executing bcm_vlan_control_vlan_set for vlan_id %s.\n", bcm_errmsg (rv));
        return rv;
   }
   rv =bcm_vlan_gport_add(unit,vlan_id,vlan_port1.vlan_port_id,0);
   if (BCM_FAILURE (rv))
   {
        printf ("Error executing bcm_vlan_gport_add for  vlan_port1.vlan_port_id %s.\n", bcm_errmsg (rv));
        return rv;
   }
   rv =bcm_vlan_gport_add(unit,vlan_id,vlan_port2.vlan_port_id,0);
   if (BCM_FAILURE (rv))
   {
        printf ("Error executing bcm_vlan_gport_add for vlan_port2.vlan_port_id %s.\n", bcm_errmsg (rv));
        return rv;
   }

   rv=bcm_port_vlan_member_set(unit,vlan_port1.vlan_port_id,(BCM_PORT_VLAN_MEMBER_INGRESS|BCM_PORT_VLAN_MEMBER_EGRESS));
   if (BCM_FAILURE (rv))
   {
        printf ("Error executing bcm_port_vlan_member_set for  vlan_port1 %s.\n", bcm_errmsg (rv));
        return rv;
   }
   rv=bcm_port_vlan_member_set(unit,vlan_port2.vlan_port_id,(BCM_PORT_VLAN_MEMBER_INGRESS|BCM_PORT_VLAN_MEMBER_EGRESS));
   if (BCM_FAILURE (rv))
   {
        printf ("Error executing bcm_port_vlan_member_set for  vlan_port2%s.\n", bcm_errmsg (rv));
        return rv;
   }

   /* Create a VPLAG trunk Group */
   int aggindex;
   bcm_trunk_info_t  trunk;
   bcm_trunk_member_t    trunkmember[2];
   print bcm_trunk_create(unit,BCM_TRUNK_FLAG_VP,&aggindex);
   trunk.psc = BCM_TRUNK_PSC_PORTFLOW;

   /* Add Active and standby node's s-channels as the member of VPLAG */
   trunkmember[0].gport=vlan_port1.vlan_port_id;
   trunkmember[1].gport=vlan_port2.vlan_port_id;
   trunk.dlf_index=BCM_TRUNK_UNSPEC_INDEX;
   trunk.mc_index=BCM_TRUNK_UNSPEC_INDEX;
   trunk.ipmc_index=BCM_TRUNK_UNSPEC_INDEX;
   rv =bcm_trunk_set(unit,aggindex,&trunk,2,&trunkmember);
   if (BCM_FAILURE (rv))
   {
        printf ("Error executing bcm_trunk_set %s.\n", bcm_errmsg (rv));
        return rv;
   }
   rv =bcm_vlan_gport_add(unit,vlan_id,vlan_port1.vlan_port_id,0);
   if (BCM_FAILURE (rv))
   {
        printf ("Error executing bcm_vlan_gport_add for vlan_port1 %s.\n", bcm_errmsg (rv));
        return rv;
   }
   rv =bcm_vlan_gport_add(unit,vlan_id,vlan_port2.vlan_port_id,0);
   if (BCM_FAILURE (rv))
   {
        printf ("Error executing bcm_vlan_gport_add for vlan_port2 %s.\n", bcm_errmsg (rv));
        return rv;
   }
   bcm_pbmp_t pbm;
   BCM_PBMP_PORT_ADD(pbm,p1);
   BCM_PBMP_PORT_ADD(pbm,p2);
   BCM_PBMP_PORT_ADD(pbm,p3);
   rv =bcm_vlan_port_add(unit,vlan_id,pbm,pbm);
   if (BCM_FAILURE (rv))
   {
        printf ("Error executing bcm_vlan_port_add %s.\n", bcm_errmsg (rv));
        return rv;
   }

   rv =bcm_switch_control_set(unit,bcmSwitchLoadBalanceHashSelect,7);
   if (BCM_FAILURE (rv))
   {
        printf ("Error executing bcmSwitchLoadBalanceHashSelect %s.\n", bcm_errmsg (rv));
        return rv;
   }
   return rv;
}


/* Vplag add switch controls*/
bcm_error_t vplag_add_switchcontrols(int unit)
{
    int flags;
    bcm_error_t rv = BCM_E_NONE;
   
    /* init hash seeds, field  */
    rv = bcm_switch_control_set(unit, bcmSwitchHashField0Config, BCM_HASH_FIELD_CONFIG_CRC32LO);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcmSwitchHashField0Config %s.\n", bcm_errmsg (rv));
        return rv;
    }
	
    rv = bcm_switch_control_set(unit, bcmSwitchHashField0Config1, BCM_HASH_FIELD_CONFIG_CRC32LO);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcmSwitchHashField0Config1 %s.\n", bcm_errmsg (rv));
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchHashSeed0, 0x33333333);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcmSwitchHashSeed0 %s.\n", bcm_errmsg (rv));
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchHashSeed1, 0x55555555);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcmSwitchHashSeed1 %s.\n", bcm_errmsg (rv));
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchHashField1Config, BCM_HASH_FIELD_CONFIG_CRC16CCITT);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcmSwitchHashField1Config %s.\n", bcm_errmsg (rv));
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchHashField1Config1, BCM_HASH_FIELD_CONFIG_CRC16CCITT);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcmSwitchHashField1Config1 %s.\n", bcm_errmsg (rv));
        return rv;
    }
	
    /* enable trunk hash on non-uc traffic */
    rv = bcm_switch_control_get(unit, bcmSwitchHashControl, &flags);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcmSwitchHashControl %s.\n", bcm_errmsg (rv));
        return rv;
    }
    
    flags |= (BCM_HASH_CONTROL_TRUNK_NUC_ENHANCE |
              BCM_HASH_CONTROL_TRUNK_NUC_SRC |
              BCM_HASH_CONTROL_TRUNK_NUC_DST |
              BCM_HASH_CONTROL_TRUNK_NUC_MODPORT);
    rv =bcm_switch_control_set(unit, bcmSwitchHashControl, flags);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcmSwitchHashControl trunk flags %s.\n", bcm_errmsg (rv));
        return rv;
	}
    /* Block A - L2 packet field selection */
    flags = BCM_HASH_FIELD_SRCMOD |
            BCM_HASH_FIELD_SRCPORT |
            BCM_HASH_FIELD_VLAN |
            BCM_HASH_FIELD_ETHER_TYPE |
            BCM_HASH_FIELD_MACDA_LO |
            BCM_HASH_FIELD_MACDA_MI |
            BCM_HASH_FIELD_MACDA_HI |
            BCM_HASH_FIELD_MACSA_LO |
            BCM_HASH_FIELD_MACSA_MI |
            BCM_HASH_FIELD_MACSA_HI;
    rv =bcm_switch_control_set(unit, bcmSwitchHashL2Field0, flags);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcmSwitchHashL2Field0 L2 packet flags %s.\n", bcm_errmsg (rv));
        return rv;
    }

    /* Block B - L2 packet field selection */
    rv =bcm_switch_control_set(unit, bcmSwitchHashL2Field1, flags);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcmSwitchHashL2Field1 L2 packet flags %s.\n", bcm_errmsg (rv));
        return rv;
    }
      
    return rv;
}

void verify(int unit)
{
    char   str[512];

    bshell(unit, "hm ieee");
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit,egress_port_1, BCM_PORT_DISCARD_NONE));	
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit,ingress_port, BCM_PORT_DISCARD_ALL));
    
    snprintf(str, 512, "tx 1 pbm=%d sm=0X12345 vlan=3; sleep quiet 1", egress_port_1); 
    bshell(unit, str);

    snprintf(str, 512, "l2 show");
    bshell(unit, str);
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit,egress_port_1, BCM_PORT_DISCARD_ALL));	
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit,ingress_port, BCM_PORT_DISCARD_NONE));

    snprintf(str, 512, "tx 1 pbm=%d dm=0X12345 sm=0x1 vlan=100; sleep quiet 1", ingress_port); 	
    bshell(unit, str);
    snprintf(str, 512, "show c");
    bshell(unit, str);

    snprintf(str, 512, "tx 1 pbm=%d dm=0X12345 sm=0x3 vlan=100; sleep quiet 1", ingress_port); 	
    bshell(unit, str);
    snprintf(str, 512, "show c");
    bshell(unit, str);

	
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
    if (BCM_FAILURE((rv = vplag_vlan_vp(unit)))) {
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

