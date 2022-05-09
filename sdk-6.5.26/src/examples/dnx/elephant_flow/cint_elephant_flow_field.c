/*
 * Configuration:
 * 
 *
   cint ../../../src/examples/dnx/elephant_flow/cint_elephant_flow_field.c
   cint ../../../../src/examples/dnx/elephant_flow/cint_elephant_flow_field.c
   cint
   int unit = 0;
   int snoop_cmd = 2;
   int snoop_str= 4;
   int age_profile_id = 1;
   cint_field_ele_flow_main(unit,bcmFieldLayerTypeEth,snoop_cmd,snoop_str,age_profile_id);
For Router setup: 
   cint_field_ele_flow_main(unit,bcmFieldLayerTypeIp4,snoop_cmd,snoop_str,age_profile_id);
 *
 * Show an example of Elephant flow configuration
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */


bcm_field_group_t   cint_field_ele_flow_exem_fg_id = 0;


bcm_field_action_t snoop_act_id;
bcm_field_action_t age_profile_act_id;
bcm_field_action_t state_act_id;
bcm_field_action_t vmv_pad_act_id;
bcm_field_action_t zero_padding_to_sizeof_learn_action;

int cint_field_ele_flow_flush_entry_id_mouse_to_elephant = 0;
int cint_field_ele_flow_flush_entry_id_elephant_to_mouse = 1;
int cint_field_ele_flow_flush_entry_id_elephant_reported_to_mouse = 2;

int fem_array_id[2] = {3, 3};
int fem_position[2] = {7, 6};

int STATE_SIZE = 2;
int SNOOP_CODE_SIZE = 9;
int SNOOP_STR_SIZE = 3;
int AGE_PROFILE_SIZE = 3;

int VMV_PADDING_SIZE = 4 - AGE_PROFILE_SIZE;

int ENTRY_SIZE_WITH_PADDING = 32;
int ENTRY_SIZE_WITHOUT_PADDING = STATE_SIZE + SNOOP_CODE_SIZE + SNOOP_STR_SIZE + VMV_PADDING_SIZE + AGE_PROFILE_SIZE;
int ENTRY_PADDING_SIZE = ENTRY_SIZE_WITH_PADDING - ENTRY_SIZE_WITHOUT_PADDING;


/*Should Encode:
 * State: Mouse/ Elephant/ Elephant reported  (0 should be reserved for NONE)
 * Snoop action
 * */
int  learn_action_value_mouse = 0;
int  learn_action_value_elephant_reported = 0;

bcm_field_action_t  trace_action;
bcm_field_action_t  cint_field_ele_flow_invalidate_trace_action;


bcm_field_context_t cint_field_ele_flow_ctx_id = 0;
bcm_field_presel_t cint_field_ele_flow_presel_id = 17;

int EL_STATE_NONE=                   0;
int EL_STATE_MOUSE=                  1;
int EL_STATE_ELEPHANT=               2;
int EL_STATE_ELEPHANT_REPORTED=      3;

/*EXEM payload is 60b size and we want to write to 32 MSB*/
int EXEM_32MSB_SHIFT=  (60-32);


char * cint_field_layer_name_get(bcm_field_layer_type_t fwd_layer)
{
    switch (fwd_layer)
    case bcmFieldLayerTypeEth:
    {
        return  "Ethernet";
        break;
    }
    case bcmFieldLayerTypeIp4:
    {
        return "Ipv4";
        break;
    }
    case bcmFieldLayerTypeIp6:
    {
        return "Ipv6";
        break;
    }
    default:
    {
        return "Invalid";
        break;
    }

    return "Invalid";
}


int cint_field_ele_flow_init_cb(int unit)
{
    int rv = BCM_E_NONE;
    /*Verify that pmf_sexem3_stage=IPMF2 is set*/

    return rv;
}



/*Get the offset of the action in EXEM FG payload, used to configure the actions*/
int cint_field_ele_flow_learn_get_actions_offsets_with_32b_of_learn_payload(int unit,int *state_action_offset, int *snoop_action_offset, int *age_action_offset)
{
    int rv = BCM_E_NONE;

    if(age_action_offset !=NULL)
    {
        *age_action_offset = 0;
    }
    if(snoop_action_offset != NULL)
    {
        *snoop_action_offset = AGE_PROFILE_SIZE + VMV_PADDING_SIZE;
    }
    if(state_action_offset != NULL)
    {
        *state_action_offset = AGE_PROFILE_SIZE + VMV_PADDING_SIZE + SNOOP_STR_SIZE + SNOOP_CODE_SIZE;
    }

    return rv;
}

int cint_field_ele_flow_get_flush_profile_offset(int unit , int *age_action_offset)
{
    int rv = BCM_E_NONE;
    rv = cint_field_ele_flow_learn_get_actions_offsets_with_32b_of_learn_payload(unit, NULL, NULL, age_action_offset);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_group_action_offset_get\n", rv);
       return rv;
    }

   return rv;

}



/**
* \brief
*  Configures how should learning payload look like
* \param [in] unit        -  Device id
* \param [in] state        -  State can be EL_STATE...
* \param [in] snoop_str        -  Snoop strength
* \param [in] snoop_cmd        -  Snoop cmd (0..32)
* \param [in] age_profile        - age profile created in api bcm_field_flush_profile_create()
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_ele_flow_learn_payload_set(int unit, int state ,int snoop_str, int snoop_cmd , int age_profile, int *learn_action_value)
{
    int state_action_offset;
    int snoop_action_offset;
    int age_action_offset=0;
    int rv = BCM_E_NONE;

    rv = cint_field_ele_flow_learn_get_actions_offsets_with_32b_of_learn_payload(unit, &state_action_offset , &snoop_action_offset , &age_action_offset);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_action_offset_get\n", rv);
        return rv;
    }

    /*Codes 480-511 has 1:1 mapping to the snoop command*/
    int snoop_act = (snoop_cmd+480) | (snoop_str << SNOOP_CODE_SIZE);

    *learn_action_value = (state << state_action_offset)| (snoop_act << snoop_action_offset) | (age_profile << age_action_offset);


    return rv;
}
/**
* \brief
*  Configures:
*  1) Hash - 5 tupple of IPv4 or IPv6, assuemd that packet type is ETH | IPV4/IPv6 | TCP/UDP
*  2) Create Context that Hash + EXEM field group (in iPMF2) will be configured on
*  3) context selection based on Trace Packet
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_ele_flow_ipmf1_set(int unit,bcm_field_layer_type_t fwd_layer)
{

    bcm_field_group_info_t fg_info;

    bcm_field_context_hash_info_t hash_info;
    bcm_field_context_info_t context_info;
    bcm_field_context_param_info_t param_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    int fwd_arg = 0;
    void *dest_char;
    int rv = BCM_E_NONE;



    printf("Creating new context for the hashing\n");
    bcm_field_context_info_t_init(&context_info);
    context_info.hashing_enabled = TRUE;
    dest_char = (&context_info.name[0]);
    sal_strncpy_s(dest_char, "Elephant Flow", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &cint_field_ele_flow_ctx_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    /**
     * Enable Flush Learning, by setting the contexts param_type to be bcmFieldContextParamTypeFlushLearnEnable
     */
    bcm_field_context_param_info_t_init(&param_info);
    param_info.param_type = bcmFieldContextParamTypeFlushLearnEnable;
    param_info.param_val = TRUE;
    rv = bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, cint_field_ele_flow_ctx_id, &param_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_param_set\n", rv);
        return rv;
    }

    printf("Working context for elephant flow in iPMF1 and 2 is (%d)!\n" , cint_field_ele_flow_ctx_id);

    /*Create the Hash using 5 tuple of the packet */
    bcm_field_context_hash_info_t_init(&hash_info);
    hash_info.hash_function = bcmFieldContextHashFunctionCrc16Bisync;
    hash_info.order = fwd_layer == bcmFieldLayerTypeIp6 ? FALSE : TRUE;
    hash_info.compression = fwd_layer == bcmFieldLayerTypeIp6 ? TRUE : FALSE;
    hash_info.hash_config.function_select = bcmFieldContextHashActionValueReplaceCrc;
    hash_info.hash_config.action_key = bcmFieldContextHashActionKeyAdditionalLbKey;
    hash_info.key_info.nof_quals = 5;
    hash_info.key_info.qual_types[0] = fwd_layer == bcmFieldLayerTypeIp6 ? bcmFieldQualifySrcIp6 : bcmFieldQualifySrcIp;
    hash_info.key_info.qual_types[1] = fwd_layer == bcmFieldLayerTypeIp6 ? bcmFieldQualifyDstIp6 : bcmFieldQualifyDstIp;
    hash_info.key_info.qual_types[2] = fwd_layer == bcmFieldLayerTypeIp6 ? bcmFieldQualifyIp6NextHeader : bcmFieldQualifyIp4Protocol;
    hash_info.key_info.qual_types[3] = bcmFieldQualifyL4SrcPort;
    hash_info.key_info.qual_types[4] = bcmFieldQualifyL4DstPort;
    hash_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[0].input_arg = 1;
    hash_info.key_info.qual_info[0].offset = 0;
    hash_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[1].input_arg = 1;
    hash_info.key_info.qual_info[1].offset = 0;
    hash_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[2].input_arg = 1;
    hash_info.key_info.qual_info[2].offset = 0;
    hash_info.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[3].input_arg = 2;
    hash_info.key_info.qual_info[3].offset = 0;
    hash_info.key_info.qual_info[4].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[4].input_arg = 2;
    hash_info.key_info.qual_info[4].offset = 0;
    rv = bcm_field_context_hash_create(unit, 0, bcmFieldStageIngressPMF1,cint_field_ele_flow_ctx_id,&hash_info );
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_context_hash_create \n", rv);
       return rv;
    }

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_ele_flow_presel_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_ele_flow_ctx_id;
    p_data.nof_qualifiers = 2;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = fwd_layer;
    p_data.qual_data[0].qual_mask = 0x1F;

    p_data.qual_data[1].qual_type = bcmFieldQualifyTracePacket;
    p_data.qual_data[1].qual_value = TRUE;
    p_data.qual_data[1].qual_mask = 1;


    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(%s) \n",
            cint_field_ele_flow_presel_id,cint_field_ele_flow_ctx_id, cint_field_layer_name_get(fwd_layer));


    return 0;
}

/*
 * Configures FEM to output actions per Elephant state
 * STATE_NONE - need to send learn action
 * STATE_ELEPHANT - need to send snoop and send Learn(Transplant) to Elephant Reported
 * Other state do nothing
 * */
int cint_field_ele_flow_FEM_add(int unit)
{
    bcm_field_action_priority_t  action_priority;
    bcm_field_fem_action_info_t fem_action_info;
    void *dest_char;
    int state_action_offset;
    int snoop_action_offset;
    int age_action_offset;
    int input_offset = EXEM_32MSB_SHIFT;
    int ii = 0;
    int vmv_shift = 4;
    int rv = BCM_E_NONE;

    rv = cint_field_ele_flow_learn_get_actions_offsets_with_32b_of_learn_payload( unit, &state_action_offset,  &snoop_action_offset,  &age_action_offset);
    if (rv != BCM_E_NONE)
    {
      printf("Error (%d), in cint_field_ele_flow_learn_get_actions_offsets_with_32b_of_learn_payload cint_field_ele_flow_EFES_add\n", rv);
      return rv;
    }

    bcm_field_fem_action_info_t_init(&fem_action_info);
    fem_action_info.fem_input.input_offset = input_offset;
    /*within the 32b input*/
    fem_action_info.condition_msb = state_action_offset + (STATE_SIZE-1) - vmv_shift;

    /* FEM condition has 4 bits, the state is only 2 bits, thus
     *  2 MSB are signficant and indicate state
     *  2 LSB are dont cares hance each state has 4 combinations
     * */
    /*
     * 0-3 EL_STATE_NONE
     */
    for (ii = 0; ii <= 3; ii++)
    {
        fem_action_info.fem_condition[ii].is_action_valid = TRUE;
        fem_action_info.fem_condition[ii].extraction_id = 0;
    }
    fem_action_info.fem_extraction[0].action_type = bcmFieldActionSmallExemLearn;

    /*
     * 8-11 EL_STATE_ELEPHANT
     */
    for (ii = 8; ii <= 11; ii++)
    {
        fem_action_info.fem_condition[ii].is_action_valid = TRUE;
        fem_action_info.fem_condition[ii].extraction_id = 1;
    }
    fem_action_info.fem_extraction[1].action_type = bcmFieldActionSnoop;



    printf("EL_FEM_1 output_bits=");
    for (ii = 0; ii < ENTRY_SIZE_WITHOUT_PADDING; ii++)
    {
        fem_action_info.fem_extraction[0].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeForce;
        fem_action_info.fem_extraction[0].output_bit[ii].forced_value = (( learn_action_value_mouse >> ii) & 0x1);
        printf("%d", fem_action_info.fem_extraction[0].output_bit[ii].forced_value);
    }

    printf(" \n");


    for (ii = 0 ; ii < (SNOOP_CODE_SIZE+SNOOP_STR_SIZE); ii++)
    {
        fem_action_info.fem_extraction[1].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeKey;
        fem_action_info.fem_extraction[1].output_bit[ii].offset = ii+(snoop_action_offset) - vmv_shift;
    }
    printf("Snoop offset: %d , within 32b -> first bit offset: %d , last bit offset: %d \n", snoop_action_offset , snoop_action_offset ,ii+(snoop_action_offset));

    /*Any action priority (FEM_ID) can be chosen here*/
    action_priority = BCM_FIELD_ACTION_POSITION(fem_array_id[0], fem_position[0]);
    rv = bcm_field_fem_action_add(unit, 0, cint_field_ele_flow_exem_fg_id, action_priority, &fem_action_info);
    if(rv != BCM_E_NONE)
    {
       printf("Error (%d) in bcm_field_fem_action_add 1 \n", rv);
       return rv;
    }
    printf("EL_FEM_1 Added\n", rv);


    bcm_field_fem_action_info_t_init(&fem_action_info);
    fem_action_info.fem_input.input_offset = input_offset;
    fem_action_info.condition_msb = state_action_offset + STATE_SIZE - 1 - vmv_shift;

    printf("condition_msb = %d\n", fem_action_info.condition_msb);

    /* FEM condition has 4 bits, the state is only 2 bits, thus
     *  2 MSB are signficant and indicate state
     *  2 LSB are dont cares hance each state has 4 combinations
     * */

    /*
     * 8-11 EL_STATE_ELEPHANT
     */
    for (ii = 8; ii <= 11; ii++)
    {
        fem_action_info.fem_condition[ii].is_action_valid = TRUE;
        fem_action_info.fem_condition[ii].extraction_id = 0;
    }
    fem_action_info.fem_extraction[0].action_type = bcmFieldActionSmallExemLearn;



    for (ii = 0; ii < ENTRY_SIZE_WITHOUT_PADDING; ii++)
    {
        fem_action_info.fem_extraction[0].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeForce;
        fem_action_info.fem_extraction[0].output_bit[ii].forced_value = ((learn_action_value_elephant_reported >> ii) & 0x1);
    }

    /*Any action priority (FEM_ID) can be chosen here*/
    action_priority = BCM_FIELD_ACTION_POSITION(fem_array_id[1], fem_position[1]);
    rv = bcm_field_fem_action_add(unit, 0, cint_field_ele_flow_exem_fg_id, action_priority, &fem_action_info);
    if(rv != BCM_E_NONE)
    {
       printf("Error (%d) in bcm_field_fem_action_add 2\n", rv);
       return rv;
    }
    printf("EL_FEM_2 Added\n", rv);

    bcm_field_fem_action_info_t_init(&fem_action_info);
    fem_action_info.fem_input.input_offset = input_offset;
    fem_action_info.condition_msb = state_action_offset + STATE_SIZE - 1 - vmv_shift;

    /* FEM condition has 4 bits, the state is only 2 bits, thus
     *  2 MSB are signficant and indicate state
     *  2 LSB are dont cares hance each state has 4 combinations
     * */

    /*
     * 8-11 EL_STATE_ELEPHANT
     */
    for (ii = 8; ii <= 11; ii++)
    {
        fem_action_info.fem_condition[ii].is_action_valid = TRUE;
        fem_action_info.fem_condition[ii].extraction_id = 0;
    }
    fem_action_info.fem_extraction[0].action_type = bcmFieldActionPacketTraceEnable;



    for (ii = 0; ii <= 1; ii++)
    {
        fem_action_info.fem_extraction[0].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeForce;
        fem_action_info.fem_extraction[0].output_bit[ii].forced_value = 1;
    }

    /*Any action priority (FEM_ID) can be chosen here*/
    action_priority = BCM_FIELD_ACTION_POSITION(3, 5);
    rv = bcm_field_fem_action_add(unit, 0, cint_field_ele_flow_exem_fg_id, action_priority, &fem_action_info);
    if(rv != BCM_E_NONE)
    {
       printf("Error (%d) in bcm_field_fem_action_add 3\n", rv);
       return rv;
    }
    printf("EL_FEM_3 Added\n", rv);

    return rv;
}


/**
* \brief
*  Configures EXEM FG in iPMF2
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_ele_flow_exem_fg_set(int unit)
{

    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_entry_info_t entry_info;
    bcm_field_group_info_t exem_fg_info;
    bcm_field_qualifier_info_create_t qual_info;

    bcm_field_action_info_t action_info;


    void *dest_char;
    int action_offset;
    int rv = BCM_E_NONE;

    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionVoid;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    action_info.size = STATE_SIZE;
    action_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "State_EF", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &state_act_id);
    if(rv != BCM_E_NONE)
    {
       printf("Error (%d) in bcm_field_action_create State Action\n", rv);
       return rv;
    }



    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionVoid;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    action_info.size = SNOOP_CODE_SIZE+SNOOP_STR_SIZE;
    action_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "Snoop_EF", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &snoop_act_id);
    if(rv != BCM_E_NONE)
    {
       printf("Error (%d) in bcm_field_action_create State Action\n", rv);
       return rv;
    }

    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionVoid;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    action_info.size = AGE_PROFILE_SIZE;
    action_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "Age_prof_EF", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &age_profile_act_id);
    if(rv != BCM_E_NONE)
    {
       printf("Error (%d) in bcm_field_action_create State Action\n", rv);
       return rv;
    }

    /** Padding the age profile to the size of the VMV is only needed for JR2, not for J2C and above.*/
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionVoid;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    action_info.size = VMV_PADDING_SIZE;
    action_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "VMV_PAD_EF", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &vmv_pad_act_id);
    if(rv != BCM_E_NONE)
    {
       printf("Error (%d) in bcm_field_action_create State Action\n", rv);
       return rv;
    }

    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionVoid;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    action_info.size = ENTRY_PADDING_SIZE;
    action_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "Void_EF", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &zero_padding_to_sizeof_learn_action);
    if(rv != BCM_E_NONE)
    {
      printf("Error (%d) in bcm_field_action_create State Action\n", rv);
      return rv;
    }



    /*
     * Create and attach EXEM group in iPMF2
     */
    bcm_field_group_info_t_init(&exem_fg_info);
    exem_fg_info.fg_type = bcmFieldGroupTypeExactMatch;
    exem_fg_info.stage = bcmFieldStageIngressPMF2;
    exem_fg_info.nof_quals = 1;
    exem_fg_info.qual_types[0] = bcmFieldQualifyHashValue;
    exem_fg_info.nof_actions = 5;
    exem_fg_info.action_types[4] = zero_padding_to_sizeof_learn_action;
    exem_fg_info.action_types[3] = state_act_id;
    exem_fg_info.action_types[2] = snoop_act_id;
    exem_fg_info.action_types[1] = vmv_pad_act_id;
    exem_fg_info.action_types[0] = age_profile_act_id;
    exem_fg_info.action_with_valid_bit[0] = FALSE;
    exem_fg_info.action_with_valid_bit[1] = FALSE;
    exem_fg_info.action_with_valid_bit[2] = FALSE;
    exem_fg_info.action_with_valid_bit[3] = FALSE;
    exem_fg_info.action_with_valid_bit[4] = FALSE;



    dest_char = &(exem_fg_info.name[0]);
    sal_strncpy_s(dest_char, "Elephant_Flow", sizeof(exem_fg_info.name));
    rv = bcm_field_group_add(unit, 0, &exem_fg_info, &cint_field_ele_flow_exem_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("EXEM FG Created: fg_id = %d\n", cint_field_ele_flow_exem_fg_id);

    rv = cint_field_ele_flow_FEM_add(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_ele_flow_FEM_add\n", rv);
        return rv;
    }

    /*
     * Attach the FG to context
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = exem_fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = exem_fg_info.qual_types[0];
    attach_info.payload_info.nof_actions = exem_fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = exem_fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = exem_fg_info.action_types[1];
    attach_info.payload_info.action_types[2] = exem_fg_info.action_types[2];
    attach_info.payload_info.action_types[3] = exem_fg_info.action_types[3];
    attach_info.payload_info.action_types[4] = exem_fg_info.action_types[4];


    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;


    rv = bcm_field_group_context_attach(unit, 0, cint_field_ele_flow_exem_fg_id, cint_field_ele_flow_ctx_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    printf("EXEM FG (%d) attached to context (%d)\n", cint_field_ele_flow_exem_fg_id, cint_field_ele_flow_ctx_id);


    return 0;
}


/*
 * Configures the flush entry to change the relevant action.
 * 1. Mouse -> Elephant
 *      Key: state = "Mouse" and hit = 1
 *      Payload = transplant - state = "Elephant"
 * 2. Elephant -> Mouse
 *      Key: state = "Elephant" and hit = 0
 *      Payload = transplant - state = "Mouse"
 * 3. Elephant Reported -> Mouse
 *      Key: state = "Elephant Reported" and hit = 0
 *      Payload = transplant - state = "Mouse"
 * */
int cint_field_ele_flow_flush_entry_add(int unit)
{
    bcm_field_flush_entry_info_t flush_entry_info;
    int rv = BCM_E_NONE;

    /* 
     * 1. Mouse -> Elephant
     *      Key: state = "Mouse" and hit = 1
     *      Payload = transplant - state = "Elephant"
     */
    bcm_field_flush_entry_info_t_init(&flush_entry_info);

    flush_entry_info.key.nof_entry_quals = 0;
    flush_entry_info.key.nof_entry_actions = 1;

    flush_entry_info.key.entry_action[0].type = state_act_id;
    flush_entry_info.key.entry_action[0].value[0] = EL_STATE_MOUSE;
    flush_entry_info.key.entry_action[0].mask[0] = 0x3;

    flush_entry_info.key.hit = 1;
    flush_entry_info.key.hit_mask = 1;

    flush_entry_info.payload.nof_entry_actions = 1;
    flush_entry_info.payload.entry_action[0].type = state_act_id;
    flush_entry_info.payload.entry_action[0].value[0] = EL_STATE_ELEPHANT;
    flush_entry_info.payload.entry_action[0].mask[0] = 0x3;

    flush_entry_info.payload.transplant_cmd = TRUE;

    rv = bcm_field_flush_entry_add(unit, 0, cint_field_ele_flow_exem_fg_id, cint_field_ele_flow_flush_entry_id_mouse_to_elephant, &flush_entry_info);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_flush_entry_add (Mouse->Elephant)\n", rv);
       return rv;
    }

    /* 
     * 2. Elephant -> Mouse
     *      Key: state = "Elephant" and hit = 0
     *      Payload = transplant - state = "Mouse"
     */
    bcm_field_flush_entry_info_t_init(&flush_entry_info);

    flush_entry_info.key.nof_entry_quals = 0;
    flush_entry_info.key.nof_entry_actions = 1;

    flush_entry_info.key.entry_action[0].type = state_act_id;
    flush_entry_info.key.entry_action[0].value[0] = EL_STATE_ELEPHANT;
    flush_entry_info.key.entry_action[0].mask[0] = 0x3;

    flush_entry_info.key.hit = 0;
    flush_entry_info.key.hit_mask = 1;

    flush_entry_info.payload.nof_entry_actions = 1;
    flush_entry_info.payload.entry_action[0].type = state_act_id;
    flush_entry_info.payload.entry_action[0].value[0] = EL_STATE_MOUSE;
    flush_entry_info.payload.entry_action[0].mask[0] = 0x3;

    flush_entry_info.payload.transplant_cmd = TRUE;

    rv = bcm_field_flush_entry_add(unit, 0, cint_field_ele_flow_exem_fg_id, cint_field_ele_flow_flush_entry_id_elephant_to_mouse, &flush_entry_info);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_flush_entry_add(Elephant->Mouse)\n", rv);
       return rv;
    }

    /* 
     * 3. Elephant Reported -> Mouse
     *      Key: state = "Elephant Reported" and hit = 0
     *      Payload = transplant - state = "Mouse"
     */
    bcm_field_flush_entry_info_t_init(&flush_entry_info);

    flush_entry_info.key.nof_entry_quals = 0;
    flush_entry_info.key.nof_entry_actions = 1;

    flush_entry_info.key.entry_action[0].type = state_act_id;
    flush_entry_info.key.entry_action[0].value[0] = EL_STATE_ELEPHANT_REPORTED;
    flush_entry_info.key.entry_action[0].mask[0] = 0x3;

    flush_entry_info.key.hit = 0;
    flush_entry_info.key.hit_mask = 1;

    flush_entry_info.payload.nof_entry_actions = 1;
    flush_entry_info.payload.entry_action[0].type = state_act_id;
    flush_entry_info.payload.entry_action[0].value[0] = EL_STATE_MOUSE;
    flush_entry_info.payload.entry_action[0].mask[0] = 0x3;

    flush_entry_info.payload.transplant_cmd = TRUE;

    rv = bcm_field_flush_entry_add(unit, 0, cint_field_ele_flow_exem_fg_id, cint_field_ele_flow_flush_entry_id_elephant_reported_to_mouse, &flush_entry_info);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_flush_entry_add(Elephant->Mouse)\n", rv);
       return rv;
    }

    return rv;
}

/*
 * Create a SNOOP to CPU.
 */
int
cint_field_ele_flow_utils_mirror_create(int unit,
                                        int cpu_port,
                                        int *mirror_dest_id)
{
    int rv;
    bcm_mirror_destination_t mirror_dest;

    /*
     * Create the mirror destination.
     */
    bcm_mirror_destination_t_init(&mirror_dest);

    mirror_dest.flags = BCM_MIRROR_DEST_IS_SNOOP;
    mirror_dest.packet_copy_size = 256;
    mirror_dest.sample_rate_dividend = 1;
    mirror_dest.sample_rate_divisor = 1;
    mirror_dest.gport = cpu_port;
    rv = bcm_mirror_destination_create(unit, &mirror_dest);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_mirror_destination_create \n");
        return rv;
    }

    *mirror_dest_id = mirror_dest.mirror_dest_id;

    printf("mirror_dest_id = %d (0x%08x) \n", mirror_dest.mirror_dest_id, mirror_dest.mirror_dest_id);

    return rv;
}

/**
* \brief
*  Configures Elephant flow, for IPv4/IPv6 packet
* \param [in] unit        -  Device id
* \param [in] fwd_layer        -  Assumed that packet is Eth | IPv4 | IPv6 in this cint, so fwd_layer can be ETH, IPv4 or IPv6
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_ele_flow_main(int unit,  bcm_field_layer_type_t fwd_layer, int snoop_cmd, int snoop_str, int age_profile)
{
    int rv = BCM_E_NONE;


    rv = cint_field_ele_flow_init_cb(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_ele_flow_init_cb\n", rv);
        return rv;
    }

    /*1. Packet assumed to be ETH | IPv4/IPv6 | TCP/UDP*/
    if(fwd_layer != bcmFieldLayerTypeEth && fwd_layer != bcmFieldLayerTypeIp4 && fwd_layer != bcmFieldLayerTypeIp6)
    {
        printf("Error Forwarding Layer type can be Ethernet, IPv4 or IPv6\n");
        return BCM_E_PARAM;
    }



    /*2. Configure iPMF1 - Hash + context slection*/
    rv = cint_field_ele_flow_ipmf1_set(unit,fwd_layer);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_ele_flow_ipmf1_set\n", rv);
        return rv;
    }


    /*Set learn actions*/
    rv = cint_field_ele_flow_learn_payload_set(unit, EL_STATE_MOUSE, snoop_str, snoop_cmd, age_profile , &learn_action_value_mouse);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_ele_flow_learn_payload_set(state MOUSE)\n", rv);
        return rv;
    }
    rv = cint_field_ele_flow_learn_payload_set(unit, EL_STATE_ELEPHANT_REPORTED, snoop_str, snoop_cmd, age_profile , &learn_action_value_elephant_reported);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_ele_flow_learn_payload_set(state ELEPHANT-REPORTED)\n", rv);
        return rv;
    }
    printf("Learn action on State None: 0x%x \n",learn_action_value_mouse);
    printf("Learn action on State Elephant: 0x%x \n",learn_action_value_elephant_reported);


    /*3.2 Add iPMF2 EXEM FG*/
    rv = cint_field_ele_flow_exem_fg_set(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_ele_flow_exem_fg_set\n", rv);
        return rv;

    }



    /* Add entries to flush TCAM */
    rv = cint_field_ele_flow_flush_entry_add(unit);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in cint_field_ele_flow_flush_entry_add\n", rv);
       return rv;
    }

    return rv;

}


/**
* \brief
*  Destroy all configuration done.
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_ele_flow_destroy(int unit)
{
    int rv = BCM_E_NONE;

    /** Delete the flush entry. */
    rv = bcm_field_flush_entry_delete(unit, 0, cint_field_ele_flow_exem_fg_id, cint_field_ele_flow_flush_entry_id_elephant_reported_to_mouse);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_flush_entry_delete(Elephant-Reported->Mouse)\n", rv);
       return rv;
    }

    rv = bcm_field_flush_entry_delete(unit, 0, cint_field_ele_flow_exem_fg_id, cint_field_ele_flow_flush_entry_id_elephant_to_mouse);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_flush_entry_delete(Elephant->Mouse)\n", rv);
       return rv;
    }

    rv = bcm_field_flush_entry_delete(unit, 0, cint_field_ele_flow_exem_fg_id, cint_field_ele_flow_flush_entry_id_mouse_to_elephant);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_flush_entry_delete(Mouse->Elephant)\n", rv);
       return rv;
    }

    return rv;
}

