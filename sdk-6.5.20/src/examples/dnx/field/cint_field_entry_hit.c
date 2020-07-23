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
 * cint ../../src/examples/dnx/field/cint_field_presel_fwd_layer.c
 * cint ../../src/examples/dnx/field/cint_field_entry_hit.c
 * cint;
 * int unit = 0;
 * cint_field_entry_hit_group_config_main(unit);
 *
 * Configuration example end
 *
 *  Test scenario:
 *      - Use cint_field_entry_hit_group_config_main(unit) to:
 *          Configure a context selection with type of forwarding layer (bcmFieldLayerTypeEth).
 *          Configure a simple TCAM field group in IPMF1 to qualify upon ETH type and to perform
 *          Traffic Class action. Attach it to the configured context.
 *
 *      - Use cint_field_entry_hit_entry_config(unit, core_id):
 *          Creates an entry on a spefic core.
 *
 *      - Use cint_field_entry_hit_verify(unit, entry_hit_verify):
 *          Compares the given 'entry_hit_verify' to the returned result
 *          from the bcm_field_entry_hit_get().
 *
 *      - Use cint_field_entry_hit_destroy(unit):
 *          Destroys all configuration done by above functions.
 *
 *      For more info check the CINT functions headers!
 */

/** Number of qualifiers for different test cases. (used as array size)*/
int CINT_FIELD_ENTRY_HIT_NOF_QUALS = 1;
/** Number of actions to be performed. */
int CINT_FIELD_ENTRY_HIT_NOF_ACTIONS = 1;
/** Value to which the TC signal will be update. */
int CINT_FIELD_ENTRY_HIT_TC_ACTION_VALUE = 6;
/** Value and mask for ETH type qualifier to qualify upon. */
int CINT_FIELD_ENTRY_HIT_ETH_TYPE_QUAL_VALUE = 0x8100;
int CINT_FIELD_ENTRY_HIT_ETH_TYPE_QUAL_MASK = 0xFFFF;

/** Global variables to be passed through function. */
bcm_field_presel_t cint_field_entry_hit_presel_id;
bcm_field_entry_t cint_field_entry_hit_entry_handle;
bcm_field_group_t cint_field_entry_hit_fg_id;
bcm_field_context_t cint_field_entry_hit_context_id;
bcm_field_stage_t cint_field_entry_hit_field_stage = bcmFieldStageIngressPMF1;
bcm_field_group_info_t cint_field_entry_hit_fg_info;

/**
 * \brief
 *  This function creates field group, with all relevant
 *  information to it and attaching it to the context.
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
cint_field_entry_hit_group_config_main(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_group_attach_info_t attach_info;
    void *dest_char;

    /** Create FWD context. */
    rv = field_presel_fwd_layer_main(unit, cint_field_entry_hit_presel_id, cint_field_entry_hit_field_stage,
                                     bcmFieldLayerTypeEth, &cint_field_entry_hit_context_id, "ENTRY_HIT");
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in field_presel_fwd_layer_main \n", rv);
        return rv;
    }

    bcm_field_group_info_t_init(&cint_field_entry_hit_fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);

    cint_field_entry_hit_fg_info.fg_type = bcmFieldGroupTypeTcam;
    cint_field_entry_hit_fg_info.stage = cint_field_entry_hit_field_stage;

    dest_char = &(cint_field_entry_hit_fg_info.name[0]);
    sal_strncpy(dest_char, "ENTRY_HIT", sizeof(cint_field_entry_hit_fg_info.name));
    cint_field_entry_hit_fg_info.name[sizeof(cint_field_entry_hit_fg_info.name) - 1] = 0;

    cint_field_entry_hit_fg_info.nof_quals = CINT_FIELD_ENTRY_HIT_NOF_QUALS;
    cint_field_entry_hit_fg_info.nof_actions = CINT_FIELD_ENTRY_HIT_NOF_ACTIONS;
    attach_info.key_info.nof_quals = cint_field_entry_hit_fg_info.nof_quals;
    attach_info.payload_info.nof_actions = cint_field_entry_hit_fg_info.nof_actions;

    cint_field_entry_hit_fg_info.qual_types[0] = bcmFieldQualifyTpid;
    attach_info.key_info.qual_types[0] = cint_field_entry_hit_fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 96;

    cint_field_entry_hit_fg_info.action_types[0] = bcmFieldActionPrioIntNew;
    cint_field_entry_hit_fg_info.action_with_valid_bit[0] = TRUE;
    attach_info.payload_info.action_types[0] = cint_field_entry_hit_fg_info.action_types[0];

    /** Create the field group. */
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &cint_field_entry_hit_fg_info,
                             &cint_field_entry_hit_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add on field_group_id = %d \n", rv, cint_field_entry_hit_fg_id);
        return rv;
    }

    printf("Field Group ID %d was created. \n", cint_field_entry_hit_fg_id);

    /** Attach the created field group to the context. */
    rv = bcm_field_group_context_attach(unit, 0, cint_field_entry_hit_fg_id, cint_field_entry_hit_context_id,
                                        &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach on contex_id = %d \n", rv,
               cint_field_entry_hit_context_id);
        return rv;
    }

    printf("Field Group ID %d was attached to Context ID %d. \n", cint_field_entry_hit_fg_id,
           cint_field_entry_hit_context_id);

    return rv;
}

/**
 * \brief
 *  Function to create an entry on a given core.
 *
 * \param [in] unit   - Device id
 * \param [in] core_id  - Core ID on which the entry have to be created.
 *  Possible values:
 *      - '0' - in case we want to create an entry on core 0.
 *      - '1' - in case we want to create an entry on core 1.
 *      - '2' - in case we want to create an entry on both cores.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_entry_hit_entry_config(
    int unit,
    bcm_core_t core_id)
{
    int rv = BCM_E_NONE;
    bcm_field_entry_info_t entry_info;

    bcm_field_entry_info_t_init(&entry_info);

    entry_info.priority = 1;
    entry_info.nof_entry_quals = cint_field_entry_hit_fg_info.nof_quals;
    entry_info.nof_entry_actions = cint_field_entry_hit_fg_info.nof_actions;

    entry_info.entry_qual[0].type = cint_field_entry_hit_fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = CINT_FIELD_ENTRY_HIT_ETH_TYPE_QUAL_VALUE;
    entry_info.entry_qual[0].mask[0] = CINT_FIELD_ENTRY_HIT_ETH_TYPE_QUAL_MASK;

    entry_info.entry_action[0].type = cint_field_entry_hit_fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = CINT_FIELD_ENTRY_HIT_TC_ACTION_VALUE;

    entry_info.core = core_id;

    /** Add an entry to the created field group. */
    rv = bcm_field_entry_add(unit, 0, cint_field_entry_hit_fg_id, &entry_info, &cint_field_entry_hit_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    printf("Entry ID 0x%X (%d) was added to Field Group ID %d. \n", cint_field_entry_hit_entry_handle,
           cint_field_entry_hit_entry_handle, cint_field_entry_hit_fg_id);

    return rv;
}

/**
 * \brief
 *  Function to be used for verifying the entry hit results.
 *
 * \param [in] unit                 - Device id
 * \param [in] entry_hit_verify  -
 *  Verify value, which will be compared with the returned
 *  bitmap value from the bcm_field_entry_hit_get() and
 *  bcm_field_entry_hit_flush() APIs.
 *  Possible values to be verified:
 *      - '0' - in case we want to verify the FLUSH API, we expect 0
 *      to be returned by the GET API.
 *      - '1' - in case we want to verify that the HIT was indicated on core 0.
 *      - '2' - in case we want to verify that the HIT was indicated on core 1.
 *      - '3' - in case we want to verify that the HIT was indicated on both cores.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_entry_hit_verify(
    int unit,
    uint8 entry_hit_verify)
{
    int rv = BCM_E_NONE;
    uint8 entry_hit_core_bmp;

    rv = bcm_field_entry_hit_get(unit, 0, cint_field_entry_hit_entry_handle, &entry_hit_core_bmp);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_hit_get\n", rv);
        return rv;
    }

    if (entry_hit_core_bmp == entry_hit_verify && entry_hit_verify != 0)
    {
        printf("Entry Hit Get was successful for entry ID 0x%X (%d). \n", cint_field_entry_hit_entry_handle,
               cint_field_entry_hit_entry_handle);
    }
    else if (entry_hit_core_bmp == 0 && entry_hit_verify == 0)
    {
        printf("Entry Hit Flush was successful for entry ID 0x%X (%d). \n", cint_field_entry_hit_entry_handle,
               cint_field_entry_hit_entry_handle);
    }
    else if (entry_hit_core_bmp != 0 && entry_hit_verify == 0)
    {
        printf("Entry Hit Flush failed for entry ID 0x%X (%d). Expected value %d | Retrieved value %d \n",
               cint_field_entry_hit_entry_handle, cint_field_entry_hit_entry_handle, entry_hit_verify,
               entry_hit_core_bmp);
        rv = BCM_E_INTERNAL;
    }
    else
    {
        printf("Entry Hit Get failed for entry ID 0x%X (%d). Expected value %d | Retrieved value %d \n",
               cint_field_entry_hit_entry_handle, cint_field_entry_hit_entry_handle, entry_hit_verify,
               entry_hit_core_bmp);
        rv = BCM_E_INTERNAL;
    }

    return rv;
}

/**
 * \brief
 *  Destroys all allocated data by the configuration.
 *
 * \param [in] unit                 - Device id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_entry_hit_destroy(
    int unit)
{
    int rv = BCM_E_NONE;

    rv = bcm_field_entry_delete(unit, cint_field_entry_hit_fg_id, NULL, cint_field_entry_hit_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, cint_field_entry_hit_fg_id, cint_field_entry_hit_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }

    rv = bcm_field_group_delete(unit, cint_field_entry_hit_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_delete\n");
        return rv;
    }

    rv = field_presel_fwd_layer_destroy(unit, cint_field_entry_hit_presel_id, cint_field_entry_hit_field_stage,
                                        cint_field_entry_hit_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in field_presel_fwd_layer_destroy. \n");
        return rv;
    }

    return rv;
}
