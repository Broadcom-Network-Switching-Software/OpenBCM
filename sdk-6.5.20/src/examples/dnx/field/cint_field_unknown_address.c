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
 * cint ../../src/examples/dnx/field/cint_field_unknown_address.c
 * cint;
 * int unit = 0;
 * bcm_field_context_t context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;
 * cint_field_unknown_address_main(unit, context_id);
 *
 * Configuration example end
 *
 *  This cint configures and shows example of bcmFieldQualifyPacketRes qualifier to
 *  qualify upon unknown address destination in IPMF1.
 */

bcm_field_group_t Cint_field_unknown_address_fg_id;
bcm_field_entry_t Cint_field_unknown_address_entry_handle;

int Cint_field_unknown_address_tc_action_value = 6;

/**
* \brief
*   Configure field group, attaching it to a context and adding an entry to it.
*
* \param [in] unit    - Device ID.
* \param [in] context_id  -  PMF context Id.
*
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_unknown_address_main(
    int unit,
    bcm_field_context_t context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    void *dest_char;
    int rv = BCM_E_NONE;

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_entry_info_t_init(&entry_info);

    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "unknown_address", sizeof(fg_info.name));
    fg_info.nof_quals = 1;
    fg_info.nof_actions = 1;
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.nof_entry_actions = fg_info.nof_actions;

    fg_info.qual_types[0] = bcmFieldQualifyPacketRes;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = 1;
    entry_info.entry_qual[0].mask[0] = 0x1;

    fg_info.action_types[0] = bcmFieldActionPrioIntNew;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = Cint_field_unknown_address_tc_action_value;

    /** Create the field group. */
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &Cint_field_unknown_address_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add on field_group_id = %d \n", rv, Cint_field_unknown_address_fg_id);
        return rv;
    }

    printf("Field Group ID %d was created. \n", Cint_field_unknown_address_fg_id);

    /** Attach the created field group to the context. */
    rv = bcm_field_group_context_attach(unit, 0, Cint_field_unknown_address_fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach on contex_id = %d \n", rv, context_id);
        return rv;
    }

    printf("Field Group ID %d was attached to Context ID %d. \n", Cint_field_unknown_address_fg_id, context_id);

    /** Add an entry to the created field group. */
    rv = bcm_field_entry_add(unit, 0, Cint_field_unknown_address_fg_id, &entry_info, &Cint_field_unknown_address_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    printf("Entry ID 0x%X (%d) was added to Field Group ID %d. \n", Cint_field_unknown_address_entry_handle,
       Cint_field_unknown_address_entry_handle, Cint_field_unknown_address_fg_id);

    return rv;
}

/**
 * \brief
 *  Destroys all allocated data by the configuration.
 *
 * \param [in] unit   - Device id
 * \param [in] context_id  -  PMF context Id
 *
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_unknown_address_destroy(
    int unit,
    bcm_field_context_t context_id)
{
    int rv = BCM_E_NONE;

    rv = bcm_field_entry_delete(unit, Cint_field_unknown_address_fg_id, NULL, Cint_field_unknown_address_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, Cint_field_unknown_address_fg_id, context_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach\n", rv);
        return rv;
    }

    rv = bcm_field_group_delete(unit, Cint_field_unknown_address_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete\n", rv);
        return rv;
    }

    return rv;
}
