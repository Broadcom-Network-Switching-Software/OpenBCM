/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * C file to put flowtracker check related api calls
 */

/* Broadscan version */
uint8 bs_ver = ft_broadscan_version;

/* checker variables */
uint8 ftv2_group_flow_checker_test_config = 0;
uint32 max_num_flow_checkers = 10;
uint8 actual_num_checkers_configured = 0;
bcm_flowtracker_check_t pri_check_id1[max_num_flow_checkers] = { 0 };
bcm_flowtracker_check_t sec_check_id1[max_num_flow_checkers] = { 0 };

/* Expected checker value for
 * param bcmFlowtrackerTrackingParamTypeFlowtrackerCheck */
uint32 expected_checker_value[max_num_flow_checkers] = 0;

/* Create check entry as per ftv2_group_flow_checker_test_config */
int
ftv2_check_create_main(uint32 tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;
    int iter = 0, iter2 = 0, j = 0, num_checkers = 0;
    bcm_flowtracker_tracking_param_type_t checker_pri_param[max_num_flow_checkers] = { 0 };
    uint32 checker_pri_param_min[max_num_flow_checkers] = { 0 };
    uint32 checker_pri_param_max[max_num_flow_checkers] = { 0 };
    uint32 checker_pri_param_mask[max_num_flow_checkers] = { 0 };
    uint32 checker_pri_param_shift[max_num_flow_checkers] = { 0 };
    uint32 checker_pri_param_flags[max_num_flow_checkers] = { 0 };
    bcm_flowtracker_check_operation_t checker_pri_param_operation[max_num_flow_checkers] = { 0 };
    uint16 checker_pri_num_drop_reason_group[max_num_flow_checkers] = { 0 };
    bcm_flowtracker_drop_reason_group_t checker_pri_drop_reason_group_id[max_num_flow_checkers][BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX];

    uint32 checker_sec_available[max_num_flow_checkers] = { 0 };
    bcm_flowtracker_tracking_param_type_t checker_sec_param[max_num_flow_checkers] = { 0 };
    uint32 checker_sec_param_min[max_num_flow_checkers] = { 0 };
    uint32 checker_sec_param_max[max_num_flow_checkers] = { 0 };
    uint32 checker_sec_param_mask[max_num_flow_checkers] = { 0 };
    uint32 checker_sec_param_shift[max_num_flow_checkers] = { 0 };
    uint32 checker_sec_param_flags[max_num_flow_checkers] = { 0 };
    bcm_flowtracker_check_operation_t checker_sec_param_operation[max_num_flow_checkers] = { 0 };
    uint16 checker_sec_num_drop_reason_group[max_num_flow_checkers] = { 0 };
    bcm_flowtracker_drop_reason_group_t checker_sec_drop_reason_group_id[max_num_flow_checkers][BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX];

    uint32 checker_action_info_available[max_num_flow_checkers] = { 0 };
    bcm_flowtracker_tracking_param_type_t checker_action_param[max_num_flow_checkers] = { 0 };
    bcm_flowtracker_check_action_t checker_action[max_num_flow_checkers] = { 0 };
    uint32 checker_action_mask[max_num_flow_checkers] = { 0 };
    uint32 checker_action_shift[max_num_flow_checkers] = { 0 };

    uint32 checker_export_info_available[max_num_flow_checkers] = { 0 };
    uint32 checker_export_thresold[max_num_flow_checkers] = { 0 };
    bcm_flowtracker_check_operation_t checker_export_action[max_num_flow_checkers] = { 0 };

    uint32 checker_delay_info_available[max_num_flow_checkers] = { 0 };
    bcm_flowtracker_timestamp_source_t checker_src_ts[max_num_flow_checkers] = { 0 };
    bcm_flowtracker_timestamp_source_t checker_dst_ts[max_num_flow_checkers] = { 0 };
    uint8 checker_delay_gran[max_num_flow_checkers] = { 0 };
    int32 checker_delay_offset[max_num_flow_checkers] = { 0 };

    uint32 options_check1 = 0;
    uint32 options_check2 = BCM_FLOWTRACKER_CHECK_SECOND;
    bcm_flowtracker_check_info_t check_info1;
    bcm_flowtracker_check_action_info_t check_action_info1;
    bcm_flowtracker_check_export_info_t check_export_info1;
    bcm_flowtracker_check_delay_info_t check_delay_info1;

    /* Skip this flow check set if no test config set */
    if (!ftv2_group_flow_checker_test_config) {
        if (!skip_log) {
            printf("[CINT] Step13 - Skip Flow Check Create ......\n");
        }
        return rv;
    }

    /* Create Flow Checkers as per test config set */
    /* Remember Me - Add checker here as per config */
    switch(ftv2_group_flow_checker_test_config) {

        case 2:
            /* ftv2_group_flow_checker_test_config = 2 */
            num_checkers = 1;
            checker_pri_param[0] = bcmFlowtrackerTrackingParamTypeTTL;
            checker_pri_param_min[0] = 1;
            checker_pri_param_max[0] = 200;
            checker_pri_param_flags[0] = BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD;
            checker_pri_param_operation[0] = bcmFlowtrackerCheckOpInRange;
            checker_sec_available[0] = 0;
            checker_action_info_available[0] = 1;
            checker_action_param[0] = bcmFlowtrackerTrackingParamTypeNone;
            checker_action[0] = bcmFlowtrackerCheckActionCounterIncr;
            checker_export_info_available[0] = 1;
            checker_export_thresold[0]  = traffic_pkt_num;
            checker_export_action[0] = bcmFlowtrackerCheckOpGreaterEqual;
            /* ALU16 threshold check triggers export */
            expected_export_reason = 0x0040;
            /* All data valid, IFT Triggers it */
            expected_export_flags = 0xe0 + (bs_ver << 1);
            /* Expected value in checker output */
            expected_checker_value[0] = traffic_pkt_num;
            break;

        case 3:
            /* ftv2_group_flow_checker_test_config = 3 */
            num_checkers = 1;
            checker_pri_param[0] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            checker_pri_param_min[0] = 1;
            checker_pri_param_max[0] = 20;
            checker_pri_param_flags[0] = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1 | BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD;
            checker_pri_param_operation[0] = bcmFlowtrackerCheckOpInRange;
            checker_sec_available[0] = 0;
            checker_action_info_available[0] = 1;
            checker_action_param[0] = bcmFlowtrackerTrackingParamTypeNone;
            checker_action[0] = bcmFlowtrackerCheckActionCounterIncr;
            checker_export_info_available[0] = 1;
            checker_export_thresold[0]  = traffic_pkt_num;
            checker_export_action[0] = bcmFlowtrackerCheckOpGreaterEqual;
            /* ALU16 threshold check triggers export */
            expected_export_reason = 0x0040;
            /* All data valid, IFT Triggers it */
            expected_export_flags = 0xe0 + (bs_ver << 1);
            /* Expected value in checker output */
            expected_checker_value[0] = traffic_pkt_num;
            break;

        case 4:
            /* ftv2_group_flow_checker_test_config = 4 */
            num_checkers = 1;
            checker_pri_param[0] = bcmFlowtrackerTrackingParamTypeIngPort;
            checker_pri_param_min[0] = traffic_ingress_port;
            checker_pri_param_flags[0] = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1 | BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD;
            checker_pri_param_operation[0] = bcmFlowtrackerCheckOpEqual;
            checker_sec_available[0] = 0;
            checker_action_info_available[0] = 1;
            checker_action_param[0] = bcmFlowtrackerTrackingParamTypeNone;
            checker_action[0] = bcmFlowtrackerCheckActionCounterIncr;
            checker_export_info_available[0] = 1;
            checker_export_thresold[0]  = traffic_pkt_num;
            checker_export_action[0] = bcmFlowtrackerCheckOpGreaterEqual;
            /* ALU16 threshold check triggers export */
            expected_export_reason = 0x0040;
            /* All data valid, MFT Triggers it */
            expected_export_flags = 0xe8 + (bs_ver << 1);
            /* Expected value in checker output */
            expected_checker_value[0] = traffic_pkt_num;
            break;

        case 5:
            /* ftv2_group_flow_checker_test_config = 5 */
            num_checkers = 1;
            checker_pri_param[0] = bcmFlowtrackerTrackingParamTypeEgrPort;
            checker_pri_param_min[0] = traffic_egress_port;
            checker_pri_param_flags[0] = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1 | BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD;
            checker_pri_param_operation[0] = bcmFlowtrackerCheckOpEqual;
            checker_sec_available[0] = 0;
            checker_action_info_available[0] = 1;
            checker_action_param[0] = bcmFlowtrackerTrackingParamTypeNone;
            checker_action[0] = bcmFlowtrackerCheckActionCounterIncr;
            checker_export_info_available[0] = 1;
            checker_export_thresold[0]  = traffic_pkt_num;
            checker_export_action[0] = bcmFlowtrackerCheckOpGreaterEqual;
            /* ALU16 threshold check triggers export */
            expected_export_reason = 0x0040;
            /* All data valid, MFT Triggers it */
            expected_export_flags = 0xF0 + (bs_ver << 1);
            /* Expected value in checker output */
            expected_checker_value[0] = traffic_pkt_num;
            break;

        case 6:
            /* ftv2_group_flow_checker_test_config = 6 */
            num_checkers = 1;
            checker_pri_param[0] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            checker_pri_param_min[0] =0x3;
            checker_pri_param_mask[0] = 0x7;
            checker_pri_param_shift[0] = 1;
            checker_pri_param_flags[0] = BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD;
            checker_pri_param_operation[0] = bcmFlowtrackerCheckOpEqual;
            checker_sec_available[0] = 1;
            checker_sec_param[0] = bcmFlowtrackerTrackingParamTypeTTL;
            checker_sec_param_min[0] = 0x4;
            checker_sec_param_mask[0] = 0xF0;
            checker_sec_param_shift[0] = 4;
            checker_sec_param_operation[0] = bcmFlowtrackerCheckOpEqual;
            checker_action_info_available[0] = 1;
            checker_action_param[0] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            checker_action[0] = bcmFlowtrackerCheckActionUpdateValue;
            checker_action_mask[0] = 0xFF00;
            checker_action_shift[0] = 8;
            checker_export_info_available[0] = 1;
            checker_export_thresold[0]  = 0x33;
            checker_export_action[0] = bcmFlowtrackerCheckOpGreaterEqual;
            /* ALU16 threshold check triggers export */
            expected_export_reason = 0x0040;
            /* IFT data valid, IFT Triggers it */
            expected_export_flags = 0x80 + (bs_ver << 1);
            /* Expected value in checker output */
            expected_checker_value[0] = 0x33;
            break;

        case 7:
            /* ftv2_group_flow_checker_test_config = 7 */
            num_checkers = 1;
            checker_pri_param[0] = bcmFlowtrackerTrackingParamTypeEgrPort;
            checker_pri_param_min[0] = traffic_egress_port;
            checker_pri_param_flags[0] = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1 | BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD;
            checker_pri_param_operation[0] = bcmFlowtrackerCheckOpEqual;
            checker_sec_available[0] = 0;
            checker_action_info_available[0] = 1;
            checker_action_param[0] = bcmFlowtrackerTrackingParamTypeNone;
            checker_action[0] = bcmFlowtrackerCheckActionCounterIncr;
            checker_export_info_available[0] = 1;
            checker_export_thresold[0]  = traffic_pkt_num;
            checker_export_action[0] = bcmFlowtrackerCheckOpGreaterEqual;
            /* ALU16 threshold check triggers export */
            expected_export_reason = 0x0040;
            /* All data valid, EFT Triggers it */
            expected_export_flags = 0xF0 + (bs_ver << 1);
            /* Expected value in checker output */
            expected_checker_value[0] = traffic_pkt_num;
            break;

        case 8:
            /* ftv2_group_flow_checker_test_config = 8 */
            num_checkers = 1;
            checker_pri_param[0] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            checker_pri_param_min[0] = pkt_protocol;
            checker_pri_param_flags[0] = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1 | BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD;
            checker_pri_param_operation[0] = bcmFlowtrackerCheckOpEqual;
            checker_sec_available[0] = 0;
            checker_action_info_available[0] = 1;
            checker_action_param[0] = bcmFlowtrackerTrackingParamTypeNone;
            checker_action[0] = bcmFlowtrackerCheckActionCounterIncr;
            checker_export_info_available[0] = 1;
            checker_export_thresold[0]  = traffic_pkt_num;
            checker_export_action[0] = bcmFlowtrackerCheckOpGreaterEqual;
            /* ALU16 threshold check triggers export */
            expected_export_reason = 0x0040;
            /* All data valid, IFT Triggers it */
            expected_export_flags = 0xE0 + (bs_ver << 1);
            /* Expected value in checker output */
            expected_checker_value[0] = traffic_pkt_num;
            break;

        case 9:
            /* ftv2_group_flow_checker_test_config = 9 */
            num_checkers = 1;
            checker_pri_param[0] = bcmFlowtrackerTrackingParamTypeEgrPort;
            checker_pri_param_min[0] = traffic_egress_port;
            checker_pri_param_flags[0] = BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD;
            checker_pri_param_operation[0] = bcmFlowtrackerCheckOpEqual;
            checker_sec_available[0] = 0;
            checker_action_info_available[0] = 1;
            checker_action_param[0] = bcmFlowtrackerTrackingParamTypeEgressDelay;
            checker_action[0] = bcmFlowtrackerCheckActionUpdateValue;
            checker_export_info_available[0] = 1;
            checker_export_thresold[0]  = 0x1;
            checker_export_action[0] = bcmFlowtrackerCheckOpGreaterEqual;
            checker_delay_info_available[0] = 1;
            checker_src_ts[0] = bcmFlowtrackerTimestampSourceCMIC;
            checker_dst_ts[0] = bcmFlowtrackerTimestampSourceIngress;
            checker_delay_gran[0] = 4;
            checker_delay_offset[0] = 768;
            /* ALU32 threshold check triggers export */
            expected_export_reason = 0x0020;
            /* All data valid, EFT Triggers it */
            expected_export_flags = 0xF2;
            /* Expected value in checker output */
            /* ((0x0a0a0a0a - 0x01230606) + 768) >> 4 */
            expected_checker_value[0] = 0x008E7070;
            break;

        case 10:
            /* ftv2_group_flow_checker_test_config = 10 */
            num_checkers = 1;
            checker_pri_param[0] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            checker_pri_param_min[0] = pkt_protocol;
            checker_pri_param_flags[0] = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT2 | BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD;
            checker_pri_param_operation[0] = bcmFlowtrackerCheckOpEqual;
            checker_sec_available[0] = 1;
            checker_sec_param[0] = bcmFlowtrackerTrackingParamTypeTTL;
            checker_sec_param_min[0] = pkt_ttl;
            checker_sec_param_operation[0] = bcmFlowtrackerCheckOpEqual;
            checker_action_info_available[0] = 1;
            checker_action_param[0] = bcmFlowtrackerTrackingParamTypeDstIPv4;
            checker_action[0] = bcmFlowtrackerCheckActionUpdateValue;
            checker_export_info_available[0] = 1;
            checker_export_thresold[0]  = 1;
            checker_export_action[0] = bcmFlowtrackerCheckOpGreaterEqual;
            /* ALU32 threshold check triggers export */
            expected_export_reason = 0x0020;
            /* IFT data valid, IFT Triggers it */
            expected_export_flags = 0x82;
            /* Expected value in checker output */
            expected_checker_value[0] = pkt_dst_ipv4;
            break;

        case 11:
            /* ftv2_group_flow_checker_test_config = 11 */
            num_checkers = 1;
            checker_pri_param[0] = bcmFlowtrackerTrackingParamTypeTcpWindowSize;
            checker_pri_param_min[0] = pkt_tcp_window_size;
            checker_pri_param_flags[0] = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT3 | BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD;
            checker_pri_param_operation[0] = bcmFlowtrackerCheckOpEqual;
            checker_sec_available[0] = 1;
            checker_sec_param[0] = bcmFlowtrackerTrackingParamTypeTTL;
            checker_sec_param_min[0] = pkt_ttl;
            checker_sec_param_operation[0] = bcmFlowtrackerCheckOpEqual;
            checker_action_info_available[0] = 1;
            checker_action_param[0] = bcmFlowtrackerTrackingParamTypeL4DstPort;
            checker_action[0] = bcmFlowtrackerCheckActionUpdateValue;
            checker_export_info_available[0] = 1;
            checker_export_thresold[0]  = 1;
            checker_export_action[0] = bcmFlowtrackerCheckOpGreaterEqual;
            /* ALU16 threshold check triggers export */
            expected_export_reason = 0x0040;
            /* IFT data valid, IFT Triggers it */
            expected_export_flags = 0x82;
            /* Expected value in checker output */
            expected_checker_value[0] = pkt_l4_dst_port;
            break;

        case 12:
            /* ftv2_group_flow_checker_test_config = 12 */
            num_checkers = 1;
            checker_pri_param[0] = bcmFlowtrackerTrackingParamTypeEgrPort;
            checker_pri_param_min[0] = traffic_egress_port;
            checker_pri_param_flags[0] = BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD;
            checker_pri_param_operation[0] = bcmFlowtrackerCheckOpEqual;
            checker_sec_available[0] = 0;
            checker_action_info_available[0] = 1;
            checker_action_param[0] = bcmFlowtrackerTrackingParamTypeNone;
            checker_action[0] = bcmFlowtrackerCheckActionCounterIncr;
            checker_export_info_available[0] = 1;
            checker_export_thresold[0]  = traffic_pkt_num;
            checker_export_action[0] = bcmFlowtrackerCheckOpGreaterEqual;
            /* ALU16 threshold check triggers export */
            expected_export_reason = 0x0040;
            /* All data valid, EFT Triggers it */
            expected_export_flags = 0xF2;
            /* Expected value in checker output */
            expected_checker_value[0] = traffic_pkt_num;
            break;

        case 13:
            /* ftv2_group_flow_checker_test_config = 13 */
            num_checkers = 1;
            checker_pri_param[0] = bcmFlowtrackerTrackingParamTypeIngPort;
            checker_pri_param_min[0] = traffic_ingress_port;
            checker_pri_param_flags[0] = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1 | BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD;
            checker_pri_param_operation[0] = bcmFlowtrackerCheckOpEqual;
            checker_sec_available[0] = 0;
            checker_action_info_available[0] = 1;
            checker_action_param[0] = bcmFlowtrackerTrackingParamTypeNone;
            checker_action[0] = bcmFlowtrackerCheckActionCounterIncr;
            checker_export_info_available[0] = 1;
            checker_export_thresold[0]  = traffic_pkt_num;
            checker_export_action[0] = bcmFlowtrackerCheckOpGreaterEqual;
            /* ALU16 threshold check triggers export */
            expected_export_reason = 0x0040;
            /* IFT,MFT data valid, MFT Triggers it */
            expected_export_flags = 0xAA;
            /* Expected value in checker output */
            expected_checker_value[0] = traffic_pkt_num;
            break;

        case 14:
            /* ftv2_group_flow_checker_test_config = 14 */
            num_checkers = 1;
            checker_pri_param[0] = bcmFlowtrackerTrackingParamTypeIngDropReasonGroupIdVector;
            checker_pri_param_min[0] = 0;
            checker_pri_param_flags[0] = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1 | BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD;
            checker_pri_param_operation[0] = bcmFlowtrackerCheckOpMask;
            checker_pri_num_drop_reason_group[0] = 2;
            checker_pri_drop_reason_group_id[0][0] = ftv2_ingress_dr_id1;
            checker_pri_drop_reason_group_id[0][1] = ftv2_ingress_dr_id2;
            checker_sec_available[0] = 0;
            checker_action_info_available[0] = 1;
            checker_action_param[0] = bcmFlowtrackerTrackingParamTypeNone;
            checker_action[0] = bcmFlowtrackerCheckActionCounterIncr;
            checker_export_info_available[0] = 1;
            checker_export_thresold[0]  = traffic_pkt_num;
            checker_export_action[0] = bcmFlowtrackerCheckOpGreaterEqual;
            /* ALU16 threshold check triggers export */
            expected_export_reason = 0x0040;
            /* IFT,MFT data valid, MFT Triggers it */
            expected_export_flags = 0xAA;
            /* Expected value in checker output */
            expected_checker_value[0] = traffic_pkt_num;
            break;

        case 15:
            /* ftv2_group_flow_checker_test_config = 15 */
            num_checkers = 1;
            checker_pri_param[0] = bcmFlowtrackerTrackingParamTypeTcpWindowSize;
            checker_pri_param_min[0] = (pkt_tcp_window_size & 0xFF00) >> 8;
            checker_pri_param_mask[0] = 0xFF00;
            checker_pri_param_shift[0] = 8;
            checker_pri_param_flags[0] = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1 | BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD;
            checker_pri_param_operation[0] = bcmFlowtrackerCheckOpEqual;
            checker_sec_available[0] = 1;
            checker_sec_param[0] = bcmFlowtrackerTrackingParamTypeDstIPv4;
            checker_sec_param_min[0] = (pkt_dst_ipv4 & 0xFFFF0000) >> 16;
            checker_sec_param_mask[0] = 0xFFFF0000;
            checker_sec_param_shift[0] = 16;
            checker_sec_param_operation[0] = bcmFlowtrackerCheckOpEqual;
            checker_action_info_available[0] = 1;
            checker_action_param[0] = bcmFlowtrackerTrackingParamTypeL4DstPort;
            checker_action[0] = bcmFlowtrackerCheckActionUpdateValue;
            checker_action_mask[0] = 0xFFF0;
            checker_action_shift[0] = 4;
            checker_export_info_available[0] = 1;
            checker_export_thresold[0]  = 1;
            checker_export_action[0] = bcmFlowtrackerCheckOpGreaterEqual;
            /* ALU32 threshold check triggers export */
            expected_export_reason = 0x0020;
            /* All IFT data valid, IFT Triggers it */
            expected_export_flags = 0x82;
            /* Expected value in checker output */
            expected_checker_value[0] = (pkt_l4_dst_port & 0xFFF0) >> 4;
            break;

        case 16:
            /* ftv2_group_flow_checker_test_config = 16 */
            num_checkers = 1;
            checker_pri_param[0] = bcmFlowtrackerTrackingParamTypeEgrPort;
            checker_pri_param_min[0] = traffic_egress_port;
            checker_pri_param_flags[0] = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1 | BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD;
            checker_pri_param_operation[0] = bcmFlowtrackerCheckOpEqual;
            checker_sec_available[0] = 0;
            checker_action_info_available[0] = 1;
            checker_action_param[0] = bcmFlowtrackerTrackingParamTypeNone;
            checker_action[0] = bcmFlowtrackerCheckActionCounterIncr;
            checker_export_info_available[0] = 1;
            checker_export_thresold[0]  = traffic_pkt_num;
            checker_export_action[0] = bcmFlowtrackerCheckOpGreaterEqual;
            /* ALU16 threshold check triggers export */
            expected_export_reason = 0x0040;
            /* All data valid, EFT Triggers it */
            expected_export_flags = 0xF2;
            /* Expected value in checker output */
            expected_checker_value[0] = traffic_pkt_num;
            break;

        case 17:
            /* ftv2_group_flow_checker_test_config = 17 */
            num_checkers = 1;
            checker_pri_param[0] = bcmFlowtrackerTrackingParamTypeEgrDropReasonGroupIdVector;
            checker_pri_param_min[0] = 0;
            checker_pri_param_flags[0] = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1 | BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD;
            checker_pri_param_operation[0] = bcmFlowtrackerCheckOpMask;
            checker_pri_num_drop_reason_group[0] = 1;
            checker_pri_drop_reason_group_id[0][0] = ftv2_egress_dr_id1;
            checker_sec_available[0] = 0;
            checker_action_info_available[0] = 1;
            checker_action_param[0] = bcmFlowtrackerTrackingParamTypeNone;
            checker_action[0] = bcmFlowtrackerCheckActionCounterIncr;
            checker_export_info_available[0] = 1;
            checker_export_thresold[0]  = traffic_pkt_num;
            checker_export_action[0] = bcmFlowtrackerCheckOpGreaterEqual;
            /* ALU16 threshold check triggers export */
            expected_export_reason = 0x0040;
            /* All data valid, EFT Triggers it */
            expected_export_flags = 0xF2;
            /* Expected value in checker output */
            expected_checker_value[0] = traffic_pkt_num;
            break;

        case 80:
            /* ftv2_group_flow_checker_test_config = 80 */
            /* IPv6 TC Config */
            num_checkers = 1;
            for (j = 0; j < num_checkers; j++) {
                checker_pri_param[j] = bcmFlowtrackerTrackingParamTypeHopLimit;
                checker_pri_param_min[j] = 1;
                checker_pri_param_max[j] = 2;
                checker_pri_param_flags[j] = BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD;
                checker_pri_param_operation[j] = bcmFlowtrackerCheckOpInRange;
                checker_sec_available[j] = 0;
                checker_action_info_available[0] = 1;
                checker_action_param[j] = bcmFlowtrackerTrackingParamTypeNone;
                checker_action[j] = bcmFlowtrackerCheckActionCounterIncr;
                checker_export_info_available[0] = 1;
                checker_export_thresold[j]  = 1;
                checker_export_action[j] = bcmFlowtrackerCheckOpGreaterEqual;
            }
            break;

        case 90:
            /* ftv2_group_flow_checker_test_config = 90 */
            /* Vxlan TC Config */
            num_checkers = 2;
            for (j = 0; j < num_checkers; j++) {
                checker_pri_param[j] = bcmFlowtrackerTrackingParamTypeVxlanNetworkId;
                if (j == 0) {
                    checker_pri_param_min[j] = 0x345550;
                    checker_pri_param_max[j] = 0x345770;
                } else if (j == 1) {
                    checker_pri_param_min[j] = 0x365660;
                    checker_pri_param_max[j] = 0x365670;
                }
                checker_pri_param_flags[j] = BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD;
                checker_pri_param_operation[j] = bcmFlowtrackerCheckOpInRange;
                checker_sec_available[j] = 0;
                checker_action_info_available[0] = 1;
                checker_action_param[j] = bcmFlowtrackerTrackingParamTypeNone;
                checker_action[j] = bcmFlowtrackerCheckActionCounterIncr;
                checker_export_info_available[0] = 1;
                checker_export_thresold[j]  = 0;
                checker_export_action[j] = bcmFlowtrackerCheckOpNone;
            }
            break;

        default:
            printf("[CINT] TC %2d - invalid checker test config %d\n",tc_id, ftv2_group_flow_checker_test_config);
            return BCM_E_FAIL;
    }

    /* Create checkers as per config set above */
    for (iter = 0; iter < num_checkers; iter++) {

        /* Create Primary Checker First */
        bcm_flowtracker_check_info_t_init(&check_info1);
        check_info1.param = checker_pri_param[iter];
        check_info1.min_value = checker_pri_param_min[iter];
        check_info1.max_value = checker_pri_param_max[iter];
        check_info1.mask_value = checker_pri_param_mask[iter];
        check_info1.shift_value = checker_pri_param_shift[iter];
        check_info1.flags = checker_pri_param_flags[iter];
        check_info1.operation = checker_pri_param_operation[iter];
        if (checker_pri_num_drop_reason_group[iter] != 0) {
            check_info1.num_drop_reason_group_id = checker_pri_num_drop_reason_group[iter];
            for (iter2 = 0; iter2 < checker_pri_num_drop_reason_group[iter]; iter2++) {
                check_info1.drop_reason_group_id[iter2] = checker_pri_drop_reason_group_id[iter][iter2];
            }
        }
        rv = bcm_flowtracker_check_create(unit, options_check1, check_info1, &pri_check_id1[iter]);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - Primary bcm_flowtracker_check_create => rv %d(%s)\n",
                                                              tc_id, rv, bcm_errmsg(rv));
            return rv;
        }
        actual_num_checkers_configured++;

        /* Create Secondary Checker if needed */
        if (checker_sec_available[iter]) {
            bcm_flowtracker_check_info_t_init(&check_info1);
            check_info1.param = checker_sec_param[iter];
            check_info1.min_value = checker_sec_param_min[iter];
            check_info1.max_value = checker_sec_param_max[iter];
            check_info1.mask_value = checker_sec_param_mask[iter];
            check_info1.shift_value = checker_sec_param_shift[iter];
            check_info1.flags = checker_sec_param_flags[iter];
            check_info1.operation = checker_sec_param_operation[iter];
            check_info1.primary_check_id = pri_check_id1[iter];
            if (checker_sec_num_drop_reason_group[iter] != 0) {
                check_info1.num_drop_reason_group_id = checker_sec_num_drop_reason_group[iter];
                for (iter2 = 0; iter2 < checker_sec_num_drop_reason_group[iter]; iter2++) {
                    check_info1.drop_reason_group_id[iter2] = checker_sec_drop_reason_group_id[iter][iter2];
                }
            }
            rv = bcm_flowtracker_check_create(unit, options_check2, check_info1, &sec_check_id1[iter]);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - Secondary bcm_flowtracker_check_create => rv %d(%s)\n",
                                                                    tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
        }

        /* Set action info on primary checker created */
        if (checker_action_info_available[iter]) {
            bcm_flowtracker_check_action_info_t_init(&check_action_info1);
            check_action_info1.param = checker_action_param[iter];
            check_action_info1.action = checker_action[iter];
            check_action_info1.mask_value = checker_action_mask[iter];
            check_action_info1.shift_value = checker_action_shift[iter];
            rv = bcm_flowtracker_check_action_info_set(unit, pri_check_id1[iter], check_action_info1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_flowtracker_check_action_info_set => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
        }

        /* Set Export info on primary checker created */
        if (checker_export_info_available[iter]) {
            bcm_flowtracker_check_export_info_t_init(&check_export_info1);
            check_export_info1.export_check_threshold = checker_export_thresold[iter];
            check_export_info1.operation = checker_export_action[iter];
            rv = bcm_flowtracker_check_export_info_set(unit, pri_check_id1[iter], check_export_info1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_flowtracker_check_export_info_set => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
        }

        /* Set delay info on primary checker created */
        if (checker_delay_info_available[iter]) {
            bcm_flowtracker_check_delay_info_t_init(&check_delay_info1);
            check_delay_info1.src_ts = checker_src_ts[iter];
            check_delay_info1.dst_ts = checker_dst_ts[iter];
            check_delay_info1.gran = checker_delay_gran[iter];
            check_delay_info1.offset = checker_delay_offset[iter];
            rv = bcm_flowtracker_check_delay_info_set(unit, pri_check_id1[iter], check_delay_info1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_flowtracker_check_delay_info_set => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
        }

        /* Add Flow Checker In FT Group */
        rv = bcm_flowtracker_group_check_add(unit, flow_group_id1, pri_check_id1[iter]);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - bcm_flowtracker_group_check_add => rv %d(%s)\n",
                                                          tc_id, rv, bcm_errmsg(rv));
            return rv;
        }
        if (!skip_log) {
            printf("[CINT] Step13 - Created & Added Primary Flow checker with id: 0x%08x in FT group 0x%08x\n",
                                                                                pri_check_id1[iter],flow_group_id1);

            if (checker_sec_available[iter]) {
                printf("[CINT] Step13 - Added Secondary Flow checker with id: 0x%08x also.\n",sec_check_id1[iter]);
            }
        }
        bshell(unit, "modreg vxlan_control UDP_DEST_PORT=8472");
    }

    /* Final Step Log */
    if (!skip_log) {
        printf ("[CINT] Step13 - Flow Checkers Added Succesfully.\n");
    }

    return BCM_E_NONE;
}

/* Destroy check */
int
ftv2_check_destroy_main(uint32 tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;
    int iter = 0, num_checkers = 0;

    /* Skip this flow check destroy if no test config */
    if (!ftv2_group_flow_checker_test_config) {
        if (!skip_log) {
            printf("[CINT] Cleanup - Skip Flow Check Destroy ......\n");
        }
        return rv;
    }

    num_checkers = actual_num_checkers_configured;

    /* Destroy Flow Checkers as per config above */
    for (iter = 0; iter < num_checkers; iter++) {

        /* Remove Flow Checker from FT Group */
        rv = bcm_flowtracker_group_check_delete(unit, flow_group_id1, pri_check_id1[iter]);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - bcm_flowtracker_group_check_delete => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
            return rv;
        }

        /* Destroy Secondary Checker First */
        if (sec_check_id1[iter] != 0) {
            rv = bcm_flowtracker_check_destroy(unit, sec_check_id1[iter]);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - Secondary bcm_flowtracker_check_destroy => rv %d(%s)\n",
                                                                     tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Cleanup - Destroyed Secondaray Flow checker with id: 0x%08x\n",
                                                                        sec_check_id1[iter]);
            }
            sec_check_id1[iter] = 0;
        }

        /* Destroy Primary Checker Next */
        rv = bcm_flowtracker_check_destroy(unit, pri_check_id1[iter]);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - Primary bcm_flowtracker_check_destroy => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
            return rv;
        }
        if (!skip_log) {
            printf("[CINT] Cleanup - Destroyed Primary Flow checker with id: 0x%08x\n",pri_check_id1[iter]);
        }
        pri_check_id1[iter] = 0;
        actual_num_checkers_configured--;
        expected_checker_value[iter] = 0;
    }

    /* Final Step Log */
    if (!skip_log) {
        printf ("[CINT] Cleanup - Flow Checkers Destroyed Succesfully.\n");
    }

    return BCM_E_NONE;
}
