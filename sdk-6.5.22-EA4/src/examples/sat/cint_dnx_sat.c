/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

/*
 * File: cint_dnx_sat.c
 *
 * Purpose:
 *     Example of SAT applications in JR2.
 *
 * Test Scenario
 *
 * ./bcm.user
 * cint sat/cint_dnx_sat.c
 * cint 
 * print sat_setup(unit, inP, outP);
 *
 * Tests:
 *
 * 1. sat_gtf_enable(unit, sat_info.gtf_id, 1); sat_gtf_show(unit, sat_info.gtf_id);
 *     Expect transmitted_packet_cnt is not zero.
 *     Expect SAT generated packets to be transmitted out from outP.
 *
 * 2. Set outP loopback, then check CTF counters:
 *     sat_ctf_show(unit, sat_info.ctf_id, 1); #clear before read
 *     bcm_port_loopback_set(unit, outP, BCM_PORT_LOOPBACK_MAC);
 *     sat_ctf_show(unit, sat_info.ctf_id, 0);
 *
 *     Expect received_packet_cnt is not zero.
 */

struct sat_info_t
{
    /*
     * One sat_port per core.
     * Default configuraion is 218 (core 0)/219 (core 1).
     */
    int sat_port[2];

    uint32 cir;

    /*
     * ETH header
     */
    bcm_mac_t da;
    bcm_mac_t sa;
    uint16 tpids[2];
    bcm_vlan_t vlan[2];
    int pri[2];
    int nof_tags;

    int mel;
    int tc;
    int dp;
    int trap_id;
    int trap_index;
    int trap_qualifier;
    int session_id; /* 0~63 */

    bcm_field_group_t group;
    bcm_field_entry_t ent;
    bcm_field_context_t context_id;

    bcm_sat_payload_type_t payload_type;
    bcm_sat_timestamp_format_t timestamp_format;

    bcm_sat_gtf_t gtf_id;
    bcm_sat_ctf_t ctf_id;

    /*
     * Seq_Num offset configured in CTF to check SN.
     * Seq_Num offset is counted from the begining of ETH header.
     */
    int seq_num_offset;
    /*
     * Timestamp offset configured in CTF to calculate the delay.
     * Timestamp offset is counted from the begining of ETH header.
     */
    int timestamp_offset;
    /*
     * Payload offset configured in CTF to check payload.
     * Payload can be one of the type in bcm_sat_payload_type_t.
     * Payload offset is counted from the begining of ETH header.
     */
    int payload_offset;
};

sat_info_t sat_info = {
    /* sat_port, cir */
    {218, 219}, 1000,
    /* DA */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},
    /* SA */
    {0x00, 0x12, 0x34, 0x56, 0x78, 0x9a},
    /* tpid[2], vlan[2], pri[2], nof_tags */
    {0x8100, 0}, {15, 0}, {3, 0}, 1,
    /* mel, tc, dp, trap_id, trap_index, trap_qualifier, session_id */
    4, 3, 0, 0, 0, 0xFE, 1
};

struct sat_field_info_t
{
    bcm_field_presel_t presel_id;
    uint32 presel_nof_quals;
    bcm_field_qualify_t presel_qual_type[4];
    uint32 presel_qual_val[4];
    uint32 presel_qual_mask[4];

    /*
     * If presel_nof_quals == 0 then context_id will be used directly.
     * If presel_nof_quals > 0 then context_id saves the created context ID.
     */
    bcm_field_context_t context_id;

    int nof_quals;
    bcm_field_qualify_t qual_type[4];
    bcm_field_qualify_attach_info_t qual_info[4];
    uint32 qual_val[4];
    uint32 qual_mask[4];

    int nof_actions;
    bcm_field_entry_action_t action[4];

    bcm_field_group_t group;
    bcm_field_entry_t ent;
};

sat_field_info_t sat_field_info;

int sat_debug_on = 1;

void dump_buffer(uint8* buffer, int len) {
    int ii = 0;

    printf("  len:%d\n", len);
    printf("  data:");

    for (ii = 0; ii < len; ii++) {
        if ((ii % 16) == 0) {
            printf("\n");
        }
        else if ((ii % 4) == 0) {
            printf(" ");
        }

        printf("%02x", buffer[ii]);
    }

    printf("\n\n");
}

/**
  * PTCH-2:
  * {parser_Program_Control[0], Opaque_PT_Attributes[2:0], In_PP_Port[9:0]}
  */
int build_ptch2(
    uint8* buffer,
    int offset,
    uint8 next_header_is_itmh,
	uint8 opaque_value,
	uint8 pp_ssp)
{
	int parser_program_ctl = 0;

    parser_program_ctl = next_header_is_itmh ? 0 : 1;
    buffer[offset++] = (parser_program_ctl<<7) | (opaque_value<<4);
    buffer[offset++] = pp_ssp;

    return offset;
}

/**
  * ITMH-JR2:
  * {Reserved[0], Forward-Action-Strength[2:0], Injected-ASE-Extension-Present[0],
  *  PPH-Type[1:0], Inbound-Mirror-Disable[0], Drop-Precedence[1:0],
  *  Destination[20:0], Snoop-Profile[4:0], Traffic-Class[2:0], ITMH-Base-Ext-Present[0]}
  */
int build_itmh_jr2(
    uint8* buffer,
    int offset,
    int dst_sys_port,
    uint8 tc,
    uint8 dp,
    uint8 pph_type,
    uint8 ase_present)
{
    int fwd_dest_info = (0x3 << 18) | dst_sys_port;
    int dest_exten = 0;

    buffer[offset++] = ((ase_present & 0x1) << 3) | ((pph_type & 0x3) << 1);
    buffer[offset++] = ((dp & 0x3) << 6) | ((fwd_dest_info & 0x1F8000) >> 15);
    buffer[offset++] = ((fwd_dest_info & 0x7F80) >> 7);
    buffer[offset++] = (fwd_dest_info & 0x7F) << 1;
    buffer[offset++] = ((tc & 0x7) <<1) | (dest_exten & 0x1);

    return offset;
}

/**
  * OAM-ASE:
  * {OAM-Sub-Type[3:0], MEP-Type[0], OAM-TS-Data[33:0], Offset[7:0], Type[0]}
  */
int build_ase_oam(
    uint8* buffer,
    int offset,
	uint8 sub_type,
	uint8 is_up_mep,
	uint8 timestamp_offset)
{
    buffer[offset] = ((sub_type & 0xF) << 4) | ((is_up_mep & 0x1) << 3)	;
    buffer[offset+4] = (timestamp_offset & 0xff) >> 7;
    buffer[offset+5] = (timestamp_offset & 0xff) << 1;

    offset += 6;

    return offset;
}

int build_eth_header(
    uint8* buffer,
    int offset,
    bcm_mac_t da,
    bcm_mac_t sa,
    uint16* tpids,
    bcm_vlan_t* vlans,
    int* vlan_pri,
    int nof_tags,
    uint16 ether_type)
{
    int ii;

    for (ii = 0; ii < 6; ii++) {
        buffer[offset++] = da[ii];
    }

    for (ii = 0; ii < 6; ii++) {
        buffer[offset++] = sa[ii];
    }

    ii = 0;

    while (nof_tags) {
        buffer[offset++] = (tpids[ii] >> 8) & 0xff;
        buffer[offset++] = tpids[ii] & 0xff;
        buffer[offset++] = (vlan_pri[ii] << 5) | ((vlans[ii] >> 8) & 0xff);
        buffer[offset++] = vlans[ii] & 0xff;
        --nof_tags;
        ii++;
    }
 
    buffer[offset++] = (ether_type >> 8) & 0xff;
    buffer[offset++] = ether_type & 0xff;
 
    return offset;
}

/**
  * Build injected ETH packet with PTCH-2+ITMH
  * ETH header:
  *     DA
  *     SA
  *     VLAN
  *     EtherType - 0x0899
  *     Seq_Num - 4B
  *     Timestamp - 8B
  *     Payload - Constant 4B/8B, PRBS starts from here
  */
int sat_header_build(int unit, int inP, int outP, uint8* pkt_data, int *seq_num_offset) {
    int offset = 0;

    offset = build_ptch2(pkt_data, offset, 1, 0, inP);
    offset = build_itmh_jr2(pkt_data, offset, outP, 3, 0, 1, 1);

    sat_info.seq_num_offset = 14+sat_info.nof_tags*4;
    sat_info.timestamp_offset = sat_info.seq_num_offset + 4;
    sat_info.payload_offset = sat_info.timestamp_offset + 8;

    offset = build_ase_oam(pkt_data,
                           offset,
                           (sat_info.timestamp_format == bcmSATTimestampFormatIEEE1588v1)?2:3,
                           0,
                           /* PTCH+ITMH+ASE+PPH, UDH_base is not terminated at ITPP thus is not added to the offset here */
                           2+5+6+12+sat_info.timestamp_offset);

    offset += 13; /* PPH+UDH base */

    offset = build_eth_header(pkt_data, offset,
        sat_info.da,
        sat_info.sa,
        sat_info.tpids,
        sat_info.vlan,
        sat_info.pri,
        sat_info.nof_tags,
        0x0899);

    *seq_num_offset = offset;
    offset += 12;

    if (sat_debug_on) {
        dump_buffer(pkt_data, offset);
    }

    return offset;
}

/*
 *   calculate cbs and granularity in bps mode:
 */
int sat_gtf_calculate_cbs_granularity(uint32 max_rate, uint32 *cbs,  uint32 *granularity, int clock_khz) {
    uint32 min_cbs = 110;

    if (!clock_khz){
        return BCM_E_PARAM;
    }

    *cbs = max_rate*1000/8/clock_khz;
    *granularity = 8000;

    *cbs = (*cbs < min_cbs) ? min_cbs : *cbs;

    return BCM_E_NONE;
}

/**
  * GTF - Set up sequence:
  *     1. Create GTF Object
  *        - Call bcm_sat_gtf_create()
  *     2. Set Bandwidth
  *        - Call bcm_sat_gtf_bandwidth_set()
  *     3. Configure packet info, including header type, header info, payload, packet edit info and packet offset.
  *        - Call bcm_sat_gtf_packet_config()
  *     4. [Optionally]Set Rate Pattern
  *        - Call bcm_sat_gtf_rate_pattern_set()
  *     5. Start sending traffic
  *        - Call bcm_sat_gtf_packet_start()
  */
int sat_gtf_setup(int unit, int inP, int outP, bcm_sat_gtf_t *gtf_id) {
    bcm_sat_gtf_packet_config_t packet_config;
    bcm_sat_gtf_bandwidth_t bw;
    bcm_pkt_t *sat_header;
    int core_clock_khz;
    int seq_num_offset = -1;
    int header_size = 0;
    int core_id;
    int ii, rv;
 
    rv = bcm_sat_gtf_create(unit, 0, gtf_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_sat_gtf_create: %d\n", rv);
        return rv;
    }

    core_clock_khz = *(dnxc_data_get(unit,  "device", "general", "core_clock_khz", NULL));

    bw.rate = sat_info.cir;

    rv = sat_gtf_calculate_cbs_granularity(bw.rate, &bw.max_burst, &bw.granularity, core_clock_khz);
    if (rv != BCM_E_NONE) {
        printf("Error in sat_gtf_calculate_cbs_granularity: %d\n", rv);
        return rv;
    }
 
    rv = bcm_sat_gtf_bandwidth_set(unit, *gtf_id, bcmSatGtfPriCir, &bw);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_sat_gtf_bandwidth_set: %d\n", rv);
        return rv;
    }

    bcm_sat_gtf_packet_config_t_init(&packet_config);
    packet_config.sat_header_type = bcmSatHeaderUserDefined;

    rv = bcm_pkt_alloc(unit, 128, 0, &sat_header);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_pkt_alloc: %d\n", rv);
        return rv;
    }

    sal_memset(sat_header->pkt_data->data, 0, 128);
    header_size = sat_header_build(unit, inP, outP, sat_header->pkt_data->data, &seq_num_offset);

    sat_header->pkt_data->len = header_size;

    packet_config.header_info.pkt_data = sat_header->pkt_data;
    packet_config.header_info.blk_count = 1;

    packet_config.payload.payload_type = bcmSatPayloadConstant8Bytes;
    for (ii = 0; ii < BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE; ii++) {
        packet_config.payload.payload_pattern[ii] = ii+1;
    }
    packet_config.packet_edit[0].packet_length[0] = header_size + 64;
    packet_config.packet_edit[0].pattern_length = 1;
    packet_config.packet_edit[0].packet_length_pattern[0] = 0;
    packet_config.packet_edit[0].number_of_ctfs = 1;

    rv = sat_port_core_get(unit, inP, &core_id);
    if (BCM_E_NONE != rv) {
        printf("Error in sat_port_core_get: %d\n", rv);
        return rv;
    }

    packet_config.packet_context_id = core_id;

    /**
     * Sequence number offset in the transmitted packet header.
     * seq_number_offset is counted from the start of PTCH-2.
     */
    packet_config.offsets.seq_number_offset = seq_num_offset;
 
    rv = bcm_sat_gtf_packet_config(unit, *gtf_id, &packet_config);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_sat_gtf_packet_config: %d\n", rv);
        return rv;
    }

    rv = bcm_pkt_free(unit, sat_header);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_pkt_free: %d\n", rv);
        return rv;
    }

    printf("Successfully created gtf %d\n", *gtf_id);

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
        printf("Error in bcm_sat_gtf_bandwidth_set: %d\n", rv);
        return rv;
    }

    rv = bcm_sat_gtf_destroy(unit, gtf_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_gtf_destroy: %d\n", rv);
        return rv;
    }

    return rv;
}

int sat_gtf_enable(int unit, bcm_sat_gtf_t gtf_id, int enable) {
    int rv;

    if (enable) {
        rv = bcm_sat_gtf_packet_start(unit, gtf_id, 0);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_sat_gtf_packet_start: %d\n", rv);
            return rv;
        }
    }
    else {
        rv = bcm_sat_gtf_packet_stop(unit, gtf_id, 0);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_sat_gtf_packet_stop: %d\n", rv);
            return rv;
        }
    }

    return rv;
}

int sat_gtf_show(int unit, bcm_sat_gtf_t gtf_id) {
    bcm_sat_gtf_stat_multi_t stat;
    int rv;

    rv = bcm_sat_gtf_stat_multi_get(unit, gtf_id, &stat);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_sat_gtf_stat_multi_get: %d\n", rv);
        return rv;
    }

    print stat.transmitted_packet_cnt;

    return rv;
}

/**
  * Create a PMF group and install an entry.
  */
int sat_pmf_config(
    int unit
)
{
    bcm_field_presel_entry_data_t presel_data;
    bcm_field_presel_entry_id_t presel_id;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_context_info_t context_info;
    bcm_field_group_t fg_id;
    bcm_field_entry_t ent;
    bcm_gport_t trap_gport;
    bcm_gport_t gport;
    int ii, rv;

    if (sat_field_info.presel_nof_quals > 0)
    {
        bcm_field_context_info_t_init(&context_info);
        rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &sat_field_info.context_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_context_create: %d\n", rv);
            return rv;
        }
    }

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    fg_info.nof_quals = sat_field_info.nof_quals;
    for (ii=0; ii<fg_info.nof_quals; ++ii)
    {
        fg_info.qual_types[ii] = sat_field_info.qual_type[ii];
    }

    fg_info.nof_actions = sat_field_info.nof_actions;
    for (ii=0; ii<fg_info.nof_actions; ++ii)
    {
        fg_info.action_types[ii] = sat_field_info.action[ii].type;
    }

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_add: %d\n", rv);
        return rv;
    }

    printf("Created TCAM field group (%d) in iPMF1 \n", fg_id);
    sat_field_info.group = fg_id;

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii=0; ii<fg_info.nof_quals; ++ii)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
        attach_info.key_info.qual_info[ii] = sat_field_info.qual_info[ii];
    }

    for (ii=0; ii<fg_info.nof_actions; ++ii)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    rv = bcm_field_group_context_attach(unit, 0, fg_id, sat_field_info.context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_attach: %d\n", rv);
        return rv;
    }

    bcm_field_entry_info_t_init(&ent_info);

    ent_info.nof_entry_quals = fg_info.nof_quals;
    for (ii=0; ii<fg_info.nof_quals; ++ii)
    {
        ent_info.entry_qual[ii].type = fg_info.qual_types[ii];
        ent_info.entry_qual[ii].value[0] = sat_field_info.qual_val[ii];
        ent_info.entry_qual[ii].mask[0] = sat_field_info.qual_mask[ii];
    }

    ent_info.nof_entry_actions = fg_info.nof_actions;
    for (ii=0; ii<fg_info.nof_actions; ++ii)
    {
        ent_info.entry_action[ii] = sat_field_info.action[ii];
    }

    rv = bcm_field_entry_add(unit, 0, sat_field_info.group, &ent_info, &ent);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_entry_add: %d\n", rv);
        return rv;
    }

    sat_field_info.ent = ent;

    if (sat_field_info.presel_nof_quals > 0)
    {
        bcm_field_presel_entry_id_info_init(&presel_id);
        presel_id.presel_id = sat_field_info.presel_id;
        presel_id.stage = bcmFieldStageIngressPMF1;

        bcm_field_presel_entry_data_info_init(&presel_data);
        presel_data.entry_valid = TRUE;
        presel_data.context_id = sat_field_info.context_id;
        presel_data.nof_qualifiers = sat_field_info.presel_nof_quals;

        for (ii=0; ii<presel_data.nof_qualifiers; ++ii)
        {
            presel_data.qual_data[ii].qual_type = sat_field_info.presel_qual_type[ii];
            presel_data.qual_data[ii].qual_value = sat_field_info.presel_qual_val[ii];
            presel_data.qual_data[ii].qual_mask = sat_field_info.presel_qual_mask[ii];
        }

        rv = bcm_field_presel_set(unit, 0, &presel_id, &presel_data);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_presel_set: %d\n", rv);
            return rv;
        }
    }

    return rv;
}

/**
  * Clean up procedure for sat_pmf_config
  */
int sat_pmf_destroy(int unit)
{
    bcm_field_entry_qual_t entry_qual_info[BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP];
    int rv = BCM_E_NONE;
    int ii= 0;

    rv = bcm_field_group_context_detach(unit, sat_field_info.group, sat_field_info.context_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach: %d\n", rv);
        return rv;
    }

    rv = bcm_field_entry_delete(unit, sat_field_info.group, &entry_qual_info, sat_field_info.ent);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_entry_delete: %d\n", rv);
        return rv;
    }

    rv = bcm_field_group_delete(unit, sat_field_info.group);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_delete: %d\n", rv);
        return rv;
    }

    if (sat_field_info.presel_nof_quals > 0)
    {
        bcm_field_presel_entry_data_t presel_data;
        bcm_field_presel_entry_id_t presel_id;

        bcm_field_presel_entry_id_info_init(&presel_id);
        presel_id.presel_id = sat_field_info.presel_id;
        presel_id.stage = bcmFieldStageIngressPMF1;

        bcm_field_presel_entry_data_info_init(&presel_data);
        presel_data.entry_valid = FALSE;
        presel_data.context_id = 0;
        
        rv = bcm_field_presel_set(unit, 0, &presel_id, &presel_data);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_presel_set: %d\n", rv);
           return rv;
        }
        
        rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, sat_field_info.context_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_context_destroy eth\n", rv);
            return rv;
        }
    }

    return rv;
}

/**
  * Identify CTF packet.
  * Setup ingress PMF group:
  *     IF InPort == rx_port THEN
  *     ACTION TRAP = {trap_id, trap_qualifier}
  */
int sat_ctf_trap_setup(
    int unit,
    int rx_port,
    int trap_id,
    int trap_qualifier,
    int is_oam,
    int oam_sub_type
)
{
    bcm_gport_t gport;

    sat_field_info.nof_quals = 1;
    sat_field_info.qual_type[0] = bcmFieldQualifyInPort;
    sat_field_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    sat_field_info.qual_info[0].input_arg = 1;
    sat_field_info.qual_info[0].offset = 0;
    BCM_GPORT_LOCAL_SET(gport, rx_port);
    sat_field_info.qual_val[0] = gport;
    sat_field_info.qual_mask[0] = 0xffffffff;

    if (is_oam)
    {
        sat_field_info.nof_actions = 2;
        sat_field_info.action[0].type = bcmFieldActionTrap;
        BCM_GPORT_TRAP_SET(gport, trap_id, 7, 0);
        sat_field_info.action[0].value[0] = gport;
        sat_field_info.action[0].value[1] = trap_qualifier;
        sat_field_info.action[1].type = bcmFieldActionOam;
        sat_field_info.action[1].value[2] = oam_sub_type;
        sat_field_info.action[1].value[3] = 0;
    }
    else
    {
        sat_field_info.nof_actions = 1;
        sat_field_info.action[0].type = bcmFieldActionTrap;
        BCM_GPORT_TRAP_SET(gport, trap_id, 7, 0);
        sat_field_info.action[0].value[0] = gport;
        sat_field_info.action[0].value[1] = trap_qualifier;
    }

    return sat_pmf_config(unit);
}

/**
  * Clean up procedure for sat_ctf_trap_setup
  */
int sat_ctf_trap_cleanup(
    int unit
)
{
    return sat_pmf_destroy(unit);
}

/**
  * CTF configurations inside SAT core.
  *
  * tc - actual TC of the packet, carried in FTMH
  * dp - actual dp of the packet, carried in FTMH
  *
  * trap-index
  *     compare packet trap_id to 3 global SAT RX trap code, returns the index
  *
  * trap_qualifier
  *     trap qualifier updated by ingress PMF
  *
  * session_id - 0~63
  *
  * CTF identification logic:
  *     trap_qualifier -> session_id
  *     {session_id, ftmh.tc, ftmh.dp, trap_index} -> CTF ID
  */
int sat_ctf_general_config(int unit, bcm_sat_ctf_t *ctf_id, int tc, int dp, int trap_index, int trap_qualifier, int session_id) {
    bcm_sat_ctf_availability_config_t availability_config;
    bcm_sat_ctf_packet_info_t packet_info;
    bcm_sat_ctf_stat_config_t stat_config;
    bcm_sat_ctf_identifier_t identifier;
    bcm_sat_ctf_bin_limit_t bins[9];
    int ii, rv;

    rv = bcm_sat_ctf_create(unit, 0, ctf_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_create: %d\n", rv);
        return rv;
    }

    bcm_sat_ctf_packet_info_t_init(&packet_info);
    packet_info.sat_header_type = bcmSatHeaderUserDefined;
    packet_info.offsets.seq_number_offset = sat_info.seq_num_offset;
    packet_info.offsets.timestamp_offset = sat_info.timestamp_offset;
    packet_info.offsets.payload_offset = sat_info.payload_offset;
    packet_info.payload.payload_type = bcmSatPayloadConstant8Bytes;
    for (ii = 0; ii < BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE; ++ii) {
        packet_info.payload.payload_pattern[ii] = ii+1;
    }

    rv = bcm_sat_ctf_packet_config (unit, *ctf_id, &packet_info);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_packet_config: %d\n", rv);
        return rv;
    }

    /** Map trap qualifier to session id  */
    rv = bcm_sat_ctf_trap_data_to_session_map(unit, trap_qualifier, 0xff, session_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_trap_data_to_session_map: %d\n", rv);
        return rv;
    }

    /** CTF identification map */
    bcm_sat_ctf_identifier_t_init(&identifier);
    identifier.session_id = session_id;
    identifier.trap_id = trap_index;
    identifier.tc = tc;
    identifier.color = dp;
    rv = bcm_sat_ctf_identifier_map(unit, &identifier, *ctf_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_identifier_map: %d\n", rv);
        return rv;
    }

    /**
     * Statistic collecting configuration
     * The following code creates 9 linear step bins:
     *     bin_limit_delay_x(x=0~8) = bin_min_delay + (x+1)*bin_step
     */
    bcm_sat_ctf_stat_config_t_init(stat_config);
    stat_config.use_global_bin_config = 0;
    stat_config.bin_min_delay = 0;
    stat_config.bin_step = 0x100;
    stat_config.update_counters_in_unvavail_state = 1;
    rv = bcm_sat_ctf_stat_config_set(unit, *ctf_id, &stat_config);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_stat_config_set: %d\n", rv);
        return rv;
    }

    /**
     * Global bin configuration
     * The following code demonstrates how to create global bins.
     * Global bins are selected when use_global_bin_config==1.
     *
     * Global bins are not necessarily linear.
     * The following code creates 9 globa bins with:
     * 0x100, 0x200, 0x400, 0x700, 0xB00, 0x1000, 0x1600, 0x1D00, 0x2500
     */
    for (ii = 0; ii < 9; ++ii)
    {
        bins[ii].bin_select = ii;

        if (ii==0)
        {
            bins[ii].bin_limit = 0x100;
        }
        else
        {
            bins[ii].bin_limit = bins[ii-1].bin_limit + ii*0x100;
        }
    }
    rv = bcm_sat_ctf_bin_limit_set(unit, 9, &bins);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_bin_limit_set: %d\n", rv);
        return rv;
    }

    bcm_sat_ctf_availability_config_t_init(&availability_config);
    availability_config.switch_state_num_of_slots = 10;
    availability_config.switch_state_threshold_per_slot = 100;
    rv = bcm_sat_ctf_availability_config_set(unit, *ctf_id, &availability_config);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_availability_config_set: %d\n", rv);
        return rv;
    }

    return rv;
}

/**
  * Show CTF statistics
  */
int sat_ctf_show(int unit, bcm_sat_ctf_t ctf_id, int clear_on_read) {
    bcm_sat_ctf_stat_t stat;
    uint32 flags = 0;
    int rv;

    if (clear_on_read == 0) {
        flags = BCM_SAT_CTF_STAT_DO_NOT_CLR_ON_READ;
    }

    rv = bcm_sat_ctf_stat_get(unit, ctf_id, flags, &stat);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_stat_get: %d\n", rv);
        return rv;
    }

    printf("CTF statistics:\n");
    print stat;

    return rv;
}

/**
  * CTF set up sequence
  *
  * CTF configuration in pipeline:
  *     Create user-defined TRAP, trap packet to SAT port
  *     Create PMF group, update packet action for CTF packets with trap_id/trap_qualifier
  */
int sat_ctf_setup(int unit, int rx_port, int sat_port, bcm_sat_ctf_t *ctf_id, int *trap_id) {
    bcm_rx_trap_config_t trap_config;
    bcm_gport_t trap_gport;
    int rv;

    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, trap_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_rx_trap_type_create: %d\n", rv);
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
    trap_config.dest_port = sat_port;
    rv = bcm_rx_trap_set(unit, *trap_id, &trap_config);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_rx_trap_set: %d\n", rv);
        return rv;
    }

    /**
     * sat_info.timestamp_format map to oam_sub_type:
     *     bcmSATTimestampFormatIEEE1588v1(0): 2
     *     bcmSATTimestampFormatNTP(1): 3
     */
    rv = sat_ctf_trap_setup(unit, rx_port, *trap_id, sat_info.trap_qualifier, 1, sat_info.timestamp_format+2);
    if (BCM_E_NONE != rv) {
        printf("Error in sat_ctf_trap_setup: %d\n", rv);
        return rv;
    }

    rv = bcm_sat_ctf_trap_add(unit, *trap_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_trap_add: %d\n", rv);
        return rv;
    }

    /**
     * SAT compares packet trap_id to 3 global RX trap codes,
     * the result is the trap_index used for CTF identification.
     *
     * trap_index shows the sequence of the trap_id being added by
     * bcm_sat_ctf_trap_add.
     */
    sat_info.trap_index = 0;

    rv = sat_ctf_general_config(unit, ctf_id, sat_info.tc, sat_info.dp, sat_info.trap_index, sat_info.trap_qualifier, sat_info.session_id);
    if (BCM_E_NONE != rv) {
        printf("Error in sat_ctf_general_config: %d\n", rv);
        return rv;
    }

    return rv;
}

/**
  * Clean up procedure for sat_ctf_setup
  */
int sat_ctf_cleanup(int unit) {
    bcm_sat_ctf_identifier_t identifier;
    int rv = BCM_E_NONE;

    rv = sat_ctf_trap_cleanup(unit);
    if (BCM_E_NONE != rv) {
        printf("Error in sat_ctf_trap_cleanup: %d\n", rv);
        return rv;
    }

    identifier.tc = sat_info.tc;
    identifier.color = sat_info.dp;
    identifier.trap_id = sat_info.trap_index;
    identifier.session_id = sat_info.session_id;

    /* Set the CTF flow as invalid  */
    rv = bcm_sat_ctf_identifier_unmap(unit, &identifier);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_identifier_unmap: %d\n", rv);
        return rv;
    }

    /* Remove all trap id */
    rv = bcm_sat_ctf_trap_remove(unit, sat_info.trap_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_trap_remove_all: %d\n", rv);
        return rv;
    }

    /* Destroy the CTF flow */
    rv = bcm_sat_ctf_destroy(unit, sat_info.ctf_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_ctf_destroy: %d\n", rv);
        return rv;
    }

    return rv;
}

/**
  * Get the core ID for port
  */
int sat_port_core_get(int unit, int port, int *core) {
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 flags;
    int rv;

    rv = bcm_port_get(unit, port, &flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_get failed: %d\n", rv);
        return rv;
    }

    *core = mapping_info.core;

    return rv;
}

/**
  * SAT setup
  *
  * inP - the port SAT will inject packet to.
  *       This is the In-PP-Port in PTCH
  *
  * outP - the port the SAT packets will be forwarded to.
  *       This is the destination port in ITMH, or the PP forwarding
  *       destination port of SAT packets
  */
int sat_setup(int unit, int inP, int outP) {
    bcm_sat_config_t config;
    int core_id;
    int rv;

    /* global configuration */
    bcm_sat_config_t_init(&config);
    config.timestamp_format = sat_info.timestamp_format;
    rv = bcm_sat_config_set(unit, &config);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_sat_config_set: %d\n", rv);
        return rv;
    }

    rv = sat_gtf_setup(unit, inP, outP, &sat_info.gtf_id);
    if (BCM_E_NONE != rv) {
        printf("Error in sat_gtf_setup: %d\n", rv);
        return rv;
    }

    rv = sat_gtf_show(unit, sat_info.gtf_id);
    if (BCM_E_NONE != rv) {
        printf("Error in sat_gtf_show: %d\n", rv);
        return rv;
    }

    rv = sat_port_core_get(unit, inP, &core_id);
    if (BCM_E_NONE != rv) {
        printf("Error in sat_port_core_get: %d\n", rv);
        return rv;
    }

    /* setup a CTF */
    rv = sat_ctf_setup(unit, outP, sat_info.sat_port[core_id], &sat_info.ctf_id, &sat_info.trap_id);
    if (BCM_E_NONE != rv) {
        printf("Error in sat_ctf_setup: %d\n", rv);
        return rv;
    }

    return rv;
}

/**
  * Clean up procedure for sat_setup
  */
int sat_cleanup(int unit) {
    int rv = BCM_E_NONE;

    rv = sat_ctf_cleanup(unit);
    if (BCM_E_NONE != rv) {
        printf("Error in sat_ctf_cleanup: %d\n", rv);
        return rv;
    }

    rv = sat_gtf_cleanup(unit, sat_info.gtf_id);
    if (BCM_E_NONE != rv) {
        printf("Error in sat_gtf_cleanup: %d\n", rv);
        return rv;
    }

    return rv;
}

/*
 * Create a MC group and its replications.
 */
int force_bubble_mc_config(int unit, bcm_port_t *ports, int nof_ports)
{
    int rc, ind;
    int nof_replications = nof_ports;
    int intf_in = 100;
    int vrf = 1;
    int flags = BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID;

    bcm_mac_t in_my_mac = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    int mc_group = 0x500;
    l3_ingress_intf_init(&ingress_rif);
    bcm_l3_intf_t l3if;
    bcm_multicast_replication_t replications[nof_ports];
    char *proc_name;

    for (ind = 0; ind < nof_ports; ind++)
    {
       /** Set In-Port to In ETh-RIF */
       rc = in_port_intf_set(unit, ports[ind], intf_in + ports[ind]);
       if (rc != BCM_E_NONE)
       {
           printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
           return rc;
       }

       /** Initialize a bcm_l3_intf_t structure.*/
       bcm_l3_intf_t_init(&l3if);
       l3if.l3a_flags = BCM_L3_WITH_ID;
       sal_memcpy(l3if.l3a_mac_addr, in_my_mac, 6);
       l3if.l3a_intf_id = l3if.l3a_vid = intf_in + ports[ind];
       rc = bcm_l3_intf_create(unit, l3if);
       if (rc != BCM_E_NONE)
       {
           printf("Error, bcm_l3_intf_create %d\n", rc);
           return rc;
       }
       /** Initialize a l3_ingress_intf structure.*/
       ingress_rif.vrf = vrf + ports[ind];
       ingress_rif.intf_id = intf_in + ports[ind];
       rc = intf_ingress_rif_set(unit, &ingress_rif);
       if (rc != BCM_E_NONE)
       {
           printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
           return rc;
       }
       /**Create MC replications*/
       set_multicast_replication(&replications[ind], ports[ind], 0);
    }

    rc = bcm_multicast_create(unit, flags, &mc_group);
    if (rc != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_create \n");
        return rc;
    }
    rc = bcm_multicast_add(unit, mc_group, BCM_MULTICAST_EGRESS_GROUP, nof_replications, replications);
    if (rc != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_egress_add\n");
        return rc;
    }

    if (is_device_or_above(unit, JERICHO2) && (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL)))
    {
        bcm_module_t mreps[2] = {0, 1};
        int nof_mreps = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));

        rc = bcm_fabric_multicast_set(unit, mc_group, 0, nof_mreps, mreps);
        if (rc != BCM_E_NONE) {
           printf("Error, bcm_fabric_multicast_set \n");
           return rc;
        }
    }
    return rc;
}

