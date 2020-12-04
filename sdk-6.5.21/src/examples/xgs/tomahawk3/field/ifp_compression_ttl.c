/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : IFP Compression
 *
 *  Usage    : BCM.0> cint ifp_compression_ttl.c
 *
 *  config   : field_config.bcm
 *
 *  Log file : ifp_compression_ttl_log.txt
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
 *
 * Summary:
 *  ========
 *      Compression using BCM APIs.
 *      It compresses a range of TTL values into 1 class value. In this way
 *      user will require only 1 entry in IFP to qualify for a range of TTL,
 *      but at the expense of equivalent number of entries in the ExactMatch
 *      stage/table. The following example shows the compression configuration
 *      for TTL in Global mode - entries installed on all the four pipes. The
 *      same can be applied to per pipe mode - entries installed in one of the
 *      four pipes. Use opt_* variables to change the test variant
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
 *          as members. Makes packet with dmac=0xDD to go to ing_port2.
 *       b) Create multiple ExactMatch entries for a range of TTL to match
 *             QSET:   IpTunnelTtl
 *             ASET:   ClassZero -> Set value == compress_cfg.ttl_class_data
 *       c) Create 1 main IFP entry to match on the above class value from
 *             ExactMatch
 *             QSET:   IpTunnelTtlClassZero.
 *                     Match value == compress_cfg.ttl_class_data
 *             Action: CopyToCpu
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==============================
 *       a) Send any IPv4 packet with TTL values changing between ttl_data_from
 *          and ttl_data_to and some packets beyond the range also. You will see
 *          that packets within the TTL range hits the IFP entry. Rest do not.
 *       b) Expected Result:
 *          ================
 *          Final stat count should be 10 (0x0000000A)
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

int unit=0;

bcm_vlan_t test_vid = 100;

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

/* Compression configuration structure */
struct compression_cfg_t{
    uint8 ttl_data_from;        /* IP TTL start value */
    uint8 ttl_data_to;         /* IP TTL end value */
    uint8 ttl_mask;        /* IP TTL Mask */
    int ttl_class_data;       /* Data to assign for TTL class */
    int ttl_class_mask;       /* Data to assign for TTL class mask */
};

/* Set the reguired Compression Configuration here */
compression_cfg_t compress_cfg = {
    0x40,   /* IP TTL start value */
    0x49,   /* IP TTL end value */
    0xFF,   /* IP TTL Mask */
    0x05,      /* Data to assign for TTL class */
    0xFF       /* Data to assign for TTL class mask */
};

/*
 *                      Compression configuration
 *  1. In Global Mode - for each compression qualifier,one group need to created
 *     globally.
 *  2. In Per Pipe Mode - for each compression qualifier,one group need to be
 *     created per pipe.
 *  3. Compression Groups of different qualifiers cannot be clubbed together.
 *  4. Group Created for Compression should consist of respective qualifier,
 *     stage class and action set.
 *  5. bcmFieldActionClassZero Action is applicable for all class types.
 *  6. bcmFieldActionClassOne Action is additionally only applicable for TTL,TOS
 *     and TCP class. (Not applicable for TH3)
 *  7. bcmFieldQualifyStageClass is applicable for Src/Dst Compression, for rest
 *     bcmFieldQualifyStageClassExactMatch is applicable.
 *  8. Mask value is ignored in all class types except Src/Dst Compression.
 */

int
configure_compression(int unit, compression_cfg_t *compression_cfg)
{
    bcm_field_entry_t entry;
    bcm_field_group_config_t group_config;
    bcm_class_t class;
    bcm_field_class_info_t class_info;

    /* Compression Group Configuration and Creation */
    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyIpTunnelTtl);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageClassExactMatch);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionClassZero);
    printf("\nDoing Group Config in EXACTMATCH\n");
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    int ttl_value;
    
    for(ttl_value = compression_cfg->ttl_data_from;
        ttl_value <= compression_cfg->ttl_data_to;
        ttl_value++)
    {
        /* Compression Entry Configuration and Creation */
        printf("Doing Entry create\n");
        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
        printf("Doing Entry qualifier clear\n");
        BCM_IF_ERROR_RETURN(bcm_field_qualify_clear(unit, entry));
        
        printf("Doing qualify IpTunnelTtl TTL Value = %d\n", ttl_value);
        BCM_IF_ERROR_RETURN(bcm_field_qualify_IpTunnelTtl(unit, entry,
                                                        ttl_value,
                                                        compression_cfg->ttl_mask));
        COMPILER_64_SET(class, 0, compression_cfg->ttl_class_data);
        bcm_field_class_info_t_init(&class_info);
        class_info.class_id = class;
        class_info.action = bcmFieldActionClassZero;
        printf("Doing Action class add\n");
        BCM_IF_ERROR_RETURN(bcm_field_action_class_add(unit, entry, &class_info));
        printf("Doing Entry Install in EXACTMATCH\n");
        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    }

    return BCM_E_NONE;
}

int
configure_fp(int unit, compression_cfg_t *compression_cfg)
{
    bcm_field_entry_t entry;
    bcm_field_group_config_t group_config;
    bcm_class_t class_data;
    bcm_class_t class_mask;

    /* IFP Group Configuration and Creation */
    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyIpTunnelTtlClassZero);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    printf("\nDoing Group Config in IFP\n");
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    /* IFP Entry Configuration and Creation */
    printf("Doing Entry Create\n");
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
    COMPILER_64_SET(class_data, 0, compression_cfg->ttl_class_data);
    COMPILER_64_SET(class_mask, 0, compression_cfg->ttl_class_mask);
    printf("Doing qualify TTL Class\n");
    BCM_IF_ERROR_RETURN(bcm_field_qualify_IpTunnelTtlClassZero(unit, entry, class_data,
                                                         class_mask));
    printf("Doing qualify port..\n");
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, ing_port1,
                                                         0xFF));
    printf("Doing action Copy2Cpu\n");
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu,
                                             1, 200));
                                             
    const bcm_field_stat_t stats[2] = { bcmFieldStatPackets, bcmFieldStatBytes };
    int                 fp_statid1 = 1;
    /* Also set up an FP stat counter to count matches */
    BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group_config.group, 2, stats, fp_statid1));
    /* Attach the stat object to the entry */
    BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry, fp_statid1));

    printf("Doing Entry Install in IFP\n");
    /* Installing IFP Entry to IFP TCAM */
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

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
    rv = configure_compression(unit, &compress_cfg);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring Compression: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Configuring IFP */
    rv = configure_fp(unit, &compress_cfg);
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
/*
MAC: ------  MAC Header  ------
MAC: 
MAC: Destination Address : 00 00 00 00 00 DD
MAC: Source Address      : 00 00 00 00 00 CC
MAC: 
VLAN: ------  VLAN Header  ------
VLAN: 
VLAN: Tag Protocol Identifier    = 0x8100
VLAN: Tag Control Information    = 0x0064
VLAN: User Priority              = 0
VLAN: Canonical Format Indicator = 0
VLAN: VLAN Identifier            = 100  (0x064)
VLAN: Type                       = 0x0800 (Ethernet II)
VLAN: 
IP: ------  IP Header  -----------
IP: 
IP: Version                        = 04 (0x04)
IP: Header Length                  = 20 (0x14)
IP: Differentiated Services Field  = 0 (0x00)
IP:                      000000..  = DSCP: Default(0x00)
IP:                      ......0.  = ECN Capable Transport (ECT): 0
IP:                      .......0  = ECN-CE: 0
IP: Type of Service                = 00 (0x00)
IP:                    000.....    = Routine
IP:                    ...0....    = Normal Delay
IP:                    ....0...    = Normal Throughput
IP:                    .....0..    = Normal Reliability
IP:                    ......0.    = Normal Cost
IP:                    .......0    = Reserved
IP: Total Length                   = 46 (0x002E)
IP: Identification                 = 0 (0x0000)
IP: Flags  Bit1  .0.  May  Fragment
IP: Flags  Bit2  ..0  Last  Fragment
IP: Fragment Offset                = 0
IP: Time to Live                   = 64 (0x40)
IP: Protocol                       = TCP
IP: Checksum                       = 0x3E8C
IP: Source Address                 = 20.20.20.22
IP: Destination Address            = 10.10.10.11
IP: 
TCP: ------  TCP Header  -----------
TCP: 
TCP: Source Port            = 0x227D
TCP: Destination Port       = 0x0050
TCP: Sequence Number        = 0x00000000
TCP: Acknowledgement Number = 0x00000000
TCP: Header Length          = 20
TCP: Acknowledge            = FALSE
TCP: Push                   = FALSE
TCP: Reset Connection       = FALSE
TCP: Synchronize            = FALSE
TCP: Finished               = FALSE
TCP: Urgent                 = FALSE
TCP: Window                 = 0x0000
TCP: Checksum               = 0x4ACA
TCP: Urgent Pointer         = 0x0000
TCP: 
*/
    sprintf(cmd, "fp show");
    printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
    
    /////////////////////////////////////////////////////////////////////////////
    // Sending packet with Matching TTL value
    int ttl_value;
    int i = 0;
    
    for(ttl_value = compress_cfg.ttl_data_from;
        ttl_value <= compress_cfg.ttl_data_to;
        ttl_value++)
    {
        printf("\n############   TTL = %d   #######\n", ttl_value);
        printf("Sending packet with Matching TTL value\n");
        sprintf(cmd, "tx 1 pbm=%d data=0000000000DD0000000000CC8100006408004500002E00000000%02X063E8C141414160A0A0A0B227D00500000000000000000500000004ACA0000000102030405B0F9EB4F", ing_port1, ttl_value);
        printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 2);
        printf("%s\n", cmd); bshell(unit, cmd);
        
        sprintf(cmd, "fp stat get si=%d t=p", 1);
        printf("%s\n", cmd); bshell(unit, cmd);
        i++;
    }
    /////////////////////////////////////////////////////////////////////////////
    // Sending packet with Non_Matching TTL value
    if( 1 )
    {
        printf("\n############   TTL = %d   #######\n", ttl_value);
        printf("Sending packet with Non_Matching TTL value\n");
        sprintf(cmd, "tx 1 pbm=%d data=0000000000DD0000000000CC8100006408004500002E00000000%02X063E8C141414160A0A0A0B227D00500000000000000000500000004ACA0000000102030405B0F9EB4F", ing_port1, ttl_value);
        printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 2);
        printf("%s\n", cmd); bshell(unit, cmd);
        
        sprintf(cmd, "fp stat get si=%d t=p", 1);
        printf("%s\n", cmd); bshell(unit, cmd);
    }
    /////////////////////////////////////////////////////////////////////////////
    // One for the original packet match and another for the match
    printf("\n\n\t Final stat count should be 0x%08X\n\n\n", i);
    
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

