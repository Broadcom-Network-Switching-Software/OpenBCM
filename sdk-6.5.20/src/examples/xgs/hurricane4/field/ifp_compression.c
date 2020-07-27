/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : IFP Compression
 *
 *  Usage    : BCM.0> cint ifp_compression.c
 *
 *  Config   : BCM56275_A1-PORT.bcm
 *
 *  Log file : ifp_compression_log.txt
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
 *  +----------------+                              |
 *                   |                              |
 *                   |                              |
 *                   +------------------------------+
 *
 *
 * Summary:
 *  ========
 *      Compression using BCM APIs.
 *      Example shows how Compression Stage can help reduce number of entries in IFP.
 *      Here 4 qualifiers values require only 2 entries in main IFP.
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
 *    2) Step2 - Configuration (Done in compression()). 
 *    ===================================================
 *       a) Create a VLAN(100) and add ing_port1, ing_port2 and egr_port
 *          as members. Makes packet to go to ing_port2.
 *       b) Create 2 COMPRESSION entries for a range of Ethertypr to match
 *             QSET:   EtherType
 *             ASET:   ClassZero -> Set a value to be matched in IFP
 *       c) Create 2 COMPRESSION entries for a range of L4SrcPort to match
 *             QSET:   L4SrcPort
 *             ASET:   ClassZero -> Set a value to be matched in IFP
 *       d) Create 2 main IFP entry to match on the above class values from EM
 *             QSET:   EtherTypeClass.
 *             QSET:   L4SrcPortClass.
 *                     Match value == Class values set in EM
 *             Action: Stat
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==============================
 *       a) Send any packet with EtherType changing.
         b) Send IPv4 packet with L4SrcPort values changing.
 *       c) Expected Result:
 *          ================
 *          Ethertype and L4srcport within the range set in EM will cause STAT in IFP increase
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

int unit=0;

bcm_vlan_t test_vid = 100;

int         fp_statid1;

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


uint16 test_ethertype0 = 0x0906;
uint16 test_ethertype1 = 0x0907;
uint32 test_ethertype_class = 0x05;

uint16 test_l4srcport0 = 0x8080;
uint16 test_l4srcport1 = 0x8081;
uint32 test_l4srcport_class = 0x06;


int
configure_compression(int unit)
{
    bcm_field_entry_t entry;
    bcm_field_group_config_t group_config;
    bcm_class_t class;
    bcm_field_class_info_t class_info;
    
    /*
        Install 2 groups in COMPRESSION. Each group with 2 entries.
        
        Group1:
            Compresses ethertype
                
        Group2:
            Compresses L4 Source port
    */

    if( 1 )
    {
        bcm_field_group_config_t_init(&group_config);
        BCM_FIELD_QSET_INIT(group_config.qset);
        BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyEtherType);
        BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageClassExactMatch);
        BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionClassZero);
        
        printf("\nCreating ETHERTYPE COMPRESSION Stage Group for ETHERTYPE\n");
        BCM_IF_ERROR_RETURN( bcm_field_group_config_create(unit, &group_config) );
        
        if( 1 )
        {
            printf("Creating ETHERTYPE COMPRESSION stage Entry 1\n");
            BCM_IF_ERROR_RETURN( bcm_field_entry_create (unit, group_config.group, &entry) );
            printf("Creating ETHERTYPE COMPRESSION stage Qualifier\n");
            BCM_IF_ERROR_RETURN( bcm_field_qualify_EtherType (unit, entry, test_ethertype0,
                                                                                    0xFFFF) );
            COMPILER_64_SET(class, 0, test_ethertype_class);
            bcm_field_class_info_t_init(&class_info);
            class_info.class_id = class;
            class_info.action = bcmFieldActionClassZero;
            printf("Creating ETHERTYPE COMPRESSION stage Adding class\n");
            BCM_IF_ERROR_RETURN( bcm_field_action_class_add(unit, entry, &class_info) );
            printf("Creating ETHERTYPE COMPRESSION stage Installing Entry\n");
            BCM_IF_ERROR_RETURN( bcm_field_entry_install(unit, entry) );
        }
        if( 1 )
        {
            printf("Creating ETHERTYPE COMPRESSION stage Entry 2\n");
            BCM_IF_ERROR_RETURN( bcm_field_entry_create (unit, group_config.group, &entry) );
            printf("Creating ETHERTYPE COMPRESSION stage Qualifier\n");
            BCM_IF_ERROR_RETURN( bcm_field_qualify_EtherType (unit, entry, test_ethertype1,
                                                                                        0xFFFF) );
            COMPILER_64_SET(class, 0, test_ethertype_class);
            bcm_field_class_info_t_init(&class_info);
            class_info.class_id = class;
            class_info.action = bcmFieldActionClassZero;
            printf("Creating ETHERTYPE COMPRESSION stage Adding class\n");
            BCM_IF_ERROR_RETURN( bcm_field_action_class_add(unit, entry, &class_info) );
            printf("Creating ETHERTYPE COMPRESSION stage Installing Entry\n");
            BCM_IF_ERROR_RETURN( bcm_field_entry_install(unit, entry) );
        }
    }
    
    if( 1 )
    {
        bcm_field_group_config_t_init(&group_config);
        BCM_FIELD_QSET_INIT(group_config.qset);
        BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyL4SrcPort);
        BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageClassExactMatch);
        //BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionClassOne);
        BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionClassZero);
        
        printf("\nCreating ETHERTYPE COMPRESSION stage Group For L4SRCPORT\n");
        BCM_IF_ERROR_RETURN( bcm_field_group_config_create(unit, &group_config) );
        
        if( 1 )
        {
            printf("Creating ETHERTYPE COMPRESSION stage Entry 1\n");
            BCM_IF_ERROR_RETURN( bcm_field_entry_create (unit, group_config.group, &entry) );
            printf("Creating ETHERTYPE COMPRESSION stage Qualifier\n");
            BCM_IF_ERROR_RETURN( bcm_field_qualify_L4SrcPort (unit, entry, test_l4srcport0,
                                                                                    0xFFFF) );
            COMPILER_64_SET(class, 0, test_l4srcport_class);
            bcm_field_class_info_t_init(&class_info);
            class_info.class_id = class;
            class_info.action = bcmFieldActionClassZero;
            printf("Creating ETHERTYPE COMPRESSION stage Adding class\n");
            BCM_IF_ERROR_RETURN( bcm_field_action_class_add(unit, entry, &class_info) );
            printf("Creating ETHERTYPE COMPRESSION stage Installing Entry\n");
            BCM_IF_ERROR_RETURN( bcm_field_entry_install(unit, entry) );
        }
        if( 1 )
        {
            printf("Creating ETHERTYPE COMPRESSION stage Entry 2\n");
            BCM_IF_ERROR_RETURN( bcm_field_entry_create (unit, group_config.group, &entry) );
            printf("Creating ETHERTYPE COMPRESSION stage Qualifier\n");
            BCM_IF_ERROR_RETURN( bcm_field_qualify_L4SrcPort (unit, entry, test_l4srcport1,
                                                                                    0xFFFF) );
            COMPILER_64_SET(class, 0, test_l4srcport_class);
            bcm_field_class_info_t_init(&class_info);
            class_info.class_id = class;
            class_info.action = bcmFieldActionClassZero;
            printf("Creating ETHERTYPE COMPRESSION stage Adding class\n");
            BCM_IF_ERROR_RETURN( bcm_field_action_class_add(unit, entry, &class_info) );
            printf("Creating ETHERTYPE COMPRESSION stage Installing Entry\n");
            BCM_IF_ERROR_RETURN( bcm_field_entry_install(unit, entry) );
        }
    }
    
    return BCM_E_NONE;
}

int
configure_fp(int unit)
{
    bcm_field_entry_t entry;
    bcm_field_group_config_t group_config;
    bcm_class_t class_data;
    bcm_class_t class_mask;
    const bcm_field_stat_t stats[2] = { bcmFieldStatPackets, bcmFieldStatBytes };

    if( 1 )
    {
        /* IFP Group Configuration and Creation */
        bcm_field_group_config_t_init(&group_config);
        BCM_FIELD_QSET_INIT(group_config.qset);
        BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldStageIngress);
        BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
        BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyEtherTypeClass);
        BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyL4SrcPortClass);
        
        BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionStatGroup);
        
        printf("\nDoing Group Config in IFP\n");
        BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));
        
        fp_statid1 = 0;
        
        if( 1 )
        {
            /* IFP Entry Configuration and Creation */
            printf("Doing Entry Create\n");
            BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
            COMPILER_64_SET(class_data, 0, test_ethertype_class);
            COMPILER_64_SET(class_mask, 0, 0xFFFFFFFF);
            printf("Doing qualify Ethertype Class\n");
            BCM_IF_ERROR_RETURN(bcm_field_qualify_EtherTypeClass(unit, entry, class_data,
                                                                 class_mask));
            BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, ing_port1, 0xFF));
            //printf("Doing action Copy2Cpu\n");
            //BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu,
            //                                         1, 200));
                                                     
            fp_statid1++;
            /* Also set up an FP stat counter to count matches */
            BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group_config.group, 2, stats,
                                                                        fp_statid1));
            /* Attach the stat object to the entry */
            BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry, fp_statid1));

            printf("Doing Entry Install in IFP\n");
            /* Installing IFP Entry to IFP TCAM */
            BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
        }
        
        if( 1 )
        {
            /* IFP Entry Configuration and Creation */
            printf("Doing Entry Create\n");
            BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
            COMPILER_64_SET(class_data, 0, test_l4srcport_class);
            COMPILER_64_SET(class_mask, 0, 0xFFFFFFFF);
            printf("Doing qualify L4SrcPort Class\n");
            BCM_IF_ERROR_RETURN(bcm_field_qualify_L4SrcPortClass(unit, entry, class_data,
                                                                 class_mask));
            BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, ing_port1, 0xFF));
            //printf("Doing action Copy2Cpu\n");
            //BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu,
            //                                         1, 200));
                                                     
            fp_statid1++;
            /* Also set up an FP stat counter to count matches */
            BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group_config.group, 2, stats,
                                                                        fp_statid1));
            /* Attach the stat object to the entry */
            BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry, fp_statid1));

            printf("Doing Entry Install in IFP\n");
            /* Installing IFP Entry to IFP TCAM */
            BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
        }
    }
    
    return BCM_E_NONE;
}

bcm_mac_t test_dmac = {0x00, 0x00, 0x00, 0x00, 0x00, 0xDD};

int compression()
{
    int unit = 0;
    bcm_error_t rv;
    
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
    
    if( 1 )
    {
        bcm_l2_addr_t l2_addr;
        
        bcm_l2_addr_t_init(&l2_addr, test_dmac, test_vid);
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = ing_port2;
        BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));
    }
    /* Configuring Compression */
    rv = configure_compression(unit);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring Compression: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Configuring IFP */
    rv = configure_fp(unit);
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
 How to verify: It is documented within the function for various packets and options
*/
bcm_error_t verify()
{
    char cmd[1024*2];
    
    printf("\n\n");
    sprintf(cmd, "fp show");
    bbshell(unit, cmd);
    printf("\n\n");
    sprintf(cmd, "fp show class");
    bbshell(unit, cmd);
    printf("\n\n");
    
    /////////////////////////////////////////////////////////////////////////////
    int ethertype_value;
    int l4srcport_value;
    int i = 0;
    
    printf("\nSending packet with changing ETHERTYPE value\n");
    for(ethertype_value = test_ethertype0-2;
        ethertype_value <= test_ethertype0+3;
        ethertype_value++)
    {
        printf("\n############   Ethertype = 0x%04X   #######\n", ethertype_value);
        
        sprintf(cmd, "tx 1 pbm=%d data=0000000000DD0000000000CC81000064%04X4500002E0000000040063683151617180B0C0D0E08AE0D050000000000000000500000004FDB00000001020304050C516CDD", ing_port1, ethertype_value);
        bbshell(unit, cmd);
        
        sprintf(cmd, "sleep quiet %d", 2);
        bshell(unit, cmd);
        
        sprintf(cmd, "fp stat get si=%d t=p", 1);
        bbshell(unit, cmd);
        i++;
    }
    /////////////////////////////////////////////////////////////////////////////
    printf("\nSending packet with changing L4 SRC PORT value\n");
    for(l4srcport_value = test_l4srcport0-2;
        l4srcport_value <= test_l4srcport0+3;
        l4srcport_value++)
    {
        printf("\n############   l4srcport = 0x%04X   #######\n", l4srcport_value);
        
        sprintf(cmd, "tx 1 pbm=%d data=0000000000DD0000000000CC8100006408004500002E0000000040063683151617180B0C0D0E%04X0D050000000000000000500000004FDB00000001020304050C516CDD", ing_port1, l4srcport_value);
        bbshell(unit, cmd);
        
        sprintf(cmd, "sleep quiet %d", 2);
        bshell(unit, cmd);
        
        sprintf(cmd, "fp stat get si=%d t=p", 2);
        bbshell(unit, cmd);
        i++;
    }
    /////////////////////////////////////////////////////////////////////////////
    // One for the original packet match and another for the match
    
    printf("\n\nFinal count of stat should be 2 each\n");
    for(i=1; i<=fp_statid1; ++i)
    {
        sprintf(cmd, "fp stat get si=%d t=p", i);
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
    
    if( (rrv = compression()) != BCM_E_NONE )
    {
        printf("Configuring compression IFP failed with %d\n", rrv);
        return rrv;
    }
    
    if( (rrv = verify()) != BCM_E_NONE )
    {
        printf("Verify mirror failed with %d\n", rrv);
        return rrv;
    }
    
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

