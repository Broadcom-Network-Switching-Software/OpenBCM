/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : IFA [ SDK - 6.5.19, CANCUN - 6.3.3 and uC - 4.3.13 ]
 *
 * Usage    : BCM.0> cint td3_ifa_l3_term.c
 *
 * config   : td3_ifa_term_config.bcm
 *
 * Log file : td3_ifa_l3_term_log.txt
 *
 * Test Topology :
 *                      +-------------------+
 *                      |                   |
 *                      |                   |
 *        egress_ port  |        TD3        |  ingress_port
 *       ----<----------+                   +<----------
 *                      |                   |
 *                      |                   |
 *                      |                   |
 *                      +-------------------+
 *
 *
 * Summary:
 * ========
 * cint script to demonstrate IFA termination flow
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup):
 *   =============================================
 *     a) Select ingress port, egress port  and collector port configure them in
 *        Loopback mode.
 *     b) Do the necessary IFA termination configurations.
 *        - One of the R5 cores should be loaded with embedded app, the needed
 *          config variables are set accordingly and one of the CPU CosQ is
 *          mapped to the embedded app Rx DMA channel
 *        - IFP is configured to qualify IFA packets and add the actions of
 *          adding termination nodes' metadata, CopyToCpu, CpuCosQ mapped to
 *          embedded app's DMA channel and drop the packet [cloned packet]
 *        - EVXLT is programmed with the metadata elements
 *
 *   2) Step2 - Verification (Done in test_verify):
 *   ===============================================
 *     a) Send the packet that matches the description through ingress port
 *        and observe the packet that goes out through collector port
 *
 *     b) Expected Result:
 *     ===================
 *     The embedded app orders the IFA packet to have the metadata of all the
 *     nodes and reports the data to the collector in IPFIX format with the
 *     configured template id and number of records
 */

cint_reset();

uint8 UC0_DMA_CHANNEL = 9;
uint8 UC1_DMA_CHANNEL = 13;
uint8 UC0_COSQ = 46;
uint8 UC1_COSQ = 47;
uint8 uC = 0;

bcm_collector_t collector_id;
int export_profile_id;
bcm_mac_t coll_dst_mac = {0x00, 0x00, 0x00, 0x00, 0x44, 0x44};
bcm_mac_t coll_src_mac = {0x00, 0x00, 0x00, 0x00, 0x33, 0x33};
uint16 coll_tpid = 0x8100;
bcm_vlan_t coll_vlan = 10;
bcm_ip_t coll_ipv4_src_ip = 0x0a0a0a14;
bcm_ip_t coll_ipv4_dst_ip = 0x1414140a;
uint8 coll_dscp= 0x39;
uint8 coll_ttl = 16;
bcm_l4_port_t coll_l4_src_port = 0x1F91;
bcm_l4_port_t coll_l4_dst_port = 0x0807;
uint16 coll_mtu = 1500;

bcm_ifa_export_template_t tmpl_id;
uint16 set_id = 257;
uint32 MAX_TMPL_ELEMENTS = 3;

uint8 ifa_protocol = 0xFD;

bcm_port_t cpu_port = 0;
bcm_port_t ingress_port;
bcm_port_t egress_port;
bcm_port_t collector_port;

bcm_vlan_t vlan = 0x15;
bcm_mac_t dmac = {0x00, 0x00, 0x00, 0x00, 0x22, 0x22};

uint16 rx_packet_export_max_length = 256;

/*
 *    Checks if given port/index is already present
 *    in the list so that same port number is not generated again
 */

int
checkPortAssigned(int *port_index_list,int no_entries, int index)
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
portNumbersGet(int unit, int *port_list, int num_ports)
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
        printf("portNumbersGet() failed \n");
        return -1;
    }

    /* generating random ports */
    for (i= 0; i < num_ports; i++) {
        index = sal_rand() % port;
        if (checkPortAssigned(port_index_list, i, index) == 0) {
            port_list[i] = tempports[index];
            port_index_list[i] = index;
        } else {
            i = i - 1;
        }
    }

    return BCM_E_NONE;
}

/*
 *    Global IFA configuration
 */

int
ifa_global_setting(int unit)
{
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIFA2Enable, 1));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIFA2HeaderType, ifa_protocol));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIFA2MetaDataFirstLookupType, 0));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIFA2MetaDataSecondLookupType, 0));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIFAMetaDataAdaptTable, BCM_SWITCH_IFA_METADATA_ADAPT1_SINGLE));

    return BCM_E_NONE;
}

/*
 *    Create vlan and add ports to vlan
 */

int
create_vlan_add_port(int unit, bcm_vlan_t vlan, bcm_port_t port1, bcm_port_t port2)
{
    bcm_error_t rv;
    bcm_pbmp_t pbmp, upbmp;

    rv = bcm_vlan_create(unit, vlan);
    if ((BCM_FAILURE(rv)) & (rv != BCM_E_EXISTS)) {
        printf("Error in creating vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port1);
    BCM_PBMP_PORT_ADD(pbmp, port2);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan, pbmp, upbmp));

    return BCM_E_NONE;
}

/*
 *    Steps to configure switching
 */

int
switch_config(int unit)
{
    bcm_error_t rv;
    bcm_l2_addr_t l2_addr;

    rv = create_vlan_add_port(unit, vlan, ingress_port, egress_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring vlan %s.\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_l2_addr_t_init(&l2_addr, dmac, vlan);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port  = egress_port;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (BCM_FAILURE(rv)) {
        printf("Error in l2 addr add %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 *    Steps to configure forwarding to collector
 */

int
collector_setup(int unit)
{
    bcm_pbmp_t pbmp, upbmp;
    bcm_l2_addr_t l2_addr;
    int rv;

    /* Associate uC cosq with DMA channel */
    BCM_IF_ERROR_RETURN(bcm_rx_queue_channel_set(unit, UC0_COSQ, UC0_DMA_CHANNEL));
    BCM_IF_ERROR_RETURN(bcm_rx_queue_channel_set(unit, UC1_COSQ, UC1_DMA_CHANNEL));

    /* Create vlan to redirect export packets */
    rv = create_vlan_add_port(unit, coll_vlan, collector_port, cpu_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring vlan %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add static MAC address */
    bcm_l2_addr_t_init(&l2_addr, coll_dst_mac, coll_vlan);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = collector_port;
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));

    return BCM_E_NONE;
}

/*
 *     IFA EVXLT configuration
 */

int
ifa_evxlt(int unit)
{
    bcm_flow_handle_t flow_handle;
    bcm_flow_option_id_t option1, option2;
    bcm_flow_encap_config_t einfo;
    bcm_flow_logical_field_t lfield[3];
    bcm_gport_t egress_gport;

    /*
     * Configuring Metadata info on EVXLT1 table
     * DGPP is used to lookup EVXLT1
     */
    BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "IFA", &flow_handle));
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, egress_port, &egress_gport));

    /* Adapt_1 single Lookup 1 */
    BCM_IF_ERROR_RETURN(bcm_flow_option_id_get(unit, flow_handle, "IFA_MD_LOOKUP_1_ADAPT1_SINGLE", &option1));

    /* Insert METADATA */
    bcm_flow_encap_config_t_init(&einfo);
    einfo.flow_handle = flow_handle;
    einfo.flow_option = option1;
    einfo.flow_port = egress_gport;
    einfo.criteria = BCM_FLOW_ENCAP_CRITERIA_FLEX;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "DEVICE_ID", &lfield[0].id));
    lfield[0].value = 0xEEEEE;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "LNS", &lfield[1].id));
    lfield[1].value = 1;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "OPAQUE_DATA", &lfield[2].id));
    lfield[2].value = 0x99999;

    einfo.valid_elements = BCM_FLOW_ENCAP_FLOW_PORT_VALID | BCM_FLOW_ENCAP_FLEX_DATA_VALID | BCM_FLOW_ENCAP_FLEX_KEY_VALID;
    BCM_IF_ERROR_RETURN(bcm_flow_encap_add(unit, &einfo, 3, lfield));

    /* Configure Adapt1 single Lookup2 */
    BCM_IF_ERROR_RETURN(bcm_flow_option_id_get(unit, flow_handle, "IFA_MD_LOOKUP_2_ADAPT1_SINGLE", &option2));

    /* Insert METADATA */
    bcm_flow_encap_config_t_init(&einfo);
    einfo.flow_handle = flow_handle;
    einfo.flow_option = option2;
    einfo.flow_port = egress_gport;
    einfo.criteria = BCM_FLOW_ENCAP_CRITERIA_FLEX;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "MMU_STAT_0", &lfield[0].id));
    lfield[0].value = 0x900ddea1;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "MMU_STAT_1", &lfield[1].id));
    lfield[1].value = 0x900dc0de;

    einfo.valid_elements = BCM_FLOW_ENCAP_FLOW_PORT_VALID | BCM_FLOW_ENCAP_FLEX_DATA_VALID | BCM_FLOW_ENCAP_FLEX_KEY_VALID;
    BCM_IF_ERROR_RETURN(bcm_flow_encap_add(unit, &einfo, 2, lfield));

    return BCM_E_NONE;
}

/*
 *    IFA FP configuration
 */

int
ifa_ifp(int unit)
{
    bcm_field_qset_t qset;
    bcm_field_group_t group;
    bcm_field_entry_t entry;

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyPktType);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIfaVersion2);


    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_PktType(unit, entry, bcmFieldPktTypeUdpIfa));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_IfaVersion2(unit, entry, 1, 0xFFFFFFFF));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionAssignExtractionCtrlId, 0xEE, 0xFF));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionAssignEditCtrlId, 0xD0, 0xFF));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 2, 2));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

    if (uC == 0) {
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCosQCpuNew, UC0_COSQ, 0));
    } else {
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCosQCpuNew, UC1_COSQ, 0));
    }

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    return BCM_E_NONE;
}

/*
 *     Steps to set IFA configuration
 */

int
ifa_cfg_info_set(int unit)
{
    bcm_ifa_config_info_t cfg;

    bcm_ifa_config_info_t_init(&cfg);
    cfg.rx_packet_export_max_length = rx_packet_export_max_length;
    BCM_IF_ERROR_RETURN(bcm_ifa_config_info_set(unit, 0, &cfg));

    return BCM_E_NONE;
}

/*
 *    Steps to get IFA configuration
 */

int
ifa_cfg_info_get(int unit)
{
    bcm_ifa_config_info_t cfg;

    BCM_IF_ERROR_RETURN(bcm_ifa_config_info_get(unit, &cfg));

    return BCM_E_NONE;
}

/*
 *    Clearing IFA stats
 */

int
ifa_stats_clear(int unit)
{
    bcm_ifa_stat_info_t stats;

    sal_memset(&stats, 0, sizeof(stats));
    BCM_IF_ERROR_RETURN(bcm_ifa_stat_info_set(unit, &stats));

    return BCM_E_NONE;
}

/*
 *    Steps to get IFA stats
 */

int
ifa_stats_get(int unit)
{
    bcm_ifa_stat_info_t stats;

    BCM_IF_ERROR_RETURN(bcm_ifa_stat_info_get(unit, &stats));


    printf("Rx pkt cnt                       : %u \n", stats.rx_pkt_cnt);
    printf("Tx pkt cnt                       : %u \n", stats.tx_pkt_cnt);
    printf("Collector not present drop       : %u \n", stats.ifa_collector_not_present_drop);
    printf("Current length exceed drop       : %u \n", stats.ifa_int_hdr_len_invalid_drop);
    printf("Pkt length exceed max drop count : %u \n", stats.rx_pkt_length_exceed_max_drop_count);
    printf("Parse error drop count           : %u \n", stats.rx_pkt_parse_error_drop_count);
    printf("Unknown namespace drop count     : %u \n", stats.rx_pkt_unknown_namespace_drop_count);
    printf("Excess rate drop count           : %u \n", stats.rx_pkt_excess_rate_drop_count);
    printf("Tx record count                  : %u \n", stats.tx_record_count);
    printf("Tx pkt failure count             : %u \n", stats.tx_pkt_failure_count);
    printf("Template not present drop        : %u \n", stats.ifa_template_not_present_drop);

    return BCM_E_NONE;
}

/*
 *    Steps to configure IPv4 collector
 */

int
ifa_collector_ipv4_create(int unit)
{
    bcm_collector_info_t collector_info;
    bcm_pbmp_t pbmp, upbmp;
    int rv;

    bcm_collector_info_t_init(&collector_info);

    sal_memcpy(collector_info.eth.dst_mac, coll_dst_mac, 6);
    sal_memcpy(collector_info.eth.src_mac, coll_src_mac, 6);

    collector_info.eth.vlan_tag_structure = BCM_COLLECTOR_ETH_HDR_SINGLE_TAGGED;
    collector_info.eth.outer_vlan_tag = coll_vlan;
    collector_info.eth.outer_tpid = coll_tpid;
    collector_info.ipv4.dscp = coll_dscp;
    collector_info.ipv4.ttl = coll_ttl;
    collector_info.ipv4.src_ip = coll_ipv4_src_ip;
    collector_info.ipv4.dst_ip = coll_ipv4_dst_ip;
    collector_info.udp.src_port = coll_l4_src_port;
    collector_info.udp.dst_port = coll_l4_dst_port;
    collector_info.udp.flags |= BCM_COLLECTOR_UDP_FLAGS_CHECKSUM_ENABLE;
    collector_info.transport_type = bcmCollectorTransportTypeIpv4Udp;
    collector_info.ipfix.observation_domain_id = 0x12345678;

    rv = bcm_collector_create(unit, 0, &collector_id, &collector_info);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 *    Steps to configure IPv6 collector
 */

int
ifa_collector_ipv6_create(int unit)
{
    bcm_collector_info_t collector_info;
    bcm_pbmp_t pbmp, upbmp;
    int rv;

    bcm_collector_info_t_init(&collector_info);

    sal_memcpy(collector_info.eth.dst_mac, coll_dst_mac, 6);
    sal_memcpy(collector_info.eth.src_mac, coll_src_mac, 6);

    collector_info.eth.vlan_tag_structure = BCM_COLLECTOR_ETH_HDR_SINGLE_TAGGED;
    collector_info.eth.outer_vlan_tag = coll_vlan;
    collector_info.eth.outer_tpid = coll_tpid;
    collector_info.ipv6.traffic_class = coll_dscp;
    collector_info.ipv6.flow_label = 0xFFFFFFFF;
    collector_info.ipv6.hop_limit = coll_ttl;
    sal_memcpy(collector_info.ipv6.src_ip, coll_ipv6_src_ip, 16);
    sal_memcpy(collector_info.ipv6.dst_ip, coll_ipv6_dst_ip, 16);
    collector_info.udp.src_port = coll_l4_src_port;
    collector_info.udp.dst_port = coll_l4_dst_port;
    collector_info.udp.flags |= BCM_COLLECTOR_UDP_FLAGS_CHECKSUM_ENABLE;
    collector_info.transport_type = bcmCollectorTransportTypeIpv6Udp;
    collector_info.ipfix.observation_domain_id = 0x12345678;

    rv = bcm_collector_create(unit, 0, &collector_id, &collector_info);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 *    Steps to get collector information
 */

int
ifa_collector_get(int unit, bcm_collector_t id)
{
    bcm_collector_info_t collector_info;
    int transport_type;
    int i;
    int rv;

    bcm_collector_info_t_init(&collector_info);

    rv = bcm_collector_get(unit, id, &collector_info);
    BCM_IF_ERROR_RETURN(rv);

    transport_type = collector_info.transport_type;

    printf("Transport type      : %d \n", transport_type);
    printf("Dst Mac             : %02x:%02x:%02x:%02x:%02x:%02x \n", collector_info.eth.dst_mac[0],
								     collector_info.eth.dst_mac[1],
								     collector_info.eth.dst_mac[2],
								     collector_info.eth.dst_mac[3],
								     collector_info.eth.dst_mac[4],
								     collector_info.eth.dst_mac[5]);
    printf("Src Mac             : %02x:%02x:%02x:%02x:%02x:%02x \n", collector_info.eth.src_mac[0],
								     collector_info.eth.src_mac[1],
								     collector_info.eth.src_mac[2],
								     collector_info.eth.src_mac[3],
								     collector_info.eth.src_mac[4],
								     collector_info.eth.src_mac[5]);
    printf("Vlan tag structure  : %u \n", collector_info.eth.vlan_tag_structure);
    printf("Outer vlan tag      : %u \n", collector_info.eth.outer_vlan_tag);
    printf("Outer tpid          : 0x%04X \n", collector_info.eth.outer_tpid);
    if (collector_info.transport_type == bcmCollectorTransportTypeIpv4Udp) {
        printf("Src Ip              : 0x%08X \n", collector_info.ipv4.src_ip);
        printf("Dst Ip              : 0x%08X \n", collector_info.ipv4.dst_ip);
        printf("DSCP                : 0x%02X \n", collector_info.ipv4.dscp);
        printf("TTL                 : %u \n", collector_info.ipv4.ttl);
    } else {
        printf("Src Ip              : ");
        for (i = 0; i < 16; i++) {
            printf("0x%02x ", collector_info.ipv6.src_ip[i]);
        }
        printf("\n");
        printf("Dst Ip              : ");
        for (i = 0; i < 16; i++) {
            printf("0x%02x ", collector_info.ipv6.dst_ip[i]);
        }
        printf("\n");
        printf("Traffic Class       : 0x%02X \n", collector_info.ipv6.traffic_class);
        printf("Hop limit           : %u \n", collector_info.ipv6.hop_limit);
        printf("Flow label          : 0x%08x \n", collector_info.ipv6.flow_label);
    }
    printf("Src Port            : 0x%04X \n", collector_info.udp.src_port);
    printf("Dst Port            : 0x%04X \n", collector_info.udp.dst_port);
    printf("UDP checksum        : %s \n",
	   collector_info.udp.flags & BCM_COLLECTOR_UDP_FLAGS_CHECKSUM_ENABLE ? "Yes" : "No");
    printf("Observation domain Id : 0x%x\n", collector_info.ipfix.observation_domain_id);
    printf("System Id           : ");
    for (i = 0; i < collector_info.protobuf.system_id_length; i++) {
	printf("%c", collector_info.protobuf.system_id[i]);
    }
    printf("\n");
    printf("Component Id        : 0x%x\n", collector_info.protobuf.component_id);

    return BCM_E_NONE;
}

/*
 *     Steps to delete collector
 */

int
collector_delete(int unit, bcm_collector_t id)
{
    BCM_IF_ERROR_RETURN(bcm_collector_destroy(unit, id));

    return BCM_E_NONE;
}

/*
 *    Steps to configure export profile
 */

int
export_profile_create(int unit)
{
    bcm_collector_export_profile_t profile_info;
    int rv;

    bcm_collector_export_profile_t_init(&profile_info);

    profile_info.wire_format = bcmCollectorWireFormatIpfix;
    profile_info.max_packet_length = 9000;
    profile_info.num_records = 10;
    profile_info.flags |= BCM_COLLECTOR_EXPORT_PROFILE_FLAGS_USE_NUM_RECORDS;

    export_profile_id = 8;
    rv = bcm_collector_export_profile_create(unit, BCM_COLLECTOR_EXPORT_PROFILE_WITH_ID, &export_profile_id, &profile_info);
    if (BCM_FAILURE(rv)) {
        printf("Error in collector export profile create %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 *    Steps to get export profile information
 */

int
export_profile_get(int unit, int id)
{
    bcm_collector_export_profile_t profile_info;
    int wire_format;
    int rv;

    bcm_collector_export_profile_t_init(&profile_info);

    rv = bcm_collector_export_profile_get(unit, id, &profile_info);
    if (BCM_FAILURE(rv)) {
        printf("Error in collector export profile get %s\n", bcm_errmsg(rv));
        return rv;
    }

    wire_format = profile_info.wire_format;
    printf("Wire Format     : %d \n", wire_format);
    printf("Export Interval : %u usecs \n", profile_info.export_interval);
    printf("MTU             : %u \n", profile_info.max_packet_length);
    printf("flags           : 0x%x \n", profile_info.flags);
    printf("Num records     : %u \n", profile_info.num_records);

    return rv;
}

/*
 *    Steps to delete export profile
 */

int
export_profile_delete(int unit, int id)
{
    BCM_IF_ERROR_RETURN(bcm_collector_export_profile_destroy(unit, id));

    return BCM_E_NONE;
};

/*
 *    Steps to attach collector to export profile and temlate id
 */

int
ifa_collector_attach(int unit)
{
    printf("Attaching collector with ID - %u, exp profile ID %u, tmpl id %u\n", collector_id, export_profile_id, tmpl_id);
    BCM_IF_ERROR_RETURN(bcm_ifa_collector_attach(unit, collector_id, export_profile_id, tmpl_id));

    return BCM_E_NONE;
}

/*
 *     Steps to get the information of attached export profile and template id of
 *     collector
 */

int
ifa_collector_attach_get(int unit)
{
    bcm_collector_t coll_id;
    int profile_id;
    bcm_ifa_export_template_t template_id;

    BCM_IF_ERROR_RETURN(bcm_ifa_collector_attach_get(unit, &coll_id, &profile_id, &template_id));
    printf("Collector Id      : %d \n", coll_id);
    printf("Export profile Id : %d \n", profile_id);
    printf("Template Id       : %d \n", template_id);

    return BCM_E_NONE;
}

/*
 *    Steps to detach collector to export profile and temlate id
 */

int
ifa_collector_detach(int unit)
{
    BCM_IF_ERROR_RETURN(bcm_ifa_collector_detach(unit, collector_id, export_profile_id, tmpl_id));

    return BCM_E_NONE;
}

/*
 *      Create IFA template
 */

int
ifa_template_create(int unit)
{
    bcm_ifa_export_element_info_t elements[3] = {
        {1, bcmIfaExportElementTypeIFAHeaders, 8, 100},  /* IFA headers */
        {1, bcmIfaExportElementTypeMetadataStack, 0xFFFF, 101},  /* IFA MD stack */
        {1, bcmIfaExportElementTypePktData, 0xFFFF, 102},  /* Pkt Hrds and Data */
    };
    BCM_IF_ERROR_RETURN(bcm_ifa_export_template_create(unit, 0, &tmpl_id, set_id, 3, &elements));

    return BCM_E_NONE;
}

/*
 *      Steps to get IFA template information
 */

int
ifa_template_get(int unit, int id)
{
    bcm_ifa_export_element_info_t elements[MAX_TMPL_ELEMENTS];
    int list_size;
    int i;
    int element;
    uint16 set_id;

    BCM_IF_ERROR_RETURN(bcm_ifa_export_template_get(unit, id, &set_id, MAX_TMPL_ELEMENTS, elements, &list_size));

    printf("Set Id        : %u \n", set_id);
    printf("List size     : %d \n", list_size);

    for (i = 0; i < list_size; i++) {
        printf("\n");
        element = elements[i].element;

        printf("Element      : %d \n", element);
        printf("Flags        : %u \n", elements[i].flags);
        printf("Data size    : %u \n", elements[i].data_size);
        printf("Info elem id : %u \n", elements[i].info_elem_id);
    }
    return BCM_E_NONE;
}

/*
 *      Steps to delete IFA template
 */

int
ifa_template_delete(int unit)
{
    BCM_IF_ERROR_RETURN(bcm_ifa_export_template_destroy(unit, tmpl_id));

    return BCM_E_NONE;
};

/*
 *      Steps to stop transmission of IFA template
 */
int
ifa_template_xmit_stop(int unit)
{
    bcm_ifa_template_transmit_config_t cfg;

    bcm_ifa_template_transmit_config_t_init(&cfg);

    cfg.retransmit_interval_secs = 0;
    cfg.initial_burst = 0;
    BCM_IF_ERROR_RETURN(bcm_ifa_template_transmit_config_set(unit, tmpl_id, collector_id, &cfg));

    return BCM_E_NONE;
}

/*
 *      Steps to transmit IFA template
 */
int
ifa_template_xmit_start(int unit)
{
    bcm_ifa_template_transmit_config_t cfg;

    bcm_ifa_template_transmit_config_t_init(&cfg);

    cfg.retransmit_interval_secs = 30;
    cfg.initial_burst = 5;
    BCM_IF_ERROR_RETURN(bcm_ifa_template_transmit_config_set(unit, tmpl_id, collector_id, &cfg));

    return BCM_E_NONE;
}

/*
 *      Steps to get transmit template configurations
 */
int
ifa_template_xmit_get(int unit)
{
    bcm_ifa_template_transmit_config_t cfg;

    bcm_ifa_template_transmit_config_t_init(&cfg);

    BCM_IF_ERROR_RETURN(bcm_ifa_template_transmit_config_get(unit, tmpl_id, collector_id, &cfg));

    printf("Interval  : %d \n", cfg.retransmit_interval_secs);
    printf("Burst     : %d \n", cfg.initial_burst);

    return BCM_E_NONE;
}

/*
 *    Timing configuration for timestamps
 */

int
time_config(int unit)
{
    bcm_time_init(unit);
    bcm_time_interface_t intf;
    bcm_error_t rv;

    bcm_time_interface_t_init(&intf);
    intf.id = 0;
    intf.flags |= BCM_TIME_ENABLE;

    rv = bcm_time_interface_add(unit, intf);
    if (BCM_FAILURE(rv)) {
        printf("Error in time interface add %s\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_time_ts_counter_t counter;
    bcm_time_tod_t tod;
    uint64 secs, nano_secs, div;
    uint32 stages;

    bcm_time_ts_counter_get(unit, &counter);
    stages = BCM_TIME_STAGE_EGRESS;
    tod.time_format = counter.time_format;
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

    rv = bcm_time_tod_set(unit, stages, &tod);
    if (BCM_FAILURE(rv)) {
        printf("Error in tod set %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 *    Configures the feature and the required set up to verify it
 */

int test_setup(int unit)
{
    int port_list[3];
    int lb_mode = BCM_PORT_LOOPBACK_MAC;
    bcm_error_t rv;

    rv = portNumbersGet(unit, port_list, 3);
    if (BCM_FAILURE(rv)) {
        printf("Error in portNumbersGet %s\n", bcm_errmsg(rv));
        return rv;
    }

    ingress_port = port_list[0];
    egress_port = port_list[1];
    collector_port = port_list[2];
    printf("Choosing port %d as ingress port %d as egress port and %d as collector port\n", ingress_port, egress_port, collector_port);

    /* Set port mac loopback mode */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, lb_mode));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port, lb_mode));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, collector_port, lb_mode));

    rv = ifa_global_setting(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in IFA global settings %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = switch_config(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in switching configuration %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = collector_setup(unit);
    if (BCM_FAILURE(rv)) {
	    printf("Error in collector_setup %s\n", bcm_errmsg(rv));
	    return rv;
    }

    rv = ifa_ifp(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in IFA FP configuration %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = ifa_evxlt(unit);
    if (BCM_FAILURE(rv)) {
	    printf("Error in IFA EVXLT configuration %s\n", bcm_errmsg(rv));
	    return rv;
    }

    rv = ifa_cfg_info_set(unit);
    if (BCM_FAILURE(rv)) {
	    printf("Error in IFA configuration info %s\n", bcm_errmsg(rv));
	    return rv;
    }

    rv = ifa_template_create(unit);
    if (BCM_FAILURE(rv)) {
	    printf("Error in template create %s\n", bcm_errmsg(rv));
	    return rv;
    }

    rv = ifa_collector_ipv4_create(unit);
    if (BCM_FAILURE(rv)) {
	    printf("Error in collector create %s\n", bcm_errmsg(rv));
	    return rv;
    }

    rv = export_profile_create(unit);
    if (BCM_FAILURE(rv)) {
	    printf("Error in export profile create %s\n", bcm_errmsg(rv));
	    return rv;
    }

    rv = ifa_collector_attach(unit);
    if (BCM_FAILURE(rv)) {
	    printf("Error in collector attach %s\n", bcm_errmsg(rv));
	    return rv;
    }

    rv = ifa_template_xmit_start(unit);
    if (BCM_FAILURE(rv)) {
	    printf("Error in template transmit start %s\n", bcm_errmsg(rv));
	    return rv;
    }

    rv = time_config(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in time config %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 *    Transmit the below packet on ingress_port and verify the functionality of
 *    the feature
 */

int test_verify(int unit)
{
    char str[512];

    snprintf(str, 512, "port %d discard=all", egress_port);
    bshell(unit, str);
    snprintf(str, 512, "port %d discard=all", collector_port);
    bshell(unit, str);
    snprintf(str, 512, "dmirror %d mode=ingress destport=cpu0", ingress_port);
    bshell(unit, str);
    snprintf(str, 512, "dmirror %d mode=ingress destport=cpu0", egress_port);
    bshell(unit, str);
    snprintf(str, 512, "dmirror %d mode=ingress destport=cpu0", collector_port);
    bshell(unit, str);
    bshell(unit,"pw start");
    bshell(unit,"pw report +raw");

    /* Send a packet to ingress port*/
    printf("Packet comes into ingress port %d\n", ingress_port);

    snprintf(str, 512, "tx 1 pbm=%d data=000000002222000000001111810000150800450000b2000000000afd973b0b0b01000a0a02002f1100ff1f001f690056d8cd33223e101ccccc0a00000000005f005b007fc20a3b9accba000bbbbb900d5eed900dbead1ddddd0a00000000005f005b009795fc3b9acc9c000aaaaa900ddeed900dfeed000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d; sleep 5", ingress_port);

    printf("%s\n", str);
    bshell(unit, str);

    return BCM_E_NONE;
}

/*
 *    This functions does the following
 *    a)test setup [configures the feature]
 *    b)test verify [verifies the configured feature]
 */

int test_execute()
{
    bcm_error_t rv;
    int unit = 0;

    bshell(unit, "config show; a ; version");

    rv = test_setup(unit);
    if (BCM_FAILURE(rv)) {
        printf("testSetup() failed, %s.\n", bcm_errmsg(rv));
        return -1;
    }
    printf("Completed test setup successfully.\n");

    rv = test_verify(unit);
    if (BCM_FAILURE(rv)) {
        printf("testVerify() failed\n");
        return -1;
    }
    printf("Completed test verify successfully.\n");

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print test_execute();
}
