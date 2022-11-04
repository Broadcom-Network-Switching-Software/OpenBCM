 /*
 * Configuration example start:
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/sand/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/tunnel/cint_dnx_ip_tunnel_encapsulation.c
  cint ../../../../src/examples/dnx/field/cint_field_crps_records.c
  cint ../../../../src/examples/sand/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/sand/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/tunnel/cint_dnx_ip_tunnel_encapsulation.c
  cint ../../../../src/examples/dnx/field/cint_field_crps_records.c
  cint;
  int unit = 0;
  int in_port = 200;
  int out_port = 201;
  int udp_dst_port = 0x1234;
  ip_tunnel_encap_udp(unit, in_port,out_port, udp_dst_port);
  cint_field_crps_records_main(unit);
  cint_field_crps_records_entry_add(unit, in_port, out_port, BCM_L3_ITF_VAL_GET(cint_ip_tunnel_basic_info.tunnel_intf));
 * Test Scenario - end
 *
 * packet is expected to be encapsulated with UDPoIPV4 tunnel
 *
 * Configuration example end
 *
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
bcm_field_action_t update_layer_qualifier_action =  bcmFieldActionParsingStartType;

int nof_actions = 3;

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

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF3;
    fg_info.nof_quals = nof_qualifiers;
    fg_info.qual_types[0] = in_port_qualifier;
    fg_info.nof_actions = nof_actions;
    fg_info.action_types[0] = update_destination_action;
    fg_info.action_types[1] = update_outlif_action;
    fg_info.action_types[2] = update_layer_qualifier_action;

    printf("Creating first group\n");
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "In_port_qual", sizeof(fg_info.name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &fg_info, &cint_in_port_qual_fg_id), "");


    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    attach_info.payload_info.action_types[2] = fg_info.action_types[2];


    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    printf("Attaching field group\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, cint_in_port_qual_fg_id, crps_records_context_id, &attach_info), "");

    return BCM_E_NONE;
}

int cint_field_crps_records_entry_add(int unit, uint32 in_port, uint32 out_port, uint32 out_lif)
{
    bcm_field_entry_info_t ent_info;

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
    ent_info.entry_action[2].type = update_layer_qualifier_action;
    ent_info.entry_action[2].value[0] =  bcmFieldLayerTypeRch;

    printf("Adding Entry\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, cint_in_port_qual_fg_id, &ent_info, &cint_entry_1_id), "");

    return BCM_E_NONE;
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
int cint_field_crps_records_destroy(int unit)
{
    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, cint_in_port_qual_fg_id, NULL, cint_entry_1_id), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, cint_in_port_qual_fg_id, crps_records_context_id), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, cint_in_port_qual_fg_id), "");

    return BCM_E_NONE;
}


