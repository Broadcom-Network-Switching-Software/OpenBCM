/*
 * Configuration example start:
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/field/cint_field_tcam_dt.c
  cint;
  int unit = 0;
  bcm_field_context_t context_id = 0;
  uint32 ttl = 10;
  uint32 tc_val = 1;
  field_tcam_dt_main(unit, context_id, ttl, dp_val);
 * Test Scenario - end
 *
 * Configuration example end
 *
 */

bcm_field_group_t fg_dt_id = BCM_FIELD_ID_INVALID;
bcm_field_entry_t entry_id = BCM_FIELD_ID_INVALID;
bcm_field_presel_entry_id_t cint_field_tcam_dt_presel_entry_id;

int field_tcam_dt_main(
    int unit,
    bcm_field_context_t context_id,
    uint32 ttl,
    uint32 tc_val)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;

    printf("Creating TCAM DT FG...\r\n");
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyIp4Ttl;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionPrioIntNew;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &fg_info, &fg_dt_id), "");

    printf("Attaching created DT FG to default context...\r\n");
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0]; 
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, fg_dt_id, context_id, &attach_info), "");

    printf("Adding entry to the created DT FG...\r\n");
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = 1;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = ttl;
    entry_info.entry_qual[0].mask[0] = 0xff;
    entry_info.nof_entry_actions = 1;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = tc_val;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, fg_dt_id, &entry_info, &entry_id), "");

    return BCM_E_NONE;
}

int field_tcam_dt_destroy(
    int unit,
    bcm_field_context_t context_id)
{
    printf("Deleting DT entry...\r\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, fg_dt_id, NULL, entry_id), "");

    printf("Detaching DT FG from context...\r\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, fg_dt_id, context_id), "");

    printf("Deleting DT FG...\r\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, fg_dt_id), "");

    return BCM_E_NONE;
}
