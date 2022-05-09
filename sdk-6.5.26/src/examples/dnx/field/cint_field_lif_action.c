/*
 * Configuration example start:
 *
  cint ../../../src/examples/dnx/field/cint_field_lif_action.c
  cint ../../../../src/examples/dnx/field/cint_field_lif_action.c
  cint;
  int unit = 0;
  bcm_field_context_t context_id = 0;
  int in_port = 200;
  cint_field_lif_main(unit, context_id, in_port);
 *
 * Configuration example end
 *
 * example of using Lif which is encoded as GPORT.
 */

bcm_field_group_t   cint_lif_fg_id = 0;
bcm_gport_t lif_gport;
bcm_field_entry_t cint_lif_ent_id;

bcm_field_entry_t cint_lif_qual_ent_id;
bcm_field_group_t cint_lif_qual_fg_id = 0;


/**
* \brief
*  Configures the written above
* \param [in] unit        -  Device id
* \param [in] context_id  -  PMF context Id
* \param [in] lif_id       - lif Id (22 bits)
* \param [in] lif_type     - inlif =0, outlif =1
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_lif_main(int unit, bcm_field_context_t context_id, int in_port)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_t lif_action;
    bcm_vlan_port_t in_vlan_port;
    int vsi = 100;

    lif_action =  bcmFieldActionIngressGportSet;

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vsi));

    bcm_vlan_port_t_init(&in_vlan_port);
    in_vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    in_vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    in_vlan_port.port = in_port;
    in_vlan_port.vsi = vsi;
    BCM_IF_ERROR_RETURN(bcm_vlan_port_create(unit, &in_vlan_port));

    lif_gport = in_vlan_port.vlan_port_id;

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyTpid;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = lif_action;

    printf("Creating first group\n");
    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, &fg_info, &cint_lif_fg_id));


    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 96;

    printf("Attaching field group\n");
    BCM_IF_ERROR_RETURN(bcm_field_group_context_attach(unit, 0, cint_lif_fg_id, context_id, &attach_info));

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = 0x8100;
    ent_info.entry_qual[0].mask[0] = 0xffff;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = lif_gport;

    printf("Adding Entry\n");
    BCM_IF_ERROR_RETURN(bcm_field_entry_add(unit, 0, cint_lif_fg_id, &ent_info, &cint_lif_ent_id));
    
    return BCM_E_NONE;
}


/**
* \brief
*  Configures the written above
* \param [in] unit        -  Device id
* \param [in] context_id  -  PMF context Id
* \param [in] stage       -  PMF stage
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_lif_qual_main(int unit, bcm_field_context_t context_id, bcm_field_stage_t stage)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_action_info_t action_info;
    bcm_vlan_port_t in_vlan_port;

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = stage;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyInVPort0;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionPrioIntNew;

    printf("Creating first group\n");
    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, &fg_info, &cint_lif_qual_fg_id));

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    printf("Attaching field group\n");
    BCM_IF_ERROR_RETURN(bcm_field_group_context_attach(unit, 0, cint_lif_qual_fg_id, context_id, &attach_info));

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];

    printf("lif_gport = %d \n",lif_gport);
    ent_info.entry_qual[0].value[0] = lif_gport;

    ent_info.entry_qual[0].mask[0] = 0x3fffff;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = 0x7;

    printf("Adding Entry\n");
    BCM_IF_ERROR_RETURN(bcm_field_entry_add(unit, 0, cint_lif_qual_fg_id, &ent_info, &cint_lif_qual_ent_id));
    
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
int cint_field_lif_destroy(int unit, bcm_field_context_t context_id)
{

    BCM_IF_ERROR_RETURN(bcm_vlan_port_destroy(unit,lif_gport));
    BCM_IF_ERROR_RETURN(bcm_field_entry_delete(unit, cint_lif_fg_id, NULL, cint_lif_ent_id));

    BCM_IF_ERROR_RETURN(bcm_field_group_context_detach(unit, cint_lif_fg_id, context_id));

    BCM_IF_ERROR_RETURN(bcm_field_group_delete(unit, cint_lif_fg_id));

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
int cint_field_lif_qual_destroy(int unit, bcm_field_context_t context_id)
{
    BCM_IF_ERROR_RETURN(bcm_field_entry_delete(unit, cint_lif_qual_fg_id, NULL, cint_lif_qual_ent_id));

    BCM_IF_ERROR_RETURN(bcm_field_group_context_detach(unit, cint_lif_qual_fg_id, context_id));

    BCM_IF_ERROR_RETURN(bcm_field_group_delete(unit, cint_lif_qual_fg_id));

    return BCM_E_NONE;
}