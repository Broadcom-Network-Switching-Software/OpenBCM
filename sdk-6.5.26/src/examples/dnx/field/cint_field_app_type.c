/*
 * Configuration example start:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../src/examples/dnx/field/cint_field_app_type.c
 * cint;
 * int unit = 0;
 * bcm_field_presel_t presel_id = 50;
 * bcm_field_AppType_t app_type = bcmFieldAppTypeTrafficManagement;
 * bcm_port_t in_port = 201;
 * field_app_type_main(unit, presel_id, app_type, in_port);
 *
 * Configuration example end
 *
 * Example cint to use the App type as Context selector for iPMF1 stage
 *
 */

bcm_field_group_t cint_field_app_type_fg_id = BCM_FIELD_INVALID;
bcm_field_context_t cint_field_app_type_context_id = BCM_FIELD_INVALID;
bcm_field_entry_t cint_field_app_type_entry_id = BCM_FIELD_INVALID;

/**
* \brief
*  Configures the following scenario:


*/
int field_app_type_main(int unit, bcm_field_presel_t presel_id, bcm_field_AppType_t app_type, bcm_port_t in_port)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;    
    bcm_field_presel_entry_id_t p_id;
    bcm_field_entry_info_t ent_info;

   printf("Creating context for App_type presel...\r\n");

    bcm_field_context_info_t_init(&context_info);
    BCM_IF_ERROR_RETURN(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &cint_field_app_type_context_id));

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);
    
    p_id.presel_id = presel_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_app_type_context_id;
    p_data.nof_qualifiers = 2;

    /*
     * We can either use bcmFieldQualifyAppType or bcmFieldQualifyAppTypePredefined. 
     * Here for example we use both (unnecessarily).
     * bcmFieldQualifyAppTypePredefined qualifies upon the apptype according to the FWD context, regardless of
     * any user defined apptypes created using bcm_field_context_create() in stage bcmFieldStageExternal.
     * bcmFieldQualifyAppType qualifies upon either the predefined apptype, or a user defined apptype if one was reached.
     * If no user defined apptype was reached, bcmFieldQualifyAppType and bcmFieldQualifyAppTypePredefined are identical.
     */
    p_data.qual_data[0].qual_type = bcmFieldQualifyAppType;
    p_data.qual_data[0].qual_value = app_type;
    p_data.qual_data[0].qual_mask = 0x1F;

    p_data.qual_data[1].qual_type = bcmFieldQualifyAppTypePredefined;
    p_data.qual_data[1].qual_value = app_type;
    p_data.qual_data[1].qual_mask = 0x1F;

    BCM_IF_ERROR_RETURN(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    printf("Presel (%d) was configured context(%d)\n",
        presel_id, cint_field_app_type_context_id);

    printf("Creating  FG...\r\n");

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyInPort;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionForward;

    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, &fg_info, &cint_field_app_type_fg_id));

    printf("Attaching  FG to the created context...\r\n");

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    BCM_IF_ERROR_RETURN(bcm_field_group_context_attach(unit, 0, cint_field_app_type_fg_id, cint_field_app_type_context_id, &attach_info));

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = in_port;
    ent_info.entry_qual[0].mask[0] = 0x1ff;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = 0xabcd;

    printf("Adding Entry\n");
    BCM_IF_ERROR_RETURN(bcm_field_entry_add(unit, 0, cint_field_app_type_fg_id, &ent_info, &cint_field_app_type_entry_id));

    return BCM_E_NONE;
}

int field_app_type_destroy(int unit, bcm_field_presel_t presel_id)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t p_id;

    printf("Running: field_app_type_destroy()\n");
    BCM_IF_ERROR_RETURN(bcm_field_entry_delete(unit, cint_field_app_type_fg_id, NULL, cint_field_app_type_entry_id));
    BCM_IF_ERROR_RETURN(bcm_field_group_context_detach(unit, cint_field_app_type_fg_id, cint_field_app_type_context_id));
    BCM_IF_ERROR_RETURN(bcm_field_group_delete(unit, cint_field_app_type_fg_id));

    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_id.presel_id = presel_id;
    BCM_IF_ERROR_RETURN(bcm_field_presel_set(unit, 0, &p_id, &presel_entry_data));

    BCM_IF_ERROR_RETURN(bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, cint_field_app_type_context_id));

    return BCM_E_NONE;
}
