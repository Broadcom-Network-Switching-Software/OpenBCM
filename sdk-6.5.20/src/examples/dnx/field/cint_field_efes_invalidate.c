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
 * cint ../../../src/examples/dnx/field/cint_field_efes_invalidate.c
 * cint;
 * int unit = 0;
 * cint_field_efes_invalidate_main(unit);
 *
 * Configuration example end
 *
 * This CINT creates a field group with two actions and activates only one of them when attaching to a context.
 */
 /**
  * Global variables, used to save information for destroy function.
  */
/**
 * FG ID
 */
int Cint_field_efes_invalidate_fg_id = BCM_FIELD_INVALID;
/**
 * TCAM entry.
 */
bcm_field_entry_t Cint_field_efes_invalidate_entry_id = 0;
/**
 * The contexts, one for IPv4 TCP using both action, and another for IPv4_OTHER using only one action.
 */
bcm_field_context_t Cint_field_efes_invalidate_context_id_ipv4_tcp = BCM_FIELD_INVALID;
bcm_field_context_t Cint_field_efes_invalidate_context_id_ipv4_other = BCM_FIELD_INVALID;
/**
 * The presel IDs for the two contexts. since ipv4_tcp is the more specific context it appears with a lower number.
 */
int Cint_field_efes_invalidate_presel_id_ipv4_tcp = 51;
int Cint_field_efes_invalidate_presel_id_ipv4_other = 52;






/**
 * This function configures a field group with two actions and then activates and EFES for only one of them when 
 * attaching to a contexts.
 * For a TCP over ipv4 context, we perform two actions, for  other ipv4 packets we perform only one action.
 *
 * \param [in] unit - The unit number.
 */
int
cint_field_efes_invalidate_main(int unit)
{
    char *proc_name;
    bcm_field_context_info_t context_info;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t fg_attach_info;
    bcm_field_entry_info_t entry_info;
    bcm_field_presel_entry_id_t entry_id;
    bcm_field_presel_entry_data_t entry_data;
    void * dest_char;
    int rv = BCM_E_NONE;
    proc_name = "cint_field_efes_invalidate_main";

    /* 
     * Create the contexts.
     */
    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "IPv4_TCP", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &Cint_field_efes_invalidate_context_id_ipv4_tcp);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_create\n", proc_name, rv);
        return rv;
    }
    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "IPv4_OTHER", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &Cint_field_efes_invalidate_context_id_ipv4_other);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_create\n", proc_name, rv);
        return rv;
    }

    /* 
     * Create the field group.
     */
    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&fg_attach_info);
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifySrcIp;
    fg_info.nof_actions = 2;
    fg_info.action_types[0] = bcmFieldActionTtlSet;
    /* The second action is only for the TCP context.*/
    fg_info.action_types[1] = bcmFieldActionUDHData0;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "EFES_invalidate_fg", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &Cint_field_efes_invalidate_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_group_add\n", proc_name, rv);
        return rv;
    }

    /* 
     * Attach to the IPv4_TCP context. 
     */
    bcm_field_group_attach_info_t_init(&fg_attach_info);
    fg_attach_info.key_info.nof_quals = fg_info.nof_quals;
    fg_attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    fg_attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    fg_attach_info.key_info.qual_info[0].input_arg = 0;
    fg_attach_info.key_info.qual_info[0].offset = 0;
    fg_attach_info.payload_info.nof_actions = fg_info.nof_actions;
    fg_attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    fg_attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    rv = bcm_field_group_context_attach(unit, 0, Cint_field_efes_invalidate_fg_id, Cint_field_efes_invalidate_context_id_ipv4_tcp, &fg_attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_group_context_attach to IPv4\n", proc_name, rv);
        return rv;
    }

    /* 
     * Attach to the IPv4_OTHER context.
     * Inavlidate the second action. Note we still need to have the action in fg_attach_info.payload_info.action_types.
     */
    fg_attach_info.payload_info.action_info[1].priority = BCM_FIELD_ACTION_INVALIDATE;
    rv = bcm_field_group_context_attach(unit, 0, Cint_field_efes_invalidate_fg_id, Cint_field_efes_invalidate_context_id_ipv4_other, &fg_attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_group_context_attach to IPv6\n", proc_name, rv);
        return rv;
    }

    /*
     * Add an entry.
     */
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = 1;
    entry_info.entry_qual[0].mask[0] = 0xFFFFFFFF;
    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = 12;
    entry_info.entry_action[1].type = fg_info.action_types[1];
    entry_info.entry_action[1].value[0] = 66;
    entry_info.priority = 20;
    rv = bcm_field_entry_add(unit, 0, Cint_field_efes_invalidate_fg_id, &entry_info, &Cint_field_efes_invalidate_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_add \n", proc_name, rv);
        return rv;
    }

    /*
     * Add the preselectors.
     */
    bcm_field_presel_entry_id_info_init(&entry_id);
    bcm_field_presel_entry_data_info_init(&entry_data);
    entry_id.presel_id = Cint_field_efes_invalidate_presel_id_ipv4_tcp;
    entry_id.stage = bcmFieldStageIngressPMF1;
    entry_data.entry_valid = TRUE;
    entry_data.context_id = Cint_field_efes_invalidate_context_id_ipv4_tcp;
    entry_data.nof_qualifiers = 2;
    entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    entry_data.qual_data[0].qual_arg = 0;
    entry_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    entry_data.qual_data[0].qual_mask = 0x1F;
    entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
    entry_data.qual_data[1].qual_arg = 1;
    entry_data.qual_data[1].qual_value = bcmFieldLayerTypeTcp;
    entry_data.qual_data[1].qual_mask = 0x1F;
    rv = bcm_field_presel_set(unit, 0, &entry_id, &entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in dnx_field_presel_set\n", proc_name, rv);
        return rv;
    }
    bcm_field_presel_entry_id_info_init(&entry_id);
    bcm_field_presel_entry_data_info_init(&entry_data);
    entry_id.presel_id = Cint_field_efes_invalidate_presel_id_ipv4_other;
    entry_id.stage = bcmFieldStageIngressPMF1;
    entry_data.entry_valid = TRUE;
    entry_data.context_id = Cint_field_efes_invalidate_context_id_ipv4_other;
    entry_data.nof_qualifiers = 1;
    entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    entry_data.qual_data[0].qual_arg = 0;
    entry_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    entry_data.qual_data[0].qual_mask = 0x1F;
    rv = bcm_field_presel_set(unit, 0, &entry_id, &entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in dnx_field_presel_set\n", proc_name, rv);
        return rv;
    }

    return rv;
}

/**
 * This function deletes what was configured in cint_field_efes_invalidate_main
 *
 * \param [in] unit - The unit number.
 */
int
cint_field_efes_invalidate_destroy(int unit)
{
    bcm_field_presel_entry_id_t entry_id;
    bcm_field_presel_entry_data_t entry_data;
    char *proc_name;
    int rv = BCM_E_NONE;
    proc_name = "cint_field_efes_invalidate_destroy";

    bcm_field_presel_entry_id_info_init(&entry_id);
    bcm_field_presel_entry_data_info_init(&entry_data);
    entry_id.stage = bcmFieldStageIngressPMF1;
    entry_id.presel_id = Cint_field_efes_invalidate_presel_id_ipv4_tcp;
    entry_data.entry_valid = FALSE;
    rv = bcm_field_presel_set(unit, 0, &entry_id, &entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in dnx_field_presel_set\n", proc_name, rv);
        return rv;
    }
    bcm_field_presel_entry_id_info_init(&entry_id);
    bcm_field_presel_entry_data_info_init(&entry_data);
    entry_id.stage = bcmFieldStageIngressPMF1;
    entry_id.presel_id = Cint_field_efes_invalidate_presel_id_ipv4_other;
    entry_data.entry_valid = FALSE;
    rv = bcm_field_presel_set(unit, 0, &entry_id, &entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in dnx_field_presel_set\n", proc_name, rv);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, Cint_field_efes_invalidate_fg_id, Cint_field_efes_invalidate_context_id_ipv4_tcp);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_group_context_detach\n", proc_name, rv);
        return rv;
    }
    rv = bcm_field_group_context_detach(unit, Cint_field_efes_invalidate_fg_id, Cint_field_efes_invalidate_context_id_ipv4_other);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_group_context_detach\n", proc_name, rv);
        return rv;
    }

    rv = bcm_field_entry_delete(unit, Cint_field_efes_invalidate_fg_id, NULL, Cint_field_efes_invalidate_entry_id);
    if (rv != BCM_E_NONE)
    {
         printf("Error (%d), in bcm_field_entry_delete\n", rv);
         return rv;
    }

    rv = bcm_field_group_delete(unit, Cint_field_efes_invalidate_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_group_delete\n", proc_name, rv);
        return rv;
    }

    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, Cint_field_efes_invalidate_context_id_ipv4_tcp);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_destroy\n", proc_name, rv);
        return rv;
    }
    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, Cint_field_efes_invalidate_context_id_ipv4_other);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_destroy\n", proc_name, rv);
        return rv;
    }
    
    return rv;
}
