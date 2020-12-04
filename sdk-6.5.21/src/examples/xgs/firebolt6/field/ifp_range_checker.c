/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : IFP Range Checker
 *
 *  Usage    : BCM.0> cint ifp_range_checker.c
 *
 *  config   : field_config.bcm
 *
 *  Log file : ifp_range_checker_log.txt
 *
 *  Test Topology : SJ-Rack 63.22
 *
 *                   +------------------------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *  +----------------+          SWITCH              +-----------------+
 *                   |                              |
 *                   |                              |
 *  +----------------+                              |
 *                   |                              |
 *                   |                              |
 *                   +------------------------------+
 *
 *  Summary:
 *  ========
 *      Cint example to show configuration of Range Checker and IFP in
 *      Tomahawk3 using BCM APIs.
 *      The following example shows the usage of Range Checker and IFP TCAM in
 *      Pipe global mode [entries installed in all of the four pipes]. Range
 *      Module in Pipe global Mode and IFP TCAM in Global mode is a miss
 *      configuration and should never be done. But both the modules Operating
 *      in same mode and Range in Global mode and Field in Pipe Global mode are
 *      allowed configurations.
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
 *    2) Step2 - Configuration (Done in range()).
 *    ===================================================
 *       a) Create Two Ranges of of type bcmRangeTypeOuterVlan with VID values
 *            Range1 = [101-105]
 *            Range2 = [111-115]
 *     
 *       c) Create entry in Main IFP - TCAM
 *            Qualifier1 : SrcMac - 0x202
 *            Qualifier2 : RangeCheckGroup with range_bmp = Range1 OR Range2
 *            Action     : CopyToCpu with match = 200
 *       d) Create STAT with main_ifp_statid = 1
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==============================
 *        a) Send packets with VID from 101 to 120
 *        b) Expected Result:
 *          ================
 *          i)  VID within Range1 and Range2: Match occurs. STAT 1 increases
 *          ii) VID outside Range1 and Range2: Match does NOT occurs. STAT 1 unchanged.
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

int opt_UseRangeGroupSetOfApis = 0; // bcmFieldQualifyRangeCheck vs ...QualifyRangeCheckGroup
int opt_UseMultipleRanges = 1;      // Use both ranges of a given range type.
int opt_UseRangeInvert = 0;
int opt_PipeLocal = 0;

if( ARGC >= 2 ) {
    if( *ARGV[1] == '1' ) {
        opt_UseRangeInvert = 1;
    } else {
        opt_UseRangeInvert = 0;
    }
}

/////// FIREBOLT6 Settings    //////
// FIREBOLT6 Supports Range Group set of APIs
opt_UseRangeGroupSetOfApis = 1;
// FIREBOLT6 Supports only Pipe Local
opt_PipeLocal = 0;
// FIREBOLT6 supports multiple ranges of a given flag
opt_UseMultipleRanges = 1;


if( !opt_UseRangeGroupSetOfApis )
{
    printf("If RangeGroup is not supported, Multiple Ranges of a RangeType cannot be used\n");
    opt_UseMultipleRanges = 0;
}

if( opt_UseMultipleRanges )
{
    printf("If using multiple ranges of a given RangeType, invert cannot be used.\n");
    opt_UseRangeInvert = 0;
}

print opt_UseRangeGroupSetOfApis;
print opt_UseMultipleRanges;
print opt_UseRangeInvert;
print opt_PipeLocal;

int unit=0;

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

bcm_vlan_t test_vid_range_min = 95;

bcm_vlan_t test_vid1_min = 101;    // = 101; // Here VID itself is a variable in RANGE
bcm_vlan_t test_vid1_max = 105;    // = 105; // Here VID itself is a variable in RANGE

bcm_vlan_t test_vid2_min = 111;    // = 111; // Here VID itself is a variable in RANGE
bcm_vlan_t test_vid2_max = 115;    // = 115; // Here VID itself is a variable in RANGE


bcm_vlan_t test_vid_range_max;

if( !opt_UseMultipleRanges )
{
    test_vid_range_max = 115;
}
else
{
    test_vid_range_max = 120;
}

bcm_mac_t test_dmac = {0x00, 0x00, 0x00, 0x00, 0x00, 0xDD};
int         main_ifp_statid = 1;

/* Range configuration structure */
struct range_cfg_t{
    bcm_range_t range_id;         /* Range id */
    bcm_range_type_t range_type;  /* Range type */
    int min;                      /* Min value for Range Checker configuration*/
    int max;                      /* Max value for Range Checker configuration*/
};

/* Set the reguired Range Checker Configuration here */
range_cfg_t range_cfg1 = {
    0x1,                    // range_id
    bcmRangeTypeOuterVlan,    // range_type
    test_vid1_min,
    test_vid1_max
};

/* Set the reguired Range Checker Configuration here */
range_cfg_t range_cfg2 = {
    0x2,                    // range_id
    bcmRangeTypeOuterVlan,    // range_type
    test_vid2_min,
    test_vid2_max
};

/* FP configuration structure */
struct fp_cfg_t{
    bcm_mac_t src_mac;       /* Src Mac to qualify in main TCAM */
    bcm_mac_t mac_mask;      /* Mac Mask to qualify in main TCAM */
};

/* Set the reguired FP Configuration here */
fp_cfg_t fp_cfg = {
    {0x00, 0x00, 0x00, 0x00, 0x02, 0x02},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};

int
configure_rangechecker(int unit)
{
    bcm_range_config_t     range_config;
    bcm_port_config_t      port_config;

    if( !opt_UseRangeGroupSetOfApis ) {
        printf("Doing SC RangeCheckersAPIType\n");
        BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,
                                               bcmSwitchRangeCheckersAPIType, 1));
    }
    
    /* Setting the Operational mode to PipeGlobal Mode */
    if( opt_PipeLocal ) {
        printf("Doing bcm_range_oper_mode_set\n");
        BCM_IF_ERROR_RETURN(bcm_range_oper_mode_set(unit, bcmRangeOperModeGlobal));
    }
    
    bcm_port_config_t_init(&port_config);
    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &port_config));

    /* Range Check Configuration and Creation */
    bcm_range_config_t_init(&range_config);
    range_config.rid = range_cfg1.range_id;
    range_config.rtype = range_cfg1.range_type;
    //range_config.offset = 0;    // bits
    //range_config.width = 16;    // bits
    range_config.min = range_cfg1.min;
    range_config.max = range_cfg1.max;
    //range_config.ports = port_config.per_pipe[0]; // bcmRangeOperModePipeLocal
    range_config.ports = port_config.all;    // bcmRangeOperModeGlobal

    printf("Doing range1 create...\n");
    BCM_IF_ERROR_RETURN(bcm_range_create(unit, BCM_RANGE_CREATE_WITH_ID,
                                          &range_config));
    
    if( opt_UseMultipleRanges )
    {
        /* Range Check Configuration and Creation */
        bcm_range_config_t_init(&range_config);
        range_config.rid = range_cfg2.range_id;
        range_config.rtype = range_cfg2.range_type;
        //range_config.offset = 0;
        //range_config.width = 16;
        range_config.min = range_cfg2.min;
        range_config.max = range_cfg2.max;
        //range_config.ports = port_config.per_pipe[0]; // bcmRangeOperModePipeLocal
        range_config.ports = port_config.all;    // bcmRangeOperModeGlobal
        
        printf("Doing range2 create...\n");
        BCM_IF_ERROR_RETURN(bcm_range_create(unit, BCM_RANGE_CREATE_WITH_ID,
                                              &range_config));
    }

    return BCM_E_NONE;
}

int
configure_fp(int unit, fp_cfg_t *ifp_cfg)
{
    bcm_field_aset_t    actn;
    bcm_field_entry_t entry;
    bcm_port_config_t port_config;
    bcm_field_group_config_t group_config;
    
    if( opt_PipeLocal ) {
        printf("Doing IFP oper mode set GLOBAL\n");
        /* Setting the Operational mode to PipeGlobal Mode */
        BCM_IF_ERROR_RETURN(bcm_field_group_oper_mode_set(unit,
                                                      bcmFieldQualifyStageIngress,
                                                      bcmFieldGroupOperModeGlobal));
    }
    
    bcm_port_config_t_init(&port_config);
    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &port_config));
    
    /* IFP Group Configuration and Creation */
    bcm_field_group_config_t_init(&group_config);
    //group_config.ports = port_config.per_pipe[ifp_cfg->pipe];
    //group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_PORT;
    //group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    
    /*
     * Different group modes [Single, Double, Triple or Quad] are possible and
     * availability depends on the chip being used. bcmFieldGroupModeAuto is the
     * default mode and it expands based on the given Qset
     */
    BCM_FIELD_ASET_INIT(actn);
    BCM_FIELD_ASET_ADD(actn, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(actn, bcmFieldActionStatGroup);
    
    group_config.aset= actn;
    
    group_config.mode = bcmFieldGroupModeAuto;
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifySrcMac);
    if( opt_UseRangeGroupSetOfApis ) {
        BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyRangeCheckGroup);
    } else {
        BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyRangeCheck);
    }
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    
    printf("Doing IFP group create\n");
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));
    
    /* IFP Entry Configuration and Creation */
    printf("Doing IFP entry create\n");
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
    printf("Doing IFP qual srcmac\n");
    //BCM_IF_ERROR_RETURN(bcm_field_qualify_SrcMac(unit, entry, ifp_cfg->src_mac,
    //                                                            ifp_cfg->mac_mask));
    printf("Doing IFP qual InPort %d\n", ing_port2);
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, ing_port2, 0xFF));
    
    if( opt_UseRangeGroupSetOfApis ) {
        bcm_range_group_config_t range_group_config;
        
        bcm_range_group_config_t_init(&range_group_config);
        range_group_config.ports = port_config.all;
        BCM_RANGE_ID_BMP_ADD (range_group_config.range_bmp, range_cfg1.range_id);
        if( opt_UseMultipleRanges )
        {
            BCM_RANGE_ID_BMP_ADD (range_group_config.range_bmp, range_cfg2.range_id);
        }
    
        printf("Doing range GROUP create..\n");
        BCM_IF_ERROR_RETURN(bcm_range_group_create (unit, &range_group_config));
        
        uint32 rangeGroupId;
        rangeGroupId = range_group_config.range_group_id;
        printf("Doing IFP qual RANGE\n");
        BCM_IF_ERROR_RETURN(bcm_field_qualify_RangeCheckGroup(unit, entry, rangeGroupId,
                                                                        rangeGroupId));
    } else {
        printf("Doing IFP qual RangeCheck\n", ing_port2);
        BCM_IF_ERROR_RETURN( bcm_field_qualify_RangeCheck(unit, entry, range_cfg1.range_id,
                                                                             opt_UseRangeInvert));
        if( opt_UseMultipleRanges )
        {
            BCM_IF_ERROR_RETURN( bcm_field_qualify_RangeCheck(unit, entry, range_cfg2.range_id,
                                                                              opt_UseRangeInvert));
        }
    }
    
    printf("Doing IFP action add CopyToCpu\n");
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 1, 200));
    
    printf("Doing stat attach..\n");
    const bcm_field_stat_t stats[1] = { bcmFieldStatPackets };
    BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group_config.group, 1, stats, main_ifp_statid));
    BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry, main_ifp_statid));
    
    /* Installing FP Entry to FP TCAM */
    printf("Doing IFP entry INSTALL\n");
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    
    return BCM_E_NONE;
}

int range(int unit)
{
    bcm_error_t rv;

    bcm_port_config_t port_config;
    
        
    bcm_port_config_t_init(&port_config);
    
    printf("Doing port config get..\n");
    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &port_config));
    
    if( 0 )
    {
        int pipe;
        for(pipe=0; pipe<8; ++pipe)    // TH3 has 8 pipes
        {
            if( BCM_PBMP_MEMBER(port_config.per_pipe[pipe], ing_port1) )
            {
                break;
            }
        }
    }
    
    /* Configuring Range Checker */
    rv = configure_rangechecker(unit);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring Range Checker: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Configuring IFP */
    rv = configure_fp(unit, &fp_cfg);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring fp: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


/* 
 * This functions gets the port numbers and sets up ingress and 
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
    int        rv;
    int cpu_port = 0;
    bcm_vlan_t        vid;
    
    printf("Doing create VLANs and add ports to them..\n");
    for(vid=test_vid_range_min; vid<test_vid_range_max; ++vid)
    {
        bcm_pbmp_t     vlan_pbmp;
        bcm_pbmp_t     ut_pbmp;

        BCM_PBMP_CLEAR(vlan_pbmp);
        BCM_PBMP_PORT_ADD(vlan_pbmp, ing_port1);
        BCM_PBMP_PORT_ADD(vlan_pbmp, ing_port2);
        BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port);
        
        BCM_PBMP_CLEAR(ut_pbmp);
        BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid, vlan_pbmp, ut_pbmp));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, 1, vlan_pbmp)); // remove from default vlan
    }
    
    printf("Doing L2 addr add..\n");
    for(vid=test_vid_range_min; vid<test_vid_range_max; ++vid)
    {
        bcm_l2_addr_t l2_addr;
        
        bcm_l2_addr_t_init(&l2_addr, test_dmac, vid);
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = ing_port2;
        
        BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));
    }
    
    if( 0 )
    {
        rv = ingress_port_setup(unit, ing_port1, BCM_PORT_LOOPBACK_MAC);
        if ( rv != BCM_E_NONE ) {
            printf("ingress_port_setup() failed for port %d (Error: %d)\n", ing_port1, rv);
            return rv;
        }

        rv = egress_port_setup(unit, egr_port, BCM_PORT_LOOPBACK_MAC);
        if ( rv != BCM_E_NONE ) {
            printf("egress_port_setup() failed for port %d (Error: %d)\n", egr_port, rv);
            return rv;
        }
    }
    else
    {
        printf("Doing port loopbacks...\n");
        
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port1, BCM_PORT_LOOPBACK_MAC));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ing_port1, BCM_PORT_DISCARD_NONE));
        
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port, BCM_PORT_LOOPBACK_MAC));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egr_port, BCM_PORT_DISCARD_ALL));
    
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port2, BCM_PORT_LOOPBACK_MAC));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ing_port2, BCM_PORT_DISCARD_ALL));
    }
    
    bshell(unit, "pw start report +raw +decode");

    return BCM_E_NONE;
}


/*
 How to verify: It is documented within the function for various packets and options
*/
bcm_error_t verify()
{
    char cmd[1024*2];
    sprintf(cmd, "fp show");
    bbshell(unit, cmd);
    int num_pkt_in_a_burst = 1;
    int i;
    
    /////////////////////////////////////////////////////////////////////////////
    // Sending IPv4 pkt with known dest IP addr
    bcm_vlan_t        vid;
    
    for(vid=test_vid_range_min; vid<=test_vid_range_max; ++vid)
    //for(vid=60; vid<=70; ++vid)
    {
        printf("\n\n\n############  VID=%d   ########\n", vid);
        sprintf(cmd, "tx %d pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X08004500002E0000000040063E8C%08X%08X227D00500000000000000000500000004ACA0000000102030405B0F9EB4F",
                num_pkt_in_a_burst, ing_port1,
                test_dmac[0], test_dmac[1], test_dmac[2],
                test_dmac[3], test_dmac[4], test_dmac[5],
                fp_cfg.src_mac[0], fp_cfg.src_mac[1], fp_cfg.src_mac[2], 
                fp_cfg.src_mac[3], fp_cfg.src_mac[4], fp_cfg.src_mac[5], 
                vid, 0x0A0A0A0B, 0x0C0C0C0D);
        printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 1);
        printf("%s\n", cmd); bshell(unit, cmd);
        
        sprintf(cmd, "fp stat get si=%d t=p", main_ifp_statid);
        printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
    }
    
    sprintf(cmd, "fp stat get si=%d t=p", main_ifp_statid);
    printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
    
    return BCM_E_NONE;
}

bcm_error_t execute()
{
    int rrv;
    
    if( (rrv = test_setup(unit)) != BCM_E_NONE )
    {
        printf("Creating the test setup failed %d\n", rrv);
        return rrv;
    }
    
    if( (rrv = range(unit)) != BCM_E_NONE )
    {
        printf("Configuring RANGE IFP failed with %d\n", rrv);
        return rrv;
    }

    if( (rrv = verify()) != BCM_E_NONE )
    {
        printf("Verify failed with %d\n", rrv);
        return rrv;
    }
    
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC >= 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

