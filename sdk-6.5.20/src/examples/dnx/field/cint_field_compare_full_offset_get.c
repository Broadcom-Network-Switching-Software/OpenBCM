/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/**
 * Configuration example start:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../src/examples/dnx/field/cint_field_compare_full_offset_get.c
 * cint;
 * int unit = 0;
 * cint_field_compare_full_offset_get_main(unit);
 * exit
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
 */
int
cint_field_compare_full_offset_get_config(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t * context_id_p)
{
    bcm_field_context_info_t context_info;
    bcm_field_context_compare_info_t compare_info;
    char *proc_name;
    void *dest_char;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_compare_full_offset_get_config";
   /**
    * Init the context_info
    */
    bcm_field_context_info_t_init(&context_info);
    /**
     * Set the compare mode to be bcmFieldContextCompareTypeDouble
     */
    context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeDouble;
    /**
     * Create the context with ID
     */
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "compare_context", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, stage, &context_info, context_id_p);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_create\n", proc_name, rv);
        return rv;
    }
   /**
    * Init the compare_info
    */
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

    /**
     * Create the context compare mode.
     */
    rv = bcm_field_context_compare_create(unit, 0, stage, *context_id_p, 0, &compare_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_compare_create\n", proc_name, rv);
        return rv;
    }
    return rv;
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
 */
int
cint_field_compare_full_offset_get_presel_set(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t * context_id_p)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    char *proc_name;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_compare_full_offset_get_presel_set";
   /**
    * Fill the info in presel_entry_data and bcm_field_presel_entry_id_t structures
    */
    Cint_field_compare_full_offset_get_presel_entry_id.presel_id = Cint_field_compare_full_offset_get_presel_id;
    Cint_field_compare_full_offset_get_presel_entry_id.stage = stage;
    presel_entry_data.context_id = *context_id_p;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.nof_qualifiers = Cint_field_compare_full_offset_get_presel_nof_quals;
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyVlanFormat;
    presel_entry_data.qual_data[0].qual_value = Cint_field_compare_full_offset_get_presel_qual_value;
    presel_entry_data.qual_data[0].qual_mask = Cint_field_compare_full_offset_get_presel_qual_mask;
    /**
     * Set the presel
     */
    rv = bcm_field_presel_set(unit, 0, &Cint_field_compare_full_offset_get_presel_entry_id, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_presel_set\n", proc_name, rv);
        return rv;
    }
    return rv;
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
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
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
    char *proc_name;
    void *dest_char;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_compare_full_offset_get_fg_config";
    /**
     * Init the fg_info structure.
     */
    bcm_field_group_info_t_init(&fg_info);
    /**
     * Fill the fg_info structure
     */
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
    /**
     * Create FG without ID
     */
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, Cint_field_compare_full_offset_get_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_add\n", proc_name, rv);
        return rv;
    }
    /**
     * Init the attach_info.
     */
    bcm_field_group_attach_info_t_init(&attach_info);
    /**
     * Fill the attach_info structure
     */
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
    /**
     * Attach the FG to the context.
     */
    rv = bcm_field_group_context_attach(unit, 0, Cint_field_compare_full_offset_get_fg_id, Cint_field_compare_full_offset_get_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
        return rv;
    }
    /**
     * Get the offset.
     * Give the enum value that you want to be compared.
     */
    rv = bcm_field_compare_operand_offset_get(unit, pair_id, bcmFieldCompareOperandNotEqual, &op_offset);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_compare_operand_offset_get\n", proc_name, rv);
        return rv;
    }
    /**
     * Add the first entry.
     * It will provide a signal if the key F and key G are equal.
     */
    /**
     * Init the entry_info structure.
     */
    bcm_field_entry_info_t_init(&entry_info);
    /**
     * Fill the entry_info structure
     */
    entry_info.nof_entry_quals = 1;
    entry_info.nof_entry_actions = 1;
    entry_info.priority = 1;
    entry_info.entry_qual[0].type = bcmFieldQualifyCompareKeysResult0;
    entry_info.entry_qual[0].value[0] = 1 << op_offset;
    entry_info.entry_qual[0].mask[0] = 1 << op_offset;
    entry_info.entry_action[0].type = bcmFieldActionDropPrecedence;
    entry_info.entry_action[0].value[0] = 0x2;
    /**
     * Add an entry to the FG
     */
    rv = bcm_field_entry_add(unit, 0, Cint_field_compare_full_offset_get_fg_id, &entry_info, &Cint_field_compare_full_offset_get_entry_arr[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add entry_id1(%d)\n", rv, Cint_field_compare_full_offset_get_entry_arr[0]);
        return rv;
    }
    /**
     * Add the second entry.
     * It will provide a signal if the key F and key G are NOT equal.
     */
    /**
     * Init the entry_info structure.
     */
    bcm_field_entry_info_t_init(&entry_info);
    /**
     * Fill the entry_info structure
     */
    entry_info.nof_entry_quals = 1;
    entry_info.nof_entry_actions = 1;
    entry_info.priority = 1;
    entry_info.entry_qual[0].type = bcmFieldQualifyCompareKeysResult0;
    entry_info.entry_qual[0].value[0] = 0x0;
    entry_info.entry_qual[0].mask[0] = 0x0;
    entry_info.entry_action[0].type = bcmFieldActionDropPrecedence;
    entry_info.entry_action[0].value[0] = 0x3;
    /**
     * Add an entry to the FG
     */
    rv = bcm_field_entry_add(unit, 0, Cint_field_compare_full_offset_get_fg_id, &entry_info, &Cint_field_compare_full_offset_get_entry_arr[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add entry_id3(%d)\n", rv, Cint_field_compare_full_offset_get_entry_arr[1]);
        return rv;
    }
    
return rv;
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
    char *proc_name;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_compare_full_offset_get_main";
    /**
     * Create the context and the context_compare
     */
    printf("Create context compare mode\n");
    rv = cint_field_compare_full_offset_get_config(unit, bcmFieldStageIngressPMF1, Cint_field_compare_full_offset_get_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_compare_presel_set \n", proc_name, rv);
        return rv;
    }
    /**
     * Create a preselector which is going to qualify upon Ethernet header
     */
    printf("Create a preselector which is going to qualify upon Ethernet header \n");
    rv = cint_field_compare_full_offset_get_presel_set(unit, bcmFieldStageIngressPMF1, Cint_field_compare_full_offset_get_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_compare_presel_set \n", proc_name, rv);
        return rv;
    }
    /**
     * Create TCAM Fg and Attach it to the context.
     */
    printf("Create TCAM Fg and Attach it to the context and add an entry. \n");
    rv = cint_field_compare_full_offset_get_fg_config(unit, Cint_field_compare_full_offset_get_context_id, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_compare_fg_config \n", proc_name, rv);
        return rv;
    }
    return rv;
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
    char *proc_name;
    int entry_iterator;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_compare_config_destroy";
    /**
     * Init and reset the presel
     */
    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    rv = bcm_field_presel_set(unit, 0, &Cint_field_compare_full_offset_get_presel_entry_id, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_presel_set\n", proc_name, rv);
        return rv;
    }
    /**
    * Delete the created entry.
    */
    for (entry_iterator = 0; entry_iterator < Cint_field_compare_full_offset_get_nof_entries; entry_iterator++)
    {
        rv = bcm_field_entry_delete(unit, Cint_field_compare_full_offset_get_fg_id, NULL, Cint_field_compare_full_offset_get_entry_arr[entry_iterator]);
        if (rv != BCM_E_NONE)
        {
            printf("%s Error (%d), in bcm_field_entry_delete entry1 pair1\n", proc_name, rv);
            return rv;
        }
    }
    /**
     * Detach the context
     */
    rv = bcm_field_group_context_detach(unit, Cint_field_compare_full_offset_get_fg_id, Cint_field_compare_full_offset_get_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_detach pair1\n", proc_name, rv);
        return rv;
    }
    /**
     * Delete the FG
     */
    rv = bcm_field_group_delete(unit, Cint_field_compare_full_offset_get_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_delete pair1\n", proc_name, rv);
        return rv;
    }
    /**
     * Compare destroy the first pair_id
     */
    rv = bcm_field_context_compare_destroy(unit, bcmFieldStageIngressPMF1, Cint_field_compare_full_offset_get_context_id, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_compare_destroy pair1\n", proc_name, rv);
        return rv;
    }
    /**
     * Destroy the context
     */
    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, Cint_field_compare_full_offset_get_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_destroy\n", proc_name, rv);
        return rv;
    }
return rv;
}
