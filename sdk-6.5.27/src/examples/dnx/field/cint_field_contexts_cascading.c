 /*
 * Configuration example start:
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/field/cint_field_contexts_cascading.c
  cint;
  int unit = 0;
  bcm_field_context_t context_id = 0;
  uint32 presel_id = 50;
  field_contexts_cascading_main(unit, presel_id, context_id);
 * Test Scenario - end
 *
 * Configuration example end
 *
 */

bcm_field_context_t context_id_ipmf2;
bcm_field_presel_t _presel_id = 0;
int field_contexts_cascading_main(int unit, uint32 presel_id, bcm_field_context_t context_id_ipmf1)
{
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_data;
    bcm_field_presel_entry_id_t presel_ent;

    bcm_field_context_info_t_init(&context_info);
    context_info.cascaded_from = context_id_ipmf1;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF2, &context_info, &context_id_ipmf2), "");

    _presel_id = presel_id;
    presel_ent.presel_id = _presel_id;
    presel_ent.stage = bcmFieldStageIngressPMF2;
    presel_data.entry_valid = TRUE;
    presel_data.context_id = context_id_ipmf2;
    presel_data.nof_qualifiers = 1;
    presel_data.qual_data[0].qual_type = bcmFieldQualifyStateTableData;
    presel_data.qual_data[0].qual_value = 0;
    presel_data.qual_data[0].qual_mask = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(unit, 0, &presel_ent, &presel_data), "");

    return BCM_E_NONE;
}

int field_contexts_cascading_destroy(int unit)
{
    bcm_field_presel_entry_id_t presel_ent;
    bcm_field_presel_entry_data_t presel_data;
    presel_ent.presel_id = _presel_id;
    presel_ent.stage = bcmFieldStageIngressPMF2;
    presel_data.entry_valid = FALSE;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(unit, 0, &presel_ent, &presel_data), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_context_destroy(unit, bcmFieldStageIngressPMF2, context_id_ipmf2), "");

    return BCM_E_NONE;
}
