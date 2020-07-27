/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * C file to put FlowTracker collector related api calls
 */

/* FT Collector Configuration Variables */
uint8 ftv2_collector_test_config   = 0;
int collector_id1                  = -1;
int export_profile_id1             = -1;
bcm_mac_t fte_coll_dst_mac         = {0x00, 0x00, 0x00, 0x00, 0x00, 0xc0};
bcm_mac_t fte_coll_src_mac         = {0x00, 0x00, 0x00, 0x00, 0x00, 0xf0};
bcm_vlan_t    fte_coll_vlan        = 0x600;
uint16        fte_coll_tpid        = 0x8100;
bcm_ip_t      fte_coll_src_ip      = 0x0a82562a;
bcm_ip_t      fte_coll_dst_ip      = 0x0a82b00a;
uint8         fte_coll_dscp        = 0xE1;
uint8         fte_coll_ttl         = 9;
bcm_l4_port_t fte_coll_l4_src_port = 0x1F91;
bcm_l4_port_t fte_coll_l4_dst_port = 0x0807;
uint16        fte_coll_mtu         = 1500;

/* FT Collector Port & Vlan Configuration Variables */
/* Collector port 0 means to CPU */
int default_switch_control_value1 = 0;
bcm_port_t  fte_coll_port = 0;

/*
 * Can be called independantly.
 * Programs following Hardware Memories
 *  - d chg L2X
 *  - d chg VLAN_TAB
 *  - d chg VLAN_ATTRS_1
 *  - d chg EGR_VLAN
 *  - d chg ING_VLAN_VFI_MEMBERSHIP
 *  - d chg EGR_VLAN_VFI_MEMBERSHIP
 *  - g chg CPU_CONTROL_1
 */
/* Configure Port & Vlan For Collector */
int ftv2_collector_port_vlan_setup(uint32 tc_id, bcm_vlan_t fte_coll_vlan, bcm_mac_t fte_coll_dst_mac)
{
    int rv = BCM_E_NONE;
    bcm_pbmp_t pbmp, upbmp;
    bcm_l2_addr_t l2_addr;

    /* Create vlan to redirect export packets */
    rv = bcm_vlan_create(unit, fte_coll_vlan);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - bcm_vlan_create => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, fte_coll_port);
    rv = bcm_vlan_port_add(unit, fte_coll_vlan, pbmp, upbmp);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - bcm_vlan_port_add => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }

    /* Add static MAC address */
    bcm_l2_addr_t_init(&l2_addr, fte_coll_dst_mac, fte_coll_vlan);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port  = fte_coll_port;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - bcm_l2_addr_add => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }

    /* Enable L2StaticMoveToCPU so that the packet goes to CPU on LB. */
    rv = bcm_switch_control_get(unit, bcmSwitchL2StaticMoveToCpu, &default_switch_control_value1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - bcm_switch_control_get => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchL2StaticMoveToCpu, TRUE);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - bcm_switch_control_set => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Destroy Port & Vlan Configured Earlier For Collector */
int ftv2_collector_port_vlan_destroy(uint32 tc_id, bcm_vlan_t fte_coll_vlan, bcm_mac_t fte_coll_dst_mac)
{
    int rv = BCM_E_NONE;

    /* Destroy vlan created to redirect export packets */
    rv = bcm_vlan_destroy(unit, fte_coll_vlan);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - bcm_vlan_destroy => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }

    /* Delete static MAC address */
    rv = bcm_l2_addr_delete(unit, fte_coll_dst_mac, fte_coll_vlan);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - bcm_l2_addr_delete => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }

    /* Restore Default Switch Control Value. */
    rv = bcm_switch_control_set(unit, bcmSwitchL2StaticMoveToCpu, default_switch_control_value1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - bcm_switch_control_set => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Can be called independantly.
 * Programs following Hardware Memories
 *  - d chg BSC_EX_COLLECTOR_IPFIX_PACKET_BUILD
 *  - d chg BSC_EX_HDR_CONSTRUCT_CFG
 *  - d chg BSC_EX_COLLECTOR_CONFIG
 *  - g chg BSC_EX_SEQ_NUM
 *  - g chg BSC_EX_EXPORT_TRIGGER
 *  - g chg IPFIX_TOD_SELECT_0-7
 */
/* Create FT Collector as per ftv2_collector_test_config */
int
ftv2_collector_create_main(uint32 tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;
    bcm_collector_info_t collector_info;
    bcm_collector_export_profile_t profile_info;
    uint32 options = 0;

    /* Skip this collector create if no test config set */
    if (!ftv2_collector_test_config) {
        if (!skip_log) {
            printf("[CINT] Step3 - Skip Collector Setup ......\n");
        }
        return rv;
    }

    /* Create FT Collector as per test config set */
    switch(ftv2_collector_test_config) {

        case 1:
            /* ftv2_collector_test_config = 1 */
            /* Setup Port & Vlan For Collector First */
            rv = ftv2_collector_port_vlan_setup(tc_id, fte_coll_vlan, fte_coll_dst_mac);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ftv2_collector_port_vlan_setup => rv %d(%s)\n",
                                                            tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Step3 - Collector Port & Vlan Setup Done.\n");
            }

            /* Create Collector now. */
            bcm_collector_info_t_init(&collector_info);
            bcm_collector_export_profile_t_init(&profile_info);
            sal_memcpy(collector_info.eth.dst_mac, fte_coll_dst_mac, 6);
            sal_memcpy(collector_info.eth.src_mac, fte_coll_src_mac, 6);
            collector_info.eth.vlan_tag_structure =
                BCM_FLOWTRACKER_COLLECTOR_ETH_HDR_SINGLE_TAGGED;
            collector_info.eth.outer_vlan_tag = fte_coll_vlan;
            collector_info.eth.outer_tpid     = fte_coll_tpid;
            collector_info.ipv4.src_ip        = fte_coll_src_ip;
            collector_info.ipv4.dst_ip        = fte_coll_dst_ip;
            collector_info.ipv4.dscp          = fte_coll_dscp;
            collector_info.ipv4.ttl           = fte_coll_ttl;
            collector_info.udp.src_port       = fte_coll_l4_src_port;
            collector_info.udp.dst_port       = fte_coll_l4_dst_port;
            collector_info.ipfix.version = 0xA;
            collector_info.ipfix.initial_sequence_num = 0x2;
            collector_info.transport_type     = bcmCollectorTransportTypeIpv4Udp;
            collector_id1 = 0;
            rv = bcm_collector_create(unit,options, &collector_id1, collector_info);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_collector_create with Legacy TS => rv %d(%s)\n",
                                                                  tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Step3 - Collector Id created with Legacy timestamp: %d\n", collector_id1);
            }

            profile_info.wire_format = bcmCollectorWireFormatIpfix;
            profile_info.max_packet_length = fte_coll_mtu;
            rv = bcm_collector_export_profile_create(unit, 0, &export_profile_id1, &profile_info);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_collector_export_profile_create => rv %d(%s)\n",
                                                                  tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Step3 - Collector Export Profile Id created: %d\n", export_profile_id1);
            }
            break;

        case 10:
            /* ftv2_collector_test_config = 10 */
            /* Setup Port & Vlan For Collector First */
            rv = ftv2_collector_port_vlan_setup(tc_id, fte_coll_vlan, fte_coll_dst_mac);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ftv2_collector_port_vlan_setup => rv %d(%s)\n",
                                                             tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Step3 - Collector Port & Vlan Setup Done.\n");
            }

            /* Create Collector now. */
            bcm_collector_info_t_init(&collector_info);
            bcm_collector_export_profile_t_init(&profile_info);
            sal_memcpy(collector_info.eth.dst_mac, fte_coll_dst_mac, 6);
            sal_memcpy(collector_info.eth.src_mac, fte_coll_src_mac, 6);
            collector_info.eth.vlan_tag_structure =
                BCM_FLOWTRACKER_COLLECTOR_ETH_HDR_SINGLE_TAGGED;
            collector_info.eth.outer_vlan_tag = fte_coll_vlan;
            collector_info.eth.outer_tpid     = fte_coll_tpid;
            collector_info.ipv4.src_ip        = fte_coll_src_ip;
            collector_info.ipv4.dst_ip        = fte_coll_dst_ip;
            collector_info.ipv4.dscp          = fte_coll_dscp;
            collector_info.ipv4.ttl           = fte_coll_ttl;
            collector_info.udp.src_port       = fte_coll_l4_src_port;
            collector_info.udp.dst_port       = fte_coll_l4_dst_port;
            collector_info.ipfix.version = 0xA;
            collector_info.ipfix.initial_sequence_num = 0x2;
            collector_info.transport_type     = bcmCollectorTransportTypeIpv4Udp;
            collector_info.src_ts = bcmCollectorTimestampSourcePTP;
            collector_id1 = 0;
            rv = bcm_collector_create(unit,options, &collector_id1, collector_info);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_collector_create with PTP timestamp => rv %d(%s)\n",
                                                                      tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Step3 - Collector Id created with PTP timestamp: %d\n", collector_id1);
            }

            profile_info.wire_format = bcmCollectorWireFormatIpfix;
            profile_info.max_packet_length = fte_coll_mtu;
            rv = bcm_collector_export_profile_create(unit, 0, &export_profile_id1, &profile_info);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_collector_export_profile_create => rv %d(%s)\n",
                                                                  tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Step3 - Collector Export Profile Id created: %d\n", export_profile_id1);
            }
            break;

        case 11:
            /* ftv2_collector_test_config = 11 */
            /* Setup Port & Vlan For Collector First */
            rv = ftv2_collector_port_vlan_setup(tc_id, fte_coll_vlan, fte_coll_dst_mac);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ftv2_collector_port_vlan_setup => rv %d(%s)\n",
                                                             tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Step3 - Collector Port & Vlan Setup Done.\n");
            }

            /* Create Collector now. */
            bcm_collector_info_t_init(&collector_info);
            bcm_collector_export_profile_t_init(&profile_info);
            sal_memcpy(collector_info.eth.dst_mac, fte_coll_dst_mac, 6);
            sal_memcpy(collector_info.eth.src_mac, fte_coll_src_mac, 6);
            collector_info.eth.vlan_tag_structure =
                BCM_FLOWTRACKER_COLLECTOR_ETH_HDR_SINGLE_TAGGED;
            collector_info.eth.outer_vlan_tag = fte_coll_vlan;
            collector_info.eth.outer_tpid     = fte_coll_tpid;
            collector_info.ipv4.src_ip        = fte_coll_src_ip;
            collector_info.ipv4.dst_ip        = fte_coll_dst_ip;
            collector_info.ipv4.dscp          = fte_coll_dscp;
            collector_info.ipv4.ttl           = fte_coll_ttl;
            collector_info.udp.src_port       = fte_coll_l4_src_port;
            collector_info.udp.dst_port       = fte_coll_l4_dst_port;
            collector_info.ipfix.version = 0xA;
            collector_info.ipfix.initial_sequence_num = 0x2;
            collector_info.transport_type     = bcmCollectorTransportTypeIpv4Udp;
            collector_info.src_ts = bcmCollectorTimestampSourceNTP;
            collector_id1 = 0;
            rv = bcm_collector_create(unit,options, &collector_id1, collector_info);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_collector_create with NTP timestamp => rv %d(%s)\n",
                                                                      tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Step3 - Collector Id created with NTP timestamp: %d\n", collector_id1);
            }

            profile_info.wire_format = bcmCollectorWireFormatIpfix;
            profile_info.max_packet_length = fte_coll_mtu;
            rv = bcm_collector_export_profile_create(unit, 0, &export_profile_id1, &profile_info);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_collector_export_profile_create => rv %d(%s)\n",
                                                                 tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Step3 - Collector Export Profile Id created: %d\n", export_profile_id1);
            }
            break;

        default:
            printf("[CINT] TC %2d - invalid collector test config %d\n",tc_id,ftv2_collector_test_config);
            return BCM_E_FAIL;
    }

    /* Dump Memory & Registers */
    if (dump_mem_reg) {
        printf("[CINT] Step3 - Dumping relevant memories & registers ....\n");
        /* Collector Port & Vlan Setup */
        bshell(unit, "d chg L2X");
        bshell(unit, "d chg VLAN_TAB");
        bshell(unit, "d chg VLAN_ATTRS_1");
        bshell(unit, "d chg EGR_VLAN");
        bshell(unit, "d chg ING_VLAN_VFI_MEMBERSHIP");
        bshell(unit, "d chg EGR_VLAN_VFI_MEMBERSHIP");
        bshell(unit, "g chg CPU_CONTROL_1");
        /* Collector Create */
        bshell(unit, "d chg BSC_EX_COLLECTOR_IPFIX_PACKET_BUILD");
        bshell(unit, "d chg BSC_EX_HDR_CONSTRUCT_CFG");
        bshell(unit, "d chg BSC_EX_COLLECTOR_CONFIG");
        bshell(unit, "g chg BSC_EX_SEQ_NUM");
        bshell(unit, "g chg BSC_EX_EXPORT_TRIGGER");
        bshell(unit, "g chg IPFIX_TOD_SELECT_0");
        bshell(unit, "g chg IPFIX_TOD_SELECT_1");
        bshell(unit, "g chg IPFIX_TOD_SELECT_2");
        bshell(unit, "g chg IPFIX_TOD_SELECT_3");
        bshell(unit, "g chg IPFIX_TOD_SELECT_4");
        bshell(unit, "g chg IPFIX_TOD_SELECT_5");
        bshell(unit, "g chg IPFIX_TOD_SELECT_6");
        bshell(unit, "g chg IPFIX_TOD_SELECT_7");
    }

    /* Final Step Log */
    if (!skip_log) {
        printf("[CINT] Step3 - Collector Created Successfully.\n");
    }

    return BCM_E_NONE;
}

/* Destroy FT Collectors */
int
ftv2_collector_destroy_main(uint32 tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;

    /* Skip if no test config set */
    if (!ftv2_collector_test_config) {
        if (!skip_log) {
            printf("[CINT] Cleanup - Skip Collector Destroy ......\n");
        }
        return rv;
    }

    /* Destroy FT Collector  */
    if (collector_id1 != -1) {

        /* Destroy Port & Vlan Configured For Collector First */
        rv = ftv2_collector_port_vlan_destroy(tc_id, fte_coll_vlan, fte_coll_dst_mac);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - ftv2_collector_port_vlan_destroy => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
            return rv;
        }
        if (!skip_log) {
            printf("[CINT] Cleanup - Destroyed Port & Vlan Setup Configured for Collector.\n");
        }

        /* De-Attach template and collector to FT Group */
        rv = bcm_flowtracker_group_collector_detach(unit, flow_group_id1, collector_id1, export_profile_id1, template_id1);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - bcm_flowtracker_group_collector_detach => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
            return rv;
        }
        if (!skip_log) {
            printf("[CINT] Cleanup - Deattached collector from FT Group.\n");
        }

        /* Destroy Collector */
        rv = bcm_collector_destroy(unit, collector_id1);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - bcm_collector_destroy => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
            return rv;
        }
        if (!skip_log) {
            printf("[CINT] Cleanup - Collector Id destroyed: %d\n", collector_id1);
        }
        collector_id1 = -1;

        /* Destroy Export Profile */
        rv = bcm_collector_export_profile_destroy(unit, export_profile_id1);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - bcm_collector_export_profile_destroy => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
            return rv;
        }
        if (!skip_log) {
            printf("[CINT] Cleanup - Collector Export Profile Id destroyed: %d\n", export_profile_id1);
        }
        export_profile_id1 = -1;
    }

    /* Final Step Log */
    if (!skip_log) {
        printf("[CINT] Cleanup - Collector Destroyed Successfully.\n");
    }

    return BCM_E_NONE;
}
