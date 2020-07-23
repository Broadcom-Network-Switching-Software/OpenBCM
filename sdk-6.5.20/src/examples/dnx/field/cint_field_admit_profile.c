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
 * cint ../../../src/examples/dnx/field/cint_field_admit_profile.c
 * cint;
 * int unit = 0;
 * bcm_field_context_t context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;
 * bcm_port_t in_port = 200;
 * bcm_core_t core_id = 1;
 * cint_field_admit_profile_main(unit, context_id, core_id, in_port);
 *
 * Configuration example end
 *
 *  Configuration:
 *      Example configuration of admit profile action (bcmFieldActionAdmitProfile) and to add an entry to update its value to 1.
 *      Admit profile values meaning:
 *          - 1 if ECN capable.
 *          - 0 if not ECN capable.
 */

/** Global variables used for storing the created FG, Entry IDs to be destroyed. */
bcm_field_group_t fg_id;    /* Field Group ID. */
bcm_field_entry_t entry_handle;
bcm_field_stage_t stage = bcmFieldStageIngressPMF1;
/**
* \brief
*
* \param [in] unit    - Device ID
* \param [in] context_id  -  PMF context Id
* \param [in] core_id  -  Core Id from which the packet will be sent.
* \param [in] in_port - In Port value to be converted to GPORT and qualify upon.
*
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_admit_profile_main(
    int unit,
    bcm_field_context_t context_id,
    bcm_core_t core_id,
    bcm_port_t in_port)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    bcm_gport_t in_port_gport;
    void *dest_char;
    int rv = BCM_E_NONE;

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_entry_info_t_init(&entry_info);

    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = stage;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "ADMIT_PROF_FG", sizeof(fg_info.name));
    fg_info.nof_quals = 1;
    fg_info.nof_actions = 1;
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.nof_entry_actions = fg_info.nof_actions;

    fg_info.qual_types[0] = bcmFieldQualifyInPort;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    if (!BCM_GPORT_IS_TRUNK(in_port)) {
        BCM_GPORT_LOCAL_SET(in_port_gport, in_port);
    } else {
        in_port_gport = in_port;
    }
    entry_info.entry_qual[0].value[0] = in_port_gport;
    entry_info.entry_qual[0].mask[0] = 0x1FF;
    entry_info.core = core_id;
    fg_info.action_types[0] = bcmFieldActionAdmitProfile;
    fg_info.action_with_valid_bit[0] = TRUE;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = 1;

    /** Create the field group. */
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add on field_group_id = %d \n", rv, fg_id);
        return rv;
    }

    printf("Field Group ID %d was created. \n", fg_id);

    /** Attach the created field group to the context. */
    rv = bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach on contex_id = %d \n", rv, context_id);
        return rv;
    }

    printf("Field Group ID %d was attached to Context ID %d. \n", fg_id, context_id);

    /** Add an entry to the created field group. */
    rv = bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    printf("Entry ID 0x%X (%d) was added to Field Group ID %d. \n", entry_handle, entry_handle, fg_id);

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
int cint_field_admit_profile_destroy(
    int unit,
    bcm_field_context_t context_id)
{
    int rv = BCM_E_NONE;

    rv = bcm_field_entry_delete(unit, fg_id, NULL, entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, fg_id, context_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach\n", rv);
        return rv;
    }

    rv = bcm_field_group_delete(unit, fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete\n", rv);
        return rv;
    }

    return rv;
}
