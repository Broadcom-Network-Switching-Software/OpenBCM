/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : EFP Ipv6 Qualifiers
 *
 *  Usage    : BCM.0> cint efp_ipv6.c
 *
 *  config   : field_config.bcm
 *
 *  Log file : efp_ipv6_log.txt
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
 *      Example shows how EFP matching IPv6 packets on certain parameters and dropping at
 *      EFP stage.
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
 *    2) Step2 - Configuration (Done in configure_efp()). 
 *    ===================================================
 *       a) Create a VLAN(100) and add ing_port1, ing_port2 and egr_port
 *          as members. Makes packet to go to ing_port2.
 *       b) Create 1 EFP entry with:
 *            Qualifier 1 : SrcIp6High - Upper 64 bits of 3FFE::1
 *            Qualifier 2 : DstIp6High - Upper 64 bits of 3FFE::2
 *            Qualifier 3 : OutPort - 5 [Egress port]
 *            Qualifier 4 : Ip6NextHeader - 6 [TCP]
 *            Qualifier 5 : Ip6TrafficClass - 0x33
 *            Qualifier 6 : L4Ports - 1 [L4 ports valid bit]
 *            Qualifier 6 : IpType - Ipv6
 *            Action      : Drop
 *
 *            ip6.pkt that will match
 *            -------------
 *            Ethernet header: DA=00:11:11:11:11:11, SA=00:22:22:22:22:22, Type=0x8100
 *            802.1Q:          VLAN=1, Priority=0, CFI=0
 *            IPv6 header:     SIPv6=3FFE:0:0:0:0:0:0:1, DIPv6=3FFE:0:0:0:0:0:0:2
 *                             Ver=6, Traffic Class=0x33
 *            
 *            001111111111 002222222222
 *            8100
 *            0001
 *            86DD
 *            633
 *            55555
 *            0026
 *            06FF
 *            3FFE0000000000000000000000000001
 *            3FFE0000000000000000000000000002
 *            3030313100003232000033335010778857DB0000111111111111111111111111111111111111
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==============================
 *       a) Send IPv6 packet with aforementioned parameters.
 *       c) Expected Result:
 *          ================
 *          Packets sent with matching IPv6 qualifiers will increase the stat in EFP
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
  printf("B_CM.%d> %s\n", unit, str);                                       /* */
  bshell(unit, str);                                                       /* */
}                                                                          /* */
/****************   GENERIC UTILITY FUNCTIONS ENDS  ****************************
 */

int unit=0;

bcm_vlan_t test_vid = 100;

int         fp_statid1 = 1;


bcm_mac_t test_dmac = {0x00, 0x11, 0x11, 0x11, 0x11, 0x11};
bcm_mac_t test_smac = {0x00, 0x22, 0x22, 0x22, 0x22, 0x22};


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


/* Qualifier set for EFP TCAM */
bcm_field_qualify_t efp_qset[] = {
    bcmFieldQualifyStageEgress,
    bcmFieldQualifySrcIp6High,
    bcmFieldQualifyDstIp6High,
    bcmFieldQualifyOutPort,
    bcmFieldQualifyIp6NextHeader,
    bcmFieldQualifyIp6TrafficClass,
    bcmFieldQualifyL4Ports,
    bcmFieldQualifyIpType,
    bcmFieldQualifyTcpControl,
    bcmFieldQualifyIpFrag,
    -1,
};

/* EFP configuration structure */
struct fp_cfg_t{
    bcm_ip6_t src_ip;           /* Src Ipv6 to qualify in EFP */
    bcm_ip6_t src_ip_mask;      /* Ipv6 Mask to qualify in EFP */
    bcm_ip6_t dst_ip;           /* Dst Ipv6 to qualify in EFP */
    bcm_ip6_t dst_ip_mask;      /* Ipv6 Mask to qualify in EFP */
    bcm_port_t outport;         /* Egress port of the packet */
    bcm_port_t outport_mask;    /* Egress port mask of the packet */
    uint8 next_header;          /* L4 header */
    uint8 next_header_mask;     /* L4 header mask */
    uint8 traffic_class;        /* Traffic Class */
    uint8 traffic_class_mask;   /* Traffic Class mask */
    uint8 l4_valid;             /* Valid L4 src port and dst port bit */
    uint8 l4_valid_mask;        /* Valid L4 src port and dst port bit mask */
    bcm_field_IpType_t ip_type; /* Ip type */
};

/* Set the reguired EFP Configuration here */
fp_cfg_t fp_cfg = {
    {0x3F, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}, /* Upper 64 bits of SrcIpv6 3FFE::1 */
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /* Upper 64 bits of SrcIpv6 FFFF:FFFF:FFFF:FFFF:0:0:0:0 */
    {0x3F, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02}, /* Upper 64 bits of DstIpv6 3FFE::2 */
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /* Upper 64 bits of DstIpv6 FFFF:FFFF:FFFF:FFFF:0:0:0:0 */
    egr_port,               /* OutPort */
    0xFF,                   /* OutPort mask - 0xFF */
    0x6,                    /* L4 header - 0x6 [TCP]*/
    0xFF,                   /* L4 header mask - 0xFF */
    0x33,                   /* Traffic Class  - 0x33 */
    0xFF,                   /* Traffic Class mask - 0xFF */
    0x1,                    /* L4 valid bit - 0x1 */
    0xFF,                   /* L4 valid bit mask - 0xFF */
    bcmFieldIpTypeIpv6      /* TpType - Ipv6 */
};

int
configure_efp(int unit, fp_cfg_t *efp_cfg, bcm_field_qualify_t * qset)
{
    bcm_field_group_config_t group_config;
    bcm_field_entry_t entry=1;
    int nstat=1;
    int stat_id=fp_statid1;
    bcm_field_stat_t stat_arr[]={bcmFieldStatPackets};

    /* EFP Group Configuration and Creation */
    bcm_field_group_config_t_init(&group_config);

    BCM_FIELD_QSET_INIT(group_config.qset);
    while (*qset != -1) {
        BCM_FIELD_QSET_ADD(group_config.qset, *qset);
        qset++;
    }
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    /* EFP counter/stat Creation */
    BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group_config.group,
                                                 nstat, &stat_arr, stat_id));
    /* EFP Entry Configuration and Creation */
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_OutPort(unit, entry, efp_cfg->outport,
                                                  efp_cfg->outport_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_IpType(unit, entry, efp_cfg->ip_type));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_DstIp6High(unit, entry, efp_cfg->dst_ip,
                                                     efp_cfg->dst_ip_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_SrcIp6High(unit, entry, efp_cfg->src_ip,
                                                     efp_cfg->src_ip_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_Ip6NextHeader(unit, entry,
                                                        efp_cfg->next_header,
                                                        efp_cfg->next_header_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_Ip6TrafficClass(unit, entry,
                                                          efp_cfg->traffic_class,
                                                          efp_cfg->traffic_class_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_L4Ports(unit, entry, efp_cfg->l4_valid,
                                                  efp_cfg->l4_valid_mask));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
    /* Attaching EFP counter to Entry */
    BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry, stat_id));

    /* Installing EFP Entry to EFP TCAM */
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    return BCM_E_NONE;
}

int efp_stat_get()
{
     int stat_id=fp_statid1;
     uint64 val;

     /*
      * COMPILER_64_HI and COMPILER_64_LO for 64-32 bit conversion
      */

     /* FP stat[Packets] collection */
     BCM_IF_ERROR_RETURN(bcm_field_stat_get(unit, stat_id, bcmFieldStatPackets, &val));
     printf("The value is 0x%x%x packets\n", COMPILER_64_HI(val), COMPILER_64_LO(val));

     return BCM_E_NONE;
}

int efp_ipv6()
{
    bcm_error_t rv;

    /* Configuring EFP */
    rv = configure_efp(unit, &fp_cfg, efp_qset);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring efp: %s.\n",bcm_errmsg(rv));
    }

    return rv;
}


/* 
 * This functions gets the port numbers and sets up ingress and 
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
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
        l2_addr.port = egr_port;
        BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));
    }
    
    if( 1 )
    {
        if (BCM_E_NONE != ingress_port_setup(unit, ing_port1, BCM_PORT_LOOPBACK_MAC)) {
            printf("ingress_port_setup() failed for port %d\n", ing_port1);
            return -1;
        }
    }
    else
    {
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port1, BCM_PORT_LOOPBACK_MAC));
        //BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ing_port1, BCM_PORT_DISCARD_ALL));
    }

    if( 0 )
    {
        if (BCM_E_NONE != egress_port_setup(unit, egr_port, BCM_PORT_LOOPBACK_MAC)) {
            printf("egress_port_setup() failed for port %d\n", egr_port);
            return -1;
        }
    }
    else
    {
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port, BCM_PORT_LOOPBACK_MAC));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egr_port, BCM_PORT_DISCARD_ALL));
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
    
    sprintf(cmd, "fp show");
    bbshell(unit, cmd);
    
    /*
      001111111111
      002222222222
      81000064
      86DD
      633
      55555
      002606
      FF3FFE0000000000000000000000000002
      3FFE0000000000000000000000000001
      CCCC
      DDDD
      0000000000000000500000003CD80000000102030405060708090A0B0C0D0E0F10119327CA34
      
      0011111111110022222222228100006486DD63355555002606FF3FFE00000000000000000000000000023FFE0000000000000000000000000001CCCCDDDD0000000000000000500000003CD80000000102030405060708090A0B0C0D0E0F10119327CA34
    */
    
    uint8 traffic_class        = fp_cfg.traffic_class;
    uint8 ipv6_sip_msbyte    = fp_cfg.src_ip[0];
    uint8 ipv6_dip_msbyte    = fp_cfg.dst_ip[0];
    
    if( 1 )
    {
        printf("\n############  Sending Matching IP   #######\n");
        
        sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X86DD6%02X55555002606FF3FFE00000000000000000000000000023FFE0000000000000000000000000001CCCCDDDD0000000000000000500000003CD80000000102030405060708090A0B0C0D0E0F10119327CA34",
        ing_port1,
        test_dmac[0], test_dmac[1], test_dmac[2],
        test_dmac[3], test_dmac[4], test_dmac[5],
        test_smac[0], test_smac[1], test_smac[2],
        test_smac[3], test_smac[4], test_smac[5],
        test_vid, traffic_class, ipv6_sip_msbyte, ipv6_dip_msbyte);
        bbshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 2);
        bbshell(unit, cmd);
        
        sprintf(cmd, "fp stat get si=%d t=p", fp_statid1);
        bbshell(unit, cmd);
    }
    /////////////////////////////////////////////////////////////////////////////
    if( 1 )
    {
        printf("\n############  Sending UN-Matching IP   #######\n");
        
        sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X86DD6%02X55555002606FF%02XFE0000000000000000000000000002%02XFE0000000000000000000000000001CCCCDDDD0000000000000000500000003CD80000000102030405060708090A0B0C0D0E0F10119327CA34",
        ing_port1,
        test_dmac[0], test_dmac[1], test_dmac[2],
        test_dmac[3], test_dmac[4], test_dmac[5],
        test_smac[0], test_smac[1], test_smac[2],
        test_smac[3], test_smac[4], test_smac[5],
        test_vid, traffic_class, ipv6_sip_msbyte+1, ipv6_dip_msbyte);
        bbshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 2);
        bbshell(unit, cmd);
        
        sprintf(cmd, "fp stat get si=%d t=p", fp_statid1);
        bbshell(unit, cmd);
    }
    /////////////////////////////////////////////////////////////////////////////
    // One for the original packet match and another for the match
    
    printf("\n\nFinal count of stat should be 1\n");
    sprintf(cmd, "fp stat get si=%d t=p", fp_statid1);
    bbshell(unit, cmd);
    
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
    
    if( (rrv = efp_ipv6()) != BCM_E_NONE )
    {
        printf("Configuring efp_ipv6 IFP failed with %d\n", rrv);
        return rrv;
    }
    
    if( (rrv = verify()) != BCM_E_NONE )
    {
        printf("Verify failed with %d\n", rrv);
        return rrv;
    }
    
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

