/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
 /*
 *Configuration example start:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/dnx/traps/cint_utils_rx.c
 * cint ../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../src/examples/dnx/field/cint_field_presel_fwd_layer.c
 * cint ../../src/examples/dnx/field/cint_field_DstGport_ipmf3_qual.c
 * cint;
 * int unit = 0;
 * uint32 vid = 100;
 * bcm_port_t in_port = 200;
 * bcm_port_t dest_port = 201;
 * cint_field_DstGport_ipmf3_qual_main(unit, vid, in_port, dest_port);
 *
 * Configuration example end
 *
 * Example of using bcmFieldQualifyDstGport qualifier which is encoded as Trap GPORT.
 */

int Cint_field_DstGport_ipmf3_qual_tc_action_value = 6;

bcm_field_group_t Cint_field_DstGport_ipmf3_qual_fg_id;
bcm_gport_t Cint_field_DstGport_ipmf3_qual_trap_gport;
bcm_field_entry_t Cint_field_DstGport_ipmf3_qual_entry_id;
bcm_field_context_t Cint_field_DstGport_ipmf3_qual_context_id;
bcm_field_context_t Cint_field_DstGport_ipmf3_qual_presel_id = 50;
int Cint_field_DstGport_ipmf3_qual_trap_id;

/**
* \brief
*  Configure FEC with trap gport destination.
* \param [in] unit        -  Device id
* \param [in] vid   -  Vlan Id
* \param [in] in_port   -  In port
* \param [in] dest_port   -  Destination port
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_DstGport_ipmf3_trap_dest_create(
    int unit,
    uint32 vid,
    bcm_port_t in_port,
    bcm_port_t dest_port)
{
    int rv = BCM_E_NONE;
    bcm_mac_t mac_address = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};
    bcm_ip_t dip_address = 0x7fffff02;
    uint32 mask = 0xfffffff0;
    bcm_rx_trap_config_t trap_config;

    bcm_if_t ingress_intf_id;
    bcm_if_t egress_intf_id;
    bcm_l3_route_t l3route;
    field_utils_l2_l3_config_t l3_config;

    cint_field_utils_l2_l3_config_t_init(&l3_config);

    l3_config.in_port = in_port;
    l3_config.out_port = dest_port;
    l3_config.vid = vid;
    l3_config.dst_mac = mac_address;
    l3_config.ipv4_dip = dip_address;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &egress_intf_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    rv = bcm_port_class_set(unit, in_port, bcmPortClassId, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_class_set\n");
        return rv;
    }

    rv = cint_field_utils_ports_create(unit, l3_config);
    if (rv != BCM_E_NONE) {
        printf("Error, cint_field_utils_ports_create\n");
        return rv;
    }

    rv = cint_field_utils_l3_intf_create(unit, l3_config, &ingress_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, cint_field_utils_l3_intf_create\n");
        return rv;
    }

    rv = cint_field_utils_l3_ingress_create(unit, l3_config, &ingress_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, cint_field_utils_l3_ingress_create\n");
        return rv;
    }

    bcm_l3_egress_t egress_intf;
    bcm_mac_t next_hop_mac = { 0x22, 0x00, 0x00, 0x00, 0x00, 0x22 };
    bcm_if_t eg_arp_intf;

    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    trap_config.trap_strength = 0;
    trap_config.dest_port = dest_port;
    rv = cint_utils_rx_trap_create_and_set(unit, 0, bcmRxTrapUserDefine, &trap_config, &Cint_field_DstGport_ipmf3_qual_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_utils_rx_trap_create_and_set\n");
        return rv;
    }
    BCM_GPORT_TRAP_SET(Cint_field_DstGport_ipmf3_qual_trap_gport, Cint_field_DstGport_ipmf3_qual_trap_id, trap_config.trap_strength, trap_config.snoop_strength);

    bcm_l3_egress_t_init(&egress_intf);
    egress_intf.flags = 0;
    sal_memcpy(egress_intf.mac_addr, next_hop_mac, 6);
    egress_intf.encap_id = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x4;
    egress_intf.vlan = l3_config.vid;
    rv = bcm_l3_egress_create(unit, BCM_L3_WITH_ID | BCM_L3_EGRESS_ONLY, &egress_intf, &eg_arp_intf);
    if(rv != BCM_E_NONE) {
        printf("bcm_l3_egress_create error: %d\n", rv);
        return rv;
    }

    bcm_l3_egress_t_init(&egress_intf);
    egress_intf.flags = 0;
    egress_intf.port = Cint_field_DstGport_ipmf3_qual_trap_gport;
    egress_intf.intf = ingress_intf_id;
    egress_intf.encap_id = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x4;
    rv = bcm_l3_egress_create(unit, BCM_L3_WITH_ID | BCM_L3_INGRESS_ONLY, &egress_intf, &egress_intf_id);
    if(rv != BCM_E_NONE) {
        printf("bcm_l3_egress_create error: %d\n", rv);
        return rv;
    }

    bcm_l3_route_t_init(&l3route);
    l3route.l3a_ip_mask = 0xFFFFFFFF;
    l3route.l3a_intf = egress_intf_id;
    l3route.l3a_vrf = l3_config.vid;
    l3route.l3a_subnet = l3_config.ipv4_dip;
    rv = bcm_l3_route_add(unit, &l3route);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_route_add\n");
        return rv;
    }

    return 0;
}

/**
* \brief
*  Configures field group, attaching it to a context and adding entry.
* \param [in] unit        -  Device id
* \param [in] vid   -  Vlan Id
* \param [in] in_port   -  In port
* \param [in] dest_port   -  Destination port
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_DstGport_ipmf3_qual_main(
    int unit,
    uint32 vid,
    bcm_port_t in_port,
    bcm_port_t dest_port)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_action_info_t action_info;
    void *dest_char;
    int rv = BCM_E_NONE;

    /** Configure FEC with trap gport destination. */
    rv = cint_field_DstGport_ipmf3_trap_dest_create(unit, vid, in_port, dest_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_DstGport_ipmf3_trap_dest_create \n", rv);
        return rv;
    }

    /** Create FWD context. */
    rv = field_presel_fwd_layer_main(unit, Cint_field_DstGport_ipmf3_qual_presel_id, bcmFieldStageIngressPMF3,
                                     bcmFieldLayerTypeIp4, &Cint_field_DstGport_ipmf3_qual_context_id, "dstgport_ipmf3_qual");
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in field_presel_fwd_layer_main \n", rv);
        return rv;
    }

    /** Create and attach TCAM group in iPMF3. */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF3;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyDstGport;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionPrioIntNew;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "dstgport_ipmf3_qual", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &Cint_field_DstGport_ipmf3_qual_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    printf("Field Group ID %d was created. \n", Cint_field_DstGport_ipmf3_qual_fg_id);

    /** Attach the FG to context. */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, Cint_field_DstGport_ipmf3_qual_fg_id, Cint_field_DstGport_ipmf3_qual_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    printf("Field Group ID %d was attached to Context ID %d. \n", Cint_field_DstGport_ipmf3_qual_fg_id,
           Cint_field_DstGport_ipmf3_qual_context_id);

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = Cint_field_DstGport_ipmf3_qual_trap_gport;
    ent_info.entry_qual[0].mask[0] = 0x1FFFFF;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = Cint_field_DstGport_ipmf3_qual_tc_action_value;

    rv = bcm_field_entry_add(unit, 0, Cint_field_DstGport_ipmf3_qual_fg_id, &ent_info, &Cint_field_DstGport_ipmf3_qual_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    printf("Entry ID 0x%X (%d) was added to Field Group ID %d. \n", Cint_field_DstGport_ipmf3_qual_entry_id,
           Cint_field_DstGport_ipmf3_qual_entry_id, Cint_field_DstGport_ipmf3_qual_fg_id);

    return 0;
}

/**
* \brief
*  Destroy all configuration done
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_DstGport_ipmf3_qual_destroy(
    int unit)
{
    int rv = BCM_E_NONE;

    /* Delete entry from Field group */
    rv = bcm_field_entry_delete(unit, Cint_field_DstGport_ipmf3_qual_fg_id, NULL, Cint_field_DstGport_ipmf3_qual_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete entry %d from fg %d\n", rv, Cint_field_DstGport_ipmf3_qual_entry_id, Cint_field_DstGport_ipmf3_qual_fg_id);
        return rv;
    }
    /* Detach the IPMF1 FG from its context */
    rv = bcm_field_group_context_detach(unit, Cint_field_DstGport_ipmf3_qual_fg_id, Cint_field_DstGport_ipmf3_qual_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, Cint_field_DstGport_ipmf3_qual_fg_id, Cint_field_DstGport_ipmf3_qual_context_id);
        return rv;
    }

    /* Delete  FG */
    rv = bcm_field_group_delete(unit, Cint_field_DstGport_ipmf3_qual_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, Cint_field_DstGport_ipmf3_qual_fg_id);
        return rv;
    }

    rv = field_presel_fwd_layer_destroy(unit, Cint_field_DstGport_ipmf3_qual_presel_id, bcmFieldStageIngressPMF3,
                                        Cint_field_DstGport_ipmf3_qual_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in field_presel_fwd_layer_destroy. \n");
        return rv;
    }

    rv = bcm_rx_trap_type_destroy(unit, Cint_field_DstGport_ipmf3_qual_trap_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error, in bcm_rx_trap_type_destroy \n");
        return rv;
    }

    return rv;
}
