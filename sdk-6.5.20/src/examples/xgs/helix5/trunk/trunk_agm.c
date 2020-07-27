/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : Aggregate Monitor
 *
 *  Usage    : BCM.0> cint trunk_agm.c YES 0/1
 *              E.g. cint trunk_agm.c YES 0      <<<<< TRUNK AGM
 *              E.g. cint trunk_agm.c YES 1      <<<<< ECMP AGM
 *
 *  config   : trunk_config.bcm
 *
 *  Log file : trunk_agm.txt
 *              Logs pasted for both the options TRUNK and ECMP
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
 *   1. Create a trunk/ecmp group with 3 port/interface members
 *   2. Send the traffic to egress from this trunk/ecmp group
 *   3. Dump the AGM Stat
 *
 *  Detailed steps done in the CINT script:
 *  ====================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ================================
 *       a) Select set of ingress and egress ports and configure them in
 *          Loopback mode.
 *
 *    2) Step2 - Configuration (Done in example_run_*() ).
 *    ===================================================
 *    A] AGM for Trunk - Configuration (Done in example_run_trunk() ).
 *       a) Create VLAN(100) and add ing_portX.
 *       b) Create Trunk by adding all egr_portX ports into it.
 *       c) Add DEST MAC destined to trunk.
 *
 *    B] AGM for ECMP - Configuration (Done in example_run_ecmp() ).
 *       a) Create VLAN(100) and add ing_portX.
 *       b) Create ECMP by adding all egr_portX ports into an l3 interface.
 *       c) Add a route entry for a DIP
 *
 *    4) Step3 - Verification (Done in verify())
 *    ==============================
 *       a) Send unicast packets from ing_portX with varying parameter of
 *          srcmac. Fixed Dest IP and fixed Dest mac. Vlan = 100.
 *       b) See the load balancing happening. and AGM stats updated.
 *       c) Expected Result:
 *          ================
 *          Dump of agm_stat will show the Monitor stats
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
  ports_pbmp = configP.xe;    // configP.hg;                                /* */
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

int unit=0;

int opt_TestTypeEcmp = 1;

if( ARGC >= 2 ) {
    if( *ARGV[1] == '1' ) {
        opt_TestTypeEcmp = 1;
    } else {
        opt_TestTypeEcmp = 0;
    }
}

print opt_TestTypeEcmp;

bcm_vlan_t test_vid = 100;

    
bcm_ip_t sipv4 = 0x14000002;
bcm_ip_t dipv4 = 0x0A000001;

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

egr_port1 = portGetNextE(unit, egr_port1);
egr_port2 = portGetNextE(unit, egr_port2);
egr_port3 = portGetNextE(unit, egr_port3);
egr_port4 = portGetNextE(unit, egr_port4);
egr_port5 = portGetNextE(unit, egr_port5);

print ing_port1;
print ing_port2;
print egr_port1;
print egr_port2;
print egr_port3;
print egr_port4;
print egr_port5;

int num_trunk_members = 3;  // FIXED. To change, update the array initializers.

bcm_trunk_t tid;

bcm_if_t ecmp_intf;


/* Init trunk configuration for DLB */
int hash_init(int unit)
{
    int rv = 0;
    int arg;

    printf("Doing HASH Initializations...");

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

    printf("DONE\n");
    
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
    printf("Doing trunk group create...");
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

    printf("Doing add ports to trunk group %d ", *tid);
    rv = bcm_trunk_set(unit, *tid, &trunk_info, nport, p_trunk_member);
    if (BCM_FAILURE(rv)) {
        printf ("failed, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf(": Done\n");
    sal_free (p_trunk_member);
    return 0;
}

int example_run_trunk(int unit)
{
    int port_array[num_trunk_members] = {egr_port1, egr_port2, egr_port3};
    
    printf ("Initialize Hashing\n");
    BCM_IF_ERROR_RETURN(hash_init(unit));
    
    printf ("Create trunk group with port %d, %d, %d\n",
            port_array[0], port_array[1], port_array[2]);
    BCM_IF_ERROR_RETURN(trunk_create(unit, port_array, num_trunk_members, BCM_TRUNK_PSC_SRCDSTMAC, &tid));
    
    return BCM_E_NONE;
}


bcm_error_t example_run_ecmp(int unit)
{
    int num_ports = 3;
    bcm_l3_intf_t l3_intf;
    int i;
    bcm_l3_egress_t l3_egress_arr;
    uint8 mac_LS_byte_arr[num_ports] = {2, 3, 4};
    bcm_port_t port_arr[num_ports] = {egr_port1, egr_port2, egr_port3};
    bcm_if_t intf_arr[num_ports];
    bcm_if_t ecmp1_members[num_ports];
    bcm_l3_egress_ecmp_t l3_ecmp1;
    int rv = BCM_E_NONE;

    
    printf ("Initialize Hashing\n");
    BCM_IF_ERROR_RETURN(hash_init(unit));

    /* ECMP creation API requires L3 Egress mode to be set */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));
    

    /* Create eight L3 egress objects */
    for (i = 0; i < num_ports; i++) {

        /* Create a L3 interface */
        bcm_l3_intf_t_init(&l3_intf);
        l3_intf.l3a_flags = BCM_L3_WITH_ID;
        l3_intf.l3a_flags |= BCM_L3_ADD_TO_ARL;
        l3_intf.l3a_intf_id = 1+i;
        l3_intf.l3a_vid = test_vid;
        sal_memcpy(l3_intf.l3a_mac_addr, test_dmac, 6);
        l3_intf.l3a_mac_addr[5] = test_dmac[5] + i;
        print("Doing intf create  %d\n", l3_intf.l3a_intf_id);
        BCM_IF_ERROR_RETURN( bcm_l3_intf_create(unit, &l3_intf));
        print l3_intf.l3a_intf_id;
                
        bcm_l3_egress_t_init(&l3_egress_arr);
        l3_egress_arr.intf = l3_intf.l3a_intf_id;
        sal_memset(l3_egress_arr.mac_addr, 0x00, 6);
        l3_egress_arr.mac_addr[5] = mac_LS_byte_arr[i];
        BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, port_arr[i], &l3_egress_arr.port));
        print("Doing l3 egress create  %d\n", i);
        BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress_arr, &intf_arr[i]));
        print("Done l3 egress create  0x%X\n", intf_arr[i]);
    }

    /* Create ECMP group 1 */
    bcm_l3_egress_ecmp_t_init(&l3_ecmp1);
    //l3_ecmp1.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
    l3_ecmp1.dynamic_mode |= BCM_L3_ECMP_DYNAMIC_MODE_RANDOM;
    l3_ecmp1.dynamic_size = 256;
    ecmp1_members[0] = intf_arr[0];
    ecmp1_members[1] = intf_arr[1];
    ecmp1_members[2] = intf_arr[2];
    printf("Doing l3 egress ecmp create...\n");
    BCM_IF_ERROR_RETURN( bcm_l3_egress_ecmp_create(unit, &l3_ecmp1, num_ports, ecmp1_members));
    print l3_ecmp1.ecmp_intf;

    ecmp_intf = l3_ecmp1.ecmp_intf;

    return rv;
}

bcm_error_t agm_trunk_stats_create(int unit, bcm_trunk_t tid, bcm_switch_agm_info_t *agm, int nport)
{
    int rv = BCM_E_NONE;
    uint32 options = 0;       /* Reserved: This should be 0 always for now */
    //bcm_switch_agm_info_t agm;

    /*Set the switch agm parameter structure*/
    bcm_switch_agm_info_t_init(agm);
    agm->period_interval = bcmSwitchAgmInterval1Second; /* 1s interval */
    agm->period_num      = 0;   /* collection period is 7 (max),
                                                      0 means continue the collection until software disable it */
    agm->agm_type        = bcmSwitchAgmTypeTrunk; /* Forwarding group is a Trunk group */
    agm->num_members     = nport;   /* 3 members in trunk group */
    
    /*Create the agm counter*/
    rv = bcm_switch_agm_create(unit, options, agm);
    if (BCM_FAILURE(rv)) {
        printf("bcm_switch_agm_create returned %d\n", rv);
        return rv;
    }

    /*Attach it to the given trunk tgid*/
    rv = bcm_trunk_agm_attach(unit, tid, agm->agm_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_trunk_agm_attach returned %d\n", rv);
        return rv;
    }

    rv = bcm_switch_agm_enable_set(unit, agm->agm_id, 0);
    if (BCM_FAILURE(rv)) {
        printf("bcm_switch_agm_enable_set returned %d\n", rv);
        return rv;
    }

    rv = bcm_switch_agm_enable_set(unit, agm->agm_id, 1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_switch_agm_enable_set returned %d\n", rv);
        return rv;
    }
    
    return BCM_E_NONE;
}

void agm_ecmp_stats_create(int unit, bcm_if_t l3_ecmp_id, bcm_switch_agm_info_t *agm, int nport)
{
    int rv = BCM_E_NONE;
    uint32 options = 0;       /* Reserved: This should be 0 always for now */
    
    /*Set the switch agm parameter structure*/
    bcm_switch_agm_info_t_init(agm);
    agm->period_interval = bcmSwitchAgmInterval1Second; /* 1s interval */
    agm->period_num      = 0;   /* collection period is 7 (max),
                                                      0 means continue the collection until software disable it */
    agm->agm_type        = bcmSwitchAgmTypeL3Ecmp; /* Forwarding group is a Ecmp Trunk group */
    agm->num_members     = nport;   /* 3 members in ecmp group */
    

    /*Create the agm counter*/
    rv = bcm_switch_agm_create(unit, options, agm);
    if (BCM_FAILURE(rv)) {
        printf("bcm_switch_agm_create returned %d\n", rv);
        return rv;
    }

    /*Attach it to the given trunk tgid*/
    rv = bcm_l3_ecmp_agm_attach(unit, l3_ecmp_id, agm->agm_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_ecmp_agm_attach returned %d\n", rv);
        return rv;
    }

    rv = bcm_switch_agm_enable_set(unit, agm->agm_id, 0);
    if (BCM_FAILURE(rv)) {
        printf("bcm_switch_agm_enable_set returned %d\n", rv);
        return rv;
    }

    rv = bcm_switch_agm_enable_set(unit, agm->agm_id, 1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_switch_agm_enable_set returned %d\n", rv);
        return rv;
    }

    return rv;
}


bcm_error_t agm_stats_get(int unit, bcm_switch_agm_info_t *agm)
{
    int rv = BCM_E_NONE;
    bcm_switch_agm_stat_t agm_stat[agm->num_members];
    
    /*Get the count stats*/
    rv = bcm_switch_agm_stat_get(unit, agm->agm_id, agm->num_members, &agm_stat[0]);
    if (BCM_FAILURE(rv)) {
        printf("bcm_switch_agm_stat_get returned %d\n", rv);
        return rv;
    }
    else {
        printf("Group AGM stats returned:\n");
        print agm_stat;
    }

    /*Clear the counter, reset the agm stat*/
    if( 0 )
    {
        rv = bcm_switch_agm_stat_clear(unit, agm->agm_id);
        if (BCM_FAILURE(rv)) {
            printf("bcm_switch_agm_stat_clear returned %d\n", rv);
            return rv;
        }
    }

    return rv;
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
    
    if( opt_TestTypeEcmp )
    {
        /* 
         * Install the route for DIP, note this is routed out on the port_out
         */
        bcm_l3_route_t route_info;
        
        bcm_l3_route_t_init(&route_info);
        route_info.l3a_flags = BCM_L3_MULTIPATH;
        //route_info.l3a_flags |= BCM_IPMC_COPYTOCPU;
        route_info.l3a_intf = ecmp_intf;
        route_info.l3a_subnet = dipv4;
        route_info.l3a_ip_mask = 0xFFFFFF00;
        printf("Doing L3 route create...\n");
        BCM_IF_ERROR_RETURN(bcm_l3_route_add(unit, &route_info));
    }
    else
    {
        bcm_l2_addr_t l2_addr;
        
        bcm_l2_addr_t_init(&l2_addr, test_dmac, test_vid);
        l2_addr.flags = BCM_L2_STATIC | BCM_L2_TRUNK_MEMBER;
        l2_addr.tgid = tid;
        BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));
    }

    l4srcport = 0x1111;
    l4dstport = 0x2222;

    bcm_switch_agm_info_t agm;

    if( opt_TestTypeEcmp )
    {
        BCM_IF_ERROR_RETURN(agm_ecmp_stats_create(unit, ecmp_intf, &agm, num_trunk_members));
    }
    else
    {
        BCM_IF_ERROR_RETURN(agm_trunk_stats_create(unit, tid, &agm, num_trunk_members));
    }
    
    for(i=0; i<10; ++i)
    {
        printf("\n############  Sending UC packet %d  #######\n", i);
        
        sprintf(cmd, "tx 100 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X08004500002E0000000040065CC8%08X%08X%04X%04X00000000000000005000000058A00000000102030405BD437B28",
        ing_port1,
        test_dmac[0], test_dmac[1], test_dmac[2],
        test_dmac[3], test_dmac[4], test_dmac[5],
        test_smac[0], test_smac[1]+i, test_smac[2]+i,
        test_smac[3], test_smac[4], test_smac[5]+i,
        test_vid, sipv4, dipv4, l4srcport+i, l4dstport+i);
        bbshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 2);
        bbshell(unit, cmd);
        
        bbshell(unit, "show c");

        BCM_IF_ERROR_RETURN(agm_stats_get(unit, &agm));

        sprintf(cmd, "sleep %d", 1);
        bbshell(unit, cmd);
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
    
    if( opt_TestTypeEcmp )
    {
        if( (rrv = example_run_ecmp(unit)) != BCM_E_NONE )
        {
            printf("Configuring example_run_dlb failed with %d\n", rrv);
            return rrv;
        }
    }
    else
    {
        if( (rrv = example_run_trunk(unit)) != BCM_E_NONE )
        {
            printf("Configuring example_run_trunk failed with %d\n", rrv);
            return rrv;
        }
    }
    printf("\n##############   VERIFY   #######################\n\n");
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

