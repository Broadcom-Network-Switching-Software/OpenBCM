/* $Id: cint_rx_trap_erpp_epmf_trap_action.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_rx_trap_erpp_epmf_trap_action.c
 * Purpose: Testing EGRESS_PMF SW Trap and all ERPP Trap actions.
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_erpp_epmf_trap_action.c
 *
 * Main Function:
 *      cint_rx_trap_erpp_epmf_trap_action_main(unit,tc,dp,dest_p,qos_map_id,snoop_profile,mirror_profile,cud,counter_0_profile,
 *                                                               counter_1_profile,counter_0_ptr,counter_1_ptr);
 * Destroy Function:
 *      cint_rx_trap_erpp_epmf_trap_action_destroy(unit);
 *
 *
 * Example Config:
 * cint ../../../../src/examples/dnx/traps/cint_utils_rx.c
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_erpp_epmf_trap_action.c
 * cint
 * cint_rx_trap_erpp_epmf_trap_action_main(0,2,1,201,5,3,2,0,0,0,0,0);
 */

bcm_field_group_t cint_rx_trap_erpp_epmf_trap_action_fg_id;
bcm_field_entry_t cint_rx_trap_erpp_epmf_trap_action_entry_id;
int cint_rx_trap_erpp_epmf_trap_action_trap_id;

/**
 * \brief
 *
 *  Creates an ERPP UD trap, which is attached to IPv4Ttl1 application trap.
 *  All valid ERPP actions are set for this trap.
 *
 * \param [in] unit         - The unit number.
 * \param [in] tc           - Traffic Class.
 * \param [in] dp           - Drop Precedence.
 * \param [in] dest_p       - Dest Port.
 * \param [in] qos_map_id   - QOS ID
 * \param [in] snoop_profile          - snoop_profile.
 * \param [in] mirror_profile          - mirror_profile.
 * \param [in] cud                      - Copy-Unique-Data.
 * \param [in] counter_0_profile    - conter #0 profile
 * \param [in] counter_1_profile    - conter #1 profile
 * \param [in] counter_0_ptr        - conter #0 pointer
 * \param [in] counter_1_ptr        - conter #1 pointer
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_erpp_pmf_trap_action_trap_create_and_set(
    int unit,
    int tc,
    int dp,
    int dest_p,
    int qos_map_id,
    int pp_drop_reason,
    int snoop_profile,
    int mirror_profile,
    int cud,
    int counter_0_profile,
    int counter_1_profile,
    int counter_0_ptr,
    int counter_1_ptr)
{
    int rv;
    bcm_rx_trap_config_t trap_config;

    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgUserDefine, &cint_rx_trap_erpp_epmf_trap_action_trap_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_type_create \n");
        return rv;
    }

    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_UPDATE_PRIO
                        | BCM_RX_TRAP_UPDATE_COLOR | BCM_RX_TRAP_UPDATE_QOS_MAP_ID | BCM_RX_TRAP_UPDATE_ENCAP_ID
                        |BCM_RX_TRAP_UPDATE_COUNTER | BCM_RX_TRAP_UPDATE_COUNTER_2 ;
    trap_config.prio = tc;
    trap_config.color = dp;
    trap_config.dest_port = dest_p;
    trap_config.qos_map_id = qos_map_id;
    trap_config.encap_id = cud;
    trap_config.pp_drop_reason = pp_drop_reason;
    trap_config.snoop_cmnd = snoop_profile;
    trap_config.mirror_profile = mirror_profile;
    trap_config.stat_obj_config_arr[0].stat_profile = counter_0_profile;
    trap_config.stat_obj_config_arr[0].stat_id = counter_0_ptr;
    trap_config.stat_obj_config_arr[1].stat_profile = counter_1_profile;
    trap_config.stat_obj_config_arr[1].stat_id = counter_1_ptr;
    rv = bcm_rx_trap_set(unit, cint_rx_trap_erpp_epmf_trap_action_trap_id, &trap_config);
    if(rv != BCM_E_NONE)
    {
       printf("Error in bcm_rx_trap_set \n");
       return rv;
    }

    return rv;
}

/**
 * \brief
 *
 *  Creates a TCAM FG and attaches it to the default context.
 *  The FG has 1 entry with qualifier VRF and action Trap.
 *
 * \param [in] unit         - The unit number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_erpp_pmf_trap_action_pmf_configuration_create(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    bcm_gport_t trap_gport;

    int rv = BCM_E_NONE;

    bcm_field_group_info_t_init(&fg_info);

    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageEgress;
    fg_info.nof_actions = 1;
    fg_info.nof_quals = 1;
    fg_info.action_with_valid_bit[0] = TRUE;
    fg_info.action_types[0] = bcmFieldActionTrap;
    fg_info.qual_types[0] = bcmFieldQualifyVrf;

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, fg_info, &cint_rx_trap_erpp_epmf_trap_action_fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_add \n");
        return rv;
    }

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    rv = bcm_field_group_context_attach(unit, 0, cint_rx_trap_erpp_epmf_trap_action_fg_id, BCM_FIELD_CONTEXT_ID_DEFAULT ,&attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_attach \n");
        return rv;
    }

    bcm_field_entry_info_t_init(&entry_info);

    entry_info.priority = 10;
    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.nof_entry_quals = fg_info.nof_quals;

    BCM_GPORT_TRAP_SET(trap_gport,cint_rx_trap_erpp_epmf_trap_action_trap_id,15,0);
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = trap_gport;

    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = 0x64;
    entry_info.entry_qual[0].mask[0] = 0xFF;

    rv = bcm_field_entry_add(unit, 0, cint_rx_trap_erpp_epmf_trap_action_fg_id, &entry_info, &cint_rx_trap_erpp_epmf_trap_action_entry_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_entry_add \n");
        return rv;
    }

    return rv;
}

/**
 * \brief
 *
 *  Destroying the full PMF and Trap config created in the CINT.
 *
 * \param [in] unit         - The unit number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_erpp_epmf_trap_action_destroy(
    int unit)
{
    int rv = BCM_E_NONE;

    rv = bcm_field_entry_delete(unit, cint_rx_trap_erpp_epmf_trap_action_fg_id, NULL, cint_rx_trap_erpp_epmf_trap_action_entry_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_entry_delete \n");
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, cint_rx_trap_erpp_epmf_trap_action_fg_id, BCM_FIELD_CONTEXT_ID_DEFAULT);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach \n");
        return rv;
    }

    rv = bcm_field_group_delete(unit, cint_rx_trap_erpp_epmf_trap_action_fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_delete \n");
        return rv;
    }

    rv = bcm_rx_trap_action_profile_clear(unit, bcmRxTrapEgIpv4Ttl1);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_action_profile_clear \n");
        return rv;
    }

    rv = bcm_rx_trap_type_destroy(unit, cint_rx_trap_erpp_epmf_trap_action_trap_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_type_destroy \n");
        return rv;
    }

    return rv;
}

/**
 * \brief
 *
 *  Main function of the cint.
 *  Creates an ERPP UD trap, which is attached to IPv4Ttl1 application trap.
 *  Creates a TCAM FG which has 1 entry with qualifier VRF and action Trap.
 *
 * \param [in] unit         - The unit number.
 * \param [in] tc           - Traffic Class.
 * \param [in] dp           - Drop Precedence.
 * \param [in] dest_p       - Dest Port.
 * \param [in] qos_map_id   - QOS ID
 * \param [in] snoop_profile          - snoop_profile.
 * \param [in] mirror_profile          - mirror_profile.
 * \param [in] cud                      - Copy-Unique-Data.
 * \param [in] counter_0_profile    - conter #0 profile
 * \param [in] counter_1_profile    - conter #1 profile
 * \param [in] counter_0_ptr        - conter #0 pointer
 * \param [in] counter_1_ptr        - conter #1 pointer

 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_erpp_epmf_trap_action_main(
    int unit,
    int tc,
    int dp,
    int dest_p,
    int qos_map_id,
    int pp_drop_reason,
    int snoop_profile,
    int mirror_profile,
    int cud,
    int counter_0_profile,
    int counter_1_profile,
    int counter_0_ptr,
    int counter_1_ptr)
{

    int rv = BCM_E_NONE;

    rv = cint_erpp_pmf_trap_action_trap_create_and_set(unit, tc, dp, dest_p, qos_map_id, pp_drop_reason, snoop_profile, mirror_profile, cud, counter_0_profile, counter_1_profile, counter_0_ptr, counter_1_ptr);
    if(rv != BCM_E_NONE)
    {
      printf("Error in egress_pmf_trap_create_and_set \n");
      return rv;
    }

    printf("Trap_ID = %d \n", cint_rx_trap_erpp_epmf_trap_action_trap_id);

    rv = cint_erpp_pmf_trap_action_pmf_configuration_create(unit);
    if(rv != BCM_E_NONE)
    {
      printf("Error in egress_pmf_trap_create_and_set \n");
      return rv;
    }

    return rv;
}
