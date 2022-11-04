 /*
 * Configuration example start:
 *  
 * Test Scenario - start
 * cint;                                                                  
 * cint_reset();                                                          
 * exit;
  cint ../../../../src/examples/dnx/utility/cint_dnx_util_rch.c
  cint ../../../../src/examples/dnx/field/cint_field_bridge_fallback.c
  cint;
  int unit = 0;
  int recycle_port_core_0 = 40;
  int recycle_port_core_1 = 41;
  cint_field_bridge_fallback_main(unit);
  int recycle_entry_encap_id;
  int vsi_profile = 1; 
  cint_field_bridge_fallback_entry_add(unit, nof_cores, recycle_port_core_0, recycle_port_core_1, vsi_profile);
 * Test Scenario - end
 * 
 * Configuration example end
 *  
 */

bcm_field_group_t bridge_fallback_fg_id=0;
bcm_field_group_t bridge_fallback_fg_id_casc=0;
bcm_field_qualify_t bridge_fallback_fwd_domain_qual_id;
bcm_field_qualify_t bridge_fallback_is_pmf_hit;
bcm_field_qualify_t bridge_fallback_is_pmf_hit_duplicate;

bcm_field_context_t bridge_fallback_context_id= BCM_FIELD_CONTEXT_ID_INVALID;

/** qualifiers */
bcm_field_qualify_t multicast_compatible_qualifier = bcmFieldQualifyIpMulticastCompatible;
bcm_field_qualify_t vsi_profile_qualifier = bcmFieldQualifyInterfaceClassL2;
/* qualifier is not supported on j2a0 only.
   see unknown_destination_qualifier_is_supported */
bcm_field_qualify_t unknown_destination_qualifier = bcmFieldQualifyPacketRes;

/** actions */
bcm_field_action_t update_destination_action = bcmFieldActionRedirect;
bcm_field_action_t update_outlif_action = bcmFieldActionOutVport0Raw;

bcm_field_action_t ipmf_1_action_type_hit;

bcm_field_entry_t  cint_bridge_fallback_entry_id[2];

bcm_field_presel_t cint_bridge_fallback_presel_ipv4 = 50;
bcm_field_presel_t cint_bridge_fallback_presel_ipv6 = 51;

/** 
 * \brief 
 * indicate if unknown destination qualifier is supported  
 */ 
int unknown_destination_qualifier_is_supported(int unit) {
    int rv; 
    bcm_field_qualifier_info_get_t qual_info;
    bcm_field_qualifier_info_get_t_init(&qual_info);
    /*In case feature not supported we dont want to see unrelevant error logs*/
    bshell(unit, "debug bcmdnx fldprocdnx off");
    rv =bcm_field_qualifier_info_get(unit, bcmFieldQualifyPacketRes, bcmFieldStageIngressPMF1, &qual_info);
    bshell(unit, "debug bcmdnx fldprocdnx warn");
    return rv == BCM_E_NONE;
}

/**
* \brief 
*  At PMF1:
*  Preselector:
*  is ipv4 MC or ipv6 MC
*  Qualifiers:
*  1) destination unknown,
*  2) compabile mc
*  3) vsi profile
*  Action:
*  fwd to recycle port and to rcy outlif
*  
*  At PMF2, when PMF1 above was hit:
*  update vsi with lif and pop the lif 
*  
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_bridge_fallback_main(int unit)
{
    BCM_IF_ERROR_RETURN_MSG(cint_field_bridge_fallback_select_unknown_dest_and_l3_mc_packets_and_vis_profile_update_lif_and_port(unit), "");

    BCM_IF_ERROR_RETURN_MSG(cint_field_bridge_fallback_update_vsi_with_lif_and_pop_lif(unit), "");

    return BCM_E_NONE;
}

/*
 * A destroy function to clear the configuration made by cint_field_bridge_fallback_main
 */
int cint_field_bridge_fallback_main_destroy (int unit)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t p_id;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, bridge_fallback_fg_id_casc, bridge_fallback_context_id), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, bridge_fallback_fg_id_casc), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_qualifier_destroy(unit, bridge_fallback_fwd_domain_qual_id), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_qualifier_destroy(unit, bridge_fallback_is_pmf_hit_duplicate), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_qualifier_destroy(unit, bridge_fallback_is_pmf_hit), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, bridge_fallback_fg_id, bridge_fallback_context_id), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, bridge_fallback_fg_id), "");

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = bridge_fallback_context_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_id.presel_id = cint_bridge_fallback_presel_ipv4;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(unit, 0, &p_id, &presel_entry_data), "");


    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = bridge_fallback_context_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_id.presel_id = cint_bridge_fallback_presel_ipv6;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(unit, 0, &p_id, &presel_entry_data), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, bridge_fallback_context_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_action_destroy(unit, ipmf_1_action_type_hit), "");


    return BCM_E_NONE;
}

/**
* \brief 
*  Preselector:
*  is ipv4 MC or ipv6 MC
*  Qualifiers:
*  1) destination unknown,
*  2) compabile mc
*  Action:
*  fwd to recycle port and to rcy outlif 
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_bridge_fallback_select_unknown_dest_and_l3_mc_packets_and_vis_profile_update_lif_and_port(int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;

    void *dest_char;
    bcm_field_action_info_t action_info; 

    bcm_field_stage_t stage = bcmFieldStageIngressPMF1;
    bcm_field_context_param_info_t param_info;

    bcm_field_action_info_t_init(&action_info);
    action_info.stage = stage;
    action_info.size = 1;
    action_info.action_type = bcmFieldActionVoid;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "action_type_hit", sizeof(action_info.name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_action_create(unit, 0, &action_info, ipmf_1_action_type_hit), "");

    bcm_field_context_info_t context_info;
    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s(dest_char, "BRIDGE_FALLBACK_CTX", sizeof(context_info.name));

    BCM_IF_ERROR_RETURN_MSG(bcm_field_context_create(unit, 0, stage, &context_info, &bridge_fallback_context_id), "");

    bcm_field_context_param_info_t_init(&param_info);
    /** Context in IPMF2 stage will configure bytes to remove 1 layer. */
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerOffset1, 0);
    BCM_IF_ERROR_RETURN_MSG(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, bridge_fallback_context_id, &param_info), "");

    BCM_IF_ERROR_RETURN_MSG(cint_field_bridge_fallback_set_preselector_ipv4(unit, stage), "");
    BCM_IF_ERROR_RETURN_MSG(cint_field_bridge_fallback_set_preselector_ipv6(unit, stage), "");

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = stage;

    /* Set quals */
    fg_info.nof_quals = 3;
    fg_info.qual_types[0] = multicast_compatible_qualifier;
    fg_info.qual_types[1] = unknown_destination_qualifier;
    fg_info.qual_types[2] = vsi_profile_qualifier;

    /* Set actions */
    fg_info.nof_actions = 3;
    fg_info.action_types[0] = update_destination_action;
    fg_info.action_types[1] = update_outlif_action;
    fg_info.action_types[2] = ipmf_1_action_type_hit;
    fg_info.action_with_valid_bit[2] = FALSE;


    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "bridge_fallback", sizeof(fg_info.name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &fg_info, &bridge_fallback_fg_id), "");

    printf("Created TCAM field group (%d) in iPMF1 \n", bridge_fallback_fg_id);


    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0]; 
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = BCM_FIELD_INVALID;
    attach_info.key_info.qual_info[0].offset = 0;
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].input_arg = BCM_FIELD_INVALID;
    attach_info.key_info.qual_info[1].offset = 0;
    attach_info.key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[2].input_arg = 0;
    attach_info.key_info.qual_info[2].offset = 0;

    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    attach_info.payload_info.action_types[2] = fg_info.action_types[2];
    
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, bridge_fallback_fg_id, bridge_fallback_context_id, &attach_info), "");

    printf("Attached  FG (%d) to context (%d)\n",bridge_fallback_fg_id, bridge_fallback_context_id);

    return BCM_E_NONE;
}

int cint_field_bridge_fallback_set_preselector_ipv4(int unit, bcm_field_stage_t stage) {
    bcm_field_presel_entry_id_t p_id = get_ipv4_ipv6_presel_entry_id(stage);
    p_id.presel_id = cint_bridge_fallback_presel_ipv4;
    bcm_field_presel_entry_data_t p_data = get_ipv4_ipv6_presel_entry_data();
    p_data.qual_data[0].qual_value = bcmFieldAppTypeIp4McastRpf;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(unit, 0, &p_id, &p_data), "");

    return BCM_E_NONE;
}

int cint_field_bridge_fallback_set_preselector_ipv6(int unit, bcm_field_stage_t stage) {

    bcm_field_presel_entry_id_t p_id = get_ipv4_ipv6_presel_entry_id(stage);
    p_id.presel_id = cint_bridge_fallback_presel_ipv6;
    bcm_field_presel_entry_data_t p_data = get_ipv4_ipv6_presel_entry_data();
    p_data.qual_data[0].qual_value = bcmFieldAppTypeIp6McastRpf;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(unit, 0, &p_id, &p_data), "");

    return BCM_E_NONE;
}

/* get preselect entry id with properties common to ipv4 and ipv6 presel entry id */
bcm_field_presel_entry_id_t get_ipv4_ipv6_presel_entry_id(bcm_field_stage_t stage) {
    bcm_field_presel_entry_id_t p_id;
    bcm_field_presel_entry_id_info_init(&p_id);
    p_id.stage = stage;
    return p_id; 
}

/* get preselect entry data with properties common to ipv4 and ipv6 presel entry data */
bcm_field_presel_entry_data_t get_ipv4_ipv6_presel_entry_data() {
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_data_info_init(&p_data);
    p_data.entry_valid = TRUE;
    p_data.context_id = bridge_fallback_context_id;
    p_data.nof_qualifiers = 1;
    p_data.qual_data[0].qual_type = bcmFieldQualifyAppTypePredefined;
    p_data.qual_data[0].qual_arg = 0; /*fwd layer */
    p_data.qual_data[0].qual_mask = 0x1F;
    return p_data;
}


int cint_field_bridge_fallback_entry_add(int unit, int nof_cores,uint32 rcy_port_core_0, uint32 rcy_port_core_1, int vsi_profile)
{
    bcm_field_entry_info_t ent_info;
    bcm_gport_t gport_rcy[2];
    int core_id;
    uint32 valid_cores_bmp = *(dnxc_data_get(unit, "device", "general", "valid_cores_bitmap", NULL));

    gport_rcy[0] = rcy_port_core_0;
    gport_rcy[1] = rcy_port_core_1;

    for (core_id = 0; core_id < nof_cores; core_id ++) {
        if ((valid_cores_bmp & (1 << core_id)) == 0)
        {
            continue;
        }
        bcm_field_entry_info_t_init(&ent_info);
        ent_info.priority = 1;
        ent_info.core = core_id;

        ent_info.nof_entry_quals = 3;
        ent_info.entry_qual[0].type = multicast_compatible_qualifier;
        ent_info.entry_qual[0].value[0] = 1;
        ent_info.entry_qual[0].mask[0] = -1;
        ent_info.entry_qual[1].type = unknown_destination_qualifier;
        ent_info.entry_qual[1].value[0] = 1;
        ent_info.entry_qual[1].mask[0] = -1;
        ent_info.entry_qual[2].type = vsi_profile_qualifier;
        ent_info.entry_qual[2].value[0] = vsi_profile;
        ent_info.entry_qual[2].mask[0] = 0xf;

        ent_info.nof_entry_actions = 2;
        ent_info.entry_action[0].type = update_destination_action;
        ent_info.entry_action[0].value[0] = gport_rcy[core_id];
        ent_info.entry_action[1].type = ipmf_1_action_type_hit;
        ent_info.entry_action[1].value[0] = TRUE;

        BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, bridge_fallback_fg_id, &ent_info, &cint_bridge_fallback_entry_id[core_id]), "");

        printf("Entry add: id:(0x%x) destination:(%d)\n",cint_bridge_fallback_entry_id[core_id], gport_rcy[core_id]);
    }

    return BCM_E_NONE;
}

/** Delete the field entry used for bridge fallback. */
int cint_field_bridge_fallback_entry_destroy(int unit, int nof_cores, int vsi_profile)
{
    int core_id = 0;
    uint32 valid_cores_bmp = *(dnxc_data_get(unit, "device", "general", "valid_cores_bitmap", NULL));

    for (core_id = 0;core_id < nof_cores;core_id++) {
        if ((valid_cores_bmp & (1 << core_id)) == 0)
        {
            continue;
        }
        BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, bridge_fallback_fg_id, NULL, cint_bridge_fallback_entry_id[core_id]), "");
    }
    return BCM_E_NONE;
}

/** 
 * In case qualifiers from cint_field_bridge_fallback_main are hit, this direct extraction will be performed: 
 * update vsi with lif[0] 
 * pop lif: lif[0] = lif[1] 
 *  
 */ 
int cint_field_bridge_fallback_update_vsi_with_lif_and_pop_lif(int unit) {

    void *dest_char;

    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualifier_info_get_t qual_info_get;

    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    int ii; 



    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "is_pmf_hit", sizeof(qual_info.name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_qualifier_create(unit, 0, &qual_info, &bridge_fallback_is_pmf_hit), "");

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "is_pmf_hit_duplicate", sizeof(qual_info.name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_qualifier_create(unit, 0, &qual_info, &bridge_fallback_is_pmf_hit_duplicate), "");

    /**bcmFieldQualifyVrf = bcmFieldActionVSwitchNew = FWD_Domain*/
    BCM_IF_ERROR_RETURN_MSG(bcm_field_qualifier_info_get(unit, bcmFieldQualifyVrf, bcmFieldStageIngressPMF1, &qual_info_get), "");

    qual_info.size = qual_info_get.size;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "BRIDGE_FALLBACK_FWD_DOMAIN", sizeof(qual_info.name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_qualifier_create(unit, 0, &qual_info, &bridge_fallback_fwd_domain_qual_id), "");

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;

    /*
     * Set quals
     */
    fg_info.nof_quals = 4;
    fg_info.qual_types[0] = bridge_fallback_is_pmf_hit;
    fg_info.qual_types[1] = bridge_fallback_fwd_domain_qual_id;
    fg_info.qual_types[2] = bridge_fallback_is_pmf_hit_duplicate;
    fg_info.qual_types[3] = bcmFieldQualifyInVPort1;

    /*
     * Set actions
     */
    fg_info.nof_actions = 2;
    fg_info.action_with_valid_bit[0] = TRUE;
    fg_info.action_types[0] = bcmFieldActionVSwitchNew;
    fg_info.action_with_valid_bit[1] = TRUE;
    fg_info.action_types[1] = bcmFieldActionInVport0;
    

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "J2_UPDATE_VSI_WITH_LIF_AND_POP_LIF_FG", sizeof(fg_info.name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &fg_info, &bridge_fallback_fg_id_casc), "");


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
    }

    /**Get the offset of GLOB_IN_LIF_0*/
    BCM_IF_ERROR_RETURN_MSG(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyInVPort0, bcmFieldStageIngressPMF2, &qual_info_get), "");


    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeCascaded;
    attach_info.key_info.qual_info[0].input_arg = bridge_fallback_fg_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_action_offset_get(unit, 0, bridge_fallback_fg_id, ipmf_1_action_type_hit, &(attach_info.key_info.qual_info[0].offset)), "");
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].offset = qual_info_get.offset;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeCascaded;
    attach_info.key_info.qual_info[2].input_arg = bridge_fallback_fg_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_action_offset_get(unit, 0, bridge_fallback_fg_id, ipmf_1_action_type_hit, &(attach_info.key_info.qual_info[2].offset)), "");
    
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeMetaData;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, bridge_fallback_fg_id_casc, bridge_fallback_context_id, &attach_info), "");

    return BCM_E_NONE;

}
