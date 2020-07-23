/**
 * \file cint_reflector_l3_internal.c
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
  cint cint_reflector_l3_internal.c
  cint ../utility/cint_dnx_util_rch.c
  int unit = 0;
  int rcy_tm_port = 211;
  int in_logical_port = 13;
  int sip = 0x01020304;
  int is_udp_swap = 0;
  print cint_reflector_l3_internal_uc_create(unit, sip, rcy_tm_port, is_udp_swap);
  exit;
  "
 */
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
    int rv;
    int value;
    bcm_field_entry_info_t epmf_entry_info;

    /** define ace action */
    bcm_field_ace_format_info_t_init(&ace_format_info);
    ace_format_info.nof_actions = 3;
    ace_format_info.action_types[0] = bcmFieldActionOutInterface0;
    ace_format_info.action_types[1] = bcmFieldActionOutInterface1;
    ace_format_info.action_types[2] = bcmFieldActionAceContextValue;
    rv = bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_format_add\n", rv);
        return rv;
    }
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

    rv = bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_entry_add\n", rv);
        return rv;
    }

    /** Field Group: Source system port to Trap */
    bcm_field_group_info_t_init(fg_info);
    fg_info->fg_type = bcmFieldGroupTypeTcam;
    fg_info->nof_quals = 2;
    fg_info->stage = bcmFieldStageEgress;
    fg_info->qual_types[0] = bcmFieldQualifySrcPort;
    fg_info->qual_types[1] = bcmFieldQualifyOutPortTrafficManagement;

    fg_info->nof_actions = 2;
    fg_info->action_types[0] = bcmFieldActionOutPortTrafficManagement;
    fg_info->action_types[1] = bcmFieldActionAceEntryId;
    dest_char = &(fg_info->name[0]);
    sal_strncpy_s(dest_char, "OUTLIF + SIP to OUTLIF and ACE", sizeof(fg_info->name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, fg_info, fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_add failed - Map SSP to TRAP\n");
        return rv;
    }

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
    rv = bcm_field_group_context_attach(unit, 0, *fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_attach failed \n");
        return rv;
    }

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
    int rcy_tm_port,
    int ace_entry_handle)
{
    int qual_index, action_index;
    bcm_field_entry_info_t entry_info;
    int rv;
    bcm_field_entry_t entry_handle;

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
    entry_info.entry_qual[0].mask[0] = -1;
    entry_info.entry_qual[1].value[0] = egress_port;
    entry_info.entry_qual[1].mask[0] = -1;
    entry_info.entry_action[0].value[0] = rcy_tm_port;
    entry_info.entry_action[1].value[0] = ace_entry_handle;

    rv = bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

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
 * 3. Create Program in EPMF for L3 UC reflector (the program attached to default context as an example)
 *    For details See cint_reflector_l3_internal_fp_create
 * 4. Add entry to field processor program
 *
 * \param [in] unit -  Unit-ID
 * \param [in] sip - expected source IP of the original packet (used as an example to reflector packet identification)
 * \param [in] tm_rcy_port -  tm port of the recycle port to use
 * \param [in] is_udp_swap -  set to 1 for UDP reflection, otherwise set to 0
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
    int tm_rcy_port,
    int is_udp_swap)
{
    int rv;
    bcm_switch_reflector_data_t data;
    bcm_if_t reflector_encap_id, rcy_encap_id;
    bcm_field_context_t context_id;
    uint32 ace_entry_handle;
    int fg_id;
    bcm_field_group_info_t fg_info;
    bcm_gport_t reflector_global_outlif, rcy_global_outlif_gport;

    /** allocate internal unicast reflector OUTLIF */
    bcm_switch_reflector_data_t_init(&data);
    data.type = bcmSwitchReflectorUc;
    rv = bcm_switch_reflector_create(unit, 0, &reflector_encap_id, &data);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_switch_reflector_create failed\n");
        return rv;
    }

    /** allocate OUTLIF for recycle */
    rv = create_recycle_entry(unit, &rcy_encap_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create_recycle_entry \n", proc_name);
        return rv;
    }

    printf("reflector_encap_id=0x%x rcy_encap_id=0x%x\n", reflector_encap_id, rcy_encap_id);
    /** Create L3 internal field processor program */
    context_id = *dnxc_data_get(unit, "field", "context", "default_context", NULL);
    rv = cint_reflector_l3_internal_fp_create(unit, context_id, reflector_encap_id, rcy_encap_id,
                                              is_udp_swap, &fg_id, &fg_info, &ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l3_external_fp_create failed\n");
        return rv;
    }

        /** add entry to field processor program */
    rv = cint_reflector_l3_internal_fp_entry_add(unit, fg_id, &fg_info, in_port, egress_port, tm_rcy_port, ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l3_internal_fp_entry_add failed\n");
        return rv;
    }

    return BCM_E_NONE;
}
/*
 * }
 * Unicast Reflector
 */
