/**
 * Configuration example start:
 *
  cint ../../../src/examples/dnx/field/cint_field_compare_full_offset_get.c
  cint ../../../../src/examples/dnx/field/cint_field_compare_full_offset_get.c
  cint;
  int unit = 0;
  cint_field_compare_full_offset_get_main(unit);
  exit
 *
 * Configuration example end
 *
 * This CINT shows how to create a context in the compare mode Double and it is
 * using bcm_field_compare_operand_offset_get() to get the offset
 *   of a specific compare operand inside the 6-bit compare result.
 *
 * Main functions:
 * cint_field_compare_full_offset_get_main(unit);
 *
 * Destroy function:
 * cint_field_compare_full_offset_get_destroy(unit);
 *
 */
 /**
  * Global variables
  * {
  */
/**
 * The ID of the context
 */
bcm_field_context_t Cint_field_compare_full_offset_get_context_id;
/**
 * The ID of the preselector
 */
bcm_field_presel_t Cint_field_compare_full_offset_get_presel_id = 10;
/**
 * The number of quals for preselection
 */
int Cint_field_compare_full_offset_get_presel_nof_quals = 1;
/**
 * The value of the preselector qualifier
 */
uint32 Cint_field_compare_full_offset_get_presel_qual_value = 0x1;
/**
 * The mask of the preselector qualifier
 */
uint32 Cint_field_compare_full_offset_get_presel_qual_mask = 0x1;
/**
 * The number of entries
 */
int Cint_field_compare_full_offset_get_nof_entries = 2;
/**
 * The presel_entry. Used for creating and destroying the presel
 */
bcm_field_presel_entry_id_t Cint_field_compare_full_offset_get_presel_entry_id;
/**
 * The variable in which we store the fg_id
 */
bcm_field_group_t Cint_field_compare_full_offset_get_fg_id;

/**
 * Array in which we will collect the entry_id values values.
 */
bcm_field_entry_t Cint_field_compare_full_offset_get_entry_arr[Cint_field_compare_full_offset_get_nof_entries] = {
       0,
       0
};


/* *INDENT-ON* */
/**
 * \brief
 *
 *  This function contains the bcm_field_context_create and
 *  bcm_field_context_compare_mode_create APIs.
 *  This function is creating a context with mode bcmFieldContextCompareTypeDouble)
 *  and with 1 pair.
 *
 * \param [in] unit - The unit number.
 * \param [in] stage - The field stage.
 * \param [in] context_id_p - The id of the context.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 *
 */
int
cint_field_compare_full_offset_get_config(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t * context_id_p)
{
    bcm_field_context_info_t context_info;
    bcm_field_context_compare_info_t compare_info;

    void *dest_char;
    printf("Running: cint_field_compare_full_offset_get_config()\n");

    bcm_field_context_info_t_init(&context_info);
    /**
     * Set the compare mode to be bcmFieldContextCompareTypeDouble
     */
    context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeDouble;
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "compare_context", sizeof(context_info.name));
    BCM_IF_ERROR_RETURN(bcm_field_context_create(unit, 0, stage, &context_info, context_id_p));


    bcm_field_context_compare_info_t_init(&compare_info);
    /**
     * Fill the compare info
     * In the example L4 Src port is being compared to L4 Dst port using initial keys F and G of IPMF1
     */
    compare_info.first_key_info.nof_quals = 1;
    compare_info.first_key_info.qual_types[0] = bcmFieldQualifyL4DstPort;
    compare_info.first_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.first_key_info.qual_info[0].input_arg = 2;
    compare_info.first_key_info.qual_info[0].offset = 0;

    compare_info.second_key_info.nof_quals = 1;
    compare_info.second_key_info.qual_types[0] = bcmFieldQualifyL4SrcPort;
    compare_info.second_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.second_key_info.qual_info[0].input_arg = 2;
    compare_info.second_key_info.qual_info[0].offset = 0;
    BCM_IF_ERROR_RETURN(bcm_field_context_compare_create(unit, 0, stage, *context_id_p, 0, &compare_info));

    return BCM_E_NONE;
}
/**
 * \brief
 *
 *  This function contains the bcm_field_presel_set
 *  application.
 *  This function sets all required HW
 *  configuration for bcm_field_presel_set
 *  to be performed for IPMF1.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 *
 */
int
cint_field_compare_full_offset_get_presel_set(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t * context_id_p)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    printf("Running: cint_field_compare_full_offset_get_presel_set()\n");

    Cint_field_compare_full_offset_get_presel_entry_id.presel_id = Cint_field_compare_full_offset_get_presel_id;
    Cint_field_compare_full_offset_get_presel_entry_id.stage = stage;
    presel_entry_data.context_id = *context_id_p;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.nof_qualifiers = Cint_field_compare_full_offset_get_presel_nof_quals;
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyVlanFormat;
    presel_entry_data.qual_data[0].qual_value = Cint_field_compare_full_offset_get_presel_qual_value;
    presel_entry_data.qual_data[0].qual_mask = Cint_field_compare_full_offset_get_presel_qual_mask;

    BCM_IF_ERROR_RETURN(bcm_field_presel_set(unit, 0, &Cint_field_compare_full_offset_get_presel_entry_id, &presel_entry_data));

    return BCM_E_NONE;
}
/**
 * \brief
 * This function is creating FG in IPMF2.
 * The IPMF2 TCAM FG will use this compare result as part of its key.
 * This is the order of the bits in bcmFieldQualifyCompareKeysResult0/1:
 *
 * F initial > G initial   ( msb )
 * F initial == G initial
 * F initial => G initial
 * F initial < G initial
 * F initial <= G initial
 * F initial != G initial
 *
 * \param [in] unit - The unit number.
 * \param [in] context_id_p - The context_id.
 * \param [in] pair_id - The the ID of the pair.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 *
 */
int static
cint_field_compare_full_offset_get_fg_config(
    int unit,
    bcm_field_context_t * context_id_p,
    int pair_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    bcm_field_action_info_t action_info;
    int op_offset;
    void *dest_char;

    printf("Running: cint_field_compare_full_offset_get_fg_config()\n");
    bcm_field_group_info_t_init(&fg_info);

    fg_info.stage = bcmFieldStageIngressPMF2;
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.nof_quals = 5;
    /**
     * Setting the qual to be bcmFieldQualifyCompareKeysTcam0. So we can see the result from the compare.
     */
    fg_info.qual_types[0] = bcmFieldQualifyCompareKeysResult0;
    fg_info.qual_types[1] = bcmFieldQualifyDstRpfGport;
    fg_info.qual_types[2] = bcmFieldQualifyOamMepId;
    fg_info.qual_types[3] = bcmFieldQualifyDstPort;
    fg_info.qual_types[4] = bcmFieldQualifyL4PortRangeCheck;
    
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionDropPrecedence;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "compare_full_offset_FG", sizeof(fg_info.name));
    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, &fg_info, Cint_field_compare_full_offset_get_fg_id));


    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = 5;
    attach_info.key_info.qual_types[0] = bcmFieldQualifyCompareKeysResult0;
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_types[1] = bcmFieldQualifyDstRpfGport;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_types[2] = bcmFieldQualifyOamMepId;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_types[3] = bcmFieldQualifyDstPort;
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_types[4] = bcmFieldQualifyL4PortRangeCheck;
    attach_info.key_info.qual_info[4].input_type = bcmFieldInputTypeMetaData;

    
    attach_info.payload_info.nof_actions = 1;
    attach_info.payload_info.action_types[0] = bcmFieldActionDropPrecedence;
    BCM_IF_ERROR_RETURN(bcm_field_group_context_attach(unit, 0, Cint_field_compare_full_offset_get_fg_id, Cint_field_compare_full_offset_get_context_id, &attach_info));

    /**
     * Get the offset.
     * Give the enum value that you want to be compared.
     */
    BCM_IF_ERROR_RETURN(bcm_field_compare_operand_offset_get(unit, pair_id, bcmFieldCompareOperandNotEqual, &op_offset));

    /**
     * Add the first entry.
     * It will provide a signal if the key F and key G are equal.
     */
    bcm_field_entry_info_t_init(&entry_info);

    entry_info.nof_entry_quals = 1;
    entry_info.nof_entry_actions = 1;
    entry_info.priority = 1;
    entry_info.entry_qual[0].type = bcmFieldQualifyCompareKeysResult0;
    entry_info.entry_qual[0].value[0] = 1 << op_offset;
    entry_info.entry_qual[0].mask[0] = 1 << op_offset;
    entry_info.entry_action[0].type = bcmFieldActionDropPrecedence;
    entry_info.entry_action[0].value[0] = 0x2;
    BCM_IF_ERROR_RETURN(bcm_field_entry_add(unit, 0, Cint_field_compare_full_offset_get_fg_id, &entry_info, &Cint_field_compare_full_offset_get_entry_arr[0]));

    /**
     * Add the second entry.
     * It will provide a signal if the key F and key G are NOT equal.
     */
    bcm_field_entry_info_t_init(&entry_info);

    entry_info.nof_entry_quals = 1;
    entry_info.nof_entry_actions = 1;
    entry_info.priority = 1;
    entry_info.entry_qual[0].type = bcmFieldQualifyCompareKeysResult0;
    entry_info.entry_qual[0].value[0] = 0x0;
    entry_info.entry_qual[0].mask[0] = 0x0;
    entry_info.entry_action[0].type = bcmFieldActionDropPrecedence;
    entry_info.entry_action[0].value[0] = 0x3;
    BCM_IF_ERROR_RETURN(bcm_field_entry_add(unit, 0, Cint_field_compare_full_offset_get_fg_id, &entry_info, &Cint_field_compare_full_offset_get_entry_arr[1]));
    
    return BCM_E_NONE;
}

/**
 * In this test we are creating context in compare_1 mode from type Double
 * with only 1 pair to compare.
 * We are comparing first and second key.
 *
 * \param [in] unit - The unit number.
 *  \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int
cint_field_compare_full_offset_get_main(
    int unit)
{
    printf("Running: cint_field_compare_full_offset_get_main()\n");
    /**
     * Create the context and the context_compare
     */
    printf("Create context compare mode\n");
    BCM_IF_ERROR_RETURN(cint_field_compare_full_offset_get_config(unit, bcmFieldStageIngressPMF1, Cint_field_compare_full_offset_get_context_id));

    printf("Create a preselector which is going to qualify upon Ethernet header \n");
    BCM_IF_ERROR_RETURN(cint_field_compare_full_offset_get_presel_set(unit, bcmFieldStageIngressPMF1, Cint_field_compare_full_offset_get_context_id));

    printf("Create TCAM Fg and Attach it to the context and add an entry. \n");
    BCM_IF_ERROR_RETURN(cint_field_compare_full_offset_get_fg_config(unit, Cint_field_compare_full_offset_get_context_id, 0));

    return BCM_E_NONE;
}
/** Destroys all allocated data from the compare configuration.
 *
 * \param [in] unit - The unit number.
 *
 *  \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int
cint_field_compare_full_offset_get_destroy(
    int unit)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    int entry_iterator;

    printf("Running: cint_field_compare_config_destroy()\n");

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    BCM_IF_ERROR_RETURN(bcm_field_presel_set(unit, 0, &Cint_field_compare_full_offset_get_presel_entry_id, &presel_entry_data));

    for (entry_iterator = 0; entry_iterator < Cint_field_compare_full_offset_get_nof_entries; entry_iterator++)
    {
        BCM_IF_ERROR_RETURN(bcm_field_entry_delete(unit, Cint_field_compare_full_offset_get_fg_id, NULL, Cint_field_compare_full_offset_get_entry_arr[entry_iterator]));

    }

    BCM_IF_ERROR_RETURN(bcm_field_group_context_detach(unit, Cint_field_compare_full_offset_get_fg_id, Cint_field_compare_full_offset_get_context_id));

    BCM_IF_ERROR_RETURN(bcm_field_group_delete(unit, Cint_field_compare_full_offset_get_fg_id));

    BCM_IF_ERROR_RETURN(bcm_field_context_compare_destroy(unit, bcmFieldStageIngressPMF1, Cint_field_compare_full_offset_get_context_id, 0));

    BCM_IF_ERROR_RETURN(bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, Cint_field_compare_full_offset_get_context_id));

    return BCM_E_NONE;
}
