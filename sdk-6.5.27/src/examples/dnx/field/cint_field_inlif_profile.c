 /*
 * Configuration example start:
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/field/cint_field_inlif_profile.c
  cint;
  int unit = 0;
  bcm_field_context_t context_id = 0;
  uint32 inlif = 0x44901000;
  int inlif_profile = 2;
  cint_field_inlif_profile_main(unit, context_id, inlif, inlif_profile);
 * Test Scenario - end
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

    /*map the LIF to inLIF profile to use as qualifier value */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit,inlif,bcmPortClassFieldIngressVport,inlif_profile), "");


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
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &fg_info, &cint_inlif_profile_fg_id), "");

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    printf("Attaching field group\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, cint_inlif_profile_fg_id, context_id, &attach_info), "");

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
    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, cint_inlif_profile_fg_id, &ent_info, &cint_inlif_profile_ent_id), "");

    return BCM_E_NONE;
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
    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, cint_inlif_profile_fg_id, NULL, cint_inlif_profile_ent_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, cint_inlif_profile_fg_id, context_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, cint_inlif_profile_fg_id), "");

    return BCM_E_NONE;
}
