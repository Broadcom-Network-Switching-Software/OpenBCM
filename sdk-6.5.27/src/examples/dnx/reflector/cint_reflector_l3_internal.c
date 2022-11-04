/**
 * \file cint_reflector_l3_internal.c
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * Purpose: Example for L3 internal application: unicast
 *
 * Unicast sequence:
 *
 * Add recycle port with PTCH2 using the following soc properties:
  "
  ucode_port_211.BCM8869X=RCY0.0:core_0.211
  "
  "
 * Test Scenario - start
  cint ../../../../src/examples/dnx/reflector/cint_reflector_l3_internal.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_util_rch.c
  cint
  int unit = 0;
  int rcy_tm_port = 211;
  int in_logical_port = 13;
  int sip = 0x01020304;
  int is_udp_swap = 0;
  print cint_reflector_l3_internal_uc_create(unit, sip, rcy_tm_port, is_udp_swap);
  exit;
 * Test Scenario - end
  "
 */
bcm_field_group_t fg_id_for_l3_internal_reflector =0;

/*
 * On epmf to indicate 
 *      1: support ACE table. 
 *      0: no more indirect ACE table.
 */
int is_epmf_ace_support=1;

/*
 * Indicate to create pmf ctx in cint.
 */
int is_create_ctx=1;

/*
 * context when not "always hit"
 */
bcm_field_context_t l3_int_reflector_created_context;
/*
 * is_l3_reflector_encap_access_optional used to select reflector outlif encap access
 *      0: default, phase2
 *      1: optional, phase3
 */
int is_l3_reflector_encap_access_optional=0;

/**configure ports when recycle header is ptch1_plus over IBCH1*/
int cint_reflector_l3_internal_config_port_for_ibch1_rch(
    int unit,
    int rcy_port,
    int egress_port)
{
    int is_ibch1_enable;
    uint32 ibch1_supported = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL);

    if (*(dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL)) == 0)
    {
        printf("IBCH1 is not support, skip rcy_port-%d, egress_port-%d setting!\n", rcy_port, egress_port);
        return BCM_E_NONE;
    }
    
    /**enable IBCH1 on rcy, default is enabled*/
    BCM_IF_ERROR_RETURN_MSG(bcm_port_control_get(unit, rcy_port, bcmPortControlIbch1Enable, &is_ibch1_enable),
        "for bcmPortControlIbch1Enable");
    if (!is_ibch1_enable)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, rcy_port, bcmPortControlIbch1Enable, 1),
            "for bcmPortControlIbch1Enable");
    }
    if (!ibch1_supported)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, rcy_port, bcmPortControlOverlayRecycle, 1),
            "for bcmPortControlOverlayRecycle");
    }
    return BCM_E_NONE;
}


/**
 * \brief - Example of field processor program for L3 external unicast reflector
 *    Stage: Egress PMF
 *    Context:
 *        - Attach program to context to given context
 *    Keys:
 *        - Packet is identified by a user specific ACL (SIP as an example)
 *    Actions:
 *        - Override PP-DSP to be recycle TM port (rcy_tm_port)
 *        - Override OUTLIF0 to be reflector OUTLIF (reflector_global_outlif)
 *        - Override OUTLIF1 to be recycle OUTLIF (rcy_global_outlif)
 *        - Override ACE context value if UDP swap is required
 *
 * \param [in] unit -  Unit-ID
 * \param [in] context_id -  field processor context-id to attach the program to in Egress PMF
 * \param [in] reflector_global_outlif -  reflector global outlif
 * \param [in] rcy_global_outlif_gport -  recycle global outlif
 * \param [out] fg_id -  field group id of the relevant program
 * \param [out] fg_info -  field group info of the relevant program
 * \param [out] ace_entry_handle - ace action handle created for this program
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_reflector_l3_internal_fp_create(
    int unit,
    bcm_field_context_t context_id,
    bcm_if_t reflector_global_outlif,
    bcm_if_t rcy_global_outlif_gport,
    int is_udp_swap,
    bcm_field_group_t * fg_id,
    bcm_field_group_info_t * fg_info,
    uint32 *ace_entry_handle)
{
    bcm_field_group_attach_info_t attach_info;
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_format_t ace_format_id;
    int qual_index, action_index;
    void *dest_char;
    int trap_offset;
    int value;
    bcm_field_entry_info_t epmf_entry_info;

    is_epmf_ace_support = *(dnxc_data_get(unit, "field", "ace", "supported", NULL));

    if (is_epmf_ace_support) 
    {
        /** define ace action */
        bcm_field_ace_format_info_t_init(&ace_format_info);
        ace_format_info.nof_actions = 3;
        ace_format_info.action_types[0] = bcmFieldActionOutInterface0;
        ace_format_info.action_types[1] = bcmFieldActionOutInterface1;
        ace_format_info.action_types[2] = bcmFieldActionAceContextValue;
        BCM_IF_ERROR_RETURN_MSG(bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id), "");
        bcm_field_ace_entry_info_t_init(&ace_entry_info);
        ace_entry_info.nof_entry_actions = ace_format_info.nof_actions;
        ace_entry_info.entry_action[0].type = bcmFieldActionOutInterface0;
        ace_entry_info.entry_action[0].value[0] = reflector_global_outlif;
        ace_entry_info.entry_action[1].type = bcmFieldActionOutInterface1;
        ace_entry_info.entry_action[1].value[0] = rcy_global_outlif_gport;
        ace_entry_info.entry_action[2].type = bcmFieldActionAceContextValue;
        /** reading the required context values from dbal DB. Expected values: 1 with UDP swap or 0 without UDP swap */
        if (is_udp_swap)
        {
            value = bcmFieldAceContextReflector;
        }
        else
        {
            value = bcmFieldAceContextNull;

        }
        /** 4 is the offset of the required context value */
        /** assuming ace context value is 0 at that point - so no need for read-modify-write*/
        ace_entry_info.entry_action[2].value[0] = value;

        BCM_IF_ERROR_RETURN_MSG(bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, ace_entry_handle), "");
    }
    
    /** Field Group: Source system port to Trap */
    bcm_field_group_info_t_init(fg_info);
    fg_info->fg_type = bcmFieldGroupTypeTcam;
    fg_info->nof_quals = 2;
    fg_info->stage = bcmFieldStageEgress;
    fg_info->qual_types[0] = bcmFieldQualifySrcPort;
    fg_info->qual_types[1] = bcmFieldQualifyOutPortTrafficManagement;

    if (is_epmf_ace_support)
    {
        fg_info->nof_actions = 2;
        fg_info->action_types[0] = bcmFieldActionOutPortTrafficManagement;
        fg_info->action_types[1] = bcmFieldActionAceEntryId;
    }
    else
    {
        fg_info->nof_actions = 5;
        fg_info->action_types[0] = bcmFieldActionOutPortTrafficManagement;
        fg_info->action_types[1] = bcmFieldActionOutInterface0;
        fg_info->action_types[2] = bcmFieldActionOutInterface1;
        fg_info->action_types[3] = bcmFieldActionAceContextValue;
        /**change pp_dsp for IBCH.ptc*/
        fg_info->action_types[4] = bcmFieldActionPpDsp;
    }
    dest_char = &(fg_info->name[0]);
    sal_strncpy_s(dest_char, "OUTLIF + SIP to OUTLIF and ACE", sizeof(fg_info->name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, fg_info, fg_id), "Map SSP to TRAP");

    /** attach program to given context */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info->nof_quals;
    attach_info.payload_info.nof_actions = fg_info->nof_actions;
    for (qual_index = 0; qual_index < fg_info->nof_quals; qual_index++)
    {
        attach_info.key_info.qual_types[qual_index] = fg_info->qual_types[qual_index];
        attach_info.key_info.qual_info[qual_index].input_type = bcmFieldInputTypeMetaData;
    }
    for (action_index = 0; action_index < fg_info->nof_actions; action_index++)
    {
        attach_info.payload_info.action_types[action_index] = fg_info->action_types[action_index];
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, *fg_id, context_id, &attach_info), "");

    return BCM_E_NONE;
}

/**
 * \brief - Add entry to the field processor program created in cint_reflector_l3_internal_fp_create()
 *
 * \param [in] unit -  Unit-ID
 * \param [in] fg_id -  field group id of the relevant program
 * \param [in] fg_info -  field group info of the relevant program
 * \param [in] sip -  expected source IP of the original packet, used to identify the reflector packet
 * \param [in] rcy_tm_port -  TM port of the recycle port that will be used.
 * \param [in] ace_entry_handle - ace action handle created by cint_reflector_l3_internal_fp_create()
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */

int
cint_reflector_l3_internal_fp_entry_add(
    int unit,
    int fg_id,
    bcm_field_group_info_t * fg_info,
    int in_port,
    int egress_port,
    int rcy_port,
    int ace_entry_handle,
    bcm_if_t reflector_global_outlif,
    bcm_if_t rcy_global_outlif_gport,
    int is_udp_swap)
{
    int qual_index, action_index;
    bcm_field_entry_info_t entry_info;
    bcm_field_entry_t entry_handle;

    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 flags;
    int rcy_tm_port, rcy_pp_dsp, egress_tm_port;

    BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, rcy_port, &flags,&interface_info,&mapping_info), "for rcy_port");
    rcy_tm_port = mapping_info.tm_port;
    rcy_pp_dsp = mapping_info.pp_dsp;

    BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, egress_port, &flags,&interface_info,&mapping_info), "for rcy_port");
    egress_tm_port = mapping_info.tm_port;
    
    /** add an entry mapping in_source system port to reflector trap and encap*/
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = fg_info->nof_quals;
    entry_info.nof_entry_actions = fg_info->nof_actions;
    for (qual_index = 0; qual_index < fg_info->nof_quals; qual_index++)
    {
        entry_info.entry_qual[qual_index].type = fg_info->qual_types[qual_index];
    }
    for (action_index = 0; action_index < fg_info->nof_actions; action_index++)
    {
        entry_info.entry_action[action_index].type = fg_info->action_types[action_index];
    }


    entry_info.entry_qual[0].value[0] = in_port;
    entry_info.entry_qual[0].mask[0] = 0xffff;
    entry_info.entry_qual[1].value[0] = egress_tm_port;
    entry_info.entry_qual[1].mask[0] = 0xffff;
    if (is_epmf_ace_support)
    {        
        entry_info.entry_action[0].value[0] = rcy_tm_port;
        entry_info.entry_action[1].value[0] = ace_entry_handle;
    }
    else
    {
        entry_info.entry_action[0].value[0] = rcy_tm_port;
        entry_info.entry_action[1].value[0] = reflector_global_outlif;
        entry_info.entry_action[2].value[0] = rcy_global_outlif_gport;
        entry_info.entry_action[3].value[0] = is_udp_swap ? bcmFieldAceContextReflector : bcmFieldAceContextNull;
        entry_info.entry_action[4].value[0] = rcy_pp_dsp;        
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_handle), "");

    return BCM_E_NONE;
}

int cint_reflector_l3_internal_fg_presel_create(int unit, int egress_port, bcm_field_context_t *context_id)
{
    int presel_id = 20;
    char error_msg[200]={0,};
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_presel_entry_data_t presel_entry_data;
    char cmd[300] = {0};
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 flags;
    int egress_tm_port;
    bcm_trunk_info_t trunk_info;
    bcm_trunk_member_t out_trunk_member_array[4];
    int trunk_member_count, member_index;

    if (BCM_GPORT_IS_TRUNK (egress_port)) {
        BCM_IF_ERROR_RETURN_MSG(bcm_trunk_get(unit, egress_port, &trunk_info, 4, out_trunk_member_array, &trunk_member_count), "");
    }

    /*create context*/
    bcm_field_context_info_t_init(&context_info);
    snprintf(error_msg, sizeof(error_msg), "context for preset[%d]", presel_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_field_context_create(unit, 0, bcmFieldStageEgress, &context_info, context_id), error_msg);

    /*mapping traffic to context*/
    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = presel_id;
    presel_entry_id.stage = bcmFieldStageEgress;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.context_id = *context_id;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.nof_qualifiers = 1;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyPortClassTrafficManagement;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = 1;
    presel_entry_data.qual_data[0].qual_mask = 0x3;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data), "");

    /**config the cs profile*/
    if (!BCM_GPORT_IS_TRUNK (egress_port)) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, egress_port, bcmPortClassFieldEgressTrafficManagementPortCs, 1),
            "bcmPortClassFieldEgressTrafficManagementPortCs");
    }
    else
    {
        for (member_index = 0; member_index < trunk_member_count; member_index++) {
            BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, out_trunk_member_array[member_index].gport, bcmPortClassFieldEgressTrafficManagementPortCs, 1),
                "bcmPortClassFieldEgressTrafficManagementPortCs");
        }
    }

    l3_int_reflector_created_context = *context_id;
    return BCM_E_NONE;
}


int cint_reflector_l3_internal_fg_presel_clean(int unit, bcm_field_presel_t presel_id, bcm_field_context_t context_id)
{
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_presel_entry_data_t presel_entry_data;


    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, fg_id_for_l3_internal_reflector, context_id), "");

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = presel_id;
    presel_entry_id.stage = bcmFieldStageEgress;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_context_destroy(unit, bcmFieldStageEgress, context_id), "");

    return BCM_E_NONE;
}


/*
 * Unicast Reflector
 * {
 */
/**
 * \brief - Example of L3 internal unicast reflector
 *
 * Steps:
 * 1. Allocate OUTLIF for internal UC reflector
 * 2. Allocate OUTLIF for recycle
 * 3. Create Field group in EPMF for L3 UC reflector in a given context.
 *    For details See cint_reflector_l3_internal_fp_create
 * 4. Add entry to field processor program
 *
 * \param [in] unit -  Unit-ID
 * \param [in] sip - expected source IP of the original packet (used as an example to reflector packet identification)
 * \param [in] tm_rcy_port -  tm port of the recycle port to use
 * \param [in] is_udp_swap -  set to 1 for UDP reflection, otherwise set to 0
 * \param [in] epmf_context_id -  Field context ID
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_reflector_l3_internal_uc_create(
    int unit,
    int in_port,
    int egress_port,
    int rcy_port,
    int is_udp_swap,
    bcm_field_context_t epmf_context_id)
{
    bcm_switch_reflector_data_t data;
    bcm_if_t reflector_encap_id, rcy_encap_id;
    uint32 ace_entry_handle;
    int fg_id;
    bcm_field_group_info_t fg_info;
    bcm_gport_t reflector_global_outlif, rcy_global_outlif_gport;
    bcm_trunk_info_t trunk_info;
    bcm_trunk_member_t out_trunk_member_array[4];
    int trunk_member_count, member_index;
    bcm_field_context_t attached_ctx = epmf_context_id;

    if (BCM_GPORT_IS_TRUNK (egress_port)) {
        BCM_IF_ERROR_RETURN_MSG(bcm_trunk_get(unit, egress_port, &trunk_info, 4, out_trunk_member_array, &trunk_member_count), "");
    }

    /** allocate internal unicast reflector OUTLIF */
    bcm_switch_reflector_data_t_init(&data);
    data.type = bcmSwitchReflectorUc;
    if (is_l3_reflector_encap_access_optional)
    {
        data.encap_access = bcmEncapAccessTunnel1;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_reflector_create(unit, 0, &reflector_encap_id, &data), "");

    /** allocate OUTLIF for recycle */
    BCM_IF_ERROR_RETURN_MSG(create_recycle_entry_with_defaults(unit, &rcy_encap_id), "");

    printf("reflector_encap_id=0x%x rcy_encap_id=0x%x\n", reflector_encap_id, rcy_encap_id);

    is_epmf_ace_support = *(dnxc_data_get(unit, "field", "ace", "supported", NULL));

    /** Create L3 internal field processor program */
    if (!is_epmf_ace_support && is_create_ctx)
    {
        /**create pre-select*/
        BCM_IF_ERROR_RETURN_MSG(cint_reflector_l3_internal_fg_presel_create(unit, egress_port, &attached_ctx), "");

        BCM_IF_ERROR_RETURN_MSG(cint_reflector_l3_internal_fp_create(unit, attached_ctx, reflector_encap_id, rcy_encap_id,
                                                  is_udp_swap, &fg_id, &fg_info, &ace_entry_handle), "");
    }
    else
    {
        BCM_IF_ERROR_RETURN_MSG(cint_reflector_l3_internal_fp_create(unit, attached_ctx, reflector_encap_id, rcy_encap_id,
                                                  is_udp_swap, &fg_id, &fg_info, &ace_entry_handle), "");
    }

    fg_id_for_l3_internal_reflector = fg_id;

    /** add entry to field processor program */
    if (!BCM_GPORT_IS_TRUNK (egress_port)) {
        BCM_IF_ERROR_RETURN_MSG(cint_reflector_l3_internal_fp_entry_add(unit, fg_id, &fg_info, in_port, egress_port, rcy_port, ace_entry_handle, reflector_encap_id, rcy_encap_id, is_udp_swap), "");

    } else {
        for (member_index = 0; member_index < trunk_member_count; member_index++) {
            BCM_IF_ERROR_RETURN_MSG(cint_reflector_l3_internal_fp_entry_add(unit, fg_id, &fg_info, in_port, out_trunk_member_array[member_index].gport, rcy_port, ace_entry_handle, reflector_encap_id, rcy_encap_id, is_udp_swap), "");

        }
    }

    BCM_IF_ERROR_RETURN_MSG(cint_reflector_l3_internal_config_port_for_ibch1_rch(unit, rcy_port, egress_port), "");

    return BCM_E_NONE;
}
/*
 * }
 * Unicast Reflector
 */
