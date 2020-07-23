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
 * cint ../../../src/examples/dnx/field/cint_field_compare_single.c
 * cint;
 * int unit = 0;
 * int compare_id = 0;
 * cint_field_compare_single_main(unit, compare_id);
 *
 * Configuration example end
 *
 * This is an example of comparing a value stored in initial
 * keys to TCAM result of 1 of the lookups done in iPMF1.
 *
 *
 * Destroy function:
 * cint_field_compare_single_destroy(unit);
 */
/** Variables to store object IDs for deletion. */
bcm_field_context_t Cint_field_compare_single_context_id;
bcm_field_action_t Cint_field_compare_single_action_void;
bcm_field_group_t Cint_field_compare_single_fg_id_ipmf1;
bcm_field_group_t Cint_field_compare_single_fg_id_ipmf2;
bcm_field_entry_t Cint_field_compare_single_ipmf1_entry_id;
/** The number of entries for ipmf2 TCAM Fg. */
int Cint_field_compare_single_nof_entries = 2;
/** Array in which we will collect the entry_id values values. */
bcm_field_entry_t Cint_field_compare_single_ipmf2_entry_id_arr[Cint_field_compare_single_nof_entries] = {
       0,
       0
};

/** The presel_entry. Used for creating and destroying the presel. */
bcm_field_presel_entry_id_t Cint_field_compare_single_presel_entry_id;
/** The ID of the preselector. */
bcm_field_presel_t Cint_field_compare_single_presel_id = 10;
/** The number of quals for preselection. */
int Cint_field_compare_single_presel_nof_quals = 1;
/** The value of the preselector qualifier. */
uint32 Cint_field_compare_single_presel_qual_value = 0x1;
/** The mask of the preselector qualifier. */
uint32 Cint_field_compare_single_presel_qual_mask = 0x1;
/** The L4 src port value. */
int Cint_field_compare_single_src_qual_value = 0x12c;
/**
 * The IPMF2 TCAM qual value.
 * Should be with this value because it should match the qualifiers
 * in the context_compare first_key (L4 SRC and DST port).
 */
int Cint_field_compare_single_tcam0_qual_value = 0x012c012c;
int Cint_field_compare_single_pair_id;

/**
 * \brief
 *
 *  This function contains the bcm_field_context_create and
 *  bcm_field_context_compare_create APIs.
 *  This function is creating a context with mode bcmFieldContextCompareTypeSingle)
 *  and with 0 or 1 pair.
 *
 * \param [in] unit - The unit number.
 * \param [in] stage - The field stage.
 * \param [in] compare_id - The compare_id.
 *  If it is set to 0 the compare_1_mode is set to bcmFieldContextCompareTypeSingle.
 *  If it is set to 1 the compare_2_mode is set to bcmFieldContextCompareTypeSingle.
 * \param [in] context_id_p - The id of the context.
 *
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int
cint_field_compare_single_config(
    int unit,
    bcm_field_stage_t stage,
    int compare_id,
    bcm_field_context_t * context_id_p)
{
    bcm_field_context_info_t context_info;
    bcm_field_context_compare_info_t compare_info;
    char *proc_name;
    void *dest_char;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_compare_config";
    /** Init the context_info. */
    bcm_field_context_info_t_init(&context_info);
    /** Set the compare mode to be bcmFieldContextCompareTypeSingle. */
    if (compare_id == 0)
    {
        context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeSingle;
    }
    else
    {
        context_info.context_compare_modes.compare_2_mode = bcmFieldContextCompareTypeSingle;
    }

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "compare_single_context", sizeof(context_info.name));
    /** Create the context with ID. */
    rv = bcm_field_context_create(unit, 0, stage, &context_info, context_id_p);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_create\n", proc_name, rv);
        return rv;
    }
    /** Init the compare_info. */
    bcm_field_context_compare_info_t_init(&compare_info);
    compare_info.first_key_info.nof_quals = 2;
    compare_info.first_key_info.qual_types[0] = bcmFieldQualifyL4DstPort;
    compare_info.first_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.first_key_info.qual_info[0].input_arg = 2;
    compare_info.first_key_info.qual_info[0].offset = 0;

    compare_info.first_key_info.qual_types[1] = bcmFieldQualifyL4SrcPort;
    compare_info.first_key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.first_key_info.qual_info[1].input_arg = 2;
    compare_info.first_key_info.qual_info[1].offset = 0;

    /** Create the context compare mode. */
    rv = bcm_field_context_compare_create(unit, 0, stage, *context_id_p, Cint_field_compare_single_pair_id, &compare_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_create\n", proc_name, rv);
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
cint_field_compare_single_presel_set(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t * context_id_p)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    char *proc_name;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_compare_single_presel_set";
    /** Fill the info in presel_entry_data and bcm_field_presel_entry_id_t structures. */
    Cint_field_compare_single_presel_entry_id.presel_id = Cint_field_compare_single_presel_id;
    Cint_field_compare_single_presel_entry_id.stage = stage;
    presel_entry_data.context_id = *context_id_p;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.nof_qualifiers = Cint_field_compare_single_presel_nof_quals;
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyVlanFormat;
    presel_entry_data.qual_data[0].qual_value = Cint_field_compare_single_presel_qual_value;
    presel_entry_data.qual_data[0].qual_mask = Cint_field_compare_single_presel_qual_mask;
    /** Set the presel. */
    rv = bcm_field_presel_set(unit, 0, &Cint_field_compare_single_presel_entry_id, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_presel_set\n", proc_name, rv);
        return rv;
    }
    return rv;
}
/**
 * \brief
 *  This function is creating FG in IPMF1 which TCAM result
 *      should be compared to the first_key in context_compare.
 *
 * \param [in] unit - The unit number.
 * \param [in] context_id_p - The context_id.
 * \param [in] compare_id - The compare_id.
 * If it is set to 0 the TCAM B is allocated.
 * If it is set to 1 the TCAM D is allocated.
 *
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int static
cint_field_compare_single_fg_config_ipmf1(
    int unit,
    bcm_field_context_t * context_id_p,
    int compare_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    bcm_field_action_info_t action_info;
    char *proc_name;
    void *dest_char;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_compare_single_fg_config_ipmf1";
    /** Create the actions that we need for the FG add. */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionVoid;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0x0;
    action_info.size = 32;
    action_info.stage = bcmFieldStageIngressPMF1;
    rv = bcm_field_action_create(unit, 0, &action_info, &Cint_field_compare_single_action_void);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_create Cint_field_compare_action_void\n", rv);
        return rv;
    }
    /** Init the fg_info structure. */
    bcm_field_group_info_t_init(&fg_info);
    /** Fill the fg_info structure. */
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyL4SrcPort;

    fg_info.nof_actions = 2;
    fg_info.action_types[0] = bcmFieldActionPrioIntNew;
    fg_info.action_types[1] = Cint_field_compare_single_action_void;
    fg_info.action_with_valid_bit[1] = FALSE;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "compare_single_FG_1", sizeof(fg_info.name));
    /** Create FG without ID. */
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &Cint_field_compare_single_fg_id_ipmf1);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_add\n", proc_name, rv);
        return rv;
    }
    /** Init the attach_info. */
    bcm_field_group_attach_info_t_init(&attach_info);
    /** Fill the attach_info structure. */
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 2;
    attach_info.key_info.qual_info[0].offset = 0;

    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    /**
     * The compare_id must be equal to the pair ID.
     * If compare_id is equal to 0 we are comparing to the key B result.
     * If compare_id is equal to 1 we are comparing to the key D result.
     */
    attach_info.compare_id = compare_id;
    /** Attach the FG to the context. */
    rv = bcm_field_group_context_attach(unit, 0, Cint_field_compare_single_fg_id_ipmf1, Cint_field_compare_single_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
        return rv;
    }
    /** Init the entry_info structure. */
    bcm_field_entry_info_t_init(&entry_info);
    /** Fill the entry_info structure. */
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.nof_entry_actions = 1;
    entry_info.priority = 1;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = Cint_field_compare_single_src_qual_value;
    entry_info.entry_qual[0].mask[0] = 0xFFFF;
    entry_info.entry_action[0].type = fg_info.action_types[1];
    entry_info.entry_action[0].value[0] = Cint_field_compare_single_tcam0_qual_value;

    /** Add an entry to the FG. */
    rv = bcm_field_entry_add(unit, 0, Cint_field_compare_single_fg_id_ipmf1, &entry_info, Cint_field_compare_single_ipmf1_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add entry_id(%d)\n", rv, Cint_field_compare_single_ipmf1_entry_id);
        return rv;
    }

    return rv;
}

/**
 * \brief
 * This function is creating FG in IPMF2 which will show the result of the comparison.
 * The bits in the bcmFieldQualifyCompareKeysTcam0/1 qualifier are how it follows:
 *
 * F initial > TCAM result initial   ( msb )
 * F initial == TCAM result initial
 * F initial => TCAM result initial
 * F initial < TCAM result initial
 * F initial <= TCAM result initial
 * F initial != TCAM result initial
 *
 *
 * \param [in] unit - The unit number.
 * \param [in] context_id_p - The context_id.
 *
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int static
cint_field_compare_single_fg_config_ipmf2(
    int unit,
    bcm_field_context_t * context_id_p)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    bcm_field_action_info_t action_info;
    char *proc_name;
    void *dest_char;
    int op_offset;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_compare_single_fg_config_ipmf2";
    /** Init the fg_info structure. */
    bcm_field_group_info_t_init(&fg_info);
    /** Fill the fg_info structure. */
    fg_info.stage = bcmFieldStageIngressPMF2;
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.nof_quals = 5;
    /** Setting the qual to be bcmFieldQualifyCompareKeysTcam0. So we can see the result from the compare. */
    if (Cint_field_compare_single_pair_id == 0)
    {
        fg_info.qual_types[0] = bcmFieldQualifyCompareKeysTcam0;
    }
    else
    {
        fg_info.qual_types[0] = bcmFieldQualifyCompareKeysTcam1;
    }
    fg_info.qual_types[1] = bcmFieldQualifyDstRpfGport;
    fg_info.qual_types[2] = bcmFieldQualifyOamMepId;
    fg_info.qual_types[3] = bcmFieldQualifyDstPort;
    fg_info.qual_types[4] = bcmFieldQualifyL4PortRangeCheck;
    
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionDropPrecedence;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "compare_single_FG_2", sizeof(fg_info.name));
    /** Create FG without ID. */
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &Cint_field_compare_single_fg_id_ipmf2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_add\n", proc_name, rv);
        return rv;
    }
    /** Init the attach_info. */
    bcm_field_group_attach_info_t_init(&attach_info);
    /** Fill the attach_info structure. */
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_types[3] = fg_info.qual_types[3];
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_types[4] = fg_info.qual_types[4];
    attach_info.key_info.qual_info[4].input_type = bcmFieldInputTypeMetaData;

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    /** Attach the FG to the context. */
    rv = bcm_field_group_context_attach(unit, 0, Cint_field_compare_single_fg_id_ipmf2, Cint_field_compare_single_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
        return rv;
    }

    rv = bcm_field_compare_operand_offset_get(unit, Cint_field_compare_single_pair_id, bcmFieldCompareOperandEqual, &op_offset);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_compare_operand_offset_get\n", proc_name, rv);
        return rv;
    }

    /** Add the first entry. It will provide a signal if the TCAM result B and compare_qual are equal. */
    /** Init the entry_info structure. */
    bcm_field_entry_info_t_init(&entry_info);
    /** Fill the entry_info structure. */
    entry_info.nof_entry_quals = 1;
    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.priority = 1;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = 1 << op_offset;
    entry_info.entry_qual[0].mask[0] = 1 << op_offset;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = 0x2;
    /** Add an entry to the FG. */
    rv = bcm_field_entry_add(unit, 0, Cint_field_compare_single_fg_id_ipmf2, &entry_info, &Cint_field_compare_single_ipmf2_entry_id_arr[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add entry_id(%d)\n", rv, Cint_field_compare_single_ipmf2_entry_id_arr[0]);
        return rv;
    }

    /**
     * Get operand offset only for first pair, to avoid getting negative offset.
     * Then use it as mask for the entry value and mask. 1 << op_offset
     */
    rv = bcm_field_compare_operand_offset_get(unit, 1, bcmFieldCompareOperandNotEqual, &op_offset);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_compare_operand_offset_get\n", proc_name, rv);
        return rv;
    }

    /** Add the second entry. It will provide a signal if the TCAM result B and compare_qual are NOT equal. */
    /** Init the entry_info structure. */
    bcm_field_entry_info_t_init(&entry_info);
    /** Fill the entry_info structure. */
    entry_info.nof_entry_quals = 1;
    entry_info.nof_entry_actions = 1;
    entry_info.priority = 1;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = 1 << op_offset;
    entry_info.entry_qual[0].mask[0] = 1 << op_offset;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = 0x3;
    /** Add an entry to the FG. */
    rv = bcm_field_entry_add(unit, 0, Cint_field_compare_single_fg_id_ipmf2, &entry_info, &Cint_field_compare_single_ipmf2_entry_id_arr[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add entry_id(%d)\n", rv, Cint_field_compare_single_ipmf2_entry_id_arr[1]);
        return rv;
    }
    
    return rv;
}
/**
 * In this test we are creating context in compare_1 mode from type Single
 * with only 1 pair to compare.
 * We are creating 1 ipmf1 TCAM FG in TCAM B and 1 ipmf2 TCAM FG.
 * We are comparing the compare first_key to the TCAM result of ipmf1 TCAM.
 *
 * \param [in] unit - The unit number.
 * \param [in] compare_id - The compare_id.
 * If it is set to 0 the TCAM B is allocated.
 * If it is set to 1 the TCAM D is allocated.
 *  \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int
cint_field_compare_single_main(
    int unit,
    int compare_id)
{
    char *proc_name;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_compare_single_main";

    if (compare_id == 0)
    {
        Cint_field_compare_single_pair_id = 0;
    }
    else
    {
        Cint_field_compare_single_pair_id = 1;
    }
    /** Create the context and the context_compare. */
    printf("Create context compare mode\n");
    rv = cint_field_compare_single_config(unit, bcmFieldStageIngressPMF1, compare_id, Cint_field_compare_single_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_compare_single_presel_set \n", proc_name, rv);
        return rv;
    }
    /** Create a preselector which is going to qualify upon Ethernet header. */
    printf("Create a preselector which is going to qualify upon Ethernet header \n");
    rv = cint_field_compare_single_presel_set(unit, bcmFieldStageIngressPMF1, Cint_field_compare_single_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_compare_single_presel_set \n", proc_name, rv);
        return rv;
    }
    /** Create TCAM Fg and Attach it to the context. */
    printf("Create IPMF1 TCAM Fg and Attach it to the context and add an entry. \n");
    rv = cint_field_compare_single_fg_config_ipmf1(unit, Cint_field_compare_single_context_id, compare_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_compare_single_fg_config_ipmf1 \n", proc_name, rv);
        return rv;
    }
    /** Create TCAM Fg and Attach it to the context. */
    printf("Create IPMF2 TCAM Fg and Attach it to the context and add an entry. \n");
    rv = cint_field_compare_single_fg_config_ipmf2(unit, Cint_field_compare_single_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_compare_single_fg_config_ipmf2 \n", proc_name, rv);
        return rv;
    }

    return rv;
}

/**
 * Destroys all allocated data from the compare configuration.
 *
 * \param [in] unit - The unit number.
 *
 *  \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int
cint_field_compare_single_destroy(
    int unit)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    char *proc_name;
    int entry_iterator;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_compare_single_destroy";
    /** Init and reset the presel. */
    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    rv = bcm_field_presel_set(unit, 0, &Cint_field_compare_single_presel_entry_id, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_presel_set\n", proc_name, rv);
        return rv;
    }
   /** Delete the created entry. */
    rv = bcm_field_entry_delete(unit, Cint_field_compare_single_fg_id_ipmf1, NULL, Cint_field_compare_single_ipmf1_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_delete\n", proc_name, rv);
        return rv;
    }
    /** Detach the context. */
    rv = bcm_field_group_context_detach(unit, Cint_field_compare_single_fg_id_ipmf1, Cint_field_compare_single_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_detach\n", proc_name, rv);
        return rv;
    }
    /** Delete the FG. */
    rv = bcm_field_group_delete(unit, Cint_field_compare_single_fg_id_ipmf1);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_delete\n", proc_name, rv);
        return rv;
    }
    /** Delete the created entry. */
    for (entry_iterator = 0; entry_iterator < Cint_field_compare_single_nof_entries; entry_iterator++)
    {
        rv = bcm_field_entry_delete(unit, Cint_field_compare_single_fg_id_ipmf2, NULL, Cint_field_compare_single_ipmf2_entry_id_arr[entry_iterator]);
        if (rv != BCM_E_NONE)
        {
            printf("%s Error (%d), in bcm_field_entry_delete entry1 pair1\n", proc_name, rv);
            return rv;
        }
    }
    /** Detach the context. */
    rv = bcm_field_group_context_detach(unit, Cint_field_compare_single_fg_id_ipmf2, Cint_field_compare_single_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_detach pair1\n", proc_name, rv);
        return rv;
    }
    /** Delete the FG. */
    rv = bcm_field_group_delete(unit, Cint_field_compare_single_fg_id_ipmf2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_delete pair1\n", proc_name, rv);
        return rv;
    }
    /** Compare destroy of the first_pair. */
    rv = bcm_field_context_compare_destroy(unit, bcmFieldStageIngressPMF1, Cint_field_compare_single_context_id, Cint_field_compare_single_pair_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_compare_destroy pair1\n", proc_name, rv);
        return rv;
    }
    /** Destroy the context. */
    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, Cint_field_compare_single_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_destroy\n", proc_name, rv);
        return rv;
    }
    rv = bcm_field_action_destroy(unit, Cint_field_compare_single_action_void);
    if (rv != BCM_E_NONE)
    {
       printf("Error in bcm_field_action_destroy dest_action\n");
       return rv;
    }

    return rv;
}

