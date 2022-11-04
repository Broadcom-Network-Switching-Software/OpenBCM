/*
 * Configuration:
 *
 * cint ../../../../src/examples/dnx/field/cint_field_outbound_mirror_save_iqm.c
 *
 * In case of Egress mirroring, the copy of the packet is recycled.
 * In the 2nd pass, the forward copy is dropped and a mirror copy is forwarded to the Egress device.
 * This consumes an extra clock in the IQM for the forward copy and affects device throughput.
 * In order to prevent the packet from being copied and dropped in Ingress, this implementation
 * is responsible for disabling the Ingress mirroring and changing the destination of the recycled packet.
 *
 * The CINT shows how to update destination and outlif only. However, additional attributes
 * of sniff packets can be override, like TC and DP. More attributes can be found on
 * packet_control_updates field of bcm_mirror_destination_t.
 *
 * The implementation can be done in 2 ways: TCAM or FEMs.
 * In this CINT the implementation is done with TCAM. If FEM resources are available,
 * and since the payload of TCAM is limited, then implementation can be done with FEMs.
 *
 * Configuration example start:
 *
 * Test Scenario - start
*cint;
*cint_reset();
*exit;
cint ../../../../src/examples/dnx/field/cint_field_outbound_mirror_save_iqm.c
cint
int unit = 0;
bcm_port_t mirror_port = 202;
bcm_gport_t mirror_gport;
uint32 port_profile = 7;
bcm_field_presel_t presel_id = 1;
bcm_port_class_set(unit, rcy_tm_port, bcmPortClassFieldIngressPMF1TrafficManagementPortCs, port_profile);
cint_field_outbound_mirror_save_iqm_pmf_config_main(unit, port_profile, presel_id);
bcm_mirror_destination_t mirror_dest;
bcm_gport_local_set(&mirror_gport, mirror_port);
mirror_dest.gport = mirror_gport;
bcm_mirror_destination_create(unit, &mirror_dest);
cint_field_outbound_mirror_save_iqm_pmf_mirror_destination_create(unit, mirror_dest.mirror_dest_id);

cint_field_outbound_mirror_save_iqm_mirror_pmf_mirror_destination_destroy(unit, mirror_dest.mirror_dest_id);
bcm_mirror_destination_destroy(unit, mirror_dest.mirror_dest_id);
cint_field_outbound_mirror_save_iqm_pmf_config_destroy(unit, presel_id);
 * Test Scenario - end
 */

/*
 * Defines
 */
/*
 * OUTBOUND_MIRROR_SAVE_IQM_FAI must be 7 for device configuration
 */
int OUTBOUND_MIRROR_SAVE_IQM_FAI = 7;
/*
 * Egress mirroring has only 8 profiles, while the first of them is default (do nothing)
 */
int OUTBOUND_MIRROR_MAX_PROFILES = 8;

 /**
  * Global variables
  */
bcm_field_group_t outbound_mirror_save_iqm_tcam_fg_id = BCM_FIELD_ID_INVALID;
bcm_field_group_t outbound_mirror_save_iqm_const_fg_id = BCM_FIELD_ID_INVALID;
bcm_field_group_t outbound_mirror_save_iqm_const_pst_fg_id = BCM_FIELD_ID_INVALID;
bcm_field_action_t outbound_mirror_save_iqm_action_id = BCM_FIELD_ID_INVALID;
bcm_field_action_t outbound_mirror_save_iqm_pst_action_id = BCM_FIELD_ID_INVALID;
bcm_field_context_t outbound_mirror_save_iqm_context_id_ipmf1 = BCM_FIELD_ID_INVALID;
bcm_field_context_t outbound_mirror_save_iqm_context_id_ipmf3 = BCM_FIELD_ID_INVALID;
bcm_field_stage_t outbound_mirror_save_iqm_stage_ipmf1 = bcmFieldStageIngressPMF1;
bcm_field_stage_t outbound_mirror_save_iqm_stage_ipmf3 = bcmFieldStageIngressPMF3;
bcm_field_entry_t outbound_mirror_save_iqm_entry_ids[OUTBOUND_MIRROR_MAX_PROFILES];
int outbound_mirror_save_iqm_rep_copy_action_support = FALSE;

/**
* \brief
*
* \param [in] unit              -  Device id
* \param [in] presel_id_ipmf1   -  Presel id for IPMF1
* \param [in] presel_id_ipmf3   -  Presel id for IPMF3
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
cint_field_outbound_mirror_save_iqm_pmf_config_main(
    int unit,
    uint32 port_profile,
    bcm_field_presel_t presel_id_ipmf1,
    bcm_field_presel_t presel_id_ipmf3)
{
    /*
     * bcmFieldActionRepCopy is supported only for J2C and above
     * This action is responsible to let the Egress know the source of the packet
     */
    cint_field_outbound_mirror_save_iqm_rep_copy_action_is_supported(unit);

    /*
     * IPMF1 configuration
     * IPMF1 responsible for qualifying upon the recycled packet, disabling its Ingress mirroring, and update its destination to mirror_port.
     */
    BCM_IF_ERROR_RETURN_MSG(cint_field_outbound_mirror_save_iqm_ipmf1_config(unit, port_profile, presel_id_ipmf1), "");

    /*
     * IPMF3 configuration
     * IPMF3 responsible for updating Egress the packet that arrives was mirrored and not forwarded
     */
    if (outbound_mirror_save_iqm_rep_copy_action_support)
    {
        BCM_IF_ERROR_RETURN_MSG(cint_field_outbound_mirror_save_iqm_ipmf3_config(unit, presel_id_ipmf3), "");
    }

    /*
     * IPMF3 configuration
     * This configures the parsing_start_type for the arrived packet
     */
    BCM_IF_ERROR_RETURN_MSG(cint_field_outbound_mirror_save_iqm_ipmf3_config_pst(unit, presel_id_ipmf3), "");

    return BCM_E_NONE;
}

/**
* \brief
*  This function creates TCAM FG and attaches it
* \param [in] unit           - Device id
* \param [in] presel_id      - Presel id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
cint_field_outbound_mirror_save_iqm_ipmf1_config(
    int unit,
    uint32 port_profile,
    bcm_field_presel_t presel_id)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    void *dest_char;
    int action_idx = 0, qual_idx = 0;

    /*
     * Create context in IPMF1 to qualify upon recycle port
     */
    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "EGR_MIRRORED_PACKET_IPMF1", sizeof(context_info.name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_context_create(unit, 0, outbound_mirror_save_iqm_stage_ipmf1, &context_info, &outbound_mirror_save_iqm_context_id_ipmf1), "");
    printf("Created context (%d) in IPMF1 \n", outbound_mirror_save_iqm_context_id_ipmf1);

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    bcm_field_presel_entry_data_info_init(&presel_entry_data);

    presel_entry_id.presel_id = presel_id;
    presel_entry_id.stage = outbound_mirror_save_iqm_stage_ipmf1;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.context_id = outbound_mirror_save_iqm_context_id_ipmf1;
    presel_entry_data.nof_qualifiers = 1;

    /*
     * Qualify on recycle mirror PTC
     */
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyPortClassTrafficManagement;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = port_profile;
    presel_entry_data.qual_data[0].qual_mask = 0x7;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data), "");

    /*
     * Create TCAM field group to disable 2nd pass Ingress mirroring and update the destination
     */
    bcm_field_group_info_t_init(&fg_info);
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "EGR_MIRROED_PACKET_TCAM", sizeof(fg_info.name));
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = outbound_mirror_save_iqm_stage_ipmf1;
    /*
     * Qualifiers
     */
    /*
     * Qualify upon the mirror code to distinguish between the mirror configurations
     */
    fg_info.qual_types[qual_idx++] = bcmFieldQualifyMirrorCode;
    fg_info.nof_quals = qual_idx;
    /*
     * Actions
     */
    /*
     * bcmFieldActionForward to disable 2nd pass Ingress mirroring
     */
    fg_info.action_types[action_idx++] = bcmFieldActionMirrorIngress;
    /*
     * bcmFieldActionForward to update the packet destination
     */
    fg_info.action_types[action_idx++] = bcmFieldActionRedirect;
    /*
     * bcmFieldActionOutVport0 to update the packet outlif
     */
    fg_info.action_types[action_idx++] = bcmFieldActionOutVport0Raw;
    /*
     * bcmFieldActionTrap to disable recycled packet trap
     */
    fg_info.action_types[action_idx++] = bcmFieldActionTrap;
    if(!outbound_mirror_save_iqm_rep_copy_action_support)
    {
        /*
         * bcmFieldActionForwardingAdditionalInfo to let Egress know the packet was mirrored and not forwarded
         */
        fg_info.action_types[action_idx++] = bcmFieldActionForwardingAdditionalInfo;
    }
    fg_info.nof_actions = action_idx;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &fg_info, &outbound_mirror_save_iqm_tcam_fg_id), "");
    printf("Created TCAM field group (%d)  \n", outbound_mirror_save_iqm_tcam_fg_id);

    /*
     * Attach the TCAM field group to the created context
     */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (qual_idx = 0; qual_idx < fg_info.nof_quals; qual_idx++)
    {
        attach_info.key_info.qual_types[qual_idx] = fg_info.qual_types[qual_idx];
        attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeMetaData;
    }
    for (action_idx = 0; action_idx < fg_info.nof_actions; action_idx++)
    {
        attach_info.payload_info.action_types[action_idx] = fg_info.action_types[action_idx];
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, outbound_mirror_save_iqm_tcam_fg_id, outbound_mirror_save_iqm_context_id_ipmf1, &attach_info), "");
    printf("Attached field group (%d) to context (%d)\n", outbound_mirror_save_iqm_tcam_fg_id, outbound_mirror_save_iqm_context_id_ipmf1);

    return BCM_E_NONE;
}

/**
* \brief
*  Create context and const field group, and attach them
*  This configuration handles the TM_action_type from J2C and above
* \param [in] unit           - Device id
* \param [in] presel_id      - Presel id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
cint_field_outbound_mirror_save_iqm_ipmf3_config(
    int unit,
    bcm_field_presel_t presel_id)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_info_t action_info_get_size;
    void *dest_char;
    int action_id;

    /*
     * Create context in IPMF3
     */
    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "EGR_MIRRORED_PACKET_IPMF3", sizeof(context_info.name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_context_create(unit, 0, outbound_mirror_save_iqm_stage_ipmf3, &context_info, &outbound_mirror_save_iqm_context_id_ipmf3), "");
    printf("Created context (%d) in IPMF3\n", outbound_mirror_save_iqm_context_id_ipmf3);

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = presel_id;
    presel_entry_id.stage = outbound_mirror_save_iqm_stage_ipmf3;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.context_id = outbound_mirror_save_iqm_context_id_ipmf3;
    /*
     * IPMF3 context should be cascading from IPMF1 context
     */
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyContextId;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = outbound_mirror_save_iqm_context_id_ipmf1;
    presel_entry_data.qual_data[0].qual_mask = 0x3F;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data), "");

    /*
     * Get bcmFieldActionRepCopy size for the const field group
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_field_action_info_get(unit, bcmFieldActionRepCopy, outbound_mirror_save_iqm_stage_ipmf3, &action_info_get_size), "");

    /*
    * Create a user define action with size 0, value is fixed (Mirror)
    */
    bcm_field_action_info_t_init(&action_info);
    /*
     * bcmFieldActionRepCopy is responsible to let Egress know the source of the packet
     */
    action_info.action_type = bcmFieldActionRepCopy;
    action_info.prefix_size = action_info_get_size.size;
    /*
     * Packet was mirrored and not forwarded
     */
    action_info.prefix_value = bcmFieldRepCopyTypeMirror;
    action_info.size = 0;
    action_info.stage = outbound_mirror_save_iqm_stage_ipmf3;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_action_create(unit, 0, &action_info, &outbound_mirror_save_iqm_action_id), "");
    action_id = outbound_mirror_save_iqm_action_id;
    printf("Created user defined action (%d)\n", action_id);

    /*
     * Create const field group with the created action
     */
    bcm_field_group_info_t_init(&fg_info);
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "EGR_MIRROED_PACKET_CONST_FG", sizeof(fg_info.name));
    fg_info.fg_type = bcmFieldGroupTypeConst;
    fg_info.stage = outbound_mirror_save_iqm_stage_ipmf3;
    fg_info.nof_quals = 0;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = outbound_mirror_save_iqm_action_id;
    fg_info.action_with_valid_bit[0] = FALSE;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &fg_info, &outbound_mirror_save_iqm_const_fg_id), "");
    printf("Created const field group \n", outbound_mirror_save_iqm_const_fg_id);

    /*
     * Attach the const field group to the IPMF3 context
     */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, outbound_mirror_save_iqm_const_fg_id, outbound_mirror_save_iqm_context_id_ipmf3, &attach_info), "");
    printf("Attached field group (%d) to context (%d)\n", outbound_mirror_save_iqm_const_fg_id, outbound_mirror_save_iqm_context_id_ipmf3);

    return BCM_E_NONE;
}

/**
* \brief
*  Create context and const field group, and attach them
*  This configures parsing_start_type for the mirroring packet
* \param [in] unit           - Device id
* \param [in] presel_id      - Presel id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
cint_field_outbound_mirror_save_iqm_ipmf3_config_pst(
    int unit,
    bcm_field_presel_t presel_id)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_info_t action_info_get_size;
    void *dest_char;
    int action_id;
    uint32 bcm_value[BCM_FIELD_ACTION_WIDTH_IN_WORDS];
    uint32 hw_value[BCM_FIELD_ACTION_WIDTH_IN_WORDS];

    if (outbound_mirror_save_iqm_context_id_ipmf3 == BCM_FIELD_ID_INVALID) {
        /*
         * Create context in IPMF3 if it hasn't been created yet
         */
        bcm_field_context_info_t_init(&context_info);
        dest_char = &(context_info.name[0]);
        sal_strncpy_s(dest_char, "EGR_MIRRORED_PACKET_IPMF3", sizeof(context_info.name));
        BCM_IF_ERROR_RETURN_MSG(bcm_field_context_create(unit, 0, outbound_mirror_save_iqm_stage_ipmf3, &context_info, &outbound_mirror_save_iqm_context_id_ipmf3), "");
        printf("Created context (%d) in IPMF3\n", outbound_mirror_save_iqm_context_id_ipmf3);

        bcm_field_presel_entry_id_info_init(&presel_entry_id);
        presel_entry_id.presel_id = presel_id;
        presel_entry_id.stage = outbound_mirror_save_iqm_stage_ipmf3;

        bcm_field_presel_entry_data_info_init(&presel_entry_data);
        presel_entry_data.entry_valid = TRUE;
        presel_entry_data.context_id = outbound_mirror_save_iqm_context_id_ipmf3;
        /*
         * IPMF3 context should be cascading from IPMF1 context
         */
        presel_entry_data.nof_qualifiers = 1;
        presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyContextId;
        presel_entry_data.qual_data[0].qual_arg = 0;
        presel_entry_data.qual_data[0].qual_value = outbound_mirror_save_iqm_context_id_ipmf1;
        presel_entry_data.qual_data[0].qual_mask = 0x3F;
        BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data), "");
    }

    /*
     * Get bcmFieldActionParsingStartType size for the const field group
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_field_action_info_get(unit, bcmFieldActionParsingStartType, outbound_mirror_save_iqm_stage_ipmf3, &action_info_get_size), "");

    /*
    * Create a user define action with size 0, value is fixed (Mirror)
    */
    bcm_field_action_info_t_init(&action_info);
    /*
     * bcmFieldActionParsingStartType is responsible to let Egress know the source of the packet
     */
    action_info.action_type = bcmFieldActionParsingStartType;
    action_info.prefix_size = action_info_get_size.size;
    /*
     * if rcy with 64B appended, packet should parsed as ETH to support 64B remove and MPLS.bos set.
     * Otherwise packet was mirrored and not forwarded
     */
    if (*(dnxc_data_get(unit, "port_pp", "general", "rcy_info_prt_qualifier_eth_over_append64_support", NULL)) != 0)
    {
        bcm_value[0] = bcmFieldLayerTypeEth;
    }
    else
    {
        bcm_value[0] = bcmFieldLayerTypeMirrorOrSs;
    }
    bcm_field_action_value_map(unit, outbound_mirror_save_iqm_stage_ipmf3, bcmFieldActionParsingStartType, bcm_value, hw_value);
    action_info.prefix_value = hw_value[0];
    action_info.size = 0;
    action_info.stage = outbound_mirror_save_iqm_stage_ipmf3;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_action_create(unit, 0, &action_info, &outbound_mirror_save_iqm_pst_action_id), "");
    action_id = outbound_mirror_save_iqm_pst_action_id;
    printf("Created user defined action (%d)\n", action_id);

    /*
     * Create const field group with the created action
     */
    bcm_field_group_info_t_init(&fg_info);
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "EGR_MIRROED_PACKET_CONST_PST", sizeof(fg_info.name));
    fg_info.fg_type = bcmFieldGroupTypeConst;
    fg_info.stage = outbound_mirror_save_iqm_stage_ipmf3;
    fg_info.nof_quals = 0;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = outbound_mirror_save_iqm_pst_action_id;
    fg_info.action_with_valid_bit[0] = FALSE;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &fg_info, &outbound_mirror_save_iqm_const_pst_fg_id), "");
    printf("Created const field group \n", outbound_mirror_save_iqm_const_pst_fg_id);

    /*
     * Attach the const field group to the IPMF3 context
     */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, outbound_mirror_save_iqm_const_pst_fg_id, outbound_mirror_save_iqm_context_id_ipmf3, &attach_info), "");
    printf("Attached field group (%d) to context (%d)\n", outbound_mirror_save_iqm_const_pst_fg_id, outbound_mirror_save_iqm_context_id_ipmf3);

    return BCM_E_NONE;
}

/**
* \brief
*  Add entry according to the configured mirror profile
* \param [in] unit            - Device id
* \param [in] mirror_dest_id  - Mirror profile
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
cint_field_outbound_mirror_save_iqm_pmf_mirror_destination_create(
    int unit,
    bcm_gport_t mirror_dest_id)
{
    bcm_field_entry_info_t entry_info;
    bcm_mirror_destination_t mirror_dest;
    bcm_gport_t mirror_disable;
    int mirror_id;
    bcm_gport_t trap_gport;
    int default_trap_id;
    int trap_strength;
    int action_idx = 0, qual_idx = 0;

    /*
     * Get mirror destination attributes
     */
    bcm_mirror_destination_t_init(&mirror_dest);
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_destination_get(unit, mirror_dest_id, &mirror_dest), "");

    mirror_id = BCM_GPORT_MIRROR_GET(mirror_dest_id);
    if (mirror_id < 0 || mirror_id >= OUTBOUND_MIRROR_MAX_PROFILES)
    {
        printf("Mirror profile is out of range");
        return BCM_E_PARAM;
    }

    bcm_field_entry_info_t_init(&entry_info);
    /*
     * Qualifiers
     */
    /*
     * Qualify upon mirror profile
     */
    entry_info.entry_qual[qual_idx].type = bcmFieldQualifyMirrorCode;
    entry_info.entry_qual[qual_idx].value[0] = mirror_id;
    entry_info.entry_qual[qual_idx].mask[0] = 0x7;
    qual_idx++;
    entry_info.nof_entry_quals = qual_idx;
    /*
     * Actions
     */
    /*
     * Disable 2nd pass Ingress mirroring
     */
    entry_info.entry_action[action_idx].type = bcmFieldActionMirrorIngress;
    BCM_GPORT_MIRROR_SET(mirror_disable, BCM_MIRROR_DISABLE);
    entry_info.entry_action[action_idx].value[0] = mirror_disable;
    action_idx++;
    /*
     * Update packet destination
     */
    entry_info.entry_action[action_idx].type = bcmFieldActionRedirect;
    entry_info.entry_action[action_idx].value[0] = mirror_dest.gport;
    action_idx++;
    /*
     * Disable recycled packet drop trap
     */
    bcm_rx_trap_type_get(unit, 0, bcmRxTrapDefault, &default_trap_id);
    trap_strength = 0xf;
    
    BCM_GPORT_TRAP_SET(trap_gport, default_trap_id, trap_strength, 0);
    entry_info.entry_action[action_idx].type = bcmFieldActionTrap;
    entry_info.entry_action[action_idx].value[0] = trap_gport;
    action_idx++;
    /*
     * Update outlif in case mirror destination encap_id is valid
     */
    if (mirror_dest.encap_id != 0 && (mirror_dest.flags & BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID))
    {
        entry_info.entry_action[action_idx].type = bcmFieldActionOutVport0Raw;
        entry_info.entry_action[action_idx].value[0] = mirror_dest.encap_id;
        action_idx++;
    }
    entry_info.nof_entry_actions = action_idx;
    /*
     * Update FAI in case RepCopy action isn't supported
     */
    if (!outbound_mirror_save_iqm_rep_copy_action_support)
    {
        entry_info.entry_action[entry_info.nof_entry_actions].type = bcmFieldActionForwardingAdditionalInfo;
        entry_info.entry_action[entry_info.nof_entry_actions].value[0] = OUTBOUND_MIRROR_SAVE_IQM_FAI;
        entry_info.nof_entry_actions++;
    }

    /*
     * Add the entry to the created field group
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, outbound_mirror_save_iqm_tcam_fg_id, &entry_info, &outbound_mirror_save_iqm_entry_ids[mirror_id]), "");
    printf("Entry 0x%X (%d) was added to field group %d. \n", outbound_mirror_save_iqm_entry_ids[mirror_id], outbound_mirror_save_iqm_entry_ids[mirror_id], outbound_mirror_save_iqm_tcam_fg_id);

    return BCM_E_NONE;
}

/**
 * \brief
 *  Destroy an allocated mirror profile and delete the relative entry
 *
 * \param [in]  unit            - Device id
 * \param [in]  mirror_dest_id -  Mirror profile ID
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_outbound_mirror_save_iqm_mirror_pmf_mirror_destination_destroy(
    int unit,
    bcm_gport_t mirror_dest_id)
{
    int mirror_id = BCM_GPORT_MIRROR_GET(mirror_dest_id);
    if (mirror_id < 0 || mirror_id >= OUTBOUND_MIRROR_MAX_PROFILES)
    {
        printf("Mirror profile is out of range");
        return BCM_E_PARAM;
    }

    /*
    * Delete the entry from the TCAM field group
    */
    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, outbound_mirror_save_iqm_tcam_fg_id, NULL, outbound_mirror_save_iqm_entry_ids[mirror_id]), "");
    printf("Entry 0x%X (%d) was deleted from field group %d. \n", outbound_mirror_save_iqm_entry_ids[mirror_id], outbound_mirror_save_iqm_entry_ids[mirror_id], outbound_mirror_save_iqm_tcam_fg_id);

    return BCM_E_NONE;
}

/**
* \brief
*  Checks if action RepCopy is enabled for current device
*  Action is not supported on JR2 only.
* \param [in] unit           - Device ID.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_outbound_mirror_save_iqm_rep_copy_action_is_supported(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_action_info_t action_info;
    bcm_field_action_info_t_init(&action_info);
    rv = bcm_field_action_info_get(unit, bcmFieldActionRepCopy, bcmFieldStageIngressPMF3, &action_info);
    outbound_mirror_save_iqm_rep_copy_action_support = (rv == BCM_E_NONE);
    return BCM_E_NONE;
 }

/**
 * \brief
 *  Destroy all allocated data by the configuration
 *
 * \param [in] unit              - Device id
 * \param [in] presel_id_ipmf1   -  Presel id for IPMF1
 * \param [in] presel_id_ipmf3   -  Presel id for IPMF3
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_outbound_mirror_save_iqm_pmf_config_destroy(
    int unit,
    bcm_field_presel_t presel_id_ipmf1,
    bcm_field_presel_t presel_id_ipmf3)
{
    BCM_IF_ERROR_RETURN_MSG(cint_field_outbound_mirror_save_iqm_ipmf3_config_destroy(unit, presel_id_ipmf3), "");
    BCM_IF_ERROR_RETURN_MSG(cint_field_outbound_mirror_save_iqm_ipmf1_config_destroy(unit, presel_id_ipmf1), "");

    return BCM_E_NONE;
}

/**
 * \brief
 *  Destroy all allocated data by the configuration in IPMF1
 *
 * \param [in] unit            - Device id
 * \param [in] presel_id      - Presel id
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_outbound_mirror_save_iqm_ipmf1_config_destroy(
    int unit,
    bcm_field_presel_t presel_id)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t presel_entry_id;

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    bcm_field_presel_entry_data_info_init(&presel_entry_data);

    presel_entry_id.presel_id = presel_id;
    presel_entry_id.stage = outbound_mirror_save_iqm_stage_ipmf1;
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = outbound_mirror_save_iqm_context_id_ipmf1;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data), "");

    /*
     * Detach the TCAM field group from its context
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, outbound_mirror_save_iqm_tcam_fg_id, outbound_mirror_save_iqm_context_id_ipmf1), "");
    printf("TCAM Field Group %d was detach from context %d. \n", outbound_mirror_save_iqm_tcam_fg_id, outbound_mirror_save_iqm_context_id_ipmf1);

    /*
     * Delete the TCAM field group
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, outbound_mirror_save_iqm_tcam_fg_id), "");
    printf("TCAM Field Group %d was deleted. \n", outbound_mirror_save_iqm_tcam_fg_id);

    /*
     * Delete the context from ipmf1
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_field_context_destroy(unit, outbound_mirror_save_iqm_stage_ipmf1, outbound_mirror_save_iqm_context_id_ipmf1), "");
    printf("Context %d was deleted from IPMF1. \n", outbound_mirror_save_iqm_context_id_ipmf1);
    return BCM_E_NONE;
}

/**
 * \brief
 *  Destroy all allocated data by the configuration in IPMF3
 *
 * \param [in] unit           - Device id
 * \param [in] presel_id      - Presel id
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_outbound_mirror_save_iqm_ipmf3_config_destroy(
    int unit,
    bcm_field_presel_t presel_id)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t presel_entry_id;

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    bcm_field_presel_entry_data_info_init(&presel_entry_data);

    presel_entry_id.presel_id = presel_id;
    presel_entry_id.stage = outbound_mirror_save_iqm_stage_ipmf3;
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = outbound_mirror_save_iqm_context_id_ipmf3;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data), "");

    if (outbound_mirror_save_iqm_const_fg_id != BCM_FIELD_ID_INVALID) {
        /*
        * Detach the const field group from its context
        */
        BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, outbound_mirror_save_iqm_const_fg_id, outbound_mirror_save_iqm_context_id_ipmf3), "");
        printf("Const Field Group %d was detach from context %d. \n", outbound_mirror_save_iqm_const_fg_id, outbound_mirror_save_iqm_context_id_ipmf3);

        /*
        * Delete the const field group
        */
        BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, outbound_mirror_save_iqm_const_fg_id), "");
        printf("Const Field Group %d was deleted. \n", outbound_mirror_save_iqm_const_fg_id);
    }

    /*
    * Detach the const field group from its context
    */
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, outbound_mirror_save_iqm_const_pst_fg_id, outbound_mirror_save_iqm_context_id_ipmf3), "");
    printf("Const Field Group %d was detach from context %d. \n", outbound_mirror_save_iqm_const_pst_fg_id, outbound_mirror_save_iqm_context_id_ipmf3);

    /*
    * Delete the const field group
    */
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, outbound_mirror_save_iqm_const_pst_fg_id), "");
    printf("Const Field Group %d was deleted. \n", outbound_mirror_save_iqm_const_pst_fg_id);

    /*
     * Delete the context from ipmf3
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_field_context_destroy(unit, outbound_mirror_save_iqm_stage_ipmf3, outbound_mirror_save_iqm_context_id_ipmf3), "");
    printf("Context %d was deleted from IPMF3. \n", outbound_mirror_save_iqm_context_id_ipmf3);

    if (outbound_mirror_save_iqm_action_id != BCM_FIELD_ID_INVALID) {
        BCM_IF_ERROR_RETURN_MSG(bcm_field_action_destroy(unit, outbound_mirror_save_iqm_action_id), "");
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_field_action_destroy(unit, outbound_mirror_save_iqm_pst_action_id), "");

    return BCM_E_NONE;
}
