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
 * cint ../../../src/examples/dnx/field/cint_field_key_gen_var.c
 * cint;
 * int unit = 0;
 * bcm_field_stage_t stage = bcmFieldStageIngressPMF1;
 * bcm_field_context_t context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;
 * cint_field_key_gen_var_main(unit, stage, context_id);
 *
 * Configuration example end
 *
 * This CINT shows how to create constant values per context and set it as qualifier.
 *
 *
 * The main function:
 * cint_field_key_gen_var_main(unit, stage)
 *
 * In the function, depends of the stage we are doing this steps:
 * !Note that for ipmf1/2 the same key_gen_var is set.
 *    1. Create a context.
 *    2. Set the context param to be bcmFieldContextParamTypeKeyVal
 *    3. Set a preselector.
 *    4. Create a FG.
 *    5. Attach a FG to a context.
 *    6. Set a preselector.
 *    7. Add an entry to the FG.
 *
 * The destroy function is destroying all configuration depends of the stage:
 * cint_field_key_gen_var_destroy(unit, stage)
 */

 /**
  * Global variables
  * {
  */
bcm_field_group_t Cint_field_key_gen_var_fg_id;
  /**
  * The entry_id. Used for creating and destroying the entry
  */
bcm_field_entry_t Cint_field_key_gen_var_entry_id;
 /**
  * The value for bcmFieldQualifyKeyGenVar that we add in the
  *  bcm_field_context_param_set and bcm_field_entry_add.
  */
uint32 Cint_field_key_gen_var_qual_value = 0xABCDEF;

 /**
  * }
  */

/**
 * The test flow:
 *    1. Create a context.
 *    2. Set the context param to be bcmFieldContextParamTypeKeyVal.
 *       !Note that for ipmf1/2 the same key_gen_var is set.
 *    3. Set a preselector.
 *    4. Create a FG.
 *    5. Attach a FG to a context.
 *    6. Set a preselector.
 *    7. Add an entry to the FG.
 *
 * Do this for each stage (IPMF1, IPMF2 and IPMF3).
 * 
 * \param [in] unit - The unit number.
 * \param [in] stage - The field stage.
 * \param [in] context_id - The context_id.
 *
 *  \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int
cint_field_key_gen_var_main(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id)
{
    char *proc_name;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_param_info_t param_info;
    bcm_field_entry_info_t entry_info;
    bcm_field_stage_t context_stage;
    void *dest_char;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_key_gen_var_main";
    /**
     * Fill the info for context_param_set
     */
    param_info.param_type = bcmFieldContextParamTypeKeyVal;
    param_info.param_val = Cint_field_key_gen_var_qual_value;
    param_info.param_arg = 0;
    /**
     * set the context param to be bcmFieldContextParamTypeKeyVal
     */
    if (stage == bcmFieldStageIngressPMF2)
    {
        context_stage = bcmFieldStageIngressPMF1;
    }
    else
    {
        context_stage = stage;
    }
    rv = bcm_field_context_param_set(unit, 0, context_stage, context_id, &param_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_param_set\n", proc_name, rv);
        return rv;
    }
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
    fg_info.qual_types[0] = bcmFieldQualifyKeyGenVar;
    fg_info.action_types[0] = bcmFieldActionForward;
    /**
     * Add the FG.
     */
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "key_gen_var_FG", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &Cint_field_key_gen_var_fg_id);
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
     * Fill the attach_info structure.
     */
    attach_info.key_info.nof_quals = 1;
    attach_info.payload_info.nof_actions = 1;
    attach_info.key_info.qual_types[0] = bcmFieldQualifyKeyGenVar;
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.payload_info.action_types[0] = bcmFieldActionForward;
    /**
     * Attach the FG to the context.
     */
    rv = bcm_field_group_context_attach(unit, 0, Cint_field_key_gen_var_fg_id, context_id, &attach_info);
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

    entry_info.entry_qual[0].type = bcmFieldQualifyKeyGenVar;
    entry_info.entry_qual[0].value[0] = Cint_field_key_gen_var_qual_value;
    entry_info.entry_qual[0].mask[0] = 0xFFFFFF;

    entry_info.nof_entry_actions = 1;
    entry_info.entry_action[0].type = bcmFieldActionForward;
    entry_info.entry_action[0].value[0] = 0xc9;
    /**
     * Add an entry to the FG
     */
    rv = bcm_field_entry_add(unit, 0, Cint_field_key_gen_var_fg_id, &entry_info, &Cint_field_key_gen_var_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add ipmf1 entry_id(%d)\n", rv, Cint_field_key_gen_var_entry_id);
        return rv;
    }
    return rv;
}

/**
 * The destroy function of the test, to be called from the TCL test.
 * It is deleting the whole configuration.
 * \param [in] unit - The unit number.
 * \param [in] stage - The field stage.
 * \param [in] context_id - The context ID.
 *
 *  \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int
cint_field_key_gen_var_destroy(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id)
{
    int rv = BCM_E_NONE;
    char * proc_name = "cint_field_key_gen_var_destroy";
    /**
     * Delete the created entry.
     */
    rv = bcm_field_entry_delete(unit, Cint_field_key_gen_var_fg_id, NULL, Cint_field_key_gen_var_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_delete\n", proc_name, rv);
        return rv;
    }
    /**
     * Detach the context.
     */
    rv = bcm_field_group_context_detach(unit, Cint_field_key_gen_var_fg_id, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_dnx_field_group_context_detach\n", proc_name, rv);
        return rv;
    }
    /**
     * Delete the FG.
     */
    rv = bcm_field_group_delete(unit, Cint_field_key_gen_var_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_delete\n", proc_name, rv);
        return rv;
    }
    return rv;
}
