/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file provides port basic functionality
 */

int cint_nif_priority_config_mpls_special_label_config(int unit, int in_port, int scenario_index)
{

    int rv = BCM_E_NONE, index;
    uint32 flags = 0;
    bcm_cosq_ingress_port_drop_map_t cosq_map;
    bcm_port_prio_config_t priority_config;
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t label_config;
    int exp, prio;
    uint32 key, tpid=0x8100, fifo_size_01, fifo_size_23;
    uint32 special_label1[4] = {1, 13, 14, 1};
    uint32 special_label2[4] = {2, 6, 10, 15};

    set_clu_prd_port_profile(unit, in_port);

    /* Set port priority groups */
    priority_config.nof_priority_groups = 2;
    priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerLow;
    priority_config.priority_groups[0].source_priority = BCM_PORT_F_PRIORITY_0  | BCM_PORT_F_PRIORITY_1;
    priority_config.priority_groups[0].num_of_entries = -1;
    priority_config.priority_groups[1].sch_priority = bcmPortNifSchedulerHigh;
    priority_config.priority_groups[1].source_priority = BCM_PORT_F_PRIORITY_2  | BCM_PORT_F_PRIORITY_3;
    priority_config.priority_groups[1].num_of_entries = -1;
    rv = bcm_port_priority_config_set(unit, in_port, &priority_config);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_dnx_port_priority_config_set\n", rv);
        return rv;
    }

    /* Fill the hard stage priority maps */
    for(exp=0;exp<8;exp++) {
        prio=exp%4;
        key=BCM_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY(exp);
        rv = bcm_cosq_ingress_port_drop_map_set(unit, in_port, flags, bcmCosqIngressPortDropMplsExpToPriorityTable, key, prio);
        if (rv != BCM_E_NONE) {
            printf("Failed(%d) bcm_cosq_ingress_port_drop_map_set\n", rv);
            return rv;
        }
    }

    if(scenario_index == 0) {
        /* Set MPLS Special labels for common usage */
        for(index=0;index<4;index++) {
            label_config.label_value = special_label1[index];
            label_config.priority = 2;
            label_config.is_tdm = 0;
            rv = bcm_cosq_ingress_port_drop_mpls_special_label_set(unit, in_port, flags, index, label_config);
            if (rv != BCM_E_NONE) {
                printf("Failed(%d) bcm_cosq_ingress_port_drop_mpls_special_label_set\n", rv);
                return rv;
            }
        }
    } else if(scenario_index == 1) {
        /* Set MPLS Special labels for special usage */
        for(index=0;index<4;index++) {
            label_config.label_value = special_label2[index];
            label_config.priority = special_label2[index]/4;
            label_config.is_tdm = 0;
            rv = bcm_cosq_ingress_port_drop_mpls_special_label_set(unit, in_port, flags, index, label_config);
            if (rv != BCM_E_NONE) {
                printf("Failed(%d) bcm_cosq_ingress_port_drop_mpls_special_label_set\n", rv);
                return rv;
            }
        }
    }

    /*Set TPID for the port*/
    rv = bcm_cosq_control_set(unit,in_port,-1,bcmCosqControlIngressPortDropTpid1,tpid);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_control_set\n", rv);
        return rv;
    }

    /* Set threshold for every priority. For the example we will create an equally divided FIFO */
    rv = bcm_port_priority_config_get(unit, in_port, &priority_config);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_port_priority_config_get\n", rv);
        return rv;
    }
    fifo_size_01 = priority_config.priority_groups[0].num_of_entries;
    fifo_size_23 = priority_config.priority_groups[1].num_of_entries;
    rv = bcm_cosq_ingress_port_drop_threshold_set(unit, in_port, flags, 0, fifo_size_01/2);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_threshold_set\n", rv);
        return rv;
    }
    rv = bcm_cosq_ingress_port_drop_threshold_set(unit, in_port, flags, 1, fifo_size_01);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_threshold_set\n", rv);
        return rv;
    }
    rv = bcm_cosq_ingress_port_drop_threshold_set(unit, in_port, flags, 2, fifo_size_23/2);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_threshold_set\n", rv);
        return rv;
    }
    rv = bcm_cosq_ingress_port_drop_threshold_set(unit, in_port, flags, 3, fifo_size_23);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_threshold_set\n", rv);
        return rv;
    }

    /*Enable hard stage PRD!*/
    rv = bcm_cosq_ingress_port_drop_enable_set(unit, in_port, flags, 1);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_enable_set\n", rv);
        return rv;
    }

    printf("cint_nif_priority_config_mpls_common_usage: PASS\n\n");
    return BCM_E_NONE;
}

int cint_nif_priority_config_soft_stage_JR2(int unit, int in_port,int lp_pcp,int low_priority,int hp_pcp,int high_priority, uint32 header,
                                             uint32 match_key_size, uint32 *lp_match_key, uint32 *hp_match_key,
                                             uint32 offset_base, uint32 lp_key_index, uint32 hp_key_index)
{
    int rv = BCM_E_NONE;
    uint32 flags = 0;
    int index, lp_key, hp_key;
    bcm_cosq_ingress_port_drop_map_t cosq_map;
    bcm_info_t info;
    bcm_cosq_ingress_drop_flex_key_construct_id_t key_id;
    bcm_cosq_ingress_drop_flex_key_construct_t flex_key_config;
    bcm_cosq_ingress_drop_flex_key_t ether_code;
    bcm_cosq_ingress_drop_flex_key_entry_t lp_flex_key_info;
    bcm_cosq_ingress_drop_flex_key_entry_t hp_flex_key_info;
    bcm_cosq_ingress_drop_flex_key_t key_field_match;
    uint32 offset = 1;
    uint32 tpid = 0x8100;
    uint32 ether_type_val = 0xc0c0;

    if (header == 0) {
        lp_key = BCM_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY(lp_pcp,0);
        hp_key = BCM_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY(hp_pcp,0);
        cosq_map = bcmCosqIngressPortDropEthPcpDeiToPriorityTable;
    } else if (header == 1 || header == 2) {
        lp_key = BCM_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY(lp_pcp,0);
        hp_key = BCM_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY(hp_pcp,0);
        cosq_map = bcmCosqIngressPortDropTmTcDpPriorityTable;
    } else if (header == 3) {
        lp_key = BCM_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY(lp_pcp);
        hp_key = BCM_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY(hp_pcp);
        cosq_map = bcmCosqIngressPortDropIpDscpToPriorityTable;
    } else if (header == 4) {
        lp_key = BCM_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY(lp_pcp);
        hp_key = BCM_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY(hp_pcp);
        cosq_map = bcmCosqIngressPortDropMplsExpToPriorityTable;
    } else {
        printf("ERROR: Incorrect value for header type %d", header);
        return BCM_E_PARAM;
    }

    /*Map the created lp_key to low priority*/
    rv = bcm_cosq_ingress_port_drop_map_set(unit,in_port,flags,cosq_map,lp_key,low_priority);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_map_set\n", rv);
        return rv;
    }

    /*Map the created hp_key to high priority */
    rv = bcm_cosq_ingress_port_drop_map_set(unit,in_port,flags,cosq_map,hp_key,high_priority);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_map_set\n", rv);
        return rv;
    }

    /*Set TPID for the port*/
    rv = bcm_cosq_control_set(unit,in_port,-1,bcmCosqControlIngressPortDropTpid1,tpid);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_control_set\n", rv);
        return rv;
    }

    /*Set 1 byte as a threshold level for the FIFO for packets mapped to low_priority*/
    rv = bcm_cosq_ingress_port_drop_threshold_set(unit,in_port,flags,low_priority,1);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_threshold_set\n", rv);
        return rv;
    }

      /********************************/
     /**  Start Soft Stage override **/
    /********************************/
    /* Configure ConfigurableEthType1(code 2) for the given port with value 0xc0c0 */
    bcm_cosq_ingress_drop_flex_key_construct_t_init(&flex_key_config);

    if((offset_base == bcmCosqIngressPortDropFlexKeyBasePacketStart) || (offset_base == bcmCosqIngressPortDropFlexKeyBaseEndOfEthHeader)) {
        flex_key_config.ether_type_header_size = -1;
    } else {
        flex_key_config.ether_type_header_size = 6;
    }

    rv = bcm_cosq_ingress_port_drop_custom_ether_type_set(unit, in_port, flags, 2, ether_type_val);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_custom_ether_type_set \n", rv);
        return rv;
    }

    /* Set the rules for constructing a key on this port */
    key_id.port = in_port;
    key_id.ether_type_code = 2;

    /* Construct the key of from the given offset value*/
    flex_key_config.array_size = 4;
    flex_key_config.offset_base = offset_base;
    flex_key_config.offset_array[0] = offset;
    flex_key_config.offset_array[1] = offset+1;
    flex_key_config.offset_array[2] = offset+2;
    flex_key_config.offset_array[3] = offset+3;
    rv = bcm_cosq_ingress_port_drop_flex_key_construct_set(unit, key_id, flags, flex_key_config);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_flex_key_construct_set \n", rv);
        return rv;
    }

    /* Override the priority of the two streams*/
    ether_code.value = 2;
    ether_code.mask = 0xf;
    /* Check for match flex_key_config.offset_array[0] flex_key_config.offset_array[1] flex_key_config.offset_array[2] flex_key_config.offset_array[3].
       The other offset bytes are around the one that is checked so this
       reasuers us that if we have a match we checked the correct byte */

    /* Add low prio key to the TCAM */
    /* If match - set to priority 2 */
    bcm_cosq_ingress_drop_flex_key_entry_t_init(&lp_flex_key_info);

    lp_flex_key_info.ether_code = ether_code;
    lp_flex_key_info.num_key_fields = match_key_size;
    lp_flex_key_info.priority = high_priority;

    for (index = 0 ; index < match_key_size ; index++)
    {
        key_field_match.value = lp_match_key[index];
        key_field_match.mask = 0xFF;
        lp_flex_key_info.key_fields[index] = key_field_match;
    }

    rv = bcm_cosq_ingress_port_drop_flex_key_entry_set(unit, in_port, flags, lp_key_index, lp_flex_key_info);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_flex_key_entry_set\n", rv);
        return rv;
    }

    /* Add high prio key to the TCAM */
    /* If match - set to priority 0 */
    bcm_cosq_ingress_drop_flex_key_entry_t_init(&hp_flex_key_info);

    hp_flex_key_info.ether_code = ether_code;
    hp_flex_key_info.num_key_fields = match_key_size;
    hp_flex_key_info.priority = low_priority;

    for (index = 0 ; index < match_key_size ; index++)
    {
        key_field_match.value = hp_match_key[index];
        key_field_match.mask = 0xFF;
        hp_flex_key_info.key_fields[index] = key_field_match;
    }

    rv = bcm_cosq_ingress_port_drop_flex_key_entry_set(unit, in_port, flags, hp_key_index, hp_flex_key_info);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_flex_key_entry_set\n", rv);
        return rv;
    }

    /*Enable the NIF PRD soft stage feature for the given port!*/
    rv = bcm_cosq_ingress_port_drop_enable_set(unit, in_port, flags, 2);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_enable_set\n", rv);
        return rv;
    }

    printf("cint_nif_priority_config_soft_stage_JR2: PASS\n\n");
    return BCM_E_NONE;
}
