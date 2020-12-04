/*
 * Feature  : Hierarchical ECMP routing
 *
 * Usage    : BCM.0> cint th4_hsdk_l3_hecmp_ipv4_route.c
 *
 * config   : th4_hsdk_l3_hecmp_ipv4_64x400.config.yml
 *
 * Log file : th4_hsdk_l3_hecmp_ipv4_route_log.txt
 *
 * Test Topology :
 *                      +--------------------+
 *                      |                    |
 *                      |       TH4          |
 *         ingress_port |                    |  egress_port
 *        --------->----+  Routing           +------->------
 *             vlan 2   |    ALPM            |  vlan 30, 40, 50 and 60, 70
 *        dst_mac-0x02  |    vrf -100        |  dst_mac-0x22, 0x2222, 0x222222 and 0x44, 0x4444
 *                      |    dip - 0xC0A80114|  src_mac-0x11, 0x1111, 0x111111 and 0x33, 0x3333
 *                      |    mask -0xFFFFFF00|
 *                      +--------------------+
 *
 *
 * Summary:
 * ========
 * cint script to demonstrate hierarchical ECMP forwarding in parallel mode
 * [l3_alpm_template: 2
 *  riot_overlay_l3_intf_mem_size: 4096
 *  riot_overlay_l3_egress_mem_size: 8192
 *  l3_ecmp_member_first_lkup_mem_size: 8192 in config file].
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup):
 *   =============================================
 *     a) Selects six ports and configure them in Loopback mode. Out of these
 *        six ports, one port is used as ingress_port and the three as egress
 *        ports for ecmp group 1 and other two as egress ports for ecmp group 2.
 *     b) Install an IFP rule to copy egress packets to CPU and also drop them.
 *        start packet watcher.
 *     c) Configure a IPv4 hierarchical ECMP route. This does the necessary
 * 		  configurations of vlans, L3_IIFs, egress intfs, next hops and ECMP
 *        underlay and overlay tables.
 *
 *     Note: IFP rule is meant for a verification purpose and is not part of the
 * 			 feature configuration.
 *
 *   2) Step2 - Verification (Done in test_verify):
 *   ==============================================
 *     a) Check the configurations by 'l3 intf show', 'l3 egress show',
 *        'l3 route show' and 'l3 ecmp show'
 *     b) Transmit the IPv4 packet.The packet used matches the hierarchical ECMP
 *        configured through script.
 *     c) Expected Result:
 *     ===================
 *       We see that dmac, smac, TTL and vlan are all changed as the packet gets
 *       routed through the egress ports and the number of sent packets should
 *       match the flex ctr stat get output
 */

/* Reset C interpreter*/
cint_reset();

int over_lay_ecmp_mem_cnt = 2;
int under_lay_1_ecmp_mem_cnt = 3;
int under_lay_2_ecmp_mem_cnt = 2;
bcm_port_t ingress_port;
bcm_vlan_t ing_vid = 2;
bcm_port_t egress_port_1[under_lay_1_ecmp_mem_cnt];
bcm_vlan_t egr_vid_1[under_lay_1_ecmp_mem_cnt] = {30, 40, 50};
bcm_mac_t src_mac_1[under_lay_1_ecmp_mem_cnt] = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x11},
						 {0x00, 0x00, 0x00, 0x00, 0x11, 0x11},
						 {0x00, 0x00, 0x00, 0x11, 0x11, 0x11}};
bcm_mac_t dst_mac_1[under_lay_1_ecmp_mem_cnt] = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x22},
						 {0x00, 0x00, 0x00, 0x00, 0x22, 0x22},
						 {0x00, 0x00, 0x00, 0x22, 0x22, 0x22}};
bcm_port_t egress_port_2[under_lay_2_ecmp_mem_cnt];
bcm_vlan_t egr_vid_2[under_lay_2_ecmp_mem_cnt] = {60, 70};
bcm_mac_t src_mac_2[under_lay_2_ecmp_mem_cnt] = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x33},
						 {0x00, 0x00, 0x00, 0x00, 0x33, 0x33}};
bcm_mac_t dst_mac_2[under_lay_2_ecmp_mem_cnt] = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x44},
						 {0x00, 0x00, 0x00, 0x00, 0x44, 0x44}};
bcm_mac_t router_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
bcm_if_t ingress_if = 100;
bcm_vrf_t vrf = 100;
bcm_ip_t dip = 0xC0A80114;
bcm_ip_t dip_mask = 0xFFFFFF00;
bcm_if_t egr_if_1[under_lay_1_ecmp_mem_cnt];
bcm_if_t egr_if_2[under_lay_2_ecmp_mem_cnt];
bcm_if_t ecmp_if_under_lay[over_lay_ecmp_mem_cnt];
bcm_if_t ecmp_if_over_lay;
uint32_t stat_counter_id, flexctr_obj_val = 7;

/*
 *    Checks if given port/index is already present
 *    in the list so that same port number is not generated again
 */

int
check_port_assigned(int *port_index_list,int no_entries, int index)
{
    int i=0;

    for (i= 0; i < no_entries; i++) {
        if (port_index_list[i] == index)
        return 1;
    }

    /* no entry found */
    return 0;
}

/*
 *    Provides required number of ports
 */

int
port_numbers_get(int unit, int *port_list, int num_ports)
{
    int i = 0, port = 0, index = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;
    int tempports[BCM_PBMP_PORT_MAX];
    int port_index_list[num_ports];

    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &configP));

    ports_pbmp = configP.e;
    for (i = 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i)) {
            tempports[port] = i;
            port++;
        }
    }

    if (( port == 0 ) || ( port < num_ports )) {
        printf("port_numbers_get() failed \n");
        return -1;
    }

    /* generating random ports */
    for (i= 0; i < num_ports; i++) {
        index = sal_rand() % port;
        if (check_port_assigned(port_index_list, i, index) == 0) {
            port_list[i] = tempports[index];
            port_index_list[i] = index;
        } else {
            i = i - 1;
        }
    }

    return BCM_E_NONE;
}

/*
 *    Configures the port in loopback mode
 */

int
ingress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_port_t     port;

    BCM_PBMP_ITER(pbm, port) {
        printf("Ingress_port_multi_setup port: %d setup\n", port);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
    }

    return BCM_E_NONE;
}

/*
 *    Configures the port in loopback mode and installs an IFP rule. This IFP
 *    rule copies the packets ingressing on the specified port to CPU. It is
 *    also configured to drop the packets on that port. This is to avoid
 *    continuous loopback of the packet from the egress port.
 */

int
egress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_field_group_t group;
    bcm_field_entry_t entry;
    bcm_port_t port;

    bcm_field_group_config_t group_config;

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    group_config.priority = 2;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_PBMP_ITER(pbm, port) {
        printf("Egress_port_multi_setup port: %d setup\n", port);

        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));

        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    }

    return BCM_E_NONE;
}

int test_failed = 0; /*Final result will be stored here */
int rx_count;   /* Global received packet count */

/*    Expected Egress packets     */

unsigned char expected_egress_packet_1[64] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x81, 0x00, 0x00, 0x1E,
        0x08, 0x00, 0x45, 0x00, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xA4, 0xE5, 0x0A, 0x0A,
        0x0A, 0x2A, 0xC0, 0xA8, 0x01, 0x14, 0x00, 0x01, 0x02, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x4D, 0x7B, 0xD6
};

unsigned char expected_egress_packet_2[64] = {
        0x00, 0x00, 0x00, 0x00, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x81, 0x00, 0x00, 0x28,
        0x08, 0x00, 0x45, 0x00, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xA4, 0xE5, 0x0A, 0x0A,
        0x0A, 0x2A, 0xC0, 0xA8, 0x01, 0x14, 0x00, 0x01, 0x02, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x4D, 0x7B, 0xD6
};

unsigned char expected_egress_packet_3[64] = {
        0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x81, 0x00, 0x00, 0x32,
        0x08, 0x00, 0x45, 0x00, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xA4, 0xE5, 0x0A, 0x0A,
        0x0A, 0x2A, 0xC0, 0xA8, 0x01, 0x14, 0x00, 0x01, 0x02, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x4D, 0x7B, 0xD6
};

unsigned char expected_egress_packet_4[64] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x81, 0x00, 0x00, 0x3C,
        0x08, 0x00, 0x45, 0x00, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xA4, 0xE5, 0x0A, 0x0A,
        0x0A, 0x2A, 0xC0, 0xA8, 0x01, 0x14, 0x00, 0x01, 0x02, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x4D, 0x7B, 0xD6
};

unsigned char expected_egress_packet_5[64] = {
        0x00, 0x00, 0x00, 0x00, 0x44, 0x44, 0x00, 0x00, 0x00, 0x00, 0x33, 0x33, 0x81, 0x00, 0x00, 0x46,
        0x08, 0x00, 0x45, 0x00, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xA4, 0xE5, 0x0A, 0x0A,
        0x0A, 0x2A, 0xC0, 0xA8, 0x01, 0x14, 0x00, 0x01, 0x02, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x4D, 0x7B, 0xD6
};


/*    Rx callback function for applications using the HSDK PKTIO    */

bcm_pktio_rx_t
pktio_Rx_call_back(int unit, bcm_pktio_pkt_t * packet, void *cookie)
{
    int *count = (auto) cookie;
    int result_packet_check = -1;
    int result_port_check = -1;
    void *buffer;
    uint32 length;
    uint32 src_port;

    (*count)++; /* Bump received packet count */

    /* Get basic packet info */
    if (BCM_FAILURE
        (bcm_pktio_pkt_data_get(unit, packet, (void *) &buffer, &length))) {

        return BCM_PKTIO_RX_NOT_HANDLED;
    }

    /* Get source port metadata */
    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                        (unit, packet, bcmPktioPmdTypeRx,
                         BCM_PKTIO_RXPMD_SRC_PORT_NUM, &src_port));

    if ((sal_memcmp(buffer, expected_egress_packet_1, length) == 0) ||
	(sal_memcmp(buffer, expected_egress_packet_2, length) == 0) ||
	(sal_memcmp(buffer, expected_egress_packet_3, length) == 0) ||
	(sal_memcmp(buffer, expected_egress_packet_4, length) == 0) ||
	(sal_memcmp(buffer, expected_egress_packet_5, length) == 0)) {
            result_packet_check = 0;
	}

    if((egress_port_1[0] == src_port) || (egress_port_1[1] == src_port) ||
       (egress_port_1[2] == src_port) || (egress_port_2[0] == src_port) ||
       (egress_port_2[1] == src_port))  {
	    result_port_check = 0;
    }

    test_failed = test_failed || (result_packet_check != 0) || (result_port_check != 0);

    return BCM_PKTIO_RX_NOT_HANDLED;
}

/*    Register callback function for received packets    */

int
register_pktioRx_callback(int unit)
{
    const uint8 priority = 101;
    const uint32 flags = 0;      /* Flags only used on match (for now) */

    return bcm_pktio_rx_register
      (unit, "rx_cb", pktio_Rx_call_back, priority, &rx_count, flags);
}

/*    RTAG7 configurations    */

int
config_rtag7(int unit)
{
    int flags;
    bcm_error_t rv;

    /* Enable RTAG7 for ECMP hashing, enable DIP,TCP/UDP ports for ECMP RTAG7 hashing */
    rv= bcm_switch_control_get(unit, bcmSwitchHashControl, &flags);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_switch_control_get(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    flags |= (BCM_HASH_CONTROL_ECMP_ENHANCE | BCM_HASH_CONTROL_MULTIPATH_L4PORTS | BCM_HASH_CONTROL_MULTIPATH_DIP);

    rv= bcm_switch_control_set(unit, bcmSwitchHashControl, flags);
    if (BCM_FAILURE(rv)) {
	printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Use port based hash selection (RTAG7_HASH_SEL->USE_FLOW_SEL_ECMP) */
    rv= bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelEcmp, TRUE);
    if (BCM_FAILURE(rv)) {
	printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	return rv;
    }

    rv= bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelEcmpOverlay, TRUE);
    if (BCM_FAILURE(rv)) {
	printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv= bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelMplsEcmp, FALSE);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    flags |= (BCM_HASH_FIELD_SRCMOD | BCM_HASH_FIELD_SRCPORT | BCM_HASH_FIELD_PROTOCOL |
	      BCM_HASH_FIELD_DSTL4 | BCM_HASH_FIELD_SRCL4 | BCM_HASH_FIELD_IPV4_ADDRESS);

    /* Block A - L3 packet field selection (RTAG7_HASH_FIELD_BMAP_1) */
    rv= bcm_switch_control_set(unit, bcmSwitchHashIP4Field0, flags);
    if (BCM_FAILURE(rv)) {
	printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	return rv;
    }

    /* Block A - L3 IPv4 TCP/UDP field selection (RTAG7_IPV4_TCP_UDP_HASH_FIELD_BMAP_2) */
    rv= bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField0, flags);
    if (BCM_FAILURE(rv)) {
	printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	return rv;
    }

    /* Block B - L3 packet field selection (RTAG7_HASH_FIELD_BMAP_1) */
    rv= bcm_switch_control_set(unit, bcmSwitchHashIP4Field1, flags);
    if (BCM_FAILURE(rv)) {
	printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	return rv;
    }

    /* Block B - L3 IPv4 TCP/UDP field selection (RTAG7_IPV4_TCP_UDP_HASH_FIELD_BMAP_2) */
    rv= bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField1, flags);
    if (BCM_FAILURE(rv)) {
	printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	return rv;
    }

    /* Configure Seed */
    rv= bcm_switch_control_set(unit, bcmSwitchHashSeed0, 0x33333333);
    if (BCM_FAILURE(rv)) {
	printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	return rv;
    }

    rv= bcm_switch_control_set(unit, bcmSwitchHashSeed1, 0x55555555);
    if (BCM_FAILURE(rv)) {
	printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	return rv;
    }

    /* Enable preprocess */
    rv= bcm_switch_control_set(unit, bcmSwitchHashField0PreProcessEnable, TRUE);
    if (BCM_FAILURE(rv)) {
	printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	return rv;
    }

    rv= bcm_switch_control_set(unit, bcmSwitchHashField1PreProcessEnable, TRUE);
    if (BCM_FAILURE(rv)) {
	printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	return rv;
    }

    /* Configure HASH A and HASH B functions */
    rv=bcm_switch_control_set(unit, bcmSwitchHashField0Config, BCM_HASH_FIELD_CONFIG_CRC32LO);
    if (BCM_FAILURE(rv)) {
	printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	return rv;
    }

    rv= bcm_switch_control_set(unit, bcmSwitchHashField0Config1, BCM_HASH_FIELD_CONFIG_CRC32HI);
    if (BCM_FAILURE(rv)) {
	printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	return rv;
    }

    rv= bcm_switch_control_set(unit, bcmSwitchHashField1Config, BCM_HASH_FIELD_CONFIG_CRC16CCITT);
    if (BCM_FAILURE(rv)) {
	printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	return rv;
    }

    rv= bcm_switch_control_set(unit, bcmSwitchHashField1Config1, BCM_HASH_FIELD_CONFIG_CRC16CCITT);
    if (BCM_FAILURE(rv)) {
	printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
	return rv;
    }

    return BCM_E_NONE;
}

/*    Configure packet spray for ingress ports to assign load balancing randomizer values for ECMP    */

int
config_packet_spray(int unit)
{
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchECMPLevel1RandomSeed, 0xff));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchECMPLevel2RandomSeed, 0xff88));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, ingress_port, bcmPortControlECMPLevel1LoadBalancingRandomizer, 6));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, ingress_port, bcmPortControlECMPLevel2LoadBalancingRandomizer, 8));

    return BCM_E_NONE;
}

/*    Create vlan and add port to vlan    */

int
create_vlan_add_port(int unit, bcm_vlan_t vlan, bcm_port_t port)
{
    bcm_error_t rv;
    bcm_pbmp_t pbmp, ubmp;

    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    rv = bcm_vlan_create(unit, vlan);
    if ((BCM_FAILURE(rv)) & (rv != BCM_E_EXISTS)) {
        printf("Error in creating vlan %s.\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan, pbmp, ubmp));

    return BCM_E_NONE;
}

/*    Create L3 Ingress Interface    */

int
create_l3_iif(int unit, bcm_vrf_t vrf, bcm_if_t ingress_if)
{
    bcm_error_t rv;
    bcm_l3_ingress_t l3_ingress;

    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.flags = BCM_L3_INGRESS_WITH_ID;
    l3_ingress.vrf   = vrf;

    rv = bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L3_IIF %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*    Config vlan_id to l3_iif mapping    */

int
configure_l3_iif_binding(int unit, bcm_vlan_t vlan, bcm_if_t ingress_if)
{
    bcm_error_t rv;
    bcm_vlan_control_vlan_t vlan_ctrl;

    bcm_vlan_control_vlan_t_init(&vlan_ctrl);
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_get(unit, vlan, &vlan_ctrl));
    vlan_ctrl.ingress_if = ingress_if;

    rv = bcm_vlan_control_vlan_set(unit, vlan, vlan_ctrl);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L3_IIF binding %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*    Create L3 interface    */

int
create_l3_intf(int unit, bcm_mac_t mac, bcm_vlan_t vlan, bcm_if_t *l3_intf_id)
{
    bcm_error_t rv;
    bcm_l3_intf_t l3_intf;

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_vid = vlan;
    sal_memcpy(l3_intf.l3a_mac_addr, mac, sizeof(mac));

    rv = bcm_l3_intf_create(unit, &l3_intf);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L3 interface %s.\n", bcm_errmsg(rv));
        return rv;
    }
    *l3_intf_id = l3_intf.l3a_intf_id;

    return BCM_E_NONE;
}

/*    Create L3 egress object    */

int
create_egr_obj(int unit, bcm_if_t l3_if, bcm_mac_t mac, bcm_gport_t gport,
               bcm_if_t *egr_if)
{
    bcm_error_t rv;
    bcm_l3_egress_t l3_egr;

    bcm_l3_egress_t_init(&l3_egr);
    l3_egr.intf = l3_if;
    l3_egr.port = gport;
    sal_memcpy(l3_egr.mac_addr, mac, sizeof(mac));

    rv = bcm_l3_egress_create(unit, 0, &l3_egr, egr_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L3 egress object %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*    Configure my station    */

int
configure_my_station_tcam(int unit, bcm_mac_t mac)
{
    bcm_error_t rv;
    bcm_mac_t mac_mask={0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    bcm_l2_station_t l2_station;
    int station_id;

    bcm_l2_station_t_init(&l2_station);
    sal_memcpy(l2_station.dst_mac, mac, sizeof(mac));
    sal_memcpy(l2_station.dst_mac_mask, mac_mask, sizeof(mac_mask));
    l2_station.flags = BCM_L2_STATION_IPV4 | BCM_L2_STATION_IPV6;

    rv = bcm_l2_station_add(unit, &station_id, &l2_station);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L2 station %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*    Adding ECMP route    */

int
configure_route(int unit, bcm_ip_t subnet_addr, bcm_ip_t subnet_mask, bcm_vrf_t vrf)
{
    bcm_error_t  rv;
    bcm_l3_route_t route;

    bcm_l3_route_t_init(&route);
    route.l3a_flags = BCM_L3_MULTIPATH;
    route.l3a_subnet  = dip;
    route.l3a_ip_mask = dip_mask;
    route.l3a_intf = ecmp_if_over_lay;
    route.l3a_vrf = vrf;
    rv = bcm_l3_route_add(unit, &route);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_l3_route_add %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*    Underlay ECMP configurations    */

int
configure_underlay_ecmp(int unit, int i)
{
    bcm_error_t rv;
    int ecmp_mem_cnt;
    bcm_vlan_t *egr_vid;
    bcm_port_t *egress_port;
    bcm_mac_t *src_mac;
    bcm_if_t egr_l3_if_1[under_lay_1_ecmp_mem_cnt];
    bcm_if_t egr_l3_if_2[under_lay_2_ecmp_mem_cnt];
    bcm_if_t *egr_l3_if;
    bcm_gport_t egress_gport_1[under_lay_1_ecmp_mem_cnt];
    bcm_gport_t egress_gport_2[under_lay_2_ecmp_mem_cnt];
    bcm_gport_t *egress_gport;
    bcm_mac_t *dst_mac;
    bcm_if_t *egr_if;
    bcm_l3_ecmp_member_t ecmp_member_array_1[under_lay_1_ecmp_mem_cnt];
    bcm_l3_ecmp_member_t ecmp_member_array_2[under_lay_2_ecmp_mem_cnt];
    bcm_l3_ecmp_member_t *ecmp_member_array;
    bcm_l3_egress_ecmp_t ecmp_info;
    int orig_i = i;

    if(orig_i==0) {
	ecmp_mem_cnt = under_lay_1_ecmp_mem_cnt;
	egr_vid = egr_vid_1;
	egress_port = egress_port_1;
	src_mac = src_mac_1;
	dst_mac = dst_mac_1;
	egr_l3_if = egr_l3_if_1;
	egress_gport = egress_gport_1;
	egr_if = egr_if_1;
	ecmp_member_array = ecmp_member_array_1;
    } else if(orig_i==1) {
	ecmp_mem_cnt = under_lay_2_ecmp_mem_cnt;
	egr_vid = egr_vid_2;
	egress_port = egress_port_2;
	src_mac = src_mac_2;
	dst_mac = dst_mac_2;
	egr_l3_if = egr_l3_if_2;
	egress_gport = egress_gport_2;
	egr_if = egr_if_2;
	ecmp_member_array = ecmp_member_array_2;
    }


    for(i=0; i<ecmp_mem_cnt; i++) {
        rv = create_vlan_add_port(unit, egr_vid[i], egress_port[i]);
        if (BCM_FAILURE(rv)) {
            printf("Error in configuring egr vlan %s\n", bcm_errmsg(rv));
            return rv;
    }

    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port[i], egr_vid[i]));

    /* Create L3 egress interface */
    rv = create_l3_intf(unit, src_mac[i], egr_vid[i], &egr_l3_if[i]);
    if (BCM_FAILURE(rv)) {
        printf("Error in create_l3_intf %s.\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, egress_port[i], &egress_gport[i]));

    /* Create L3 egress object */
    rv = create_egr_obj(unit, egr_l3_if[i], dst_mac[i], egress_gport[i], egr_if[i]);
    if (BCM_FAILURE(rv)) {
        printf("Error in create_egr_obj %s.\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_l3_ecmp_member_t_init(&ecmp_member_array[i]);
    ecmp_member_array[i].egress_if = egr_if[i];
    }

    bcm_l3_egress_ecmp_t_init(&ecmp_info);
    ecmp_info.max_paths = 0;
    ecmp_info.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RANDOM;

    rv = bcm_l3_ecmp_create(unit, 0, &ecmp_info, ecmp_mem_cnt, ecmp_member_array);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_l3_ecmp_create %s.\n", bcm_errmsg(rv));
        return rv;
    }

    ecmp_if_under_lay[orig_i] = ecmp_info.ecmp_intf;

    return BCM_E_NONE;
}

/*    Overlay ECMP configurations     */

int
configure_overlay_ecmp(int unit)
{
    bcm_error_t rv;
    bcm_l3_ecmp_member_t ecmp_member_array[over_lay_ecmp_mem_cnt];
    bcm_l3_egress_ecmp_t ecmp_info;
    int i;

    for (i=0; i<over_lay_ecmp_mem_cnt; i++){
        bcm_l3_ecmp_member_t_init(&ecmp_member_array[i]);
        ecmp_member_array[i].egress_if = ecmp_if_under_lay[i];
    }

    bcm_l3_egress_ecmp_t_init(&ecmp_info);
    ecmp_info.max_paths = 0;
    ecmp_info.dynamic_size = 512;
    ecmp_info.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RANDOM;
    ecmp_info.ecmp_group_flags = BCM_L3_ECMP_OVERLAY;
    rv = bcm_l3_ecmp_create(unit, 0, &ecmp_info, over_lay_ecmp_mem_cnt, ecmp_member_array);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_l3_ecmp_create %s.\n", bcm_errmsg(rv));
        return rv;
    }

    ecmp_if_over_lay = ecmp_info.ecmp_intf;

    return BCM_E_NONE;
}

/*    Hierarchical ECMP configurations    */

int
configure_hecmp(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    int i;

    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, ingress_port, bcmPortControlIP4, 1));

    /* Create vlan and add ports */
    rv = create_vlan_add_port(unit, ing_vid, ingress_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring ing vlan %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set the default VLAN for the port */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port, ing_vid));

    /* Create L3 ingress interface - l3_iif */
    rv = create_l3_iif(unit, vrf, ingress_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in create_l3_iif %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* VLAN to L3_IIF mapping */
    rv = configure_l3_iif_binding(unit, ing_vid, ingress_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_l3_iif_binding %s.\n", bcm_errmsg(rv));
        return rv;
    }

    for (i=0; i<over_lay_ecmp_mem_cnt; i++) {
	rv = configure_underlay_ecmp(unit, i);
	if (BCM_FAILURE(rv)) {
 	    printf("Error in configure_underlay_ecmp %s.\n", bcm_errmsg(rv));
	    return rv;
	}
    }

    rv = configure_overlay_ecmp(unit);
    if (BCM_FAILURE(rv)) {
	printf("Error in configure_overlay_ecmp %s.\n", bcm_errmsg(rv));
	return rv;
    }

    /* Configure MY_STATION TCAM for router MAC */
    rv = configure_my_station_tcam(unit, router_mac);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_my_station_tcam %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Program ROUTE in ALPM table */
    rv = configure_route(unit, dip, dip_mask, vrf);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_route %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Create flex counter action with overlay ECMP table as source and user set index */

int
configure_flex_counter(int unit)
{
    int options = 0;
    bcm_error_t  rv;
    bcm_flexctr_action_t action;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;

    print bcm_flexctr_action_t_init(&action);
    action.flags = 0;
    action.source = bcmFlexctrSourceEcmpOverlay;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_num = 100; /* Number of required counters */

    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectStaticIngOverlayEcmpGroup;
    index_op->mask_size[0] = 7;
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

    /* Create an ingress action */
    rv = bcm_flexctr_action_create(unit, options, &action, &stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_flexctr_action_create %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Attach counter action to ecmp_if */
    rv = bcm_l3_ecmp_stat_attach(unit, ecmp_if_over_lay, stat_counter_id);
    if (BCM_FAILURE (rv)) {
        printf ("Error in bcm_l3_route_stat_attach %s\n", bcm_errmsg (rv));
        return rv;
    }

    /* Set the counter index */
    rv = bcm_l3_ecmp_flexctr_object_set(unit, ecmp_if_over_lay, flexctr_obj_val);
    if (BCM_FAILURE (rv)) {
        printf ("Error in bcm_l3_route_flexctr_object_set %s\n", bcm_errmsg (rv));
        return rv;
    }


    return BCM_E_NONE;
}

/*
 *    Configures the IPV4 hierarchical ECMP route and the required set up to verify it
 */

int
test_setup(int unit)
{
    int num_ports = 1+under_lay_1_ecmp_mem_cnt+under_lay_2_ecmp_mem_cnt;
    int port_list[num_ports];
    bcm_error_t rv;
    bcm_pbmp_t pbmp;
    int i;

    rv = port_numbers_get(unit, port_list, num_ports);
    if (BCM_FAILURE(rv)) {
        printf("Error in port_numbers_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    ingress_port = port_list[0];

    for (i=1; i<under_lay_1_ecmp_mem_cnt+1; i++)
        egress_port_1[i-1] = port_list[i];

    for (i=under_lay_1_ecmp_mem_cnt+1; i<num_ports; i++)
        egress_port_2[i-4] = port_list[i];

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    rv = ingress_port_multi_setup(unit, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error in ingress_port_multi_setup %s\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    for (i=0; i<under_lay_1_ecmp_mem_cnt; i++)
        BCM_PBMP_PORT_ADD(pbmp, egress_port_1[i]);
    for (i=0; i<under_lay_2_ecmp_mem_cnt; i++)
        BCM_PBMP_PORT_ADD(pbmp, egress_port_2[i]);
    rv = egress_port_multi_setup(unit, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error in egress_port_multi_setup %s\n", bcm_errmsg(rv));
        return rv;
    }

    bshell(unit, "pw start report +raw +decode");

    rv = register_pktioRx_callback(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in register_pktioRx_callback %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = config_rtag7(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in config_rtag7 %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = config_packet_spray(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in config_packet_spray %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = configure_hecmp(unit);
    if (BCM_FAILURE(rv)) {
        printf("error in configure_hecmp %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = configure_flex_counter(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_flex_counter %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Flex counter verification */

int
test_verify(int unit)
{
    char str[512];
    int rv;
    bcm_flexctr_counter_value_t counter_value;
    uint32 num_entries = 1;
    uint32 counter_index = flexctr_obj_val;
    int count = 8;

    print "l3 ecmp show";
    bshell(unit, "l3 ecmp show");
    printf("\n");
    print "l3 route show";
    bshell(unit, "l3 route show");
    printf("\n");
    print "l3 intf show";
    bshell(unit, "l3 intf show");
    print "l3 egress show";
    bshell(unit, "l3 egress show");

    printf("Sending %d unicast packets to ingress port %d\n", count, ingress_port);
    snprintf(str, 512, "tx %d pbm=%d data=0x0000000000020000000000018100000208004500002A0000000040FFA3E50A0A0A2AC0A8011400010203000000000000000000000000000000000000024D7BD6", count, ingress_port);
    bshell(unit, str);
    sal_sleep(5);

    printf("\n------------------------------------- \n");
    printf("    RESULT OF PACKET VERIFICATION  ");
    printf("\n------------------------------------- \n");
    printf("Test = [%s]",(test_failed == 0)?"PASS":"FAIL");
    printf("\n------------------------------------- \n\n");

    /* test_fail is asserted by pktio_Rx_call_back ()
       sleep command is needed to give pktio_Rx_call_back chance to run. */
    if (test_failed == 1) {
       return BCM_E_FAIL;
    }

    /* Get counter value. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    rv = bcm_flexctr_stat_get(unit, stat_counter_id, num_entries, &counter_index,
                              &counter_value);
    if (BCM_FAILURE(rv)) {
      printf("Error in bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* print counter value. */
    printf("Routed : %d packets / %d bytes\n", COMPILER_64_LO(counter_value.value[0]),
                                               COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != count) {
      printf("Error in packet verification. Expected packet stat %d but got %d\n", count, COMPILER_64_LO(counter_value.value[0]));
      test_failed = 1;
    };

    printf("\n-------------------------------------------- \n");
    printf("    RESULT OF FLEX COUNTER VERIFICATION  ");
    printf("\n-------------------------------------------- \n");
    printf("Test = [%s]",(test_failed == 0)?"PASS":"FAIL");
    printf("\n-------------------------------------------- \n\n");


    if(test_failed ==0)
    {
       return BCM_E_NONE;
    }
    else
    {
       return BCM_E_FAIL;
    }
}

/*
 *    This functions does the following
 *    a)test setup [configures the feature]
 *    b)test verify [verifies the configured feature]
 */

int
execute()
{
    bcm_error_t rv;
    int unit = 0;

    rv = test_setup(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in test_setup %s \n", bcm_errmsg(rv));
        return rv;
    }
    printf("\nCompleted test setup successfully.\n\n");

    rv = test_verify(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in test_verify %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Completed test verify successfully.\n");

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
