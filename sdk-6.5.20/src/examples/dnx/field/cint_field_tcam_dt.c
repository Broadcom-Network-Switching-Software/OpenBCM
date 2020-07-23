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
 * cint ../../../src/examples/dnx/field/cint_field_presel_fwd_layer.c
 * cint ../../../src/examples/dnx/field/cint_field_tcam_dt.c
 * cint;
 * int unit = 0;
 * bcm_field_stage_t field_stage = bcmFieldStageIngressPMF1;
 * uint32 ttl = 10;
 * uint32 dp_val = 1;
 * int bank_mode = 1;
 * field_tcam_dt_main(unit, field_stage, ttl, dp_val, bank_mode);
 *
 * Configuration example end
 */

bcm_field_group_t fg_dt_id = 0;
bcm_field_group_t fg_special_id = 0;
bcm_field_entry_t ent_special_id;
bcm_field_entry_t ent_dt_id;
bcm_field_context_t context_id;
bcm_field_qualify_t cint_field_tcam_dt_qual_id;
bcm_field_presel_t cint_field_tcam_dt_ipmf3_presel_id = 50;
bcm_field_context_t cint_field_tcam_dt_ipmf3_context_id;

int field_tcam_dt_main(
    int unit,
    bcm_field_stage_t field_stage,
    uint32 ttl,
    uint32 dp_val,
    int bank_mode)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_special_info;
    bcm_field_context_info_t context_info;
    bcm_field_stage_t field_stage_internal;
    bcm_field_entry_info_t ent_dt_info;

    int bank_id = 8;
    int rv = BCM_E_NONE;

    if (field_stage == bcmFieldStageIngressPMF3)
    {
        bcm_field_qualifier_info_create_t qual_info;
        bcm_field_qualifier_info_create_t_init(&qual_info);
        void *dest_char;
        dest_char = &(qual_info.name[0]);
        sal_strncpy_s(dest_char, "tcam_dt_const", sizeof(qual_info.name));
        qual_info.name[sizeof(qual_info.name) - 1] = 0;
        qual_info.size = 8;
        rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_tcam_dt_qual_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_qualifier_create on qual_id = %d (%s) \n", rv,
                   cint_field_tcam_dt_qual_id, qual_info.name);
            return rv;
        }

        rv = field_presel_fwd_layer_main(unit, cint_field_tcam_dt_ipmf3_presel_id, field_stage,
                                         bcmFieldLayerTypeEth,
                                         &cint_field_tcam_dt_ipmf3_context_id, "udh_casc_ipmf3");
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in field_presel_fwd_layer_main \n", rv);
            return rv;
        }
    }

    printf("Creating TCAM DT FG...\r\n");
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectTcam;
    fg_info.stage = field_stage;
    fg_info.nof_quals = 1;
    if (field_stage == bcmFieldStageIngressPMF3)
    {
        fg_info.qual_types[0] = cint_field_tcam_dt_qual_id;
    }
    else
    {
        fg_info.qual_types[0] = bcmFieldQualifyIp4Ttl;
    }
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionDropPrecedence;

    /* Request specific bank */
    if (bank_mode == 1)
    {
        fg_info.tcam_info.bank_allocation_mode = bcmFieldTcamBankAllocationModeSelect;
        fg_info.tcam_info.nof_tcam_banks = 1;
        fg_info.tcam_info.tcam_bank_ids[0] = bank_id;
        printf("Creating DT FG with Select mode\n");
    }
    else
    {
        fg_info.tcam_info.bank_allocation_mode = bcmFieldTcamBankAllocationModeAuto;
        printf("Creating DT FG with Auto mode\n");
    }

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_dt_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    printf("Attaching created DT FG to default context...\r\n");
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0]; 
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    if (field_stage == bcmFieldStageIngressPMF3)
    {
        attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeConst;
        attach_info.key_info.qual_info[0].input_arg = 0;
        attach_info.key_info.qual_info[0].offset = 0;
    }
    else
    {
        attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
        attach_info.key_info.qual_info[0].input_arg = 1;
        attach_info.key_info.qual_info[0].offset = 0;
    }

    if (field_stage == bcmFieldStageIngressPMF3)
    {
         rv = bcm_field_group_context_attach(unit, 0, fg_dt_id, cint_field_tcam_dt_ipmf3_context_id, &attach_info);
    }
    else
    {
        rv = bcm_field_group_context_attach(unit, 0, fg_dt_id, BCM_FIELD_CONTEXT_ID_DEFAULT, &attach_info);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    printf("Creating TCAM Special FG...\r\n");

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = field_stage;
    if (field_stage == bcmFieldStageIngressPMF3)
    {
        fg_info.nof_quals = 4;
        fg_info.qual_types[0] = cint_field_tcam_dt_qual_id;
        fg_info.qual_types[1] = bcmFieldQualifyUDHData0;
        fg_info.qual_types[2] = bcmFieldQualifyUDHData1;
        fg_info.qual_types[3] = bcmFieldQualifyUDHData2;
    }
    else
    {
        fg_info.nof_quals = 3;
        fg_info.qual_types[0] = bcmFieldQualifyIp4Ttl;
        fg_info.qual_types[1] = bcmFieldQualifySrcMac;
        fg_info.qual_types[2] = bcmFieldQualifyDstMac;
    }
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionDropPrecedence;

    /* Request specific bank */
    fg_info.tcam_info.bank_allocation_mode = bcmFieldTcamBankAllocationModeSelect;
    fg_info.tcam_info.nof_tcam_banks = 1;
    fg_info.tcam_info.tcam_bank_ids[0] = bank_id;

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_special_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    /** In case of IPMF2 stage the context should be created in IPMF1 stage. */
    if (field_stage == bcmFieldStageIngressPMF2)
    {
        field_stage_internal = bcmFieldStageIngressPMF1;
    }
    else
    {
        field_stage_internal = field_stage;
    }

    printf("Creating new context for the special FG...\n");
    bcm_field_context_info_t_init(&context_info);
    context_id = 10;
    rv = bcm_field_context_create(unit, 1, field_stage_internal, &context_info, &context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    printf("Attaching created Special FG to new context...\r\n");
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0]; 
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1]; 
    attach_info.key_info.qual_types[2] = fg_info.qual_types[2];
    if (field_stage == bcmFieldStageIngressPMF3)
    {
        attach_info.key_info.qual_types[3] = fg_info.qual_types[3];
    }
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    if (field_stage == bcmFieldStageIngressPMF3)
    {
        attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeConst;
        attach_info.key_info.qual_info[0].input_arg = 0;
        attach_info.key_info.qual_info[0].offset = 0;
        attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
        attach_info.key_info.qual_info[1].input_arg = 0;
        attach_info.key_info.qual_info[1].offset = 0;
        attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;
        attach_info.key_info.qual_info[2].input_arg = 0;
        attach_info.key_info.qual_info[2].offset = 0;
        attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeMetaData;
        attach_info.key_info.qual_info[3].input_arg = 0;
        attach_info.key_info.qual_info[3].offset = 0;
    }
    else
    {
        attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
        attach_info.key_info.qual_info[0].input_arg = 1;
        attach_info.key_info.qual_info[0].offset = 0;
        attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
        attach_info.key_info.qual_info[1].input_arg = 0;
        attach_info.key_info.qual_info[1].offset = 0;
        attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
        attach_info.key_info.qual_info[2].input_arg = 0;
        attach_info.key_info.qual_info[2].offset = 0;
    }

    rv = bcm_field_group_context_attach(unit, 0, fg_special_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    printf("Adding entry to the created DT FG...\r\n");
    bcm_field_entry_info_t_init(&ent_dt_info);
    ent_dt_info.nof_entry_quals = 1;
    ent_dt_info.entry_qual[0].type = fg_info.qual_types[0];
    if (field_stage == bcmFieldStageIngressPMF3)
    {
        ent_dt_info.entry_qual[0].value[0] = 0;
        ent_dt_info.entry_qual[0].mask[0] = 0xff;
    }
    else
    {
        ent_dt_info.entry_qual[0].value[0] = ttl;
        ent_dt_info.entry_qual[0].mask[0] = 0xff;
    }

    ent_dt_info.nof_entry_actions = 1;
    ent_dt_info.entry_action[0].type = fg_info.action_types[0];
    ent_dt_info.entry_action[0].value[0] = dp_val;

    rv = bcm_field_entry_add(unit, 0, fg_dt_id, &ent_dt_info, &ent_dt_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    printf("Adding entry to the created special FG...\r\n");
    bcm_field_entry_info_t_init(&ent_special_info);
    ent_special_info.nof_entry_quals = 1;
    ent_special_info.entry_qual[0].type = fg_info.qual_types[0];
    if (field_stage == bcmFieldStageIngressPMF3)
    {
        ent_special_info.entry_qual[0].value[0] = 0x0;
        ent_special_info.entry_qual[0].mask[0] = 0xff;
    }
    else
    {
        ent_special_info.entry_qual[0].value[0] = 0x5;
        ent_special_info.entry_qual[0].mask[0] = 0xff;
    }
    ent_special_info.entry_qual[1].type = fg_info.qual_types[1];
    ent_special_info.entry_qual[1].value[0] = 0x5;
    ent_special_info.entry_qual[1].mask[0] = 0xff;
    ent_special_info.entry_qual[2].type = fg_info.qual_types[2];
    ent_special_info.entry_qual[2].value[0] = 0x5;
    ent_special_info.entry_qual[2].mask[0] = 0xff;

    ent_special_info.nof_entry_actions = 1;
    ent_special_info.entry_action[0].type = fg_info.action_types[0];
    ent_special_info.entry_action[0].value[0] = 0x1;

    rv = bcm_field_entry_add(unit, 0, fg_special_id, &ent_special_info, &ent_special_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    return rv;
}

int field_tcam_dt_destroy(
    int unit,
    bcm_field_stage_t field_stage)
{
    int rv = BCM_E_NONE;
    bcm_field_stage_t field_stage_internal;

    printf("Deleting special entry...\r\n");
    rv = bcm_field_entry_delete(unit, fg_special_id, NULL, ent_special_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete  fg %d \n", rv, fg_special_id);
        return rv;
    }

    printf("Deleting DT entry...\r\n");
    rv = bcm_field_entry_delete(unit, fg_dt_id, NULL, ent_dt_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete  fg %d \n", rv, fg_dt_id);
        return rv;
    }

    printf("Detaching special FG from created context...\r\n");
    rv = bcm_field_group_context_detach(unit, fg_special_id, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, fg_special_id, context_id);
        return rv;
    }

    /** In case of IPMF2 stage the context should be created in IPMF1 stage. */
    if (field_stage == bcmFieldStageIngressPMF2)
    {
        field_stage_internal = bcmFieldStageIngressPMF1;
    }
    else
    {
        field_stage_internal = field_stage;
    }

    printf("Destroying created context...\r\n");
    rv = bcm_field_context_destroy(unit, field_stage_internal, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy contex_id %d \n", rv, context_id);
        return rv;
    }


    if (field_stage == bcmFieldStageIngressPMF3)
    {
        printf("Detaching DT FG from IPMF3 context...\r\n");
        rv = bcm_field_group_context_detach(unit, fg_dt_id, cint_field_tcam_dt_ipmf3_context_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_group_context_detach FG %d from context %d\n", rv, fg_dt_id,
                cint_field_tcam_dt_ipmf3_context_id);
            return rv;
        }

        printf("Destroying created context...\r\n");
        rv = field_presel_fwd_layer_destroy(unit, cint_field_tcam_dt_ipmf3_presel_id, field_stage,
                                            cint_field_tcam_dt_ipmf3_context_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error in field_presel_fwd_layer_destroy. \n");
            return rv;
        }
    }
    else
    {
        printf("Detaching DT FG from default context...\r\n");
        rv = bcm_field_group_context_detach(unit, fg_dt_id, BCM_FIELD_CONTEXT_ID_DEFAULT);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_group_context_detach FG %d from default context\n", rv, fg_dt_id);
            return rv;
        }
    }

    printf("Deleting special FG...\r\n");
    rv = bcm_field_group_delete(unit, fg_special_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  FG %d \n", rv, fg_dt_id);
        return rv;
    }
 
    printf("Deleting DT FG...\r\n");
    rv = bcm_field_group_delete(unit, fg_dt_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  FG %d \n", rv, fg_dt_id);
        return rv;
    }

    if (field_stage == bcmFieldStageIngressPMF3)
    {
        rv = bcm_field_qualifier_destroy(unit, cint_field_tcam_dt_qual_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_qualifier_destroy. \n");
            return rv;
        }
    }

    return rv;
}
