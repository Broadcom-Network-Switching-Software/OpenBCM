/*~~~~~~~~~~~~~~~~~~~~~~~~~~Cosq: VOQ counter processor~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_counter_processor.c
 * Purpose:     Example of configuring counter processor statistics gatheting using dynamic APIs.
 * 
 * *
 * The settings include:
 *  - Configuring engine's counting source, and counting formats.
 *  - Configuring LIF counting ranges.
 */

int lif_counting_set(int unit,                                           /*in*/ 
                     bcm_stat_counter_source_type_t source,              /*in*/ 
                     int command_id,                                     /*in*/ 
                     bcm_stat_counter_lif_range_id_t lif_counting_mask,  /*in*/ 
                     bcm_stat_counter_lif_stack_id_t lif_stack_to_map,   /*in*/ 
                     bcm_stat_counter_lif_stack_id_t lif_stack_to_count  /*in*/ 
                ) {
    bcm_stat_counter_lif_range_id_t 
        lif_ranges[BCM_STAT_COUNT_LIF_NUMBER_OF_STACK_IDS] = {
            bcmStatCounterLifRangeIdLifInvalid, 
            bcmStatCounterLifRangeIdNotInAny, 
            bcmBcmStatCounterLifRangeId0, 
            bcmBcmStatCounterLifRangeId1};
    bcm_error_t rv = BCM_E_NONE;
    uint32 flags = 0;
    int bitmap = 0;
    int match;
    int lif_0 = 0,
        lif_1 = 0,
        lif_2 = 0,
        lif_3 = 0;
    bcm_stat_counter_source_t counter_source;
    bcm_stat_counter_lif_mask_t counting_mask;
    counter_source.engine_source = source;
    counter_source.command_id = command_id;
    for (bitmap = 0; bitmap < 0x100 ;bitmap++) {
        lif_0 = bitmap & 0x3;
        lif_1 = (bitmap >> 2) & 0x3;
        lif_2 = (bitmap >> 4) & 0x3;
        lif_3 = (bitmap >> 6) & 0x3;
        counting_mask.lif_counting_mask[0] = lif_ranges[lif_0];
        counting_mask.lif_counting_mask[1] = lif_ranges[lif_1];
        counting_mask.lif_counting_mask[2] = lif_ranges[lif_2];
        counting_mask.lif_counting_mask[3] = lif_ranges[lif_3];

        match = 0;
        if (lif_stack_to_map != bcmStatCounterLifStackIdNone) {
            if (counting_mask.lif_counting_mask[lif_stack_to_map] == lif_counting_mask) {match = 1;}
        } else {
            if (counting_mask.lif_counting_mask[0] == lif_counting_mask) {match = 1;}
            if (counting_mask.lif_counting_mask[1] == lif_counting_mask) {match = 1;}
            if (counting_mask.lif_counting_mask[2] == lif_counting_mask) {match = 1;}
            if (counting_mask.lif_counting_mask[3] == lif_counting_mask) {match = 1;}
        }
        if (match) {
            rv = bcm_stat_counter_lif_counting_set (unit, flags, &counter_source, &counting_mask, lif_stack_to_count);
            if (rv != BCM_E_NONE) {
                printf("bcm_stat_counter_lif_counting_set() failed $rv\n");
                return rv;
            }
        }
    }
    return rv;
}

int lif_counting_get(int unit,                                           /*in*/ 
                     bcm_stat_counter_source_type_t source,              /*in*/ 
                     int command_id,                                     /*in*/ 
                     bcm_stat_counter_lif_range_id_t  lif_counting_mask, /*in*/ 
                     bcm_stat_counter_lif_stack_id_t  lif_stack_to_map,  /*in*/ 
                     bcm_stat_counter_lif_stack_id_t* lif_stack_to_count /*out*/
                ) {
    bcm_stat_counter_lif_range_id_t 
        lif_ranges[BCM_STAT_COUNT_LIF_NUMBER_OF_STACK_IDS] = {
            bcmStatCounterLifRangeIdLifInvalid, 
            bcmStatCounterLifRangeIdNotInAny, 
            bcmBcmStatCounterLifRangeId0, 
            bcmBcmStatCounterLifRangeId1};
    bcm_error_t rv = BCM_E_NONE;
    uint32 flags = 0;
    int bitmap = 0;
    int lif_0 = 0,
        lif_1 = 0,
        lif_2 = 0,
        lif_3 = 0;
    bcm_stat_counter_source_t counter_source;
    bcm_stat_counter_lif_mask_t counting_mask;
    counter_source.engine_source = source;
    counter_source.command_id = command_id;
    bitmap = sal_rand() & 0xff;
    lif_0 = bitmap & 0x3;
    lif_1 = (bitmap >> 2) & 0x3;
    lif_2 = (bitmap >> 4) & 0x3;
    lif_3 = (bitmap >> 6) & 0x3;
    counting_mask.lif_counting_mask[0] = lif_ranges[lif_0];
    counting_mask.lif_counting_mask[1] = lif_ranges[lif_1];
    counting_mask.lif_counting_mask[2] = lif_ranges[lif_2];
    counting_mask.lif_counting_mask[3] = lif_ranges[lif_3];

    
    if (lif_stack_to_map != bcmStatCounterLifStackIdNone) {
        counting_mask.lif_counting_mask[lif_stack_to_map] = lif_counting_mask;
    } else {
        rv = BCM_E_PARAM;
        return rv;
    }
    rv = bcm_stat_counter_lif_counting_get(unit, flags, &counter_source, &counting_mask, lif_stack_to_count);
    if (rv != BCM_E_NONE) {
        printf("bcm_stat_counter_lif_counting_get() failed $rv\n");
        return rv;
    }
    
    return rv;
}

int lif_counting_test(int unit,                                           /*in*/ 
                      bcm_stat_counter_source_type_t source,              /*in*/ 
                      int command_id,                                     /*in*/ 
                      bcm_stat_counter_lif_range_id_t lif_counting_mask,  /*in*/ 
                      bcm_stat_counter_lif_stack_id_t lif_stack_to_map,   /*in*/ 
                      bcm_stat_counter_lif_stack_id_t lif_stack_to_count, /*in*/
                      int nof_iterations                                  /*in*/
                ) {
    bcm_stat_counter_lif_stack_id_t lif_stack_to_count_get;
    bcm_error_t rv = BCM_E_NONE;
    int idx_iteration;
    rv = lif_counting_set(unit, source, command_id, lif_counting_mask, lif_stack_to_map, lif_stack_to_count);
    if (rv != BCM_E_NONE) {
        printf("lif_counting_set() failed $rv\n");
        return rv;
    }

    if (lif_stack_to_map != bcmStatCounterLifStackIdNone) {
        for (idx_iteration = 0; idx_iteration < nof_iterations; idx_iteration++) {
            rv = lif_counting_get(unit, source, command_id, lif_counting_mask, lif_stack_to_map, &lif_stack_to_count_get);
            if (rv != BCM_E_NONE) {
                printf("lif_counting_get() failed $rv\n");
                return rv;
            }
            if (lif_stack_to_count != lif_stack_to_count_get) {
                printf("expected lif_stack_to_count_get to be $d but got %d.\n", lif_stack_to_count, lif_stack_to_count_get);
                return BCM_E_CONFIG;
            }
        }
    }
    return rv;
}

int lif_counting_get_test(int unit,                                                    /*in*/ 
                          bcm_stat_counter_source_type_t source,                       /*in*/ 
                          int command_id,                                              /*in*/ 
                          bcm_stat_counter_lif_stack_id_t expected_lif_stack_to_count, /*in*/
                          int nof_iterations                                           /*in*/)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 flags = 0;
    int idx_iteration; 
    int lif_mask_iter;
    bcm_stat_counter_source_t counter_source;
    bcm_stat_counter_lif_mask_t counting_mask;

    counter_source.engine_source = source;
    counter_source.command_id = command_id;
    int lif_stack_to_count;
    bcm_stat_counter_lif_range_id_t 
        lif_ranges[BCM_STAT_COUNT_LIF_NUMBER_OF_STACK_IDS] = {
            bcmStatCounterLifRangeIdLifInvalid, 
            bcmStatCounterLifRangeIdNotInAny, 
            bcmBcmStatCounterLifRangeId0, 
            bcmBcmStatCounterLifRangeId1};
    for (idx_iteration = 0; idx_iteration < nof_iterations; idx_iteration++) {
        for (lif_mask_iter = bcmStatCounterLifStackId0; lif_mask_iter < BCM_STAT_COUNT_LIF_NUMBER_OF_STACK_IDS; lif_mask_iter++) {
            if (lif_mask_iter == expected_lif_stack_to_count) {
                counting_mask.lif_counting_mask[lif_mask_iter] = (command_id == 0) ? bcmBcmStatCounterLifRangeId0 : bcmBcmStatCounterLifRangeId1; 
            } else {
                counting_mask.lif_counting_mask[lif_mask_iter] = lif_ranges[sal_rand() & 0x3];
            }
        }
        rv = _bcm_petra_stat_counter_lif_counting_get(unit, flags, &counter_source, &counting_mask, &lif_stack_to_count); 
        if (rv != BCM_E_NONE) {
            printf("bcm_stat_counter_lif_counting_get() failed %d\n", rv);
            return rv;
        }
        if (lif_stack_to_count != expected_lif_stack_to_count) {
            printf("bcm_stat_counter_lif_counting_get() expected %d, got %d\n", expected_lif_stack_to_count, lif_stack_to_count);
            return BCM_E_CONFIG;
        }
    }
    return rv;
}



int sfi_stat_bad_get(int unit)
{
    uint64 temp_val;
    int port;
    bcm_port_config_t pcfg;

    bcm_port_config_get(0, &pcfg);

    BCM_PBMP_ITER(pcfg.sfi, port) {
        bcm_stat_get(0, port, snmpBcmTxControlCells, &temp_val);
        bcm_stat_get(0, port, snmpBcmTxDataCells, &temp_val);
        bcm_stat_get(0, port, snmpBcmTxDataBytes, &temp_val);
        bcm_stat_get(0, port, snmpBcmRxCrcErrors, &temp_val);
        bcm_stat_get(0, port, snmpBcmRxFecCorrectable, &temp_val);
        bcm_stat_get(0, port, snmpBcmRxControlCells, &temp_val);
        bcm_stat_get(0, port, snmpBcmRxDataCells, &temp_val);
        bcm_stat_get(0, port, snmpBcmRxDataBytes, &temp_val);
        bcm_stat_get(0, port, snmpBcmRxDroppedRetransmittedControl, &temp_val);
        bcm_stat_get(0, port, snmpBcmTxAsynFifoRate, &temp_val);
        bcm_stat_get(0, port, snmpBcmRxAsynFifoRate, &temp_val);
        bcm_stat_get(0, port, snmpBcmRxFecUncorrectable, &temp_val);
        bcm_stat_get(0, port, snmpBcmRxCrcErrors, &temp_val);
    }
    return BCM_E_NONE;
}

int sfi_stat_counter_clear(int unit, int isFE)
{
    uint64 temp_val;
    int port;
    bcm_port_config_t pcfg;
    bcm_error_t rv = BCM_E_NONE;

    bcm_port_config_get(0, &pcfg);

    BCM_PBMP_ITER(pcfg.sfi, port) {
        rv = bcm_stat_clear(0, port);
        if(rv !=  BCM_E_NONE) {
            printf("clear port %d counter error. \n", port);
            return rv;
        }
    }
    return BCM_E_NONE;
}

int sfi_stat_rxcrc_err_check(int unit, int isFE)
{
    uint64 temp_val;
    uint32 hi,lo;
    int port;
    bcm_port_config_t pcfg;
    uint32 link_status, errored_token_count = 0;

    COMPILER_64_ZERO(temp_val);
    bcm_port_config_get(0, &pcfg);
    if( isFE == 1) { /*FE device*/
        printf("FE device. \n");
        BCM_PBMP_ITER(pcfg.sfi, port) {
            bcm_fabric_link_status_get(unit, port, &link_status, &errored_token_count);
            if ( errored_token_count ) {
                printf("Failed,port:%d Errored tokens count expected 0,but got %d\n",port,errored_token_count);
                return BCM_E_FAIL;
            }
        }
    } else {
        BCM_PBMP_ITER(pcfg.sfi, port) {
            bcm_stat_get(0, port, snmpBcmRxCrcErrors, &temp_val);
            hi = COMPILER_64_HI(temp_val);
            lo = COMPILER_64_LO(temp_val);
            if (hi || lo) {
                printf("Failed,port:%d snmpBcmRxCrcErrors expected(0,0), but got(hi=%d,lo=%d)\n", port,hi,lo);
                return BCM_E_FAIL;
            }
        }
    }
    return BCM_E_NONE;
}


int sfi_stat_rxcrc_err_test(int unit, int isFE)
{
    int i, loop = 30;
    bcm_error_t rv = BCM_E_NONE;

    printf("Clear sfi port counter. \n");
    rv = sfi_stat_counter_clear(unit,isFE);
    if(rv !=  BCM_E_NONE) {
        return rv;
    }
    printf("Check pre configuration. \n");
    rv = sfi_stat_rxcrc_err_check(unit,isFE);
    if(rv !=  BCM_E_NONE) {
        return rv;
    }

    printf("Loop %d times,please wait...\n", loop);
    for(i = 0; i < loop; i++) {
        sfi_stat_bad_get(0);
        sal_sleep(1);
    }
    printf("Check test result. \n");
    return sfi_stat_rxcrc_err_check(unit,isFE);

}


int lif_counting_erpp_double_packet_check(int unit, int in_port, int out_port, int pkt_num)
{
    int rv;
    int countId = 1024*16;
    int counter_proc = 5;
    int is_qux=0;
    uint32 dummy_flags ;
    bcm_port_interface_info_t interface_info ;
    bcm_port_mapping_info_t mapping_info ;

    rv = bcm_port_get(unit, out_port, &dummy_flags, &interface_info, &mapping_info); 
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_port_get!\n");
        return rv;
    }

    rv = is_qux_only(unit, &is_qux);
    if (is_qux && (rv == BCM_E_NONE)) {
        countId = 1024*8;
    } else if (rv == BCM_E_NONE) {
        countId = 1024*16;
    } else {
        printf("Error in is_qux_only!\n");
        return rv;
    }  

    bcm_stat_counter_engine_t counter_engine;
    bcm_stat_counter_config_t counter_config;

    counter_config.format.format_type = bcmStatCounterFormatDoublePackets;
    counter_config.format.counter_set_mapping.counter_set_size = 2;
    counter_config.format.counter_set_mapping.num_of_mapping_entries = 2;
    counter_config.format.counter_set_mapping.entry_mapping[0].entry.is_forward_not_drop =1;
    counter_config.format.counter_set_mapping.entry_mapping[0].offset =0;
    counter_config.format.counter_set_mapping.entry_mapping[1].entry.is_forward_not_drop =0;
    counter_config.format.counter_set_mapping.entry_mapping[1].offset =1;

    counter_config.source.pointer_range.start = countId;
    counter_config.source.pointer_range.end = countId + 1024 - 1;
    counter_config.source.engine_source = bcmStatCounterSourceEgressReceiveOutlif;
    counter_config.source.core_id = mapping_info.core;

    counter_engine.engine_id = counter_proc;

    rv = bcm_stat_counter_config_set(unit, &counter_engine, &counter_config);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_stat_counter_config_set!\n");
        return rv;
    }

    int port=1;
    int vlan = 1;
    char cmd[300] = {0};

    bcm_pbmp_t pbmp, untag_pbmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(untag_pbmp);
    bcm_l2_addr_t l2_addr;
    bcm_mac_t mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};

    bcm_vlan_port_t vlan_port;
    bcm_gport_t default_inlif;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = out_port;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.flags = BCM_VLAN_PORT_WITH_ID;
    vlan_port.vsi = vlan;

    BCM_GPORT_SUB_TYPE_LIF_SET(vlan_port.vlan_port_id, BCM_GPORT_SUB_TYPE_LIF, countId);
    BCM_GPORT_VLAN_PORT_ID_SET(vlan_port.vlan_port_id, vlan_port.vlan_port_id);

    bcm_vlan_port_create(unit, &vlan_port);
    default_inlif = vlan_port.vlan_port_id;

    bcm_l2_addr_t_init(&l2_addr, mac, 1);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = default_inlif;

    bcm_l2_addr_add(unit, &l2_addr);

    sprintf(cmd, "tx %d psrc=%d data=0x000000000002000000000001123123919371093719873173098173097198371983719827398173289172309817238091730987128937198237810927309812", pkt_num, in_port);
    bshell(unit, cmd);

    uint32 numOfpackets = 0;
    int stat_id = 0;

    BCM_FIELD_STAT_ID_SET(stat_id, counter_proc, countId);
    /*check fwd packet*/
    rv = bcm_field_stat_get32(unit, stat_id, bcmFieldStatOffset0Packets, numOfpackets);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_stat_get32!\n");
        return rv;
    }
    if (numOfpackets != pkt_num) {

        printf("Error in Fwd pkt count error!\n");
        return -1;
    }

    /*check drop packet*/
    rv = bcm_field_stat_get32(unit, stat_id, bcmFieldStatOffset1Packets, numOfpackets);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_stat_get32!\n");
        return rv;
    }
    if (numOfpackets != 0) {

        printf("Error in Drop pkt count error!\n");
        return -1;
    }
    
    return BCM_E_NONE;
}



