/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : Class IDs in Field Module
 *
 *  Usage    : BCM.0> cint fp_class_id.c
 *
 *  Config   : BCM56275_A1-PORT.bcm
 *
 *  Log file : fp_class_id_log.txt
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
 *      Cint example to show carrying of class-id from VFP to IFP and then from
 *      IFP to EFP using BCM APIs.
 *
 *      The following example illustrates the 12-tuple fields of OpenFlow being
 *      qualified across VFP,IFP and EFP TCAM each coming up in Single mode. A
 *      stat is attached in the EFP to count the number of packets that matches
 *      the corresponding flow.
 *
 *      VFP Entry
 *      Qualifier 1: InPort                                 - port 35
 *      Qualifier 2: Source Mac Address                     - 0x1
 *      Qualifier 3: Destination Mac Address                - 0x2
 *      Qualifier 4: Ether Type                             - 0x0800
 *      Qualifier 5: Outer Vlan Id                          - 0x5
 *      Qualifier 7: Ip Protocol                            - 0x6 [TCP]
 *      Action     : Assign Upper 10 bits of VFP Class id   - 0x32
 *
 *      IFP Entry
 *      Qualifier 1: Source IP Address                      - 10.10.10.40
 *      Qualifier 2: Destination IP Address                 - 192.168.01.20
 *      Qualifier 3: Source Class Id assined in VFP         - 0x32
 *      Action     : Assign new Class id to qualify in EFP  - 0x64
 *
 *      EFP Entry
 *      Qualifier 1: L4 Source Port                         - 0x1404
 *      Qualifier 2: L4 Destination Port                    - 0x1AE8
 *      Qualifier 3: Ingress Class Id assined in IFP        - 0x64
 *      Action 1   : Assign new Outer Vlan                  - 0x6
 *      Action 2   : Count the packets
 *
 *      class_id.pkt
 *      -------------
 *      Ethernet header: DA=00:00:00:00:00:02, SA=00:00:00:00:00:01, Type=0x8100
 *      802.1Q:          VLAN=5, Priority=2, CFI=0
 *      IPv4 header:     SIP=10.10.10.40, DIP=192.168.1.20
 *      TCP header:      Src Port=5124 Dst Port=6888
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
 *    2) Step2 - Configuration (Done in configure_class_id()). 
 *    ===================================================
 *       a) Create a VLAN(100) and add ing_port1, ing_port2 and egr_port
 *          as members. Makes packet to go to ing_port2.
 *       b) Create VFP Entry, assign class ID for IFP
 *       c) Create IFP Entry to Match that class ID from VFP. Assign a new class id for EFP
 *       d) Create EFP Entry to Match that class ID from IFP.
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==============================
 *       a) Send any packet with Matching attributes
 *       b) Expected Result:
 *          ================
 *          Stat in EFP will increase as classids come cascading from VFP...
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


bcm_mac_t test_dmac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
bcm_mac_t test_smac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
bcm_mac_t mac_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

bcm_vlan_t test_vid = 5;
uint8 vlan_pri = 0x2;

uint16 ether_type = 0x0800;
uint8 ip_protocol = 0x06;   /* TCP */
uint8 dscp = 0x18;
bcm_ip_t sip = 0x0A0A0A28;  // 10.10.10.40
bcm_ip_t dip = 0xC0A80114;  // 192.168.1.20

bcm_l4_port_t l4_src_port = 0x1404;
bcm_l4_port_t l4_dst_port = 0x1AE8;

int         fp_statid1 = 1;



/* Qualifier set for VFP TCAM */
int vfp_qset[] = {
    bcmFieldQualifyStageLookup,
    bcmFieldQualifyInPort,
    bcmFieldQualifySrcMac,
    bcmFieldQualifyDstMac,
    bcmFieldQualifyEtherType,
    bcmFieldQualifyOuterVlanId,
    bcmFieldQualifyOuterVlanPri,
    bcmFieldQualifyIpProtocol,
    bcmFieldQualifyDSCP,
    -1,
};

/* Qualifier set for IFP TCAM */
int ifp_qset[] = {
    bcmFieldQualifyStageIngress,
    bcmFieldQualifySrcIp,
    bcmFieldQualifyDstIp,
    bcmFieldQualifySrcClassField,
    -1,
};

/* Qualifier set for EFP TCAM */
int efp_qset[] = {
    bcmFieldQualifyStageEgress,
    bcmFieldQualifyL4SrcPort,
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyIngressClassField,
    -1,
};

int
configure_vfp(int unit, int * qset, uint32 src_class_id_assign)
{
    bcm_field_entry_t entry ;
    bcm_field_group_config_t group_config;
    bcm_field_aset_t    actn;
    bcm_field_group_config_t_init(&group_config);
    group_config.mode = bcmFieldGroupModeAuto;

    BCM_FIELD_QSET_INIT(group_config.qset);
    while (*qset != -1) {
        BCM_FIELD_QSET_ADD(group_config.qset, *qset);
        qset++;
    }
    BCM_FIELD_ASET_INIT(actn);
    BCM_FIELD_ASET_ADD(actn, bcmFieldActionClassSourceSet);
    BCM_FIELD_ASET_ADD(actn, bcmFieldActionStatGroup);
    group_config.aset= actn;

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, ing_port1, 0xFF));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_SrcMac(unit, entry, test_smac, mac_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_DstMac(unit, entry, test_dmac, mac_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_EtherType(unit, entry, ether_type, 0xFFFF));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_OuterVlanId(unit, entry, test_vid, 0xFFFF));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_OuterVlanPri(unit, entry, vlan_pri, 0x0F));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_IpProtocol(unit, entry, ip_protocol, 0xFF));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_DSCP(unit, entry, dscp, 0xFF));
    
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,
                                             bcmFieldActionClassSourceSet,
                                             src_class_id_assign, 0));
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    return BCM_E_NONE;
}

int
configure_ifp(int unit, int * qset, uint32 vfp_src_cls_id, uint32 ifp_cls_id_assign)
{
    bcm_field_entry_t entry ;
    bcm_field_group_config_t group_config;
    bcm_field_aset_t    actn;
    bcm_field_group_config_t_init(&group_config);
    group_config.mode = bcmFieldGroupModeAuto;

    BCM_FIELD_QSET_INIT(group_config.qset);
    while (*qset != -1) {
        BCM_FIELD_QSET_ADD(group_config.qset, *qset);
        qset++;
    }
    
    BCM_FIELD_ASET_INIT(actn);
    BCM_FIELD_ASET_ADD(actn, bcmFieldActionEgressClassSelect);
    BCM_FIELD_ASET_ADD(actn, bcmFieldActionNewClassId);
    BCM_FIELD_ASET_ADD(actn, bcmFieldActionStatGroup);
    group_config.aset= actn;

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_SrcIp(unit, entry, sip, 0xFFFFFFFF));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_DstIp(unit, entry, dip, 0xFFFFFFFF));
    
    uint32 vfp_src_cls_mask = 0xFFFFFFFF;
    BCM_IF_ERROR_RETURN(bcm_field_qualify_SrcClassField(unit, entry,
                                                        vfp_src_cls_id,
                                                        0xFFFFFFFF));

    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,
                                             bcmFieldActionEgressClassSelect,
                                             BCM_FIELD_EGRESS_CLASS_SELECT_NEW,
                                             0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,
                                             bcmFieldActionNewClassId,
                                             ifp_cls_id_assign,
                                             0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));


    return BCM_E_NONE;
}

int
configure_efp(int unit, int * qset, uint32 ifp_cls_id)
{
    bcm_field_entry_t entry;
    bcm_field_group_config_t group_config;
    bcm_field_aset_t    actn;
    int nstat=1;
    int stat_id=fp_statid1;
    bcm_field_stat_t stat_arr[1]={bcmFieldStatPackets};

    bcm_field_group_config_t_init(&group_config);
    group_config.mode = bcmFieldGroupModeAuto;

    BCM_FIELD_QSET_INIT(group_config.qset);
    while (*qset != -1) {
        BCM_FIELD_QSET_ADD(group_config.qset, *qset);
        qset++;
    }
    
    //BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    
    BCM_FIELD_ASET_INIT(actn);
    BCM_FIELD_ASET_ADD(actn, bcmFieldActionOuterVlanNew);
    BCM_FIELD_ASET_ADD(actn, bcmFieldActionStatGroup);
    group_config.aset= actn;

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group_config.group,
                                                 nstat, &stat_arr, stat_id));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
    
    BCM_IF_ERROR_RETURN(bcm_field_qualify_L4SrcPort(unit, entry, l4_src_port, 0xFFFF));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_L4DstPort(unit, entry, l4_dst_port, 0xFFFF));

    //BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, ing_port1, 0xFF));
    
    BCM_IF_ERROR_RETURN(bcm_field_qualify_IngressClassField(unit, entry, ifp_cls_id,
                                                        0xFFFFFFFF));

    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,
                                             bcmFieldActionOuterVlanNew, 6, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry, stat_id));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));


    return BCM_E_NONE;
}

int configure_class_id()
{
    int unit = 0;
    bcm_error_t rv;
    uint32 vfp_src_class_id = 50; /* Class Id assigned in VFP */
    uint32 ifp_class_id = 100; /* Class Id assigned in IFP */

    /* Configuring VFP */
    rv = configure_vfp(unit, vfp_qset, vfp_src_class_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring vfp: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Configuring IFP */
    rv = configure_ifp(unit, ifp_qset, vfp_src_class_id, ifp_class_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring ifp: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Configuring EFP */
    rv = configure_efp(unit, efp_qset, ifp_class_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring efp: %s.\n",bcm_errmsg(rv));
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

    if( 1 )
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
      000000000002 000000000001
      8100
      4005
      0800
      4518
      002E00000000
      4006
      A4C4
      0A0A0A28
      C0A80114
      1404 1AE8
      000000000000000050000000AB04000000010203FDFB4DDF4333
    */
    
    if( 1 )
    {
        printf("\n############  Sending Matching   #######\n");
        
        sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X%04X45%02X002E0000000040%02XA4C4%08X%08X%04X%04X000000000000000050000000A4FB0000000102030405640D7C14",
        ing_port1,
        test_dmac[0], test_dmac[1], test_dmac[2],
        test_dmac[3], test_dmac[4], test_dmac[5],
        test_smac[0], test_smac[1], test_smac[2],
        test_smac[3], test_smac[4], test_smac[5],
        (vlan_pri << 13) | test_vid,
        ether_type,
        dscp,
        ip_protocol,
        sip, dip, l4_src_port, l4_dst_port );
        bbshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 1);
        bbshell(unit, cmd);
        
        sprintf(cmd, "fp stat get si=%d t=p", fp_statid1);
        bbshell(unit, cmd);
    }
    /////////////////////////////////////////////////////////////////////////////
    if( 1 )
    {
        printf("\n############  Sending MisMatching in VFP   #######\n");
        
        sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X%04X45%02X002E0000000040%02XA4C4%08X%08X%04X%04X000000000000000050000000A4FB0000000102030405640D7C14",
        ing_port1,
        test_dmac[0], test_dmac[1], test_dmac[2],
        test_dmac[3], test_dmac[4], test_dmac[5],
        test_smac[0], test_smac[1], test_smac[2],
        test_smac[3], test_smac[4], test_smac[5],
        (vlan_pri << 13) | test_vid,
        ether_type,
        dscp+1,
        ip_protocol,
        sip, dip, l4_src_port, l4_dst_port );
        bbshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 1);
        bbshell(unit, cmd);
        
        sprintf(cmd, "fp stat get si=%d t=p", fp_statid1);
        bbshell(unit, cmd);
    }
    /////////////////////////////////////////////////////////////////////////////
    if( 1 )
    {
        printf("\n############  Sending MisMatching in IFP   #######\n");
        
        sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X%04X45%02X002E0000000040%02XA4C4%08X%08X%04X%04X000000000000000050000000A4FB0000000102030405640D7C14",
        ing_port1,
        test_dmac[0], test_dmac[1], test_dmac[2],
        test_dmac[3], test_dmac[4], test_dmac[5],
        test_smac[0], test_smac[1], test_smac[2],
        test_smac[3], test_smac[4], test_smac[5],
        (vlan_pri << 13) | test_vid,
        ether_type,
        dscp,
        ip_protocol,
        sip+1, dip, l4_src_port, l4_dst_port );
        bbshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 1);
        bbshell(unit, cmd);
        
        sprintf(cmd, "fp stat get si=%d t=p", fp_statid1);
        bbshell(unit, cmd);
    }
    /////////////////////////////////////////////////////////////////////////////
    if( 1 )
    {
        printf("\n############  Sending MisMatching in EFP   #######\n");
        
        sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X%04X45%02X002E0000000040%02XA4C4%08X%08X%04X%04X000000000000000050000000A4FB0000000102030405640D7C14",
        ing_port1,
        test_dmac[0], test_dmac[1], test_dmac[2],
        test_dmac[3], test_dmac[4], test_dmac[5],
        test_smac[0], test_smac[1], test_smac[2],
        test_smac[3], test_smac[4], test_smac[5],
        (vlan_pri << 13) | test_vid,
        ether_type,
        dscp,
        ip_protocol,
        sip, dip, l4_src_port+1, l4_dst_port );
        bbshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 1);
        bbshell(unit, cmd);
        
        sprintf(cmd, "fp stat get si=%d t=p", fp_statid1);
        bbshell(unit, cmd);
    }
    /////////////////////////////////////////////////////////////////////////////
    // One for the original packet match and another for the match
    
    printf("\n\nFinal count of stat should be 2?\n");   
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
    
    if( (rrv = configure_class_id()) != BCM_E_NONE )
    {
        printf("Configuring configure_class_id failed with %d\n", rrv);
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

