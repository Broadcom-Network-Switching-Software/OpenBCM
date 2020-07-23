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
 * cint ../../../src/examples/dnx/field/cint_field_presel_rx_trap_profile.c
 * cint;
 * int unit = 0;
 * int trap_profile = 1;
 * bcm_field_presel_t presel_id = 50;
 * bcm_field_context_t context_id;
 * int trap_strength = 12;
 * int trap_id;
 * cint_field_presel_rx_trap_profile_trap_config_main(unit, trap_strength, &trap_id);
 * cint_field_presel_rx_trap_profile_main(unit, trap_profile, presel_id, &context_id);
 *
 * Configuration example end
 *
 *  Configuration:
 *      Example for how to configure context selection in IPMF3 stage upon
 *      RX trap profile (bcmFieldQualifyRxTrapProfile).
 *      The cint shows also the configuration of trap that sets RX trap profile
 *      by using of BCM_RX_TRAP_FLAGS2_UPDATE_ACL_PROFILE flag.
 */

/**
 * \brief
 *   Set trap action to drop packets whose smac equal to dmac.
 *
 * \param [in] unit    - Device ID
 * \param [in] trap_strength - Trap strenght to be used.
 * \param [out] trap_id  - Trap id created
 *
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_presel_rx_trap_profile_trap_config_main(
    int unit,
    int trap_strength,
    int * trap_id)
{
    bcm_rx_trap_config_t trap_config;
    int rv = BCM_E_NONE;

    bcm_rx_trap_config_t_init(&trap_config);

    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    /** Use the flag in order to set the ACL_PROFILE. */
    trap_config.flags2 = BCM_RX_TRAP_FLAGS2_UPDATE_ACL_PROFILE;
    trap_config.dest_port = BCM_GPORT_BLACK_HOLE;
    trap_config.trap_strength = trap_strength;

    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapLinkLayerSaEqualsDa, trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_rx_trap_type_create \n", rv);
        return rv;
    }

    rv = bcm_rx_trap_set(unit, *trap_id, &trap_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_rx_trap_set \n", rv);
        return rv;
    }

    printf("Trap LinkLayerSaEqualsDa (trap_id=%d) was created and set (trap_strength=%d)!\n", *trap_id, trap_config.trap_strength);

    return rv;
}

/**
 * \brief
 *
 * \param [in] unit    - Device ID
 * \param [in] trap_profile - Trap profile, will be used as context selection value.
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
int cint_field_presel_rx_trap_profile_main(
    int unit,
    int trap_profile,
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
    sal_strncpy_s(dest_char, "RX_TRAP_PROFLE", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create \n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = presel_id;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = TRUE;
    p_data.context_id = *context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyRxTrapProfile;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = trap_profile;
    p_data.qual_data[0].qual_mask = 0x1;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set \n", rv);
        return rv;
    }

    printf("Presel (%d) was configured for stage(IngressPMF3) context(%d) RxTrapProfile(%d) \n",
        presel_id, *context_id, trap_profile);

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
int cint_field_presel_rx_trap_profile_destroy(
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
    p_id.stage = bcmFieldStageIngressPMF3;
    p_id.presel_id = presel_id;
    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_presel_set \n");
        return rv;
    }

    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF3, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy \n", rv);
        return rv;
    }

    return rv;
}
