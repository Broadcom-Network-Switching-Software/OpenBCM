/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


cint_reset();

int unit = 0;
bcm_port_t ingress_port;
bcm_port_t egress_port;

const int FTE_MAX_INFO_ELEMENTS = 40;

/* Collector 1 config */
bcm_collector_info_t collector_info1;
bcm_port_t fte_coll_port1          = 0;
bcm_vlan_t fte_coll_vlan1          = 0x600;
bcm_mac_t fte_coll_dst_mac1        = {0x00, 0x00, 0x00, 0x00, 0x00, 0xc0};
bcm_mac_t fte_coll_src_mac1        = {0x00, 0x00, 0x00, 0x00, 0x00, 0xf0};

bcm_collector_info_t_init(&collector_info1);
sal_memcpy(collector_info1.eth.dst_mac, fte_coll_dst_mac1, 6);
sal_memcpy(collector_info1.eth.src_mac, fte_coll_src_mac1, 6);
collector_info1.eth.vlan_tag_structure =
BCM_FLOWTRACKER_COLLECTOR_ETH_HDR_SINGLE_TAGGED;
collector_info1.eth.outer_vlan_tag = fte_coll_vlan1;;
collector_info1.eth.outer_tpid     = 0x8100;
collector_info1.ipv4.src_ip        = 0x0a82562a;
collector_info1.ipv4.dst_ip        = 0x0a82b00a;
collector_info1.ipv4.dscp          = 0xE1;
collector_info1.ipv4.ttl           = 9;
collector_info1.udp.src_port       = 0x1F91;
collector_info1.udp.dst_port       = 0x0807;
collector_info1.ipfix.version = 0xA;
collector_info1.ipfix.initial_sequence_num = 0;
collector_info1.transport_type     = bcmCollectorTransportTypeIpv4Udp;

/* Collector 2 config */
bcm_collector_info_t collector_info2;
int export_profile_id2             = -1;
bcm_port_t fte_coll_port2          = 0;
bcm_vlan_t fte_coll_vlan2          = 0x601;
bcm_mac_t fte_coll_dst_mac2        = {0x00, 0x00, 0x00, 0x00, 0x00, 0xd0};
bcm_mac_t fte_coll_src_mac2        = {0x00, 0x00, 0x00, 0x00, 0x00, 0xa0};
bcm_collector_info_t_init(&collector_info2);
sal_memcpy(collector_info2.eth.dst_mac, fte_coll_dst_mac2, 6);
sal_memcpy(collector_info2.eth.src_mac, fte_coll_src_mac2, 6);
collector_info2.eth.vlan_tag_structure =
BCM_FLOWTRACKER_COLLECTOR_ETH_HDR_SINGLE_TAGGED;
collector_info2.eth.outer_vlan_tag = fte_coll_vlan2;
collector_info2.eth.outer_tpid     = 0x8100;
collector_info2.ipv4.src_ip        = 0x0a82562b;
collector_info2.ipv4.dst_ip        = 0x0a82b00b;
collector_info2.ipv4.dscp          = 0xE1;
collector_info2.ipv4.ttl           = 15;
collector_info2.udp.src_port       = 0x1F91;
collector_info2.udp.dst_port       = 0x0860;
collector_info2.ipfix.version = 0xA;
collector_info2.ipfix.initial_sequence_num = 0x2;
collector_info2.transport_type     = bcmCollectorTransportTypeIpv4Udp;

/* Collector 3 config */
bcm_collector_info_t collector_info3;
int export_profile_id3             = -1;
bcm_port_t fte_coll_port3          = 0;
bcm_vlan_t fte_coll_vlan3          = 0x701;
bcm_mac_t fte_coll_dst_mac3        = {0x00, 0x00, 0x00, 0x10, 0x00, 0xd0};
bcm_mac_t fte_coll_src_mac3        = {0x00, 0x00, 0x00, 0x10, 0x00, 0xa0};
bcm_collector_info_t_init(&collector_info3);
sal_memcpy(collector_info3.eth.dst_mac, fte_coll_dst_mac3, 6);
sal_memcpy(collector_info3.eth.src_mac, fte_coll_src_mac3, 6);
collector_info3.eth.vlan_tag_structure =
                        BCM_FLOWTRACKER_COLLECTOR_ETH_HDR_SINGLE_TAGGED;
collector_info3.eth.outer_vlan_tag = fte_coll_vlan3;
collector_info3.eth.outer_tpid     = 0x8100;
collector_info3.ipv4.src_ip        = 0x0a82562c;
collector_info3.ipv4.dst_ip        = 0x0a82b00c;
collector_info3.ipv4.dscp          = 0xE2;
collector_info3.ipv4.ttl           = 20;
collector_info3.udp.src_port       = 0x1F92;
collector_info3.udp.dst_port       = 0x0861;
collector_info3.ipfix.version = 0xA;
collector_info3.ipfix.initial_sequence_num = 0x3;
collector_info3.transport_type     = bcmCollectorTransportTypeIpv4Udp;

/* Collector 4 config */
bcm_collector_info_t collector_info4;
int export_profile_id4             = -1;
bcm_port_t fte_coll_port4          = 0;
bcm_vlan_t fte_coll_vlan4          = 0x901;
bcm_mac_t fte_coll_dst_mac4        = {0x00, 0x00, 0x00, 0x10, 0x20, 0xd0};
bcm_mac_t fte_coll_src_mac4        = {0x00, 0x00, 0x00, 0x10, 0x20, 0xa0};
bcm_collector_info_t_init(&collector_info4);
sal_memcpy(collector_info4.eth.dst_mac, fte_coll_dst_mac4, 6);
sal_memcpy(collector_info4.eth.src_mac, fte_coll_src_mac4, 6);
collector_info4.eth.vlan_tag_structure =
                BCM_FLOWTRACKER_COLLECTOR_ETH_HDR_SINGLE_TAGGED;
collector_info4.eth.outer_vlan_tag = fte_coll_vlan4;
collector_info4.eth.outer_tpid     = 0x8100;
collector_info4.ipv4.src_ip        = 0x0a82262b;
collector_info4.ipv4.dst_ip        = 0x0a82200b;
collector_info4.ipv4.dscp          = 0xE6;
collector_info4.ipv4.ttl           = 26;
collector_info4.udp.src_port       = 0x1F80;
collector_info4.udp.dst_port       = 0x0850;
collector_info4.ipfix.version = 0xA;
collector_info4.ipfix.initial_sequence_num = 0x4;
collector_info4.transport_type     = bcmCollectorTransportTypeIpv4Udp;


const uint8 ipv4_tcp_sync_pkt1[] = {
0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x81, 0x00, 0x00, 0x0a,
0x08, 0x00, 0x45, 0x08, 0x00, 0x58, 0x00, 0x00, 0x40, 0x00, 0x40, 0x06,
0x49, 0x98, 0x0A, 0x0A, 0x02, 0x02, 0x0A, 0x0A, 0x02, 0x03, 0x33, 0x44, 0x55, 0x66, 0x00, 0xA1,
0xA2, 0xA3, 0x00, 0xB1, 0xB2, 0xB3, 0x50, 0x12, 0x11, 0xF5, 0xF8, 0xC3, 0x00, 0x00, 0xF5, 0xF5,
0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5,
0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5,
0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5
};

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

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
    return BCM_E_NONE;
}

/*
 * Configures egress port in loopback mode and discard all traffic
 */
bcm_error_t egress_port_setup(int unit, bcm_port_t port)
{

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));

    return BCM_E_NONE;
}


/*   Select one ingress port and configure them in
 *   Loopback mode. Start flow tracker remote monitoring application.
 *   Ingress port setup is done in ingress_port_setup().
 *   Egress port setup is done in egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
    int port_list[2];
    int rv;
    bcm_vlan_t vlan=10;
    bcm_pbmp_t pbmp, upbmp;
    
    if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) { 
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port = port_list[0];
    egress_port = port_list[1];
 
    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port)) {
        printf("egress_port_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    /* Create vlan to add test ports */
    rv = bcm_vlan_create(unit, vlan);
    if (BCM_FAILURE(rv)) {
        printf("Failed bcm_vlan_create => rv %d(%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    BCM_PBMP_PORT_ADD(pbmp, egress_port);
    rv = bcm_vlan_port_add(unit, vlan, pbmp, upbmp);
    if (BCM_FAILURE(rv)) {
        printf("Failed bcm_vlan_port_add => rv %d(%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }

    /* "ftrmon" is flow tracker remote monitoring tool like a packet watcher
    * tool for processing remote collector IPFIX packets (received at CPU dest port)
    * can be accessed using the ?ftrmon? diag command.
    */
    bshell(unit, "ftrmon start");
    bshell(unit, "ftrmon report +decode");
    bshell(unit, "ftrmon report +count");
    bshell(unit, "counter interval=1000");
    return BCM_E_NONE;
}


int tracking_to_export_param_xlate(int unit,
        bcm_flowtracker_tracking_param_type_t tparam,
        bcm_flowtracker_export_element_type_t *exp_ele)
{
    int found = TRUE;
    switch(tparam) {
        /* 0-4 */
        case bcmFlowtrackerTrackingParamTypeSrcIPv4:
            *exp_ele = bcmFlowtrackerExportElementTypeSrcIPv4;
            break;
        case bcmFlowtrackerTrackingParamTypeDstIPv4:
            *exp_ele = bcmFlowtrackerExportElementTypeDstIPv4;
            break;
        case bcmFlowtrackerTrackingParamTypeSrcIPv6:
            *exp_ele = bcmFlowtrackerExportElementTypeSrcIPv6;
            break;
        case bcmFlowtrackerTrackingParamTypeDstIPv6:
            *exp_ele = bcmFlowtrackerExportElementTypeDstIPv6;
            break;
            /* 5-9 */
        case bcmFlowtrackerTrackingParamTypeL4SrcPort:
            *exp_ele = bcmFlowtrackerExportElementTypeL4SrcPort;
            break;
        case bcmFlowtrackerTrackingParamTypeL4DstPort:
            *exp_ele = bcmFlowtrackerExportElementTypeL4DstPort;
            break;
        case bcmFlowtrackerTrackingParamTypeIPProtocol:
            *exp_ele = bcmFlowtrackerExportElementTypeIPProtocol;
            break;
        case bcmFlowtrackerTrackingParamTypePktCount:
            *exp_ele = bcmFlowtrackerExportElementTypePktCount;
            break;
        case bcmFlowtrackerTrackingParamTypeByteCount:
            *exp_ele = bcmFlowtrackerExportElementTypeByteCount;
            break;
            /* 10-14 */
        case bcmFlowtrackerTrackingParamTypeVRF:
            *exp_ele = bcmFlowtrackerExportElementTypeVRF;
            break;
        case bcmFlowtrackerTrackingParamTypeTTL:
            *exp_ele = bcmFlowtrackerExportElementTypeTTL;
            break;
        case bcmFlowtrackerTrackingParamTypeIPLength:
            *exp_ele = bcmFlowtrackerExportElementTypeIPLength;
            break;
        case bcmFlowtrackerTrackingParamTypeTcpWindowSize:
            *exp_ele = bcmFlowtrackerExportElementTypeTcpWindowSize;
            break;
        case bcmFlowtrackerTrackingParamTypeDosAttack:
            *exp_ele = bcmFlowtrackerExportElementTypeDosAttack;
            break;
            /* 15-19 */
        case bcmFlowtrackerTrackingParamTypeVxlanNetworkId:
            *exp_ele = bcmFlowtrackerExportElementTypeVxlanNetworkId;
            break;
        case bcmFlowtrackerTrackingParamTypeNextHeader:
            *exp_ele = bcmFlowtrackerExportElementTypeNextHeader;
            break;
        case bcmFlowtrackerTrackingParamTypeHopLimit:
            *exp_ele = bcmFlowtrackerExportElementTypeHopLimit;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerSrcIPv4:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerSrcIPv4;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerDstIPv4:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerDstIPv4;
            break;
            /* 20-24 */
        case bcmFlowtrackerTrackingParamTypeInnerSrcIPv6:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerSrcIPv6;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerDstIPv6:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerDstIPv6;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerIPProtocol:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerIPProtocol;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerTTL:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerTTL;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerNextHeader:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerNextHeader;
            break;
            /* 25-29 */
        case bcmFlowtrackerTrackingParamTypeInnerHopLimit:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerHopLimit;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerL4SrcPort:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerL4SrcPort;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerL4DstPort:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerL4DstPort;
            break;
        case bcmFlowtrackerTrackingParamTypeTcpFlags:
            *exp_ele = bcmFlowtrackerExportElementTypeTcpFlags;
            break;
        case bcmFlowtrackerTrackingParamTypeOuterVlanTag:
            *exp_ele = bcmFlowtrackerExportElementTypeOuterVlanTag;
            break;
            /* 30-34 */
        case bcmFlowtrackerTrackingParamTypeIP6Length:
            *exp_ele = bcmFlowtrackerExportElementTypeIP6Length;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerIPLength:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerIPLength;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerIP6Length:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerIP6Length;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampNewLearn:
            *exp_ele = bcmFlowtrackerExportElementTypeTimestampNewLearn;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampFlowStart:
            *exp_ele = bcmFlowtrackerExportElementTypeTimestampFlowStart;
            break;
            /* 35-39 */
        case bcmFlowtrackerTrackingParamTypeTimestampFlowEnd:
            *exp_ele = bcmFlowtrackerExportElementTypeTimestampFlowEnd;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1:
            *exp_ele = bcmFlowtrackerExportElementTypeTimestampCheckEvent1;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2:
            *exp_ele = bcmFlowtrackerExportElementTypeTimestampCheckEvent2;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerDosAttack:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerDosAttack;
            break;
        case bcmFlowtrackerTrackingParamTypeTunnelClass:
            *exp_ele = bcmFlowtrackerExportElementTypeTunnelClass;
            break;
            /* 40-44 */
        case bcmFlowtrackerTrackingParamTypeFlowtrackerCheck:
            *exp_ele = bcmFlowtrackerExportElementTypeFlowtrackerCheck;
            break;
        case bcmFlowtrackerTrackingParamTypeCustom:
            *exp_ele = bcmFlowtrackerExportElementTypeCustom;
            break;
        case bcmFlowtrackerTrackingParamTypeIngPort:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPort;
            break;
        case bcmFlowtrackerTrackingParamTypeChipDelay:
            *exp_ele = bcmFlowtrackerExportElementTypeChipDelay;
            break;
        case bcmFlowtrackerTrackingParamTypeE2EDelay:
            *exp_ele = bcmFlowtrackerExportElementTypeE2EDelay;
            break;
            /* 45-49 */
        case bcmFlowtrackerTrackingParamTypeIPATDelay:
            *exp_ele = bcmFlowtrackerExportElementTypeIPATDelay;
            break;
        case bcmFlowtrackerTrackingParamTypeIPDTDelay:
            *exp_ele = bcmFlowtrackerExportElementTypeIPDTDelay;
            break;
        case bcmFlowtrackerTrackingParamTypeIngPortPGMinCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPortPGMinCongestionLevel;
            break;
        case bcmFlowtrackerTrackingParamTypeIngPortPGSharedCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPortPGSharedCongestionLevel;
            break;
        case bcmFlowtrackerTrackingParamTypeIngPortPoolMinCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPortPoolMinCongestionLevel;
            break;
            /* 50-54 */
        case bcmFlowtrackerTrackingParamTypeIngPortPoolSharedCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPortPoolSharedCongestionLevel;
            break;
        case bcmFlowtrackerTrackingParamTypeIngPoolSharedCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPoolSharedCongestionLevel;
            break;
        case bcmFlowtrackerTrackingParamTypeIngPortPGHeadroomCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPortPGHeadroomCongestionLevel;
            break;
        case bcmFlowtrackerTrackingParamTypeIngPortPoolHeadroomCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPortPoolHeadroomCongestionLevel;
            break;
        case bcmFlowtrackerTrackingParamTypeIngGlobalHeadroomCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeIngGlobalHeadroomCongestionLevel;
            break;
            /* 55-59 */
        case bcmFlowtrackerTrackingParamTypeEgrPoolSharedCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrPoolSharedCongestionLevel;
            break;
        case bcmFlowtrackerTrackingParamTypeEgrQueueMinCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrQueueMinCongestionLevel;
            break;
        case bcmFlowtrackerTrackingParamTypeEgrQueueSharedCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrQueueSharedCongestionLevel;
            break;
        case bcmFlowtrackerTrackingParamTypeEgrQueueGroupMinCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrQueueGroupMinCongestionLevel;
            break;
        case bcmFlowtrackerTrackingParamTypeEgrQueueGroupSharedCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrQueueGroupSharedCongestionLevel;
            break;
            /* 60-64 */
        case bcmFlowtrackerTrackingParamTypeEgrPortPoolSharedCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrPortPoolSharedCongestionLevel;
            break;
        case bcmFlowtrackerTrackingParamTypeEgrRQEPoolSharedCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrRQEPoolSharedCongestionLevel;
            break;
        case bcmFlowtrackerTrackingParamTypeCFAPCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeCFAPCongestionLevel;
            break;
        case bcmFlowtrackerTrackingParamTypeMMUCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeMMUCongestionLevel;
            break;
        case bcmFlowtrackerTrackingParamTypeMMUQueueId:
            *exp_ele = bcmFlowtrackerExportElementTypeMMUQueueId;
            break;
            /* 65-69 */
        case bcmFlowtrackerTrackingParamTypeECMPGroupIdLevel1:
            *exp_ele = bcmFlowtrackerExportElementTypeECMPGroupIdLevel1;
            break;
        case bcmFlowtrackerTrackingParamTypeECMPGroupIdLevel2:
            *exp_ele = bcmFlowtrackerExportElementTypeECMPGroupIdLevel2;
            break;
        case bcmFlowtrackerTrackingParamTypeTrunkMemberId:
            *exp_ele = bcmFlowtrackerExportElementTypeTrunkMemberId;
            break;
        case bcmFlowtrackerTrackingParamTypeTrunkGroupId:
            *exp_ele = bcmFlowtrackerExportElementTypeTrunkGroupId;
            break;
        case bcmFlowtrackerTrackingParamTypeIngDropReasonGroupIdVector:
            *exp_ele = bcmFlowtrackerExportElementTypeIngDropReasonGroupIdVector;
            break;
            /* 70-74 */
        case bcmFlowtrackerTrackingParamTypeNextHopB:
            *exp_ele = bcmFlowtrackerExportElementTypeNextHopB;
            break;
        case bcmFlowtrackerTrackingParamTypeNextHopA:
            *exp_ele = bcmFlowtrackerExportElementTypeNextHopA;
            break;
        case bcmFlowtrackerTrackingParamTypeIPPayloadLength:
            *exp_ele = bcmFlowtrackerExportElementTypeIPPayloadLength;
            break;
        case bcmFlowtrackerTrackingParamTypeQosAttr:
            *exp_ele = bcmFlowtrackerExportElementTypeQosAttr;
            break;
        case bcmFlowtrackerTrackingParamTypeEgrPort:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrPort;
            break;
            /* 75-79 */
        case bcmFlowtrackerTrackingParamTypeMMUCos:
            *exp_ele = bcmFlowtrackerExportElementTypeMMUCos;
            break;
        case bcmFlowtrackerTrackingParamTypeEgrDropReasonGroupIdVector:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrDropReasonGroupIdVector;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent3:
            *exp_ele = bcmFlowtrackerExportElementTypeTimestampCheckEvent3;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent4:
            *exp_ele = bcmFlowtrackerExportElementTypeTimestampCheckEvent4;
            break;
        case bcmFlowtrackerTrackingParamTypeIngressDelay:
            *exp_ele = bcmFlowtrackerExportElementTypeIngressDelay;
            break;
            /* 80-85 */
        case bcmFlowtrackerTrackingParamTypeEgressDelay:
            *exp_ele = bcmFlowtrackerExportElementTypeEgressDelay;
            break;
        case bcmFlowtrackerTrackingParamTypeMMUQueueGroupId:
            *exp_ele = bcmFlowtrackerExportElementTypeMMUQueueGroupId;
            break;
        case  bcmFlowtrackerTrackingParamTypeMMUPacketTos:
            *exp_ele = bcmFlowtrackerExportElementTypeMMUPacketTos;
            break;
        case bcmFlowtrackerTrackingParamTypeEgrPacketTos:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrPacketTos;
            break;
        case bcmFlowtrackerTrackingParamTypeIngPacketTos:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPacketTos;
            break;
        case bcmFlowtrackerTrackingParamTypeL2ForwardingField:
            *exp_ele = bcmFlowtrackerExportElementTypeL2ForwardingField;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerTcpFlags:
            *exp_ele = bcmFlowtrackerExportElementmTypeInnerTcpFlags;
            break;
        case bcmFlowtrackerTrackingParamTypeGbpSrcId:
            *exp_ele =  bcmFlowtrackerExportElementTypeGbpSrcId;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampIngress:
            *exp_ele = bcmFlowtrackerExportElementTypeTimestampIngress;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampEgress:
            *exp_ele = bcmFlowtrackerExportElementTypeTimestampEgress;
            break;

        default:
            found = FALSE;
            break;
    }

    if (found == FALSE)  {
        printf("Not Found\n");
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/* Configure Port & Vlan For Collector */
int vlan_port_setup(bcm_port_t port, bcm_vlan_t vlan, bcm_mac_t dst_mac)
{
    int rv = BCM_E_NONE;
    bcm_pbmp_t pbmp, upbmp;
    bcm_l2_addr_t l2_addr;

    print vlan;
    print port;

    /* Create vlan to redirect export packets */
    rv = bcm_vlan_create(unit, vlan);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] bcm_vlan_create => rv %d(%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    rv = bcm_vlan_port_add(unit, vlan, pbmp, upbmp);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] bcm_vlan_port_add => rv %d(%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }

    /* Add static MAC address */
    bcm_l2_addr_t_init(&l2_addr, dst_mac, vlan);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port  = port;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] bcm_l2_addr_add => rv %d(%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }

    /* Enable L2StaticMoveToCPU so that the packet goes to CPU on LB. */
    rv = bcm_switch_control_set(unit, bcmSwitchL2StaticMoveToCpu, TRUE);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] bcm_switch_control_set => rv %d(%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


int add_check_info_p1(bcm_flowtracker_check_info_t *check_info,
        uint32 flags,
        bcm_flowtracker_tracking_param_type_t param,
        bcm_flowtracker_check_t primary_check_id)
{
    check_info->param = param;
    check_info->flags = flags;
    check_info->primary_check_id = primary_check_id;

    return 0;
}

int add_check_info_p2(bcm_flowtracker_check_info_t *check_info,
        bcm_flowtracker_check_operation_t operation,
        uint32 min_value, uint32 max_value,
        uint32 mask_value, uint8 shift_value)
{
    check_info->operation = operation;
    check_info->min_value = min_value;
    check_info->max_value = max_value;
    check_info->mask_value = mask_value;
    check_info->shift_value = shift_value;

    return 0;
}

int add_check_info_p3(bcm_flowtracker_check_info_t *check_info,
        uint32 state_transition_flags)
{
    check_info->state_transition_flags = state_transition_flags;

    return 0;
}

int add_check_info_p4(bcm_flowtracker_check_info_t *check_info,
        int num_dr_gid_count, uint32 *dr_gid_list)
{
    check_info->num_drop_reason_group_id = num_dr_gid_count;

    for (i = 0; i < num_dr_gid_count; i++) {
        check_info->drop_reason_group_id[i] = dr_gid_list[i];
    }

    return 0;
}

int add_check_action_info(bcm_flowtracker_check_t check_id,
        uint32 flags, bcm_flowtracker_tracking_param_type_t param,
        bcm_flowtracker_check_action_t action,
        uint32 mask_value, uint32 shift_value)
{
    bcm_flowtracker_check_action_info_t action_info;

    bcm_flowtracker_check_action_info_t_init(&action_info);
    action_info.flags = flags;
    action_info.param = param;
    action_info.action = action;
    action_info.mask_value = mask_value;
    action_info.shift_value = shift_value;

    return bcm_flowtracker_check_action_info_set(unit, check_id, action_info);
}

int add_check_export_info(bcm_flowtracker_check_t check_id,
        uint32 export_check_threshold,
        bcm_flowtracker_check_operation_t operation)
{
    bcm_flowtracker_check_export_info_t export_info;

    bcm_flowtracker_check_export_info_t_init(&export_info);
    export_info.export_check_threshold = export_check_threshold;
    export_info.operation = operation;

    return bcm_flowtracker_check_export_info_set(unit, check_id, export_info);
}

int ft_drop_reason_group_create(int case_num, uint32 *drg_id)
{
    int uint = 0;
    int rv = BCM_E_NONE;
    uint32 options = 0;
    bcm_flowtracker_drop_reasons_t drop_reasons;

    printf("ft_drop_reason_group_create -> case_num=%d\n", case_num);

    switch(case_num) {
        case 1: /* ING MMU DROP REASON GROUP for Dos Attack */
            options = BCM_FLOWTRACKER_DROP_REASON_GROUP_AGGREGATE_MMU;
            BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(drop_reasons);
            BCM_FLOWTRACKER_DROP_REASON_SET(drop_reasons,
                        bcmFlowtrackerDropReasonIngL3DosAttack);
            BCM_FLOWTRACKER_DROP_REASON_SET(drop_reasons,
                        bcmFlowtrackerDropReasonIngL4DosAttack);
            rv = bcm_flowtracker_drop_reason_group_create(unit,
                    options, drop_reasons, drg_id);
            break;

        case 2: /* EGR DROP REASON GROUP for EFP Drop */
            options = BCM_FLOWTRACKER_DROP_REASON_GROUP_AGGREGATE_EGRESS;
            BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(drop_reasons);
            BCM_FLOWTRACKER_DROP_REASON_SET(drop_reasons,
                        bcmFlowtrackerDropReasonEgrFieldProcessor);
            rv = bcm_flowtracker_drop_reason_group_create(unit,
                    options, drop_reasons, drg_id);
            break;

        default:
            return BCM_E_FAIL;
    }

    if (BCM_FAILURE(rv)) {
        printf("[CINT] Failed to Create group rv (%d)\n", rv);
    }

    return rv;
}

int ft_check_create(int case_num, uint32 *check_id)
{
    int rv = BCM_E_NONE;
    uint32 flags = 0;
    uint32 st_flags = 0;
    bcm_flowtracker_check_info_t check_info;
    uint32 dr_gid;

    printf("\nft_check_create -> config_num=%d\n", case_num);

    /* Default Flags */
    flags = BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD;

    /* Create FT Checks */
    switch(case_num) {
        case 1:
            bcm_flowtracker_check_info_t_init(&check_info);
            add_check_info_p1(&check_info, flags, bcmFlowtrackerTrackingParamTypeTTL, 0);
            add_check_info_p2(&check_info, bcmFlowtrackerCheckOpInRange, 1, 100, 0, 0);
            rv = bcm_flowtracker_check_create(unit, 0, check_info, check_id);
            if (rv != BCM_E_NONE) {
                printf("[CINT} Failed to create ft check . rv=%d\n", rv);
                return rv;
            }
            rv = add_check_action_info(*check_id, flags,
                    bcmFlowtrackerTrackingParamTypeNone,
                    bcmFlowtrackerCheckActionCounterIncr, 0, 0);
            BCM_IF_ERROR_RETURN(rv);

            rv = add_check_export_info(*check_id, 2, bcmFlowtrackerCheckOpGreaterEqual);
            BCM_IF_ERROR_RETURN(rv);
            break;

        case 2:
            bcm_flowtracker_check_info_t_init(&check_info);
            add_check_info_p1(&check_info, flags, bcmFlowtrackerTrackingParamTypeByteCount, 0);
            add_check_info_p2(&check_info, bcmFlowtrackerCheckOpInRange, 60, 200, 0, 0);
            rv = bcm_flowtracker_check_create(unit, 0, check_info, check_id);
            if (rv != BCM_E_NONE) {
                printf("[CINT} Failed to create ft check . rv=%d\n", rv);
                return rv;
            }
            rv = add_check_action_info(*check_id, flags,
                    bcmFlowtrackerTrackingParamTypeNone,
                    bcmFlowtrackerCheckActionCounterIncr, 0, 0);
            BCM_IF_ERROR_RETURN(rv);

            rv = add_check_export_info(*check_id, 2, bcmFlowtrackerCheckOpGreaterEqual);
            BCM_IF_ERROR_RETURN(rv);
            break;

        case 3:
            bcm_flowtracker_check_info_t_init(&check_info);
            add_check_info_p1(&check_info, flags, bcmFlowtrackerTrackingParamTypeByteCount, 0);
            add_check_info_p2(&check_info, bcmFlowtrackerCheckOpInRange, 60, 200, 0, 0);
            rv = bcm_flowtracker_check_create(unit, 0, check_info, check_id);
            if (rv != BCM_E_NONE) {
                printf("[CINT} Failed to create ft check . rv=%d\n", rv);
                return rv;
            }
            rv = add_check_action_info(*check_id, flags,
                    bcmFlowtrackerTrackingParamTypeNone,
                    bcmFlowtrackerCheckActionCounterIncr, 0, 0);
            BCM_IF_ERROR_RETURN(rv);

            rv = add_check_export_info(*check_id, 2, bcmFlowtrackerCheckOpGreaterEqual);
            BCM_IF_ERROR_RETURN(rv);
            break;

        case 4:
            bcm_flowtracker_check_info_t_init(&check_info);
            add_check_info_p1(&check_info, flags, bcmFlowtrackerTrackingParamTypeByteCount, 0);
            add_check_info_p2(&check_info, bcmFlowtrackerCheckOpInRange, 60, 200, 0, 0);
            rv = bcm_flowtracker_check_create(unit, 0, check_info, check_id);
            if (rv != BCM_E_NONE) {
                printf("[CINT} Failed to create ft check . rv=%d\n", rv);
                return rv;
            }
            rv = add_check_action_info(*check_id, flags,
                    bcmFlowtrackerTrackingParamTypeNone,
                    bcmFlowtrackerCheckActionCounterIncr, 0, 0);
            BCM_IF_ERROR_RETURN(rv);

            rv = add_check_export_info(*check_id, 2, bcmFlowtrackerCheckOpGreaterEqual);
            BCM_IF_ERROR_RETURN(rv);
            break;

        default:
            return BCM_E_FAIL;
    }

    return 0;
}

int ft_collector_create(int case_num, int *ft_col_id, int *exp_prof_id)
{
    int rv = 0;
    uint32 options = 0;
    bcm_collector_export_profile_t profile_info;
    bcm_collector_info_t *curr_collector_info = NULL;

    printf("\nft_collector_create -> case_num=%d\n", case_num);

    switch(case_num) {
        case 1:
            rv = vlan_port_setup(fte_coll_port1, fte_coll_vlan1, fte_coll_dst_mac1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] vlan_port_setup => rv %d(%s)\n",  rv, bcm_errmsg(rv));
                return rv;
            }

            profile_info.wire_format = bcmCollectorWireFormatIpfix;
            profile_info.max_packet_length = 1500;
            rv = bcm_collector_export_profile_create(unit, 0, exp_prof_id, &profile_info);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] bcm_collector_export_profile_create => rv %d(%s)\n",
                        rv, bcm_errmsg(rv));
                return rv;
            }

            curr_collector_info = &collector_info1;
            break;
        case 2:
            rv = vlan_port_setup(fte_coll_port2, fte_coll_vlan2, fte_coll_dst_mac2);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] vlan_port_setup => rv %d(%s)\n",  rv, bcm_errmsg(rv));
                return rv;
            }

            profile_info.wire_format = bcmCollectorWireFormatIpfix;
            profile_info.max_packet_length = 1500;
            rv = bcm_collector_export_profile_create(unit, 0, exp_prof_id, &profile_info);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] bcm_collector_export_profile_create => rv %d(%s)\n",
                        rv, bcm_errmsg(rv));
                return rv;
            }

            curr_collector_info = &collector_info2;
            break;
        case 3:
            rv = vlan_port_setup(fte_coll_port3, fte_coll_vlan3, fte_coll_dst_mac3);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] vlan_port_setup => rv %d(%s)\n",  rv, bcm_errmsg(rv));
                return rv;
            }

            profile_info.wire_format = bcmCollectorWireFormatIpfix;
            profile_info.max_packet_length = 1500;
            rv = bcm_collector_export_profile_create(unit, 0, exp_prof_id, &profile_info);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] bcm_collector_export_profile_create => rv %d(%s)\n",
                        rv, bcm_errmsg(rv));
                return rv;
            }

            curr_collector_info = &collector_info3;
            break;
        case 4:
            rv = vlan_port_setup(fte_coll_port4, fte_coll_vlan4, fte_coll_dst_mac4);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] vlan_port_setup => rv %d(%s)\n",  rv, bcm_errmsg(rv));
                return rv;
            }

            profile_info.wire_format = bcmCollectorWireFormatIpfix;
            profile_info.max_packet_length = 1500;
            rv = bcm_collector_export_profile_create(unit, 0, exp_prof_id, &profile_info);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] bcm_collector_export_profile_create => rv %d(%s)\n",
                        rv, bcm_errmsg(rv));
                return rv;
            }

            curr_collector_info = &collector_info4;
            break;
        default:
            return BCM_E_FAIL;
    }

    options = 0;
    rv = bcm_collector_create(unit, 0, ft_col_id, curr_collector_info);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] bcm_collector_create => rv %d(%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/* FT_COLLECTOR Ends */

/* Cleanup Start */

/* Destroy Port & Vlan Configured Earlier For Collector */
int vlan_port_destroy(bcm_vlan_t vlan, bcm_mac_t dst_mac)
{
    int rv = BCM_E_NONE;

    /* Destroy vlan created to redirect export packets */
    rv = bcm_vlan_destroy(unit, vlan);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] bcm_vlan_destroy => rv %d(%s)\n", rv, bcm_errmsg(rv));
    }

    /* Delete static MAC address */
    rv = bcm_l2_addr_delete(unit, dst_mac, vlan);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] bcm_l2_addr_delete => rv %d(%s)\n", rv, bcm_errmsg(rv));
    }

    return BCM_E_NONE;
}


int ft_export_template_create(int ft_gid, uint32 set_id,
        int checks, uint32 *ft_check_list, int *ft_templ_id)
{
    int rv = BCM_E_NONE;
    int ix = 0, elem = 0, j = 0;
    uint32 options = 0x0;
    int actual_tracking_params = 0;
    int actual_export_elements = 0;
    char *export_element_string = NULL;
    int max_export_elements = FTE_MAX_INFO_ELEMENTS;
    int max_tracking_params = 20;
    bcm_flowtracker_tracking_param_info_t tracking_params[FTE_MAX_INFO_ELEMENTS];
    bcm_flowtracker_export_element_info_t export_elems_ip[FTE_MAX_INFO_ELEMENTS];
    bcm_flowtracker_export_element_info_t export_elems_op[FTE_MAX_INFO_ELEMENTS];

    printf("ft_export_template_create -> ft_gid=0x%x set_id=0x%x checks=%d\n", ft_gid, set_id, checks);
    for (ix = 0; ix < checks; ix++) {
        printf("[%d]=0x%x ", ix, ft_check_list[ix]);
    }
    printf("\n");

    /*
     * Get tracking params first. This step is done to get
     * export elements from tracking params automatically
     */
    rv = bcm_flowtracker_group_tracking_params_get(unit,
            ft_gid, max_tracking_params, tracking_params,
            &actual_tracking_params);
    if (BCM_FAILURE(rv)) {
        printf("[CINT]bcm_flowtracker_group_tracking_params_get => rv %d(%s)\n",
                rv, bcm_errmsg(rv));
        return rv;
    }

    /* Initialise export elements ip/op structure */
    for (ix = 0; ix < FTE_MAX_INFO_ELEMENTS; ix++) {
        export_elems_ip[ix].data_size = 0;
        export_elems_ip[ix].element = bcmFlowtrackerExportElementTypeReserved;
        export_elems_op[ix].data_size = 0;
        export_elems_op[ix].element = bcmFlowtrackerExportElementTypeReserved;
    }

    /* Covert tracking params into export elements */
    for (j = 0, ix = 0; j < actual_tracking_params; j++) {
        printf("%d ", j);
        print tracking_params[j].param;

        /* Add it later if check_id are given in inputs */
        if (tracking_params[j].param == bcmFlowtrackerTrackingParamTypeFlowtrackerCheck) {
            printf("Skipped\n");
            continue;
        }
        rv = tracking_to_export_param_xlate(unit,
                            tracking_params[j].param,
                            &(export_elems_ip[ix].element));
        ix++;
        if (BCM_FAILURE(rv)) {
            printf("[CINT] Translating tracking param %d to export element
                    failed with rv %d(%s)\n", tracking_params[j].param, rv, bcm_errmsg(rv));
            return rv;
        }
    }

    /* Add FlowGroup to info elements */
    export_elems_ip[ix].element = bcmFlowtrackerExportElementTypeFlowtrackerGroup;
    ix++;

//    export_elems_ip[ix].element = bcmFlowtrackerExportElementTypeFlowIdentifier;
//    export_elems_ip[ix].data_size = 4;
//    ix++;

    export_elems_ip[ix].element = bcmFlowtrackerExportElementTypeExportFlags;
    ix++;

    export_elems_ip[ix].element = bcmFlowtrackerExportElementTypeExportReasons;
    ix++;

    if (checks) {
        for (j = 0; j < checks; j++, ix++) {
            export_elems_ip[ix].element = bcmFlowtrackerExportElementTypeFlowtrackerCheck;
            export_elems_ip[ix].check_id = ft_check_list[j];
        }
    }
    actual_tracking_params = ix;

    /* Validate export template once */
    rv = bcm_flowtracker_export_template_validate(unit,
            ft_gid, actual_tracking_params, export_elems_ip,
            max_export_elements, export_elems_op, &actual_export_elements);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] bcm_flowtracker_export_template_validate => rv %d(%s)\n",
                rv, bcm_errmsg(rv));
        return rv;
    }

    /* Create Template First */
    rv = bcm_flowtracker_export_template_create(unit, options,
            ft_templ_id, set_id, actual_export_elements, export_elems_op);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] bcm_flowtracker_export_template_create => rv %d(%s)\n",
                rv, bcm_errmsg(rv));
        return rv;
    }

    return rv;
}


int ft_group_create(bcm_flowtracker_group_type_t group_type, bcm_flowtracker_group_t *id)
{
    int rv;
    bcm_flowtracker_group_info_t info;
    bcm_flowtracker_group_info_t_init(&info);

    info.group_type = group_type;
    rv = bcm_flowtracker_group_create(unit, 0, id, &info);
    if (rv != BCM_E_NONE) {
        printf("[CINT] Failed to create ft group for group type=%d, err_rv=%d\n", group_type, rv);
    }

    return rv;
}

int ft_group_tracking_params_set(int ft_gid, int case_num, int checks, uint32 *ft_check_list)
{
    int rv = BCM_E_NONE;
    int i = 0, j = 0;
    int num_key_params = 0, num_data_params = 0;
    int num_tracking_params = 0;
    uint8 key_param_mask[10][128];
    bcm_flowtracker_tracking_param_type_t key_param[10] = { 0 };
    bcm_flowtracker_tracking_param_type_t data_param[10] = { 0 };
    bcm_flowtracker_timestamp_source_t data_param_src_ts[10] = { 0 };
    bcm_flowtracker_tracking_param_info_t tracking_info[20];
    uint32 data_param_flags[20] = { 0 };

    for (i = 0; i < checks; i++) {
        printf("[%d]=0x%x ", i, ft_check_list[i]);
    }
    printf("\n");

    /* Initialise tracking info structure */
    for (i = 0; i < 20; i++)
    {
        tracking_info[i].flags = 0;
        tracking_info[i].param = 0;
        tracking_info[i].src_ts = 0;
    }

    switch(case_num) {
        case 1: /* IFT/MFT/EFT */
            num_key_params = 5;
            key_param[0] = bcmFlowtrackerTrackingParamTypeSrcIPv4;
            key_param[1] = bcmFlowtrackerTrackingParamTypeDstIPv4;
            key_param[2] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            key_param[3] = bcmFlowtrackerTrackingParamTypeL4DstPort;
            key_param[4] = bcmFlowtrackerTrackingParamTypeIPProtocol;

            num_data_params = 2;
            data_param[0] = bcmFlowtrackerTrackingParamTypePktCount;
            data_param[1] = bcmFlowtrackerTrackingParamTypeByteCount;
            break;

        case 2:
            num_data_params = 2;
            data_param[0] = bcmFlowtrackerTrackingParamTypePktCount;
            data_param[1] = bcmFlowtrackerTrackingParamTypeByteCount;
            break;

        case 3:
            num_data_params = 2;
            data_param[0] = bcmFlowtrackerTrackingParamTypePktCount;
            data_param[1] = bcmFlowtrackerTrackingParamTypeByteCount;
            break;

        case 4:
            num_data_params = 2;
            data_param[0] = bcmFlowtrackerTrackingParamTypePktCount;
            data_param[1] = bcmFlowtrackerTrackingParamTypeByteCount;
            break;

        default:
            printf("[CINT] Incorrect Case for Param set\n");
            return BCM_E_FAIL;
    }

    /* Fill Tracking info in local structure */
    for (i = 0; i < num_key_params; i++) {
        tracking_info[i].flags = BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY;
        tracking_info[i].param = key_param[i];
        sal_memcpy(tracking_info[i].mask, key_param_mask[i], 128);
    }
    for (; i < (num_key_params + num_data_params); i++) {
        tracking_info[i].flags |= data_param_flags[i - num_key_params];
        tracking_info[i].param = data_param[i - num_key_params];
        tracking_info[i].src_ts = data_param_src_ts[i - num_key_params];
    }

    if (checks) {
        for (j = 0; j < checks; j++, i++) {
            tracking_info[i].flags = 0;
            tracking_info[i].param = bcmFlowtrackerTrackingParamTypeFlowtrackerCheck;
            tracking_info[i].check_id = ft_check_list[j];
        }
    }

    num_tracking_params = num_key_params + num_data_params + checks;

    if (num_tracking_params > 0) {
        /* Set Tracking info to FT Group */
        rv = bcm_flowtracker_group_tracking_params_set(unit,
                ft_gid, num_tracking_params, tracking_info);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] bcm_flowtracker_group_tracking_params_set => rv %d(%s)\n",
                    rv, bcm_errmsg(rv));
            return rv;
        }
    }

    return rv;
}

int ft_group_checks_add(int ft_gid, int checks, uint32 *check_id_list)
{
    int rv = BCM_E_NONE;
    int  i = 0;

    printf("ft_group_checks_add -> ft_gid=0x%x checks=%d\n", ft_gid, checks);
    for (i = 0; i < checks; i++) {
        printf("[%d]=0x%x ", i, check_id_list[i]);
    }
    printf("\n");

    for (i = 0; i < checks; i++) {
        rv = bcm_flowtracker_group_check_add(unit, ft_gid, check_id_list[i]);
        if (rv != BCM_E_NONE) {
            printf ("[CINT] Failed to Add FT Check (0x%x) to FT Group (0x%x) rv (%d)\n", check_id_list[i], ft_gid, rv);
            return rv;
        }
    }

    return rv;
}

int ft_group_collector_add(int ft_gid, int exp_prof_id, int ft_col_id, int ft_temp_id)
{
    int rv = 0;

    printf("ft_group_collector_add -> ft_gid=0x%x exp_prof_id=%d ft_col_id=%d ft_temp_id=%d\n", ft_gid, exp_prof_id, ft_col_id, ft_temp_id);

    rv = bcm_flowtracker_group_collector_attach(unit,
            ft_gid, ft_col_id, exp_prof_id, ft_temp_id);
    if (BCM_FAILURE(rv)) {
        printf ("[CINT] Failed to Add FT Collector to FT Group rv (%d)\n", rv);
        return rv;
    }

    return rv;
}

int ft_group_agg_user_entry_add(int ft_gid, int *flow_index)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_user_entry_handle_t entry_handle;

    printf("ft_group_agg_user_entry_add -> ft_gid=0x%x\n", ft_gid);

    bcm_flowtracker_user_entry_handle_t_init(&entry_handle);

    rv = bcm_flowtracker_user_entry_add(unit, ft_gid, 0, 0, NULL, &entry_handle);
    if (BCM_FAILURE(rv)) {
        printf ("[CINT] Failed to Add User Entry rv (%d)\n", rv);
        return rv;
    }

    *flow_index = entry_handle.flow_index;

    return rv;
}

int ftfp_presel_create(int case_num, int *ftfp_presel)
{
    int rv = BCM_E_NONE;
    bcm_field_entry_t presel_eid;

    printf("ftfp_presel_create -> case_num=%d\n", case_num);

    switch(case_num) {
        case 1:
            rv = bcm_field_presel_create(unit, ftfp_presel);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] bcm_field_presel_create => rv %d(%s)\n",
                        rv, bcm_errmsg(rv));
                return rv;
            }
            presel_eid = *ftfp_presel | BCM_FIELD_QUALIFY_PRESEL;
            rv = bcm_field_qualify_Stage(unit, presel_eid, bcmFieldStageIngressFlowtracker);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] bcm_field_qualify_Stage => rv %d(%s)\n",
                        rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_L4Ports(unit, presel_eid, 1, 1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] bcm_field_qualify_L4Ports => rv %d(%s)\n",
                        rv,bcm_errmsg(rv));
                return rv;
            }
            break;
        default:
            rv = BCM_E_FAIL;
    }

    if (BCM_FAILURE(rv)) {
        printf ("Failed to Create FTFP Presel rv (%d) \n", rv);
    }

    return rv;
}

int ftfp_group_create(int case_num, int slice, int presels, int *presel_ids, int *ftfp_gid)
{
    int rv = 0;
    int i = 0;
    bcm_field_hint_t hint;
    bcm_field_hintid_t hint_id = 0;
    bcm_field_group_config_t group_config;

    printf("ftfp_group_create -> case_num=%d slice=%d presels=%d\n", case_num, slice, presels);
    for (i = 0; i < presels; i++) {
        printf("[%d]=%d ", i, presel_ids[i]);
    }
    printf("\n");

    switch(case_num) {
        case 1:
            /* Create IPv4 - FTFP Group for slice-0 */
            bcm_field_group_config_t_init(&group_config);
            if (presels) {
                group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
                for (i = 0; i < presels; i++) {
                    BCM_FIELD_PRESEL_ADD(group_config.preselset, presel_ids[i]);
                }
            }
            if (slice) {
                /* Hint for Secondary Slice */
                rv = bcm_field_hints_create(unit, &hint_id);
                if (BCM_FAILURE(rv)) {
                    printf ("[CINT] Failed to Create Hint rv (%d)\n", rv);
                    return rv;
                }

                /* Hint Data */
                bcm_field_hint_t_init(&hint);
                hint.hint_type = bcmFieldHintTypeIngressFlowtracker;
                hint.flags = BCM_FIELD_HINT_INGRESS_FLOWTRACKER_GROUP_SECONDARY_SLICE;

                /* Associating the above configured hints to hint id */
                rv = bcm_field_hints_add(unit, hint_id, &hint);
                if (BCM_FAILURE(rv)) {
                    printf ("[CINT] Failed to Add Hint rv (%d)\n", rv);
                    return rv;
                }

                group_config.hintid = hint_id;
            }
            BCM_FIELD_QSET_INIT(group_config.qset);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngressFlowtracker);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyIpProtocol);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyOuterVlanId);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInnerVlanId);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyVlanFormat);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyForwardingType);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyVpn);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyForwardingVlanId);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyApplSignatureId);
            group_config.mode = bcmFieldGroupModeAuto;

            break;

        case 2:
            /* Create AMFTFP Group */
            bcm_field_group_config_t_init(&group_config);

            BCM_FIELD_QSET_INIT(group_config.qset);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageAggregateMmuFlowtracker);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyFlowtrackerAggregateClass);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyFlowtrackerClassIdSource1);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyFlowtrackerClassIdSource2);
            group_config.mode = bcmFieldGroupModeAuto;
            break;

        case 3:
            /* Create AEFTFP Group */
            BCM_FIELD_QSET_INIT(group_config.qset);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageAggregateEgressFlowtracker);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyFlowtrackerAggregateClass);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyFlowtrackerClassIdSource1);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyFlowtrackerClassIdSource2);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyCosQueueId);
            group_config.mode = bcmFieldGroupModeAuto;
            break;

        default:
            printf ("[CINT] Wrong Arguments to create ftfp group\n");
            return BCM_E_FAIL;
    }

    rv = bcm_field_group_config_create(unit, &group_config);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] bcm_field_group_config_create => rv %d(%s)\n",
                rv, bcm_errmsg(rv));
        return rv;
    }
    *ftfp_gid = group_config.group;

    return rv;
}

int ftfp_entry_create(int case_num, int ftfp_gid, int *ftfp_entry)
{
    int rv = BCM_E_NONE;

    rv = bcm_field_entry_create(unit, ftfp_gid, ftfp_entry);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] bcm_field_entry_create => rv %d(%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }

    switch(case_num) {
        case 1:
            /* Create & Install IPv4 FTFP Group Entry */
            rv = bcm_field_qualify_IpProtocol(unit, *ftfp_entry, 0x6, 0xff);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] bcm_field_qualify_IpProtocol => rv %d(%s)\n",
                        rv, bcm_errmsg(rv));
                return rv;
            }
            /* In Port */
            rv = bcm_field_qualify_InPort(unit, *ftfp_entry, ingress_port, 0xff);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] bcm_field_qualify_InPort => rv %d(%s)\n",
                        rv, bcm_errmsg(rv));
                return rv;
            }
            break;

        case 2:
            /* Qualify later */
            break;

        default:
            return BCM_E_FAIL;
    }

    return 0;
}

int ft_aggregate_class_create(uint32 *agg_class)
{
    int rv = BCM_E_NONE;
    uint32 options = 0;
    bcm_flowtracker_aggregate_class_info_t aggregate_info;

    bcm_flowtracker_aggregate_class_info_t_init(&aggregate_info);
    aggregate_info.source_type1 = bcmFlowtrackerClassSourceTypeNone;
    aggregate_info.source_type2 = bcmFlowtrackerClassSourceTypeNone;
    rv = bcm_flowtracker_aggregate_class_create(unit, options,
            bcmFlowtrackerGroupTypeAggregateMmu, &aggregate_info, agg_class);
    BCM_IF_ERROR_RETURN(rv);

    return rv;
}


int ftfp_entry_qualify_agg_class(int ftfp_entry, uint32 agg_class, uint16 src_class1, uint16 src_class2)
{
    int rv = BCM_E_NONE;

    printf("ftfp_entry_qualify_agg_class -> ftfp_entry=%d agg_class=%d src_class1=%d src_class_2=%d\n", ftfp_entry, agg_class, src_class1, src_class2);

    if (agg_class != 0) {
        rv = bcm_field_qualify_FlowtrackerAggregateClass(unit, ftfp_entry, agg_class, -1);
    }
    if (BCM_FAILURE(rv)) {
        printf("Failed to Qualify Aggregate Class rv (%d)\n", rv);
        return BCM_E_FAIL;
    }

    if (src_class1 != 0) {
        rv = bcm_field_qualify_FlowtrackerClassIdSource1(unit, ftfp_entry, src_class1, -1);
    }
    if (BCM_FAILURE(rv)) {
        printf("Failed to Qualify Aggregate Class Source 1 rv (%d)\n", rv);
        return BCM_E_FAIL;
    }

    if (src_class2 != 0) {
        rv = bcm_field_qualify_FlowtrackerClassIdSource2(unit, ftfp_entry, src_class2, -1);
    }
    if (BCM_FAILURE(rv)) {
        printf("Failed to Qualify Aggregate Class Source 2 rv (%d)\n", rv);
        return BCM_E_FAIL;
    }

    return 0;
}

int ftfp_entry_actions_add_micro(int ftfp_entry, int ft_gid)
{
    int rv = BCM_E_NONE;

    printf("ftfp_entry_actions_add_micro -> ftfp_entry=%d ft_gid=0x%x \n", ftfp_entry, ft_gid);

    if (ft_gid != -1) {
        rv = bcm_field_action_add(unit, ftfp_entry, bcmFieldActionFlowtrackerGroupId, ft_gid, 0);
    }

    return rv;
}

int ftfp_entry_actions_add_agg(int ftfp_entry, int type, int ft_gid, int ft_index)
{
    int rv = BCM_E_NONE;
    bcm_field_action_t action1;
    bcm_field_action_t action2;

    printf("ftfp_entry_actions_add_agg -> ftfp_entry=%d type=%d ft_gid=0x%x ft_index=%d\n", ftfp_entry, type, ft_gid, ft_index);

    switch(type) {
        case 1:
            action1 = bcmFieldActionFlowtrackerAggregateIngressGroupId;
            action2 = bcmFieldActionFlowtrackerAggregateIngressFlowIndex;
            break;
        case 2:
            action1 = bcmFieldActionFlowtrackerAggregateMmuGroupId;
            action2 = bcmFieldActionFlowtrackerAggregateMmuFlowIndex;
            break;
        case 3:
            action1 = bcmFieldActionFlowtrackerAggregateEgressGroupId;
            action2 = bcmFieldActionFlowtrackerAggregateEgressFlowIndex;
            break;
        default:
            break;
    }

    if (ft_gid != -1) {
        rv = bcm_field_action_add(unit, ftfp_entry, action1, ft_gid, 0);
            BCM_IF_ERROR_RETURN(rv);
    }
    if (ft_index != -1) {
        rv = bcm_field_action_add(unit, ftfp_entry, action2, ft_gid, ft_index);
    }

    return rv;
}

int ftfp_entry_actions_add_other(int ftfp_entry, uint32 agg_class, int new_learn, int ft_enable)
{
    int rv = BCM_E_NONE;

    printf("ftfp_entry_actions_add_other -> ftfp_entry=%d agg_class=%d\n", ftfp_entry, agg_class);

    if (agg_class != 0) {
        rv = bcm_field_action_add(unit, ftfp_entry, bcmFieldActionFlowtrackerAggregateClass, agg_class, 0);
    }
    if (BCM_FAILURE(rv)) {
        printf("[CINT] Faled to Add Aggregate Class as Action rv (%d)\n", rv);
        return BCM_E_FAIL;
    }

    if (new_learn != -1) {
        rv = bcm_field_action_add(unit, ftfp_entry, bcmFieldActionFlowtrackerNewLearnEnable, new_learn, 0);
    }
    if (BCM_FAILURE(rv)) {
        printf("[CINT] Faled to Add NewLearnEnable as Action rv (%d)\n", rv);
        return BCM_E_FAIL;
    }


    if (ft_enable != -1) {
        rv = bcm_field_action_add(unit, ftfp_entry, bcmFieldActionFlowtrackerEnable, ft_enable, 0);
    }
    if (BCM_FAILURE(rv)) {
        printf("[CINT] Faled to Add FloatrackerEnable as Action rv (%d)\n", rv);
        return BCM_E_FAIL;
    }

    return rv;
}

int ftfp_entry_install(int ftfp_entry)
{
    int rv = BCM_E_NONE;

    printf("ftfp_entry_install -> ftfp_entry=%d\n", ftfp_entry);

    rv = bcm_field_entry_install(unit, ftfp_entry);
    if (BCM_FAILURE(rv)) {
        printf ("[CINT] failed to install FTFP Entry rv (%d)\n", rv);
    }

    return rv;
}



int broadscan_setup(int unit)
{
    int micro_gid, micro_col_id, micro_temp_id;
    int aift_gid, aift_col_id, aift_temp_id, aift_fidx;
    int amft_gid, amft_col_id, amft_temp_id, amft_fidx;
    int aeft_gid, aeft_col_id, aeft_temp_id, aeft_fidx;
    int micro_exp_prof_id, aift_exp_prof_id, amft_exp_prof_id, aeft_exp_prof_id;
    int ftfp_ing_presel[1], ftfp_ing_group1, ftfp_ing_entry1;
    int ftfp_mmu_group1, ftfp_mmu_entry1;
    int ftfp_egr_group1, ftfp_egr_entry1;
    uint32 check_list[10];
    uint32 agg_class;


    /* Micro Flow Group setup */
    /*------------------------------------------------------------------------------------*/
    /* Create FT Collector for FT Group */
    BCM_IF_ERROR_RETURN(ft_collector_create(1, &micro_col_id, &micro_exp_prof_id));

    /* Create FT Group */
    BCM_IF_ERROR_RETURN(ft_group_create(bcmFlowtrackerGroupTypeNone, &micro_gid));

    /* Set Ft Group Flow Limit to 10 */
    BCM_IF_ERROR_RETURN(bcm_flowtracker_group_flow_limit_set(unit, micro_gid, 10));

    /* Create Flow Check */
    BCM_IF_ERROR_RETURN(ft_check_create(1, &check_list[0]));

    /* Add Check to FT Group */
    BCM_IF_ERROR_RETURN(ft_group_checks_add(micro_gid, 1, &check_list[0]));

    /* Set Tracking param to FT Group */
    BCM_IF_ERROR_RETURN(ft_group_tracking_params_set(micro_gid, 1, 1, &check_list[0]));

    /* Validate and create template for FT Group */
    BCM_IF_ERROR_RETURN(ft_export_template_create(micro_gid, 0x1ef1, 1, &check_list[0], &micro_temp_id));

    /* Add FT Collector / Template to Group */
    BCM_IF_ERROR_RETURN(ft_group_collector_add(micro_gid, micro_exp_prof_id, micro_col_id, micro_temp_id));
    /*------------------------------------------------------------------------------------*/




    /* Aggregate Ingress Flow Group setup */
    /*------------------------------------------------------------------------------------*/
    /* Create FT Collector for AIFT Group */
    BCM_IF_ERROR_RETURN(ft_collector_create(2, &aift_col_id, &aift_exp_prof_id));

    /* Create AIFT Group */
    BCM_IF_ERROR_RETURN(ft_group_create(bcmFlowtrackerGroupTypeAggregateIngress, &aift_gid));

    /* Create Flow Check */
    BCM_IF_ERROR_RETURN(ft_check_create(2, &check_list[1]));

    /* Add Check to FT Group */
    BCM_IF_ERROR_RETURN(ft_group_checks_add(aift_gid, 1, &check_list[1]));

    /* Set Tracking param to FT Group */
    BCM_IF_ERROR_RETURN(ft_group_tracking_params_set(aift_gid, 2, 1, &check_list[1]));

    /* Validate and create template for FT Group */
    BCM_IF_ERROR_RETURN(ft_export_template_create(aift_gid, 0x1ef2, 1, &check_list[1], &aift_temp_id));

    /* Add FT Collector / Template to Group */
    BCM_IF_ERROR_RETURN(ft_group_collector_add(aift_gid, aift_exp_prof_id, aift_col_id, aift_temp_id));

    /* Create User Entry */
    BCM_IF_ERROR_RETURN(ft_group_agg_user_entry_add(aift_gid, &aift_fidx));
    /*------------------------------------------------------------------------------------*/



    /* Aggregate MMU Flow Group setup */
    /*------------------------------------------------------------------------------------*/
    /* Create FT Collector for AMFT Group */
    BCM_IF_ERROR_RETURN(ft_collector_create(3, &amft_col_id, &amft_exp_prof_id));

    /* Create AMFT Group */
    BCM_IF_ERROR_RETURN(ft_group_create(bcmFlowtrackerGroupTypeAggregateMmu, &amft_gid));

    /* Create Flow Check */
    BCM_IF_ERROR_RETURN(ft_check_create(3, &check_list[2]));

    /* Add Check to FT Group */
    BCM_IF_ERROR_RETURN(ft_group_checks_add(amft_gid, 1, &check_list[2]));

    /* Set Tracking param to FT Group */
    BCM_IF_ERROR_RETURN(ft_group_tracking_params_set(amft_gid, 3, 1, &check_list[2]));

    /* Validate and create template for FT Group */
    BCM_IF_ERROR_RETURN(ft_export_template_create(amft_gid, 0x1ef3, 1, &check_list[2], &amft_temp_id));

    /* Add FT Collector / Template to Group */
    BCM_IF_ERROR_RETURN(ft_group_collector_add(amft_gid, amft_exp_prof_id, amft_col_id, amft_temp_id));

    /* Create User Entry */
    BCM_IF_ERROR_RETURN(ft_group_agg_user_entry_add(amft_gid, &amft_fidx));
    /*------------------------------------------------------------------------------------*/



    /* Aggregate EFT Flow Group setup */
    /*------------------------------------------------------------------------------------*/
    /* Create FT Collector for AEFT Group */
    BCM_IF_ERROR_RETURN(ft_collector_create(4, &aeft_col_id, &aeft_exp_prof_id));

    /* Create AEFT Group */
    BCM_IF_ERROR_RETURN(ft_group_create(bcmFlowtrackerGroupTypeAggregateEgress, &aeft_gid));

    /* Create Flow Check */
    BCM_IF_ERROR_RETURN(ft_check_create(4, &check_list[3]));

    /* Add Check to FT Group */
    BCM_IF_ERROR_RETURN(ft_group_checks_add(aeft_gid, 1, &check_list[3]));

    /* Set Tracking param to FT Group */
    BCM_IF_ERROR_RETURN(ft_group_tracking_params_set(aeft_gid, 4, 1, &check_list[3]));

    /* Validate and create template for FT Group */
    BCM_IF_ERROR_RETURN(ft_export_template_create(aeft_gid, 0x1ef4, 1, &check_list[3], &aeft_temp_id));

    /* Add FT Collector / Template to Group */
    BCM_IF_ERROR_RETURN(ft_group_collector_add(aeft_gid, aeft_exp_prof_id, aeft_col_id, aeft_temp_id));

    /* Create User Entry */
    BCM_IF_ERROR_RETURN(ft_group_agg_user_entry_add(aeft_gid, &aeft_fidx));
    /*------------------------------------------------------------------------------------*/




    /* Flow Selection Steps for FTFP, FTFP2 */
    /*------------------------------------------------------------------------------------*/
    /* Create Aggregate Class */
    BCM_IF_ERROR_RETURN(ft_aggregate_class_create(&agg_class));

    /* Create FTFP Presel */
    BCM_IF_ERROR_RETURN(ftfp_presel_create(1, &ftfp_ing_presel[0]));

    /* Create FTFP Group */
    BCM_IF_ERROR_RETURN(ftfp_group_create(1, 1, 1, &ftfp_ing_presel[0], &ftfp_ing_group1));

    /* FTFP Entry Create */
    BCM_IF_ERROR_RETURN(ftfp_entry_create(1, ftfp_ing_group1, &ftfp_ing_entry1));

    /* Add Actions to FTFP Entry */
    BCM_IF_ERROR_RETURN(ftfp_entry_actions_add_micro(ftfp_ing_entry1, micro_gid));
    BCM_IF_ERROR_RETURN(ftfp_entry_actions_add_agg(ftfp_ing_entry1, 1, aift_gid, aift_fidx));
    BCM_IF_ERROR_RETURN(ftfp_entry_actions_add_other(ftfp_ing_entry1, agg_class, -1, -1));

    /* FTFP Entry Install */
    BCM_IF_ERROR_RETURN(ftfp_entry_install(ftfp_ing_entry1));
    /*------------------------------------------------------------------------------------*/



    /* Flow Selection Steps for AMFT */
    /*------------------------------------------------------------------------------------*/
    /* Create FTFP MMU Group */
    BCM_IF_ERROR_RETURN(ftfp_group_create(2, 0, 0, NULL, &ftfp_mmu_group1));

    /* FTFP MMU Entry Create */
    BCM_IF_ERROR_RETURN(ftfp_entry_create(2, ftfp_mmu_group1, &ftfp_mmu_entry1));

    /* FTFP MMU Entry Qualify */
    BCM_IF_ERROR_RETURN(ftfp_entry_qualify_agg_class(ftfp_mmu_group1, agg_class, 0, 0));

    /* Add Actions to FTFP MMU Entry */
    BCM_IF_ERROR_RETURN(ftfp_entry_actions_add_agg(ftfp_mmu_entry1, 2, amft_gid, amft_fidx));

    /* FTFP Entry MMU Install */
    BCM_IF_ERROR_RETURN(ftfp_entry_install(ftfp_mmu_entry1));
    /*------------------------------------------------------------------------------------*/



    /* Flow Selection Steps for AEFT */
    /*------------------------------------------------------------------------------------*/
    /* Create FTFP EGR Group */
    BCM_IF_ERROR_RETURN(ftfp_group_create(3, 0, 0, NULL, &ftfp_egr_group1));

    /* FTFP EGR Entry Create */
    BCM_IF_ERROR_RETURN(ftfp_entry_create(2, ftfp_egr_group1, &ftfp_egr_entry1));

    /* FTFP EGR Entry Qualify */
    BCM_IF_ERROR_RETURN(ftfp_entry_qualify_agg_class(ftfp_egr_group1, agg_class, 0, 0));

    /* Add Actions to FTFP EGR Entry */
    BCM_IF_ERROR_RETURN(ftfp_entry_actions_add_agg(ftfp_egr_entry1, 3, aeft_gid, aeft_fidx));

    /* FTFP Entry EGR Install */
    BCM_IF_ERROR_RETURN(ftfp_entry_install(ftfp_egr_entry1));
    /*------------------------------------------------------------------------------------*/


    return 0;
}

int tx_pkt_data(int unit, uint8 *pktData, int length, int target_port)
{
    int rv = BCM_E_NONE;
    bcm_pkt_t  *pkt;

    /* Allocate a packet structure and associated DMA packet data buffer. */
    rv = bcm_pkt_alloc(unit, length, BCM_TX_CRC_REGEN, &pkt);
    if(BCM_FAILURE(rv)) {
        printf("[CINT] Traffic - Error in bcm_pkt_alloc(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Copy packet data from caller into DMA packet data buffer */
    bcm_pkt_memcpy(pkt, 0, pktData, length);
    BCM_PBMP_PORT_SET(pkt->tx_pbmp, target_port);

    /* Transmit packet, wait for DMA to complete before returning */
    rv = bcm_tx(unit, pkt, NULL);
    if(BCM_FAILURE(rv)) {
        printf("[CINT] Traffic - Error in bcm_tx(): %s.\n",bcm_errmsg(rv));
    }

    /* Free packet resources */
    bcm_pkt_free(unit, pkt);

    return rv;
}

int tx_broadscan_test_packet(int unit, uint8 *pktData, int length, int target_port, int num_packets)
{
    int rv = BCM_E_NONE, idx = 0;

    /* Transmit all packets across target port */
    for (idx = 0; idx < num_packets; idx++) {
        rv = tx_pkt_data(unit, pktData, length, target_port);
        if(BCM_FAILURE(rv)) {
            printf("[CINT] Traffic - tx_pkt_data failed with rv %d\n",rv,bcm_errmsg(rv));
            return rv;
        }

        sal_sleep(1);
        bshell(0, "ft show entry");
    }

    return BCM_E_NONE;
}



/* verify broadscan feature by sending packets */
bcm_error_t verify(int unit)
{
    int rv;
    int traffic_pkt_num = 2;

    /* Send traffic */
    rv = tx_broadscan_test_packet(unit, ipv4_tcp_sync_pkt1,
            sizeof(ipv4_tcp_sync_pkt1), ingress_port, traffic_pkt_num);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] Traffic - Test Traffic Tx failed on port %d, rv %d(%s)\n",
                ingress_port, rv, bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;

}

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

    /* Configure Broadscan */
    rv = broadscan_setup(unit);
    if (BCM_FAILURE(rv)) {
        printf("\n Failed to configure broadscan: %d\n", rv);
            return rv;
    } else {
        printf ("\n !!! Broadscan configured succesfully !!!!\r\n");
    }
 
    /* Verify */
    rv = verify(unit);
    if (BCM_FAILURE(rv)) {
        printf("\n Failed to verify: %d\n", rv);
            return rv;
    } else {
        printf ("\n !!! Broadscan verification succesful !!!!\r\n");
    }

    return rv;
}


const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}


