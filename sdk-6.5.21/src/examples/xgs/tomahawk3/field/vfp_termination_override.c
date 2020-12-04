/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : VFP Tunnel termination Overrides
 *
 *  Usage    : BCM.0> cint vfp_termination_override.c
 *
 *  config   : field_config.bcm
 *
 *  Log file : vfp_termination_override_log.txt
 *
 *  Test Topology :
 *
 *                   +------------------------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *  +----------------+          SWITCH              +-----------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   +------------------------------+
 *
 *  Summary:
 *  ========
 *   Due to pipeline optimizations, TH3 stages now behave differently.
 *   Some of the HW signals are not available to the stages which were
 *   available in earlier chips. E.g. VID change done by VFP is not
 *   visible to MY_STATION_TCAM. Hence MY_STATION_TCAM will not recognize
 *   the changed packet, which may result into the lookup failure. To solve
 *   this, VFP has introduced a set of dedicated actions.
 *       Goal is to change the VID of a packet at the lookup stage. Then,
 *   this changed VID, should hit one of L3 interfaces (programmed in
 *   MY_STATION_TCAM) and go for tunnel termination. But in TH3 this
 *   won't work due to pipeline optimizations. Hence VFP itself has to
 *   play the role of initiating route lookup
 *
 *  Detailed steps done in the CINT script:
 *  ====================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ================================
 *       a) Select one ingress and three egress ports and configure them in
 *          Loopback mode. Install a rule to copy incoming packets to CPU and
 *          additional action to drop the packets when it loops back on egress
 *          ports. Start packet watcher.
 *
 *    2) Step2 - Configuration (Done in ConfigureVfpTermination()).
 *    ===================================================
 *       a) Create L3 interface
 *       b) Create L3 egress object and associate it with the above L3 interface
         c) Create entry in the route table for dip = 0x0A0A0A0B
 *       d) Create MPLS POP action and add tunnel initiation for DMAC=0xDD,
 *                                                               VID=0x64.
 *       e) Add tunnel initiation for DMAC=0xDD, VID=0xC8. Here 0xC8 is the new VID
 *             that will be assigned by VFP.
 *       f) Create entry in VFP:
               QUAL:   InPort -> ing_port1
               ACTION: VlanNew -> vid_new (0xC8)
 *             STAT    : Counter with STAT ID = 1
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==============================
 *       a) Expected Result:
 *          ================
 *          i) Send PACKET1. Expected is that PACKET2 should egress from egr_port.
 *                  IF opt_ChangeVlanInVfp == 0.
 *          ii) Expected Result: Send PACKET1. Expected is that PACKET3 should egress from egr_port.
 *                  IF opt_ChangeVlanInVfp == 1.
 *     
 *          PACKET1:
 *               DMAC         = 0xDD
 *               VID          = 0x64
 *               MPLS label   = 1000
 *               DIP          = 0x0A0A0A0B
 *     
 *          PACKET2:
 *               DMAC         = 0xDD
 *               VID          = 0x64
 *               DIP          = 0x0A0A0A0B
 *     
 *          PACKET3:
 *               DMAC         = 0xDD
 *               VID          = 0xC8 (200)
 *               DIP          = 0x0A0A0A0B
 *
 * NOTE: Use opt_* variables to change the test variant
 */

cint_reset();

/****************   GENERIC UTILITY FUNCTIONS STARTS  **************************
 */
                                                                           /* */
// Gets the next valid port after the last valid returned port.            /* */
                                                                           /* */
bcm_port_t PORT_ANY = -1;                                                  /* */
bcm_port_t __last_returned_port = 1;                                       /* */
// Get next valid Ethernet port                                            /* */
bcm_port_t                                                                 /* */
portGetNextE(int unit, bcm_port_t PreferredPort)                           /* */
{                                                                          /* */
  int i=0;                                                                 /* */
  int rv=0;                                                                /* */
  bcm_port_config_t configP;                                               /* */
  bcm_pbmp_t ports_pbmp;                                                   /* */
                                                                           /* */
  if( PreferredPort != PORT_ANY )                                          /* */
  {                                                                        /* */
    printf("Using preferred port %d\n", PreferredPort);                    /* */
    return PreferredPort;                                                  /* */
  }                                                                        /* */
                                                                           /* */
  rv = bcm_port_config_get(unit, &configP);                                /* */
  if(BCM_FAILURE(rv)) {                                                    /* */
    printf("\nError in retrieving port configuration: %s %d.\n",           /* */
    bcm_errmsg(rv), __last_returned_port);                                 /* */
    exit;                                                                  /* */
  }                                                                        /* */
                                                                           /* */
  ports_pbmp = configP.e;    // configP.hg;                                /* */
  for (i= __last_returned_port; i < BCM_PBMP_PORT_MAX; i++)                /* */
  {                                                                        /* */
    if ( BCM_PBMP_MEMBER(&ports_pbmp, i) )                                 /* */
    {                                                                      /* */
        __last_returned_port = i+1; // point to a probable next port       /* */
        return i;                                                          /* */
    }                                                                      /* */
  }                                                                        /* */
                                                                           /* */
  printf("ERROR: Not enough ports %d\n", __last_returned_port);            /* */
  exit;                                                                    /* */
}                                                                          /* */
                                                                           /* */
//                                                                         /* */
// Configures the port in loopback mode and installs                       /* */
// an IFP rule. This IFP rule copies the packets ingressing                /* */
// on the specified port to CPU.                                           /* */
//                                                                         /* */
bcm_error_t                                                                /* */
ingress_port_setup(int unit, bcm_port_t port, int LoopBackType)            /* */
{                                                                          /* */
  bcm_field_qset_t  qset;                                                  /* */
  bcm_field_group_t group = 9998;                                          /* */
  int                pri = group;                                          /* */
  bcm_field_entry_t entry = 99981;                                         /* */
  int         fp_statid = 9998;                                            /* */
  const bcm_field_stat_t stats[2] = { bcmFieldStatPackets,                 /* */
                                      bcmFieldStatBytes };                 /* */
                                                                           /* */
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, LoopBackType));    /* */
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port,                     /* */
                            BCM_PORT_DISCARD_NONE));                       /* */
                                                                           /* */
  BCM_FIELD_QSET_INIT(qset);                                               /* */
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);                         /* */
                                                                           /* */
  BCM_IF_ERROR_RETURN(bcm_field_group_create_id(unit, qset, pri, group));  /* */
                                                                           /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_create_id(unit, group, entry));      /* */
                                                                           /* */
  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port,          /* */
            BCM_FIELD_EXACT_MATCH_MASK));                                  /* */
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,                    /* */
            bcmFieldActionCopyToCpu, 1, 222));                             /* */
                                                                           /* */
  BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group, 2,             /* */
            stats, fp_statid));                                            /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry,             /* */
            fp_statid));                                                   /* */
                                                                           /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));               /* */
  return BCM_E_NONE;                                                       /* */
}                                                                          /* */
                                                                           /* */
//                                                                         /* */
// Configures the port in loopback mode and installs                       /* */
// an IFP rule. This IFP rule copies the packets ingressing                /* */
// on the specified port to CPU. Port is also configured                   /* */
// to discard all packets. This is to avoid continuous                     /* */
// loopback of the packet.                                                 /* */
//                                                                         /* */
bcm_error_t                                                                /* */
egress_port_setup(int unit, bcm_port_t port, int LoopBackType)             /* */
{                                                                          /* */
  bcm_field_qset_t  qset;                                                  /* */
  bcm_field_group_t group = 9999;                                          /* */
  int                pri = group;                                          /* */
  bcm_field_entry_t entry = 99991;                                         /* */
  int         fp_statid = 9999;                                            /* */
  const bcm_field_stat_t stats[2] = { bcmFieldStatPackets,                 /* */
                                      bcmFieldStatBytes };                 /* */
                                                                           /* */
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, LoopBackType));    /* */
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port,                     /* */
            BCM_PORT_DISCARD_ALL));                                        /* */
                                                                           /* */
  BCM_FIELD_QSET_INIT(qset);                                               /* */
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);                         /* */
                                                                           /* */
  BCM_IF_ERROR_RETURN(bcm_field_group_create_id(unit, qset, pri, group));  /* */
                                                                           /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_create_id(unit, group, entry));      /* */
                                                                           /* */
  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port,          /* */
            BCM_FIELD_EXACT_MATCH_MASK));                                  /* */
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,                    /* */
            bcmFieldActionCopyToCpu, 1, 223));                             /* */
                                                                           /* */
  BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group, 2,             /* */
            stats, fp_statid));                                            /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry, fp_statid));/* */
                                                                           /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));               /* */
                                                                           /* */
  return BCM_E_NONE;                                                       /* */
}                                                                          /* */
                                                                           /* */
bcm_error_t                                                                /* */
egress_port_multi_setup(int unit, bcm_pbmp_t pbm, int LoopBackType)        /* */
{                                                                          /* */
    bcm_field_qset_t  qset;                                                /* */
    bcm_field_group_t group = 9999;                                        /* */
    int                pri = group;                                        /* */
    bcm_field_entry_t entry = 99991;                                       /* */
    int         fp_statid = 9999;                                          /* */
    const bcm_field_stat_t stats[2] = { bcmFieldStatPackets,               /* */
                                        bcmFieldStatBytes };               /* */
    bcm_port_t     port;                                                   /* */
                                                                           /* */
                                                                           /* */
    BCM_PBMP_ITER(pbm, port)                                               /* */
    {                                                                      /* */
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port,              /* */
                    LoopBackType));                                        /* */
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port,               /* */
                    BCM_PORT_DISCARD_ALL));                                /* */
    }                                                                      /* */
    print pbm;                                                             /* */
    bcm_pbmp_t pbm_mask;                                                   /* */
    BCM_PBMP_CLEAR(pbm_mask);                                              /* */
    BCM_PBMP_NEGATE(pbm_mask, pbm_mask);                                   /* */
    BCM_FIELD_QSET_INIT(qset);                                             /* */
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPorts);                      /* */
                                                                           /* */
    BCM_IF_ERROR_RETURN(bcm_field_group_create_id(unit, qset, pri, group));/* */
                                                                           /* */
    BCM_IF_ERROR_RETURN(bcm_field_entry_create_id(unit, group, entry));    /* */
                                                                           /* */
    // Due to a bug SDK-144931 related to InPorts, commenting              /* */
    // out below line.                                                     /* */
    //BCM_IF_ERROR_RETURN(bcm_field_qualify_InPorts(unit, entry,           /* */
    //            pbm, pbm_mask));                                         /* */
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,                  /* */
                bcmFieldActionCopyToCpu, 1, 224));                         /* */
                                                                           /* */
    BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group, 2,           /* */
                stats, fp_statid));                                        /* */
    BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry,           /* */
                fp_statid));                                               /* */
                                                                           /* */
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));             /* */
                                                                           /* */
    return BCM_E_NONE;                                                     /* */
}                                                                          /* */
                                                                           /* */
void bbshell(int unit, char *str)                                          /* */
{                                                                          /* */
    printf("BCM.%d> %s\n", unit, str);                                     /* */
    bshell(unit, str);                                                     /* */
}                                                                          /* */
/****************   GENERIC UTILITY FUNCTIONS ENDS  ****************************
 */

int opt_ChangeVlanInVfp = 1;        // Add Change VID as action in IFP
int opt_AddMplsLookupEnable = 0;    // As IFP Action

print opt_ChangeVlanInVfp;
print opt_AddMplsLookupEnable;

int unit=0;

bcm_vlan_t test_vid = 0x0064;    // d100
bcm_vlan_t vid_new = 0x00C8;    // d200

bcm_port_t ing_port1 = PORT_ANY; // Set to a valid port if known already.
bcm_port_t ing_port2 = PORT_ANY;
bcm_port_t egr_port = PORT_ANY;

/* Populate the ports */
ing_port1 = portGetNextE(unit, ing_port1);
ing_port2 = portGetNextE(unit, ing_port2);

egr_port = portGetNextE(unit, egr_port);

print ing_port1;
print ing_port2;
print egr_port;

bcm_mac_t local_mac_1 = {0x00, 0x00, 0x00, 0x00, 0x00, 0xDD};
bcm_mac_t remote_mac_1 = {0x00, 0x00, 0x00, 0x00, 0x00, 0xEE};
uint32 rx_label = 1000;
uint32 rx_label_two = 2000;

bcm_ip_t ip_subnet=0x0A0A0A0B;    // dest IP
bcm_ip_t ip_subnet_mask = 0xffffff00;

int main_ifp_statid = 1;

/* 
 * This functions gets the port numbers and sets up ingress and 
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
    if (BCM_E_NONE != ingress_port_setup(unit, ing_port1, BCM_PORT_LOOPBACK_MAC)) {
        printf("ingress_port_setup() failed for port %d\n", ing_port1);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egr_port, BCM_PORT_LOOPBACK_MAC)) {
        printf("egress_port_setup() failed for port %d\n", egr_port);
        return -1;
    }

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port2, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ing_port2, BCM_PORT_DISCARD_ALL));

    bshell(unit, "pw start report +raw +decode");

    return BCM_E_NONE;
}

/*
    Creates the MPLS tunnel termination.
    Adds 2 entries for tunnel processing
        1. Original VID and DMAC
        2. Changed VID and original DMAC
*/
bcm_error_t mpls_action()
{
    bcm_port_t port_1 = egr_port; 
    bcm_gport_t gport_1;

    bcm_vlan_t vid_1 = test_vid;
    bcm_pbmp_t pbmp, ubmp;

    bcm_l3_intf_t l3_intf_1;
    bcm_if_t intf_id_1 = 1;
    bcm_vrf_t vrf = 0; 
    bcm_mpls_tunnel_switch_t tun_switch;

    bcm_l3_route_t l3_route;

    bcm_l3_egress_t l3_egress;
    bcm_if_t egr_obj_1;
    bcm_module_t modid_1;
    bcm_port_t modport_1;

    printf("Doing sc egress mode\n");
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));
    
    printf("Doing gport get\n");
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, port_1, &gport_1));
    
    if( 1 )
    {
        modid_1 = BCM_GPORT_MODPORT_MODID_GET(gport_1);
        modport_1 = BCM_GPORT_MODPORT_PORT_GET(gport_1);

        // l3 intf for customer port
        bcm_l3_intf_t_init(&l3_intf_1);
        l3_intf_1.l3a_flags  = BCM_L3_ADD_TO_ARL | BCM_L3_WITH_ID;
        l3_intf_1.l3a_intf_id = intf_id_1;
        sal_memcpy(l3_intf_1.l3a_mac_addr, local_mac_1, 6);
        l3_intf_1.l3a_vid = vid_1;
        l3_intf_1.l3a_vrf = vrf;
        printf("Doing l3 intf create\n");
        BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf_1));

        /* MPLS termination flow */
        bcm_l3_egress_t_init(&l3_egress);
        l3_egress.intf = intf_id_1; 
        sal_memcpy(l3_egress.mac_addr, remote_mac_1, 6);
        l3_egress.vlan = vid_1;
        l3_egress.module = modid_1;
        l3_egress.port = modport_1;
        printf("Doing l3 egress create\n");
        BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_1));

        bcm_l3_route_t_init(&l3_route);
        l3_route.l3a_subnet = ip_subnet;
        l3_route.l3a_ip_mask = ip_subnet_mask;
        l3_route.l3a_intf = egr_obj_1;
        printf("Doing l3 route add\n");
        BCM_IF_ERROR_RETURN(bcm_l3_route_add(unit, l3_route));
    }
    
    bcm_mpls_tunnel_switch_t_init(&tun_switch);
    tun_switch.flags = BCM_MPLS_SWITCH_COUNTED;//BCM_MPLS_SWITCH_COUNTED;
    //tun_switch.flags |= BCM_MPLS_SWITCH_DIRECT_ATTACHED;
    tun_switch.label = rx_label;
    
    tun_switch.port = BCM_GPORT_INVALID;
    
    if( 1 ) // POP
    {
        tun_switch.action = BCM_MPLS_SWITCH_ACTION_POP;
        tun_switch.egress_if = egr_obj_1;
    }
    else if( 0 ) // SWAP
    {
        tun_switch.action = BCM_MPLS_SWITCH_ACTION_SWAP;
        tun_switch.egress_label.label = rx_label+1; //1000;    // new label
        tun_switch.egress_if = egr_obj_1;
    }
    else // NOP
    {
        tun_switch.action = BCM_MPLS_SWITCH_ACTION_NOP;
        tun_switch.egress_port = gport_1;
        tun_switch.egress_if = egr_obj_1;
    }
    
    printf("Doing Add l2 tunnel switch add\n");
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_switch_add(unit, &tun_switch));
    
    printf("Doing Add l2 tunnel add for VID= %04X\n", vid_1);
    BCM_IF_ERROR_RETURN(bcm_l2_tunnel_add(unit, local_mac_1, vid_1));
    
    printf("Doing Add l2 tunnel add for CHANGED VID %04X\n", vid_new);
    BCM_IF_ERROR_RETURN(bcm_l2_tunnel_add(unit, local_mac_1, vid_new));
    
    return BCM_E_NONE;
}

bcm_error_t ConfigureVfpTermination()
{
    bcm_udf_pkt_format_id_t     pkt_format_id;
    
    if( 1 )
    {
        bcm_pbmp_t     vlan_pbmp;
        bcm_pbmp_t     ut_pbmp;

        BCM_PBMP_CLEAR(vlan_pbmp);
        BCM_PBMP_PORT_ADD(vlan_pbmp, ing_port1);
        BCM_PBMP_PORT_ADD(vlan_pbmp, ing_port2);
        BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port);

        BCM_PBMP_CLEAR(ut_pbmp);
        BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, test_vid));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, test_vid, vlan_pbmp, ut_pbmp));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, 1, vlan_pbmp)); // remove from default vlan
    }
    
    if( 0 )    // Not needed as BCM_L3_ADD_TO_ARL flag is used later
    {
        bcm_l2_addr_t l2_addr;

        bcm_l2_addr_t_init(&l2_addr, local_mac_1, test_vid);
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = ing_port2;
        BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));
    }
    
    printf("Doing mpls related configuration...\n");
    BCM_IF_ERROR_RETURN( mpls_action() );
    
    ////////////////////////////////////////////////////////////////////////////
    if( 1 )
    {
        bcm_field_qset_t    testQset;
        
        BCM_FIELD_QSET_INIT(testQset);
        BCM_FIELD_QSET_ADD(testQset, bcmFieldQualifyStageLookup);
        BCM_FIELD_QSET_ADD(testQset, bcmFieldQualifyInPort);

        bcm_field_group_t group = 100;
        const int   pri = group;
        /* And finally create a group based on QSET and UDF qualifiers */
        printf("Doing Group create %d\n", group);
        BCM_IF_ERROR_RETURN(bcm_field_group_create_id(unit, testQset, pri, group));
        
        bcm_field_entry_t     entry;

        printf("Doing bcm_field_entry_create!\n");
        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));
        
        
        /* Remove all qualifiers from field entry */
        printf("Doing bcm_field_qualify_clear!\n");
        BCM_IF_ERROR_RETURN(bcm_field_qualify_clear(unit, entry));
        
        printf("Doing InPort qualifier\n");
        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort
                            (unit, entry, ing_port1, 0xFFFF));
        
        if( 0 )
        {
            printf("Doing CopyToCpu action add\n");
            BCM_IF_ERROR_RETURN(bcm_field_action_add
                                (unit, entry, bcmFieldActionCopyToCpu, 1, 555));
        }
        
        if( opt_ChangeVlanInVfp )
        {
            printf("Doing VlanNew action add\n");
            BCM_IF_ERROR_RETURN(bcm_field_action_add
                                (unit, entry, bcmFieldActionVlanNew, vid_new, 0xFFFF));
        }
        
        if( 1 )
        {
            printf("Doing TerminationAllowed action add\n");
            BCM_IF_ERROR_RETURN(bcm_field_action_add
                                (unit, entry, bcmFieldActionTerminationAllowed,
                                BCM_FIELD_MPLS_TERM_ALLOWED | BCM_FIELD_IPV4_TERM_ALLOWED, 0xFFFFFFFF));
            if( opt_AddMplsLookupEnable )    // Is it required?
            {
                printf("Doing MplsLookupEnable action add\n");
                BCM_IF_ERROR_RETURN(bcm_field_action_add
                                    (unit, entry, bcmFieldActionMplsLookupEnable, BCM_FIELD_MPLS_ENABLE,
                                    0xFFFFFFFF));
            }
        }
        
        const bcm_field_stat_t stats[2] = { bcmFieldStatPackets, bcmFieldStatBytes };
        /* Also set up an FP stat counter to count matches */
        BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group, 2, stats, main_ifp_statid));
        /* Attach the stat object to the entry */
        BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry, main_ifp_statid));

        printf("Doing Entry create %d\n", entry);
        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    }
    
    return BCM_E_NONE;
}


/*
 How to verify: It is documented within the function for various packets and options
*/
// SDK-144976
bcm_error_t verify()
{
    char cmd[2048];
    
    if( 1 )
    {
        // MACDA: 0xDD, VID: 100, MPLS ONE label: 1000(h3E8), 7(h7), 39(h27)
        sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X0000000000CC8100%04X8847%05XF274500003A0000000040FF3D8714141416%08X000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2021222324258569A726",
        ing_port1,
        local_mac_1[0], local_mac_1[1], local_mac_1[2],
        local_mac_1[3], local_mac_1[4], local_mac_1[5],
        test_vid, rx_label, ip_subnet);
        printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 3);
        printf("%s\n", cmd); bshell(unit, cmd);
        
        sprintf(cmd, "fp stat get si=%d t=p", main_ifp_statid);
        printf("%s\n", cmd); bshell(unit, cmd);
    }
    
    if( 1 )
    {
        // MACDA: 0xDD, VID: 100, MPLS TWO label: 1000(h3E8), 7(h7), 33(h21)  |  2000(h7D0), 6(h6), 39(h27)
        sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X0000000000CC8100%04X8847%05XE27%05XD27450000360000000040FF3D8B14141416%08X000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2021222324258569A726",
        ing_port1,
        local_mac_1[0], local_mac_1[1], local_mac_1[2],
        local_mac_1[3], local_mac_1[4], local_mac_1[5],
        test_vid, rx_label, rx_label_two, ip_subnet);
        printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 3);
        printf("%s\n", cmd); bshell(unit, cmd);
        
        sprintf(cmd, "fp stat get si=%d t=p", main_ifp_statid);
        printf("%s\n", cmd); bshell(unit, cmd);
    }
    
    printf("\n\n\t Final STAT count should be %d", 2);
    printf("\n\n\t Also Final Egress packet should be a normal IP packet (not mpls)\n\n");
    
    return BCM_E_NONE;
}

bcm_error_t execute()
{
    if( (rrv = test_setup(unit)) != BCM_E_NONE )
    {
        printf("Creating the test setup failed %d\n", rrv);
        return rrv;
    }
    
    if( (rrv = ConfigureVfpTermination()) != BCM_E_NONE )
    {
        printf("Configuring VFP test failed with %d\n", rrv);
        return rrv;
    }
    
    if( (rrv = verify()) != BCM_E_NONE )
    {
        printf("Verify mirror failed with %d\n", rrv);
        return rrv;
    }
    
    return BCM_E_NONE;
}


int rrv;

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

