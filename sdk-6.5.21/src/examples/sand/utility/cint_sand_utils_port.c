/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file provides port basic functionality
 *
 * files including this cint should also load cint_sand_utils_global.c
 */

int cint_ire_nif_shaper_drops_set(int unit)
{
    int rv = BCM_E_NONE;
    int is_jer = 0, is_qmx = 0;
    bcm_info_t info;

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_info_get\n", rv);
        return rv;
    }

    if(info.device == 0x8675 || info.device == 0x8680) {
        is_jer = 1;
    }

    if(info.device == 0x8375) {
        is_qmx = 1;
    }

    /* artificially create pressure in the NIF buffers */
    if (is_jer || is_qmx) {
        bshell(unit, "m IRE_NETWORK_INTERFACE_SHAPER NIF_N_SHAPER_TIMER_CYCLES=1 NIF_N_SHAPER_GRANT_SIZE=2 NIF_N_SHAPER_MAX_BURST=0x101");
    }
    else {
        bshell(unit, "m IRE_NETWORK_INTERFACE_SHAPER NIF_SHAPER_TIMER_CYCLES=3 NIF_SHAPER_GRANT_SIZE=3 NIF_SHAPER_MAX_BURST=0x101");
    }

    return BCM_E_NONE;
}

int cint_nif_priority_config(int unit, int in_port,
                             int lp_pcp, int low_priority,
                             int hp_pcp, int high_priority, uint32 tpid, uint32 header)
{

    int rv = BCM_E_NONE;
    uint32 flags = 0;
    int lp_key, hp_key;
    bcm_cosq_ingress_port_drop_map_t cosq_map;
    bcm_info_t info;
    bcm_port_interface_info_t itf_info;
    bcm_port_mapping_info_t map_info;
    uint32 port_flags;

    /* Get port info */
    rv = bcm_port_get(unit, in_port, &port_flags, &itf_info, &map_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), bcm_port_get\n", rv);
        return rv;
    }

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_info_get\n", rv);
        return rv;
    }

    /* Configure the NIF ports priority drop*/

    /* Header type legend
    0 - VLAN
    1 - ITMH
    2 - HIGIG
    3 - IP
    4 - MPLS
    */

    /*Depending on the header we chose also and the map type
    bcmCosqIngressPortDropTmTcDpPriorityTable = 0,
    bcmCosqIngressPortDropIpDscpToPriorityTable = 1,
    bcmCosqIngressPortDropEthPcpDeiToPriorityTable = 2,
    bcmCosqIngressPortDropMplsExpToPriorityTable = 3
    }*/

    /* Choose the right key constructor macro for the specified header type and the right COSQ table*/
    if (header == 0) {
        lp_key = BCM_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY(lp_pcp,0);
        hp_key = BCM_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY(hp_pcp,0);
        cosq_map = bcmCosqIngressPortDropEthPcpDeiToPriorityTable;
    } else if (header == 1 || header == 2) {
        lp_key = BCM_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY(lp_pcp,0);
        hp_key = BCM_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY(hp_pcp,0);
        cosq_map = bcmCosqIngressPortDropTmTcDpPriorityTable;
    } else if ((info.device == 0x8680 || is_device_or_above(unit, JERICHO2)) && (header == 3)) {
        /*Check if device is Jericho Plus and header type IP*/
        lp_key = BCM_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY(lp_pcp);
        hp_key = BCM_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY(hp_pcp);
        cosq_map = bcmCosqIngressPortDropIpDscpToPriorityTable;
    } else if ((info.device == 0x8680 || is_device_or_above(unit, JERICHO2)) && (header == 4)) {
        /*Check if device is Jericho Plus and header type MPLS*/
        lp_key = BCM_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY(lp_pcp);
        hp_key = BCM_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY(hp_pcp);
        cosq_map = bcmCosqIngressPortDropMplsExpToPriorityTable;
    } else {
        printf("ERROR: Incorrect value for header type %d", header);
        return BCM_E_PARAM;
    }

    set_clu_prd_port_profile(unit, in_port);

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

    /*Change the TPID value for the MPLS and IP tests*/
    if (header == 3 || header == 4){
        tpid=0x9100;
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

    /*Return traffic back to the IXIA to get stream statistics*/
    if((port_flags & BCM_PORT_ADD_FLEXE_MAC) == 0) {
        /* Exclude force forward setting on FlexE MAC client */
        rv = bcm_port_force_forward_set(unit,in_port,in_port,1);
        if (rv != BCM_E_NONE) {
            printf("Failed(%d) bcm_port_force_forward_set\n", rv);
            return rv;
        }
    }

    /*Enable the NIF PRD feature for the given port!
      If device is JER+ enable hard mode!*/
    rv = bcm_cosq_ingress_port_drop_enable_set(unit, in_port, flags, 1);  
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_enable_set\n", rv);
        return rv;
    }

    bcm_port_prio_config_t priority_config;
    priority_config.nof_priority_groups = 2;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    rv = bcm_port_get(unit, in_port, &flags, &interface_info, &mapping_info);
    if (interface_info.interface != BCM_PORT_IF_ILKN) {
        if (high_priority == BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_TDM) {
            priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerLow;
            priority_config.priority_groups[0].source_priority = BCM_PORT_F_PRIORITY_0  | BCM_PORT_F_PRIORITY_1 | BCM_PORT_F_PRIORITY_2  | BCM_PORT_F_PRIORITY_3;
            priority_config.priority_groups[1].sch_priority = bcmPortNifSchedulerTDM;
            priority_config.priority_groups[1].source_priority = BCM_PORT_F_PRIORITY_TDM;
        } else {
            priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerLow;
            priority_config.priority_groups[0].source_priority = BCM_PORT_F_PRIORITY_0  | BCM_PORT_F_PRIORITY_1;
            priority_config.priority_groups[1].sch_priority = bcmPortNifSchedulerHigh;
            priority_config.priority_groups[1].source_priority = BCM_PORT_F_PRIORITY_2  | BCM_PORT_F_PRIORITY_3;
        }
        priority_config.priority_groups[0].num_of_entries = 4;
        priority_config.priority_groups[1].num_of_entries = -1;
        if(port_flags & BCM_PORT_ADD_FLEXE_MAC) {
            /* FlexE MAC client doesn't allow FIFO size setting */
            priority_config.priority_groups[0].num_of_entries = 0;
            priority_config.priority_groups[1].num_of_entries = 0;
        }
        rv = bcm_port_priority_config_set(unit, in_port, &priority_config);
        if (rv != BCM_E_NONE) {
            printf("Failed(%d) bcm_port_priority_config_set\n", rv);
            return rv;
        }
    }
    printf("cint_nif_priority_config: PASS\n\n");
    return BCM_E_NONE;

}

int cint_nif_priority_control_frame_config(int unit, int in_port,
                             int lp_pcp, int low_priority, uint32 tpid, uint32 header, uint32 control_frame_index, uint32 ether_type_code, uint32 ether_type_code_mask, uint32 ether_type_val)
{

    int rv = BCM_E_NONE;
    uint32 flags = 0;
    int lp_key, hp_key;
    bcm_cosq_ingress_port_drop_map_t cosq_map;
    bcm_info_t info;
    bcm_cosq_ingress_drop_control_frame_config_t control_frame_config;
    uint64 mac_da_val,mac_da_mask;


    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_info_get\n", rv);
        return rv;
    }

    set_clu_prd_port_profile(unit, in_port);

    /* Configure the NIF ports priority drop*/
    /* Header type legend
    0 - VLAN
    1 - ITMH
    2 - HIGIG
    3 - IP
    4 - MPLS
    */

    /*Depending on the header we chose also and the map type
    bcmCosqIngressPortDropTmTcDpPriorityTable = 0,
    bcmCosqIngressPortDropIpDscpToPriorityTable = 1,
    bcmCosqIngressPortDropEthPcpDeiToPriorityTable = 2,
    bcmCosqIngressPortDropMplsExpToPriorityTable = 3
    }*/

    /* Choose the right key constructor macro for the specified header type and the right COSQ table*/
    if (header == 0) {
        lp_key = BCM_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY(lp_pcp,0);
        cosq_map = bcmCosqIngressPortDropEthPcpDeiToPriorityTable;
    } else if (header == 1 || header == 2) {
        lp_key = BCM_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY(lp_pcp,0);
        cosq_map = bcmCosqIngressPortDropTmTcDpPriorityTable;
    } else if (info.device == 0x8680 && header == 3) {
        /*Check if device is Jericho Plus and header type IP*/
        lp_key = BCM_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY(lp_pcp);
        cosq_map = bcmCosqIngressPortDropIpDscpToPriorityTable;
    } else if (info.device == 0x8680 && header == 4) {
        /*Check if device is Jericho Plus and header type MPLS*/
        lp_key = BCM_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY(lp_pcp);
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

    /* set ether type value oxc0c0 for protocol code 2 (part of key for control frame) */
    rv = bcm_cosq_ingress_port_drop_custom_ether_type_set (unit, in_port, flags,  ether_type_code, ether_type_val);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_custom_ether_type_set \n", rv);
        return rv;
    }

    /* The pkt will be treated as control frame when dmac is 0x112233445566 and protocol code is 2 (the corresponding ether type value is 0xc0c0) */
    COMPILER_64_SET(control_frame_config.mac_da_val,0x1122,0x33445566);
    COMPILER_64_SET(control_frame_config.mac_da_mask,0xffff,0xffffffff);
    control_frame_config.ether_type_code=ether_type_code;
    control_frame_config.ether_type_code_mask=ether_type_code_mask;
    rv = bcm_cosq_ingress_port_drop_control_frame_set(unit,in_port,flags,control_frame_index,&control_frame_config);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_control_frame_set\n", rv);
        return rv;
    }

    /*Change the TPID value for the MPLS and IP tests*/
    if (header == 3 || header == 4){
        tpid=0x9100;
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

    /*Return traffic back to the IXIA to get stream statistics*/
    rv = bcm_port_force_forward_set(unit,in_port,in_port,1);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_port_force_forward_set\n", rv);
        return rv;
    }

    /*Enable the NIF PRD feature for the given port!
      If device is JER+ enable hard mode!*/
    rv = bcm_cosq_ingress_port_drop_enable_set(unit, in_port, flags, 1);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_enable_set\n", rv);
        return rv;
    }

    printf("cint_nif_priority_control_frame_config: PASS\n\n");
    return BCM_E_NONE;

}

uint32 eth_type_header_size = -1;
bcm_cosq_ingress_port_drop_flex_key_base_t user_offset_base = bcmCosqIngressPortDropFlexKeyBaseEndOfEthHeader;

int cint_nif_priority_config_soft_stage(int unit, int in_port,
                             int lp_pcp, int low_priority,
                             int hp_pcp, int high_priority, uint32 tpid,
                             uint32 header, uint32 ether_type_val,
                             uint32 lp_match_key, uint32 hp_match_key, uint32 offset)
{

    int rv = BCM_E_NONE;
    uint32 port_flags, flags = 0;
    int lp_key, hp_key;
    bcm_cosq_ingress_port_drop_map_t cosq_map;
    int is_jer_plus, is_dnx;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;

    rv = bcm_port_get(unit, in_port, &port_flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_port_get\n", rv);
        return rv;
    }

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_device_member_get\n", rv);
        return rv;
    }

    rv = is_device_jericho_plus_only(unit, &is_jer_plus);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) is_device_jericho_plus_only\n", rv);
        return rv;
    }

    if (!(is_dnx || is_jer_plus)) {
        printf("NIF PRD Soft Stage is supported only for Jer plus or Jr2 and above! \n", rv);
        return BCM_E_UNAVAIL;
    }

    set_clu_prd_port_profile(unit, in_port);

    /* Configure the NIF ports priority drop*/

    /* Header type legend
    0 - VLAN
    1 - ITMH
    2 - HIGIG
    3 - IP
    4 - MPLS
    */ 

    /*Depending on the header we chose also and the map type
    bcmCosqIngressPortDropTmTcDpPriorityTable = 0,
    bcmCosqIngressPortDropIpDscpToPriorityTable = 1,
    bcmCosqIngressPortDropEthPcpDeiToPriorityTable = 2,
    bcmCosqIngressPortDropMplsExpToPriorityTable = 3
    }*/

    /* Choose the right key constructor macro for the specified header type and the right COSQ table*/
    if (header == 0) {
        lp_key = BCM_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY(lp_pcp,0);
        hp_key = BCM_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY(hp_pcp,0);
        cosq_map = bcmCosqIngressPortDropEthPcpDeiToPriorityTable;
    } else if (header == 1 || header == 2) {
        lp_key = BCM_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY(lp_pcp,0);
        hp_key = BCM_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY(hp_pcp,0);
        cosq_map = bcmCosqIngressPortDropTmTcDpPriorityTable;
    } else if (info.device == 0x8680 && header == 3) {
        /*Check if device is Jericho Plus and header type IP*/
        lp_key = BCM_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY(lp_pcp);
        hp_key = BCM_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY(hp_pcp);
        cosq_map = bcmCosqIngressPortDropIpDscpToPriorityTable;
    } else if (info.device == 0x8680 && header == 4) {
        /*Check if device is Jericho Plus and header type MPLS*/
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

    /* Configure ConfigurableEthType1(code 2) for the given port with value 0x1234 */
    rv = bcm_cosq_ingress_port_drop_custom_ether_type_set ( unit, in_port, flags,  2, ether_type_val);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_custom_ether_type_set \n", rv);
        return rv;
    }

    /* Set the rules for constructing a key on this port */
    bcm_cosq_ingress_drop_flex_key_construct_id_t key_id;
    key_id.port = in_port;
    key_id.ether_type_code = 2;

    /* Construct the key of from the given offset value*/
    bcm_cosq_ingress_drop_flex_key_construct_t flex_key_config;
    flex_key_config.offset_base = user_offset_base;
    flex_key_config.ether_type_header_size = eth_type_header_size;
    flex_key_config.array_size = 4;
    flex_key_config.offset_array[0] = offset;
    flex_key_config.offset_array[1] = offset+1;
    flex_key_config.offset_array[2] = offset+2;
    flex_key_config.offset_array[3] = offset+3;

    rv = bcm_cosq_ingress_port_drop_flex_key_construct_set ( unit, key_id, flags, flex_key_config);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_flex_key_construct_set \n", rv);
        return rv;
    }

    /* Override the priority of the two streams*/
    bcm_cosq_ingress_drop_flex_key_t ether_code;
    ether_code.value = 2;
    ether_code.mask = 0xF;
    /* Check for match only the first part of the key - flex_key_config.offset_array[0].
       The other offset bytes are around the one that is checked so this
       reasuers us that if we have a match we checked the correct byte */
    int num_key_fields = 1;

    /* Add low prio key to the TCAM */
    /* If match - set to priority 2 */
    bcm_cosq_ingress_drop_flex_key_entry_t lp_flex_key_info;

    bcm_cosq_ingress_drop_flex_key_t lp_key_field_match;
    lp_key_field_match.value = lp_match_key;
    lp_key_field_match.mask = 0xFF;

    lp_flex_key_info.ether_code = ether_code;
    lp_flex_key_info.num_key_fields = num_key_fields;
    lp_flex_key_info.priority = high_priority;
    lp_flex_key_info.key_fields[0] = lp_key_field_match;

    uint32 lp_key_index = 10;

    rv = bcm_cosq_ingress_port_drop_flex_key_entry_set( unit, in_port, flags, lp_key_index, lp_flex_key_info);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_flex_key_entry_set\n", rv);
        return rv;
    }

    /* Add high prio key to the TCAM */
    /* If match - set to priority 0 */
    bcm_cosq_ingress_drop_flex_key_entry_t hp_flex_key_info;

    bcm_cosq_ingress_drop_flex_key_t hp_key_field_match;
    hp_key_field_match.value = hp_match_key;
    hp_key_field_match.mask = 0xFF;

    hp_flex_key_info.ether_code = ether_code;
    hp_flex_key_info.num_key_fields = num_key_fields;
    hp_flex_key_info.priority = low_priority;

    hp_flex_key_info.key_fields[0] = hp_key_field_match;

    uint32 hp_key_index = 20;
    rv = bcm_cosq_ingress_port_drop_flex_key_entry_set( unit, in_port, flags, hp_key_index, hp_flex_key_info);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_flex_key_entry_set\n", rv);
        return rv;
    }

    /*Return traffic back to the IXIA to get stream statistics*/
    if((port_flags & BCM_PORT_ADD_FLEXE_MAC) == 0) {
        rv = bcm_port_force_forward_set(unit,in_port,in_port,1);
        if (rv != BCM_E_NONE) {
            printf("Failed(%d) bcm_port_force_forward_set\n", rv);
            return rv;
        }
    }

    /*Enable the NIF PRD soft stage feature for the given port!*/
    rv = bcm_cosq_ingress_port_drop_enable_set(unit, in_port, flags, 2);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_enable_set\n", rv);
        return rv;
    }

    bcm_port_prio_config_t priority_config;
    priority_config.nof_priority_groups = 2;
    if (interface_info.interface != BCM_PORT_IF_ILKN) {
        priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerLow;
        priority_config.priority_groups[0].source_priority = BCM_PORT_F_PRIORITY_0  | BCM_PORT_F_PRIORITY_1;
        priority_config.priority_groups[0].num_of_entries = 4;
        priority_config.priority_groups[1].sch_priority = bcmPortNifSchedulerHigh;
        priority_config.priority_groups[1].source_priority = BCM_PORT_F_PRIORITY_2  | BCM_PORT_F_PRIORITY_3;
        priority_config.priority_groups[1].num_of_entries = -1;
        if(port_flags & BCM_PORT_ADD_FLEXE_MAC) {
            /* FlexE MAC client doesn't allow FIFO size setting */
            priority_config.priority_groups[0].num_of_entries = 0;
            priority_config.priority_groups[1].num_of_entries = 0;
        }
        rv = bcm_port_priority_config_set(unit, in_port, &priority_config);
        if (rv != BCM_E_NONE) {
            printf("Failed(%d) bcm_port_priority_config_set\n", rv);
            return rv;
        }
    }
    printf("cint_nif_priority_config_soft_stage: PASS\n\n");
    return BCM_E_NONE;

}

int cint_nif_priority_config_for_ilkn(int unit, int in_port, int ilkn_port,
                                      bcm_mac_t mac_address, int vid,
                                      int lp_pcp, int low_priority,
                                      int hp_pcp, int high_priority, uint32 tpid, uint32 header, int is_both_tpid_set_equal_pkt_tpid)
{

    int rv = BCM_E_NONE;
    uint32 flags = 0;
    int lp_key, hp_key, untagged_pcp = 6;
    bcm_cosq_ingress_port_drop_map_t cosq_map;
    bcm_info_t info;
    bcm_gport_t in_sys_port;
    bcm_gport_t ilkn_sys_port;
    bcm_port_nif_prio_t nif_priority;
    bcm_pbmp_t affected_ports;

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_info_get\n", rv);
        return rv;
    }

    if (!((info.device == 0x8680) || (info.device == 0x8470))) {
    printf("NIF PRD for ILKN is supported only for Jericho plus and QAX! \n", rv);
    return BCM_E_UNIT;
    }

    /* Configure the NIF ports priority drop*/

    /* Header type legend
    0 - VLAN
    1 - ITMH
    2 - HIGIG
    3 - IP
    4 - MPLS
    */

    /*Depending on the header we chose also and the map type
    bcmCosqIngressPortDropTmTcDpPriorityTable = 0,
    bcmCosqIngressPortDropIpDscpToPriorityTable = 1,
    bcmCosqIngressPortDropEthPcpDeiToPriorityTable = 2,
    bcmCosqIngressPortDropMplsExpToPriorityTable = 3
    }*/

    /* Choose the right key constructor macro for the specified header type and the right COSQ table*/
    if (header == 0) {
        lp_key = BCM_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY(lp_pcp,0);
    if (is_both_tpid_set_equal_pkt_tpid) {
        hp_key = BCM_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY(hp_pcp,0);
    } else {
        rv = bcm_cosq_control_set(unit,in_port,-1,bcmCosqControlIngressPortDropUntaggedPCP,untagged_pcp);
        if (rv != BCM_E_NONE) {
                printf("Failed(%d) bcm_cosq_control_set\n", rv);
                return rv;
            }

        rv = bcm_cosq_control_set(unit,ilkn_port,-1,bcmCosqControlIngressPortDropUntaggedPCP,untagged_pcp);
        if (rv != BCM_E_NONE) {
                printf("Failed(%d) bcm_cosq_control_set\n", rv);
                return rv;
            }
    } 
        cosq_map = bcmCosqIngressPortDropEthPcpDeiToPriorityTable;
    } else if (header == 1 || header == 2) {
        lp_key = BCM_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY(lp_pcp,0);
        hp_key = BCM_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY(hp_pcp,0);
        cosq_map = bcmCosqIngressPortDropTmTcDpPriorityTable;
    } else if (info.device == 0x8680 && header == 3) {
        /*Check if device is Jericho Plus and header type IP*/
        lp_key = BCM_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY(lp_pcp);
        hp_key = BCM_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY(hp_pcp);
        cosq_map = bcmCosqIngressPortDropIpDscpToPriorityTable;
    } else if (info.device == 0x8680 && header == 4) {
        /*Check if device is Jericho Plus and header type MPLS*/
        lp_key = BCM_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY(lp_pcp);
        hp_key = BCM_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY(hp_pcp);
        cosq_map = bcmCosqIngressPortDropMplsExpToPriorityTable;
    } else {
        printf("ERROR: Incorrect value for header type %d", header);
        return BCM_E_PARAM;
    }

    /*Map the created lp_key to low priority*/
    rv = bcm_cosq_ingress_port_drop_map_set(unit,ilkn_port,flags,cosq_map,lp_key,low_priority);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_map_set\n", rv);
        return rv;
    }

    /*Map the created hp_key to high priority*/
    if (is_both_tpid_set_equal_pkt_tpid == 0 && header == 0) {
        rv = bcm_cosq_ingress_port_drop_map_set(unit,ilkn_port,flags,cosq_map,untagged_pcp|0x1000000,high_priority);
        if (rv != BCM_E_NONE) {
            printf("Failed(%d) bcm_cosq_ingress_port_drop_map_set\n", rv);
            return rv;
        }
    } else {
        rv = bcm_cosq_ingress_port_drop_map_set(unit,ilkn_port,flags,cosq_map,hp_key,high_priority);
        if (rv != BCM_E_NONE) {
            printf("Failed(%d) bcm_cosq_ingress_port_drop_map_set\n", rv);
            return rv;
        }
    }
 
    /*Set TPID for the ports*/
    rv = bcm_cosq_control_set(unit,in_port,-1,bcmCosqControlIngressPortDropTpid1,tpid);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_control_set\n", rv);
        return rv;
    }

    rv = bcm_cosq_control_set(unit,ilkn_port,-1,bcmCosqControlIngressPortDropTpid1,tpid);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_control_set\n", rv);
        return rv;
    }

    /*Set 1 byte as a threshold level for the FIFO for packets mapped to low_priority*/
    rv = bcm_cosq_ingress_port_drop_threshold_set(unit,ilkn_port,flags,low_priority,1);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_threshold_set\n", rv);
        return rv;
    }
    /* Set the NIF priority of the IN_PORT to be higher than that of the ILKN_PORT*/
    /* In order to not receive drops at least until we reach tx of the ILNK_PORT*/
    nif_priority.priority_level=1;
    rv =  bcm_port_nif_priority_set( unit, in_port,  flags, nif_priority, affected_ports);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_threshold_set\n", rv);
        return rv;
    }
    nif_priority.priority_level=0;
    rv =  bcm_port_nif_priority_set( unit, ilkn_port,  flags, nif_priority, affected_ports);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_threshold_set\n", rv);
        return rv;
    }

       /*****************************************/
      /**  Configure snake from the IXIA port **/
     /***   to the ILKN port and back again ***/
    /*****************************************/

    if (is_both_tpid_set_equal_pkt_tpid == 0 && header == 0) {
        rv = bcm_port_force_forward_set(unit,in_port,ilkn_port,1);
        if (rv != BCM_E_NONE) {
            printf("Failed(%d) bcm_port_force_forward_set\n", rv);
            return rv;
        }

        rv = bcm_port_loopback_set(unit,ilkn_port,BCM_PORT_LOOPBACK_PHY);
        if (rv != BCM_E_NONE) {
            printf("Failed(%d) bcm_port_loopback_set\n", rv);
            return rv;
        }

        rv = bcm_port_force_forward_set(unit,ilkn_port,in_port,1);
            if (rv != BCM_E_NONE) {
            printf("Failed(%d) bcm_port_force_forward_set\n", rv);
            return rv;
        }
    } else {
    rv = cint_vlan_nif_snake_test_on_two_ports( unit, mac_address, vid, in_port, ilkn_port);
        if (rv != BCM_E_NONE) {
            printf("Failed(%d) cint_vlan_nif_snake_test_on_two_ports\n", rv);
            return rv;
        }
    }
    
    /*Enable the NIF PRD hard mode for both ports!*/
    rv = bcm_cosq_ingress_port_drop_enable_set(unit, ilkn_port, flags, 1);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_enable_set\n", rv);
        return rv;
    }

    printf("cint_nif_priority_config_for_ilkn: PASS\n\n");
    return BCM_E_NONE;

}

int cint_nif_prd_port_set_all_to_default_priority(int unit,int port,uint32 def_prio,
                                                   uint32 ignore_headers, uint32 tpid_mismatch, uint32 tpid_value)
{

    int rv = BCM_E_NONE;
    uint32 flags = 0;
    int is_jer_plus, is_dnx;

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_device_member_get\n", rv);
        return rv;
    }

    rv = is_device_jericho_plus_only(unit, &is_jer_plus);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) is_device_jericho_plus_only\n", rv);
        return rv;
    }

    if (!(is_dnx || is_jer_plus)) {
        printf("NIF PRD Soft Stage is supported only for Jer plus or Jr2 and above! \n", rv);
        return BCM_E_UNAVAIL;
    }

    /* Set default priority*/
    rv = bcm_cosq_ingress_port_drop_default_priority_set (unit, port, flags, def_prio);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_default_priority_set\n", rv);
        return rv;
    }

    /* If we use the cint for the TPID mismatch test*/
    /* We change the expected TPID value for the second port*/
    if (tpid_mismatch) {
        rv = bcm_cosq_control_set(unit,port,-1,bcmCosqControlIngressPortDropTpid1,tpid_value);
        if (rv != BCM_E_NONE) {
            printf("Failed(%d) bcm_cosq_control_set\n", rv);
            return rv;
        }
    }

    if (ignore_headers) {

          /**Ignore all header information **/
         /** in order to receive  default **/
        /**    priority in second_port   **/

        /* bcm_cosq_control_t{
        bcmCosqControlIngressPortDropIgnoreIpDscp = 143, cosq ingress port drop ignore IP DSCP.
        bcmCosqControlIngressPortDropIgnoreMplsExp = 144,  cosq ingress port drop ignore MPLS EXP. 
        bcmCosqControlIngressPortDropIgnoreInnerTag = 145, cosq ingress port drop ignore inner VLAN tag.
        bcmCosqControlIngressPortDropIgonreOuterTag = 146,  cosq ingress port drop ignore outer
            } */

        rv = bcm_cosq_control_set (unit, port, -1, bcmCosqControlIngressPortDropIgnoreIpDscp, 1);
        if (rv != BCM_E_NONE) {
            printf("Failed(%d) bcm_cosq_control_set\n", rv);
            return rv;
        }

        rv = bcm_cosq_control_set (unit, port, -1, bcmCosqControlIngressPortDropIgnoreMplsExp, 1);
        if (rv != BCM_E_NONE) {
            printf("Failed(%d) bcm_cosq_control_set\n", rv);
            return rv;
        }

        rv = bcm_cosq_control_set (unit, port, -1, bcmCosqControlIngressPortDropIgnoreInnerTag, 1);
        if (rv != BCM_E_NONE) {
            printf("Failed(%d) bcm_cosq_control_set\n", rv);
            return rv;
        }

        rv = bcm_cosq_control_set (unit, port, -1, bcmCosqControlIngressPortDropIgonreOuterTag, 1);
        if (rv != BCM_E_NONE) {
            printf("Failed(%d) bcm_cosq_control_set\n", rv);
            return rv;
        }
    }

    printf("cint_nif_prd_port_set_all_to_default_priority: PASS\n\n");
    return BCM_E_NONE;

}

uint64 cint_port_dropped_packets_get (int unit, int port) {
    int rv = BCM_E_NONE;
    uint64 dropped_cnt;

    rv = bcm_stat_get(unit,port,snmpEtherStatsDropEvents,&dropped_cnt);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_stat_get \n", rv);
        return rv;
    }

    return dropped_cnt;
}

int set_clu_prd_port_profile(int unit, int in_port) {
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    int rv;
    uint32 flags;

    rv = bcm_port_get(unit, in_port, &flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_get\n");
        return rv;
    }

    portmod_dispatch_type_t port_type;
    cint_port_phy_type_get(unit, in_port, &port_type);
    if(interface_info.interface == BCM_PORT_IF_NIF_ETH && port_type == portmodDispatchTypePm4x25) {
        rv = bcm_cosq_control_set(unit, in_port, -1, bcmCosqControlIngressPortDropPortProfileMap, in_port % 4);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_cosq_control_set for bcmCosqControlIngressPortDropPortProfileMap\n");
            return rv;
        }
    }

    if(flags & BCM_PORT_ADD_FLEXE_MAC) {
        rv = bcm_cosq_control_set(unit, in_port, -1, bcmCosqControlIngressPortDropPortProfileMap, in_port % 8);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_cosq_control_set for bcmCosqControlIngressPortDropPortProfileMap\n");
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * brief - set tdm indication using prd for a given port and L2 tc,dei
 */
int cint_nif_priority_l2_tdm_set(int unit, int in_port, int pcp, int dei)
{

    int rv = BCM_E_NONE;
    uint32 flags = 0;

    /* map pcp and dei to tdm */
    rv = bcm_cosq_ingress_port_drop_map_set(unit, in_port, flags, bcmCosqIngressPortDropEthPcpDeiToPriorityTable, BCM_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY(pcp, dei), BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_TDM);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_map_set\n", rv);
        return rv;
    }

    /*Set TPID for the port*/
    rv = bcm_cosq_control_set(unit,in_port,-1,bcmCosqControlIngressPortDropTpid1,0x8100);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_control_set\n", rv);
        return rv;
    }

    /*Enable hard stage PRD!*/
    rv = bcm_cosq_ingress_port_drop_enable_set(unit, in_port, flags, bcmCosqIngressPortDropEnableHardStage);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_enable_set\n", rv);
        return rv;
    }

    printf("cint_nif_priority_l2_tdm_set: PASS\n\n");
    return BCM_E_NONE;
}
