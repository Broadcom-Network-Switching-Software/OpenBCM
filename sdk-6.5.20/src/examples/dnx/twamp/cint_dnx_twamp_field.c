/* 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/* 
 * The below CINT demonstrates TWAMP TX/RX and Reflector PMF rule.
 *
 * TWAMP Reflector Scenario:
 * 1. TWAMP Reflector 1st pass:
 *     1.1 IPMF1 TCAM lookup covered - {SIP, DIP, UDP.DST_PORT}.
 *           Action:
 *               bcmFieldActionTrap -
 *               bcmFieldActionTtlSet - Set TTL as 255
 *               bcmFieldActionForwardingAdditionalInfo -don't decrement TTL 
 *               user_act_id - present the 1st pass
 *     1.2 IPMF2 DE lookup covered - {user_qual_id(1), user_qual_id2(1), LayerRecordOffset(8)}.
 *           Action: 
 *               bcmFieldActionOam -
 *                   OAM-Stamp-Offset = 32B, respective to UDP offset (8B + 24B)
 *                   OAM-Offset = 0
 *                   OAM-Sub-Type = 6 (TWAMP-REFLECTOR-1ST)
 *                   OAM-Up-MEP = 0 (DOWN) 
 *
 * 2. TWAMP Reflector 2nd pass:
 *    2.1 PMF lookup covered - {SIP, DIP, UDP.SRC_PORT}.
 *         Action:
 *            bcmFieldActionStatId7 - Statistics ID, used for SEQ stamping
 *            bcmFieldActionStatProfile7 - Statistics profile, used for SEQ stamping
 *            bcmFieldActionForwardingAdditionalInfo -don't decrement TTL   
 *            user_act_id - present the 2nd pass
 *     2.2 IPMF2 DE lookup covered - {user_qual_id(1), user_qual_id2(1), LayerRecordOffset(8)}.
 *           Action:  
 *             bcmFieldActionOam -
 *                 OAM-Stamp-Offset = 12B, respective to UDP offset (8B + 4B)
 *                 OAM-Offset = 0
 *                 OAM-Sub-Type = TWAMP-REFLECTOR-2ND
 *                 OAM-Up-MEP = 0 (DOWN)
 *
 * TWAMP RX Scenario:
 *     1.1 IPMF1 TCAM lookup covered - {SIP, DIP, UDP.DST_PORT}.
 *           Action:
 *               bcmFieldActionTrap -to CPU
 *     1.2 IPMF2 DE lookup covered - {user_qual_id(1),  LayerRecordOffset(8)}.
 *           Action: 
 *               bcmFieldActionOam -
 *                   OAM-Stamp-Offset = 32B, respective to UDP offset (8B + 24B)
 *                   OAM-Offset = 0
 *                   OAM-Sub-Type = 3 (DM-NTP)
 *                   OAM-Up-MEP = 0 (DOWN) 
 *
 * TWAMP TX Scenario:
 *    2.1 PMF lookup covered - {SIP, DIP, UDP.SRC_PORT}.
 *         Action:
 *            bcmFieldActionForwardingAdditionalInfo -don't decrement TTL   
 *     2.2 IPMF2 DE lookup covered - {user_qual_id(1), LayerRecordOffset(8)}.
 *           Action:  
 *             bcmFieldActionOam -
 *                 OAM-Stamp-Offset = 12B, respective to UDP offset (8B + 4B)
 *                 OAM-Offset = 0
 *                 OAM-Sub-Type = 3 (DM-NTP)
 *                 OAM-Up-MEP = 1 (UP)
 */

bcm_field_context_t twamp_context_id;
bcm_field_action_t twamp_reflector_user_act_id;
bcm_field_group_t twamp_reflector_1st_pass_ipmf1_fg_id;
bcm_field_group_t twamp_reflector_1st_pass_ipmf2_fg_id;
bcm_field_group_t twamp_reflector_2nd_pass_ipmf1_fg_id;
bcm_field_group_t twamp_reflector_2nd_pass_ipmf2_fg_id;

bcm_field_group_t twamp_tx_ipmf1_fg_id;
bcm_field_group_t twamp_tx_ipmf2_fg_id;
bcm_field_group_t twamp_rx_ipmf1_fg_id;
bcm_field_group_t twamp_rx_ipmf2_fg_id;

/**
 * \brief
 *
 *  This function contains the bcm_field_presel_set application.
 *  This function sets all required HW configuration for bcm_field_presel_set
 *  to be performed for IPMF1.
 *
 * \param [in] unit - The unit number.
 * \param [in] is_ipv6 - The flag indicates it is the ipv6.
 * \param [in] presel_id - The presel id.
 * \param [in] context_id - The context id.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int cint_field_twamp_presel_set(int unit, int is_ipv6, bcm_field_presel_t presel_id, bcm_field_context_t context_id) {
    int rv = BCM_E_NONE;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_context_param_info_t context_param;

    bcm_field_context_param_info_t_init(context_param);
    context_param.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
    context_param.param_val = bcmFieldSystemHeaderProfileFtmhTshPphUdh;
    rv = bcm_field_context_param_set(unit,0,bcmFieldStageIngressPMF1,context_id,&context_param);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_param_set system header info\n", rv);
        return rv;
    }
    bcm_field_context_param_info_t_init(&context_param);
    context_param.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    context_param.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerOffset1, 0);
    rv = bcm_field_context_param_set(unit,0,bcmFieldStageIngressPMF2,context_id,&context_param);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_param_set system header info\n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = presel_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id;
    p_data.nof_qualifiers = 2;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 1;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeUdp;
    p_data.qual_data[0].qual_mask = 0x1F;
    
    p_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[1].qual_arg = 0;
    p_data.qual_data[1].qual_value = is_ipv6 ? bcmFieldLayerTypeIp6 : bcmFieldLayerTypeIp4;
    p_data.qual_data[1].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_presel_set \n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *
 *  This function creates the user action to present the TWAMP 1st pass and 2nd pass.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int cint_field_twamp_reflector_user_act_create(int unit) {    
    int rv = BCM_E_NONE;
    void *dest_char;
    bcm_field_action_info_t user_action_info;
    
    /* Create a user action to be used as cascaded action 
    * action value meaning: 0 (TWAMP 1st pass), 1 (TWAMP 2nd pass) 
    */
    bcm_field_action_info_t_init(&user_action_info);
    user_action_info.action_type = bcmFieldActionVoid;
    user_action_info.prefix_size = 0;
    user_action_info.prefix_value = 0;
    user_action_info.size = 1;
    user_action_info.stage = bcmFieldStageIngressPMF1;
    dest_char = &(user_action_info.name[0]);
    sal_strncpy_s(dest_char, "TWAMP_REF_PMF1_ACT", sizeof(user_action_info.name));
    rv = bcm_field_action_create(unit, 0, &user_action_info, &twamp_reflector_user_act_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }  

    return rv;
}

void  cint_field_twamp_reflector_reverse_memcpy(void *dst, void* src, int size) {
    int i;
    uint32 *destination = dst;
    uint8 *source = src;
    sal_memset(destination, 0 , size);
    for (i = 0; i < size; ++i)
        destination[size/4-1-(i/4)] |= (source[i] << (8 * (3 - (i & 3))));
}

/**
 * \brief
 *
 *  This function sets all required HW configuration for TWAMP reflector over IPv4/IPv6 for the 1st pass IPMF1.
 *
 * \param [in] unit - The unit number.
 * \param [in] is_ipv6 - The flag indicates it is the ipv6.
 * \param [in] gport_trap1 - The gport trap id.
 * \param [in] local_ip_addr - Local ip address.
 * \param [in] remote_ip_addr - Remote ip address.
 * \param [in] local_ip6_addr - Local ip6 address.
 * \param [in] remote_ip6_addr - Remote ip6 address.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int cint_field_twamp_reflector_1st_pass_set_ipmf1(int unit, int is_ipv6, bcm_gport_t gport_trap1, 
    uint32 local_ip_addr, uint32 remote_ip_addr, 
    bcm_ip6_t *local_ip6_addr, bcm_ip6_t *remote_ip6_addr) {
    int i, rv = BCM_E_NONE;
    void *dest_char;
    bcm_field_entry_t ent_id;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;

    /** Create field group */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Qualifiers: 
     *    (SIP == Session-SIP)               obtained by TWAMP-Control session initialization     
     *    (DIP == Session-DIP)               obtained by TWAMP-Control session initialization
     *    (UDP.Dst-Port == Session-Src-Port) obtained by TWAMP-Control session initialization
     *    (UDP.Src-Port == Session-Dst-Port) obtained by TWAMP-Control session initialization
     */
    fg_info.nof_quals = 4;
    if (is_ipv6) {
        fg_info.qual_types[0] = bcmFieldQualifySrcIp6;
        fg_info.qual_types[1] = bcmFieldQualifyDstIp6;
    } else {
        fg_info.qual_types[0] = bcmFieldQualifySrcIp;
        fg_info.qual_types[1] = bcmFieldQualifyDstIp;
    }
    fg_info.qual_types[2] = bcmFieldQualifyL4SrcPort;
    fg_info.qual_types[3] = bcmFieldQualifyL4DstPort;

    /* Actions:
     * CPU-Trap-Code indicating TWAMP-REFLECTOR-TRAP
     * In-TTL = 255
     * FAI[2] = 1, do not decrement TTL
     */
    fg_info.nof_actions = 4;
    fg_info.action_types[0] = bcmFieldActionTrap;
    fg_info.action_types[1] = bcmFieldActionTtlSet;
    fg_info.action_types[2] = bcmFieldActionForwardingAdditionalInfo;   
    fg_info.action_types[3] = twamp_reflector_user_act_id;  
    fg_info.action_with_valid_bit[3] = FALSE;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "TWAMP_REF1_PMF1_FG", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &twamp_reflector_1st_pass_ipmf1_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("twamp_reflector_1st_pass_ipmf1_fg_id:%d\n", twamp_reflector_1st_pass_ipmf1_fg_id);

    /* Attaching the FG */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    for (i = 0; i < fg_info.nof_quals; i++)
    {
        attach_info.key_info.qual_types[i] = fg_info.qual_types[i];
    }

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for (i = 0; i < fg_info.nof_actions; i++)
    {
        attach_info.payload_info.action_types[i] = fg_info.action_types[i];
        if (fg_info.action_types[i] == twamp_reflector_user_act_id) {
            attach_info.payload_info.action_info[i].priority = BCM_FIELD_ACTION_DONT_CARE;
        } else {
            attach_info.payload_info.action_info[i].priority = BCM_FIELD_ACTION_PRIORITY(0, 2);
        }
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[1].input_arg = 0;
    attach_info.key_info.qual_info[1].offset = 0;

    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[2].input_arg = 1;
    attach_info.key_info.qual_info[2].offset = 0;

    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[3].input_arg = 1;
    attach_info.key_info.qual_info[3].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, twamp_reflector_1st_pass_ipmf1_fg_id, twamp_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    /* Add field entry */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = fg_info.nof_quals;
    if (is_ipv6) {   
        ent_info.entry_qual[0].type = fg_info.qual_types[0];
        cint_field_twamp_reflector_reverse_memcpy(ent_info.entry_qual[0].value, remote_ip6_addr, 16);
        sal_memset(ent_info.entry_qual[0].mask, 0xff, 16);

        ent_info.entry_qual[1].type = fg_info.qual_types[1];
        cint_field_twamp_reflector_reverse_memcpy(ent_info.entry_qual[1].value, local_ip6_addr, 16);
        sal_memset(ent_info.entry_qual[1].mask, 0xff, 16);
    } else {
        ent_info.entry_qual[0].type = fg_info.qual_types[0];
        ent_info.entry_qual[0].value[0] = remote_ip_addr;
        ent_info.entry_qual[0].mask[0] = 0xffffffff;

        ent_info.entry_qual[1].type = fg_info.qual_types[1];
        ent_info.entry_qual[1].value[0] = local_ip_addr;
        ent_info.entry_qual[1].mask[0] = 0xffffffff;
    }

    ent_info.entry_qual[2].type = fg_info.qual_types[2];
    ent_info.entry_qual[2].value[0] = 50001;
    ent_info.entry_qual[2].mask[0] = 0xffff;

    ent_info.entry_qual[3].type = fg_info.qual_types[3];
    ent_info.entry_qual[3].value[0] = 862;
    ent_info.entry_qual[3].mask[0] = 0xffff;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = gport_trap1;
    ent_info.entry_action[0].value[1] = 0x1234;

    ent_info.entry_action[1].type = fg_info.action_types[1];
    ent_info.entry_action[1].value[0] = 255;
	
    ent_info.entry_action[2].type = fg_info.action_types[2];
    ent_info.entry_action[2].value[0] = 4; /* Do not decrement the TTL indication */

    ent_info.entry_action[3].type = fg_info.action_types[3];
    ent_info.entry_action[3].value[0] = 0; /* Present the 1st pass */

    rv = bcm_field_entry_add(unit, 0, twamp_reflector_1st_pass_ipmf1_fg_id, &ent_info, &ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *
 *  This function sets all required HW configuration for TWAMP reflector over IPv4/IPv6 for the 1st pass IPMF2.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int cint_field_twamp_reflector_1st_pass_set_ipmf2(int unit)
{   int i, rv = BCM_E_NONE;   
    void *dest_char;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualify_t user_qual_id, user_qual_id2;
    bcm_field_action_t user_act_id;
    bcm_field_action_info_t user_action_info;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;    
    bcm_field_fem_action_info_t  fem_action_info;
    int fem_action_oam_value = 0x60000; /* MEP-type: Down, MEP-sub-type: TWAMP-Reflector-1st */
    int action_offset;

    /* Create an user-qualifier in ipmf2 that will parse the action buffer received from ipmf1 (cascaded)  */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1; 
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "TWAMP_REF1_PMF2_Q1", sizeof(qual_info.name));

    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &user_qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1; 
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "TWAMP_REF1_PMF2_Q2", sizeof(qual_info.name));

    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &user_qual_id2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    /* Create a user action to be used as FEM action */
    bcm_field_action_info_t_init(&user_action_info);
    user_action_info.action_type = bcmFieldActionVoid;
    user_action_info.prefix_size = 0;
    user_action_info.prefix_value = 0;
    user_action_info.size = 10;
    user_action_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(user_action_info.name[0]);
    sal_strncpy_s(dest_char, "TWAMP_REF1_PMF2_A", sizeof(user_action_info.name));
    
    rv = bcm_field_action_create(unit, 0, &user_action_info, &user_act_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }    

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;

    /* Qualifiers: 
     * User define qualifier    read the action buffer received from ipmf1   
     * LayerRecordOffset      read the offset of FWD layer 
     */
    fg_info.nof_quals = 3;    
    fg_info.qual_types[0] = user_qual_id;
    fg_info.qual_types[1] = user_qual_id2;
    fg_info.qual_types[2] = bcmFieldQualifyLayerRecordOffset;

    /* Actions: use Direct Extration to read the offset of FED layer and use FEM to increment the TWAMP offset
     * bcmFieldActionOam construction:
     *     oam_stamp_offset[0:7]: fwd_header.offset + 0x34(TWAMP offset)
     *     oam_offset[8:15]: 0
     *     oam_sub_type[16:19]: 6(TWAMP-Reflector-1st)
     *     oam_up_mep[20]: 0 (DOWN)
     */
    fg_info.nof_actions = 1;
  
    fg_info.action_types[0] = user_act_id;
    fg_info.action_with_valid_bit[0] = FALSE;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "TWAMP_REF1_PMF2_FG", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, 0, &fg_info, &twamp_reflector_1st_pass_ipmf2_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_group_add ipmf2_fg\n", rv);
        return rv;
    }
    printf("twamp_reflector_1st_pass_ipmf2_fg_id:%d\n", twamp_reflector_1st_pass_ipmf2_fg_id);
    
    /**FEM CONFIG*/
    bcm_field_fem_action_info_t_init(&fem_action_info);
    fem_action_info.fem_input.input_offset = 0;  
    fem_action_info.condition_msb = 3;
    for(i = 0 ; i < BCM_FIELD_NUMBER_OF_CONDITIONS_PER_FEM; i++)
    {
        fem_action_info.fem_condition[i].is_action_valid = ((i & 0x3) == 0x1) ? TRUE : FALSE; /* 1, 5, 9, 13 */
        fem_action_info.fem_condition[i].extraction_id = 0;
    }
    fem_action_info.fem_extraction[0].action_type = bcmFieldActionOam;
    /* oam action is 21 bits width, use the FEM to set 8 LSB as mapping for oam_stamp_offset
    * 13 MSB as force for oam_offset, oam_sub_type and oam_up_mep */
    for(i = 0 ; i < 21; i++)
    {
        if (i < 8) {
            fem_action_info.fem_extraction[0].output_bit[i].source_type = bcmFieldFemExtractionOutputSourceTypeKey;
            fem_action_info.fem_extraction[0].output_bit[i].offset = i + 2;
            fem_action_info.fem_extraction[0].output_bit[i].forced_value = 0;
        } else {
            fem_action_info.fem_extraction[0].output_bit[i].source_type = bcmFieldFemExtractionOutputSourceTypeForce;
            fem_action_info.fem_extraction[0].output_bit[i].offset = 0;
            fem_action_info.fem_extraction[0].output_bit[i].forced_value = (fem_action_oam_value & (0x1 << i)) ? 1 : 0;
        }
    }
    fem_action_info.fem_extraction[0].increment = 0x20;
    
    rv = bcm_field_fem_action_add(unit,0 ,twamp_reflector_1st_pass_ipmf2_fg_id, BCM_FIELD_ACTION_POSITION(3, 2), &fem_action_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_fem_action_add ipmf2\n", rv);
        return rv;
    }
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for(i = 0; i < fg_info.nof_quals; i++)
    {
        attach_info.key_info.qual_types[i] = fg_info.qual_types[i];
    }
    for(i = 0; i < fg_info.nof_actions; i++)
    {
        attach_info.payload_info.action_types[i] = fg_info.action_types[i];
    }
    
    /* fg_id_ipmf1 is the Field group in ipmf1 to use the result of */
    bcm_field_group_info_get( unit, twamp_reflector_1st_pass_ipmf1_fg_id, &fg_info);
    
    /* Get the FAI action valid bit   */
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeCascaded;
    attach_info.key_info.qual_info[0].input_arg = twamp_reflector_1st_pass_ipmf1_fg_id;    

    /* Get the offset for the action */
    rv = bcm_field_group_action_offset_get(unit, 0, twamp_reflector_1st_pass_ipmf1_fg_id, fg_info.action_types[2], &action_offset);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_action_offset_get\n", rv);
        return rv;
    }
    attach_info.key_info.qual_info[0].offset = action_offset - 1;  

    /* Get the TWAMP user action bit   */
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeCascaded;
    attach_info.key_info.qual_info[1].input_arg = twamp_reflector_1st_pass_ipmf1_fg_id;
    
    /* Get the offset for the action */
    rv = bcm_field_group_action_offset_get(unit, 0, twamp_reflector_1st_pass_ipmf1_fg_id, fg_info.action_types[3], &action_offset);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_action_offset_get\n", rv);
        return rv;
    }
    attach_info.key_info.qual_info[1].offset = action_offset;    

    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[2].input_arg = 1;
    
    rv = bcm_field_group_context_attach(unit, 0, twamp_reflector_1st_pass_ipmf2_fg_id, twamp_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_group_context_attach ipmf2\n", rv);
        return rv;
    }
    
    return 0;
}

/**
 * \brief
 *
 *  This function sets all required HW configuration for TWAMP reflector over IPv4/IPv6 for the 2nd pass IPMF1.
 *
 * \param [in] unit - The unit number.
 * \param [in] is_ipv6 - The flag indicates it is the ipv6.
 * \param [in] local_ip_addr - Local ip address.
 * \param [in] remote_ip_addr - Remote ip address.
 * \param [in] local_ip6_addr - Local ip6 address.
 * \param [in] remote_ip6_addr - Remote ip6 address.

 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int cint_field_twamp_reflector_2nd_pass_set_ipmf1(int unit, int is_ipv6,  
    uint32 local_ip_addr, uint32 remote_ip_addr, 
    bcm_ip6_t *local_ip6_addr, bcm_ip6_t *remote_ip6_addr) {
    int i, rv = BCM_E_NONE;
	void *dest_char;
    bcm_field_entry_t ent_id;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;

    /** Create field group */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Qualifiers:     
     *    (SIP == Session-SIP)               obtained by TWAMP-Control session initialization
     *    (DIP == Session-DIP)               obtained by TWAMP-Control session initialization
     *    (UDP.Dst-Port == Session-Src-Port) obtained by TWAMP-Control session initialization
     *    (UDP.Src-Port == Session-Dst-Port) obtained by TWAMP-Control session initialization
     */
    fg_info.nof_quals = 4;
    if (is_ipv6) {        
        fg_info.qual_types[0] = bcmFieldQualifySrcIp6;
        fg_info.qual_types[1] = bcmFieldQualifyDstIp6;
    } else {
        fg_info.qual_types[0] = bcmFieldQualifySrcIp;
        fg_info.qual_types[1] = bcmFieldQualifyDstIp;
    }
    fg_info.qual_types[2] = bcmFieldQualifyL4SrcPort;
    fg_info.qual_types[3] = bcmFieldQualifyL4DstPort;

    /* Actions:
     *    OAM-Counter-Idx = pointing to counter associated with session sequence number
     *    Forwarding-Layer-Additional-Info = Do not Decrement TTL
     */
    fg_info.nof_actions = 4;
    fg_info.action_types[0] = bcmFieldActionStatId7;
    fg_info.action_types[1] = bcmFieldActionStatProfile7;
    fg_info.action_types[2] = bcmFieldActionForwardingAdditionalInfo;
    fg_info.action_types[3] = twamp_reflector_user_act_id;
    fg_info.action_with_valid_bit[3] = FALSE;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "TWAMP_REF2_PMF1_FG", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &twamp_reflector_2nd_pass_ipmf1_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("twamp_reflector_2nd_pass_ipmf1_fg_id:%d\n", twamp_reflector_2nd_pass_ipmf1_fg_id);
	
    /* Attaching the FG */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    for (i = 0; i < fg_info.nof_quals; i++)
    {
        attach_info.key_info.qual_types[i] = fg_info.qual_types[i];
    }

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for (i = 0; i < fg_info.nof_actions; i++)
    {
        attach_info.payload_info.action_types[i] = fg_info.action_types[i];
        if (fg_info.action_types[i] == twamp_reflector_user_act_id) {
            attach_info.payload_info.action_info[i].priority = BCM_FIELD_ACTION_DONT_CARE;
        } else {
            attach_info.payload_info.action_info[i].priority = BCM_FIELD_ACTION_PRIORITY(0, 1);
        }
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[1].input_arg = 0;
    attach_info.key_info.qual_info[1].offset = 0;

    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[2].input_arg = 1;
    attach_info.key_info.qual_info[2].offset = 0;

    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[3].input_arg = 1;
    attach_info.key_info.qual_info[3].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, twamp_reflector_2nd_pass_ipmf1_fg_id, twamp_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    /* Add field entry */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 2;

    ent_info.nof_entry_quals = fg_info.nof_quals;
    if (is_ipv6) {  
        ent_info.entry_qual[0].type = fg_info.qual_types[0];
        cint_field_twamp_reflector_reverse_memcpy(ent_info.entry_qual[0].value, local_ip6_addr, 16);
        sal_memset(ent_info.entry_qual[0].mask, 0xff, 16);

        ent_info.entry_qual[1].type = fg_info.qual_types[1];
        cint_field_twamp_reflector_reverse_memcpy(ent_info.entry_qual[1].value, remote_ip6_addr, 16);
        sal_memset(ent_info.entry_qual[1].mask, 0xff, 16);
    } else {    
        ent_info.entry_qual[0].type = fg_info.qual_types[0];
        ent_info.entry_qual[0].value[0] = local_ip_addr;
        ent_info.entry_qual[0].mask[0] = 0xffffffff;

        ent_info.entry_qual[1].type = fg_info.qual_types[1];
        ent_info.entry_qual[1].value[0] = remote_ip_addr;
        ent_info.entry_qual[1].mask[0] = 0xffffffff;
    }

    ent_info.entry_qual[2].type = fg_info.qual_types[2];
    ent_info.entry_qual[2].value[0] = 862;
    ent_info.entry_qual[2].mask[0] = 0xffff;

    ent_info.entry_qual[3].type = fg_info.qual_types[3];
    ent_info.entry_qual[3].value[0] = 50001;
    ent_info.entry_qual[3].mask[0] = 0xffff;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = 8; /* counter_id */
	
    ent_info.entry_action[1].type = fg_info.action_types[1];
    ent_info.entry_action[1].value[0] = 1; /* is_meter or lm_incr */
    ent_info.entry_action[1].value[1] = 1; /* is_lm */
    ent_info.entry_action[1].value[2] = 0; /* type */
    ent_info.entry_action[1].value[3] = 1; /* valid */
	
	ent_info.entry_action[2].type = fg_info.action_types[2];
    ent_info.entry_action[2].value[0] = 4; /* Do not decrement the TTL indication */

    ent_info.entry_action[3].type = fg_info.action_types[3];
    ent_info.entry_action[3].value[0] = 1; /* Present the 2nd pass */

    rv = bcm_field_entry_add(unit, 0, twamp_reflector_2nd_pass_ipmf1_fg_id, &ent_info, &ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *
 *  This function sets all required HW configuration for TWAMP reflector over IPv4 for the 2nd pass IPMF2.
 *
 * \param [in] unit - The unit number. *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int cint_field_twamp_reflector_2nd_pass_set_ipmf2(int unit)
{   int i, rv = BCM_E_NONE;   
    void *dest_char;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualify_t user_qual_id, user_qual_id2;
    bcm_field_action_t user_act_id;
    bcm_field_action_info_t user_action_info;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;  
    bcm_field_fem_action_info_t  fem_action_info;
    int fem_action_oam_value = 0x70000; /* MEP-type: Down, MEP-sub-type: TWAMP-Reflector-2nd */
    int action_offset;


    /* Create an user-qualifier in ipmf2 that will parse the action buffer received from ipmf1 (cascaded)  */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1; 
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "TWAMP_REF2_PMF2_Q1", sizeof(qual_info.name));

    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &user_qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1; 
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "TWAMP_REF2_PMF2_Q2", sizeof(qual_info.name));

    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &user_qual_id2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    /* Create a user action to be used as FEM action */
    bcm_field_action_info_t_init(&user_action_info);
    user_action_info.action_type = bcmFieldActionVoid;
    user_action_info.prefix_size = 0;
    user_action_info.prefix_value = 0;
    user_action_info.size = 10;
    user_action_info.stage = bcmFieldStageIngressPMF2;
    rv = bcm_field_action_create(unit, 0, &user_action_info, &user_act_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }    

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;

    /* Qualifiers: 
     * User define qualifier    read the action buffer received from ipmf1   
     * LayerRecordOffset      read the offset of FWD layer 
     */
    fg_info.nof_quals = 3;    
    fg_info.qual_types[0] = user_qual_id;
    fg_info.qual_types[1] = user_qual_id2;
    fg_info.qual_types[2] = bcmFieldQualifyLayerRecordOffset;

    /* Actions: use Direct Extration to read the offset of FED layer and use FEM to increment the TWAMP offset
     * bcmFieldActionOam construction:
     *     oam_stamp_offset[0:7]: fwd_header.offset + 0x34(TWAMP offset)
     *     oam_offset[8:15]: 0
     *     oam_sub_type[16:19]: 7(TWAMP-Reflector-2nd)
     *     oam_up_mep[20]: 0 (DOWN)
     */
    fg_info.nof_actions = 1;
  
    fg_info.action_types[0] = user_act_id;
    fg_info.action_with_valid_bit[0] = FALSE;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "TWAMP_REF2_PMF2_FG", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, 0, &fg_info, &twamp_reflector_2nd_pass_ipmf2_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_group_add ipmf2_fg\n", rv);
        return rv;
    }
    printf("twamp_reflector_2nd_pass_ipmf2_fg_id:%d\n", twamp_reflector_2nd_pass_ipmf2_fg_id);
    
    /**FEM CONFIG*/
    bcm_field_fem_action_info_t_init(&fem_action_info);
    fem_action_info.fem_input.input_offset = 0;  
    fem_action_info.condition_msb = 3;
    for(i = 0 ; i < BCM_FIELD_NUMBER_OF_CONDITIONS_PER_FEM; i++)
    {
        fem_action_info.fem_condition[i].is_action_valid = ((i & 0x3) == 0x3) ? TRUE : FALSE; /* 3, 7, 11, 15 */
        fem_action_info.fem_condition[i].extraction_id = 0;
    }
    fem_action_info.fem_extraction[0].action_type = bcmFieldActionOam;
    /* oam action is 21 bits width, use the FEM to set 8 LSB as mapping for oam_stamp_offset
    * 13 MSB as force for oam_offset, oam_sub_type and oam_up_mep */
    for(i = 0 ; i < 21; i++)
    {
        if (i < 8) {
            fem_action_info.fem_extraction[0].output_bit[i].source_type = bcmFieldFemExtractionOutputSourceTypeKey;
            fem_action_info.fem_extraction[0].output_bit[i].offset = i + 2;
            fem_action_info.fem_extraction[0].output_bit[i].forced_value = 0;
        } else {
            fem_action_info.fem_extraction[0].output_bit[i].source_type = bcmFieldFemExtractionOutputSourceTypeForce;
            fem_action_info.fem_extraction[0].output_bit[i].offset = 0;
            fem_action_info.fem_extraction[0].output_bit[i].forced_value = (fem_action_oam_value & (0x1 << i)) ? 1 : 0;
        }
    }
    fem_action_info.fem_extraction[0].increment = 12;
    
    rv = bcm_field_fem_action_add(unit,0 ,twamp_reflector_2nd_pass_ipmf2_fg_id, BCM_FIELD_ACTION_POSITION(3, 3), &fem_action_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_fem_action_add ipmf2\n", rv);
        return rv;
    }
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for(i = 0; i < fg_info.nof_quals; i++)
    {
        attach_info.key_info.qual_types[i] = fg_info.qual_types[i];
    }
    for(i = 0; i < fg_info.nof_actions; i++)
    {
        attach_info.payload_info.action_types[i] = fg_info.action_types[i];
    }
    
    /* fg_id_ipmf1 is the Field group in ipmf1 to use the result of */
    bcm_field_group_info_get( unit, twamp_reflector_2nd_pass_ipmf1_fg_id, &fg_info);
    
    /* Get the FAI action valid bit   */
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeCascaded;
    attach_info.key_info.qual_info[0].input_arg = twamp_reflector_2nd_pass_ipmf1_fg_id;   

    /* Get the offset for the action */
    rv = bcm_field_group_action_offset_get(unit, 0, twamp_reflector_2nd_pass_ipmf1_fg_id, fg_info.action_types[2], &action_offset);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_action_offset_get\n", rv);
        return rv;
    }
    attach_info.key_info.qual_info[0].offset = action_offset - 1;   

    /* Get the TWAMP user action bit  */
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeCascaded;
    attach_info.key_info.qual_info[1].input_arg = twamp_reflector_2nd_pass_ipmf1_fg_id;   

    /* Get the offset for the action */
    rv = bcm_field_group_action_offset_get(unit, 0, twamp_reflector_2nd_pass_ipmf1_fg_id, fg_info.action_types[3], &action_offset);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_action_offset_get\n", rv);
        return rv;
    }
    attach_info.key_info.qual_info[1].offset = action_offset;   

    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[2].input_arg = 1;
    
    rv = bcm_field_group_context_attach(unit, 0, twamp_reflector_2nd_pass_ipmf2_fg_id, twamp_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_group_context_attach ipmf2\n", rv);
        return rv;
    }
    
    return 0;
}

/**
 * \brief
 *
 *  This function sets all required HW configuration for LM read index feature and attach it to given as param context.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int cint_field_twamp_reflector_lm_read_index_set_ipmf3(int unit) {
    int i, rv = BCM_E_NONE;
    void *dest_char;
    bcm_field_context_t context_id = dnx_data_field.context.default_context_get(unit);
    bcm_field_group_t fg_id;
    bcm_field_entry_t ent_id;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;

    /** Create field group */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF3;
    
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyStatOamLM;

    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionStatOamLM;
    
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "TWAMP_PMF3_FG", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    /* Attaching the FG */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    for (i = 0; i < fg_info.nof_quals; i++)
    {
        attach_info.key_info.qual_types[i] = fg_info.qual_types[i];
    }

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for (i = 0; i < fg_info.nof_actions; i++)
    {
        attach_info.payload_info.action_types[i] = fg_info.action_types[i];
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    rv = bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    /* Add field entry */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = bcmFieldStatOamLmIndexInvalid;
    ent_info.entry_qual[0].mask[0] = 3;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = bcmFieldStatOamLmIndex0;


    rv = bcm_field_entry_add(unit, 0, fg_id, &ent_info, &ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *
 *  This function sets all required HW configuration for TWAMP reflector over IPv4 for the 1st pass and 2nd pass.
 *
 * \param [in] unit - The unit number. 
 * \param [in] is_ipv6 - The flag indicates it is the ipv6.
 * \param [in] gport_trap1 - The gport trap id.
 * \param [in] local_ip_addr - Local ip address.
 * \param [in] remote_ip_addr - Remote ip address.
 * \param [in] local_ip6_addr - Local ip6 address.
 * \param [in] remote_ip6_addr - Remote ip6 address.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int cint_field_twamp_reflector_rule(int unit, int is_ipv6, bcm_gport_t gport_trap1, 
    uint32 local_ip_addr, uint32 remote_ip_addr,
    bcm_ip6_t *local_ip6_addr, bcm_ip6_t *remote_ip6_addr) {
    int rv = BCM_E_NONE;
    bcm_field_context_info_t context_info;
    bcm_field_presel_t ingress_presel_id = 10;

    /*create context */
    bcm_field_context_info_t_init(&context_info);
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &twamp_context_id);
    if(rv != BCM_E_NONE)
    {
        printf("bcm_field_context_create error %d context %d\n", rv, twamp_context_id);
        return rv;
    }
    printf("twamp_context_id:%d\n", twamp_context_id);

    /* Create presel */
    rv = cint_field_twamp_presel_set(unit, is_ipv6, ingress_presel_id, twamp_context_id);
    if(rv != BCM_E_NONE)
    {
        printf("cint_field_twamp_presel_set error %d\n", rv);
        return rv;
    }

    /*** create user action ***/
    rv = cint_field_twamp_reflector_user_act_create(unit);
    if(rv != BCM_E_NONE)
    {
        printf("cint_field_twamp_reflector_user_act_create error %d\n", rv);
        return rv;
    }
    
    /*** create IPMF1 rule for 1st pass ***/
    rv = cint_field_twamp_reflector_1st_pass_set_ipmf1(unit, is_ipv6, gport_trap1, local_ip_addr, remote_ip_addr, local_ip6_addr, remote_ip6_addr);
    if(rv != BCM_E_NONE)
    {
        printf("cint_field_twamp_reflector_1st_pass_set_ipmf1 error %d\n", rv);
        return rv;
    }

    /*** create IPMF2 rule for 1st pass ***/
    rv = cint_field_twamp_reflector_1st_pass_set_ipmf2(unit);
    if(rv != BCM_E_NONE)
    {
        printf("cint_field_twamp_reflector_1st_pass_set_ipmf2 error %d\n", rv);
        return rv;
    }
	
	/*** create IPMF1 rule for 2nd pass ***/
    rv = cint_field_twamp_reflector_2nd_pass_set_ipmf1(unit, is_ipv6, local_ip_addr, remote_ip_addr, local_ip6_addr, remote_ip6_addr);
    if(rv != BCM_E_NONE)
    {
        printf("cint_field_twamp_reflector_2nd_pass_set_ipmf1 error %d\n", rv);
        return rv;
    }

    /*** create IPMF2 rule for 2nd pass ***/
    rv = cint_field_twamp_reflector_2nd_pass_set_ipmf2(unit);
    if(rv != BCM_E_NONE)
    {
        printf("cint_field_twamp_reflector_2nd_pass_set_ipmf2 error %d\n", rv);
        return rv;
    }

    /* Add lm_read_index for TWAMP */
    if (*(dnxc_data_get(unit,  "field", "init", "oam_stat", NULL)) == 0) {
	    rv = cint_field_twamp_reflector_lm_read_index_set_ipmf3(unit);
	    if(rv != BCM_E_NONE)
	    {
	        printf("cint_field_twamp_reflector_lm_read_index_set_ipmf3 error %d\n", rv);
	        return rv;
	    }
	}

    return rv;
}

/**
 * \brief
 *
 *  This function sets all required HW configuration for TWAMP RX over IPv4 for the IPMF1.
 *
 * \param [in] unit - The unit number.
 * \param [in] gport_trap1 - The gport trap id.
 * \param [in] trap_code_qual - The trap code qualifier.
 * \param [in] local_ip_addr - Local ip address.
 * \param [in] remote_ip_addr - Remote ip address.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int cint_field_twamp_rx_set_ipmf1(int unit, bcm_gport_t gport_trap1, int trap_code_qual, uint32 local_ip_addr, uint32 remote_ip_addr) {
    int i, rv = BCM_E_NONE;
    void *dest_char;
    bcm_field_entry_t ent_id;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;

    /** Create field group */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Qualifiers: 
     *    (SIP == Session-SIP)               obtained by TWAMP-Control session initialization     
     *    (DIP == Session-DIP)               obtained by TWAMP-Control session initialization
     *    (UDP.Src-Port == Session-Src-Port) obtained by TWAMP-Control session initialization
     *    (UDP.Dst-Port == Session-Dst-Port) obtained by TWAMP-Control session initialization
     */
    fg_info.nof_quals = 4;    
    fg_info.qual_types[0] = bcmFieldQualifySrcIp;
    fg_info.qual_types[1] = bcmFieldQualifyDstIp;
    fg_info.qual_types[2] = bcmFieldQualifyL4SrcPort;
    fg_info.qual_types[3] = bcmFieldQualifyL4DstPort;

    /* Actions:
     * CPU-Trap-Code indicating TWAMP-REFLECTOR-TRAP
     */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionTrap;    
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "TWAMP_RX_PMF1_FG", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &twamp_rx_ipmf1_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("twamp_rx_ipmf1_fg_id:%d\n", twamp_rx_ipmf1_fg_id);

    /* Attaching the FG */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    for (i = 0; i < fg_info.nof_quals; i++)
    {
        attach_info.key_info.qual_types[i] = fg_info.qual_types[i];
    }

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for (i = 0; i < fg_info.nof_actions; i++)
    {
        attach_info.payload_info.action_types[i] = fg_info.action_types[i];
        attach_info.payload_info.action_info[i].priority = BCM_FIELD_ACTION_PRIORITY(0, 2);
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[1].input_arg = 0;
    attach_info.key_info.qual_info[1].offset = 0;

    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[2].input_arg = 1;
    attach_info.key_info.qual_info[2].offset = 0;

    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[3].input_arg = 1;
    attach_info.key_info.qual_info[3].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, twamp_rx_ipmf1_fg_id, twamp_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    /* Add field entry */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = remote_ip_addr;
    ent_info.entry_qual[0].mask[0] = 0xffffffff;

    ent_info.entry_qual[1].type = fg_info.qual_types[1];
    ent_info.entry_qual[1].value[0] = local_ip_addr;
    ent_info.entry_qual[1].mask[0] = 0xffffffff;

    ent_info.entry_qual[2].type = fg_info.qual_types[2];
    ent_info.entry_qual[2].value[0] = 50001;
    ent_info.entry_qual[2].mask[0] = 0xffff;

    ent_info.entry_qual[3].type = fg_info.qual_types[3];
    ent_info.entry_qual[3].value[0] = 862;
    ent_info.entry_qual[3].mask[0] = 0xffff;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = gport_trap1;
    ent_info.entry_action[0].value[1] = (trap_code_qual != -1) ? trap_code_qual : 0x1234;

    rv = bcm_field_entry_add(unit, 0, twamp_rx_ipmf1_fg_id, &ent_info, &ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *
 *  This function sets all required HW configuration for TWAMP RX over IPv4 for the IPMF2.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int cint_field_twamp_rx_set_ipmf2(int unit)
{   int i, rv = BCM_E_NONE;   
    void *dest_char;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualify_t user_qual_id;
    bcm_field_action_t user_act_id;
    bcm_field_action_info_t user_action_info;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;    
    bcm_field_fem_action_info_t  fem_action_info;
    int fem_action_oam_value = 0x30000; /* MEP-type: Down, MEP-sub-type: DM-NTP */
    int action_offset;

    /* Create an user-qualifier in ipmf2 that will parse the action buffer received from ipmf1 (cascaded)  */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1; 
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "TWAMP_RX_QUAL", sizeof(qual_info.name));

    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &user_qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    /* Create a user action to be used as FEM action */
    bcm_field_action_info_t_init(&user_action_info);
    user_action_info.action_type = bcmFieldActionVoid;
    user_action_info.prefix_size = 0;
    user_action_info.prefix_value = 0;
    user_action_info.size = 9;
    user_action_info.stage = bcmFieldStageIngressPMF2;
    rv = bcm_field_action_create(unit, 0, &user_action_info, &user_act_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }    

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;

    /* Qualifiers: 
     * User define qualifier    read the action buffer received from ipmf1   
     * LayerRecordOffset      read the offset of FWD layer 
     */
    fg_info.nof_quals = 2;    
    fg_info.qual_types[0] = user_qual_id;
    fg_info.qual_types[1] = bcmFieldQualifyLayerRecordOffset;

    /* Actions: use Direct Extration to read the offset of UDP layer and use FEM to increment the TWAMP offset
     * bcmFieldActionOam construction:
     *     oam_stamp_offset[0:7]: UDP.offset + 32(TWAMP offset)
     *     oam_offset[8:15]: 0
     *     oam_sub_type[16:19]: 3 (DM-NTP)
     *     oam_up_mep[20]: 0 (Down)
     */
    fg_info.nof_actions = 1;
  
    fg_info.action_types[0] = user_act_id;
    fg_info.action_with_valid_bit[0] = FALSE;    
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "TWAMP_RX_PMF2_FG", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, 0, &fg_info, &twamp_rx_ipmf2_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_group_add ipmf2_fg\n", rv);
        return rv;
    }
    printf("twamp_rx_ipmf2_fg_id:%d\n", twamp_rx_ipmf2_fg_id);
    
    /**FEM CONFIG*/
    bcm_field_fem_action_info_t_init(&fem_action_info);
    fem_action_info.fem_input.input_offset = 0;  
    fem_action_info.condition_msb = 3;
    for(i = 0 ; i < BCM_FIELD_NUMBER_OF_CONDITIONS_PER_FEM; i++)
    {
        fem_action_info.fem_condition[i].is_action_valid = ((i & 0x1) == 0x1) ? TRUE : FALSE; 
        fem_action_info.fem_condition[i].extraction_id = 0;
    }
    fem_action_info.fem_extraction[0].action_type = bcmFieldActionOam;
    /* oam action is 21 bits width, use the FEM to set 8 LSB as mapping for oam_stamp_offset
    * 13 MSB as force for oam_offset, oam_sub_type and oam_up_mep */
    for(i = 0 ; i < 21; i++)
    {
        if (i < 8) {
            fem_action_info.fem_extraction[0].output_bit[i].source_type = bcmFieldFemExtractionOutputSourceTypeKey;
            fem_action_info.fem_extraction[0].output_bit[i].offset = i + 1;
            fem_action_info.fem_extraction[0].output_bit[i].forced_value = 0;
        } else {
            fem_action_info.fem_extraction[0].output_bit[i].source_type = bcmFieldFemExtractionOutputSourceTypeForce;
            fem_action_info.fem_extraction[0].output_bit[i].offset = 0;
            fem_action_info.fem_extraction[0].output_bit[i].forced_value = (fem_action_oam_value & (0x1 << i)) ? 1 : 0;
        }
    }
    fem_action_info.fem_extraction[0].increment = 32;
    
    rv = bcm_field_fem_action_add(unit,0 ,twamp_rx_ipmf2_fg_id, BCM_FIELD_ACTION_POSITION(3, 3), &fem_action_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_fem_action_add ipmf2\n", rv);
        return rv;
    }
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for(i = 0; i < fg_info.nof_quals; i++)
    {
        attach_info.key_info.qual_types[i] = fg_info.qual_types[i];
    }
    for(i = 0; i < fg_info.nof_actions; i++)
    {
        attach_info.payload_info.action_types[i] = fg_info.action_types[i];
    }
    
    /* fg_id_ipmf1 is the Field group in ipmf1 to use the result of */
    bcm_field_group_info_get( unit, twamp_rx_ipmf1_fg_id, &fg_info);
    
    /* Get the Trap action valid bit   */
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeCascaded;
    attach_info.key_info.qual_info[0].input_arg = twamp_rx_ipmf1_fg_id;    

    /* Get the offset for the action */
    rv = bcm_field_group_action_offset_get(unit, 0, twamp_rx_ipmf1_fg_id, fg_info.action_types[0], &action_offset);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_action_offset_get\n", rv);
        return rv;
    }
    attach_info.key_info.qual_info[0].offset = action_offset - 1;  

    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[1].input_arg = 1;
    
    rv = bcm_field_group_context_attach(unit, 0, twamp_rx_ipmf2_fg_id, twamp_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_group_context_attach ipmf2\n", rv);
        return rv;
    }
    
    return 0;
}

/**
 * \brief
 *
 *  This function sets all required HW configuration for TWAMP TX over IPv4 for the IPMF1.
 *
 * \param [in] unit - The unit number.
 * \param [in] local_ip_addr - Local ip address.
 * \param [in] remote_ip_addr - Remote ip address.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int cint_field_twamp_tx_set_ipmf1(int unit, uint32 local_ip_addr, uint32 remote_ip_addr) {
    int i, rv = BCM_E_NONE;
    void *dest_char;
    bcm_field_entry_t ent_id;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;

    /** Create field group */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Qualifiers: 
     *    (SIP == Session-SIP)               obtained by TWAMP-Control session initialization     
     *    (DIP == Session-DIP)               obtained by TWAMP-Control session initialization
     *    (UDP.Src-Port == Session-Src-Port) obtained by TWAMP-Control session initialization
     *    (UDP.Dst-Port == Session-Dst-Port) obtained by TWAMP-Control session initialization
     */
    fg_info.nof_quals = 4;
    fg_info.qual_types[0] = bcmFieldQualifySrcIp;
    fg_info.qual_types[1] = bcmFieldQualifyDstIp;
    fg_info.qual_types[2] = bcmFieldQualifyL4SrcPort;
    fg_info.qual_types[3] = bcmFieldQualifyL4DstPort;

    /* Actions:
     *     FAI[2] = 1, do not decrement TTL
     */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionForwardingAdditionalInfo;      
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "TWAMP_TX_PMF1_FG", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &twamp_tx_ipmf1_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("twamp_tx_ipmf1_fg_id:%d\n", twamp_tx_ipmf1_fg_id);

    /* Attaching the FG */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    for (i = 0; i < fg_info.nof_quals; i++)
    {
        attach_info.key_info.qual_types[i] = fg_info.qual_types[i];
    }

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for (i = 0; i < fg_info.nof_actions; i++)
    {
        attach_info.payload_info.action_types[i] = fg_info.action_types[i];
        attach_info.payload_info.action_info[i].priority = BCM_FIELD_ACTION_PRIORITY(0, 2);
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[1].input_arg = 0;
    attach_info.key_info.qual_info[1].offset = 0;

    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[2].input_arg = 1;
    attach_info.key_info.qual_info[2].offset = 0;

    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[3].input_arg = 1;
    attach_info.key_info.qual_info[3].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, twamp_tx_ipmf1_fg_id, twamp_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    /* Add field entry */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = local_ip_addr;
    ent_info.entry_qual[0].mask[0] = 0xffffffff;

    ent_info.entry_qual[1].type = fg_info.qual_types[1];
    ent_info.entry_qual[1].value[0] = remote_ip_addr;
    ent_info.entry_qual[1].mask[0] = 0xffffffff;

    ent_info.entry_qual[2].type = fg_info.qual_types[2];
    ent_info.entry_qual[2].value[0] = 50001;
    ent_info.entry_qual[2].mask[0] = 0xffff;

    ent_info.entry_qual[3].type = fg_info.qual_types[3];
    ent_info.entry_qual[3].value[0] = 862;
    ent_info.entry_qual[3].mask[0] = 0xffff;

    ent_info.nof_entry_actions = fg_info.nof_actions;	
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = 4; /* Do not decrement the TTL indication */

    rv = bcm_field_entry_add(unit, 0, twamp_tx_ipmf1_fg_id, &ent_info, &ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *
 *  This function sets all required HW configuration for TWAMP TX over IPv4 for the IPMF2.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int cint_field_twamp_tx_set_ipmf2(int unit)
{   int i, rv = BCM_E_NONE;   
    void *dest_char;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualify_t user_qual_id;
    bcm_field_action_t user_act_id;
    bcm_field_action_info_t user_action_info;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;    
    bcm_field_fem_action_info_t  fem_action_info;
    int fem_action_oam_value = 0x130000; /* MEP-type: Up, MEP-sub-type: DM-NTP */
    int action_offset;

    /* Create an user-qualifier in ipmf2 that will parse the action buffer received from ipmf1 (cascaded)  */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1; 
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "TWAMP_TX_QUAL", sizeof(qual_info.name));

    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &user_qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    /* Create a user action to be used as FEM action */
    bcm_field_action_info_t_init(&user_action_info);
    user_action_info.action_type = bcmFieldActionVoid;
    user_action_info.prefix_size = 0;
    user_action_info.prefix_value = 0;
    user_action_info.size = 9;
    user_action_info.stage = bcmFieldStageIngressPMF2;
    rv = bcm_field_action_create(unit, 0, &user_action_info, &user_act_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }    

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;

    /* Qualifiers: 
     * User define qualifier    read the action buffer received from ipmf1   
     * LayerRecordOffset      read the offset of FWD layer 
     */
    fg_info.nof_quals = 2;    
    fg_info.qual_types[0] = user_qual_id;
    fg_info.qual_types[1] = bcmFieldQualifyLayerRecordOffset;

    /* Actions: use Direct Extration to read the offset of UDP layer and use FEM to increment the TWAMP offset
     * bcmFieldActionOam construction:
     *     oam_stamp_offset[0:7]: UDP.offset + 12(TWAMP offset)
     *     oam_offset[8:15]: 0
     *     oam_sub_type[16:19]: 3(DM-NTP)
     *     oam_up_mep[20]: 1
     */
    fg_info.nof_actions = 1;
  
    fg_info.action_types[0] = user_act_id;
    fg_info.action_with_valid_bit[0] = FALSE;    
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "TWAMP_TX_PMF2_FG", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, 0, &fg_info, &twamp_tx_ipmf2_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_group_add ipmf2_fg\n", rv);
        return rv;
    }
    printf("twamp_tx_ipmf2_fg_id:%d\n", twamp_tx_ipmf2_fg_id);
    
    /**FEM CONFIG*/
    bcm_field_fem_action_info_t_init(&fem_action_info);
    fem_action_info.fem_input.input_offset = 0;  
    fem_action_info.condition_msb = 3;
    for(i = 0 ; i < BCM_FIELD_NUMBER_OF_CONDITIONS_PER_FEM; i++)
    {
        fem_action_info.fem_condition[i].is_action_valid = ((i & 0x1) == 0x1) ? TRUE : FALSE; 
        fem_action_info.fem_condition[i].extraction_id = 0;
    }
    fem_action_info.fem_extraction[0].action_type = bcmFieldActionOam;
    /* oam action is 21 bits width, use the FEM to set 8 LSB as mapping for oam_stamp_offset
    * 13 MSB as force for oam_offset, oam_sub_type and oam_up_mep */
    for(i = 0 ; i < 21; i++)
    {
        if (i < 8) {
            fem_action_info.fem_extraction[0].output_bit[i].source_type = bcmFieldFemExtractionOutputSourceTypeKey;
            fem_action_info.fem_extraction[0].output_bit[i].offset = i + 1;
            fem_action_info.fem_extraction[0].output_bit[i].forced_value = 0;
        } else {
            fem_action_info.fem_extraction[0].output_bit[i].source_type = bcmFieldFemExtractionOutputSourceTypeForce;
            fem_action_info.fem_extraction[0].output_bit[i].offset = 0;
            fem_action_info.fem_extraction[0].output_bit[i].forced_value = (fem_action_oam_value & (0x1 << i)) ? 1 : 0;
        }
    }
    fem_action_info.fem_extraction[0].increment = 12;
    
    rv = bcm_field_fem_action_add(unit,0 ,twamp_tx_ipmf2_fg_id, BCM_FIELD_ACTION_POSITION(3, 2), &fem_action_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_fem_action_add ipmf2\n", rv);
        return rv;
    }
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for(i = 0; i < fg_info.nof_quals; i++)
    {
        attach_info.key_info.qual_types[i] = fg_info.qual_types[i];
    }
    for(i = 0; i < fg_info.nof_actions; i++)
    {
        attach_info.payload_info.action_types[i] = fg_info.action_types[i];
    }
    
    /* fg_id_ipmf1 is the Field group in ipmf1 to use the result of */
    bcm_field_group_info_get( unit, twamp_tx_ipmf1_fg_id, &fg_info);
    
    /* Get the FAI action valid bit   */
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeCascaded;
    attach_info.key_info.qual_info[0].input_arg = twamp_tx_ipmf1_fg_id;    

    /* Get the offset for the action */
    rv = bcm_field_group_action_offset_get(unit, 0, twamp_tx_ipmf1_fg_id, fg_info.action_types[0], &action_offset);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_action_offset_get\n", rv);
        return rv;
    }
    attach_info.key_info.qual_info[0].offset = action_offset - 1;    

    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[1].input_arg = 1;
    
    rv = bcm_field_group_context_attach(unit, 0, twamp_tx_ipmf2_fg_id, twamp_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_group_context_attach ipmf2\n", rv);
        return rv;
    }
    
    return 0;
}

/**
 * \brief
 *
 *  This function sets all required HW configuration for TWAMP TX/RX over IPv4.
 *
 * \param [in] unit - The unit number. 
 * \param [in] gport_trap1 - The gport trap id.
 * \param [in] trap_code_qual - The trap code qualifier.
 * \param [in] local_ip_addr - Local ip address.
 * \param [in] remote_ip_addr - Remote ip address.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int cint_field_twamp_tx_rx_rule(int unit, bcm_gport_t gport_trap1, int trap_code_qual, uint32 local_ip_addr, uint32 remote_ip_addr) {
    int rv = BCM_E_NONE;
    bcm_field_context_info_t context_info;
    bcm_field_presel_t ingress_presel_id = 10;

    /*create context */
    bcm_field_context_info_t_init(&context_info);
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &twamp_context_id);
    if(rv != BCM_E_NONE)
    {
        printf("bcm_field_context_create error %d context %d\n", rv, twamp_context_id);
        return rv;
    }
    printf("twamp_context_id:%d\n", twamp_context_id);

    /* Create presel */
    rv = cint_field_twamp_presel_set(unit, 0, ingress_presel_id, twamp_context_id);
    if(rv != BCM_E_NONE)
    {
        printf("cint_field_twamp_presel_set error %d\n", rv);
        return rv;
    }
	
	/*** create IPMF1 rule for RX ***/
    rv = cint_field_twamp_rx_set_ipmf1(unit, gport_trap1, trap_code_qual, local_ip_addr, remote_ip_addr);
    if(rv != BCM_E_NONE)
    {
        printf("cint_field_twamp_rx_set_ipmf1 error %d\n", rv);
        return rv;
    }

    /*** create IPMF2 rule for RX ***/
    rv = cint_field_twamp_rx_set_ipmf2(unit);
    if(rv != BCM_E_NONE)
    {
        printf("cint_field_twamp_rx_set_ipmf2 error %d\n", rv);
        return rv;
    }
    
    /*** create IPMF1 rule for TX ***/
    rv = cint_field_twamp_tx_set_ipmf1(unit, local_ip_addr, remote_ip_addr);
    if(rv != BCM_E_NONE)
    {
        printf("cint_field_twamp_tx_set_ipmf1 error %d\n", rv);
        return rv;
    }

    /*** create IPMF2 rule for TX ***/
    rv = cint_field_twamp_tx_set_ipmf2(unit);
    if(rv != BCM_E_NONE)
    {
        printf("cint_field_twamp_tx_set_ipmf2 error %d\n", rv);
        return rv;
    }

    return rv;
}

