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
  * Global variables
  */
bcm_field_group_t cint_field_presel_rx_trap_profile_fg_id;
bcm_field_action_t cint_field_presel_rx_trap_profile_action_id;
uint32 cint_field_presel_rx_trap_profile_action_value = 0xabcd;
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

    rv = cint_field_presel_rx_trap_profile_fg_config(unit, *context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_presel_rx_trap_profile_fg_config\n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 * Create Const FG and attach it to created context (for verification)
 *
 * \param [in] unit    - Device ID
 * \param [in] context_id  - Context id created
 *
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_presel_rx_trap_profile_fg_config(
    int unit,
    bcm_field_context_t context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_info_t action_info_get_size;
    void *dest_char;
    int action_id;
    int rv = BCM_E_NONE;

    rv = bcm_field_action_info_get(unit, bcmFieldActionUDHData0, bcmFieldStageIngressPMF3, &action_info_get_size);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_info_get\n", rv);
        return rv;
    }

    /*
     * Create a user define action with size 0, value is fixed
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionUDHData0;
    action_info.prefix_size = action_info_get_size.size;
    action_info.prefix_value = cint_field_presel_rx_trap_profile_action_value;
    action_info.size = 0;
    action_info.stage = bcmFieldStageIngressPMF3;
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_presel_rx_trap_profile_action_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_create\n", rv);
        return rv;
    }
    action_id = cint_field_presel_rx_trap_profile_action_id;
    printf("Created user defined action (%d)\n", action_id);

    bcm_field_group_info_t_init(&fg_info);
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "RX_TRAP_PROFLE_FG", sizeof(fg_info.name));
    fg_info.fg_type = bcmFieldGroupTypeConst;
    fg_info.stage = bcmFieldStageIngressPMF3;
    fg_info.nof_quals = 0;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = cint_field_presel_rx_trap_profile_action_id;
    fg_info.action_with_valid_bit[0] = FALSE;

    rv = bcm_field_group_add(unit, 0, &fg_info, &cint_field_presel_rx_trap_profile_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created Const field group (%d)\n", cint_field_presel_rx_trap_profile_fg_id);

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    rv = bcm_field_group_context_attach(unit, 0, cint_field_presel_rx_trap_profile_fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached field group (%d) to context (%d)\n", cint_field_presel_rx_trap_profile_fg_id, context_id);

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

    /* Detach the IPMF3 const FG from its context */
    rv = bcm_field_group_context_detach(unit, cint_field_presel_rx_trap_profile_fg_id, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach\n", rv);
        return rv;
    }

    /* Delete constant FG */
    rv = bcm_field_group_delete(unit, cint_field_presel_rx_trap_profile_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete \n", rv);
        return rv;
    }

    /* Delete user define action */
    rv = bcm_field_action_destroy(unit, cint_field_presel_rx_trap_profile_action_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_destroy\n", rv);
        return rv;
    }

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
