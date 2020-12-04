/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

 /*
 * Configuration example start:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../src/examples/dnx/tunnel/cint_dnx_ip_tunnel_encapsulation.c
 * cint ../../../src/examples/dnx/field/cint_field_crps_records.c
 * cint;
 * int unit = 0;
 * int in_port = 200;
 * int out_port = 201;
 * int udp_dst_port = 0x1234;
 * ip_tunnel_encap_udp(0,in_port,out_port, udp_dst_port);
 * cint_field_crps_records_main(unit);
 * cint_field_crps_records_entry_add(unit, in_port, out_port, BCM_L3_ITF_VAL_GET(cint_ip_tunnel_basic_info.tunnel_intf));
 *
 *
 * packet is expected to be encapsulated with UDPoIPV4 tunnel
 *
 * Configuration example end
 */

bcm_field_group_t   cint_in_port_qual_fg_id = 0;
bcm_field_entry_t cint_entry_1_id;

/* context id */
bcm_field_context_t crps_records_context_id= BCM_FIELD_CONTEXT_ID_DEFAULT;


/* qualifiers */
bcm_field_qualify_t in_port_qualifier = bcmFieldQualifyInPort;
int nof_qualifiers = 1;

/** actions */
bcm_field_action_t update_destination_action = bcmFieldActionRedirect;
bcm_field_action_t update_outlif_action = bcmFieldActionOutVport0Raw;
int nof_actions = 2;

/**
* \brief
*  Configures the written above
* \param [in] unit        -  Device id
* \param [in] context_id  -  PMF context Id
* \param [in] in_port     -  in pp_port id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_crps_records_main(int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_action_info_t action_info;
    void *dest_char;
    int rv = BCM_E_NONE;


    /*
     * Create and attach TCAM group in iPMF1
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_quals = nof_qualifiers;
    fg_info.qual_types[0] = in_port_qualifier;
    fg_info.nof_actions = nof_actions;
    fg_info.action_types[0] = update_destination_action;
    fg_info.action_types[1] = update_outlif_action;


    printf("Creating first group\n");
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "In_port_qual", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_in_port_qual_fg_id);
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
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    printf("Attaching field group\n");
    rv = bcm_field_group_context_attach(unit, 0, cint_in_port_qual_fg_id, crps_records_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    return 0;
}

int cint_field_crps_records_entry_add(int unit, uint32 in_port, uint32 out_port, uint32 out_lif) {
    bcm_field_entry_info_t ent_info;
    int rv = BCM_E_NONE;


    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    ent_info.nof_entry_quals = nof_qualifiers;
    ent_info.entry_qual[0].type = in_port_qualifier;
    ent_info.entry_qual[0].value[0] = in_port;
    ent_info.entry_qual[0].mask[0] = 0x1ff;

    ent_info.nof_entry_actions = nof_actions;
    ent_info.entry_action[0].type = update_destination_action;
    ent_info.entry_action[0].value[0] = out_port;
    ent_info.entry_action[1].type = update_outlif_action;
    ent_info.entry_action[1].value[0] = out_lif;

    printf("Adding Entry\n");
    rv = bcm_field_entry_add(unit, 0, cint_in_port_qual_fg_id, &ent_info, &cint_entry_1_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    return 0;
}

/**
* \brief
*  Destroy all configuration done
* \param [in] unit        -  Device id
* \param [in] context_id  -  PMF context Id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_in_port_qual_destroy(int unit)
{
    int rv = BCM_E_NONE;

    /* Delete entry from Field group */
    rv = bcm_field_entry_delete(unit, cint_in_port_qual_fg_id, NULL, cint_entry_1_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete entry %d from fg %d\n", rv, cint_entry_1_id, cint_in_port_qual_fg_id);
        return rv;
    }
    /* Detach the IPMF1 FG from its context */
    rv = bcm_field_group_context_detach(unit, cint_in_port_qual_fg_id, crps_records_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, cint_in_port_qual_fg_id, crps_records_context_id);
        return rv;
    }

    /* Delete  FG */
    rv = bcm_field_group_delete(unit, cint_in_port_qual_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, cint_in_port_qual_fg_id);
        return rv;
    }

    return rv;
}


