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
 * cint ../../../src/examples/dnx/field/cint_field_cascading.c
 * cint;
 * int unit = 0;
 * bcm_field_context_t ipmf1_context_id = 20;
 * bcm_field_context_t ipmf2_context_id = 21;
 * cint_field_cascading_main(unit, ipmf1_context_id, ipmf2_context_id);
 * quit;
 *
 * Configuration example end

**********************************************************************

*
* TCL test for traffic test based on this cint:
*

cd ../../../regress/bcm;
tcl;
source tests/sand/testDvapiSand.tcl
Logging::LevelSet 10;

NTestSeedSet 123456789;

test run ResetOnFail::AT_Dnx_Cint_field_ipmf1_ipmf2_cascaded
 */

/*
 * Global variables.
 * Declared global to make easy destroy functions.
 */
bcm_field_group_t ipmf1_fg_id = BCM_FIELD_INVALID;
bcm_field_group_t ipmf2_fg_id = BCM_FIELD_INVALID;
bcm_field_group_info_t Cint_field_cascading_ipmf1_fg_info;
bcm_field_group_info_t Cint_field_cascading_ipmf2_fg_info;
bcm_field_entry_t ipmf1_entry_handle = -1;
bcm_field_entry_t ipmf2_entry_handle = -1;
bcm_field_qualify_t bcm_udf_qual;
bcm_field_presel_entry_id_t ipmf1_p_id;
bcm_field_presel_entry_id_t ipmf2_p_id;
/**********************************************************************/

char * cint_field_stage_name_get(bcm_field_stage_t stage)
{
    switch (stage)
    case bcmFieldStageEgress:
    {
        return  "Egress";
        break;
    }
    case bcmFieldStageIngressPMF1:
    {
        return "IngressPMF1";
        break;
    }
    case bcmFieldStageIngressPMF2:
    {
        return "IngressPMF2";
        break;
    }
    case bcmFieldStageIngressPMF3:
    {
        return "IngressPMF3";
        break;
    }
    default:
    {
        return "Invalid";
        break;
    }

    return "Invalid";
}

int cint_field_cascading_fg_add(
    int unit, bcm_field_stage_t stage,
    bcm_field_group_type_t fg_type,
    bcm_field_group_t * fg_id_p,
    bcm_field_group_info_t * fg_info_p)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    void *dest_char;

    proc_name = "cint_field_cascading_fg_add";

    bcm_field_qualifier_info_create_t qual_info;

    bcm_field_group_info_t_init(fg_info_p);
    bcm_field_qualifier_info_create_t_init(&qual_info);

    fg_info_p->stage = stage;
    fg_info_p->fg_type = fg_type;

    switch (stage)
    {
        case bcmFieldStageIngressPMF1:
        {
            fg_info_p->nof_quals = 3;
            fg_info_p->qual_types[0] = bcmFieldQualifySrcIp;
            fg_info_p->qual_types[1] = bcmFieldQualifyDstIp;
            fg_info_p->qual_types[2] = bcmFieldQualifyDstMac;
            /**Same for the actions */
            fg_info_p->nof_actions = 2;
            /* Dummy action to increase action size to 64b in order to be used for context cascaded */
            fg_info_p->action_types[0] = bcmFieldActionForward;
            fg_info_p->action_types[1] = bcmFieldActionTtlSet;

            /* Set FG name */
            dest_char = &(fg_info_p->name[0]);
            sal_strncpy_s(dest_char, "TEST_CASCADED_IPMF1", sizeof(fg_info_p->name));

            break;
        }
        case bcmFieldStageIngressPMF2:
        {
            fg_info_p->nof_quals = 3;

            fg_info_p->qual_types[0] = bcmFieldQualifySrcMac;

            /*
             * Create an user-qualifier in ipmf2 that will parse the action buffer received from ipmf1 (cascaded)
             */
            qual_info.size = 8; /* TTL */

            dest_char = &(qual_info.name[0]);
            sal_strncpy_s(dest_char, "CASCADED_ACTION_QUAL", sizeof(qual_info.name));

            rv = bcm_field_qualifier_create(unit, 0, &qual_info, &bcm_udf_qual);
            if (rv != BCM_E_NONE)
            {
                printf("%s Error (%d), in bcm_field_qualifier_create\n", proc_name, rv);
                return rv;
            }

            fg_info_p->qual_types[1] = bcm_udf_qual;
            fg_info_p->qual_types[2] = bcmFieldQualifyDstMac;

            /**Same for the actions */
            fg_info_p->nof_actions = 1;
            fg_info_p->action_types[0] = bcmFieldActionDropPrecedence;
            /* Set FG name */
            dest_char = &(fg_info_p->name[0]);
            sal_strncpy_s(dest_char, "TEST_CASCADED_IPMF2", sizeof(fg_info_p->name));

            break;
        }
    }


    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, fg_info_p, fg_id_p);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_add\n", proc_name, rv);
        return rv;
    }

    return rv;
}

int cint_field_cascading_set_attach_info(
        int unit,
        bcm_field_stage_t stage,
        bcm_field_group_t ipmf1_cascaded_fg_id,
        bcm_field_group_info_t * cascaded_fg_info_p,
        bcm_field_group_attach_info_t * attach_info_p)
{
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "cint_field_cascading_set_attach_info";

    switch (stage)
    {
        case bcmFieldStageIngressPMF1:
        {
            /*bcmFieldQualifySrcIp*/
            attach_info_p->key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
            attach_info_p->key_info.qual_info[0].input_arg = 1;
            attach_info_p->key_info.qual_info[0].offset = 0;

            /*bcmFieldQualifyDstIp*/
            attach_info_p->key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
            attach_info_p->key_info.qual_info[1].input_arg = 1;
            attach_info_p->key_info.qual_info[1].offset = 0;

            /*bcmFieldQualifyDstMac*/
            attach_info_p->key_info.qual_info[2].input_type = bcmFieldInputTypeLayerFwd;
            attach_info_p->key_info.qual_info[2].input_arg = 0;
            attach_info_p->key_info.qual_info[2].offset = 0;

            break;
        }
        case bcmFieldStageIngressPMF2:
        {
            int action_offset;

            /*bcmFieldQualifySrcMac*/
            attach_info_p->key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
            attach_info_p->key_info.qual_info[0].input_arg = 0;
            attach_info_p->key_info.qual_info[0].offset = 0;

            /*
             * Cascaded input type for parsing the action buffer
             */
            attach_info_p->key_info.qual_info[1].input_type = bcmFieldInputTypeCascaded;
            /*
             * fg_id_ipmf1 is the Field group in ipmf1 to use the result of
             */
            attach_info_p->key_info.qual_info[1].input_arg = ipmf1_cascaded_fg_id;

            /*
             * Get the offset for the action
             */
            rv = bcm_field_group_action_offset_get(unit, 0, ipmf1_cascaded_fg_id,
                    cascaded_fg_info_p->action_types[1], &action_offset);
            if (rv != BCM_E_NONE)
            {
                printf("%s Error (%d), in bcm_field_group_action_offset_get\n", proc_name, rv);
                return rv;
            }
            /*
             * Place the action offset in the qualifier offset to perform cascading
             */
            attach_info_p->key_info.qual_info[1].offset = action_offset;

            /*bcmFieldQualifyDstMac*/
            attach_info_p->key_info.qual_info[2].input_type = bcmFieldInputTypeLayerFwd;
            attach_info_p->key_info.qual_info[2].input_arg = 0;
            attach_info_p->key_info.qual_info[2].offset = 0;

            break;
        }
        default:
        {
            printf("%s Error - supported only %s-%s cascading, received stage %d %s\n",
                    proc_name,
                    cint_field_stage_name_get(bcmFieldStageIngressPMF1),
                    cint_field_stage_name_get(bcmFieldStageIngressPMF2),
                    stage,
                    ((stage < bcmFieldStageFirst) || (stage >= bcmFieldStageCount)) ?
                            "Invalid" : cint_field_stage_name_get(stage));
            return rv;
        }
    }

    return rv;
}

int cint_field_cascading_fg_attach(
        int unit,
        bcm_field_stage_t stage,
        bcm_field_context_t context_id,
        bcm_field_group_t fg_id,
        bcm_field_group_t ipmf1_cascaded_fg_id,
        bcm_field_group_info_t * fg_info_p,
        bcm_field_group_info_t * cascaded_fg_info_p)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    int qual_idx;
    bcm_field_group_attach_info_t attach_info;

    proc_name = "cint_field_cascading_fg_attach";

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info_p->nof_quals;
    attach_info.payload_info.nof_actions = fg_info_p->nof_actions;
    attach_info.payload_info.action_types[0] = fg_info_p->action_types[0];
    attach_info.payload_info.action_types[1] = fg_info_p->action_types[1];

    for (qual_idx = 0; qual_idx < fg_info_p->nof_quals; qual_idx ++)
    {
        attach_info.key_info.qual_types[qual_idx] = fg_info_p->qual_types[qual_idx];
        attach_info.key_info.qual_info[qual_idx].input_arg = 0;
        attach_info.key_info.qual_info[qual_idx].offset = 0;
    }

    rv = cint_field_cascading_set_attach_info(unit, stage, ipmf1_cascaded_fg_id, cascaded_fg_info_p, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_cascading_ipmf2_set_attach_info\n", proc_name, rv);
        return rv;
    }

    rv = bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
        return rv;
    }
    return rv;
}


int cint_field_cascading_entry_add(
        int unit,
        bcm_field_stage_t stage,
        bcm_field_context_t context_id,
        bcm_field_group_t fg_id,
        bcm_field_group_info_t * fg_info_p,
        int cascaded_action_val,
        uint32 entry_priority,
        bcm_field_entry_t *entry_handle_p)
{
    char *proc_name;
    int qual_idx = 0;
    int action_idx = 0;

    bcm_field_entry_info_t entry_in_info;

    int rv = BCM_E_NONE;

    proc_name = "cint_field_cascading_entry_add";

    bcm_field_entry_info_t_init(&entry_in_info);

    switch (stage)
    {
        case bcmFieldStageIngressPMF1:
        {
            /* SIP */
            entry_in_info.entry_qual[qual_idx].type = fg_info_p->qual_types[qual_idx];   /* bcmFieldQualifySrcIp */
            entry_in_info.entry_qual[qual_idx].mask[0] = 0xFFFFFFFF;
            entry_in_info.entry_qual[qual_idx].value[0] = 0x0a0b0c0d;               /* 10.11.12.13 */
            qual_idx++;
            /* DIP */
            entry_in_info.entry_qual[qual_idx].type = fg_info_p->qual_types[qual_idx];   /* bcmFieldQualifyDstIp */
            entry_in_info.entry_qual[qual_idx].mask[0] = 0xFFFFFFFF;
            entry_in_info.entry_qual[qual_idx].value[0] = 0x0a0b0c0e;               /* 10.11.12.14 */
            qual_idx++;
            /* DA */
            entry_in_info.entry_qual[qual_idx].type = fg_info_p->qual_types[qual_idx];   /* bcmFieldQualifyDstMac */
            entry_in_info.entry_qual[qual_idx].mask[0] = 0xFFFFFFFF;
            entry_in_info.entry_qual[qual_idx].value[0] = 0x12341234;               /* 00.00.12.34.12.34 */
            qual_idx++;
            /* TTL Action */
            entry_in_info.entry_action[action_idx].type = fg_info_p->action_types[action_idx + 1];
            entry_in_info.entry_action[action_idx].value[0] = cascaded_action_val;
            action_idx++;
            break;
        }
        case bcmFieldStageIngressPMF2:
        {
            /* SIP */
            entry_in_info.entry_qual[qual_idx].type = fg_info_p->qual_types[qual_idx];   /* bcmFieldQualifySrcMac */
            entry_in_info.entry_qual[qual_idx].mask[0] = 0xFFFFFFFF;
            entry_in_info.entry_qual[qual_idx].value[0] = 0xabcdab;               /* 0xabcdab*/
            qual_idx++;

            /* User qualifier for cascading */
            entry_in_info.entry_qual[qual_idx].type = fg_info_p->qual_types[qual_idx];   /* CASCADED_ACTION_QUAL */
            entry_in_info.entry_qual[qual_idx].mask[0] = 0xFFFF;
            entry_in_info.entry_qual[qual_idx].value[0] = cascaded_action_val;  /* Should be 20 (TTL value from the ipmf1 group )*/
            qual_idx++;

            /* DA */
            entry_in_info.entry_qual[qual_idx].type = fg_info_p->qual_types[qual_idx];   /* bcmFieldQualifyDstMac */
            entry_in_info.entry_qual[qual_idx].mask[0] = 0xFFFFFFFF;
            entry_in_info.entry_qual[qual_idx].value[0] = 0x12341234;               /* 00.00.12.34.12.34 */
            qual_idx++;

            /* DP Action */
            entry_in_info.entry_action[action_idx].type = fg_info_p->action_types[action_idx];
            entry_in_info.entry_action[action_idx].value[0] = 2;
            action_idx++;
            break;
        }
        default:
        {
            for (qual_idx = 0; qual_idx < fg_info_p->nof_quals; qual_idx ++)
            {
                entry_in_info.entry_qual[qual_idx].type = fg_info_p->qual_types[qual_idx];
                entry_in_info.entry_qual[qual_idx].mask[0] = 0xF;
                entry_in_info.entry_qual[qual_idx].value[0] = 0;

            }
            for (action_idx = 0; action_idx < fg_info_p->nof_actions; action_idx ++)
            {
                entry_in_info.entry_action[action_idx].type = fg_info_p->action_types[action_idx];
                entry_in_info.entry_action[action_idx].value[0] = 0;
            }
            break;
        }
    }
    entry_in_info.priority = entry_priority;
    entry_in_info.nof_entry_quals = qual_idx;
    entry_in_info.nof_entry_actions = action_idx;
    rv = bcm_field_entry_add(unit, 0, fg_id, &entry_in_info, entry_handle_p);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_add for context %d, fg_id %d\n", proc_name, rv, context_id, fg_id);
        return rv;
    }
    return rv;
}

int cint_field_cascading_context_create(int unit, bcm_field_context_t ipmf1_context_id)
{
    bcm_field_context_info_t ipmf1_context_info;
    bcm_field_presel_entry_data_t p_data;
    int rv = BCM_E_NONE;
    void *dest_char;
    char *proc_name = "cint_field_cascading_context_create";

    bcm_field_context_info_t_init(&ipmf1_context_info);
    /* Set FG name */
    dest_char = &(ipmf1_context_info.name[0]);
    sal_strncpy_s(dest_char, "CASCADED_IPMF1_CTX", sizeof(ipmf1_context_info.name));

    rv = bcm_field_context_create(unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF1, &ipmf1_context_info, &ipmf1_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_field_context_create for IngressPMF1\n", proc_name, rv);
        return rv;
    }
    /*
     */
    bcm_field_presel_entry_id_info_init(&ipmf1_p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    ipmf1_p_id.presel_id = 16;
    ipmf1_p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = ipmf1_context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeEth;
    p_data.qual_data[0].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &ipmf1_p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in dnx_field_presel_set\n", proc_name, rv);
        return rv;
    }

    return rv;
}

int cint_field_cascading_cascaded_context_create(int unit, bcm_field_context_t ipmf1_context_id, bcm_field_context_t ipmf2_context_id, bcm_field_group_t ipmf1_fg_id, int cascaded_action_value)
{
    bcm_field_context_info_t ipmf2_context_info;
    bcm_field_presel_entry_data_t p_data;    
    int rv = BCM_E_NONE;
    void *dest_char;
    char *proc_name = "cint_field_cascading_cascaded_context_create";

    bcm_field_context_info_t_init(&ipmf2_context_info);

    dest_char = &(ipmf2_context_info.name[0]);
    sal_strncpy_s(dest_char, "CASCADED_IPMF2_CTX", sizeof(ipmf2_context_info.name));
    /*
     * First of all, we need to set the cascaded_from in the context info when creating the iPMF2 context to
     * the ID of the iPMF1 context
     */
    ipmf2_context_info.cascaded_from = ipmf1_context_id;
    rv = bcm_field_context_create(unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF2, &ipmf2_context_info, &ipmf2_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_field_context_create for IngressPMF2\n", proc_name, rv);
        return rv;
    }
    /*
     * Now we add preselector(s) to the new iPMF2 context to activate context switching to it when
     * the condition of the preselector(s) is met.rv
     */

    bcm_field_presel_entry_id_info_init(&ipmf2_p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    ipmf2_p_id.presel_id = 16;
    ipmf2_p_id.stage = bcmFieldStageIngressPMF2;
    p_data.entry_valid = TRUE;
    p_data.context_id = ipmf2_context_id;
    p_data.nof_qualifiers = 1;

    /*
     * Add cascaded preselector qualifier from the action of the FG in iPMF1 context we're cascading from.
     * The cascaded preselector qualifier takes into account only the 4msb of the payload. Since payloads
     * are MSB-aligned, if the first action is larger than 4b, it means that the cascaded preselector will
     * read the 4msb of the action, since our action is TTL and is 8b in length, we only need to shift left
     * the action value by 4 to get the actual result that we except.
     */
    p_data.qual_data[0].qual_type = bcmFieldQualifyCascadedKeyValue;
    p_data.qual_data[0].qual_arg = ipmf1_fg_id;
    p_data.qual_data[0].qual_value = (cascaded_action_value >> 4);
    p_data.qual_data[0].qual_mask = 0xF;

    rv = bcm_field_presel_set(unit, 0, &ipmf2_p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in dnx_field_presel_set\n", proc_name, rv);
        return rv;
    }

    return rv;
}


int cint_field_cascading_main(int unit, bcm_field_context_t ipmf1_context_id, bcm_field_context_t ipmf2_context_id)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    int cascaded_action_value = 20;

    proc_name = "field_cascading_main";

    /**********************************************/
    /******************IPMF1 Configuration* *******/
    /**********************************************/
    rv = cint_field_cascading_context_create(unit, ipmf1_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_cascading_create_context for IngressPMF1\n", proc_name, rv);
        return rv;
    }

    rv = cint_field_cascading_fg_add(unit, bcmFieldStageIngressPMF1, bcmFieldGroupTypeTcam, &ipmf1_fg_id, &Cint_field_cascading_ipmf1_fg_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_cascading_fg_add  for IngressPMF1\n", proc_name, rv);
        return rv;
    }

    rv = cint_field_cascading_fg_attach(unit, bcmFieldStageIngressPMF1, ipmf1_context_id, ipmf1_fg_id,
                                        0 /* Not relevant for IPMF1*/, &Cint_field_cascading_ipmf1_fg_info, NULL /* Not relevant for IPMF1 */);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_cascading_fg_attach  for IngressPMF1\n", proc_name, rv);
        return rv;
    }

    /**********************************************/
    /******************IPMF2 Configuration* *******/
    /**********************************************/
    if (ipmf2_context_id != ipmf1_context_id)
    {
        /*
         * Advanced context cascading mode.
         * When iPMF2 context ID is equal to iPMF1 context ID it happens by default.
         * But when those values are different, then we need to use additional API
         * to enable context cascading.
         */
        rv = cint_field_cascading_cascaded_context_create(unit, ipmf1_context_id, ipmf2_context_id, ipmf1_fg_id, cascaded_action_value);
        if (rv != BCM_E_NONE)
        {
            printf("%s Error (%d), in cint_field_cascading_create_cascaded_context\n", proc_name, rv);
            return rv;
        }
    }

    rv = cint_field_cascading_fg_add(unit, bcmFieldStageIngressPMF2, bcmFieldGroupTypeTcam, &ipmf2_fg_id, &Cint_field_cascading_ipmf2_fg_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_cascading_fg_add  for IngressPMF2\n", proc_name, rv);
        return rv;
    }

    rv = cint_field_cascading_fg_attach(unit, bcmFieldStageIngressPMF2, ipmf2_context_id, ipmf2_fg_id, ipmf1_fg_id, &Cint_field_cascading_ipmf2_fg_info, &Cint_field_cascading_ipmf1_fg_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_cascading_fg_attach  for IngressPMF2\n", proc_name, rv);
        return rv;
    }

    /**********************************************/
    /******************Entry Configuration* *******/
    /**********************************************/
    rv = cint_field_cascading_entry_add(unit, bcmFieldStageIngressPMF1, ipmf1_context_id, ipmf1_fg_id, &Cint_field_cascading_ipmf1_fg_info, cascaded_action_value, 2, &ipmf1_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_cascading_entry_add  for IngressPMF1\n", proc_name, rv);
        return rv;
    }
    rv = cint_field_cascading_entry_add(unit, bcmFieldStageIngressPMF2, ipmf2_context_id, ipmf2_fg_id, &Cint_field_cascading_ipmf2_fg_info, cascaded_action_value, 0, &ipmf2_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_cascading_entry_add  for IngressPMF2\n", proc_name, rv);
        return rv;
    }

    return rv;
}

int cint_field_cascading_destroy(int unit, bcm_field_context_t ipmf1_context_id, bcm_field_context_t ipmf2_context_id)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    bcm_field_presel_entry_data_t presel_entry_data;

    proc_name = "field_cascading_destroy";

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    rv = bcm_field_presel_set(unit, 0, &ipmf1_p_id, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_presel_set\n", proc_name, rv);
        return rv;
    }
    /*
     * If cascading is not trivial (iPMF1 context ID != iPMF2 context ID) then we need to destroy the iPMF2 presel as
     * well.
     */
    if (ipmf1_context_id != ipmf2_context_id)
    {
        bcm_field_presel_entry_data_info_init(&presel_entry_data);
        presel_entry_data.entry_valid = FALSE;
        rv = bcm_field_presel_set(unit, 0, &ipmf2_p_id, &presel_entry_data);
        if (rv != BCM_E_NONE)
        {
            printf("%s Error (%d), in bcm_field_presel_set\n", proc_name, rv);
            return rv;
        }
    }

    /* delete the entry from the IPMF1 FG*/
    rv = bcm_field_entry_delete(unit, ipmf1_fg_id, NULL, ipmf1_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_delete entry_id %d  fg %d stage %s\n", proc_name, rv, ipmf1_entry_handle, ipmf1_fg_id, cint_field_stage_name_get(bcmFieldStageIngressPMF1));
        return rv;
    }

    /* delete the entry from the IPMF2 FG*/
    rv = bcm_field_entry_delete(unit, ipmf2_fg_id, NULL, ipmf2_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_delete entry_id %d  fg %d stage %s\n", proc_name, rv, ipmf2_entry_handle, ipmf2_fg_id, cint_field_stage_name_get(bcmFieldStageIngressPMF2));
        return rv;
    }

    /* Detach the IPMF2 FG from its context */
    rv = bcm_field_group_context_detach(unit, ipmf2_fg_id, ipmf2_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_detach fg %d from context %d stage %s\n", proc_name, rv, ipmf2_fg_id, ipmf2_context_id, cint_field_stage_name_get(bcmFieldStageIngressPMF2));
        return rv;
    }

    /* Detach the IPMF1 FG from its context */
    rv = bcm_field_group_context_detach(unit, ipmf1_fg_id, ipmf1_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_detach fg %d from context %d stage %s\n", proc_name, rv, ipmf1_fg_id, ipmf1_context_id, cint_field_stage_name_get(bcmFieldStageIngressPMF1));
        return rv;
    }

    /* destroy the field group from the IPMF1 */
    rv = bcm_field_group_delete(unit, ipmf1_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_delete fg %d stage %s\n", proc_name, rv, ipmf1_fg_id, cint_field_stage_name_get(bcmFieldStageIngressPMF1));
        return rv;
    }

    /* destroy the field group from the IPMF2 */
    rv = bcm_field_group_delete(unit, ipmf2_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_delete fg %d stage %s\n", proc_name, rv, ipmf2_fg_id, cint_field_stage_name_get(bcmFieldStageIngressPMF2));
        return rv;
    }

    /* destroy the qualifier from the IPMF1 FG*/
    rv = bcm_field_qualifier_destroy(unit, bcm_udf_qual);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_qualifier_destroy\n", proc_name, rv);
        return rv;
    }

    /*
     * If cascading is not trivial (iPMF1 context ID != iPMF2 context ID) then we need to destroy iPMF2 context first,
     * (cascaded from contexts can't be destroyed). Otherwise destroying iPMF1 context is enough for destroying both contexts
     */
    if (ipmf1_context_id != ipmf2_context_id)
    {
        rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF2, ipmf2_context_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s Error (%d), in bcm_field_context_destroy  for context %d stage %s\n", proc_name, rv, ipmf2_context_id, cint_field_stage_name_get(bcmFieldStageIngressPMF2));
            return rv;
        }
    }

    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, ipmf1_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_destroy  for context %d stage %s\n", proc_name, rv, ipmf1_context_id, cint_field_stage_name_get(bcmFieldStageIngressPMF1));
        return rv;
    }
    return rv;
}
