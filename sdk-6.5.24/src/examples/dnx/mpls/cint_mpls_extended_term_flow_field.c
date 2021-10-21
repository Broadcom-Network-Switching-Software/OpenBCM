/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2021 Broadcom Inc. All rights reserved. File: cint_mpls_5_label_term_field.c  
 */

/*
 * The below CINT demonstrates identify all LSR missing packets which doesn't come from rcy port,
 * and recycle these packets to 2nd pass.
 */

 /**
  * \brief
  *
  *  Setup ingress PMF rules to identify all LSR missing packets not come from rcy port and trap to recycle port.
  *
  * \param [in] unit     - The unit number.
  * \param [in] in_port  - in port.
  * \param [in] rcy_port - recycle port
  *
  * \return
  *  int - Error Type, in sense of bcm_error_t
 */
 int mpls_field_extended_term_flow_set_ipmf1(int unit, 
       int in_port, 
       int rcy_port, 
       bcm_if_t rcy_extended_term_encap_id) 
 {
    int rv = 0;
    bcm_field_group_t fg_id = 0;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    int nof_quals = 2;
    int nof_actions = 1;
    int ii = 0;
    int context_id;
    void *dest_char;
    bcm_field_entry_info_t entry_info;
    bcm_field_entry_t entry_handle;
    int mpls_unknown_label_trap_id = 0xc;
    uint32 normal_port_profile = 2;
    bcm_gport_t gport_trap = 0;
    int new_trap_id = 0;
    bcm_rx_trap_config_t config;
    int trap_strength = 7;

    /* Set ipmf_port_profile to differ general port(2) and rcycle port(0) */
    rv = bcm_port_class_set(unit, in_port, bcmPortClassFieldIngressPMF1PacketProcessingPort, normal_port_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_class_set with type == bcmPortClassFieldIngressPMF1PacketProcessingPort\n");
        return rv;
    }

    /** Configure RCH port of Extended Termination type */
    rv = bcm_port_control_set(unit, rcy_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm);
    if (rv != BCM_E_NONE)
    {
        printf("mpls_field_extended_term_flow_set_ipmf1(): Error, bcm_port_control_set for rch_port %d\n", rcy_port);
        return rv;
    }

    context_id = *dnxc_data_get(unit, "field", "context", "default_context", NULL);
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_quals = nof_quals;
    fg_info.qual_types[0] = bcmFieldQualifyRxTrapCode;
    fg_info.qual_types[1] = bcmFieldQualifyPortClassPacketProcessing;

    fg_info.nof_actions = nof_actions;
    fg_info.action_types[0] = bcmFieldActionTrap;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "MPLS_LABEL_FWD_MISS", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, 0, &fg_info, &fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_field_group_add\n");
        return rv;
    }

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
        attach_info.payload_info.action_info[ii].priority = BCM_FIELD_ACTION_PRIORITY(0, (ii + 1));
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    rv = bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_field_group_context_attach\n");
        return rv;
    }

    /*
     * Create a trap that new destination is rcy port and bring extended term encap to egress.
     */
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &new_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_rx_trap_type_create\n");
        return rv;
    }

    bcm_rx_trap_config_t_init(&config);
    config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_UPDATE_ENCAP_ID;
    config.dest_port = rcy_port;
    config.trap_strength = 0;
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(config.encap_id, rcy_extended_term_encap_id);
    rv = bcm_rx_trap_set(unit, new_trap_id, &config);     
    BCM_GPORT_TRAP_SET(gport_trap, new_trap_id, trap_strength, 0);

    /*
     * Qualify all LSR miss packets from normal port, 
     * Trap these packet to recycle port and stard term more label at 2nd pass.
     */
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.entry_qual[0].type = bcmFieldQualifyRxTrapCode;
    entry_info.entry_qual[0].value[0] = mpls_unknown_label_trap_id;
    entry_info.entry_qual[0].mask[0] = 0x1FF;
    entry_info.entry_qual[1].type = bcmFieldQualifyPortClassPacketProcessing;
    entry_info.entry_qual[1].value[0] = normal_port_profile;
    entry_info.entry_qual[1].mask[0] = 0x7;

    entry_info.entry_action[0].type = bcmFieldActionTrap;
    entry_info.entry_action[0].value[0] = gport_trap;
    rv = bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_field_entry_add\n");
        return rv;
    }

    return rv;
 }

