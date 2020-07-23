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
 * cint ../../src/examples/dnx/field/cint_field_data_qualifier_create.c
 * cint;
 * int unit = 0;
 * int num_of_qualifiers = 50;
 * cint_field_data_qualifier_create_main(unit, num_of_qualifiers);
 *
 * Configuration example end
 *
 *  Test scenario:
 *      - Cint configuration:
 *          1. Create 8 TCAM FGs.
 *          2. To FGs 0 and 4:
 *              - To every group add 1 UDH qualifier
 *                  - Qualify upon first bit of the DA MAC address. Expects to be 0.
 *              - Add TC action to the groups:
 *                  - FG1 - bcmFieldActionPrioIntNew - value of 4
 *                  - FG2 - bcmFieldActionPrioIntNew - value of 5
 *          3. Attach FGs to two different contexts:
 *               - bcmFieldQualifyForwardingType
 *                  - Presel 1 - bcmFieldLayerTypeEth
 *                  - Presel 2 - bcmFieldLayerTypeIp4
 *          4. Add entries.
 *      - Traffic test:
 *          5. Send 1 packet to match on the FG1 and verify that only TC is being updated (Value of 4).
 *              - Packet: 0x00000000009977887766550081000000ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *          6. Send 1 packet to match on the FG2 and verify that only TC is being updated (Value of 5).
 *              - Packet: 0x00000000009977887766550081000000080045000035000000008000fa45c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 */

int Cint_field_data_qualifier_create_nof_fg = 8;
int Cint_field_data_qualifier_create_nof_presels = 2;
int Cint_field_data_qualifier_create_nof_cs = 2;
int Cint_field_data_qualifier_create_nof_entries = 2;
int Cint_field_data_qualifier_create_fg1_action_tc_value = 4;
int Cint_field_data_qualifier_create_fg2_action_tc_value = 5;

/** Global variables to be passed through function. */
bcm_field_presel_t Cint_field_data_qualifier_create_presel_id[Cint_field_data_qualifier_create_nof_presels] = {51, 50};
uint32 Cint_field_data_qualifier_create_presel_qual_value[Cint_field_data_qualifier_create_nof_presels] = {bcmFieldLayerTypeEth, bcmFieldLayerTypeIp4};
uint32 Cint_field_data_qualifier_create_presel_qual_arg[Cint_field_data_qualifier_create_nof_presels] = {0, 1};
bcm_field_entry_t Cint_field_data_qualifier_create_entry_handle[Cint_field_data_qualifier_create_nof_entries];
bcm_field_group_t Cint_field_data_qualifier_create_fg_ids[Cint_field_data_qualifier_create_nof_fg];
bcm_field_context_t Cint_field_data_qualifier_create_context_id[Cint_field_data_qualifier_create_nof_cs];
bcm_field_stage_t Cint_field_data_qualifier_create_field_stage = bcmFieldStageIngressPMF1;
bcm_field_qualify_t Cint_field_data_qualifier_create_qual_ids[256];

/**
 * \brief
 *  This function is used for creation of two different context for both FGs.
 *
 * \param [in] unit   - Device id
 * \param [in] field_stage  - Field stage on which context will be created.
 * \param [in] presel_id  - Presel ID for the current context.
 * \param [in] fwd_layer  - Forwarding Layer type.
 * \param [in] presel_qual_arg  - Presel qualifier argument.
 * \param [out] context_id  - Context ID, to be returned.
 * \param [in] context_name  - Name of the current context.
 *  to be created.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_data_qualifier_create_context_create(
    int unit,
    bcm_field_stage_t field_stage,
    bcm_field_presel_t presel_id,
    bcm_field_layer_type_t fwd_layer,
    uint32 presel_qual_arg,
    bcm_field_context_t  *context_id,
    char * context_name)
{
    int rv = BCM_E_NONE;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    void * dest_char;

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, context_name, sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, field_stage, &context_info, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = presel_id;
    p_id.stage = field_stage;
    p_data.entry_valid = TRUE;
    p_data.context_id = *context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = presel_qual_arg;
    p_data.qual_data[0].qual_value = fwd_layer;
    p_data.qual_data[0].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set \n", rv);
        return rv;
    }

    printf("Context ID %d was created. \n", *context_id);

    return rv;
}

/**
 * \brief
 *  This function Creates up to 256 user define qualifiers.
 *  Creates 2 field groups and add one qualifier to every group.
 *  Add entries to the FGs, to verify the them with traffic.
 *
 * \param [in] unit   - Device id
 * \param [in] num_of_qualifiers   - Number of qualifiers to be created
 *
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_data_qualifier_create_main(
    int unit,
    int num_of_qualifiers)
{
    int rv = BCM_E_NONE;
    int qual_iter;
    bcm_field_group_info_t fg_info;
    bcm_field_entry_info_t entry_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_group_attach_info_t attach_info;
    int fg_config_iter = 0;
    int fg_iter;

    /** Create constant qualifier, which will be used for all groups. */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;

    for (qual_iter = 0; qual_iter < num_of_qualifiers; qual_iter++)
    {
        rv = bcm_field_qualifier_create(unit, 0, &qual_info, &Cint_field_data_qualifier_create_qual_ids[qual_iter]);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_qualifier_create on qual_id = %d (%s) \n", rv,
                   Cint_field_data_qualifier_create_qual_ids[qual_iter], qual_info.name);
            return rv;
        }
    }

    printf("Created UDH qualifiers %d. \n", num_of_qualifiers);

    while (fg_config_iter < Cint_field_data_qualifier_create_nof_fg)
    {
        switch (fg_config_iter)
        {
            case 0:
            {
                /** Create FWD context. */
                rv = cint_field_data_qualifier_create_context_create(unit, Cint_field_data_qualifier_create_field_stage,
                                                                     Cint_field_data_qualifier_create_presel_id[0],
                                                                     Cint_field_data_qualifier_create_presel_qual_value[0],
                                                                     Cint_field_data_qualifier_create_presel_qual_arg[0],
                                                                     &Cint_field_data_qualifier_create_context_id[0], "data_qual_create_cs1");
                if (rv != BCM_E_NONE)
                {
                    printf("Error (%d), in field_presel_fwd_layer_main \n", rv);
                    return rv;
                }
                break;
            }
            case 4:
            {
                /** Create FWD context. */
                rv = cint_field_data_qualifier_create_context_create(unit, Cint_field_data_qualifier_create_field_stage,
                                                                     Cint_field_data_qualifier_create_presel_id[1],
                                                                     Cint_field_data_qualifier_create_presel_qual_value[1],
                                                                     Cint_field_data_qualifier_create_presel_qual_arg[1],
                                                                     &Cint_field_data_qualifier_create_context_id[1], "data_qual_create_cs2");
                if (rv != BCM_E_NONE)
                {
                    printf("Error (%d), in field_presel_fwd_layer_main \n", rv);
                    return rv;
                }
                break;
            }
        }

        for (fg_iter = 0; fg_iter < Cint_field_data_qualifier_create_nof_fg / 2; fg_iter++)
        {
            bcm_field_group_info_t_init(&fg_info);
            fg_info.fg_type = bcmFieldGroupTypeTcam;
            fg_info.stage = Cint_field_data_qualifier_create_field_stage;
            fg_info.nof_quals = 1;
            fg_info.nof_actions = 1;
            fg_info.qual_types[0] = Cint_field_data_qualifier_create_qual_ids[fg_config_iter];
            fg_info.action_types[0] = bcmFieldActionPrioIntNew;

            if (fg_config_iter >= Cint_field_data_qualifier_create_nof_fg / 2)
            {
                rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &Cint_field_data_qualifier_create_fg_ids[fg_config_iter + fg_iter]);
                if (rv != BCM_E_NONE)
                {
                    printf("Error (%d), in bcm_field_group_add on field_group_id = %d \n", rv, Cint_field_data_qualifier_create_fg_ids[fg_config_iter + fg_iter]);
                    return rv;
                }
                printf("Field Group ID %d was created. \n", Cint_field_data_qualifier_create_fg_ids[fg_config_iter + fg_iter]);
            }
            else
            {
                rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &Cint_field_data_qualifier_create_fg_ids[fg_iter]);
                if (rv != BCM_E_NONE)
                {
                    printf("Error (%d), in bcm_field_group_add on field_group_id = %d \n", rv, Cint_field_data_qualifier_create_fg_ids[fg_iter]);
                    return rv;
                }
                printf("Field Group ID %d was created. \n", Cint_field_data_qualifier_create_fg_ids[fg_iter]);
            }
        }

        bcm_field_group_attach_info_t_init(&attach_info);
        attach_info.key_info.nof_quals = fg_info.nof_quals;
        attach_info.payload_info.nof_actions = fg_info.nof_actions;
        attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
        attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
        attach_info.key_info.qual_info[0].input_arg = 0;
        attach_info.key_info.qual_info[0].offset = 0;
        attach_info.payload_info.action_types[0] = fg_info.action_types[0];

        if (fg_config_iter == Cint_field_data_qualifier_create_nof_fg / 2)
        {
            rv = bcm_field_group_context_attach(unit, 0, Cint_field_data_qualifier_create_fg_ids[fg_config_iter + (fg_iter - 1)],
                                                Cint_field_data_qualifier_create_context_id[1], &attach_info);
            if (rv != BCM_E_NONE)
            {
                printf("Error (%d), in bcm_field_group_context_attach\n", rv);
                return rv;
            }

            printf("Field Group ID %d was attached to Context ID %d. \n", Cint_field_data_qualifier_create_fg_ids[fg_config_iter + (fg_iter - 1)],
                    Cint_field_data_qualifier_create_context_id[1]);
        }
        else
        {
            rv = bcm_field_group_context_attach(unit, 0, Cint_field_data_qualifier_create_fg_ids[fg_iter - 1],
                                                Cint_field_data_qualifier_create_context_id[0], &attach_info);
            if (rv != BCM_E_NONE)
            {
                printf("Error (%d), in bcm_field_group_context_attach\n", rv);
                return rv;
            }

            printf("Field Group ID %d was attached to Context ID %d. \n", Cint_field_data_qualifier_create_fg_ids[fg_iter - 1],
                    Cint_field_data_qualifier_create_context_id[0]);
        }

        bcm_field_entry_info_t_init(&entry_info);
        entry_info.nof_entry_quals = fg_info.nof_quals;
        entry_info.nof_entry_actions = fg_info.nof_actions;
        entry_info.priority = 1;
        entry_info.entry_qual[0].type = fg_info.qual_types[0];
        entry_info.entry_qual[0].value[0] = 0x0;
        entry_info.entry_qual[0].mask[0] = 0x1;
        entry_info.entry_action[0].type = fg_info.action_types[0];

        if (fg_config_iter == Cint_field_data_qualifier_create_nof_fg / 2)
        {
            entry_info.entry_action[0].value[0] = Cint_field_data_qualifier_create_fg2_action_tc_value;
            rv = bcm_field_entry_add(unit, 0, Cint_field_data_qualifier_create_fg_ids[fg_config_iter + (fg_iter - 1)], &entry_info,
                                     &Cint_field_data_qualifier_create_entry_handle[1]);
            if (rv != BCM_E_NONE)
            {
                printf("Error (%d), in bcm_field_entry_add\n", rv);
                return rv;
            }
            printf("Entry ID 0x%X (%d) was added to Field Group ID %d. \n", Cint_field_data_qualifier_create_entry_handle[1],
                   Cint_field_data_qualifier_create_entry_handle[1], Cint_field_data_qualifier_create_fg_ids[fg_config_iter + (fg_iter - 1)]);
        }
        else
        {
            entry_info.entry_action[0].value[0] = Cint_field_data_qualifier_create_fg1_action_tc_value;
            rv = bcm_field_entry_add(unit, 0, Cint_field_data_qualifier_create_fg_ids[fg_iter - 1], &entry_info,
                                     &Cint_field_data_qualifier_create_entry_handle[0]);
            if (rv != BCM_E_NONE)
            {
                printf("Error (%d), in bcm_field_entry_add\n", rv);
                return rv;
            }
            printf("Entry ID 0x%X (%d) was added to Field Group ID %d. \n", Cint_field_data_qualifier_create_entry_handle[0],
                   Cint_field_data_qualifier_create_entry_handle[0], Cint_field_data_qualifier_create_fg_ids[fg_iter - 1]);
        }

        fg_config_iter = fg_config_iter + fg_iter;
    }

    return rv;
}

/**
 * \brief
 *  Destroys all allocated data by the configuration.
 *
 * \param [in] unit   - Device id
 * \param [in] num_of_qualifiers   - Number of qualifiers to be created
 *
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_data_qualifier_create_destroy(
    int unit,
    int num_of_qualifiers)
{
    int rv = BCM_E_NONE;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t p_id;
    int qual_index, fg_iter;

    rv = bcm_field_entry_delete(unit, Cint_field_data_qualifier_create_fg_ids[3], NULL, Cint_field_data_qualifier_create_entry_handle[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }
    rv = bcm_field_entry_delete(unit, Cint_field_data_qualifier_create_fg_ids[7], NULL, Cint_field_data_qualifier_create_entry_handle[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, Cint_field_data_qualifier_create_fg_ids[3], Cint_field_data_qualifier_create_context_id[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }
    rv = bcm_field_group_context_detach(unit, Cint_field_data_qualifier_create_fg_ids[7], Cint_field_data_qualifier_create_context_id[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }

    for (fg_iter = 0; fg_iter < Cint_field_data_qualifier_create_nof_fg; fg_iter++)
    {
        rv = bcm_field_group_delete(unit, Cint_field_data_qualifier_create_fg_ids[fg_iter]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_group_delete\n");
            return rv;
        }
    }

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    p_id.stage = Cint_field_data_qualifier_create_field_stage;
    p_id.presel_id = Cint_field_data_qualifier_create_presel_id[0];
    rv = bcm_field_presel_set(unit, 0, &p_id, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_presel_set\n");
        return rv;
    }

    p_id.presel_id = Cint_field_data_qualifier_create_presel_id[1];
    rv = bcm_field_presel_set(unit, 0, &p_id, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_presel_set\n");
        return rv;
    }

    rv = bcm_field_context_destroy(unit, Cint_field_data_qualifier_create_field_stage, Cint_field_data_qualifier_create_context_id[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy ipmf1 \n", rv);
        return rv;
    }

    rv = bcm_field_context_destroy(unit, Cint_field_data_qualifier_create_field_stage, Cint_field_data_qualifier_create_context_id[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy ipmf1 \n", rv);
        return rv;
    }

    for (qual_index = 0; qual_index < num_of_qualifiers; qual_index++)
    {
        rv = bcm_field_qualifier_destroy(unit, Cint_field_data_qualifier_create_qual_ids[qual_index]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_qualifier_destroy. \n");
            return rv;
        }
    }

    return rv;

}
