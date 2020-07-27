/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : Load Balancing
 *
 *  Usage : BCM.0> cint LoadBalancing_DLB_HECMP_DGM.c YES [resilient=0/1] [dlb=0/1] [dgm-hecmp=0/1]
 *                                                                          [port speed=10/100/400]
 * E.g.  cint LoadBalancing_DLB_HECMP_DGM.c YES 0 1 0     <<< Runs only DLB
 * E.g.  cint LoadBalancing_DLB_HECMP_DGM.c YES 1 1 1     <<< Runs all three for port speed = 10G
 * E.g.  cint LoadBalancing_DLB_HECMP_DGM.c YES 1 1 1 10  <<< -do- for port speed =  10G
 * E.g.  cint LoadBalancing_DLB_HECMP_DGM.c YES 1 1 1 100 <<< -do- for port speed = 100G
 *
 *  config   : lb_config.bcm
 *              l3_ecmp_levels=2        <<<< For HECMP
 *
 *  Log file : LoadBalancing_DLB_HECMP_DGM_log.txt
 *              Logs pasted for both the options RH + DLB + DGM
 *
 *  Test Topology :
 *
 *                   +------------------------------+
 *                   |                              |
 *                   |                              |
 *                   |                              +----------------+
 *                   |                              |
 *  +----------------+          SWITCH              +----------------+
 *                   |                              |
 *                   |                              +----------------+
 *                   |                              |
 *                   |                              +----------------+
 *                   |                              |
 *                   +------------------------------+
 *
 *
 * Summary:
 *  ========
 *   This CINT shows how to:
 *   1. Create a ecmp group with a number of interface members (for 3 type of load balancing)
 *      a) Resilient Hashing based load balancing
 *      b) Dynamic Load Balancing (DLB)
 *      c) Dynamic Group Multipathing (DGM) for Hierarchical ECMP (H-ECMP)
 *   2. Associate Aggregate Group Monitor (AGM) for each ecmp.
 *   3. Send the traffic to egress from this ecmp group
 *   4. Dump the AGM Stats
 *
 *  Detailed steps done in the CINT script:
 *  ====================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ================================
 *       a) Select set of ingress and egress ports and configure them in
 *          Loopback mode.
 *
 *    2) Step2 - Configuration (Done in do_all_load_balancing_config())
 *    ===================================================
 *      A] RH Configuration (Done in do_all_load_balancing_config()->rh_ecmp_setup() ).
 *      ------------------
 *      The RH Application configuration comes from the structure variable rh_ecmp_info.
 *      It defines the dest mac, Vlan and egress port for the egress objects which will
 *      be part of the ecmp group
 *       a) Initialize hashing infrastructure with RTAG7 as common init do_common_init()
 *       b) Create L3 egress objects with associated interface.
 *       c) Set the ECMP load balancing type as DYNAMIC_MODE_RESILIENT.
 *       d) Create ECMP group by adding the next hops as members
 *       e) Associate AGM
 *
 *      B] DLB Configuration (Done in do_all_load_balancing_config()->dlb_ecmp_setup() ).
 *      ------------------
 *      The DLB Application configuration comes from the structure variable dlb_ecmp_info.
 *      It defines the dest mac, Vlan and egress port for the egress objects which will
 *      be part of the ecmp group
 *       a) Create L3 egress objects with associated interface.
 *       b) Set the ECMP load balancing type as DYNAMIC_MODE_NORMAL.
 *       c) Create ECMP group by adding the next hops as members
 *       d) Associate AGM
 *
 *      C] DGM Configuration (Done in do_all_load_balancing_config()->dgm_hecmp_setup() ).
 *      ------------------
 *      The DLB Application configuration comes from the structure variable dgm_hecmp_info.
 *      It defines the dest mac, Vlan and egress port for the egress objects which will
 *      be part of the ecmp group
 *      It also defines what type of tunnel path this next hop will be. Fo that, it specifies the
 *      DIP, SIP and tunnel type of MAIN or Alternate paths. This application defines DGM on a
 *      hierarchical ECMP configuration.
 *       a) Create L3 egress objects with associated interface.
 *       b) Set the ECMP load balancing type as DYNAMIC_MODE_DGM.
 *       c) Create 2 ECMP groups by adding the next hops as members for UNDERLAY
 *          i) For Group 1 : 2 paths in MAIN and 2 paths in ALTernate
 *         ii) For Group 2 : 3 paths in MAIN and 2 paths in ALTernate
 *       d) Create 1 ECMP by adding above 2 ECMP groups for OVERLAY
 *       e) Associate AGM
 *
 *    4) Step3 - Verification (Done in verify())
 *    ==============================
 *       a) Send unicast packets from ing_portX with varying parameter of
 *          srcmac. Fixed Dest IP and fixed Dest mac. Vlan = 10.
 *       b) See the load balancing happening. and AGM stats updated.
 *       c) Expected Result:
 *          ================
 *          Dump of agm_stat will show the Monitor stats. Also, it will be reflected from the
 *          dump of "show c" command.
 *
 * NOTE: Use opt_* variables to change the test variant
 */

cint_reset();

/****************   GENERIC UTILITY FUNCTIONS STARTS  **************************
 */
// Gets the next valid port after the last valid returned port.            /* */
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
  if( PreferredPort != PORT_ANY )                                          /* */
  {                                                                        /* */
    printf("Using preferred port %d\n", PreferredPort);                    /* */
    return PreferredPort;                                                  /* */
  }                                                                        /* */
  rv = bcm_port_config_get(unit, &configP);                                /* */
  if(BCM_FAILURE(rv)) {                                                    /* */
    printf("\nError in retrieving port configuration: %s %d.\n",           /* */
    bcm_errmsg(rv), __last_returned_port);                                 /* */
    exit;                                                                  /* */
  }                                                                        /* */
  ports_pbmp = configP.e;    // configP.hg;                                /* */
  for (i= __last_returned_port; i < BCM_PBMP_PORT_MAX; i++)                /* */
  {                                                                        /* */
    if ( BCM_PBMP_MEMBER(&ports_pbmp, i) )                                 /* */
    {                                                                      /* */
        __last_returned_port = i+1; // point to a probable next port       /* */
        return i;                                                          /* */
    }                                                                      /* */
  }                                                                        /* */
  printf("ERROR: Not enough ports %d\n", __last_returned_port);            /* */
  exit;                                                                    /* */
}                                                                          /* */
//                                                                         /* */
// Configures the port in loopback mode and installs                       /* */
// an IFP rule. This IFP rule copies the packets ingressing                /* */
// on the specified port to CPU.                                           /* */
//                                                                         /* */
bcm_error_t                                                                /* */
ingress_port_setup(int unit, bcm_port_t port, int LoopBackType)            /* */
{                                                                          /* */
  bcm_field_group_config_t group_config;                                   /* */
  bcm_field_aset_t    actn;                                                /* */
  bcm_field_group_t group = 9998;                                          /* */
  int                pri = group;                                          /* */
  bcm_field_entry_t entry = 99981;                                         /* */
  int         fp_statid = 9998;                                            /* */
  const bcm_field_stat_t stats[2] = { bcmFieldStatPackets,                 /* */
                                      bcmFieldStatBytes };                 /* */
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, LoopBackType));    /* */
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port,                     /* */
                            BCM_PORT_DISCARD_NONE));                       /* */
  bcm_field_group_config_t_init(&group_config);                            /* */
  BCM_FIELD_ASET_INIT(actn);                                               /* */
  BCM_FIELD_ASET_ADD(actn, bcmFieldActionCopyToCpu);                       /* */
  BCM_FIELD_ASET_ADD(actn, bcmFieldActionStatGroup);                       /* */
  group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_ID;                    /* */
  group_config.aset= actn;                                                 /* */
  group_config.mode = bcmFieldGroupModeAuto;                               /* */
  group_config.group = group;                                              /* */
  group_config.priority = pri;                                             /* */
  BCM_FIELD_QSET_INIT(group_config.qset);                                  /* */
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);            /* */
  BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit,                  /* */
                                                &group_config));           /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_create_id(unit, group, entry));      /* */
  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port,          /* */
            BCM_FIELD_EXACT_MATCH_MASK));                                  /* */
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,                    /* */
            bcmFieldActionCopyToCpu, 1, 222));                             /* */
  BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group, 2,             /* */
            stats, fp_statid));                                            /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry,             /* */
            fp_statid));                                                   /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));               /* */
  return BCM_E_NONE;                                                       /* */
}                                                                          /* */
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
  bcm_field_group_t group = 9999;                                          /* */
  bcm_field_group_config_t group_config;                                   /* */
  bcm_field_aset_t    actn;                                                /* */
  int                pri = group;                                          /* */
  bcm_field_entry_t entry = 99991;                                         /* */
  int         fp_statid = 9999;                                            /* */
  const bcm_field_stat_t stats[2] = { bcmFieldStatPackets,                 /* */
                                      bcmFieldStatBytes };                 /* */
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, LoopBackType));    /* */
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port,                     /* */
            BCM_PORT_DISCARD_ALL));                                        /* */
  bcm_field_group_config_t_init(&group_config);                            /* */
  BCM_FIELD_ASET_INIT(actn);                                               /* */
  BCM_FIELD_ASET_ADD(actn, bcmFieldActionCopyToCpu);                       /* */
  BCM_FIELD_ASET_ADD(actn, bcmFieldActionStatGroup);                       /* */
  group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_ID;                    /* */
  group_config.aset= actn;                                                 /* */
  group_config.mode = bcmFieldGroupModeAuto;                               /* */
  group_config.group = group;                                              /* */
  group_config.priority = pri;                                             /* */
  BCM_FIELD_QSET_INIT(group_config.qset);                                  /* */
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);            /* */
  BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit,                  /* */
                                                &group_config));           /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_create_id(unit, group, entry));      /* */
  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port,          /* */
            BCM_FIELD_EXACT_MATCH_MASK));                                  /* */
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,                    /* */
            bcmFieldActionCopyToCpu, 1, 223));                             /* */
  BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group, 2,             /* */
            stats, fp_statid));                                            /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry, fp_statid));/* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));               /* */
  return BCM_E_NONE;                                                       /* */
}                                                                          /* */
bcm_error_t                                                                /* */
egress_port_multi_setup(int unit, bcm_pbmp_t pbm, int LoopBackType)        /* */
{                                                                          /* */
  bcm_field_group_t group = 10000;                                         /* */
  bcm_field_group_config_t group_config;                                   /* */
  bcm_field_aset_t    actn;                                                /* */
  int                pri = group;                                          /* */
  bcm_field_entry_t entry = 100001;                                        /* */
  int         fp_statid = 10000;                                           /* */
  const bcm_field_stat_t stats[2] = { bcmFieldStatPackets,                 /* */
                                      bcmFieldStatBytes };                 /* */
  bcm_port_t     port;                                                     /* */
  BCM_PBMP_ITER(pbm, port)                                                 /* */
  {                                                                        /* */
      BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port,                /* */
                  LoopBackType));                                          /* */
      BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port,                 /* */
                  BCM_PORT_DISCARD_ALL));                                  /* */
  }                                                                        /* */
  print pbm;                                                               /* */
  bcm_pbmp_t pbm_mask;                                                     /* */
  BCM_PBMP_CLEAR(pbm_mask);                                                /* */
  BCM_PBMP_NEGATE(pbm_mask, pbm_mask);                                     /* */
  bcm_field_group_config_t_init(&group_config);                            /* */
  BCM_FIELD_ASET_INIT(actn);                                               /* */
  BCM_FIELD_ASET_ADD(actn, bcmFieldActionCopyToCpu);                       /* */
  BCM_FIELD_ASET_ADD(actn, bcmFieldActionStatGroup);                       /* */
  group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_ID;                    /* */
  group_config.aset= actn;                                                 /* */
  group_config.mode = bcmFieldGroupModeAuto;                               /* */
  group_config.group = group;                                              /* */
  group_config.priority = pri;                                             /* */
  BCM_FIELD_QSET_INIT(group_config.qset);                                  /* */
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPorts);           /* */
  BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit,                  /* */
                                                    &group_config));       /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_create_id(unit, group, entry));      /* */
  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPorts(unit, entry,               /* */
              pbm, pbm_mask));                                             /* */
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,                    /* */
              bcmFieldActionCopyToCpu, 1, 224));                           /* */
  BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group, 2,             /* */
              stats, fp_statid));                                          /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry,             /* */
              fp_statid));                                                 /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));               /* */
  return BCM_E_NONE;                                                       /* */
}                                                                          /* */
void bbshell(int unit, char *str)                                          /* */
{                                                                          /* */
  printf("B_CM.%d> %s\n", unit, str);                                      /* */
  bshell(unit, str);                                                       /* */
}                                                                          /* */
/****************   GENERIC UTILITY FUNCTIONS ENDS  ****************************
 */


/*

VID: 10


rh_ecmp
RH on ECMP
                            +-------+
                            |       | port1_rh (DM:0x2A, SM:0x11, DIP:10..1, SIP:11..1)
                            |       +----------------->
                            |       |
                            |       | port2_rh (DM:0x2B, SM:0x11, DIP:10..1, SIP:11..1)
                            |       +----------------->
ing_port1 (DM:0x22, SM:0xAA)|       |
      (DIP:16..1, SIP:17..1)|       |
 +-------------------------->       |
                            |       | port3_rh (DM:0x2C, SM:0x11, DIP:10..1, SIP:11..1)
                            |       +----------------->
                            |       |
                            |       | port4_rh (DM:0x3D, SM:0x11, DIP:10..1, SIP:11..1)
                            |       +----------------->
                            |       |
                            +-------+

dlb_ecmp
DLB on ECMP
                            +-------+
                            |       | port1_dlb (DM:0x3A, SM:0x11, DIP:20..1, SIP:21..1)
                            |       +----------------->
                            |       |
                            |       | port2_dlb (DM:0x3B, SM:0x11, DIP:20..1, SIP:0x11)
                            |       +----------------->
ing_port1 (DM:0x33, SM:0xAA)|       |
      (DIP:32..1, SIP:33..1)|       |
 +-------------------------->       |
                            |       | port3_dlb (DM:0x3C, SM:0x11, DIP:20..1, SIP:0x11)
                            |       +----------------->
                            |       |
                            |       | port4_dlb (DM:0x3D, SM:0x11, DIP:20..1, SIP:0x11)
                            |       +----------------->
                            |       |
                            +-------+


dgm_hecmp
DGM on H-ECMP
                            +-------+
                            |       | port1_hecmp_ul1 (DM:0x46, SM:0x11, DIP:30..1, SIP:31..1)
                            |       +----------------->
                            |       | port2_hecmp_ul1 (DM:0x47, SM:0x11, DIP:30..1, SIP:31..1)
                            |       +----------------->
ing_port1 (DM:0x44, SM:0xAA)|       |
      (DIP:80..1, SIP:81..1)|       |
 +-------------------------->       |
                            |       | port1_hecmp_ul2 (DM:0x48, SM:0x11, DIP:40..1, SIP:31..1)
                            |       +----------------->
                            |       | port2_hecmp_ul2 (DM:0x49, SM:0x11, DIP:40..1, SIP:31..1)
                            |       +----------------->
                            |       | port3_hecmp_ul2 (DM:0x4A, SM:0x11, DIP:40..1, SIP:31..1)
                            |       +----------------->
                            |       |
         --  ALT  ----------+-------+-----------  ALT  --
                            |       |
                            |       | port1_hecmp_ul1_alt (DM:0x4B, SM:0x11, DIP:30..1, SIP:31..1)
                            |       +----------------->
                            |       | port2_hecmp_ul1_alt (DM:0x4C, SM:0x11, DIP:30..1, SIP:31..1)
                            |       +----------------->
                            |       |
                            |       | port1_hecmp_ul2_alt (DM:0x4D, SM:0x11, DIP:40..1, SIP:31..1)
                            |       +----------------->
                            |       | port2_hecmp_ul2_alt (DM:0x4E, SM:0x11, DIP:40..1, SIP:31..1)
                            |       +----------------->
                            |       |
                            +-------+

*/

uint32 mine_dtoi(char *str)
{
    int i;
    uint32 dec = 0;
    
    for(i=0; str[i]!=0; i++){
        dec = dec * 10 + ( str[i] - '0' );
    }
    return dec;
}

int opt_TestTypeRH = 1;
int opt_TestTypeDLB = 1;
int opt_TestTypeDGM_HECMP = 1;

// 10: for 10G
// 100: for 100G
int opt_PortSpeedFactor = 10; // used for dynamic_scaling_factor

int opt_DevIsHelix5 = 1;
int opt_DevIsTomahawk3 = 0;

print ARGC;

if( ARGC >= 2 ) {
    if( *ARGV[1] == '1' ) {
        opt_TestTypeRH = 1;
    } else {
        opt_TestTypeRH = 0;
    }
}

if( ARGC >= 3 ) {
    if( *ARGV[2] == '1' ) {
        opt_TestTypeDLB = 1;
    } else {
        opt_TestTypeDLB = 0;
    }
}

if( ARGC >= 4 ) {
    if( *ARGV[3] == '1' ) {
        opt_TestTypeDGM_HECMP = 1;
    } else {
        opt_TestTypeDGM_HECMP = 0;
    }
}

if( ARGC >= 5 ) {
    opt_PortSpeedFactor = mine_dtoi(*ARGV[4]);
}


/* Resilient Hashing and DLB are mutually exclusive for TD3/Helix5 family of devices */
if( opt_TestTypeRH )
{
    printf("Resilient Hashing and DLB are mutually exclusive for TD3/Helix5 family of devices\n");
    opt_TestTypeDLB = 0;
    opt_TestTypeDGM_HECMP = 0;
}

print opt_TestTypeRH;
print opt_TestTypeDLB;
print opt_TestTypeDGM_HECMP;
print opt_PortSpeedFactor;

int unit = 0;

const int LB_MEMBER_MAX = 10;

bcm_vlan_t test_vid = 10;

bcm_port_t ing_port1 = PORT_ANY;    // Port for injecting packet from CPU
bcm_port_t ing_port2 = PORT_ANY;    // Unused port

bcm_port_t egr_port1_rh = PORT_ANY;     // Ports for Resilient Hashing
bcm_port_t egr_port2_rh = PORT_ANY;
bcm_port_t egr_port3_rh = PORT_ANY;
bcm_port_t egr_port4_rh = PORT_ANY;

bcm_port_t egr_port1_dlb = PORT_ANY;    // Ports for DLB Dynamic Load Balancing
bcm_port_t egr_port2_dlb = PORT_ANY;
bcm_port_t egr_port3_dlb = PORT_ANY;
bcm_port_t egr_port4_dlb = PORT_ANY;

bcm_port_t egr_port1_hecmp_ul1 = PORT_ANY;   // Ports for H-ECMP for Tunnel#1 network for DGM
bcm_port_t egr_port2_hecmp_ul1 = PORT_ANY;
bcm_port_t egr_port1_hecmp_ul2 = PORT_ANY;   // Ports for H-ECMP for Tunnel#2 network
bcm_port_t egr_port2_hecmp_ul2 = PORT_ANY;
bcm_port_t egr_port3_hecmp_ul2 = PORT_ANY;

bcm_port_t egr_port1_hecmp_ul1_alt = PORT_ANY;   // Ports for H-ECMP for Tunnel#1 network for DGM
bcm_port_t egr_port2_hecmp_ul1_alt = PORT_ANY;
bcm_port_t egr_port1_hecmp_ul2_alt = PORT_ANY;   // Ports for H-ECMP for Tunnel#2 network
bcm_port_t egr_port2_hecmp_ul2_alt = PORT_ANY;


/* Populate the ports */

ing_port1 = portGetNextE(unit, ing_port1);
ing_port2 = portGetNextE(unit, ing_port2);
egr_port1_rh = portGetNextE(unit, egr_port1_rh);
egr_port2_rh = portGetNextE(unit, egr_port2_rh);
egr_port3_rh = portGetNextE(unit, egr_port3_rh);
egr_port4_rh = portGetNextE(unit, egr_port4_rh);
egr_port1_dlb = portGetNextE(unit, egr_port1_dlb);
egr_port2_dlb = portGetNextE(unit, egr_port2_dlb);
egr_port3_dlb = portGetNextE(unit, egr_port3_dlb);
egr_port4_dlb = portGetNextE(unit, egr_port4_dlb);
egr_port1_hecmp_ul1 = portGetNextE(unit, egr_port1_hecmp_ul1);
egr_port2_hecmp_ul1 = portGetNextE(unit, egr_port2_hecmp_ul1);
egr_port1_hecmp_ul2 = portGetNextE(unit, egr_port1_hecmp_ul2);
egr_port2_hecmp_ul2 = portGetNextE(unit, egr_port2_hecmp_ul2);
egr_port3_hecmp_ul2 = portGetNextE(unit, egr_port3_hecmp_ul2);
egr_port1_hecmp_ul1_alt = portGetNextE(unit, egr_port1_hecmp_ul1_alt);
egr_port2_hecmp_ul1_alt = portGetNextE(unit, egr_port2_hecmp_ul1_alt);
egr_port1_hecmp_ul2_alt = portGetNextE(unit, egr_port1_hecmp_ul2_alt);
egr_port2_hecmp_ul2_alt = portGetNextE(unit, egr_port2_hecmp_ul2_alt);

print ing_port1;
print ing_port2;
print egr_port1_rh;
print egr_port2_rh;
print egr_port3_rh;
print egr_port4_rh;
print egr_port1_dlb;
print egr_port2_dlb;
print egr_port3_dlb;
print egr_port4_dlb;
print egr_port1_hecmp_ul1;
print egr_port2_hecmp_ul1;
print egr_port1_hecmp_ul2;
print egr_port2_hecmp_ul2;
print egr_port3_hecmp_ul2;
print egr_port1_hecmp_ul1_alt;
print egr_port2_hecmp_ul1_alt;
print egr_port1_hecmp_ul2_alt;
print egr_port2_hecmp_ul2_alt;

struct next_hop_t {
    bcm_mac_t   dst_mac;
    bcm_vlan_t  vid;
    bcm_port_t  dst_port;
};

const int LAST_ENTRY = -1;

bcm_mac_t test_smac = {0x00, 0x00, 0x00, 0x00, 0x00, 0xAA};

////////////////  Resilient Hash   /////////////////////////////////
/* RH configuration definition */
next_hop_t rh_ecmp_info[] = 
    {
        {
            {0x00, 0x00, 0x00, 0x00, 0x00, 0x2A},   // dst_mac
            test_vid,                               // vid
            egr_port1_rh                            // dst_port     <<<<
        },
        {
            {0x00, 0x00, 0x00, 0x00, 0x00, 0x2B},
            test_vid,
            egr_port2_rh
        },
        {
            {0x00, 0x00, 0x00, 0x00, 0x00, 0x2C},
            test_vid,
            egr_port3_rh
        },
        {
            {0x00, 0x00, 0x00, 0x00, 0x00, 0x2D},
            test_vid,
            egr_port4_rh
        },
        {
            {0x00, 0x00, 0x00, 0x00, 0x00, 0xFF},
            test_vid,
            LAST_ENTRY,
        }
    };

bcm_mac_t rh_ecmp_ing_intf_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x22};
bcm_ip_t rh_ecmp_dip = 0x10000001;  /* 16.0.0.1 FIXED */
bcm_ip_t rh_ecmp_sip = 0x11000001;  /* 17.0.0.1 FIXED */
bcm_ip_t rh_ecmp_ipmask = 0xFFFFFFFF;  /* 255.255.255.255 */
bcm_switch_agm_info_t  rh_ecmp_agm;

///////////////    DLB    ///////////////////////////
/* DLB configuration definition */
next_hop_t dlb_ecmp_info[] = 
    {
        {
            {0x00, 0x00, 0x00, 0x00, 0x00, 0x3A},   // dst_mac
            test_vid,                               // vid
            egr_port1_dlb                           // dst_port <<<<
        },
        {
            {0x00, 0x00, 0x00, 0x00, 0x00, 0x3B},
            test_vid,
            egr_port2_dlb
        },
        {
            {0x00, 0x00, 0x00, 0x00, 0x00, 0x3C},
            test_vid,
            egr_port3_dlb
        },
        {
            {0x00, 0x00, 0x00, 0x00, 0x00, 0x3D},
            test_vid,
            egr_port4_dlb
        },
        {
            {0x00, 0x00, 0x00, 0x00, 0x00, 0xFF},
            test_vid,
            LAST_ENTRY,
        }
    };

bcm_mac_t dlb_ecmp_ing_intf_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33};
bcm_ip_t dlb_ecmp_dip = 0x20000001;  /* 16.0.0.1 */
bcm_ip_t dlb_ecmp_sip = 0x21000001;  /* 17.0.0.1 */
bcm_ip_t dlb_ecmp_ipmask = 0xFFFFFFFF;  /* 255.255.255.255 */
bcm_switch_agm_info_t  dlb_ecmp_agm;

/////////////////   DGM   //////////////////////////////
// Bit#8 =1 : ALT path; =0 : Main path
// Bit(3,2,1,0): Tunnel Number
const uint32 TUN_TYPE_UL1_MAIN = 0x01;    // Tunnel #1    DIP:30..1(0x1E000001), SIP:31..1
const uint32 TUN_TYPE_UL1_ALT  = 0x81;    // Tunnel #1    DIP:30..1(0x28000001), SIP:31..1
const uint32 TUN_TYPE_UL2_MAIN = 0x02;    // Tunnel #2    DIP:40..1, SIP:31..1
const uint32 TUN_TYPE_UL2_ALT  = 0x82;    // Tunnel #2    DIP:40..1, SIP:31..1

uint32 IS_NHOP_ALT_GETFLAG(uint32 val)
{
    if(val & 0x80)
    {
        return BCM_L3_ECMP_MEMBER_DGM_ALTERNATE;
    }
    else
    {
        return 0;
    }
}

uint32 GET_TUNNEL_NUM(uint32 val)
{
    return (val & 0x0F);
}

struct hecmp_tunnel_t {
    uint32      tun_type;   // TUN_TYPE_XXXX. E.g.: TUN_TYPE_UL1_MAIN
    bcm_ip_t    tun_dip;
    bcm_ip_t    tun_sip;
    next_hop_t  nhop;
};

/* DGM HECMP configuration definition */
hecmp_tunnel_t dgm_hecmp_info[] = 
    {
        //////// Tunnel # 1  /////////////
        {
            TUN_TYPE_UL1_MAIN,          // tun_type
            0x1E000001,  // 30.0.0.1    // tun_dip
            0x1F000001,  // 31.0.0.1    // tun_sip
            {
                {0x00, 0x00, 0x00, 0x00, 0x00, 0x46},   // nhop.dst_mac
                test_vid,                               // nhop.vid
                egr_port1_hecmp_ul1                     // nhop.dst_port
            }
        },
        
        {
            TUN_TYPE_UL1_MAIN,
            0x1E000001,
            0x1F000001,
            {
                {0x00, 0x00, 0x00, 0x00, 0x00, 0x47},
                test_vid,
                egr_port2_hecmp_ul1
            }
        },
        
        {
            TUN_TYPE_UL1_ALT,
            0x1E000001,
            0x1F000001,
            {
                {0x00, 0x00, 0x00, 0x00, 0x00, 0x4B},
                test_vid,
                egr_port1_hecmp_ul1_alt
            }
        },
        
        {
            TUN_TYPE_UL1_ALT,
            0x1E000001,
            0x1F000001,
            {
                {0x00, 0x00, 0x00, 0x00, 0x00, 0x4C},
                test_vid,
                egr_port2_hecmp_ul1_alt
            }
        },
        ///////////////   TUNNEL # 2  ////////////////////
        {
            TUN_TYPE_UL2_MAIN,
            0x28000001, // 40.0.0.1
            0x1F000001, // 31.0.0.1
            {
                {0x00, 0x00, 0x00, 0x00, 0x00, 0x48},
                test_vid,
                egr_port1_hecmp_ul2
            }
        },
        
        {
            TUN_TYPE_UL2_MAIN,
            0x28000001,
            0x1F000001,
            {
                {0x00, 0x00, 0x00, 0x00, 0x00, 0x49},
                test_vid,
                egr_port2_hecmp_ul2
            }
        },
        
        {
            TUN_TYPE_UL2_MAIN,
            0x28000001,
            0x1F000001,
            {
                {0x00, 0x00, 0x00, 0x00, 0x00, 0x4A},
                test_vid,
                egr_port3_hecmp_ul2
            }
        },
        
        {
            TUN_TYPE_UL2_ALT,           // tun_type
            0x28000001,                 // tun_dip
            0x1F000001,                 // tun_sip
            {
                {0x00, 0x00, 0x00, 0x00, 0x00, 0x4D},   // nhop.dst_mac
                test_vid,                               // nhop.vid
                egr_port1_hecmp_ul2_alt                 // nhop.dst_port
            }
        },
        
        {
            TUN_TYPE_UL2_ALT,
            0x28000001,
            0x1F000001,
            {
                {0x00, 0x00, 0x00, 0x00, 0x00, 0x4E},
                test_vid,
                egr_port2_hecmp_ul2_alt
            }
        },
        {
            0xFF,
            0xFF,
            0xFF,
            {
                {0x00, 0x00, 0x00, 0x00, 0x00, 0xFF}, /*DONTCARE*/
                0xFF,
                LAST_ENTRY
            }
        },
    };

bcm_mac_t dgm_hecmp_ing_intf_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x44};

bcm_switch_agm_info_t  dgm_hecmp_agm_ol;         // OverLay Network. NOT USED. Only for demo.
bcm_switch_agm_info_t  dgm_hecmp_agm_ul[2];      // Underlay Tunnel #1 and #2

bcm_ip_t dgm_hecmp_dip = 0x50000001;  /* 80.0.0.1 */
bcm_ip_t dgm_hecmp_sip = 0x51000001;  /* 81.0.0.1 */
bcm_ip_t dgm_hecmp_ipmask = 0xFFFFFFFF;  /* 255.255.255.0 */

/////////////   A G M STATs   ////////////////////////////////

bcm_error_t
    agm_ecmp_stats_create(int unit, bcm_if_t l3_ecmp_id, int member_count,
                                                bcm_switch_agm_info_t *p_agm)
{
    uint32 options = 0;
    
    bcm_switch_agm_info_t_init(p_agm);
    p_agm->period_interval = bcmSwitchAgmInterval1Second; /* 1s interval */
    p_agm->period_num = 0;   /* collection period is 7, 0 means continue until SW disable it */
    p_agm->agm_type   = bcmSwitchAgmTypeL3Ecmp;
    p_agm->num_members = member_count;

    /* Create the agm counter */
    printf("Doing agm create...\n");
    BCM_IF_ERROR_RETURN(bcm_switch_agm_create(unit, options, p_agm));

    printf("Doing agm attach...\n");
    /* Attach it to the given ECMP Group */
    BCM_IF_ERROR_RETURN(bcm_l3_ecmp_agm_attach(unit, l3_ecmp_id, p_agm->agm_id));
    
    return BCM_E_NONE;
}

bcm_error_t
    agm_ecmp_stats_start(int unit, bcm_switch_agm_info_t *p_agm)
{
    /* Set the time stamp and start AGM counter collection */
    printf("Doing agm restart...\n");
    BCM_IF_ERROR_RETURN(bcm_switch_agm_enable_set(unit, p_agm->agm_id, 0));
    BCM_IF_ERROR_RETURN(bcm_switch_agm_enable_set(unit, p_agm->agm_id, 1));
    
    return BCM_E_NONE;
}

// Caller must consume stat immediately...
bcm_error_t
    agm_ecmp_stats_print(int unit, bcm_switch_agm_info_t *p_agm, int DoClear)
{
    bcm_switch_agm_stat_t agm_stat[LB_MEMBER_MAX];
    int i;
    
    /* Get the count stats */
    //printf("Doing agm stat get...\n");
    BCM_IF_ERROR_RETURN(bcm_switch_agm_stat_get(unit, p_agm->agm_id, p_agm->num_members,
                        &agm_stat));
    for(i=0; i<p_agm->num_members; ++i)
    {
        print agm_stat[i];
    }
    
    if( DoClear )
    {
        /* Clear the counter, reset AGM stat */
        bcm_switch_agm_stat_clear(unit, p_agm->agm_id);
    }
    
    //printf("Doing agm disable...\n");
    BCM_IF_ERROR_RETURN(bcm_switch_agm_enable_set(unit, p_agm->agm_id, 0));

    return BCM_E_NONE;
}


bcm_error_t
    ecmp_rtag7_config(int unit)
{
    int flags;
    
    /* Use port based hash selection (RTAG7_HASH_SEL->USE_FLOW_SEL_ECMP) */
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelEcmp, 0) );

    flags = BCM_HASH_FIELD_SRCMOD | BCM_HASH_FIELD_SRCPORT |
            BCM_HASH_FIELD_SRCL4 | BCM_HASH_FIELD_DSTL4 |
            BCM_HASH_FIELD_IP4DST_LO | BCM_HASH_FIELD_IP4DST_HI |
            BCM_HASH_FIELD_IP4SRC_LO | BCM_HASH_FIELD_IP4SRC_HI;
  /* Block A - L3 packet field selection(RTAG7_HASH_FIELD_BMAP_1r.IPV4_FIELD_BITMAP_Af) */
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchHashIP4Field0, flags) );
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField0, flags) );

    /* Block B - L3 packet field selection(RTAG7_HASH_FIELD_BMAP_1r.IPV4_FIELD_BITMAP_Bf) */
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchHashIP4Field1, flags) );
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField1, flags) );
    
    /* Configure A0 and B0 HASH Selection */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField0Config,
                                               BCM_HASH_FIELD_CONFIG_CRC16XOR8));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField1Config,
                                               BCM_HASH_FIELD_CONFIG_CRC16));
    /* Configure Seed */
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchHashSeed0, 0x33333333) );
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchHashSeed1, 0x55555555) );
    
    BCM_IF_ERROR_RETURN (bcm_switch_control_set(unit, bcmSwitchECMPLevel1RandomSeed, 0x1234));
    BCM_IF_ERROR_RETURN (bcm_switch_control_set(unit, bcmSwitchECMPLevel2RandomSeed, 0xF021));
    BCM_IF_ERROR_RETURN
      (bcm_port_control_set(unit, ing_port1, bcmPortControlECMPLevel1LoadBalancingRandomizer, 8));
    BCM_IF_ERROR_RETURN
      (bcm_port_control_set(unit, ing_port1, bcmPortControlECMPLevel2LoadBalancingRandomizer, 5));

    /* Enable preprocess (RTAG7_HASH_CONTROL_3r.HASH_PRE_PROCESSING_ENABLE_A/Bf) */
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchHashField0PreProcessEnable, TRUE) );
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchHashField1PreProcessEnable, TRUE) );

    /* Enable RTAG7 HASH on Non-unicast traffic */
    BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchHashControl, &flags));
    flags |= BCM_HASH_CONTROL_TRUNK_NUC_ENHANCE;
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashControl, flags));
    
    return BCM_E_NONE;
}

/* Select which subfields from the 83-bit output are used for ECMP */
bcm_error_t
    ecmp_rtag7_key_select(int unit, int port, int offset)
{
    /* RTAG7_PORT_BASED_HASHm.(SUB_SEL_ECMPf, OFFSET_ECMPf, CONCATENATE_HASH_FIELDS_ECMPf) */
    return bcm_switch_control_port_set (unit, port, bcmSwitchECMPHashSet0Offset, offset);
}

bcm_error_t
    ecmp_rtag7_enable(int unit, int port)
{
    int flags;

    /* HASH_CONTROLr->ECMP_HASH_USE_RTAG7f = 1 */
    BCM_IF_ERROR_RETURN( bcm_switch_control_port_get(unit, port, bcmSwitchHashControl, &flags) );
    flags |= BCM_HASH_CONTROL_ECMP_ENHANCE;
    BCM_IF_ERROR_RETURN( bcm_switch_control_port_set(unit, port, bcmSwitchHashControl, flags) );
    
    return BCM_E_NONE;
}


bcm_error_t
    ecmp_setup_route(bcm_ip_t dip, bcm_ip_t mask, bcm_if_t ecmp_intf)
{
    bcm_l3_route_t route_info;
    
    /* Install the route for DIP, note this is routed out on the port_out */
    bcm_l3_route_t_init(&route_info);
    route_info.l3a_flags = BCM_L3_MULTIPATH;
    route_info.l3a_intf = ecmp_intf;
    route_info.l3a_subnet = dip;
    route_info.l3a_ip_mask = mask;
    BCM_IF_ERROR_RETURN( bcm_l3_route_add(unit, &route_info) );
    
    return BCM_E_NONE;
}

bcm_error_t
    rh_ecmp_setup(bcm_if_t *ecmp_intf, int *member_count)
{
    int i;
    bcm_if_t    rh_ecmp_egr_obj[LB_MEMBER_MAX];
    bcm_l3_ecmp_member_t    ecmp_member[LB_MEMBER_MAX];
    bcm_l3_egress_ecmp_t ecmp_info;
    
    bcm_l3_intf_t l3_intf;
    bcm_l3_egress_t l3_egress_info;
    
    printf("Doing Setup of Resilient Hashing...\n");
    for(i=0; rh_ecmp_info[i].dst_port != LAST_ENTRY; ++i)
    {
        /* Create a L3 interface */
        bcm_l3_intf_t_init(&l3_intf);
        //l3_intf.l3a_flags = BCM_L3_WITH_ID;
        l3_intf.l3a_flags |= BCM_L3_ADD_TO_ARL;
        //l3_intf.l3a_intf_id = 1+i;
        l3_intf.l3a_vid = rh_ecmp_info[i].vid;
        sal_memcpy(l3_intf.l3a_mac_addr, rh_ecmp_ing_intf_mac, 6);
        printf("Doing intf create...\n");
        BCM_IF_ERROR_RETURN( bcm_l3_intf_create(unit, &l3_intf));
        print l3_intf.l3a_intf_id;
        
        bcm_l3_egress_t_init(&l3_egress_info);
        l3_egress_info.intf = l3_intf.l3a_intf_id;
        sal_memcpy(l3_egress_info.mac_addr, rh_ecmp_info[i].dst_mac, 6);
        BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, rh_ecmp_info[i].dst_port,
                                                    &l3_egress_info.port));
        uint32 flags = 0;
        printf("Doing l3 egress create  %d\n", i);
        BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, flags, &l3_egress_info,
                                                              &rh_ecmp_egr_obj[i]));
        printf("Done l3 egress create  %d\n", rh_ecmp_egr_obj[i]);
        bcm_l3_ecmp_member_t_init(&ecmp_member[i]);
        ecmp_member[i].egress_if = rh_ecmp_egr_obj[i];
    }
    
    *member_count = i;
    
    uint32 options = 0;
    
    /* ECMP group - RH */
    bcm_l3_egress_ecmp_t_init(&ecmp_info);
    ecmp_info.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
    ecmp_info.dynamic_size = 64;
    ecmp_info.max_paths = LB_MEMBER_MAX;
    ecmp_info.ecmp_group_flags = BCM_L3_ECMP_UNDERLAY;
    printf("Doing RH ECMP create for members = %d\n", *member_count);
    //bshell(unit, "debug thread 514 soc +");
    if( 1 )
    {
        BCM_IF_ERROR_RETURN( bcm_l3_ecmp_create(unit, options, &ecmp_info,
                                                               *member_count, ecmp_member) );
    }
    else
    {
        BCM_IF_ERROR_RETURN( bcm_l3_egress_ecmp_create(unit, &ecmp_info,
                                                             *member_count, rh_ecmp_egr_obj) );
    }
    //bshell(unit, "debug thread 514 soc -");
    //bshell(unit, "debug =");
    *ecmp_intf = ecmp_info.ecmp_intf;
    printf("Done creating RH ecmp.\n\n");
    
    return BCM_E_NONE;
}

bcm_error_t
    set_ethertype_config(int IsForResilient)
{
    int ethtypes_count = 2;
    int ethtypes[2] = { 0x0800, 0x86DD };
    
    /*  BCM_L3_ECMP_DYNAMIC_ETHERTYPE_RESILIENT:
            DLB or resilent hashing configuration. If set, configure Ethertype eligibility for
            resilient hashing, else for dynamic load balancing. */
    if( IsForResilient )
    {
        BCM_IF_ERROR_RETURN( bcm_l3_egress_ecmp_ethertype_set(unit,
                                        BCM_L3_ECMP_DYNAMIC_ETHERTYPE_ELIGIBLE |
                                        BCM_L3_ECMP_DYNAMIC_ETHERTYPE_RESILIENT,
                                        ethtypes_count, ethtypes) );
    }
    else
    {
        BCM_IF_ERROR_RETURN( bcm_l3_egress_ecmp_ethertype_set(unit,
                                        BCM_L3_ECMP_DYNAMIC_ETHERTYPE_ELIGIBLE, /* Only for DLB */
                                        ethtypes_count, ethtypes) );

    }
    return BCM_E_NONE;
}


bcm_error_t
    dlb_common_init(int unit)
{
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchEcmpDynamicSampleRate,
                                            62500 /*samples per sec. in 16us, 1 sample*/) );
    
    /* Average Value Weightage Settings and DONT Reset when below-average, EWMA. */
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchEcmpDynamicEgressBytesExponent,
                                            3 /* Higher value means, more weightage to average
                                                 than current sample */) );
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchEcmpDynamicEgressBytesDecreaseReset,
                                            0 /* */) );
    if( opt_DevIsTomahawk3 )
    {
        printf("Doing QBytesExponent...\n");
        BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchEcmpDynamicQueuedBytesExponent,
                                                3 /* */) );
        printf("Doing QBytesDecreaseReset...\n");
        BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchEcmpDynamicQueuedBytesDecreaseReset,
                                                0 /* */) );
    }
    printf("Doing PQBytesExponent...\n");
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit,
                                            bcmSwitchEcmpDynamicPhysicalQueuedBytesExponent,
                                            3 /* */) );
    printf("Doing PQDecreaseReset...\n");
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit,
                                            bcmSwitchEcmpDynamicPhysicalQueuedBytesDecreaseReset,
                                            0 /* */) );
    printf("Doing EBytesMinThreshold\n");
    /* Quantization: Port, Logical Queue, Physical Queue */
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchEcmpDynamicEgressBytesMinThreshold,
                                    1000 /* 10% of 10 Gb/sec,  unit -> Mb/sec*/) );
    printf("Doing EBytesMaxThreshold\n");
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchEcmpDynamicEgressBytesMaxThreshold,
                                    10000 /* 10 Gb/sec, unit ->  Mb/sec*/) );
    if( opt_DevIsTomahawk3 )
    {
        printf("Doing QBytesMinThreshold\n");
        BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchEcmpDynamicQueuedBytesMinThreshold,
                                        0x4C60 /* 0x5E cells, unit ->  Bytes, cell -> 208 Bytes*/) );
        printf("Doing QBytesMaxThreshold\n");
        BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchEcmpDynamicQueuedBytesMaxThreshold,
                                        0xD000 /* 0x100 cells, unit -> Bytes, cell -> 208 Bytes*/) );
    }
    printf("Doing PQBytesMinThreshold\n");
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit,
                                    bcmSwitchEcmpDynamicPhysicalQueuedBytesMinThreshold,
                                    0x2630 /* 0x2F cells, unit -> Bytes, cell -> 208 Bytes*/) );
    printf("Doing PQBytesMaxThreshold\n");
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit,
                                    bcmSwitchEcmpDynamicPhysicalQueuedBytesMaxThreshold,
                                    0x6800 /* 0x80 cells, unit -> Bytes, cell -> 208 Bytes*/) );
    /* Seed for randomness to select when 2 ports in equal quality band */
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchEcmpDynamicRandomSeed, 0x3333) );
    
    // QUERY: DEBUG: Below call fails with -16
    if( 0 )
    {
        printf("Configuring for Resilient Hashing...\n");
        BCM_IF_ERROR_RETURN( set_ethertype_config(1) ); // 1: Enable for Resilient
    }
    
    return BCM_E_NONE;
}


bcm_error_t
    dlb_ecmp_setup(bcm_if_t *ecmp_intf, int *member_count)
{
    int i;
    bcm_if_t                dlb_ecmp_egr_obj[LB_MEMBER_MAX];
    bcm_l3_egress_ecmp_t    ecmp_info;
    bcm_l3_ecmp_member_t    ecmp_member[LB_MEMBER_MAX];
    bcm_l3_intf_t l3_intf;
    bcm_l3_egress_t l3_egress_info;
    
    printf("Doing Setup of Resilient Hashing...\n");
    for(i=0; dlb_ecmp_info[i].dst_port != LAST_ENTRY; ++i)
    {
        /* Create a L3 interface */
        bcm_l3_intf_t_init(&l3_intf);
        //l3_intf.l3a_flags = BCM_L3_WITH_ID;
        l3_intf.l3a_flags |= BCM_L3_ADD_TO_ARL;
        //l3_intf.l3a_intf_id = 1+i;
        l3_intf.l3a_vid = dlb_ecmp_info[i].vid;
        sal_memcpy(l3_intf.l3a_mac_addr, dlb_ecmp_ing_intf_mac, 6);
        printf("Doing intf create...\n");
        BCM_IF_ERROR_RETURN( bcm_l3_intf_create(unit, &l3_intf));
        print l3_intf.l3a_intf_id;
        
        bcm_l3_egress_t_init(&l3_egress_info);
        l3_egress_info.intf = l3_intf.l3a_intf_id;
        sal_memcpy(l3_egress_info.mac_addr, dlb_ecmp_info[i].dst_mac, 6);
        BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, dlb_ecmp_info[i].dst_port,
                                                        &l3_egress_info.port));
        l3_egress_info.dynamic_load_weight        = 50;  // 50% weightage to port loading
        l3_egress_info.dynamic_queue_size_weight  = 40;  // 40% weightage to logical q size
                                                         // Remaining 10% weight to physical q size
        
        l3_egress_info.dynamic_scaling_factor  = opt_PortSpeedFactor;
        
        uint32 flags = 0;
        printf("Doing l3 egress create  %d\n", i);
        BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, flags, &l3_egress_info,
                                                              &dlb_ecmp_egr_obj[i]));
        printf("Done l3 egress create  %d\n", dlb_ecmp_egr_obj[i]);
        bcm_l3_ecmp_member_t_init(&ecmp_member[i]);
        ecmp_member[i].egress_if = dlb_ecmp_egr_obj[i];
    }
    
    *member_count = i;
    
    /* ECMP group - DLB */
    bcm_l3_egress_ecmp_t_init(&ecmp_info);
    ecmp_info.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_NORMAL;
    ecmp_info.ecmp_group_flags = BCM_L3_ECMP_UNDERLAY;
    ecmp_info.dynamic_size = 2048;  // Only allowed: power of 2. From 64.
    ecmp_info.dynamic_age = 256;    // usec. Inactivity Duration.
    ecmp_info.max_paths = LB_MEMBER_MAX;
    
    uint32 options = 0;
    
    printf("Doing DLB ECMP create for members = %d\n", *member_count);
    //bshell(unit, "debug thread 514 soc +");
    if( 1 )
    {
        BCM_IF_ERROR_RETURN( bcm_l3_ecmp_create(unit, options, &ecmp_info,
                                                            *member_count, ecmp_member) );
    }
    else
    {
        BCM_IF_ERROR_RETURN( bcm_l3_egress_ecmp_create(unit, &ecmp_info, *member_count,
                                                            dlb_ecmp_egr_obj) );
    }
    printf("Doing member status set...\n");
    /* Configure ECMP member, Link Status. This configuration has one member per port */
    for(i = 0; i < *member_count; ++i) {
        BCM_IF_ERROR_RETURN( bcm_l3_egress_ecmp_member_status_set(unit, dlb_ecmp_egr_obj[i],
                                                            BCM_L3_ECMP_DYNAMIC_MEMBER_HW) );
    }
    
    //bshell(unit, "debug thread 514 soc -");
    //bshell(unit, "debug =");
    *ecmp_intf = ecmp_info.ecmp_intf;
    
    return BCM_E_NONE;
}

bcm_error_t
    tunnel_initiator_setup(int unit, bcm_ip_t sip, bcm_ip_t dip, bcm_if_t l3if)
{
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    bcm_tunnel_initiator_t_init(&tunnel_init);
    l3_intf.l3a_intf_id = l3if;
    tunnel_init.ttl = 10;
    tunnel_init.type = bcmTunnelTypeIp4In4;
    tunnel_init.dip = dip;
    tunnel_init.sip = sip;
    BCM_IF_ERROR_RETURN( bcm_tunnel_initiator_set(unit, &l3_intf, &tunnel_init) );

    return BCM_E_NONE;
}

bcm_error_t
    dgm_hecmp_setup(bcm_if_t *ecmp_intf_ol, int *member_count_ol,
                    bcm_if_t *ecmp_intf_ul, int *member_count_ul)
{
    int i;
    bcm_if_t    dgm_hecmp_egr_obj_ol;
    bcm_if_t    dgm_hecmp_egr_obj_ul1[LB_MEMBER_MAX];
    bcm_l3_ecmp_member_t    ecmp_member_tun1[LB_MEMBER_MAX];
    bcm_l3_ecmp_member_t    ecmp_member_tun2[LB_MEMBER_MAX];
    bcm_l3_ecmp_member_t    *p_ecmp_member;
    bcm_if_t    dgm_hecmp_egr_obj_ul2[LB_MEMBER_MAX];
    bcm_if_t    *p_egr_obj;
    bcm_l3_egress_ecmp_t ecmp_info;
    int ul1_nhop_count = 0;  // belonging to tunnel1
    int ul2_nhop_count = 0;
    uint32 tun_type;
    
    bcm_l3_intf_t l3_intf;
    bcm_l3_egress_t l3_egress_info;
    
    printf("Doing Setup of DGM Hashing for HECMP...\n");
    printf("Creating Underlay NHOPS...\n");
    for(i=0; dgm_hecmp_info[i].nhop.dst_port != LAST_ENTRY; ++i)
    {
        tun_type = dgm_hecmp_info[i].tun_type;
        
        /* Create a L3 interface */
        bcm_l3_intf_t_init(&l3_intf);
        l3_intf.l3a_flags |= BCM_L3_ADD_TO_ARL;
        l3_intf.l3a_vid = dgm_hecmp_info[i].nhop.vid;
        sal_memcpy(l3_intf.l3a_mac_addr, dgm_hecmp_ing_intf_mac, 6);
        printf("Doing intf create...\n");
        BCM_IF_ERROR_RETURN( bcm_l3_intf_create(unit, &l3_intf));
        print l3_intf.l3a_intf_id;
        
        printf("Doing tunnel initiator set...");
        BCM_IF_ERROR_RETURN( tunnel_initiator_setup(unit, dgm_hecmp_info[i].tun_sip,
                                dgm_hecmp_info[i].tun_dip, l3_intf.l3a_intf_id) );
        
        if( GET_TUNNEL_NUM(tun_type) == 1 )
        {
            printf("for Tunnel # 1\n");
            p_egr_obj = &dgm_hecmp_egr_obj_ul1[ul1_nhop_count];
            p_ecmp_member = &ecmp_member_tun1[ul1_nhop_count];
            ul1_nhop_count++;
        }
        else //if( GET_TUNNEL_NUM(tun_type) == 2 )
        {
            printf("for Tunnel # 2\n");
            p_egr_obj = &dgm_hecmp_egr_obj_ul2[ul2_nhop_count];
            p_ecmp_member = &ecmp_member_tun2[ul2_nhop_count];
            ul2_nhop_count++;
        }
        
        bcm_l3_egress_t_init(&l3_egress_info);
        l3_egress_info.intf = l3_intf.l3a_intf_id;
        sal_memcpy(l3_egress_info.mac_addr, dgm_hecmp_info[i].nhop.dst_mac, 6);
        BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, dgm_hecmp_info[i].nhop.dst_port,
                                                        &l3_egress_info.port));
        l3_egress_info.dynamic_load_weight = 50;         // 50% weightage to port loading
        l3_egress_info.dynamic_queue_size_weight  = 40;  // 40% weightage to logical q size
                                                         // 10% weightage to physical q size
        l3_egress_info.dynamic_scaling_factor  = opt_PortSpeedFactor;
        
        printf("Doing l3 egress create  %d for path %s\n", i,
                                        IS_NHOP_ALT_GETFLAG(tun_type) ? "ALT": "MAIN");
        BCM_IF_ERROR_RETURN( bcm_l3_egress_create(unit, 0, &l3_egress_info, p_egr_obj) );
        printf(" Done l3 egress create %d\n", *p_egr_obj);
        
        bcm_l3_ecmp_member_t_init(p_ecmp_member);
        p_ecmp_member->egress_if = *p_egr_obj;
        p_ecmp_member->flags = IS_NHOP_ALT_GETFLAG(tun_type); // BCM_L3_ECMP_MEMBER_DGM_ALTERNATE
    }
    
    member_count_ul[0] = ul1_nhop_count;
    member_count_ul[1] = ul2_nhop_count;
    
    /********* ECMP group. Underlay Tunnel # 1 - DGM ***********/
    bcm_l3_egress_ecmp_t_init(&ecmp_info);
    ecmp_info.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DGM;
    ecmp_info.dynamic_size = 2048;
    ecmp_info.dynamic_age = 256;    // usec. Inactivity Duration.
    ecmp_info.max_paths = LB_MEMBER_MAX;
    ecmp_info.ecmp_group_flags = BCM_L3_ECMP_UNDERLAY;
    ecmp_info.dgm.threshold = 5;
    ecmp_info.dgm.cost = 3;
    ecmp_info.dgm.bias = 0;

    //print dgm_hecmp_egr_obj_ul1;
    //print dgm_hecmp_egr_obj_ul2;
    
    uint32 options = 0;
    
    printf("\nDoing Underlay ECMP for Tunnel 1. Create for members = %d\n", member_count_ul[0]);
    BCM_IF_ERROR_RETURN( bcm_l3_ecmp_create(unit, options, &ecmp_info, member_count_ul[0],
                                                        ecmp_member_tun1) );
    ecmp_intf_ul[0] = ecmp_info.ecmp_intf;
    
    /* Configure ECMP member, Link Status. This configuration has one member per port */
    printf("Setting member status...\n");
    for(i = 0; i < member_count_ul[0]; ++i) {
        BCM_IF_ERROR_RETURN( bcm_l3_egress_ecmp_member_status_set(unit, dgm_hecmp_egr_obj_ul1[i],
                                                            BCM_L3_ECMP_DYNAMIC_MEMBER_HW) );
    }
    ////////////////////////////////////////////////////////////////////////
    /********* ECMP group. Underlay Tunnel # 2 - DGM ***********/
    bcm_l3_egress_ecmp_t_init(&ecmp_info);
    ecmp_info.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DGM;
    ecmp_info.dynamic_size = 2048;
    ecmp_info.dynamic_age = 256;    // usec. Inactivity Duration.
    ecmp_info.max_paths = LB_MEMBER_MAX;
    ecmp_info.ecmp_group_flags = BCM_L3_ECMP_UNDERLAY;
    ecmp_info.dgm.threshold = 5;
    ecmp_info.dgm.cost = 3;
    ecmp_info.dgm.bias = 0;
    
    printf("Doing Underlay ECMP for Tunnel 2. Create for members = %d\n", member_count_ul[1]);
    BCM_IF_ERROR_RETURN( bcm_l3_ecmp_create(unit, options, &ecmp_info,
                                                        member_count_ul[1], ecmp_member_tun2) );
    ecmp_intf_ul[1] = ecmp_info.ecmp_intf;
    
    /* Configure ECMP member, Link Status. This configuration has one member per port */
    printf("Setting member status...\n");
    for(i = 0; i < member_count_ul[1]; ++i) {
        BCM_IF_ERROR_RETURN( bcm_l3_egress_ecmp_member_status_set(unit, dgm_hecmp_egr_obj_ul2[i],
                                                            BCM_L3_ECMP_DYNAMIC_MEMBER_HW) );
    }
    ////////////////////////////////////////////////////////////////////////
    /********* ECMP group. Overlay  ***********/
    /* Add above two underlay ECMP groups */
    bcm_l3_egress_ecmp_t_init(&ecmp_info);
    ecmp_info.ecmp_group_flags = BCM_L3_ECMP_OVERLAY;
    ecmp_info.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RANDOM;
    ecmp_info.dynamic_size = 64;
    ecmp_info.max_paths = 64;
    printf("\nDoing Overlay ECMP. Create for members = %d\n\n", 2);
    BCM_IF_ERROR_RETURN( bcm_l3_egress_ecmp_create(unit, &ecmp_info, 2, ecmp_intf_ul) );
    
    *ecmp_intf_ol = ecmp_info.ecmp_intf;
    *member_count_ol = 2;
    
    return BCM_E_NONE;
}

bcm_error_t
    add_default_route(int unit)
{
    bcm_l3_route_t      route_info;
    bcm_if_t            egr_obj_id_default;
    bcm_l3_egress_t     l3_egress;
    bcm_l3_intf_t       l3_intf_out;
    int                 vrf_default = 0;
    bcm_mac_t           dst_mac_nhop   = {0x00, 0x00, 0x00, 0x00, 0xFE, 0x02};
    
    bcm_l3_intf_t_init(&l3_intf_out);
    sal_memcpy(l3_intf_out.l3a_mac_addr, test_smac,
                                sizeof(test_smac)); // This will be smac for egress pkt
    l3_intf_out.l3a_vid = test_vid;
    printf("Doing L3 intf create for default route...\n");
    BCM_IF_ERROR_RETURN( bcm_l3_intf_create(unit, &l3_intf_out) );
    
    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr, dst_mac_nhop, sizeof(dst_mac_nhop));
    l3_egress.port = egr_port1_rh;  // Set to any egress port.
                                    // We are not exercising default route in this test.
    l3_egress.intf = l3_intf_out.l3a_intf_id;
    l3_egress.mtu = 1500;
    printf("Doing L3 DEFAULT egress create...\n");
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_id_default));
    
    bcm_l3_route_t_init(&route_info);
    route_info.l3a_intf = egr_obj_id_default;
    route_info.l3a_subnet = 0;
    route_info.l3a_ip_mask = 0;
    route_info.l3a_vrf = vrf_default;
    printf("Doing DEFAULT route add %d(x%X)...\n", egr_obj_id_default, egr_obj_id_default);
    BCM_IF_ERROR_RETURN(bcm_l3_route_add(unit, &route_info));
    
    return BCM_E_NONE;
}

bcm_error_t
    do_common_init(int unit)
{
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE) );
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchL3IngressMode, TRUE) );

    //Configure RTAG7 engines
    printf("Doing RTAG7 Config...\n");
    BCM_IF_ERROR_RETURN( ecmp_rtag7_config(unit) );
    //printf("Doing RTAG7 offset...\n");
    //ecmp_rtag7_key_select(unit, ing_port1, 0);
    
    BCM_IF_ERROR_RETURN( ecmp_rtag7_enable(unit, ing_port1) );
    
    BCM_IF_ERROR_RETURN( add_default_route(unit) );
    
    return BCM_E_NONE;
}


bcm_error_t
    do_all_load_balancing_config(int unit)
{
    int member_count;
    
    do_common_init(unit);
    
    if( opt_TestTypeRH )
    {
        bcm_if_t    rh_ecmp_intf;
        
        printf("\n\n//////////   Resilient Hashing  ///////////////\n");
        BCM_IF_ERROR_RETURN( rh_ecmp_setup(&rh_ecmp_intf, &member_count) );
        
        BCM_IF_ERROR_RETURN( ecmp_setup_route(rh_ecmp_dip, rh_ecmp_ipmask, rh_ecmp_intf) );
        
        printf("Doing agm stat create for %d, members:%d...\n", rh_ecmp_intf, member_count);
        BCM_IF_ERROR_RETURN( agm_ecmp_stats_create(unit, rh_ecmp_intf, member_count,
                                                                       &rh_ecmp_agm) );
    }
    ///////////////////////////////////////////////////////////////////
    printf("\n\n");
    if( opt_TestTypeDLB || opt_TestTypeDGM_HECMP )
    {
        printf("Doing DLB COMMON INIT...\n");
        BCM_IF_ERROR_RETURN( dlb_common_init(unit) );
    }
    
    if( opt_TestTypeDLB )
    {
        printf("\n//////////   DLB Hashing  ///////////////\n");
        
        bcm_if_t  dlb_ecmp_intf;
        
        BCM_IF_ERROR_RETURN( dlb_ecmp_setup(&dlb_ecmp_intf, &member_count) );
        
        BCM_IF_ERROR_RETURN( ecmp_setup_route(dlb_ecmp_dip, dlb_ecmp_ipmask, dlb_ecmp_intf) );
        
        printf("Doing agm stat create for DLB %d, members:%d...\n", dlb_ecmp_intf, member_count);
        BCM_IF_ERROR_RETURN( agm_ecmp_stats_create(unit, dlb_ecmp_intf, member_count,
                                                    &dlb_ecmp_agm) );
    }
    ///////////////////////////////////////////////////////////////////
    if( opt_TestTypeDGM_HECMP )
    {
        printf("\n\n//////////   DGM Hashing on HECMP ///////////////\n");
        int member_count_ol;
        int member_count_ul[2];
        bcm_if_t  dgm_hecmp_intf_ol;
        bcm_if_t  dgm_hecmp_intf_ul[2];
        
        BCM_IF_ERROR_RETURN( dgm_hecmp_setup(&dgm_hecmp_intf_ol,  &member_count_ol,
                              &dgm_hecmp_intf_ul, &member_count_ul) );
        
        BCM_IF_ERROR_RETURN( ecmp_setup_route(dgm_hecmp_dip, dgm_hecmp_ipmask,
                                            dgm_hecmp_intf_ol) );
        
        printf("Doing agm stat create for DGM-HECMP ul1 %d, members:%d...\n",
                                            dgm_hecmp_intf_ul[0], member_count_ul[0]);
        BCM_IF_ERROR_RETURN( agm_ecmp_stats_create(unit, dgm_hecmp_intf_ul[0], member_count_ul[0],
                                            &dgm_hecmp_agm_ul[0]) );
        printf("Doing agm stat create for DGM-HECMP ul2 %d, members:%d...\n",
                                            dgm_hecmp_intf_ul[1], member_count_ul[1]);
        BCM_IF_ERROR_RETURN( agm_ecmp_stats_create(unit, dgm_hecmp_intf_ul[1], member_count_ul[1],
                                            &dgm_hecmp_agm_ul[1]) );
        if( 0 ) // No AGM for Overlay ECMP DGM
        {
            printf("Doing agm stat create for DGM-HECMP OL %d, members:%d...\n",
                                                dgm_hecmp_intf_ol, member_count_ol);
            BCM_IF_ERROR_RETURN( agm_ecmp_stats_create(unit, dgm_hecmp_intf_ol, member_count_ol,
                                                &dgm_hecmp_agm_ol));
        }
    }
    return BCM_E_NONE;
}

bcm_error_t
    do_test_setup(int unit)
{
    int LoopBackType = BCM_PORT_LOOPBACK_PHY;
    
    //LoopBackType = BCM_PORT_LOOPBACK_MAC;
    
    bcm_pbmp_t     vlan_pbmp;
    bcm_pbmp_t     ut_pbmp;
    
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port1, LoopBackType));
    //BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ing_port1, BCM_PORT_DISCARD_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port2, LoopBackType));
    //BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ing_port2, BCM_PORT_DISCARD_NONE));
    
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port1_rh, LoopBackType));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit,  egr_port1_rh, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port2_rh, LoopBackType));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit,  egr_port2_rh, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port3_rh, LoopBackType));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit,  egr_port3_rh, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port4_rh, LoopBackType));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit,  egr_port4_rh, BCM_PORT_DISCARD_ALL));
    
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port1_dlb, LoopBackType));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit,  egr_port1_dlb, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port2_dlb, LoopBackType));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit,  egr_port2_dlb, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port3_dlb, LoopBackType));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit,  egr_port3_dlb, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port4_dlb, LoopBackType));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit,  egr_port4_dlb, BCM_PORT_DISCARD_ALL));
    
    
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port1_hecmp_ul1, LoopBackType));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit,  egr_port1_hecmp_ul1, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port2_hecmp_ul1, LoopBackType));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit,  egr_port2_hecmp_ul1, BCM_PORT_DISCARD_ALL));
    
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port1_hecmp_ul2, LoopBackType));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit,  egr_port1_hecmp_ul2, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port2_hecmp_ul2, LoopBackType));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit,  egr_port2_hecmp_ul2, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port3_hecmp_ul2, LoopBackType));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit,  egr_port3_hecmp_ul2, BCM_PORT_DISCARD_ALL));
    
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port1_hecmp_ul1_alt, LoopBackType));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit,  egr_port1_hecmp_ul1_alt, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port2_hecmp_ul1_alt, LoopBackType));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit,  egr_port2_hecmp_ul1_alt, BCM_PORT_DISCARD_ALL));
    
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port1_hecmp_ul2_alt, LoopBackType));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit,  egr_port1_hecmp_ul2_alt, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port2_hecmp_ul2_alt, LoopBackType));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit,  egr_port2_hecmp_ul2_alt, BCM_PORT_DISCARD_ALL));
    
    BCM_PBMP_CLEAR(vlan_pbmp);
    BCM_PBMP_PORT_ADD(vlan_pbmp, 0);    // cpu
    BCM_PBMP_PORT_ADD(vlan_pbmp, ing_port1);
    BCM_PBMP_PORT_ADD(vlan_pbmp, ing_port2);
    
    BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port1_rh);
    BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port2_rh);
    BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port3_rh);
    BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port4_rh);
    
    BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port1_dlb);
    BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port2_dlb);
    BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port3_dlb);
    BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port4_dlb);
    
    BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port1_hecmp_ul1);
    BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port2_hecmp_ul1);
    BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port1_hecmp_ul2);
    BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port2_hecmp_ul2);
    BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port3_hecmp_ul2);
    BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port1_hecmp_ul1_alt);
    BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port2_hecmp_ul1_alt);
    BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port1_hecmp_ul2_alt);
    BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port2_hecmp_ul2_alt);

    BCM_PBMP_CLEAR(ut_pbmp);
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, test_vid));
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, test_vid, vlan_pbmp, ut_pbmp));
    BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, 1, vlan_pbmp)); // remove from default vlan
    
    return BCM_E_NONE;
}


/*
 How to verify: It is documented within the function for various packets and options
*/
bcm_error_t verify()
{
    char cmd[1024*2];
    int i;
    uint16 l4srcport;
    uint16 l4dstport;
    
    l4srcport = 0x1111;
    l4dstport = 0x2222;
    
    print opt_TestTypeRH;
    print opt_TestTypeDLB;
    print opt_TestTypeDGM_HECMP;
    
    if( opt_TestTypeRH )
    {
        for(i=0; i<5; ++i)
        {
            printf("\n\n\n############  Sending for RESILIENT HASHING %d  #######\n", i+1);
            
            BCM_IF_ERROR_RETURN(agm_ecmp_stats_start(unit, &rh_ecmp_agm));
            
            sprintf(cmd, "tx 100 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X08004500004E00000000400659A9%08X%08X%04X%04X00000000000000005000000050D70000ECB0E14491AAC4836F54D30B8902602E00200000E7A41AA3B1040000E70727EB2AA8ECB0E1446B931F11",
                ing_port1,
                rh_ecmp_ing_intf_mac[0], rh_ecmp_ing_intf_mac[1], rh_ecmp_ing_intf_mac[2],
                rh_ecmp_ing_intf_mac[3], rh_ecmp_ing_intf_mac[4], rh_ecmp_ing_intf_mac[5],
                test_smac[0], test_smac[1]+i, test_smac[2]+i,
                test_smac[3], test_smac[4], test_smac[5]+i,
                test_vid, rh_ecmp_sip, rh_ecmp_dip, l4srcport+i, l4dstport+i);
            bbshell(unit, cmd);
            
            sprintf(cmd, "sleep %d", 2);
            bbshell(unit, cmd);
            
            bbshell(unit, "show c");
            
            BCM_IF_ERROR_RETURN(agm_ecmp_stats_print(unit, &rh_ecmp_agm, 1));
            
            sprintf(cmd, "sleep %d", 1);
            bbshell(unit, cmd);
        }
    }
    
    if( opt_TestTypeDLB )
    {
        printf("\n\n");
        
        printf("Setting ethertypes for DLB...\n");
        BCM_IF_ERROR_RETURN( set_ethertype_config(0) );
        
        for(i=0; i<5; ++i)
        {
            printf("\n\n############  Sending for DLB HASHING %d  #######\n", i+1);
            
            BCM_IF_ERROR_RETURN(agm_ecmp_stats_start(unit, &dlb_ecmp_agm));
            
            sprintf(cmd, "tx 100 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X08004500004E00000000400639A9%08X%08X%04X%04X00000000000000005000000030D70000ECB0E14491AAC4836F54D30B8902602E00200000E7A41AA3B1040000E70727EB2AA8ECB0E14418804592",
                ing_port1,
                dlb_ecmp_ing_intf_mac[0], dlb_ecmp_ing_intf_mac[1], dlb_ecmp_ing_intf_mac[2],
                dlb_ecmp_ing_intf_mac[3], dlb_ecmp_ing_intf_mac[4], dlb_ecmp_ing_intf_mac[5],
                test_smac[0], test_smac[1]+i, test_smac[2]+i,
                test_smac[3], test_smac[4], test_smac[5]+i,
                test_vid, dlb_ecmp_sip, dlb_ecmp_dip, l4srcport+i, l4dstport+i);
            bbshell(unit, cmd);
            
            sprintf(cmd, "sleep %d", 2);
            bbshell(unit, cmd);
            
            bbshell(unit, "show c");
            
            BCM_IF_ERROR_RETURN(agm_ecmp_stats_print(unit, &dlb_ecmp_agm, 1));
            
            sprintf(cmd, "sleep %d", 1);
            bbshell(unit, cmd);
        }
    }
    
    if( opt_TestTypeDGM_HECMP )
    {
        printf("\n\n");
        
        for(i=0; i<5; ++i)
        {
            printf("\n\n############  Sending for DGM HASHING for HECMP %d  #######\n", i+1);
            
            //BCM_IF_ERROR_RETURN(agm_ecmp_stats_start(unit, &dgm_hecmp_agm_ol));
            BCM_IF_ERROR_RETURN(agm_ecmp_stats_start(unit, &dgm_hecmp_agm_ul[0]));
            BCM_IF_ERROR_RETURN(agm_ecmp_stats_start(unit, &dgm_hecmp_agm_ul[1]));
            
            sprintf(cmd, "tx 100 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X08004500004E000000004006D9A8%08X%08X%04X%04X000000000000000050000000D0D60000ECB0E14491AAC4836F54D30B8902602E00200000E7A41AA3B1040000E70727EB2AA8ECB0E1443E4A9062",
                ing_port1,
                dgm_hecmp_ing_intf_mac[0], dgm_hecmp_ing_intf_mac[1], dgm_hecmp_ing_intf_mac[2],
                dgm_hecmp_ing_intf_mac[3], dgm_hecmp_ing_intf_mac[4], dgm_hecmp_ing_intf_mac[5],
                test_smac[0], test_smac[1]+i, test_smac[2]+i,
                test_smac[3], test_smac[4], test_smac[5]+i,
                test_vid, dgm_hecmp_sip, dgm_hecmp_dip, l4srcport+i, l4dstport+i);
            bbshell(unit, cmd);
            
            sprintf(cmd, "sleep %d", 2);
            bbshell(unit, cmd);
            
            bbshell(unit, "show c");
            
            //BCM_IF_ERROR_RETURN(agm_ecmp_stats_print(unit, &dgm_hecmp_agm_ol, 1));
            BCM_IF_ERROR_RETURN(agm_ecmp_stats_print(unit, &dgm_hecmp_agm_ul[0], 1));
            BCM_IF_ERROR_RETURN(agm_ecmp_stats_print(unit, &dgm_hecmp_agm_ul[1], 1));
            
            sprintf(cmd, "sleep %d", 1);
            bbshell(unit, cmd);
        }
    }
    
    return BCM_E_NONE;
}

bcm_error_t execute()
{
    int rv;
    
    if( (rv = do_test_setup(unit)) != BCM_E_NONE )
    {
        printf("Creating the test setup failed %d\n", rv);
        return rv;
    }
    
    if( (rv = do_all_load_balancing_config(unit)) != BCM_E_NONE )
    {
        printf("Configuring load balancing config failed with %d\n", rv);
        return rv;
    }

    printf("\n##############   VERIFY   #######################\n\n");
    if( (rv = verify()) != BCM_E_NONE )
    {
        printf("Verify failed with %d\n", rv);
        return rv;
    }
    
    return BCM_E_NONE;
}


const char *auto_execute = (ARGC >= 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

/*
BCM.0> d chg L3_ECMP
    L3_ECMP.ipipe0[0]: <NHOP_ECMP_GROUP=5,NEXT_HOP_INDEX=5,DATA=5>
    L3_ECMP.ipipe0[1]: <NHOP_ECMP_GROUP=2,NEXT_HOP_INDEX=2,DATA=2>
    ....
    L3_ECMP.ipipe0[63]: <NHOP_ECMP_GROUP=4,NEXT_HOP_INDEX=4,DATA=4>
    BCM.0> 
BCM.0> d chg EGR_L3_INTF
    EGR_L3_INTF.epipe0[0]: <VID=0xa,OVID=0xa,MAC_ADDRESS=0x000000000033,DATA=0x0000000000000660000000028000,>
    ...
    EGR_L3_INTF.epipe0[8191]: <MAC_ADDRESS=0xffffffffffff,L2_SWITCH=1,DATA=0x001ffffffffffff0000000000000,>
    BCM.0> 
BCM.0> d chg L3_ECMP_GROUP
    L3_ECMP_GROUP.ipipe0[0]: <MEMBER_COUNT=6,LB_MODE=1,DATA=0x0040000006,COUNT=6,BASE_PTR=0,>
    L3_ECMP_GROUP.ipipe0[1]: <MEMBER_COUNT=6,DATA=0x0000000006,COUNT=6,BASE_PTR=0,> <<<<<<<< 2^6 = 64
BCM.0> d chg EGR_L3_NEXT_HOP
    EGR_L3_NEXT_HOP.epipe0[1]: <MAC_ADDRESS=0xffffffffffff,L3MC:MAC_ADDRESS=0xfffff8000000,L3MC:L3_MC_VLAN_DISABLE=1,L3MC:L3_MC_TTL_DISABLE=1,L3MC:L3_MC_SA_DISABLE=1,L3MC:L3_MC_DA_DISABLE=1,L3MC:L3_DROP=1,L3MC:L3MC_USE_CONFIGURED_MAC=1,L3MC:FLEX_CTR_POOL_NUMBER=3,L3MC:FLEX_CTR_OFFSET_MODE=3,L3MC:FLEX_CTR_BASE_COUNTER_IDX=0xfff,L3:MAC_ADDRESS=0xffffffffffff,DATA=0x001fffffffffffe000000000,>
    EGR_L3_NEXT_HOP.epipe0[2]: <MAC_ADDRESS=0x00000000003a,L3MC:MAC_ADDRESS=0x0001d0000000,L3:MAC_ADDRESS=0x00000000003a,DATA=0x000000000000074000000000,>
    ....
    EGR_L3_NEXT_HOP.epipe0[5]: <MAC_ADDRESS=0x00000000003d,L3MC:MAC_ADDRESS=0x0001e8000000,L3:MAC_ADDRESS=0x00000000003d,DATA=0x00000000000007a000000000,>
BCM.0> d chg EGR_L3_NEXT_HOP_2
    EGR_L3_NEXT_HOP_2.epipe0[1]: <INTF_NUM=0x1fff,DATA=0x1fff>
    ...
    EGR_L3_NEXT_HOP_2.epipe0[5]: <INTF_NUM=3,DATA=3>
BCM.0> d chg ING_L3_NEXT_HOP
    ING_L3_NEXT_HOP.ipipe0[0]: <DROP=1,DATA=0x800000,>
    ING_L3_NEXT_HOP.ipipe0[2]: <VLAN_ID=0xa,TGID=7,PORT_NUM=7,MTU_SIZE=0xa,L3_OIF=0xa,DVP_RES_INFO=0xa,DATA=0x1c00a,>
    ...
    ING_L3_NEXT_HOP.ipipe0[5]: <VLAN_ID=0xa,TGID=0xa,PORT_NUM=0xa,MTU_SIZE=0xa,L3_OIF=0xa,DVP_RES_INFO=0xa,DATA=0x2800a,>
    BCM.0> 
*/

/*
CONFIG:
    phy_chain_rx_lane_map_physical{89.0}=0x02134657
    portmap_120=193:100
    portmap_112=185:100
    portmap_104=169:100
    portmap_8=15:100
    portmap_121=195:100
    portmap_113=187:100
    portmap_105=171:100
    portmap_9=17:100
    portmap_130=213:100
    portmap_122=197:100
    portmap_114=189:100
    portmap_106=173:100
    portmap_10=19:100
    phy_chain_rx_lane_map_physical{73.0}=0x45230617
    portmap_131=215:100
    device_clock_frequency=1325
    portmap_123=199:100
    portmap_115=191:100
    portmap_107=175:100
    phy_chain_rx_lane_map_physical{105.0}=0x16073425
    portmap_11=21:100
    ctr_evict_enable=0
    parity_correction=0
    portmap_140=225:100
    portmap_132=217:100
    portmap_124=201:100
    portmap_108=177:100
    portmap_20=33:100
    portmap_12=23:100
    phy_chain_tx_lane_map_physical{177.0}=0x74615230
    ccmdma_intr_enable=0
    portmap_141=227:100
    portmap_133=219:100
    portmap_125=203:100
    portmap_109=179:100
    portmap_21=35:100
    portmap_13=25:100
    serdes_core_tx_polarity_flip_physical{193}=0xdc
    portmap_150=245:100
    portmap_142=229:100
    portmap_134=221:100
    portmap_126=205:100
    portmap_118=258:10
    portmap_30=53:100
    portmap_22=37:100
    portmap_14=27:100
    portmap_151=247:100
    portmap_143=231:100
    portmap_135=223:100
    portmap_127=207:100
    portmap_119=264:10
    phy_chain_tx_lane_map_physical{161.0}=0x34567210
    portmap_31=55:100
    portmap_23=39:100
    portmap_15=29:100
    serdes_core_tx_polarity_flip_physical{177}=0x86
    serdes_core_tx_polarity_flip_physical{73}=0xcb
    portmap_152=249:100
    portmap_144=233:100
    portmap_128=209:100
    phy_chain_rx_lane_map_physical{193.0}=0x25713064
    portmap_40=65:100
    portmap_32=57:100
    portmap_24=41:100
    portmap_16=31:100
    phy_chain_rx_lane_map_physical{57.0}=0x04152736
    l3_ecmp_levels=2
    portmap_153=251:100
    portmap_145=235:100
    portmap_129=211:100
    serdes_core_rx_polarity_flip_physical{193}=0x2c
    portmap_41=67:100
    portmap_33=59:100
    portmap_25=43:100
    serdes_core_tx_polarity_flip_physical{57}=0x59
    portmap_154=253:100
    portmap_146=237:100
    portmap_50=85:100
    portmap_42=69:100
    portmap_34=61:100
    portmap_26=45:100
    ccm_dma_enable=0
    pll_bypass=1
    portmap_155=255:100
    portmap_147=239:100
    portmap_139=265:10
    serdes_core_rx_polarity_flip_physical{177}=0x46
    portmap_51=87:100
    portmap_43=71:100
    portmap_35=63:100
    portmap_27=47:100
    portmap_19=259:10
    portmap_148=241:100
    portmap_60=97:100
    portmap_52=89:100
    portmap_44=73:100
    portmap_28=49:100
    portmap_149=243:100
    phy_chain_tx_lane_map_physical{145.0}=0x57463012
    portmap_61=99:100
    portmap_53=91:100
    portmap_45=75:100
    portmap_29=51:100
    serdes_core_rx_polarity_flip_physical{73}=0x96
    serdes_core_tx_polarity_flip_physical{105}=0x30
    phy_chain_rx_lane_map_physical{177.0}=0x50142376
    portmap_70=117:100
    portmap_62=101:100
    portmap_54=93:100
    portmap_46=77:100
    portmap_38=257:10
    os=unix
    portmap_159=266:10
    init_all_modules=0
    portmap_71=119:100
    portmap_63=103:100
    portmap_55=95:100
    portmap_47=79:100
    portmap_39=260:10
    serdes_core_rx_polarity_flip_physical{57}=0x63
    portmap_80=129:100
    portmap_72=121:100
    portmap_64=105:100
    portmap_48=81:100
    phy_chain_rx_lane_map_physical{161.0}=0x02475316
    portmap_81=131:100
    portmap_73=123:100
    portmap_65=107:100
    portmap_49=83:100
    serdes_core_rx_polarity_flip_physical{105}=0x98
    port_flex_enable=1
    portmap_90=149:100
    portmap_82=133:100
    portmap_74=125:100
    portmap_66=109:100
    portmap_91=151:100
    portmap_83=135:100
    portmap_75=127:100
    portmap_67=111:100
    portmap_59=261:10
    phy_enable=0
    phy_chain_tx_lane_map_physical{89.0}=0x35147260
    phy_null=1
    portmap_92=153:100
    portmap_84=137:100
    portmap_68=113:100
    mem_cache_enable=0
    parity_enable=0
    pbmp_xport_xe.0=0x8ffff8ffffcffff8ffff8ffff8ffffcffff9fffe
    portmap_93=155:100
    portmap_85=139:100
    core_clock_frequency=1325
    portmap_69=115:100
    portmap_94=157:100
    portmap_86=141:100
    phy_chain_tx_lane_map_physical{73.0}=0x64753210
    serdes_core_tx_polarity_flip_physical{89}=0xa1
    phy_chain_rx_lane_map_physical{145.0}=0x71604352
    load_firmware=0x2
    portmap_95=159:100
    portmap_87=143:100
    portmap_79=262:10
    serdes_core_tx_polarity_flip_physical{161}=0xc2
    portmap_1=1:100
    portmap_88=145:100
    phy_chain_tx_lane_map_physical{105.0}=0x57463210
    dpr_clock_frequency=1000
    portmap_2=3:100
    portmap_89=147:100
    serdes_core_tx_polarity_flip_physical{145}=0x6f
    portmap_3=5:100
    portmap_99=263:10
    portmap_100=161:100
    portmap_4=7:100
    serdes_core_rx_polarity_flip_physical{161}=0x14
    phy_chain_tx_lane_map_physical{57.0}=0x43516270
    portmap_101=163:100
    portmap_5=9:100
    serdes_core_rx_polarity_flip_physical{89}=0x3c
    portmap_110=181:100
    portmap_102=165:100
    portmap_6=11:100
    serdes_core_rx_polarity_flip_physical{145}=0x96
    phy_chain_tx_lane_map_physical{193.0}=0x71435260
    portmap_111=183:100
    portmap_103=167:100
    portmap_7=13:100

*/