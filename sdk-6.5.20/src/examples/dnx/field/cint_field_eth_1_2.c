/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * This cint shows how to qualify on Ethernet 1/2 type on ETH LR Qualifier
 *
 * Configuration example start:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../src/examples/dnx/field/cint_field_presel_fwd_layer.c
 * cint ../../../src/examples/dnx/field/cint_field_eth_1_2.c
 * cint;
 * int unit = 0;
 * bcm_field_stage_t field_stage = bcmFieldStageIngressPMF1;
 * uint32 is_eth1 = 1;
 * uint32 dp_val = 1;
 * field_eth_1_2(unit, field_stage, is_eth1, dp_val);
 *
 * Configuration example end
 */

bcm_field_group_t fg_eth_1_2_id = 0;
bcm_field_entry_t ent_eth_1_2_id;
bcm_field_context_t context_id;
bcm_field_qualify_t cint_field_tcam_eth_1_2_qual_id;

int field_eth_1_2_main(
    int unit,
    bcm_field_stage_t field_stage,
    uint32 is_eth1,
    uint32 dp_val)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_eth_1_2_info;
    int rv = BCM_E_NONE;

    printf("Creating TCAM eth_1_2 FG...\r\n");
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = field_stage;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyL2Format;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionDropPrecedence;

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_eth_1_2_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    printf("Attaching created eth_1_2 FG to default context...\r\n");
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerRecordsAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, fg_eth_1_2_id, BCM_FIELD_CONTEXT_ID_DEFAULT, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    printf("Adding entry to the created eth_1_2 FG...\r\n");
    bcm_field_entry_info_t_init(&ent_eth_1_2_info);
    ent_eth_1_2_info.nof_entry_quals = 1;
    ent_eth_1_2_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_eth_1_2_info.entry_qual[0].value[0] = is_eth1;
    ent_eth_1_2_info.entry_qual[0].mask[0] = 0x1;
    ent_eth_1_2_info.nof_entry_actions = 1;
    ent_eth_1_2_info.entry_action[0].type = fg_info.action_types[0];
    ent_eth_1_2_info.entry_action[0].value[0] = dp_val;

    rv = bcm_field_entry_add(unit, 0, fg_eth_1_2_id, &ent_eth_1_2_info, &ent_eth_1_2_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    return rv;
}

int field_eth_1_2_destroy(
    int unit)
{
    int rv = BCM_E_NONE;

    printf("Deleting eth_1_2 entry...\r\n");
    rv = bcm_field_entry_delete(unit, fg_eth_1_2_id, NULL, ent_eth_1_2_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete  fg %d \n", rv, fg_eth_1_2_id);
        return rv;
    }


    printf("Detaching eth_1_2 FG from default context...\r\n");
    rv = bcm_field_group_context_detach(unit, fg_eth_1_2_id, BCM_FIELD_CONTEXT_ID_DEFAULT);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach FG %d from default context\n", rv, fg_eth_1_2_id);
        return rv;
    }

    printf("Deleting eth_1_2 FG...\r\n");
    rv = bcm_field_group_delete(unit, fg_eth_1_2_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  FG %d \n", rv, fg_eth_1_2_id);
        return rv;
    }

    return rv;
}
