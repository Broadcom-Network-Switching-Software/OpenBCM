/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Configuration example start:
 *
 * cint;                                                                  
 * cint_reset();                                                          
 * exit; 
 * cint ../../../src/examples/dnx/field/cint_field_etpp_fwd_term_min_bytes.c
 * cint;
 * int unit = 0;
 * cint_field_etpp_fwd_term_min_bytes_main(unit);
 *
 * Configuration example end
 *
 * This CINT configures two FGs (DE and TCAM) in iPMF3 that in case of
 * inner-IP(layer 3/4) forwarding packets it changes the FWD_LAYER index/offset to the inner-IP
 * and changes the Egress parser context in order for the ETPP to terminate minimum required bytes
 * from the header, therefore allowing the packet to be rebuilt accordingly.
 * 
 * This CINT skip Ingress termination that prevents the Ingress side from
 * terminating the outter layers before sending packet to Egress.
 */
bcm_field_context_t etpp_fwd_term_context_id_ipmf3_v4;
bcm_field_context_t etpp_fwd_term_context_id_ipmf3_v6;
bcm_field_group_t   etpp_fwd_term_tcam_fg_id;
bcm_field_group_t   etpp_fwd_term_de_fg_id;
bcm_field_entry_t   etpp_fwd_term_ent_3_ipv4_id;
bcm_field_entry_t   etpp_fwd_term_ent_3_ipv6_id;
bcm_field_entry_t   etpp_fwd_term_ent_4_ipv4_id;
bcm_field_entry_t   etpp_fwd_term_ent_4_ipv6_id;
bcm_field_entry_t   etpp_fwd_term_ent_def_id;
bcm_field_qualify_t etpp_fwd_term_qual_layer_offset;
bcm_field_qualify_t etpp_fwd_term_qual_const_one;
int                 etpp_fwd_term_presel_id_v4 = 20;
int                 etpp_fwd_term_presel_id_v6 = 21;
bcm_field_action_t  etpp_fwd_term_parsing_start_offset_action_id;

int cint_field_etpp_fwd_term_min_bytes_main(
    int unit)
{
    /* IPMF3 IPv4 Context Create + Presel */
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_context_info_t context_info;
    void *dest_char;
    int hw_value;

    int rv = BCM_E_NONE;

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "nof_rebuild_ipmf3_v4", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, &etpp_fwd_term_context_id_ipmf3_v4);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = etpp_fwd_term_presel_id_v4;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = TRUE;
    p_data.context_id = etpp_fwd_term_context_id_ipmf3_v4;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set \n", rv);
        return rv;
    }

    /* IPMF3 IPv6 Context Create + Presel */

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "nof_rebuild_ipmf3_v6", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, &etpp_fwd_term_context_id_ipmf3_v6);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = etpp_fwd_term_presel_id_v6;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = TRUE;
    p_data.context_id = etpp_fwd_term_context_id_ipmf3_v6;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp6;
    p_data.qual_data[0].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set \n", rv);
        return rv;
    }

    /* IPMF3 FG Create + Attach */

    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t qual_info;
    int extracted_action_size = 0;

    /*
     * Create and attach DE group in iPMF3
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;

    /**
     * Create two qualifiers for DE group:
     * 1) 1b of Const-one for the inner-valid bit
     * 2) 7b of layer_offset since layer_offset is 8b but one bit is not in use and action expects 7b
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "etpp_term_const_one", sizeof(fg_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &etpp_fwd_term_qual_const_one);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create, etpp_fwd_term_qual_layer_offset\n", rv);
        return rv;
    }
    extracted_action_size += qual_info.size;

    qual_info.size = 7;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "etpp_term_layer_off", sizeof(fg_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &etpp_fwd_term_qual_layer_offset);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create, etpp_fwd_term_qual_layer_offset\n", rv);
        return rv;
    }
    extracted_action_size += qual_info.size;

    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = etpp_fwd_term_qual_const_one;
    fg_info.qual_types[1] = etpp_fwd_term_qual_layer_offset;

    /* Set actions */
    fg_info.nof_actions = 1;

    bcm_field_action_info_t pars_offset_action_info;
    int pars_offset_act_size;
    bcm_field_action_info_t_init(&pars_offset_action_info);
    rv = bcm_field_action_info_get(unit, bcmFieldActionParsingStartOffset, fg_info.stage, &pars_offset_action_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }
    pars_offset_act_size = pars_offset_action_info.size;

    bcm_field_action_info_t_init(&pars_offset_action_info);
    pars_offset_action_info.action_type = bcmFieldActionParsingStartOffset;
    dest_char = &(pars_offset_action_info.name[0]);
    sal_strncpy_s(dest_char, "Parsing_Start_Offset_Action", sizeof(pars_offset_action_info.name));
    pars_offset_action_info.size = extracted_action_size;
    pars_offset_action_info.prefix_size = (pars_offset_act_size - extracted_action_size);
    pars_offset_action_info.stage = fg_info.stage;
    pars_offset_action_info.prefix_value = 0;
    rv = bcm_field_action_create(unit, 0, &pars_offset_action_info, &etpp_fwd_term_parsing_start_offset_action_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }
    fg_info.action_types[0] = etpp_fwd_term_parsing_start_offset_action_id;
    fg_info.action_with_valid_bit[0] = FALSE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "nof_rebuild_de_fg", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, 0, &fg_info, &etpp_fwd_term_de_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    /*
     * Attach the FG to contexts
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    /* 1b inner valid */
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;

    /* 7b from Layer FWD offset*/
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[1].input_arg = 0;
    attach_info.key_info.qual_info[1].offset = 0;

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_info[0].priority = BCM_FIELD_ACTION_POSITION(0, 1);

    rv = bcm_field_group_context_attach(unit, 0, etpp_fwd_term_de_fg_id, etpp_fwd_term_context_id_ipmf3_v4, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_attach(unit, 0, etpp_fwd_term_de_fg_id, etpp_fwd_term_context_id_ipmf3_v6, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    /*
     * Create and attach TCAM group in iPMF3
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF3;

    fg_info.nof_quals = 4;
    fg_info.qual_types[0] = bcmFieldQualifyForwardingLayerIndex;
    fg_info.qual_types[1] = bcmFieldQualifyLayerRecordType;
    fg_info.qual_types[2] = bcmFieldQualifyIp4DstMulticast;
    fg_info.qual_types[3] = bcmFieldQualifyIp6MulticastCompatible;

    /* Set actions */
    fg_info.nof_actions = 4;
    fg_info.action_types[0] = bcmFieldActionParsingStartTypeRaw;
    fg_info.action_types[1] = bcmFieldActionEgressForwardingIndex;
    fg_info.action_types[2] = bcmFieldActionStartPacketStrip;
    fg_info.action_types[3] = bcmFieldActionInvalidNext;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "nof_rebuild_tcam_fg", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &etpp_fwd_term_tcam_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    /*
     * Attach the FG to contexts
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[1].input_arg = 0;
    attach_info.key_info.qual_info[1].offset = 0;
    attach_info.key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[2].input_arg = 0;
    attach_info.key_info.qual_info[2].offset = 0;
    attach_info.key_info.qual_types[3] = fg_info.qual_types[3];
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[3].input_arg = 0;
    attach_info.key_info.qual_info[3].offset = 0;

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    attach_info.payload_info.action_types[2] = fg_info.action_types[2];
    attach_info.payload_info.action_types[3] = fg_info.action_types[3];
    attach_info.payload_info.action_info[3].priority = BCM_FIELD_ACTION_POSITION(0, 0);

    rv = bcm_field_group_context_attach(unit, 0, etpp_fwd_term_tcam_fg_id, etpp_fwd_term_context_id_ipmf3_v4, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_attach(unit, 0, etpp_fwd_term_tcam_fg_id, etpp_fwd_term_context_id_ipmf3_v6, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    /*
     * Add TCAM entries
     */
    bcm_field_entry_info_t ent_info;
    bcm_field_entry_info_t_init(&ent_info);

    ent_info.nof_entry_quals = 4;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[1].type = fg_info.qual_types[1];
    ent_info.entry_qual[2].type = fg_info.qual_types[2];
    ent_info.entry_qual[3].type = fg_info.qual_types[3];

    ent_info.entry_qual[0].value[0] = 3; /* fwd_layer_index = 3 */
    ent_info.entry_qual[0].mask[0] = 0x7;
    ent_info.entry_qual[1].value[0] = bcmFieldLayerTypeIp4;
    ent_info.entry_qual[1].mask[0] = 0x1F;
    ent_info.entry_qual[2].value[0] = 0;
    ent_info.entry_qual[2].mask[0] = 0x1;
    ent_info.entry_qual[3].value[0] = 0;
    ent_info.entry_qual[3].mask[0] = 0; /* Ignore IPv6 UC check for IPv4 packets */

    ent_info.nof_entry_actions = 3;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    dnx_dbal_fields_enum_value_get(unit, "PARSING_START_TYPE_ENCODING", "IPV4_12B", &hw_value); /* DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_IPV4_12B */
    hw_value = hw_value << 1 | 1; /* The valid bit value is the LSB, and it's set to 1 */
    ent_info.entry_action[0].value[0] = hw_value;
    ent_info.entry_action[1].type = fg_info.action_types[1];
    ent_info.entry_action[1].value[0] = 3; /* Egress FWD Index should be equal to fwd_layer_index */
    ent_info.entry_action[2].type = fg_info.action_types[2];
    ent_info.entry_action[2].value[0] = bcmFieldStartToConfigurableStrip;
    ent_info.entry_action[2].value[1] = 0; /* nof_bytes_to_remove = 0 */

    rv = bcm_field_entry_add(unit, 0, etpp_fwd_term_tcam_fg_id, &ent_info, &etpp_fwd_term_ent_3_ipv4_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    ent_info.entry_qual[0].value[0] = 4; /* fwd_layer_index = 4 */
    ent_info.entry_action[1].type = fg_info.action_types[1];
    ent_info.entry_action[1].value[0] = 4; /* Egress FWD Index should be equal to fwd_layer_index */

    rv = bcm_field_entry_add(unit, 0, etpp_fwd_term_tcam_fg_id, &ent_info, &etpp_fwd_term_ent_4_ipv4_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    ent_info.entry_qual[1].value[0] = bcmFieldLayerTypeIp6;
    dnx_dbal_fields_enum_value_get(unit, "PARSING_START_TYPE_ENCODING", "IPV6_8B", &hw_value); /* DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_IPV6_8B */
    hw_value = hw_value << 1 | 1; /* The valid bit value is the LSB, and it's set to 1 */
    ent_info.entry_action[0].value[0] = hw_value;
    ent_info.entry_qual[2].mask[0] = 0; /* Ignore IPv4 UC check for IPv6 packets */
    ent_info.entry_qual[3].mask[0] = 0x1;

    rv = bcm_field_entry_add(unit, 0, etpp_fwd_term_tcam_fg_id, &ent_info, &etpp_fwd_term_ent_4_ipv6_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    ent_info.entry_qual[0].value[0] = 3; /* fwd_layer_index = 3 */
    ent_info.entry_action[1].value[0] = 3; /* Egress FWD Index should be equal to fwd_layer_index */

    rv = bcm_field_entry_add(unit, 0, etpp_fwd_term_tcam_fg_id, &ent_info, &etpp_fwd_term_ent_3_ipv6_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    /* Add default entry that enables the Invalid-Next to ensure no offset gets changed by DE
     * when none of the above entries gets hit */
    ent_info.nof_entry_quals = 0; /* No QUALS: Default entry */
    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = fg_info.action_types[2];
    ent_info.entry_action[0].value[0] = 1;

    rv = bcm_field_entry_add(unit, 0, etpp_fwd_term_tcam_fg_id, &ent_info, &etpp_fwd_term_ent_def_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    return rv;
}

int cint_field_etpp_fwd_term_min_bytes_destroy(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;

    rv = bcm_field_entry_delete(unit, etpp_fwd_term_tcam_fg_id, NULL, etpp_fwd_term_ent_3_ipv4_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }

    rv = bcm_field_entry_delete(unit, etpp_fwd_term_tcam_fg_id, NULL, etpp_fwd_term_ent_4_ipv4_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }

    rv = bcm_field_entry_delete(unit, etpp_fwd_term_tcam_fg_id, NULL, etpp_fwd_term_ent_3_ipv6_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }

    rv = bcm_field_entry_delete(unit, etpp_fwd_term_tcam_fg_id, NULL, etpp_fwd_term_ent_4_ipv6_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }

    rv = bcm_field_entry_delete(unit, etpp_fwd_term_tcam_fg_id, NULL, etpp_fwd_term_ent_def_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, etpp_fwd_term_tcam_fg_id, etpp_fwd_term_context_id_ipmf3_v4);
    if (rv != BCM_E_NONE)
    {
       printf("Error in bcm_field_group_context_detach\n");
       return rv;
    }

    rv = bcm_field_group_context_detach(unit, etpp_fwd_term_tcam_fg_id, etpp_fwd_term_context_id_ipmf3_v6);
    if (rv != BCM_E_NONE)
    {
       printf("Error in bcm_field_group_context_detach\n");
       return rv;
    }

    rv = bcm_field_group_delete(unit, etpp_fwd_term_tcam_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_delete\n");
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, etpp_fwd_term_de_fg_id, etpp_fwd_term_context_id_ipmf3_v4);
    if (rv != BCM_E_NONE)
    {
       printf("Error in bcm_field_group_context_detach\n");
       return rv;
    }

    rv = bcm_field_group_context_detach(unit, etpp_fwd_term_de_fg_id, etpp_fwd_term_context_id_ipmf3_v6);
    if (rv != BCM_E_NONE)
    {
       printf("Error in bcm_field_group_context_detach\n");
       return rv;
    }

    rv = bcm_field_group_delete(unit, etpp_fwd_term_de_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_delete\n");
        return rv;
    }

    rv = bcm_field_qualifier_destroy(unit, etpp_fwd_term_qual_const_one);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_destroy. \n");
        return rv;
    }

    rv = bcm_field_qualifier_destroy(unit, etpp_fwd_term_qual_layer_offset);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_destroy. \n");
        return rv;
    }

    rv = bcm_field_action_destroy(unit, etpp_fwd_term_parsing_start_offset_action_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_action_destroy. \n");
        return rv;
    }

    p_id.presel_id = etpp_fwd_term_presel_id_v4;
    p_id.stage = bcmFieldStageIngressPMF3;

    p_data.entry_valid = FALSE;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set \n", rv);
        return rv;
    }

    p_id.presel_id = etpp_fwd_term_presel_id_v6;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set \n", rv);
        return rv;
    }

    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF3, etpp_fwd_term_context_id_ipmf3_v4);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_context_destroy \n", rv);
       return rv;
    }

    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF3, etpp_fwd_term_context_id_ipmf3_v6);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_context_destroy \n", rv);
       return rv;
    }

    return rv;
}
