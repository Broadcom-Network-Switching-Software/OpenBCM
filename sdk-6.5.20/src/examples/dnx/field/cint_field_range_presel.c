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
 * cint ../../../src/examples/dnx/field/cint_field_range_presel.c
 * int unit = 0;
 * bcm_field_stage_t stage = bcmFieldStageIngressPMF1;
 * int range_configuration = 0;
 * cint_field_range_presel_vport_range_main(unit, stage, range_configuration);
 *
 * Configuration example end
 *
 * Cint shows how to configure a range for bcmFieldQualifyVPortRangeCheck presel qual.
 *
 * ************************************************************************
 * Main function:
 * cint_field_range_presel_vport_range_main(unit,stage,range_configuration);
 *
 * Destroy function:
 * cint_field_range_presel_destroy(unit,stage);
 */

/**
  * Global variables
  * {
  */
/**
 * The ID of the FG. Used for creating and for deleting.
 */
bcm_field_group_t CINT_FIELD_RANGE_PRESEL_FG_ID = 16;
/**
 * The ID of the context
 */
bcm_field_context_t CINT_FIELD_RANGE_PRESEL_CONTEXT_ID = 9;
/**
 * The ID of the context
 */
bcm_field_presel_t CINT_FIELD_RANGE_PRESEL_ID = 10;
/**
 * The range_id. Used for creating and reseting a range.
 */
bcm_field_range_t CINT_FIELD_RANGE_PRESEL_RANGE_ID = 0;
 /**
  * The entry_id. Used for creating and destroying the entry
  */
bcm_field_entry_t entry_id;
 /**
  * The presel_entry. Used for creating and destroying the presel
  */
bcm_field_presel_entry_id_t presel_entry_id;

/**
 * The range_info. Used for creating and destroying a range.
 */
bcm_field_range_info_t range_info;

/**
 * \brief
 *
 *  This function contains the bcm_field_presel_set
 *  application and bcm_field_context_create.
 *  This function sets all required HW
 *  configuration for bcm_field_context_create and bcm_field_presel_set
 *  to be performed for IPMF1 and IPMF3
 *
 * \param [in] unit - The unit number.
 * \param [in] context_id - The ID of the context
 * \param [in] stage - The field stage.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int
cint_field_range_presel_context_create_presel_set(
    int unit,
    bcm_field_context_t context_id,
    bcm_field_stage_t stage)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_info_t context_info;
    char *proc_name;
    void *dest_char;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_range_presel_context_create_presel_set";
    /**
     * Adding the proper stage for each case which is tested.
     * For IPMF1 and IPMF2 we are creating a preselector in IPMF1 stage.
     */
    if (stage == bcmFieldStageIngressPMF2)
    {
        stage = bcmFieldStageIngressPMF1;
    }
   /**
    * Init the context_info
    */
    bcm_field_context_info_t_init(&context_info);
    /**
     * Create the context with ID
     */
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "range_presel_context", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, BCM_FIELD_FLAG_WITH_ID, stage, &context_info, &context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_create\n", proc_name, rv);
        return rv;
    }
   /**
    * Fill the info in bcm_field_presel_entry_data_t and bcm_field_presel_entry_id_t structures
    */
    presel_entry_id.presel_id = CINT_FIELD_RANGE_PRESEL_ID;
    presel_entry_id.stage = stage;
    presel_entry_data.context_id = context_id;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyVPortRangeCheck;
    /**
     * We are using range 0, so bit 0 from the qual_value is set.
     */
    presel_entry_data.qual_data[0].qual_value = 0x0;
    presel_entry_data.qual_data[0].qual_mask = 0x7;
    /**
     * We are setting it to 0 because we want to indicate the outmost outlif.
     */
    presel_entry_data.qual_data[0].qual_arg = 0;
    /**
     * Set the presel
     */
    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_presel_set\n", proc_name, rv);
        return rv;
    }
    return rv;
}
/**
 * \brief
 *
 *  This function sets all required HW
 *  configuration for bcm_field_group_add and bcm_field_group_context_attach
 *  and bcm_field_entry_add
 *  to be performed for IPMF1,IPMF2 and IPMF3
 *
 * \param [in] unit - The unit number.
 * \param [in] context_id - The ID of the context
 * \param [in] stage - The field_stage
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int
cint_field_range_presel_fg_create_context_attach_entry_add(
    int unit,
    bcm_field_context_t context_id,
    bcm_field_stage_t stage)
{
    char *proc_name;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    void *dest_char;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_range_presel_fg_create_context_attach_entry_add";
    /**
     * Init the fg_info structure.
     */
    bcm_field_group_info_t_init(&fg_info);
    /**
     * Fill the fg_info structure
     */
    fg_info.stage = stage;
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.nof_quals = 1;
    fg_info.nof_actions = 1;
    fg_info.qual_types[0] = bcmFieldQualifyVrf;
    fg_info.action_types[0] = bcmFieldActionForward;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "range_presel_FG", sizeof(fg_info.name));
    /**
     * Add the FG.
     */
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &CINT_FIELD_RANGE_PRESEL_FG_ID);
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
     * Fill the attach_info strucuture.
     */
    attach_info.key_info.nof_quals = 1;
    attach_info.payload_info.nof_actions = 1;
    attach_info.key_info.qual_types[0] = bcmFieldQualifyVrf;
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.payload_info.action_types[0] = bcmFieldActionForward;
    /**
     * Attach the FG to the context.
     */
    rv = bcm_field_group_context_attach(unit, 0, CINT_FIELD_RANGE_PRESEL_FG_ID, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
        return rv;
    }
   /**
    * Fill the entry_info.
    */
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.priority = 1;

    entry_info.nof_entry_quals = 1;
    entry_info.entry_qual[0].type = bcmFieldQualifyVrf;
    entry_info.entry_qual[0].value[0] = 0x64;
    entry_info.entry_qual[0].mask[0] = 0x3FFFF;

    entry_info.nof_entry_actions = 1;
    entry_info.entry_action[0].type = bcmFieldActionForward;
    entry_info.entry_action[0].value[0] = 0xabcd;
    /**
     * Add an entry to the FG
     */
    rv = bcm_field_entry_add(unit, 0, CINT_FIELD_RANGE_PRESEL_FG_ID, &entry_info, &entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add ipmf1 entry_id(%d)\n", rv, entry_id);
        return rv;
    }
    return rv;
}

/**
 *
 * \brief
 *
 *  This function sets the range for bcmFieldRangeTypeOutVport range type.
 *
 * \param [in] unit - The unit number.
 * \param [in] stage - The field_stage
 * \param [in] range_configuration - The iterator from the TCL test.
 * It is iterating between the 3 kind of ranges that the test sets.
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int
cint_field_range_presel_vport_range_set(
    int unit,
    bcm_field_stage_t stage,
    int range_configuration)
{
    bcm_field_range_info_t range_info;
    char *proc_name;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_range_presel_vport_range_set";
    /**
     * Init the fg_info structure.
     */
    bcm_field_range_info_t_init(&range_info);
    range_info.range_type = bcmFieldRangeTypeOutVport;
    /**
     * Adding a proper minimum and maximum range for each case.
     * The value of the Vport which is configured in the TCL test is 10000.
     * We are using 3 different configurations(3 different min and max ranges)
     * for the same range_id.
     */
    switch (range_configuration)
    {
        /**
         * The valid range
         */
        case 0:
        {
            range_info.min_val = 9000;
            range_info.max_val = 20000;
            break;
        }
        /**
         * The range bellow the minimum [20:9000]
         */
        case 1:
        {
            range_info.min_val = 20;
            range_info.max_val = 9000;
            break;
        }
        /**
         * The range above the maximum [110000:20000]
         */
        case 2:
        {
            range_info.min_val = 11000;
            range_info.max_val = 20000;
            break;
        }
    }
    /**
     * Set the range.
     */
    rv = bcm_field_range_set(unit, 0, stage, CINT_FIELD_RANGE_PRESEL_RANGE_ID, &range_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_range_set\n", proc_name, rv);
        return rv;
    }
    return rv;
}
/**
 *
 * \brief
 *
 *  This function sets the range for bcmFieldRangeTypeOutVport range type.
 *  Creates a Context and a preselector.
 *  Creates a FG, attach it to the context and add an entry.
 *
 * \param [in] unit - The unit number.
 * \param [in] stage - The field_stage
 * \param [in] range_configuration - It is iterating between the 3 kind of ranges
 *  that the test sets (min, max and legit).
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int
cint_field_range_presel_vport_range_main(
    int unit,
    bcm_field_stage_t stage,
    int range_configuration)
{
    char *proc_name;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_range_presel_vport_range_main";
    /**
     * Set the range.
     */
    rv = cint_field_range_presel_vport_range_set(unit, stage, range_configuration);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_range_presel_vport_range_set\n", proc_name, rv);
        return rv;
    }
    /**
     * Create a context and set a preselector.
     */
    rv = cint_field_range_presel_context_create_presel_set(unit, CINT_FIELD_RANGE_PRESEL_CONTEXT_ID, stage);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_fem_context_create_presel_set\n", proc_name, rv);
        return rv;
    }
    /**
     * Create a FG, attach it to a context and add an entry.
     */
    rv = cint_field_range_presel_fg_create_context_attach_entry_add(unit, CINT_FIELD_RANGE_PRESEL_CONTEXT_ID, stage);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_range_presel_fg_create_context_attach_entry_add\n", proc_name, rv);
        return rv;
    }
    return rv;
}
/**
 * The destroy function of the test, to be called from the TCL test.
 * It is deleting the whole configuration.
 * \param [in] unit - The unit number.
 * \param [in] stage - The field_stage
 * It is iterating between the 3 supported stages.
 *
 *  \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int
cint_field_range_presel_destroy(
    int unit,
    bcm_field_stage_t stage)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    int rv = BCM_E_NONE;

   /**
    *  Set the preselector with default values
    */
    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_presel_set\n");
        return rv;
    }
    /**
     *  Set the range with default values
     */
    bcm_field_range_info_t_init(&range_info);
    range_info.range_type = bcmFieldRangeTypeOutVport;
    rv = bcm_field_range_set(unit, 0, stage, CINT_FIELD_RANGE_PRESEL_RANGE_ID, &range_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_range_set\n", rv);
        return rv;
    }
    /**
     * Delete the created entry.
     */
    rv = bcm_field_entry_delete(unit, CINT_FIELD_RANGE_PRESEL_FG_ID, NULL, entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }
    /**
     * Detach the context.
     */
    rv = bcm_field_group_context_detach(unit, CINT_FIELD_RANGE_PRESEL_FG_ID, CINT_FIELD_RANGE_PRESEL_CONTEXT_ID);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_dnx_field_group_context_detach\n", rv);
        return rv;
    }
    /**
     * Delete the FG.
     */
    rv = bcm_field_group_delete(unit, CINT_FIELD_RANGE_PRESEL_FG_ID);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete\n", rv);
        return rv;
    }
    /**
     * Adding the proper stage for each case which is tested.
     * For IPMF1 and IPMF2 the context is in IPMF1 stage.
     */
    if (stage == bcmFieldStageIngressPMF2)
    {
        stage = bcmFieldStageIngressPMF1;
    }
    /**
     * Destroy the created context
     */
    rv = bcm_field_context_destroy(unit, stage, CINT_FIELD_RANGE_PRESEL_CONTEXT_ID);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_context_destroy\n");
        return rv;
    }
    return rv;
}
