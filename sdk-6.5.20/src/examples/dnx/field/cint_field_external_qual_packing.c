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
 * cint ../../../src/examples/dnx/field/cint_field_external_qual_packing.c
 * cint;
 * int unit = 0;
 * cint_field_external_qual_packing_main(unit);
 *
 * Example of using the flag BCM_FIELD_FLAG_QUAL_PACKING, to make consecutive qualifiers whose size is not an 
 * integer of bytes,combining them into a signel segment to save space on both DB key and master key.
 */


/*
 * Global variables.
 * Declared global to make easy traffic and destroy functions.
 */

bcm_field_group_t cint_field_external_qual_packing_fg_id = BCM_FIELD_ID_INVALID;
uint32 cint_field_external_qual_packing_payload_id = 2;
bcm_field_AppType_t cint_field_external_qual_packing_external_app_type = bcmFieldAppTypeIp4UcastRpf;
uint32 cint_field_external_qual_packing_payload_offset = 120;
uint32 cint_field_external_qual_packing_payload_value = 0xabc;
uint32 cint_field_external_qual_packing_payload_value_default = 0xdef;
uint32 cint_field_external_qual_packing_payload_size = 32;
bcm_field_qualify_t cint_field_external_qual_packing_external_udq_tcp_flags_3lsb = bcmFieldQualifyCount;
bcm_field_action_t cint_field_external_qual_packing_external_uda = bcmFieldActionCount;
bcm_field_entry_t cint_field_external_qual_packing_entry_id = BCM_FIELD_ID_INVALID;




int
cint_field_external_qual_packing_fg_add(
    int unit,
    bcm_field_group_info_t *fg_info_p,
    bcm_field_group_t * fg_id_p)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    int qual_idx = 0;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    void *dest_char;
    char *tmp_ptr;
    uint32 flags;
    proc_name = "cint_field_external_qual_packing_fg_add";

    /*
     *  Create void user defined action for the 3LSB of TCP flags (FIN,SYN and RST).
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 3;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "ext_UDQ_TCP_FLAGS_3LSB", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_external_qual_packing_external_udq_tcp_flags_3lsb);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_qualifier_create\n", proc_name, rv);
        return rv;
    }

    fg_info_p->stage = bcmFieldStageExternal;
    fg_info_p->fg_type = bcmFieldGroupTypeExternalTcam;

    dest_char = &(fg_info_p->name[0]);
    tmp_ptr = dest_char;

    snprintf(tmp_ptr, sizeof(fg_info_p->name), "ext_FG");
    /*
     *  Add qualifiers to the Field Group
     */
    fg_info_p->qual_types[qual_idx++] = bcmFieldQualifySrcIp;
    fg_info_p->qual_types[qual_idx++] = bcmFieldQualifyIpFlags;
    fg_info_p->qual_types[qual_idx++] = cint_field_external_qual_packing_external_udq_tcp_flags_3lsb;
    fg_info_p->qual_types[qual_idx++] = bcmFieldQualifyIp4Protocol;
    fg_info_p->nof_quals = qual_idx;
    
    /*
     *  Create a void user defined action
     */
    bcm_field_action_info_t_init(&action_info);

    action_info.stage = bcmFieldStageExternal;
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = cint_field_external_qual_packing_payload_size;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "ext_uda", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_external_qual_packing_external_uda);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    fg_info_p->action_types[0] = cint_field_external_qual_packing_external_uda;
    fg_info_p->action_with_valid_bit[0] = FALSE;

    fg_info_p->nof_actions = 1;

    /*
     * Use the flag BCM_FIELD_FLAG_QUAL_PACKING to pack consecutive (with a non byte alignedsize) qualifiers.
     */
    flags = BCM_FIELD_FLAG_QUAL_PACKING;

    rv = bcm_field_group_add(unit, flags, fg_info_p, fg_id_p);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    return rv;
}

int
cint_field_external_qual_packing_fg_attach(
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
    bcm_field_qualifier_info_get_t qual_info_get;
    bcm_field_group_attach_info_t group_attach_info;
    bcm_field_context_t context_id;
    int tcp_flags_3lsb_offset;
    proc_name = "cint_field_external_qual_packing_fg_attach";

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

    /* Get the offset of the 3LSB of the TCP flags. */
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyTcpControl, bcmFieldStageExternal, &qual_info_get);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_info_get\n", rv);
        return rv;
    }
    tcp_flags_3lsb_offset = qual_info_get.offset + qual_info_get.size - 3;
    
    /*
     * Build the qualifier attach information
     */
    qual_ndx = 0;

    group_attach_info.key_info.qual_types[qual_ndx] = bcmFieldQualifySrcIp;
    group_attach_info.key_info.qual_info[qual_ndx].input_arg = 0;
    group_attach_info.key_info.qual_info[qual_ndx].input_type = bcmFieldInputTypeLayerFwd;
    group_attach_info.key_info.qual_info[qual_ndx++].offset = 0;

    group_attach_info.key_info.qual_types[qual_ndx] = bcmFieldQualifyIpFlags;
    group_attach_info.key_info.qual_info[qual_ndx].input_arg = 0;
    group_attach_info.key_info.qual_info[qual_ndx].input_type = bcmFieldInputTypeLayerFwd;
    group_attach_info.key_info.qual_info[qual_ndx++].offset = 0;

    group_attach_info.key_info.qual_types[qual_ndx] = cint_field_external_qual_packing_external_udq_tcp_flags_3lsb;
    group_attach_info.key_info.qual_info[qual_ndx].input_arg = 1;
    group_attach_info.key_info.qual_info[qual_ndx].input_type = bcmFieldInputTypeLayerFwd;
    group_attach_info.key_info.qual_info[qual_ndx++].offset = tcp_flags_3lsb_offset;

    group_attach_info.key_info.qual_types[qual_ndx] = bcmFieldQualifyIp4Protocol;
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
cint_field_external_qual_packing_entry_add(
    int unit,
    bcm_field_group_t external_fg_id,
    bcm_field_group_info_t *fg_info_p)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    bcm_field_entry_info_t entry_info;

    proc_name = "cint_field_external_qual_packing_entry_add";

    bcm_field_entry_info_t_init(&entry_info);

    /* We check that the source address is within 1.1.1.0/24. */
    entry_info.entry_qual[0].type = bcmFieldQualifySrcIp;
    entry_info.entry_qual[0].value[0] = 0x01010100;
    entry_info.entry_qual[0].mask[0] = 0xFFFFFF00;
    /* We check that the "don't fragment" flag is set, but not the other two. */
    entry_info.entry_qual[1].type = bcmFieldQualifyIpFlags;
    entry_info.entry_qual[1].value[0] = 0x2;
    entry_info.entry_qual[1].mask[0] = 0x7;
    /* We check that the RST flag is 1, and the SYN and FIN flags are zero. */
    entry_info.entry_qual[2].type = cint_field_external_qual_packing_external_udq_tcp_flags_3lsb;
    entry_info.entry_qual[2].value[0] = 0x4;
    entry_info.entry_qual[2].mask[0] = 0x7;
    /* We verify that the IP protocol is indeed TCP. */
    entry_info.entry_qual[3].type = bcmFieldQualifyIp4Protocol;
    entry_info.entry_qual[3].value[0] = 0x6;
    entry_info.entry_qual[3].mask[0] = 0xFF;

    entry_info.nof_entry_quals = fg_info_p->nof_quals;
    
    entry_info.entry_action[0].type = fg_info_p->action_types[0];
    entry_info.entry_action[0].value[0] = cint_field_external_qual_packing_payload_value;

    entry_info.nof_entry_actions = fg_info_p->nof_actions;
    
    entry_info.priority = 10;

    rv = bcm_field_entry_add(unit, 0, external_fg_id, &entry_info, &cint_field_external_qual_packing_entry_id);
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
 *  Usage: run in cint shell "cint_field_external_qual_packing_main(0)"
 */
int
cint_field_external_qual_packing_main(int unit)
{
    bcm_field_group_info_t fg_info;
    char *proc_name;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_external_qual_packing_main";

    bcm_field_group_info_t_init( &fg_info);

    rv = cint_field_external_qual_packing_fg_add(unit, &fg_info, &cint_field_external_qual_packing_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_qual_packing_fg_add fg_id %d\n", proc_name, rv, cint_field_external_qual_packing_fg_id);
        return rv;
    }

    rv = cint_field_external_qual_packing_fg_attach(unit, cint_field_external_qual_packing_fg_id, &fg_info, cint_field_external_qual_packing_external_app_type, cint_field_external_qual_packing_payload_id, cint_field_external_qual_packing_payload_offset);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_qual_packing_fg_attach fg_id %d external_app_type %d\n", proc_name, rv, cint_field_external_qual_packing_fg_id,  (cint_field_external_qual_packing_external_app_type & 0x00ff));
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchExternalTcamSync, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), bcm_switch_control_set - ExternalTcamSync failed fg_id %d external_app_type %d\n", proc_name, rv, cint_field_external_qual_packing_fg_id,  (cint_field_external_qual_packing_external_app_type & 0x00ff));
        return rv;
    }

    rv = cint_field_external_qual_packing_entry_add(unit, cint_field_external_qual_packing_fg_id, &fg_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_qual_packing_entry_add fg_id %d external_app_type %d\n", proc_name, rv, cint_field_external_qual_packing_fg_id,  (cint_field_external_qual_packing_external_app_type & 0x00ff));
        return rv;
    }

    printf("Config external FG on IFWD2, fg_id=%d, external_app_type %d\n", cint_field_external_qual_packing_fg_id,  (cint_field_external_qual_packing_external_app_type & 0x00ff));

    return rv;
}

int
cint_field_external_qual_packing_destroy(int unit)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    char *proc_name;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_external_qual_packing_destroy";

    /*
     * At this point external field group cannot be deleted, so we only delete the entries.
     */
    rv = bcm_field_entry_delete(unit, cint_field_external_qual_packing_fg_id, NULL, cint_field_external_qual_packing_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_delete fg_id %d \n", proc_name, rv, cint_field_external_qual_packing_fg_id);
        return rv;
    }

    return rv;
}
