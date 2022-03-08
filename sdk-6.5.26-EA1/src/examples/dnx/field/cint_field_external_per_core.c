/*
 * Configuration example start:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../src/examples/dnx/field/cint_field_external_per_core.c
 * cint;
 * int unit = 0;
 * int core_id = 0;
 * cint_field_external_per_core_main(unit,core_id);
 *
 * Configuring external tcam entries for a specific core.
 *
 */


/*
 * Global variables.
 * Declared global to make easy traffic and destroy functions.
 * */

bcm_field_group_t cint_field_external_per_core_fg_id = BCM_FIELD_ID_INVALID;
uint32 cint_field_external_per_core_payload_id = 2;
bcm_field_AppType_t cint_field_external_per_core_external_app_type = bcmFieldAppTypeIp4UcastRpf;
uint32 cint_field_external_per_core_payload_offset = 120;
uint32 cint_field_external_per_core_payload_value = 0xabc;
uint32 cint_field_external_per_core_payload_value_default = 0xdef;
uint32 cint_field_external_per_core_payload_size = 32;
bcm_field_action_t cint_field_external_per_core_external_uda = bcmFieldActionCount;
bcm_field_entry_t cint_field_external_per_core_entry_id = BCM_FIELD_ID_INVALID;




int
cint_field_external_per_core_fg_add(
    int unit,
    bcm_field_group_info_t *fg_info_p,
    bcm_field_group_t * fg_id_p)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    int qual_idx = 0;
    bcm_field_action_info_t action_info;
    void *dest_char;
    char *tmp_ptr;
    uint32 flags;
    proc_name = "cint_field_external_per_core_fg_add";

    fg_info_p->stage = bcmFieldStageExternal;
    fg_info_p->fg_type = bcmFieldGroupTypeExternalTcam;

    dest_char = &(fg_info_p->name[0]);
    tmp_ptr = dest_char;

    snprintf(tmp_ptr, sizeof(fg_info_p->name), "ext_FG");
    /*
     *  Add qualifiers to the Field Group. bcmFieldQualifySrcIp isn't related to qualifying per port.
     */
    fg_info_p->qual_types[qual_idx++] = bcmFieldQualifySrcIp;
    fg_info_p->nof_quals = qual_idx;
    
    /*
     *  Create a void user defined action
     */
    bcm_field_action_info_t_init(&action_info);

    action_info.stage = bcmFieldStageExternal;
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = cint_field_external_per_core_payload_size;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "ext_uda", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_external_per_core_external_uda);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    fg_info_p->action_types[0] = cint_field_external_per_core_external_uda;
    fg_info_p->action_with_valid_bit[0] = FALSE;

    fg_info_p->nof_actions = 1;

    /*
     * Mark the field group as having the option to add different entries for different cores.
     */
    fg_info_p->external_info.flags |= BCM_FIELD_GROUP_EXTERNAL_FLAG_ENTRIES_PER_CORE;

    rv = bcm_field_group_add(unit, flags, fg_info_p, fg_id_p);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    return rv;
}

int
cint_field_external_per_core_fg_attach(
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
    proc_name = "cint_field_external_per_core_fg_attach";

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

    group_attach_info.key_info.qual_types[qual_ndx] = bcmFieldQualifySrcIp;
    group_attach_info.key_info.qual_info[qual_ndx].input_arg = 0;
    group_attach_info.key_info.qual_info[qual_ndx].input_type = bcmFieldInputTypeLayerFwd;
    group_attach_info.key_info.qual_info[qual_ndx++].offset = 0;
    
    /* Cast */
    context_id  = external_app_type + 0 ;

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
cint_field_external_per_core_entry_add(
    int unit,
    bcm_field_group_t external_fg_id,
    bcm_field_group_info_t *fg_info_p,
    bcm_core_t core_id)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    bcm_field_entry_info_t entry_info;

    proc_name = "cint_field_external_per_core_entry_add";

    bcm_field_entry_info_t_init(&entry_info);

    /* We check that the source address is within 1.1.1.0/24. */
    entry_info.entry_qual[0].type = bcmFieldQualifySrcIp;
    entry_info.entry_qual[0].value[0] = 0x01010100;
    entry_info.entry_qual[0].mask[0] = 0xFFFFFF00;
    entry_info.nof_entry_quals = fg_info_p->nof_quals;
    
    entry_info.entry_action[0].type = fg_info_p->action_types[0];
    entry_info.entry_action[0].value[0] = cint_field_external_per_core_payload_value;

    entry_info.nof_entry_actions = fg_info_p->nof_actions;
    
    entry_info.priority = 10;

    /*
     * Set the core ID.
     */
    entry_info.core = core_id;

    rv = bcm_field_entry_add(unit, 0, external_fg_id, &entry_info, &cint_field_external_per_core_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_add fg_id %d fg_name %s \n", proc_name, rv);
        return rv;
    }    

    return rv;
}

/**
 * \brief - run external main function
 *
 * \param [in] unit - The unit number in system
 * \param [in] core_id - The core to create an entry for.
 *
 *  Usage: run in cint shell "cint_field_external_per_core_main(0)"
 */
int
cint_field_external_per_core_main(
    int unit,
    bcm_core_t core_id)
{
    bcm_field_group_info_t fg_info;
    char *proc_name;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_external_per_core_main";

    bcm_field_group_info_t_init( &fg_info);

    rv = cint_field_external_per_core_fg_add(unit, &fg_info, &cint_field_external_per_core_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_per_core_fg_add fg_id %d\n", proc_name, rv, cint_field_external_per_core_fg_id);
        return rv;
    }

    rv = cint_field_external_per_core_fg_attach(unit, cint_field_external_per_core_fg_id, &fg_info, cint_field_external_per_core_external_app_type, cint_field_external_per_core_payload_id, cint_field_external_per_core_payload_offset);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_per_core_fg_attach fg_id %d external_app_type %d\n", proc_name, rv, cint_field_external_per_core_fg_id,  (cint_field_external_per_core_external_app_type & 0x00ff));
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchExternalTcamSync, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), bcm_switch_control_set - ExternalTcamSync failed fg_id %d external_app_type %d\n", proc_name, rv, cint_field_external_per_core_fg_id,  (cint_field_external_per_core_external_app_type & 0x00ff));
        return rv;
    }

    rv = cint_field_external_per_core_entry_add(unit, cint_field_external_per_core_fg_id, &fg_info, core_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_per_core_entry_add fg_id %d external_app_type %d\n", proc_name, rv, cint_field_external_per_core_fg_id,  (cint_field_external_per_core_external_app_type & 0x00ff));
        return rv;
    }

    printf("Config external FG on IFWD2, fg_id=%d, external_app_type %d\n", cint_field_external_per_core_fg_id,  (cint_field_external_per_core_external_app_type & 0x00ff));

    return rv;
}

int
cint_field_external_per_core_destroy(int unit)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    char *proc_name;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_external_per_core_destroy";

    /*
     * At this point external field group cannot be deleted, so we only delete the entries.
     */
    rv = bcm_field_entry_delete(unit, cint_field_external_per_core_fg_id, NULL, cint_field_external_per_core_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_delete fg_id %d \n", proc_name, rv, cint_field_external_per_core_fg_id);
        return rv;
    }

    return rv;
}
