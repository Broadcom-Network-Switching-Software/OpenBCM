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
 *  cint ../../../../src/examples/dnx/field/cint_field_dt_multi.c
 *  cint ../../../../src/examples/dnx/field/cint_field_dt_multi_entries.c
 *  cint;
 *  int unit = 0;
 *  bcm_port_t port1 = 200;
 *  bcm_port_t port2 = 201;
 *  bcm_port_t port3 = 202;
 *  bcm_port_t port4 = 203;
 *  bcm_field_stage_t field_stage = bcmFieldStageIngressPMF1;
 *  int bank_mode = 0;
 *  cint_field_dt_multi_entries_dt_fg_main(unit, port1, port2, port3, port4, field_stage, bank_mode);
 *
 *  Configuration example end
 *
 *  Test scenario:
 *      1. Create 1 DT FG - use bank 8.
 *      2. Use as qualifier:
 *          - Ingress: bcmFieldQualifyInPort
 *          - Egress: bcmFieldQualifyOutPort
 *      3. USe action:
 *          - bcmFieldActionPrioIntNew
 *      4. Attach to context.
 *      5. Add entries.
 *      6. Set Qualifier values for entries as follow:
 *          - Entry 0 ---> Port 200
 *          - Entry 500 ---> Port 201
 *          - Entry 1000 ---> Port 202
 *          - Entry 1500 ---> Port 203
 *      7. Set Action values for entries as follow:
 *          - Entry 0 ---> 2
 *          - Entry 500 ---> 3
 *          - Entry 1000 ---> 4
 *          - Entry 1500 ---> 5
 *      8. Send 4 packet to match on the 4 different entries, verify that only TC is being updated with relevant values.
 *      9. Delete the first entries from 0 to 1023 and send packets again. Verify that both signals, related to those entries are not being updated.
 *
 *      - The exhaustive scenario is done for all stages:
 *          bcmFieldStageIngressPMF1/2/3 and bcmFieldStageEgress.
 */

int Cint_field_dt_multi_entries_nof_quals = 1;
int Cint_field_dt_multi_entries_nof_actions = 1;
int Cint_field_dt_multi_entries_bank_id = 8;
int Cint_field_dt_multi_entries_nof_valid_entries = 4;
int Cint_field_dt_multi_entries_action_tc_value[Cint_field_dt_multi_entries_nof_valid_entries] = {2, 3, 4, 5};
int Cint_field_dt_multi_entries_nof_entries = 8;
bcm_field_presel_t Cint_field_dt_multi_entries_presel_id = 51;
uint32 Cint_field_dt_multi_entries_presel_qual_value = bcmFieldLayerTypeEth;
uint32 Cint_field_dt_multi_entries_presel_qual_arg = 0;

bcm_field_entry_info_t Cint_field_dt_multi_entries_entry_info_array[Cint_field_dt_multi_entries_nof_entries];
bcm_field_entry_t Cint_field_dt_multi_entries_entry_handle[Cint_field_dt_multi_entries_nof_entries];

/**
 * Following array is global, used for all test cases. It contains information per field group. Like:
 * fg_id, fg_name, entry_handle, quals, actions.
 */
cint_field_dt_multi_fg_info_t Cint_field_dt_multi_entries_fg_info_array[1] = {
    /** DT FG info. */
    {
     0,         /** Field group ID */
     "DT_multi_entries",          /** Field group Name */
     0,                            /* The Entry handle. */
     {NULL},        /* Entry Info. */
     0,      /** Qualifier value. */
     bcmFieldActionPrioIntNew,      /** Action Type .*/
     0,    /** Action value. */
     Cint_field_dt_multi_entries_presel_id,       /* Presel Id. */
     Cint_field_dt_multi_entries_presel_qual_value,      /* Presel Value. */
     Cint_field_dt_multi_entries_presel_qual_arg,     /* Presel arg*/
     0     /* Context Id. */
    }
};

/**
 * \brief
 *  This function created a field group and attaching it to a context.
 *
 * \param [in] unit   - Device id
 * \param [in] field_stage  - Field stage on which configuration will be done.
 * \param [in] dt_fg_info  - Global Array, which contains information per field group.
 * \param [in] bank_mode - 1 for 'SELECT' 0 for 'AUTO'
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_dt_multi_entries_fg_create(
    int unit,
    bcm_field_stage_t field_stage,
    cint_field_dt_multi_fg_info_t * dt_fg_info,
    int bank_mode)
{
    int rv = BCM_E_NONE;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_stage_t field_stage_internal;
    void *dest_char;

    /** In case of IPMF2 stage the context should be created in IPMF1 stage. */
    if (field_stage == bcmFieldStageIngressPMF2)
    {
        field_stage_internal = bcmFieldStageIngressPMF1;
    }
    else
    {
        field_stage_internal = field_stage;
    }

    rv = cint_field_dt_multi_context_create(unit, 0, field_stage_internal, dt_fg_info);
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
    fg_info.nof_quals = Cint_field_dt_multi_entries_nof_quals;
    fg_info.nof_actions = Cint_field_dt_multi_entries_nof_actions;
    if (field_stage == bcmFieldStageEgress)
    {
        fg_info.qual_types[0] = bcmFieldQualifyOutPort;
    }
    else
    {
        fg_info.qual_types[0] = bcmFieldQualifyInPort;
    }
    fg_info.action_types[0] = dt_fg_info->action_type;
    fg_info.action_with_valid_bit[0] = TRUE;
    /*
     * Request specific bank
     */
    if (bank_mode == 1)
    {
        fg_info.tcam_info.bank_allocation_mode = bcmFieldTcamBankAllocationModeSelect;
        fg_info.tcam_info.nof_tcam_banks = 1;
        fg_info.tcam_info.tcam_bank_ids[0] = Cint_field_dt_multi_entries_bank_id;
        printf("Creating DT FG with Select mode\n");
    }
    else
    {
        fg_info.tcam_info.bank_allocation_mode = bcmFieldTcamBankAllocationModeAuto;
        printf("Creating DT FG with Auto mode\n");
    }

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
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    rv = bcm_field_group_context_attach(unit, 0, dt_fg_info->fg_id, dt_fg_info->context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Field Group ID %d was attached to Context ID %d. \n", dt_fg_info->fg_id, dt_fg_info->context_id);

    return rv;
}

/**
 * \brief
 *  This function adding an entry to a given field group.
 *
 * \param [in] unit   - Device id
 * \param [in] entry_index  - Index of the current entry.
 * \param [in] port  - Port to be used as entry qualifier value.
 * \param [in] is_dummy_port  - if true, uses raw qualifier
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
cint_field_dt_multi_entries_entry_create(
    int unit,
    int entry_index,
    bcm_port_t port,
    int is_dummy_port,
    bcm_field_stage_t field_stage,
    cint_field_dt_multi_fg_info_t * dt_fg_info)
{
    int rv = BCM_E_NONE;
    bcm_gport_t gport;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags;
    int core;

    if (is_dummy_port)
    {
        core = 0;
    }
    else
    {
        rv = bcm_port_get(unit, port, &flags, &interface_info, &mapping_info);
        if (rv != BCM_E_NONE)
        {
          printf("Error (%d), in bcm_port_get\n", rv);
          return rv;
        }
        core = mapping_info.core;
    }
    bcm_field_entry_info_t_init(&Cint_field_dt_multi_entries_entry_info_array[entry_index]);
    Cint_field_dt_multi_entries_entry_info_array[entry_index].nof_entry_quals = Cint_field_dt_multi_entries_nof_quals;
    Cint_field_dt_multi_entries_entry_info_array[entry_index].nof_entry_actions = Cint_field_dt_multi_entries_nof_actions;
    Cint_field_dt_multi_entries_entry_info_array[entry_index].core = core;

    if (field_stage == bcmFieldStageEgress)
    {
        Cint_field_dt_multi_entries_entry_info_array[entry_index].entry_qual[0].type = (is_dummy_port)?(bcmFieldQualifyOutPortRaw):(bcmFieldQualifyOutPort);
    }
    else
    {
        Cint_field_dt_multi_entries_entry_info_array[entry_index].entry_qual[0].type = (is_dummy_port)?(bcmFieldQualifyInPortRaw):(bcmFieldQualifyInPort);
    }
    BCM_GPORT_LOCAL_SET(gport, port);

    Cint_field_dt_multi_entries_entry_info_array[entry_index].entry_qual[0].value[0] = (is_dummy_port)?(port):(gport);
    Cint_field_dt_multi_entries_entry_info_array[entry_index].entry_qual[0].mask[0] = 0x1FF;
    Cint_field_dt_multi_entries_entry_info_array[entry_index].entry_action[0].type = dt_fg_info->action_type;
    Cint_field_dt_multi_entries_entry_info_array[entry_index].entry_action[0].value[0] = dt_fg_info->action_value;

    rv = bcm_field_entry_add(unit, 0, dt_fg_info->fg_id, Cint_field_dt_multi_entries_entry_info_array[entry_index], &dt_fg_info->entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    printf("Entry IDs added %d out of %d to Field Group ID %d. \n", (entry_index + 1),
               Cint_field_dt_multi_entries_nof_entries, dt_fg_info->fg_id);

    return rv;
}

/**
 * \brief
 *  This function configures DT field group for the given stage.
 *
 * \param [in] unit   - Device id
 * \param [in] port1   - Port1, from which the packet was sent.
 * \param [in] port2   - Port2, from which the packet was sent.
 * \param [in] port3   - Port3, from which the packet was sent.
 * \param [in] port4   - Port4, from which the packet was sent.
 * \param [in] field_stage  - Field stage on which configuration have to be done.
 * \param [in] bank_mode - 1 for 'SELECT' 0 for 'AUTO'
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_dt_multi_entries_dt_fg_main(
    int unit,
    bcm_port_t port1,
    bcm_port_t port2,
    bcm_port_t port3,
    bcm_port_t port4,
    bcm_field_stage_t field_stage,
    int bank_mode)
{
    int rv = BCM_E_NONE;
    bcm_port_t port, dummy_port;
    int valid_entry_index = 0;
    int entry_index;

    printf("*************************** Stage: %s ***************************\n\n", cint_field_utils_stage_name_get(field_stage));

    rv = cint_field_dt_multi_entries_fg_create(unit, field_stage, Cint_field_dt_multi_entries_fg_info_array[0], bank_mode);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_dt_multi_fg_create\n", rv);
        return rv;
    }

    for (entry_index = 0; entry_index < Cint_field_dt_multi_entries_nof_entries; entry_index++)
    {
        switch (valid_entry_index)
        {
            case 0:
            {
                port = port1;
                dummy_port = 14;
                break;
            }
            case 1:
            {
                port = port2;
                dummy_port = 15;
                break;
            }
            case 2:
            {
                port = port3;
                dummy_port = 16;
                break;
            }
            case 3:
            {
                port = port4;
                dummy_port = 17;
                break;
            }
            default:
            {
                port = port1;
                dummy_port = 14;
                break;
            }
        }

        if (valid_entry_index < Cint_field_dt_multi_entries_nof_valid_entries)
        {
            Cint_field_dt_multi_entries_fg_info_array[0].action_value = Cint_field_dt_multi_entries_action_tc_value[valid_entry_index];
        }

        if (((entry_index+1) % 2) == 0)
        {
            rv = cint_field_dt_multi_entries_entry_create(unit, entry_index, port, FALSE, field_stage, Cint_field_dt_multi_entries_fg_info_array[0]);
            if (rv != BCM_E_NONE)
            {
                printf("Error (%d), in cint_field_dt_multi_entries_entry_create\n", rv);
                return rv;
            }

            valid_entry_index++;
        }
        else
        {
            rv = cint_field_dt_multi_entries_entry_create(unit, entry_index, dummy_port, TRUE, field_stage, Cint_field_dt_multi_entries_fg_info_array[0]);
            if (rv != BCM_E_NONE)
            {
                printf("Error (%d), in cint_field_dt_multi_entries_entry_create\n", rv);
                return rv;
            }
        }
        Cint_field_dt_multi_entries_entry_handle[entry_index] = Cint_field_dt_multi_entries_fg_info_array[0].entry_handle;
    }

    return rv;
}

/**
 * \brief
 *  Destroys all allocated data by the configuration.
 *
 * \param [in] unit  - Device id
 * \param [in] field_stage  - Field stage on which configuration have to be erased.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_dt_multi_entries_destroy(
    int unit,
    bcm_field_stage_t field_stage)
{
    int rv = BCM_E_NONE;
    bcm_field_stage_t field_stage_internal;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t p_id;
    int entry_index;

    for (entry_index = 0; entry_index < Cint_field_dt_multi_entries_nof_entries; entry_index++)
    {
        rv = bcm_field_entry_delete(unit, Cint_field_dt_multi_entries_fg_info_array[0].fg_id,
            Cint_field_dt_multi_entries_entry_info_array[entry_index].entry_qual, Cint_field_dt_multi_entries_entry_handle[entry_index]);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_entry_delete\n", rv);
            return rv;
        }
    }

    rv = bcm_field_group_context_detach(unit, Cint_field_dt_multi_entries_fg_info_array[0].fg_id, Cint_field_dt_multi_entries_fg_info_array[0].context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }

    rv = bcm_field_group_delete(unit, Cint_field_dt_multi_entries_fg_info_array[0].fg_id);
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
    p_id.presel_id = Cint_field_dt_multi_entries_fg_info_array[0].presel_id;
    rv = bcm_field_presel_set(unit, 0, &p_id, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_presel_set\n");
        return rv;
    }

    rv = bcm_field_context_destroy(unit, field_stage_internal, Cint_field_dt_multi_entries_fg_info_array[0].context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy ipmf1 \n", rv);
        return rv;
    }

    return rv;
}
