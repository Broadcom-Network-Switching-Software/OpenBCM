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
 * cint ../../../src/examples/dnx/field/cint_field_inlif_profile.c
 * cint;
 * int unit = 0;
 * bcm_field_context_t context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;
 * uint32 inlif = 0x44901000;
 * int inlif_profile = 2;
 * cint_field_inlif_profile_main(unit, context_id, inlif, inlif_profile);
 *
 * Configuration example end
 *
 * Example of using inPort qualifier which is encoded as GPORT.
 */

bcm_field_group_t   cint_inlif_profile_fg_id = 0;
bcm_field_entry_t  cint_inlif_profile_ent_id;


/**
* \brief
*  Configures the written above
* \param [in] unit        -  Device id
* \param [in] context_id  -  PMF context Id
* \param [in] inlif     -  In-LIF
* \param [in] inlif_profile -  In-LIF profile to qualify upon
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_inlif_profile_main(int unit, bcm_field_context_t context_id, uint32 inlif, int inlif_profile)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_action_info_t action_info;
    void *dest_char;

    int rv = BCM_E_NONE;

    /*map the LIF to inLIF profile to use as qualifier value */
    rv = bcm_port_class_set(unit,inlif,bcmPortClassFieldIngressVport,inlif_profile);
	if (rv != BCM_E_NONE)
	{
		printf("Error (%d), in bcm_port_class_set\n", rv);
		return rv;
	}
    /*
     * Create and attach TCAM group in iPMF1
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyInVportClass0;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionForward;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "inlif_fg", sizeof(fg_info.name));

    printf("Creating first group\n");
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_inlif_profile_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    /*
     * Attach the FG to context
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    printf("Attaching field group\n");
    rv = bcm_field_group_context_attach(unit, 0, cint_inlif_profile_fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = inlif_profile;
    ent_info.entry_qual[0].mask[0] = 0xf;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = 0xabcd;

    printf("Adding Entry\n");
    rv = bcm_field_entry_add(unit, 0, cint_inlif_profile_fg_id, &ent_info, &cint_inlif_profile_ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    return 0;
}

/**
* \brief
*  Destroy all configuration done
* \param [in] unit        -  Device id
* \param [in] context_id  -  PMF context Id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_inlif_profile_destroy(int unit, bcm_field_context_t context_id)
{
    int rv = BCM_E_NONE;

    /* Delete entry from Field group */
    rv = bcm_field_entry_delete(unit, cint_inlif_profile_fg_id, NULL, cint_inlif_profile_ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete entry %d from fg %d\n", rv, cint_inlif_profile_ent_id, cint_inlif_profile_fg_id);
        return rv;
    }
    /* Detach the IPMF1 FG from its context */
    rv = bcm_field_group_context_detach(unit, cint_inlif_profile_fg_id, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, cint_inlif_profile_fg_id, context_id);
        return rv;
    }

    /* Delete  FG */
    rv = bcm_field_group_delete(unit, cint_inlif_profile_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, cint_inlif_profile_fg_id);
        return rv;
    }

    return rv;
}
