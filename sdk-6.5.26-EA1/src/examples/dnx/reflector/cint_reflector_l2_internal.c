/**
 * \file cint_reflector_l2_internal.c
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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
 *
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

/*
 * On epmf to indicate 
 *      1: support ACE table. 
 *      0: no more indirect ACE table.
 */
int is_epmf_ace_support=1;
/*
 * is_pp_context_support=1, can't support mir_rcy port
 */
int is_pp_context_support=0;

bcm_field_group_t fg_id_for_l2_internal_reflector =0;

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

    is_epmf_ace_support = *(dnxc_data_get(unit, "field", "ace", "supported", NULL));

    if (is_epmf_ace_support)
    {
        /** define ace action */
        bcm_field_ace_format_info_t_init(&ace_format_info);
        ace_format_info.nof_actions = 2;
        /*
         * If with EVE, reflector outlif must be bcmFieldActionOutInterface1
         * The action bcmFieldActionOutInterface0 to carry OutAC should be kept.
         */
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
        ace_entry_info.entry_action[0].value[0] = encap_id;
        ace_entry_info.entry_action[1].type = bcmFieldActionAceContextValue;
        ace_entry_info.entry_action[1].value[0] = bcmFieldAceContextReflectorL2IntMc;

        rv = bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, ace_entry_handle);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_ace_entry_add\n", rv);
            return rv;
        }
    }
    
    /** Field Group: Source system port to Trap */
    bcm_field_group_info_t_init(fg_info);
    fg_info->fg_type = bcmFieldGroupTypeTcam;
    fg_info->nof_quals = 2;
    fg_info->stage = bcmFieldStageEgress;
    fg_info->qual_types[0] = bcmFieldQualifySrcPort;
    fg_info->qual_types[1] = bcmFieldQualifyOutPortTrafficManagement;

    if (is_use_dedicated_rcy_port)
    {
        if (is_epmf_ace_support)
        {
            /**actually, can't go into this case*/
            fg_info->nof_actions = 2;
            fg_info->action_types[0] = bcmFieldActionOutPortTrafficManagement;
            fg_info->action_types[1] = bcmFieldActionAceEntryId;
        }
        else
        {
            fg_info->nof_actions = 4;
            fg_info->action_types[0] = bcmFieldActionOutPortTrafficManagement;
            fg_info->action_types[1] = bcmFieldActionOutInterface1;
            fg_info->action_types[2] = bcmFieldActionAceContextValue;
            /**change pp_dsp for IBCH.ptc*/
            fg_info->action_types[3] = bcmFieldActionPpDsp;
        }
    }
    else
    {
        if (is_epmf_ace_support)
        {
            fg_info->nof_actions = 1;
            fg_info->action_types[0] = bcmFieldActionAceEntryId;
        }
        else
        {
            fg_info->nof_actions = 2;
            fg_info->action_types[0] = bcmFieldActionOutInterface1;
            fg_info->action_types[1] = bcmFieldActionAceContextValue;
        }
    }

    
    dest_char = &(fg_info->name[0]);
    sal_strncpy_s(dest_char, "ACE PP-DSP and OUTLIF", sizeof(fg_info->name));
    rv = bcm_field_group_add(unit, 0, fg_info, fg_id);
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
 * \param [in] egress_port - fwd_port for 1st forward
 * \param [in] rcy_port - rcy_port if use dedicated RCY port
 * \param [in] ace_entry_handle - ace action handle created by cint_reflector_l2_internal_fp_create()
 * \param [in] reflector_global_outlif - reflector outlif
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
    int in_port,
    int egress_port,
    int rcy_port,
    int ace_entry_handle,
    bcm_if_t reflector_global_outlif)
{
    int qual_index, action_index;
    bcm_field_entry_info_t entry_info;
    int rv;
    bcm_field_entry_t entry_handle;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 flags;
    int rcy_tm_port, rcy_pp_dsp, egress_tm_port;
    
    if (is_use_dedicated_rcy_port) {
        rv = bcm_port_get(unit, rcy_port, &flags,&interface_info,&mapping_info);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_port_get failed for rcy_port\n");
            return rv;
        }
        rcy_tm_port = mapping_info.tm_port;
        rcy_pp_dsp = mapping_info.pp_dsp;
    }
    rv = bcm_port_get(unit, egress_port, &flags,&interface_info,&mapping_info);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_port_get failed for egress_port\n");
        return rv;
    }
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
    entry_info.entry_qual[0].mask[0] = 0xFFFF;
    entry_info.entry_qual[1].value[0] = egress_tm_port;
    entry_info.entry_qual[1].mask[0] = 0xFFFF;

    if (is_use_dedicated_rcy_port)
    {
        if (is_epmf_ace_support)
        {
            entry_info.entry_action[0].value[0] = rcy_tm_port;
            entry_info.entry_action[1].value[0] = ace_entry_handle;            
        }
        else
        {
            entry_info.entry_action[0].value[0] = rcy_tm_port;
            entry_info.entry_action[1].value[0] = reflector_global_outlif;
            entry_info.entry_action[2].value[0] = bcmFieldAceContextReflectorL2IntMc;
            entry_info.entry_action[3].value[0] = rcy_pp_dsp;
        }
    }
    else
    {
        if (is_epmf_ace_support)
        {
            entry_info.entry_action[0].value[0] = ace_entry_handle;
        }
        else
        {
            entry_info.entry_action[0].value[0] = reflector_global_outlif;
            entry_info.entry_action[1].value[0] = bcmFieldAceContextReflectorL2IntMc;
        }
    }

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
 * 2. Create a field group in EPMF for MC reflector (attached to a given context)
 *    For details See cint_reflector_l2_internal_fp_create
 * 3. Add entry to field processor program
 *
 * \param [in] unit -  Unit-ID
 * \param [in] out_port - used as an example to reflector packet identification
 * \param [in] rcy_port - Dedicated RCY port, also used when pp_context is supported.
 * \param [in] my_mac - mac to be stamped on the reflected packet
 * \param [in] epmf_context_id - field context id

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
    int in_port,
    int out_port,
    int rcy_port,
    bcm_mac_t my_mac,
    bcm_field_context_t epmf_context_id)
{
    int rv;
    bcm_switch_reflector_data_t data;
    bcm_if_t encap_id;
    uint32 ace_entry_handle;
    int fg_id;
    bcm_field_group_info_t fg_info;
    bcm_mirror_port_to_rcy_map_info_t rcy_map_info, rcy_map_info_clear;
    int out_tm_port;

    is_pp_context_support = *(dnxc_data_get(unit, "ingr_reassembly", "context", "pp_tm_context", NULL));

    /**if pp_context support, can't use mrir_rcy port*/
    if (is_pp_context_support)
    {
        is_use_dedicated_rcy_port = 1;
    }
    else
    {
        is_use_dedicated_rcy_port = 0;
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

    rv = cint_reflector_l2_internal_fp_create(unit, epmf_context_id, encap_id, &fg_id, &fg_info,
                                              &ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l2_internal_fp_create failed\n");
        return rv;
    }
    fg_id_for_l2_internal_reflector = fg_id;

    /** add entry to field processor program */    
    rv = cint_reflector_l2_internal_fp_entry_add(unit, fg_id, &fg_info, in_port, out_port, rcy_port, ace_entry_handle, encap_id);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l2_internal_fp_entry_add failed\n");
        return rv;
    }

    rv = cint_reflector_l2_internal_config_port_for_ibch1_ptch1_plus(unit, rcy_port, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l2_internal_config_port_for_ibch1_ptch1_plus failed\n");
        return rv;
    }

    return BCM_E_NONE;
}

/**
 * \brief - Example of L2 internal multicast reflector on non RCY port.
 *          customers will configure loopback themselves such as PHY loopback
 *
 * Steps:
 * 1. Allocate OUTLIF for internal MC reflector and outlif for recycle.
 * 2. Create Program in EPMF for L2 internal reflector (the program attached to context).
 * 3. Add entry to egress field processor program
 *
 * \param [in] unit -  Unit-ID
 * \param [in] out_port - out port for 1st pass.
 * \param [in] my_mac - mac to be stamped on the reflected packet
 * \param [in] epmf_context_id - field context id
 *
 */
int
cint_reflector_l2_internal_mc_non_rcy_port_create(
    int unit,
    int in_port,
    int out_port,
    bcm_mac_t my_mac,
    bcm_field_context_t epmf_context_id)
{
    int rv;
    bcm_switch_reflector_data_t data;
    bcm_if_t encap_id;
    uint32 ace_entry_handle;
    int fg_id;
    bcm_field_group_info_t fg_info;
    bcm_mirror_port_to_rcy_map_info_t rcy_map_info, rcy_map_info_clear;
    int out_tm_port;

    is_use_dedicated_rcy_port = 0;

    /** allocate internal multicast reflector OUTLIF */
    bcm_switch_reflector_data_t_init(&data);
    /**rcy without PTCH2*/
    data.type = bcmSwitchReflectorL2McInternalOnePass;
    sal_memcpy(data.mc_reflector_my_mac, my_mac, sizeof(my_mac));
    rv = bcm_switch_reflector_create(unit, 0, &encap_id, &data);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_switch_reflector_create failed\n");
        return rv;
    }

    rv = cint_reflector_l2_internal_fp_create(unit, epmf_context_id, encap_id, &fg_id, &fg_info,
                                              &ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l2_internal_fp_create failed\n");
        return rv;
    }
    fg_id_for_l2_internal_reflector = fg_id;

    /** add entry to field processor program */
    rv = cint_reflector_l2_internal_fp_entry_add(unit, fg_id, &fg_info, in_port, out_port, 0, ace_entry_handle, encap_id);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l2_internal_fp_entry_add failed\n");
        return rv;
    }

    return BCM_E_NONE;
}


int cint_reflector_l2_internal_mc_with_eve(
        int unit,
        int in_port,
        int egress_port,
        int rcy_port,
        bcm_mac_t my_mac,
        bcm_vlan_t old_vlan,
        bcm_vlan_t new_vlan,
        bcm_field_context_t epmf_context_id)
{
    int rv;
    bcm_gport_t out_gport;

    rv = cint_reflector_l2_internal_uc_p2p_outlif_from_esem(unit,in_port,old_vlan,egress_port,old_vlan,&out_gport);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l2_internal_uc_p2p_outlif_from_esem failed\n");
        return rv;
    }
    /**update outer vlan from old_vlan to new_vlan*/
    rv = ive_eve_translation_set(unit,out_gport,0x8100,0,bcmVlanActionReplace,bcmVlanActionNone,new_vlan,0,2,4,0);
    if (rv != BCM_E_NONE)
    {
        printf("ive_eve_translation_set for EVE failed\n");
        return rv;
    }
    
    rv = cint_reflector_l2_internal_mc_create(unit, in_port, egress_port, rcy_port, my_mac, epmf_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l2_internal_uc_create failed\n");
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, old_vlan, in_port, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
    if (rv != BCM_E_NONE) {
        printf("Error (%d), in bcm_vlan_gport_add for in_port\n", rv);
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, new_vlan, egress_port, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
    if (rv != BCM_E_NONE) {
        printf("Error (%d), in bcm_vlan_gport_add for in_port\n", rv);
        return rv;
    }    
    
    return BCM_E_NONE;    
}


/*
 * }
 * Multicast Reflector
 */


/*
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

/*
 * is_use_dedicated_rcy_port used to select recycle interface. Default to use RCY_Mir port.
 */
int is_use_dedicated_rcy_port=0;

/**configure ports when recycle header is ptch1_plus over IBCH1*/
int cint_reflector_l2_internal_config_port_for_ibch1_ptch1_plus(
    int unit,
    int rcy_port,
    int egress_port)
{
    int rv;
    int is_ibch1_enable;
    bcm_switch_control_info_t value;
    bcm_switch_control_key_t key;

    if (*(dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL)) == 0)
    {
        printf("IBCH1 is not support, skip rcy_port-%d, egress_port-%d setting!\n", rcy_port, egress_port);
        return 0;
    }
    
    /**enable IBCH1 on rcy, default is enabled*/
    rv = bcm_port_control_get(unit, rcy_port, bcmPortControlIbch1Enable, &is_ibch1_enable);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_port_control_get for bcmPortControlIbch1Enable\n", rv);
        return rv;
    }
    else
    {
        if (!is_ibch1_enable)
        {
            rv = bcm_port_control_set(unit, rcy_port, bcmPortControlIbch1Enable, 1);
            if (rv != BCM_E_NONE)
            {
                printf("Error (%d), in bcm_port_control_set for bcmPortControlIbch1Enable\n", rv);
                return rv;
            }
        }
    }
    
    /**config RCY_port in header type*/
    value.value = BCM_SWITCH_PORT_HEADER_TYPE_IBCH1_MODE;
    key.index = 1;
    key.type = bcmSwitchPortHeaderType;
    rv = bcm_switch_control_indexed_port_set(unit, rcy_port, key, value);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_switch_control_indexed_port_set for BCM_SWITCH_PORT_HEADER_TYPE_IBCH1_MODE\n", rv);
        return rv;
    }

    /**PTCH1_plus mapping set*/
    rv = bcm_port_control_set(unit, egress_port, bcmPortControlSystemPortInjectedMap, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_port_control_set for bcmPortControlSystemPortInjectedMap\n", rv);
        return rv;
    }

    return rv;
}

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

    is_epmf_ace_support = *(dnxc_data_get(unit, "field", "ace", "supported", NULL));

    if (is_epmf_ace_support)
    {
        /** define ace action */
        bcm_field_ace_format_info_t_init(&ace_format_info);
        ace_format_info.nof_actions = 2;
        /*
         * If with EVE, must be bcmFieldActionOutInterface1
         * Other cases can be bcmFieldActionOutInterface0
         */
        ace_format_info.action_types[0] = bcmFieldActionOutInterface1;
        ace_format_info.action_types[1] = bcmFieldActionAceContextValue;

        rv = bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_ace_format_add\n", rv);
            return rv;
        }
        bcm_field_ace_entry_info_t_init(&ace_entry_info);
        ace_entry_info.nof_entry_actions = ace_format_info.nof_actions;
        ace_entry_info.entry_action[0].type = bcmFieldActionOutInterface1;
        ace_entry_info.entry_action[0].value[0] = reflector_global_outlif;
        ace_entry_info.entry_action[1].type = bcmFieldActionAceContextValue;
        /*
         * Expected values: 1 with swap or 0 without swap
         */
        if (is_udp_swap && is_ip_swap)
        {
            value = bcmFieldAceContextReflectorL2IntIpUdpSwap;
        }
        else if (is_ip_swap)
        {
            value = bcmFieldAceContextReflectorL2IntIpSwap;
        }
        else
        {
            value = bcmFieldAceContextReflectorL2Int;

        }
        ace_entry_info.entry_action[1].value[0] = value;

        rv = bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, ace_entry_handle);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_ace_entry_add\n", rv);
            return rv;
        }
    }

    /** Field Group: Source system port to Trap */
    bcm_field_group_info_t_init(fg_info);
    fg_info->fg_type = bcmFieldGroupTypeTcam;
    fg_info->nof_quals = 2;
    fg_info->stage = bcmFieldStageEgress;
    fg_info->qual_types[0] = bcmFieldQualifySrcPort;
    fg_info->qual_types[1] = bcmFieldQualifyOutPortTrafficManagement;
    if (is_use_dedicated_rcy_port)
    {
        if (is_epmf_ace_support)
        {
            fg_info->nof_actions = 2;
            fg_info->action_types[0] = bcmFieldActionOutPortTrafficManagement;
            fg_info->action_types[1] = bcmFieldActionAceEntryId;
        }
        else
        {
            fg_info->nof_actions = 4;
            fg_info->action_types[0] = bcmFieldActionOutPortTrafficManagement;
            fg_info->action_types[1] = bcmFieldActionOutInterface1;
            fg_info->action_types[2] = bcmFieldActionAceContextValue;
            /**change pp_dsp for IBCH.ptc*/
            fg_info->action_types[3] = bcmFieldActionPpDsp;
        }
    }
    else
    {
        if (is_epmf_ace_support)
        {
            fg_info->nof_actions = 1;
            fg_info->action_types[0] = bcmFieldActionAceEntryId;
        }
        else
        {
            fg_info->nof_actions = 2;
            fg_info->action_types[0] = bcmFieldActionOutInterface1;
            fg_info->action_types[1] = bcmFieldActionAceContextValue;
        }
    }
    dest_char = &(fg_info->name[0]);
    sal_strncpy_s(dest_char, "ACE PP-DSP and OUTLIF", sizeof(fg_info->name));
    rv = bcm_field_group_add(unit, 0, fg_info, fg_id);
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
    int rcy_port,
    int ace_entry_handle,
    int is_ip_swap,
    int is_udp_swap,
    bcm_if_t reflector_global_outlif)
{
    int qual_index, action_index;
    bcm_field_entry_info_t entry_info;
    int rv;
    bcm_field_entry_t entry_handle;
    int ace_value;

    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 flags;
    int rcy_tm_port, rcy_pp_dsp, egress_tm_port;

    if (is_use_dedicated_rcy_port) {
        rv = bcm_port_get(unit, rcy_port, &flags,&interface_info,&mapping_info);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_port_get failed for rcy_port\n");
            return rv;
        }
        rcy_tm_port = mapping_info.tm_port;
        rcy_pp_dsp = mapping_info.pp_dsp;
    }
    rv = bcm_port_get(unit, egress_port, &flags,&interface_info,&mapping_info);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_port_get failed for egress_port\n");
        return rv;
    }
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
    entry_info.entry_qual[0].mask[0] = 0xFFFF;
    entry_info.entry_qual[1].value[0] = egress_tm_port;
    entry_info.entry_qual[1].mask[0] = 0xFF;

    /*
     * ace_value
     */
    if (!is_epmf_ace_support)
    {
        if (is_udp_swap && is_ip_swap)
        {
            ace_value = bcmFieldAceContextReflectorL2IntIpUdpSwap;
        }
        else if (is_ip_swap)
        {
            ace_value = bcmFieldAceContextReflectorL2IntIpSwap;
        }
        else
        {
            ace_value = bcmFieldAceContextReflectorL2Int;
        }
    }
    
    if (is_use_dedicated_rcy_port)
    {
        entry_info.entry_action[0].value[0] = rcy_tm_port;
        if (is_epmf_ace_support)
        {
            entry_info.entry_action[1].value[0] = ace_entry_handle;
        }
        else
        {
            entry_info.entry_action[1].value[0] = reflector_global_outlif;
            entry_info.entry_action[2].value[0] = ace_value;
            entry_info.entry_action[3].value[0] = rcy_pp_dsp;
        }
    }
    else
    {
        if (is_epmf_ace_support)
        {
            entry_info.entry_action[0].value[0] = ace_entry_handle;
        }
        else
        {
            entry_info.entry_action[0].value[0] = reflector_global_outlif;
            entry_info.entry_action[1].value[0] = ace_value;
        }
    }
    rv = bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    return BCM_E_NONE;
}

/**
 * \brief - Basic example of L2 internal unicast reflector.
 *
 * Steps:
 * 1. Allocate OUTLIF for internal UC reflector and outlif for recycle.
 * 2. Create Program in EPMF for L2 internal reflector.
 * 3. Add entry to field processor program
 *
 * \param [in] unit -  Unit-ID
 * \param [in] in_port - input port
 * \param [in] egress_port - output port in 1-pass
 * \param [in] rcy_port - Dedicated RCY port, also used when pp_context is supported.
 * \param [in] is_ip_swap - control to swap IP address
 * \param [in] is_udp_swap - control to swap UDP port
 * \param [in] epmf_context_id - field context id
 *
 */
int cint_reflector_l2_internal_uc_create(
        int unit,
        int in_port,
        int egress_port,
        int rcy_port,
        int is_ip_swap,
        int is_udp_swap,
        bcm_field_context_t epmf_context_id)
{
    int rv;
    bcm_switch_reflector_data_t data;
    bcm_if_t reflector_encap_id, rcy_encap_id;
    uint32 ace_entry_handle;
    int fg_id;
    bcm_field_group_info_t fg_info;
    bcm_mirror_port_to_rcy_map_info_t rcy_map_info, rcy_map_info_clear;

    is_pp_context_support = *(dnxc_data_get(unit, "ingr_reassembly", "context", "pp_tm_context", NULL));

    /**if pp_context support, can't use mir_rcy port*/
    if (is_pp_context_support)
    {
        is_use_dedicated_rcy_port = 1;
    }
    else
    {
        /** in this function, use mir_rcy port  */
        is_use_dedicated_rcy_port = 0;
        /** assume that mirror recycle port for destination already created by reference application */
        /** just enable high priority */
        rv = bcm_mirror_port_to_rcy_port_map_get(unit, 0, egress_port, &rcy_map_info);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_mirror_port_to_rcy_port_map_get failed\n");
            return rv;
        }
        /** first clear */
        sal_memset(&rcy_map_info_clear, 0, sizeof(rcy_map_info_clear));
        rcy_map_info_clear.rcy_mirror_port = BCM_PORT_INVALID;
        rv = bcm_mirror_port_to_rcy_port_map_set(unit, 0, egress_port, &rcy_map_info_clear);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_mirror_port_to_rcy_port_map_set failed\n");
            return rv;
        }
        /** then update */
        rcy_map_info.priority_bitmap |= BCM_MIRROR_PORT_TO_RCY_MAP_INFO_PRIORITY_HIGH;
        rv = bcm_mirror_port_to_rcy_port_map_set(unit, 0, egress_port, &rcy_map_info);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_mirror_port_to_rcy_port_map_set failed\n");
            return rv;
        }
    }
    
    /** allocate internal unicast reflector OUTLIF */
    bcm_switch_reflector_data_t_init(&data);
    data.type = bcmSwitchReflectorL2UcInternal;
    rv = bcm_switch_reflector_create(unit, 0, &reflector_encap_id, &data);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_switch_reflector_create failed\n");
        return rv;
    }
    
    rv = cint_reflector_l2_internal_uc_fp_create(unit, epmf_context_id, reflector_encap_id, is_ip_swap,
                                              is_udp_swap, &fg_id, &fg_info, &ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l2_internal_fp_create failed\n");
        return rv;
    }
    fg_id_for_l2_internal_reflector = fg_id;

    /*
     * add entry to field processor program 
     */
    
    rv = cint_reflector_l2_internal_uc_fp_entry_add(unit, fg_id, &fg_info, in_port, egress_port, rcy_port, ace_entry_handle, is_ip_swap, is_udp_swap, reflector_encap_id);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l2_internal_fp_entry_add failed\n");
        return rv;
    }

    rv = cint_reflector_l2_internal_config_port_for_ibch1_ptch1_plus(unit, rcy_port, egress_port);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l2_internal_config_port_for_ibch1_ptch1_plus failed\n");
        return rv;
    }
    
    return BCM_E_NONE;    
}


/**
 * \brief - Another example to config L2 internal unicast reflector with a dedicated rcy port for recycling.
 *
 * Steps:
 * 1. Allocate OUTLIF for internal UC reflector and outlif for recycle.
 * 2. Create Program in EPMF for L2 internal reflector.
 * 3. Add entry to field processor program
 *
 * \param [in] unit -  Unit-ID
 * \param [in] in_port - input port
 * \param [in] egress_port - output port in 1-pass
 * \param [in] rcy_port - Dedicated RCY port
 * \param [in] is_ip_swap - control to swap IP address
 * \param [in] is_udp_swap - control to swap UDP port
 * \param [in] epmf_context_id - field context id
 *
 */
int cint_reflector_l2_internal_uc_with_dedicated_rcy_create(
        int unit,
        int in_port,
        int egress_port,
        int rcy_port,
        int is_ip_swap,
        int is_udp_swap,
        bcm_field_context_t epmf_context_id)
{
    int rv;
    bcm_switch_reflector_data_t data;
    bcm_if_t reflector_encap_id, rcy_encap_id;
    uint32 ace_entry_handle;
    int fg_id;
    bcm_field_group_info_t fg_info;

    is_use_dedicated_rcy_port = 1;

    /** allocate internal unicast reflector OUTLIF */
    bcm_switch_reflector_data_t_init(&data);
    data.type = bcmSwitchReflectorL2UcInternal;
    rv = bcm_switch_reflector_create(unit, 0, &reflector_encap_id, &data);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_switch_reflector_create failed\n");
        return rv;
    }
    
    rv = cint_reflector_l2_internal_uc_fp_create(unit, epmf_context_id, reflector_encap_id, is_ip_swap,
                                              is_udp_swap, &fg_id, &fg_info, &ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l2_internal_fp_create failed\n");
        return rv;
    }
    fg_id_for_l2_internal_reflector = fg_id;

    /*
     * add entry to field processor program 
     */    
    rv = cint_reflector_l2_internal_uc_fp_entry_add(unit, fg_id, &fg_info, in_port, egress_port, rcy_port, ace_entry_handle, is_ip_swap, is_udp_swap, reflector_encap_id);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l2_internal_fp_entry_add failed\n");
        return rv;
    }

    rv = cint_reflector_l2_internal_config_port_for_ibch1_ptch1_plus(unit, rcy_port, egress_port);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l2_internal_config_port_for_ibch1_ptch1_plus failed\n");
        return rv;
    }

    return BCM_E_NONE;    
}


/**
 * \brief - Example of L2 internal unicast reflector on non RCY port.
 *          customers will configure loopback themselves such as PHY loopback
 *
 * Steps:
 * 1. Allocate OUTLIF for internal UC reflector and outlif for recycle.
 * 2. Create Program in EPMF for L2 internal reflector.
 * 3. Add entry to field processor program
 *
 * \param [in] unit -  Unit-ID
 * \param [in] in_port - input port
 * \param [in] egress_port - output port in 1-pass
 * \param [in] is_ip_swap - control to swap IP address
 * \param [in] is_udp_swap - control to swap UDP port
 * \param [in] epmf_context_id - field context id
 *
 */
int cint_reflector_l2_internal_uc_non_rcy_port_create(
        int unit,
        int in_port,
        int egress_port,
        int is_ip_swap,
        int is_udp_swap,
        bcm_field_context_t epmf_context_id)
{
    int rv;
    bcm_switch_reflector_data_t data;
    bcm_if_t reflector_encap_id, rcy_encap_id;
    uint32 ace_entry_handle;
    int fg_id;
    bcm_field_group_info_t fg_info;

    is_use_dedicated_rcy_port = 0;
    
    /** allocate internal unicast reflector OUTLIF */
    bcm_switch_reflector_data_t_init(&data);
    
    /*
     * packet recycle without PTHC2, raw updated packet for recycling.
     */
    data.type = bcmSwitchReflectorL2UcInternalOnePass;
    rv = bcm_switch_reflector_create(unit, 0, &reflector_encap_id, &data);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_switch_reflector_create failed\n");
        return rv;
    }

    rv = cint_reflector_l2_internal_uc_fp_create(unit, epmf_context_id, reflector_encap_id, is_ip_swap,
                                              is_udp_swap, &fg_id, &fg_info, &ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l2_internal_fp_create failed\n");
        return rv;
    }
    fg_id_for_l2_internal_reflector = fg_id;

    /*
     * add entry to field processor program 
     */  
    rv = cint_reflector_l2_internal_uc_fp_entry_add(unit, fg_id, &fg_info, in_port, egress_port, 0, ace_entry_handle, is_ip_swap, is_udp_swap, reflector_encap_id);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l2_internal_fp_entry_add failed\n");
        return rv;
    }
    
    return BCM_E_NONE;    
}

/**
 * \brief - Create p2p service with carrying outlif
 *
 * \param [in] unit -  Unit-ID
 * \param [in] in_port - input port
 * \param [in] in_vlan - vlan for in_port
 * \param [in] out_port - out port
 * \param [in] out_vlan - vlan for out_port
 * \param [in] out_gport - return gport of out AC
 *
 */

int cint_reflector_l2_internal_uc_p2p_carry_outlif(
    int unit,
    int in_port,
    bcm_vlan_t in_vlan,
    int out_port,
    bcm_vlan_t out_vlan,
    bcm_gport_t *out_gport)
{
    int rv;
    bcm_vswitch_cross_connect_t p2p_lifs;

    int gport_id;

    bcm_vlan_port_t vp1;
    bcm_vlan_port_t vp2;

    bcm_vlan_port_t_init(&vp1);
    vp1.criteria =BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vp1.flags = BCM_VLAN_PORT_WITH_ID;
    vp1.vsi = 0;
    vp1.match_vlan = in_vlan;
    BCM_GPORT_SYSTEM_PORT_ID_SET(vp1.port, in_port);
    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id,0,0x1001);
    BCM_GPORT_VLAN_PORT_ID_SET(vp1.vlan_port_id, gport_id);

    rv = bcm_vlan_port_create(unit, &vp1);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_vlan_port_create failed for vp1\n");
        return rv;
    }

    p2p_lifs.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    p2p_lifs.port1 = vp1.vlan_port_id;
    BCM_GPORT_SYSTEM_PORT_ID_SET(p2p_lifs.port2, out_port);
    BCM_FORWARD_ENCAP_ID_VAL_SET(p2p_lifs.encap2, BCM_FORWARD_ENCAP_ID_TYPE_OUTLIF, BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_VLAN_PORT, 0x2001);
    rv = bcm_vswitch_cross_connect_add(unit, &p2p_lifs);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_vswitch_cross_connect_add failed from vp1\n");
        return rv;
    }


    bcm_vlan_port_t_init(&vp2);
    gport_id=0;
    vp2.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vp2.flags = BCM_VLAN_PORT_WITH_ID;
    vp2.vsi = 0;
    vp2.match_vlan = out_vlan;
    BCM_GPORT_SYSTEM_PORT_ID_SET(vp2.port, out_port);
    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id,0,0x2001);
    BCM_GPORT_VLAN_PORT_ID_SET(vp2.vlan_port_id, gport_id);
    rv = bcm_vlan_port_create(unit, &vp2);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_vswitch_cross_connect_add failed from vp2\n");
        return rv;
    }
    
    p2p_lifs.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    p2p_lifs.port1 = vp2.vlan_port_id;
    BCM_GPORT_SYSTEM_PORT_ID_SET(p2p_lifs.port2, in_port);
    BCM_FORWARD_ENCAP_ID_VAL_SET(p2p_lifs.encap2, BCM_FORWARD_ENCAP_ID_TYPE_OUTLIF, BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_VLAN_PORT, 0x1001);
    rv = bcm_vswitch_cross_connect_add(unit, &p2p_lifs);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_vswitch_cross_connect_add failed from vp2\n");
        return rv;
    }

    *out_gport = vp2.vlan_port_id;
    return BCM_E_NONE;
    
}


/**
 * \brief - Create p2p service with outAC got from esem
 *
 * \param [in] unit -  Unit-ID
 * \param [in] in_port - input port
 * \param [in] in_vlan - vlan for in_port
 * \param [in] out_port - out port
 * \param [in] out_vlan - vlan for out_port
 * \param [in] out_gport - return gport of out AC
 *
 */
int cint_reflector_l2_internal_uc_p2p_outlif_from_esem(
    int unit,
    int in_port,
    bcm_vlan_t in_vlan,
    int out_port,
    bcm_vlan_t out_vlan,
    bcm_gport_t *out_gport)
{
    int rv;
    bcm_vswitch_cross_connect_t p2p_lifs;

    bcm_vlan_port_t vp1;
    bcm_vlan_port_t vp2;
    bcm_vlan_port_t vp3;
    
    bcm_vlan_port_t_init(&vp1);
    vp1.criteria =BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vp1.vsi = 0;
    vp1.match_vlan = in_vlan;
    BCM_GPORT_SYSTEM_PORT_ID_SET(vp1.port, in_port);
    rv = bcm_vlan_port_create(unit, &vp1);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_vswitch_cross_connect_add failed from vp1\n");
        return rv;
    }

    p2p_lifs.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    p2p_lifs.port1 = vp1.vlan_port_id;
    BCM_GPORT_SYSTEM_PORT_ID_SET(p2p_lifs.port2, out_port);
    rv = bcm_vswitch_cross_connect_add(unit, &p2p_lifs);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_vswitch_cross_connect_add failed from vp2\n");
        return rv;
    }

    bcm_vlan_port_t_init(&vp2);
    vp2.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vp2.vsi = 0;
    vp2.match_vlan = out_vlan;
    BCM_GPORT_SYSTEM_PORT_ID_SET(vp2.port, out_port);
    rv = bcm_vlan_port_create(unit, &vp2);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_vlan_port_create failed from vp2\n");
        return rv;
    }

    p2p_lifs.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    p2p_lifs.port1 = vp2.vlan_port_id;
    BCM_GPORT_SYSTEM_PORT_ID_SET(p2p_lifs.port2, in_port);
    rv = bcm_vswitch_cross_connect_add(unit, &p2p_lifs);    
    if (rv != BCM_E_NONE)
    {
        printf("bcm_vswitch_cross_connect_add failed from vp2\n");
        return rv;
    }

    /** set outlif in ESEM*/
    bcm_port_tpid_class_t port_tpid_class;
    /*
     * set tag format
     */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = out_port;
    port_tpid_class.tpid1 = 0x8100;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = 4;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    port_tpid_class.flags |= BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_C;
    port_tpid_class.vlan_translation_action_id = 0;
    rv =  bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_port_tpid_class_set failed from vp3\n");
        return rv;
    }
    
    bcm_vlan_port_t_init(&vp3);
    vp3.criteria =BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp3.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
    vp3.vsi = 0;
    vp3.match_vlan = out_vlan;
    vp3.port = out_port;
    rv = bcm_vlan_port_create(unit, &vp3);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_vlan_port_create failed from vp3\n");
        return rv;
    }

    *out_gport = vp3.vlan_port_id;

    return BCM_E_NONE;
}


/**
 * \brief - Example of L2 internal unicast reflector with EVE in 1st pass(also support for non_rcy port case).
 *
 * Steps:
 * 1. Create p2p service according to "is_carry_outlif" and apply eve.
 * 2. Allocate OUTLIF for internal UC reflector and outlif for recycle.
 * 3. Create Program in EPMF for L2 internal reflector
 * 4. Add entry to field processor program
 *
 * \param [in] unit -  Unit-ID
 * \param [in] in_port - input port
 * \param [in] egress_port - output port in 1-pass
 * \param [in] is_ip_swap - control to swap IP address
 * \param [in] is_udp_swap - control to swap UDP port
 * \param [in] is_carry_outlif - selet which mode to get outAC
 * \param [in] old_vlan - old vlan of outAC
 * \param [in] new_vlan - updated vlan of outAC
 * \param [in] epmf_context_id - field context id
 *
 */
int cint_reflector_l2_internal_uc_with_eve(
        int unit,
        int in_port,
        int egress_port,
        int rcy_port,
        int is_ip_swap,
        int is_udp_swap,
        int is_carry_outlif,
        bcm_vlan_t old_vlan,
        bcm_vlan_t new_vlan,
        bcm_field_context_t epmf_context_id)
{
    int rv;
    bcm_gport_t out_gport;


    if (is_carry_outlif)
    {
        rv = cint_reflector_l2_internal_uc_p2p_carry_outlif(unit,in_port,old_vlan,egress_port,old_vlan,&out_gport);
        if (rv != BCM_E_NONE)
        {
            printf("cint_reflector_l2_internal_uc_p2p_carry_outlif failed\n");
            return rv;
        }
        /**update outer vlan from old_vlan to new_vlan*/
        rv = ive_eve_translation_set(unit,out_gport,0x8100,0,bcmVlanActionReplace,bcmVlanActionNone,new_vlan,0,2,8,0);
        if (rv != BCM_E_NONE)
        {
            printf("ive_eve_translation_set for EVE failed\n");
            return rv;
        }
    }
    else
    {
        rv = cint_reflector_l2_internal_uc_p2p_outlif_from_esem(unit,in_port,old_vlan,egress_port,old_vlan,&out_gport);
        if (rv != BCM_E_NONE)
        {
            printf("cint_reflector_l2_internal_uc_p2p_outlif_from_esem failed\n");
            return rv;
        }
        /**update outer vlan from old_vlan to 100*/
        rv = ive_eve_translation_set(unit,out_gport,0x8100,0,bcmVlanActionReplace,bcmVlanActionNone,new_vlan,0,2,4,0);
        if (rv != BCM_E_NONE)
        {
            printf("ive_eve_translation_set for EVE failed\n");
            return rv;
        }
    }


    
    rv = cint_reflector_l2_internal_uc_create(unit, in_port, egress_port, rcy_port, is_ip_swap, is_udp_swap, epmf_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l2_internal_uc_create failed\n");
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, old_vlan, in_port, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
    if (rv != BCM_E_NONE) {
        printf("Error (%d), in bcm_vlan_gport_add for in_port\n", rv);
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, new_vlan, egress_port, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
    if (rv != BCM_E_NONE) {
        printf("Error (%d), in bcm_vlan_gport_add for in_port\n", rv);
        return rv;
    }    
    
    return BCM_E_NONE;    
}


