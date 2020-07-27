/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : Broadscan
 *
 *  Usage    : BCM.0> cint bsc_ipv4_5Tuple_DosAttack_l4src_l4dst_eq_ERSPANV3_sampling.c 
 *
 *  config   : broadscan_config.bcm
 *
 *  Log file : bsc_ipv4_5Tuple_DosAttack_l4src_l4dst_eq_ERSPANV3_sampling_log.txt
 *  
 *  Test Topology :
 *
 *                   +------------------------------+ 
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *  ingress_port     |                              |  egress_port(MTP port for sampling)
 *  +----------------+          SWITCH              +-----------------------------------+
 *                   |                              |
 *                   |                              |
 *                   |                              | 
 *                   |                              |
 *                   |                              |
 *                   +------------------------------+
 *
 *
 *  Summary:
 *  ========
 *  This CINT configures Broadscan Engine to export a record to remote collector for L4SrcPort==L4DstPort DOS attack packets
 *  recieved on a ingress port and also sample the original packet to collector on ERSPANV3 Escapsulation. 
 *  Flow Selection is composed of SrcIPv4/DstIPv4/DOSType/IpProtocol.
 * 
 *  Detailed steps done in the CINT script:
 *  =======================================
 *    1) Step1 - Test Setup 
 *    =============================================
 *       a) Select one ingress and mtp ports and configure them in
 *          Loopback mode. Install a rule to copy incoming packets to CPU and
 *          additional action to drop the packets when it loops back on egress
 *          ports. Start packet watcher.
 *      
 *    2) Step2 - Configuration
 *    ===================================================================================
 *       a) Flow tracker checker create
 *       b) Add an action to flow check
 *       c) Add an export info to Flow check
 *       d) Setup flow group (Done in broadscan_flow_group_setup())
 *       e) Attach Flow checker to Flow group
 *       f) Setup forwarding path for Collector (Done in broadscan_collector_setup()):
 *       g) Create collector with all IPFIX encapsulation details (Done in broadscan_collector_create()).
 *       h) Validate and Install template (broadscan_template_install()).
 *       i) Attach template and collector to Group
 *       j) Create Flow Selection (broadscan_flow_selection())
 *
 *    3) Step3 - Verification 
 *    =======================================================================
 *       a) Transmit below IPV4 DOS packet 
 *          Packet:
 *          ======
 *        1020 3040 5060 2233 4455 6677 8100 001E 
 *        0800 4508 0038 0000 4000 4006 4998 0A0A
 *        0101 0A0A 0102 3344 3344 00A1 A2A3 00B1 
 *        B2B3 5002 11F5 F8C3 0000 F5F5 F5F5 F5F5 
 *        F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5
 *        F5F5 F5F5
 *
 *
 *       b) Expected Result:
 *       ===================
 *       After Step 10.a, the packet flow will be learnt as a part of flow selection
 *       and this can be see as a part of broadscan_stat_check() which is part of the
 *       verification step as below.
 *
 *     +++++++++++++++++++++++++++++++++++++++++++++++++++++
 *     Printing Group stats for Group Id : = 1000
 *      bcm_flowtracker_group_stat_t group_stats = {
 *              uint64 flow_exceeded_count = {0x00000000 0x00000000}
 *              uint64 flow_missed_count = {0x00000000 0x00000000}
 *              uint64 flow_aged_out_count = {0x00000000 0x00000000}
 *              uint64 flow_learnt_count = {0x00000000 0x00000001}
 *              uint64 flow_meter_exceeded_count = {0x00000000 0x00000000}
 *     int $$ = 0 (0x0)
 *     +++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 *      In addtion to that  you would see three sample packets coming back from loopbacked collector port to CPU 
 *      Packet from data=<>, length=88                                                                                                                        [25/96674]
 *      [bcmPW.0]
 *      [bcmPW.0]Packet[1]: data[0000]: {000000000002} {000000000001} 8100 00c8
 *      [bcmPW.0]Packet[1]: data[0010]: 0800 4500 0084 0000 0000 ff2f b545 0202
 *      [bcmPW.0]Packet[1]: data[0020]: 0202 0101 0101 1000 22eb 0000 000a 201e
 *      [bcmPW.0]Packet[1]: data[0030]: 03ff 0000 0002 0000 83f7 1404 0001 0000
 *      [bcmPW.0]Packet[1]: data[0040]: 0000 1020 3040 5060 2233 4455 6677 8100
 *      [bcmPW.0]Packet[1]: data[0050]: 001e 0800 4508 0038 0000 4000 4006 4998
 *      [bcmPW.0]Packet[1]: data[0060]: 0a0a 0101 0a0a 0102 3344 3344 00a1 a2a3
 *      [bcmPW.0]Packet[1]: data[0070]: 00b1 b2b3 5002 11f5 f8c3 0000 f5f5 f5f5
 *      [bcmPW.0]Packet[1]: data[0080]: f5f5 f5f5 f5f5 f5f5 f5f5 f5f5 f5f5 f5f5
 *      [bcmPW.0]Packet[1]: data[0090]: f5f5 f5f5 f5f5 7fe5 9ea6
 *      [bcmPW.0]Packet[1]: length 154 (154). rx-port 2. cos 0. prio_int 0. vlan 200. reason 0x1. Outer tagged.
 *      [bcmPW.0]Packet[1]: dest-port 0. dest-mod 0. src-port 2. src-mod 0. opcode 0.  matched 0. classification-tag 0.
 *      [bcmPW.0]Packet[1]: reasons: FilterMatch
 *      [bcmPW.0]
 *      [bcmPW.0]Packet[2]: data[0000]: {000000000002} {000000000001} 8100 00c8
 *      [bcmPW.0]Packet[2]: data[0010]: 0800 4500 0084 0000 0000 ff2f b545 0202
 *      [bcmPW.0]Packet[2]: data[0020]: 0202 0101 0101 1000 22eb 0000 000b 201e
 *      [bcmPW.0]Packet[2]: data[0030]: 03ff 0000 0002 0000 83f7 1404 0001 0000
 *      [bcmPW.0]Packet[2]: data[0040]: 0000 1020 3040 5060 2233 4455 6677 8100
 *      [bcmPW.0]Packet[2]: data[0050]: 001e 0800 4508 0038 0000 4000 4006 4998
 *      [bcmPW.0]Packet[2]: data[0060]: 0a0a 0101 0a0a 0102 3344 3344 00a1 a2a3
 *      [bcmPW.0]Packet[2]: data[0070]: 00b1 b2b3 5002 11f5 f8c3 0000 f5f5 f5f5
 *      [bcmPW.0]Packet[2]: data[0080]: f5f5 f5f5 f5f5 f5f5 f5f5 f5f5 f5f5 f5f5
 *      [bcmPW.0]Packet[2]: data[0090]: f5f5 f5f5 f5f5 7ac5 2978
 *      [bcmPW.0]Packet[2]: length 154 (154). rx-port 2. cos 0. prio_int 0. vlan 200. reason 0x1. Outer tagged.
 *      [bcmPW.0]Packet[2]: dest-port 0. dest-mod 0. src-port 2. src-mod 0. opcode 0.  matched 0. classification-tag 0.
 *      [bcmPW.0]Packet[2]: reasons: FilterMatch
 *      [bcmPW.0]
 *      [bcmPW.0]Packet[3]: data[0000]: {000000000002} {000000000001} 8100 00c8
 *      [bcmPW.0]Packet[3]: data[0010]: 0800 4500 0084 0000 0000 ff2f b545 0202
 *      [bcmPW.0]Packet[3]: data[0020]: 0202 0101 0101 1000 22eb 0000 000c 201e
 *      [bcmPW.0]Packet[3]: data[0030]: 03ff 0000 0002 0000 83f7 1404 0001 0000
 *      [bcmPW.0]Packet[3]: data[0040]: 0000 1020 3040 5060 2233 4455 6677 8100
 *      [bcmPW.0]Packet[3]: data[0050]: 001e 0800 4508 0038 0000 4000 4006 4998
 *      [bcmPW.0]Packet[3]: data[0060]: 0a0a 0101 0a0a 0102 3344 3344 00a1 a2a3
 *      [bcmPW.0]Packet[3]: data[0070]: 00b1 b2b3 5002 11f5 f8c3 0000 f5f5 f5f5
 *      [bcmPW.0]Packet[3]: data[0080]: f5f5 f5f5 f5f5 f5f5 f5f5 f5f5 f5f5 f5f5
 *      [bcmPW.0]Packet[3]: data[0090]: f5f5 f5f5 f5f5 a22f bf0c
 *      [bcmPW.0]Packet[3]: length 154 (154). rx-port 2. cos 0. prio_int 0. vlan 200. reason 0x1. Outer tagged.
 *      Sleeping for 1 second
 *      [bcmPW.0]Packet[3]: dest-port 0. dest-mod 0. src-port 2. src-mod 0. opcode 0.  matched 0. classification-tag 0.
 *      [bcmPW.0]Packet[3]: reasons: FilterMatch
 *      Executing 'show c'
 *      MC_PERQ_PKT(0).cpu0         :                     3                  +3
 *      MC_PERQ_BYTE(0).cpu0        :                   462                +462
 *      RUC_64.xe0                  :                    20                 +20
 *      RDBGC0_64.xe0               :                    20                 +20
 *      ING_NIV_RX_FRAMES_VL.xe0    :                    20                 +20
 *      XLMIB_R127.xe0              :                    20                 +20
 *      XLMIB_RPKT.xe0              :                    20                 +20
 *      XLMIB_RUCA.xe0              :                    20                 +20
 *      XLMIB_RPRM.xe0              :                    20                 +20
 *      XLMIB_RVLN.xe0              :                    20                 +20
 *      XLMIB_RPOK.xe0              :                    20                 +20
 *      XLMIB_RBYT.xe0              :                 1,760              +1,760
 *      XLMIB_T127.xe0              :                    20                 +20
 *      XLMIB_TPOK.xe0              :                    20                 +20
 *      XLMIB_TPKT.xe0              :                    20                 +20
 *      XLMIB_TUCA.xe0              :                    20                 +20
 *      XLMIB_TVLN.xe0              :                    20                 +20
 *      XLMIB_TBYT.xe0              :                 1,760              +1,760
 *      XLMIB_RPROG1.xe0            :                    20                 +20
 *      UC_PERQ_PKT(0).xe0          :                    20                 +20
 *      UC_PERQ_BYTE(0).xe0         :                 1,760              +1,760
 *      RUC_64.xe1                  :                     3                  +3
 *      ING_NIV_RX_FRAMES_VL.xe1    :                     3                  +3
 *      XLMIB_R255.xe1              :                     3                  +3
 *      XLMIB_RPKT.xe1              :                     3                  +3
 *      XLMIB_RUCA.xe1              :                     3                  +3
 *      XLMIB_RPRM.xe1              :                     3                  +3
 *      XLMIB_RVLN.xe1              :                     3                  +3
 *      XLMIB_RPOK.xe1              :                     3                  +3
 *      XLMIB_RBYT.xe1              :                   462                +462
 *      XLMIB_T255.xe1              :                     3                  +3
 *      XLMIB_TPOK.xe1              :                     3                  +3
 *      XLMIB_TPKT.xe1              :                     3                  +3
 *      XLMIB_TUCA.xe1              :                     3                  +3
 *      XLMIB_TVLN.xe1              :                     3                  +3
 *      XLMIB_TBYT.xe1              :                   462                +462
 *      MC_PERQ_PKT(0).xe1          :                     3                  +3
 *      MC_PERQ_BYTE(0).xe1         :                   462                +462
 */

cint_reset();

bcm_port_t ingress_port;
bcm_port_t mtp_port;
const int BSCAN_MAX_INFO_ELEMENTS = 40;
const int BSCAN_NUM_EXPORT_ELEMENTS = 41;
int fte_export_profile_id=-1;

/* This function is written so that hardcoding of port 
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{

	int i=0,port=0,rv=BCM_E_NONE;
	bcm_port_config_t configP;
	bcm_pbmp_t ports_pbmp;

	rv = bcm_port_config_get(unit,&configP);
	if(BCM_FAILURE(rv)) {
		printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
		return rv;
	}

	ports_pbmp = configP.e;
	for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
		if (BCM_PBMP_MEMBER(&ports_pbmp,i)&& (port < num_ports)) {
			port_list[port]=i;
			port++;
		}
	}

	if (( port == 0 ) || ( port != num_ports )) {
		printf("portNumbersGet() failed \n");
		return -1;
	}

	return BCM_E_NONE;

}

/* 
 * Configures ingress port in loopback mode 
 */
bcm_error_t ingress_port_setup(int unit, bcm_port_t port)
{
	bcm_field_qset_t  qset;
	bcm_field_group_t group;
	bcm_field_entry_t entry;

	BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

	return BCM_E_NONE;
}

/* 
 * Configures MTP  port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU and drop the packets. This is
 * to avoid continuous loopback of the packet.
 */
bcm_error_t egress_port_setup(int unit, bcm_port_t port)
{
  bcm_field_qset_t  qset;
  bcm_field_group_t group;
  bcm_field_entry_t entry;

  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));
  
  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
  BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, 0, &group));
  BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));
  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

  return BCM_E_NONE;
}



/*   Select one ingress and egress port and configure them in 
 *   Loopback mode. Ingress port setup is done in ingress_port_setup()
 *   Also set the packet watcher utility . 
 */
bcm_error_t test_setup(int unit)
{
	int port_list[2];

	if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
		printf("portNumbersGet() failed\n");
		return -1;
	}

	ingress_port = port_list[0];
	mtp_port = port_list[1];

	if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
		printf("ingress_port_setup() failed for port %d\n", ingress_port);
		return -1;
	}


        if (BCM_E_NONE != egress_port_setup(unit, mtp_port)) {
               printf("egress_port_setup() failed for port %d\n", mtp_port);
               return -1;
        }

	/* "pw" is packet watcher tool for verifying the packet was receivedd on CPU
 	 * as the MTP is loop backed to and the packet from MTP will be sent to CPU for 
 	 * verifying.  
	 */
	bshell(unit, "pw start");
	bshell(unit, "pw report +raw");
	bshell(unit, "counter interval=1000");
	return BCM_E_NONE;
}

/*     Trasmits the packet from ingress port
 *     Checks the packets ingress using "show counters"
 *     Also checks Broadscan counters.
 */
int 
verify(int unit, bcm_flowtracker_group_t flow_group_id)
{
	char   str[512];
	int rv = BCM_E_NONE;
	bshell(unit, "hm ieee");
	printf("Transmiting  packet on ingress_port:%d\n", ingress_port);
	snprintf(str, 512, "tx 20 pbm=%d 
			data=0x1020304050602233445566778100001E08004508003800004000400649980A0A01010A0A01023344334400A1A2A300B1B2B3500211F5F8C30000F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5;
			sleep 1", ingress_port);
	bshell(unit, str);
	printf("Executing 'show c'\n");
	bshell(unit, "show c");
	rv = broadscan_stat_check(unit, flow_group_id);

	if (BCM_FAILURE(rv)) {
		printf("Failed to fetch Group stats for group: %d\n", flow_group_id);
		return rv;
	}

	return rv;
}

/* Setting up forwaring path for collector so that IPFIX export 
 * packets from Broadscan Engine will be forwarded  to remote 
 * collector reachable via collector port
 */
int 
broadscan_collector_setup(int unit, bcm_vlan_t fte_coll_vlan, bcm_mac_t fte_coll_dst_mac, bcm_port_t fte_coll_port)
{
	bcm_pbmp_t        pbmp, upbmp;
	bcm_l2_addr_t     l2_addr;
	bcm_field_qset_t  qset;
	bcm_field_group_t group;
	bcm_field_entry_t entry;

	/* Create vlan to redirect export packets */
	BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, fte_coll_vlan));
	BCM_PBMP_CLEAR(pbmp);
	BCM_PBMP_CLEAR(upbmp);
	BCM_PBMP_PORT_ADD(pbmp, fte_coll_port);
	BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, fte_coll_vlan, pbmp, upbmp));

	/* Add static MAC address */
	bcm_l2_addr_t_init(&l2_addr, fte_coll_dst_mac, fte_coll_vlan);
	l2_addr.flags = BCM_L2_STATIC;
	l2_addr.port  = fte_coll_port;
	BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));

	return 0;
}

/* Creates broadscan collector with all IPFIX header details */
int 
broadscan_collector_create(int unit, int collector_id, bcm_collector_info_t collector_info)
{
	int rv = BCM_E_NONE;
	uint32 options = 0;

	options = BCM_FLOWTRACKER_COLLECTOR_WITH_ID;

	rv = bcm_collector_create(unit,
			options, &collector_id, collector_info);
	if (BCM_FAILURE(rv)) {
		printf("\n Failed to create collector ret=%d\n", rv);
		return rv;
	}

	return rv;
}

/* Flow selection in Broadscan is done with the help of flex key gen logic 
 * which is very similar to flexible Field processor. Flow selection consists 
 * of two stages. Flow Preselection and flow key selection.
 */
int
broadscan_flow_selection(int unit, int *ftfp_group, bcm_flowtracker_group_t flow_group_id,
			int ipprot, int outer_vlan, int vlanformat)
{
	int rv = BCM_E_NONE;
	bcm_field_entry_t presel_eid;     
	bcm_field_presel_t ftfp_presel_id1;
	bcm_field_presel_t ftfp_presel_id2;
	bcm_field_presel_t ftfp_presel_id3;                
	bcm_field_group_config_t ftfp_group_config;
	bcm_field_entry_t ftfp_entry; 
	int stat_id;

	/* STAGE 1 ==> Flow Preselection Stage*/

	/* Presel to select Unicast L2 and Unicast IPv4 packet formats */
	rv = bcm_field_presel_create(unit, &ftfp_presel_id1);
	if (BCM_FAILURE(rv)) {
		printf("\n Failed to create a presel =%d\n", rv);
		return rv;
	}
	presel_eid = ftfp_presel_id1 | BCM_FIELD_QUALIFY_PRESEL;
	BCM_IF_ERROR_RETURN(bcm_field_qualify_Stage(unit, presel_eid, 
				bcmFieldStageIngressFlowtracker));
	BCM_IF_ERROR_RETURN(bcm_field_qualify_L4Ports(unit, presel_eid, 1, 1));
	BCM_IF_ERROR_RETURN(bcm_field_qualify_PktDstAddrType(unit, presel_eid, 
				BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_MAC | BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_IP));
	BCM_IF_ERROR_RETURN(bcm_field_qualify_IpType(unit, presel_eid, bcmFieldIpTypeIpv4NoOpts));

	/* Presel to select Multicast L2 and unicast IPv4 Packet formats */
	rv = bcm_field_presel_create(unit, &ftfp_presel_id2);
	if (BCM_FAILURE(rv)) {
		printf("\n Failed to create a presel =%d\n", rv);
		return rv;
	}
	presel_eid = ftfp_presel_id2 | BCM_FIELD_QUALIFY_PRESEL;
	BCM_IF_ERROR_RETURN(bcm_field_qualify_Stage(unit, presel_eid, bcmFieldStageIngressFlowtracker));
	BCM_IF_ERROR_RETURN(bcm_field_qualify_L4Ports(unit, presel_eid, 1, 1));
	BCM_IF_ERROR_RETURN(bcm_field_qualify_PktDstAddrType(unit, presel_eid, 
				BCM_FIELD_PKT_DST_ADDR_TYPE_MCAST_MAC| BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_IP));
	BCM_IF_ERROR_RETURN(bcm_field_qualify_IpType(unit, presel_eid, bcmFieldIpTypeIpv4NoOpts));

	/* Presel to select Unicast L2 and Multicast IPv4 Packet formats */
	rv = bcm_field_presel_create(unit, &ftfp_presel_id3);
	if (BCM_FAILURE(rv)) {
		printf("\n Failed to create a presel =%d\n", rv);
		return rv;
	}
	presel_eid = ftfp_presel_id3 | BCM_FIELD_QUALIFY_PRESEL;
	BCM_IF_ERROR_RETURN(bcm_field_qualify_Stage(unit, presel_eid, bcmFieldStageIngressFlowtracker));
	BCM_IF_ERROR_RETURN(bcm_field_qualify_L4Ports(unit, presel_eid, 1, 1));
	BCM_IF_ERROR_RETURN(bcm_field_qualify_PktDstAddrType(unit, presel_eid, 
				BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_MAC | BCM_FIELD_PKT_DST_ADDR_TYPE_MCAST_IP));
	BCM_IF_ERROR_RETURN(bcm_field_qualify_IpType(unit, presel_eid, bcmFieldIpTypeIpv4NoOpts));

	/* STAGE 2 ==> Flow key selection stage*/

	/* FTFP Group for IPV4 packets Identify
	 * - Unicast L2 and Unicast IPV4
	 * - Multicast L2 and Unicast IPV4
	 * - Unicast L2 and Multicast IPV4
	 * - both TCP and UDP Protocols.
	 */
	bcm_field_group_config_t_init(&ftfp_group_config);
	ftfp_group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
	BCM_FIELD_PRESEL_ADD(ftfp_group_config.preselset, ftfp_presel_id1);
	BCM_FIELD_PRESEL_ADD(ftfp_group_config.preselset, ftfp_presel_id2);
	BCM_FIELD_PRESEL_ADD(ftfp_group_config.preselset, ftfp_presel_id3);

	BCM_FIELD_QSET_INIT(ftfp_group_config.qset);
	BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyStageIngressFlowtracker);
	BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyIpProtocol);
	BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyOuterVlanId);
	BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyInnerVlanId);
	BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyVlanFormat);
	BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyInPort);
	ftfp_group_config.mode = bcmFieldGroupModeAuto;
	rv = bcm_field_group_config_create(unit, &ftfp_group_config);
	if (BCM_FAILURE(rv)) {
		printf("\n Failed to create a field group =%d\n", rv);
		return rv;
	}
	*ftfp_group = ftfp_group_config.group;

	/* Create an entry in Flow tracker FP gorup */
	BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, *ftfp_group, &ftfp_entry));
	BCM_IF_ERROR_RETURN(bcm_field_qualify_IpProtocol(unit, ftfp_entry, ipprot, 0xff)); 
	BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, ftfp_entry, ingress_port, 0xff)); 
	BCM_IF_ERROR_RETURN(bcm_field_qualify_OuterVlanId(unit, ftfp_entry, outer_vlan, 0xfff)); 
	BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, ftfp_entry, 
					bcmFieldActionFlowtrackerGroupId, flow_group_id, 0));
	rv =  bcm_field_entry_install(unit, ftfp_entry);
	if (BCM_FAILURE(rv)) {
		printf("\n Failed to create a field group entry with error =%d\n", rv);
		return rv;
	}

	return rv;
}


/* Flowtracker Group is the prime object in flow tracking which identifies 
 * what to track, how to track ,what to export and when to export. 
 * All major flow functions  like aging, metering, learning is associated with flow group.
 * This function does the following
 * 	Creates Flow Gorup
 * 	Add Max limit for Flow Learning
 *	Sets Tracking parameters for Flow Group
 *	Sets Age time to max
 */	
int
broadscan_flow_group_setup(int unit, int flow_group_id, int flow_gorup_options, 
		bcm_flowtracker_group_info_t flow_group_info,
		bcm_flowtracker_tracking_param_info_t *traking_param_list, 
		int num_tracking_list, int flow_limit)
{
	int rv = BCM_E_NONE;

	/* Creating Flow TRacker Group */
	rv = bcm_flowtracker_group_create (unit, flow_gorup_options, &flow_group_id, &flow_group_info);
	if (BCM_FAILURE(rv)) {
		printf("\n Failed to create a flow tracker group =%d\n", rv);
		return rv;
	}  

	/* Configuring max number of flows for this group */
	rv = bcm_flowtracker_group_flow_limit_set(unit, flow_group_id, flow_limit);
	if (BCM_FAILURE(rv)) {
		printf("\n Failed to set max limit for a group =%d\n", rv);
		return rv;
	}  

	/* Configure tracking parameters for this group */
	rv = bcm_flowtracker_group_tracking_params_set(unit, flow_group_id, num_tracking_list, 
			*traking_param_list);
	if (BCM_FAILURE(rv)) {
		printf("\n Failed to set tracking params =%d\n", rv);
		return rv;
	}  
	/* Setting Flow grouop Age time to max for debugging */
	rv = bcm_flowtracker_group_age_timer_set(unit, flow_group_id, 86400000);
	if (BCM_FAILURE(rv)) {
		printf("\n Failed to set age limit to flow group =%d\n", rv);
		return rv;
	}
	return rv;
}

/* Flow checkers helps to check the flow with some arithmetic checks and operations. 
 * Based on the outcome of that check, an action can be triggered.
 * This function does the following
 *  	Created Flow Check
 *  	Adds specific actions when the check results positive
 *  	Also sets what needs to be exported in case the result is positive
 */

int 
broadscan_add_check(int unit, int check_options, int min_value, int max_value, 
		bcm_flowtracker_tracking_param_type_t check_track_param, 
		bcm_flowtracker_check_operation_t check_operation, 
		bcm_flowtracker_check_action_info_t check_action_info, 
		bcm_flowtracker_check_export_info_t export_info, 
		bcm_flowtracker_check_t check_id)
{

	int rv = BCM_E_NONE;
	/* Flowtracker check information. */
	bcm_flowtracker_check_info_t check_info;
	bcm_flowtracker_check_info_t_init(&check_info);
	check_info.param = check_track_param;
	check_info.min_value = min_value;
	check_info.max_value = max_value;
	check_info.operation = check_operation;

	/* Create a flow check */ 
	rv =  bcm_flowtracker_check_create(unit, check_options, check_info, &check_id);
	if (BCM_FAILURE(rv)) {
		printf("\n Failed to setup flow check =%d\n", rv);
		return rv;
	}  
	/* Add an action to the flow check */
	rv =  bcm_flowtracker_check_action_info_set(unit, check_id, check_action_info);
	if (BCM_FAILURE(rv)) {
		printf("\n Failed to add flow action =%d\n", rv);
		return rv;
	}  

	return rv;

}

struct fte_export_element_map_s {
	bcm_flowtracker_tracking_param_type_t tracking_param;
	bcm_flowtracker_export_element_type_t export_element;
	int data_size;
};
fte_export_element_map_s  fte_export_element_maps[BSCAN_NUM_EXPORT_ELEMENTS] =  {
    { bcmFlowtrackerTrackingParamTypeSrcIPv4,           bcmFlowtrackerExportElementTypeSrcIPv4,           4,  },
    { bcmFlowtrackerTrackingParamTypeDstIPv4,           bcmFlowtrackerExportElementTypeDstIPv4,           4,  },
    { bcmFlowtrackerTrackingParamTypeL4SrcPort,         bcmFlowtrackerExportElementTypeL4SrcPort,         2,  },
    { bcmFlowtrackerTrackingParamTypeL4DstPort,         bcmFlowtrackerExportElementTypeL4DstPort,         2,  },
    { bcmFlowtrackerTrackingParamTypeSrcIPv6,           bcmFlowtrackerExportElementTypeSrcIPv6,           16, },
    { bcmFlowtrackerTrackingParamTypeDstIPv6,           bcmFlowtrackerExportElementTypeDstIPv6,           16, },
    { bcmFlowtrackerTrackingParamTypeIPProtocol,        bcmFlowtrackerExportElementTypeIPProtocol,        1,  },
    { bcmFlowtrackerTrackingParamTypePktCount,          bcmFlowtrackerExportElementTypePktCount,          4,  },
    { bcmFlowtrackerTrackingParamTypeByteCount,         bcmFlowtrackerExportElementTypeByteCount,         4,  },
    { bcmFlowtrackerTrackingParamTypeVRF,               bcmFlowtrackerExportElementTypeVRF,               2,  },
    { bcmFlowtrackerTrackingParamTypeTTL,               bcmFlowtrackerExportElementTypeTTL,               1,  },
    { bcmFlowtrackerTrackingParamTypeIPLength,          bcmFlowtrackerExportElementTypeIPLength,          2,  },
    { bcmFlowtrackerTrackingParamTypeIP6Length,         bcmFlowtrackerExportElementTypeIP6Length,         2,  },
    { bcmFlowtrackerTrackingParamTypeTcpWindowSize,     bcmFlowtrackerExportElementTypeTcpWindowSize,     2,  },
    { bcmFlowtrackerTrackingParamTypeDosAttack,         bcmFlowtrackerExportElementTypeDosAttack,         4,  },
    { bcmFlowtrackerTrackingParamTypeVxlanNetworkId,    bcmFlowtrackerExportElementTypeVxlanNetworkId,    3,  },
    { bcmFlowtrackerTrackingParamTypeNextHeader,        bcmFlowtrackerExportElementTypeNextHeader,        1,  },
    { bcmFlowtrackerTrackingParamTypeHopLimit,          bcmFlowtrackerExportElementTypeHopLimit,          1,  },
    { bcmFlowtrackerTrackingParamTypeInnerSrcIPv4,      bcmFlowtrackerExportElementTypeInnerSrcIPv4,      4,  },
    { bcmFlowtrackerTrackingParamTypeInnerDstIPv4,      bcmFlowtrackerExportElementTypeInnerDstIPv4,      4,  },
    { bcmFlowtrackerTrackingParamTypeInnerL4SrcPort,    bcmFlowtrackerExportElementTypeInnerL4SrcPort,    2,  },
    { bcmFlowtrackerTrackingParamTypeInnerL4DstPort,    bcmFlowtrackerExportElementTypeInnerL4DstPort,    2,  },
    { bcmFlowtrackerTrackingParamTypeInnerSrcIPv6,      bcmFlowtrackerExportElementTypeInnerSrcIPv6,      16, },
    { bcmFlowtrackerTrackingParamTypeInnerDstIPv6,      bcmFlowtrackerExportElementTypeInnerDstIPv6,      16, },
    { bcmFlowtrackerTrackingParamTypeInnerIPProtocol,   bcmFlowtrackerExportElementTypeInnerIPProtocol,   1,  },
    { bcmFlowtrackerTrackingParamTypeInnerTTL,          bcmFlowtrackerExportElementTypeInnerTTL,          1,  },
    { bcmFlowtrackerTrackingParamTypeInnerIPLength,     bcmFlowtrackerExportElementTypeInnerIPLength,     2,  },
    { bcmFlowtrackerTrackingParamTypeInnerNextHeader,   bcmFlowtrackerExportElementTypeInnerNextHeader,   1,  },
    { bcmFlowtrackerTrackingParamTypeInnerHopLimit,     bcmFlowtrackerExportElementTypeInnerHopLimit,     1,  },
    { bcmFlowtrackerTrackingParamTypeInnerIP6Length,    bcmFlowtrackerExportElementTypeInnerIP6Length,    2,  },
    { bcmFlowtrackerTrackingParamTypeTcpFlags,          bcmFlowtrackerExportElementTypeTcpFlags,          1,  },
    { bcmFlowtrackerTrackingParamTypeOuterVlanTag,      bcmFlowtrackerExportElementTypeOuterVlanTag,      2,  },
    { bcmFlowtrackerTrackingParamTypeTimestampNewLearn,      bcmFlowtrackerExportElementTypeTimestampNewLearn,      6,  },
    { bcmFlowtrackerTrackingParamTypeTimestampFlowStart,      bcmFlowtrackerExportElementTypeTimestampFlowStart,      6,  },
    { bcmFlowtrackerTrackingParamTypeTimestampFlowEnd,      bcmFlowtrackerExportElementTypeTimestampFlowEnd,      6,  },
    { bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1,      bcmFlowtrackerExportElementTypeTimestampCheckEvent1,      6,  },
    { bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2,      bcmFlowtrackerExportElementTypeTimestampCheckEvent2,      6,  },
    { bcmFlowtrackerTrackingParamTypeInnerDosAttack,   bcmFlowtrackerExportElementTypeInnerDosAttack,      4,  },
    { bcmFlowtrackerTrackingParamTypeTunnelClass,      bcmFlowtrackerExportElementTypeTunnelClass,      3,  },
    { bcmFlowtrackerTrackingParamTypeFlowtrackerCheck,      bcmFlowtrackerExportElementTypeFlowtrackerCheck,      2,  },
};

int 
fte_tracking_param_xlate(int unit,
		bcm_flowtracker_tracking_param_type_t elem,
		uint32 *size,
		bcm_flowtracker_export_element_type_t *info_elem)
{
	int ix;

	for (ix = 0; ix < BSCAN_NUM_EXPORT_ELEMENTS; ix++) {
		if (fte_export_element_maps[ix].tracking_param == elem) {
			*info_elem = fte_export_element_maps[ix].export_element;
			*size = fte_export_element_maps[ix].data_size;
			break;
		}
	}

	return BCM_E_NONE;
} 


/* Broadscan has debug counter that are avaiable for Flow group
 * Below funtion helps retrieve the stats for debugging
 */
int 
broadscan_stat_check(int unit, bcm_flowtracker_group_t flow_group_id)
{
	int rv = BCM_E_NONE;
	bcm_flowtracker_group_stat_t group_stats;
	bcm_flowtracker_group_stat_t_init(&group_stats);
	rv = bcm_flowtracker_group_stat_get(unit, flow_group_id, &group_stats);
	if (BCM_FAILURE(rv)) {
		printf("Failed to fetch Group stats for group: %d\n", flow_group_id);
		return rv;
	}
	printf("////////////////////////////////////////////////////////////\n");
	printf("Printing Group stats for Group Id : = %d\n", flow_group_id);
	print group_stats;
	printf("////////////////////////////////////////////////////////////\n");
	return rv;
}


int 
broadscan_template_install(int unit,
		bcm_flowtracker_group_t flow_group_id,
		bcm_collector_t collector_id,
		bcm_flowtracker_export_template_t *template_id,
		uint16 fte_set_id)
{

	int ix, elem, rv = BCM_E_NONE;
	uint32 options = 0x0;
	int max_in_export_elements = 0;
	int max_out_export_elements = 0;
	int actual_out_export_elements = 0;
	bcm_flowtracker_tracking_param_info_t tracking_params[BSCAN_MAX_INFO_ELEMENTS];
	bcm_flowtracker_export_element_info_t export_elems_ip[BSCAN_MAX_INFO_ELEMENTS];
	bcm_flowtracker_export_element_info_t export_elems_op[BSCAN_MAX_INFO_ELEMENTS];

	rv = bcm_flowtracker_group_tracking_params_get(unit,
			flow_group_id,
			BSCAN_MAX_INFO_ELEMENTS,
			tracking_params,
			&max_in_export_elements);
	if (BCM_FAILURE(rv)) {
		printf("Failed to fetch tracking params for group: %d\n", flow_group_id);
		return rv;
	}  

	/* Init ip/op element */
	for (ix = 0; ix < BSCAN_MAX_INFO_ELEMENTS; ix++) {
		export_elems_ip[ix].data_size = 0;
		export_elems_ip[ix].element = bcmFlowtrackerExportElementTypeReserved;

		export_elems_op[ix].data_size = 0;
		export_elems_op[ix].element = bcmFlowtrackerExportElementTypeReserved;
	}

	for (ix = 0; ix < max_in_export_elements; ix++) {
		rv = fte_tracking_param_xlate(unit,
				tracking_params[ix].param,
				&(export_elems_ip[ix].data_size),
				&(export_elems_ip[ix].element));
		if (BCM_FAILURE(rv)) {
			printf ("translating tracking param %d to export element failed\n", tracking_params[ix].param);
			return rv;
		}
	}

	/* Add flowgroup to info elems  */
	export_elems_ip[ix].element = bcmFlowtrackerExportElementTypeFlowtrackerGroup;
	export_elems_ip[ix].data_size = 2;
	max_in_export_elements++;
	/* Get max number of output elements */
	rv = bcm_flowtracker_export_template_validate(unit,
			flow_group_id,
			max_in_export_elements,
			export_elems_ip,
			0,
			NULL,
			&max_out_export_elements);
	if (BCM_FAILURE(rv)) {
		printf ("validate template for template with flow_group_id = %d Failed to get MAX\n", *template_id);
		return rv;
	}
	/* Validates the template */
	rv = bcm_flowtracker_export_template_validate(unit,
			flow_group_id,
			max_in_export_elements,
			export_elems_ip,
			max_out_export_elements,
			export_elems_op,
			&actual_out_export_elements);
	if (BCM_FAILURE(rv)) {
		printf ("validate template for template with flow_group_id = %d\n", *template_id);
		return rv;
	}

	printf("***********************************\n");
	printf("max_in_export_elements %d  max_out_export_elements %d actual_out_export_elements %d \r\n", max_in_export_elements, max_out_export_elements, actual_out_export_elements);
	printf("Input information elements: \n");
	for (ix = 0; ix < max_in_export_elements; ix++) {
		elem = (export_elems_ip[ix].element);
		printf("\tinput info element (%d): elem = %d and size = %d\n", ix, elem, export_elems_ip[ix].data_size);
	}
	printf("***********************************\n");


	printf("***********************************\n");
	printf("Output information elements: \n");
	for (ix = 0; ix < actual_out_export_elements; ix++) {
		elem = (export_elems_op[ix].element);
		printf("\toutput info element (%d): elem = %d and size = %d\n", ix, elem, export_elems_op[ix].data_size);
	}
	printf("***********************************\n");

	/* Created Export Templete */
	rv = bcm_flowtracker_export_template_create(unit, options,
			*template_id,
			fte_set_id,
			actual_out_export_elements,
			export_elems_op);
	if (BCM_FAILURE(rv)) {
		printf ("creating template failed template_id = %d\n", *template_id);
		return rv;
	}

	return rv;
}                                                 

int
broadscan_sampling_ifp_config(int unit, int flow_group_id, bcm_gport_t mirror_gport)
{
  int rv = 0, counterIdx;
  bcm_field_qset_t  qset;
  bcm_field_group_t group;
  bcm_field_entry_t entry;

  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyFlowtrackerGroupId);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyFlowtrackerGroupValid);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyFlowtrackerCollectorCopy);
  rv = bcm_field_group_create(unit, qset, 0, &group);
  if (BCM_FAILURE(rv)) {
      printf ("Failed to create IFP group for Sampling = %d\n", rv);
      return rv;
  }
  rv = bcm_field_entry_create(unit, group, &entry);
  if (BCM_FAILURE(rv)) {
      printf ("Failed to create IFP Entry for Sampling = %d\n", rv);
      return rv;
  }
  BCM_IF_ERROR_RETURN(bcm_field_qualify_FlowtrackerGroupId(unit, entry, flow_group_id, flow_group_id));
  BCM_IF_ERROR_RETURN(bcm_field_qualify_FlowtrackerGroupValid(unit, entry, 1, 0xf));
  BCM_IF_ERROR_RETURN(bcm_field_qualify_FlowtrackerCollectorCopy(unit, entry, 1, 0xf));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,
                        bcmFieldActionMirrorIngress, 0, mirror_gport));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,
                        bcmFieldActionErspan3HdrVlanCosPktCopy, 0, 0));
  rv = bcm_field_stat_create(unit, group, 1, bcmFieldStatPackets, &counterIdx);
  if (BCM_FAILURE(rv)) {
      printf ("Failed to create stat entry %d\n", rv);
      return rv;
  } 
  rv = bcm_field_entry_stat_attach(unit, entry, counterIdx);
  if (BCM_FAILURE(rv)) {
      printf ("Failed to attach stat entry %d\n", rv);
      return rv;
  }
  rv = bcm_field_entry_install(unit, entry);
  if (BCM_FAILURE(rv)) {
      printf ("Failed to install IFP group for Sampling = %d\n", rv);
      return rv;
  }

return rv;
}

int
configure_erspan_v3(int unit, bcm_mirror_destination_t mirror_dest, int flow_group_id)
{
    int rv  = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchFlexibleMirrorDestinations, 1));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchDirectedMirroring, 1));
    BCM_IF_ERROR_RETURN(bcm_mirror_init(unit));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));

    /* Create Mirror Session */
    rv = bcm_mirror_destination_create(unit, &mirror_dest);
    if(BCM_FAILURE(rv)) {
        printf("\nError in mirror dest create: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Cofigure IFP to send sampled packets to collector */
     rv = broadscan_sampling_ifp_config(unit, flow_group_id, mirror_dest.mirror_dest_id);
     if (BCM_FAILURE(rv)) {
           printf("\n Failed to configure IFP for Sampling =%d\n", rv);
           return rv;
     } else {
           printf ("\n !!! Configured IFP for sampling packet to collector !!!!\r\n");
    }

    return BCM_E_NONE;
}

/* Main functuion to configure Broadscan Engine*/
int execute()
{
	int unit = 0; 
	int ftfp_group;	
	int rv = BCM_E_NONE;

        bshell(unit, "config show; a ; version");

	/* Setting up test bed */ 
	rv = test_setup(unit);   
	if (BCM_FAILURE(rv)) {
		printf("\n Failed to setup test bed =%d\n", rv);
		return rv;
	} else {
		printf ("\n !!! Test Setup Done Succesfully !!!!\r\n");
	}


	/* Flow checkers helps to check the flow with some arithmetic checks and operations. 
	 * Based on the outcome of that check, an action can be triggered.
	 * In this script we will check for packets coming in particular TTL range
	 * and increment the counter and trigger to export the packet when the counter is greater than 2 */  

	int min_value = 0x400; /* L4SrcPort == L4DstPort */
	int max_value = 0x400; /* Will be trated as mask*/
	bcm_flowtracker_check_t check_id = 0x4000000; /* checker ID to attach it to Flow group */     
	int check_options = BCM_FLOWTRACKER_CHECK_WITH_ID;
	bcm_flowtracker_check_operation_t check_operation;
	bcm_flowtracker_tracking_param_type_t check_track_param;
	bcm_flowtracker_check_action_info_t check_action_info;
	bcm_flowtracker_check_export_info_t export_info;
	bcm_flowtracker_check_action_info_t_init(&check_action_info);
	bcm_flowtracker_check_export_info_t_init(&export_info);
	check_action_info.param = bcmFlowtrackerTrackingParamTypeDosAttack; /* The attribute of flow on which the
										check is performed.*/
	check_action_info.action = bcmFlowtrackerCheckActionCounterIncr; /* Action to be performed */

	check_track_param = bcmFlowtrackerTrackingParamTypeDosAttack; /* Check on DOS Attack Packets */
	check_operation = bcmFlowtrackerCheckOpInRange; /* Check operation which is range*/

	export_info.export_check_threshold=20; /*  Threshold at which the export should
						  be triggered for this check*/
	export_info.operation=bcmFlowtrackerCheckOpGreaterEqual; /* Operation to be checked with
								    threshold value for export check. */

	rv = broadscan_add_check(unit, check_options, min_value, max_value, check_track_param, 
			check_operation, check_action_info, export_info, check_id);
	if (BCM_FAILURE(rv)) {
		printf("\n Failed to setup a Flow checker =%d\n", rv);
		return rv;
	} else {
		printf ("\n !!! Added Flow checker Succesfully !!!!\r\n");
	}  							

	/* Flowtracker Group is the prime object in flow tracking which identifies 
	 * what to track, how to track ,  what to export and when to export. 
	 * All major flow functions  like aging, metering, learning is associated with flow group.*/
	int flow_group_id = 1000; /* Flow Group ID */
	int num_tracking_list = 8; /* Number of tracking params list */
	int tracking_param_tuple[4] =  {bcmFlowtrackerTrackingParamTypeSrcIPv4,
		bcmFlowtrackerTrackingParamTypeDstIPv4, 
                bcmFlowtrackerTrackingParamTypeIPProtocol,
		bcmFlowtrackerTrackingParamTypeDosAttack};
	bcm_flowtracker_group_info_t flow_group_info;
	bcm_flowtracker_group_info_t_init(&flow_group_info);
	bcm_flowtracker_tracking_param_info_t tracking_param_list[num_tracking_list];
	int flow_gorup_options = BCM_FLOWTRACKER_GROUP_WITH_ID;
	int flow_limit = 100; /* Maximum number of flows for this group */
	int i = 0;
	/* Add above list as a part of tracking params */
	for (i=0; i<4; i++) {
		tracking_param_list[i].flags = BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY; 
		tracking_param_list[i].param = tracking_param_tuple[i];
	}
	/* Add pkt count as data for exporting in record */
	tracking_param_list[i].flags = 0; /* Zero to notify as data which will be exported */
	tracking_param_list[i].param = bcmFlowtrackerTrackingParamTypePktCount;
	i++;
	tracking_param_list[i].flags = 0;
	tracking_param_list[i].param = bcmFlowtrackerTrackingParamTypeIPProtocol;
	i++;
	tracking_param_list[i].flags = 0;
	tracking_param_list[i].param = bcmFlowtrackerTrackingParamTypeL4DstPort;
	i++;
	tracking_param_list[i].flags = 0;
	tracking_param_list[i].param = bcmFlowtrackerTrackingParamTypeL4DstPort;
	i++;

	rv = broadscan_flow_group_setup(unit, flow_group_id, flow_gorup_options, 
			flow_group_info, &tracking_param_list, num_tracking_list, flow_limit);
	if (BCM_FAILURE(rv)) {
		printf("\n Failed to setup a Flow Group =%d\n", rv);
		return rv;
	} else {
		printf ("\n !!! Flow Group Setup Done Succesfully !!!!\r\n");
	}  							

	/* Once the Flow group is created attach flow checker to the group */
	rv = bcm_flowtracker_group_check_add(unit, flow_group_id, check_id);
	if (BCM_FAILURE(rv)) {
		printf("\n Failed to add flow checker to a Flow Group =%d\n", rv);
		return rv;
	} else {
		printf ("\n !!! Attched flow checker to the group Succesfully !!!!\r\n");
	}  


        /* Broadscan can copy the first N packets or some random packets of a flow 
 	 * to the collector so that the collector can do a Deep Packet Inspection 
 	 * and based on that allow the flow to continue of not.
 	 * 
 	 * Sampling information from Broadscan will be sent to Ingress Field Processor 
 	 * and IFP can send it to collector as is OR by encapsulating it into ERSPANV3 header
 	 */
         
         /* Configure Sampling and attach it to Flow Group */
         bcm_flowtracker_collector_copy_info_t collector_copy;
         bcm_flowtracker_collector_copy_info_t_init (&collector_copy);
         collector_copy.num_pkts_initial_samples= 2; /* Number of initial samples 
							to be sent to collector. */
         collector_copy.num_pkts_skip_for_next_sample = 10; /* Number of packets to skip to send
                                                     next sample copy to collector. */
         rv = bcm_flowtracker_group_collector_copy_info_set (unit, flow_group_id,
			           				collector_copy); 
         if (BCM_FAILURE(rv)) {
                printf("\n Failed to configure collector copy =%d\n", rv);
                return rv;
         } else {
                printf ("\n !!! Configured Collector sampling information and attached to Group !!!!\r\n");
         }

        /* Setup ERSPANV3 Mirror Tunnel */

	 /* Set the required configuration for mirroring */
	 bcm_vlan_t outer_vlan = 30;
	 bcm_gport_t mtp_gport;

	 bcm_port_gport_get(unit, mtp_port, &mtp_gport);

	 /* Mirroring configuration */
	 bcm_mirror_destination_t mirror_dest;
	 bcm_port_match_info_t match_info; /* Required for SGT configuration */
	 bcm_mirror_destination_t_init(&mirror_dest);
	 mirror_dest.gport = mtp_gport;
	 mirror_dest.flags |= BCM_MIRROR_DEST_TUNNEL_IP_GRE; /* BCM_MIRROR_DEST_TUNNEL_IP_GRE
								flag is used for
								ERSPAN encapsulation*/
	 bcm_mac_t src_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
	 bcm_mac_t dst_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
	 mirror_dest.dst_mac = dst_mac;
	 mirror_dest.src_mac = src_mac;
	 mirror_dest.tpid = 0x8100;
	 mirror_dest.vlan_id = 200;
	 mirror_dest.version = 4;
	 mirror_dest.src_addr = 0x02020202;
	 mirror_dest.dst_addr = 0x01010101;
	 mirror_dest.ttl = 255;

	 /* Configuration specific to ERSPANv3 */
	 mirror_dest.gre_protocol = 0x22eb; /* GRE Protocol Type value for ERSPANv3 */
	 mirror_dest.gre_seq_number = 10; /* Sequence number value used in GRE header. If no
					     value is provided,gre_seq_number will start with
					     0. Valid only if BCM_MIRROR_DEST_TUNNEL_IP_GRE
					     is set. */
	 mirror_dest.erspan_header.truncated_flag = 0; /* This should be set if ERSPAN encapsulated frame
							  exceeds the configured MTU */
	 mirror_dest.erspan_header.session_id = 0x3ff; /* Id associated with each ERSPAN session */
	 mirror_dest.erspan_header.gbp_sid = 0x1234;  /* Security Group Tag [SGT] of the
								  monitored frame */
	 mirror_dest.erspan_header.hw_id = 0x3f; /* Unique Id of an ERSPAN engine within a system */
	 mirror_dest.erspan_header.optional_hdr = 1; /* Indicates if the optional platform-specific sub-header
							is present */
	 mirror_dest.erspan_header.platform_id = 0x5; /* Platform Id to parse the sub-header. Valid only
							 if optional_hdr is set */
	 mirror_dest.erspan_header.switch_id = 4; /* Identifies a source switch at the receiving end.
						     Valid only if optional_hdr is set and platform_id
						     is 0x5 */
         rv =  configure_erspan_v3(unit, mirror_dest, flow_group_id);
         if (BCM_FAILURE(rv)) {
                printf("\n Failed to configure ERSPAN V3 =%d\n", rv);
                return rv;
         } else {
                printf ("\n !!! Configured ERSPAN V3 Mirror tunnel !!!!\r\n");
         }


	/* Given that we now setup Flow Groups along with checker and tracking params,
	 * We now setup a the Flow Expor Part starting up with collector setup*/

	/* Collector is a receiver of information from exporter which is generated from  
	 * tracking a particular flow. The collector is distinguished by its L2/L3 information. 
	 * The data is encapsulated into this L2/L3 header and sent mostly over UDP.*/

	/* Collector Info */
	bcm_port_t    fte_coll_port        = 0;
	bcm_mac_t     fte_coll_dst_mac     = {0x00, 0x00, 0x00, 0x00, 0x00, 0xc0};
	bcm_vlan_t    fte_coll_vlan        = 0x600;
	bcm_mac_t     fte_coll_src_mac     = {0x00, 0x00, 0x00, 0x00, 0x00, 0xf0};
	uint16        fte_coll_tpid        = 0x8100;
	bcm_ip_t      fte_coll_src_ip      = 0x0a82562a;
	bcm_ip_t      fte_coll_dst_ip      = 0x0a82b00a;
	uint8         fte_coll_dscp        = 0xE1;
	uint8         fte_coll_ttl         = 9;
	uint16        fte_coll_mtu         = 1500;
	uint16        fte_set_id           = 0x1efe;
	bcm_l4_port_t fte_coll_l4_src_port = 0x1F91;
	bcm_l4_port_t fte_coll_l4_dst_port = 0x0807;
	int           collector_id         = 0;
	bcm_collector_info_t collector_info;
        bcm_collector_export_profile_t profile_info;

        bcm_collector_info_t_init(&collector_info);
        bcm_collector_export_profile_t_init(&profile_info);

	/* Flowtracker collector information. */
	sal_memcpy(collector_info.eth.dst_mac, fte_coll_dst_mac, 6);
	sal_memcpy(collector_info.eth.src_mac, fte_coll_src_mac, 6);

	/* Ethernet encapsulation of the packet sent to collector. */
	collector_info.eth.vlan_tag_structure =
		BCM_FLOWTRACKER_COLLECTOR_ETH_HDR_SINGLE_TAGGED;
	collector_info.eth.outer_vlan_tag = fte_coll_vlan;
	collector_info.eth.outer_tpid     = fte_coll_tpid;

        /* IPv4 encapsulation of the packet sent to collector. */
	collector_info.ipv4.src_ip        = fte_coll_src_ip;
	collector_info.ipv4.dst_ip        = fte_coll_dst_ip;
	collector_info.ipv4.dscp          = fte_coll_dscp;
	collector_info.ipv4.ttl           = fte_coll_ttl;

        /* UDP encapsulation of the packet sent to collector. */
	collector_info.udp.src_port       = fte_coll_l4_src_port;
	collector_info.udp.dst_port       = fte_coll_l4_dst_port;
        collector_info.transport_type     = bcmCollectorTransportTypeIpv4Udp; /* Transport type
                                             used for exporting flow data to the collector.*/

        /* export profile */
        profile_info.wire_format = bcmCollectorWireFormatIpfix;
        profile_info.max_packet_length = fte_coll_mtu;

	/* Setting up path for IPFIX export packets towards collector */
	rv = broadscan_collector_setup(unit, fte_coll_vlan, fte_coll_dst_mac, fte_coll_port);
	if (BCM_FAILURE(rv)) {
		printf("\n Failed to setup collector =%d\n", rv);
		return rv;
	} else {
		printf ("\n !!! Collector Setup Done Succesfully !!!!\r\n");
	}  

	/* Creating Collector */
	rv = broadscan_collector_create(unit, collector_id, collector_info);
	if (BCM_FAILURE(rv)) {
		printf("\n Failed to setup collector =%d\n", rv);
		return rv;
	} else {
		printf ("\n !!! Created Collector Succesfully !!!!\r\n");
	}

        /* Create Export Profile */
        rv = bcm_collector_export_profile_create(unit, 0, &fte_export_profile_id, &profile_info);
        if (rv != BCM_E_NONE) {
            printf("\n Failed to create export profile ret=%d\n", rv);
            return BCM_E_INTERNAL;
        }

	/* The template is a handshake between collector and exporter on how the exported 
	 * data will look like. Template provides the following information. 
	 * 1. Key of flow to be tracked
	 * 2. Associated data to be exported of tracked flow.
	 * 3. Order in which flow information(Record) to be sent.*/

	bcm_flowtracker_export_template_t template_id;
	rv = broadscan_template_install( unit, flow_group_id, collector_id, &template_id, fte_set_id);
	if (BCM_FAILURE(rv)) {
		printf ("Failed to setup a template (%d) failed rv = %d\n", template_id, rv);
		return rv;
	} else {
		printf ("\n !!! Template Setup done Succesfully !!!!\r\n");
	}

	/* Attach Template and Collector to Group */ 

	rv = bcm_flowtracker_group_collector_attach(unit, flow_group_id, collector_id, fte_export_profile_id, template_id);
	if (BCM_FAILURE(rv)) {
		printf ("Attaching template (%d) and collector (%d) to group (%d) failed rv = %d\n",
				template_id, collector_id, flow_group_id, rv);
		return rv;
	} else {
		printf ("\n !!! Attached Template and Collector to Group Succesfully !!!!\r\n");
	}

	/* Creating Flow Selection */
        int ipprot = 6;
        int vlanformat = 3; /* double tagged */
	rv = broadscan_flow_selection(unit, &ftfp_group, flow_group_id, ipprot, outer_vlan, vlanformat); 
	if (BCM_FAILURE(rv)) {
		printf("\n Failed to create Flow Selection =%d\n", rv);
		return rv;
	} else {
		printf ("\n !!! Created Flow Selection Succesfully !!!!\r\n");
	}

	rv = verify(unit, flow_group_id);
	if (BCM_FAILURE(rv)) {
		printf("\n Failed to verify =%d\n", rv);
		return rv;
	}

	return rv;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

