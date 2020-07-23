/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/**
 *  Testing UDH Cascading.
 *
 *  Configuration example start:
 *
 *  cint;
 *  cint_reset();
 *  exit;
 *  cint ../../src/examples/dnx/field/cint_field_presel_fwd_layer.c
 *  cint ../../src/examples/dnx/field/cint_field_fem_tcam_double_key.c
 *  cint;
 *  int unit = 0;
 *  bcm_field_context_t context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;
 *  bcm_field_presel_t ipmf1_presel_id = 50;
 *  bcm_field_stage_t ipmf1_stage = bcmFieldStageIngressPMF1;
 *  bcm_field_layer_type_t fwd_layer = bcmFieldLayerTypeEth;
 *  field_presel_fwd_layer_main(unit, ipmf1_presel_id, ipmf1_stage, fwd_layer, &context_id, "ctx_ipmf1");
 *  cint_field_fem_tcam_double_key_main(unit, context_id);
 *
 *  Configuration example end
 *
 *  Test scenario:
 *      Configure double key TCAM field group with 128b payload in IPMF1 stage, which is using FEM actions.
 *      Attach it to a context and add an entry to it. Context Selection is done according
 *      to Forwarding type bcmFieldLayerTypeEth.
 *
 *      1. FES actions:
 *
 *             ___________________ ___________ _________________________ _____________ ___________
 * Action:    |  VlanActionSetNew |  StatId0  |  IngressTimeStampInsert |  Container  |  Forward  |
 *             ~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~ ~~~~~~~~~~~
 * Value:     |  0x1F             |  0x1237   |        0x5678           |  0x4321     |  0xABCD   |
 *             ~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~ ~~~~~~~~~~~
 *           MSB                                                                                 LSB
 *      Note all the actions had valid bit!
 *
 *      2. FEM actions:
 *                  _________________________ ___________________ _________________ ______________
 * Payload value:  |         3f012378        |      0002b3c4     |     00010c86    |   0001579b   |
 *                  ~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~
 * FEM ID:         |          FEM4           |       FEM3        |      FEM2       |     FEM1     |
 *                  ~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~
 * FEM Action:     |  EgressForwardingIndex  |  FabricHeaderSet  |  PphPresentSet  |  PrioIntNew  |
 *                  ~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~
 * Input Offset    |           96            |        64         |       32        |      0       |
 *                  ~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~
 * Condition msb   |           3             |        3          |       3         |      3       |
 *                  ~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~
 * Condition val:  |           8             |        4          |       6         |     11       |
 *                  ~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~
 */

/** Offset of ETH_TYPE Qualifier for ETH Header[111:96] (16b) */
int Cint_field_fem_tcam_double_key_user_qual_eth_type_offset = 96;
/** Length of ETH_TYPE Qualifier for EHT Header[111:96] (16b) */
int Cint_field_fem_tcam_double_key_user_qual_eth_type_length = 16;
int Cint_field_fem_tcam_double_key_nof_fems = 4;
int Cint_field_fem_tcam_double_key_nof_quals = 1;
int Cint_field_fem_tcam_double_key_fem_fg_nof_actions = 5;

/**
 * Structure, which contains information,
 * for creating of user qualifiers and predefined one.
 */
struct cint_field_fem_tcam_double_key_qual_info_t {
    char * name;
    int qual_size;
    bcm_field_qualify_t qual_type;      /* Qualifier type. */
    uint32 qual_value;          /* Qualifier value. */
    uint32 qual_mask;           /* Qualifier mask. */
    bcm_field_qualify_attach_info_t qual_attach_info;   /* Qualifier Attach info. */
};

/**
 * Structure, which contains needed action information.
 */
struct cint_field_fem_tcam_double_key_action_info_t
{
    bcm_field_action_t action_type;     /* Action type. */
    uint32 action_value;        /* Action value. */
};

/**
 * Structure, which contains need information
 * for configuring of FEM condition actions.
 */
struct cint_field_fem_tcam_double_key_fem_action_info_t {
    int nof_conditions;
    int condition_ids[BCM_FIELD_NUMBER_OF_CONDITIONS_PER_FEM];
    int nof_extractions;
    int nof_mapping_bits;
    /*
     * Structure replacing FEM_ID to indicate selected FEM by
     * its priority in the general FES/FEM structure.
     * See DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET
     */
    bcm_field_array_n_position_t field_array_n_position;
    bcm_field_fem_action_info_t fem_action_info;
};

/**
 * Structure, which contains all needed information
 * about configuring of one field group.
 */
struct cint_field_fem_tcam_double_key_fg_info_t {
    bcm_field_group_t fg_id;
    char * fg_name;
    bcm_field_stage_t stage;
    bcm_field_group_type_t fg_type;
    bcm_field_entry_t entry_handle;
    int nof_quals;
    int nof_actions;
    cint_field_fem_tcam_double_key_qual_info_t * qual_info;
    cint_field_fem_tcam_double_key_action_info_t * action_info;
    int nof_fems;
    cint_field_fem_tcam_double_key_fem_action_info_t * fem_action_info;
};

/** USER DEFINE Qualifiers info per field stage (IPMF1-2). */
cint_field_fem_tcam_double_key_qual_info_t Cint_field_fem_tcam_double_key_user_qual_info_array[Cint_field_fem_tcam_double_key_nof_quals] = {
 /**  qual_type   |   qual_value   |   qual_mask    |  input_type    |    input_arg  |  qual_offset  */
    {"fem_tcam_double", Cint_field_fem_tcam_double_key_user_qual_eth_type_length,
     bcmFieldQualifyCount, 0x8100, 0xFFFF, {bcmFieldInputTypeLayerAbsolute, 0, Cint_field_fem_tcam_double_key_user_qual_eth_type_offset}},
};

/** IPMF1 Actions info. */
cint_field_fem_tcam_double_key_action_info_t Cint_field_fem_tcam_double_key_fem_fg_action_info_array[Cint_field_fem_tcam_double_key_fem_fg_nof_actions] = {
 /** action_type       |   action_value   */
    {bcmFieldActionForward, 0xABCD},
    {bcmFieldActionContainer, 0x4321},
    {bcmFieldActionIngressTimeStampInsert, 0x5678},
    {bcmFieldActionStatId0, 0x1237},
    {bcmFieldActionVlanActionSetNew, 0x1F}
};

static cint_field_fem_tcam_double_key_fem_action_info_t Cint_field_fem_tcam_double_key_fem_action_info_array[Cint_field_fem_tcam_double_key_nof_fems] = {
/** nof_conditions | condition_indexes | nof_extractions | nof_map_bits | array_id | fem_position_in_array | fem_input | condition_msb | fem_condition_info */
    {1, {11}, 1, 3, {3, 3}, {{0, 0}, 3, {{0, 1}},
        /** bcm_action  |  extraction_info */
        {{bcmFieldActionPrioIntNew, {
            /** src_type                 |       offset | force_value */
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},}, 0}}}},

    {1, {6}, 1, 2, {3, 4}, {{32, 0}, 3, {{0, 1}},
        /** bcm_action  |  extraction_info */
        {{bcmFieldActionPphPresentSet, {
            /** src_type                 |       offset | force_value */
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},}, 0}}}},

    {1, {4}, 1, 4, {3, 5}, {{64, 0}, 3, {{0, 1}},
        /** bcm_action  |  extraction_info */
        {{bcmFieldActionFabricHeaderSet, {
            /** src_type                 |       offset | force_value */
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},}, 0}}}},

    {1, {8}, 1, 3, {3, 6}, {{96, 0}, 3, {{0, 1}},
        /** bcm_action  |  extraction_info */
        {{bcmFieldActionEgressForwardingIndex, {
            /** src_type                 |       offset | force_value */
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},}, 0}}}},
};

/**
 * Following array is global, used for all test cases. They contain information per field group. Like:
 * fg_id, fg_type, context selection, quals, actions, fems.
 */
cint_field_fem_tcam_double_key_fg_info_t Cint_field_fem_tcam_double_key_fg_info_array[1] = {
    /** FEM TCAM FG info. */
    {
     0,         /** Field group ID */
     "fem_tcam_double_ipmf1",          /** Field group Name */
     bcmFieldStageIngressPMF1,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                           /* The Entry handle. */
     Cint_field_fem_tcam_double_key_nof_quals,        /** Number of tested qualifiers */
     Cint_field_fem_tcam_double_key_fem_fg_nof_actions,       /** Number of tested actions */
     Cint_field_fem_tcam_double_key_user_qual_info_array,         /** Qualifiers info */
     Cint_field_fem_tcam_double_key_fem_fg_action_info_array,        /** Actions info */
     Cint_field_fem_tcam_double_key_nof_fems,
     Cint_field_fem_tcam_double_key_fem_action_info_array
    }
};

/**
 * \brief
 *  Used to configure the user qualifiers.
 *
 * \param [in] unit - The unit number.
 * \param [in] fem_qual_info - Contains all need information
 *                              for one user qualifier to be created.
 */
int
cint_field_fem_tcam_double_key_user_qual_config(
    int unit,
    cint_field_fem_tcam_double_key_qual_info_t * fem_qual_info)
{
    int rv = BCM_E_NONE;
    bcm_field_qualifier_info_create_t qual_info;
    void *dest_char;

    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, fem_qual_info->name, sizeof(qual_info.name));
    qual_info.size = fem_qual_info->qual_size;
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &(fem_qual_info->qual_type));
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create \n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *  This function is configuring all needed FEM actions.
 *
 * \param [in] unit - The unit number.
 * \param [in] fg_id - The field group ID, to which the FEM action
 *                     should be added.
 * \param [in] fem_action_info_p - Contains all need information
 *                                    for one FEM action to be constructed.
 */
int
cint_field_fem_tcam_double_key_fem_action_config(
    int unit,
    bcm_field_group_t fg_id,
    cint_field_fem_tcam_double_key_fem_action_info_t * fem_action_info_p)
{
    int rv = BCM_E_NONE;
    int mapping_bits_index, conditions_index, extractions_index;
    bcm_field_fem_action_info_t fem_action_info;
    int *condition_id = fem_action_info_p->condition_ids;
    bcm_field_action_priority_t action_priority;

    bcm_field_fem_action_info_t_init(&fem_action_info);

    fem_action_info.fem_input.input_offset = fem_action_info_p->fem_action_info.fem_input.input_offset;
    fem_action_info.condition_msb = fem_action_info_p->fem_action_info.condition_msb;
    for (conditions_index = 0; conditions_index < fem_action_info_p->nof_conditions; conditions_index++)
    {
        fem_action_info.fem_condition[condition_id[conditions_index]].extraction_id =
            fem_action_info_p->fem_action_info.fem_condition[conditions_index].extraction_id;
        fem_action_info.fem_condition[condition_id[conditions_index]].is_action_valid =
            fem_action_info_p->fem_action_info.fem_condition[conditions_index].is_action_valid;
    }
    for (extractions_index = 0; extractions_index < fem_action_info_p->nof_extractions; extractions_index++)
    {
        fem_action_info.fem_extraction[extractions_index].action_type =
            fem_action_info_p->fem_action_info.fem_extraction[extractions_index].action_type;
        fem_action_info.fem_extraction[extractions_index].increment =
            fem_action_info_p->fem_action_info.fem_extraction[extractions_index].increment;
        for (mapping_bits_index = 0; mapping_bits_index < fem_action_info_p->nof_mapping_bits;
             mapping_bits_index++)
        {
            fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].offset =
                fem_action_info_p->fem_action_info.fem_extraction[extractions_index].
                output_bit[mapping_bits_index].offset;
            fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].forced_value =
                fem_action_info_p->fem_action_info.fem_extraction[extractions_index].
                output_bit[mapping_bits_index].forced_value;
            fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].source_type =
                fem_action_info_p->fem_action_info.fem_extraction[extractions_index].
                output_bit[mapping_bits_index].source_type;
        }
    }

    int array_id, fem_position_in_array;
    array_id = fem_action_info_p->field_array_n_position.array_id;
    fem_position_in_array = fem_action_info_p->field_array_n_position.fem_position_in_array;
    action_priority = BCM_FIELD_ACTION_POSITION(array_id, fem_position_in_array);
    rv = bcm_field_fem_action_add(unit, 0, fg_id, action_priority, &fem_action_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_fem_action_add \n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *  This function creates field group, with all
 *  relevant information to it and adding it to the context.
 *
 * \param [in] unit - The unit number.
 * \param [in] context_id - Context Id.
 * \param [in] fem_fg_info - Contains all need information
 *                            for one group to be created.
 */
int
cint_field_fem_tcam_double_key_group_config(
    int unit,
    bcm_field_context_t context_id,
    cint_field_fem_tcam_double_key_fg_info_t * fem_fg_info)
{
    int rv = BCM_E_NONE;
    int qual_index, action_index, fem_index;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    void *dest_char;

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_entry_info_t_init(&entry_info);

    fg_info.fg_type = fem_fg_info->fg_type;
    fg_info.stage = fem_fg_info->stage;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, fem_fg_info->fg_name, sizeof(fg_info.name));
    fg_info.nof_quals = fem_fg_info->nof_quals;
    fg_info.nof_actions = fem_fg_info->nof_actions;
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.priority = 1;

    /** Set qualifiers. */
    for (qual_index = 0; qual_index < fg_info.nof_quals; qual_index++)
    {
        fg_info.qual_types[qual_index] = fem_fg_info->qual_info[qual_index].qual_type;
        attach_info.key_info.qual_types[qual_index] = fg_info.qual_types[qual_index];
        attach_info.key_info.qual_info[qual_index].input_type =
            fem_fg_info->qual_info[qual_index].qual_attach_info.input_type;
        attach_info.key_info.qual_info[qual_index].input_arg =
            fem_fg_info->qual_info[qual_index].qual_attach_info.input_arg;
        attach_info.key_info.qual_info[qual_index].offset =
            fem_fg_info->qual_info[qual_index].qual_attach_info.offset;
        entry_info.entry_qual[qual_index].type = fg_info.qual_types[qual_index];
        entry_info.entry_qual[qual_index].value[0] = fem_fg_info->qual_info[qual_index].qual_value;
        entry_info.entry_qual[qual_index].mask[0] = fem_fg_info->qual_info[qual_index].qual_mask;
    }

    /** Set actions. */
    for (action_index = 0; action_index < fg_info.nof_actions; action_index++)
    {
        fg_info.action_types[action_index] = fem_fg_info->action_info[action_index].action_type;
        fg_info.action_with_valid_bit[action_index] = TRUE;
        attach_info.payload_info.action_types[action_index] = fg_info.action_types[action_index];
        entry_info.entry_action[action_index].type = fg_info.action_types[action_index];
        entry_info.entry_action[action_index].value[0] = fem_fg_info->action_info[action_index].action_value;
    }

    /** Create the field group. */
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &(fem_fg_info->fg_id));
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add on field_group_id = %d \n", rv, fem_fg_info->fg_id);
        return rv;
    }

    printf("Field Group ID %d was created. \n", fem_fg_info->fg_id);

    /** Configuring the FEM actions. */
    if (fem_fg_info->fem_action_info != NULL)
    {
        for (fem_index = 0; fem_index < fem_fg_info->nof_fems; fem_index++)
        {
            rv = cint_field_fem_tcam_double_key_fem_action_config
                            (unit, fem_fg_info->fg_id, &(fem_fg_info->fem_action_info[fem_index]));
            if (rv != BCM_E_NONE)
            {
                printf("Error (%d), in cint_field_fem_tcam_double_key_fem_action_config\n", rv);
                return rv;
            }
        }

        printf("FEM actions were added to Field Group ID %d. \n", fem_fg_info->fg_id);
    }

    /** Attach the created field group to the context. */
    rv = bcm_field_group_context_attach(unit, 0, fem_fg_info->fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach on contex_id = %d \n", rv, context_id);
        return rv;
    }

    printf("Field Group ID %d was attached to Context ID %d. \n", fem_fg_info->fg_id, context_id);

    /** Add an entry to the created field group. */
    rv = bcm_field_entry_add(unit, 0, fem_fg_info->fg_id, &entry_info, &(fem_fg_info->entry_handle));
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    printf("Entry ID 0x%X (%d) was added to Field Group ID %d. \n", fem_fg_info->entry_handle,
           fem_fg_info->entry_handle, fem_fg_info->fg_id);

    return rv;
}

/**
 * \brief
 *  This function creates TCAM FG with payload of 128b
 *  and shows usage of FEMs.
 *
 * \param [in] unit - The unit number.
 * \param [in] context_id - Context Id.
 */
int
cint_field_fem_tcam_double_key_main(
    int unit,
    bcm_field_context_t context_id)
{
    int rv = BCM_E_NONE;

    rv = cint_field_fem_tcam_double_key_user_qual_config(unit, &(Cint_field_fem_tcam_double_key_fg_info_array[0].qual_info[0]));
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_fem_tcam_double_key_user_qual_config\n", rv);
        return rv;
    }

    /** Creating FEM TCAM FG. */
    rv = cint_field_fem_tcam_double_key_group_config(unit, context_id, Cint_field_fem_tcam_double_key_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_fem_tcam_double_key_group_config\n", rv);
        return rv;
    }

    return rv;
}

/**
* \brief
*  Destroy all configuration done
* \param [in] unit        -  Device id
* \param [in] context_id  -  Context id
* \param [in] fem_fg_info  -  Array with all info to be destroyed.
*
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
cint_field_fem_tcam_double_key_destroy_internal(
    int unit,
    bcm_field_context_t context_id,
    cint_field_fem_tcam_double_key_fg_info_t * fem_fg_info)
{
    int rv = BCM_E_NONE;
    int qual_index, fem_index;

    rv = bcm_field_entry_delete(unit, fem_fg_info->fg_id, NULL, fem_fg_info->entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, fem_fg_info->fg_id, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }

    for (fem_index = 0; fem_index < fem_fg_info->nof_fems; fem_index++)
    {
        bcm_field_action_priority_t action_priority;
        int array_id, position;

        array_id = fem_fg_info->fem_action_info[fem_index].field_array_n_position.array_id;
        position = fem_fg_info->fem_action_info[fem_index].field_array_n_position.fem_position_in_array;
        action_priority = BCM_FIELD_ACTION_POSITION(array_id,position);
        rv = bcm_field_fem_action_delete(unit, fem_fg_info->fg_id, action_priority);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_fem_action_delete. \n", rv);
            return rv;
        }
    }

    rv = bcm_field_group_delete(unit, fem_fg_info->fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_delete\n");
        return rv;
    }

    for (qual_index = 0; qual_index < fem_fg_info->nof_quals; qual_index++)
    {
        rv = bcm_field_qualifier_destroy(unit, fem_fg_info->qual_info[qual_index].qual_type);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_qualifier_destroy \n");
            return rv;
        }
    }

    return rv;
}

/**
* \brief
*  Destroy all configuration done
* \param [in] unit        -  Device id
* \param [in] context_id  -  Context id
*
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
cint_field_fem_tcam_double_key_destroy(
    int unit,
    bcm_field_context_t context_id)
{
    int rv = BCM_E_NONE;
    int qual_index, fem_index;

    rv = cint_field_fem_tcam_double_key_destroy_internal(unit, context_id, Cint_field_fem_tcam_double_key_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_fem_tcam_double_key_destroy_internal\n", rv);
        return rv;
    }

    return rv;
}
