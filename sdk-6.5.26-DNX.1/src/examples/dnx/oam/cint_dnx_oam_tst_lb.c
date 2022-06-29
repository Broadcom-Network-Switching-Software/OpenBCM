/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * Purpose: Example of using SAT APIs specific for Jericho2
 *
 * Usage:
 * Test Scenario
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_multicast.c
 * cint ../../src/./examples/dnx/cint_dnx_advanced_vlan_translation_mode.c
 * cint ../../src/./examples/dnx/cint_dnx_vswitch_metro_mp.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_oam.c
 * cint ../../src/./examples/dnx/oam/cint_dnx_oam.c
 * cint ../../src/./examples/dnx/oam/cint_oam_basic.c
 * cint ../../src/./examples/dnx/oam/cint_oam_action.c
 * cint ../../src/./examples/dnx/crps/cint_crps_oam_config.c
 * cint ../../src/./examples/dnx/oam/cint_dnx_oam_tst_lb.c
 * cint
 * dnx_oam_lb_tst_sat_run(0,201,202,203,1,0,0);
 * exit;
 *
 * ETH OAM LBM TX:
 *   Received packets on unit 0 should be:
 *   Data: 0x5678efcd00000001020304018100200a890240030004000001c2030024abcdeeffabcdeeffabcdeeffabcdeeffabcdeeffabcdeeffabcdeeffabcdeeffabcdeeff00
 *
 * ETH OAM LBM RX and reflector:
 *   tx 1 psrc=201 data=0x0000000102ff0005040302018100000a890240030004000000120000000000000000000001000000000000000000020000000000000000000300000000000000000004
 *   Received packets on unit 0 should be:
 *   Data: 0x0005040302010000000102ff8100000a890240020004000000120000000000000000000001000000000000000000020000000000000000000300000000000000000004
 *
 * ETH OAM LBR RX, expecting counters to get updated:
 *   tx 1 psrc=201 data=0x0000000102ff0005040302018100000a890240020004000000120020002400000000000000000000000000000000000000000000000000000000000000000000000000
 *
*/

/*
 * Structure for returned information
 */
struct dnx_oam_lb_tst_s
{
    /* Created gtf id */
    int global_gtf_id;
    /* Created ctf id */
    int global_ctf_id;
};

/*
 * Returned information
 */
dnx_oam_lb_tst_s dnx_oam_lb_tst_glb = {
    /* gtf id */
    0,
    /* ctf id */
    0
};

/**
*  An example of creating LB and TST function base on SAT
* 
*  1, Create oam action profile
*  2, Set profiles to for lif gport
*  3, configure PMF rule to update lb/tst packet destnation to SAT port.
* 
* @author xiao (09/02/2015)
* 
* @param unit 
* @param is_upmep indicate upmep or downmep
* @param ing_lif_profile_id ingress lif profile id
* @param egr_lif_profile_id egress lif profile id
* @param sat_port SAT port to receive lb/tst packet
* @param lb_tst_flag 1 for LB packet, 2 for TST packet
* @param mep_id mep ID
* @return int 
*/
int dnx_oam_set_tst_trap(
    int unit, int is_upmep, 
    bcm_oam_profile_t ing_lif_profile_id, bcm_oam_profile_t egr_lif_profile_id,
    bcm_gport_t sat_port, int lb_tst_flag, int mep_id, int ctf_id)
{
    int rv = BCM_E_NONE;
    void *dest_char;
    bcm_field_context_info_t context_info;
    bcm_field_context_param_info_t context_param;
    bcm_field_context_t context_id;
    bcm_field_presel_entry_data_t presel_data;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_rx_trap_config_t trap_config;
    int count;
    uint32 flags = 0;
    int down_mep_sat_trap_id;
    int up_mep_sat_trap_id;
    int up_mep_ingress_trap_id;
    int up_mep_stamp_trap;	  
    int trap_id = 0;
    bcm_oam_action_key_t action_key;
    bcm_oam_action_result_t action_result;
    bcm_oam_endpoint_info_t endpoint_info;
    bcm_oam_profile_t enc_ing_lif_profile_id, enc_egr_lif_profile_id;
    bcm_oam_profile_t acc_ing_action_profile_id;
    bcm_gport_t gport;
    bcm_sat_ctf_identifier_t identifier;
    int session_id;
    int trap_qualifier = 0;
    int tc = 0;
    int dp = 0;
    int tc_max = 7;
    int dp_max = 3;

    /*
     * 1, Create SAT trap of up or down mep for lb/tst 
     */
    if (is_upmep)
    {
        rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapEgTxOamUpMEPDest2, &up_mep_sat_trap_id);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("Error. bcm_rx_trap_type_create failed(%s) \n", bcm_errmsg(rv));
        return rv;
        }
        trap_id = up_mep_sat_trap_id & 0x1FF;

        rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &up_mep_ingress_trap_id);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("Error. bcm_rx_trap_type_create failed(%s) \n", bcm_errmsg(rv));
            return rv;
        }
    }
    else {
       rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &down_mep_sat_trap_id);
       if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
           printf("Error. bcm_rx_trap_type_create failed(%s) \n", bcm_errmsg(rv));
           return rv;
       }
       trap_id = down_mep_sat_trap_id & 0x1FF;
    }
    printf("  trap id %d \n", trap_id);

    rv = bcm_oam_endpoint_get(unit, mep_id, &endpoint_info);
    if (rv != BCM_E_NONE) {
        printf("Error. bcm_oam_endpoint_get failed(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config);
    /*config dest port to SAT port.*/
    trap_config.dest_port = sat_port;
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
    if (!is_upmep && (endpoint_info.type == bcmOAMEndpointTypeEthernet))
    {
        trap_config.flags |= BCM_RX_TRAP_UPDATE_EGRESS_FWD_INDEX;
        trap_config.egress_forwarding_index = 1;
    }
    rv = bcm_rx_trap_set(unit, is_upmep ? up_mep_ingress_trap_id : down_mep_sat_trap_id, trap_config);
    if (rv != BCM_E_NONE) {
        printf("Error. bcm_rx_trap_set failed(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    if(is_upmep)
    {
        bcm_rx_trap_config_t_init(&trap_config);
        /** Ingress trap strength range 0-15 */
        BCM_GPORT_TRAP_SET(trap_config.cpu_trap_gport, up_mep_ingress_trap_id, 15, 0);
        /** Trap stamped on FHEI header */
        rv = bcm_rx_trap_set(unit, up_mep_sat_trap_id, trap_config);
        if (rv != BCM_E_NONE) {
            printf("Error. bcm_rx_trap_set failed(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        /*set stamped_trap_code in FHEI*/
        rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapEgTxOamUpMEPDest1, &up_mep_stamp_trap);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("Error. bcm_rx_trap_type_create failed(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        rv = bcm_rx_trap_type_get(unit, flags, bcmRxTrapEgTxOamUpMEPDest1, &up_mep_stamp_trap);
        if (rv != BCM_E_NONE) {
            printf("Error. bcm_rx_trap_type_get failed(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        rv = bcm_rx_trap_get(unit, up_mep_stamp_trap, &trap_config);
        if (rv != BCM_E_NONE) {
            printf("Error. bcm_rx_trap_get failed(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        trap_config.stamped_trap_code =  up_mep_sat_trap_id & 0x1FF;
        rv = bcm_rx_trap_set(unit, up_mep_stamp_trap, trap_config);
        if (rv != BCM_E_NONE) {
            printf("Error. bcm_rx_trap_set failed(%s) \n", bcm_errmsg(rv));
            return rv;
        }
    
        bcm_field_context_info_t_init(&context_info);
        dest_char = &(context_info.name[0]);
        sal_strncpy_s(dest_char, "OAM_No_Header", sizeof(context_info.name));
        rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id);
        if (rv != BCM_E_NONE) {
            printf("Error. bcm_field_context_create failed(%s) \n", bcm_errmsg(rv));
            return rv;
        }
    
        /** Changing new context SYS HDR profile to None (No SYS HDRs are added) */
        context_param.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
        context_param.param_arg = 0;
        context_param.param_val = bcmFieldSystemHeaderProfileNone;
        rv = bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF1, context_id, &context_param);
        if (rv != BCM_E_NONE) {
            printf("Error. bcm_field_context_param_set failed(%s) \n", bcm_errmsg(rv));
            return rv;
        }
    
        context_param.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
        context_param.param_arg = 0;
        context_param.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerOffset0, 0);
        /**Context that was created for iPMF1 is also created for iPMF2*/
        rv = bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, context_id, &context_param);
        if (rv != BCM_E_NONE) {
            printf("Error. bcm_field_context_param_set failed(%s) \n", bcm_errmsg(rv));
            return rv;
        }
    
        /** Adding trap_code preselector for the context */
        presel_entry_id.presel_id = 4;
        presel_entry_id.stage = bcmFieldStageIngressPMF1;
    
        presel_data.entry_valid = TRUE;
        presel_data.context_id = context_id;
        presel_data.nof_qualifiers = 1;
        presel_data.qual_data[0].qual_type = bcmFieldQualifyRxTrapCode;
        presel_data.qual_data[0].qual_value = up_mep_ingress_trap_id;
        presel_data.qual_data[0].qual_mask = 0x1FF;
    
        rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_data);
        if (rv != BCM_E_NONE) {
            printf("Error. bcm_field_presel_set failed(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        printf("Up mep trap Done\n");
    }

    /*
     * 2, Create oam action profile
     */
    rv = bcm_oam_profile_create(unit, 0, bcmOAMProfileEgressLIF, &egr_lif_profile_id);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("Error. bcm_oam_profile_create failed(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_oam_profile_create(unit, 0, bcmOAMProfileIngressLIF, &ing_lif_profile_id);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("Error. bcm_oam_profile_create failed(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    gport = endpoint_info.gport;
    flags = 0;
    bcm_oam_action_key_t_init(&action_key);
    if (endpoint_info.type == bcmOAMEndpointTypeEthernet)
    {
        action_key.dest_mac_type = bcmOAMDestMacTypeMyCfmMac;
    }
    else if ((endpoint_info.type == bcmOAMEndpointTypeBHHMPLS) || (endpoint_info.type == bcmOAMEndpointTypeBHHPwe))
    {
        action_key.dest_mac_type = bcmOAMDestMacTypeUknownUcast;
    }
    else
    {
        printf("TST just support in ETH / MPLS/PWE");
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(bcm_oam_profile_id_get_by_type(unit, ing_lif_profile_id, bcmOAMProfileIngressLIF, &flags, &enc_ing_lif_profile_id));
    BCM_IF_ERROR_RETURN(bcm_oam_profile_id_get_by_type(unit, egr_lif_profile_id, bcmOAMProfileEgressLIF, &flags, &enc_egr_lif_profile_id));
    BCM_IF_ERROR_RETURN(bcm_oam_lif_profile_set(unit, 0, gport, enc_ing_lif_profile_id, (endpoint_info.type == bcmOAMEndpointTypeBHHPwe)? BCM_OAM_PROFILE_INVALID : enc_egr_lif_profile_id));
    
    acc_ing_action_profile_id = endpoint_info.acc_profile_id & 0xff;
    bcm_oam_profile_create(unit, BCM_OAM_PROFILE_WITH_ID, bcmOAMProfileIngressAcceleratedEndpoint,&acc_ing_action_profile_id);
    acc_ing_action_profile_id = endpoint_info.acc_profile_id;
    
    /*
     * Prepare action to redirect TST to SAT (on OAMP)
     */
    action_key.flags = 0;
    action_key.inject = 0;
    if (lb_tst_flag & 0x1)
    {
        action_key.opcode = bcmOamOpcodeLBR;
    }
    else if (lb_tst_flag & 0x2)
    {
        action_key.opcode = bcmOamOpcodeTST;
    }
    else
    {
        printf("error lb_tst_flag\n");
        return BCM_E_PARAM;
    }
    action_key.endpoint_type = bcmOAMMatchTypeMEP;
    
    /*
     * Set action destination and set the action
     */
    bcm_oam_action_result_t_init(&action_result);
    BCM_GPORT_TRAP_SET(action_result.destination, trap_id, 15, 0);		/* Taken from default values */
    action_result.flags = 0;
    BCM_IF_ERROR_RETURN(bcm_oam_profile_action_set(unit, 0, acc_ing_action_profile_id, &action_key, &action_result));

    /*
     * 3, Config SAT trap, session and identification
     */
    /** add SAT Trap */
    rv = bcm_sat_ctf_trap_add(unit, trap_id);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("Error. bcm_dnx_sat_ctf_trap_add failed(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    /** Map trap qualifier to session id (0~63)  */
    trap_qualifier = mep_id;
    
    rv = bcm_sat_ctf_trap_data_to_session_map(unit, trap_qualifier, 0, session_id);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("Error. bcm_sat_ctf_trap_data_to_session_map failed(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    bcm_sat_ctf_identifier_t_init(&identifier);
    identifier.session_id = session_id;
    identifier.trap_id = trap_id;
    /**all dp&tc could be identified*/
    for (dp = 0; dp <= dp_max; dp++)
    {
        for (tc = 0; tc <= tc_max; tc++)
        {
            identifier.color = dp;
            identifier.tc = tc;
            rv = bcm_sat_ctf_identifier_map(unit, &identifier, ctf_id);
            if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
                printf("Error. bcm_dnx_sat_ctf_identifier_map failed(%s) \n", bcm_errmsg(rv));
                return rv;
            }
        }
    }

    return rv;
}


/*
 * configure SAT starting to send packet.
 */
int dnx_oam_sat_start(int unit, int gtf_id)
 {
    int rv;
    bcm_sat_gtf_bandwidth_t bw1,bw2;
    bcm_sat_gtf_rate_pattern_t rate_pattern1,rate_pattern2;

    bcm_sat_gtf_bandwidth_t_init(&bw1);
    bcm_sat_gtf_bandwidth_t_init(&bw2);
    bcm_sat_gtf_rate_pattern_t_init(&rate_pattern1);
    bcm_sat_gtf_rate_pattern_t_init(&rate_pattern2);

    /*1 Set Bandwidth */
    bw1.rate = 128; /*gtf_bw_cir_rate;*/
    bw1.max_burst = 100;/*gtf_bw_cir_max_burst;*/
    rv = bcm_sat_gtf_bandwidth_set(unit, gtf_id, 0, &bw1);
    if (rv != BCM_E_NONE) {
       printf("Error, in bcm_sat_gtf_bandwidth_set\n");
       return rv;
    }

    /* 2. Set Rate Pattern */
    rate_pattern1.rate_pattern_mode = bcmSatGtfRatePatternContinues; /*gtf_rate_pattern_mode;*/
    rate_pattern1.high_threshold = 10; /*gtf_rate_pattern_high;*/
    rate_pattern1.low_threshold =10 ; /*gtf_rate_pattern_low;*/
    rate_pattern1.stop_iter_count = 1000; /*gtf_rate_pattern_stop_iter_count;*/
    rate_pattern1.stop_burst_count = 10000; /*gtf_rate_pattern_stop_burst_count;*/
    rate_pattern1.stop_interval_count = 10000; /*gtf_rate_pattern_stop_interval_count;*/
    rate_pattern1.burst_packet_weight = 1; /*gtf_rate_pattern_burst_packet_weight;*/
    rate_pattern1.interval_packet_weight = 1; /*gtf_rate_pattern_interval_packet_weight;*/
    rv = bcm_sat_gtf_rate_pattern_set(unit,gtf_id, 0, &rate_pattern1);
    if (rv != BCM_E_NONE) {
       printf("Error, in bcm_sat_gtf_rate_pattern_set\n");
       return rv;
    }

    /* 3. Start transmitting */
    rv = bcm_sat_gtf_packet_start(unit, gtf_id, bcmSatGtfPriCir);
    if (rv != BCM_E_NONE) {
       printf("Error, in bcm_sat_gtf_packet_start\n");
       return rv;
    }

    return rv;
}

/**
  *  An example of creating Loopback and TST function based on SAT
  *
  *  1. create an MEP
  *  2. create a LoopBack or TST function with lb_tst_flag
  *  3. configure SAT to send lb or tst packet.
  *
  * @param unit
  * @param port1
  * @param port2
  * @param port3
  * @param lb_tst_flag: 1 means creating lb, 2 means creating tst
  * @param mep_is_up: 0 - the created mep would be down facing, otherwise up facing.
  * @param lb_pcp_dei_flag: default 0, 1 means test lb with different pcp dei.
  * @return int
  */
int dnx_oam_lb_tst_sat_run(int unit, int port1, int port2, int port3, int lb_tst_flag, int mep_is_up, int lb_pcp_dei_flag)
{
    int rv;
    bcm_oam_endpoint_info_t acc_endpoint; 
    bcm_oam_endpoint_info_t remote_endpoint;
    bcm_oam_endpoint_info_t mep_get;
    bcm_oam_loopback_t lb;
    bcm_oam_tst_rx_t tst_rx_ptr;
    bcm_oam_tst_tx_t tst_tx_ptr;
    int gtf_id = 0;
    int ctf_id = 0;
    uint8 lb_pcp = 5; /* different with ccm packet pri(2)*/
    uint8 lb_dei = 1;
    uint8 lb_inner_pcp = 4; /* different with ccm packet inner pri(7)*/
    uint8 lb_inner_dei = 1;
    uint8 lb_int_pri = 10;

    single_vlan_tag = 1;
    dnx_oam_endpoint_info_glb.port_1 = port1;
    dnx_oam_endpoint_info_glb.port_2 = port2;
    dnx_oam_endpoint_info_glb.port_3 = port3;
    dnx_oam_endpoint_info_glb.vsi = 2;

    /* Create vswitch and AC */
    rv = create_vswitch_and_mac_entries(unit);
    if (rv != BCM_E_NONE) {
        printf("Error in create_vswitch_and_mac_entries\n");
        return rv;
    }

    /* OAM basic example */
    rv = oam_example(unit);
    if (rv != BCM_E_NONE) {
        printf("Error in oam_example\n");
        return rv;
    }

    bcm_oam_loopback_t_init(&lb);
    bcm_oam_tst_rx_t_init(&tst_rx_ptr);
    bcm_oam_tst_tx_t_init(&tst_tx_ptr);
    if(lb_pcp_dei_flag){
        lb_pcp_dei_enable = 1;
        lb.pkt_pri = (lb_pcp* 2) + lb_dei;
        lb.inner_pkt_pri =  (lb_inner_pcp * 2) + lb_inner_dei;
        lb.int_pri= lb_int_pri;
    }

    /* Create lb/tst entries for endpoint */
    tst_tx_ptr.endpoint_id=tst_rx_ptr.endpoint_id=lb.id = (mep_is_up ? dnx_oam_results_glb.ep2.id : dnx_oam_results_glb.ep3.id);
    tst_tx_ptr.peer_da_mac_address[3]= lb.peer_da_mac_address[3] = 0xef;
    tst_tx_ptr.peer_da_mac_address[1]= lb.peer_da_mac_address[1] = 0xab;
    tst_tx_ptr.flags = BCM_OAM_TST_TX_WITH_ID;
    tst_tx_ptr.gtf_id = 3;
    tst_rx_ptr.flags = BCM_OAM_TST_RX_WITH_ID;
    tst_rx_ptr.ctf_id = 3;

    /*lb for config*/
    lb.num_tlvs =1;
    lb.tlvs[0].four_byte_repeatable_pattern = lb_pcp_dei_flag ? 0x0:0xABCDEEFF;
    lb.tlvs[0].tlv_type =bcmOamTlvTypeData;
    lb.tlvs[0].tlv_length =46;
    lb.flags = BCM_OAM_LOOPBACK_WITH_GTF_ID | BCM_OAM_LOOPBACK_WITH_CTF_ID;
    lb.gtf_id = 2;
    lb.ctf_id = 2;

    /*tst config*/
    tst_rx_ptr.expected_tlv.tlv_length = 34; /* tlv length should be longer than 34 */
    tst_rx_ptr.expected_tlv.tlv_type =  bcmOamTlvTypeTestPrbsWithoutCRC;
    tst_tx_ptr.tlv.tlv_length = 34;
    tst_tx_ptr.tlv.tlv_type =  bcmOamTlvTypeTestPrbsWithoutCRC;

    if (1 == lb_tst_flag){ 
        rv = bcm_oam_loopback_add(unit, &lb);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_oam_loopback_add\n");
            return rv;
        }
        gtf_id = lb.gtf_id;
        ctf_id = lb.ctf_id;
        printf("lb gtf %d \n\n",lb.gtf_id);
    }
    else if (2 == lb_tst_flag){
        rv = bcm_oam_tst_rx_add(unit, &tst_rx_ptr);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_oam_tst_rx_add\n");
            return rv;
        }
        rv = bcm_oam_tst_tx_add(unit, &tst_tx_ptr);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_oam_tst_tx_add\n");
            return rv;
        }
        gtf_id = tst_tx_ptr.gtf_id;
        ctf_id = tst_rx_ptr.ctf_id;
        printf("lb_tst gtf %d ctf %d\n\n",tst_tx_ptr.gtf_id,tst_rx_ptr.ctf_id);
     }
    else{
        printf("lb_tst flag error \n");
        return -1;
    }

    /*lb update config*/
    lb.num_tlvs =1;
    lb.tlvs[0].four_byte_repeatable_pattern = lb_pcp_dei_flag? 0x0:0xABCDEEFF;
    lb.tlvs[0].tlv_type =bcmOamTlvTypeData;
    lb.tlvs[0].tlv_length =36;
    lb.flags = BCM_OAM_LOOPBACK_UPDATE;
    tst_tx_ptr.peer_da_mac_address[0] = lb.peer_da_mac_address[0] = 0x56;
    tst_tx_ptr.peer_da_mac_address[1] = lb.peer_da_mac_address[1] = 0x78;
    tst_tx_ptr.peer_da_mac_address[2] = lb.peer_da_mac_address[2] = 0xef;
    tst_tx_ptr.peer_da_mac_address[3] = lb.peer_da_mac_address[3] = 0xcd;
    tst_tx_ptr.peer_da_mac_address[4] = lb.peer_da_mac_address[4] = 0x00;
    tst_tx_ptr.peer_da_mac_address[5] = lb.peer_da_mac_address[5] = 0x00;
    tst_tx_ptr.tlv.tlv_length = 36;
    tst_tx_ptr.tlv.tlv_type =  bcmOamTlvTypeTestPrbsWithCRC;
    tst_tx_ptr.flags = BCM_OAM_TST_TX_UPDATE;
    tst_rx_ptr.expected_tlv.tlv_length = 36;
    tst_rx_ptr.expected_tlv.tlv_type =  bcmOamTlvTypeTestPrbsWithCRC;
    tst_rx_ptr.flags = BCM_OAM_TST_RX_UPDATE;

    /* Update lb configure*/
    if (1 == lb_tst_flag){ 
       rv = bcm_oam_loopback_add(unit, &lb);
       if (rv != BCM_E_NONE) {
           printf("Error in bcm_oam_loopback_add\n");
           return rv;
       }
       gtf_id = lb.gtf_id;
       ctf_id = lb.ctf_id;
       printf("UPDATE_lb gtf %d \n\n",lb.gtf_id);
    }
    else if (2 == lb_tst_flag){
       rv = bcm_oam_tst_rx_add(unit, &tst_rx_ptr);
       if (rv != BCM_E_NONE) {
           printf("Error in bcm_oam_tst_rx_add\n");
           return rv;
       }
       rv = bcm_oam_tst_tx_add(unit, &tst_tx_ptr);
       if (rv != BCM_E_NONE) {
           printf("Error in bcm_oam_tst_tx_add\n");
           return rv;
       }
       gtf_id=tst_tx_ptr.gtf_id;
       ctf_id = tst_rx_ptr.ctf_id;
       printf("UPDATE_tst gtf %d ctf %d\n\n",tst_tx_ptr.gtf_id,tst_rx_ptr.ctf_id);
    }

    dnx_oam_lb_tst_glb.global_gtf_id = gtf_id;
    dnx_oam_lb_tst_glb.global_ctf_id = ctf_id;

    /*start to send lb/tst packet*/
    rv = dnx_oam_sat_start(unit, gtf_id);
    if (rv != BCM_E_NONE) {
        printf("Error in dnx_oam_sat_start\n");
        return rv;
    }

    return rv;
}


/**
 *  An example of creating Loopback and TST function base on MPLS-TP
 *
 *  1.  Create accelerated y.1731 OAM endpoint of type MPLS-TP
 *  2.  Create a LoopBack or TST function MPLS-TP or PWE
 *  3.  Configure SAT to send lb or tst packet.
 *
 * @param unit
 * @param port1 physical ports to be used
 * @param port2
 * @param lb_tst_flag: 1 means creating lb, 2 means creating tst
 * @param mpls_pwe_flag: 1 means mpls, 2 means pwe, 3 means MPLS-TP Section OAM
 * @return int 
 */
int dnx_oam_run_lb_tst_mpls_pwe(int unit, int port1, int port2, int lb_tst_flag, int mpls_pwe_flag)
{
    int rv = BCM_E_NONE;
    bcm_oam_loopback_t lb;
    int gtf_id = 0;
    bcm_oam_tst_rx_t tst_rx_ptr;
    bcm_oam_tst_tx_t tst_tx_ptr;

    if(mpls_pwe_flag == 1) { /*mpls*/
        rv= oam_run_with_defaults_mpls_tp(unit, port1, port2, 0);
        if (rv != BCM_E_NONE) {
           printf("Error, in oam_run_with_defaults_mpls_tp\n");
           return rv;
        }
   } else if(mpls_pwe_flag == 2) {/*pwe*/
        rv= oam_run_with_defaults_pwe(unit, port1, port2, 0);
        if (rv != BCM_E_NONE) {
           printf("Error, in oam_run_with_defaults_pwe\n");
           return rv;
        }
    } else {/*MPLS-TP Section */
        rv= oam_run_with_defaults_section(unit, port1, port2, 0);
        if (rv != BCM_E_NONE) {
           printf("Error, in oam_run_with_defaults_section\n");
           return rv;
        }
    }

    bcm_oam_loopback_t_init(&lb);
    bcm_oam_tst_rx_t_init(&tst_rx_ptr);
    bcm_oam_tst_tx_t_init(&tst_tx_ptr);

    tst_tx_ptr.endpoint_id=tst_rx_ptr.endpoint_id = lb.id = dnx_mpls_oam_results_glb.mep_acc_info.id;

    if (1 == lb_tst_flag){
        lb.num_tlvs =1;
        lb.tlvs[0].four_byte_repeatable_pattern =0x0;
        lb.tlvs[0].tlv_type =bcmOamTlvTypeData;
        lb.tlvs[0].tlv_length =36;
        lb.peer_da_mac_address[1] = 0xab;
        rv = bcm_oam_loopback_add(unit,&lb);
        if (rv != BCM_E_NONE) {
           printf("Error, in bcm_oam_loopback_add\n");
           return rv;
        }

        gtf_id = lb.gtf_id;
        printf("MEP id %d, lb gtf %d \n\n", dnx_mpls_oam_results_glb.mep_acc_info.id, lb.gtf_id);
    } else if (2 == lb_tst_flag) {
        tst_tx_ptr.tlv.tlv_length = 36;
        tst_tx_ptr.tlv.tlv_type = bcmOamTlvTypeTestNullWithoutCRC;
        tst_rx_ptr.expected_tlv.tlv_length = 36;
        tst_rx_ptr.expected_tlv.tlv_type = bcmOamTlvTypeTestNullWithoutCRC;
        rv = bcm_oam_tst_rx_add(unit, &tst_rx_ptr);
        if (rv != BCM_E_NONE) {
           printf("Error, in bcm_oam_tst_rx_add\n");
           return rv;
        }

        rv = bcm_oam_tst_tx_add(unit, &tst_tx_ptr);
        if (rv != BCM_E_NONE) {
           printf("Error, in bcm_oam_tst_tx_add\n");
           return rv;
        }
        gtf_id = tst_tx_ptr.gtf_id;
        printf("`````````  tst gtf %d ctf %d  ``````````\n\n",tst_tx_ptr.gtf_id,tst_rx_ptr.ctf_id);

    }

    dnx_oam_lb_tst_glb.global_gtf_id = gtf_id;

    /*start to send lb/tst packet*/
    rv = dnx_oam_sat_start(unit, gtf_id);
    if (rv != BCM_E_NONE) {
       printf("Error, in dnx_oam_sat_start\n");
       return rv;
    }

    return rv;
}
