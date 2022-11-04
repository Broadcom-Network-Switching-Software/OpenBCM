/*
 * This cint includes an example of using the Field Group TCAM API to add
 * a new FG that adds new entries by a specific location (EAL mode).
 *
 * Configuration example start:
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/field/cint_field_tcam_entry_add_by_location.c
  cint;
  int unit = 0;
  bcm_field_context_t context_id = 0;
  field_tcam_entry_add_by_location(unit, context_id);
 * Test Scenario - end
 *
 * Configuration example end
 *
 */

/*
* Restrictions: bank_offset should always be even for single/double-sized key FGs. Also bank_id should always be even for double-sized key FGs
*/
int convert_bank_address_to_absolute(
    int unit,
    int bank_id,
    int bank_offset)
{
    int nof_lines_per_bank = *(dnxc_data_get(unit, "field", "tcam", "nof_big_bank_lines", NULL));
    return bank_id*nof_lines_per_bank + bank_offset;
}

bcm_field_group_t cint_field_tcam_entry_add_by_location_fg_id;
bcm_field_entry_t cint_field_tcam_entry_add_by_location_entry_id;
bcm_field_entry_t cint_field_tcam_entry_add_by_location_entry_id_2;

int field_tcam_entry_add_by_location(int unit, bcm_field_context_t context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_group_attach_info_t attach_info;
    void *dest_char;
    int dnx_data_nof_lines_per_bank = *(dnxc_data_get(unit, "field", "tcam", "nof_big_bank_lines", NULL));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifySrcIp;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionPrioIntNew;
    /* Enabel EAL mode */
    fg_info.tcam_info.bank_allocation_mode = bcmFieldTcamBankAllocationModeSelectWithLocation;
    /* Allocate bank_id 8 */
    fg_info.tcam_info.nof_tcam_banks = 1;
    fg_info.tcam_info.tcam_bank_ids[0] = 8;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "EAL_FG", sizeof(fg_info.name));

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &fg_info, &cint_field_tcam_entry_add_by_location_fg_id), "");


    /** Attach the FG to default context. */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, cint_field_tcam_entry_add_by_location_fg_id, context_id, &attach_info), "");


    /* Add entry by location */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = convert_bank_address_to_absolute(unit, 8, 2);

    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = 5;
    ent_info.entry_qual[0].mask[0] = 0xFF;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = 0x2;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, cint_field_tcam_entry_add_by_location_fg_id, &ent_info, &cint_field_tcam_entry_add_by_location_entry_id), "");


    ent_info.priority = convert_bank_address_to_absolute(unit, 8, 4);

    ent_info.entry_qual[0].value[0] = 5;
    ent_info.entry_qual[0].mask[0] = 0xF;

    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = 0x1;


    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, cint_field_tcam_entry_add_by_location_fg_id, &ent_info, &cint_field_tcam_entry_add_by_location_entry_id_2), "");


    return BCM_E_NONE;
}

int field_tcam_entry_add_by_location_destroy(int unit, bcm_field_context_t context_id)
{
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, cint_field_tcam_entry_add_by_location_fg_id, context_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, cint_field_tcam_entry_add_by_location_fg_id, NULL, cint_field_tcam_entry_add_by_location_entry_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, cint_field_tcam_entry_add_by_location_fg_id, NULL, cint_field_tcam_entry_add_by_location_entry_id_2), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, cint_field_tcam_entry_add_by_location_fg_id), "");

    return BCM_E_NONE;
}
