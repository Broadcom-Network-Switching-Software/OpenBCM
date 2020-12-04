/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : Mirroring, sFLow, PSAMP and Payload Wiping
 *
 *  Usage    : BCM.0> cint mirror_sFlow_PSAMP.c [YES] [IpType] [TunnelType]
 *                                                                 [AddSequence] [PayloadWiping]
 *             IpType:
 *                  0: Ipv4
 *                  1: Ipv6
 *             TunnelType:
 *                  0: No Tunnel
 *                  1: Erspan type tunnel
 *                  2: Sflow type tunnel
 *                  3: PSAMP type tunnel
 *             AddSequence:
 *                  0: No sflow sequence
 *                  1: Add sflow sequence if sflow/psamp is selected
 *             PayloadWiping: (Not supported on Maverick2)
 *                  0: No payload wiping
 *                  1: Wipe off payload in the tunneled mirrored packet
 *  E.g.
 *          cint mirror_sFlow_PSAMP.c YES 0 2 1 1       <<<< sFlow
 *          cint mirror_sFlow_PSAMP.c YES 0 3 1 1       <<<< PSAMP
 *
 *  config   : mirroring_config.bcm
 *
 *  Log file : mirror_sFlow_PSAMP_log.txt
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
 *  +----------------+                              |
 *                   |                              |
 *                   |                              |
 *                   +------------------------------+
 *
 *  Summary:
 *  ========
 *      MIRRORING of the packet ingressing at ing_port1. Mirrored packet will egress from
 *      mtp_port. While the switched packet will egress from ing_port1
 *
 *  Detailed steps done in the CINT script:
 *  ====================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ================================
 *       a) Put ing_port1, ing_port2, ing_port3 and mtp_port in loopback.
 *       b) Except ing_port1, all ports are DISCARD ALL.
 *       c) Install IFP entries to catch the ingress (on ing_port1) and egress packets
 *          (on mtp_port1) for visibility.
 *
 *    2) Step2 - Configuration (Done in ConfigureMirror()).
 *    ===================================================
 *       a) Configure Payload wiping HW to match and choose wiping begin offset
 *          UDP Dest port is selected to match on.
 *       b) Configure Mirror Port with Tunnel. Either sFlow and ERSPAN can
 *          be selected.
 *
 *    2) Step3 - Verification (Done in verify())
 *    ==============================
 *       a) Send unicast L3 UDP packet to ing_port1 from CPU. mtp_port will egress tunneled mirrored
 *          packet.
 *       b) Expected Result:
 *          ================
 *          Packet is mirrored with tunnel headers of sFlow/PSAMP/ERSPAN. IP can be v4 or v6.
 *          Payload will be wiped off. Payload content is wiped right after the Dest IP from
 *          an offset as specified in the corresponding UDP match.
 *

PSAMP Time Diff capture:
{0000 07d2}-(Export Time (T-1))
{0000 07d2 0046 1f55}->(Observation time ns (T-1)
---
{0000 07d3}-(Export Time (T))
{0000 07d3 0046 6da6}->(Observation time ns (T))

PSAMP DECODE:
{000000445566}->(Tunnel DMAC) {000000112233}->(Tunnel SMAC)
{8100 006f}->(VLAN TAG)
{0800}->(Ether type IPv4) 4500 009c 0000 0000 4011 4e24
{0a0a 0a0b}-(Src IP) {0c0c 0c0d}->(Dst IP)
{aaaa}->(Src Port) {bbbb}->(Dst Port)
{0088}->(Len)
{0000}->(UDP Chksum)
{{000a}->(IPFIX Version) {0080}-(IPFIX Len)
{0000 07d3}-(Export Time (T)) {0000 0002}(Seq Num)
{0000 0000}->(Observation Domain ID)
{0000}->(Template ID)
{0070}->(PSAMP Len)
{0000 07d3 0046 6da6}->(Observation time ns (T))
{00ff}->(PAD + FF)
{0064}->(PSAMP Sampled Len)}->(PSAMP Hdr)
{0000 
0000 00dd 0000 0000 00cc 8100 0064 0800 
4500 004a 0000 0000 4011 3e66 1010 1011 
2020 2021 5555 6666 0036 c6b0 0001 0203 
0405 0607 0809 0a0b 0c0d 0e0f 1011 1213 
1415 1617 1819 1a1b 1c1d 1e1f 2021 2223 
2425 2627 2829 2a2b 2c2d 2919 7737 4ca9 
485e}->(Payload Packet)

SAMPLE DECODE OF PSAMP PACKET:
{000000445566}->(Tunnel DMAC) {000000112233}->(Tunnel SMAC)
{8100 006f}->(VLAN TAG)
{86dd}->(Ether type IPv6) 6000 0000 0068 1140 {fe80 0000 0000
0000 0000 0000 0000 2d6e}->(Src IP6) {ff02 0000 0000
0000 0000 0000 0000 0005}-(Dst IP6) {2b67}-(Src L4 Port) {56ce}->(Dst L4 Port)
{0068}->(Len)
{0000}->(UDP Chksm, always 0) {{000a}->(IPFIX Version) {0060}->(IPFIX Len)
{0000 0000}->(Export Time) {0000 0000}->(Seq Num) {0000
0000}->(Observation Domain ID) {0000}->(Template ID) {0050}->(PSAMP Len)
{0000 0000 0000 0000}->(Observation time ns) {00ff}->(PAD + FF)
{0044}->(PSAMP Sampled Len)}->(PSAMP Hdr) {0000 0000 00c4 0001 0203 0405 8100
00c8 002e 0000 0000 0000 0000 1234 5678
1234 5679 1234 567a 1234 567b 1234 567c
1234 567d 1234 567e 1234 567f 1234 5680
1234 0000 0000}->(Payload Packet)


SAMPLE DECODE OF SFLOW PACKET:
{000000445566}->(Tunnel DMAC) {000000112233}->(Tunnel SMAC)
{8100 006f}->(VLAN TAG)
{0800 4500 0068 0000 0000 4011 7480 0101
0101 0202 0202 2b67 56ce 0054 0000 }->(IP+UDP Header)
{ {2900}->(Source Port+ModID)
{2a00}->(Dest Port+ModID) {8080}->(sFLow Flags) {0000}->(Metadata)
{0000 0000}->(Seq Num) }->(sFlow HDR) {0000 0000 00c4
0001 0203 0405 8100 00c8 002e 0000 0000
0000 0000 1234 5678 1234 5679 1234 567a
1234 567b 1234 567c 1234 567d 1234 567e
1234 567f 1234 5680 1234 0000 0000}->(Payload Packet)

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
  printf("B_CM.%d> %s\n", unit, str);                                      /* */
  bshell(unit, str);                                                       /* */
}                                                                          /* */
/****************   GENERIC UTILITY FUNCTIONS ENDS  ****************************
 */

/********   Test variants **************/
int opt_SelectIpv6 = 0;

// At most one opt_* can be set to 1 below
int opt_DoSflowTunnel = 0;
int opt_DoPsampTunnel = 1;
int opt_DoErspanTunnel = 0;

int opt_AddSeqToTunnelPkt = 0;

/* Payload wiping is not supported for Maverick2 */
int opt_DoPayloadWiping = 0;

// TH3 has blackhawk (BH) cores
int opt_IsPortBlackHawk = 1;

int opt_IsDevTomahawk3 = 1;

/****************************************/


if( ARGC >= 2 ) {
    if( *ARGV[1] == '1' ) {
        opt_SelectIpv6 = 1;
    } else {
        opt_SelectIpv6 = 0;
    }
}
if( ARGC >= 3 ) {
    if( *ARGV[2] == '0' ) {
        opt_DoSflowTunnel  = 0;
        opt_DoPsampTunnel  = 0;
        opt_DoErspanTunnel = 0;
    } else if( *ARGV[2] == '1' ) {
        opt_DoErspanTunnel = 1;
        opt_DoSflowTunnel  = 0;
        opt_DoPsampTunnel  = 0;
    } else if( *ARGV[2] == '2' ) {
        opt_DoErspanTunnel = 0;
        opt_DoSflowTunnel  = 1;
        opt_DoPsampTunnel  = 0;
    } else if( *ARGV[2] == '3' ) {
        opt_DoErspanTunnel = 0;
        opt_DoSflowTunnel  = 0;
        opt_DoPsampTunnel  = 1;
    }
}
if( ARGC >= 4 ) {
    if( *ARGV[3] == '1' ) {
        opt_AddSeqToTunnelPkt = 1;
    } else {
        opt_AddSeqToTunnelPkt = 0;
    }
}
if( ARGC >= 5 ) {
    if( *ARGV[4] == '1' ) {
        opt_DoPayloadWiping = 1;    // Set it to 0 as Maverick2 does not support Payload Wiping.
    } else {
        opt_DoPayloadWiping = 0;
    }
}

// Settings for Maverick2
opt_DoPayloadWiping = 0;
opt_IsPortBlackHawk = 0;
opt_IsDevTomahawk3 = 0;

print opt_SelectIpv6;

print opt_DoSflowTunnel;
print opt_DoPsampTunnel;
print opt_DoErspanTunnel;

print opt_AddSeqToTunnelPkt;

print opt_DoPayloadWiping;

int unit=0;


bcm_port_t ing_port1 = PORT_ANY; // Set to a valid port if known already.
bcm_port_t ing_port2 = PORT_ANY;
bcm_port_t ing_port3 = PORT_ANY;

bcm_port_t mtp_port = PORT_ANY;

/* Populate the ports */
ing_port1 = portGetNextE(unit, ing_port1);
ing_port2 = portGetNextE(unit, ing_port2);
ing_port3 = portGetNextE(unit, ing_port3);

mtp_port = portGetNextE(unit, mtp_port);

print ing_port1;
print ing_port2;
print ing_port3;

print mtp_port;

////    PLEASE NOTE THAT BROADCOM SWITCH DO NOT CHECK FOR   ////
////    THE VALIDITY L3 CHECKSUM FIELD AT ALL.              ////

////////    TUNNEL HEADER's    /////////////
uint16      tunnel_tpid = 0x8100;
uint8       tunnel_ttl  = 0x40;

bcm_ip6_t   tunnel_dip6 = {0xFF,0x02,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
                            0x0,0x0,0x0,0x0,0x0,0x5};
bcm_ip6_t   tunnel_sip6 = {0xFE,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
                            0x0,0x0,0x0,0x0,0x2D,0x6E};

bcm_ip_t    tunnel_sip = 0x0A0A0A0B;
bcm_ip_t    tunnel_dip = 0x0C0C0C0D;

uint16      tunnel_udp_src_port = 0xAAAA;
uint16      tunnel_udp_dst_port = 0xBBBB;

bcm_mac_t   tunnel_smac = {0x00, 0x00, 0x00, 0x11, 0x22, 0x33};
bcm_mac_t   tunnel_dmac = {0x00, 0x00, 0x00, 0x44, 0x55, 0x66};
uint32      mirror_flags = 0;

bcm_vlan_t  tunnel_vid = 0x6F;    // 111

////////    PAYLOAD's       ///////////

bcm_mac_t   test_dmac = {0x00, 0x00, 0x00, 0x00, 0x00, 0xDD};
bcm_mac_t   test_smac = {0x00, 0x00, 0x00, 0x00, 0x00, 0xCC};   // DONTCARE

bcm_vlan_t  test_vid = 0x64;

bcm_ip_t    payload_sip = 0x10101011;   // DONTCARE
bcm_ip_t    payload_dip = 0x20202021;   // DONTCARE

// matching with payload wiping engine
uint16      payload_udp_src_port1 = 0x1111;
uint16      payload_udp_dst_port1 = 0x2222;

// matching with payload wiping engine
uint16      payload_udp_src_port2 = 0x3333;
uint16      payload_udp_dst_port2 = 0x4444;

// NO entry in wiping engine
uint16      payload_udp_src_port3 = 0x5555;
uint16      payload_udp_dst_port3 = 0x6666;

uint64 tmp_hack_sec;
int tmp_hack_sec_lower = 2000;
uint64 tmp_hack_nsec;
int tmp_hack_nsec_lower = 3000;

bcm_error_t UpdateYCounter()
{
    bcm_time_ts_counter_t counter;
    bcm_time_tod_t tod;
    uint64 secs, nano_secs, div;
    uint32 stages;
    
    bcm_time_ts_counter_get(unit, &counter);
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

    bcm_time_tod_set(unit, stages, &tod);
    //print tod;

    bshell(unit, "d EGR_TS_UTC_CONVERSION");
    if( opt_IsDevTomahawk3 )
    {
        bshell(unit, "d EGR_TS_UTC_CONVERSION_2");
    }
    
    return BCM_E_NONE;
    
}

int Th3TimeConfigSet(bcm_port_t ing_port, bcm_port_t egr_port, int is_port_bh) {

    bcm_time_init(0);
    bcm_time_interface_t intf;
    intf.id = 0;
    intf.flags = BCM_TIME_ENABLE | BCM_TIME_WITH_ID;
    int rv;
    
    rv = bcm_time_interface_add(0, intf);
    
    bcm_time_capture_t time = {0};
    
    time.flags = BCM_TIME_CAPTURE_IMMEDIATE;
    print bcm_time_capture_get(0, 0, &time);
    bcm_time_ts_counter_t counter;
    
    COMPILER_64_SET(tmp_hack_sec, 0, tmp_hack_sec_lower);
    COMPILER_64_SET(tmp_hack_nsec, 0, tmp_hack_nsec_lower);
    
    print rv;
    if( is_port_bh ) {

        uint64 ctl;
            
        COMPILER_64_SET(ctl, 0, 0);
            
        printf("Doing timesync related settings...\n");
        BCM_IF_ERROR_RETURN( bcm_port_enable_set(unit, ing_port, FALSE) );
        BCM_IF_ERROR_RETURN( bcm_port_enable_set(unit, egr_port, FALSE) );
            
        BCM_IF_ERROR_RETURN( bcm_port_control_phy_timesync_set(unit, ing_port,
                    bcmPortControlPhyTimesyncOneStepEnable, ctl) );
        BCM_IF_ERROR_RETURN( bcm_port_control_phy_timesync_set(unit, egr_port,
                    bcmPortControlPhyTimesyncOneStepEnable, ctl) );
             
        sal_sleep(1);
            
        BCM_IF_ERROR_RETURN( bcm_port_enable_set(unit, ing_port, TRUE) );
        BCM_IF_ERROR_RETURN( bcm_port_enable_set(unit, egr_port, TRUE) );
            
        sal_sleep(2);   // ports would have comeup by now.
            
        COMPILER_64_SET(ctl, 0, 0);
        BCM_IF_ERROR_RETURN( bcm_port_control_phy_timesync_set(unit, ing_port,
                bcmPortControlPhyTimesyncTimestampAdjust, ctl) );
        BCM_IF_ERROR_RETURN( bcm_port_control_phy_timesync_set(unit, egr_port,
                bcmPortControlPhyTimesyncTimestampAdjust, ctl) );
    }
    
    bcm_time_tod_t tod;
    uint64 secs, nano_secs, div;
    uint32 stages;

    bcm_time_ts_counter_get(0, &counter);
    print counter;
    stages = BCM_TIME_STAGE_EGRESS;
    tod.time_format = counter.time_format;
    print counter;
    tod.ts_adjustment_counter_ns = counter.ts_counter_ns;

    /* Get seconds from the timestamp value */
    secs = counter.ts_counter_ns;
    COMPILER_64_SET(div, 0, 1000000000);
    COMPILER_64_UDIV_64(secs, div);
    tod.ts.seconds = secs;
    /* Get nanoseconds from the timestamp value */
    nano_secs = counter.ts_counter_ns;
    COMPILER_64_UMUL_32(secs, 1000000000);
    COMPILER_64_SUB_64(nano_secs, secs);
    tod.ts.nanoseconds = COMPILER_64_LO(nano_secs);
    bcm_time_tod_set(0, stages, &tod);
    print tod;
    return BCM_E_NONE;
}


/////////////////////  Payload wiping
/*
    This function configures payload wiping offsets for the mirrored packet
        of 3 type of ingressed packets, each
    1. UDP packet of port # udp_portd1(63) : From 18th byte
    2. UDP packet of port # udp_portd2(64) : From 22th byte
    3. Any IP Packet : From 30th byte
    
    Function will also setup the mirror encapsulation. SFLOW encap or
    ERSPAN encap.
    
*/
bcm_mac_t dmac;
bcm_error_t ConfigureMirror()
{
    if( 1 )
    {
        bcm_pbmp_t     vlan_pbmp;
        bcm_pbmp_t     ut_pbmp;

        BCM_PBMP_CLEAR(vlan_pbmp);
        BCM_PBMP_PORT_ADD(vlan_pbmp, ing_port1);
        BCM_PBMP_PORT_ADD(vlan_pbmp, ing_port2);
        BCM_PBMP_PORT_ADD(vlan_pbmp, ing_port3);
        //BCM_PBMP_PORT_ADD(vlan_pbmp, mtp_port);     // MTP port need not be part of the VLAN

        BCM_PBMP_CLEAR(ut_pbmp);
        BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, test_vid));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, test_vid, vlan_pbmp, ut_pbmp));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, 1, vlan_pbmp)); // remove from default vlan
    }
    
    if( opt_DoPsampTunnel )
    {
        BCM_IF_ERROR_RETURN(Th3TimeConfigSet(ing_port1, mtp_port, opt_IsPortBlackHawk));
    }
    
    if( 1 )
    {
        bcm_l2_addr_t l2_addr;

        bcm_l2_addr_t_init(&l2_addr, test_dmac, test_vid);
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = ing_port2;
        printf("Doing L2 entry add\n");
        BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));
    }
    
    printf("Doing init of mirror module\n");
    BCM_IF_ERROR_RETURN(bcm_init_selective(unit, BCM_MODULE_MIRROR));
    printf("Doing switch control enable for DirectedMirroring\n");
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchDirectedMirroring, 1));

    if( 1 )
    {
        printf("Doing port control set for port %d\n", ing_port1);
        BCM_IF_ERROR_RETURN(bcm_port_control_set(0, ing_port1,
                                    bcmPortControlSampleIngressEnable, 1));
        BCM_IF_ERROR_RETURN(bcm_port_control_set(0, ing_port1,
                                    bcmPortControlSampleIngressDest,
                                    BCM_PORT_CONTROL_SAMPLE_DEST_MIRROR));
        printf("Doing sample rate set for port %d\n", ing_port1);
        BCM_IF_ERROR_RETURN(bcm_port_control_set(0, ing_port1,
                                    bcmPortControlSampleIngressRate, 100));
        BCM_IF_ERROR_RETURN(bcm_port_control_set(0, ing_port1,
                                    bcmPortControlSampleEgressRate, 0));
    }
    
    if( opt_DoPayloadWiping )
    {
        int i = 0;
        bcm_mirror_payload_zero_info_t         payload_zero_config_array[4];

        //for(i=0; i<4; ++i)
        //{
        //    bcm_mirror_payload_zero_info_t_init(&payload_zero_config_array[i]);
        //}
        i = 0;
        if( 1 )
        {
            payload_zero_config_array[i].field_id = bcmMirrorPayloadZeroUdpPort0;
            payload_zero_config_array[i].protocol_port_value = payload_udp_dst_port1;
            // 8 * 2 = 16 bytes starting from end of DIP in payload. i.e. start of UDP port
            payload_zero_config_array[i].protocol_port_offset = 8;
            i++;
        }
        if( 1 )
        {
            payload_zero_config_array[i].field_id = bcmMirrorPayloadZeroUdpPort1;
            payload_zero_config_array[i].protocol_port_value = payload_udp_dst_port2;
            payload_zero_config_array[i].protocol_port_offset = 12;
            i++;
        }
        if( 1 )
        {
            // Default offset for L3 packets
            payload_zero_config_array[i].field_id = bcmMirrorPayloadZeroL3Offset;
            payload_zero_config_array[i].protocol_port_value = 0;    // Not applicable
            payload_zero_config_array[i].protocol_port_offset = 16;    // A Default offset
            i++;
        }
        printf("Doing ZERO PAYLOAD configuration %d\n", i);
        BCM_IF_ERROR_RETURN( bcm_mirror_payload_zero_control_multi_set(unit, &payload_zero_config_array, i));
    }

    /*  configure 100% ingress sampling on bcm port number ing_port1 */
    //print bcm_port_sample_rate_set(0, ing_port1, 1, 0);
    //print bcm_port_sample_rate_set(0, ing_port2, 1, 0);

    //print bcm_switch_control_set(0, bcmSwitchL3EgressMode, 1);

    bcm_gport_t gport_dest;
    /* configure MTP port. In this example it is port mtp_port */
    bcm_port_gport_get(unit, mtp_port, &gport_dest);

    bcm_mirror_destination_t mdest;

    bcm_mirror_destination_t_init(&mdest);


    if( opt_DoSflowTunnel )
    {
        mdest.gport = gport_dest;
        /* Configure sflow encapsulation. Packet format is as below */
        /* DMAC + SMAC + VLAN HEADER + IP HEADER + UDP HEADER + Sampled packet data */
        sal_memcpy(&mdest.src_mac, tunnel_smac, 6);
        sal_memcpy(&mdest.dst_mac, tunnel_dmac, 6);
        if( opt_SelectIpv6 )
        {
            mdest.version = 6;
            mdest.dst6_addr = tunnel_dip6;
            mdest.src6_addr = tunnel_sip6;
        }
        else
        {
            mdest.version = 4;
            mdest.src_addr = tunnel_sip;
            mdest.dst_addr = tunnel_dip;
        }

        mdest.udp_src_port = tunnel_udp_src_port;
        mdest.udp_dst_port = tunnel_udp_dst_port;
        mdest.ttl = tunnel_ttl;
        //mdest.flow_label = 0xFDEA9;
        mdest.vlan_id = tunnel_vid;
        mdest.tpid = tunnel_tpid;
        mdest.flags |= BCM_MIRROR_DEST_TUNNEL_SFLOW;
        mdest.flags |= BCM_MIRROR_DEST_PORT;
        if( opt_AddSeqToTunnelPkt )
        {
            mdest.flags |= BCM_MIRROR_DEST_TUNNEL_WITH_SEQ;    // if ver >= 6.5.13
        }
        //mdest.egress_packet_copy_size = 256;
        //mdest.packet_copy_size = 64;
        // BCM_MIRROR_PAYLOAD_TRUNCATE_AND_ZERO not supported on Maverick 2
        if( opt_DoPayloadWiping )
        {
            mdest.truncate = BCM_MIRROR_PAYLOAD_TRUNCATE_AND_ZERO;    // BCM_MIRROR_PAYLOAD_TRUNCATE
        }
    }
    else if( opt_DoPsampTunnel )
    {
        mdest.gport = gport_dest;
        /* Configure sflow encapsulation. Packet format is as below */
        /* DMAC + SMAC + VLAN HEADER + IP HEADER + UDP HEADER + Sampled packet data */
        sal_memcpy(&mdest.src_mac, tunnel_smac, 6);
        sal_memcpy(&mdest.dst_mac, tunnel_dmac, 6);
        if( opt_SelectIpv6 )
        {
            mdest.version = 6;
            mdest.dst6_addr = tunnel_dip6;
            mdest.src6_addr = tunnel_sip6;
        }
        else
        {
            mdest.version = 4;
            mdest.src_addr = tunnel_sip;
            mdest.dst_addr = tunnel_dip;
        }

        mdest.udp_src_port = tunnel_udp_src_port;
        mdest.udp_dst_port = tunnel_udp_dst_port;
        mdest.ttl = tunnel_ttl;
        //mdest.flow_label = 0xFDEA9;
        mdest.vlan_id = tunnel_vid;
        mdest.tpid = tunnel_tpid;
        mdest.flags |= BCM_MIRROR_DEST_TUNNEL_PSAMP;
        mdest.flags |= BCM_MIRROR_DEST_PORT;
        if( opt_AddSeqToTunnelPkt )
        {
            mdest.flags |= BCM_MIRROR_DEST_TUNNEL_WITH_SEQ;    // if ver >= 6.5.13
        }
        //mdest.egress_packet_copy_size = 256;
        //mdest.packet_copy_size = 64;
        if( opt_DoPayloadWiping )
        {
            mdest.truncate = BCM_MIRROR_PAYLOAD_TRUNCATE_AND_ZERO;    // BCM_MIRROR_PAYLOAD_TRUNCATE
        }
    }
    else if( opt_DoErspanTunnel )
    {
        mdest.gport = gport_dest;
        /* Configure sflow encapsulation. Packet format is as below */
        /* DMAC + SMAC + VLAN HEADER + IP HEADER + UDP HEADER + Sampled packet data */
        sal_memcpy(&mdest.src_mac, tunnel_smac, 6);
        sal_memcpy(&mdest.dst_mac, tunnel_dmac, 6);
        if( opt_SelectIpv6 )
        {
            mdest.version = 6;
            mdest.dst6_addr = tunnel_dip6;
            mdest.src6_addr = tunnel_sip6;
        }
        else
        {
            mdest.version = 4;
            mdest.src_addr = tunnel_sip;
            mdest.dst_addr = tunnel_dip;
        }

        mdest.udp_src_port = tunnel_udp_src_port;
        mdest.udp_dst_port = tunnel_udp_dst_port;
        mdest.ttl = tunnel_ttl;
        //mdest.flow_label = 0xFDEA9;
        mdest.vlan_id = tunnel_vid;
        mdest.tpid = tunnel_tpid;
        mdest.flags |= BCM_MIRROR_DEST_TUNNEL_IP_GRE;
        mdest.flags |= BCM_MIRROR_DEST_PORT;
        if( opt_AddSeqToTunnelPkt )
        {
            mdest.flags |= BCM_MIRROR_DEST_TUNNEL_WITH_SEQ;    // if ver >= 6.5.13
        }
        //mdest.egress_packet_copy_size = 256;
        //mdest.packet_copy_size = 64;
        if( opt_DoPayloadWiping )
        {
            mdest.truncate = BCM_MIRROR_PAYLOAD_TRUNCATE_AND_ZERO;    // BCM_MIRROR_PAYLOAD_TRUNCATE
        }
    }
    else
    {
        mdest.flags = 0;
        mdest.gport = gport_dest;
    }

    printf("Doing mirror dest create\n");
    BCM_IF_ERROR_RETURN(bcm_mirror_destination_create(unit, &mdest));

    mirror_flags = 0;
    //mirror_flags |= BCM_MIRROR_PORT_SFLOW;
    mirror_flags |= BCM_MIRROR_PORT_INGRESS;
    //mirror_flags |= BCM_MIRROR_PORT_EGRESS;
    /* Set the MTP port and tunnel attributes to ingress sflow port */
    printf("Doing MTP Add\n");
    BCM_IF_ERROR_RETURN(bcm_mirror_port_dest_add(unit, ing_port1, mirror_flags, mdest.mirror_dest_id));

    return BCM_E_NONE;

}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

/* 
 * This functions gets the port numbers and sets up ingress and 
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
    int rv;
    
    int loopBackType = BCM_PORT_LOOPBACK_PHY;
    
    if( 1 )
    {
        rv = ingress_port_setup(unit, ing_port1, loopBackType);
        if ( rv != BCM_E_NONE ) {
            printf("ingress_port_setup() failed for port %d (Error: %d)\n", ing_port1, rv);
            return rv;
        }

        rv = egress_port_setup(unit, mtp_port, loopBackType);
        if ( rv != BCM_E_NONE ) {
            printf("egress_port_setup() failed for port %d (Error: %d)\n", mtp_port, rv);
            return rv;
        }
    }
    else
    {
        printf("Doing port loopbacks...\n");
        
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port1, loopBackType));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ing_port1, BCM_PORT_DISCARD_NONE));
        
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, mtp_port, loopBackType));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, mtp_port, BCM_PORT_DISCARD_ALL));
    }

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port2, loopBackType));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ing_port2, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port3, loopBackType));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ing_port3, BCM_PORT_DISCARD_ALL));

    //bshell(unit, "pw start report +raw +decode dma");
    bshell(unit, "pw start");

    return BCM_E_NONE;
}

int rrv;

/*
    Packet should get wiped to 0x00 starting from the offset (x * 2) starting from the UDP header as base.
    Where 'x' is the offset applicable to the type of packet as programmed.
*/
bcm_error_t verify()
{
    char cmd[2*1024];
    
    if( opt_DoPsampTunnel )
    {
        BCM_IF_ERROR_RETURN( UpdateYCounter() );
    }

    if( 1 )
    {
        printf("##########   Sending L3 packet ########\n");
        sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X08004500004A0000000040113E66%08X%08X%04X%04X0036C6B0000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D29197737CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC2222222222222222222222",
            ing_port1,
            test_dmac[0], test_dmac[1], test_dmac[2],
            test_dmac[3], test_dmac[4], test_dmac[5],
            test_smac[0], test_smac[1], test_smac[2],
            test_smac[3], test_smac[4], test_smac[5],
            test_vid, payload_sip, payload_dip,
            payload_udp_src_port1,
            payload_udp_dst_port1 );
        bbshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 1);
        bbshell(unit, cmd);
        printf("#####################################################\n\n");
    }
    
    if( opt_DoPsampTunnel )
    {
        BCM_IF_ERROR_RETURN( UpdateYCounter() );
    }
    
    if( 1 )
    {
        printf("##########   Sending L3 packet ########\n");
        sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X08004500004A0000000040113E66%08X%08X%04X%04X0036C6B0000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D29197737CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC2222222222222222222222",
            ing_port1,
            test_dmac[0], test_dmac[1], test_dmac[2],
            test_dmac[3], test_dmac[4], test_dmac[5],
            test_smac[0], test_smac[1], test_smac[2],
            test_smac[3], test_smac[4], test_smac[5],
            test_vid, payload_sip, payload_dip,
            payload_udp_src_port2,
            payload_udp_dst_port2 );
        bbshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 1);
        bbshell(unit, cmd);
        printf("#####################################################\n\n");
    }
    
    if( opt_DoPsampTunnel )
    {
        BCM_IF_ERROR_RETURN( UpdateYCounter() );
    }
    
    if( 1 )
    {
        printf("##########   Sending L3 packet ########\n");
        sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X08004500004A0000000040113E66%08X%08X%04X%04X0036C6B0000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D29197737CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC2222222222222222222222",
            ing_port1,
            test_dmac[0], test_dmac[1], test_dmac[2],
            test_dmac[3], test_dmac[4], test_dmac[5],
            test_smac[0], test_smac[1], test_smac[2],
            test_smac[3], test_smac[4], test_smac[5],
            test_vid, payload_sip, payload_dip,
            payload_udp_src_port3,
            payload_udp_dst_port3 );
        bbshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 1);
        bbshell(unit, cmd);
        printf("#####################################################\n\n");
    }
    
    if( opt_DoPsampTunnel )
    {
        BCM_IF_ERROR_RETURN( UpdateYCounter() );
    }
    
    if( opt_DoPayloadWiping )   // Not supported on Maverick2
    {
        //bshell(unit, "sleep 1");
        bbshell(unit, "g chg EGR_MIRROR_ZERO_PAYLOAD_PARSE_CFG_0");    // For matching UDP ports
        bbshell(unit, "g chg EGR_MIRROR_ZERO_PAYLOAD_PARSE_CFG_1");    // For matching UDP ports
        bbshell(unit, "g chg EGR_MIRROR_ZERO_PAYLOAD_PARSE_CFG_2");    // For matching UDP ports
        bbshell(unit, "g chg EGR_MIRROR_ZERO_PAYLOAD_OFFSET_CFG_0");
        bbshell(unit, "g chg EGR_MIRROR_ZERO_PAYLOAD_OFFSET_CFG_1");
        bbshell(unit, "g chg EGR_MIRROR_ZERO_PAYLOAD_OFFSET_CFG_2");
        bbshell(unit, "g chg EGR_MIRROR_ZERO_PAYLOAD_OFFSET_CFG_3");
    }
    
    printf("\n\n\t Expected Result: You will see that Tunnelled packets..\n\n");
    
    return BCM_E_NONE;
}


bcm_error_t execute()
{
    if( (rrv = test_setup(unit)) != BCM_E_NONE )
    {
        printf("Creating the test setup failed %d\n", rrv);
        return rrv;
    }
    
    if( (rrv = ConfigureMirror()) != BCM_E_NONE )
    {
        printf("Configuring mirror failed with %d\n", rrv);
        return rrv;
    }
    
    if( (rrv = verify()) != BCM_E_NONE )
    {
        printf("Verify mirror failed with %d\n", rrv);
        return rrv;
    }
    
    return BCM_E_NONE;
}

char *auto_execute;
if( ARGC >= 1 ) {
    auto_execute =  ARGV[0];
} else {
    auto_execute = "YES";
}

if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
