/* $Id: cint_rx_trap_fhei_config.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: cint_rx_trap_fhei_config.c
 * Purpose: Creating Trap and setting FHEI stamping.
 *          Creating PMF rule with Action TRAP-Code and TRAP-Qual.
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_fhei_config.c
 *
 * Main Function:
 *      cint_rx_trap_fhei_main(unit, port, trap_id, context_id);
 * Destroy Function:
 *      cint_rx_trap_fhei_destroy(unit, context_id);
 *
 * Example Config:
 * Test Scenario - start
  cint ../../../../src/examples/dnx/traps/cint_utils_rx.c
  cint ../../../../src/examples/dnx/traps/cint_rx_trap_fhei_config.c
  cint
  cint_rx_trap_fhei_main(0,201,26,4);
 * Test Scenario - end
 *
 */

bcm_field_group_t cint_rx_trap_fhei_fg_id;
bcm_field_entry_t cint_rx_trap_fhei_entry_id;
int cint_rx_trap_fhei_trap_id;


/**
 * \brief
 *
 *  Creates an ERPP UD trap, which is attached to IPv4Ttl1 application trap.
 *  All valid ERPP actions are set for this trap.
 *
 * \param [in] unit         - The unit number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_ingress_pmf_trap_action_trap_create_and_set(
    int unit)
{
    int flags;
    int fwd_strength = 7;
    bcm_gport_t gport;
    bcm_rx_trap_config_t trap_config;
    bcm_port_config_t port_config;
    bcm_gport_t cpu_port;
    int count;
    int trap_id;

    BCM_IF_ERROR_RETURN_MSG(bcm_port_config_get(unit, &port_config), "");

    bcm_rx_trap_config_t_init(&trap_config);

    if(cint_rx_trap_fhei_trap_id > 0)
    {
        flags = BCM_RX_TRAP_WITH_ID;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, flags, bcmRxTrapUserDefine, &cint_rx_trap_fhei_trap_id), "");

    trap_config.flags = BCM_RX_TRAP_TRAP | BCM_RX_TRAP_UPDATE_DEST;
    BCM_PBMP_ITER(port_config.cpu, cpu_port)
    {
        trap_config.dest_port = cpu_port;
        break;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, cint_rx_trap_fhei_trap_id, &trap_config), "");

    return BCM_E_NONE;
}

/**
 * \brief
 *
 *  Creates a TCAM FG and attaches it to the default context.
 *  The FG has 1 entry with qualifier SrcPort and action Trap.
 *
 * \param [in] unit        - The unit number.
 * \param [in] port        - Src Port.
 * \param [in] trap_id     - Trap_ID to be stamped.
 * \param [in] context_id     - IPMF1 context.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_fhei_main(
    int unit,
    int port,
    int trap_id,
    bcm_field_context_t context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    bcm_gport_t trap_gport, gport;

    cint_rx_trap_fhei_trap_id = trap_id;

    BCM_IF_ERROR_RETURN_MSG(cint_ingress_pmf_trap_action_trap_create_and_set(unit), "");

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_entry_info_t_init(&entry_info);

    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_actions = 1;
    fg_info.nof_quals = 1;
    fg_info.action_with_valid_bit[0] = TRUE;
    fg_info.action_types[0] = bcmFieldActionTrap;
    fg_info.qual_types[0] = bcmFieldQualifySrcPort;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, fg_info, &cint_rx_trap_fhei_fg_id), "");

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, cint_rx_trap_fhei_fg_id, context_id ,&attach_info), "");

    BCM_GPORT_TRAP_SET(trap_gport, cint_rx_trap_fhei_trap_id, 15, 0);

    BCM_GPORT_SYSTEM_PORT_ID_SET(gport,port);
    entry_info.priority = 10;
    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.nof_entry_quals = fg_info.nof_quals;

    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = trap_gport;
    entry_info.entry_action[0].value[1] = 0xAA;

    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = gport;
    entry_info.entry_qual[0].mask[0] = 0xFFFFF;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, cint_rx_trap_fhei_fg_id, &entry_info, &cint_rx_trap_fhei_entry_id), "");

    return BCM_E_NONE;
}


/**
 * \brief
 *
 *  Destroying the full PMF and Trap config created in the CINT.
 *
 * \param [in] unit          - The unit number.
 * \param [in] context_id    - Context ID.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_fhei_destroy(
    int unit, 
    bcm_field_context_t context_id)
{
    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, cint_rx_trap_fhei_fg_id, NULL, cint_rx_trap_fhei_entry_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, cint_rx_trap_fhei_fg_id, context_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, cint_rx_trap_fhei_fg_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_destroy(unit, cint_rx_trap_fhei_trap_id), "");

    return BCM_E_NONE;
}
