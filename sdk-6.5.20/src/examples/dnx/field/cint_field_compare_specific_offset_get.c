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
 * cint ../../../src/examples/dnx/field/cint_field_compare_specific_offset_get.c
 * cint;
 * int unit = 0;
 * cint_field_compare_specific_offset_get_main(unit);
 *
 * Configuration example end
 *
 *   This CINT shows how to create a context in the compare mode Double and it is
 *   using bcm_field_compare_operand_offset_get() to get the offset
 *   of a specific compare operand inside the 6-bit compare result.
 *
 * Main functions:
 * cint_field_compare_specific_offset_get_main(unit);
 *
 * Destroy function:
 * cint_field_compare_specific_offset_get_destroy(unit);
 */
 /**
  * Global variables
  * {
  */
/**
 * The ID of the context in IPMF1
 */
bcm_field_context_t Cint_field_compare_specific_offset_get_context_id_ipmf1;
/**
 * The ID of the first context in IPMF2
 */
bcm_field_context_t Cint_field_compare_specific_offset_get_context_id_ipmf2;
/**
 * The ID of the preselector in IPMF1
 */
bcm_field_presel_t Cint_field_compare_specific_offset_get_presel_id_ipmf1 = 10;
/**
 * The ID of the preselector in IPMF2
 */
bcm_field_presel_t Cint_field_compare_specific_offset_get_presel_id_ipmf2 = 11;
/**
 * The number of quals for preselection
 */
int Cint_field_compare_specific_offset_get_presel_nof_quals = 1;
/**
 * The value of the preselector qualifier
 */
uint32 Cint_field_compare_specific_offset_get_presel_qual_value = 0x1;
/**
 * The mask of the preselector qualifier
 */
uint32 Cint_field_compare_specific_offset_get_presel_qual_mask = 0x1;
/**
 * The number of entries
 */
int Cint_field_compare_specific_offset_get_nof_entries = 2;
/**
 * The presel_entry. Used for creating and destroying the presel in iPMF1
 */
bcm_field_presel_entry_id_t Cint_field_compare_specific_offset_get_presel_entry_id_ipmf1;
/**
 * The presel_entry. Used for creating and destroying the presel in iPMF2
 */
bcm_field_presel_entry_id_t Cint_field_compare_specific_offset_get_presel_entry_id_ipmf2;
/**
 * The variable in which we store the fg_id
 */
bcm_field_group_t Cint_field_compare_specific_offset_get_fg_id;

/**
 * Array in which we will collect the entry_id values values.
 */
bcm_field_entry_t Cint_field_compare_specific_offset_get_entry_id;
/**
 * The ID of the qualifier.
 */
bcm_field_qualify_t Cint_field_compare_specific_offset_get_qual_id;

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
cint_field_compare_specific_offset_get_main(
    int unit)
{
    char *proc_name;
    bcm_field_action_info_t user_action_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    bcm_field_action_info_t action_info;
    int op_offset1, op_offset2, ud_qual_offset;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_info_t context_info;
    bcm_field_context_compare_info_t compare_info;
    bcm_field_qualifier_info_get_t qual_info_get;
    void *dest_char;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_compare_specific_offset_get_main";
    /**
     * Create user defined qual
     * Which will be 1bit result of the desired compare operand.
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "1bitCmpResult", sizeof(qual_info.name));
    qual_info.size = 1;
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &Cint_field_compare_specific_offset_get_qual_id);
    if (rv != BCM_E_NONE)
    {
      printf("%s(): Error (%d), in bcm_field_qualifier_create on qual_id = %d (%s) \n", proc_name, rv,
              Cint_field_compare_specific_offset_get_qual_id, dest_char);
      return rv;
    }
    printf("1 step: Create user defined qual 1bitCmpResult \n");

    /**
     * Create context on IPMF1
     */
   /**
    * Init the context_info
    */
    bcm_field_context_info_t_init(&context_info);
    /**
    * Set the compare mode to be bcmFieldContextCompareTypeDouble
    */
    context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeDouble;
    context_info.context_compare_modes.compare_2_mode = bcmFieldContextCompareTypeDouble;
    /**
    * Create the context without ID
    */
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "compare_context_ipmf1", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &Cint_field_compare_specific_offset_get_context_id_ipmf1);
    if (rv != BCM_E_NONE)
    {
       printf("%s Error (%d), in bcm_field_context_create\n", proc_name, rv);
       return rv;
    }
    printf("2.a. step: Create context(%d) on ipmf1 \n", Cint_field_compare_specific_offset_get_context_id_ipmf1);
   /**
    * Fill the info in presel_entry_data and bcm_field_presel_entry_id_t structures
    */
    Cint_field_compare_specific_offset_get_presel_entry_id_ipmf1.presel_id = Cint_field_compare_specific_offset_get_presel_id_ipmf1;
    Cint_field_compare_specific_offset_get_presel_entry_id_ipmf1.stage = bcmFieldStageIngressPMF1;
    presel_entry_data.context_id = Cint_field_compare_specific_offset_get_context_id_ipmf1;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.nof_qualifiers = Cint_field_compare_specific_offset_get_presel_nof_quals;
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyVlanFormat;
    presel_entry_data.qual_data[0].qual_value = Cint_field_compare_specific_offset_get_presel_qual_value;
    presel_entry_data.qual_data[0].qual_mask = Cint_field_compare_specific_offset_get_presel_qual_mask;
    /**
     * Set the presel
     */
    rv = bcm_field_presel_set(unit, 0, &Cint_field_compare_specific_offset_get_presel_entry_id_ipmf1, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_presel_set\n", proc_name, rv);
        return rv;
    }
    printf("2.b. step: Set a preselector (%d) for context(%d) \n",Cint_field_compare_specific_offset_get_presel_id_ipmf1, Cint_field_compare_specific_offset_get_context_id_ipmf1);
    /**
     * Create 2 compare keys.
     */
   /**
    * Init the compare_info
    */
    bcm_field_context_compare_info_t_init(&compare_info);
    /**
     * Fill the compare info
     */
    compare_info.first_key_info.nof_quals = 1;
    compare_info.first_key_info.qual_types[0] = bcmFieldQualifyDstIp;
    compare_info.first_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.first_key_info.qual_info[0].input_arg = 1;
    compare_info.first_key_info.qual_info[0].offset = 0;

    compare_info.second_key_info.nof_quals = 1;
    compare_info.second_key_info.qual_types[0] = bcmFieldQualifySrcIp;
    compare_info.second_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.second_key_info.qual_info[0].input_arg = 1;
    compare_info.second_key_info.qual_info[0].offset = 0;

    /**
    * Create the context compare mode for first pair.
    */
    rv = bcm_field_context_compare_create(unit, 0, bcmFieldStageIngressPMF1, Cint_field_compare_specific_offset_get_context_id_ipmf1, 0, &compare_info);
    if (rv != BCM_E_NONE)
    {
       printf("%s Error (%d), in bcm_field_context_compare_create\n", proc_name, rv);
       return rv;
    }
    printf("3.a. step: Create context compare first and second key for pair 1 \n");
    /**
    * Init the compare_info
    */
    bcm_field_context_compare_info_t_init(&compare_info);
    /**
     * Fill the compare info
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
    * Create the context compare mode for first pair.
    */
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "compare_context_1", sizeof(context_info.name));
    rv = bcm_field_context_compare_create(unit, 0, bcmFieldStageIngressPMF1, Cint_field_compare_specific_offset_get_context_id_ipmf1, 1, &compare_info);
    if (rv != BCM_E_NONE)
    {
       printf("%s Error (%d), in bcm_field_context_compare_create\n", proc_name, rv);
       return rv;
    }
    printf("3.b. step: Create context compare first and second key for pair 2 \n");
    /**
    * Init the context_info
    */
    bcm_field_context_info_t_init(&context_info);
    /**
    * Create the context without ID
    */
    context_info.cascaded_from = Cint_field_compare_specific_offset_get_context_id_ipmf1 ;
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "compare_context_ipmf2", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF2, &context_info, &Cint_field_compare_specific_offset_get_context_id_ipmf2);
    if (rv != BCM_E_NONE)
    {
     printf("%s Error (%d), in bcm_field_context_create\n", proc_name, rv);
     return rv;
    }
    printf("4.a. step: Create context(%d) on ipmf2 \n", Cint_field_compare_specific_offset_get_context_id_ipmf2);
    /**
     * Set the presel
     */
    /**
    * Fill the info in presel_entry_data and bcm_field_presel_entry_id_t structures
    */
    Cint_field_compare_specific_offset_get_presel_entry_id_ipmf2.presel_id = Cint_field_compare_specific_offset_get_presel_id_ipmf2;
    Cint_field_compare_specific_offset_get_presel_entry_id_ipmf2.stage = bcmFieldStageIngressPMF2;
    presel_entry_data.context_id = Cint_field_compare_specific_offset_get_context_id_ipmf2;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.nof_qualifiers = Cint_field_compare_specific_offset_get_presel_nof_quals;
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyStateTableData;
    presel_entry_data.qual_data[0].qual_value = 0x0;
    presel_entry_data.qual_data[0].qual_mask = 0x0;
    rv = bcm_field_presel_set(unit, 0, &Cint_field_compare_specific_offset_get_presel_entry_id_ipmf2, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_presel_set\n", proc_name, rv);
        return rv;
    }
    printf("4.b. step: Set a preselector (%d) for context(%d) \n", Cint_field_compare_specific_offset_get_presel_id_ipmf1, Cint_field_compare_specific_offset_get_context_id_ipmf2);
    /**
     * Create FG in IPMF2
     */
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
    fg_info.qual_types[0] = Cint_field_compare_specific_offset_get_qual_id;
    fg_info.qual_types[1] = bcmFieldQualifyDstRpfGport;
    fg_info.qual_types[2] = bcmFieldQualifyOamMepId;
    fg_info.qual_types[3] = bcmFieldQualifyDstPort;
    fg_info.qual_types[4] = bcmFieldQualifyL4PortRangeCheck;

    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionDropPrecedence;

    /**
     * Create FG without ID
     */
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "Compare_FG", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &Cint_field_compare_specific_offset_get_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_add\n", proc_name, rv);
        return rv;
    }
    printf("5. step: Create FG(%d) on ipmf2 \n", Cint_field_compare_specific_offset_get_fg_id);

    /**
     * Retrieve the desired result offset of the qualifier.
     */
    rv = bcm_field_group_qualifier_offset_get(unit, 0, Cint_field_compare_specific_offset_get_fg_id, Cint_field_compare_specific_offset_get_qual_id, &ud_qual_offset);
    if (rv != BCM_E_NONE)
    {
       printf("%s Error (%d), in bcm_field_group_qualifier_offset_get\n", proc_name, rv);
       return rv;
    }
    printf("6. step: Get the user defines qualifier offset(%d)\n", ud_qual_offset);

    /**
     * Retrieve the desired result offset of the qualifier.
     */
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyCompareKeysResult0, bcmFieldStageIngressPMF2, &qual_info_get);
    if (rv != BCM_E_NONE)
    {
       printf("%s Error (%d), in bcm_field_group_qualifier_offset_get\n", proc_name, rv);
       return rv;
    }
    printf("7. step: Get the bcmFieldQualifyCompareKeysResult0 qualifier offset(%d)\n", qual_info_get.offset);
    /**
     * Get the offset.
     * Give the enum value that you want to be compared.
     */
    rv = bcm_field_compare_operand_offset_get(unit, 0, bcmFieldCompareOperandFirstKeyNotBigger, &op_offset1);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_compare_operand_offset_get\n", proc_name, rv);
        return rv;
    }
    printf("8.a. step: Get the operand offset for first pair(%d)\n", op_offset1);
    /**
     * Get the offset.
     * Give the enum value that you want to be compared.
     */
    rv = bcm_field_compare_operand_offset_get(unit, 1, bcmFieldCompareOperandNotEqual, &op_offset2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_compare_operand_offset_get\n", proc_name, rv);
        return rv;
    }
    printf("8.b. step: Get the operand offset for first pair(%d)\n", op_offset2);
   /**
    * Init the attach_info.
    */
   bcm_field_group_attach_info_t_init(&attach_info);
   /**
    * Fill the attach_info structure
    */
    attach_info.key_info.nof_quals = 5;
    attach_info.key_info.qual_types[0] = Cint_field_compare_specific_offset_get_qual_id;
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = (ud_qual_offset + op_offset2 + qual_info_get.offset);
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
   rv = bcm_field_group_context_attach(unit, 0, Cint_field_compare_specific_offset_get_fg_id, Cint_field_compare_specific_offset_get_context_id_ipmf2, &attach_info);
   if (rv != BCM_E_NONE)
   {
       printf("%s Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
       return rv;
   }
   printf("9. step: Attach the FG (%d) to the context(%d)\n", Cint_field_compare_specific_offset_get_fg_id, Cint_field_compare_specific_offset_get_context_id_ipmf2);
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
   entry_info.entry_qual[0].type = Cint_field_compare_specific_offset_get_qual_id;
   entry_info.entry_qual[0].value[0] = 1;
   entry_info.entry_qual[0].mask[0] = 1;
   entry_info.entry_action[0].type = bcmFieldActionDropPrecedence;
   entry_info.entry_action[0].value[0] = 0x2;
   /**
    * Add an entry to the FG
    */
   rv = bcm_field_entry_add(unit, 0, Cint_field_compare_specific_offset_get_fg_id, &entry_info, &Cint_field_compare_specific_offset_get_entry_id);
   if (rv != BCM_E_NONE)
   {
       printf("Error (%d), in bcm_field_entry_add entry_id1(%d)\n", rv, Cint_field_compare_specific_offset_get_entry_id);
       return rv;
   }
   printf("10 step: Add an entry (%d) to the FG(%d)\n", Cint_field_compare_specific_offset_get_entry_id, Cint_field_compare_specific_offset_get_fg_id);
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
cint_field_compare_specific_offset_get_destroy(
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
    rv = bcm_field_presel_set(unit, 0, &Cint_field_compare_specific_offset_get_presel_entry_id_ipmf1, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_presel_set\n", proc_name, rv);
        return rv;
    }
    rv = bcm_field_presel_set(unit, 0, &Cint_field_compare_specific_offset_get_presel_entry_id_ipmf2, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_presel_set\n", proc_name, rv);
        return rv;
    }
    rv = bcm_field_entry_delete(unit, Cint_field_compare_specific_offset_get_fg_id, NULL, Cint_field_compare_specific_offset_get_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_delete entry1 pair1\n", proc_name, rv);
        return rv;
    }
    /**
     * Detach the context
     */
    rv = bcm_field_group_context_detach(unit, Cint_field_compare_specific_offset_get_fg_id, Cint_field_compare_specific_offset_get_context_id_ipmf2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_detach pair1\n", proc_name, rv);
        return rv;
    }
    /**
     * Delete the FG
     */
    rv = bcm_field_group_delete(unit, Cint_field_compare_specific_offset_get_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_delete pair1\n", proc_name, rv);
        return rv;
    }
    /**
     * Compare destroy the first pair_id
     */
    rv = bcm_field_context_compare_destroy(unit, bcmFieldStageIngressPMF1, Cint_field_compare_specific_offset_get_context_id_ipmf1, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_compare_destroy pair1\n", proc_name, rv);
        return rv;
    }
    /**
     * Compare destroy the second pair_id
     */
    rv = bcm_field_context_compare_destroy(unit, bcmFieldStageIngressPMF1, Cint_field_compare_specific_offset_get_context_id_ipmf1, 1);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_compare_destroy pair2\n", proc_name, rv);
        return rv;
    }
    /**
    * Destroy the context
    */
    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF2, Cint_field_compare_specific_offset_get_context_id_ipmf2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_destroy\n", proc_name, rv);
        return rv;
    }
    /**
     * Destroy the context
     */
    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, Cint_field_compare_specific_offset_get_context_id_ipmf1);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_destroy\n", proc_name, rv);
        return rv;
    }
    /**
     * Destroy the user defined qualifier
     */
    rv = bcm_field_qualifier_destroy(unit, Cint_field_compare_specific_offset_get_qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_destroy \n");
        return rv;
    }
return rv;
}

