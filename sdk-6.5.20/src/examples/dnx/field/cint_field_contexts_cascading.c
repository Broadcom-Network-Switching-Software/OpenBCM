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
 * cint ../../../src/examples/dnx/field/cint_field_contexts_cascading.c
 * cint;
 * int unit = 0;
 * uint32 presel_id = 50;
 * field_contexts_cascading_main(unit, presel_id);
 *
 * Configuration example end
 */

bcm_field_context_t context_id;
bcm_field_presel_t _presel_id = 0;
int field_contexts_cascading_main(int unit, uint32 presel_id)
{
    /* Created cascaded contexts */
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_data;
    bcm_field_presel_entry_id_t presel_ent;
    int rv;

    bcm_field_context_info_t_init(&context_info);
    context_info.cascaded_from = BCM_FIELD_CONTEXT_ID_DEFAULT;
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF2, &context_info, &context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    _presel_id = presel_id;
    presel_ent.presel_id = _presel_id;
    presel_ent.stage = bcmFieldStageIngressPMF2;
    presel_data.entry_valid = TRUE;
    presel_data.context_id = context_id;
    presel_data.nof_qualifiers = 1;
    presel_data.qual_data[0].qual_type = bcmFieldQualifyStateTableData;
    presel_data.qual_data[0].qual_value = 0;
    presel_data.qual_data[0].qual_mask = 0;
    rv = bcm_field_presel_set(unit, 0, &presel_ent, &presel_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_dnx_field_presel_set\n", rv);
        return rv;
    }
    return rv;
}

int field_contexts_cascading_destroy(int unit)
{
    int rv;
    bcm_field_presel_entry_id_t presel_ent;
    bcm_field_presel_entry_data_t presel_data;
    presel_ent.presel_id = _presel_id;
    presel_ent.stage = bcmFieldStageIngressPMF2;
    presel_data.entry_valid = FALSE;

    rv = bcm_field_presel_set(unit, 0, &presel_ent, &presel_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_dnx_field_presel_set\n", rv);
        return rv;
    }
    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF2, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy\n", rv);
        return rv;
    }
    return rv;
}
