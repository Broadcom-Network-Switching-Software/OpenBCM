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
 * cint ../../../src/examples/dnx/field/cint_field_external.c
 * cint;
 * int unit = 0;
 * cint_field_external_main(unit);
 * cint_field_external_shared_acl_main(unit);
 *
 * Configuration example end
 */


/*
 * Global variables.
 * Declared global to make easy traffic and destroy functions.
 */

bcm_field_group_t cint_field_external_fg_id = BCM_FIELD_INVALID;
bcm_field_group_t cint_field_external_fg_2_id = BCM_FIELD_INVALID;
uint32 payload_id = 0;
uint32 payload_id_2 = 1;
bcm_field_AppType_t external_app_type = bcmFieldAppTypeTrafficManagement;
bcm_field_presel_entry_id_t presel_entry_id;
uint32 external_fg_payload_offset = 184;
uint32 external_fg_payload_offset_2 = 136;
uint32 external_fg_payload_value = 0xabc;
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
    char *proc_name;
    int rv = BCM_E_NONE;
    int qual_idx = 0;
    bcm_field_action_t bcm_action;
    bcm_field_action_info_t action_info;
    void *dest_char;
    char *char_dest_char;
    proc_name = "cint_field_external_fg_add";

    fg_info_p->stage = bcmFieldStageExternal;
    fg_info_p->fg_type = bcmFieldGroupTypeExternalTcam;

    dest_char = &(fg_info_p->name[0]);
    char_dest_char = dest_char;

    snprintf(char_dest_char, sizeof(fg_info_p->name), "ext_FG_%s", name);
    /*
     *  Add qualifiers to the Field Group
     */
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

    rv = bcm_field_action_create(unit, 0, &action_info, &bcm_action);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    fg_info_p->action_types[0] = bcm_action;
    fg_info_p->action_with_valid_bit[0] = FALSE;

    fg_info_p->nof_actions = 1;

    rv = bcm_field_group_add(unit, 0, fg_info_p, fg_id_p);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    return rv;
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
    char *proc_name;
    int rv = BCM_E_NONE;
    uint32 qual_ndx, action_ndx;
    bcm_field_group_attach_info_t group_attach_info;
    bcm_field_context_t context_id;
    proc_name = "cint_field_external_fg_attach";

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
 */
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

    /*
     * Call the attach API
     */
    rv = bcm_field_group_context_attach(unit, 0, external_fg_id, context_id, &group_attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach fg_id %d external_app_type %d context %d\n", proc_name, rv, external_fg_id,   (external_app_type & 0x00ff), context_id);
        return rv;
    }

    return rv;
}


int
cint_field_external_entry_add(
    int unit,
    bcm_field_group_t external_fg_id,
    bcm_field_group_info_t *fg_info_p)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    uint32 qual_ndx, action_ndx;
    bcm_field_entry_info_t entry_info;

    proc_name = "cint_field_external_entry_add";

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

    /*
     * Call the attach API
     */
    rv = bcm_field_entry_add(unit, 0, external_fg_id, &entry_info, &entry_id);
    if (rv != BCM_E_NONE)
    {
        void *dest_char;
        char *fg_name;
        dest_char = &(fg_info_p->name[0]);
        fg_name = dest_char;

        printf("%s Error (%d), in bcm_field_entry_add fg_id %d fg_name %s \n", proc_name, rv, external_fg_id, fg_name);
        return rv;
    }

    return rv;
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
    char *proc_name;
    int rv = BCM_E_NONE;
    bcm_field_group_t external_fg_id = cint_field_external_fg_id;

    proc_name = "cint_field_external_main";

    bcm_field_group_info_t_init( &fg_info);

    rv = cint_field_external_fg_add(unit, &fg_info, &external_fg_id, "main");
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_fg_add fg_id %d\n", proc_name, rv, external_fg_id);
        return rv;
    }

    rv = cint_field_external_fg_attach(unit, external_fg_id, &fg_info, external_app_type, payload_id, external_fg_payload_offset);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_fg_attach fg_id %d external_app_type %d\n", proc_name, rv, external_fg_id,  (external_app_type & 0x00ff));
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchExternalTcamSync, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), bcm_switch_control_set - ExternalTcamSync failed fg_id %d external_app_type %d\n", proc_name, rv, external_fg_id,  (external_app_type & 0x00ff));
        return rv;
    }

    rv = cint_field_external_entry_add(unit, external_fg_id, &fg_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_entry_add fg_id %d external_app_type %d\n", proc_name, rv, external_fg_id,  (external_app_type & 0x00ff));
        return rv;
    }

    printf("Config external FG on IFWD2, fg_id=%d, external_app_type %d\n", external_fg_id,  (external_app_type & 0x00ff));

    return rv;
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
    char *proc_name;
    int rv = BCM_E_NONE;
    bcm_field_group_t external_fg_id = cint_field_external_fg_id;
    bcm_field_group_t external_fg_2_id = cint_field_external_fg_2_id;

    proc_name = "cint_field_external_shared_acl_main";

    bcm_field_group_info_t_init( &fg_info);
    bcm_field_group_info_t_init( &fg_info_2);

    rv = cint_field_external_fg_add(unit, &fg_info, &external_fg_id, "1");
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_fg_add fg_id %d\n", proc_name, rv, external_fg_id);
        return rv;
    }

    rv = cint_field_external_fg_attach(unit, external_fg_id, &fg_info, external_app_type, payload_id, external_fg_payload_offset);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_fg_attach fg_id %d AppType %d\n", proc_name, rv, external_fg_id,  (external_app_type & 0x00ff));
        return rv;
    }

    rv = cint_field_external_fg_add(unit, &fg_info_2, &external_fg_2_id, "2");
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_fg_add fg_2_id %d\n", proc_name, rv, external_fg_2_id);
        return rv;
    }

    rv = cint_field_external_fg_attach(unit, external_fg_2_id, &fg_info_2, external_app_type, payload_id_2, external_fg_payload_offset_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_fg_attach fg_2_id %d AppType %d\n", proc_name, rv, external_fg_2_id,  (external_app_type & 0x00ff));
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchExternalTcamSync, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_switch_control_set - ExternalTcamSync failed \n", proc_name, rv);
        return rv;
    }

    rv = cint_field_external_entry_add(unit, external_fg_id, &fg_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_entry_add fg_id %d \n", proc_name, rv, external_fg_id);
        return rv;
    }

    rv = cint_field_external_entry_add(unit, external_fg_2_id, &fg_info_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_entry_add fg_id %d \n", proc_name, rv, cint_field_external_fg_2_id);
        return rv;
    }

    printf("Config external FG on IFWD2, fg_id=%d, AppType %d\n", external_fg_id,  (external_app_type & 0x00ff));
    printf("Config external FG on IFWD2, fg_id_2=%d, AppType %d\n", external_fg_2_id,  (external_app_type & 0x00ff));

    return rv;
}
int
cint_field_external_destroy(int unit)
{
    bcm_field_group_info_t fg_info;
    char *proc_name;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_external_destroy";
    /*
     * Call the entry delete API
     */
    rv = bcm_field_entry_delete(unit, external_fg_id, NULL, entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_delete 1 fg_id %d \n", proc_name, rv, external_fg_id);
        return rv;
    }
    /*
     * Call the entry delete API
     */
    rv = bcm_field_entry_delete(unit, external_fg_id, NULL, traffic_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_delete 2 fg_id %d \n", proc_name, rv, external_fg_id);
        return rv;
    }

    return rv;
}
