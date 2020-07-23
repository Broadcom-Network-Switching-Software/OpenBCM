/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : INT over UDP
 *  
 *  Usage    : BCM.0> cint int_over_udp.c
 *  
 *  config   : int_over_udp_config.bcm
 *  
 *  Log file : int_over_udp_log.txt
 *  
 *  Test Topology :
 * +-----------------+            +---------+
 * |                 |            |         |
 * |                 |            |         |
 * |         ingPort <------------+         |
 * |                 |            |         |
 * |                 |            |Traffic  |
 * |     SVK         |            |Generator|
 * |                 |            |         |
 * |                 |            |         |
 * |                 |            |         |
 * |         egrPort +------------>         |
 * |                 |            |         |
 * |                 |            |         |
 * |                 |            |         |
 * +-----------------+            +---------+
 *
 *  Summary:
 *  ========
 *  This CINT script demonstrates how to configure INT over UDP.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select one ingress and one egress port
 *    2) Step2 - Configuration (Done in INT_config())
 *    ===============================================
 *      a) Configures INT on switch and sets up required l3 data path
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *    a) Send wirespeed traffic into xe0
 *       00000200000100000400000008004500
 *       0047000100004011659b0a0000010a00
 *       010a2b671a6f00330de3aaaaaaaabbbb
 *       bbbb01010000ffffffffff0000001fe0
 *       000000000001494e5420554450207061
 *       636b65742e
 * Probe-Header:
 * =============
 * #    0                   1                   2                   3
 * #    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * #   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * #   |                         Probe Marker (1)                      |
 * #   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * #   |                         Probe Marker (2)                      |
 * #   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * #   |   Version     | Message Type  |             Flags             |
 * #   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * #   |                     Telemetry Request Vector                  |
 * #   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * #   |   Hop Limit   |   Hop Count   |         Must Be Zero          |
 * #   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * #   |         Maximum Length        |        Current Length         |
 * #   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * #   |        Sender's Handle        |        Sequence Number        |
 * #   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * #
 * 
 * Metadata Format:
 * =================
 * #    0                   1                   2                   3
 * #    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * #   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * #   |                     Device_ID (32bits)                        |
 * #   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * #   |Temp |Cong     |   Reserved    | IP_TTL(8bits) | Queue(8bits)  |
 * #   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * #   |                  Receive_Seconds(47-16)                       |
 * #   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * #   |  Receive_seconds(15-0)        |  Receive_nano-seconds(31-16)  |
 * #   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * #   |  Receive_nano-seconds(15-00)  | Transmit_nano-Seconds(31-16)  |
 * #   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * #   |  Transmit_nano-seconds(15-00) |          Reserved             |
 * #   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * #   |         Ingress Port          |        Egress Port            |
 * #   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * #   |                           Reserved                            |
 * #   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    * Temp:     Template-Id (3-bits)
 *    * Cong:     Congestion  (5-bits)
 * 
 * * CASE-1:
 * *    Ingress: UDP INT-Probe packet
 * *    Egress : UDP INT-Probe + Metadata packet
 * *    Ingress-packet:
 * *
 * Ingress packet with probe-header on port-1:
 * ================
 * ###[ Ethernet ]###
 *   dst       = 00:00:02:00:00:01
 *   src       = 00:00:04:00:00:00
 *   type      = IPv4
 * ###[ IP ]###
 *   version   = 4
 *   ihl       = None
 *   tos       = 0x0
 *   len       = None
 *   id        = 1
 *   ttl       = 64
 *   proto     = udp
 *   src       = 10.0.0.1
 *   dst       = 10.0.1.10
 * ###[ UDP ]###
 *   sport     = 11111
 *   dport     = 6767
 * ###[ INT_PROBE ]###
 *   Probe_Marker_1= 0xaaaaaaaa
 *   Probe_Marker_2= 0xbbbbbbbb
 *   Version   = 0x1
 *   Msg_Type  = 0x1
 *   Flags     = 0x0
 *   Req_Vector= 0xffffffff
 *   Hop_limit = 0xff
 *   Hop_count = 0x0
 *   Reserved0 = 0x0
 *   Max_length= 0x1fe0
 *   Curr_length= 0x0
 *   Sender_handle= 0x0
 *   Sequence_num= 0x1
 * ###[ Raw ]###
 *   load      = 'INT UDP packet.'
 *
 *    b) Expected Result:
 *    ===================
 *    Captured packet on xe1:
 *    00000200000100000400000008004500
 *    0067000100004011657B0A0000010A00
 *    010A2B671A6F00530000AAAAAAAABBBB
 *    BBBB01010000FFFFFFFFFF0100001FE0
 *    002000000001FEEDF00D01EE40000000
 *    00000000000000020000000000EE0001
 *    0005CCCCCCCC494E5420554450207061
 *    636B65742E6B9E5523
 *    ====================
 * ###[ Ethernet ]###
 *   dst       = 00:00:02:00:00:01
 *   src       = 00:00:04:00:00:00
 *   type      = n_802_1Q
 * ###[ 802.1Q ]###
 *   prio      = 0L
 *   id        = 0L
 *   vlan      = 1L
 *   type      = IPv4
 * ###[ IP ]###
 *   version   = 4L
 *   ihl       = 5L
 *   tos       = 0x0
 *   len       = 103
 *   frag      = 0L
 *   ttl       = 64
 *   proto     = udp
 *   chksum    = 0x657b
 *   src       = 10.0.0.1
 *   dst       = 10.0.1.10
 * ###[ UDP ]###
 *   sport     = 11111
 *   dport     = 6767
 *   len       = 83
 * ###[ INT_PROBE ]###
 *   Probe_Marker_1= 0xaaaaaaaa
 *   Probe_Marker_2= 0xbbbbbbbb
 *   Version   = 0x1
 *   Msg_Type  = 0x1
 *   Flags     = 0x0
 *   Req_Vector= 0xffffffff
 *   Hop_limit = 0xff
 *   Hop_count = 0x1
 *   Reserved0 = 0x0
 *   Max_length= 0x1fe0
 *   Curr_length= 0x20
 *   Sender_handle= 0x0
 *   Sequence_num= 0x1
 * ###[ INT_METADATA ]###
 *   DeviceId  = 0xfeedf00d
 *   Temp_congestion= 0x1
 *   RSVD field (OPAQUE_MD1)= 0xee
 *   IP_TTL    = 0x40
 *   QUEUE_ID  = 0x0
 *   RX_TIMESTAMP_SECs= 0x0
 *   Recv_Secs_1500= 0x2
 *   Recv_Nsec_3116= 0x80f0
 *   Recv_Nsec_1500= 0xc10e
 *   Tx_Nsec_3116= 0x80f0
 *   Tx_Nsec_1500= 0xc3e4
 *   RSVD field (OPAQUE_MD1)= 0xee
 *   Ingess_Port= 0x1
 *   Egress_Port= 0x5
 *   RSVD field (OPAQUE_MD2)= 0xcccccccc
 * ###[ Raw ]###
 *   load      = 'INT UDP packet.'
 */


cint_reset();

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
                                                                           /* */
/****************   GENERIC UTILITY FUNCTIONS ENDS  ****************************
 */
int unit = 0;

bcm_port_t ingPort = PORT_ANY; // Set to a valid port if known already.
bcm_port_t egrPort = PORT_ANY;
/* Populate the ports */
ingPort = portGetNextE(unit, ingPort);
egrPort = portGetNextE(unit, egrPort);

bcm_udf_id_t udf_id_arr[4] = {0};
uint32 chunk_bitmap;
uint32 chunks;
int offset;
int width;
int num_udfs = 2;            
bcm_udf_id_t udf_int_pm1_id;
bcm_udf_id_t udf_int_pm2_id;
bcm_udf_id_t udf_int_rv_id;  

/*
uint8 int_pkt_data[] = {0xaa, 0xaa, 0xaa, 0xaa, 0xbb, 0xbb, 0xbb, 0xbb, 
                        0x01, 0x01, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
                        0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
uint8 int_pkt_mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
                        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
*/
uint32 probe_marker1=0xAAAAAAAA;
uint32 probe_marker2=0xBBBBBBBB;
uint32 probe_mask=0xFFFFFFFF;

bcm_mac_t remote_mac_network = "00:00:00:00:00:02";
bcm_mac_t remote_mac_network1 = "00:00:00:00:00:03";
bcm_mac_t local_mac_network = "00:00:00:00:22:22";

bcm_mac_t pkt_in_src_mac = "00:00:00:00:aa:aa";
bcm_mac_t pkt_in_dst_mac = "00:00:00:00:bb:bb";

bcm_mac_t pkt_ta_src_mac = "00:00:00:00:00:01";
bcm_mac_t pkt_ta_dst_mac = "00:00:00:00:11:11";
bcm_mac_t mac_mask = "ff:ff:ff:ff:ff:ff";

bcm_vlan_t vid_acc = 21;
bcm_vlan_t vid_network = 22;
bcm_vlan_t vid_ta = 23;
bcm_if_t intf_id_acc = 1;
bcm_if_t intf_id_network = 2;
bcm_if_t intf_id_ta = 3;
bcm_if_t intf_id_ta1 = 4;
uint8 ttl = 16;
bcm_vrf_t vrf = 10;
bcm_vrf_t vrf_ta = 20;
bcm_port_t port_ta = 65; /* loop back */

bcm_gport_t gingPort = BCM_GPORT_INVALID;
bcm_gport_t gegrPort = BCM_GPORT_INVALID;
bcm_gport_t gport_ta = BCM_GPORT_INVALID;

print bcm_port_gport_get(unit, ingPort, &gingPort);
print bcm_port_gport_get(unit, egrPort, &gegrPort);
print bcm_port_gport_get(unit, port_ta, &gport_ta);

bcm_pbmp_t pbmp, upbmp;
bcm_l3_intf_t l3_intf;
bcm_if_t egr_obj_network;
bcm_if_t egr_obj_acc;
bcm_if_t egr_obj_ta;
bcm_l3_egress_t l3_egress;
bcm_l2_addr_t l2_addr;
bcm_l2_station_t l2_station;
bcm_ip_t pkt_in_dst_ip = 0x0a0a0200;
bcm_ip_t pkt_in_dst_ip1 = 0x0a0a0201;
bcm_ip_t pkt_in_src_ip = 0x0a0a0101;   
int int_udp_port = 0x1a6f;

/* IFP configuration */

/* UDF */
int udf_create(int unit, int offset_bytes, int width_bytes, uint32 chunk_bmap, bcm_udf_abstract_pkt_format_t pkt_fmt, bcm_udf_id_t *udf_id)
{
    int rv;
    bcm_udf_chunk_info_t info;
    
    info.offset = offset_bytes * 8;
    info.width = width_bytes * 8;
    info.chunk_bmap = chunk_bmap;
    info.abstract_pkt_format = pkt_fmt;
    rv = bcm_udf_chunk_create(0, NULL, &info, udf_id);
        if (BCM_SUCCESS(rv)) {
           printf("UDF created Successfully: %d\n\r", *udf_id);
        } else {
           printf("UDF Failed to created: rv:%d\n\r", rv);
        }
        return rv;
}

int fp_udf_qset_set(int unit, bcm_udf_id_t *udf_id_arr, int num_udfs, 
                     bcm_field_qset_t *qset)
{
   int rv, i;
   rv = bcm_field_qset_id_multi_set(unit, bcmFieldQualifyUdf, num_udfs, udf_id_arr, qset);
   if (BCM_SUCCESS(rv)) {
       printf("FP UDF Qset Set Successfully.\n\r");
   } else {
       printf("FP UDF Qset Set Failed for IDs [");
           for (i=0; i < num_udfs; i++) {
               printf(" %d", udf_id_arr[i]);
           }
           printf(" ]\n\r");
   }
   return rv;
}


bcm_error_t INT_config(int unit)
{ 
  /* check if flow_init_mode=1 is set */
  char *config_char_val;
  int config_val;
  config_char_val = sal_config_get("flow_init_mode");
  if((config_char_val == NULL) || (config_char_val[0] == '0'))
  {
     printf("TD3 INT needs flow_init_mode=1 configuration (config.bcm). Please check this and re-run!!\n");
     return BCM_E_CONFIG;
  }
  
  uint32 flow_handle;
  uint32 encap_flow_option;
  print bcm_flow_handle_get(0,"INT",&flow_handle);
  print bcm_flow_option_id_get(0,flow_handle,
                 "LOOKUP_DGPP_ASSIGN_DEVICE_ID_OPAQUE_MD1_OPAQUE_MD2",
                             &encap_flow_option);

  /* VLAN settings */
  BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid_acc));
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_CLEAR(upbmp);
  BCM_PBMP_PORT_ADD(pbmp, ingPort);
  BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid_acc, pbmp, upbmp));
  
  BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid_network));
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_CLEAR(upbmp);
  BCM_PBMP_PORT_ADD(pbmp, egrPort);
  BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid_network, pbmp, upbmp));
  
  BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid_ta));
  
  BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));
  BCM_IF_ERROR_RETURN(bcm_vlan_control_set(unit, bcmVlanTranslate, TRUE));
  BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit, ingPort, bcmVlanTranslateIngressEnable, 1));
  BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIntL4DestPortEnable, 1));
  BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIntL4DestPort1, int_udp_port));
  BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIntProbeMarkerEnable,1));
  BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIntProbeMarker1,probe_marker1));
  BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIntProbeMarker2,probe_marker2));
  
  int max_payload_len = 1024;
  BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIntMaxPayloadLength,
             max_payload_len));
/*
  //ProbeMarker 1 is extracted in chunk 14 and 15.
  offset = 8;
  width = 4;
  chunk_bitmap = (1 << 3) | (1 << 2);
  BCM_IF_ERROR_RETURN(udf_create(0, offset, width, chunk_bitmap, bcmUdfAbstractPktFormatUdpINT, &udf_int_pm1_id));
  
  //ProbeMarker 2 is extracted in chunk 12 and 13
  offset = 12;
  width = 4;
  chunk_bitmap = (1 << 1) | (1 << 0);
  BCM_IF_ERROR_RETURN(udf_create(0, offset, width, chunk_bitmap, 
                    bcmUdfAbstractPktFormatUdpINT, &udf_int_pm2_id));
  
  udf_id_arr[0] = udf_int_pm1_id;
  udf_id_arr[1] = udf_int_pm2_id;
  udf_id_arr[2] = udf_int_rv_id;
*/

  bcm_field_group_mode_t mode;
  bcm_field_qset_t qset;
  bcm_field_entry_t eid;
  int gid = 1;
  
  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyPktType);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIntPktVersionOne);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIntPktOverflowed);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIntPktLengthValid);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIntPktType);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIntPktFinalHop);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyINTProbeMarker1);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyINTProbeMarker2);
  /* Start from SDK 6.5.15, no need to qualify ProbeMarker with UDF */
  //BCM_IF_ERROR_RETURN(fp_udf_qset_set(unit, udf_id_arr, num_udfs, &qset));
  
  bcm_field_group_config_t group;
  bcm_field_group_config_t_init(group);
  group.qset=qset;
  group.priority=1;
  group.group = gid;
  group.mode=bcmFieldGroupModeAuto;
  group.flags = BCM_FIELD_GROUP_CREATE_WITH_ID;
  
  /* Assign ASET */
  BCM_FIELD_ASET_INIT(group.aset);
  BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionIntEncapEnable);
  BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionCopyToCpu);
  BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionRedirectEgrNextHop);
  BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionIntTurnAround);
  BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionStatGroup);
  group.flags |= BCM_FIELD_GROUP_CREATE_WITH_ASET;
  
  /* Group create */
  BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group));
  
  /* Entry create */
  BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group.group, &eid));
  
  /* Match on INT Match ID */
  BCM_IF_ERROR_RETURN(bcm_field_qualify_PktType(unit, eid, bcmFieldPktTypeIpv4UdpINT));
  
  /* Match on INT valid version */
  BCM_IF_ERROR_RETURN(bcm_field_qualify_IntPktVersionOne(unit, eid, 0x1, 0x1));
  
  /* Check if OverFlow Bit is not set */
  BCM_IF_ERROR_RETURN(bcm_field_qualify_IntPktOverflowed(unit, eid, 0x0, 0x1));
  
  /* Match on Valid Length */
  BCM_IF_ERROR_RETURN(bcm_field_qualify_IntPktLengthValid(unit, eid, 0x1, 0x1));
  
  /* Match on Probe Request */
  BCM_IF_ERROR_RETURN(bcm_field_qualify_IntPktType(unit, eid, 
                                     bcmFieldIntPktTypeProbeRequest));
  
  /* Match on valid Hop Limit */
  BCM_IF_ERROR_RETURN(bcm_field_qualify_IntPktFinalHop(unit, eid, 0x0, 0x1));
  
  /* Match on InPort  */
  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, eid, ingPort, -1));
  
  BCM_IF_ERROR_RETURN(bcm_field_qualify_INTProbeMarker1(unit, eid, probe_marker1, probe_mask));
  BCM_IF_ERROR_RETURN(bcm_field_qualify_INTProbeMarker2(unit, eid, probe_marker2, probe_mask));
  /*
  //Valid Probe Marker 1
  BCM_IF_ERROR_RETURN(bcm_field_qualify_udf(unit, eid, udf_int_pm1_id, 4, &int_pkt_data[0], 
                              &int_pkt_mask[0]));
  
  //Valid Probe Marker 2
  BCM_IF_ERROR_RETURN(bcm_field_qualify_udf(unit, eid, udf_int_pm2_id, 4, &int_pkt_data[4], 
                              &int_pkt_mask[4]));
  */
  
  /* Action to Insert INT Metadata */
  //BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, eid, bcmFieldActionIntEncapEnable, 0, 0));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, eid, bcmFieldActionCopyToCpu, 1, 1));
  
  /* Install the Entry */
  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, eid));
  
  /* L3 interfaces */
  /* access side: assign l3_iif from vlan, vrf from l3_iif 
   * Use BCM_L3_ADD_TO_ARL to set up the l2 termination(my_station_tcam)
   */
  bcm_l3_intf_t_init(&l3_intf);
  l3_intf.l3a_flags =       BCM_L3_WITH_ID | BCM_L3_ADD_TO_ARL;
  l3_intf.l3a_intf_id =     intf_id_acc;
  sal_memcpy(l3_intf.l3a_mac_addr, pkt_in_dst_mac, sizeof(pkt_in_dst_mac));
  l3_intf.l3a_vid =         vid_acc;
  l3_intf.l3a_vrf =         vrf;
  BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));
  
  /* network side: egr_intf */
  bcm_l3_intf_t_init(&l3_intf);
  l3_intf.l3a_flags =       BCM_L3_WITH_ID;
  l3_intf.l3a_intf_id =     intf_id_network;
  sal_memcpy(l3_intf.l3a_mac_addr, local_mac_network, sizeof(local_mac_network));
  l3_intf.l3a_vid =         vid_network;
  l3_intf.l3a_vrf =         0;
  BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));
  
  /* Egress object for the network port */
  bcm_l3_egress_t_init(&l3_egress);
  l3_egress.intf =       intf_id_network;
  sal_memcpy(l3_egress.mac_addr,  remote_mac_network, sizeof(remote_mac_network));
  l3_egress.vlan =       vid_network;
  l3_egress.port =       gegrPort;
  BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_network));
  
  /* encap  */
  bcm_flow_encap_config_t einfo;
  bcm_flow_encap_config_t_init(&einfo);
  bcm_flow_logical_field_t lfield[3];
  
  einfo.flow_handle = flow_handle;
  einfo.flow_option = encap_flow_option;
  einfo.flow_port = gegrPort;
  einfo.criteria = BCM_FLOW_ENCAP_CRITERIA_FLEX;
  BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(0,flow_handle,
                "DEVICE_ID", &lfield[0].id));
  lfield[0].value = 0xdddddddd;
  BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(0,flow_handle,
                "OPAQUE_MD1", &lfield[1].id));
  lfield[1].value = 0xeeee;
  BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(0,flow_handle,
                "OPAQUE_MD2", &lfield[2].id));
  lfield[2].value = 0xcccccccc;
  einfo.valid_elements = BCM_FLOW_ENCAP_FLOW_PORT_VALID |
                         BCM_FLOW_ENCAP_FLEX_DATA_VALID | 
                         BCM_FLOW_ENCAP_FLEX_KEY_VALID;
  BCM_IF_ERROR_RETURN(bcm_flow_encap_add(0,&einfo,3,lfield));
  
  /* L3 host setup */
  bcm_l3_host_t hinfo;
  bcm_l3_host_t_init(hinfo);
  hinfo.l3a_vrf     = vrf;
  hinfo.l3a_intf    = egr_obj_network;
  hinfo.l3a_ip_addr = pkt_in_dst_ip;
  BCM_IF_ERROR_RETURN(bcm_l3_host_add(0,&hinfo));
  
  /************** turn around packet handling **************/
  
  /* turn around on hoplimit reached */
  
  bcm_field_entry_t eid_ta;
  
  /* Entry create */
  BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group.group, &eid_ta));
  
  BCM_IF_ERROR_RETURN(bcm_field_qualify_PktType(unit, eid_ta, bcmFieldPktTypeIpv4UdpINT));
  BCM_IF_ERROR_RETURN(bcm_field_qualify_IntPktVersionOne(unit, eid_ta, 0x1, 0x1));
  BCM_IF_ERROR_RETURN(bcm_field_qualify_IntPktFinalHop(unit, eid_ta, 0x1, 0x1));
  BCM_IF_ERROR_RETURN(bcm_field_qualify_IntPktType(unit, eid_ta, bcmFieldIntPktTypeProbeRequest));
  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, eid_ta, ingPort, -1));
 
  BCM_IF_ERROR_RETURN(bcm_field_qualify_INTProbeMarker1(unit, eid_ta, probe_marker1, probe_mask));
  BCM_IF_ERROR_RETURN(bcm_field_qualify_INTProbeMarker2(unit, eid_ta, probe_marker2, probe_mask));
  /*
  BCM_IF_ERROR_RETURN(bcm_field_qualify_udf(unit, eid_ta, udf_int_pm1_id, 4, &int_pkt_data[0],
                              &int_pkt_mask[0]));
  BCM_IF_ERROR_RETURN(bcm_field_qualify_udf(unit, eid_ta, udf_int_pm2_id, 4, &int_pkt_data[4],
                              &int_pkt_mask[4]));
  */
  
  /* Action to change INT request to INT reply packet */
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, eid_ta, bcmFieldActionIntTurnAround, 0, 0));
  
  bcm_l3_intf_t_init(&l3_intf);
  l3_intf.l3a_flags =       BCM_L3_WITH_ID;
  l3_intf.l3a_intf_id =     intf_id_ta;
  sal_memcpy(l3_intf.l3a_mac_addr, pkt_ta_src_mac, 
                 sizeof(pkt_ta_src_mac));
  l3_intf.l3a_vid =         vid_ta; 
  l3_intf.l3a_vrf =         0;
  BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));
  
  /* Egress object for outgoing port */
  bcm_l3_egress_t_init(&l3_egress);
  l3_egress.intf =       intf_id_ta;
  sal_memcpy(l3_egress.mac_addr,  pkt_ta_dst_mac, sizeof(pkt_ta_dst_mac));
  l3_egress.vlan =       vid_ta; 
  l3_egress.port =       gport_ta;
  BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_ta));
  
  /* send to loopback interface through NH action */
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, eid_ta, bcmFieldActionRedirectEgrNextHop, 
                 egr_obj_ta, 0));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, eid_ta, bcmFieldActionCopyToCpu, 1, 1));
  
  /* Install the Entry */
  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, eid_ta));
  
  /* disable vlan check on loopback port */
  BCM_IF_ERROR_RETURN(bcm_port_vlan_member_set(unit,gport_ta,0));
  
  /* handle the second pass, the packet sent to the loopback port */
  /* L3 interfaces */
  /* access side: assign l3_iif from vlan, vrf from l3_iif
   * Use BCM_L3_ADD_TO_ARL to set up the l2 termination(my_station_tcam)
   */
  bcm_l3_intf_t_init(&l3_intf);
  l3_intf.l3a_flags =       BCM_L3_WITH_ID | BCM_L3_ADD_TO_ARL;
  l3_intf.l3a_intf_id =     intf_id_ta1;
  sal_memcpy(l3_intf.l3a_mac_addr, pkt_ta_dst_mac, sizeof(pkt_ta_dst_mac));
  l3_intf.l3a_vid =         vid_ta; 
  l3_intf.l3a_vrf =         vrf_ta; 
  BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));
  
  /* Egress object for outgoing port */
  bcm_l3_egress_t_init(&l3_egress);
  l3_egress.intf =       intf_id_acc;
  sal_memcpy(l3_egress.mac_addr,  pkt_in_src_mac, sizeof(pkt_in_src_mac));
  l3_egress.vlan =       vid_acc;
  l3_egress.port =       gingPort;
  BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_acc));
  
  /* L3 host setup */
  bcm_l3_host_t_init(hinfo);
  hinfo.l3a_vrf     = vrf_ta; 
  hinfo.l3a_intf    = egr_obj_acc;
  hinfo.l3a_ip_addr = pkt_in_src_ip;
  BCM_IF_ERROR_RETURN(bcm_l3_host_add(unit,&hinfo));
  
  return BCM_E_NONE;
}

bcm_mac_t dmac1 = {0x00, 0x00, 0x00, 0x00, 0x22,0x22};
bcm_mac_t dmac2 = {0x00, 0x00, 0x00, 0x00, 0xBB, 0xBB};
/* 
 * This functions gets the port numbers and sets up ingress and 
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
	BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingPort, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ingPort, BCM_PORT_DISCARD_NONE));
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egrPort, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egrPort, BCM_PORT_DISCARD_NONE));
  
  bcm_pbmp_t 	vlan_pbmp;
  bcm_pbmp_t 	ut_pbmp;
  BCM_PBMP_CLEAR(vlan_pbmp);
  BCM_PBMP_PORT_ADD(vlan_pbmp, ingPort);
  BCM_PBMP_PORT_ADD(vlan_pbmp, egrPort);
  BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, 1, vlan_pbmp));
  bcm_l2_addr_t l2_addr;
  bcm_l2_addr_t_init(&l2_addr, dmac1, vid_network);
  l2_addr.flags = BCM_L2_STATIC;
  l2_addr.port = ingPort;
  BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));
  bcm_l2_addr_t_init(&l2_addr, dmac2, vid_ta);
  l2_addr.flags = BCM_L2_STATIC;
  l2_addr.port = egrPort;
  BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));
	bshell(0,"sc L2StaticMoveToCpu=1");
  bshell(0,"pw start");
  bshell(0,"pw report +raw");
  bshell(0,"clear c");
  
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

	/////////////////////////////////////////////////////////////////////////////
	for(i=0; i<1; ++i)
	{        
		printf("\n\n\n############ %d ###########\n", i);
		sprintf(cmd, "tx %d pbm=%d data=00000000bbbb00000000aaaa810000150800453a006e000000003f1164310a0a01010a0a0200003e1a6f005a0000aaaaaaaabbbbbbbb01010000ffffffffff0000001fe0000000000001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132330001259ffb7a",
				num_pkt_in_a_burst, ingPort);
		printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
		
		sprintf(cmd, "sleep quiet %d", 2);
    bshell(unit, cmd);
    
    printf("\n\n\n############ %d ###########\n", i);
		sprintf(cmd, "tx %d pbm=%d data=00000000bbbb00000000aaaa810000150800453a006e000000003f1164310a0a01010a0a0200003e1a6f005a0000aaaaaaaabbbbbbbb01010000ffffffffffff00001fe0000000000001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132330001259ffb7a",
				num_pkt_in_a_burst, ingPort);
		printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
		
		sprintf(cmd, "sleep quiet %d", 2);
    bshell(unit, cmd);
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
	
	if( (rrv = INT_config(unit)) != BCM_E_NONE )
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
