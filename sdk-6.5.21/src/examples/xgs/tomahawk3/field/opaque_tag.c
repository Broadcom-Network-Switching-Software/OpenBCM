/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : Opaque Tag
 *
 *  Usage    : BCM.0> cint opaque_tag.c
 *
 *  config   : field_config.bcm
 *
 *  Log file : opaque_tag_log.txt
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
 *    Opaque TAG usage demonstration using BCM APIs.
 *    TH3 does not support double tagged packets. If a double tagged
 *    packets enters the pipeline it treats everything beyong 1st tag
 *    as data. E.g. a double tagged L3 packet will not be recognised as
 *    an IP packet. This CINT excercises the processing of opaque tag in
 *    TH3. To do this we are sending double tag packet to qualify on
 *    fields beyond the double tag field. You have to use Opaque TAG
 *    to successfully parse the fields beyond 2nd tag.
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
 *    2) Step2 - Configuration (Done in ConfigureOpaqueTagFp()).
 *    ===================================================
 *       a) Create entry
 *             QUAL    : OpaqueTagHigh and OpaqueTagLow
 *                       OpaqueTag      == 0x91000064
 *                       OpaqueTag_Extn == 0xAABBCCDD
 *             ACTION  : CopyToCpu
 *             STAT    : Counter with STAT ID = 1
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==============================
 *       a) Send packet with
 *             a) Matched OpaqueTag,   Unmatched OpaqueTag_Extn
 *             b) Unmatched OpaqueTag, Unmatched OpaqueTag_Extn
 *             c) Matched OpaqueTag,   Matched OpaqueTag_Extn
 *             d) Unmatched OpaqueTag, Matched OpaqueTag_Extn
 *       b) Expected Result:
 *          ================
 *          The final stat should be 1 because as per the IFP entry, both tag + extn parts
 *          should match. (opt_OpaqueTagLen = 2)
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
    printf("Executing \" %s \" \n", str);                                     /* */
    bshell(unit, str);                                                     /* */
}                                                                          /* */
/****************   GENERIC UTILITY FUNCTIONS ENDS  ****************************
 */

/********   Test variants **************/
//int opt_OpaqueTagLen = 1; // 1 means 4 bytes of TAG to match: 0x91000064
int opt_OpaqueTagLen = 2; // 2 means 4+4 bytes of TAG to match: 0x91000064AABBCCDD
/****************************************/

print opt_OpaqueTagLen;

char cmd[2048];

int unit=0;

bcm_port_t ing_port1 = PORT_ANY; // Set to a valid port if known already.
bcm_port_t ing_port2 = PORT_ANY;
bcm_port_t egr_port = PORT_ANY;

bcm_port_t mtp_port = PORT_ANY; // Set to a valid port if known already.

/* Populate the ports */
ing_port1 = portGetNextE(unit, ing_port1);
ing_port2 = portGetNextE(unit, ing_port2);

egr_port = portGetNextE(unit, egr_port);

bcm_vlan_t  test_vid = 0x64; // 100
bcm_vlan_t  inner_vid = 0x65; // 101
bcm_vlan_t  inner_vid_unmatched = 0x66; // 102

uint16  TestTag = 0x9100;
uint32  OpaqueTag                = (TestTag << 16) | inner_vid;
uint32  OpaqueTag_unmatched      = (TestTag << 16) | inner_vid_unmatched;
uint32  OpaqueTag_Extn           = 0xAABBCCDD;
uint32  OpaqueTag_Extn_unmatched = 0xAABBCCEE;

int 		main_ifp_statid = 1;

bcm_mac_t test_dmac = {0x00, 0x00, 0x00, 0x00, 0x00, 0xDD};

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

// http://cspprod-jira-ntsw.broadcom.com/browse/ARCHTH3-441

bcm_error_t ConfigureOpaqueTagFp()
{
    bcm_switch_opaque_tag_params_t opaque_tag_config;
    
    if( 1 )
    {
        bcm_l2_addr_t l2_addr;
        
        bcm_l2_addr_t_init(&l2_addr, test_dmac, test_vid);
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = ing_port2;
        BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));
    }
    
    bcm_switch_opaque_tag_config_t_init(&opaque_tag_config);
    opaque_tag_config.ethertype = TestTag;    // TH3 will find 9100 at place of ethertype.
                                            // as DualTag is not valid for TH3.
    opaque_tag_config.tag_size = 1;//opt_OpaqueTagLen;    // In units of 4 bytes
    opaque_tag_config.tag_type = 2;    // Not using UDF for this test.
                                    // If used, the UDF flag applicable is BCMI_XGS4_UDF_CTRL_OPAQUE_TAG_TYPE
    opaque_tag_config.valid = 1;
    
    printf("Doing Opaque tag config..\n");
    BCM_IF_ERROR_RETURN(bcm_switch_opaque_tag_config_set(unit, 0, &opaque_tag_config));
    
    sprintf(cmd, "g chg OPAQUE_TAG_CONFIG_0");
    bbshell(unit, cmd);

    bcm_field_qset_t qset;
    
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    if( opt_OpaqueTagLen == 1 )
    {
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOpaqueTagLow);
    }
    else if( opt_OpaqueTagLen == 2 ) // if tag len 8 bytes, add HIGH also.
    {
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOpaqueTagHigh);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOpaqueTagLow);
    }
    
    bcm_field_group_t group = 1;
    bcm_field_entry_t entry = 1;

    printf("Doing FP Group create..\n");
    BCM_IF_ERROR_RETURN(bcm_field_group_create_id(unit, qset, entry, group));

    printf("Doing FP Entry create..\n");
    BCM_IF_ERROR_RETURN(bcm_field_entry_create_id(unit,group, entry));

    if( opt_OpaqueTagLen == 1 )
    {
        printf("Doing Opaque LOW set..\n");
        BCM_IF_ERROR_RETURN(bcm_field_qualify_OpaqueTagLow(unit, entry, OpaqueTag, 0xFFFFFFFF));
    }
    else if( opt_OpaqueTagLen == 2 )
    {
        printf("Doing Opaque HIGH set..\n");
        BCM_IF_ERROR_RETURN(bcm_field_qualify_OpaqueTagHigh(unit, entry, OpaqueTag, 0xFFFFFFFF));
        BCM_IF_ERROR_RETURN(bcm_field_qualify_OpaqueTagLow (unit, entry, OpaqueTag_Extn, 0xFFFFFFFF));
    }
    printf("Doing FP Action add...\n");
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

    const bcm_field_stat_t stats[2] = { bcmFieldStatPackets, bcmFieldStatBytes };

    /* Also set up an FP stat counter to count matches */
    BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group, 2, stats, main_ifp_statid));
    /* Attach the stat object to the entry */
    BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry, main_ifp_statid));

    printf("Doing FP Entry Install\n");
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    
    return BCM_E_NONE;
}

/*
 How to verify: It is documented within the function for various packets and options
*/
bcm_error_t verify()
{
    // >>>>> MATCHING TAG    for opt_OpaqueTagLen=1
    // >>>>> UN_MATCHING TAG for opt_OpaqueTagLen=2
    // TAG1: 0x81000064, TAG2: 0x91000065   Extn: 0xAABBCCEE
    sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X0000000000CC8100%04X%08X%08Xc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5",
        ing_port1,
        test_dmac[0], test_dmac[1], test_dmac[2],
        test_dmac[3], test_dmac[4], test_dmac[5],
        test_vid, OpaqueTag, OpaqueTag_Extn_unmatched);
    printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
    
    sprintf(cmd, "sleep %d", 3);
    printf("%s\n", cmd); bshell(unit, cmd);
    
    sprintf(cmd, "fp stat get si=%d t=p", main_ifp_statid);
    printf("%s\n", cmd); bshell(unit, cmd);
    
    /////////////////////////////////////////////////////////////////////////////
    // >>>>> UN_MATCHING TAG for opt_OpaqueTagLen=1
    // >>>>> UN_MATCHING TAG for opt_OpaqueTagLen=2
    // TAG1: 0x81000064, TAG2: 0x91000066   Extn: 0xAABBCCEE
    sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X0000000000CC8100%04X%08X%08Xc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5",
        ing_port1,
        test_dmac[0], test_dmac[1], test_dmac[2],
        test_dmac[3], test_dmac[4], test_dmac[5],
        test_vid, OpaqueTag_unmatched, OpaqueTag_Extn_unmatched);
    printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
    
    sprintf(cmd, "sleep %d", 3);
    printf("%s\n", cmd); bshell(unit, cmd);
    
    sprintf(cmd, "fp stat get si=%d t=p", main_ifp_statid);
    printf("%s\n", cmd); bshell(unit, cmd);
    
    /////////////////////////////////////////////////////////////////////////////
    // >>>>> MATCHING TAG    for opt_OpaqueTagLen=1
    // >>>>> MATCHING TAG    for opt_OpaqueTagLen=2
    // TAG1: 0x8100, TAG2: 0x91000064   Extn: 0xAABBCCDD
    sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X0000000000CC8100%04X%08X%08Xc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5",
        ing_port1,
        test_dmac[0], test_dmac[1], test_dmac[2],
        test_dmac[3], test_dmac[4], test_dmac[5],
        test_vid, OpaqueTag, OpaqueTag_Extn);
    printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
    
    sprintf(cmd, "sleep %d", 3);
    printf("%s\n", cmd); bshell(unit, cmd);
    
    sprintf(cmd, "fp stat get si=%d t=p", main_ifp_statid);
    printf("%s\n", cmd); bshell(unit, cmd);
    
    /////////////////////////////////////////////////////////////////////////////
    // >>>>> UN_MATCHING TAG    for opt_OpaqueTagLen=1
    // >>>>> MATCHING TAG       for opt_OpaqueTagLen=2
    // TAG1: 0x8100, TAG2: 0x91000066   Extn: 0xAABBCCDD
    sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X0000000000CC8100%04X%08X%08Xc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5",
        ing_port1,
        test_dmac[0], test_dmac[1], test_dmac[2],
        test_dmac[3], test_dmac[4], test_dmac[5],
        test_vid, OpaqueTag_unmatched, OpaqueTag_Extn);
    printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
    
    sprintf(cmd, "sleep %d", 3);
    printf("%s\n", cmd); bshell(unit, cmd);
    
    sprintf(cmd, "fp stat get si=%d t=p", main_ifp_statid);
    printf("%s\n", cmd); bshell(unit, cmd);
    
    /////////////////////////////////////////////////////////////////////////////
    if( opt_OpaqueTagLen == 1 )
    {
        printf("\n\n\nOnSuccess: Final stat count should be 2\n\n\n");
    }
    else if( opt_OpaqueTagLen == 2 )
    {
        printf("\n\n\nOnSuccess: Final stat count should be 1\n\n\n");
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
    
    if( (rrv = ConfigureOpaqueTagFp()) != BCM_E_NONE )
    {
        printf("Configuring Opaque TAG in FP failed with %d\n", rrv);
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

