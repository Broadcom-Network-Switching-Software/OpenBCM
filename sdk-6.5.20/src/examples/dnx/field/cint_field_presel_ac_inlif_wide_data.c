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
 * cint ../../../src/examples/dnx/field/cint_field_presel_ac_inlif_wide_data.c
 * cint;
 * int unit = 0;
 * int ac_inlif_wide_data_qual_value = 1;
 * bcm_field_presel_t presel_id = 50;
 * bcm_field_context_t context_id;
 * cint_field_presel_ac_inlif_wide_data_main(unit, ac_inlif_wide_data_qual_value, presel_id, &context_id);
 *
 * Configuration example end
 *
 *  Configuration:
 *      Example for how to configure context selection in IPMF1 stage upon
 *      AC In-Lif Wide Data (bcmFieldQualifyAcInLifWideData).
 */

/**
 * \brief
 *
 * \param [in] unit    - Device ID
 * \param [in] ac_inlif_wide_data_qual_value - AC In-Lif wide data value, will be used as context selection value.
 * \param [in] presel_id   - Presel to configure
 * \param [out] context_id  - Context id created
 *
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_presel_ac_inlif_wide_data_main(
    int unit,
    int ac_inlif_wide_data_qual_value,
    bcm_field_presel_t presel_id,
    bcm_field_context_t  * context_id)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    void *dest_char;
    int rv = BCM_E_NONE;

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "AC_INLIF_WIDE_DATA", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create \n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = presel_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = *context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyAcInLifWideData;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = ac_inlif_wide_data_qual_value;
    p_data.qual_data[0].qual_mask = 0x1;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set \n", rv);
        return rv;
    }

    printf("Presel (%d) was configured for stage(IngressPMF1) context(%d) AcInLifWideData(%d) \n",
        presel_id, *context_id, ac_inlif_wide_data_qual_value);

    return rv;
}

/**
 * \brief
 *  Destroys all allocated data by the configuration.
 *
 * \param [in] unit   - Device id
 * \param [in] presel_id   - Presel to destroy
 * \param [in] context_id  - Context Id to destroy
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_presel_ac_inlif_wide_data_destroy(
    int unit,
    bcm_field_presel_t presel_id,
    bcm_field_context_t context_id)
{
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    int rv = BCM_E_NONE;

    bcm_field_presel_entry_data_info_init(&p_data);
    bcm_field_presel_entry_id_info_init(&p_id);
    p_data.entry_valid = FALSE;
    p_data.context_id = 0;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_id.presel_id = presel_id;
    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_presel_set \n");
        return rv;
    }

    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy \n", rv);
        return rv;
    }

    return rv;
}
