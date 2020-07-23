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
    bcm_gport_t in_port_gport;

    int rv = BCM_E_NONE;

   printf("Creating context for App_type presel...\r\n");

    bcm_field_context_info_t_init(&context_info);
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &cint_field_app_type_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

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

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set \n", rv);
        return rv;
    }

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

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_app_type_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    printf("Attaching  FG to the created context...\r\n");

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_app_type_fg_id, cint_field_app_type_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    BCM_GPORT_LOCAL_SET(in_port_gport,in_port);
    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = in_port_gport;
    ent_info.entry_qual[0].mask[0] = 0x1ff;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = 0xabcd;

    printf("Adding Entry\n");
    rv = bcm_field_entry_add(unit, 0, cint_field_app_type_fg_id, &ent_info, &cint_field_app_type_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    return 0;

    return rv;
}

int field_app_type_destroy(int unit, bcm_field_presel_t presel_id)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t p_id;
    int rv = BCM_E_NONE;

    rv = bcm_field_entry_delete(unit, cint_field_app_type_fg_id, NULL, cint_field_app_type_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete  fg %d \n", rv, cint_field_app_type_fg_id);
        return rv;
    }

    printf("Detaching  FG from created context...\r\n");
    rv = bcm_field_group_context_detach(unit, cint_field_app_type_fg_id, cint_field_app_type_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, cint_field_app_type_fg_id, cint_field_app_type_context_id);
        return rv;
    }


    printf("Deleting  FG...\r\n");
    rv = bcm_field_group_delete(unit, cint_field_app_type_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, cint_field_app_type_fg_id);
        return rv;
    }


    printf("Destroying created preselector...\r\n");
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_id.presel_id = presel_id;
    rv = bcm_field_presel_set(unit, 0, &p_id, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_presel_set\n");
        return rv;
    }

    printf("Destroying created context...\r\n");
    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, cint_field_app_type_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy contex_id %d \n", rv, cint_field_app_type_context_id);
        return rv;
    }

    return rv;
}
