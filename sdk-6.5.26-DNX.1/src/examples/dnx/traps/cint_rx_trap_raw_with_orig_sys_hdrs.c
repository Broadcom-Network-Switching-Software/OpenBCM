/* $Id:
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: cint_rx_trap_raw_with_orig_sys_hdrs.c
 * Purpose: Shows an example for configuration of recycling packet with appended system headers (64B)
 *          and removing the appended system headers on the 2nd pass.
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_raw_with_orig_sys_hdrs.c
 *
 * Main Function:
 *      cint_rx_trap_raw_with_orig_sys_hdrs_main(unit,out_port,ipmf1_context_id);
 * Destroy Function:
 *      cint_rx_trap_raw_with_orig_sys_hdrs_destroy(unit,out_port,ipmf1_context_id);
 *
 * Example Config:
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../../../src/examples/dnx/field/cint_field_always_hit_context.c
 * cint ../../../../src/examples/dnx/traps/cint_utils_rx.c
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_raw_with_orig_sys_hdrs.c
 * cint
 * bcm_field_context_t ipmf1_context_id;
 * cint_field_always_hit_context_main(0,20,bcmFieldStageIngressPMF1,&ipmf1_context_id);
 * cint_rx_trap_raw_with_orig_sys_hdrs_main(0,200,ipmf1_context_id);
 */

bcm_field_group_t cint_in_port_qaul_fg_id = 0;
bcm_field_entry_t cint_ent_id;
bcm_switch_control_info_t default_out_port_value;
int default_first_hdr_size;
uint32 rcy_pmf_profile=2; 
bcm_field_context_t ipmf_ctx_id_2nd;

int cint_field_preselector_create_for_2nd_pass(int unit, int rcy_port, bcm_field_context_t *context_id)
{
    int presel_id = 20;
    
    int rv = BCM_E_NONE;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;

    /*create context*/
    bcm_field_context_info_t_init(&context_info);
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, context_id);
    if(rv != BCM_E_NONE) {
        printf("Error in bcm_field_context_create iPMF context for preset[%d]\n", presel_id);
        return rv;
    }

    bcm_field_context_param_info_t_init(&param_info);
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
    param_info.param_val = bcmFieldSystemHeaderProfileFtmhTshPphUdh;
    rv = bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF1, *context_id, &param_info);
    if(rv != BCM_E_NONE) {
        printf("Error in bcm_field_context_param_set for iPMF1\n");
        return rv;
    }

    bcm_field_context_param_info_t_init(&param_info);
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = 0;
    rv = bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, *context_id, &param_info);
    if(rv != BCM_E_NONE) {
        printf("Error in bcm_field_context_param_set for iPMF2\n");
        return rv;
    }

    /*mapping traffic to context*/
    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = presel_id;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.context_id = *context_id;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.nof_qualifiers = 1;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyPortClassPacketProcessing;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = rcy_pmf_profile;
    presel_entry_data.qual_data[0].qual_mask = 0x7;

    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if(rv != BCM_E_NONE) {
        printf("Error in bcm_field_presel_set\n");
        return rv;
    }
    
    return rv;
}


int cint_field_redirect_to_rcy_port(int unit, int out_port, int rcy_port, bcm_field_context_t epmf_context_id)
{
    int qual_index, action_index;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_group_t fg_id;
    void *dest_char;

    bcm_field_entry_info_t entry_info;
    bcm_field_entry_t entry_handle;

    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 flags;

    int rv = BCM_E_NONE;

    if (*(dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL)) == 0)
    {
        printf("IBCH1 is not support, skip epmf setting!\n");
        return BCM_E_NONE;
    }

    bcm_field_group_info_t_init(fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.nof_quals = 1;
    fg_info.stage = bcmFieldStageEgress;
    fg_info.qual_types[0] = bcmFieldQualifyOutPortTrafficManagement;

    fg_info.nof_actions = 2;
    fg_info.action_types[0] = bcmFieldActionOutPortTrafficManagement;
    /**change pp_dsp for IBCH.ptc*/
    fg_info.action_types[1] = bcmFieldActionPpDsp;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "Raw_Append_64B redirect to Gen Rcy Port", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, 0, &fg_info, &fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_add failed\n");
        return rv;
    }

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for (qual_index = 0; qual_index < fg_info.nof_quals; qual_index++)
    {
        attach_info.key_info.qual_types[qual_index] = fg_info.qual_types[qual_index];
        attach_info.key_info.qual_info[qual_index].input_type = bcmFieldInputTypeMetaData;
    }
    for (action_index = 0; action_index < fg_info.nof_actions; action_index++)
    {
        attach_info.payload_info.action_types[action_index] = fg_info.action_types[action_index];
    }
    rv = bcm_field_group_context_attach(unit, 0, fg_id, epmf_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_attach failed \n");
        return rv;
    }

    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.nof_entry_actions = fg_info.nof_actions;
    for (qual_index = 0; qual_index < fg_info.nof_quals; qual_index++)
    {
        entry_info.entry_qual[qual_index].type = fg_info.qual_types[qual_index];
    }
    for (action_index = 0; action_index < fg_info.nof_actions; action_index++)
    {
        entry_info.entry_action[action_index].type = fg_info.action_types[action_index];
    }

    rv = bcm_port_get(unit, out_port, &flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_port_get failed for out_port\n");
        return rv;
    }

    entry_info.entry_qual[0].value[0] = mapping_info.tm_port;
    entry_info.entry_qual[0].mask[0] = 0xFF;

    rv = bcm_port_get(unit, rcy_port, &flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_port_get failed for out_port\n");
        return rv;
    }
    entry_info.entry_action[0].value[0] = mapping_info.tm_port;
    entry_info.entry_action[1].value[0] = mapping_info.pp_dsp;

    rv = bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    return BCM_E_NONE;
}


/**
* \brief
*  Qualifies on in port and SIP and applies trap action
* \param [in] unit        -  Device id
* \param [in] in_port     -  in pp_port id
* \param [in] ipmf1_context_id - The context used in iPMF1
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_in_port_and_sip_qual_trap_action_create(int unit, int in_port, bcm_field_context_t ipmf1_context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_action_info_t action_info;
    void *dest_char;
    int rv = BCM_E_NONE, trap_id;
    bcm_gport_t trap_gport;
    bcm_rx_trap_config_t trap_config;

    int is_ibch1_support = *(dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL));

    /*
     * Create and attach TCAM group in iPMF1
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = bcmFieldQualifyInPort;
    fg_info.qual_types[1] = bcmFieldQualifySrcIp;
    fg_info.nof_actions = is_ibch1_support ? 2 : 1;
    fg_info.action_types[0] = bcmFieldActionTrap;
    if (is_ibch1_support)
    {
        fg_info.action_types[1] = bcmFieldActionForwardingLayerIndex;
    }

    printf("Creating first group\n");
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "In_port_qual", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, 0, &fg_info, &cint_in_port_qaul_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    /*
     * Attach the FG to context
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    if (is_ibch1_support)
    {
        attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[1].input_arg = 1;
    attach_info.key_info.qual_info[1].offset = 0;

    printf("Attaching field group\n");
    rv = bcm_field_group_context_attach(unit, 0, cint_in_port_qaul_fg_id, ipmf1_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attach FG-%d to ingress CTX-%d\n", cint_in_port_qaul_fg_id, ipmf1_context_id);

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    ent_info.nof_entry_quals = 2;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = in_port;
    ent_info.entry_qual[0].mask[0] = 0x1ff;

    ent_info.entry_qual[1].type = fg_info.qual_types[1];
    /*IPv4 src of the packet, now can be qualified on the second pass 192.128.1.1 */
    ent_info.entry_qual[1].value[0] = 0xc0800101;
    ent_info.entry_qual[1].mask[0] = -1;

    bcm_rx_trap_type_get(unit, 0, bcmRxTrapDfltRedirectToCpuPacket, &trap_id);
    /*
     * 64Bytes in 2nd pass can't terminate in IRPP while should be ITPP when is_ibch1_support
     * HW can't support when (term_size > append_size).
     * Set trap_strength to max value(6) that can term.
     *      1. Termination as Network Header
     *      2. Termination enable when (ITPP_ITPP_GENERAL_CFG.ENABLE_TERMINATION & ITPP_MAPPING_FWD_INFO_TO_EN_TERM)
     *      3. ITPP_MAPPING_FWD_INFO_TO_EN_TERM indexed by {TM_profile(2), fwd-Strength(1)} and enable term up to parsing_offset
     *      4. {TM_profile(2), fwd-Strength(1)} = IPPD_FORWARDING_STRENGTH_PROFILE[Fwd_strength_4_bit(4)]
     */
    if (is_ibch1_support)
    {      
        BCM_GPORT_TRAP_SET(trap_gport,trap_id,15,0);
        
        rv = bcm_rx_trap_get(unit, trap_id, &trap_config);
        if (rv != BCM_E_NONE) 
        {
            printf("Error, in bcm_rx_trap_get \n");
            return rv;
        } 
        trap_config.flags |= BCM_RX_TRAP_UPDATE_MAPPED_STRENGTH;
        trap_config.mapped_trap_strength = 6;
        rv = bcm_rx_trap_set(unit, trap_id, trap_config);
        if (rv != BCM_E_NONE) 
        {
            printf("Error, in bcm_rx_trap_set \n");
            return rv;
        } 
    }
    else
    {
        BCM_GPORT_TRAP_SET(trap_gport,trap_id,15,0);
    }
    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = trap_gport;
    if (is_ibch1_support)
    {
        ent_info.entry_action[1].type = fg_info.action_types[1];
        ent_info.entry_action[1].value[0] = 1;
    }

    printf("Adding Entry\n");
    rv = bcm_field_entry_add(unit, 0, cint_in_port_qaul_fg_id, &ent_info, &cint_ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    return 0;
}

 /**
 * \brief
 *  Main function for configuring packet with appended FTMH original header
 * \par DIRECT INPUT:
 *   \param [in] unit      -  Unit Id
 *   \param [in] out_port  -  Out port
 *   \param [in] ipmf1_context_id - The context used in iPMF1
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int -  Error Type
 * \par INDIRECT OUTPUT:
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_rx_trap_raw_with_orig_sys_hdrs_main(
    int unit,
    int out_port,
    int rcy_port,
    bcm_field_context_t ipmf1_context_id,
    bcm_field_context_t epmf_context_id)
{
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;
    bcm_mirror_port_to_rcy_map_info_t rcy_map_info;
    int rv = BCM_E_NONE;
    int is_ibch1_support = *(dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL));
    int in_port_2nd_pass;

    /** Index 2 means direction OUT */
    key.index = 2;
    key.type = bcmSwitchPortHeaderType;

    /** Retrieve default out port configuration */
    rv = bcm_switch_control_indexed_port_get(unit, out_port, key, &default_out_port_value);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_switch_control_indexed_port_get\n");
        return rv;
    }

    /** Set port header type to RAW_WITH_ORIGINAL_SYSTEM_HEADERS */
    value.value = BCM_SWITCH_PORT_HEADER_TYPE_RAW_WITH_ORIGINAL_SYSTEM_HEADERS;

    rv = bcm_switch_control_indexed_port_set(unit, out_port, key, value);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_switch_control_indexed_port_set\n");
        return rv;
    }

    if (is_ibch1_support)
    {
        in_port_2nd_pass = rcy_port;
        /*
         * Configure pp_profile on rcy port for pmf selection.
         */
        rv = bcm_port_class_set(unit, in_port_2nd_pass, bcmPortClassFieldIngressPMF1PacketProcessingPortCs, rcy_pmf_profile);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_port_class_set for bcmPortClassFieldIngressPMF1PacketProcessingPortCs failed\n");
            return rv;
        }        
    }
    else
    {
        /** Get recycle port configure for the out port (meaning 2nd pass in port) */
        rv = bcm_mirror_port_to_rcy_port_map_get(unit, 0, out_port, &rcy_map_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_mirror_port_to_rcy_port_map_get\n");
            return rv;
        }
        in_port_2nd_pass = rcy_map_info.rcy_mirror_port;
    }

    /** 1st pass: redirect to recycle port*/
    if (is_ibch1_support)
    {
        rv = cint_field_redirect_to_rcy_port(unit, out_port, rcy_port, epmf_context_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_mirror_port_to_rcy_port_map_get\n");
            return rv;
        }
    }

    /** 2nd pass: Create desired PMF rule in this case redirect packet from rcy_port to CPU */
    ipmf_ctx_id_2nd = ipmf1_context_id;
    if (is_ibch1_support)
    {
        /*
         * Append 64B should terminate in ITPP with PMF context para setting. 
         */
        rv = cint_field_preselector_create_for_2nd_pass(unit, in_port_2nd_pass, &ipmf_ctx_id_2nd);
        if (rv != BCM_E_NONE)
        {
            printf("Error in cint_field_preselector_create_for_2nd_pass\n");
            return rv;
        }
    }
    rv = cint_field_in_port_and_sip_qual_trap_action_create(unit, in_port_2nd_pass, ipmf_ctx_id_2nd);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_field_in_port_and_sip_qual_trap_action_create\n");
        return rv;
    }

    return rv;
}

/**
* \brief
*  Destroy all configuration done
* \param [in] unit        -  Device id
* \param [in] ipmf1_context_id - The context used in iPMF1
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_in_port_and_sip_qual_trap_action_destroy(int unit, bcm_field_context_t ipmf1_context_id)
{
    int rv = BCM_E_NONE;

    /* Delete entry from Field group */
    rv = bcm_field_entry_delete(unit, cint_in_port_qaul_fg_id, NULL, cint_ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete entry %d from fg %d\n", rv, cint_ent_id, cint_in_port_qaul_fg_id);
        return rv;
    }
    /* Detach the IPMF1 FG from its context */
    rv = bcm_field_group_context_detach(unit, cint_in_port_qaul_fg_id, ipmf1_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, cint_in_port_qaul_fg_id, ipmf1_context_id);
        return rv;
    }

    /* Delete  FG */
    rv = bcm_field_group_delete(unit, cint_in_port_qaul_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, cint_in_port_qaul_fg_id);
        return rv;
    }

    return rv;
}

/**
* \brief
*  Destroy erpp trap
* \par DIRECT INPUT:
*   \param [in] unit      -  Unit Id
*   \param [in] out_port  -  Out port
*   \param [in] ipmf1_context_id - The context used in iPMF1
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int -  Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int cint_rx_trap_raw_with_orig_sys_hdrs_destroy(
    int unit,
	int out_port,
    bcm_field_context_t ipmf1_context_id)
{
    bcm_switch_control_key_t key;
    bcm_mirror_port_to_rcy_map_info_t rcy_map_info;
    int rv = BCM_E_NONE;

    /** Index 2 means direction OUT */
    key.index = 2;
    key.type = bcmSwitchPortHeaderType;

    rv = bcm_switch_control_indexed_port_set(unit, out_port, key, default_out_port_value);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_switch_control_indexed_port_set\n");
        return rv;
    }

    rv = cint_field_in_port_and_sip_qual_trap_action_destroy(unit, ipmf1_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_field_in_port_and_sip_qual_trap_action_destroy\n");
        return rv;
    }

    return rv;
}
