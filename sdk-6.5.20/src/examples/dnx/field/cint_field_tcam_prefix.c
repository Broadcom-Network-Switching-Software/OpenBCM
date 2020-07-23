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
 * cint ../../../src/examples/dnx/field/cint_field_tcam_prefix.c
 * cint;
 * int unit = 0;
 * int src_mac_val = 10;
 * int tc_val = 1;
 * int bad_tc_val = 0;
 * field_tcam_prefix_main(unit, src_mac_val, tc_val, bad_tc_val);
 *
 * Configuration example end
 *
 * Sets-up two TCAM FGs that request to reside on the same TCAM bank, enforcing that
 * different prefix be assigned to each. Then both FG add the same exact entry
 * with different payload.
 * First FG gets attached to a newly created context, while the second FG gets attached to the default context.
 * The FGs both qualify on the given src_mac_val then the first FG (which we're not supposed to hit) has bad_tc_val
 * as payload, while the second FG (the one we're supposed to hit) has tc_val payload.
 * Few remarks on the cint:
 * The FG we want to check the test against gets added second, to ensure prefix != 0
 * The entry we want to check against also is added second, to ensure TCAM search passes on bad entry (same val,
 * diff prefix).
 */

bcm_field_group_t fg1_id = 0;
bcm_field_group_t fg2_id = 0;
bcm_field_entry_t ent1_id;
bcm_field_entry_t ent2_id;
bcm_field_context_t context_id;

int field_tcam_prefix_main(int unit, int src_mac_val, int tc_val, int bad_tc_val)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_entry_info_t ent_info;
    int rv = BCM_E_NONE;
    int bank_id = 5;

    /*
     * Create a new context for first group, since two groups on same context can't share banks
     */
    printf("Creating new context for the first group\n");
    bcm_field_context_info_t_init(&context_info);
    context_id = 10;
    rv = bcm_field_context_create(unit, 1, bcmFieldStageIngressPMF1, &context_info, &context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    printf("Context created!\n");
    /*
     * Create and attach first group
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Set quals */
    fg_info.nof_quals = 3;
    fg_info.qual_types[0] = bcmFieldQualifySrcIp;
    fg_info.qual_types[1] = bcmFieldQualifyDstIp;
    fg_info.qual_types[2] = bcmFieldQualifySrcMac;
    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionPrioIntNew;

    /* Request bank */
	fg_info.tcam_info.bank_allocation_mode = bcmFieldTcamBankAllocationModeSelect;
    fg_info.tcam_info.nof_tcam_banks = 1;
    fg_info.tcam_info.tcam_bank_ids[0] = bank_id;

    printf("Creating first group\n");
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg1_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0]; 
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1]; 
    attach_info.key_info.qual_types[2] = fg_info.qual_types[2]; 
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[1].input_arg = 1;
    attach_info.key_info.qual_info[1].offset = 0;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[2].input_arg = 0;
    attach_info.key_info.qual_info[2].offset = 0;

    printf("Attaching first group\n");
    rv = bcm_field_group_context_attach(unit, 0, fg1_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    /*
     * Create and attach second group
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Set quals */
    fg_info.nof_quals = 3;
    fg_info.qual_types[0] = bcmFieldQualifySrcIp;
    fg_info.qual_types[1] = bcmFieldQualifyDstIp;
    fg_info.qual_types[2] = bcmFieldQualifySrcMac;
    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionPrioIntNew;

    /* Request bank 5 */
	fg_info.tcam_info.bank_allocation_mode = bcmFieldTcamBankAllocationModeSelect;
    fg_info.tcam_info.nof_tcam_banks = 1;
    fg_info.tcam_info.tcam_bank_ids[0] = bank_id;

    printf("Creating second group\n");
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg2_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0]; 
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1]; 
    attach_info.key_info.qual_types[2] = fg_info.qual_types[2]; 
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[1].input_arg = 1;
    attach_info.key_info.qual_info[1].offset = 0;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[2].input_arg = 0;
    attach_info.key_info.qual_info[2].offset = 0;

    printf("Attaching second group to default context\n");
    rv = bcm_field_group_context_attach(unit, 0, fg2_id, 0, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    /*
     * Add entries
     */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = fg_info.qual_types[2];
    ent_info.entry_qual[0].value[0] = src_mac_val;
    ent_info.entry_qual[0].mask[0] = 0xffffffff;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = bad_tc_val;

    printf("Adding first entry\n");
    rv = bcm_field_entry_add(unit, 0, fg1_id, &ent_info, &ent1_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = fg_info.qual_types[2];
    ent_info.entry_qual[0].value[0] = src_mac_val;
    ent_info.entry_qual[0].mask[0] = 0xffffffff;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = tc_val;

    printf("Adding second entry\n");
    rv = bcm_field_entry_add(unit, 0, fg2_id, &ent_info, &ent2_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    return rv;
}

int field_tcam_prefix_destroy(int unit)
{
    int rv = BCM_E_NONE;

    rv = bcm_field_entry_delete(unit, fg1_id, NULL, ent1_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete  fg %d \n", rv, fg1_id);
        return rv;
    }
    rv = bcm_field_entry_delete(unit, fg2_id, NULL, ent2_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete  fg %d \n", rv, fg2_id);
        return rv;
    }
    /* Detach the IPMF2 FG from its context */
    
    rv = bcm_field_group_context_detach(unit, fg1_id, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, fg1_id, context_id);
        return rv;
    }
    /* Detach the IPMF2 FG from its context */
    rv = bcm_field_group_context_detach(unit, fg2_id, 0 /*Default context */);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d stage %s\n", rv, fg2_id, 0);
        return rv;
    }
    rv = bcm_field_group_delete(unit, fg1_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, fg1_id);
        return rv;
    }
    rv = bcm_field_group_delete(unit, fg2_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, fg2_id);
        return rv;
    }
    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy context_id %d \n", rv, context_id);
        return rv;
    }
    return rv;
}
