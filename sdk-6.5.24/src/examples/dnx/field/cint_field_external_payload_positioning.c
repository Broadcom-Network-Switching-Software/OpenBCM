/*
 * Configuration example start:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../src/examples/dnx/field/cint_field_external_payload_positioning.c
 * cint;
 * int unit = 0;
 * cint_field_external_payload_positioning_main(unit);
 *
 * Configuration example end
 *
 * This file is an example of how to choose he payload ID and payload offset for external field groups.
 * The function that does that is cint_field_external_payload_positioning_choose_placement().
 */

int cint_field_external_payload_positioning_nof_fg = 3;
bcm_field_group_t cint_field_external_payload_positioning_fg_id[cint_field_external_payload_positioning_nof_fg];
bcm_field_group_info_t cint_field_external_payload_positioning_fg_info[cint_field_external_payload_positioning_nof_fg];
uint32 cint_field_external_payload_positioning_payload_id[cint_field_external_payload_positioning_nof_fg];
uint32 cint_field_external_payload_positioning_payload_offset[cint_field_external_payload_positioning_nof_fg];

bcm_field_AppType_t cint_field_external_payload_positioning_external_app_type_1 = bcmFieldAppTypeL2;
bcm_field_AppType_t cint_field_external_payload_positioning_external_app_type_2 = bcmFieldAppTypeIp4UcastRpf;

/** Just providing qualifiers for each field group, what qualifiers are used is not relevant for the example.*/
bcm_field_qualify_t cint_field_external_payload_positioning_qual[cint_field_external_payload_positioning_nof_fg] = 
    { bcmFieldQualifyInVPort0, bcmFieldQualifyInVPort1, bcmFieldQualifyOutVPort0};

/** List of hit bit qualifiers according to payload_id.*/
bcm_field_qualify_t cint_field_external_payload_positioning_qual_hitbit[8] = 
    { bcmFieldQualifyExternalHit0,
      bcmFieldQualifyExternalHit1, 
      bcmFieldQualifyExternalHit2,
      bcmFieldQualifyExternalHit3,
      bcmFieldQualifyExternalHit4,
      bcmFieldQualifyExternalHit5,
      bcmFieldQualifyExternalHit6,
      bcmFieldQualifyExternalHit7 };

int cint_field_external_payload_positioning_payload_size[cint_field_external_payload_positioning_nof_fg] = {24, 32, 8};

bcm_field_group_t cint_field_external_payload_positioning_fg_id_pmf;

/**
 * \brief - Select, with explanation, payload_id and payload_offset for external TCAM field group.
 *
 * \param [in] unit - the unit number in system
 */
int
cint_field_external_payload_positioning_choose_placement(int unit)
{
    char *proc_name;
    proc_name = "cint_field_external_payload_positioning_choose_placement";

    /*
     * We now have 3 field groups to find payload_id and payload offset for, for two apptypes:
     * bcmFieldAppTypeIp4UcastRpf and bcmFieldAppTypeL2. The three field groups have 24 bits of payload each.
     * we want the two apptypes to have the same payload_id and payload_offset so that the PMF can use them from
     * whichever apptype. However note that it is not mandatory, and we can attach the same field group to different
     * apptypes with different payload_id and different payload_offset.
     */
    /*
     * First, we consider bcmFieldAppTypeL2. we have no FWD lookup in this apptype, so we can use payload_id 0.
     * Let's say we want the first field group to have payload_id id 0 and maximum offset. the result buffer size,
     * not including valid bits, is 248. With payload of 24, the most is 224.
     * Now for the other two field groups, when we have two field groups attached to the same apptype, the one with
     * the lower payload_id must have the higher payload_offse. So we place them as high as we can as well.
     */
    cint_field_external_payload_positioning_payload_id[0] = 0;
    cint_field_external_payload_positioning_payload_offset[0] = 248 - cint_field_external_payload_positioning_payload_size[0];
    cint_field_external_payload_positioning_payload_id[1] = 1;
    cint_field_external_payload_positioning_payload_offset[1] = cint_field_external_payload_positioning_payload_offset[0] - cint_field_external_payload_positioning_payload_size[1];
    cint_field_external_payload_positioning_payload_id[2] = 2;
    cint_field_external_payload_positioning_payload_offset[2] = cint_field_external_payload_positioning_payload_offset[1] - cint_field_external_payload_positioning_payload_size[2];
    /*
     * Now let's say we want to use payload IDs 1, 2 and 5 instead.
     * Each unsued payload_id up to the maximum used payload_id reuqires a minimum of 8 bits, so we push back
     * paylad_id 1 by 8 bits and payload_id 5 by 16.
     */
    cint_field_external_payload_positioning_payload_id[0] = 1;
    cint_field_external_payload_positioning_payload_offset[0] = 248 - cint_field_external_payload_positioning_payload_size[0] - 8;
    cint_field_external_payload_positioning_payload_id[1] = 2;
    cint_field_external_payload_positioning_payload_offset[1] = cint_field_external_payload_positioning_payload_offset[0] - cint_field_external_payload_positioning_payload_size[1];
    cint_field_external_payload_positioning_payload_id[2] = 5;
    cint_field_external_payload_positioning_payload_offset[2] = cint_field_external_payload_positioning_payload_offset[1] - cint_field_external_payload_positioning_payload_size[2] - 16;
    /*
     * Now let's say we want have the offset a little bit smaller. the minimum we can reduce it is by 8 bits,
     * since all poffsets must by byte aligned.
     */
    cint_field_external_payload_positioning_payload_id[0] = 1;
    cint_field_external_payload_positioning_payload_offset[0] = 248 - cint_field_external_payload_positioning_payload_size[0] - 8 - 8;
    cint_field_external_payload_positioning_payload_id[1] = 2;
    cint_field_external_payload_positioning_payload_offset[1] = cint_field_external_payload_positioning_payload_offset[0] - cint_field_external_payload_positioning_payload_size[1];
    cint_field_external_payload_positioning_payload_id[2] = 5;
    cint_field_external_payload_positioning_payload_offset[2] = cint_field_external_payload_positioning_payload_offset[1] - cint_field_external_payload_positioning_payload_size[2] - 16;
    /*
     * Now let's say we want to use payload IDs 1, 2 and 3 and use the minimum offsets.
     */
    cint_field_external_payload_positioning_payload_id[0] = 1;
    cint_field_external_payload_positioning_payload_offset[0] = 0 + cint_field_external_payload_positioning_payload_size[1] + cint_field_external_payload_positioning_payload_size[2];
    cint_field_external_payload_positioning_payload_id[1] = 2;
    cint_field_external_payload_positioning_payload_offset[1] = 0 + cint_field_external_payload_positioning_payload_size[2];
    cint_field_external_payload_positioning_payload_id[2] = 3;
    cint_field_external_payload_positioning_payload_offset[2] = 0;
    /*
     * Now let's say we want to use payload IDs 0, 1 and 2 and use the minimum offsets. paylaod_id 0 must have
     * payload_offset of at least 120.
     */
    cint_field_external_payload_positioning_payload_id[0] = 0;
    cint_field_external_payload_positioning_payload_offset[0] = 120;
    cint_field_external_payload_positioning_payload_id[1] = 1;
    cint_field_external_payload_positioning_payload_offset[1] = 0 + cint_field_external_payload_positioning_payload_size[2];
    cint_field_external_payload_positioning_payload_id[2] = 2;
    cint_field_external_payload_positioning_payload_offset[2] = 0;
    /*
     * Now let's say we want to use the first field group with maximum offset and the others with minimum offsets.
     * If we still want to use payload_id 0, 1 and 2, we cannot have more than 128 bits between consecutive payload IDs.
     */
    cint_field_external_payload_positioning_payload_id[0] = 0;
    cint_field_external_payload_positioning_payload_offset[0] = 248-cint_field_external_payload_positioning_payload_size[0];
    cint_field_external_payload_positioning_payload_id[1] = 1;
    cint_field_external_payload_positioning_payload_offset[1] = cint_field_external_payload_positioning_payload_offset[0] - 128;
    cint_field_external_payload_positioning_payload_id[2] = 2;
    cint_field_external_payload_positioning_payload_offset[2] = 0;
    /*
     * If we still want to have the other field groups at the beginning, we can leave a blank payload_id between them.
     */
    cint_field_external_payload_positioning_payload_id[0] = 0;
    cint_field_external_payload_positioning_payload_offset[0] = 120;
    cint_field_external_payload_positioning_payload_id[1] = 1 + 1;
    cint_field_external_payload_positioning_payload_offset[1] = 0 + cint_field_external_payload_positioning_payload_size[2];
    cint_field_external_payload_positioning_payload_id[2] = 2 + 1;
    cint_field_external_payload_positioning_payload_offset[2] = 0;
    /*
     * Let's go back to having the biggest offsets we can.
     */
    cint_field_external_payload_positioning_payload_id[0] = 0;
    cint_field_external_payload_positioning_payload_offset[0] = 248 - 24;
    cint_field_external_payload_positioning_payload_id[1] = 1;
    cint_field_external_payload_positioning_payload_offset[1] = cint_field_external_payload_positioning_payload_offset[0] - cint_field_external_payload_positioning_payload_size[1];
    cint_field_external_payload_positioning_payload_id[2] = 2;
    cint_field_external_payload_positioning_payload_offset[2] = cint_field_external_payload_positioning_payload_offset[1] - cint_field_external_payload_positioning_payload_size[2];
    /*
     * Now let's consider bcmFieldAppTypeIp4UcastRpf as well. This apptype has two FWD lookup taking payload_id 0 and 1,
     * and the 96MSB of the result buffer. So this pushed us down.
     */
    cint_field_external_payload_positioning_payload_id[0] = 2;
    cint_field_external_payload_positioning_payload_offset[0] = 248 - 96 - 24;
    cint_field_external_payload_positioning_payload_id[1] = 3;
    cint_field_external_payload_positioning_payload_offset[1] = cint_field_external_payload_positioning_payload_offset[0] - cint_field_external_payload_positioning_payload_size[1];
    cint_field_external_payload_positioning_payload_id[2] = 4;
    cint_field_external_payload_positioning_payload_offset[2] = cint_field_external_payload_positioning_payload_offset[1] - cint_field_external_payload_positioning_payload_size[2];

    return BCM_E_NONE;
}

int
cint_field_external_payload_positioning_fg_add(
    int unit)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    int qual_idx = 0;
    bcm_field_action_info_t action_info;
    void *dest_char;
    char *tmp_ptr;
    int fg_id_index;
    proc_name = "cint_field_external_payload_positioning_fg_add";

    for (fg_id_index = 0; fg_id_index < cint_field_external_payload_positioning_nof_fg; fg_id_index++)
    {
        bcm_field_group_info_t_init(&cint_field_external_payload_positioning_fg_info[fg_id_index]);
        cint_field_external_payload_positioning_fg_info[fg_id_index].stage = bcmFieldStageExternal;
        cint_field_external_payload_positioning_fg_info[fg_id_index].fg_type = bcmFieldGroupTypeExternalTcam;
        dest_char = &(cint_field_external_payload_positioning_fg_info[fg_id_index].name[0]);
        tmp_ptr = dest_char;
        snprintf(tmp_ptr, sizeof(cint_field_external_payload_positioning_fg_info[fg_id_index].name), "ext_FG_%d", fg_id_index);

        cint_field_external_payload_positioning_fg_info[fg_id_index].nof_quals = 1;
        cint_field_external_payload_positioning_fg_info[fg_id_index].qual_types[0] = cint_field_external_payload_positioning_qual[fg_id_index];

        bcm_field_action_info_t_init(&action_info);
        action_info.stage = bcmFieldStageExternal;
        action_info.action_type = bcmFieldActionVoid;
        action_info.size = cint_field_external_payload_positioning_payload_size[fg_id_index];
        dest_char = &(action_info.name[0]);
        tmp_ptr = dest_char;
        snprintf(tmp_ptr, sizeof(action_info.name), "ext_uda_%d", fg_id_index);
        rv = bcm_field_action_create(unit, 0, &action_info, &(cint_field_external_payload_positioning_fg_info[fg_id_index].action_types[0]));
        if (rv != BCM_E_NONE)
        {
            printf("%s Error (%d), in bcm_field_action_create %d\n", proc_name, rv, fg_id_index);
            return rv;
        }
        cint_field_external_payload_positioning_fg_info[fg_id_index].nof_actions = 1;
        cint_field_external_payload_positioning_fg_info[fg_id_index].action_with_valid_bit[0] = FALSE;

        rv = bcm_field_group_add(unit, 0, &cint_field_external_payload_positioning_fg_info[fg_id_index], &cint_field_external_payload_positioning_fg_id[fg_id_index]);
        if (rv != BCM_E_NONE)
        {
            printf("%s Error (%d), in bcm_field_action_create %d\n", proc_name, rv, fg_id_index);
            return rv;
        }
        
    }

    return rv;
}

int
cint_field_external_payload_positioning_fg_attach(
    int unit,
    bcm_field_AppType_t external_app_type)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    uint32 qual_ndx, action_ndx;
    bcm_field_group_attach_info_t group_attach_info;
    bcm_field_context_t context_id;
    int fg_id_index;
    proc_name = "cint_field_external_payload_positioning_fg_attach";

    for (fg_id_index = 0; fg_id_index < cint_field_external_payload_positioning_nof_fg; fg_id_index++)
    {
        bcm_field_group_attach_info_t_init(&group_attach_info);
        group_attach_info.key_info.nof_quals = cint_field_external_payload_positioning_fg_info[fg_id_index].nof_quals;
        group_attach_info.payload_info.nof_actions = cint_field_external_payload_positioning_fg_info[fg_id_index].nof_actions;
        for (qual_ndx = 0; qual_ndx < group_attach_info.key_info.nof_quals; qual_ndx++)
        {
            group_attach_info.key_info.qual_types[qual_ndx] = cint_field_external_payload_positioning_fg_info[fg_id_index].qual_types[qual_ndx];
            group_attach_info.key_info.qual_info[qual_ndx].input_type = bcmFieldInputTypeMetaData;
        }

        for (action_ndx = 0; action_ndx < group_attach_info.payload_info.nof_actions; action_ndx++)
        {
            group_attach_info.payload_info.action_types[action_ndx] = cint_field_external_payload_positioning_fg_info[fg_id_index].action_types[action_ndx];
        }

        group_attach_info.payload_info.payload_id = cint_field_external_payload_positioning_payload_id[fg_id_index];
        group_attach_info.payload_info.payload_offset = cint_field_external_payload_positioning_payload_offset[fg_id_index];

        context_id  = (external_app_type + 0) ;
        rv = bcm_field_group_context_attach(unit, 0, cint_field_external_payload_positioning_fg_id[fg_id_index], context_id, &group_attach_info);
        if (rv != BCM_E_NONE)
        {
            printf("%s Error (%d), in bcm_field_group_context_attach external_app_type %d fg number %d\n", proc_name, rv, context_id, fg_id_index);
            return rv;
        }
    }

    return rv;
}

int
cint_field_external_payload_positioning_ipmf1(
    int unit)
{
    char *proc_name;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_id_t p_entry;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_context_t context_id;
    bcm_field_group_info_t fg_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    bcm_field_group_attach_info_t attach_info;
    int qual_index_hit;
    int qual_index_payload;
    int fg_id_index;
    int action_offset_in_fg;
    void *dest_char;
    char *tmp_ptr;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_external_payload_positioning_ipmf1";
    
    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "ext_PMF_test", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_create\n", proc_name, rv);
        return rv;
    }

    /** Configure preselectors for the two apptypes used.*/
    bcm_field_presel_entry_id_info_init(&p_entry);
    bcm_field_presel_entry_data_info_init(&p_data);
    p_entry.presel_id = 15;
    p_entry.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id;
    p_data.nof_qualifiers = 1;
    p_data.qual_data[0].qual_type = bcmFieldQualifyAppType;
    p_data.qual_data[0].qual_value = cint_field_external_payload_positioning_external_app_type_1;
    p_data.qual_data[0].qual_mask = 0x3F;
    rv = bcm_field_presel_set(unit, 0, &p_entry, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in dnx_field_presel_set 1\n", proc_name, rv);
        return rv;
    }
    p_entry.presel_id = 16;
    p_data.qual_data[0].qual_value = cint_field_external_payload_positioning_external_app_type_2;
    rv = bcm_field_presel_set(unit, 0, &p_entry, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in dnx_field_presel_set 2\n", proc_name, rv);
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.fg_type = bcmFieldGroupTypeTcam;

    fg_info.nof_quals = 2 * cint_field_external_payload_positioning_nof_fg;
    for (fg_id_index = 0; fg_id_index < cint_field_external_payload_positioning_nof_fg; fg_id_index++)
    {
        qual_index_hit = fg_id_index * 2 + 0;
        qual_index_payload = fg_id_index * 2 + 1;
        fg_info.qual_types[qual_index_hit] = cint_field_external_payload_positioning_qual_hitbit[cint_field_external_payload_positioning_payload_id[fg_id_index]];
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = cint_field_external_payload_positioning_payload_size[fg_id_index];
        dest_char = &(qual_info.name[0]);
        tmp_ptr = dest_char;
        snprintf(tmp_ptr, sizeof(qual_info.name), "ext_UDP_iPMF1_%d", fg_id_index);
        rv = bcm_field_qualifier_create(unit, 0, &qual_info, &fg_info.qual_types[qual_index_payload]);
        if (rv != BCM_E_NONE)
        {
            printf("%s Error (%d), in bcm_field_qualifier_create %d\n", proc_name, rv, fg_id_index);
            return rv;
        }
    }

    /** Some action will be chosen. not relevant for the example.*/
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionUDHData0;

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_external_payload_positioning_fg_id_pmf);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    for (fg_id_index = 0; fg_id_index < cint_field_external_payload_positioning_nof_fg; fg_id_index++)
    {
        qual_index_hit = fg_id_index * 2 + 0;
        qual_index_payload = fg_id_index * 2 + 1;
        attach_info.key_info.qual_types[qual_index_hit] = fg_info.qual_types[qual_index_hit];
        attach_info.key_info.qual_info[qual_index_hit].input_type = bcmFieldInputTypeMetaData;
        rv = bcm_field_group_action_offset_get(unit, 0, cint_field_external_payload_positioning_fg_id[fg_id_index], cint_field_external_payload_positioning_fg_info[fg_id_index].action_types[0], &action_offset_in_fg);
        if(rv != BCM_E_NONE)
        {
            printf("%s Error (%d), in bcm_field_group_action_offset_get %d\n", proc_name, rv, fg_id_index);
            return rv;
        }
        attach_info.key_info.qual_types[qual_index_payload] = fg_info.qual_types[qual_index_payload];
        attach_info.key_info.qual_info[qual_index_payload].input_type = bcmFieldInputTypeExternal;
        attach_info.key_info.qual_info[qual_index_payload].offset = cint_field_external_payload_positioning_payload_offset[fg_id_index] + action_offset_in_fg;
    }
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    rv = bcm_field_group_context_attach(unit, 0, cint_field_external_payload_positioning_fg_id_pmf, context_id, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
        return rv;
    }

    return rv;
}

/**
 * \brief - run external main function
 *
 * \param [in] unit - the unit number in system
 *
 *  Usage: run in cint shell "cint_field_external_payload_positioning_main(0)"
 */
int
cint_field_external_payload_positioning_main(int unit)
{
    int fg_id_index;
    char *proc_name;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_external_payload_positioning_main";

    /** Initialize arrays to illegal values, so that we won't miss any when we fill them with legal values.*/
    for (fg_id_index = 0; fg_id_index < cint_field_external_payload_positioning_nof_fg; fg_id_index++)
    {
        cint_field_external_payload_positioning_payload_id[fg_id_index] = -1;
        cint_field_external_payload_positioning_payload_offset[fg_id_index] = -1;
    }

    rv = cint_field_external_payload_positioning_fg_add(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_payload_positioning_fg_add fg_id %d\n", proc_name, rv, cint_field_external_payload_positioning_fg_id);
        return rv;
    }

    /** Choose payload_id and payload_offset.*/
    rv = cint_field_external_payload_positioning_choose_placement(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_payload_positioning_choose_placement\n", proc_name, rv);
        return rv;
    }

    rv = cint_field_external_payload_positioning_fg_attach(unit, cint_field_external_payload_positioning_external_app_type_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_payload_positioning_fg_attach app type 1\n", proc_name, rv);
        return rv;
    }
    rv = cint_field_external_payload_positioning_fg_attach(unit, cint_field_external_payload_positioning_external_app_type_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_payload_positioning_fg_attach app type 2\n", proc_name, rv);
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchExternalTcamSync, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), bcm_switch_control_set - ExternalTcamSync failed\n", proc_name, rv);
        return rv;
    }

    rv = cint_field_external_payload_positioning_ipmf1(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_payload_positioning_context_ipmf1_create\n", proc_name, rv);
        return rv;
    }

    return rv;
}
