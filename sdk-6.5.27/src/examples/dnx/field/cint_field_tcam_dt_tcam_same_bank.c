/*
 * Configuration example start:
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/field/cint_field_tcam_dt_tcam_same_bank.c
  cint;
  int unit = 0;
  bcm_field_context_t context_id_dt = bcmFieldStageIngressPMF1;
  bcm_field_context_t context_id_tcam = bcmFieldStageIngressPMF1;
  uint32 tc_val_dt = 1;
  uint32 tc_val_tcam = 1;
  int bank_id = 1;
  field_tcam_dt_tcam_same_bank_main(unit, context_id_dt, context_id_tcam, tc_val_dt, tc_val_tcam, bank_id);
 * Test Scenario - end
 * Configures a TCAM DT field group and a TCAM field group that use the same TCAM bank.
 * Configuration example end
 *
 */

bcm_field_group_t fg_id_dt = BCM_FIELD_ID_INVALID;
bcm_field_group_t fg_id_tcam = BCM_FIELD_ID_INVALID;
bcm_field_entry_t entry_id_dt = BCM_FIELD_ID_INVALID;
bcm_field_entry_t entry_id_tcam = BCM_FIELD_ID_INVALID;

int field_tcam_dt_tcam_same_bank_main(
    int unit,
    bcm_field_context_t context_id_dt,
    bcm_field_context_t context_id_tcam,
    uint32 tc_val_dt,
    uint32 tc_val_tcam,
    int bank_id)
{
    bcm_field_group_info_t fg_info_dt;
    bcm_field_group_info_t fg_info_tcam;
    bcm_field_group_attach_info_t attach_info_dt;
    bcm_field_group_attach_info_t attach_info_tcam;
    bcm_field_entry_info_t entry_info_dt;
    bcm_field_entry_info_t entry_info_tcam;

    printf("Creating a TCAM DT FG...\r\n");
    bcm_field_group_info_t_init(&fg_info_dt);
    fg_info_dt.fg_type = bcmFieldGroupTypeDirectTcam;
    fg_info_dt.stage = bcmFieldStageIngressPMF1;
    fg_info_dt.nof_quals = 1;
    fg_info_dt.qual_types[0] = bcmFieldQualifyIp4Ttl;
    fg_info_dt.nof_actions = 1;
    fg_info_dt.action_types[0] = bcmFieldActionPrioIntNew;
    /*
     * We place the two field groups together on the same bank using manual TCAM bank allocation, but we could have used
     * the default bcmFieldTcamBankAllocationModeAuto instead, and they might have ended up in the same bank.
     */
    fg_info_dt.tcam_info.bank_allocation_mode = bcmFieldTcamBankAllocationModeSelect;
    fg_info_dt.tcam_info.nof_tcam_banks = 1;
    fg_info_dt.tcam_info.tcam_bank_ids[0] = bank_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &fg_info_dt, &fg_id_dt), "");

    printf("Attaching created DT FG to the first context...\r\n");
    bcm_field_group_attach_info_t_init(&attach_info_dt);
    attach_info_dt.key_info.nof_quals = fg_info_dt.nof_quals;
    attach_info_dt.payload_info.nof_actions = fg_info_dt.nof_actions;
    attach_info_dt.key_info.qual_types[0] = fg_info_dt.qual_types[0]; 
    attach_info_dt.payload_info.action_types[0] = fg_info_dt.action_types[0];
    attach_info_dt.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info_dt.key_info.qual_info[0].input_arg = 1;
    attach_info_dt.key_info.qual_info[0].offset = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, fg_id_dt, context_id_dt, &attach_info_dt), "");

    printf("Adding entry to the created DT FG...\r\n");
    bcm_field_entry_info_t_init(&entry_info_dt);
    entry_info_dt.nof_entry_quals = 1;
    entry_info_dt.entry_qual[0].type = fg_info_dt.qual_types[0];
    entry_info_dt.entry_qual[0].value[0] = 0x20;
    entry_info_dt.entry_qual[0].mask[0] = 0xff;
    entry_info_dt.nof_entry_actions = 1;
    entry_info_dt.entry_action[0].type = fg_info_dt.action_types[0];
    entry_info_dt.entry_action[0].value[0] = tc_val_dt;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, fg_id_dt, &entry_info_dt, &entry_id_dt), "");

    printf("Creating a TCAM FG with a key size between 81-160 bits and payload no bigger than 32...\r\n");
    bcm_field_group_info_t_init(&fg_info_tcam);
    fg_info_tcam.fg_type = bcmFieldGroupTypeTcam;
    fg_info_tcam.stage = bcmFieldStageIngressPMF1;
    fg_info_tcam.nof_quals = 4;
    fg_info_tcam.qual_types[0] = bcmFieldQualifySrcIp;
    fg_info_tcam.qual_types[1] = bcmFieldQualifyDstIp;
    fg_info_tcam.qual_types[2] = bcmFieldQualifyL4SrcPort;
    fg_info_tcam.qual_types[3] = bcmFieldQualifyL4DstPort;
    fg_info_tcam.nof_actions = 1;
    fg_info_tcam.action_types[0] = bcmFieldActionPrioIntNew;
    fg_info_tcam.tcam_info.bank_allocation_mode = bcmFieldTcamBankAllocationModeSelect;
    fg_info_tcam.tcam_info.nof_tcam_banks = 1;
    fg_info_tcam.tcam_info.tcam_bank_ids[0] = bank_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &fg_info_tcam, &fg_id_tcam), "");

    printf("Attaching created TCAM FG to the second context...\r\n");
    bcm_field_group_attach_info_t_init(&attach_info_tcam);

    attach_info_tcam.key_info.nof_quals = fg_info_tcam.nof_quals;
    attach_info_tcam.payload_info.nof_actions = fg_info_tcam.nof_actions;
    attach_info_tcam.key_info.qual_types[0] = fg_info_tcam.qual_types[0]; 
    attach_info_tcam.key_info.qual_types[1] = fg_info_tcam.qual_types[1]; 
    attach_info_tcam.key_info.qual_types[2] = fg_info_tcam.qual_types[2];
    attach_info_tcam.key_info.qual_types[3] = fg_info_tcam.qual_types[3];
    attach_info_tcam.payload_info.action_types[0] = fg_info_tcam.action_types[0];
    attach_info_tcam.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info_tcam.key_info.qual_info[0].input_arg = 1;
    attach_info_tcam.key_info.qual_info[0].offset = 0;
    attach_info_tcam.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info_tcam.key_info.qual_info[1].input_arg = 1;
    attach_info_tcam.key_info.qual_info[1].offset = 0;
    attach_info_tcam.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info_tcam.key_info.qual_info[2].input_arg = 2;
    attach_info_tcam.key_info.qual_info[2].offset = 0;
    attach_info_tcam.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info_tcam.key_info.qual_info[3].input_arg = 2;
    attach_info_tcam.key_info.qual_info[3].offset = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, fg_id_tcam, context_id_tcam, &attach_info_tcam), "");

    printf("Adding entry to the created TCAM FG...\r\n");
    bcm_field_entry_info_t_init(&entry_info_tcam);
    entry_info_tcam.nof_entry_quals = 1;
    entry_info_tcam.entry_qual[0].type = fg_info_tcam.qual_types[0];
    entry_info_tcam.entry_qual[0].value[0] = 0x01010100;
    entry_info_tcam.entry_qual[0].mask[0] = 0xFFFFFF00;
    entry_info_tcam.entry_qual[1].value[0] = 0x01010100;
    entry_info_tcam.entry_qual[1].mask[0] = 0xFFFFFF00;
    entry_info_tcam.entry_qual[2].value[0] = 300;
    entry_info_tcam.entry_qual[2].mask[0] = 0xFFFF;
    entry_info_tcam.entry_qual[2].value[0] = 400;
    entry_info_tcam.entry_qual[2].mask[0] = 0xFFFF;
    entry_info_tcam.nof_entry_actions = 1;
    entry_info_tcam.entry_action[0].type = fg_info_tcam.action_types[0];
    entry_info_tcam.entry_action[0].value[0] = tc_val_tcam;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, fg_id_tcam, &entry_info_tcam, &entry_id_tcam), "");
    return BCM_E_NONE;
}

int field_tcam_dt_tcam_same_bank_destroy(
    int unit,
    bcm_field_context_t context_id_dt,
    bcm_field_context_t context_id_tcam)
{
    printf("Deleting TCAM entry...\r\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, fg_id_tcam, NULL, entry_id_tcam), "");

    printf("Deleting DT entry...\r\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, fg_id_dt, NULL, entry_id_dt), "");

    printf("Detaching TCAM FG from created context...\r\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, fg_id_tcam, context_id_tcam), "");

    printf("Detaching DT FG from context...\r\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, fg_id_dt, context_id_dt), "");

    printf("Deleting TCAM FG...\r\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, fg_id_tcam), "");
 
    printf("Deleting DT FG...\r\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, fg_id_dt), "");

    return BCM_E_NONE;
}
