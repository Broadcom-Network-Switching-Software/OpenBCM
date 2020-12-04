/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * This cint includes an example of using the bcmFieldQualifyIPv4FragmentedNonFirst Qualifier
 *
 * Configuration example start:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../src/examples/dnx/field/cint_field_ipv4_frag_non_first_qual.c
 * cint;
 * int unit = 0;
 * bcm_field_context_t context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;
 * field_ipv4_frag_non_first_qual_main(unit, context_id);
 *
 * Configuration example end
 */

bcm_field_group_t cint_field_ipv4_frag_non_first_qual_fg_id;
bcm_field_entry_t cint_field_ipv4_frag_non_first_qual_entry_id;

int field_ipv4_frag_non_first_qual_main(int unit, bcm_field_context_t context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_group_attach_info_t attach_info;
    void *dest_char;
    int rv = BCM_E_NONE;

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageEgress;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyIPv4FragmentedNonFirst;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionPrioIntNew;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "IPv4_FNF_FG", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_ipv4_frag_non_first_qual_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    /** Attach the FG to default context. */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerRecordsAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_ipv4_frag_non_first_qual_fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    bcm_field_entry_info_t_init(&ent_info);

    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = 0x1;
    ent_info.entry_qual[0].mask[0] = 0x1;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = 0x2;

    rv = bcm_field_entry_add(unit, 0, cint_field_ipv4_frag_non_first_qual_fg_id, &ent_info, &cint_field_ipv4_frag_non_first_qual_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    return rv;
}

int field_ipv4_frag_non_first_qual_destroy(int unit, bcm_field_context_t context_id)
{
    int rv;

    rv = bcm_field_group_context_detach(unit, cint_field_ipv4_frag_non_first_qual_fg_id, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach\n", rv);
        return rv;
    }

    rv = bcm_field_entry_delete(unit, cint_field_ipv4_frag_non_first_qual_fg_id, NULL, cint_field_ipv4_frag_non_first_qual_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete\n", rv);
        return rv;
    }

    rv = bcm_field_group_delete(unit, cint_field_ipv4_frag_non_first_qual_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete\n", rv);
        return rv;
    }

    return rv;
}
