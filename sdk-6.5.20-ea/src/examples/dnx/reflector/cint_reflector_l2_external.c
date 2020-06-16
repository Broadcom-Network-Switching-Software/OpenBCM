/**
 * \file cint_reflector_l2_external.c
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: Example for L2 external application: both unicast and multicast
 *
 * Unicast sequence:
  "
  cint cint_reflector_l2_external.c
  cint
  int unit = 0;
  int in_logical_port = 13;
  bcm_gport_t sysport_gport;
  print bcm_stk_gport_sysport_get(unit, in_logical_port, &sysport_gport);
  print cint_reflector_l2_external_uc_create(unit, sysport_gport);
  exit;
  "
 *
 * Multicast sequence:
  "
  cint cint_reflector_l2_external.c
  cint
  int unit = 0;
  bcm_mac_t my_mac;
  my_mac[0] = 1;
  int in_logical_port = 13;
  bcm_gport_t sysport_gport;
  print bcm_stk_gport_sysport_get(unit, in_logical_port, &sysport_gport);
  print cint_reflector_l2_external_mc_create(unit, sysport_gport, my_mac);
  exit;
  "
 */

/**
 * \brief - Example of field processor program for L2 external unicast reflector
 *    Stage: PMF1
 *    Context:
 *        - Attach program to context to given context
 *    Keys:
 *        - Packet is identified by a user specific ACL (source system port as an example)
 *    Actions:
 *        - Override Forward-Action (trap-id) to unicast reflector trap

 *    Stage: PMF2
 *    Context:
 *        - Attach program to context to given context
  *   Keys:
 *        - PMF1 hit bit (used as a condition)
 *        - source system port (used as direct extraction)
 *    Actions:
 *        - Override destination port to source system port
 *        - For MC reflector, override OUTLIF to be MC reflector OUTLIF
 *
 * \param [in] unit -  Unit-ID
 * \param [in] pmf1_context_id -  field processor context-id to attach the program to in PMF1
 * \param [in] is_multicast -  1 for multicast reflector or 0 for unicast (for mutlicast the program will override OUTLIF as well)
 * \param [out] pmf1_fg_id -  field group id to be used when adding entry
 * \param [out] fg_info -  field group info to be used when adding entry
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_reflector_l2_external_fp_create(
    int unit,
    bcm_field_context_t pmf1_context_id,
    int is_multicast,
    bcm_field_group_t * pmf1_fg_id,
    bcm_field_group_info_t * fg_info)
{
    bcm_field_action_info_t action_info;
    bcm_field_action_t action_ssp;
    bcm_field_group_t fg_id;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_group_info_t pmf2_fg_info;
    int qual_index, action_index;
    void *dest_char;
    int trap_offset;
    int rv;

    /**
     * PMF1 program (see function header for details)
     */
    /** Field Group: Source system port to Trap */
    bcm_field_group_info_t_init(fg_info);
    fg_info->fg_type = bcmFieldGroupTypeTcam;
    fg_info->nof_quals = 1;
    fg_info->stage = bcmFieldStageIngressPMF1;
    fg_info->qual_types[0] = bcmFieldQualifySrcPort;
    fg_info->nof_actions = 1;
    fg_info->action_types[0] = bcmFieldActionTrap;
    char *fg_name = "Map SSP to TRAP";
    if (is_multicast)
    {
        fg_info->nof_actions++;
        fg_info->action_types[1] = bcmFieldActionOutInterface0;
        fg_name = "Map SSP to TRAP and OUTLIF";
    }
    dest_char = &(fg_info->name[0]);
    sal_strncpy_s(dest_char, fg_name, sizeof(fg_info->name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, fg_info, pmf1_fg_id);
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
    rv = bcm_field_group_context_attach(unit, 0, *pmf1_fg_id, pmf1_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_attach failed \n");
        return rv;
    }

        /** PMF2 - if UC reflector trap was set - override destination to be SSP */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionForward;
    action_info.prefix_size = 16;
    action_info.prefix_value = 0xc;
    action_info.size = 16;
    action_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(action_info.name[0]);
    sal_strncpy(dest_char, "Over_Dest", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &action_ssp);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_action_create failed \n");
        return rv;
    }

    /** define a field group with source system port will be used as a key - direct extraction*/
    bcm_field_qualifier_info_create_t qual_info;
    char *qual_name = "UC REFLECTOR VALID";
    int uc_reflector_valid_qual_id;

    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    sal_strncpy(dest_char, qual_name, sizeof(qual_info.name));
    qual_info.size = 1;
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &uc_reflector_valid_qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_field_qualifier_create failed \n");
        return rv;
    }

    bcm_field_group_info_t_init(&pmf2_fg_info);
    pmf2_fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    pmf2_fg_info.nof_quals = 2;
    pmf2_fg_info.stage = bcmFieldStageIngressPMF2;
    pmf2_fg_info.qual_types[0] = uc_reflector_valid_qual_id;
    pmf2_fg_info.qual_types[1] = bcmFieldQualifySrcPort;
    pmf2_fg_info.nof_actions = 1;
    pmf2_fg_info.action_types[0] = action_ssp;
    pmf2_fg_info.action_with_valid_bit[0] = TRUE;

    dest_char = &(pmf2_fg_info.name[0]);
    sal_strncpy_s(dest_char, "DEST<-SSP", sizeof(pmf2_fg_info.name));
    rv = bcm_field_group_add(unit, 0, &pmf2_fg_info, &fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_add failed - DEST<-SSP\n");
        return rv;
    }

    /** attach to context */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = pmf2_fg_info.nof_quals;
    attach_info.payload_info.nof_actions = pmf2_fg_info.nof_actions;
    for (qual_index = 0; qual_index < pmf2_fg_info.nof_quals; qual_index++)
    {
        attach_info.key_info.qual_types[qual_index] = pmf2_fg_info.qual_types[qual_index];
        if (pmf2_fg_info.qual_types[qual_index] == uc_reflector_valid_qual_id)
        {
            attach_info.key_info.qual_info[qual_index].input_type = bcmFieldInputTypeCascaded;
            attach_info.key_info.qual_info[qual_index].input_arg = *pmf1_fg_id;
            rv = bcm_field_group_action_offset_get(unit, 0, *pmf1_fg_id, bcmFieldActionTrap, &trap_offset);
            if (rv != BCM_E_NONE)
            {
                printf("Error in bcm_field_group_action_offset_get failed - DEST<-SSP\n");
                return rv;
            }
                /** -1 is for the valid bit of trap*/
            attach_info.key_info.qual_info[qual_index].offset = trap_offset - 1;
        }
        else
        {
            attach_info.key_info.qual_info[qual_index].input_type = bcmFieldInputTypeMetaData;
        }
    }
    for (action_index = 0; action_index < pmf2_fg_info.nof_actions; action_index++)
    {
        attach_info.payload_info.action_types[action_index] = pmf2_fg_info.action_types[action_index];
    }
    rv = bcm_field_group_context_attach(unit, 0, fg_id, pmf1_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_attach failed \n");
        return rv;
    }

    return BCM_E_NONE;
}

/**
 * \brief - Add entry to the field processor program created in cint_reflector_l2_external_fp_create()
 *
 * \param [in] unit -  Unit-ID
 * \param [in] fg_id -  field group id of the relevant program
 * \param [in] fg_info -  field group info of the relevant program
 * \param [in] sysport_gport -  system port gport used to identify the reflector packet
 * \param [in] trap_id -  reflector trap id
 * \param [in] encap_id -  reflector encap id
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_reflector_l2_external_fp_entry_add(
    int unit,
    int fg_id,
    bcm_field_group_info_t * fg_info,
    bcm_gport_t sysport_gport,
    int trap_id,
    bcm_if_t encap_id)
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
    entry_info.entry_qual[0].value[0] = sysport_gport;
    entry_info.entry_qual[0].mask[0] = 0x7FFF;
    entry_info.entry_action[0].value[0] = trap_id;
    if (encap_id != -1)
    {
        entry_info.entry_action[1].value[0] = encap_id;
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
 * Unicast Reflector
 * {
 */
/**
 * \brief - Example of L2 external unicast reflector
 *
 * Steps:
 * 1. Allocate OUTLIF for UC reflector.
 * 2. Allocate and set trap for UC reflector.
 *    When unicast reflector forward action is resolved, OUTLIF will be override to given UC reflector OUTLIF
 * 3. Create Program in PMF1 for UC reflector (the program attached to default context as an example)
 *    For details See cint_reflector_l2_external_fp_create
 * 4. Add entry to field processor program
 *
 * \param [in] unit -  Unit-ID
 * \param [in] sysport_gport -  used as example for identification of reflector packets (source system port)
 * \param [in] pmf1_context_id -  pmf1_context_id to attach the program to.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_reflector_l2_external_uc_create(
    int unit,
    bcm_gport_t sysport_gport,
    bcm_field_context_t pmf1_context_id)
{
    int rv;
    bcm_switch_reflector_data_t data;
    bcm_if_t encap_id;
    int trap_id, fg_id;
    bcm_rx_trap_config_t trap_config;
    bcm_field_group_info_t fg_info;
    bcm_gport_t trap_gport, lif_gport;
        /** allocate unicast reflector OUTLIF */
    bcm_switch_reflector_data_t_init(&data);
    data.type = bcmSwitchReflectorUc;
    rv = bcm_switch_reflector_create(unit, 0, &encap_id, &data);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_switch_reflector_create failed\n");
        return rv;
    }

    /** allocate Trap Code */
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_rx_trap_type_create failed\n");
        return rv;
    }
    /** Set trap to override OUTLIF to be the reflector OUTLIF */
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(lif_gport, encap_id);

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.snoop_strength = 0;
    trap_config.trap_strength = 0;
    trap_config.encap_id = lif_gport;
    trap_config.flags = BCM_RX_TRAP_UPDATE_ENCAP_ID;
    print("encap = 0%x\n", encap_id);
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_rx_trap_set failed\n");
        return rv;
    }

        /** Create L2 unicast field processor program */
    rv = cint_reflector_l2_external_fp_create(unit, pmf1_context_id, 0, &fg_id, &fg_info);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l2_external_fp_create failed\n");
        return rv;
    }

        /** add entry to field processor program */
    BCM_GPORT_TRAP_SET(trap_gport, trap_id, 15, 0);
    rv = cint_reflector_l2_external_fp_entry_add(unit, fg_id, &fg_info, sysport_gport, trap_gport, -1);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l2_external_fp_entry_add failed\n");
        return rv;
    }

    return BCM_E_NONE;
}
/*
 * }
 * Unicast Reflector
 */

/*
 * Multicast Reflector
 * {
 */
/**
 * \brief - Example of L2 external multicast reflector
 *
 * Steps:
 * 1. Allocate OUTLIF for external MC reflector and set SA to be stamped on the reflected packet
 * 2. Allocate and set trap for MC reflector.
 *    When unicast reflector forward action is resolved, OUTLIF will be override to given MC reflector OUTLIF
 * 3. Create Program in PMF1 for UC reflector (the program attached to default context as an example)
 *    For details See cint_reflector_l2_external_fp_create
 * 4. Add entry to field processor program
 *
 * \param [in] unit -  Unit-ID
 * \param [in] sysport_gport -  used as example for identification of reflector packets
 * \param [in] pmf1_context_id -  pmf1_context_id to attach the program to.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_reflector_l2_external_mc_create(
    int unit,
    bcm_gport_t sysport_gport,
    bcm_mac_t my_mac,
    bcm_field_context_t pmf1_context_id)
{
    int rv;
    bcm_switch_reflector_data_t data;
    bcm_if_t encap_id;
    int trap_id, fg_id, id;
    bcm_rx_trap_config_t trap_config;
    bcm_field_group_info_t fg_info;
    bcm_gport_t trap_gport, lif_gport;

        /** allocate external multicast reflector OUTLIF */
    bcm_switch_reflector_data_t_init(&data);
    data.type = bcmSwitchReflectorL2McExternal;
    sal_memcpy(data.mc_reflector_my_mac, my_mac, sizeof(my_mac));
    rv = bcm_switch_reflector_create(unit, 0, &encap_id, &data);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_switch_reflector_create failed\n");
        return rv;
    }
    print("encap = 0%x\n", encap_id);

    /** allocate Trap Code */
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_rx_trap_type_create failed\n");
        return rv;
    }
    /** Set trap */
    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.snoop_strength = 0;
    trap_config.trap_strength = 0;;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_rx_trap_set failed\n");
        return rv;
    }

        /** Create L2 unicast field processor program */
    rv = cint_reflector_l2_external_fp_create(unit, pmf1_context_id, 1, &fg_id, &fg_info);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l2_external_fp_create failed\n");
        return rv;
    }

        /** add entry to field processor program */
        /** create trap gport */
    BCM_GPORT_TRAP_SET(trap_gport, trap_id, 15, 0);
    rv = cint_reflector_l2_external_fp_entry_add(unit, fg_id, &fg_info, sysport_gport, trap_gport, encap_id);
    if (rv != BCM_E_NONE)
    {
        printf("cint_reflector_l2_external_fp_entry_add failed\n");
        return rv;
    }
    return BCM_E_NONE;
}
/*
 * }
 * Multicast Reflector
 */
