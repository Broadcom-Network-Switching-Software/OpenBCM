/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : Usage of HINT APIs in Tomahawk style FP
 *
 *  Usage    : BCM.0> cint hints.c
 *
 *  config   : field_config.bcm
 *
 *  Log file : hints_log.txt
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
 *      Cint example to show configuration of Tomahawk IFP using BCM APIs.
 *      Hints are the indications/suggestions given during the Field Group
 *      Creation.There are currently four types of hints in SDK across different
 *      devices
 *      - bcmFieldHintTypeCompression [For Auto Compression]
 *      - bcmFieldHintTypeExtraction [For Qualifier Extraction]
 *      - bcmFieldHintTypeGroupAutoExpansion [For Group Auto Expansion]
 *      - bcmFieldHintTypeExactMatch [For Exact Match]
 *
 *      The following example is for hint type Extraction[bcmFieldHintTypeExtraction]
 *      for Tomahawk. It shows extracting the user specified number of bits in
 *      Global mode - entries installed on all the four pipes [legacy mode]. The
 *      same can be applied in pipe local mode - entries installed in one of the
 *      four pipes.
 *
 *      IFP Entry
 *      Qualifier 1: SrcIp6 - MSB 16 bits + LSB 64 bits
 *      Qualifier 2: DstMac - LSB 23 bits
 *      Action     : CopyToCpu
 *
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
 *    2) Step2 - Configuration (Done in hints()). 
 *    ===================================================
 *       a) Create a VLAN(100) and add ing_port1, ing_port2 and egr_port
 *          as members. Makes packet to go to ing_port2.
 *       b) Create Hint for the bits to be extracted.
 *       c) Associate that hint with the IFP entry to be created.
 *       d) Create IFP entry with STAT.
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==============================
 *       a) Send IPv6 packet with Matching attributes. Entry SHOULD hit.
 *       b) Send IPv6 packet with Miss Matching Src Ip in the Non-Extracted area. Entry SHOULD hit.
 *       c) Send IPv6 packet with Miss Matching Src Ip in the Extracted area. Entry should NOT hit.
 *       d) Send IPv6 packet with Miss Matching DstMac in the Non-Extracted area. Entry SHOULD hit.
 *       e) Send IPv6 packet with Miss Matching DstMac in the Extracted area. Entry should NOT hit.
 *       f) Expected Result:
 *          ================
 *          Final count will be 3 for the case a, b and d
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

bcm_vlan_t test_vid = 100;

int         fp_statid1 = 1;

int num_hints = 3;
bcm_field_hintid_t hint_id;

/* Hint configuration structure */
struct hint_cfg_t{
    bcm_field_hint_type_t hint_type;    /* Hint Type */
    bcm_field_qualify_t qual;           /* Qualifier */
    int start_bit;                      /* Start bit for Hint Extraction */
    int end_bit;                        /* End bit for Hint Extraction */
};

/* Set the reguired Hint Configurations here for specified num_hints */
hint_cfg_t hint_cfg[num_hints] = {
    /* MSB 16 bits of Source IpV6 */
    {
        bcmFieldHintTypeExtraction,     /* Hint Type */
        bcmFieldQualifySrcIp6,          /* Qualifier */
        112,                            /* Start bit for Extraction - bit 112 */
        127,                            /* End bit for Extraction - bit 127 */
    },
    /* LSB 64 bits of Source IpV6 */
    {
        bcmFieldHintTypeExtraction,
        bcmFieldQualifySrcIp6,
        0,
        63,
    },
    /* LSB 23 bits of Destination Mac */
    {
        bcmFieldHintTypeExtraction,
        bcmFieldQualifyDstMac,
        0,
        22,
    }
};

/*
 * Only the bits specified in hints can be provided for configuring FP TCAM [SrcIp6]
 * or all the qualifier bits [DstMac] can also be provided to configure FP TCAM
 */
bcm_ip6_t src_ip6 = {0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x02, 0x01, 0x02 , 0xFF, 0xFE,0x03, 0x04, 0x05};
 // src ip mask
//bcm_ip6_t src_ip6_mask = {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//                          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
bcm_ip6_t src_ip6_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
 // dest mac
bcm_mac_t test_dmac = {0x01, 0x00, 0x5E, 0x7D, 0x02, 0x02};
 // dest mac mask
bcm_mac_t test_dmac_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

int
configure_hints(int unit, hint_cfg_t *hints_cfg)
{
    int i;
    bcm_error_t rv;

    for(i=0; i<num_hints; i++) {
        rv = configure_hint(unit, hints_cfg);
        if(BCM_FAILURE(rv)) {
            printf("\nError in configuring hints %s.\n",bcm_errmsg(rv));
        }

        hints_cfg++;
    }

    return BCM_E_NONE;
}

int
configure_hint(int unit, hint_cfg_t *hints_cfg)
{
    bcm_field_hint_t hint;

    /* configuring hint type, number of bits and the qualifier */
    bcm_field_hint_t_init(&hint);
    hint.hint_type = hints_cfg->hint_type;
    hint.qual = hints_cfg->qual;
    hint.start_bit = hints_cfg->start_bit;
    hint.end_bit = hints_cfg->end_bit;

    /* Associating the above configured hints to hint id */
    BCM_IF_ERROR_RETURN(bcm_field_hints_add(unit, hint_id, &hint));

    return BCM_E_NONE;
}

int
configure_fp(int unit)
{
    bcm_field_entry_t entry;
    bcm_field_group_config_t group_config;
    bcm_field_aset_t    actn;
    bcm_field_stat_t stat_arr[1]={bcmFieldStatPackets};

    /* IFP Group Configuration and Creation */
    bcm_field_group_config_t_init(&group_config);
    group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE;

    /* Associating hints wth IFP Group */
    group_config.hintid = hint_id;

     /*
     * Different group modes [Single, Double, Triple or Quad] are possible and
     * availability depends on the chip being used. bcmFieldGroupModeAuto is the
     * default mode and it expands based on the given Qset
     */

    group_config.mode = bcmFieldGroupModeAuto;
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifySrcIp6);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyDstMac);
    
    BCM_FIELD_ASET_INIT(actn);
    BCM_FIELD_ASET_ADD(actn, bcmFieldActionDrop);
    BCM_FIELD_ASET_ADD(actn, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(actn, bcmFieldActionStatGroup);
    group_config.aset= actn;
    
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));
    
    BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group_config.group,
                                             1, &stat_arr, fp_statid1));
    
                                             /* IFP Entry Configuration and Creation */
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_SrcIp6(unit, entry, src_ip6,
                                                 src_ip6_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_DstMac(unit, entry, test_dmac,
                                                 test_dmac_mask));
    //BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu,
    //                                         1, 1));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop,
                                             0, 0));
    
    BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry, fp_statid1));
    
    /* Installing FP Entry to FP TCAM */
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    return BCM_E_NONE;
}

int hints()
{
    int unit = 0;
    bcm_error_t rv;

    /* Creating hint id to associate with IFP Group */
    BCM_IF_ERROR_RETURN(bcm_field_hints_create(unit, &hint_id));

    /* Configuring Hints */
    rv = configure_hints(unit, &hint_cfg);
    if(BCM_FAILURE(rv)) {
        printf("\nError in creating Hints %s.\n",bcm_errmsg(rv));
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
      01005E7D0202 000102030405
 *      8100
 *      0001
 *      86DD
 *      603
 *      00000
 *      0006
 *      3BFF
 *      FE80000000000000020102FFFE030405
 *      35555555666666667777777788888888
 *      00010203040523FA37E3
    */
    
    if( 1 )
    {
        printf("\n############  Sending Matching IP   #######\n");
        
        sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X0000000000CC8100%04X86DD60300000002606FF%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X3555555566666666777777778888888811112222000000000000000050000000D74D0000000102030405060708090A0B0C0D0E0F10111681294E",
        ing_port1,
        test_dmac[0], test_dmac[1], test_dmac[2],
        test_dmac[3], test_dmac[4], test_dmac[5], test_vid,
        src_ip6[0],  src_ip6[1],  src_ip6[2],  src_ip6[3],
        src_ip6[4],  src_ip6[5],  src_ip6[6],  src_ip6[7],
        src_ip6[8],  src_ip6[9],  src_ip6[10], src_ip6[11],
        src_ip6[12], src_ip6[13], src_ip6[14], src_ip6[15]);
        bbshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 2);
        bbshell(unit, cmd);
        
        sprintf(cmd, "fp stat get si=%d t=p", fp_statid1);
        bbshell(unit, cmd);
    }
    
    if( 1 )
    {
        printf("\n############  Sending Mis-Matching IP in non-hinted area  #######\n");
        
        sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X0000000000CC8100%04X86DD60300000002606FF%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X3555555566666666777777778888888811112222000000000000000050000000D74D0000000102030405060708090A0B0C0D0E0F10111681294E",
        ing_port1,
        test_dmac[0], test_dmac[1], test_dmac[2],
        test_dmac[3], test_dmac[4], test_dmac[5], test_vid,
        src_ip6[0],  src_ip6[1],  src_ip6[2],  src_ip6[3],
        src_ip6[4],  src_ip6[5]+1,  src_ip6[6],  src_ip6[7],
        src_ip6[8],  src_ip6[9],  src_ip6[10], src_ip6[11],
        src_ip6[12], src_ip6[13], src_ip6[14], src_ip6[15]);
        bbshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 2);
        bbshell(unit, cmd);
        
        sprintf(cmd, "fp stat get si=%d t=p", fp_statid1);
        bbshell(unit, cmd);
    }
    if( 1 )
    {
        printf("\n############  Sending Mis-Matching IP in hinted area  #######\n");
        
        sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X0000000000CC8100%04X86DD60300000002606FF%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X3555555566666666777777778888888811112222000000000000000050000000D74D0000000102030405060708090A0B0C0D0E0F10111681294E",
        ing_port1,
        test_dmac[0], test_dmac[1], test_dmac[2],
        test_dmac[3], test_dmac[4], test_dmac[5], test_vid,
        src_ip6[0],  src_ip6[1],  src_ip6[2],  src_ip6[3],
        src_ip6[4],  src_ip6[5],  src_ip6[6],  src_ip6[7],
        src_ip6[8],  src_ip6[9],  src_ip6[10], src_ip6[11],
        src_ip6[12], src_ip6[13], src_ip6[14], src_ip6[15]+1);
        bbshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 2);
        bbshell(unit, cmd);
        
        sprintf(cmd, "fp stat get si=%d t=p", fp_statid1);
        bbshell(unit, cmd);
    }
    if( 1 )
    {
        printf("\n############  Sending Mis-Matching DMAC in non-hinted area  #######\n");
        
        sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X0000000000CC8100%04X86DD60300000002606FF%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X3555555566666666777777778888888811112222000000000000000050000000D74D0000000102030405060708090A0B0C0D0E0F10111681294E",
        ing_port1,
        test_dmac[0], test_dmac[1], test_dmac[2],
        test_dmac[3], test_dmac[4]+1, test_dmac[5], test_vid,
        src_ip6[0],  src_ip6[1],  src_ip6[2],  src_ip6[3],
        src_ip6[4],  src_ip6[5],  src_ip6[6],  src_ip6[7],
        src_ip6[8],  src_ip6[9],  src_ip6[10], src_ip6[11],
        src_ip6[12], src_ip6[13], src_ip6[14], src_ip6[15]);
        bbshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 2);
        bbshell(unit, cmd);
        
        sprintf(cmd, "fp stat get si=%d t=p", fp_statid1);
        bbshell(unit, cmd);
    }
    if( 1 )
    {
        printf("\n############  Sending Mis-Matching DMAC in hinted area  #######\n");
        
        sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X0000000000CC8100%04X86DD60300000002606FF%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X3555555566666666777777778888888811112222000000000000000050000000D74D0000000102030405060708090A0B0C0D0E0F10111681294E",
        ing_port1,
        test_dmac[0]+1, test_dmac[1], test_dmac[2],
        test_dmac[3], test_dmac[4], test_dmac[5], test_vid,
        src_ip6[0],  src_ip6[1],  src_ip6[2],  src_ip6[3],
        src_ip6[4],  src_ip6[5],  src_ip6[6],  src_ip6[7],
        src_ip6[8],  src_ip6[9],  src_ip6[10], src_ip6[11],
        src_ip6[12], src_ip6[13], src_ip6[14], src_ip6[15]);
        bbshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 2);
        bbshell(unit, cmd);
        
        sprintf(cmd, "fp stat get si=%d t=p", fp_statid1);
        bbshell(unit, cmd);
    }
    
    printf("\n\nFinal count of stat should be 3\n");
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
    
    if( (rrv = hints()) != BCM_E_NONE )
    {
        printf("Configuring hints failed with %d\n", rrv);
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


