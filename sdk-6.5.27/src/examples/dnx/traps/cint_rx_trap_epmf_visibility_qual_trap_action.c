/* $Id: cint_rx_trap_epmf_visibility_qual_trap_action.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: cint_rx_trap_epmf_visibility_qual_trap_action.c
 * Purpose: Example for ePMF configuration with visibility qualifier and trap action.
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_epmf_visibility_qual_trap_action.c
 *
 * Main Function:
 *      cint_rx_trap_epmf_visibility_qual_trap_action_main(unit,context_id);
 * Destroy Function:
 *      cint_rx_trap_epmf_visibility_qual_trap_action_destroy(unit);
 *
 * Example Config:
 * Test Scenario - start
  cint ../../../../src/examples/dnx/traps/cint_utils_rx.c
  cint ../../../../src/examples/dnx/traps/cint_rx_trap_erpp_traps.c
 * or cint_rx_trap_egress_traps.c (depending on device)
  cint ../../../../src/examples/dnx/traps/cint_rx_trap_epmf_visibility_qual_trap_action.c
  cint
  cint_rx_trap_epmf_visibility_qual_trap_action_main(0,0);
 * Test Scenario - end
 *
 */

bcm_field_group_t cint_rx_trap_epmf_visibility_qual_trap_action_fg_id;
bcm_field_entry_t cint_rx_trap_epmf_visibility_qual_trap_action_entry_id;
int cint_rx_trap_epmf_visibility_qual_trap_action_trap_id;

/**
 * \brief
 *
 *  Create and set an Egress User-defined trap
 *
 * \param [in] unit         - The unit number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_epmf_visibility_qual_trap_action_trap_create_and_set(
    int unit)
{
    bcm_rx_trap_t trap_type;
    bcm_rx_trap_config_t trap_config;

    if(*dnxc_data_get(unit, "pp", "stages", "is_erpp_supported", NULL))
    {
        trap_type = bcmRxTrapEgUserDefine;
        BCM_IF_ERROR_RETURN_MSG(trap_config_from_erpp_config_fill(&cint_rx_trap_erpp_traps_config, &trap_config),
            "in trap_config structure fill");
    }
    else
    {
        trap_type = bcmRxTrapEgTxUserDefine;
        BCM_IF_ERROR_RETURN_MSG(trap_config_from_egress_config_fill(&cint_rx_trap_egress_traps_config, &trap_config),
            "in trap_config structure fill");
    }


    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, 0, trap_type, &cint_rx_trap_epmf_visibility_qual_trap_action_trap_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, cint_rx_trap_epmf_visibility_qual_trap_action_trap_id, &trap_config), "");

    return BCM_E_NONE;
}

/**
 * \brief
 *
 *  Creates a TCAM FG and attaches it to the default context.
 *  The FG has 1 entry with qualifier VRF and action Trap.
 *
 * \param [in] unit         - The unit number.
 * \param [in] context_id   - The field context id number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_epmf_visibility_qual_trap_action_pmf_configuration_create(
    int unit,
    bcm_field_context_t  context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    bcm_gport_t trap_gport;

    bcm_field_group_info_t_init(&fg_info);

    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageEgress;
    fg_info.nof_actions = 1;
    fg_info.nof_quals = 1;
    fg_info.action_with_valid_bit[0] = TRUE;
    fg_info.action_types[0] = bcmFieldActionTrap;
    fg_info.qual_types[0] = bcmFieldQualifyVisibility;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, fg_info, &cint_rx_trap_epmf_visibility_qual_trap_action_fg_id), "");

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id, context_id, &attach_info), "");

    bcm_field_entry_info_t_init(&entry_info);

    entry_info.priority = 10;
    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.nof_entry_quals = fg_info.nof_quals;

    BCM_GPORT_TRAP_SET(trap_gport,cint_rx_trap_epmf_visibility_qual_trap_action_trap_id,15,0);
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = trap_gport;

    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = 1;
    entry_info.entry_qual[0].mask[0] = 0x1;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id, &entry_info, &cint_rx_trap_epmf_visibility_qual_trap_action_entry_id), "");

    return BCM_E_NONE;
}

/**
 * \brief
 *
 *  Destroying the full PMF and Trap config created in the CINT.
 *
 * \param [in] unit         - The unit number.
 * \param [in] context_id   - The field context id number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_epmf_visibility_qual_trap_action_destroy(
    int unit,
    bcm_field_context_t  context_id)
{

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id, NULL, cint_rx_trap_epmf_visibility_qual_trap_action_entry_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id, context_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_destroy(unit, cint_rx_trap_epmf_visibility_qual_trap_action_trap_id), "");

    return BCM_E_NONE;
}

/**
 * \brief
 *
 *  Main function of the cint.
 *  Creates and sets an Egress User-defined trap.
 *  Creates a TCAM FG which has 1 entry with qualifier visibility and action Trap.
 *
 * \param [in] unit         - The unit number.
 * \param [in] context_id   - The field context id number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_epmf_visibility_qual_trap_action_main(
    int unit,
    bcm_field_context_t context_id)
{


    BCM_IF_ERROR_RETURN_MSG(cint_rx_trap_epmf_visibility_qual_trap_action_trap_create_and_set(unit), "");

    printf("Trap_ID = %d \n", cint_rx_trap_epmf_visibility_qual_trap_action_trap_id);

    BCM_IF_ERROR_RETURN_MSG(cint_rx_trap_epmf_visibility_qual_trap_action_pmf_configuration_create(unit, context_id), "");

    return BCM_E_NONE;
}

/**
 * \brief
 *
 *  Create and set an Egress User-defined trap
 *
 * \param [in] unit         - The unit number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_epmf_visibility_qual_trap_action_trap_create_and_set(
    int unit)
{
    bcm_rx_trap_t trap_type;
    bcm_rx_trap_config_t trap_config;

    if(*dnxc_data_get(unit, "pp", "stages", "is_erpp_supported", NULL))
    {
        trap_type = bcmRxTrapEgUserDefine;
        BCM_IF_ERROR_RETURN_MSG(trap_config_from_erpp_config_fill(&cint_rx_trap_erpp_traps_config, &trap_config),
            "in trap_config structure fill");
    }
    else
    {
        trap_type = bcmRxTrapEgTxUserDefine;
        BCM_IF_ERROR_RETURN_MSG(trap_config_from_egress_config_fill(&cint_rx_trap_egress_traps_config, &trap_config),
            "in trap_config structure fill");
    }


    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, 0, trap_type, &cint_rx_trap_epmf_visibility_qual_trap_action_trap_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, cint_rx_trap_epmf_visibility_qual_trap_action_trap_id, &trap_config), "");

    return BCM_E_NONE;
}

/**
 * \brief
 *
 *  Creates a TCAM FG and attaches it to the default context.
 *  The FG has 1 entry with qualifier VRF and action Trap.
 *
 * \param [in] unit         - The unit number.
 * \param [in] context_id   - The field context id number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_epmf_visibility_qual_trap_action_pmf_configuration_create(
    int unit,
    bcm_field_context_t  context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    bcm_gport_t trap_gport;

    bcm_field_group_info_t_init(&fg_info);

    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageEgress;
    fg_info.nof_actions = 1;
    fg_info.nof_quals = 1;
    fg_info.action_with_valid_bit[0] = TRUE;
    fg_info.action_types[0] = bcmFieldActionTrap;
    fg_info.qual_types[0] = bcmFieldQualifyVisibility;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, fg_info, &cint_rx_trap_epmf_visibility_qual_trap_action_fg_id), "");

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id, context_id, &attach_info), "");

    bcm_field_entry_info_t_init(&entry_info);

    entry_info.priority = 10;
    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.nof_entry_quals = fg_info.nof_quals;

    BCM_GPORT_TRAP_SET(trap_gport,cint_rx_trap_epmf_visibility_qual_trap_action_trap_id,15,0);
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = trap_gport;

    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = 1;
    entry_info.entry_qual[0].mask[0] = 0x1;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id, &entry_info, &cint_rx_trap_epmf_visibility_qual_trap_action_entry_id), "");

    return BCM_E_NONE;
}

/**
 * \brief
 *
 *  Destroying the full PMF and Trap config created in the CINT.
 *
 * \param [in] unit         - The unit number.
 * \param [in] context_id   - The field context id number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_epmf_visibility_qual_trap_action_destroy(
    int unit,
    bcm_field_context_t  context_id)
{

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id, NULL, cint_rx_trap_epmf_visibility_qual_trap_action_entry_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id, context_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_destroy(unit, cint_rx_trap_epmf_visibility_qual_trap_action_trap_id), "");

    return BCM_E_NONE;
}

/**
 * \brief
 *
 *  Main function of the cint.
 *  Creates and sets an Egress User-defined trap.
 *  Creates a TCAM FG which has 1 entry with qualifier visibility and action Trap.
 *
 * \param [in] unit         - The unit number.
 * \param [in] context_id   - The field context id number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_epmf_visibility_qual_trap_action_main(
    int unit,
    bcm_field_context_t context_id)
{


    BCM_IF_ERROR_RETURN_MSG(cint_rx_trap_epmf_visibility_qual_trap_action_trap_create_and_set(unit), "");

    printf("Trap_ID = %d \n", cint_rx_trap_epmf_visibility_qual_trap_action_trap_id);

    BCM_IF_ERROR_RETURN_MSG(cint_rx_trap_epmf_visibility_qual_trap_action_pmf_configuration_create(unit, context_id), "");

    return BCM_E_NONE;
}

/**
 * \brief
 *
 *  Create and set an Egress User-defined trap
 *
 * \param [in] unit         - The unit number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_epmf_visibility_qual_trap_action_trap_create_and_set(
    int unit)
{
    bcm_rx_trap_t trap_type;
    bcm_rx_trap_config_t trap_config;

    if(*dnxc_data_get(unit, "pp", "stages", "is_erpp_supported", NULL))
    {
        trap_type = bcmRxTrapEgUserDefine;
        BCM_IF_ERROR_RETURN_MSG(trap_config_from_erpp_config_fill(&cint_rx_trap_erpp_traps_config, &trap_config),
            "in trap_config structure fill");
    }
    else
    {
        trap_type = bcmRxTrapEgTxUserDefine;
        BCM_IF_ERROR_RETURN_MSG(trap_config_from_egress_config_fill(&cint_rx_trap_egress_traps_config, &trap_config),
            "in trap_config structure fill");
    }


    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, 0, trap_type, &cint_rx_trap_epmf_visibility_qual_trap_action_trap_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, cint_rx_trap_epmf_visibility_qual_trap_action_trap_id, &trap_config), "");

    return BCM_E_NONE;
}

/**
 * \brief
 *
 *  Creates a TCAM FG and attaches it to the default context.
 *  The FG has 1 entry with qualifier VRF and action Trap.
 *
 * \param [in] unit         - The unit number.
 * \param [in] context_id   - The field context id number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_epmf_visibility_qual_trap_action_pmf_configuration_create(
    int unit,
    bcm_field_context_t  context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    bcm_gport_t trap_gport;

    bcm_field_group_info_t_init(&fg_info);

    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageEgress;
    fg_info.nof_actions = 1;
    fg_info.nof_quals = 1;
    fg_info.action_with_valid_bit[0] = TRUE;
    fg_info.action_types[0] = bcmFieldActionTrap;
    fg_info.qual_types[0] = bcmFieldQualifyVisibility;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, fg_info, &cint_rx_trap_epmf_visibility_qual_trap_action_fg_id), "");

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id, context_id, &attach_info), "");

    bcm_field_entry_info_t_init(&entry_info);

    entry_info.priority = 10;
    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.nof_entry_quals = fg_info.nof_quals;

    BCM_GPORT_TRAP_SET(trap_gport,cint_rx_trap_epmf_visibility_qual_trap_action_trap_id,15,0);
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = trap_gport;

    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = 1;
    entry_info.entry_qual[0].mask[0] = 0x1;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id, &entry_info, &cint_rx_trap_epmf_visibility_qual_trap_action_entry_id), "");

    return BCM_E_NONE;
}

/**
 * \brief
 *
 *  Destroying the full PMF and Trap config created in the CINT.
 *
 * \param [in] unit         - The unit number.
 * \param [in] context_id   - The field context id number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_epmf_visibility_qual_trap_action_destroy(
    int unit,
    bcm_field_context_t  context_id)
{

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id, NULL, cint_rx_trap_epmf_visibility_qual_trap_action_entry_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id, context_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_destroy(unit, cint_rx_trap_epmf_visibility_qual_trap_action_trap_id), "");

    return BCM_E_NONE;
}

/**
 * \brief
 *
 *  Main function of the cint.
 *  Creates and sets an Egress User-defined trap.
 *  Creates a TCAM FG which has 1 entry with qualifier visibility and action Trap.
 *
 * \param [in] unit         - The unit number.
 * \param [in] context_id   - The field context id number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_epmf_visibility_qual_trap_action_main(
    int unit,
    bcm_field_context_t context_id)
{


    BCM_IF_ERROR_RETURN_MSG(cint_rx_trap_epmf_visibility_qual_trap_action_trap_create_and_set(unit), "");

    printf("Trap_ID = %d \n", cint_rx_trap_epmf_visibility_qual_trap_action_trap_id);

    BCM_IF_ERROR_RETURN_MSG(cint_rx_trap_epmf_visibility_qual_trap_action_pmf_configuration_create(unit, context_id), "");

    return BCM_E_NONE;
}

/**
 * \brief
 *
 *  Create and set an Egress User-defined trap
 *
 * \param [in] unit         - The unit number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_epmf_visibility_qual_trap_action_trap_create_and_set(
    int unit)
{
    bcm_rx_trap_t trap_type;
    bcm_rx_trap_config_t trap_config;

    if(*dnxc_data_get(unit, "pp", "stages", "is_erpp_supported", NULL))
    {
        trap_type = bcmRxTrapEgUserDefine;
        BCM_IF_ERROR_RETURN_MSG(trap_config_from_erpp_config_fill(&cint_rx_trap_erpp_traps_config, &trap_config),
            "in trap_config structure fill");
    }
    else
    {
        trap_type = bcmRxTrapEgTxUserDefine;
        BCM_IF_ERROR_RETURN_MSG(trap_config_from_egress_config_fill(&cint_rx_trap_egress_traps_config, &trap_config),
            "in trap_config structure fill");
    }


    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, 0, trap_type, &cint_rx_trap_epmf_visibility_qual_trap_action_trap_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, cint_rx_trap_epmf_visibility_qual_trap_action_trap_id, &trap_config), "");

    return BCM_E_NONE;
}

/**
 * \brief
 *
 *  Creates a TCAM FG and attaches it to the default context.
 *  The FG has 1 entry with qualifier VRF and action Trap.
 *
 * \param [in] unit         - The unit number.
 * \param [in] context_id   - The field context id number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_epmf_visibility_qual_trap_action_pmf_configuration_create(
    int unit,
    bcm_field_context_t  context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    bcm_gport_t trap_gport;

    bcm_field_group_info_t_init(&fg_info);

    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageEgress;
    fg_info.nof_actions = 1;
    fg_info.nof_quals = 1;
    fg_info.action_with_valid_bit[0] = TRUE;
    fg_info.action_types[0] = bcmFieldActionTrap;
    fg_info.qual_types[0] = bcmFieldQualifyVisibility;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, fg_info, &cint_rx_trap_epmf_visibility_qual_trap_action_fg_id), "");

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id, context_id, &attach_info), "");

    bcm_field_entry_info_t_init(&entry_info);

    entry_info.priority = 10;
    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.nof_entry_quals = fg_info.nof_quals;

    BCM_GPORT_TRAP_SET(trap_gport,cint_rx_trap_epmf_visibility_qual_trap_action_trap_id,15,0);
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = trap_gport;

    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = 1;
    entry_info.entry_qual[0].mask[0] = 0x1;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id, &entry_info, &cint_rx_trap_epmf_visibility_qual_trap_action_entry_id), "");

    return BCM_E_NONE;
}

/**
 * \brief
 *
 *  Destroying the full PMF and Trap config created in the CINT.
 *
 * \param [in] unit         - The unit number.
 * \param [in] context_id   - The field context id number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_epmf_visibility_qual_trap_action_destroy(
    int unit,
    bcm_field_context_t  context_id)
{

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id, NULL, cint_rx_trap_epmf_visibility_qual_trap_action_entry_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id, context_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_destroy(unit, cint_rx_trap_epmf_visibility_qual_trap_action_trap_id), "");

    return BCM_E_NONE;
}

/**
 * \brief
 *
 *  Main function of the cint.
 *  Creates and sets an Egress User-defined trap.
 *  Creates a TCAM FG which has 1 entry with qualifier visibility and action Trap.
 *
 * \param [in] unit         - The unit number.
 * \param [in] context_id   - The field context id number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_epmf_visibility_qual_trap_action_main(
    int unit,
    bcm_field_context_t context_id)
{


    BCM_IF_ERROR_RETURN_MSG(cint_rx_trap_epmf_visibility_qual_trap_action_trap_create_and_set(unit), "");

    printf("Trap_ID = %d \n", cint_rx_trap_epmf_visibility_qual_trap_action_trap_id);

    BCM_IF_ERROR_RETURN_MSG(cint_rx_trap_epmf_visibility_qual_trap_action_pmf_configuration_create(unit, context_id), "");

    return BCM_E_NONE;
}

/**
 * \brief
 *
 *  Create and set an Egress User-defined trap
 *
 * \param [in] unit         - The unit number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_epmf_visibility_qual_trap_action_trap_create_and_set(
    int unit)
{
    bcm_rx_trap_t trap_type;
    bcm_rx_trap_config_t trap_config;

    if(*dnxc_data_get(unit, "pp", "stages", "is_erpp_supported", NULL))
    {
        trap_type = bcmRxTrapEgUserDefine;
        BCM_IF_ERROR_RETURN_MSG(trap_config_from_erpp_config_fill(&cint_rx_trap_erpp_traps_config, &trap_config),
            "in trap_config structure fill");
    }
    else
    {
        trap_type = bcmRxTrapEgTxUserDefine;
        BCM_IF_ERROR_RETURN_MSG(trap_config_from_egress_config_fill(&cint_rx_trap_egress_traps_config, &trap_config),
            "in trap_config structure fill");
    }


    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, 0, trap_type, &cint_rx_trap_epmf_visibility_qual_trap_action_trap_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, cint_rx_trap_epmf_visibility_qual_trap_action_trap_id, &trap_config), "");

    return BCM_E_NONE;
}

/**
 * \brief
 *
 *  Creates a TCAM FG and attaches it to the default context.
 *  The FG has 1 entry with qualifier VRF and action Trap.
 *
 * \param [in] unit         - The unit number.
 * \param [in] context_id   - The field context id number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_epmf_visibility_qual_trap_action_pmf_configuration_create(
    int unit,
    bcm_field_context_t  context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    bcm_gport_t trap_gport;

    bcm_field_group_info_t_init(&fg_info);

    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageEgress;
    fg_info.nof_actions = 1;
    fg_info.nof_quals = 1;
    fg_info.action_with_valid_bit[0] = TRUE;
    fg_info.action_types[0] = bcmFieldActionTrap;
    fg_info.qual_types[0] = bcmFieldQualifyVisibility;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, fg_info, &cint_rx_trap_epmf_visibility_qual_trap_action_fg_id), "");

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id, context_id, &attach_info), "");

    bcm_field_entry_info_t_init(&entry_info);

    entry_info.priority = 10;
    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.nof_entry_quals = fg_info.nof_quals;

    BCM_GPORT_TRAP_SET(trap_gport,cint_rx_trap_epmf_visibility_qual_trap_action_trap_id,15,0);
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = trap_gport;

    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = 1;
    entry_info.entry_qual[0].mask[0] = 0x1;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id, &entry_info, &cint_rx_trap_epmf_visibility_qual_trap_action_entry_id), "");

    return BCM_E_NONE;
}

/**
 * \brief
 *
 *  Destroying the full PMF and Trap config created in the CINT.
 *
 * \param [in] unit         - The unit number.
 * \param [in] context_id   - The field context id number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_epmf_visibility_qual_trap_action_destroy(
    int unit,
    bcm_field_context_t  context_id)
{

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id, NULL, cint_rx_trap_epmf_visibility_qual_trap_action_entry_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id, context_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, cint_rx_trap_epmf_visibility_qual_trap_action_fg_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_destroy(unit, cint_rx_trap_epmf_visibility_qual_trap_action_trap_id), "");

    return BCM_E_NONE;
}

/**
 * \brief
 *
 *  Main function of the cint.
 *  Creates and sets an Egress User-defined trap.
 *  Creates a TCAM FG which has 1 entry with qualifier visibility and action Trap.
 *
 * \param [in] unit         - The unit number.
 * \param [in] context_id   - The field context id number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_epmf_visibility_qual_trap_action_main(
    int unit,
    bcm_field_context_t context_id)
{


    BCM_IF_ERROR_RETURN_MSG(cint_rx_trap_epmf_visibility_qual_trap_action_trap_create_and_set(unit), "");

    printf("Trap_ID = %d \n", cint_rx_trap_epmf_visibility_qual_trap_action_trap_id);

    BCM_IF_ERROR_RETURN_MSG(cint_rx_trap_epmf_visibility_qual_trap_action_pmf_configuration_create(unit, context_id), "");

    return BCM_E_NONE;
}

