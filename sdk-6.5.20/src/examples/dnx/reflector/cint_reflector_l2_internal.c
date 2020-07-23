/**
 * \file cint_reflector_l2_internal.c
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: Example for L2 internal application: multicast
 *
 * Multicast sequence:
 * "
 * Assume SNIF application reference create recycle mirror ports (for destination port).
 *
  "
  cint cint_reflector_l2_internal.c
  cint
  int unit = 0;
  int out_tm_port = 1;
  bcm_mac_t my_mac;
  my_mac[0] = 1;
  print cint_reflector_l2_internal_mc_create(unit, out_tm_port, my_mac);
  exit;
  "
 */

/**
 * \brief - Example of field processor program for L2 external multicast reflector
 *    Stage: Egress PMF
 *    Context:
 *        - Attach program to context to given context
 *    Keys:
 *        - Packet is identified by a user specific ACL (PP-DSP as an example as an example)
 *    Actions:
 *        - Override OUTLIF to be reflector OUTLIF (encap_id)
 *
 * \param [in] unit -  Unit-ID
 * \param [in] context_id -  field processor context-id to attach the program to in Egress PMF
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
cint_reflector_l2_internal_fp_create(
    int unit,
    bcm_field_context_t context_id,
    int encap_id,
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
    bcm_field_entry_info_t epmf_entry_info;

    /** define ace action */
    bcm_field_ace_format_info_t_init(&ace_format_info);
    ace_format_info.nof_actions = 1;
    ace_format_info.action_types[0] = bcmFieldActionOutInterface0;

    rv = bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_format_add\n", rv);
        return rv;
    }
    bcm_field_ace_entry_info_t_init(&ace_entry_info);
    ace_entry_info.nof_entry_actions = ace_format_info.nof_actions;
    ace_entry_info.entry_action[0].type = bcmFieldActionOutInterface0;
    ace_entry_info.entry_action[0].value[0] = encap_id;

    rv = bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_entry_add\n", rv);
        return rv;
    }

    /** Field Group: Source system port to Trap */
    bcm_field_group_info_t_init(fg_info);
    fg_info->fg_type = bcmFieldGroupTypeTcam;
    fg_info->nof_quals = 1;
    fg_info->stage = bcmFieldStageEgress;
    fg_info->qual_types[0] = bcmFieldQualifyOutPortTrafficManagement;
    fg_info->nof_actions = 1;
    fg_info->action_types[0] = bcmFieldActionAceEntryId;
    dest_char = &(fg_info->name[0]);
    sal_strncpy_s(dest_char, "ACE PP-DSP and OUTLIF", sizeof(fg_info->name));
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
 * \brief - Add entry to the field processor program created in cint_reflector_l2_internal_fp_create()
 *
 * \param [in] unit -  Unit-ID
 * \param [in] fg_id -  field group id of the relevant program
 * \param [in] fg_info -  field group info of the relevant program
 * \param [in] out_tm_port -  tm port used to identify the reflector packet
 * \param [in] ace_entry_handle - ace action handle created by cint_reflector_l2_internal_fp_create()
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_reflector_l2_internal_fp_entry_add(
    int unit,
    int fg_id,
    bcm_field_group_info_t * fg_info,
    int out_tm_port,
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
    entry_info.entry_qual[0].value[0] = out_tm_port;
    entry_info.entry_qual[0].mask[0] = 0xFF;
    entry_info.entry_action[0].value[0] = ace_entry_handle;

    rv = bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Multicast Reflector
 * {
 */
/**
 * \brief - Example of L2 internal multicast reflector
 *
 * Steps:
 * 1. Allocate OUTLIF for internal MC reflector and set SA to be stamped on the reflected packet
 * 2. Create Program in EPMF for MC reflector (the program attached to default context as an example)
 *    For details See cint_reflector_l2_internal_fp_create
 * 3. Add entry to field processor program
 *
 * \param [in] unit -  Unit-ID
 * \param [in] out_tm_port - used as an example to reflector packet identification
 * \param [in] my_mac - mac to be stamped on the reflected packet
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_reflector_l2_internal_mc_create(
    int unit,
    int out_port,
    bcm_mac_t my_mac)
{
    int rv;
    bcm_switch_reflector_data_t data;
    bcm_if_t encap_id;
    bcm_field_context_t context_id;
    uint32 ace_entry_handle;
    int fg_id;
    bcm_field_group_info_t fg_info;
    bcm_mirror_port_to_rcy_map_info_t rcy_map_info, rcy_map_info_clear;
    int out_tm_port;
    int mirror_port;


    /** assume that mirror recycle port for destination already created by reference application */
    /** just enable high priority */
    rv = bcm_mirror_port_to_rcy_port_map_get(unit, 0, out_port, &rcy_map_info);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_mirror_port_to_rcy_port_map_get failed\n");
        return rv;
    }
    /** first clear */
    sal_memset(&rcy_map_info_clear, 0, sizeof(rcy_map_info_clear));
    rcy_map_info_clear.rcy_mirror_port = BCM_PORT_INVALID;
    rv = bcm_mirror_port_to_rcy_port_map_set(unit, 0, out_port, &rcy_map_info_clear);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_mirror_port_to_rcy_port_map_set failed\n");
        return rv;
    }
    /** then update */
    rcy_map_info.priority_bitmap |= BCM_MIRROR_PORT_TO_RCY_MAP_INFO_PRIORITY_HIGH;
    rv = bcm_mirror_port_to_rcy_port_map_set(unit, 0, out_port, &rcy_map_info);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_mirror_port_to_rcy_port_map_set failed\n");
        return rv;
    }


    /** allocate internal multicast reflector OUTLIF */
    bcm_switch_reflector_data_t_init(&data);
    data.type = bcmSwitchReflectorL2McInternal;
    sal_memcpy(data.mc_reflector_my_mac, my_mac, sizeof(my_mac));
    rv = bcm_switch_reflector_create(unit, 0, &encap_id, &data);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_switch_reflector_create failed\n");
        return rv;
    }

    /** Create L2 internal field processor program */
    context_id = *dnxc_data_get(unit, "field", "context", "default_context", NULL);
    rv = cint_reflector_l2_internal_fp_create(unit, context_id, encap_id, &fg_id, &fg_info,
                                              &ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l2_internal_fp_create failed\n");
        return rv;
    }

    /** add entry to field processor program */
    uint32 port_flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    rv = bcm_port_get(unit, out_port, &port_flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_port_get failed\n");
        return rv;
    }
    out_tm_port = mapping_info.tm_port;
    rv = cint_reflector_l2_internal_fp_entry_add(unit, fg_id, &fg_info, out_tm_port, ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l2_internal_fp_entry_add failed\n");
        return rv;
    }

    return BCM_E_NONE;
}
/*
 * }
 * Multicast Reflector
 */


/*
 * SoC:
 * ucode_port_211.BCM8848X=RCY0.0:core_0.211
 * tm_port_header_type_in_211.BCM8848X=INJECTED_2_PP
 *
 *  cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 *  cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 *  cint ../../../../src/examples/sand/cint_ip_route_basic.c
 *  cint ../../../../src/examples/sand/cint_vpls_mp_basic.c
 *  cint ../../../../src/examples/sand/cint_sand_advanced_vlan_translation_mode.c
 *  cint ../../../../src/examples/dnx/utility/cint_dnx_util_rch.c
 *
 *  c
 *  int unit=0;
 *  int ac_port=201;
 *  int pwe_port=200;
 *  
 *  print vpls_mp_basic_main(unit,ac_port,pwe_port);
 *  
 *  int in_port=pwe_port;
 *  int egress_port=ac_port;
 *  int rcy_port=211;
 *  int is_ip_swap=1;
 *  int is_udp_swap=1;
 *  print cint_reflector_l2_internal_uc_create(unit, in_port, egress_port, rcy_port, is_ip_swap, is_udp_swap);
 *  print bcm_vlan_gport_add(unit, 5, rcy_port, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
 *  exit;
 *  debug bcm rx
 *  ipv4 case:
 *  tx 1 psrc=200 data=000c0002000100000000cd1d81000000884700d0506400d80164001100000112000c000200008100000508004500004E000000004011F94BC0A80002C0A80001CCDDAABB003AAC19000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F3031A09F66B5
 *  tx 1 psrc=200 data=000c0002000100000000cd1d81000000884700d0506400d80164001100000112000c000200008100000508004500004E0000000040FFF94BC0A80002C0A80001CCDDAABB003AAC19000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F3031A09F66B5
 *  ipv6 case:
 *  tx 1 psrc=200 data=000c0002000100000000cd1d81000000884700d0506400d80164001100000112000c000200008100000586DD60300000002611FFFE80000000000000020000FFFE00000135555555666666667777777788888888CCDDAABB00265E2E000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D9FB4DFC8
 *  tx 1 psrc=200 data=000c0002000100000000cd1d81000000884700d0506400d80164001100000112000c000200008100000586DD60300000002601FFFE80000000000000020000FFFE00000135555555666666667777777788888888CCDDAABB00265E2E000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D9FB4DFC8

 */

int cint_reflector_l2_internal_uc_fp_create(
    int unit,
    bcm_field_context_t context_id,
    bcm_if_t reflector_global_outlif,
    int is_ip_swap,
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
    ace_format_info.nof_actions = 2;
    ace_format_info.action_types[0] = bcmFieldActionOutInterface0;
    ace_format_info.action_types[1] = bcmFieldActionAceContextValue;

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
    ace_entry_info.entry_action[1].type = bcmFieldActionAceContextValue;
    /*
     * Expected values: 1 with swap or 0 without swap
     */
    if (is_udp_swap && is_ip_swap)
    {
        value = bcmFieldAceContextReflector;
    }
    else if (is_ip_swap)
    {
        value = bcmFieldAceContextMplsTransitCCMSignal;
    }
    else
    {
        value = bcmFieldAceContextNull;

    }
    ace_entry_info.entry_action[1].value[0] = value;

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
    sal_strncpy_s(dest_char, "ACE PP-DSP and OUTLIF", sizeof(fg_info->name));
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


int cint_reflector_l2_internal_uc_fp_entry_add(
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
    entry_info.entry_qual[0].mask[0] = 0xFFFF;
    entry_info.entry_qual[1].value[0] = egress_port;
    entry_info.entry_qual[1].mask[0] = 0xFF;
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

/**
 * \brief - Example of L2 internal unicast reflector
 *
 * Steps:
 * 1. Allocate OUTLIF for internal UC reflector and outlif for recycle.
 * 2. Create Program in EPMF for L2 internal reflector (the program attached to default context as an example).
 * 3. Add entry to field processor program
 *
 * \param [in] unit -  Unit-ID
 * \param [in] in_port - input port
 * \param [in] egress_port - output port in 1-pass before redirect
 * \param [in] rcy_port - rcy port
 * \param [in] is_ip_swap - control to swap IP address
 * \param [in] is_udp_swap - control to swap UDP port
 */
int cint_reflector_l2_internal_uc_create(int unit, int in_port, int egress_port, int rcy_port, int is_ip_swap, int is_udp_swap)
{
    int rv;
    bcm_switch_reflector_data_t data;
    bcm_if_t reflector_encap_id, rcy_encap_id;
    bcm_field_context_t context_id;
    uint32 ace_entry_handle;
    int fg_id;
    bcm_field_group_info_t fg_info;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 flags;
    
    /** allocate internal unicast reflector OUTLIF */
    bcm_switch_reflector_data_t_init(&data);
    data.type = bcmSwitchReflectorL2UcInternal;
    rv = bcm_switch_reflector_create(unit, 0, &reflector_encap_id, &data);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_switch_reflector_create failed\n");
        return rv;
    }
    
    context_id = *dnxc_data_get(unit, "field", "context", "default_context", NULL);
    rv = cint_reflector_l2_internal_uc_fp_create(unit, context_id, reflector_encap_id, is_ip_swap,
                                              is_udp_swap, &fg_id, &fg_info, &ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l2_internal_fp_create failed\n");
        return rv;
    }

    /*
     * add entry to field processor program 
     */
    rv = bcm_port_get(unit, rcy_port, &flags,&interface_info,&mapping_info);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_port_get failed for RCY_PORT\n");
        return rv;
    }
    
    rv = cint_reflector_l2_internal_uc_fp_entry_add(unit, fg_id, &fg_info, in_port, egress_port, mapping_info.tm_port, ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l2_internal_fp_entry_add failed\n");
        return rv;
    }
    
    return BCM_E_NONE;    
}

