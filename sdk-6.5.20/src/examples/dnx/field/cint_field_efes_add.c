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
 * cint ../../../src/examples/dnx/field/cint_field_efes_add.c
 * cint;
 * int unit = 0;
 * int context = 20;
 * int context2 = 21;
 * cint_field_efes_add_diff_actions_diff_contexts_main(unit, context, context2);
 * OR
 * cint_field_efes_add_two_actions_same_data_tcam_main(unit, context);
 * OR
 * cint_field_efes_add_two_actions_same_data_de_main(unit, context);
 * OR
 * cint_field_efes_add_diff_actions_diff_entries_main(unit, context);
 * OR
 * cint_field_efes_add_de_three_bit_condition_main(unit, context);
 *
 * Configuration example end
 *
 * This CINT creates a Field group and adds EFES to it, according to 5 different scenarios:
 *
 *
 * ************************************************************************
 * The main functions:
 * cint_field_efes_add_diff_actions_diff_contexts_main(unit, context, context2);
 * cint_field_efes_add_two_actions_same_data_tcam_main(unit, context);
 * cint_field_efes_add_two_actions_same_data_de_main(unit, context);
 * cint_field_efes_add_diff_actions_diff_entries_main(unit, context);
 * cint_field_efes_add_de_three_bit_condition_main(unit, context);
 *
 * The destroy function:
 * cint_field_efes_add_destroy(unit,fg_type)
 * fg_type can be TCAM or DE.
 */
 /**
  * Global variables
  */
 /**
  * {
  */
/**
 * The field group ID
 */
bcm_field_group_t Cint_field_efes_add_fg_id = BCM_FIELD_ID_INVALID;
/**
 * User defined actions.
 */
bcm_field_action_t Cint_field_efes_add_uda_1 = BCM_FIELD_ID_INVALID;
bcm_field_action_t Cint_field_efes_add_uda_2 = BCM_FIELD_ID_INVALID;
bcm_field_action_t Cint_field_efes_add_uda_3 = BCM_FIELD_ID_INVALID;
/**
 * User defined qualifiers.
 */
bcm_field_qualify_t Cint_field_efes_add_udq_1 = BCM_FIELD_ID_INVALID;
bcm_field_qualify_t Cint_field_efes_add_udq_2 = BCM_FIELD_ID_INVALID;
bcm_field_qualify_t Cint_field_efes_add_udq_3 = BCM_FIELD_ID_INVALID;
/**
 * User defined actions.
 */
bcm_field_entry_t Cint_field_efes_add_entry_id = BCM_FIELD_ID_INVALID;
bcm_field_entry_t Cint_field_efes_add_entry_id_2 = BCM_FIELD_ID_INVALID;


/**
 * Function returns only a certain abount of lsb bits from uint32
 * \param [in] unit - The unit number.
 * \param [in] value - The input uint32.
 * \param [in] num_bits - The number of lsb bits to take
 *  \return
 *  int - Error Type, in sense of bcm_error_t.
 */
uint32
cint_field_efes_take_lsb_bits(
    uint32 value,
    int num_bits)
{
    if (num_bits >= 32)
    {
        return value;
    }
    else if (num_bits <= 0)
    {
        return 0;
    }
    else
    {
        return ((value << (32 - num_bits)) >> (32 - num_bits));
    }
}

/**
 * Configures a field group that performs different actions for different contexts.
 * \param [in] unit - The unit number.
 * \param [in] context1 - First context.
 * \param [in] context2 - Second context.
 *  \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int
cint_field_efes_add_diff_actions_diff_contexts_main(
    int unit,
    bcm_field_context_t context1,
    bcm_field_context_t context2)
{
    char *proc_name;
    bcm_field_stage_t stage = bcmFieldStageIngressPMF1;
    bcm_field_action_info_t action_info;
    bcm_field_group_info_t fg_info;
    bcm_field_entry_info_t entry_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_efes_action_info_t efes_action_info;
    bcm_field_action_t action_1 = bcmFieldActionOutVport2Raw;
    bcm_field_action_t action_2 = bcmFieldActionOutVport3Raw;
    bcm_field_action_priority_t action_priority;
    int action_size;
    int action_offset_within_fg;
    int ii;
    void *dest_char;
    int rv = BCM_E_NONE;
    proc_name = "cint_field_efes_diff_actions_diff_contexts_main";

    /*
     *  Get the size of the actions.
     */
    rv = bcm_field_action_info_get(unit, action_1, stage, &action_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_info_get\n", proc_name, rv);
        return rv;
    }
    action_size = action_info.size;
    rv = bcm_field_action_info_get(unit, action_2, stage, &action_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_info_get\n", proc_name, rv);
        return rv;
    }
    if (action_size != action_info.size)
    {
        printf("%s Error, The two actions are of different sizes (%d and %d).\n", proc_name, action_size, action_info.size);
        return rv;
    }

    /*
     *  Create a void user defined action, to have somewhere to write the lookup's payload to.
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.stage = stage;
    action_info.action_type = bcmFieldActionVoid;
    /* In this example, we increas the size by one to account for the valid bit (which is not mandatory)*/
    action_info.size = action_size + 1;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "EFES_UDA1", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &Cint_field_efes_add_uda_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    /*
     *  Create a field group.
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.stage = stage;
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "EFES_FG", sizeof(fg_info.name));
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifySrcMac;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = Cint_field_efes_add_uda_1;
    /* 
     * We still need to tell the field group that there is no valid bit because it is a void action, with no 
     * automatically generated EFES. We will implement the valid bit ourselves
     */
    fg_info.action_with_valid_bit[0] = FALSE;
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &Cint_field_efes_add_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    /* 
     * Add an entry
     */
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = 0x00000001;
    entry_info.entry_qual[0].mask[0] = 0xffffffff;
    entry_info.entry_qual[1].value[0] = 0x00000000;
    entry_info.entry_qual[1].mask[0] = 0x0000ffff;
    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    /** Since this is a void action, we need to write the valid bit, it will not be added automatically.*/
    entry_info.entry_action[0].value[0] = 1 | (5 << 1);    
    entry_info.priority = 9;
    rv = bcm_field_entry_add(unit, 0, Cint_field_efes_add_fg_id, &entry_info, &Cint_field_efes_add_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_add\n", proc_name, rv);
        return rv;
    }

    /*
     *  Attach the field group to both contexts.
     */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for(ii=0; ii< fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for(ii=0; ii<fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;
    rv = bcm_field_group_context_attach(unit, 0, Cint_field_efes_add_fg_id, context1, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
        return rv;
    }
    rv = bcm_field_group_context_attach(unit, 0, Cint_field_efes_add_fg_id, context2, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
        return rv;
    }

    /*
     * Find position of void action on the field group.
     * Add one to account for the valid bit that we placed inside the void action.
     */
    rv = bcm_field_group_action_offset_get(unit, 0, Cint_field_efes_add_fg_id, Cint_field_efes_add_uda_1, &action_offset_within_fg);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_action_offset_get\n", proc_name, rv);
        return rv;
    }
    action_offset_within_fg++;

    /* 
     * Add a separate action for each context.
     */
    bcm_field_efes_action_info_t_init(&efes_action_info);
    efes_action_info.is_condition_valid = FALSE;
    efes_action_info.efes_condition_conf[0].is_action_valid = TRUE;
    efes_action_info.efes_condition_conf[0].action_type = action_1;
    efes_action_info.efes_condition_conf[0].action_lsb = action_offset_within_fg;
    efes_action_info.efes_condition_conf[0].action_nof_bits = action_size;
    efes_action_info.efes_condition_conf[0].action_with_valid_bit = TRUE;
    action_priority = BCM_FIELD_ACTION_POSITION(0, 1);
    rv = bcm_field_efes_action_add(unit, 0, Cint_field_efes_add_fg_id, context1, &action_priority, &efes_action_info);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_efes_action_add\n", proc_name, rv);
        return rv;
    }
    efes_action_info.efes_condition_conf[0].action_type = action_2;
    action_priority = BCM_FIELD_ACTION_POSITION(0, 2);
    rv = bcm_field_efes_action_add(unit, 0, Cint_field_efes_add_fg_id, context2, &action_priority, &efes_action_info);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_efes_action_add\n", proc_name, rv);
        return rv;
    }
    
    return rv;
}

/**
 * Configures a field group that writes the same data to two actions.
 * \param [in] unit - The unit number.
 * \param [in] context - context.
 *  \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int
cint_field_efes_add_two_actions_same_data_tcam_main(
    int unit,
    bcm_field_context_t context)
{
    char *proc_name;
    bcm_field_stage_t stage = bcmFieldStageIngressPMF1;
    bcm_field_action_info_t action_info;
    bcm_field_group_info_t fg_info;
    bcm_field_entry_info_t entry_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_efes_action_info_t efes_action_info;
    bcm_field_action_t action_1 = bcmFieldActionOutVport2Raw;
    bcm_field_action_t action_2 = bcmFieldActionOutVport3Raw;
    bcm_field_action_priority_t action_priority;
    int action_size;
    int action_offset_within_fg;
    int ii;
    void *dest_char;
    int rv = BCM_E_NONE;
    proc_name = "cint_field_efes_diff_actions_diff_contexts_main";

    /*
     *  Get the size of the actions.
     */
    rv = bcm_field_action_info_get(unit, action_1, stage, &action_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_info_get\n", proc_name, rv);
        return rv;
    }
    action_size = action_info.size;
    rv = bcm_field_action_info_get(unit, action_2, stage, &action_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_info_get\n", proc_name, rv);
        return rv;
    }
    if (action_size != action_info.size)
    {
        printf("%s Error, The two actions are of different sizes (%d and %d).\n", proc_name, action_size, action_info.size);
        return rv;
    }

    /*
     *  Create a field group.
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.stage = stage;
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "EFES_FG", sizeof(fg_info.name));
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifySrcMac;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = action_1;
    /* 
     * The action by default has a valid bit:
     * fg_info.action_with_valid_bit[0] = TRUE;
     */
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &Cint_field_efes_add_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    /* 
     * Add an entry
     */
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = 0x00000001;
    entry_info.entry_qual[0].mask[0] = 0xffffffff;
    entry_info.entry_qual[1].value[0] = 0x00000000;
    entry_info.entry_qual[1].mask[0] = 0x0000ffff;
    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    /* We don't need to write the valid bit here.*/
    entry_info.entry_action[0].value[0] = 5;    
    entry_info.priority = 9;
    rv = bcm_field_entry_add(unit, 0, Cint_field_efes_add_fg_id, &entry_info, &Cint_field_efes_add_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_add\n", proc_name, rv);
        return rv;
    }    

    /*
     *  Attach the field group to context.
     */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for(ii=0; ii< fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for(ii=0; ii<fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;
    rv = bcm_field_group_context_attach(unit, 0, Cint_field_efes_add_fg_id, context, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
        return rv;
    }

    /*
     * Find position of action on the field group.
     */
    rv = bcm_field_group_action_offset_get(unit, 0, Cint_field_efes_add_fg_id, action_1, &action_offset_within_fg);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_action_offset_get\n", proc_name, rv);
        return rv;
    }

    /* 
     * Add an action.
     */
    bcm_field_efes_action_info_t_init(&efes_action_info);
    efes_action_info.is_condition_valid = FALSE;
    efes_action_info.efes_condition_conf[0].is_action_valid = TRUE;
    efes_action_info.efes_condition_conf[0].action_type = action_2;
    efes_action_info.efes_condition_conf[0].action_lsb = action_offset_within_fg;
    efes_action_info.efes_condition_conf[0].action_nof_bits = action_size;
    efes_action_info.efes_condition_conf[0].action_with_valid_bit = TRUE;
    action_priority = BCM_FIELD_ACTION_POSITION(0, 1);
    rv = bcm_field_efes_action_add(unit, 0, Cint_field_efes_add_fg_id, context, &action_priority, &efes_action_info);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_efes_action_add\n", proc_name, rv);
        return rv;
    }
    
    return rv;
}


/**
 * Configures a field group that uses the same data for two actions, specificayy reuse the valid bit at teh end of
 * bcmFieldActionStatProfile0 with the one at the beginning of bcmFieldActionLatencyFlowId 
 * (the valid bit internal to the action, not the EFES's valid bit).
 * \param [in] unit - The unit number.
 * \param [in] context - context.
 *  \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int
cint_field_efes_add_two_actions_same_data_de_main(
    int unit,
    bcm_field_context_t context)
{
    char *proc_name;
    bcm_field_stage_t stage = bcmFieldStageIngressPMF2;
    bcm_field_action_info_t action_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_group_info_t fg_info;
    bcm_field_entry_info_t entry_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_efes_action_info_t efes_action_info;
    bcm_field_action_t action_1 = bcmFieldActionStatProfile0Raw;
    bcm_field_action_t action_2 = bcmFieldActionLatencyFlowIdRaw;
    bcm_field_action_priority_t action_priority;
    int action_size_1;
    int action_size_2;
    int action_offset_within_fg_1;
    int action_offset_within_fg_2;
    int ii;
    void *dest_char;
    int rv = BCM_E_NONE;
    proc_name = "cint_field_efes_diff_actions_diff_contexts_main";

    /*
     *  Get the size of the actions.
     */
    rv = bcm_field_action_info_get(unit, action_1, stage, &action_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_info_get\n", proc_name, rv);
        return rv;
    }
    action_size_1 = action_info.size;
    rv = bcm_field_action_info_get(unit, action_2, stage, &action_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_info_get\n", proc_name, rv);
        return rv;
    }
    action_size_2 = action_info.size;

    /*
     *  Create void user defined actions and user defined qualifiers, for the bit shared and for the rest fo the actions.
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.stage = stage;
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = action_size_1 - 1;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "EFES_UDA1", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &Cint_field_efes_add_uda_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }
    bcm_field_action_info_t_init(&action_info);
    action_info.stage = stage;
    action_info.action_type = bcmFieldActionVoid;
    action_info.size =  1;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "EFES_UDA2", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &Cint_field_efes_add_uda_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }
    bcm_field_action_info_t_init(&action_info);
    action_info.stage = stage;
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = action_size_2 - 1;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "EFES_UDA3", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &Cint_field_efes_add_uda_3);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = action_size_1 - 1;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "EFES_UDQ1", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &Cint_field_efes_add_udq_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_qualifier_create\n", proc_name, rv);
        return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "EFES_UDQ2", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &Cint_field_efes_add_udq_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_qualifier_create\n", proc_name, rv);
        return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = action_size_2 - 1;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "EFES_UDQ3", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &Cint_field_efes_add_udq_3);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_qualifier_create\n", proc_name, rv);
        return rv;
    }

    /*
     *  Create a field group.
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.stage = stage;
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "EFES_FG", sizeof(fg_info.name));
    fg_info.nof_quals = 3;
    fg_info.qual_types[0] = Cint_field_efes_add_udq_1;
    fg_info.qual_types[1] = Cint_field_efes_add_udq_2;
    fg_info.qual_types[2] = Cint_field_efes_add_udq_3;
    fg_info.nof_actions = 3;
    fg_info.action_types[0] = Cint_field_efes_add_uda_1;
    fg_info.action_types[1] = Cint_field_efes_add_uda_2;
    fg_info.action_types[2] = Cint_field_efes_add_uda_3;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_with_valid_bit[2] = FALSE;
    rv = bcm_field_group_add(unit, 0, &fg_info, &Cint_field_efes_add_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    /*
     *  Attach the field group.
     */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for(ii=0; ii< fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for(ii=0; ii<fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[0].input_arg = 5;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[1].input_arg = 1;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[2].input_arg = 6;
    rv = bcm_field_group_context_attach(unit, 0, Cint_field_efes_add_fg_id, context, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
        return rv;
    }

    /*
     * Find position of void actions on the field group.
     */
    rv = bcm_field_group_action_offset_get(unit, 0, Cint_field_efes_add_fg_id, Cint_field_efes_add_uda_1, &action_offset_within_fg_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_action_offset_get\n", proc_name, rv);
        return rv;
    }
    rv = bcm_field_group_action_offset_get(unit, 0, Cint_field_efes_add_fg_id, Cint_field_efes_add_uda_2, &action_offset_within_fg_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_action_offset_get\n", proc_name, rv);
        return rv;
    }

    /* 
     * Add two actions.
     */
    bcm_field_efes_action_info_t_init(&efes_action_info);
    efes_action_info.is_condition_valid = FALSE;
    efes_action_info.efes_condition_conf[0].is_action_valid = TRUE;
    efes_action_info.efes_condition_conf[0].action_type = action_1;
    efes_action_info.efes_condition_conf[0].action_lsb = action_offset_within_fg_1;
    efes_action_info.efes_condition_conf[0].action_nof_bits = action_size_1;
    efes_action_info.efes_condition_conf[0].action_with_valid_bit = FALSE;
    action_priority = BCM_FIELD_ACTION_POSITION(0, 1);
    rv = bcm_field_efes_action_add(unit, 0, Cint_field_efes_add_fg_id, context, &action_priority, &efes_action_info);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_efes_action_add\n", proc_name, rv);
        return rv;
    }
    efes_action_info.efes_condition_conf[0].action_type = action_2;
    efes_action_info.efes_condition_conf[0].action_lsb = action_offset_within_fg_2;
    efes_action_info.efes_condition_conf[0].action_nof_bits = action_size_2;
    action_priority = BCM_FIELD_ACTION_POSITION(0, 2);
    rv = bcm_field_efes_action_add(unit, 0, Cint_field_efes_add_fg_id, context, &action_priority, &efes_action_info);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_efes_action_add\n", proc_name, rv);
        return rv;
    }
    
    return rv;
}

/**
 * Configures a field group that performs different actions for different entries, as a kind of result type.
 * one option writes two 22 bit actions (46 bits with valid bits), and the other an 32 bit action (33 with valid bit).
 * \param [in] unit - The unit number.
 * \param [in] context - context.
 *  \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int
cint_field_efes_add_diff_actions_diff_entries_main(
    int unit,
    bcm_field_context_t context)
{
    char *proc_name;
    bcm_field_stage_t stage = bcmFieldStageIngressPMF1;
    bcm_field_action_info_t action_info;
    bcm_field_group_info_t fg_info;
    bcm_field_entry_info_t entry_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_efes_action_info_t efes_action_info;
    bcm_field_action_t action_1_1 = bcmFieldActionOutVport2Raw;
    bcm_field_action_t action_1_2 = bcmFieldActionOutVport3Raw;
    bcm_field_action_t action_2_1 = bcmFieldActionUDHData2;
    bcm_field_action_priority_t action_priority;
    int action_size_1_1;
    int action_size_1_2;
    int action_size_2_1;
    int total_action_size_1;
    int total_action_size_2;
    int total_action_size_overall;
    int action_size_uda_1;
    int action_size_uda_2;
    int condition_value_1 = 0;
    int condition_value_2 = 1;
    unsigned int action_value_1_1 = 0x5;
    unsigned int action_value_1_2 = 0x5678;
    unsigned int action_value_2_1 = 0xFFFFFFFF;
    int nof_bits_of_action_on_uda1;
    int nof_bits_of_action_2_on_uda1;
    int first_action_offset_on_fg;
    int condition_offset_on_fg;
    int ii;
    void *dest_char;
    int rv = BCM_E_NONE;
    proc_name = "cint_field_efes_diff_actions_diff_contexts_main";

    /*
     *  Get the size of the actions.
     */
    rv = bcm_field_action_info_get(unit, action_1_1, stage, &action_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_info_get\n", proc_name, rv);
        return rv;
    }
    action_size_1_1 = action_info.size;
    rv = bcm_field_action_info_get(unit, action_1_2, stage, &action_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_info_get\n", proc_name, rv);
        return rv;
    }
    action_size_1_2 = action_info.size;
    rv = bcm_field_action_info_get(unit, action_2_1, stage, &action_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_info_get\n", proc_name, rv);
        return rv;
    }
    action_size_2_1 = action_info.size;

    total_action_size_1 = action_size_1_1 +1 + action_size_1_2 + 1;
    total_action_size_2 = action_size_2_1 + 1;
    total_action_size_overall = (total_action_size_1 > total_action_size_2) ? (total_action_size_1) : (total_action_size_2);
    action_size_uda_1 = 32;
    action_size_uda_2 = total_action_size_overall - action_size_uda_1;
    
    /*
     *  Create 2 void user defined actions for the action info (as we have 46 bits and maximum size for action is 32)
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.stage = stage;
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = action_size_uda_1;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "EFES_UDA1", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &Cint_field_efes_add_uda_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }
    bcm_field_action_info_t_init(&action_info);
    action_info.stage = stage;
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = action_size_uda_2;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "EFES_UDA2", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &Cint_field_efes_add_uda_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }
    /*
     *  Create another void user defined action for the condition bits.
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.stage = stage;
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = 2;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "EFES_UDA3", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &Cint_field_efes_add_uda_3);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    /*
     *  Create a field group.
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.stage = stage;
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "EFES_FG", sizeof(fg_info.name));
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifySrcMac;
    fg_info.nof_actions = 3;
    fg_info.action_types[0] = Cint_field_efes_add_uda_1;
    fg_info.action_types[1] = Cint_field_efes_add_uda_2;
    fg_info.action_types[2] = Cint_field_efes_add_uda_3;
    /* Void actions cannot have valid bits, we implement them ourselves.*/
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_with_valid_bit[2] = FALSE;
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &Cint_field_efes_add_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    /* 
     * Add an entry with one condition.
     */
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = 0x00000001;
    entry_info.entry_qual[0].mask[0] = 0xffffffff;
    entry_info.entry_qual[1].value[0] = 0x00000000;
    entry_info.entry_qual[1].mask[0] = 0x0000ffff;
    entry_info.nof_entry_actions = fg_info.nof_actions;
    /* We write valid bits and action values.*/
    nof_bits_of_action_2_on_uda1 = action_size_uda_1-action_size_1_1-2;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = 1 | (action_value_1_1 << 1) | (1<<(action_size_1_1+1)) | (cint_field_efes_take_lsb_bits(action_value_1_2,nof_bits_of_action_2_on_uda1)<<(action_size_1_1+2)); 
    entry_info.entry_action[1].type = fg_info.action_types[1];
    entry_info.entry_action[1].value[0] = action_value_1_2 >> nof_bits_of_action_2_on_uda1;
    entry_info.entry_action[2].type = fg_info.action_types[2];
    entry_info.entry_action[2].value[0] = condition_value_1;
    entry_info.priority = 9;
    entry_info.priority = 9;
    rv = bcm_field_entry_add(unit, 0, Cint_field_efes_add_fg_id, &entry_info, &Cint_field_efes_add_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_add\n", proc_name, rv);
        return rv;
    }
    /* 
     * Add an entry with another condition.
     */
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = 0x00000003;
    entry_info.entry_qual[0].mask[0] = 0xffffffff;
    entry_info.entry_qual[1].value[0] = 0x00000000;
    entry_info.entry_qual[1].mask[0] = 0x0000ffff;
    entry_info.nof_entry_actions = fg_info.nof_actions;
    /* We write valid bits and action value.*/
    nof_bits_of_action_on_uda1 = action_size_uda_1 - 1;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = 1 | (cint_field_efes_take_lsb_bits(action_value_2_1, nof_bits_of_action_on_uda1)<<1);
    entry_info.entry_action[1].type = fg_info.action_types[1];
    entry_info.entry_action[1].value[0] = action_value_2_1 >> nof_bits_of_action_on_uda1; 
    entry_info.entry_action[2].type = fg_info.action_types[2];
    entry_info.entry_action[2].value[0] = condition_value_2; 
    entry_info.priority = 9;
    rv = bcm_field_entry_add(unit, 0, Cint_field_efes_add_fg_id, &entry_info, &Cint_field_efes_add_entry_id_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_add\n", proc_name, rv);
        return rv;
    }

    /*
     *  Attach the field group to context.
     */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for(ii=0; ii< fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for(ii=0; ii<fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;
    rv = bcm_field_group_context_attach(unit, 0, Cint_field_efes_add_fg_id, context, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
        return rv;
    }

    /*
     * Find position of first void action and condition bits on the field group.
     */
    rv = bcm_field_group_action_offset_get(unit, 0, Cint_field_efes_add_fg_id, Cint_field_efes_add_uda_1, &first_action_offset_on_fg);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_action_offset_get\n", proc_name, rv);
        return rv;
    }
    rv = bcm_field_group_action_offset_get(unit, 0, Cint_field_efes_add_fg_id, Cint_field_efes_add_uda_3, &condition_offset_on_fg);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_action_offset_get\n", proc_name, rv);
        return rv;
    }

    /* 
     * Add actions.
     */
    bcm_field_efes_action_info_t_init(&efes_action_info);
    efes_action_info.is_condition_valid = TRUE;
    /* Note that the condition bits offset we provide is the MSB of the two bits, so we add +1.*/
    efes_action_info.condition_msb = condition_offset_on_fg + 1;
    /* */
    efes_action_info.efes_condition_conf[condition_value_1].is_action_valid = TRUE;
    efes_action_info.efes_condition_conf[condition_value_1].action_type = action_1_1;
    /* We add +1 for valid bit.*/
    efes_action_info.efes_condition_conf[condition_value_1].action_lsb = first_action_offset_on_fg + 1;
    efes_action_info.efes_condition_conf[condition_value_1].action_nof_bits = action_size_1_1;
    efes_action_info.efes_condition_conf[condition_value_1].action_with_valid_bit = TRUE;
    efes_action_info.efes_condition_conf[condition_value_2].is_action_valid = TRUE;
    efes_action_info.efes_condition_conf[condition_value_2].action_type = action_2_1;
    /* We add +1 for valid bit.*/
    efes_action_info.efes_condition_conf[condition_value_2].action_lsb = first_action_offset_on_fg + 1;
    efes_action_info.efes_condition_conf[condition_value_2].action_nof_bits = action_size_2_1;
    efes_action_info.efes_condition_conf[condition_value_2].action_with_valid_bit = TRUE;
    action_priority = BCM_FIELD_ACTION_POSITION(0, 1);
    rv = bcm_field_efes_action_add(unit, 0, Cint_field_efes_add_fg_id, context, &action_priority, &efes_action_info);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_efes_action_add\n", proc_name, rv);
        return rv;
    }
    bcm_field_efes_action_info_t_init(&efes_action_info);
    efes_action_info.is_condition_valid = TRUE;
    /* Note that the condition bits offset we provide is the MSB of the two bits, so we add +1.*/
    efes_action_info.condition_msb = condition_offset_on_fg + 1;
    efes_action_info.efes_condition_conf[condition_value_1].is_action_valid = TRUE;
    efes_action_info.efes_condition_conf[condition_value_1].action_type = action_1_2;
    efes_action_info.efes_condition_conf[condition_value_1].action_lsb = first_action_offset_on_fg + action_size_1_1 + 2;
    efes_action_info.efes_condition_conf[condition_value_1].action_nof_bits = action_size_1_2;
    efes_action_info.efes_condition_conf[condition_value_1].action_with_valid_bit = TRUE;
    action_priority = BCM_FIELD_ACTION_POSITION(0, 2);
    rv = bcm_field_efes_action_add(unit, 0, Cint_field_efes_add_fg_id, context, &action_priority, &efes_action_info);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_efes_action_add\n", proc_name, rv);
        return rv;
    }
    
    return rv;
}

/**
 * Configures a field group that performs an action according to the TCP flags, if either the FIN or SYN flags are set
 * (but not both), and if the RST flag is not set. This is to be done using direct extraction and without FEM.
 * The SYN and FIN flags are used in the condition bits, the RST flag is used in the valid bit with polarity 0.
 * \param [in] unit - The unit number.
 * \param [in] context - context.
 *  \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int
cint_field_efes_add_de_three_bit_condition_main(
    int unit,
    bcm_field_context_t context)
{
    char *proc_name;
    bcm_field_stage_t stage = bcmFieldStageIngressPMF2;
    bcm_field_action_info_t action_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_group_info_t fg_info;
    bcm_field_entry_info_t entry_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_efes_action_info_t efes_action_info;
    bcm_field_qualifier_info_get_t qual_info_get;
    bcm_field_action_t action = bcmFieldActionUDHData2;
    bcm_field_action_priority_t action_priority;
    int action_size;
    int action_offset_within_fg_action_bits;
    int action_offset_within_fg_condition_bits;
    int tcp_flags_offset;
    int tcp_flags_syn_fin_offset;
    int tcp_flags_rst_offset;
    int ii;
    void *dest_char;
    int rv = BCM_E_NONE;
    proc_name = "cint_field_efes_diff_actions_diff_contexts_main";

    /*
     *  Get the size of the action.
     */
    rv = bcm_field_action_info_get(unit, action, stage, &action_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_info_get\n", proc_name, rv);
        return rv;
    }
    action_size = action_info.size;

    /*
     *  Create void user defined action and user defined qualifiers
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.stage = stage;
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = 1;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "A_EFES_VALID_BIT", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &Cint_field_efes_add_uda_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }
    bcm_field_action_info_t_init(&action_info);
    action_info.stage = stage;
    action_info.action_type = bcmFieldActionVoid;
    action_info.size =  action_size;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "A_EFES_ACTION", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &Cint_field_efes_add_uda_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }
    bcm_field_action_info_t_init(&action_info);
    action_info.stage = stage;
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = 2;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "A_EFES_CONDITION", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &Cint_field_efes_add_uda_3);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "Q_EFES_VALID_BIT", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &Cint_field_efes_add_udq_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_qualifier_create\n", proc_name, rv);
        return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = action_size;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "Q_EFES_ACTION", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &Cint_field_efes_add_udq_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_qualifier_create\n", proc_name, rv);
        return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 2;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "Q_EFES_CONDITION", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &Cint_field_efes_add_udq_3);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_qualifier_create\n", proc_name, rv);
        return rv;
    }

    /*
     *  Create a field group.
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.stage = stage;
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "EFES_FG", sizeof(fg_info.name));
    fg_info.nof_quals = 3;
    fg_info.qual_types[0] = Cint_field_efes_add_udq_1;
    fg_info.qual_types[1] = Cint_field_efes_add_udq_2;
    fg_info.qual_types[2] = Cint_field_efes_add_udq_3;
    fg_info.nof_actions = 3;
    fg_info.action_types[0] = Cint_field_efes_add_uda_1;
    fg_info.action_types[1] = Cint_field_efes_add_uda_2;
    fg_info.action_types[2] = Cint_field_efes_add_uda_3;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_with_valid_bit[2] = FALSE;
    rv = bcm_field_group_add(unit, 0, &fg_info, &Cint_field_efes_add_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    /*
     *  Get the offset of the TCP flags. 
     * Note that bcmFieldQualifyTcpControl is the 6 LSB flags, and that the offset for header qualifiers 
     * is from MSB to LSB, unlike metadata or layer recod qualifiers.
     */
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyTcpControl, stage, &qual_info_get);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_qualifier_info_get\n", proc_name, rv);
        return rv;
    }
    tcp_flags_offset = qual_info_get.offset;
    tcp_flags_syn_fin_offset = tcp_flags_offset + 4;
    tcp_flags_rst_offset = tcp_flags_offset + 3;
    

    /*
     *  Attach the field group.
     */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for(ii=0; ii< fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for(ii=0; ii<fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 2;
    attach_info.key_info.qual_info[0].offset = tcp_flags_rst_offset;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[1].input_arg = 17;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[2].input_arg = 2;
    attach_info.key_info.qual_info[2].offset = tcp_flags_syn_fin_offset;
    /* For direct extraction with EFES condition, we use the BCM_FIELD_FLAG_32_RESULT_MSB_ALIGN flag.*/
    rv = bcm_field_group_context_attach(unit, BCM_FIELD_FLAG_32_RESULT_MSB_ALIGN, Cint_field_efes_add_fg_id, context, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
        return rv;
    }

    /*
     * Find position of void actions on the field group.
     */
    rv = bcm_field_group_action_offset_get(unit, 0, Cint_field_efes_add_fg_id, Cint_field_efes_add_uda_2, &action_offset_within_fg_action_bits);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_action_offset_get\n", proc_name, rv);
        return rv;
    }
    rv = bcm_field_group_action_offset_get(unit, 0, Cint_field_efes_add_fg_id, Cint_field_efes_add_uda_3, &action_offset_within_fg_condition_bits);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_action_offset_get\n", proc_name, rv);
        return rv;
    }

    /* 
     * Add the action.
     */
    bcm_field_efes_action_info_t_init(&efes_action_info);
    efes_action_info.is_condition_valid = TRUE;
    efes_action_info.condition_msb = action_offset_within_fg_condition_bits + 1;
    /* We want either or the two bits to be 1, so we use combinations 1 and 2.*/
    efes_action_info.efes_condition_conf[1].is_action_valid = TRUE;
    efes_action_info.efes_condition_conf[1].action_type = action;
    efes_action_info.efes_condition_conf[1].action_lsb = action_offset_within_fg_action_bits;
    efes_action_info.efes_condition_conf[1].action_nof_bits = action_size;
    efes_action_info.efes_condition_conf[1].action_with_valid_bit = TRUE;
    /* We want RST flag to be 0, so we use polarity 0.*/
    efes_action_info.efes_condition_conf[1].valid_bit_polarity = 0;
    
    efes_action_info.efes_condition_conf[2].is_action_valid = TRUE;
    efes_action_info.efes_condition_conf[2].action_type = action;
    efes_action_info.efes_condition_conf[2].action_lsb = action_offset_within_fg_action_bits;
    efes_action_info.efes_condition_conf[2].action_nof_bits = action_size;
    efes_action_info.efes_condition_conf[2].action_with_valid_bit = TRUE;
    /* We want RST flag to be 0, so we use polarity 0.*/
    efes_action_info.efes_condition_conf[2].valid_bit_polarity = 0;
    
    action_priority = BCM_FIELD_ACTION_POSITION(0, 1);
    rv = bcm_field_efes_action_add(unit, 0, Cint_field_efes_add_fg_id, context, &action_priority, &efes_action_info);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_efes_action_add\n", proc_name, rv);
        return rv;
    }
    return rv;
}


/** Destroys all allocated data from the EFES configuration.
 *
 * \param [in] unit - The unit number.
 * \param [in] context1 - First context.
 * \param [in] context2 - Second context. If equals BCM_FIELD_ID_INVALID, taken to mean there is only one context. 
 *  \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int
cint_field_efes_add_destroy(
    int unit,
    bcm_field_context_t context1,
    bcm_field_context_t context2)
{
    char *proc_name;
    int rv;
    rv = BCM_E_NONE;
    proc_name = "cint_field_efes_add_destroy";

    if (Cint_field_efes_add_entry_id != BCM_FIELD_ID_INVALID)
    {
        rv = bcm_field_entry_delete(unit, Cint_field_efes_add_fg_id, NULL, Cint_field_efes_add_entry_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_entry_delete\n", rv);
            return rv;
        }
        Cint_field_efes_add_entry_id = BCM_FIELD_ID_INVALID;
    }
    if (Cint_field_efes_add_entry_id_2 != BCM_FIELD_ID_INVALID)
    {
        rv = bcm_field_entry_delete(unit, Cint_field_efes_add_fg_id, NULL, Cint_field_efes_add_entry_id_2);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_entry_delete\n", rv);
            return rv;
        }
        Cint_field_efes_add_entry_id_2 = BCM_FIELD_ID_INVALID;
    }

    rv = bcm_field_group_context_detach(unit, Cint_field_efes_add_fg_id, context1);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }
    if (context2 != BCM_FIELD_ID_INVALID)
    {
        rv = bcm_field_group_context_detach(unit, Cint_field_efes_add_fg_id, context2);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_group_context_detach\n");
            return rv;
        }
    }

    rv = bcm_field_group_delete(unit, Cint_field_efes_add_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_delete\n");
        return rv;
    }
    Cint_field_efes_add_fg_id = BCM_FIELD_ID_INVALID;

    if (Cint_field_efes_add_uda_1 != BCM_FIELD_ID_INVALID)
    {
        rv = bcm_field_action_destroy(unit, Cint_field_efes_add_uda_1);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_action_destroy dest_action\n");
            return rv;
        }
        Cint_field_efes_add_uda_1 = BCM_FIELD_ID_INVALID;
    }

    if (Cint_field_efes_add_uda_2 != BCM_FIELD_ID_INVALID)
    {
        rv = bcm_field_action_destroy(unit, Cint_field_efes_add_uda_2);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_action_destroy dest_action\n");
            return rv;
        }
        Cint_field_efes_add_uda_2 = BCM_FIELD_ID_INVALID;
    }

    if (Cint_field_efes_add_uda_3 != BCM_FIELD_ID_INVALID)
    {
        rv = bcm_field_action_destroy(unit, Cint_field_efes_add_uda_3);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_action_destroy dest_action\n");
            return rv;
        }
        Cint_field_efes_add_uda_3 = BCM_FIELD_ID_INVALID;
    }

    if (Cint_field_efes_add_udq_1 != BCM_FIELD_ID_INVALID)
    {
        rv = bcm_field_qualifier_destroy(unit, Cint_field_efes_add_udq_1);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_qualifier_destroy \n");
            return rv;
        }
        Cint_field_efes_add_udq_1 = BCM_FIELD_ID_INVALID;
    }

    if (Cint_field_efes_add_udq_2 != BCM_FIELD_ID_INVALID)
    {
        rv = bcm_field_qualifier_destroy(unit, Cint_field_efes_add_udq_2);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_qualifier_destroy \n");
            return rv;
        }
        Cint_field_efes_add_udq_2 = BCM_FIELD_ID_INVALID;
    }

    if (Cint_field_efes_add_udq_3 != BCM_FIELD_ID_INVALID)
    {
        rv = bcm_field_qualifier_destroy(unit, Cint_field_efes_add_udq_3);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_qualifier_destroy \n");
            return rv;
        }
        Cint_field_efes_add_udq_3 = BCM_FIELD_ID_INVALID;
    }

    return rv;
}
