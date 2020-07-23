/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 *  Configuration example start:
 *
 *  cint;
 *  cint_reset();
 *  exit;
 *  cint ../../../../src/examples/dnx/field/cint_field_utils.c
 *  cint ../../src/examples/dnx/field/cint_field_dt_multi.c
 *  cint;
 *  int unit = 0;
 *  bcm_port_t in_port = 200;
 *  bcm_port_t ipmf3_in_port = 201;
 *  bcm_port_t out_port = 202;
 *  bcm_field_stage_t field_stage = bcmFieldStageIngressPMF1;
 *  cint_field_dt_multi_main(unit, in_port, ipmf3_in_port, out_port, field_stage);
 *
 *  Configuration example end
 *
 *  Test scenario:
 *      - Cint configuration:
 *          1. Create 2 DT FGs - both should use same bank (8).
 *          2. To every group add 2 qualifiers:.
 *              - IPMF1, IPMF2 and IPMF3 stages:
 *                  - bcmFieldQualifyInPortWithoutCore - Meta data
 *                  - Create 2b constant qualifier and use it as MSB of both groups (values should be different "0x1" and "0x2").
 *              - EPMF stage:
 *                  - bcmFieldQualifyOutPort - Meta data
 *                  - Cause we not support none zero qualifiers we will take some data from the ETH header:
 *                      - bcmFieldQualifyVlanCfi - Header
 *          3. Add TC action to the groups:
 *             - FG1 - bcmFieldActionPrioIntNew - value of 4
 *             - FG2 - bcmFieldActionPrioIntNew - value of 6
 *          4. Attach FGs to two different contexts:
 *              - IPMF1, IPMF2 and EPMF stage:
 *                  - bcmFieldQualifyForwardingType
 *                      - Presel 1 - bcmFieldLayerTypeEth
 *                      - Presel 2 - bcmFieldLayerTypeIp4
 *              - IPMF3 stage:
 *                  - Presel 1 - bcmFieldLayerTypeEth
 *                  - Presel 2 - bcmFieldQualifyPortClassPacketProcessing
 *          5. Add entries.
 *      - Traffic test:
 *          6. Send 1 packet to match on the FG1 and verify that only TC is being updated (Value of 4).
 *          7. Send 1 packet to match on the FG2 and verify that only TC is being updated (Value of 6).
 *          8. Delete the entries and send packets again. Verify that both signals are not being updated.
 *          9. Re-add entries and do steps "6" and "7".
 *
 *      - The multi scenario is done for all stages:
 *          bcmFieldStageIngressPMF1/2/3 and bcmFieldStageEgress.
 */

int Cint_field_dt_multi_nof_quals = 2;
int Cint_field_dt_multi_nof_actions = 1;
int Cint_field_dt_multi_bank_id = 8;
int Cint_field_dt_multi_nof_fg = 2;
int Cint_field_dt_multi_nof_presels = 2;
int Cint_field_dt_multi_nof_cs = 2;

int Cint_field_dt_multi_fg1_action_tc_value = 4;
int Cint_field_dt_multi_fg2_action_tc_value = 6;
int Cint_field_dt_multi_fg1_const_qual_value = 0x0;
int Cint_field_dt_multi_fg2_const_qual_value = 0x1;

bcm_field_qualify_t Cint_field_dt_multi_qual_id;
bcm_field_qualify_t Cint_field_dt_multi_eth_samac_8b_qual_id = -1;
bcm_field_presel_t Cint_field_dt_multi_presel_id[Cint_field_dt_multi_nof_presels] = {51, 50};
uint32 Cint_field_dt_multi_presel_qual_value[Cint_field_dt_multi_nof_presels] = {bcmFieldLayerTypeEth, bcmFieldLayerTypeIp4};
uint32 Cint_field_dt_multi_presel_qual_arg[Cint_field_dt_multi_nof_presels] = {0, 1};
bcm_field_context_t Cint_field_dt_multi_context_id[Cint_field_dt_multi_nof_cs];
int Cint_field_dt_multi_dt_max_key_size_dnx_data_value;

/**
 * Structure, which contains all needed information
 * about configuring of one field group.
 */
struct cint_field_dt_multi_fg_info_t
{
    bcm_field_group_t fg_id;    /* Field Group ID. */
    char * fg_name;     /* Field Group Name. */

    bcm_field_entry_t entry_handle;     /* The Entry handle. */
    bcm_field_entry_info_t entry_info;      /* Entry Info. */

    /** Qualifier Info. */
    uint32 qual_value;      /* Qualifier value */

    /** Action Info. */
    bcm_field_action_t action_type;     /* Action type. */
    uint32 action_value;      /* Action value */

    /** Context and Presel Info. */
    bcm_field_presel_t presel_id;       /* Presel Id. */
    uint32 presel_value;    /* Presel value. */
    uint32 presel_arg;    /* Presel arg. */
    bcm_field_context_t context_id;     /* Context Id. */
};

/**
 * Following array is global, used for all test cases. It contains information per field group. Like:
 * fg_id, fg_name, entry_handle, quals, actions.
 */
cint_field_dt_multi_fg_info_t Cint_field_dt_multi_fg_info_array[Cint_field_dt_multi_nof_fg] = {
    /** DT FG1 info. */
    {
     0,         /** Field group ID */
     "DT_MULTI_FG1",          /** Field group Name */
     0,                            /* The Entry handle. */
     NULL,                  /* Entry Info. */
     Cint_field_dt_multi_fg1_const_qual_value,      /** Qualifier value. */
     bcmFieldActionPrioIntNew,      /** Action Type .*/
     Cint_field_dt_multi_fg1_action_tc_value,    /** Action value. */
     Cint_field_dt_multi_presel_id[0],       /* Presel Id. */
     Cint_field_dt_multi_presel_qual_value[0],      /* Presel Value. */
     Cint_field_dt_multi_presel_qual_arg[0],     /* Presel arg*/
     Cint_field_dt_multi_context_id[0]     /* Context Id. */
    },
    /** DT FG2 info. */
    {
     0,         /** Field group ID */
     "DT_MULTI_FG2",          /** Field group Name */
     0,                            /* The Entry handle. */
     NULL,                  /* Entry Info. */
     Cint_field_dt_multi_fg2_const_qual_value,      /** Qualifier value. */
     bcmFieldActionPrioIntNew,      /** Action Type .*/
     Cint_field_dt_multi_fg2_action_tc_value,    /** Action value. */
     Cint_field_dt_multi_presel_id[1],       /* Presel Id. */
     Cint_field_dt_multi_presel_qual_value[1],      /* Presel Value. */
     Cint_field_dt_multi_presel_qual_arg[1],     /* Presel arg*/
     Cint_field_dt_multi_context_id[1]     /* Context Id. */
    }
};

/**
 * \brief
 *  This function is used to configure constant qualifier for DT FGs.
 *
 * \param [in] unit   - Device id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_dt_multi_qual_create(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_qualifier_info_create_t qual_info;
    void *dest_char;

    /** Create constant qualifier, which will be used for all groups. */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "tcam_dt_const", sizeof(qual_info.name));
    qual_info.name[sizeof(qual_info.name) - 1] = 0;
    qual_info.size = 1;
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &Cint_field_dt_multi_qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create on qual_id = %d (%s) \n", rv,
               Cint_field_dt_multi_qual_id, qual_info.name);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *  This function is used for creation of two different context for both DT FGs.
 *
 * \param [in] unit   - Device id
 * \param [in] in_port  - In Port to be used as preselector qualifier value.
 * \param [in] field_stage  - Field stage on which context will be created.
 * \param [in] dt_fg_info  - Global Array, which contains information about both context
 *  to be created.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_dt_multi_context_create(
    int unit,
    bcm_port_t in_port,
    bcm_field_stage_t field_stage,
    cint_field_dt_multi_fg_info_t * dt_fg_info)
{
    int rv = BCM_E_NONE;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    void * dest_char;

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "dt_multi", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, field_stage, &context_info, &dt_fg_info->context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = dt_fg_info->presel_id;
    p_id.stage = field_stage;
    p_data.entry_valid = TRUE;
    p_data.context_id = dt_fg_info->context_id;
    p_data.nof_qualifiers = 1;

    if (field_stage == bcmFieldStageIngressPMF3 && dt_fg_info->presel_id == Cint_field_dt_multi_presel_id[1])
    {
        bcm_gport_t in_gport;
        uint32 port_class_id = 2;
        BCM_GPORT_LOCAL_SET(in_gport, in_port);
        rv = bcm_port_class_set(unit, in_gport, bcmPortClassFieldIngressPMF3PacketProcessingPortCs, port_class_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_port_class_set, bcmPortClassFieldIngressPMF3PacketProcessingPortCs\n", rv);
            return rv;
        }

        p_data.qual_data[0].qual_type = bcmFieldQualifyPortClassPacketProcessing;
        p_data.qual_data[0].qual_arg = 0;
        p_data.qual_data[0].qual_value = port_class_id;
        p_data.qual_data[0].qual_mask = 0x7;
    }
    else
    {
        p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
        p_data.qual_data[0].qual_arg = dt_fg_info->presel_arg;
        p_data.qual_data[0].qual_value = dt_fg_info->presel_value;
        p_data.qual_data[0].qual_mask = 0x1F;
    }

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set \n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *  This function created a field group and attaching it to a context.
 *  Adding an entry as well.
 *
 * \param [in] unit   - Device id
 * \param [in] in_port  - In Port to be used as entry qualifier value.
 * \param [in] ipmf3_in_port   - In Port, from which the second packet was sent in case of IPMF3 stage,
 *  will be used as entry qualifier value and presel qualifier value.
 * \param [in] out_port   - Out Port, on which the packet was received, will be used as
 *  entry qualifier value in case of EPMF stage.
 * \param [in] field_stage  - Field stage on which configuration will be done.
 * \param [in] dt_fg_info  - Global Array, which contains information per field group.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_dt_multi_fg_create(
    int unit,
    bcm_port_t in_port,
    bcm_port_t ipmf3_in_port,
    bcm_port_t out_port,
    bcm_field_stage_t field_stage,
    cint_field_dt_multi_fg_info_t * dt_fg_info)
{
    int rv = BCM_E_NONE;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_stage_t field_stage_internal;
    bcm_gport_t in_gport;
    void *dest_char;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags;
    bcm_port_t internal_port;

    /** In case of IPMF2 stage the context should be created in IPMF1 stage. */
    if (field_stage == bcmFieldStageIngressPMF2)
    {
        field_stage_internal = bcmFieldStageIngressPMF1;
    }
    else
    {
        field_stage_internal = field_stage;
    }

    rv = cint_field_dt_multi_context_create(unit, ipmf3_in_port, field_stage_internal, dt_fg_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_dt_multi_context_create \n", rv);
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);

    fg_info.fg_type = bcmFieldGroupTypeDirectTcam;
    fg_info.stage = field_stage;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, dt_fg_info->fg_name, sizeof(fg_info.name));
    fg_info.nof_quals = Cint_field_dt_multi_nof_quals;
    fg_info.nof_actions = Cint_field_dt_multi_nof_actions;
    if (field_stage == bcmFieldStageEgress)
    {
        /* Q2A */
        if (Cint_field_dt_multi_dt_max_key_size_dnx_data_value == 10)
        {
            if (Cint_field_dt_multi_eth_samac_8b_qual_id == -1)
            {
                bcm_field_qualifier_info_create_t eth_qual_info;
                void *eth_qual_dest_char;

                bcm_field_qualifier_info_create_t_init(&eth_qual_info);
                eth_qual_dest_char = &(eth_qual_info.name[0]);
                sal_strncpy_s(eth_qual_dest_char, "tcam_dt_eth_samac_8b", sizeof(eth_qual_info.name));
                eth_qual_info.size = 9;
                rv = bcm_field_qualifier_create(unit, 0, &eth_qual_info, &Cint_field_dt_multi_eth_samac_8b_qual_id);
                if (rv != BCM_E_NONE)
                {
                    printf("Error (%d), in bcm_field_qualifier_create on qual_id = %d (%s) \n", rv,
                           Cint_field_dt_multi_eth_samac_8b_qual_id, eth_qual_info.name);
                    return rv;
                }
            }

            fg_info.qual_types[0] = Cint_field_dt_multi_eth_samac_8b_qual_id;
        }
        else
        {
            fg_info.qual_types[0] = bcmFieldQualifyOutPort;
        }
        fg_info.qual_types[1] = bcmFieldQualifyVlanCfi;
    }
    else
    {
        /* Q2A */
        if (Cint_field_dt_multi_dt_max_key_size_dnx_data_value == 10)
        {
            fg_info.qual_types[0] = bcmFieldQualifyRxTrapCode;
        }
        else
        {
            fg_info.qual_types[0] = bcmFieldQualifyInPortWithoutCore;
        }
        fg_info.qual_types[1] = Cint_field_dt_multi_qual_id;
    }
    fg_info.action_types[0] = dt_fg_info->action_type;
    fg_info.action_with_valid_bit[0] = TRUE;
    /*
     * Request specific bank
     */
    fg_info.tcam_info.bank_allocation_mode = bcmFieldTcamBankAllocationModeSelect;
    fg_info.tcam_info.nof_tcam_banks = 1;
    fg_info.tcam_info.tcam_bank_ids[0] = Cint_field_dt_multi_bank_id;

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &(dt_fg_info->fg_id));
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add on field_group_id = %d \n", rv, dt_fg_info->fg_id);
        return rv;
    }
    printf("Field Group ID %d was created. \n", dt_fg_info->fg_id);

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    /** None zero constant qualifiers are not allowed on EPMF stage. */
    if (field_stage == bcmFieldStageEgress)
    {
        /**
         * Q2A.
         * Use header qualifier to get value from the ETH-SA-MAC field.
         *      SAMAC address is "00:00:07:00:01:00"
         *      offset of 79 will get 0x01 [ETH header bits 88:79] (9b)
         */
        if (Cint_field_dt_multi_dt_max_key_size_dnx_data_value == 10)
        {
            attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
            attach_info.key_info.qual_info[0].input_arg = 0;
            attach_info.key_info.qual_info[0].offset = 79;
        }
        /** Use header qualifier to get value from the ETH-Outer-PCP field. */
        attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
        attach_info.key_info.qual_info[1].input_arg = 0;
        attach_info.key_info.qual_info[1].offset = 96;
    }
    else
    {
        attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeConst;
        attach_info.key_info.qual_info[1].input_arg = dt_fg_info->qual_value;
        attach_info.key_info.qual_info[1].offset = 0;
    }

    rv = bcm_field_group_context_attach(unit, 0, dt_fg_info->fg_id, dt_fg_info->context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Field Group ID %d was attached to Context ID %d. \n", dt_fg_info->fg_id, dt_fg_info->context_id);

    bcm_field_entry_info_t_init(&dt_fg_info->entry_info);
    dt_fg_info->entry_info.nof_entry_quals = fg_info.nof_quals;
    dt_fg_info->entry_info.nof_entry_actions = fg_info.nof_actions;
    if (field_stage == bcmFieldStageIngressPMF3 &&
        dt_fg_info->qual_value == Cint_field_dt_multi_fg2_const_qual_value)
    {
        internal_port = ipmf3_in_port;
    }
    else if (field_stage == bcmFieldStageEgress)
    {
        internal_port = out_port;
    }
    else
    {
        internal_port = in_port;
    }

    BCM_GPORT_LOCAL_SET(in_gport, internal_port);
    rv = bcm_port_get(unit, internal_port, &flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE)
    {
      printf("Error (%d), in bcm_port_get\n", rv);
      return rv;
    }

    if (!BCM_GPORT_IS_TRUNK(in_gport)) {
        dt_fg_info->entry_info.core = mapping_info.core;
    }
    dt_fg_info->entry_info.entry_qual[0].type = fg_info.qual_types[0];
    /* Q2A */
    if (Cint_field_dt_multi_dt_max_key_size_dnx_data_value == 10)
    {
        if (field_stage == bcmFieldStageEgress)
        {
            dt_fg_info->entry_info.entry_qual[0].value[0] = 0x01;
            dt_fg_info->entry_info.entry_qual[0].mask[0] = 0xFF;
        }
        else
        {
            dt_fg_info->entry_info.entry_qual[0].value[0] = 0x10;
            dt_fg_info->entry_info.entry_qual[0].mask[0] = 0xFF;
        }
    }
    else
    {
        dt_fg_info->entry_info.entry_qual[0].value[0] = in_gport;
        dt_fg_info->entry_info.entry_qual[0].mask[0] = 0x1FF;
    }
    dt_fg_info->entry_info.entry_qual[1].type = fg_info.qual_types[1];
    dt_fg_info->entry_info.entry_qual[1].value[0] = dt_fg_info->qual_value;
    if (field_stage == bcmFieldStageEgress)
    {
        dt_fg_info->entry_info.entry_qual[1].mask[0] = 0x7;
    }
    else
    {
        dt_fg_info->entry_info.entry_qual[1].mask[0] = 0x3;
    }
    dt_fg_info->entry_info.entry_action[0].type = fg_info.action_types[0];
    dt_fg_info->entry_info.entry_action[0].value[0] = dt_fg_info->action_value;

    rv = bcm_field_entry_add(unit, 0, dt_fg_info->fg_id, &dt_fg_info->entry_info, &dt_fg_info->entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    printf("Entry ID 0x%X (%d) was added to Field Group ID %d. \n", dt_fg_info->entry_handle,
            dt_fg_info->entry_handle, dt_fg_info->fg_id);

    return rv;
}

/**
 * \brief
 *  This function configures 2 DT field groups for the give stage.
 *
 * \param [in] unit   - Device id
 * \param [in] in_port   - In Port, from which the packet was sent.
 * \param [in] ipmf3_in_port   - In Port, from which the second packet was sent in case of IPMF3 stage.
 * \param [in] out_port   - Out Port, on which the packet was received.
 * \param [in] field_stage  - Field stage on which configuration have to be done.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_dt_multi_main(
    int unit,
    bcm_port_t in_port,
    bcm_port_t ipmf3_in_port,
    bcm_port_t out_port,
    bcm_field_stage_t field_stage)
{
    int rv = BCM_E_NONE;
    int fg_iter;

    printf("*************************** Stage: %s ***************************\n\n", cint_field_utils_stage_name_get(field_stage));

    Cint_field_dt_multi_dt_max_key_size_dnx_data_value = *(dnxc_data_get(unit, "field", "tcam", "dt_max_key_size", NULL));

    rv = cint_field_dt_multi_qual_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_dt_multi_qual_create\n", rv);
        return rv;
    }

    for (fg_iter = 0; fg_iter < Cint_field_dt_multi_nof_fg; fg_iter++)
    {
        rv = cint_field_dt_multi_fg_create(unit, in_port, ipmf3_in_port, out_port, field_stage, Cint_field_dt_multi_fg_info_array[fg_iter]);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in cint_field_dt_multi_fg_create\n", rv);
            return rv;
        }
    }

    return rv;
}

/**
 * \brief
 *  Destroys all allocated data by the configuration.
 *
 * \param [in] unit  - Device id
 * \param [in] field_stage  - Field stage on which configuration have to be erased.
 * \param [in] is_second_fg - Flag to indicate if the iterator reached the second group,
 *  in case 'yes' we have to delete the constant qualifier, if not it means that
 *  we are deleting information about the first group. Used because both groups
 *  are using same qualifier.
 * \param [in] dt_fg_info  - Global Array, which contains
 *  all needed information to be destroyed.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_dt_multi_destroy_internal(
    int unit,
    bcm_field_stage_t field_stage,
    int is_second_fg,
    cint_field_dt_multi_fg_info_t * dt_fg_info)
{
    int rv = BCM_E_NONE;
    bcm_field_stage_t field_stage_internal;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t p_id;

    rv = bcm_field_entry_delete(unit, dt_fg_info->fg_id, dt_fg_info->entry_info.entry_qual, dt_fg_info->entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, dt_fg_info->fg_id, dt_fg_info->context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }

    rv = bcm_field_group_delete(unit, dt_fg_info->fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_delete\n");
        return rv;
    }

    /** In case of IPMF2 stage the context was created in IPMF1 stage. */
    if (field_stage == bcmFieldStageIngressPMF2)
    {
        field_stage_internal = bcmFieldStageIngressPMF1;
    }
    else
    {
        field_stage_internal = field_stage;
    }

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    p_id.stage = field_stage_internal;
    p_id.presel_id = dt_fg_info->presel_id;
    rv = bcm_field_presel_set(unit, 0, &p_id, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_presel_set\n");
        return rv;
    }

    rv = bcm_field_context_destroy(unit, field_stage_internal, dt_fg_info->context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy ipmf1 \n", rv);
        return rv;
    }

    if (is_second_fg)
    {
        rv = bcm_field_qualifier_destroy(unit, Cint_field_dt_multi_qual_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_qualifier_destroy. \n");
            return rv;
        }

         /** Q2A */
        if (Cint_field_dt_multi_dt_max_key_size_dnx_data_value == 10 && field_stage == bcmFieldStageEgress)
        {
            rv = bcm_field_qualifier_destroy(unit, Cint_field_dt_multi_eth_samac_8b_qual_id);
            if (rv != BCM_E_NONE)
            {
                printf("Error in bcm_field_qualifier_destroy. \n");
                return rv;
            }
            Cint_field_dt_multi_eth_samac_8b_qual_id = -1;
        }
    }

    return rv;
}

/**
 * \brief
 *  Destroys all allocated data by the configuration.
 *
 * \param [in] unit         - Device id
 * \param [in] field_stage  - Field stage for which info should be erased.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_dt_multi_destroy(
    int unit,
    bcm_field_stage_t field_stage)
{
    int rv = BCM_E_NONE;
    int fg_iter;

    for (fg_iter = 0; fg_iter < Cint_field_dt_multi_nof_fg; fg_iter++)
    {
        rv = cint_field_dt_multi_destroy_internal(unit, field_stage, fg_iter, Cint_field_dt_multi_fg_info_array[fg_iter]);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_entry_delete\n", rv);
            return rv;
        }
    }

    return rv;
}
