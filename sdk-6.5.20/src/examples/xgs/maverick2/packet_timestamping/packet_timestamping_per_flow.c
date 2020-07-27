/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

cint_reset();

/*  Feature  : Packet Timestamping - Flow based
 *
 *  Usage    : BCM.0> cint packet_timestamping_per_flow.c
 *
 *  config   : packet_timestamping_config.bcm
 *
 *  Log file : packet_timestamping_per_flow_log.txt
 *
 *  Test Topology :
 *  LB: Loopback
 *  LBd: Loopback and discard
 *
 *   PORT1-LB  +---------------------+
 *   +--------->                     |
 *             |                     |
 *             |                     |
 *             |                     |
 *             |                     |
 *             |                     |
 *             |                     |    EGR-LBd
 *             |                     +--------->
 *             |                     |
 *             +---------------------+
 *
 *  Summary:
 *      Cint example to show configuration of inserting the timestamp data of ingressed
 *      packet based on the flow parameters of the packet. It also inserts time stamp at 
 *      at egress. Packet Timestamping: Time stamp insertion on data packets through the XGS
 *      data-path. The goal is to attach arrival and departure times to a frame at one or more
 *      routers along a flow path across the network.
 *
 *  Detailed steps done in the CINT script:
 *  ====================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ================================
 *       a) Select one ingress and three egress ports and configure them in
 *          Loopback mode. Install a rule to copy incoming packets to CPU and
 *          additional action to drop the packets when it loops back on egress
 *          ports. Start packet watcher.
 *       b) Create a VLAN(100) and add ing_port1, ing_port2 and egr_port
 *          as members. Makes packet with dmac=0xDD to go to ing_port2.
 *
 *    2) Step2 - Configuration (Done in ConfigurePktTimestamp()). 
 *    ===================================================
 *       a) Create main IFP
 *             QSET:   InPort (NA)
 *             ASET:   IngressTimeStampInsert + EgressTimeStampInsert
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==============================
 *       a) Send any IPv4 packet and see that at the tail of the ingressed packet TS_SHIM
 *          is inserted. At the egressed packet another TS_SHIM is inserted
 *       b) Expected Result:
 *          ================
 *          TS_SHIM inserted
 */

/**********************   GENERIC UTILITY FUNCTIONS STARTS  *************************
 */

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

bcm_gport_t gport_1;
bcm_gport_t gport_2;

bcm_vlan_t     test_vid = 100;
bcm_mac_t     test_dmac = {0x00, 0x00, 0x00, 0x00, 0x00, 0xDD};

int         main_ifp_statid = 1;
uint64 tmp_hack_sec;
int tmp_hack_sec_lower = 2000;
uint64 tmp_hack_nsec;
int tmp_hack_nsec_lower = 3000;

void UpdateYCounter()
{
    bcm_time_ts_counter_t counter;
    bcm_time_tod_t tod;
    uint64 secs, nano_secs, div;
    uint32 stages;
    
    bcm_time_ts_counter_get(0, &counter);
    //print counter;

    stages = BCM_TIME_STAGE_EGRESS;
    tod.time_format = counter.time_format;
    tod.ts_adjustment_counter_ns = counter.ts_counter_ns;
    
    /* Get seconds from the timestamp value */
    secs = counter.ts_counter_ns;

    COMPILER_64_SET(div, 0, 1000000000);
    
    tmp_hack_sec_lower++;
    tmp_hack_nsec_lower++;
    COMPILER_64_SET(tmp_hack_sec, 0, tmp_hack_sec_lower);
    COMPILER_64_SET(tmp_hack_nsec, 0, tmp_hack_nsec_lower);
    
    //COMPILER_64_UDIV_64(secs, div);
    //tod.ts.seconds = secs;
    tod.ts.seconds = tmp_hack_sec;

    /* Get nanoseconds from the timestamp value */
    nano_secs = counter.ts_counter_ns;
    //COMPILER_64_UMUL_32(secs, 1000000000);
    //COMPILER_64_SUB_64(nano_secs, secs);
    //tod.ts.nanoseconds = COMPILER_64_LO(nano_secs);
    tod.ts.nanoseconds = tmp_hack_nsec_lower;

    bcm_time_tod_set(0, stages, &tod);
    //print tod;

    bshell(unit, "d EGR_TS_UTC_CONVERSION");
    bshell(unit, "d EGR_TS_UTC_CONVERSION_2");
    
}


bcm_error_t
ConfigurePktTimestamp(int unit)
{
    bcm_port_gport_get(unit, ing_port1, &gport_1);
    bcm_port_gport_get(unit, egr_port, &gport_2);
    
    printf("Doing port control for PacketTimeStampRxId..\n");
    BCM_IF_ERROR_RETURN( bcm_port_control_set(unit, ing_port1, bcmPortControlPacketTimeStampRxId, 0xcc) );
    printf("Doing port control for PacketTimeStampTxId..\n");
    BCM_IF_ERROR_RETURN( bcm_port_control_set(unit, egr_port, bcmPortControlPacketTimeStampTxId, 0xdd) );


    bcm_field_group_config_t group_config;
    bcm_field_entry_t entry_1;

    /* FP Group Configuration and Creation */
    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);

    group_config.mode = bcmFieldGroupModeAuto;
    printf("Doing IFP group create..\n");
    BCM_IF_ERROR_RETURN( bcm_field_group_config_create(unit, &group_config) );

    /* FP Entry 1 Configuration and Creation */
    printf("Doing IFP entry create..\n");
    BCM_IF_ERROR_RETURN( bcm_field_entry_create(unit, group_config.group, &entry_1) );
    printf("Doing IFP action add IngTimeStamp Insert..\n");
    BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry_1, bcmFieldActionIngressTimeStampInsert, 0, 0) );
    printf("Doing IFP action add EgrTimeStamp Insert..\n");
    BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry_1, bcmFieldActionEgressTimeStampInsert, 0, 0) );
    
    printf("Doing stat attach..\n");
    const bcm_field_stat_t stats[2] = { bcmFieldStatPackets, bcmFieldStatBytes };
    BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group_config.group, 2, stats, main_ifp_statid));
    BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry_1, main_ifp_statid));
    
    /* Installing FP Entry 1 to FP TCAM */
    printf("Doing IFP entry instal l111..\n");
    BCM_IF_ERROR_RETURN( bcm_field_entry_install(unit, entry_1) );
    printf("Done IFP entry instal 2222..\n");
    
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
    
    if( 1 )
    {
        bcm_pbmp_t     vlan_pbmp;
        bcm_pbmp_t     ut_pbmp;

        BCM_PBMP_CLEAR(vlan_pbmp);
        BCM_PBMP_PORT_ADD(vlan_pbmp, ing_port1);
        BCM_PBMP_PORT_ADD(vlan_pbmp, ing_port2);
        BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port);

        BCM_PBMP_CLEAR(ut_pbmp);
        printf("Doing create vlan and port add...\n");
        BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, test_vid));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, test_vid, vlan_pbmp, ut_pbmp));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, 1, vlan_pbmp)); // remove from default vlan
    }
    
    //print bcm_port_control_set(unit, cpu_port, bcmPortControlIP4, TRUE);
    
    if( 1 )
    {
        bcm_l2_addr_t l2_addr;
        
        bcm_l2_addr_t_init(&l2_addr, test_dmac, test_vid);
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = egr_port;
        printf("Doing L2 address add...\n");
        BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));
    }
    
    if( 1 )    // disabling this code as we are testing FP functionality in local mode
    {
        rv = ingress_port_setup(unit, ing_port1, BCM_PORT_LOOPBACK_PHY);
        if ( rv != BCM_E_NONE ) {
            printf("ingress_port_setup() failed for port %d (Error: %d)\n", ing_port1, rv);
            return rv;
        }

        rv = egress_port_setup(unit, egr_port, BCM_PORT_LOOPBACK_PHY);
        if ( rv != BCM_E_NONE ) {
            printf("egress_port_setup() failed for port %d (Error: %d)\n", egr_port, rv);
            return rv;
        }
    }
    else
    {
        printf("Doing port loopbacks...\n");
        
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port1, BCM_PORT_LOOPBACK_PHY));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ing_port1, BCM_PORT_DISCARD_NONE));
        
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port, BCM_PORT_LOOPBACK_PHY));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egr_port, BCM_PORT_DISCARD_ALL));
    }
    
    printf("Doing port loopback for %d...\n", ing_port2);
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port2, BCM_PORT_LOOPBACK_PHY));
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
    int num_pkt_in_a_burst = 1;
    int i;

    sprintf(cmd, "fp show");
    printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
    
    /////////////////////////////////////////////////////////////////////////////
    // Sending IPv4 pkt with known dest IP addr
    for(i=0; i<10; ++i)
    {
        printf("\n\n\n############ %d ###########\n", i);
        sprintf(cmd, "tx %d pbm=%d data=%02X%02X%02X%02X%02X%02X0000000000CC8100%04X08004500002E0000000040063E8C%08X%08X000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1f202122232425262728292a2b2c2d2e2f; sleep quiet 1;",
                num_pkt_in_a_burst, ing_port1,
                test_dmac[0], test_dmac[1], test_dmac[2],
                test_dmac[3], test_dmac[4], test_dmac[5],
                test_vid, 0x0A0A0AAA, 0x0B0B0BBB);
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
    
    if( (rrv = ConfigurePktTimestamp(unit)) != BCM_E_NONE )
    {
        printf("Configuring Packettime stamping in IFP failed with %d\n", rrv);
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
