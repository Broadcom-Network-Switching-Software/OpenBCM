/*
 * Configuration example start:
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/field/cint_field_external.c
  cint;
  int unit = 0;
  cint_field_external_main(unit);
  cint_field_external_shared_acl_main(unit);
 * Test Scenario - end
 *
 * Configuration example end
 *
 */


/*
 * Global variables.
 * Declared global to make easy traffic and destroy functions.
 * */

bcm_field_group_t cint_field_external_fg_id = BCM_FIELD_INVALID;
bcm_field_group_t cint_field_external_fg_2_id = BCM_FIELD_INVALID;
uint32 payload_id = 0;
uint32 payload_id_2 = 1;
bcm_field_AppType_t external_app_type = bcmFieldAppTypeTrafficManagement;
bcm_field_presel_entry_id_t presel_entry_id;
uint32 external_fg_payload_offset = 184;
uint32 external_fg_payload_offset_2 = 136;
uint32 external_fg_payload_value = 0xabc;
uint32 external_fg_payload_updated_value = 0xdef;
bcm_field_entry_t entry_id;
bcm_field_entry_t traffic_entry_id;

bcm_mac_t my_mac_address        = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};

int
cint_field_external_fg_add(
    int unit,
    bcm_field_group_info_t *fg_info_p,
    bcm_field_group_t * fg_id_p,
    char * name)
{
    int qual_idx = 0;
    bcm_field_action_t bcm_action;
    bcm_field_action_info_t action_info;
    void *dest_char;
    char *char_dest_char;
    printf("Running: cint_field_external_fg_add()\n");

    fg_info_p->stage = bcmFieldStageExternal;
    fg_info_p->fg_type = bcmFieldGroupTypeExternalTcam;

    dest_char = &(fg_info_p->name[0]);
    char_dest_char = dest_char;

    snprintf(char_dest_char, sizeof(fg_info_p->name), "ext_FG_%s", name);

    fg_info_p->qual_types[qual_idx++] = bcmFieldQualifySrcMac;
    fg_info_p->qual_types[qual_idx++] = bcmFieldQualifyDstMac;
    fg_info_p->qual_types[qual_idx++] = bcmFieldQualifySrcIp6;
    fg_info_p->qual_types[qual_idx++] = bcmFieldQualifySrcIp;
    fg_info_p->nof_quals = qual_idx;
    /*
     *  Create a void user defined action
     */
    bcm_field_action_info_t_init(&action_info);

    action_info.stage = bcmFieldStageExternal;
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = 32;
    /*
     * size + prefix_size summary should be 32
     */
    action_info.prefix_size = 0;

    dest_char = &(action_info.name[0]);
    char_dest_char = dest_char;
    snprintf(char_dest_char, sizeof(action_info.name), "ext_UDA_%s", name);

    BCM_IF_ERROR_RETURN_MSG(bcm_field_action_create(unit, 0, &action_info, &bcm_action), "");

    fg_info_p->action_types[0] = bcm_action;
    fg_info_p->action_with_valid_bit[0] = FALSE;

    fg_info_p->nof_actions = 1;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, fg_info_p, fg_id_p), "");

    return BCM_E_NONE;
}

int
cint_field_external_fg_attach(
    int unit,
    bcm_field_group_t external_fg_id,
    bcm_field_group_info_t *fg_info_p,
    bcm_field_AppType_t external_app_type /*opcode*/,
    uint32 payload_id,
    uint32 payload_offset)
{
    uint32 qual_ndx, action_ndx;
    bcm_field_group_attach_info_t group_attach_info;
    bcm_field_context_t context_id;
    printf("Running: cint_field_external_fg_attach()\n");

    bcm_field_group_attach_info_t_init(&group_attach_info);

    /*
     * Build the attach information
     */
    group_attach_info.key_info.nof_quals = fg_info_p->nof_quals;
    group_attach_info.payload_info.nof_actions = fg_info_p->nof_actions;
    /* Payload id is the lookup id in the external */
    group_attach_info.payload_info.payload_id = payload_id;
    /*
     * Payload offset is the location of the result inside the external payload buffer.
     * In order to extract the result this value should be used
     * as the qualifier offset in the PMF12.
     * */
    group_attach_info.payload_info.payload_offset = payload_offset;

    for (qual_ndx = 0; qual_ndx < fg_info_p->nof_quals; qual_ndx++)
    {
        group_attach_info.key_info.qual_types[qual_ndx] = fg_info_p->qual_types[qual_ndx];
    }

    for (action_ndx = 0; action_ndx < fg_info_p->nof_actions; action_ndx++)
    {
        group_attach_info.payload_info.action_types[action_ndx] = fg_info_p->action_types[action_ndx];
    }
    /*
     * Build the qualifier attach information
     */
    qual_ndx = 0;
    group_attach_info.key_info.qual_info[qual_ndx].input_arg = 0;
    group_attach_info.key_info.qual_info[qual_ndx].input_type = bcmFieldInputTypeLayerFwd;
    group_attach_info.key_info.qual_info[qual_ndx++].offset = 0;

    group_attach_info.key_info.qual_info[qual_ndx].input_arg = 0;
    group_attach_info.key_info.qual_info[qual_ndx].input_type = bcmFieldInputTypeLayerFwd;
    group_attach_info.key_info.qual_info[qual_ndx++].offset = 0;

    group_attach_info.key_info.qual_info[qual_ndx].input_arg = 2;
    group_attach_info.key_info.qual_info[qual_ndx].input_type = bcmFieldInputTypeLayerAbsolute;
    group_attach_info.key_info.qual_info[qual_ndx++].offset = 0xf;

    group_attach_info.key_info.qual_info[qual_ndx].input_arg = 0;
    group_attach_info.key_info.qual_info[qual_ndx].input_type = bcmFieldInputTypeLayerFwd;
    group_attach_info.key_info.qual_info[qual_ndx++].offset = 0;
    /* Cast */
    context_id  = (external_app_type & 0x00ff) ;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, external_fg_id, context_id, &group_attach_info), "");

    return BCM_E_NONE;
}


int
cint_field_external_entry_add(
    int unit,
    bcm_field_group_t external_fg_id,
    bcm_field_group_info_t *fg_info_p)
{
    uint32 qual_ndx, action_ndx;
    bcm_field_entry_info_t entry_info;

    printf("Running: cint_field_external_entry_add()\n");

    bcm_field_entry_info_t_init(&entry_info);

    for (qual_ndx = 0; qual_ndx < fg_info_p->nof_quals; qual_ndx++)
    {
        entry_info.entry_qual[qual_ndx].type = fg_info_p->qual_types[qual_ndx];
        if (entry_info.entry_qual[qual_ndx].type == bcmFieldQualifyInPort)
        {
            bcm_gport_t gport;
            BCM_GPORT_LOCAL_SET(gport, 13);
            entry_info.entry_qual[qual_ndx].value[0] = gport;
            entry_info.entry_qual[qual_ndx].mask[0] = gport;
        }
        else
        {
            entry_info.entry_qual[qual_ndx].value[0] = qual_ndx;
            entry_info.entry_qual[qual_ndx].mask[0] = qual_ndx;
        }
    }

    entry_info.nof_entry_quals = fg_info_p->nof_quals;

    for (action_ndx = 0; action_ndx < fg_info_p->nof_actions; action_ndx++)
    {
        entry_info.entry_action[action_ndx].type = fg_info_p->action_types[action_ndx];
        entry_info.entry_action[action_ndx].value[0] = external_fg_payload_value;
    }

    entry_info.nof_entry_actions = fg_info_p->nof_actions;


    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, external_fg_id, &entry_info, &entry_id), "");

    return BCM_E_NONE;
}

int
cint_field_external_entry_update(
    int unit,
    bcm_field_group_t external_fg_id,
    bcm_field_group_info_t *fg_info_p)
{
    uint32 qual_ndx, action_ndx;
    bcm_field_entry_info_t entry_info;

    printf("Running: cint_field_external_entry_update()\n");

    bcm_field_entry_info_t_init(&entry_info);

    for (qual_ndx = 0; qual_ndx < fg_info_p->nof_quals; qual_ndx++)
    {
        entry_info.entry_qual[qual_ndx].type = fg_info_p->qual_types[qual_ndx];
        if (entry_info.entry_qual[qual_ndx].type == bcmFieldQualifyInPort)
        {
            bcm_gport_t gport;
            BCM_GPORT_LOCAL_SET(gport, 13);
            entry_info.entry_qual[qual_ndx].value[0] = gport;
            entry_info.entry_qual[qual_ndx].mask[0] = gport;
        }
        else
        {
            entry_info.entry_qual[qual_ndx].value[0] = qual_ndx;
            entry_info.entry_qual[qual_ndx].mask[0] = qual_ndx;
        }
    }

    entry_info.nof_entry_quals = fg_info_p->nof_quals;

    for (action_ndx = 0; action_ndx < fg_info_p->nof_actions; action_ndx++)
    {
        entry_info.entry_action[action_ndx].type = fg_info_p->action_types[action_ndx];
        entry_info.entry_action[action_ndx].value[0] = external_fg_payload_updated_value;
    }

    entry_info.nof_entry_actions = fg_info_p->nof_actions;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, BCM_FIELD_FLAG_UPDATE, external_fg_id, &entry_info, &entry_id), "");

    return BCM_E_NONE;
}

/**
 * \brief - run external main function
 *
 * \param [in] unit - the unit number in system
 *
 *  Usage: run in cint shell "cint_field_external_main(0)"
 */
int
cint_field_external_main(int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_t external_fg_id = cint_field_external_fg_id;

    printf("Running: cint_field_external_main()\n");

    bcm_field_group_info_t_init( &fg_info);

    BCM_IF_ERROR_RETURN_MSG(cint_field_external_fg_add(unit, &fg_info, &external_fg_id, "main"), "");

    BCM_IF_ERROR_RETURN_MSG(cint_field_external_fg_attach(unit, external_fg_id, &fg_info, external_app_type, payload_id, external_fg_payload_offset), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_set(unit, bcmSwitchExternalTcamSync, 0), "");

    BCM_IF_ERROR_RETURN_MSG(cint_field_external_entry_add(unit, external_fg_id, &fg_info), "");

    printf("Config external FG on IFWD2, fg_id=%d, external_app_type %d\n", external_fg_id,  (external_app_type & 0x00ff));

    return BCM_E_NONE;
}

/**
 * \brief - run external main function for shared ACL/FWD segments.
    Create 2 similar field groups, 
    Attach them both to the same context,
    Add entry to each field group.
 *
 * \param [in] unit - the unit number in system
 *
 *  Usage: run in cint shell "cint_field_external_shared_acl_main(0)"
 */
int
cint_field_external_shared_acl_main(int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_info_t fg_info_2;
    bcm_field_group_t external_fg_id = cint_field_external_fg_id;
    bcm_field_group_t external_fg_2_id = cint_field_external_fg_2_id;

    printf("Running: cint_field_external_shared_acl_main()\n");

    bcm_field_group_info_t_init( &fg_info);
    bcm_field_group_info_t_init( &fg_info_2);

    BCM_IF_ERROR_RETURN_MSG(cint_field_external_fg_add(unit, &fg_info, &external_fg_id, "1"), "");

    BCM_IF_ERROR_RETURN_MSG(cint_field_external_fg_attach(unit, external_fg_id, &fg_info, external_app_type, payload_id, external_fg_payload_offset), "");

    BCM_IF_ERROR_RETURN_MSG(cint_field_external_fg_add(unit, &fg_info_2, &external_fg_2_id, "2"), "");

    BCM_IF_ERROR_RETURN_MSG(cint_field_external_fg_attach(unit, external_fg_2_id, &fg_info_2, external_app_type, payload_id_2, external_fg_payload_offset_2), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_set(unit, bcmSwitchExternalTcamSync, 0), "");

    BCM_IF_ERROR_RETURN_MSG(cint_field_external_entry_add(unit, external_fg_id, &fg_info), "");

    BCM_IF_ERROR_RETURN_MSG(cint_field_external_entry_add(unit, external_fg_2_id, &fg_info_2), "");

    printf("Config external FG on IFWD2, fg_id=%d, AppType %d\n", external_fg_id,  (external_app_type & 0x00ff));
    printf("Config external FG on IFWD2, fg_id_2=%d, AppType %d\n", external_fg_2_id,  (external_app_type & 0x00ff));

    return BCM_E_NONE;
}
int
cint_field_external_destroy(int unit)
{
    bcm_field_group_info_t fg_info;

    printf("Running: cint_field_external_destroy()\n");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, external_fg_id, NULL, entry_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, external_fg_id, NULL, traffic_entry_id), "");

    return BCM_E_NONE;
}
