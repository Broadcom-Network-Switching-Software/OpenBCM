/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * File: cint_sat.c
 * 
 * Purpose: 
 *     Example of using SAT APIs.
 *     The following CINT provides a calling sequence example to Setup one GTF and one CTF. 
 *
 * Global - configure sequence:
 *     1. global configure
 *        - Call bcm_sat_config_set()
 * 
 * GTF - Set up sequence:
 *     1. Create GTF Object
 *        - Call bcm_sat_gtf_create()
 *     2. Configure packet info, including header type, header info, payload, packet edit info and packet offset. 
 *        Note: The format of SAT packet transsmitted from GTF is defined in MEF Sat spec.
 *        - Call bcm_sat_gtf_packet_config()
 *     3. Set Bandwidth
 *        - Call bcm_sat_gtf_bandwidth_set()
 *     4. Set Rate Pattern
 *        - Call bcm_sat_gtf_rate_pattern_set()
 *
 * CTF - Set up sequence:
 *     1. Create CTF Object
 *        - Call bcm_sat_ctf_create()
 *     2. Use PMF to capture the sat packets to OAMP
 *        - Call bcm_field_group_create()
 *     3. Map identification to specific flow
 *        - Call bcm_sat_ctf_trap_add()
 *        - Call bcm_sat_ctf_trap_data_to_session_map()
 *        - Call bcm_sat_ctf_identifier_map()
 *     4. Get the collected information
 *        - Call bcm_sat_ctf_stat_get()
 *
 *
 * To Activate Above Settings Run:
 *     BCM> cint examples/sat/cint_sat.c
 *     BCM> cint 
 *     cint> print sat_setup(0);
 *     cint> print sat_cleanup(0);
 */

int is_xgs = 1;                             /* Device Type Configure */
int oam_packets = 1;                        /* Packet Type*/
/************** Packet Parameters - used by both GTF and CTF ******************/
bcm_mac_t pkt_dmac = {0x00, 0x00, 0x00, 0xca, 0xfe, 0xda};
bcm_mac_t pkt_smac = {0x00, 0x00, 0x00, 0xca, 0xfe, 0x5a};
bcm_vlan_t pkt_vlans[2] = {0x100, 0x200};
uint8 pkt_vlan_pri[2] = {0, 1};
uint8 pkt_vlan_cfi[2] = {0, 0};
uint16 pkt_ether_type = 0x8902;
/* FL Packet Related Rarameter */
uint32 gtf_seq_number_offset = -1;             
/* FD Packet Related Rarameter */
uint32 gtf_timestamp_offset = 0;

/************** CTF Parameters ******************/
bcm_field_group_t grp;
int trap_id=0;
int trap_code_qualifier = 0;
bcm_sat_ctf_identifier_t identifier;
bcm_sat_ctf_t ctf_id = 0;
uint32 dnx_sat_identifier_color = 0;
bcm_sat_payload_type_t dnx_sat_payload_type = bcmSatPayloadConstant8Bytes;
int dnx_sat_payload_offset = 38; /*ptch(2B)+ITMH(5B)+ETH(18B)+FL PDU Header(12B)*/
bcm_gport_t dnx_gtf_dst_sys_port = 14;

/************** PMF for receiving SAT packet on JR2 ******************/
bcm_field_group_t sat_fg_id=0;
bcm_field_entry_t sat_fg_entry_id = 0;
bcm_field_context_t sat_field_context_id;

/* Used for trapping packets according to conditions*/
struct cint_sat_pmf_filtered_fields
{
    int ttl;                                        /** TTL field */
    int protocol;                                   /** Protocol field */
};

cint_sat_pmf_filtered_fields cint_sat_fields_info = {
    /*
     * ttl
     */
    0x30,
    /*
     * protocol
     */
    0x30
};


/* SAT EVENT counters*/
int event_count =0;

/*  
 * Create a FG to capture matched packets to SAT engine.
 * The filter can be SA, DA, SIP, DIP or other packet's fields.
 */
int ctf_jer_specific_config(int unit, int ctf_id) {
 
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    int group_priority = 1;
    bcm_ip_t src_ip = 0x0a0a0a0a;
    bcm_ip_t src_ip_mask = 0xffffffff;
    bcm_ip_t dst_ip = 0x0b0b0b0b;
    bcm_ip_t dst_ip_mask = 0xffffffff;
    uint32 dest_port = 232; /*dest_port (OAMP PP port)*/
    bcm_gport_t sat_gport;
    bcm_rx_trap_config_t trap_config;
    bcm_sat_config_t sat_cfg;
    uint32 oam_value = 0;
    int rv = 0;
    /* uint16 ether_type = 0x8902;  */

    rv = bcm_sat_config_get(unit, &sat_cfg);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_config_get\n");
        return rv;
    }
    oam_value = (sat_cfg.timestamp_format == bcmSATTimestampFormatIEEE1588v1) ? 0x8000: 0xC000;

    /* Create a trap for SAT */
    /* create trap */
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rx_trap_type_create (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    sal_memset(&trap_config, 0, sizeof(trap_config));
    trap_config.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP | BCM_RX_TRAP_REPLACE);
    trap_config.trap_strength = 0;
    trap_config.dest_port = dest_port;
    
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rx_trap_set (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    BCM_GPORT_TRAP_SET(sat_gport, trap_id, 7, 0);

    /* Jericho specific - set PMF */
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOuterVlan);

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionTrap);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionOam);

    rv = bcm_field_group_create(unit, qset, group_priority, &grp);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_group_create\n");
        return rv;
    }

    rv = bcm_field_group_action_set(unit, grp, aset);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_group_action_set\n");
        return rv;
    }

    rv = bcm_field_group_install(unit, grp);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_group_install\n");
        return rv;
    }

    rv = bcm_field_entry_create(unit, grp, &ent);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_entry_create\n");
        return rv;
    }

    /*rv = bcm_field_qualify_SrcIp(unit, ent, src_ip, src_ip_mask); 
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_SrcIp\n");
        return rv;
    }

    rv = bcm_field_qualify_DstIp(unit, ent, dst_ip, dst_ip_mask); 
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_DstIp\n");
        return rv;
    }
    
    rv = bcm_field_qualify_EtherType(unit, ent, ether_type, 0xFFFF);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_EtherType\n");
        return rv;
    }*/

    rv = bcm_field_qualify_OuterVlan(unit, ent, 5, 0xFFFF);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_EtherType\n");
        return rv;
    }

    /*rv = bcm_field_qualify_InPort(unit, ent, 40, 0xFFFFFFFF);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_EtherType\n");
        return rv;
    }*/

    /* Trap the packets to SAT engine, and update the FHEI header as followed.
     * Trap-Code:           trap_id
     * Trap-Qualifier: ctf_id
     * 
     * In this case, 
     *  As for the Trap-Code, viewed it as the trap origin
     *  As for the Trap-Qualifier, viewed it as the SAT test flow-id
     */


    trap_code_qualifier = ctf_id;
    rv = bcm_field_action_add(unit, ent, bcmFieldActionTrap, sat_gport, trap_code_qualifier); 
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_action_add -- bcmFieldActionTrap\n");
        return rv;
    }

    rv = bcm_field_action_add(unit, ent, bcmFieldActionOam, oam_value, 0);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_action_add -- bcmFieldActionOam\n");
        return rv;
    }

    rv = bcm_field_entry_install(unit, ent);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_entry_install\n");
        return rv;
    }

    return 0;
}

/*
 * Set followed CTF configuration.
 *   Set recieved packet information.
 *   Add trap id, it is used to mapping identification to specific flow.
 *   Set a mapping between trap qualifier and session id.
 *   Set a mapping between identification and specific flow.
 *   Set Statistic collecting configuration.
 *   Set Availability configuration
 */
int ctf_general_config(int unit, bcm_sat_ctf_t ctf_id) {
 
    bcm_sat_ctf_packet_info_t packet_info;
    bcm_sat_ctf_stat_config_t stat_config;
    bcm_sat_ctf_availability_config_t availability_config;
    bcm_sat_ctf_bin_limit_t bins[9];
    int i = 0;
    int rv = 0;

    /* Set the recieved packet information */
    bcm_sat_ctf_packet_info_t_init(&packet_info);
    packet_info.sat_header_type = bcmSatHeaderUserDefined;
    if (gtf_timestamp_offset != 0) {
        packet_info.offsets.timestamp_offset = gtf_timestamp_offset - gtf_pkt_len_of_int_hdr;
    }
    else {
        packet_info.offsets.timestamp_offset = gtf_timestamp_offset;
    }
    if (gtf_seq_number_offset != -1) {
        packet_info.offsets.seq_number_offset = gtf_seq_number_offset - gtf_pkt_len_of_int_hdr;
    }
    else {
        packet_info.offsets.seq_number_offset = gtf_seq_number_offset;
    }
    
    /*packet_info.offsets.seq_number_offset = 30;*/
    /*packet_info.offsets.timestamp_offset = 22;*/
    
    if (is_device_or_above(unit, JERICHO2))
    {
        packet_info.offsets.payload_offset = dnx_sat_payload_offset;
        packet_info.payload.payload_type = dnx_sat_payload_type;
        identifier.color = dnx_sat_identifier_color;
        for (i = 0; i < BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE; i++) {
            packet_info.payload.payload_pattern[i] = gtf_payload_pattern[i];
        }

    }
    else {
        packet_info.offsets.payload_offset = 32;
        packet_info.payload.payload_type = bcmSatPayloadConstant4Bytes;
        packet_info.offsets.crc_byte_offset = gtf_crc_byte_offset;
        packet_info.flags = BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV | BCM_SAT_CTF_PACKET_INFO_ADD_CRC;
        identifier.color = 1;
    }
    rv = bcm_sat_ctf_packet_config (unit, ctf_id, &packet_info);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_packet_config\n");
        return rv;
    }

    /* Add trap id, it is used to mapping identification to specific flow
     * Needn't add trap id for saber2 device as trap_id always is 0.
     * and it's same with default value.
     */
    if (!is_xgs && !is_device_or_above(unit, JERICHO2)) {
        rv = bcm_sat_ctf_trap_add(unit, trap_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_sat_ctf_trap_add\n");
            return rv;
        }
    }
    /* Map trap data to session id  */
    rv = bcm_sat_ctf_trap_data_to_session_map(unit, trap_code_qualifier, 0xff, 0);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_trap_data_to_session_map\n");
        return rv;
    }

    /* Map identification to specific flow */
    bcm_sat_ctf_identifier_t_init(&identifier); 
    identifier.session_id = 0; /* the result from trap data mapping */
    identifier.trap_id = 0; /* index in trap_id array */
    identifier.tc = 0;
    rv = bcm_sat_ctf_identifier_unmap(unit, &identifier);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_identifier_unmap\n");
        return rv;
    }
    rv = bcm_sat_ctf_identifier_map(unit, &identifier, ctf_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_identifier_map\n");
        return rv;
    }

    /* Statistic collecting configuration */
    bcm_sat_ctf_stat_config_t_init(stat_config);
    stat_config.use_global_bin_config = 0;
    stat_config.bin_min_delay = 50;
    stat_config.bin_step = 10;
    stat_config.update_counters_in_unvavail_state = 1;
    rv = bcm_sat_ctf_stat_config_set(unit, ctf_id, &stat_config);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_stat_config_set\n");
        return rv;
    }

    /* If want to set configuration for each bin, please use followed BCM API */
    for (i = 0; i < 9; i++)
    {
        bins[i].bin_select = i;
        bins[i].bin_limit = 100 + i*100;
    }
    rv = bcm_sat_ctf_bin_limit_set(unit, 9, &bins);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_bin_limit_set\n");
        return rv;
    }

    /* SAT Availability */
    bcm_sat_ctf_availability_config_t_init(&availability_config);
    availability_config.switch_state_num_of_slots = 10;
    availability_config.switch_state_threshold_per_slot = 100;
    rv = bcm_sat_ctf_availability_config_set(unit, ctf_id, &availability_config);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_availability_config_set\n");
        return rv;
    }

    return 0;
}

/* Get the collected information */
int ctf_general_show_results(int unit, bcm_sat_ctf_t ctf_id, int read_clear) {
 
    bcm_sat_ctf_stat_t stat;
    int i  = 0;
    int rv  = 0;
    uint32 flags = 0;

    if (read_clear == 0) {
        flags = BCM_SAT_CTF_STAT_DO_NOT_CLR_ON_READ;
    }
    rv = bcm_sat_ctf_stat_get (unit, ctf_id, flags, &stat);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_stat_get\n");
        return rv;
    }

    printf("STAT informations:\n");
    print stat;

    return 0;
}

int cb_sat(int unit,
            bcm_sat_event_type_t event_type,
            bcm_sat_report_event_data_t *event_data,
            void *user_data) {
   			
    event_count ++;  
	/*To avoid printing too much */
	if(event_count < 100){
        printf("++++++++++++ SAT event %d++++++++++ \n",event_count );
        print unit;
        print event_type;
        print *event_data;
	}

    return 0;
}

int ctf_setup(int unit) {

    int rv = BCM_E_NONE;

    /* Create a CTF */
    rv = bcm_sat_ctf_create(unit, 0, &ctf_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_create\n");
        return rv;
    }

    /* Capture the matched packets to SAT engine.
     * In Jericho, use PMF to capture packets to SAT engine.
     * In Saber2, use bcm_sat_endpoint_create() to capture 
     * packets to SAT engine.
     */
    if (is_xgs) {

      /* Configure OLP header - already implemented in olp_config() .
         * These bcm command should be convert to BCM APIs by IDC
         * as IDC is owner of this feature.
         */

        /*bshell(unit, "modify EGR_OLP_DGLP_CONFIG 0 1 DGLP=0x5 MACDA=0xD0C0");
        bshell(unit, "modify EGR_OLP_DGLP_CONFIG 1 1 DGLP=0x1f84 MACDA=0x100020304051");
        bshell(unit, "modify EGR_OLP_CONFIG 0 1 MACSA=0x202122232425 ETHERTYPE=0xb4b4 VLAN_TPID=0x8100");
        bshell(unit, "modify EGR_OLP_HEADER_TYPE_MAPPING 0 1 HDR_TYPE=1 HDR_SUBTYPE=12");
        bshell(unit, "modreg EGR_OLP_VLAN VLAN_TAG=0x8002");
        bshell(unit, "modreg EGR_OAM_RX_MODE_FOR_CPU ADD_OLP=1");*/


        /* Configure endpoint to trap the packet to OAMP port.
         * These bcm command should be convert to BCM APIs by IDC
         * as IDC is owner of this feature.
         */

        /*bshell(unit, "write ING_SAT_SAMP_TCAM 0 1 VALID=0x3 GLP=1 OTAG_VALID=1 OVID=0x100 MACSA=0xcafe5a MACDA=0xcafeda ETHERTYPE=0x8902 GLP_MASK=0xFFFF OTAG_VALID_MASK=0x1 OVID_MASK=0xFFF MACSA_MASK=0xFFFFFFFFFFFF MACDA_MASK=0xFFFFFFFFFFFF");
        bshell(unit, "write ING_SAT_SAMP_DATA 0 1 FORWARD_ACTION=0x1 COPYTO_CPU=0 DESTINATION=0x5 ADD_OLP_HDR=0x1 SAMPLE_TIMESTAMP=1 TIMESTAMP_TYPE=1 SAT_SESSION_ID=0xFF INT_PRI=0 OLP_HDR_TYPE_COMPRESSED=0x0");*/

      bcm_sat_endpoint_info_t endpoint_info;

      /* UPMEP SAT TCAM Key fields */
      endpoint_info.flags = BCM_SAT_ENDPOINT_DOWNMEP | 
                            BCM_SAT_ENDPOINT_MATCH_OUTER_VLAN;

      endpoint_info.outer_vlan = pkt_vlan_pri[0]<<13 | pkt_vlan_cfi[0]<<12 | pkt_vlans[0];
      endpoint_info.src_gport = 0x8000001;
      endpoint_info.ether_type = pkt_ether_type;
	  sal_memcpy(endpoint_info.src_mac_address, pkt_smac, 6);
	  sal_memcpy(endpoint_info.dst_mac_address, pkt_dmac, 6);

      /* UPMEP SAT TCAM Data fields */
      endpoint_info.action_flags = BCM_SAT_ACTION_FWD_ACTION_REDIRECT;
      endpoint_info.action_flags |= BCM_SAT_ACTION_SAMPLE_TIMESTAMP;
      endpoint_info.dest_gport = 0x8000005;
      endpoint_info.timestamp_format = 1;
      endpoint_info.session_id = 0xff;

      rv = bcm_sat_endpoint_create(unit, &endpoint_info);
      if(rv != BCM_E_NONE)
      {
        printf("Error ! Failed to create SAT endpoint rv %d \n",rv);
        return rv;
      }
      /* It will be used as trap_data in bcm_sat_ctf_trap_data_to_session_map() */
      trap_code_qualifier = endpoint_info.session_id;
      } else if (!is_device_or_above(unit, JERICHO2)) {
        rv = ctf_jer_specific_config(unit, ctf_id);
        if (BCM_E_NONE != rv) {
            printf("Error in ctf_jer_specific_config\n");
            return rv;
        }
    }
    /* Set CTF configuration */
    rv = ctf_general_config(unit, ctf_id);
    if (BCM_E_NONE != rv) {
        printf("Error in ctf_general_config\n");
        return rv;
    }

    /*Set CTF Report*/
    bcm_sat_ctf_report_config_t ctf_report_config;
    bcm_sat_ctf_report_config_t_init(&ctf_report_config);
    ctf_report_config.report_sampling_percent = 90;
    ctf_report_config.flags = BCM_SAT_CTF_REPORT_ADD_SEQ_NUM;
    rv = bcm_sat_ctf_reports_config_set(unit, ctf_id, &ctf_report_config);  
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_reports_config_set\n");
        return rv;
    }

   /* Sat event register
    rv = bcm_sat_event_register(unit, bcmSATEventReport, cb_sat, (void*)1);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_event_register\n");
        return rv;
    }
    */

    /* Get the collected information */
    rv = ctf_general_show_results(unit, ctf_id, 1);
    if (BCM_E_NONE != rv) {
        printf("Error in ctf_general_show_results\n");
        return rv;
    }

    return rv;

}

int register_sat_events(int unit) {
    int rv = BCM_E_NONE;
    rv = bcm_sat_event_register(unit, bcmSATEventReport, cb_sat, (void*)1);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_event_register\n");
        return rv;
    }
    return rv;
}

/* Clean up CTF configuration */
int ctf_setup_cleanup(int unit) {

    int rv = BCM_E_NONE;

    /* clean up */
    /* Set the CTF flow as invalid  */
    rv = bcm_sat_ctf_identifier_unmap(unit, &identifier);
    if (BCM_E_NONE != rv) {
        printf("Error in soc_sat_ctf_identifier_unmap\n");
        return rv;
    }

    /* Remove all trap id */
    rv = bcm_sat_ctf_trap_remove_all(unit);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_trap_add\n");
        return rv;
    }

    /* Destroy the CTF flow */
    rv = bcm_sat_ctf_destroy(unit, ctf_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_destroy\n");
        return rv;
    }

    return rv;

}

/**************Gtf Structure*********************************/
enum gtf_test_mode_e {
    gtf_test_mode_fl_down,         /* Test SAT Frame LM Packet for Down Mep */
    gtf_test_mode_fl_up,           /* Test SAT Frame LM Packet for Up   Mep */
    gtf_test_mode_fd_down,         /* Test SAT Frame DM Packet for Down Mep */
    gtf_test_mode_fd_up            /* Test SAT Frame DM Packet for Up   Mep */
};

/**************Envrionment Configure**********************/
int gtf_debug_on = 1;                       /* Gtf Debug */

int gtf_test_mode = gtf_test_mode_fl_down;
uint32 gtf_counters = 0;

/**************Gtf Object*********************************/
bcm_sat_gtf_t gtf_id;

/**************Gtf Packet Parameter***********************/
bcm_sat_header_type_t gtf_header_type = bcmSatHeaderUserDefined;
int dnx_gtf_min_packet_length = 69;/* JR2 DNX_SAT_GTF_PACKET_LENGTH_MIN(65B) + CRC(4B)*/

bcm_sat_payload_type_t gtf_payload_type = bcmSatPayloadConstant8Bytes;
uint8 gtf_payload_pattern[BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8};

uint32 gtf_packet_length[BCM_SAT_GTF_NUM_OF_PRIORITIES][BCM_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS] = {                                                /* 6bytes dune header + 64bytes */
    {64, 128, 256, 512, 1024, 1280, 1518, 9000},
	{64, 128, 256, 512, 1024, 1280, 1518, 9000}};
uint32 gtf_packet_length_pattern[BCM_SAT_GTF_NUM_OF_PRIORITIES][BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7},
	{0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7}};
uint32 gtf_pattern_length[BCM_SAT_GTF_NUM_OF_PRIORITIES] = {1,1};

bcm_sat_stamp_type_t gtf_stamp_type[BCM_SAT_GTF_NUM_OF_PRIORITIES][3] = {
    {bcmSatStampConstant2Bit, bcmSatStampCounter8Bit, bcmSatStampCounter16Bit},
    {bcmSatStampConstant2Bit, bcmSatStampCounter8Bit, bcmSatStampCounter16Bit}};
bcm_sat_stamp_field_t gtf_stamp_field[BCM_SAT_GTF_NUM_OF_PRIORITIES][3] = {
    {bcmSatStampFieldUserDefined, bcmSatStampFieldOuterVlan, bcmSatStampFieldSrcMac},
    {bcmSatStampFieldUserDefined, bcmSatStampFieldOuterVlan, bcmSatStampFieldSrcMac}};
uint32 gtf_stamp_inc_step[BCM_SAT_GTF_NUM_OF_PRIORITIES][3] = {
    {1, 1, 1},
	{1, 1, 1}};
uint32 gtf_stamp_inc_period_packets[BCM_SAT_GTF_NUM_OF_PRIORITIES][3] = {
    {1, 1, 1},
    {1, 1, 1}};
uint32 gtf_stamp_value[BCM_SAT_GTF_NUM_OF_PRIORITIES][3] = {
    {1, 0, 0},
    {2, 0, 0}};
uint32 gtf_stamp_offset[BCM_SAT_GTF_NUM_OF_PRIORITIES][3] = {                      /* start from ethernet header, index of first bit is 1 */ 
    {50, 1, 1},            /* stamp last two bits of SMAC to differentiate cir and eir */
    {50, 1, 1}};
uint32 gtf_number_of_stamps[BCM_SAT_GTF_NUM_OF_PRIORITIES] = {0,0};

uint32 gtf_number_of_ctfs[BCM_SAT_GTF_NUM_OF_PRIORITIES] = {1,1};

uint32 gtf_add_data_tlv = 0;                          /* valid only when there is one paket length for one GTF */
uint32 gtf_add_end_tlv = 1;
uint32 gtf_add_crc = 0;
uint32 gtf_crc_byte_offset = 38;

/* FD Packet Related Rarameter */
uint32 gtf_ts_offset = 22;                            /* length of ethernet header + offset in FD PDU */

/**************Gtf Bandwidth Parameter*********************/
uint32 gtf_bw_cir_rate = 100;                             /* traffic rate - 5G.  Units: kbps */
uint32 gtf_bw_cir_max_burst = 1000;                     /* traffic burst - 1Mbit. Units: kbit */
uint32 gtf_bw_eir_rate = 100;                             /* traffic rate - 5G.  Units: kbps */
uint32 gtf_bw_eir_max_burst = 1000;                     /* traffic burst - 1Mbit. Units: kbit */

/**************Gtf Rate Pattern Parameter******************/
bcm_sat_gtf_rate_pattern_mode_t gtf_rate_pattern_mode = bcmSatGtfRatePatternInterval;
uint32 gtf_rate_pattern_high = 10;               /* 10kbits */
uint32 gtf_rate_pattern_low = 10;                /* 10kbits */
uint32 gtf_rate_pattern_stop_iter_count = 1000;      /* Stop after 1000 iterations */
uint32 gtf_rate_pattern_stop_burst_count = 10000;
uint32 gtf_rate_pattern_stop_interval_count = 10000; 
uint32 gtf_rate_pattern_burst_packet_weight = 1;     /* Every packet in burst state increases the counter by 1 */
uint32 gtf_rate_pattern_interval_packet_weight = 1;  /* Every packet in interval state increases the counter by 1 */

/**************GTF Packet Parameter - Packet Header******************/
bcm_port_t gtf_source_port = 13;    /* For UP-MEP case, port from where traffic will exit ASIC */
bcm_port_t gtf_dst_port = 0;        /* Port where MEP resides. GTF simulate traffic generation from this port */
int        gtf_dst_modid = 0;                        /* relevant to XGS */

uint16 gtf_tpids[2] = {0x8100, 0x9100};
int gtf_no_tags = 1;

uint8 gtf_mel = 0x3;
uint8 gtf_version = 0x0;
uint8 gtf_fl_opcode = 43;
uint8 gtf_fd_opcode = 47;
uint8 gtf_flags = 0;
uint16 gtf_source_mep_id = 0;
uint32 gtf_session_id = 0x12345678;
int   gtf_fd_timestamp_offset = 22; 

/* Temp varible */
bcm_gport_t gtf_dst_sys_port = 0;
uint32 gtf_pkt_len_of_int_hdr = 0;
uint32 gtf_data_tlv_type = 3;
uint16 gtf_data_tlv_len = 0;

/**********************OLP Parameter**************************/
bcm_mac_t olp_mac = {0xc0, 0x94, 0x2f, 0xc3, 0x83, 0x9e};
bcm_mac_t xgs_mac = {0xf2, 0x44, 0x49, 0x39, 0x30, 0xec};
uint16 olp_tpid = 0xa96d;
uint16 olp_vlan = 0xdd4;
uint16 olp_ether_type = 0x5843;
bcm_port_t olp_port = 5;
uint32 olp_tx_header_subtype = 2;
int olp_timestamp_action = 1;


int olp_config(int unit) {
    int rv = BCM_E_NONE;
    int station_id;
    bcm_l2_station_t station;
    bcm_gport_t gport;

    rv = bcm_port_gport_get(unit, olp_port, &gport); 
    if (rv != BCM_E_NONE) {
        printf("Failed($rv) in bcm_port_gport_get\n");
        return rv;
    }

    /* set olp tpid */
    rv = bcm_switch_control_set(unit, bcmSwitchOamOlpChipTpid, olp_tpid);
    if (rv != BCM_E_NONE) {
        printf("Failed($rv) in bcmSwitchOamOlpChipEtherType\n");
        return rv;
    }

    /* set VLAN for olp-xgs communication */
    rv = bcm_switch_control_set(unit, bcmSwitchOamOlpChipVlan, olp_vlan);
    if (rv != BCM_E_NONE) {
        printf("Failed($rv) in bcmSwitchOamOlpChipVlan\n");
        return rv;
    }

    /* set olp ethertype */
    rv = bcm_switch_control_set(unit, bcmSwitchOamOlpChipEtherType, olp_ether_type);
    if (rv != BCM_E_NONE) {
        printf("Failed($rv) in bcmSwitchOamOlpChipEtherType\n");
        return rv;
    }

    /* set olp mac */
    station.flags = BCM_L2_STATION_OLP;
    station.src_port = gport;
    station.dst_mac[0] = olp_mac[0];
    station.dst_mac[1] = olp_mac[1];
    station.dst_mac[2] = olp_mac[2];
    station.dst_mac[3] = olp_mac[3];
    station.dst_mac[4] = olp_mac[4];
    station.dst_mac[5] = olp_mac[5];
    rv = bcm_l2_station_add(unit, &station_id, &station);
    if (rv != BCM_E_NONE) {
        printf("Failed($rv) in setting olp mac\n");
        return rv;
    }

    /* set xgs mac */
    station.flags = BCM_L2_STATION_XGS_MAC;
    station.src_port = gport;
    station.dst_mac[0] = xgs_mac[0];
    station.dst_mac[1] = xgs_mac[1];
    station.dst_mac[2] = xgs_mac[2];
    station.dst_mac[3] = xgs_mac[3];
    station.dst_mac[4] = xgs_mac[4];
    station.dst_mac[5] = xgs_mac[5];
    rv = bcm_l2_station_add(unit, &station_id, &station);
    if (rv != BCM_E_NONE) {
        printf("Failed($rv) in setting xgs mac\n");
        return rv;
    }
	
	return rv;
}

int l2_addr_add(int unit, bcm_mac_t mac, uint16 vid, bcm_gport_t port) {
    int rv = BCM_E_NONE;
	bcm_l2_addr_t l2addr;

	bcm_l2_addr_t_init(&l2addr, mac, vid);
    l2addr.port = port;
	
    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("Failed($rv) in bcm_l2_addr_add\n");
        return rv;
    }
	
	return rv;
}

int set_port_keep_tag(int unit, uint16 vid, bcm_port_t port) {
    int rv = BCM_E_NONE;
    bcm_pbmp_t pbmp, ut_pbmp;

    BCM_PBMP_PORT_SET(pbmp, port);
    BCM_PBMP_CLEAR(ut_pbmp);
    rv = bcm_vlan_port_remove(unit, vid, pbmp);
    if (rv != BCM_E_NONE) {
        printf("Failed($rv) in bcm_vlan_port_remove\n");
        return rv;
    }

    rv =  bcm_vlan_port_add(unit, vid, pbmp, ut_pbmp);
    if (rv != BCM_E_NONE) {
        printf("Failed($rv) in bcm_vlan_port_add\n");
        return rv;
    }
	
	return rv;
}

/**
* \brief
*  CTF packet trap configure, trap packet to sat port.
* \param [in] unit  - Device ID
* \param [in] ctf_id   - CTF  ID
* \param [in] in_port  - IN port
* \param [in] ts_format - time stamp format
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int dnx_sat_ctf_inport_trap(int unit, bcm_sat_ctf_t ctf_id, int in_port, bcm_sat_timestamp_format_t ts_format)
{
    int oam_offset, trap_id, rv;
    int sat_port = 218;
    bcm_rx_trap_config_t trap_config;    
    bcm_gport_t trap_gport;
    if (ts_format == bcmSATTimestampFormatIEEE1588v1) {
        oam_offset = 0x80;
    } else {
        oam_offset = 0xC0;
    }
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_rx_trap_type_create\n");
        return rv;
    }
    if (gtf_debug_on) {
        printf(" ======== trap_id is  %d  ==========\n", trap_id);
    }

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
    trap_config.trap_strength = 0;
    trap_config.dest_port= sat_port;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_rx_trap_set\n");
        return rv;
    }
    BCM_GPORT_TRAP_SET(trap_gport, trap_id, 7, 0);
    rv = bcm_sat_ctf_trap_add(unit, trap_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_trap_add\n");
        return rv;
    }
    rv = ctf_pmf_config_jr2(unit, in_port, trap_gport, ctf_id, oam_offset);
    if (BCM_E_NONE != rv) {
        printf("Error in ctf_pmf_config_jr2\n");
        return rv;
    }
    return BCM_E_NONE;
}

int _insert_dnx_ptch2(uint8* buffer, int byte_start_offset, 
    uint8 next_header_is_itmh, 
	uint8 opaque_value, 
	uint8 pp_ssp) 
{
    int offset = byte_start_offset;
	int parser_program_ctl = 0;

    parser_program_ctl = next_header_is_itmh ? 0 : 1;
    buffer[offset++] = (parser_program_ctl<<7) | (opaque_value<<4);
    buffer[offset++] = pp_ssp;
    
    return offset;
}

int _insert_dnx_itmh_jer2(uint8* buffer, int byte_start_offset,
    int dst_sys_port,
    uint8 fwd_tc,
    uint8 fwd_dp,
    uint8 snoop_cmd,
    uint8 in_mirr_flag,
    uint8 pph_type)
{
    int offset = byte_start_offset;
    int fwd_dest_info = 0x3<<18 | dst_sys_port;
    int dest_exten = 0;

    buffer[offset++] = ((pph_type&0x3)<<1)|((in_mirr_flag&0x1)<<5);
    buffer[offset++] = ((fwd_dp&0x3)<<6)|((fwd_dest_info&0x1F8000)>>15);
    buffer[offset++] = ((fwd_dest_info&0x7F80)>>7);
    buffer[offset++] = ((fwd_dest_info&0x7F)<<1) | ((snoop_cmd & 0x10)>>4);
    buffer[offset++] = ((snoop_cmd&0xf)<<4)|((fwd_tc&0x7)<<1)|(dest_exten&0x1);

    return offset;
}

int _insert_dnx_itmh_jer(uint8* buffer, int byte_start_offset, 
    int dst_sys_port, 
	uint8 fwd_tc, 
	uint8 fwd_dp, 
	uint8 snoop_cmd, 
	uint8 in_mirr_flag, 
	uint8 pph_type)
{
    int offset = byte_start_offset;
    int fwd_dest_info = 1<<16 | dst_sys_port;
    int dest_exten = 0;

    buffer[offset++] = ((pph_type&0x3)<<6)|((in_mirr_flag&0x1)<<5)|((fwd_dp&0x3)<<3)|((fwd_dest_info&0x70000)>>16);
    buffer[offset++] = ((fwd_dest_info&0xff00)>>8);
    buffer[offset++] = (fwd_dest_info&0xff);
    buffer[offset++] = ((snoop_cmd&0xf)<<4)|((fwd_tc&0x7)<<1)|(dest_exten&0x1);
    
    return offset;
}

int _insert_dnx_itmh_arad(uint8* buffer, int byte_start_offset, 
    int dst_sys_port, 
	uint8 fwd_tc, 
	uint8 fwd_dp, 
	uint8 snoop_cmd, 
	uint8 in_mirr_flag, 
	uint8 pph_type)
{
    int offset = byte_start_offset;
    int fwd_dest_info = 0xc0000 | dst_sys_port;
    int dest_exten = 0;

    buffer[offset++] = ((pph_type&0x3)<<6)|((in_mirr_flag&0x1)<<5)|((snoop_cmd&0xf)<<1)|((fwd_tc&0x4)>>2);
    buffer[offset++] = ((fwd_tc&0x3)<<6) | ((fwd_dp&0x3)<<4) | ((fwd_dest_info&0xf0000)>>16);
    buffer[offset++] = (fwd_dest_info&0xff00)>>8;
    buffer[offset++] = (fwd_dest_info&0xff);

    return offset;
}

int _insert_dnx_oam_ts(uint8* buffer, int byte_start_offset, 
	uint8 type, 
	uint8 sub_type,
	uint8 is_up_mep,
	uint64 ts_data,
	uint8 ts_offset) 
{
    int offset = byte_start_offset;
    int mep_type;
	int ts_data_hi = 0, ts_data_lo = 0;	

    mep_type = is_up_mep?1:0;
	ts_data_hi = COMPILER_64_HI(ts_data);
	ts_data_lo = COMPILER_64_LO(ts_data);
    buffer[offset++] = ((type&0x3)<<6)|((sub_type&0x7)<<3)|((mep_type&0x1)<<2)|(ts_data_hi&0x3);	
    buffer[offset++] = (ts_data_lo&0xff000000)>>24;
	buffer[offset++] = (ts_data_lo&0xff0000)>>16;
	buffer[offset++] = (ts_data_lo&0xff00)>>8;
	buffer[offset++] = (ts_data_lo&0xff);
    buffer[offset++] = ts_offset&0xff;

    return offset;
}

int _insert_dnx_pph_arad(uint8* buffer, int byte_start_offset)
{
    int offset = byte_start_offset;

    buffer[offset++] = 0;
    buffer[offset++] = 0;
    buffer[offset++] = 0;
    buffer[offset++] = 0;
    buffer[offset++] = 0;
    buffer[offset++] = 0;
    buffer[offset++] = 0;
    
    return offset;
}

/* 
 *  XGS OLP L2 Header
 *    Field                      Size
 *    OLP-DA                     48
 *    OLP-SA                     48
 *    OLP-VLAN                   32
 */
int _insert_xgs_olp_l2(uint8* buffer, int byte_start_offset, 
    bcm_mac_t dest_addr, 
	bcm_mac_t src_addr, 
    uint16 tpid, 
	bcm_vlan_t vlan, 
	uint8 vlan_pri, 
	uint8 vlan_cfi, 
	uint16 ether_type) 
{
    int i = 0, offset = byte_start_offset; 
    for (i = 0; i < 6; i++) {
        buffer[offset++] = dest_addr[i];
    }
    for (i = 0; i < 6; i++) {
        buffer[offset++] = src_addr[i];
    }

    buffer[offset++] = (tpid>>8)&0xff;
    buffer[offset++] = tpid&0xff;
    buffer[offset++] = (vlan_pri<<5)|((vlan_cfi&&0x1)<<4)|((vlan>>8)&0xff);        
    buffer[offset++] = vlan&0xff;
    
    buffer[offset++] = (ether_type>>8)&0xff;
    buffer[offset++] = ether_type&0xff;

    return offset;
}

/* 
 *  XGS OLP TX Header
 *  From SB2_OLP_Architecture_Spec_0.94_Clean.docx
 *    Field                                     Size     Bit/s    Bytes    Description
 *    Version                                      4     127:124  15       Version =0 for Saber 2 & Katana 2
 *    HEADER_TYPE                                  8     123:116  15-14    00=OAM_TX_HEADER
 *    HEADER_SUBTYPE                               8     115:108  14-13    03 = SAT_DOWN_MEP_TX
 *                                                                         04 = SAT_UPMEP_TX
 *    INT_PRI                                      4     107:104  13       Internal Priority used for UP-MEP Tx. DP is assumed to be green. 
 *    PORT_ID                                      16    103:88   12-11    [15-8] : MODID
 *                                                                         [7-0]  : PORT_NUM 
 *    COUNTER_1_LOCATION                           1     87       10       0 = in IP 
 *                                                                         1 = in EP
 *    COUNTER_1_ID                                 15    86:72    10-9     Counter ID to be incremented or sampled by this packet
 *    COUNTER_1_ACTION                             2     71:70    8        00: No-op 
 *                                                                         01: Increment 
 *                                                                         10: Sample
 *                                                                         11: reserved
 *    COUNTER_2_LOCATION                           1     69       8
 *    COUNTER_2_ID                                 15    68:54    8-6
 *    COUNTER_2_ACTION                             2     53:52    6
 *    TIMESTAMP_ACTION                             2     51:50    6        00: No-op 
 *                                                                         01: Sample 1588 Timestamp 
 *                                                                         10: Sample NTP Timestamp 
 *                                                                         11: Reserved
 *    OAM_REPLACEMENT_OFFSET                       8     49:42    6-5
 *    RELATIVE_LM_REPLACEMENT_OFFSET               4     41:38    5-4
 *    RELATIVE_LM_REPLACEMENT_OFFSET_ADD_OR_SUB    1     37       4
 *    COUNTER_3_LOCATION                           1     36       4
 *    COUNTER_3_ID                                 15    35:21    4-2
 *    COUNTER_3_ACTION                             2     20:19    2
 *    COUNTER1_OFFSET_TO_LM_PAYLOAD                6     18:13    2-1
 *    COUNTER2_OFFSET_TO_LM_PAYLOAD                6     12:7     1-0
 *    COUNTER3_OFFSET_TO_LM_PAYLOAD                6     6:1      0
 *    Reserved                                     1     0        0
 *    Total                      128
 */
int _insert_xgs_olp_tx(uint8* buffer, int byte_start_offset, 
    int is_down_mep, 
	int modid, 
	int port, 
	int timestamp_action,
	int oam_offset) 
{
    int offset = byte_start_offset;
    int version = 0;
	int header_type = 0;
	int header_subtype = (is_down_mep) ? (2) : (3);
	int int_pri = 0;
	
	int oam_replacement_offset = 0;

    int counter_1_location = 0;
	int counter_1_id = 0;
	int counter_1_action = 0;
	int counter_1_offset_to_lm_payload = 0;
    int counter_2_location = 0;
	int counter_2_id = 0;
	int counter_2_action = 0;
	int counter_2_offset_to_lm_payload = 0;
    int counter_3_location = 0;
	int counter_3_id = 0;
	int counter_3_action = 0;
	int counter_3_offset_to_lm_payload = 0;
	
	int relative_lm_repalce_offset = 0;
	int relative_lm_repalce_offset_add_or_sub = 0;
	
	oam_replacement_offset = (oam_offset - 14) / 2;

    /* byte 15: Version    [3:0]->[7:4], 
	 *          header_type[7:4]->[3:0]
	 */
    buffer[offset++] = ((version&0xf)<<4)|((header_type&0xf0)>>4); 
	/* byte 14: header_type   [3:0]->[7:4], 
	 *          header_subtype[7:4]->[3:0]
	 */
	buffer[offset++] = ((header_type&0xf)<<4)|((header_subtype&0xf0)>>4);        
    /* byte 13: header_subtype[3:0]->[7:4], 
     *          int_pri       [3:0]->[3:0]
     */
	buffer[offset++] = ((header_subtype&0xf)<<4)|((int_pri&0xf)>>4);
    /* byte 12: MODID [7:0]->[7:0] */
	buffer[offset++] = (modid&0xff);
    /* byte 11: port  [7:0]->[7:0] */
	buffer[offset++] = (port&0xff);
    /* byte 10: COUNTER_1_LOCATION [0]->[7]
     *          COUNTER_1_ID       [14:8]->[6:0]
	 */
    buffer[offset++] = 0;
    /* byte  9: COUNTER_1_ID       [7:0]->[7:0] */
    buffer[offset++] = 0;
    /* byte  8: COUNTER_1_ACTION   [1:0]->[7:6]
     *          COUNTER_2_LOCATION [0]->[5]
	 *          COUNTER_2_ID       [14:10]->[4:0]
	 */
    buffer[offset++] = 0;
    /* byte  7: COUNTER_2_ID       [9:2]->[7:0]
	 */
    buffer[offset++] = 0;
    /* byte  6: COUNTER_2_ID           [1:0]->[7:6]
     *          COUNTER_2_ACTION       [1:0]->[5:4]
	 *          TIMESTAMP_ACTION       [1:0]->[3:2]
	 *          OAM_REPLACEMENT_OFFSET [7:6]->[1:0]
	 */
    buffer[offset++] = ((timestamp_action&0x3)<<2)|((oam_replacement_offset&0xc0)>>6);
    /* byte  5: OAM_REPLACEMENT_OFFSET         [5:0]->[7:2]
	 *          RELATIVE_LM_REPLACEMENT_OFFSET [3:2]->[1:0]
 */
    buffer[offset++] = (oam_replacement_offset&0x3f)<<2;
    /* byte  4: RELATIVE_LM_REPLACEMENT_OFFSET            [1:0]->[7:6]
	 *          RELATIVE_LM_REPLACEMENT_OFFSET_ADD_OR_SUB [0]->[5]
	 *          COUNTER_3_LOCATION                        [0]->[4]
	 *          COUNTER_3_ID                              [14:11]->[3:0]
	 */
    buffer[offset++] = 0;
    /* byte  3: COUNTER_3_ID                              [10:3]->[7:0] */
    buffer[offset++] = 0;
    /* byte  2: COUNTER_3_ID                              [2:0]->[7:5]
	 *          COUNTER_3_ACTION                          [1:0]->[4:3]
     *		    COUNTER1_OFFSET_TO_LM_PAYLOAD             [5:3]->[2:0]
	 */
    buffer[offset++] = 0;
    /* byte  1: COUNTER1_OFFSET_TO_LM_PAYLOAD             [2:0]->[7:5]
	 *          COUNTER2_OFFSET_TO_LM_PAYLOAD             [5:1]->[4:0]
	 */
    buffer[offset++] = 0;
    /* byte  0: COUNTER2_OFFSET_TO_LM_PAYLOAD             [0]->[7]
	 *          COUNTER2_OFFSET_TO_LM_PAYLOAD             [5:0]->[6:1]
	 *          Reserved                                  [0]->[0]
	 */
    buffer[offset++] = 0;

    return offset;
}

/* 
 *  Insert Ethernet Header
 */
int _insert_eth(uint8* buffer, int byte_start_offset,
    bcm_mac_t dest_addr,
	bcm_mac_t src_addr, 
    uint16* tpids,
	bcm_vlan_t* vlans,
	uint8* vlan_pri,
	uint8* vlan_cfi,
	int no_tags,
	uint16 ether_type) 
{
    int i = 0, offset = byte_start_offset; 
    for (i = 0; i < 6; i++) {
        buffer[offset++] = dest_addr[i];
    }
    for (i = 0; i < 6; i++) {
        buffer[offset++] = src_addr[i];
    }
    i = 0;
    while (no_tags) {
        buffer[offset++] = (tpids[i]>>8)&0xff;
        buffer[offset++] = tpids[i]&0xff;
        buffer[offset++] = (vlan_pri[i]<<5)|((vlan_cfi[i]&&0x1)<<4)|((vlans[i]>>8)&0xff);
        buffer[offset++] = vlans[i]&0xff;
        --no_tags;
        i++;
    }
    
    buffer[offset++] = (ether_type>>8)&0xff;
    buffer[offset++] = ether_type&0xff;
    
    return offset;
}

/* 
 *  FL PDU Header format: Refer to MEF Sat spec.
 */
int _insert_fl_pdu(uint8* buffer, int byte_start_offset, 
    uint8 mel, 
	uint8 version,
	uint8 opcode,
	uint8 flags,
	uint8 tlv_offset,
	uint16 source_mep_id,
	uint32 session_id,
	uint8 add_data_tlv)
{    
    int tmp = 0, offset = byte_start_offset; 
    buffer[offset++] = ((mel&0x7)<<5)|(version&0x1f);
    buffer[offset++] = opcode;
    buffer[offset++] = flags;
    buffer[offset++] = tlv_offset;                    /* TLV Offset: 16 */
    buffer[offset++] = source_mep_id>>8;
    buffer[offset++] = source_mep_id&0xff;
    buffer[offset++] = 0;                             /* Reserved */
    buffer[offset++] = 0;                             /* Reserved */
    buffer[offset++] = (session_id>>24)&0xff;
    buffer[offset++] = (session_id>>16)&0xff;
    buffer[offset++] = (session_id>>8)&0xff;
    buffer[offset++] = (session_id&0xff);
    gtf_seq_number_offset = offset;                   /* Sequence Number */
    offset += 4;
    buffer[offset++] = 0;                             /* Reserved */
    buffer[offset++] = 0;                             /* Reserved */
    buffer[offset++] = 0;                             /* Reserved */ 
    buffer[offset++] = 0;                             /* Reserved */

    if (add_data_tlv) {
        tmp = offset - gtf_pkt_len_of_int_hdr;
        gtf_data_tlv_len = gtf_packet_length[0][0] - tmp - 4 - 3;     /* 4bytes CRC for whole packet + 3bytes Data TLV Header */
        gtf_data_tlv_len = gtf_add_crc?(gtf_data_tlv_len-4):gtf_data_tlv_len;
        gtf_data_tlv_len = gtf_add_end_tlv?(gtf_data_tlv_len-1):gtf_data_tlv_len;
	
        buffer[offset++] = gtf_data_tlv_type;                      /* Data TLV Type */
        buffer[offset++] = (gtf_data_tlv_len>>8)&0xff;             /* Data TLV Length */
        buffer[offset++] = (gtf_data_tlv_len&0xff);                /* Data TLV Length */
    }
    
    return offset;
}

/* 
 *  FD PDU Header format: Refer to MEF Sat spec.
 */
int _insert_fd_pdu(uint8* buffer, int byte_start_offset, 
    uint8 mel, 
	uint8 version,
	uint8 opcode,
	uint8 flags,
	uint8 tlv_offset,
	uint8 add_data_tlv)
{    
    int i = 0, tmp = 0, offset = byte_start_offset; 
	
    buffer[offset++] = ((mel&0x7)<<5)|(version&0x1f);
    buffer[offset++] = opcode;
    buffer[offset++] = flags;
    buffer[offset++] = tlv_offset;                    /* TLV Offset: 32 */

    gtf_timestamp_offset = offset;
    for (i = 0; i < 8; i++) {                    /* TxTimeStampf: 8bytes */
        buffer[offset++] = 0;
    }
	
    for (i = 0; i < 8; i++) {                    /* RxTimeStampf: 8bytes */
        buffer[offset++] = 0;
    }

    for (i = 0; i < 8; i++) {                    /* TxTimeStampb: 8bytes */
        buffer[offset++] = 0;
    }

    for (i = 0; i < 8; i++) {                    /* Reserved for DMR receiving equipment: 8bytes */
        buffer[offset++] = 0x0;
    }
   
    return offset;
}

/* 
 *  Dump data in buffer - For debug usage.
 */
void _buffer_dump(uint8* buffer, int len) {
    int i = 0;
    printf("  len:%d\n", len);
    printf("  data:");
    for (i = 0; i < len; i++) {
        if ((i % 16) == 0) printf("\n");
        else if ((i % 4) == 0)  printf(" ");
        printf("%02x", buffer[i]);
    }
    printf("\n\n");
}

/* 
 *  FL Packet format will be :
 *    PTCH_2-O-ITMH-O-PPH-O-EthernetHeader-O-FL PDU for Down MEPs
 */
int _sat_gtf_packet_header_config_fl_down(int unit, uint8* pkt_data) {
    int offset = 0;

    /* 1. Prepare internal header */
    if (!is_xgs) {
        if (is_device_or_above(unit, JERICHO2)) {
            sal_memset(pkt_data, 0, 64);
            gtf_add_end_tlv =0;
            /* 1.1 Prepare internal DUNE header: PTCH */
            offset = _insert_dnx_ptch2(pkt_data, offset, 
                /* next_header_is_itmh */ 1, 
                /* opaque_value */ 0,
                /* pp_ssp */ gtf_source_port);

            /* 1.2 Prepare internal DUNE header: ITMH */
            offset = _insert_dnx_itmh_jer2(pkt_data, offset,
                /* dst_sys_port */ gtf_dst_sys_port,
                /* fwd_tc */ 0,
                /* fwd_dp */ 0,
                /* snoop_cmd */ 0,
                /* in_mirr_flag */ 0,
                /* pph_type */ 0);
            if (gtf_debug_on) {
                printf("_sat_gtf_packet_header_config_fl_down: After _insert_dnx_itmh:\n");
                _buffer_dump(pkt_data, offset);
            }
        }else {
            /* 1.1 Prepare internal DUNE header: PTCH */
            offset = _insert_dnx_ptch2(pkt_data, offset,
                /* next_header_is_itmh */ 1,
                /* opaque_value */ 7,
                /* pp_ssp */ gtf_source_port);
            if (gtf_debug_on) {
                printf("After _insert_dnx_ptch2:\n");
                _buffer_dump(pkt_data, offset);
            }

            /* 1.2 Prepare internal DUNE header: ITMH */
            offset = _insert_dnx_itmh_jer(pkt_data, offset,
                /* dst_sys_port */ gtf_dst_sys_port,
                /* fwd_tc */ 3,
                /* fwd_dp */ 0,
                /* snoop_cmd */ 0,
                /* in_mirr_flag */ 0,
                /* pph_type */ 1);
            if (gtf_debug_on) {
                printf("After _insert_dnx_itmh:\n");
                _buffer_dump(pkt_data, offset);
            }

           /* 1.3 Prepare internal DUNE header: PPH */
           offset = _insert_dnx_pph_arad(pkt_data, offset);    
           if (gtf_debug_on) {
               printf("After _insert_dnx_pph:\n");
               _buffer_dump(pkt_data, offset);
           }
        }
    }
	else {
	    /* 1.1 Prepare XGS OLP L2 Header */
        offset = _insert_xgs_olp_l2(pkt_data, offset, 
            /* dest_addr */ xgs_mac, 
            /* src_addr */ olp_mac, 
            /* tpid */ olp_tpid, 
            /* vlan */ olp_vlan, 
            /* vlan_pri */ 0, 
            /* vlan_cfi */ 0, 
			/* ether_type */ olp_ether_type);
        if (gtf_debug_on) {
            printf("After _insert_xgs_olp_l2:\n");
            _buffer_dump(pkt_data, offset);
        }
		
		/* 1.2 Prepare XGS OLP TX Header */
        offset = _insert_xgs_olp_tx(pkt_data, offset, 
            /* is_down_mep */ 1, 
            /* modid */ gtf_dst_modid, 
            /* port */ gtf_dst_port,
            /* timestamp_action */ 0,
            /* oam_offset */ 0);
        if (gtf_debug_on) {
            printf("After _insert_xgs_olp_tx:\n");
            _buffer_dump(pkt_data, offset);
        }
	}

    gtf_pkt_len_of_int_hdr = offset;

    /* 2. Prepare Ethernet header */
    offset = _insert_eth(pkt_data, offset, 
        pkt_dmac, 
        pkt_smac, 
        gtf_tpids,
        pkt_vlans, 
        pkt_vlan_pri, 
        pkt_vlan_cfi, 
        gtf_no_tags, 
        pkt_ether_type);
    if (gtf_debug_on) {
        printf("After _insert_eth:\n");
        _buffer_dump(pkt_data, offset);
    }
    
    /* 3. Prepare FL PDU header */
    offset = _insert_fl_pdu(pkt_data, offset, 
        gtf_mel, 
        gtf_version, 
        gtf_fl_opcode, 
        gtf_flags, 
		/* tlv_offset */ 16,
        gtf_source_mep_id, 
        gtf_session_id, 
        gtf_add_data_tlv);
    if (gtf_debug_on) {
        printf("After _insert_fl_pdu:\n");
        _buffer_dump(pkt_data, offset);
    }
    
    return offset;
}

/* 
 *  FL Packet format will be :
 *    PTCH-2-O-EthernetHeader-O-FL PDU for Up MEPs
 */
int _sat_gtf_packet_header_config_fl_up(int unit, uint8* pkt_data) {
    int offset = 0;

    /* 1. Prepare internal header */
    if (!is_xgs) {
        /*1.1 Prepare internal DUNE header: PTCH */
        offset = _insert_dnx_ptch2(pkt_data, offset, 
            /* next_header_is_itmh */ 0, 
            /* opaque_value */ 7, 
            /* pp_ssp */ gtf_source_port);
        if (gtf_debug_on) {
            printf("After _insert_dnx_ptch2:\n");
            _buffer_dump(pkt_data, offset);
        }
    }
	else {
	    /* 1.1 Prepare XGS OLP L2 Header */
        offset = _insert_xgs_olp_l2(pkt_data, offset, 
            /* dest_addr */ xgs_mac, 
            /* src_addr */ olp_mac, 
            /* tpid */ olp_tpid, 
            /* vlan */ olp_vlan, 
            /* vlan_pri */ 0, 
            /* vlan_cfi */ 0, 
			/* ether_type */ olp_ether_type);
        if (gtf_debug_on) {
            printf("After _insert_xgs_olp_l2:\n");
            _buffer_dump(pkt_data, offset);
        }
		
		/* 1.2 Prepare XGS OLP TX Header */
        offset = _insert_xgs_olp_tx(pkt_data, offset, 
            /* is_down_mep */ 0, 
            /* modid */ gtf_dst_modid, 
            /* port */ gtf_dst_port,
            /* timestamp_action */ 0,
            /* oam_offset */ 0);
        if (gtf_debug_on) {
            printf("After _insert_xgs_olp_tx:\n");
            _buffer_dump(pkt_data, offset);
        }
	}

    
	gtf_pkt_len_of_int_hdr = offset;
	
    /*2. Prepare Ethernet header */
    offset = _insert_eth(pkt_data, offset, 
        pkt_dmac, 
		pkt_smac, 
		gtf_tpids, 
		pkt_vlans, 
		pkt_vlan_pri, 
		pkt_vlan_cfi, 
		gtf_no_tags, 
		pkt_ether_type);
    if (gtf_debug_on) {
        printf("After _insert_eth:\n");
        _buffer_dump(pkt_data, offset);
    }
    
    /*3. Prepare FL PDU header */
    offset = _insert_fl_pdu(pkt_data, offset, 
        gtf_mel,
        gtf_version,
        gtf_fl_opcode,
        gtf_flags,
		/* tlv_offset */ 16,
        gtf_source_mep_id,
        gtf_session_id, 
        gtf_add_data_tlv);
    if (gtf_debug_on) {
        printf("After _insert_fl_pdu:\n");
        _buffer_dump(pkt_data, offset);
    }
    
    return offset;
}

/* 
 *  FD Packet format will be :
 *    PTCH-2-O-ITMH-O-OAM-TS-O-EthernetHeader-O-FL PDU for Down MEPs
 */
int _sat_gtf_packet_header_config_fd_down(int unit, uint8* pkt_data) {
    int offset = 0;
    int oam_sub_type = 3;
    uint64 oam_ts_data;

    /* 1. Prepare internal header */
    if (!is_xgs) {
        /*1.1 Prepare internal DUNE header: PTCH */
        offset = _insert_dnx_ptch2(pkt_data, offset, 
            /* next_header_is_itmh */ 1, 
            /* opaque_value */ 7, 
            /* pp_ssp */ gtf_source_port);
        if (gtf_debug_on) {
            printf("After _insert_dnx_ptch2:\n");
            _buffer_dump(pkt_data, offset);
        }
    
        /*1.2 Prepare internal DUNE header: ITMH */
        offset = _insert_dnx_itmh_jer(pkt_data, offset, 
            /* dst_sys_port */ gtf_dst_sys_port, 
            /* fwd_tc */ 3, 
            /* fwd_dp */ 0, 
            /* snoop_cmd */ 0, 
            /* in_mirr_flag */ 0, 
            /* pph_type */ 3);
        if (gtf_debug_on) {
            printf("After _insert_dnx_itmh:\n");
            _buffer_dump(pkt_data, offset);
        }

        /*1.3 Prepare internal DUNE header: OAM-TS */
        bcm_sat_config_t config;
        int rv = BCM_E_NONE;
        rv = bcm_sat_config_get(unit, &config);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_sat_config_get\n");
            return rv;
        }
        if (config.timestamp_format == bcmSATTimestampFormatIEEE1588v1){
            oam_sub_type = 2;
        }
            
        
        COMPILER_64_ZERO(oam_ts_data);
        offset = _insert_dnx_oam_ts(pkt_data, offset, 
            /* type */ 0, 
            /* sub_type */ oam_sub_type,
            /* is_up_mep */ 0,
            /* ts_data */ oam_ts_data,
            /* ts_offset */ gtf_ts_offset);
        if (gtf_debug_on) {
            printf("After _insert_dns_oam_ts:\n");
            _buffer_dump(pkt_data, offset);
        }

        /*1.4 Prepare internal DUNE header: PPH */
       offset = _insert_dnx_pph_arad(pkt_data, offset);    
       if (gtf_debug_on) {
           printf("After _insert_dnx_pph:\n");
           _buffer_dump(pkt_data, offset);
       }
    }
    else {
	    /* 1.1 Prepare XGS OLP L2 Header */
        offset = _insert_xgs_olp_l2(pkt_data, offset, 
            /* dest_addr */ xgs_mac, 
            /* src_addr */ olp_mac, 
            /* tpid */ olp_tpid, 
            /* vlan */ olp_vlan, 
            /* vlan_pri */ 0, 
            /* vlan_cfi */ 0, 
			/* ether_type */ olp_ether_type);
        if (gtf_debug_on) {
            printf("After _insert_xgs_olp_l2:\n");
            _buffer_dump(pkt_data, offset);
        }
		
		/* 1.2 Prepare XGS OLP TX Header */
        offset = _insert_xgs_olp_tx(pkt_data, offset, 
            /* is_down_mep */ 1, 
            /* modid */ gtf_dst_modid, 
            /* port */ gtf_dst_port,
            /* timestamp_action */ olp_timestamp_action,
            /* oam_offset */ gtf_fd_timestamp_offset);
        if (gtf_debug_on) {
            printf("After _insert_xgs_olp_tx:\n");
            _buffer_dump(pkt_data, offset);
        }
    }

    gtf_pkt_len_of_int_hdr = offset;

    /*4. Prepare Ethernet header */
    offset = _insert_eth(pkt_data, offset, 
        pkt_dmac, 
        pkt_smac, 
        gtf_tpids, 
        pkt_vlans, 
        pkt_vlan_pri, 
        pkt_vlan_cfi, 
        gtf_no_tags, 
        pkt_ether_type);
    if (gtf_debug_on) {
        printf("After _insert_eth:\n");
        _buffer_dump(pkt_data, offset);
    }
    
    /*5. Prepare FD PDU header */
    offset = _insert_fd_pdu(pkt_data, offset,
        gtf_mel, 
        gtf_version, 
        gtf_fd_opcode,
        gtf_flags,
        /* tlv_offset */ 32,
        gtf_add_data_tlv);
    if (gtf_debug_on) {
        printf("After _insert_fd_pdu:\n");
        _buffer_dump(pkt_data, offset);
    }
    
    return offset;
}

/*
 * setup an gtf:
 *    1. Create an GTF Object
 *    2. Configure packet info, including header type, header info, payload, packet edit info and packet offset.
 *    3. Set Bandwidth
 *    4. Set Rate Pattern
 */
int sat_gtf_setup(int unit, bcm_sat_gtf_t *gtf_id) {
    int rv = 0;    
    bcm_sat_gtf_packet_config_t config;
    bcm_pkt_t *sat_header;
    uint8 pkt_data[128] = {0};
    int pkt_size = 0;
    bcm_sat_gtf_bandwidth_t bw;
    bcm_sat_gtf_rate_pattern_t rate_pattern;
    int i = 0, priority = 0;
    bcm_gport_t dst_local_port;
    bcm_gport_t dst_sys_port;

    if (!is_xgs && !is_device_or_above(unit, JERICHO2)) {
        BCM_GPORT_LOCAL_SET(dst_local_port, gtf_dst_port);    
        rv = bcm_stk_gport_sysport_get(unit, dst_local_port, &dst_sys_port);
        if (rv != BCM_E_NONE) {
            printf("Failed($rv) in bcm_stk_gport_sysport_get\n");
            return rv;
        }
        gtf_dst_sys_port = BCM_GPORT_SYSTEM_PORT_ID_GET(dst_sys_port);
        print gtf_dst_sys_port;
    }

    if ((gtf_test_mode == gtf_test_mode_fl_up) || (gtf_test_mode == gtf_test_mode_fd_up)) {
        bcm_port_t port;

        if (is_xgs || is_device_or_above(unit, JERICHO2)) {
            port = gtf_source_port;
            printf("UP-MEP: Simulate Traffic generated from %d and egress from %d \n", gtf_dst_port, gtf_source_port); 
        } else {
            port = gtf_dst_port;
        }
        
        /* add l2 addr for UP Mep test */
        rv = l2_addr_add(unit, pkt_dmac, pkt_vlans[0], port);
        if (rv != BCM_E_NONE) {
            printf("Failed($rv) in l2_addr_add\n");
            return rv;
        }
		
        /* for egress port, configure to be transmitted as tagged */
        rv = set_port_keep_tag(unit, pkt_vlans[0], port);
        if (rv != BCM_E_NONE) {
            printf("Failed($rv) in set_port_keep_tag\n");
            return rv;
        }
    } 

    /* 1. Create GTF */
    rv = bcm_sat_gtf_create(unit, 0, gtf_id);
    if (rv != BCM_E_NONE) {
        printf("Failed($rv) in bcm_sat_gtf_create\n");
        return rv;
    }
    if (gtf_debug_on) printf("bcm_sat_gtf_create success\n");

    /* 2. Set Bandwidth */
    bw.rate = gtf_bw_cir_rate;
    bw.max_burst = gtf_bw_cir_max_burst;
    rv = bcm_sat_gtf_bandwidth_set(unit, *gtf_id, 0, &bw);
    if (rv != BCM_E_NONE) {
        printf("Failed($rv) in bcm_sat_gtf_bandwidth_set\n");
        return rv;
    }
    if (gtf_debug_on) printf("bcm_sat_gtf_bandwidth_set for cir success\n");

    bw.rate = gtf_bw_eir_rate;
    bw.max_burst = gtf_bw_eir_max_burst;
    rv = bcm_sat_gtf_bandwidth_set(unit, *gtf_id, 1, &bw);
    if (rv != BCM_E_NONE) {
        printf("Failed($rv) in bcm_sat_gtf_bandwidth_set\n");
        return rv;
    }
    if (gtf_debug_on) printf("bcm_sat_gtf_bandwidth_set for eir success\n");

    bw.rate = gtf_bw_cir_rate + gtf_bw_eir_rate;
    bw.max_burst = gtf_bw_cir_max_burst+ gtf_bw_eir_max_burst;
    rv = bcm_sat_gtf_bandwidth_set(unit, *gtf_id, -1, &bw);
    if (rv != BCM_E_NONE) {
        printf("Failed($rv) in bcm_sat_gtf_bandwidth_set\n");
        return rv;
    }
    if (gtf_debug_on) printf("bcm_sat_gtf_bandwidth_set for eir success\n");

    /* 3. Set Rate Pattern */
    rate_pattern.rate_pattern_mode = gtf_rate_pattern_mode;
    rate_pattern.high_threshold = gtf_rate_pattern_high;
    rate_pattern.low_threshold = gtf_rate_pattern_low;
    rate_pattern.stop_iter_count = gtf_rate_pattern_stop_iter_count;
    rate_pattern.stop_burst_count = gtf_rate_pattern_stop_burst_count;
    rate_pattern.stop_interval_count = gtf_rate_pattern_stop_interval_count;
    rate_pattern.burst_packet_weight = gtf_rate_pattern_burst_packet_weight;
    rate_pattern.interval_packet_weight = gtf_rate_pattern_interval_packet_weight;
    rv = bcm_sat_gtf_rate_pattern_set(unit, *gtf_id, 0, &rate_pattern);
    if (rv != BCM_E_NONE) {
        printf("Failed($rv) in bcm_sat_gtf_rate_pattern_set\n");
        return rv;
    }
    if (gtf_debug_on) printf("bcm_sat_gtf_rate_pattern_set success\n");

    rv = bcm_sat_gtf_rate_pattern_set(unit, *gtf_id, 1, &rate_pattern);
    if (rv != BCM_E_NONE) {
        printf("Failed($rv) in bcm_sat_gtf_rate_pattern_set\n");
        return rv;
    }
    if (gtf_debug_on) printf("bcm_sat_gtf_rate_pattern_set success\n");

    /* 4. Configure packet */
    bcm_sat_gtf_packet_config_t_init(&config);
    
    /* 4.1 Configure header type*/
    config.sat_header_type = gtf_header_type;
    
    /* 4.2 Configure header info */
    rv = bcm_pkt_alloc(unit, 128, 0, &sat_header); 
    if (rv != BCM_E_NONE) {
        printf("Failed($rv) in bcm_pkt_alloc\n");
        return rv;
    }
    if (gtf_debug_on) printf("bcm_pkt_alloc success\n");

    switch(gtf_test_mode) {
        case gtf_test_mode_fl_down:
            pkt_size = _sat_gtf_packet_header_config_fl_down(unit, sat_header->pkt_data->data);
            break;
        case gtf_test_mode_fl_up:
            pkt_size = _sat_gtf_packet_header_config_fl_up(unit, sat_header->pkt_data->data);
            break;
        case gtf_test_mode_fd_down:
		    pkt_size = _sat_gtf_packet_header_config_fd_down(unit, sat_header->pkt_data->data);
			break;
        default:
            printf("ERR: wrong gtf_test_mode(%d)\n", gtf_test_mode);
            return BCM_E_PARAM;
    }

    sat_header->pkt_data->len = pkt_size;

    config.header_info.pkt_data = sat_header->pkt_data;    
    config.header_info.blk_count = 1;

    /* 4.3 Configure payload */
    config.payload.payload_type = gtf_payload_type;
	if (config.payload.payload_type != bcmSatPayloadPRBS) {
        for (i = 0; i < BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE; i++) {
	        config.payload.payload_pattern[i] = gtf_payload_pattern[i];
	    }
	}
    
    /* 4.4 Configure packet edit */
    for (priority = 0; priority < 2; priority++) {
        for (i = 0; i < BCM_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS; i++) {
            config.packet_edit[priority].packet_length[i] = gtf_packet_length[priority][i] + gtf_pkt_len_of_int_hdr - 4;  /* subtract 4bytes CRC for whole packet */
        }
        config.packet_edit[priority].pattern_length = gtf_pattern_length[priority];
        for (i = 0; i < gtf_pattern_length[priority]; i++) {
            config.packet_edit[priority].packet_length_pattern[i] = gtf_packet_length_pattern[priority][i];
        }
        config.packet_edit[priority].number_of_stamps = gtf_number_of_stamps[priority];
        for (i = 0; i < gtf_number_of_stamps[priority]; i++) {
            config.packet_edit[priority].stamps[i].stamp_type = gtf_stamp_type[priority][i];
            config.packet_edit[priority].stamps[i].field_type = gtf_stamp_field[priority][i];
            config.packet_edit[priority].stamps[i].inc_step = gtf_stamp_inc_step[priority][i];
            config.packet_edit[priority].stamps[i].inc_period_packets = gtf_stamp_inc_period_packets[priority][i];
            config.packet_edit[priority].stamps[i].value = gtf_stamp_value[priority][i];
            config.packet_edit[priority].stamps[i].offset = gtf_stamp_offset[priority][i] + gtf_pkt_len_of_int_hdr*8;
        }
        config.packet_edit[priority].number_of_ctfs = gtf_number_of_ctfs[priority];

        if (gtf_add_end_tlv) {
            config.packet_edit[priority].flags |= BCM_SAT_GTF_PACKET_EDIT_ADD_END_TLV;
        }
        if (gtf_add_crc) {
            config.packet_edit[priority].flags |= BCM_SAT_GTF_PACKET_EDIT_ADD_CRC;
        }
    }

    /* 4.5 Configure packet offset */
    config.offsets.seq_number_offset = gtf_seq_number_offset;
    config.offsets.crc_byte_offset = gtf_crc_byte_offset + gtf_pkt_len_of_int_hdr;
    config.offsets.timestamp_offset = gtf_timestamp_offset;

    if (is_xgs == 0) {
	    config.packet_context_id = 0;
    }
	
    rv = bcm_sat_gtf_packet_config(unit, *gtf_id, &config);
    if (rv != BCM_E_NONE) {
        printf("Failed($rv) in bcm_sat_gtf_packet_config\n");
        return rv;
    }
    if (gtf_debug_on) printf("bcm_sat_gtf_packet_config success\n");

    /* 5. Start packet generation */
    rv = bcm_sat_gtf_packet_start(unit, *gtf_id, bcmSatGtfPriAll);
    if (rv != BCM_E_NONE) {
        printf("Failed($rv) in bcm_sat_gtf_packet_start\n");
        return rv;
    }
    if (gtf_debug_on) printf("bcm_sat_gtf_packet_start success\n");

    printf("create gtf(%d) success\n", *gtf_id);
    return rv;  
}

int sat_gtf_cleanup(int unit, bcm_sat_gtf_t gtf_id) {
    int rv = 0;
    bcm_sat_gtf_bandwidth_t bw;

    /* Reset GTF bandwidth */
    bw.rate = 0;
    bw.max_burst = 0;  
    rv = bcm_sat_gtf_bandwidth_set(unit, gtf_id, 0, &bw);
    if (rv != BCM_E_NONE) {
        printf("Failed($rv) in bcm_sat_gtf_bandwidth_set\n");
        return rv;
    }
    rv = bcm_sat_gtf_bandwidth_set(unit, gtf_id, 0, &bw);
    if (rv != BCM_E_NONE) {
        printf("Failed($rv) in bcm_sat_gtf_bandwidth_set\n");
        return rv;
    }
    rv = bcm_sat_gtf_bandwidth_set(unit, gtf_id, 0, &bw);
    if (rv != BCM_E_NONE) {
        printf("Failed($rv) in bcm_sat_gtf_bandwidth_set\n");
        return rv;
    }

    /* Destroy the GTF Object */
    rv = bcm_sat_gtf_destroy(unit, gtf_id);
    if (BCM_E_NONE != rv) {
        printf("Failed($rv) in bcm_sat_gtf_destroy\n");
        return rv;
    }
    if (gtf_debug_on) printf("bcm_sat_gtf_destroy success\n");

    return rv;
}

int sat_gtf_enable(int unit, bcm_sat_gtf_t gtf_id, int priority, int is_enable) {
    int rv = 0;

    if (is_enable) {
        /* Start packet generation */
        rv = bcm_sat_gtf_packet_start(unit, gtf_id, priority);
        if (BCM_E_NONE != rv) {
            printf("Failed($rv) in bcm_sat_gtf_packet_start\n");
            return rv;
        }
        if (gtf_debug_on) printf("bcm_sat_gtf_packet_start success\n");
    }
    else {
        /* Stop packet generation */
        rv = bcm_sat_gtf_packet_stop(unit, gtf_id, priority);
        if (BCM_E_NONE != rv) {
            printf("Failed($rv) in bcm_sat_gtf_packet_stop\n");
            return rv;
        }
        if (gtf_debug_on) printf("bcm_sat_gtf_packet_stop success\n");
    }

    return rv;
}

int sat_gtf_show(int unit, bcm_sat_gtf_t gtf_id) {
    int rv = 0;
    bcm_sat_gtf_bandwidth_t bw;
    bcm_sat_gtf_rate_pattern_t rate_pattern; 
    uint64 stat_value;
    int priority = 0;
    bcm_sat_gtf_stat_multi_t stat;

    /* 1. Get Bandwidth */
    for (priority = -1; priority < 2; priority++) {
        rv = bcm_sat_gtf_bandwidth_get(unit, gtf_id, priority, &bw);
        if (rv != BCM_E_NONE) {
            printf("Failed($rv) in bcm_sat_gtf_bandwidth_get\n");
            return rv;
        }
        if (gtf_debug_on) printf("bcm_sat_gtf_bandwidth_get success\n");
        print bw;
    }

    /* 2. Get Rate Pattern */
    for (priority = 0; priority < 2; priority++) {
        rv = bcm_sat_gtf_rate_pattern_get(unit, gtf_id, priority, &rate_pattern);
        if (rv != BCM_E_NONE) {
            printf("Failed($rv) in bcm_sat_gtf_rate_pattern_get\n");
            return rv;
        }
        if (gtf_debug_on) printf("bcm_sat_gtf_rate_pattern_get success\n");
        print rate_pattern;
    }

    /* 3. Get Stat */
    if (is_device_or_above(unit, JERICHO2)) {
        rv = bcm_sat_gtf_stat_multi_get(unit, gtf_id, &stat);
        if (rv != BCM_E_NONE) {
            printf("Failed($rv) in bcm_sat_gtf_stat_multi_get\n");
            return rv;
        }
        if (gtf_debug_on) printf("bcm_sat_gtf_stat_multi_get success\n");
        print stat.transmitted_packet_cnt;
    } else {
        for (priority = 0; priority < 2; priority++) {
            rv = bcm_sat_gtf_stat_get(unit, gtf_id, priority, 0, bcmSatGtfStatPacketCount, &stat_value);
            if (rv != BCM_E_NONE) {
                printf("Failed($rv) in bcm_sat_gtf_stat_get\n");
                return rv;
            }
            if (gtf_debug_on) printf("bcm_sat_gtf_stat_get success\n");
            print stat_value;
        }
    }
    return 0;
}

int sat_gtf_stat_show(int unit, bcm_sat_gtf_t gtf_id) {
    int rv = 0;
    uint64 stat_value;
    int priority = 0;

    /* 1. Get Stat */
    for (priority = 0; priority < 2; priority++) {
        rv = bcm_sat_gtf_stat_get(unit, gtf_id, priority, 0, bcmSatGtfStatPacketCount, &stat_value);
        if (rv != BCM_E_NONE) {
            printf("Failed($rv) in bcm_sat_gtf_stat_get\n");
            return rv;
        }
        if (gtf_debug_on) printf("bcm_sat_gtf_stat_get success\n");
        print stat_value;
    }
    
    return 0;
}

/*
*  In Jericho, the gtf counters can't get correctly both during the SAT traffic and also after stopping SAT traffic
*  avariable  if the sequence number is enabled.
*/
int get_gtf_tx_counter(int chip, bcm_sat_gtf_t gtf_id_wk)
{
    int rv=0;
    int priority=0;
    uint32 flags=0;
    bcm_sat_gtf_stat_counter_t type=0;
    uint32 value_last=0;
    int cnt;
    uint32 total_hi=0;
    uint32 total_lo=0;
    uint32 lo_work=0;
    mem_val value = {0};

    /*start GTF pkt*/
    rv |= bcm_sat_gtf_packet_start(chip, gtf_id_wk, bcmSatGtfPriCir);

    printf("The transmission of pkt begins.\n");
    printf("... It waits for 10 seconds.\n");
    sal_sleep(10);

    for(cnt=0; cnt<10; cnt++)
    {
        lo_work = total_lo;
        /*get sequence number*/
        diag_mem_field_get(0, "OAMP_SAT_TX_GEN_PARAMS", "SEQ_NUM_NXT_VALUE", (16 + gtf_id_wk), value);

        total_lo += (value[0] - value_last);
        if(total_lo < lo_work)
        {
            total_hi += 1;
        }
        /*printf("[cnt=%04d] %08x %08x   ", cnt, total_hi, total_lo);
        print (value[0] - value_last);*/
        value_last = value[0];
    }

    /*stop  pkt*/
    rv |= bcm_sat_gtf_packet_stop(chip, gtf_id_wk, bcmSatGtfPriCir);

    printf("The transmission of pkt is stopped.\n");
    printf("... It waits for 1 seconds.\n");
    sal_sleep(1);

    /*get sequence number*/
    diag_mem_field_get(0, "OAMP_SAT_TX_GEN_PARAMS", "SEQ_NUM_NXT_VALUE", (16 + gtf_id_wk) , value);

    total_lo += (value[0] - value_last);
    if(total_lo < lo_work)
    {
        total_hi += 1;
    }

    printf("[cnt=%04d] %08x %08x   ", cnt, total_hi, total_lo);
    print value[0];
    gtf_counters = value[0];
    
    return 0;
}
int sat_setup(int unit) {

    bcm_sat_config_t config;
    int rv = BCM_E_NONE;

    /* global configuration */
    bcm_sat_config_t_init(&config);
    config.timestamp_format = bcmSATTimestampFormatNTP;
    config.config_flags = 0;
    rv = bcm_sat_config_set(unit, &config);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_config_set\n");
        return rv;
    }
    if (is_device_or_above(unit, JERICHO2)) {
        gtf_dst_sys_port = dnx_gtf_dst_sys_port;
        gtf_packet_length[0][0] = dnx_gtf_min_packet_length;
        gtf_packet_length[1][0] = dnx_gtf_min_packet_length;
    }

    /* setup a GTF */
    rv = sat_gtf_setup(unit, &gtf_id);
    if (BCM_E_NONE != rv) {
        printf("Failed($rv) in sat_gtf_setup\n");
        return rv;
    }

    /* show GTF info */
    rv = sat_gtf_show(unit, gtf_id);
    if (BCM_E_NONE != rv) {
        printf("Failed($rv) in sat_gtf_show\n");
        return rv;
    }

    /* setup a CTF */
    rv = ctf_setup(unit);
    if (BCM_E_NONE != rv) {
        printf("Error in ctf_setup\n");
        return rv;
    }

    return rv;

}
/**
* \brief
*  Add the PMF field group that SAT uses it to capture matched packets to SAT engine.
* \param [in] unit  - Device ID
* \param [in] in_port  - IN port
* \param [in] ctf_id   - CTF  ID
* \param [in] oam_offset - OAM offset to set in the OAM PMF action
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int ctf_pmf_config_jr2(int unit,int in_port,bcm_gport_t sat_gport,int ctf_id,int oam_offset)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    int rv = BCM_E_NONE;
    int trap_code_qualifier;
    uint32 attach_presel = 127;
    bcm_gport_t gport;

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyInPort;
    /*fg_info.qual_types[1] = bcmFieldQualifyStageIngress;*/

    if (oam_packets == 1){
        /* Set actions */
        fg_info.nof_actions = 2;
        fg_info.action_types[0] = bcmFieldActionTrap;  /*bcmFieldActionTrapCodeQualifier */
        fg_info.action_types[1] = bcmFieldActionOam;
    } else {
        /* Set actions */
        fg_info.nof_actions = 1;
        fg_info.action_types[0] = bcmFieldActionTrap;  /*bcmFieldActionTrapCodeQualifier */
    }
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &sat_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in iPMF1 \n", sat_fg_id);

    /**Attach context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &sat_field_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    if (oam_packets ==1){
        attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    }


    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;


    rv = bcm_field_group_context_attach(unit, 0, sat_fg_id, sat_field_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached  FG (%d) to context (%d)\n",sat_fg_id,sat_field_context_id);

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    ent_info.nof_entry_quals = 1;

    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    /**in port 13*/
    BCM_GPORT_LOCAL_SET(gport, in_port);
    ent_info.entry_qual[0].value[0] = gport;
    ent_info.entry_qual[0].mask[0] = 0xffffffff;

    trap_code_qualifier = ctf_id;
    if (oam_packets ==1) {
        ent_info.nof_entry_actions = 2;
    } else {
        ent_info.nof_entry_actions = 1;
    }
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = sat_gport;
    ent_info.entry_action[0].value[1] = trap_code_qualifier;
    if (oam_packets == 1) {
        ent_info.entry_action[1].type = fg_info.action_types[1];
        ent_info.entry_action[1].value[0] = 0;
        ent_info.entry_action[1].value[1] = oam_offset;
        ent_info.entry_action[1].value[2] = 0;
        ent_info.entry_action[1].value[3] = 0;
    }
    rv = bcm_field_entry_add(unit, 0, sat_fg_id, &ent_info, &sat_fg_entry_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("Entry add: id:(0x%x)  Src_ip:(0x%x) tc_val:(%d)\n",sat_fg_entry_id,ent_info.entry_qual[0].value[0],
                                                                            ent_info.entry_action[0].value[0] );

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = attach_presel;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = sat_field_context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    if (oam_packets == 1){
        p_data.qual_data[0].qual_value = bcmFieldLayerTypeEth;
    }else {
        p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    }
    p_data.qual_data[0].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(Ethernet) \n",
            attach_presel,sat_field_context_id);

    return 0;
}


/*
 * Create a FG to capture matched packets to SAT engine.
 * The filter is TTL and PROTOCOL fields from the IPv4 header.
 */
int ctf_pmf_config_jr2_conditions(int unit,int in_port,bcm_gport_t sat_gport,int ctf_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    int rv = BCM_E_NONE;
    int trap_code_qualifier;
    uint32 attach_presel = 127;
    bcm_gport_t gport;

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Set quals */
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = bcmFieldQualifyIp4Ttl;
    fg_info.qual_types[1] = bcmFieldQualifyIp4Protocol;


    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionTrap;  /*bcmFieldActionTrapCodeQualifier */

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &sat_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in iPMF1 \n", sat_fg_id);

    /**Attach context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &sat_field_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];

    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;

    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[1].input_arg = 1;
    attach_info.key_info.qual_info[1].offset = 0;


    rv = bcm_field_group_context_attach(unit, 0, sat_fg_id, sat_field_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached  FG (%d) to context (%d)\n",sat_fg_id,sat_field_context_id);

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    ent_info.nof_entry_quals = 2;

    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = cint_sat_fields_info.ttl;
    ent_info.entry_qual[0].mask[0] = 0xff;
    ent_info.entry_qual[1].type = fg_info.qual_types[1];
    ent_info.entry_qual[1].value[0] = cint_sat_fields_info.protocol;
    ent_info.entry_qual[1].mask[0] = 0xff;


    trap_code_qualifier = ctf_id;
    ent_info.nof_entry_actions = 1;

    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = sat_gport;
    ent_info.entry_action[0].value[1] = trap_code_qualifier;
    rv = bcm_field_entry_add(unit, 0, sat_fg_id, &ent_info, &sat_fg_entry_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("Entry add: id:(0x%x)  Src_ip:(0x%x) tc_val:(%d)\n",sat_fg_entry_id,ent_info.entry_qual[0].value[0],
                                                                            ent_info.entry_action[0].value[0] );

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = attach_presel;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = sat_field_context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0xFF;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(IPv4) \n",
            attach_presel,sat_field_context_id);
    return 0;
}
/**
* \brief
*  Delete the PMF field group that SAT uses it.
* \param [in] unit             - Device ID
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int ctf_pmf_destroy_jr2(int unit)
{
    int rv = BCM_E_NONE;
    int ii= 0;
    bcm_field_entry_qual_t entry_qual_info[BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP];
    uint32 attach_presel = 127;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = attach_presel;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = FALSE;
    p_data.context_id = 0;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    rv = bcm_field_group_context_detach(unit, sat_fg_id, sat_field_context_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach\n", rv);
        return rv;
    }

    rv = bcm_field_context_destroy(unit,bcmFieldStageIngressPMF1,sat_field_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy eth\n", rv);
        return rv;
    }

    rv = bcm_field_entry_delete(unit, sat_fg_id, &entry_qual_info, sat_fg_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete\n", rv);
        return rv;
    }
  
    rv = bcm_field_group_delete(unit, sat_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete\n", rv);
        return rv;
    }
    return 0;

}

int sat_cleanup(int unit) {

    int rv = BCM_E_NONE;

    /* clean up CTF */
    rv = ctf_setup_cleanup(unit);
    if (BCM_E_NONE != rv) {
        printf("Error in ctf_setup_cleanup\n");
        return rv;
    }

    /* clean up GTF */
    rv = sat_gtf_cleanup(unit, gtf_id);
    if (BCM_E_NONE != rv) {
        printf("Failed($rv) in sat_gtf_cleanup\n");
        return rv;
    }


    return rv;

}
