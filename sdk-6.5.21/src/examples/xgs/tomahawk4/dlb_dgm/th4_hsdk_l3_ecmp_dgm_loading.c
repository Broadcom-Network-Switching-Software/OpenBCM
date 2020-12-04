/*  Feature  : L3 IPv4 Route to DGM ECMP group
 *
 *  Usage    : BCM.0> cint th4_hsdk_l3_ecmp_dgm_loading.c
 *
 *  config   : bcm56990-generic-l3.config.yml
 *
 *  Log file : th4_hsdk_l3_ecmp_dgm_loading_log.txt
 *
 *  Test Topology :
 *                  +------------------------------+ dgm ecmp primary member 0
 *                  |                              +--------------------------->
 *                  |                              | dgm ecmp primary member 1
 *                  |                              +--------------------------->
 *     ingress_port |                              | dgm ecmp primary member 2
 * +----------------+          SWITCH              +--------------------------->
 *                  |                              |
 *                  |                              | dgm ecmp alternate member 0
 *                  |                              +--------------------------->
 *                  |                              | dgm ecmp alternate member 1
 *                  |                              +--------------------------->
 *                  |                              |
 *                  +------------------------------+
 *
 *  This script can verify below L3 APIs:
 *  ========
 *   bcm_l3_route_add()
 *   bcm_l3_ecmp_create() with flags BCM_L3_ECMP_DYNAMIC_MODE_DGM
 *
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate L3 IPv4 route to DGM ECMP group with RTAG7 hash
 *  configuration.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Create vlan and add ports to vlan. Enable L3 feature per port.
 *
 *    2) Step2 - Configuration (Done in config_rtag7_l3_ecmp_dgm())
 *    ======================================================
 *      a) Create egress objects used for unicast route destionation.
 *      b) Create a DGM ECMP group.
 *      c) Create a L3 route to that DGM ECMP group.
 *
 *    3) Step3 - Verification (Done in test_verify())
 *    ==========================================
 *      a) For a packet received destinated to DGM ECMP group, DGM logic will normally determine the
 *      destination ECMP member assignment for one of the primary path only. When the primary path
 *      loading got changed, the DLB/DGM internal mapping quality band value equal to/lower than the
 *      per DGM ECMP group configured primary path threshold, DGM logic then consider to choose
 *      an alternate path of DGM ECMP group, the final DGM ECMP group member will be determined by
 *      the highest quality band value of the primary path and alternate path, this is done by TH4
 *      Switch hardware itself, no need application software involved.
 *
 *      Verify the result after sending test packets into ingress_port two times, first time the
 *      packet go through the primary path as expected, and after manually change the parimay path
 *      quality band value to be a lower value, and configure the alternate path quality band value
 *      to be a higher value, the same test packet send into ingress_port second time will go through
 *      the alternated path. The PktIO call back function and Flexctr fucntion verify the packet count
 *      and packet data on expected primary path or alternate path.
 *
 *         ingress_port:
 *         {000000000001} {002a10777700} 8100 000b
 *         0800 4500 003c 6762 0000 ff01 bd76 0a3a
 *         4001 0a3a 4273 0000 550a 0001 0051 6162
 *         6364 6566 6768 696a 6b6c 6d6e 6f70 7172
 *         7374 7576 7761 6263 6465 6667 6869
 *
 *      b) Expected Result:
 *      Expect the L3 IPv4 route packets egressed from primary path or alternal path of different
 *      L3 egress next hops/ egress ports.
 *         egress_port[0]/egr_obj_id[0]:
 *
 *         {00000000fe12} {000000000012} 8100 000c
 *         0800 4500 003c 6762 0000 fe01 be76 0a3a
 *         4001 0a3a 4273 0000 550a 0001 0051 6162
 *         6364 6566 6768 696a 6b6c 6d6e 6f70 7172
 *         7374 7576 7761 6263 6465 6667 6869
 *
 *         Ethernet: dst<00:00:00:00:fe:12> src<00:00:00:00:00:12> Tagged Packet ProtID<0x8100> Ctrl<0x000c> Internet Protocol (IP)
 *         IP: V(4) src<10.58.64.1> dst<10.58.66.115> hl<5> service-type<0> tl<60> id<26466> frg-off<0> ttl<254> > chk-sum<0xbe76>
 *         ICMP: Type-0 (Echo Reply [RFC792]) Code-0
 *         Generic: Code <25088> Checksum <0x6364> hun <65 66 67 68>
 *
 *         egress_port[1] / egr_obj_id[1]:
 *         {00000002fe22} {000000000022} 8100 0016
 *         0800 4500 003c 6762 0000 fe01 be76 0a3a
 *         4001 0a3a 4273 0000 550a 0001 0051 6162
 *         6364 6566 6768 696a 6b6c 6d6e 6f70 7172
 *         7374 7576 7761 6263 6465 6667 6869
 *
 *         Ethernet: dst<00:00:00:02:fe:22> src<00:00:00:00:00:22> Tagged Packet ProtID<0x8100> Ctrl<0x0016> Internet Protocol (IP)
 *         IP: V(4) src<10.58.64.1> dst<10.58.66.115> hl<5> service-type<0> tl<60> id<26466> frg-off<0> ttl<254> > chk-sum<0xbe76>
 *         ICMP: Type-0 (Echo Reply [RFC792]) Code-0
 *         Generic: Code <25088> Checksum <0x6364> hun <65 66 67 68>
 *
 *         egress_port[2] / egr_obj_id[2]:
 *         {00000002fe32} {000000000032} 8100 0020
 *         0800 4500 003c 6762 0000 fe01 be76 0a3a
 *         4001 0a3a 4273 0000 550a 0001 0051 6162
 *         6364 6566 6768 696a 6b6c 6d6e 6f70 7172
 *         7374 7576 7761 6263 6465 6667 6869
 *
 *         Ethernet: dst<00:00:00:02:fe:32> src<00:00:00:00:00:32> Tagged Packet ProtID<0x8100> Ctrl<0x0020> Internet Protocol (IP)
 *         IP: V(4) src<10.58.64.1> dst<10.58.66.115> hl<5> service-type<0> tl<60> id<26466> frg-off<0> ttl<254> > chk-sum<0xbe76>
 *         ICMP: Type-0 (Echo Reply [RFC792]) Code-0
 *         Generic: Code <25088> Checksum <0x6364> hun <65 66 67 68>
 *
 *         egress_port[3] / egr_obj_id[3]:
 *         {00000002fe42} {000000000042} 8100 002a
 *         0800 4500 003c 6762 0000 fe01 be76 0a3a
 *         4001 0a3a 4273 0000 550a 0001 0051 6162
 *         6364 6566 6768 696a 6b6c 6d6e 6f70 7172
 *         7374 7576 7761 6263 6465 6667 6869
 *
 *         Ethernet: dst<00:00:00:02:fe:42> src<00:00:00:00:00:42> Tagged Packet  ProtID<0x8100> Ctrl<0x002a> Internet Protocol (IP)
 *         IP: V(4) src<10.58.64.1> dst<10.58.66.115> hl<5> service-type<0> tl<60> id<26466> frg-off<0> ttl<254> > chk-sum<0xbe76>
 *         ICMP: Type-0 (Echo Reply [RFC792]) Code-0
 *         Generic: Code <25088> Checksum <0x6364> hun <65 66 67 68>
 *
 *         egress_port[4] / egr_obj_id[4]:
 *         {00000002fe52} {000000000052} 8100 0034
 *         0800 4500 003c 6762 0000 fe01 be76 0a3a
 *         4001 0a3a 4273 0000 550a 0001 0051 6162
 *         6364 6566 6768 696a 6b6c 6d6e 6f70 7172
 *         7374 7576 7761 6263 6465 6667 6869
 *
 *         Ethernet: dst<00:00:00:02:fe:52> src<00:00:00:00:00:52> Tagged Packet ProtID<0x8100> Ctrl<0x0034> Internet Protocol (IP)
 *         IP: V(4) src<10.58.64.1> dst<10.58.66.115> hl<5> service-type<0> tl<60> id<26466> frg-off<0> ttl<254> > chk-sum<0xbe76>
 *         ICMP: Type-0 (Echo Reply [RFC792]) Code-0
 *         Generic: Code <25088> Checksum <0x6364> hun <65 66 67 68>
 *
 *    4) Step4 - The L3 route to DGM ECMP group with RTAG7 hash teardown configuration(Done in test_cleanup())
 *    ==========================================
 *      a) Reverse the configuration, done in clear_rtag7_l3_ecmp_dgm()
 */
cint_reset();

int unit = 0;
bcm_error_t rv = BCM_E_NONE;
bcm_port_t ingress_port;
int ingress_port_num = 1;
int egress_port_num = 5;
bcm_port_t egress_port[egress_port_num];
bcm_vlan_t in_vlan = 11;
bcm_vlan_t out_vlan[egress_port_num] = {12, 22, 32, 42, 52};
bcm_vrf_t vrf = 1;
bcm_if_t l3_intf_id[egress_port_num];
int egr_obj_num = egress_port_num;
bcm_if_t egr_obj_id[egr_obj_num];
bcm_if_t ecmp_group_id;
int my_station_id;
bcm_mac_t router_mac_in = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
bcm_mac_t router_mac_out[egress_port_num] = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x12},
	                                         {0x00, 0x00, 0x00, 0x00, 0x00, 0x22},
	                                         {0x00, 0x00, 0x00, 0x00, 0x00, 0x32},
	                                         {0x00, 0x00, 0x00, 0x00, 0x00, 0x42},
	                                         {0x00, 0x00, 0x00, 0x00, 0x00, 0x52},
	                                        };
bcm_mac_t dst_mac[egress_port_num] = {{0x00, 0x00, 0x00, 0x00, 0xfe, 0x12},
                                      {0x00, 0x00, 0x00, 0x02, 0xfe, 0x22},
                                      {0x00, 0x00, 0x00, 0x02, 0xfe, 0x32},
                                      {0x00, 0x00, 0x00, 0x02, 0xfe, 0x42},
                                      {0x00, 0x00, 0x00, 0x02, 0xfe, 0x52},
	                                 };

bcm_ip_t dip = 0x0a3a4273;
bcm_ip_t mask = 0xffffff00;
bcm_field_group_t ifp_group;
bcm_field_entry_t ifp_entry[egress_port_num];
uint32_t stat_counter_id;

int primary_path_count = 3; /* alternal path count is 2 */
int alternate_path = FALSE; /* by default, choose primary path */

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t
portNumbersGet(int unit, int *port_list, int num_ports)
{
	bcm_error_t rv = BCM_E_NONE;
    int i = 0, port = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit, &configP);
    if (BCM_FAILURE(rv)) {
        printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = configP.e;
    for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i) && (port < num_ports)) {
            port_list[port] = i;
            port++;
        }
    }

    if ((0 == port) || (port != num_ports)) {
        printf("portNumbersGet() failed \n");
        return BCM_E_PORT;
    }

    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode
 */
bcm_error_t
ingress_port_setup(int unit, bcm_port_t port)
{
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_PHY));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_NONE));

    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU. Port is also configured
 * to discard all packets. This is to avoid continuous
 * loopback of the packet.
 */
bcm_error_t
egress_port_setup(int unit, int port_count, bcm_port_t *port)
{
	int i=0;
    bcm_field_entry_t entry;
    bcm_field_group_config_t group_config;

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));
	ifp_group = group_config.group;
	print ifp_group;

    for(i=0;i<port_count; i++){
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
	ifp_entry[i] = entry;
	print ifp_entry[i];
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, *port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, *port, BCM_PORT_LOOPBACK_PHY));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, *port, BCM_PORT_DISCARD_ALL));
	port++;
	}

    return BCM_E_NONE;
}

bcm_error_t clear_field_resource(int unit, bcm_field_group_t group, bcm_field_entry_t *entry, int entry_num)
{
	bcm_error_t rv = BCM_E_NONE;
	int i;

	for(i=0; i<entry_num; i++){
    /* Remove field entry from hardware table */
    rv = bcm_field_entry_remove(unit, entry[i]);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_remove() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Destroy the field entry */
    rv = bcm_field_entry_destroy(unit, entry[i]);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_destroy() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }
	}

    /* Destroy the field group */
    rv = bcm_field_group_destroy(unit, group);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_group_destroy() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/*
 * Test Setup:
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
	bcm_error_t rv = BCM_E_NONE;
    int port_list[egress_port_num+ingress_port_num];
	int i=0;

    rv = portNumbersGet(unit, port_list, egress_port_num+ingress_port_num);
    if (BCM_FAILURE(rv)) {
        printf("portNumbersGet() failed\n");
        return rv;
    }

    ingress_port = port_list[0];
	print ingress_port;
	for(i=0;i<egress_port_num;i++){
        egress_port[i] = port_list[i+1];
        print egress_port[i];
	}

    printf("setting ingress port:%d PHY loopback\n", ingress_port);
    rv = ingress_port_setup(unit, ingress_port);
    if (BCM_FAILURE(rv)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port);
        return rv;
    }

	for(i=0;i<egress_port_num;i++){
        printf("setting egress port:%d in PHY loopback\n", egress_port[i]);
	}
    rv = egress_port_setup(unit, egress_port_num, egress_port);
    if (BCM_FAILURE(rv)) {
        printf("egress_port_setup() failed.\n");
        return rv;
    }

    /* Enable IPv4 on ingress port */
    print bcm_port_control_set(unit, ingress_port, bcmPortControlIP4, TRUE);

    /*bshell(unit, "pw start report +raw +decode");*/

	return BCM_E_NONE;
}

/*Expected Egress packets */
unsigned char expected_egress_packet0[78] = {
0x00, 0x00, 0x00, 0x00, 0xfe, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x81, 0x00, 0x00, 0x0c,
0x08, 0x00, 0x45, 0x00, 0x00, 0x3c, 0x67, 0x62, 0x00, 0x00, 0xfe, 0x01, 0xbe, 0x76, 0x0a, 0x3a,
0x40, 0x01, 0x0a, 0x3a, 0x42, 0x73, 0x00, 0x00, 0x55, 0x0a, 0x00, 0x01, 0x00, 0x51, 0x61, 0x62,
0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72,
0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
};

unsigned char expected_egress_packet1[78] = {
0x00, 0x00, 0x00, 0x02, 0xfe, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x81, 0x00, 0x00, 0x16,
0x08, 0x00, 0x45, 0x00, 0x00, 0x3c, 0x67, 0x62, 0x00, 0x00, 0xfe, 0x01, 0xbe, 0x76, 0x0a, 0x3a,
0x40, 0x01, 0x0a, 0x3a, 0x42, 0x73, 0x00, 0x00, 0x55, 0x0a, 0x00, 0x01, 0x00, 0x51, 0x61, 0x62,
0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72,
0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
};

unsigned char expected_egress_packet2[78] = {
0x00, 0x00, 0x00, 0x02, 0xfe, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x32, 0x81, 0x00, 0x00, 0x20,
0x08, 0x00, 0x45, 0x00, 0x00, 0x3c, 0x67, 0x62, 0x00, 0x00, 0xfe, 0x01, 0xbe, 0x76, 0x0a, 0x3a,
0x40, 0x01, 0x0a, 0x3a, 0x42, 0x73, 0x00, 0x00, 0x55, 0x0a, 0x00, 0x01, 0x00, 0x51, 0x61, 0x62,
0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72,
0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
};

unsigned char expected_egress_packet3[78] = {
0x00, 0x00, 0x00, 0x02, 0xfe, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x81, 0x00, 0x00, 0x2a,
0x08, 0x00, 0x45, 0x00, 0x00, 0x3c, 0x67, 0x62, 0x00, 0x00, 0xfe, 0x01, 0xbe, 0x76, 0x0a, 0x3a,
0x40, 0x01, 0x0a, 0x3a, 0x42, 0x73, 0x00, 0x00, 0x55, 0x0a, 0x00, 0x01, 0x00, 0x51, 0x61, 0x62,
0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72,
0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
};

unsigned char expected_egress_packet4[78] = {
0x00, 0x00, 0x00, 0x02, 0xfe, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x52, 0x81, 0x00, 0x00, 0x34,
0x08, 0x00, 0x45, 0x00, 0x00, 0x3c, 0x67, 0x62, 0x00, 0x00, 0xfe, 0x01, 0xbe, 0x76, 0x0a, 0x3a,
0x40, 0x01, 0x0a, 0x3a, 0x42, 0x73, 0x00, 0x00, 0x55, 0x0a, 0x00, 0x01, 0x00, 0x51, 0x61, 0x62,
0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72,
0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
};

int test_failed = 0; /*Final result will be stored here */
int rx_count;   /* Global received packet count */
unsigned char *expected_packets[egress_port_num];
const uint8 priority = 100;

/* Rx callback function for applications using the HSDK PKTIO */
bcm_pktio_rx_t
pktioRxCallback(int unit, bcm_pktio_pkt_t * packet, void *cookie)
{
    int                *count = (auto) cookie;
    void               *buffer;
    uint32              length;
    uint32              port;

	(*count)++; /* Bump received packet count */

    /* Get basic packet info */
    if (BCM_FAILURE(bcm_pktio_pkt_data_get(unit, packet, (void *) &buffer, &length))) {
        return BCM_PKTIO_RX_NOT_HANDLED;
    }
    /* Get source port metadata */
    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                        (unit, packet, bcmPktioPmdTypeRx,
                         BCM_PKTIO_RXPMD_SRC_PORT_NUM, &port));

	if ((egress_port[0] != port) && (egress_port[1] != port) && (egress_port[2] != port) && (egress_port[3] != port) && (egress_port[4] != port)){
   	    test_failed = 1;
        printf("pktioRxCallback: packet received from wrong port %d.\n", port);
	} else if ((egress_port[0] == port)&&(FALSE == alternate_path)){
		if (sal_memcmp(buffer, expected_packets[0], length) != 0)
		{
   	        test_failed = 1;
            printf("pktioRxCallback: packet received from port %d as expected, but packet data not match\n", port);
		}
	} else if ((egress_port[1] == port)&&(FALSE == alternate_path)){
		if(sal_memcmp(buffer, expected_packets[1], length) != 0)
		{
   	        test_failed = 1;
            printf("pktioRxCallback: packet received from port %d as expected, but packet data not match\n", port);
		}
	} else if ((egress_port[2] == port)&&(FALSE == alternate_path)){
		if(sal_memcmp(buffer, expected_packets[2], length) != 0)
		{
   	        test_failed = 1;
            printf("pktioRxCallback: packet received from port %d as expected, but packet data not match\n", port);
		}
	} else if ((egress_port[3] == port)&&(TRUE == alternate_path)){
		if(sal_memcmp(buffer, expected_packets[3], length) != 0)
		{
   	        test_failed = 1;
            printf("pktioRxCallback: packet received from port %d as expected, but packet data not match\n", port);
		}
	} else if ((egress_port[4] == port)&&(TRUE == alternate_path)){
		if(sal_memcmp(buffer, expected_packets[4], length) != 0)
		{
   	        test_failed = 1;
            printf("pktioRxCallback: packet received from port %d as expected, but packet data not match\n", port);
		}
	}

    return BCM_PKTIO_RX_NOT_HANDLED;
}

/* Register callback function for received packets */
bcm_error_t
registerPktioRxCallback(int unit)
{
    const uint32        flags = 0;      /* Flags only used on match (for now) */

    return bcm_pktio_rx_register
      (unit, "rx_cb", pktioRxCallback, priority, &rx_count, flags);
}

bcm_error_t
unregisterPktioRxCallback(int unit)
{
    return bcm_pktio_rx_unregister(unit, pktioRxCallback, priority);
}

/* Create vlan and add port to vlan */
bcm_error_t
vlan_create_add_port(int unit, int vid, int port)
{
    bcm_pbmp_t pbmp, upbmp;

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);
}

/* Create vlan and add array of ports to vlan */
int
vlan_create_add_ports(int unit, int vid, int count, bcm_port_t *ports)
{
    bcm_pbmp_t pbmp, upbmp;
    int i;

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);

    for (i = 0; i < count; i++) {
        BCM_PBMP_PORT_ADD(pbmp, ports[i]);
    }

    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);
}

/* Destroy vlan and remove port from vlan */
bcm_error_t
vlan_destroy_remove_port(int unit, int vid, int port)
{
    bcm_pbmp_t pbmp;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, vid, pbmp));
    return bcm_vlan_destroy(unit, vid);
}

/* Destroy vlan and remove ports from vlan */
bcm_error_t
vlan_destroy_remove_ports(int unit, int vid, int count, int *ports)
{
    bcm_pbmp_t pbmp;
    int i;

    BCM_PBMP_CLEAR(pbmp);
    for (i = 0; i < count; i++) {
        BCM_PBMP_PORT_ADD(pbmp, ports[i]);
	}

    BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, vid, pbmp));
    return bcm_vlan_destroy(unit, vid);
}

/* Create vlan and add array of ports to vlan */
bcm_error_t
vlan_create_add_ports(int unit, int vid, int count, bcm_port_t *ports)
{
    bcm_pbmp_t pbmp, upbmp;
    int i;

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);

    for (i = 0; i < count; i++) {
        BCM_PBMP_PORT_ADD(pbmp, ports[i]);
    }

    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);
}

bcm_error_t
create_l3_ingress_interface(int unit, bcm_vrf_t vrf, bcm_if_t *ingress_if)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_l3_ingress_t l3_ingress;

    bcm_l3_ingress_t_init(&l3_ingress);
    if(*ingress_if)
        l3_ingress.flags = BCM_L3_INGRESS_WITH_ID;
    l3_ingress.vrf = vrf;
    rv = bcm_l3_ingress_create(unit, &l3_ingress, ingress_if);

    return rv;
}

bcm_error_t
clear_l3_ingress_interface(int unit, bcm_if_t intf_id)
{
    bcm_error_t rv = BCM_E_NONE;

	rv = bcm_l3_ingress_destroy(unit, intf_id);
    return rv;
}

bcm_error_t
create_l3_interface(int unit, int flags, bcm_mac_t local_mac, int vid, bcm_if_t *intf_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_l3_intf_t l3_intf;

    /* L3 interface and egress object for access */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags |= flags;
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
    l3_intf.l3a_vid = vid;
    l3_intf.l3a_intf_id = *intf_id;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_intf_create()%s\n", bcm_errmsg(rv));
        return rv;
    }
    *intf_id = l3_intf.l3a_intf_id;

    return rv;
}

bcm_error_t
clear_l3_interface(int unit, bcm_if_t intf_id)
{
    bcm_l3_intf_t l3_intf;
    bcm_error_t rv = BCM_E_NONE;

    bcm_l3_intf_t_init(&l3_intf);
	l3_intf.l3a_intf_id = intf_id;
	rv = bcm_l3_intf_get(unit, &l3_intf);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_intf_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_l3_intf_delete(unit, &l3_intf);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_intf_delete %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/* Create L3 egress object */
bcm_error_t
create_egr_obj(int unit, int l3_if, bcm_mac_t nh_mac, bcm_port_t port,
               bcm_if_t *egr_obj_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_l3_egress_t l3_egress;

    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr,  nh_mac, sizeof(nh_mac));
    l3_egress.intf  = l3_if;
    l3_egress.port = port;
    rv = bcm_l3_egress_create(unit, 0, &l3_egress, egr_obj_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_egress_create() %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/* Clear L3 egress object */
bcm_error_t
clear_egr_obj(int unit, bcm_if_t egr_obj_id)
{
    return bcm_l3_egress_destroy(unit, egr_obj_id);
}

int config_l3_egress_flexctr(int unit, int egr_obj_num, bcm_if_t *egr_obj_id, uint32 *stat_counter_id)
{
    int rv;
	int i = 0;
    int options = 0;
    bcm_flexctr_counter_value_t counter_value;
    bcm_flexctr_action_t action = {0};
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;

    action.source = bcmFlexctrSourceL3Egress;
    action.hint = 0;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_num = 16;

    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectStaticEgrL3NextHop;
    index_op->mask_size[0] = 16;
    index_op->shift[0] = 0;
    index_op->object[1] = bcmFlexctrObjectConstZero;
    index_op->mask_size[1] = 1;
    index_op->shift[1] = 0;

    /* Increase counter per packet. */
    value_a_op = &action.operation_a;
    value_a_op->select = bcmFlexctrValueSelectCounterValue;
    value_a_op->object[0] = bcmFlexctrObjectConstOne;
    value_a_op->mask_size[0] = 16;
    value_a_op->shift[0] = 0;
    value_a_op->object[1] = bcmFlexctrObjectConstZero;
    value_a_op->mask_size[1] = 1;
    value_a_op->shift[1] = 0;
    value_a_op->type = bcmFlexctrValueOperationTypeInc;

    /* Increase counter per packet bytes. */
    value_b_op = &action.operation_b;
    value_b_op->select = bcmFlexctrValueSelectPacketLength;
    value_b_op->type = bcmFlexctrValueOperationTypeInc;

    /* First action in group. */
    rv = bcm_flexctr_action_create(unit, options, &action, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_action_create %s\n", bcm_errmsg(rv));
        return rv;
    }
	print *stat_counter_id;

	for(i = 0; i < egr_obj_num; i++){
        rv = bcm_l3_egress_stat_attach(unit, egr_obj_id[i], *stat_counter_id);
        if (BCM_FAILURE(rv)) {
            printf("ERROR: bcm_l3_egress_stat_attach(%d) - %d : %s.\n", egr_obj_id[i], rv, bcm_errmsg(rv));
            return rv;
        }
	}

    return rv;
}

/* Config my_station_tcam to enable l3 lookup */
bcm_error_t enable_l3_lookup(int unit, bcm_mac_t router_mac_in, bcm_vlan_t in_vlan, int *station_id)
{
	bcm_error_t rv = BCM_E_NONE;
    bcm_l2_station_t l2_station;

    bcm_l2_station_t_init(&l2_station);
	l2_station.vlan = in_vlan;
	l2_station.vlan_mask = BCM_VLAN_MAX;
    sal_memcpy(l2_station.dst_mac, router_mac_in, sizeof(router_mac_in));
    l2_station.dst_mac_mask = "ff:ff:ff:ff:ff:ff";
    l2_station.flags = BCM_L2_STATION_IPV4 ;
    rv = bcm_l2_station_add(unit, station_id, &l2_station);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l2_station_add() %s\n", bcm_errmsg(rv));
        return rv;
    }

	return rv;
}

/* Clear my_station_tcam entry to disable l3 looup*/
bcm_error_t disable_l3_lookup(int unit, int station_id)
{
	bcm_error_t rv = BCM_E_NONE;

    rv = bcm_l2_station_delete(unit, station_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l2_station_delete() %s\n", bcm_errmsg(rv));
        return rv;
    }

	return rv;
}

bcm_error_t
check_flexctr(int unit, uint32 stat_counter_id, int *flexctr_check_failed)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 num_entries = egress_port_num+1;
    uint32 counter_index[egress_port_num+1] = {0};
    bcm_flexctr_counter_value_t counter_value[egress_port_num+1];
    int expected_packet_length = 82;
    int egr_obj_id_base = 100000;
	int primary_path_received=0;
	int alternate_path_received=0;
	int i=0;

	for(i=1;i<=egress_port_num;i++){
		counter_index[i] = egr_obj_id[i-1] - egr_obj_id_base;
	}
	print counter_index;

    /* Get counter value. */
    sal_memset(counter_value, 0, sizeof(counter_value));
    rv = bcm_flexctr_stat_get(unit,
                              stat_counter_id,
                              num_entries,
                              counter_index,
                              counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }

	for(i=0;i<egress_port_num;i++){
    printf("Flexctr Get : %d packets / %d bytes from egr l3 obj:%d\n",
           COMPILER_64_LO(counter_value[i+1].value[0]),
           COMPILER_64_LO(counter_value[i+1].value[1]),
		   egr_obj_id[i]);
    }

	/* expected to receive 1 packet from primary path and alternate path */
	for(i=0;i<egress_port_num;i++){
    if ((COMPILER_64_LO(counter_value[i+1].value[0]) == 1) &&
	   (COMPILER_64_LO(counter_value[i+1].value[1]) == expected_packet_length)){
		if(i < primary_path_count){
		    primary_path_received++;
		}else{
		    alternate_path_received++;
		}
	}
	}


	if((1 != primary_path_received)||(1 != alternate_path_received)){
        printf("Flexctr packets verify failed, primary path received %d packet, alternate path received %d packet.n",
			primary_path_received, alternate_path_received);
        *flexctr_check_failed = 1;
	    print counter_index;
        print counter_value;
        rv = BCM_E_FAIL;
	}

    return rv;
}

bcm_error_t config_ecmp_rtag7(int unit)
{
    int flags;
    bcm_error_t rv = BCM_E_NONE;

    /* Enable RTAG7 for ECMP hashing, enable DIP,TCP/UDP ports for ECMP RTAG7 hashing */
    rv = bcm_switch_control_get(unit, bcmSwitchHashControl, &flags);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_get(): %s.\n", bcm_errmsg(rv));
      return rv;
    }
    flags |= BCM_HASH_CONTROL_ECMP_ENHANCE |
  	  BCM_HASH_CONTROL_MULTIPATH_L4PORTS |
  	  BCM_HASH_CONTROL_MULTIPATH_DIP;
    rv = bcm_switch_control_set(unit, bcmSwitchHashControl, flags);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    /* Use port based hash selection (RTAG7_HASH_SEL->USE_FLOW_SEL_ECMP) */
    rv = bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelEcmp, FALSE);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    /* Use port based hash selection (RTAG7_HASH_SEL->USE_FLOW_SEL_DLB_ECMP) */
    rv = bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelEcmpDynamic, FALSE);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    flags = BCM_HASH_FIELD_SRCMOD |
      BCM_HASH_FIELD_SRCPORT |
      BCM_HASH_FIELD_PROTOCOL |
      BCM_HASH_FIELD_DSTL4 |
      BCM_HASH_FIELD_SRCL4 |
      BCM_HASH_FIELD_IPV4_ADDRESS;

    /* Block A - L3 packet field selection (RTAG7_HASH_FIELD_BMAP_1) */
    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4Field0, flags);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    /* Block A - L3 IPv4 TCP/UDP field selection (RTAG7_IPV4_TCP_UDP_HASH_FIELD_BMAP_2) */
    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField0, flags);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    /* Block B - L3 packet field selection (RTAG7_HASH_FIELD_BMAP_1) */
    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4Field1, flags);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    /* Block B - L3 IPv4 TCP/UDP field selection (RTAG7_IPV4_TCP_UDP_HASH_FIELD_BMAP_2) */
    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField1, flags);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    /* Configure Seed */
    rv = bcm_switch_control_set(unit, bcmSwitchHashSeed0, 0x33333333);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashSeed1, 0x55555555);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    /* Enable preprocess */
    rv = bcm_switch_control_set(unit, bcmSwitchHashField0PreProcessEnable, TRUE);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashField1PreProcessEnable, TRUE);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    /* Configure HASH A and HASH B functions */
    rv = bcm_switch_control_set(unit, bcmSwitchHashField0Config, BCM_HASH_FIELD_CONFIG_CRC32LO);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashField0Config1, BCM_HASH_FIELD_CONFIG_CRC32HI);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashField1Config, BCM_HASH_FIELD_CONFIG_CRC32HI);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashField1Config1, BCM_HASH_FIELD_CONFIG_CRC32LO);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    return rv;
}

bcm_error_t clear_ecmp_rtag7(int unit)
{
	int flags;
	bcm_error_t rv = BCM_E_NONE;

	/* Disable RTAG7 for ECMP hashing, Disable DIP,TCP/UDP ports for ECMP RTAG7 hashing */
	flags = 0;
	rv = bcm_switch_control_set(unit, bcmSwitchHashControl, flags);
	if (BCM_FAILURE(rv)) {
	  printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	  return rv;
	}

	/* Use flow based hash selection (RTAG7_HASH_SEL->USE_FLOW_SEL_ECMP) */
	rv = bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelEcmp, TRUE);
	if (BCM_FAILURE(rv)) {
	  printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	  return rv;
	}

    /* Use flow based hash selection (RTAG7_HASH_SEL->USE_FLOW_SEL_DLB_ECMP) */
	rv = bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelEcmpDynamic, TRUE);
	if (BCM_FAILURE(rv)) {
		printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
		return rv;
	 }

	flags = 0;
	/* Reset Block A - L3 packet field selection (RTAG7_HASH_FIELD_BMAP_1) */
	rv = bcm_switch_control_set(unit, bcmSwitchHashIP4Field0, flags);
	if (BCM_FAILURE(rv)) {
	  printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	  return rv;
	}

	/* Reset Block A - L3 IPv4 TCP/UDP field selection (RTAG7_IPV4_TCP_UDP_HASH_FIELD_BMAP_2) */
	rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField0, flags);
	if (BCM_FAILURE(rv)) {
	  printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	  return rv;
	}

	/* Reset Block B - L3 packet field selection (RTAG7_HASH_FIELD_BMAP_1) */
	rv = bcm_switch_control_set(unit, bcmSwitchHashIP4Field1, flags);
	if (BCM_FAILURE(rv)) {
	  printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	  return rv;
	}

	/* Reset Block B - L3 IPv4 TCP/UDP field selection (RTAG7_IPV4_TCP_UDP_HASH_FIELD_BMAP_2) */
	rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField1, flags);
	if (BCM_FAILURE(rv)) {
	  printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	  return rv;
	}

	/* Reset Hash Seed0 */
	rv = bcm_switch_control_set(unit, bcmSwitchHashSeed0, 0);
	if (BCM_FAILURE(rv)) {
	  printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	  return rv;
	}

	/* Reset Hash Seed1 */
	rv = bcm_switch_control_set(unit, bcmSwitchHashSeed1, 0);
	if (BCM_FAILURE(rv)) {
	  printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	  return rv;
	}

	/* Disable preprocess */
	rv = bcm_switch_control_set(unit, bcmSwitchHashField0PreProcessEnable, FALSE);
	if (BCM_FAILURE(rv)) {
	  printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	  return rv;
	}

	rv = bcm_switch_control_set(unit, bcmSwitchHashField1PreProcessEnable, FALSE);
	if (BCM_FAILURE(rv)) {
	  printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	  return rv;
	}

	/* Configure HASH A and HASH B functions */
	rv = bcm_switch_control_set(unit, bcmSwitchHashField0Config, 0);
	if (BCM_FAILURE(rv)) {
	  printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	  return rv;
	}

	rv = bcm_switch_control_set(unit, bcmSwitchHashField0Config1, 0);
	if (BCM_FAILURE(rv)) {
	  printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	  return rv;
	}

	rv = bcm_switch_control_set(unit, bcmSwitchHashField1Config, 0);
	if (BCM_FAILURE(rv)) {
	  printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	  return rv;
	}

	rv = bcm_switch_control_set(unit, bcmSwitchHashField1Config1, 0);
	if (BCM_FAILURE(rv)) {
	  printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	  return rv;
	}

	return rv;
}

/* Global (per device) Configuration */
struct glb_dlb_config_t {
    int   sc_type;
    int   sc_value;
};

glb_dlb_config_t
glb_ecmp_config[] =
{
    /* EMWA parameters */
    {bcmSwitchEcmpDynamicEgressBytesExponent, 3},
    {bcmSwitchEcmpDynamicEgressBytesDecreaseReset, 0},
    {bcmSwitchEcmpDynamicQueuedBytesExponent, 3},
    {bcmSwitchEcmpDynamicQueuedBytesDecreaseReset, 0},
    {bcmSwitchEcmpDynamicPhysicalQueuedBytesExponent, 3},
    {bcmSwitchEcmpDynamicPhysicalQueuedBytesDecreaseReset, 0},

    /* Quantization parameters, SDK config 8 equally spaced bands between Min & Max */
    {bcmSwitchEcmpDynamicEgressBytesMinThreshold, 1000},   /* 10% of 10Gbs, unit Mbs */
    {bcmSwitchEcmpDynamicEgressBytesMaxThreshold, 10000},  /* 10 Gbs, unit Mbs */
    {bcmSwitchEcmpDynamicQueuedBytesMinThreshold, 0x4C60}, /* in bytes, 0x2F cells x 208Bytes */
    {bcmSwitchEcmpDynamicQueuedBytesMaxThreshold, 0xD000}, /* 218 x 0x100 cells */
    {bcmSwitchEcmpDynamicPhysicalQueuedBytesMinThreshold, 0x2630}, /* 218 x 0x5E cells */
    {bcmSwitchEcmpDynamicPhysicalQueuedBytesMaxThreshold, 0x6800}, /* 218 x 0x80 cells */
    {-1, 0}
};

/* ECMP Group - primary versus alt path selection criteria */
bcm_l3_dgm_t
dgm_selection_criteria =
{
    6, /* threshold */
    3, /* cost */
    5, /* bias */
};

/* Global DLB Configuraton */
int global_ecmp_dlb_config(int unit)
{
    int glb_ecmp_dlb_ethtypes[] = {0x0800, 0x86DD};
    int i, rv = BCM_E_NONE;

    /* Sample Rate */
    rv = bcm_switch_control_set(unit, bcmSwitchEcmpDynamicSampleRate, 62500);  /* 16 us*/

    /* EMWA and Quantization parameters */
    for (i = 0; ; i++) {
        if (glb_ecmp_config[i].sc_type == -1) {
            break;
        }
        rv = bcm_switch_control_set(unit, glb_ecmp_config[i].sc_type, glb_ecmp_config[i].sc_value);
    }

    /* Set the random seed */
    bcm_switch_control_set(unit, bcmSwitchEcmpDynamicRandomSeed, 0x5555);

    /* Eligibility based on EtherType */
    rv = bcm_l3_egress_ecmp_ethertype_set(unit,
                      BCM_L3_ECMP_DYNAMIC_ETHERTYPE_ELIGIBLE,
                      sizeof(glb_ecmp_dlb_ethtypes)/sizeof(glb_ecmp_dlb_ethtypes[0]),
                      glb_ecmp_dlb_ethtypes);

    return rv;
}

/* Global DLB Configuraton clear */
int Clear_global_ecmp_dlb_config(int unit)
{
    int glb_ecmp_dlb_ethtypes[] = {0};
    int i, rv = BCM_E_NONE;

    /* EMWA and Quantization parameters */
    for (i = 0; ; i++) {
        if (glb_ecmp_config[i].sc_type == -1) {
            break;
        }
        rv = bcm_switch_control_set(unit, glb_ecmp_config[i].sc_type, 0);
    }

    /* Set the random seed */
    bcm_switch_control_set(unit, bcmSwitchEcmpDynamicRandomSeed, 0);

    /* Eligibility based on EtherType */
    rv = bcm_l3_egress_ecmp_ethertype_set(unit,
                      0,
                      sizeof(glb_ecmp_dlb_ethtypes)/sizeof(glb_ecmp_dlb_ethtypes[0]),
                      glb_ecmp_dlb_ethtypes);

    return rv;
}

bcm_error_t
config_rtag7_l3_ecmp_dgm(int unit)
{
	bcm_error_t rv = BCM_E_NONE;
	int flags, i;
    bcm_if_t ingress_if = 0x1b2;
    bcm_l3_egress_ecmp_t ecmp_info;
    bcm_l3_ecmp_member_t ecmp_member_array[egress_port_num];
    int ecmp_dynamic_age = 256; /* usec */
    int ecmp_dynamic_size = 512; /* Flowset table size */
    int ecmp_loading_weight[egress_port_num] = {50, 50, 50, 50, 50};
    int ecmp_queue_size_weight[egress_port_num] = {40, 40, 40, 40, 40};
    int ecmp_scaling_factor[egress_port_num] = {10, 10, 10, 10, 10};
	bcm_l3_ecmp_dlb_port_quality_attr_t quality_attr;

    rv = config_ecmp_rtag7(unit);
    if (BCM_FAILURE(rv)) {
        printf("\nError in config_ecmp_rtag7(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Global DLB Configuraton */
    rv = global_ecmp_dlb_config(unit);
    if (BCM_FAILURE(rv)) {
        printf("\nError in global_ecmp_dlb_config(): %s.\n", bcm_errmsg(rv));
        return rv;
	}

    /* Create and add ingress port to ingress VLAN */
    rv = vlan_create_add_port(unit, in_vlan, ingress_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_create_add_port(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

	for(i=0;i<egress_port_num;i++){
    /* Create and add egress port to egress VLAN */
    rv = vlan_create_add_port(unit, out_vlan[i], egress_port[i]);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_create_add_port(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
	}

	/* Enable L3 Lookup for incoming l3 packet */
    rv = enable_l3_lookup(unit, router_mac_in, in_vlan, &my_station_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in enable_l3_lookup(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create L3 ingress interface */
    rv = create_l3_ingress_interface(unit, vrf, &ingress_if);
    if (BCM_FAILURE(rv)) {
        printf("\nError in create_l3_ingress_interface(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
	print ingress_if;

    /* Config vlan_id to l3_iif mapping */
    bcm_vlan_control_vlan_t vlan_ctrl;
    bcm_vlan_control_vlan_t_init(&vlan_ctrl);
    bcm_vlan_control_vlan_get(unit, in_vlan, &vlan_ctrl);
    vlan_ctrl.ingress_if = ingress_if;
    print bcm_vlan_control_vlan_set(unit, in_vlan, vlan_ctrl);


    /* Create egress L3 interface */
	for(i=0;i<egress_port_num;i++){
	flags = 0;
    rv = create_l3_interface(unit, flags, router_mac_out[i], out_vlan[i], &l3_intf_id[i]);
    if (BCM_FAILURE(rv)) {
        printf("\nError in create_l3_interface: %s.\n",bcm_errmsg(rv));
        return rv;
    }
    printf("l3_intf_id[%d] = %d\n", i, l3_intf_id[i]);

    /* Create L3 egress object */
    rv = create_egr_obj(unit, l3_intf_id[i], dst_mac[i], egress_port[i], &egr_obj_id[i]);
    if (BCM_FAILURE(rv)) {
        printf("\nError in create_egr_obj(): %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("egr_obj_id[%d] = %d\n", i, egr_obj_id[i]);

	/*Set DLB egress port attributes*/
	bcm_l3_ecmp_dlb_port_quality_attr_t_init(&quality_attr);
	quality_attr.load_weight = ecmp_loading_weight[i];
	quality_attr.queue_size_weight = ecmp_queue_size_weight[i];
	quality_attr.scaling_factor = ecmp_scaling_factor[i];
	print bcm_l3_ecmp_dlb_port_quality_attr_set(unit, egress_port[i], &quality_attr);
	}

    /* Level 2 ECMP group */
    bcm_l3_egress_ecmp_t_init(&ecmp_info);
    ecmp_info.ecmp_group_flags = BCM_L3_ECMP_UNDERLAY;
    ecmp_info.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DGM;
    ecmp_info.dynamic_size = ecmp_dynamic_size;
    ecmp_info.dynamic_age = ecmp_dynamic_age;
    ecmp_info.max_paths = 128;
    ecmp_info.dgm.threshold = dgm_selection_criteria.threshold;
    ecmp_info.dgm.cost      = dgm_selection_criteria.cost;
    ecmp_info.dgm.bias      = dgm_selection_criteria.bias;

	flags = 0;
	for(i=0;i<egress_port_num;i++){
    bcm_l3_ecmp_member_t_init(&ecmp_member_array[i]);
    ecmp_member_array[i].egress_if = egr_obj_id[i];
	ecmp_member_array[i].flags = (i < primary_path_count) ? 0 : BCM_L3_ECMP_MEMBER_DGM_ALTERNATE;
	}
    rv = bcm_l3_ecmp_create(unit, flags, &ecmp_info, egress_port_num, ecmp_member_array);
    if (BCM_FAILURE(rv)) {
       printf("Error executing bcm_l3_ecmp_create(): %s.\n", bcm_errmsg(rv));
       return rv;
    }
    ecmp_group_id = ecmp_info.ecmp_intf;
    print ecmp_group_id;

    /* Configure ECMP member, per hardware link status. This configuration has one member per port */
	for(i=0;i<egress_port_num;i++){
    rv = bcm_l3_egress_ecmp_member_status_set(unit, egr_obj_id[i], BCM_L3_ECMP_DYNAMIC_MEMBER_HW);
    if (BCM_FAILURE(rv)) {
       printf("Error executing bcm_l3_egress_ecmp_member_status_set(): %s.\n", bcm_errmsg(rv));
       return rv;
    }
	}

    /* Install the route for DIP */
    bcm_l3_route_t route_info;
    bcm_l3_route_t_init(&route_info);
	route_info.l3a_flags |= BCM_L3_MULTIPATH;
    route_info.l3a_intf = ecmp_group_id;
    route_info.l3a_subnet = dip;
    route_info.l3a_ip_mask = mask;
    route_info.l3a_vrf = vrf;
    rv = bcm_l3_route_add(unit, &route_info);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_l3_route_add(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

	/* create flex ctr and attach to l3 egress objects */
	rv = config_l3_egress_flexctr(unit, egress_port_num, egr_obj_id, &stat_counter_id);
    if (BCM_FAILURE(rv)) {
       printf("ERROR: config_l3_egress_flexctr() : %s.\n", bcm_errmsg(rv));
       return rv;
    }
	print stat_counter_id;

	return rv;
}

bcm_error_t clear_rtag7_l3_ecmp_dgm(int unit)
{
	bcm_error_t rv = BCM_E_NONE;

    bcm_l3_route_t route_info;
	bcm_l3_egress_t egr;
    bcm_vlan_control_vlan_t vlan_ctrl;
	bcm_l3_ecmp_dlb_port_quality_attr_t quality_attr;
    int i=0;

    rv = clear_ecmp_rtag7(unit);
    if (BCM_FAILURE(rv)) {
       printf("Error in clear_ecmp_rtag7(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

	rv = Clear_global_ecmp_dlb_config(unit);
    if (BCM_FAILURE(rv)) {
       printf("Error in Clear_global_ecmp_dlb_config(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

    /* Config vlan_id to l3_iif mapping */
    bcm_vlan_control_vlan_t_init(&vlan_ctrl);
    bcm_vlan_control_vlan_get(unit, in_vlan, &vlan_ctrl);
    print vlan_ctrl.ingress_if;
	rv = clear_l3_ingress_interface(unit, vlan_ctrl.ingress_if);
    if (BCM_FAILURE(rv)) {
       printf("Error in clear_l3_ingress_interface(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

    vlan_ctrl.ingress_if = 0;
    rv = bcm_vlan_control_vlan_set(unit, in_vlan, vlan_ctrl);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_vlan_control_vlan_set(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

	for(i=0;i<egress_port_num;i++){
	rv = clear_l3_interface(unit, l3_intf_id[i]);
    if (BCM_FAILURE(rv)) {
       printf("Error in clear_l3_interface(): %s.\n", bcm_errmsg(rv));
       return rv;
    }
	}

	bcm_l3_ecmp_dlb_port_quality_attr_t_init(&quality_attr);
	for(i=0;i<egress_port_num;i++){
	rv = bcm_l3_ecmp_dlb_port_quality_attr_set(unit, egress_port[i], &quality_attr);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_l3_ecmp_dlb_port_quality_attr_set(): %s.\n", bcm_errmsg(rv));
       return rv;
    }
	}

	rv = bcm_l3_ecmp_destroy(unit, ecmp_group_id);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_l3_ecmp_destroy(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

	for(i=0;i<egr_obj_num;i++){
	rv = clear_egr_obj(unit, egr_obj_id[i]);
    if (BCM_FAILURE(rv)) {
       printf("Error in clear_egr_obj(): %s.\n", bcm_errmsg(rv));
       return rv;
    }
	}

    bcm_l3_route_t_init(&route_info);
    route_info.l3a_subnet = dip;
    route_info.l3a_ip_mask = mask;
    route_info.l3a_vrf = vrf;
    rv = bcm_l3_route_get(unit, &route_info);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_l3_route_get(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

	rv = bcm_l3_route_delete(unit, &route_info);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_l3_route_delete(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

    rv = disable_l3_lookup(unit, my_station_id);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_l3_route_delete(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

	for(i=0;i<egress_port_num;i++){
	BCM_IF_ERROR_RETURN(vlan_destroy_remove_port(unit, out_vlan[i], egress_port[i]));
	}
	BCM_IF_ERROR_RETURN(vlan_destroy_remove_port(unit, in_vlan, ingress_port));

    return rv;
}

bcm_error_t
clear_l3_egress_flexctr(int unit,  int egr_obj_num, bcm_if_t *egr_obj_id, uint32 stat_counter_id)
{
    bcm_error_t rv = BCM_E_NONE;
	int i=0;

    /* Detach flexctr from l3 egr objects */
	for(i = 0; i < egr_obj_num; i++){
	    rv = bcm_l3_egress_stat_detach(unit, egr_obj_id[i]);
            if (BCM_FAILURE(rv)) {
                printf("bcm_l3_egress_stat_detach() %s\n", bcm_errmsg(rv));
                return rv;
            }
	}

    rv = bcm_flexctr_action_destroy(unit, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_action_destroy() %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

void Overide_port_quality(void)
{
    char   str[512];

    printf("Program the socmem directly to let DGM choose alternate path members.\n");
    snprintf(str, 512, "bsh -c 'pt DLB_ECMP_PORT_QUALITY_MAPPINGm set BCMLT_PT_INDEX=1023  ASSIGNED_QUALITY=6'");
    bshell(unit, str);
    snprintf(str, 512, "bsh -c 'pt DLB_ECMP_PORT_QUALITY_MAPPINGm set BCMLT_PT_INDEX=1535  ASSIGNED_QUALITY=7'");
    bshell(unit, str);
    snprintf(str, 512, "bsh -c 'pt DLB_ECMP_QUANTIZE_CONTROLm set BCMLT_PT_INDEX=%d PORT_QUALITY_MAPPING_PROFILE_PTR=2'", egress_port[3]);
    bshell(unit, str);
    snprintf(str, 512, "bsh -c 'pt DLB_ECMP_QUANTIZE_CONTROLm set BCMLT_PT_INDEX=%d PORT_QUALITY_MAPPING_PROFILE_PTR=2'", egress_port[4]);
    bshell(unit, str);

	return;
}

void test_verify(int unit)
{
	bcm_error_t rv = BCM_E_NONE;
    char   str[512];

    expected_packets[0] = expected_egress_packet0;
    expected_packets[1] = expected_egress_packet1;
    expected_packets[2] = expected_egress_packet2;
    expected_packets[3] = expected_egress_packet3;
    expected_packets[4] = expected_egress_packet4;

    rv = registerPktioRxCallback(unit);
    if (BCM_FAILURE(rv)) {
        printf("registerPktioRxCallback() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

	alternate_path = FALSE; /* indication of choosing primary path or alternate path */

    bshell(unit, "sleep quiet 1");
    printf("Transmiting l3 packet into ingress_port:%d first time, expect go through primary path\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000000001002a107777008100000B08004500003c67620000ff01bd760a3a40010a3a42730000550a000100516162636465666768696a6b6c6d6e6f7071727374757677616263646566676869; sleep quiet 1", ingress_port);
    bshell(unit, str);

    Overide_port_quality();
	alternate_path = TRUE; /* indication of choosing primary path or alternate path */

    bshell(unit, "sleep quiet 1");
    printf("Transmiting l3 packet into ingress_port:%d second time, expect go through alternate path\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000000001002a107777008100000B08004500003c67620000ff01bd760a3a40010a3a42730000550a000100516162636465666768696a6b6c6d6e6f7071727374757677616263646566676869; sleep quiet 1", ingress_port);
    bshell(unit, str);


    bshell(unit, "sleep quiet 1");
    rv = check_flexctr(unit, stat_counter_id, &test_failed);
    if (BCM_FAILURE(rv)) {
        printf("check_flexctr() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("\n----------------------------------------------------------------------------------- \n");
    printf("    RESULT OF L3 IPv4 ROUTE to DGM ECMP GROUP with RTAG7 HASH VERIFICATION  ");
    printf("\n----------------------------------------------------------------------------------- \n");
    printf("Test = [%s]", !test_failed? "PASS":"FAIL");
    printf("\n------------------------------------- \n");

    if (!test_failed)
        return BCM_E_NONE;
    else
        return BCM_E_FAIL;
}

/*
 * Clean up pre-test setup.
 */
int test_cleanup(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_l3_route_t route_info;
	int i=0;

	/* unregister Pktio callback */
    unregisterPktioRxCallback(unit);

    /* Remove the flex counter from l3 egress object */
    rv = clear_l3_egress_flexctr(unit, egr_obj_num, egr_obj_id, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("clear_l3_egress_flexctr() failed.\n");
        return rv;
    }

    /* Remove the l3 route to DGM ECMP group related configuration */
	rv = clear_rtag7_l3_ecmp_dgm(unit);
    if (BCM_FAILURE(rv)) {
        printf("clear_rtag7_l3_ecmp_dgm() failed.\n");
        return rv;
	}

    /* Remove the field configiuration */
	BCM_IF_ERROR_RETURN(clear_field_resource(unit, ifp_group, ifp_entry, egress_port_num));

    /* Remove the port loopback configiuration */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, BCM_PORT_LOOPBACK_NONE));
    for(i=0;i<egress_port_num; i++){
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port[i], BCM_PORT_LOOPBACK_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egress_port[i], BCM_PORT_DISCARD_NONE));
	}

    return BCM_E_NONE;
}


/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) demonstrates the deployment for L3 route to DGM ECMP group with RTAG7 configure sequence
 *     (Done in config_rtag7_l3_ecmp_dgm())
 *  c) demonstrates the functionality(done in test_verify()).
 *  d) demonstrates the L3 IPv4 route to DGM ECMP group with RTAG7 teardown configure sequence
 *     (Done in test_cleanup())
 */
bcm_error_t
execute(void)
{
    int unit = 0;
    bcm_error_t rv;

    print "~~~ #1) test_setup(): ** start **";
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return rv;
    }
    print "~~~ #1) test_setup(): ** end **";

    print "~~~ #2) config_rtag7_l3_ecmp_dgm(): ** start **";
    if (BCM_FAILURE((rv = config_rtag7_l3_ecmp_dgm(unit)))) {
        printf("config_rtag7_l3_ecmp_dgm() failed.\n");
        return rv;
    }
    print "~~~ #2) config_rtag7_l3_ecmp_dgm(): ** end **";

    print "~~~ #3) test_verify(): ** start **";
    test_verify(unit);
    print "~~~ #3) test_verify(): ** end **";

    print "~~~ #4) test_cleanup(): ** start **";
    if (BCM_FAILURE(rv = test_cleanup(unit))) {
         printf("test_cleanup() failed.\n");
         return rv;
    }
    print "~~~ #4) test_cleanup(): ** end **";

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print "execute(): Start";
  print execute();
  print "execute(): End";
}

