/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : UDF Hints in IFP with HW Sharing
 *
 *  Usage    : BCM.0> cint udf_hints_hw_sharing.c
 *
 *  config   : field_config.bcm
 *
 *  Log file : udf_hints_hw_sharing_log.txt
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
 *    2) Step2 - Configuration (Done in ConfigureUdfAndFp()).
 *    ===================================================
 *       a) Create L3 interface MAC=0xDD, VID=100
 *       b) Create L3 egress object DMAC=0xEE, VID=100, Interface as above.
 *       c) Configure MPLS POP action for label=1000. After POP,
 *                      applicable egress object as above.
 *       d) Create 1st UDF extraction for 1 label MPLS packets
 *                  OFFSET to fetch TTL from mpls label
 *       e) Create 2nd UDF extraction for 2 labels MPLS packets
 *                  OFFSET to fetch TTL from INNER mpls label (HW SHARED)
 *       f) Create IFP Entry
 *             QUAL    : UDF1 and UDF2
 *             ACTION  : None
 *             STAT    : Counter with STAT ID = 1
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==============================
 *       a) Send PACKET1, STAT UP (See below)
 *       b) Send PACKET2, STAT SAME
 *       c) Send PACKET3, STAT UP
 *       d) Send PACKET4, STAT SAME
 *       e) Expected Result:
 *          ================
 *          The final STAT count should be 2
 *
 *    PACKET1:
 *          MACDA: 0xDD, VID: 100, MPLS ONE label: 1000(h3E8), 7(h7), 39(h27)
 *    PACKET2:
 *          MACDA: 0xDD, VID: 100, MPLS ONE label: 1000(h3E8), 7(h7), 33(h21)
 *    PACKET3:
 *          MACDA: 0xDD, VID: 100, MPLS TWO label: 1000(h3E8), 7(h7), 33(h21) 
 *                                                 2000(h7D0), 6(h6), 39(h27)
 *    PACKET4:
 *          MACDA: 0xDD, VID: 100, MPLS TWO label: 1000(h3E8), 7(h7), 33(h21)
 *                                                 2000(h7D0), 6(h6), 33(h21)
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

bshell(unit, "fp init");

bshell(unit, "init udf");

/*uint8 mpls_label4_sample_FULL[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0xDD, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCC, 0x81, 0x00, 0x00, 0x64, 0x88, 0x47, 0x00, 0x11, 0x11, 0x40, 0x45, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x40, 0xFF, 0x73, 0xD8, 0x02, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0A, 0xA8, 0x42, 0x1E };*/
// 0000000000DD0000000000CC81000064884700111140450000220000000040FF73D80202020201010101000102030405060708090A0B0C0D0AA8421E

uint32 mpls_1_label = 0x2222;
uint32 mpls_2_label = 0x3333;
uint32 mpls_3_label = 0x4444;
uint32 mpls_4_label = 0x5555;
uint32 mpls_5_label = 0x6666;
uint32 mpls_6_label = 0x7777;

bcm_vlan_t test_vid = 0x0064; // 100

bcm_mac_t local_mac_1  = {0x00, 0x00, 0x00, 0x00, 0x00, 0xDD};
bcm_mac_t test_smac_1  = {0x00, 0x00, 0x00, 0x00, 0x00, 0xCC};

// For L3 interface and egress-object
bcm_mac_t remote_mac_1 = {0x00, 0x00, 0x00, 0x00, 0x00, 0xEE};  // not used
bcm_mac_t intf_mac_1   = {0x00, 0x00, 0x00, 0x00, 0x00, 0x99};  // not used

// SIP+DIP for upto 5 labels. Data for "matching" in IFP.
uint8  sip_dip_data[8] = 
    { 0xA0, 0xA0, 0xA0, 0xAA, 0xA1, 0xA1, 0xA1, 0xA2 };

uint8 sip_dip_mask[8] =
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

int main_ifp_statid = 1;

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


/* 
 * This functions gets the port numbers and sets up ingress and 
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port1, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ing_port1, BCM_PORT_DISCARD_NONE));

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port2, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ing_port2, BCM_PORT_DISCARD_ALL));

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egr_port, BCM_PORT_DISCARD_NONE));

    bshell(unit, "pw start report +raw +decode");

    return BCM_E_NONE;
}

/////////////////   CHUNK MANAGEMENT   - START  ///////////////////////////
// 16 chunks, each of 2 Bytes. Total 32 bytes. (Or 128 bits)

const int NumBytesInEachChunk = 2;
uint32 FreeChunkMask = 0xFFFF;
uint32 ChunkError = 0xFFFFFFFF;
// Returns ChunkError in case of error.
uint32 ChunksBitmapAllocate(int unit, int WidthInBytes /* Always mult of 2 */)
{
    int numChunksNeeded = WidthInBytes / NumBytesInEachChunk;
    uint32 bmp = (1 << numChunksNeeded) - 1;
    
    if( FreeChunkMask == 0x0000 )
    {
        return ChunkError;
    }
    
    while( bmp )
    {
        /* If matched, break; else keep looking*/
        if( (FreeChunkMask & bmp) == bmp )
        {
            break;
        }
        else
        {
            bmp = bmp << 1;
        }
    }
    
    FreeChunkMask &= ~(bmp);
    

    return bmp;
}

uint16 ChunksBitmapFree(int unit, uint16 BmpToFree)
{
    
    FreeChunkMask &= (~BmpToFree);
    
    
    return FreeChunkMask;
}

/////////////////   CHUNK MANAGEMENT   - END   ///////////////////////////

int fp_udf_rule_match(int unit, int gid, int eid, bcm_field_qset_t qset /* IN */,
                        bcm_udf_id_t udf_id /* IN */, uint8 num_bytes /* IN */,
                        uint8 *data /* IN */, uint8 *mask /* IN */)
{
    int rv;
    int flagNewGroupCreated = FALSE;
    
    // Try creating the group. If EXISTS, then we SKIP creating STAT
    rv = bcm_field_group_create_id(unit, qset, gid /* prio */, gid);
    if (BCM_FAILURE(rv)) {
        if (rv != BCM_E_EXISTS) {
            printf("Group[%d] creation Failed:%d\n\r", gid, rv);
            return rv;
        } else {
            printf("[OK] IGNORE THIS ERROR...\n\r");
        }
    } else {
        flagNewGroupCreated = TRUE;
    }
    
    rv = bcm_field_entry_create_id(unit, gid, eid);
    if (BCM_FAILURE(rv)) {
        if (rv != BCM_E_EXISTS) {
            printf("Entry[%d] Creation Failed:%d\n\r", eid, rv);
            return rv;
        } else {
            //printf("[OK] IGNORE THIS ERROR for Entry...\n\r");
        }
    }
    
    rv = bcm_field_qualify_udf(unit, eid, udf_id, num_bytes, data, mask);
    if (BCM_FAILURE(rv)) {
        printf("Entry[%d] UDF[%d] Qualify Set Failed:%d\n\r", eid, udf_id, rv);
        return rv;
    }
    
    
    rv = bcm_field_qualify_PacketRes(unit, eid, BCM_FIELD_PKT_RES_UNKNOWN, 0xFF);    // RES_UNKNOWN, as MPLS not configured in this CINT
    if (BCM_FAILURE(rv)) {
        printf("Entry[%d] PacketRes[%d] Qualify Set Failed:%d\n\r", eid, udf_id, rv);
        return rv;
    }
    
    bcm_field_action_remove(unit, eid, bcmFieldActionCopyToCpu);
    rv = bcm_field_action_add(unit, eid, bcmFieldActionCopyToCpu, 1, eid);
    if (BCM_FAILURE(rv)) {
        printf("Entry[%d] Action Add Failed:%d\n\r", eid, rv);
        return rv;
    }
    
    if( flagNewGroupCreated )
    {
        const bcm_field_stat_t stats[2] = { bcmFieldStatPackets, bcmFieldStatBytes };
        /* Also set up an FP stat counter to count matches */
        BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, gid, 2, stats, main_ifp_statid));
        /* Attach the stat object to the entry */
        BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, eid, main_ifp_statid));
    }
    
    rv = bcm_field_entry_install(unit, eid);
    if (BCM_FAILURE(rv)) {
        printf("Entry[%d] Install Failed:%d\n\r", eid, rv);
    }
    
    return rv;
}

int match_on_mpls_pkt_type(int unit, int NumLabels, int offset_bytes, int width_bytes,
                           uint32 ChunkMask, int gid, int eid)
{
    int rv;
    bcm_udf_id_t udf_id;
    bcm_field_qset_t qset;
    bcm_udf_abstract_pkt_format_t pkt_fmt;
    uint8 *data;
    uint8 *mask;

    if (NumLabels == 0) {
        printf("NumLabels is not mentioned.\n");
        return BCM_E_PARAM;
    }
    
    data = sip_dip_data;
    mask = sip_dip_mask;
    
    switch( NumLabels ) {
        case 1:
            pkt_fmt = bcmUdfAbstractPktFormatKnownL3MplsOneLabel;
            break;

        case 2:
            pkt_fmt = bcmUdfAbstractPktFormatKnownL3MplsTwoLabel;
            break;

        case 3:
            pkt_fmt = bcmUdfAbstractPktFormatKnownL3MplsThreeLabel;
            break;

        case 4:
            pkt_fmt = bcmUdfAbstractPktFormatKnownL3MplsFourLabel;
            break;

        case 5:
            pkt_fmt = bcmUdfAbstractPktFormatKnownL3MplsMoreThanFourLabel;
            break;

        default:
            printf("Invalid NumLabels:%d\n\r", NumLabels);
            return BCM_E_PARAM;
    }
    
    bcm_udf_chunk_info_t    info;

    bcm_udf_chunk_info_t_init(&info);
    
    info.offset = offset_bytes * 8; // bits
    info.width  = width_bytes  * 8; // bits
    info.chunk_bmap = ChunkMask;
    info.abstract_pkt_format = pkt_fmt;
    rv = bcm_udf_chunk_create(0, NULL, &info, &udf_id);
    if (BCM_SUCCESS(rv)) {
        printf("UDF created Successfully: %d\n\r", udf_id);
    } else {
        printf("UDF Failed to created: rv:%d\n\r", rv);
    }
    
    BCM_FIELD_QSET_INIT(qset);
    //BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngressExactMatch);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyPacketRes);
    
    rv = bcm_field_qset_id_multi_set(unit, bcmFieldQualifyUdf, 1, &udf_id, qset);
    if (BCM_FAILURE(rv)) {
        printf("Entry[%d] qset_id_multi_set %d: %d\n\r", eid, udf_id, rv);
        return rv;
    }
    
    rv = fp_udf_rule_match(unit, gid, eid, qset, udf_id, width_bytes, data, mask);
    if (BCM_FAILURE(rv)) {
        printf("Entry[%d] UDF Rule match failed:%d\n\r", eid, rv);
        return rv;
    }
    
    return rv;
}

int DoUdfFpConfig()
{
    print bcm_udf_init(unit);
    print bcm_field_init(unit);
    const int width_bytes = 8;    // IP (SIP+DIP) is 4+4 bytes @ offset 16B from end of 1 label
                                  // MPLS packet ethertype (0x8847)
    uint32 bmpChunk = ChunksBitmapAllocate(0, width_bytes);
    if( bmpChunk == ChunkError )
    {
        printf("Not enough chunks remaining\n");
        return BCM_E_RESOURCE;
    }
    
    printf("F: 0x%08X\n", bmpChunk);
    // Same bmpChunk is used for setting multiple UDF data. This is equivalent to
    // BCM_UDF_CREATE_O_SHARED_HWID
    //                   match_on_mpls_pkt_type(unit, NumLabels, offset_bytes,
    //                                              width_bytes, ChunkMask, gid, eid)
    BCM_IF_ERROR_RETURN( match_on_mpls_pkt_type(unit,    1,        16,
                                                    width_bytes,  bmpChunk, 1, 1) );
    BCM_IF_ERROR_RETURN( match_on_mpls_pkt_type(unit,    2,        16+4,
                                                    width_bytes,  bmpChunk, 1, 1) );
    BCM_IF_ERROR_RETURN( match_on_mpls_pkt_type(unit,    3,        16+4+4,
                                                    width_bytes,  bmpChunk, 1, 1) );
    BCM_IF_ERROR_RETURN( match_on_mpls_pkt_type(unit,    4,        16+4+4+4,
                                                    width_bytes,  bmpChunk, 1, 1) );
    BCM_IF_ERROR_RETURN( match_on_mpls_pkt_type(unit,    5,        16+4+4+4+4,
                                                    width_bytes,  bmpChunk, 1, 1) );

    // Try enabling the below code to see the error thown by the SDK...
    // It passes a totally different chunk bitmap which was not already part of this group's qset.
    if( 0 )
    {
        printf("Trying 5 LABEL MPLS with UDF Chunk not part of Group %d...\n", 1);
        BCM_IF_ERROR_RETURN( match_on_mpls_pkt_type(unit,    5,        16+4+4+4+4,
                                                        width_bytes,  0xF000, 1, 1) );
    }
    return BCM_E_NONE;
}

bcm_error_t
verify()
{
    char cmd[2048];
    
    // 1 label mpls packet
    printf("\n\n###########   Sending ONE LABEL MPLS Packet    #############\n");
    sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X8847%05X1404500002E0000000040063E8F%02X%02X%02X%02X%02X%02X%02X%02X004000410000000000000000500000006D190000000102030405AEA6E999",
    ing_port1,
    local_mac_1[0], local_mac_1[1], local_mac_1[2],
    local_mac_1[3], local_mac_1[4], local_mac_1[5],
    test_smac_1[0], test_smac_1[1], test_smac_1[2],
    test_smac_1[3], test_smac_1[4], test_smac_1[5],
    test_vid, mpls_1_label,
    sip_dip_data[0], sip_dip_data[1], sip_dip_data[2], sip_dip_data[3],
    sip_dip_data[4], sip_dip_data[5], sip_dip_data[6], sip_dip_data[7] );
    bbshell(unit, cmd);
    
    sprintf(cmd, "fp stat get si=%d t=p", main_ifp_statid);
    bbshell(unit, cmd);
    
    printf("\n\n###########   Sending ONE LABEL MPLS Packet - MISMATCH IP    #############\n");
    sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X8847%05X1404500002E0000000040063E8F%02X%02X%02X%02X%02X%02X%02X%02X004000410000000000000000500000006D190000000102030405AEA6E999",
    ing_port1,
    local_mac_1[0], local_mac_1[1], local_mac_1[2],
    local_mac_1[3], local_mac_1[4], local_mac_1[5],
    test_smac_1[0], test_smac_1[1], test_smac_1[2],
    test_smac_1[3], test_smac_1[4], test_smac_1[5],
    test_vid, mpls_1_label,
    sip_dip_data[0]+1, sip_dip_data[1], sip_dip_data[2], sip_dip_data[3],
    sip_dip_data[4], sip_dip_data[5], sip_dip_data[6], sip_dip_data[7] );
    bbshell(unit, cmd);
    
    sprintf(cmd, "fp stat get si=%d t=p", main_ifp_statid);
    bbshell(unit, cmd);

    printf("\n\n###########   Sending TWO LABEL MPLS Packet    #############\n");
    sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X8847%05X040%05X1404500002E0000000040063E8F%02X%02X%02X%02X%02X%02X%02X%02X004000410000000000000000500000006D190000000102030405AEA6E999",
    ing_port1,
    local_mac_1[0], local_mac_1[1], local_mac_1[2],
    local_mac_1[3], local_mac_1[4], local_mac_1[5],
    test_smac_1[0], test_smac_1[1], test_smac_1[2],
    test_smac_1[3], test_smac_1[4], test_smac_1[5],
    test_vid, mpls_1_label, mpls_2_label,
    sip_dip_data[0], sip_dip_data[1], sip_dip_data[2], sip_dip_data[3],
    sip_dip_data[4], sip_dip_data[5], sip_dip_data[6], sip_dip_data[7] );
    bbshell(unit, cmd);
    
    sprintf(cmd, "fp stat get si=%d t=p", main_ifp_statid);
    bbshell(unit, cmd);
    
    printf("\n\n###########   Sending THREE LABEL MPLS Packet    #############\n");
    sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X8847%05X040%05X040%05X1404500002E0000000040063E8F%02X%02X%02X%02X%02X%02X%02X%02X004000410000000000000000500000006D190000000102030405AEA6E999",
    ing_port1,
    local_mac_1[0], local_mac_1[1], local_mac_1[2],
    local_mac_1[3], local_mac_1[4], local_mac_1[5],
    test_smac_1[0], test_smac_1[1], test_smac_1[2],
    test_smac_1[3], test_smac_1[4], test_smac_1[5],
    test_vid, mpls_1_label, mpls_2_label, mpls_3_label,
    sip_dip_data[0], sip_dip_data[1], sip_dip_data[2], sip_dip_data[3],
    sip_dip_data[4], sip_dip_data[5], sip_dip_data[6], sip_dip_data[7] );
    bbshell(unit, cmd);
    
    sprintf(cmd, "fp stat get si=%d t=p", main_ifp_statid);
    bbshell(unit, cmd);
    
    printf("\n\n###########   Sending FOUR LABEL MPLS Packet    #############\n");
    sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X8847%05X040%05X040%05X040%05X1404500002E0000000040063E8F%02X%02X%02X%02X%02X%02X%02X%02X004000410000000000000000500000006D190000000102030405AEA6E999",
    ing_port1,
    local_mac_1[0], local_mac_1[1], local_mac_1[2],
    local_mac_1[3], local_mac_1[4], local_mac_1[5],
    test_smac_1[0], test_smac_1[1], test_smac_1[2],
    test_smac_1[3], test_smac_1[4], test_smac_1[5],
    test_vid, mpls_1_label, mpls_2_label, mpls_3_label, mpls_4_label,
    sip_dip_data[0], sip_dip_data[1], sip_dip_data[2], sip_dip_data[3],
    sip_dip_data[4], sip_dip_data[5], sip_dip_data[6], sip_dip_data[7] );
    bbshell(unit, cmd);
    
    sprintf(cmd, "fp stat get si=%d t=p", main_ifp_statid);
    bbshell(unit, cmd);
    
    printf("\n\n###########   Sending FIVE LABEL MPLS Packet    #############\n");
    sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X8847%05X040%05X040%05X040%05X040%05X1404500002E0000000040063E8F%02X%02X%02X%02X%02X%02X%02X%02X004000410000000000000000500000006D190000000102030405AEA6E999",
    ing_port1,
    local_mac_1[0], local_mac_1[1], local_mac_1[2],
    local_mac_1[3], local_mac_1[4], local_mac_1[5],
    test_smac_1[0], test_smac_1[1], test_smac_1[2],
    test_smac_1[3], test_smac_1[4], test_smac_1[5],
    test_vid, mpls_1_label, mpls_2_label, mpls_3_label, mpls_4_label, mpls_5_label,
    sip_dip_data[0], sip_dip_data[1], sip_dip_data[2], sip_dip_data[3],
    sip_dip_data[4], sip_dip_data[5], sip_dip_data[6], sip_dip_data[7] );
    bbshell(unit, cmd);
    
    sprintf(cmd, "fp stat get si=%d t=p", main_ifp_statid);
    bbshell(unit, cmd);
    
    printf("\n\n###########   Sending SIX LABEL MPLS Packet    #############\n");
    sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X8847%05X040%05X040%05X040%05X040%05X040%05X1404500002E0000000040063E8F%02X%02X%02X%02X%02X%02X%02X%02X004000410000000000000000500000006D190000000102030405AEA6E999",
    ing_port1,
    local_mac_1[0], local_mac_1[1], local_mac_1[2],
    local_mac_1[3], local_mac_1[4], local_mac_1[5],
    test_smac_1[0], test_smac_1[1], test_smac_1[2],
    test_smac_1[3], test_smac_1[4], test_smac_1[5],
    test_vid, mpls_1_label, mpls_2_label, mpls_3_label,
    mpls_4_label, mpls_5_label, mpls_6_label,
    sip_dip_data[0], sip_dip_data[1], sip_dip_data[2], sip_dip_data[3],
    sip_dip_data[4], sip_dip_data[5], sip_dip_data[6], sip_dip_data[7] );
    bbshell(unit, cmd);
    
    sprintf(cmd, "fp stat get si=%d t=p", main_ifp_statid);
    bbshell(unit, cmd);
    
    bcm_udf_abstract_pkt_format_t           abstract_pkt_format;
    bcm_udf_abstract_pkt_format_info_t      pkt_format_info;
    
    if( 1 )
    {
        printf("\n\n###########    bcm_udf_abstract_pkt_format_info_get() API Demo       ###########\n");
        print bcm_udf_abstract_pkt_format_info_get(unit, bcmUdfAbstractPktFormatKnownL3MplsTwoLabel, &pkt_format_info);
        print pkt_format_info;
        
        print bcm_udf_abstract_pkt_format_info_get(unit, bcmUdfAbstractPktFormatKnownL3MplsThreeLabel, &pkt_format_info);
        print pkt_format_info;
        
        print bcm_udf_abstract_pkt_format_info_get(unit, bcmUdfAbstractPktFormatKnownL3MplsFourLabel, &pkt_format_info);
        print pkt_format_info;
    }
    
    return BCM_E_NONE;
}


bcm_error_t
execute()
{
    int rrv;
    if( (rrv = test_setup(unit)) != BCM_E_NONE )
    {
        printf("Creating the test setup failed %d\n", rrv);
        return rrv;
    }
    
    if( (rrv = DoUdfFpConfig()) != BCM_E_NONE )
    {
        printf("Configuring UDF and FP failed with %d\n", rrv);
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

