/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * C file to put Monitoring Thread related api calls
 */

/* Global variables for monitoring related api */
int rx_task_active = 0;

/* Global received packet count */
int rx_count = 0;

/* Packet handled by rmon */
uint8 packet_handled_by_watcher = 0;

/* By default set to 1 to fail tc if export packet is not
 * received by utility */
uint8 packet_validation_failed = 1;

int
ftv2_validate_export_element(bcm_flowtracker_export_element_type_t exp_element, uint32 exp_element_value,
                             uint8 *exp_element_array_value, bcm_flowtracker_check_t export_check_id)
{
    int rv = BCM_E_NONE;
    int i = 0;

    switch(exp_element) {
        case bcmFlowtrackerExportElementTypeFlowtrackerGroup:
            if (flow_group_id1 != exp_element_value) {
                printf("\n[CINT] Monitoring - Export Element Flowtracker Group expected value 0x%08x vs received value in export packet 0x%08x\n",
                        flow_group_id1, exp_element_value);
                rv = BCM_E_FAIL;
            }
            break;
        case bcmFlowtrackerExportElementTypeIngPort:
            if (traffic_ingress_port != exp_element_value) {
                printf("\n[CINT] Monitoring - Export Element Ingress Port expected value 0x%08x vs received value in export packet 0x%08x\n",
                        traffic_ingress_port, exp_element_value);
                rv = BCM_E_FAIL;
            }
            break;
        case bcmFlowtrackerExportElementTypeEgrPort:
            if (traffic_egress_port != exp_element_value) {
                printf("\n[CINT] Monitoring - Export Element Egress Port expected value 0x%08x vs received value in export packet 0x%08x\n",
                        traffic_egress_port, exp_element_value);
                rv = BCM_E_FAIL;
            }
            break;
        case bcmFlowtrackerExportElementTypeSrcIPv4:
            if (pkt_src_ipv4 != exp_element_value) {
                printf("\n[CINT] Monitoring - Export Element SrcIpv4 expected value 0x%08x vs received value in export packet 0x%08x\n",
                                                                                                    pkt_src_ipv4, exp_element_value);
                rv = BCM_E_FAIL;
            }
            break;
        case bcmFlowtrackerExportElementTypeDstIPv4:
            if (pkt_dst_ipv4 != exp_element_value) {
                printf("\n[CINT] Monitoring - Export Element DstIpv4 expected value 0x%08x vs received value in export packet 0x%08x\n",
                        pkt_dst_ipv4, exp_element_value);
                rv = BCM_E_FAIL;
            }
            break;
        case bcmFlowtrackerExportElementTypeL4SrcPort:
            if (pkt_l4_src_port != exp_element_value) {
                printf("\n[CINT] Monitoring - Export Element L4 Src Port expected value 0x%08x vs received value in export packet 0x%08x\n",
                        pkt_l4_src_port, exp_element_value);
                rv = BCM_E_FAIL;
            }
            break;
        case bcmFlowtrackerExportElementTypeL4DstPort:
            if (pkt_l4_dst_port != exp_element_value) {
                printf("\n[CINT] Monitoring - Export Element L4 Dst Port expected value 0x%08x vs received value in export packet 0x%08x\n",
                        pkt_l4_dst_port, exp_element_value);
                rv = BCM_E_FAIL;
            }
            break;
        case bcmFlowtrackerExportElementTypeIPProtocol:
            if (pkt_protocol != exp_element_value) {
                printf("\n[CINT] Monitoring - Export Element IP Protocol expected value 0x%08x vs received value in export packet 0x%08x\n",
                        pkt_protocol, exp_element_value);
                rv = BCM_E_FAIL;
            }
            break;
        case bcmFlowtrackerExportElementTypeTTL:
            if (pkt_ttl != exp_element_value) {
                printf("\n[CINT] Monitoring - Export Element TTL expected value 0x%08x vs received value in export packet 0x%08x\n",
                        pkt_ttl, exp_element_value);
                rv = BCM_E_FAIL;
            }
            break;
        case bcmFlowtrackerExportElementTypeTcpWindowSize:
            if (pkt_tcp_window_size != exp_element_value) {
                printf("\n[CINT] Monitoring - Export Element TCP Window Size expected value 0x%08x vs received value in export packet 0x%08x\n",
                        pkt_tcp_window_size, exp_element_value);
                rv = BCM_E_FAIL;
            }
            break;
        case bcmFlowtrackerExportElementTypePktCount:
            if (expected_tparam_pkt_count_value != exp_element_value) {
                printf("\n[CINT] Monitoring - Export Element PktCount expected value 0x%08x vs received value in export packet 0x%08x\n",
                        expected_tparam_pkt_count_value, exp_element_value);
                rv = BCM_E_FAIL;
            }
            break;
        case bcmFlowtrackerExportElementTypeByteCount:
            if (expected_tparam_byte_count_value != exp_element_value) {
                printf("\n[CINT] Monitoring - Export Element Byte Count expected value 0x%08x vs received value in export packet 0x%08x\n",
                        expected_tparam_byte_count_value, exp_element_value);
                rv = BCM_E_FAIL;
            }
            break;
        case bcmFlowtrackerExportElementTypeChipDelay:
            if (expected_tparam_chip_delay_value != exp_element_value) {
                printf("\n[CINT] Monitoring - Export Element Chip Delay expected value 0x%08x vs received value in export packet 0x%08x\n",
                        expected_tparam_chip_delay_value, exp_element_value);
                rv = BCM_E_FAIL;
            }
            break;
        case bcmFlowtrackerExportElementTypeIPATDelay:
            if (expected_tparam_ipat_delay_value != exp_element_value) {
                printf("\n[CINT] Monitoring - Export Element IPAT Delay expected value 0x%08x vs received value in export packet 0x%08x\n",
                                                                                      expected_tparam_ipat_delay_value, exp_element_value);
                rv = BCM_E_FAIL;
            }
            break;
        case bcmFlowtrackerExportElementTypeIPDTDelay:
            if (expected_tparam_ipdt_delay_value != exp_element_value) {
                printf("\n[CINT] Monitoring - Export Element IPDT Delay expected value 0x%08x vs received value in export packet 0x%08x\n",
                        expected_tparam_ipdt_delay_value, exp_element_value);
                rv = BCM_E_FAIL;
            }
            break;
        case bcmFlowtrackerExportElementTypeIngDropReasonGroupIdVector:
            if (expected_ing_drop_vector_value != exp_element_value) {
                printf("\n[CINT] Monitoring - Export Element IngDropReasonGroupIdVector expected value 0x%08x vs received value in export packet 0x%08x\n",
                        expected_ing_drop_vector_value, exp_element_value);
                rv = BCM_E_FAIL;
            }
            break;
        case bcmFlowtrackerExportElementTypeEgrDropReasonGroupIdVector:
            if (expected_egr_drop_vector_value != exp_element_value) {
                printf("\n[CINT] Monitoring - Export Element EgrDropReasonGroupIdVector expected value 0x%08x vs received value in export packet 0x%08x\n",
                        expected_egr_drop_vector_value, exp_element_value);
                rv = BCM_E_FAIL;
            }
            break;
        case bcmFlowtrackerExportElementTypeFlowtrackerCheck:
            if (export_check_id != 0) {
                for (i = 0; i < actual_num_checkers_configured; i++) {
                    if (export_check_id == pri_check_id1[i]) {
                        if (expected_checker_value[i] != exp_element_value) {
                            printf("\n[CINT] Monitoring - Export Element Checker 0x%08x - expected value 0x%08x vs received value 0x%08x\n",
                                                                            export_check_id, expected_checker_value[i], exp_element_value);
                            rv = BCM_E_FAIL;
                        }
                        break;
                    }
                }
            }
            break;
        case bcmFlowtrackerExportElementTypeTimestampCheckEvent1:
            if ((expected_tod_value[0] != exp_element_array_value[0]) ||
                (expected_tod_value[1] != exp_element_array_value[1]) ||
                (expected_tod_value[2] != exp_element_array_value[2]) ||
                (expected_tod_value[3] != exp_element_array_value[3]) ||
                (expected_tod_value[4] != exp_element_array_value[4]) ||
                (expected_tod_value[5] != exp_element_array_value[5])) {
                printf("\n[CINT] Monitoring - Export Element Timestamp CheckEvent1 ");
                printf("expected value 0x%02x%02x%02x%02x%02x%02x vs ",
                        expected_tod_value[0], expected_tod_value[1],
                        expected_tod_value[2], expected_tod_value[3],
                        expected_tod_value[4], expected_tod_value[5]);
                printf("received value in export packet 0x%02x%02x%02x%02x%02x%02x\n",
                        exp_element_array_value[0], exp_element_array_value[1],
                        exp_element_array_value[2], exp_element_array_value[3],
                        exp_element_array_value[4], exp_element_array_value[5]);
                rv = BCM_E_FAIL;
            }
            break;
        case bcmFlowtrackerExportElementTypeTimestampCheckEvent2:
            if ((expected_tod_value[0] != exp_element_array_value[0]) ||
                (expected_tod_value[1] != exp_element_array_value[1]) ||
                (expected_tod_value[2] != exp_element_array_value[2]) ||
                (expected_tod_value[3] != exp_element_array_value[3]) ||
                (expected_tod_value[4] != exp_element_array_value[4]) ||
                (expected_tod_value[5] != exp_element_array_value[5])) {
                printf("\n[CINT] Monitoring - Export Element Timestamp CheckEvent2 ");
                printf("expected value 0x%02x%02x%02x%02x%02x%02x vs ",
                        expected_tod_value[0], expected_tod_value[1],
                        expected_tod_value[2], expected_tod_value[3],
                        expected_tod_value[4], expected_tod_value[5]);
                printf("received value in export packet 0x%02x%02x%02x%02x%02x%02x\n",
                        exp_element_array_value[0], exp_element_array_value[1],
                        exp_element_array_value[2], exp_element_array_value[3],
                        exp_element_array_value[4], exp_element_array_value[5]);
                rv = BCM_E_FAIL;
            }
            break;
        case bcmFlowtrackerExportElementTypeTimestampCheckEvent3:
            if ((expected_tod_value[0] != exp_element_array_value[0]) ||
                (expected_tod_value[1] != exp_element_array_value[1]) ||
                (expected_tod_value[2] != exp_element_array_value[2]) ||
                (expected_tod_value[3] != exp_element_array_value[3]) ||
                (expected_tod_value[4] != exp_element_array_value[4]) ||
                (expected_tod_value[5] != exp_element_array_value[5])) {
                printf("\n[CINT] Monitoring - Export Element Timestamp CheckEvent3 ");
                printf("expected value 0x%02x%02x%02x%02x%02x%02x vs ",
                        expected_tod_value[0], expected_tod_value[1],
                        expected_tod_value[2], expected_tod_value[3],
                        expected_tod_value[4], expected_tod_value[5]);
                printf("received value in export packet 0x%02x%02x%02x%02x%02x%02x\n",
                        exp_element_array_value[0], exp_element_array_value[1],
                        exp_element_array_value[2], exp_element_array_value[3],
                        exp_element_array_value[4], exp_element_array_value[5]);
                rv = BCM_E_FAIL;
            }
            break;
        case bcmFlowtrackerExportElementTypeTimestampCheckEvent4:
            if ((expected_tod_value[0] != exp_element_array_value[0]) ||
                (expected_tod_value[1] != exp_element_array_value[1]) ||
                (expected_tod_value[2] != exp_element_array_value[2]) ||
                (expected_tod_value[3] != exp_element_array_value[3]) ||
                (expected_tod_value[4] != exp_element_array_value[4]) ||
                (expected_tod_value[5] != exp_element_array_value[5])) {
                printf("\n[CINT] Monitoring - Export Element Timestamp CheckEvent4 ");
                printf("expected value 0x%02x%02x%02x%02x%02x%02x vs ",
                        expected_tod_value[0], expected_tod_value[1],
                        expected_tod_value[2], expected_tod_value[3],
                        expected_tod_value[4], expected_tod_value[5]);
                printf("received value in export packet 0x%02x%02x%02x%02x%02x%02x\n",
                        exp_element_array_value[0], exp_element_array_value[1],
                        exp_element_array_value[2], exp_element_array_value[3],
                        exp_element_array_value[4], exp_element_array_value[5]);
                rv = BCM_E_FAIL;
            }
            break;
        case bcmFlowtrackerExportElementTypeTimestampNewLearn:
            if ((expected_tod_value[0] != exp_element_array_value[0]) ||
                (expected_tod_value[1] != exp_element_array_value[1]) ||
                (expected_tod_value[2] != exp_element_array_value[2]) ||
                (expected_tod_value[3] != exp_element_array_value[3]) ||
                (expected_tod_value[4] != exp_element_array_value[4]) ||
                (expected_tod_value[5] != exp_element_array_value[5])) {
                printf("\n[CINT] Monitoring - Export Element Timestamp NewLearn ");
                printf("expected value 0x%02x%02x%02x%02x%02x%02x vs ",
                        expected_tod_value[0], expected_tod_value[1],
                        expected_tod_value[2], expected_tod_value[3],
                        expected_tod_value[4], expected_tod_value[5]);
                printf("received value in export packet 0x%02x%02x%02x%02x%02x%02x\n",
                        exp_element_array_value[0], exp_element_array_value[1],
                        exp_element_array_value[2], exp_element_array_value[3],
                        exp_element_array_value[4], exp_element_array_value[5]);
                rv = BCM_E_FAIL;
            }
            break;

        case bcmFlowtrackerExportElementTypeTimestampFlowStart:
            if ((expected_tod_value[0] != exp_element_array_value[0]) ||
                (expected_tod_value[1] != exp_element_array_value[1]) ||
                (expected_tod_value[2] != exp_element_array_value[2]) ||
                (expected_tod_value[3] != exp_element_array_value[3]) ||
                (expected_tod_value[4] != exp_element_array_value[4]) ||
                (expected_tod_value[5] != exp_element_array_value[5])) {
                printf("\n[CINT] Monitoring - Export Element Timestamp FlowStart");
                printf("expected value 0x%02x%02x%02x%02x%02x%02x vs ",
                        expected_tod_value[0], expected_tod_value[1],
                        expected_tod_value[2], expected_tod_value[3],
                        expected_tod_value[4], expected_tod_value[5]);
                printf("received value in export packet 0x%02x%02x%02x%02x%02x%02x\n",
                        exp_element_array_value[0], exp_element_array_value[1],
                        exp_element_array_value[2], exp_element_array_value[3],
                        exp_element_array_value[4], exp_element_array_value[5]);
                rv = BCM_E_FAIL;
            }
            break;

        case bcmFlowtrackerExportElementTypeTimestampFlowEnd:
            if ((expected_tod_value[0] != exp_element_array_value[0]) ||
                (expected_tod_value[1] != exp_element_array_value[1]) ||
                (expected_tod_value[2] != exp_element_array_value[2]) ||
                (expected_tod_value[3] != exp_element_array_value[3]) ||
                (expected_tod_value[4] != exp_element_array_value[4]) ||
                (expected_tod_value[5] != exp_element_array_value[5])) {
                printf("\n[CINT] Monitoring - Export Element Timestamp FlowEnd");
                printf("expected value 0x%02x%02x%02x%02x%02x%02x vs ",
                        expected_tod_value[0], expected_tod_value[1],
                        expected_tod_value[2], expected_tod_value[3],
                        expected_tod_value[4], expected_tod_value[5]);
                printf("received value in export packet 0x%02x%02x%02x%02x%02x%02x\n",
                        exp_element_array_value[0], exp_element_array_value[1],
                        exp_element_array_value[2], exp_element_array_value[3],
                        exp_element_array_value[4], exp_element_array_value[5]);
                rv = BCM_E_FAIL;
            }
            break;

        default:
            rv = BCM_E_NONE;
    }

    return rv;
}

void
packet_data_parse_without_dump(bcm_pkt_t * pkt)
{
    int rv = BCM_E_NONE;
    int i = 0, start_idx = 0;
    uint8 *pkt_data = NULL;
    uint16 tmp16 = 0;
    int cur_byte = 0;
    int elem_size = 0;
    uint32 pkt_len = 0;
    int exp_element = 0;
    int actual_export_element_get = 0;
    int actual_export_element_value = 0;
    uint8 actual_export_element_array_value[16] = { 0 };
    char *export_element_string = NULL;
    int max_export_element_get = FTE_MAX_INFO_ELEMENTS;
    bcm_flowtracker_export_element_info_t export_elem_get_op[FTE_MAX_INFO_ELEMENTS];

    pkt_len = pkt->pkt_len;
    pkt_data = &pkt->pkt_data->data[0];
    packet_validation_failed = 0;

    /* Initialise export elements */
    for (i = 0; i < FTE_MAX_INFO_ELEMENTS; i++) {
        export_elem_get_op[i].data_size = 0;
        export_elem_get_op[i].element = bcmFlowtrackerExportElementTypeReserved;
    }

    /* L2 Header(18B) */
    tmp16 = (pkt_data[16] << 8) | (pkt_data[17] << 0);
    if (tmp16 != 0x0800) {
        packet_validation_failed = 1;
        printf("\n[CINT] Monitoring - Packet Received is not IPv4, packet parsing failed.\n");
        return;
    }

    /* IPv4 Header(20B) */
    if (pkt_data[27] != 17) {
        packet_validation_failed = 1;
        printf("\n[CINT] Monitoring - Packet Received is IPv4 but not UDP, packet parsing failed.\n");
        return;
    }

    /* Flow Group Id(2B) */
    tmp16 = (pkt_data[68] << 8) | (pkt_data[69] << 0);
    tmp16 = tmp16 & 0xFFF;
    if (tmp16 != flow_group_id1) {
        packet_validation_failed = 1;
        printf("\n[CINT] Monitoring - FT Group ID Mismatch ! Received Group Id 0x%04x vs Expected 0x%04x\n",
                                                                                    tmp16, flow_group_id1);
        return;
    }

    /* Export Reason(2B) */
    tmp16 = (pkt_data[70] << 8) | (pkt_data[71] << 0);
    if (tmp16 != expected_export_reason) {
        packet_validation_failed = 1;
        printf("\n[CINT] Monitoring - FT Export Reason does not match !, Received Export Reason 0x%04x vs Expected 0x%04x\n",
                                                                                             tmp16, expected_export_reason);
        return;
    }

    /* Export Flags(1B) */
    tmp16 = ((pkt_data[72] & 0x18) >> 3);
    if (tmp16 == 0) {
    } else if (tmp16 == 1) {
    } else if (tmp16 == 2) {
    } else {
        packet_validation_failed = 1;
        printf("\n[CINT] Monitoring - Invalid Trigger Source\n");
        return;
    }
    tmp16 = pkt_data[72];
    if (tmp16 != expected_export_flags) {
        packet_validation_failed = 1;
        printf("\n[CINT] Monitoring - FT Export Flags does not match !, Received Export Flags 0x%02x vs Expected 0x%02x\n",
                tmp16, expected_export_flags);
        return;
    }

    /* Export Timestamp(6B) */
    if ((expected_export_tod_value[0] != pkt_data[73]) ||
        (expected_export_tod_value[1] != pkt_data[74]) ||
        (expected_export_tod_value[2] != pkt_data[75]) ||
        (expected_export_tod_value[3] != pkt_data[76]) ||
        (expected_export_tod_value[4] != pkt_data[77]) ||
        (expected_export_tod_value[5] != pkt_data[78])) {
        packet_validation_failed = 1;
        printf("\n[CINT] Monitoring - FT Export Timestamp does not match.\n");
        printf("expected value 0x%02x%02x%02x%02x%02x%02x vs ",
                expected_export_tod_value[0], expected_export_tod_value[1],
                expected_export_tod_value[2], expected_export_tod_value[3],
                expected_export_tod_value[4], expected_export_tod_value[5]);
        printf("received value in export packet 0x%02x%02x%02x%02x%02x%02x\n",
                pkt_data[73], pkt_data[74], pkt_data[75],
                pkt_data[76], pkt_data[77], pkt_data[78]);
        return;
    }

    /* Dump as per export element list after setid */
    rv = bcm_flowtracker_export_template_get(unit, template_id1, fte_set_id,
                                             max_export_element_get,
                                             export_elem_get_op,
                                             &actual_export_element_get);
    if (BCM_FAILURE(rv)) {
        packet_validation_failed = 1;
        printf("\n[CINT] Monitoring - bcm_flowtracker_export_template_get => rv %d(%s)\n",
                                                                    rv, bcm_errmsg(rv));
        return;
    }

    if (actual_export_element_get > 0) {
        /* Export Elements start from byte 66 in ipfix packet received */
        cur_byte = 66;
        for (i = 0; i < actual_export_element_get; i++) {

            /* Get current export element */
            exp_element = export_elem_get_op[i].element;
            export_element_string = ftv2_convert_export_element_to_string(exp_element);

            /* Get current export element value */
            actual_export_element_value = 0;
            for (elem_size = 0; elem_size < export_elem_get_op[i].data_size; elem_size++) {
                /* Capture value of element if size is upto 4Bytes */
                if (export_elem_get_op[i].data_size <= 4) {
                    actual_export_element_value |=
                        pkt_data[cur_byte + elem_size] <<
                        ((export_elem_get_op[i].data_size - 1 - elem_size) * 8);
                }
                if ((export_elem_get_op[i].data_size > 4) &&
                    (export_elem_get_op[i].data_size < 17)) {
                    actual_export_element_array_value[elem_size] =
                                                pkt_data[cur_byte + elem_size];
                }
            }
            /* Updating Flowtracker Group element with relevent bits */
            if (export_elem_get_op[i].element == bcmFlowtrackerExportElementTypeFlowtrackerGroup) {
                actual_export_element_value = actual_export_element_value & 0xFFF;
            }

            rv = ftv2_validate_export_element(export_elem_get_op[i].element, actual_export_element_value,
                                              actual_export_element_array_value, export_elem_get_op[i].check_id);
            if (BCM_FAILURE(rv)) {
                packet_validation_failed = 1;
                printf("\n[CINT] Monitoring - ftv2_validate_export_element failed for export element <%s> => rv %d(%s)\n",
                        export_element_string, rv, bcm_errmsg(rv));
                return;
            }

            /* Move to next export element in template */
            cur_byte += export_elem_get_op[i].data_size;
        }
    }
}

void
packet_data_parse_with_dump(bcm_pkt_t * pkt)
{
    int rv = BCM_E_NONE;
    int i = 0, start_idx = 0;
    uint8 *pkt_data = NULL;
    uint16 tmp16 = 0, tmp16_2 = 0;
    int cur_byte = 0;
    int exp_element = 0;
    int elem_size = 0;
    uint32 tmp32 = 0, pkt_len = 0;
    int actual_export_element_get = 0;
    int actual_export_element_value = 0;
    uint8 actual_export_element_array_value[16] = { 0 };
    char *export_element_string = NULL;
    int max_export_element_get = FTE_MAX_INFO_ELEMENTS;
    bcm_flowtracker_export_element_info_t export_elem_get_op[FTE_MAX_INFO_ELEMENTS];

    pkt_len = pkt->pkt_len;
    pkt_data = &pkt->pkt_data->data[0];
    packet_validation_failed = 0;

    /* Initialise export elements */
    for (i = 0; i < FTE_MAX_INFO_ELEMENTS; i++) {
        export_elem_get_op[i].data_size = 0;
        export_elem_get_op[i].element = bcmFlowtrackerExportElementTypeReserved;
    }

    printf(":::::::::: Parsing Packet ::::::::::\n");
    /* L2 Header(18B) */
    printf("L2 Header(18B):\n");
    printf("\tDMAC    - %02x:%02x:%02x:%02x:%02x:%02x\n",
                    pkt_data[0],pkt_data[1],pkt_data[2],
                    pkt_data[3],pkt_data[4],pkt_data[5]);
    printf("\tSMAC    - %02x:%02x:%02x:%02x:%02x:%02x\n",
                    pkt_data[6],pkt_data[7],pkt_data[8],
                    pkt_data[9],pkt_data[10],pkt_data[11]);
    tmp32 = (pkt_data[12] << 24) | (pkt_data[13] << 16) | (pkt_data[14] << 8) | (pkt_data[15] << 0);
    printf("\tVlanTag - 0x%08x(Vlan %d)\n", tmp32, (tmp32 & 0x00000FFF));
    tmp16 = (pkt_data[16] << 8) | (pkt_data[17] << 0);
    printf("\tType    - 0x%04x\n",tmp16);
    if (tmp16 != 0x0800) {
        packet_validation_failed = 1;
        printf("\n[CINT] Monitoring - Packet Received is not IPv4, packet parsing failed.\n");
        return;
    }

    /* IPv4 Header(20B) */
    printf("IPv4 Header(20B):\n");
    printf("\tVersion(%d), Header Length(%d), ToS(0x%02x)\n",
                            (pkt_data[18] & 0xF0),(pkt_data[18] & 0x0F), pkt_data[19]);
    tmp16 = (pkt_data[20] << 8) | (pkt_data[21] << 0);
    printf("\tLength %d(0x%04x), ",tmp16,tmp16);
    tmp16 = (pkt_data[22] << 8) | (pkt_data[23] << 0);
    printf("Identication 0x%04x\n",tmp16);
    tmp16 = (pkt_data[24] << 8) | (pkt_data[25] << 0);
    printf("\tFlags 0x%x, Fragment Offset 0x%x\n",(tmp16 & 0xE000), (tmp16 & 0x1FFF));
    printf("\tTTL %d(0x%02x), Protocol %d(0x%02x), HeaderChecksum 0x%02x%02x\n",pkt_data[26],pkt_data[26],
                                                    pkt_data[27],pkt_data[27],pkt_data[28],pkt_data[29]);
    printf("\tSrcIp: %d.%d.%d.%d(0x%02x.%02x.%02x.%02x)\n",
                             pkt_data[30],pkt_data[31],pkt_data[32],pkt_data[33],
                             pkt_data[30],pkt_data[31],pkt_data[32],pkt_data[33]);
    printf("\tDstIp: %d.%d.%d.%d(0x%02x.%02x.%02x.%02x)\n",
                             pkt_data[34],pkt_data[35],pkt_data[36],pkt_data[37],
                             pkt_data[34],pkt_data[35],pkt_data[36],pkt_data[37]);
    if (pkt_data[27] != 17) {
        packet_validation_failed = 1;
        printf("\n[CINT] Monitoring - Packet Received is IPv4 but not UDP, packet parsing failed.\n");
        return;
    }

    /* UDP Header(8B) */
    printf("UDP Header(8B):\n");
    tmp16 = (pkt_data[38] << 8) | (pkt_data[39] << 0);
    printf("\tSrcPort %d(0x%04x),",tmp16,tmp16);
    tmp16 = (pkt_data[40] << 8) | (pkt_data[41] << 0);
    printf(" DstPort %d(0x%04x),",tmp16,tmp16);
    tmp16 = (pkt_data[42] << 8) | (pkt_data[43] << 0);
    printf(" Length %d(0x%04x),",tmp16,tmp16);
    tmp16 = (pkt_data[44] << 8) | (pkt_data[45] << 0);
    printf(" Checksum 0x%04x\n",tmp16);

    /* IPFIX Header(16B) */
    printf("IPFIX Header(16B): \n");
    printf("\t");
    for (i = 0; i < 16; i++) {
        printf("%02x ",pkt_data[i+46]);
    }
    printf("\n");

    /* Set Header(4B) */
    printf("Set Header(4B):\n");
    tmp16 = (pkt_data[62] << 8) | (pkt_data[63] << 0);
    printf("\tID %d(0x%04x),",tmp16, tmp16);
    tmp16 = (pkt_data[64] << 8) | (pkt_data[65] << 0);
    printf(" Length %d(0x%04x)\n",tmp16,tmp16);

    /* Flow Index(2B) */
    tmp16 = (pkt_data[66] << 8) | (pkt_data[67] << 0);

    /* Flow Group Id(2B) */
    tmp16_2 = (pkt_data[68] << 8) | (pkt_data[69] << 0);

    tmp16 = (tmp16 << 4) + (tmp16_2 >> 12);
    tmp16_2 = tmp16_2 & 0xFFF;
    printf("\tFlow Index: %d(0x%04x)\n",tmp16,tmp16);
    printf("\tFT Group Id: %d(0x%04x)\n",tmp16_2,tmp16_2);
    if (tmp16_2 != flow_group_id1) {
        packet_validation_failed = 1;
        printf("\n[CINT] Monitoring - FT Group ID Mismatch ! Received Group Id 0x%04x vs Expected 0x%04x\n",
                                                                                    tmp16_2, flow_group_id1);
        return;
    }

    /* Export Reason(2B) */
    tmp16 = (pkt_data[70] << 8) | (pkt_data[71] << 0);
    printf("\tExport Reason: 0x%04x (",tmp16);
    if (tmp16 & 0x0001) {
        printf("Periodic, ");
    }
    if (tmp16 & 0x0002) {
        printf("Group Flush, ");
    }
    if (tmp16 & 0x0004) {
        printf("Age Out, ");
    }
    if (tmp16 & 0x0008) {
        printf("New Flow Learned, ");
    }
    if (tmp16 & 0x0010) {
        printf("State Transition, ");
    }
    if (tmp16 & 0x0020) {
        printf("ALU32 Thresold Check, ");
    }
    if (tmp16 & 0x0040) {
        printf("ALU16 Thresold Check, ");
    }
    printf(")\n");
    if (tmp16 != expected_export_reason) {
        packet_validation_failed = 1;
        printf("\n[CINT] Monitoring - FT Export Reason does not match !, Received Export Reason 0x%04x vs Expected 0x%04x\n",
                                                                                            tmp16, expected_export_reason);
        return;
    }

    /* Export Flag(1B) */
    printf("\tExport Flag: 0x%02x\n",pkt_data[72]);
    if (pkt_data[72] & 0x01) {
        printf("\t  Key Direction 1, ");
    } else {
        printf("\t  Key Direction 0, ");
    }
    printf("Version %d, ",((pkt_data[72] & 0x06) >> 1));
    tmp16 = ((pkt_data[72] & 0x18) >> 3);
    printf("Trigger Source %d(",tmp16);
    if (tmp16 == 0) {
        printf("IFT");
    } else if (tmp16 == 1) {
        printf("MFT");
    } else if (tmp16 == 2) {
        printf("EFT");
    } else {
        printf("Invalid)\n");
        printf("\n[CINT] Monitoring - Invalid Trigger Source\n");
        return;
    }
    printf(")\n");
    if ((pkt_data[72] & 0x20) >> 5) {
        printf("\t  MFT Export-Valid, ");
    } else {
        printf("\t  MFT Export-Invalid, ");
    }
    if ((pkt_data[72] & 0x40) >> 6) {
        printf("EFT Export-Valid, ");
    } else {
        printf("EFT Export-Invalid, ");
    }
    if ((pkt_data[72] & 0x80) >> 7) {
        printf("IFT Export-Valid");
    } else {
        printf("IFT Export-Invalid");
    }
    printf("\n");
    tmp16 = pkt_data[72];
    if (tmp16 != expected_export_flags) {
        packet_validation_failed = 1;
        printf("\n[CINT] Monitoring - FT Export Flags does not match !, Received Export Flags 0x%02x vs Expected 0x%02x\n",
                tmp16, expected_export_flags);
        return;
    }

    /* Export TimeStamp(6B) */
    printf("\tExport TimeStamp: ");
    for (i = 0; i < 6; i++) {
        printf("%02x ",pkt_data[i+73]);
    }
    printf("\n");
    if ((expected_export_tod_value[0] != pkt_data[73]) ||
        (expected_export_tod_value[1] != pkt_data[74]) ||
        (expected_export_tod_value[2] != pkt_data[75]) ||
        (expected_export_tod_value[3] != pkt_data[76]) ||
        (expected_export_tod_value[4] != pkt_data[77]) ||
        (expected_export_tod_value[5] != pkt_data[78])) {
        packet_validation_failed = 1;
        printf("\n[CINT] Monitoring - FT Export Timestamp does not match.\n");
        printf("expected value 0x%02x%02x%02x%02x%02x%02x vs ",
                expected_export_tod_value[0], expected_export_tod_value[1],
                expected_export_tod_value[2], expected_export_tod_value[3],
                expected_export_tod_value[4], expected_export_tod_value[5]);
        printf("received value in export packet 0x%02x%02x%02x%02x%02x%02x\n",
                pkt_data[73], pkt_data[74], pkt_data[75],
                pkt_data[76], pkt_data[77], pkt_data[78]);
        return;
    }

    /* Flow Key(47B) */
    printf("\tFlow Key: \n");
    printf("\t");
    for (i = 0; i < 47; i++) {
        printf("%02x ",pkt_data[i+79]);
        if (i%16 == 15) {
            printf("\n\t");
        }
    }
    printf("\n");

    /* Flow Data(64B) */
    printf("\tFlow Data: \n");
    printf("\t");
    for (i = 0; i < 64; i++) {
        printf("%02x ",pkt_data[i+126]);
        if (i%16 == 15) {
            printf("\n\t");
        }
    }
    printf("\n");

    /* Dump as per export element list after setid */
    printf("Export Template Wise ....\n");
    rv = bcm_flowtracker_export_template_get(unit, template_id1, fte_set_id,
                                             max_export_element_get,
                                             export_elem_get_op,
                                            &actual_export_element_get);
    if (BCM_FAILURE(rv)) {
        packet_validation_failed = 1;
        printf("\n[CINT] Monitoring - bcm_flowtracker_export_template_get => rv %d(%s)\n",
                                                                    rv, bcm_errmsg(rv));
        return;
    }

    if (actual_export_element_get > 0) {
        /* Export Elements start from byte 66 in ipfix packet received */
        cur_byte = 66;
        for (i = 0; i < actual_export_element_get; i++) {

            /* Print current export element */
            exp_element = export_elem_get_op[i].element;
            export_element_string = ftv2_convert_export_element_to_string(exp_element);
            printf("Export Element %2d(%35s), Size %3d - ",exp_element, export_element_string,
                                                              export_elem_get_op[i].data_size);

            /* Print current export element value */
            actual_export_element_value = 0;
            for (elem_size = 0; elem_size < export_elem_get_op[i].data_size; elem_size++) {

                /* Print value of element byte by byte */
                printf("%02x ",pkt_data[cur_byte + elem_size]);

                /* Capture value of element if size is upto 4Bytes */
                if (export_elem_get_op[i].data_size <= 4) {
                    actual_export_element_value |=
                        pkt_data[cur_byte + elem_size] <<
                      ((export_elem_get_op[i].data_size - 1 - elem_size) * 8);
                }
                if ((export_elem_get_op[i].data_size > 4) &&
                    (export_elem_get_op[i].data_size < 17)) {
                    actual_export_element_array_value[elem_size] =
                        pkt_data[cur_byte + elem_size];
                }

                /* Print next line if bytes printed are more than 16 in one line */
                if ((elem_size + 1) % 16 == 0) {
                    printf("\n");
                    printf("\t\t\t\t\t\t\t\t   ");
                }
            }

            /* Updating Flowtracker Group element with relevent bits */
            if (export_elem_get_op[i].element == bcmFlowtrackerExportElementTypeFlowtrackerGroup) {
                actual_export_element_value = actual_export_element_value & 0xFFF;
            }

            rv = ftv2_validate_export_element(export_elem_get_op[i].element, actual_export_element_value,
                                              actual_export_element_array_value, export_elem_get_op[i].check_id);
            if (BCM_FAILURE(rv)) {
                packet_validation_failed = 1;
                printf("\n[CINT] Monitoring - ftv2_validate_export_element failed for export element <%s> => rv %d(%s)\n",
                        export_element_string, rv, bcm_errmsg(rv));
                return;
            }

            /* Move to next export element in template */
            cur_byte += export_elem_get_op[i].data_size;
            printf("\n");
        }
    }
    printf("\n");
}

void
packet_info_metadata(bcm_pkt_t * pkt)
{
    bcm_rx_reason_t reason;
    int i = 0, reason_count = 0;

    /* Print basic packet information first */
    printf
        ("  length %4d; rx-port %2d; COS %2d; prio_int %2d; VLAN %4d; DMA chan: %d; %s;\n",
         pkt->pkt_len, pkt->rx_port, pkt->cos, pkt->prio_int, pkt->vlan,
         pkt->dma_channel, (pkt->rx_untagged & BCM_PKT_OUTER_UNTAGGED)
         ? ((pkt->rx_untagged & BCM_PKT_INNER_UNTAGGED)
             ? "Untagged" : "Inner tagged")
         : ((pkt->rx_untagged & BCM_PKT_INNER_UNTAGGED) ? "Outer tagged" :
             "Double tagged"));

    /* Print additional packet information */
    if ((pkt->flags & BCM_PKT_STK_F_SRC_PORT) && (pkt->flags & BCM_PKT_STK_F_DST_PORT)) {
        /* Both destination and source GPORTs available */
        printf
            ("  dest-gport %d; src-gport %d; opcode %d;%s matched %d; classification-tag %d;\n",
             pkt->dst_gport, pkt->src_gport, pkt->opcode,
             (pkt->flags & BCM_RX_TRUNCATED) ? " Truncated;" : "", pkt->rx_matched,
             pkt->rx_classification_tag);
    } else if ((pkt->flags & BCM_PKT_STK_F_SRC_PORT)) {
        /* Source GPORT available */
        printf
            ("  dest-port %d; dest-mod %d; src-gport %d; opcode %d;%s matched %d; classification-tag %d;\n",
             pkt->dest_port, pkt->dest_mod, pkt->src_gport, pkt->opcode,
             (pkt->flags & BCM_RX_TRUNCATED) ? " Truncated;" : "", pkt->rx_matched,
             pkt->rx_classification_tag);
    } else if ((pkt->flags & BCM_PKT_STK_F_DST_PORT)) {
        printf
            (" dest-gport %d; %s %d; src-mod %d; opcode %d;%s matched %d; classification-tag %d;\n",
             pkt->dst_gport, (pkt->flags & BCM_PKT_F_TRUNK) ? "src-trunk" : "src-port",
             (pkt->flags & BCM_PKT_F_TRUNK) ? pkt->src_trunk : pkt->src_port,
             pkt->src_mod, pkt->opcode,
             (pkt->flags & BCM_RX_TRUNCATED) ? " Truncated;" : "", pkt->rx_matched,
             pkt->rx_classification_tag);
    } else {
        /* No GPORTs */
        printf
            ("  dest-port %d; dest-mod %d;%s %d; src-mod %d; opcode %d;%s matched %d; classification-tag %d;\n",
             pkt->dest_port, pkt->dest_mod,
             (pkt->flags & BCM_PKT_F_TRUNK) ? "src-trunk" : "src-port",
             (pkt->flags & BCM_PKT_F_TRUNK) ? pkt->src_trunk : pkt->src_port,
             pkt->src_mod, pkt->opcode,
             (pkt->flags & BCM_RX_TRUNCATED) ? " Truncated;" : "", pkt->rx_matched,
             pkt->rx_classification_tag);
    }

    /*
     * Print out reasons (if any) by iterating through the entire reasons
     * mask and printing the reason string associated with each reason.
     */
    reason_count = 0;
    BCM_RX_REASON_ITER(pkt->rx_reasons, reason) {
        reason_count++;
    }
    if (reason_count) {
        printf("\nPacket sent for %d reason(s)\n", reason_count);
    } else {
        printf("Packet switched to CPU\n");
    }
}

bcm_rx_t
ftPacketWatcher(int unit, bcm_pkt_t *pkt, void *cookie)
{
    int i = 0, *packet_count = (auto) cookie;

    /* Increment packet count */
    (*packet_count)++;

    if (!skip_log) {

        printf("[CINT] ftPacketWatch utility called ..\n");
        printf(".........................ftPw.........................\n");
        printf("[CINT] Received Packet: %2d\n", *packet_count);

        /* Print Packet metadata */
        packet_info_metadata(pkt);

        /* Print full packet received */
        for (i = 0; i < pkt->tot_len; i++) {
            uint32 print_byte = pkt->pkt_data->data[i] & 0xFF;
            if ((i & 0xf) == 0) {
                printf("\n");
            }
            printf("%02X ", print_byte);
        }
        if (((i & 0xf) != 1)) {
            printf("\n");
        }

        /* Parse packet with logs */
        if (pkt->tot_len >= 190) {
            packet_data_parse_with_dump(pkt);
        }
        printf(".........................ftPw.........................\n");

    } else {

        /* Parse packet without logs */
        if (pkt->tot_len >= 190) {
            packet_data_parse_without_dump(pkt);
        }
    }

    packet_handled_by_watcher = 1;
    return BCM_RX_HANDLED;
}

/* Start Rx Thread & Register callback utility */
int
ftv2_rx_task_init(int unit)
{
    int rv = BCM_E_NONE;
    /* Use a priority above 100 to be able to co-exist with diagnostic shell packetWatcher */
    const int priority = BCM_RX_PRIO_MAX;
    /* Receive packets from all CPU queues. */
    const int flags = BCM_RCO_F_ALL_COS;

    /* Check  if rx Task is already active */
    rx_task_active = bcm_rx_active(unit);

    /* If necessary, initialize and start the receive task */
    if (!rx_task_active) {
        rv = bcm_rx_init(unit);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] bcm_rx_init failed,rv %d(%s)\n",
                                         rv,bcm_errmsg(rv));
            return rv;
        }
        rv = bcm_rx_start(unit, NULL);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] bcm_rx_start failed, rv %d(%s)\n",
                                           rv,bcm_errmsg(rv));
            return rv;
        }
        if (!skip_log) {
            printf("[CINT] Monitoring - Started the Rx task\n");
        }
    } else {
        if (!skip_log) {
            printf("[CINT] Monitoring - Rx task already active\n");
        }
    }

    /* Unregister & Register Packet Watcher Rx Callback. */
    bcm_rx_unregister (unit, ftPacketWatcher, priority);
    rv = bcm_rx_register(unit, "FT Rx PacketWatch", ftPacketWatcher, priority, &rx_count, flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] bcm_rx_register failed, rv %d(%s)\n",rv,bcm_errmsg(rv));
        return rv;
    }

    if (!skip_log) {
        printf("[CINT] Monitoring - Registered CINT Packet Watcher callback utility.\n");
    }

    return BCM_E_NONE;
}

/* Stop Rx Thread & Register callback utility */
int
ftv2_rx_task_stop(int unit)
{
    int rv = BCM_E_NONE;
    const int priority = BCM_RX_PRIO_MAX;

    /* Un-Register Packet Watcher Rx Callback. */
    rv = bcm_rx_unregister (unit, ftPacketWatcher, priority);
    if (BCM_FAILURE(rv)) {
        printf("[CINT]  bcm_rx_unregister failed, rv %d(%s)\n",rv,bcm_errmsg(rv));
        return rv;
    }
    if (!skip_log) {
        printf("[CINT] Monitoring - Un-registered CINT Packet Watcher callback utility.\n");
    }

    if (!rx_task_active) {
        /* stop rx task */
        rv = bcm_rx_stop(unit, NULL);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] bcm_rx_stop failed,rv %d(%s)\n",rv,bcm_errmsg(rv));
            return rv;
        }
        /* initialise rx */
        rv = bcm_rx_init(unit);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] bcm_rx_init failed,rv %d(%s)\n",rv,bcm_errmsg(rv));
            return rv;
        }
        if (!skip_log) {
            printf("[CINT] Monitoring - Stopped the Rx task\n");
        }
    } else {
        printf("[CINT] Monitoring - Rx task already active, hence no need to stop.\n");
    }

    /* Final log */
    if (!skip_log) {
        printf("[CINT] Monitoring stopped succesfully.\n");
    }

    return BCM_E_NONE;
}

/* Start monitoring utility */
int
ftv2_test_start_monitoring(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;

    if (!use_ftrmon_thread) {
        rv = ftv2_rx_task_init(unit);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - ftv2_rx_task_init failed, rv %d(%s)\n", tc_id,rv,bcm_errmsg(rv));
            return rv;
        }
    } else {
        bshell(unit, "ftrmon start");
        bshell(unit, "ftrmon report +decode");
        if (!skip_log) {
            printf("[CINT] Monitoring - Started ftrmon task\n");
        }
    }

    return BCM_E_NONE;
}

/* Stop monitoring utility */
int
ftv2_test_stop_monitoring(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;

    /* Skip stopping monitoring task */
    if (skip_cleanup) {
        if (!skip_log) {
            printf("[CINT] Cleanup - Skip stopping monitoring task ......\n");
        }
        return rv;
    }

    if (!use_ftrmon_thread) {
        rv = ftv2_rx_task_stop(unit);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - ftv2_rx_task_stop failed, rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
            return rv;
        }
    } else {
        bshell(unit, "ftrmon stop");
        if (!skip_log) {
            printf("[CINT] Monitoring - Stopped ftrmon task\n");
        }
    }

    return BCM_E_NONE;
}
