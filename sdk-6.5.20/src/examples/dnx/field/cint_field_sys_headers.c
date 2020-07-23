/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Configuration example start:
 *
 * cint
 * cint_reset();
 * exit;
 * cint ../../src/examples/dnx/field/cint_field_sys_headers.c
 * cint;
 * int unit = 0;
 * int fg_info_array_index = 0;
 * cint_field_sys_headers_main(unit, fg_info_array_index);
 * exit;
 *
 * Configuration example end
 *
 * Configure TCAM field group to test the system headers.
 *   - First case - Configure context and set its params
 *             to change the system header profile to
 *             bcmFieldSystemHeaderProfileFtmhTsh (HW value = 5).
 *   - Second case - Configure same context as in the first case,
 *             and override the value with action result from
 *             IPMF2-IPMF3.
 */

/** Number of Actions (used as array size) */
int SYS_HDR_NOF_IPMF1_ACTIONS = 1;
int SYS_HDR_NOF_IPMF2_ACTIONS = 1;
/** Number of Data Qualifiers needed (used as array size) */
int SYS_HDR_NOF_IPMF1_QUALIFIERS = 1;
int SYS_HDR_NOF_IPMF2_QUALIFIERS = 1;
/** Number of Field Groups needed (used as array size) */
int SYS_HDR_NOF_FG = 2;
/**
 * Value to be used for bcmFieldActionFabricHeaderSet in IPMF2 stage.
 * This is the HW value of bcmFieldSystemHeaderProfileFtmhPph.
 * Change the configured by the context value bcmFieldSystemHeaderProfileFtmhTsh (HW value = 5)
 * to bcmFieldSystemHeaderProfileFtmhPph (HW value = 4).
 */
int SYSTEM_HEADER_PROFILE_ACTION_VALUE = bcmFieldSystemHeaderProfileFtmhPph;
/**
 * Value to be used for bcmFieldActionPrioIntNew in IPMF1 stage.
 * It will update the Traffic Class from IPMF1 to FER.
 */
int SYSTEM_HEADER_TC_ACTION_VALUE = 6;

/** Offset of VLAN_FORMAT Qualifier for ETH Header[6:5] (2b) */
int VLAN_FORMAT_QUAL_OFFSET = 96;
/** Length of VLAN_FORMAT Qualifier for EHT Header[6:5] (2b) */
int VLAN_FORMAT_QUAL_LENGTH = 16;

/** Value and mask, which will be used for entry qualifier bcmFieldQualifyTpid */
int SYS_HDR_ETH_TYPE_QUAL_VALUE = 0x8100;
int SYS_HDR_ETH_TYPE_QUAL_MASK  = 0xFFFF;

/**The presel_entry. Used for creating and destroying the presel. */
bcm_field_presel_entry_id_t presel_entry_id;
/**The Entry handle. Used for storing the entry handle to be destroyed. */
bcm_field_entry_t entry_handle;

/**
 * Structure, which contains information,
 * for creating of user actions and predefined one.
 */
struct cint_field_sys_headers_qual_info_t
{
    char *name;                 /* Qualifier name. */
    bcm_field_qualify_t qual_id;        /* Qualifier type. */
    uint32 qual_length;         /* Qualifier length. */
    bcm_field_qualify_attach_info_t qual_attach_info;   /* Qualifier Attach info. */
};

/**
 * Structure, which contains needed action information.
 */
struct cint_field_sys_headers_action_info_t
{
    bcm_field_action_t action_id;       /* Action type. */
    uint32 action_value;        /* Action value. */
};

/**
 * Structure, which contains all needed information
 * about configuring of one field group.
 */
struct cint_field_sys_headers_fg_info_t
{
    bcm_field_group_t fg_id;    /* Field Group ID. */
    bcm_field_context_t context_id;     /* Context ID. */
    bcm_field_stage_t stage;    /* Field stage. */
    bcm_field_group_type_t fg_type;     /* Field Group type. */
    int nof_quals;              /* Number of qualifiers per group. */
    int nof_actions;            /* Number of actions per group. */
    cint_field_sys_headers_qual_info_t *sys_hdr_qual_info;   /* Qualifier Info. */
    cint_field_sys_headers_action_info_t *sys_hdr_action_info;       /* Action Info. */
};

/** IPMF1 Qualifiers info. */
cint_field_sys_headers_qual_info_t field_sys_headers_ipmf1_qual_info_array[SYS_HDR_NOF_IPMF1_QUALIFIERS] = {
  /** qual_name  |  qual_id   |   qual_size     |  input_type    |    input_arg  |  qual_offset  */
    {NULL, bcmFieldQualifyTpid, 0, {bcmFieldInputTypeLayerAbsolute, 0, 96}}
};

/** IPMF2 Qualifiers info. In this stage we have an user define qualifier. */
cint_field_sys_headers_qual_info_t field_sys_headers_ipmf2_qual_info_array[SYS_HDR_NOF_IPMF2_QUALIFIERS] = {
  /** qual_name  |  qual_id   |   qual_size     |  input_type    |    input_arg  |  qual_offset   */
    {"vlan_format", 0, VLAN_FORMAT_QUAL_LENGTH, {bcmFieldInputTypeLayerAbsolute, 0, VLAN_FORMAT_QUAL_OFFSET}}
};

/**
 * IPMF1 Action info.
 */
cint_field_sys_headers_action_info_t field_sys_headers_ipmf1_action_info_array[SYS_HDR_NOF_IPMF1_ACTIONS] = {
/**       action_id    |   {action_value}*/
    {bcmFieldActionPrioIntNew, SYSTEM_HEADER_TC_ACTION_VALUE}
};
/**
 * IPMF2 Action info.
 */
cint_field_sys_headers_action_info_t field_sys_headers_ipmf2_action_info_array[SYS_HDR_NOF_IPMF2_ACTIONS] = {
/**       action_id    |   {action_value}*/
    {bcmFieldActionFabricHeaderSet, SYSTEM_HEADER_PROFILE_ACTION_VALUE}
};

/**
 * Array, which contains information per field group. Like:
 * fg_id, fg_type, qulas, actions.
 */
cint_field_sys_headers_fg_info_t sys_headers_fg_info_array[SYS_HDR_NOF_FG] = {
    /** IPMF1 field group info */
    {
     0,      /** Field group ID */
     0,       /** Context ID */
     bcmFieldStageIngressPMF1,      /** Field stage */
     bcmFieldGroupTypeTcam,     /** Field group type */
     SYS_HDR_NOF_IPMF1_QUALIFIERS,     /** Number of tested qualifiers */
     SYS_HDR_NOF_IPMF1_ACTIONS,    /** Number of tested actions */
     field_sys_headers_ipmf1_qual_info_array,      /** Qualifiers info */
     field_sys_headers_ipmf1_action_info_array     /** Actions info */
     },
    /** IPMF2 field group info */
    {
     0,      /** Field group ID */
     0,       /** Context ID */
     bcmFieldStageIngressPMF2,      /** Field stage */
     bcmFieldGroupTypeTcam,     /** Field group type */
     SYS_HDR_NOF_IPMF2_QUALIFIERS,     /** Number of tested qualifiers */
     SYS_HDR_NOF_IPMF2_ACTIONS,    /** Number of tested actions */
     field_sys_headers_ipmf2_qual_info_array,      /** Qualifiers info */
     field_sys_headers_ipmf2_action_info_array     /** Actions info */
     }
};

/**
 * This function is used to create a context and configure
 * the relevant program for the testing.
 */
int
cint_field_sys_headers_context(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t * context_id)
{
    int rv = BCM_E_NONE;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    void *dest_char;

    if (stage == bcmFieldStageIngressPMF2)
    {
        stage = bcmFieldStageIngressPMF1;
    }

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy(dest_char, "SYS_HDR_CS", sizeof(context_info.name));
    context_info.name[sizeof(context_info.name) - 1] = 0;
    rv = bcm_field_context_create(unit, 0, stage, &context_info, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create \n", rv);
        return rv;
    }

    printf("Context ID %d was created.\n", *context_id);

    /**
     * Set context attribute to change the system header profile bcmFieldContextParamTypeSystemHeaderProfile.
     * Set value for the system header profile to bcmFieldSystemHeaderProfileFtmhTsh (HW value = 5)
     */
    bcm_field_context_param_info_t_init(&param_info);
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
    param_info.param_val = bcmFieldSystemHeaderProfileFtmhTsh;

    rv = bcm_field_context_param_set(unit, 0, stage, *context_id, &param_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_presel_set\n");
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    /** For iPMF2, iPMF1 presel must be configured. */
    presel_entry_id.presel_id = 2;
    presel_entry_id.stage = stage;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.context_id = *context_id;
    presel_entry_data.entry_valid = TRUE;

    /** Check that the ETH header has only one tag. */
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyVlanFormat;
    presel_entry_data.qual_data[0].qual_value = 0x1;
    presel_entry_data.qual_data[0].qual_mask = 0x1;

    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_presel_set\n");
        return rv;
    }

    return rv;
}

/** Used to configure the user qualifiers. */
int
cint_field_sys_headers_user_qual_config(
    int unit,
    bcm_field_stage_t field_stage,
    cint_field_sys_headers_qual_info_t * sys_hdr_qual_info)
{
    int rv = BCM_E_NONE;
    bcm_field_qualifier_info_create_t qual_info;
    char *proc_name;
    void *dest_char;

    proc_name = "field_sys_headers_user_qual_config";

    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    sal_strncpy(dest_char, sys_hdr_qual_info->name, sizeof(qual_info.name));
    qual_info.name[sizeof(qual_info.name) - 1] = 0;
    qual_info.size = sys_hdr_qual_info->qual_length;
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &(sys_hdr_qual_info->qual_id));
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_qualifier_create on qual_id = %d (%s) \n", proc_name, rv,
               sys_hdr_qual_info->qual_id, sys_hdr_qual_info->name);
        return rv;
    }

    return rv;
}

/**
 * This function creates field group, with all relevant information to it and attaching it to the context.
 */
int
cint_field_sys_headers_group_config(
    int unit,
    cint_field_sys_headers_fg_info_t * sys_hdr_fg_info)
{
    int rv = BCM_E_NONE;
    int qual_index, action_index;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    void *dest_char;
    bcm_field_entry_info_t entry_info;

    rv = cint_field_sys_headers_context(unit, sys_hdr_fg_info->stage, &(sys_hdr_fg_info->context_id));
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in field_sys_headers_context. \n", rv);
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);

    fg_info.fg_type = sys_hdr_fg_info->fg_type;
    fg_info.stage = sys_hdr_fg_info->stage;

    dest_char = &(fg_info.name[0]);
    sal_strncpy(dest_char, "SYS_HDR_FG", sizeof(fg_info.name));
    fg_info.name[sizeof(fg_info.name) - 1] = 0;

    fg_info.nof_quals = sys_hdr_fg_info->nof_quals;
    fg_info.nof_actions = sys_hdr_fg_info->nof_actions;
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    /**
     * In case we have qual name for the current qualifier, it means that this is user define qual
     * and we should call field_sys_headers_user_qual_config() to configure it.
     */
    if (sys_hdr_fg_info->sys_hdr_qual_info[0].name != NULL)
    {
        rv = cint_field_sys_headers_user_qual_config(unit, fg_info.stage, &(sys_hdr_fg_info->sys_hdr_qual_info[0]));
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in field_sys_headers_user_qual_config. \n", rv);
            return rv;
        }
    }
    fg_info.qual_types[0] = sys_hdr_fg_info->sys_hdr_qual_info[0].qual_id;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = sys_hdr_fg_info->sys_hdr_qual_info[0].qual_attach_info.input_type;
    attach_info.key_info.qual_info[0].input_arg = sys_hdr_fg_info->sys_hdr_qual_info[0].qual_attach_info.input_arg;
    attach_info.key_info.qual_info[0].offset = sys_hdr_fg_info->sys_hdr_qual_info[0].qual_attach_info.offset;

    fg_info.action_types[0] = sys_hdr_fg_info->sys_hdr_action_info[0].action_id;
    fg_info.action_with_valid_bit[0] = TRUE;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    /** Create the field group. */
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &(sys_hdr_fg_info->fg_id));
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add on field_group_id = %d \n", rv, sys_hdr_fg_info->fg_id);
        return rv;
    }

    printf("Field Group ID %d was created. \n", sys_hdr_fg_info->fg_id);

    /** Attach the created field group to the context. */
    rv = bcm_field_group_context_attach(unit, 0, sys_hdr_fg_info->fg_id, sys_hdr_fg_info->context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach on contex_id = %d \n", rv, sys_hdr_fg_info->context_id);
        return rv;
    }

    printf("Field Group ID %d was attached to Context ID %d. \n", sys_hdr_fg_info->fg_id, sys_hdr_fg_info->context_id);

    bcm_field_entry_info_t_init(&entry_info);

    entry_info.priority = 1;
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.nof_entry_actions = fg_info.nof_actions;

    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = SYS_HDR_ETH_TYPE_QUAL_VALUE;
    entry_info.entry_qual[0].mask[0] = SYS_HDR_ETH_TYPE_QUAL_MASK;

    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = sys_hdr_fg_info->sys_hdr_action_info[0].action_value;

    /** Add an entry to the created field group. */
    rv = bcm_field_entry_add(unit, 0, sys_hdr_fg_info->fg_id, &entry_info, &entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    printf("Entry ID 0x%X (%d) was added to Field Group ID %d. \n", entry_handle, entry_handle, sys_hdr_fg_info->fg_id);

    return rv;
}

/**
 * \brief
 * Configure TCAM field group to test the system headers.
 * Create two groups:
 *   - First case - Configure context and set its params
 *             to change the system header profile.
 *   - Second case - Configure same context as in the first case,
 *             and override the value with action result from
 *             IPMF2-IPMF3.
 *
 * \param [in] unit                 - Device id
 * \param [in] fg_info_array_index  - Array index to be parsed
 *  to the global array "sys_headers_fg_info_array[]", which contains
 *  all needed information for the configuration.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_sys_headers_main(
    int unit,
    int fg_info_array_index)
{
    int rv = BCM_E_NONE;

    /**
     * Parsing the global array "sys_headers_fg_info_array[]" with all needed
     * information for the given fg_info_array_index.
     */
    rv = cint_field_sys_headers_group_config(unit, sys_headers_fg_info_array[fg_info_array_index]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in field_sys_headers_group_config. \n", rv);
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
 *  to the global array "sys_headers_fg_info_array[]", which contains
 *  all needed information to be destroyed.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_sys_headers_destroy(
    int unit,
    int fg_info_array_index)
{
    int rv = BCM_E_NONE;
    bcm_field_presel_entry_data_t presel_entry_data;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_presel_set\n");
        return rv;
    }

    rv = bcm_field_entry_delete(unit, sys_headers_fg_info_array[fg_info_array_index].fg_id, NULL, entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, sys_headers_fg_info_array[fg_info_array_index].fg_id,
                                        sys_headers_fg_info_array[fg_info_array_index].context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }

    rv = bcm_field_group_delete(unit, sys_headers_fg_info_array[fg_info_array_index].fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_delete\n");
        return rv;
    }

    if (sys_headers_fg_info_array[fg_info_array_index].stage == bcmFieldStageIngressPMF2)
    {
        rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, sys_headers_fg_info_array[fg_info_array_index].context_id);
    }
    else
    {
        rv = bcm_field_context_destroy(unit, sys_headers_fg_info_array[fg_info_array_index].stage,
                                       sys_headers_fg_info_array[fg_info_array_index].context_id);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_context_destroy\n");
        return rv;
    }

    if (sys_headers_fg_info_array[fg_info_array_index].sys_hdr_qual_info[0].name != NULL)
    {
        rv = bcm_field_qualifier_destroy(unit, sys_headers_fg_info_array[fg_info_array_index].sys_hdr_qual_info[0].qual_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_qualifier_destroy \n");
            return rv;
        }
    }

    return rv;
}
