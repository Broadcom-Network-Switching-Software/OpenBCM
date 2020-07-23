/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 *  Testing VlanFormat regular qualifier and context selection qualifier.
 *
 * Configuration example start:
 *
 * cint
 * cint_reset();
 * exit;
 * cint ../../src/examples/dnx/field/cint_field_vlan_format.c
 * cint;
 * int unit = 0;
 * int fg_info_array_index = 0;
 * cint_field_vlan_format_main(unit, fg_info_array_index);
 *
 * Configuration example end
 *
 *  Test scenario:
 *      Configuring a TCAM field group in IPMF1 stage, with relevant qualifiers and actions.
 *      Attaching it to a context and adding an entry to it.
 *      The test is done in 2 cases as follow:
 *          1. Context selection Vlan Format qualifier:
 *               Context selection is done according:
 *                   - bcmFieldQualifyVlanFormat
 *               FG qualifiers:
 *                   - bcmFieldQualifyEtherTpid
 *          2. Regular Vlan Format qualifier:
 *               Context selection is done according:
 *                   - bcmFieldQualifyForwardingType
 *               FG qualifiers:
 *                   - bcmFieldQualifyVlanFormat
 *
 *      Following actions will be performed in case of match:
 *          1. bcmFieldActionPrioIntNew
 */

/** Number of qualifiers for different test cases. (used as array size)*/
int VLAN_FORMAT_NOF_QUALS = 1;
/** Number of actions to be performed. */
int VLAN_FORMAT_NOF_ACTIONS = 1;
/** Value to which the TC signal will be update. */
int VLAN_FORMAT_TC_ACTION_VALUE = 6;

/** Number of Context Selection qualifiers. */
int VLAN_FORMAT_NOF_CS_QUALS = 1;

/** Number of Field Groups (cases). (used as array size) */
int VLAN_FORMAT_NOF_FG = 2;

/**The presel_entry. Used for creating and destroying the presel. */
bcm_field_presel_entry_id_t presel_entry_id;
/**The Entry handle. Used for storing the entry handle to be destroyed. */
bcm_field_entry_t entry_handle;

/**
 * Structure, which contains information,
 * for creating of user actions and predefined one.
 */
struct cint_field_vlan_format_qual_info_t
{
    bcm_field_qualify_t qual_type;      /* Qualifier type. */
    uint32 qual_value;          /* Qualifier value. */
    uint32 qual_mask;           /* Qualifier mask. */
    bcm_field_qualify_attach_info_t qual_attach_info;   /* Qualifier Attach info. */
};

/**
 * Structure, which contains needed action information.
 */
struct cint_field_vlan_format_action_info_t
{
    bcm_field_action_t action_type;     /* Action type. */
    uint32 action_value;        /* Action value. */
};

/**
 * Structure, which contains all needed information
 * about configuring of one field group.
 */
struct cint_field_vlan_format_fg_info_t
{
    bcm_field_group_t fg_id;    /* Field Group ID. */
    bcm_field_context_t context_id;     /* Context ID. */
    bcm_field_stage_t stage;    /* Field stage. */
    bcm_field_group_type_t fg_type;     /* Field Group type. */
    int nof_quals;              /* Number of qualifiers per group. */
    int nof_actions;            /* Number of actions per group. */
    bcm_field_presel_qualify_data_t *vlan_format_cs_info;      /* Context Selection Info. */
    cint_field_vlan_format_qual_info_t *vlan_format_qual_info;  /* Qualifier Info. */
    cint_field_vlan_format_action_info_t *vlan_format_action_info;      /* Action Info. */
};

/** Qualifiers info. */
cint_field_vlan_format_qual_info_t field_vlan_format_qual_info_array[VLAN_FORMAT_NOF_QUALS] = {
  /**  qual_type   |   qual_value   |   qual_mask    |  input_type    |    input_arg  |  qual_offset  */
    {bcmFieldQualifyVlanFormat, BCM_FIELD_VLAN_FORMAT_UNTAGGED, 0xFFFF, {bcmFieldInputTypeMetaData, 0, 0}}
};

/** Regular qualifiers info. */
cint_field_vlan_format_qual_info_t field_vlan_format_regular_qual_info_array[VLAN_FORMAT_NOF_QUALS] = {
  /**  qual_type   |   qual_value   |   qual_mask    |  input_type    |    input_arg  |  qual_offset  */
    {bcmFieldQualifyTpid, 0x8100, 0xFFFF, {bcmFieldInputTypeLayerAbsolute, 0, 96}}
};

/** Action info. */
cint_field_vlan_format_action_info_t field_vlan_format_action_info_array[VLAN_FORMAT_NOF_ACTIONS] = {
    /**     action_type       |   action_value   */
    {bcmFieldActionPrioIntNew, VLAN_FORMAT_TC_ACTION_VALUE}
};

/** Context Selection info. */
bcm_field_presel_qualify_data_t field_vlan_format_cs_info_array[VLAN_FORMAT_NOF_CS_QUALS] = {
  /** qual_type  |        qual_arg    |       qual_value     |     qual_mask  */
    {bcmFieldQualifyVlanFormat, 0, BCM_FIELD_VLAN_FORMAT_UNTAGGED, 0x1F}
};

/** Regular Context Selection info. */
bcm_field_presel_qualify_data_t field_vlan_format_cs_regular_info_array[VLAN_FORMAT_NOF_CS_QUALS] = {
  /** qual_type  |        qual_arg    |       qual_value     |     qual_mask  */
    {bcmFieldQualifyForwardingType, 0, bcmFieldLayerTypeEth, 0x1F}
};

/**
 * Array, which contains information per field group. Like:
 * fg_id, fg_type, context selection, qulas, actions.
 */
cint_field_vlan_format_fg_info_t vlan_format_fg_info_array[VLAN_FORMAT_NOF_FG] = {
    /** IPMF1 VlanFormat CS test case field group info */
    {
     0,         /** Field group ID */
     0,          /** Context ID */
     bcmFieldStageIngressPMF1,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     VLAN_FORMAT_NOF_QUALS,        /** Number of tested qualifiers */
     VLAN_FORMAT_NOF_ACTIONS,       /** Number of tested actions */
     field_vlan_format_cs_info_array,       /** Context Selection info */
     field_vlan_format_regular_qual_info_array,         /** Qualifiers info */
     field_vlan_format_action_info_array        /** Actions info */
     },
     /** IPMF1 VlanFormat regular qualifier test case field group info */
    {
     0,         /** Field group ID */
     0,          /** Context ID */
     bcmFieldStageIngressPMF1,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     VLAN_FORMAT_NOF_QUALS,        /** Number of tested qualifiers */
     VLAN_FORMAT_NOF_ACTIONS,       /** Number of tested actions */
     field_vlan_format_cs_info_array,       /** Context Selection info */
     field_vlan_format_regular_qual_info_array,         /** Qualifiers info */
     field_vlan_format_action_info_array        /** Actions info */
     }
};

/**
 * This function is used to create a context and configure
 * the relevant program for the testing.
 */
int
cint_field_vlan_format_context(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_presel_qualify_data_t * vlan_format_cs_info,
    bcm_field_context_t * context_id)
{
    int rv = BCM_E_NONE;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    void *dest_char;

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "VLAN_FORMAT_CS", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, stage, &context_info, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create \n", rv);
        return rv;
    }

    printf("Context ID %d was created.\n", *context_id);

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = 2;
    presel_entry_id.stage = stage;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.context_id = *context_id;
    presel_entry_data.entry_valid = TRUE;

    presel_entry_data.qual_data[0].qual_type = vlan_format_cs_info->qual_type;
    presel_entry_data.qual_data[0].qual_value = vlan_format_cs_info->qual_value;
    presel_entry_data.qual_data[0].qual_arg = vlan_format_cs_info->qual_arg;
    presel_entry_data.qual_data[0].qual_mask = vlan_format_cs_info->qual_mask;

    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_presel_set\n");
        return rv;
    }

    return rv;
}

/**
 * This function creates field group, with all relevant information to it,
 * attaching it to the context and adding an entry.
 */
int
cint_field_vlan_format_group_config(
    int unit,
    cint_field_vlan_format_fg_info_t * vlan_format_fg_info)
{
    int rv = BCM_E_NONE;
    int qual_index, action_index;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    void *dest_char;
    bcm_field_entry_info_t entry_info;

    /**
     * We have only one context selection qualifier per case, which is indicated with VLAN_FORMAT_NOF_IPMF1_CS_QUALS,
     * because of that we will parse only the first index of vlan_format_cs_info[] array.
     */
    rv = cint_field_vlan_format_context(unit, vlan_format_fg_info->stage, vlan_format_fg_info->vlan_format_cs_info[0], &(vlan_format_fg_info->context_id));
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_vlan_format_context. \n", rv);
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_entry_info_t_init(&entry_info);

    fg_info.fg_type = vlan_format_fg_info->fg_type;
    fg_info.stage = vlan_format_fg_info->stage;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "VLAN_FORMAT_FG", sizeof(fg_info.name));

    fg_info.nof_quals = vlan_format_fg_info->nof_quals;
    fg_info.nof_actions = vlan_format_fg_info->nof_actions;
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.nof_entry_actions = fg_info.nof_actions;

    entry_info.priority = 1;

    for (qual_index = 0; qual_index < vlan_format_fg_info->nof_quals; qual_index++)
    {
        fg_info.qual_types[qual_index] = vlan_format_fg_info->vlan_format_qual_info[qual_index].qual_type;
        attach_info.key_info.qual_types[qual_index] = fg_info.qual_types[qual_index];
        attach_info.key_info.qual_info[qual_index].input_type =
            vlan_format_fg_info->vlan_format_qual_info[qual_index].qual_attach_info.input_type;
        attach_info.key_info.qual_info[qual_index].input_arg =
            vlan_format_fg_info->vlan_format_qual_info[qual_index].qual_attach_info.input_arg;
        attach_info.key_info.qual_info[qual_index].offset =
            vlan_format_fg_info->vlan_format_qual_info[qual_index].qual_attach_info.offset;
        entry_info.entry_qual[qual_index].type = fg_info.qual_types[qual_index];
        entry_info.entry_qual[qual_index].value[0] = vlan_format_fg_info->vlan_format_qual_info[qual_index].qual_value;
        entry_info.entry_qual[qual_index].mask[0] = vlan_format_fg_info->vlan_format_qual_info[qual_index].qual_mask;
    }

    for (action_index = 0; action_index < vlan_format_fg_info->nof_actions; action_index++)
    {
        fg_info.action_types[action_index] = vlan_format_fg_info->vlan_format_action_info[action_index].action_type;
        fg_info.action_with_valid_bit[action_index] = TRUE;
        attach_info.payload_info.action_types[action_index] = fg_info.action_types[action_index];
        entry_info.entry_action[action_index].type = fg_info.action_types[action_index];
        entry_info.entry_action[action_index].value[0] = vlan_format_fg_info->vlan_format_action_info[action_index].action_value;
    }

    /** Create the field group. */
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &(vlan_format_fg_info->fg_id));
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add on field_group_id = %d \n", rv, vlan_format_fg_info->fg_id);
        return rv;
    }

    printf("Field Group ID %d was created. \n", vlan_format_fg_info->fg_id);

    /** Attach the created field group to the context. */
    rv = bcm_field_group_context_attach(unit, 0, vlan_format_fg_info->fg_id, vlan_format_fg_info->context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach on contex_id = %d \n", rv, vlan_format_fg_info->context_id);
        return rv;
    }

    printf("Field Group ID %d was attached to Context ID %d. \n", vlan_format_fg_info->fg_id, vlan_format_fg_info->context_id);

    /** Add an entry to the created field group. */
    rv = bcm_field_entry_add(unit, 0, vlan_format_fg_info->fg_id, &entry_info, &entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    printf("Entry ID 0x%X (%d) was added to Field Group ID %d. \n", entry_handle, entry_handle, vlan_format_fg_info->fg_id);

    return rv;
}

/**
 * \brief
 * Configure TCAM field group to test the Vlan Format qualifiers.
 * Attach it to a context and add an entry to it.
 * Create two groups:
 *
 * \param [in] unit                 - Device id
 * \param [in] fg_info_array_index  - Array index to be parsed
 *  to the global array "vlan_format_fg_info_array[]", which contains
 *  all needed information for the configuration.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_vlan_format_main(
    int unit,
    int fg_info_array_index)
{
    int rv = BCM_E_NONE;

    /**
     * Parsing the global array "vlan_format_fg_info_array[]" with all needed
     * information for the given fg_info_array_index.
     */
    rv = cint_field_vlan_format_group_config(unit, vlan_format_fg_info_array[fg_info_array_index]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_vlan_format_group_config. \n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *  Destroys all allocated data by the configuration.
 *
 * \param [in] unit                 - Device id
 * \param [in] fg_info_array_index  - Array index to be parsed
 *  to the global array "vlan_format_fg_info_array[]", which contains
 *  all needed information to be destroyed.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_vlan_format_destroy(
    int unit,
    int fg_info_array_index)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    int rv = BCM_E_NONE;

    
    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_presel_set\n");
        return rv;
    }

    rv = bcm_field_entry_delete(unit, vlan_format_fg_info_array[fg_info_array_index].fg_id, NULL, entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, vlan_format_fg_info_array[fg_info_array_index].fg_id,
                                        vlan_format_fg_info_array[fg_info_array_index].context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }

    rv = bcm_field_group_delete(unit, vlan_format_fg_info_array[fg_info_array_index].fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_delete\n");
        return rv;
    }

    rv = bcm_field_context_destroy(unit, vlan_format_fg_info_array[fg_info_array_index].stage,
                                   vlan_format_fg_info_array[fg_info_array_index].context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_context_destroy\n");
        return rv;
    }

    return rv;
}
