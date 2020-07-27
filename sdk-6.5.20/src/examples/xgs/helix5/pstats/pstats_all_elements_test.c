/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : Packetized Statistic Test
 *
 *  Usage    : BCM.0> cint pstats_all_elements_test.c
 *
 *  config   : pstat_config.bcm
 *
 *  Log file : pstats_all_elements_test_log.txt
 *
 *  Build: FEATURE_LIST += PSTATS
 *
 *  Test Topology :
 *
 *                   +------------------------------+
 *                   |                              +-----------------+
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
 * FEATURE_LIST=ATPTRANS_SOCKET L3 I2C MEM_SCAN EDITLINE \
 *       CUSTOMER TELNET DRIVERS CHASSIS TEST RCPU BCM_SAL_PROFILE CINT \
 *       PTP CES FCMAP BOARD KNET REGEX XFLOW_MACSEC PSTATS
 *
 * CONFIG:
 *   buffer_stats_collect_mode=0x0
 *   buffer_stats_collect_type=0x2
 *
 *  Summary:
 *  ========
 * Helix5 supports a feature packetized statistics (PSTATS) which allows software
 * to read out MMU buffer use counts over SBUS and send this data via packets into the
 * Helix5 switch to any destination in the network in a timely manner.
 *
 * PSTATS related config variables:
 *
 * buffer_stats_collect_mode
 *
 * Flags to denote pstats and oob stats mode.
 *    0x0 Instantaneous mode
 *    0x1 Max use count mode with HW clear on Read
 *
 * buffer_stats_collect_type
 *
 * Flags to enable pstats or oob stats, only one can be enabled at a time.
 *    0x0 None: disable both oob stats and pstats
 *    0x1 OOB STATS: only enable out-of-band stats
 *    0x2 PSTATS: only enable packetized statistic
 *
 *
 * bcm_num_cos
 *
 * Initial number of CoS queues, by default is 8
 *
 *
 *  Detailed steps done in the CINT script:
 *  ====================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ================================
 *       a) Put required ports in loopback so that test can be performed
 *          using CPU generated traffic.
 *       b) Install IFP entries to catch the ingress and egress packets
 *          for visibility.
 *
 *    2) Step2 - Configuration (Done in do_pstats_config()).
 *    ===================================================
 *       a) Detect how many cosqs are present for each port and find out number of unicast cosqs,
 *          multicast cosqs and port scheduler level cosqs. This will be used in case pstats are 
 *          required for gport which represents cosqs (instead of port + cosqs). This is done by
 *          registering a callout function to bcm_cosq_gport_traverse.
 *       b) Calculate the buffer size rewuired to hold the pstats data for ingress, egress and port
 *          cosq levels.
 *       c) Create a PSTAT session, sync and then get the PSTAT data in the preallocated buffers.
 *       d) Send the captured data from the CPU by encapsulating it in a tunnelled packet.
 *       e) Do cleanup by destroying the session.
 *
 *    2) Step3 - Verification (Done in verify())
 *    ==============================
 *       a) CPU sends the encapsulated packet with the proper PSTATS header and the payload data.
 *       b) Expected Result:
 *          ================
 *          Encalsulated packet is seen egressing from the egress port with the PSTATS data of
 *          ingress, egress and gport levels.
 *
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

int unit = 0;

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

//////////////////////////////////////////////////////////////
// COSQ related
const int MAX_PORTS = 256;// 136;
const int NUM_CPU_Q = 48;
const int NUM_PORT_L0 = 12; // 10
const int NUM_PORT_Q_UC = 8; //10;
const int NUM_PORT_Q_MC = 4;
bcm_gport_t cpu_cosq_gp_l0[NUM_PORT_L0];
bcm_gport_t cpu_cosq_gp_mc[NUM_CPU_Q];

bcm_gport_t cosq_gp_l0[MAX_PORTS][NUM_PORT_L0];
bcm_gport_t cosq_gp_uc[MAX_PORTS][NUM_PORT_Q_UC];
bcm_gport_t cosq_gp_mc[MAX_PORTS][NUM_PORT_Q_MC];

int uc_cosq_num;    // to be calculated
int mc_cosq_num;    // to be calculated
int gp_cosq_num;    // to be calculated


int cpu_l0 = 0;
int cpu_mc = 0;
int index = 0;
int iterations = 0;
int lastFlag = -1;
int lastLclPort = -1;
int gport_callback(int unit, bcm_gport_t port, int numq, uint32 flags,
             bcm_gport_t gport, void *user_data) {
    int local_port;
    char *flagStr = "NA";
    
    local_port = BCM_GPORT_MODPORT_PORT_GET(port);
    
    if( lastFlag != flags )
    {
        index = 0;
        //printf("FLAGRESET: %02X %02X\n", lastFlag, flags);
    }
    if( lastLclPort != local_port )
    {
        index = 0;
        //printf("PORTRESET: %d %d\n", lastLclPort, local_port);
    }

    if (flags & BCM_COSQ_GPORT_UCAST_QUEUE_GROUP)
    {
        flagStr = "UC";
    }
    else if(flags & BCM_COSQ_GPORT_MCAST_QUEUE_GROUP)
    {
        flagStr = "MC";
    }
    else if (flags & BCM_COSQ_GPORT_SCHEDULER)
    {
        flagStr = "GS";
    }
    
    //printf("HHHHHHHHH p:0x%08X g:0x%08X lp:%d f:%s i:%d j:%d idx:%d\n",
    //        port, gport, local_port, flagStr, cpu_l0, cpu_mc, index);
    if( local_port >= MAX_PORTS )
    {
        printf("Crossing the port limit %d vs %d\n", local_port, MAX_PORTS);
        return 0;
    }
    //printf("    localport %d\n", local_port);
    if (local_port == 0) {

        if (flags & BCM_COSQ_GPORT_SCHEDULER) // 0x02
        {
            cpu_cosq_gp_l0[cpu_l0] = gport;
            cpu_l0++;
        }
        else if (flags & BCM_COSQ_GPORT_MCAST_QUEUE_GROUP)  // 0x20
        {
            cpu_cosq_gp_mc[cpu_mc] = gport;
            cpu_mc++;
        }
        else
        {
            printf("ERR: INVALID FLAG for port 0x%x(localport %d)\n",
                   port, local_port);
        }
    }
    else
    {
        
        if (flags & BCM_COSQ_GPORT_SCHEDULER)  // 0x02
        {
            cosq_gp_l0[local_port][index] = gport;
            if( gp_cosq_num < (index+1) )
            {
                gp_cosq_num = index+1;    // capture the max index found
            }
            index = (index + 1) % NUM_PORT_L0;
        }
        else if (flags & BCM_COSQ_GPORT_UCAST_QUEUE_GROUP)  // 0x08
        {
            cosq_gp_uc[local_port][index] = gport;
            if( uc_cosq_num < (index+1) )
            {
                uc_cosq_num = index+1;    // capture the max index found
            }
            index = (index + 1) % NUM_PORT_Q_UC;
        }
        else if (flags & BCM_COSQ_GPORT_MCAST_QUEUE_GROUP)  // 0x20
        {
            cosq_gp_mc[local_port][index] = gport;
            if( mc_cosq_num < (index+1) )
            {
                mc_cosq_num = index+1;    // capture the max index found
            }
            index = (index + 1) % NUM_PORT_Q_MC;
        }
        else
        {
            printf("ERR: INVALID FLAG for port 0x%x(localport %d)\n",
                   port, local_port);
        }
        
    }
    
    lastFlag = flags;
    lastLclPort = local_port;
    
    return 0;

}

int dump_gports (int port)
{
    int start = 0, end = 0, idx, g_idx, num_q = 0, n_idx, q_idx;

    if (port < -1) {
        printf ("Err. Input valid port number or -1 for all (%d) ports", MAX_PORTS);
    }

    if ((port == -1) || (port == 0)) {
    printf("\nPrinting Gport Hierarchy for CPU Port %d\n", port);
    for (n_idx = 0; n_idx < NUM_PORT_L0; n_idx++) {
            printf ("    L0.%d: 0x%x\n", n_idx, cpu_cosq_gp_l0[n_idx]);
            if (n_idx == 0) {
                for (q_idx = 0; q_idx < NUM_CPU_Q; q_idx++) {
                    printf ("        MC.%d: 0x%x\n", q_idx, cpu_cosq_gp_mc[q_idx]); 
                }
            }
        }
    }
    if (port == 0) {
        return 1;
    }
    if (port == -1) {
        start = 1; end = MAX_PORTS - 1;
    } else {
        start = end = port;
    }

    for (idx = start; idx <= end; idx++) {
        printf("\nPrinting Gport Hierarchy for Port %d\n", port);
        for (n_idx = 0; n_idx < NUM_PORT_L0; n_idx++) {
            printf ("    L0.%d: 0x%x\n", n_idx, cosq_gp_l0[idx][n_idx]);
            printf ("        UC.%d: 0x%x\n", n_idx, cosq_gp_uc[idx][n_idx]);
            printf ("        MC.%d: 0x%x\n", n_idx, cosq_gp_mc[idx][n_idx]);
        }
    }
    return 1;
}
//////////////////////////////////////////////////////////////


/* packet header */
const int common_head_len = 18;
const int ipv4_header_len = 20;
const int udp_header_len = 4;
const int pstats_header_len = 14;
const unsigned char common_hdr[18] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x02, 0x81, 0x00, 0x00, 0x64, 0x08, 0x00 };
unsigned short int ipv4_header[10];
unsigned short int udp_header[4];
unsigned char pstats_header[14];

/*calculate the ipv4 header checksum */
unsigned short int ipv4_checksum(unsigned short int *header, unsigned int length) {
    int i=0;
    unsigned int sum[1];
    sum[0] = 0;
    unsigned short int ans[1];
    ans[0] = 0;
    for(i=0; i<length; i++){
        sum[0] += header[i];
    }
    sum[0] = (sum[0] >> 16) + (sum[0] & 0xffff);
    sum[0] += (sum[0] >> 16);
    ans[0] = ~sum[0];
    return ans[0];
}

unsigned short int swap16(unsigned short int val){
    return ((val & 0xff) << 8) | ((val >> 8) & 0xff);
}

int pstats_build_packet_header(int unit, bcm_pstats_timestamp_t ts) {
    unsigned short int dscp = 0;
    unsigned short int ipv4_length = 108; /* calculated */
    unsigned short int ipv4_identifier = 0;
    unsigned short int ipv4_fragment_offset = 0;
    unsigned short int ipv4_flags = 0;
    unsigned short int ipv4_ttl = 64;
    unsigned short int ipv4_protocol = 17;

    unsigned int sip_addr = 0xC0A80101; /*192.168.1.1*/
    unsigned int dip_addr = 0xC0A80102; /*192.168.1.2*/

    unsigned short int udp_source_port = 0x1234;
    unsigned short int udp_dest_port = 0x5678;
    unsigned short int udp_length = 8; /* at least 8 bytes for the UDP header length */
    unsigned short int udp_checksum = 0; /* 0 for not used */

    unsigned char *ptr_timestamp = &pstats_header[5];
    unsigned char *ts_ptr = (auto)&(ts.nanoseconds);
    unsigned int *ptr_seconds = (auto)&(ts.seconds);
    int i;

    ipv4_header[0] = 0x4500;
    ipv4_header[0] |= dscp;
    ipv4_header[1] = ipv4_length;
    ipv4_header[2] = ipv4_identifier;
    ipv4_header[3] = ipv4_fragment_offset;
    ipv4_header[3] |= ipv4_flags<<13;
    ipv4_header[4] = ipv4_protocol;
    ipv4_header[4] |= ipv4_ttl<<8;
    ipv4_header[5] = 0; /* ip header checksum */
    ipv4_header[6] = sip_addr & 0xffff;
    ipv4_header[7] = sip_addr >> 16;
    ipv4_header[8] = dip_addr & 0xffff;
    ipv4_header[9] = dip_addr >> 16;
    ipv4_header[5] = ipv4_checksum(ipv4_header, 10);
    for(i=0; i<10; i++){
        ipv4_header[i] = swap16(ipv4_header[i]);
    }
    
    udp_header[0] = udp_source_port;
    udp_header[1] = udp_dest_port;
    udp_header[2] = udp_length;
    udp_header[3] = udp_checksum;
    
    for(i=0; i<4; i++){
        udp_header[i] = swap16(udp_header[i]);
    }
    
    pstats_header[0] = 0x10;
    pstats_header[1] = 0;
    pstats_header[2] = 1; // 8b SOURCE ID = 1
    pstats_header[3] = 0;
    pstats_header[4] = 0; // 8b MSG_TYPE 
    pstats_header[5] = 0; // 48b MSG_TIMESTAMP
    pstats_header[6] = 0; // 48b timestamp
    pstats_header[7] = 0;
    pstats_header[8] = 0;
    pstats_header[9] = 0;
    pstats_header[10] = 0;
    pstats_header[11] = 0; // 8b PKT_SEQ_NUM 
    pstats_header[12] = 0; // 8b RESERVED
    pstats_header[13] = 0; // 8b RESERVED

    /* polulate the timestamp field in pstats header */

    pstats_header[5] = *ts_ptr;
    pstats_header[6] = *(ts_ptr+1);
    pstats_header[7] = *(ts_ptr+2);
    pstats_header[8] = *(ts_ptr+3) & 0x3f;
    ts_ptr = (auto)&(ts.seconds);
    pstats_header[8] |= ((*ts_ptr & 0x3) << 6);
    *ptr_seconds = *ptr_seconds >> 2;
    ts_ptr = (auto) ptr_seconds;
    pstats_header[9] = *ts_ptr;
    pstats_header[10] = *(ts_ptr+1);

    return BCM_E_NONE;
}

int pstats_data_form(uint8 *data_buf, int *data_len,
                     bcm_pstats_timestamp_t timestamp,
                     bcm_pstats_data_t *data,
                     int array_count)
{
    int i,j, buf_len = 0;
    uint8 *buf = data_buf;

    sal_memset(buf, 0, *data_len);
    for (i = 0; i < array_count; i++) {
        buf_len = buf_len + data[i].array_count * 3;
        if (buf_len > *data_len) {
            return BCM_E_PARAM;
        }

        for (j = 0; j < data[i].array_count; j++) {
            sal_memcpy(buf, &(data[i].use_counts_array[j]), 3);
            buf = buf + 3;
        }
    }

    print buf_len;
    *data_len = buf_len;
    return BCM_E_NONE;
}

bcm_error_t
tx_pkt(int unit, int port, int count, uint8 *data_buf, int len)
{
    bcm_pkt_t pkt;
    int rv, i, pkt_len = 0, max_len = 1500;

    uint8 *pkt_data = soc_cm_salloc(unit, max_len, "tx_packet");
    uint8 *pkt_ptr = pkt_data;

    pkt_len = len + common_head_len + ipv4_header_len + udp_header_len + pstats_header_len;
    print pkt_len;
    if (pkt_len > max_len) {
        soc_cm_sfree(unit, pkt_data);
        return BCM_E_PARAM;
    } else if(pkt_len < 64) {
        pkt_len = 64;
    }

    sal_memset(pkt_ptr, 0, max_len);
    sal_memcpy(pkt_ptr, common_hdr, common_head_len);
    pkt_ptr += common_head_len;

    /* add ipv4 header */
    sal_memcpy(pkt_ptr, ipv4_header, ipv4_header_len);
    pkt_ptr += ipv4_header_len;

    /* add udp header */
    sal_memcpy(pkt_ptr, udp_header, udp_header_len);
    pkt_ptr += udp_header_len;

    /* add pstats header */
    sal_memcpy(pkt_ptr, pstats_header, pstats_header_len);
    pkt_ptr += pstats_header_len;

    /* fill the pstats data items */
    sal_memcpy(pkt_ptr, data_buf, len);
    pkt_ptr += len;

    bcm_pkt_t_init(&pkt);
    pkt.pkt_data = &pkt._pkt_data;
    pkt.blk_count = 1;
    pkt.pkt_data->data = pkt_data;

    print pkt_len;
    pkt.pkt_data->len = pkt_len;
    pkt.flags = BCM_TX_CRC_APPEND;
    BCM_PBMP_PORT_SET(pkt.tx_pbmp, port);

    for (i = 0; i < count; i++){
        rv = bcm_tx(unit, &pkt, NULL);
        if (BCM_FAILURE(rv)) {
            soc_cm_sfree(unit, pkt_data);
            return rv;
        }
    }

    soc_cm_sfree(unit, pkt_data);
    return BCM_E_NONE;
}

int pstats_packet_tx(bcm_pstats_timestamp_t ts,
                     bcm_pstats_data_t *ip_data, int ip_num,
                     bcm_pstats_data_t *ep_data, int ep_num,
                     bcm_pstats_data_t *uc_data, int uc_num)
{
    uint8                           data_buf[1200];
    int                             data_len, pkt_count = 0;

    /* ing pool packet */
    if (ip_num > 0) {
        data_len = 1200;
        printf("Sending packet for ing pool...\n");
        BCM_IF_ERROR_RETURN( pstats_data_form(data_buf, &data_len, ts, ip_data, ip_num) );
        BCM_IF_ERROR_RETURN( tx_pkt(unit, ing_port1, 1, data_buf, data_len) );
        pkt_count++;
    }

    /* egr pool packet */
    if (ep_num > 0) {
        data_len = 1200;
        printf("Sending packet for egr pool...\n");
        BCM_IF_ERROR_RETURN( pstats_data_form(data_buf, &data_len, ts, ep_data, ep_num) );
        BCM_IF_ERROR_RETURN( tx_pkt(unit, ing_port1, 1, data_buf, data_len) );
        pkt_count++;
    }

    if (uc_num > 0) {
        /* uc packet */
        int                         total_num = uc_num;
        int                         tmp_num;
        bcm_pstats_data_t           *tmp_data = uc_data;
        do {
            /* one packet contains max 192 uc elements data */
            if (total_num > 192) {
                tmp_num = 192;
            } else {
                tmp_num = total_num;
            }

            data_len = 1200;
            printf("Sending uc packet...\n");
            BCM_IF_ERROR_RETURN( pstats_data_form(data_buf, &data_len, ts, tmp_data, tmp_num) );
            BCM_IF_ERROR_RETURN( tx_pkt(unit, ing_port1, 1, data_buf, data_len) );

            tmp_data += tmp_num;
            total_num -= tmp_num;
            pkt_count++;
        } while (total_num);
    }
    sal_sleep(2);
    printf("pkt count %d\n", pkt_count);

    return BCM_E_NONE;
}
//////////////////////////////////////////////////////////////

void print_pstats_data(bcm_pstats_data_t *ps_data)
{
    int i;
    
    printf("array_count: %d\n", ps_data->array_count);
    
    for(i=0; i<BCM_PSTATS_BUFFER_COUNT; ++i)
    {
        printf("%02X ", ps_data->use_counts_array[i]);
    }
    printf("\n");
    
    return;
}

int do_pstats_config(int cos_mode) {
    int                             ele_num=0, port_count=0;
    /* config.bcm bcm_num_cos default is 8 */
    int                             cos_num;//cos_mode ? 8 : 10;
    bcm_port_config_t               port_config;
    
    //bshell(unit, "config add buffer_stats_collect_mode=0x0");
    //bshell(unit, "config add buffer_stats_collect_type=0x2");
    //bshell(unit, "config save");
    bbshell(unit, "config show stats");
    
    printf("Doing gport traverse..\n");
    BCM_IF_ERROR_RETURN( bcm_cosq_gport_traverse(unit, gport_callback, NULL) );
    
    cos_num = uc_cosq_num;
    
    printf("uc_cosq_num = %d\n", uc_cosq_num);
    printf("mc_cosq_num = %d\n", mc_cosq_num);
    printf("gp_cosq_num = %d\n", gp_cosq_num);

    /* get all port bitmap */
    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &port_config));
    BCM_PBMP_COUNT(port_config.port, port_count);
    printf("Total ports available is %d\n", port_count);
    
    /* ing pool + egr pool + ucq */
    ele_num = 4 + 4 + port_count * cos_num;

    bcm_pstats_session_id_t         sid;
    bcm_pstats_session_element_t    ele[ele_num], ele_get[ele_num];
    bcm_pstats_timestamp_t          ts;
    bcm_pstats_data_t               data[ele_num];
    int                             array_count;
    int                             i, count = 0;
    int                             port;

    bcm_pstats_data_t               *ip_data = &data[count];
    int                             ip_num = 4;
    for (i = 0; i < 4; i++) {
        if (count > ele_num) {
            printf("#########################111111\n");
            return BCM_E_PARAM;
        }
        ele[count].type = bcmPStatsSessionIngPool;
        ele[count].pool = i;
        count ++;
    }

    bcm_pstats_data_t               *ep_data = &data[count];
    int                             ep_num = 4;
    for (i = 0; i < 4; i++) {
        if (count > ele_num) {
            printf("#########################222222\n");
            return BCM_E_PARAM;
        }
        ele[count].type = bcmPStatsSessionEgrPool;
        ele[count].pool = i;
        count ++;
    }
    
    //printf("HERE %d %d %d\n", count, cos_num, uc_cosq_num);
    bcm_pstats_data_t               *uc_data = &data[count];
    int                             uc_num = port_count * cos_num;
    BCM_PBMP_ITER(port_config.port, port) {
        
        if( port >= MAX_PORTS )
        {
            printf("Reaching the port array limit %d vs %d\n", port, MAX_PORTS);
            break;
        }
        if (cos_mode == 1) {
            
            for (i = 0; i < cos_num; i++) {
                if (count > ele_num) {
                    return BCM_E_PARAM;
                }
                ele[count].type = bcmPStatsSessionUnicastQueue;
                ele[count].gport = port;
                ele[count].cosq = i;
                count ++;
            }
        }
        else
        {
            for (i = 0; i < cos_num; i++) {
                if (count > ele_num) {
                    return BCM_E_PARAM;
                }
                ele[count].type = bcmPStatsSessionUnicastQueue;
                ele[count].gport = cosq_gp_uc[port][i];
                count ++;
            }
        }
    }
    
    printf("total ele num = %d   %d\n", count, ele_num);

    printf("Doing pstat session create...\n");
    BCM_IF_ERROR_RETURN(bcm_pstats_session_create(unit, 0, ele_num, ele, &sid));
    printf("sid = %d\n", sid);

    printf("Sending burst of packets from CPU...\n");
    char cmd[2*1024];
    sprintf(cmd, "tx 10000 pbm=%d", ing_port2);
    bbshell(unit, cmd);
    
    if( 0 )
    {
        printf("Doing pstat session get...\n");
        BCM_IF_ERROR_RETURN(
                bcm_pstats_session_get(unit, sid, ele_num, ele_get, &array_count));
        printf("ele array count %d\n", array_count);
    }

    printf("Doing pstat data sync...\n");
    BCM_IF_ERROR_RETURN(bcm_pstats_data_sync(unit));

    printf("Doing pstat session data get...\n");
    BCM_IF_ERROR_RETURN(
            bcm_pstats_session_data_get(unit, sid, &ts, ele_num, data, &array_count));
    printf("timestamp \n");
    print ts;
    printf("data array count %d\n", array_count);
    
    if( 0 )
    {
        printf("++++++++  IP DATA   ++++++++ %d\n", ip_num);
        for(i=0; i<ip_num; ++i)
        {
            print_pstats_data(&ip_data[i]);
        }
        printf("++++++++  EP DATA   ++++++++ %d\n", ep_num);
        for(i=0; i<ep_num; ++i)
        {
            print_pstats_data(&ep_data[i]);
        }
        printf("++++++++  UC DATA   ++++++++ %d\n", uc_num);
        for(i=0; i<uc_num; ++i)
        {
            printf(">>> %d\n", i);
            print_pstats_data(&uc_data[i]);
        }
    }

    printf("Building packet header...\n");
    BCM_IF_ERROR_RETURN( pstats_build_packet_header(unit, ts) );
    printf("Building packet header...\n");
    BCM_IF_ERROR_RETURN( pstats_packet_tx(ts, ip_data, ip_num, ep_data, ep_num, uc_data, uc_num) );
    
    if( 1 )
    {
        printf("Doing clear session...%d\n", sid);
        BCM_IF_ERROR_RETURN( bcm_pstats_session_data_clear(unit, sid) );

        printf("Doing destroy session...\n");
        BCM_IF_ERROR_RETURN( bcm_pstats_session_destroy(unit, sid) );
    }

    return BCM_E_NONE;
}

/* 
 * This functions gets the port numbers and sets up ingress and 
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
    int rv;
    
    rv = ingress_port_setup(unit, ing_port1, BCM_PORT_LOOPBACK_MAC);
    if ( rv != BCM_E_NONE ) {
        printf("ingress_port_setup() failed for port %d (Error: %d)\n", ing_port1, rv);
        return rv;
    }

    rv = egress_port_setup(unit, egr_port, BCM_PORT_LOOPBACK_MAC);
    if ( rv != BCM_E_NONE ) {
        printf("egress_port_setup() failed for port %d (Error: %d)\n", egr_port, rv);
        return rv;
    }

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port2, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ing_port2, BCM_PORT_DISCARD_ALL));

    bshell(unit, "pw start report +raw +decode");

    return BCM_E_NONE;
}

int rrv;


bcm_error_t verify()
{
    //char cmd[2*1024];
    
    printf("\n\n\t Expected Result: You will see encapsulated packet in proper format...\n\n");
    
    return BCM_E_NONE;
}


bcm_error_t execute()
{
    if( (rrv = test_setup(unit)) != BCM_E_NONE )
    {
        printf("Creating the test setup failed %d\n", rrv);
        return rrv;
    }
    
    if( (rrv = do_pstats_config(0)) != BCM_E_NONE )
    {
        printf("Configuring pstats failed with %d\n", rrv);
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
  print "execute(): Start";
  print execute();
  print "execute(): End";
}

