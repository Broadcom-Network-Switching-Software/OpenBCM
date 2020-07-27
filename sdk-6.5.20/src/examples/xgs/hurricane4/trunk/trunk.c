/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : Trunk API Demo
 *
 *  Usage    : BCM.0> cint trunk.c [YES] [TestTypeDlb]
 *             TestTypeDlb:
 *                  0: Non DLB Trunk testings
 *                  1: DLB Trunk testings
 *                  
 *              E.g. cint trunk.c YES 0    <<< Trunk
 *              E.g. cint trunk.c YES 1    <<< Trunk with DLB
 *                                   (Hurrcane4 doesn't support DLB.
 *                                    So, the 'Trunk with DLB' test
 *                                    cannot be run on Hurrcane4.)
 *
 *  config   : trunk_config.bcm
 *             Add   dlb_hgt_lag_selection=1
 *
 *  Log file : trunk_log.txt
 *
 *  Test Topology :
 *
 *                   +------------------------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
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
 *   1. Create a trunk group with two memebers
 *   2. Add a port to this trunk group
 *   3. Remove the port from this trunk group
 *   4. Change PSC of the specific trunk group
 *   5. Enable UDF hash on device
 *   6. Create Dynamic Load Balancing trunk group
 *
 *  Detailed steps done in the CINT script:
 *  ====================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ================================
 *       a) Select a set of ingress and egress ports and configure them in
 *          Loopback mode.
 *
 *    2) Step2 - Configuration (Done in example_run_*()). 
 *    ===================================================
 *      CASE1: Trunk (Done in example_run_trunk())
 *       a) Create a VLAN(100) and add ing_portX.
 *       b) Create Trunk by adding all egr_portX ports into it.
 *       c) Add trunk to VLAN 100.
 *
 *      CASE2: DLB (Done in example_run_dlb()). 
 *       a) Create a VLAN(100) and add ing_portX.
 *       b) Create Trunk by adding all egr_portX ports into it.
 *       c) Add trunk to VLAN 100.
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==============================
 *       a) Send any unicast packet from ing_portX with varying parameter.
 *       b) See the load balancing happening.
 *       c) Expected Result:
 *          ================
 *          "show c" should show random egr_portX being selected for every packet egressed
 *          which was in gressed with varying flow parameters.
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
  bcm_pbmp_t ports_pbmp, ports_pbmp_hg;                                    /* */
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
  ports_pbmp = configP.e;       // configP.e;                              /* */
  ports_pbmp_hg = configP.hg;   // configP.hg;                             /* */
  for (i= __last_returned_port; i < BCM_PBMP_PORT_MAX; i++)                /* */
  {                                                                        /* */
    if ( BCM_PBMP_MEMBER(&ports_pbmp_hg, i) )                              /* */
         continue;                                                         /* */
    if ( BCM_PBMP_MEMBER(&ports_pbmp, i) )                                 /* */
    {                                                                      /* */
        __last_returned_port = i+1; // point to a probable next port       /* */
        return i;                                                          /* */
    }                                                                      /* */
  }                                                                        /* */
  printf("ERROR: Not enough ports %d\n", __last_returned_port);            /* */
  exit;                                                                    /* */
}
bcm_port_t __last_returned_port_hg = 1;                                    /* */
// Get next valid HiGig-capable port                                       /* */
bcm_port_t                                                                 /* */
portGetNextH(int unit, bcm_port_t PreferredPort)                           /* */
{                                                                          /* */
  int i=0;                                                                 /* */
  int rv=0;                                                                /* */
  bcm_port_config_t configP;                                               /* */
  bcm_pbmp_t ports_pbmp_hg, ports_pbmp_xe;                                 /* */
  if( PreferredPort != PORT_ANY )                                          /* */
  {                                                                        /* */
    printf("Using preferred port %d\n", PreferredPort);                    /* */
    return PreferredPort;                                                  /* */
  }                                                                        /* */
  rv = bcm_port_config_get(unit, &configP);                                /* */
  if(BCM_FAILURE(rv)) {                                                    /* */
    printf("\nError in retrieving port configuration: %s %d.\n",           /* */
    bcm_errmsg(rv), __last_returned_port_hg);                              /* */
    exit;                                                                  /* */
  }                                                                        /* */
  ports_pbmp_hg = configP.hg;   // configP.hg;                             /* */
  ports_pbmp_xe = configP.xe;   // configP.xe;                             /* */
  for (i= __last_returned_port_hg; i < BCM_PBMP_PORT_MAX; i++)             /* */
  {                                                                        /* */
    if ( BCM_PBMP_MEMBER(&ports_pbmp_hg, i) ||                             /* */
         BCM_PBMP_MEMBER(&ports_pbmp_xe, i) )                              /* */
    {                                                                      /* */
        __last_returned_port_hg = i+1; // point to a probable next port    /* */
        return i;                                                          /* */
    }                                                                      /* */
  }                                                                        /* */
  printf("ERROR: Not enough ports %d\n", __last_returned_port_hg);         /* */
  exit;                                                                    /* */
}
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
  // Due to a bug SDK-144931 related to InPorts, commenting                /* */
  // out below line.                                                       /* */
  //BCM_IF_ERROR_RETURN(bcm_field_qualify_InPorts(unit, entry,             /* */
  //            pbm, pbm_mask));                                           /* */
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

int unit=0;

int opt_TestTypeDlb = 0;

if( ARGC >= 2 ) {
    if( *ARGV[1] == '1' ) {
        opt_TestTypeDlb = 1;
    } else {
        opt_TestTypeDlb = 0;
    }
}

print opt_TestTypeDlb;

bcm_vlan_t test_vid = 100;

int         fp_statid1 = 1;


bcm_mac_t test_dmac = {0x00, 0x11, 0x11, 0x11, 0x11, 0x11};
bcm_mac_t test_smac = {0x00, 0x22, 0x22, 0x22, 0x22, 0x22};


bcm_port_t ing_port1 = PORT_ANY; // Set to a valid port if known already.
bcm_port_t ing_port2 = PORT_ANY;

bcm_port_t egr_port1 = PORT_ANY;
bcm_port_t egr_port2 = PORT_ANY;
bcm_port_t egr_port3 = PORT_ANY;
bcm_port_t egr_port4 = PORT_ANY;
bcm_port_t egr_port5 = PORT_ANY;

/* Populate the ports */
ing_port1 = portGetNextE(unit, ing_port1);
ing_port2 = portGetNextE(unit, ing_port2);

if ( opt_TestTypeDlb ) {
    egr_port1 = portGetNextH(unit, egr_port1);
    egr_port2 = portGetNextH(unit, egr_port2);
    egr_port3 = portGetNextH(unit, egr_port3);
    egr_port4 = portGetNextH(unit, egr_port4);
    egr_port5 = portGetNextH(unit, egr_port5);
}
else {
    egr_port1 = portGetNextE(unit, egr_port1);
    egr_port2 = portGetNextE(unit, egr_port2);
    egr_port3 = portGetNextE(unit, egr_port3);
    egr_port4 = portGetNextE(unit, egr_port4);
    egr_port5 = portGetNextE(unit, egr_port5);
}

print ing_port1;
print ing_port2;
print egr_port1;
print egr_port2;
print egr_port3;
print egr_port4;
print egr_port5;

bcm_gport_t trunk_gport;

bcm_trunk_t tid;


int example_run_trunk()
{
    int port_array[4] = {egr_port1, egr_port2, egr_port3, egr_port4};
    int port5 = egr_port5;

    printf ("\n1. Enable non uc traffic on trunk port\n");
    BCM_IF_ERROR_RETURN(trunk_enable_nouc(0));

    printf ("\n2. Create trunk group with port %d, %d, %d\n",
            port_array[0], port_array[1], port_array[2]);
    BCM_IF_ERROR_RETURN(trunk_create(0, port_array, 4, BCM_TRUNK_PSC_SRCDSTMAC, &tid));
    
    BCM_GPORT_TRUNK_SET(trunk_gport, tid);

    printf ("\n3. Add port %d to trunk group %d\n", port5, tid);
    BCM_IF_ERROR_RETURN(trunk_add_member(0, tid, port5));

    printf ("\n4. Delete port %d from trunk group %d\n", port5, tid);
    BCM_IF_ERROR_RETURN(trunk_delete_member(0, tid, port5));

    printf ("\n5. Update trunk hash psc to SRCIP\n");
    BCM_IF_ERROR_RETURN(trunk_update_psc(0, tid, BCM_TRUNK_PSC_SRCIP));

    
    printf ("\n6. Update trunk hash psc to hash based on IPv4 L4 Ports\n");
    BCM_IF_ERROR_RETURN(trunk_rtag7_src_dst_l4port(0, tid));
    
    //BCM_IF_ERROR_RETURN(trunk_update_psc(unit, tid, BCM_TRUNK_PSC_PORTFLOW));
    
    //printf ("\n7. Enable UDF on device\n");
    //BCM_IF_ERROR_RETURN(trunk_udf_hash_enable(0));

    return BCM_E_NONE;
}


bcm_error_t example_run_dlb()
{
    int nport = 4;
    int stackingport[nport] = {egr_port1, egr_port2, egr_port3, egr_port4};
    //printf("\nBefore running this example, please config port 27~30 as higig port\n");
    BCM_IF_ERROR_RETURN( dlb_trunk_init(0) );
    BCM_IF_ERROR_RETURN( dlb_trunk_create(0, stackingport, nport, &tid) );
    BCM_GPORT_TRUNK_SET(trunk_gport, tid);
    return BCM_E_NONE;
}

/* Enable trunk hash for non-uc traffic */
bcm_error_t trunk_enable_nouc(int unit)
{
    int flags, rv;
    flags = BCM_HASH_CONTROL_TRUNK_NUC_SRC |
            BCM_HASH_CONTROL_TRUNK_NUC_MODPORT |
            BCM_HASH_CONTROL_TRUNK_NUC_DST;
    printf ("Enable non uc traffic trunk ");
    rv = bcm_switch_control_set(unit,bcmSwitchHashControl,flags);
    if (BCM_FAILURE(rv)) {
        printf ("failed, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf(": Done\n");
    return BCM_E_NONE;
}

/* Description: Create trunk group with two ports
 * How to:
 *     Create trunk on port 1 and port 2, hash the traffic base on source
 *     and destination mac address
 *
 *     Cint:
 *     int port_array = {1, 2}
 *     int nport = 2;
 *     int tid;
 *     print trunk_enable_nouc(0);
 *     print trunk_create(0, port_array, nport, BCM_TRUNK_PSC_SRCDSTMAC, &tid);
 */
int trunk_create(int unit, int *port, int nport, int psc, bcm_trunk_t *tid)
{
    int i, rv = 0;
    int flags = 0; // BCM_TRUNK_FLAG_WITH_ID
    bcm_trunk_info_t    trunk_info;
    bcm_trunk_member_t  *p_trunk_member;

    /* trunk_member_tmp is defined for sizeof() calculation
       cint doesn't support sizeof(data_type)
    */
    bcm_trunk_member_t  trunk_member_tmp;

    bcm_trunk_info_t_init(&trunk_info);

    /* sizeof(bcm_trunk_member_t) is 16 */
    p_trunk_member = sal_alloc (sizeof(trunk_member_tmp) * nport, "Trunk Members");

    //printf("Configure psc as BCM_TRUNK_PSC_SRCDSTMAC\n");
    trunk_info.psc        = psc;
    //trunk_info.dlf_index  = BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.dlf_index  = 0;
    //trunk_info.mc_index   = BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.mc_index   = 0;
    //trunk_info.ipmc_index = BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.ipmc_index = 0;

    /* Create trunk group, allocate trunk group id  */
    printf("Create trunk group ");
    rv = bcm_trunk_create(unit, flags, tid);
    if (BCM_FAILURE(rv)) {
        printf ("failed, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf(": Done. Trunk group id: %d\n", *tid);

    /* Add port to trunk group */
    for(i=0; i<nport; i++) {
        bcm_trunk_member_t_init (&p_trunk_member[i]);
        BCM_GPORT_MODPORT_SET(p_trunk_member[i].gport, 0, port[i]);
    }

    printf("Add ports to trunk group %d ", *tid);
    rv = bcm_trunk_set(unit, *tid, &trunk_info, nport, p_trunk_member);
    if (BCM_FAILURE(rv)) {
        printf ("failed, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf(": Done\n");
    sal_free (p_trunk_member);
    return 0;
}

/* Description: Add a trunk member to specific trunk group */
int trunk_add_member(int unit, bcm_trunk_t tid, int port)
{
    int rv;
    bcm_trunk_member_t member_array;
    BCM_GPORT_MODPORT_SET(member_array.gport, 0, port);
    printf("Add port to trunk group %d ", tid);
    rv =  bcm_trunk_member_add(unit, tid, &member_array);
    if (BCM_FAILURE(rv)) {
        printf ("failed, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf(": Done\n");
    return 0;
}

/* Remove a member from trunk group */
int trunk_delete_member(int unit, bcm_trunk_t tid, int port)
{
    int rv;
    int member_max = 128;
    int member_count, i;
    bcm_trunk_info_t    trunk_info;
    bcm_trunk_member_t  member_array[member_max];
    bcm_trunk_member_t  trunk_member;

    BCM_GPORT_MODPORT_SET(trunk_member.gport, 0, port);
    printf("Delete port %d from trunk group %d ", port, tid);
    rv = bcm_trunk_member_delete(unit, tid, &trunk_member);
    if (BCM_FAILURE(rv)) {
        printf ("failed, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf(": Done\n");

    return 0;
}

/* Update PSC of a trunk group */
int trunk_update_psc(int unit, bcm_trunk_t tid, int psc)
{
    int rv, member_count;
    bcm_trunk_info_t trunk_info;
    int member_max = 8;
    bcm_trunk_member_t member_array[member_max];

    printf ("Get trunk group %d info ", tid);
    rv = bcm_trunk_get(unit, tid, &trunk_info, member_max, member_array, &member_count);
    if (BCM_FAILURE(rv))
    {
        printf("failed, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf (": Done\n");
    trunk_info.psc = psc;
    printf ("Set trunk group %d psc as %d ", tid, psc);
    rv = bcm_trunk_set(unit, tid, &trunk_info, member_count, member_array);
    if (BCM_FAILURE(rv))
    {
        printf("failed, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf (": Done\n");
    return 0;
}


/* Description:
 *   Configure RTAG7 as hashing based on L4 ports of IPv4 packet
*/
int trunk_rtag7_src_dst_l4port(int unit, bcm_trunk_t tid)
{
      int rv, flags;

      /* Set trunk group tid psc as RTAG7 */
    rv = trunk_update_psc(unit, tid, BCM_TRUNK_PSC_PORTFLOW);
    BCM_IF_ERROR_RETURN(BCM_FAILURE(rv));

    /* Enable RTAG7 hashing on non-uc traffic */
    printf ("Get Hash control information ");
    rv = bcm_switch_control_get(unit, bcmSwitchHashControl, &flags);
    if (BCM_FAILURE(rv))
    {
        printf("failed, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf (": Done\n");

    flags |= BCM_HASH_CONTROL_TRUNK_NUC_ENHANCE;
    printf ("Enable RTAG7 on non-uc traffic ");
    rv = bcm_switch_control_set(unit, bcmSwitchHashControl, flags);
    if (BCM_FAILURE(rv))
    {
        printf("failed, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf (": Done\n");

    /* Use CRC32LO on Rtag7 hashing */
    printf ("Enable CRC32LO on rtag7 hashing ");
    rv = bcm_switch_control_set(unit, bcmSwitchHashField0Config, BCM_HASH_FIELD_CONFIG_CRC32LO);
    if (BCM_FAILURE(rv))
    {
        printf("failed, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf (": Done\n");

    printf ("Get Hash select control information ");
    rv = bcm_switch_control_get(unit, bcmSwitchHashSelectControl, &flags);
    if (BCM_FAILURE(rv))
    {
        printf("failed, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf (": Done\n");

    printf ("Enable IPv4 hash ");
    flags &= ~BCM_HASH_FIELD0_DISABLE_IP4;
    rv = bcm_switch_control_set(unit, bcmSwitchHashSelectControl, flags);
    if (BCM_FAILURE(rv))
    {
        printf("failed, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf (": Done\n");

    /* Enable src/dest hashing */
    flags =  BCM_HASH_FIELD_DSTL4 | BCM_HASH_FIELD_SRCL4;
    printf ("Enable src and dst L4 port hashing for ipv4");
    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4Field0,
                                BCM_HASH_FIELD_SRCL4 | BCM_HASH_FIELD_DSTL4);
    if (BCM_FAILURE(rv))
    {
        printf("failed, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf (": Done\n");

    printf ("Enable src and dst L4 port hashing for udp/tcp  ");
    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField0,
                                BCM_HASH_FIELD_SRCL4 | BCM_HASH_FIELD_DSTL4);
    if (BCM_FAILURE(rv))
    {
        printf("failed, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf (": Done\n");

    printf ("Enable src and dst L4 port hashing for udp/tcp where dest port equal to src port ");
    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpPortsEqualField0,
                                BCM_HASH_FIELD_SRCL4 | BCM_HASH_FIELD_DSTL4);
    if (BCM_FAILURE(rv))
    {
        printf("failed, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf (": Done\n");

   return 0;
}


/* Description:
 *  Configure UDF hashing based on lowest 16-bit of DMAC and lowest 16-bit of SMAC
 *  To apply UDF hash, enable BCM_TRUNK_PSC_PORTFLOW on the trunk group:
 *      print trunk_update_psc(unit, tid, BCM_TRUNK_PSC_PORTFLOW);
 *
 *  NOTE: UDF hashing is not supported in Hurricane4
 */
int trunk_udf_hash_enable(int unit)
{
    int i, rv;
    int udf_len=2;
    int mask_len=2;
    
    bcm_udf_alloc_hints_t hints;
    bcm_udf_t udf_info;
    bcm_udf_pkt_format_id_t pkt_format_id;
    bcm_udf_pkt_format_options_t options;
    bcm_udf_pkt_format_info_t pkt_format;
    bcm_udf_hash_config_t config;
    int flags;

    printf ("UDF hashing support is start from SDK6.4.4 and 6.3.11\n");
    printf ("Enable CRC32LO on rtag7 hashing ");
    rv = bcm_switch_control_set(unit, bcmSwitchHashField0Config, BCM_HASH_FIELD_CONFIG_CRC32LO);
    if (BCM_FAILURE(rv))
    {
        printf("failed, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf (": Done\n");
    /* Hash field selection, make sure srcport/srcmod are enabled */
    printf ("Enable src mod and src port hashing on IP packet ");
    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4Field0,
                                BCM_HASH_FIELD_SRCMOD | BCM_HASH_FIELD_SRCPORT );
    if (BCM_FAILURE(rv))
    {
        printf("failed in enabling it on IP4, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchHashIP6Field0,
                                BCM_HASH_FIELD_SRCMOD | BCM_HASH_FIELD_SRCPORT );
    if (BCM_FAILURE(rv))
    {
        printf("failed in enabling it on IP6, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf (": Done\n");

    printf ("Get Hash control information ");
    rv = bcm_switch_control_get(unit, bcmSwitchHashControl, &flags);
    if (BCM_FAILURE(rv))
    {
        printf("failed, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf (": Done\n");

    printf ("Configure hash control and enable hash on non-uc traffic ");
    flags |= BCM_HASH_CONTROL_TRUNK_NUC_ENHANCE | BCM_HASH_CONTROL_TRUNK_NUC_SRC |
         BCM_HASH_CONTROL_TRUNK_NUC_DST | BCM_HASH_CONTROL_TRUNK_NUC_MODPORT;
    rv = bcm_switch_control_set(unit, bcmSwitchHashControl, flags);
    if (BCM_FAILURE(rv))
    {
        printf("failed, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf (": Done\n");
    
    if( 0 ) // UDF hashing is not supported in Hurricane4
    {
        /* Two udf section, one is for dst mac, another is for src mac */
        bcm_udf_id_t udf_id[2];

        /* lowest 16 bits of DMAC, the offset from start of packet is 4
         * lowest 16 bits of SMAC, the offset from start of packet is 10
         */
        int udf_start[2] = {4, 10};
        
        /* Enable UDF Hashing */
        printf ("Enable UDF hash ");
        rv = bcm_switch_control_set(unit, bcmSwitchUdfHashEnable, BCM_HASH_FIELD0_ENABLE_UDFHASH);
        if (BCM_FAILURE(rv))
        {
            printf("failed, error code: %s\n", bcm_errmsg(rv));
            return rv;
        }
        printf (": Done\n");

        bcm_udf_pkt_format_info_t_init(&pkt_format);
        printf ("Gnerate packet format to match IP packet ");
        options = 0;
        pkt_format.l2 = BCM_PKT_FORMAT_L2_ANY;
        pkt_format.vlan_tag = BCM_PKT_FORMAT_VLAN_TAG_ANY;
        pkt_format.ethertype = 0x0800;
        pkt_format.ethertype_mask = 0xffff;
        rv = bcm_udf_pkt_format_create(unit, options, &pkt_format, &pkt_format_id);
        if (BCM_FAILURE(rv))
        {
            printf("failed, error code: %s\n", bcm_errmsg(rv));
            return rv;
        }
        printf (": Done. pkt_format_id = %d\n", pkt_format_id);

        /* Create two udf_id to match dmac and smac seperately, and attach it to ip packet format */
        for (i=0; i<2; i++)
        {
            /* Dest mac */
            bcm_udf_alloc_hints_t_init(&hints);
            bcm_udf_t_init(&udf_info);
            hints.flags = BCM_UDF_CREATE_O_UDFHASH;
            udf_info.layer = bcmUdfLayerL2Header;

            /* The resolusion start and width are all bit */
            udf_info.start = udf_start[i] * 8;
            udf_info.width = udf_len * 8;
            printf ("Create UDF ID for dst MAC ");
            rv = bcm_udf_create(unit, &hints, &udf_info, &udf_id[i]);
            if (BCM_FAILURE(rv))
            {
                printf("failed, error code: %s\n", bcm_errmsg(rv));
                return rv;
            }
            printf (": Done. udf_id = %d\n", udf_id[i]);

            /* Attach udf to ip packet */

            printf ("Attach udf %d to packet format %d ", udf_id[i], pkt_format_id);
            rv = bcm_udf_pkt_format_add(unit, udf_id[i], pkt_format_id);
            if (BCM_FAILURE(rv))
            {
                printf("failed, error code: %s\n", bcm_errmsg(rv));
                return rv;
            }
            printf (": Done. \n");

            /* Config hash mask */
            printf("Config 16 bits hash mask for udf %d ", udf_id[i]);
            bcm_udf_hash_config_t_init(&config);
            config.udf_id = udf_id[i];
            config.mask_length = mask_len;
            config.hash_mask[0] = 0xff;
            config.hash_mask[1] = 0xff;
            rv = bcm_udf_hash_config_add(unit, 0, &config);
            if (BCM_FAILURE(rv))
            {
                printf("failed, error code: %s\n", bcm_errmsg(rv));
                return rv;
            }
            printf (": Done. \n");
        }
    }
    return 0;
}


/*
   Description:
       Create DLB (Dynamic Load Balancing) trunk group on higig ports
   For example: Create Higig port (27, 28, 29, 30) as a DLB trunk
   In a stacking system, run cint as below:
        int tid, nport = 4;
        int stackingport[nport] = {27, 28, 29, 30};
        print dlb_trunk_init(0);
        print dlb_trunk_create(0, stackingport, nport, &tid);
   L2 Traffic:
     Inject 4 different known unicast flows (srcmac/destmac are different) to front panel ports
   Check counters on higig port, 4 flows should be able dispatched on 4 higig ports randomly
*/
int dlb_trunk_create(int unit, int *port, int nport, bcm_trunk_t *tid)
{
    int i, rv, my_modid, flags=0;
    bcm_trunk_info_t    trunk_info;
    bcm_trunk_member_t  *p_trunk_member;

    /* Create trunk group, allocate trunk group id  */
    printf("Create trunk group ");

    bcm_trunk_chip_info_t trunk_chip_info;
    rv = bcm_trunk_chip_info_get(unit, &trunk_chip_info);
    if (BCM_FAILURE(rv)) {
        printf ("failed to get trunk_chip_info: %s\n", bcm_errmsg(rv));
        return rv;
    }

    *tid = trunk_chip_info.trunk_fabric_id_min;
    rv = bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, tid);
    if (BCM_FAILURE(rv)) {
        printf ("failed, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Done. \n");

    bcm_trunk_info_t_init(&trunk_info);
    //trunk_info.dlf_index  = BCM_TRUNK_UNSPEC_INDEX;
    //trunk_info.mc_index   = BCM_TRUNK_UNSPEC_INDEX;
    //trunk_info.ipmc_index = BCM_TRUNK_UNSPEC_INDEX;
    
    trunk_info.dlf_index  = 0;
    trunk_info.mc_index   = 0;
    trunk_info.ipmc_index = 0;
    //trunk_info.psc        = BCM_TRUNK_PSC_ROUND_ROBIN; /* DLB mode */
    trunk_info.psc        = BCM_TRUNK_PSC_DYNAMIC;     /* DLB mode */
    trunk_info.dynamic_age  = 16;     /* inactivity duration in usec */
    trunk_info.dynamic_size = 512;    /* Flow Set size */

    /* sizeof(bcm_trunk_member_t) is 20...but go with a higher value */
    p_trunk_member = sal_alloc (32 * nport, "Trunk Members");
    if (!p_trunk_member) {
        printf("Failed to allocate memory. \n");
        return BCM_E_MEMORY;
    }

    printf("Get my modid: ");
    rv = bcm_stk_modid_get(unit, &my_modid);
    if (BCM_FAILURE(rv)) {
        printf ("failed, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("%d\n", my_modid);
    
    for (i = 0; i < nport; i++) {
        bcm_trunk_member_t_init(&p_trunk_member[i]);
        //BCM_GPORT_MODPORT_SET(p_trunk_member[i].gport, my_modid, port[i]);
        BCM_GPORT_LOCAL_SET(p_trunk_member[i].gport, port[i]);
        p_trunk_member[i].dynamic_load_weight = 100;
        p_trunk_member[i].dynamic_scaling_factor = 10;  // {10, 25, 40, 50, 75, 100} indicating 10G
    }
    
    printf("Configure DLB information for trunk %d ", *tid);
    rv = bcm_trunk_set(unit, *tid, &trunk_info, nport, p_trunk_member);
    if (BCM_FAILURE(rv)) {
        printf ("failed, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Done \n");
    
    for (i = 0; i < nport; i++) {
        rv = bcm_trunk_member_status_set(unit, p_trunk_member[i].gport,
                                         BCM_TRUNK_DYNAMIC_MEMBER_FORCE_UP);
        if (BCM_FAILURE(rv)) {
            printf("DLB Trunk member force up set failue! rv: %s, unit: %d, port: %d, gport: %d.\n",
                    bcm_errmsg(rv), unit, port[i], p_trunk_member[i].gport);
            return rv;
        }
    }
    sal_free (p_trunk_member);
    return BCM_E_NONE;
}

/* Init trunk configuration for DLB */
int dlb_trunk_init(int unit)
{
    int rv = 0;
    int arg;

    /* L2 - This section to setup RTAG 7 with L2 header parameters only*/
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashL2Field0,
                                 BCM_HASH_FIELD_SRCMOD | BCM_HASH_FIELD_SRCPORT |
                                 BCM_HASH_FIELD_ETHER_TYPE | BCM_HASH_FIELD_MACDA_LO |
                                 BCM_HASH_FIELD_MACDA_MI | BCM_HASH_FIELD_MACDA_HI |
                                 BCM_HASH_FIELD_MACSA_LO | BCM_HASH_FIELD_MACSA_MI |
                                 BCM_HASH_FIELD_MACSA_HI | BCM_HASH_FIELD_VLAN));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashL2Field1,
                                 BCM_HASH_FIELD_SRCMOD  |  BCM_HASH_FIELD_SRCPORT |
                              BCM_HASH_FIELD_ETHER_TYPE |  BCM_HASH_FIELD_MACDA_LO |
                                 BCM_HASH_FIELD_MACDA_MI | BCM_HASH_FIELD_MACDA_HI |
                                 BCM_HASH_FIELD_MACSA_LO | BCM_HASH_FIELD_MACSA_MI |
                                 BCM_HASH_FIELD_MACSA_HI | BCM_HASH_FIELD_VLAN));

    /* Configure A0 and B0 HASH Selection */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField0Config,
                                               BCM_HASH_FIELD_CONFIG_CRC16XOR8));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField1Config,
                                               BCM_HASH_FIELD_CONFIG_CRC16));

    /* Configure A0 and B0 HASH Seed */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashSeed0, 0x33333333));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashSeed1, 0x55555555));

    /* Enable RTAG7 HASH on Non-unicast traffic */
    BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchHashControl, &arg));
    arg |= BCM_HASH_CONTROL_TRUNK_NUC_ENHANCE;
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashControl, arg));

    /* Disable RTAG7 hash selection on IP4 and IP6 field*/
    BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit,bcmSwitchHashSelectControl, &arg));
    arg |=(BCM_HASH_FIELD0_DISABLE_IP4|
           BCM_HASH_FIELD1_DISABLE_IP4|
           BCM_HASH_FIELD0_DISABLE_IP6|
           BCM_HASH_FIELD1_DISABLE_IP6);
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchHashSelectControl, arg));
    return BCM_E_NONE;
}


/* 
 * This functions gets the port numbers and sets up ingress and 
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
    char cmd[128];
    bcm_pbmp_t     vlan_pbmp;
    bcm_pbmp_t     ut_pbmp;

    if( opt_TestTypeDlb ) {
        sprintf(cmd, "port %d encap=higig2", egr_port1);
        bbshell(unit, cmd);
        sprintf(cmd, "port %d encap=higig2", egr_port2);
        bbshell(unit, cmd);
        sprintf(cmd, "port %d encap=higig2", egr_port3);
        bbshell(unit, cmd);
        sprintf(cmd, "port %d encap=higig2", egr_port4);
        bbshell(unit, cmd);
        sprintf(cmd, "port %d encap=higig2", egr_port5);
        bbshell(unit, cmd);
    }
    
    BCM_PBMP_CLEAR(vlan_pbmp);
    BCM_PBMP_PORT_ADD(vlan_pbmp, ing_port1);
    BCM_PBMP_PORT_ADD(vlan_pbmp, ing_port2);
    BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port1);
    BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port2);
    BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port3);
    BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port4);
    BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port5);

    BCM_PBMP_CLEAR(ut_pbmp);
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, test_vid));
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, test_vid, vlan_pbmp, ut_pbmp));
    BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, 1, vlan_pbmp)); // remove from default vlan

    //////////////////////////
    
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port1, BCM_PORT_LOOPBACK_MAC));
    //BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ing_port1, BCM_PORT_DISCARD_ALL));

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port1, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egr_port1, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port2, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egr_port2, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port3, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egr_port3, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port4, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egr_port4, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port5, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egr_port5, BCM_PORT_DISCARD_ALL));


    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port2, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ing_port2, BCM_PORT_DISCARD_ALL));

    bshell(unit, "pw start report +raw +decode");

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
    
    //sprintf(cmd, "fp show");
    //bbshell(unit, cmd);
    
    uint8 sipv4_lsb;
    uint8 dipv4_lsb;
    
    if( 1 )
    {
        bcm_l2_addr_t l2_addr;
        
        bcm_l2_addr_t_init(&l2_addr, test_dmac, test_vid);
        l2_addr.flags = BCM_L2_STATIC | BCM_L2_TRUNK_MEMBER;
        l2_addr.tgid = tid;
        //l2_addr.port = trunk_gport;
        BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));
    }
    
    sipv4_lsb = 0x02;
    dipv4_lsb = 0x01;
    l4srcport = 0x1111;
    l4dstport = 0x2222;
    
    for(i=0; i<10; ++i)
    {
        printf("\n############  Sending UC packet %d  #######\n", i);
        
        sprintf(cmd, "tx 100 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X08004500002E0000000040065CC8140000%02X0A0000%02X%04X%04X00000000000000005000000058A00000000102030405BD437B28",
        ing_port1,
        test_dmac[0], test_dmac[1], test_dmac[2],
        test_dmac[3], test_dmac[4], test_dmac[5],
        test_smac[0], test_smac[1]+i, test_smac[2]+i,
        test_smac[3], test_smac[4], test_smac[5],
        test_vid, sipv4_lsb+i, dipv4_lsb+i, l4srcport+i, l4dstport+i);
        bbshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 2);
        bbshell(unit, cmd);
        
        bbshell(unit, "show c");
    }
    
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
    if( opt_TestTypeDlb )
    {
        if( (rrv = example_run_dlb()) != BCM_E_NONE )
        {
            printf("Configuring example_run_dlb failed with %d\n", rrv);
            return rrv;
        }
    }
    else
    {
        if( (rrv = example_run_trunk()) != BCM_E_NONE )
        {
            printf("Configuring example_run_trunk failed with %d\n", rrv);
            return rrv;
        }
    }
    
    if( (rrv = verify()) != BCM_E_NONE )
    {
        printf("Verify failed with %d\n", rrv);
        return rrv;
    }
    
    return BCM_E_NONE;
}

const char *auto_execute = ARGV[0];
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}


