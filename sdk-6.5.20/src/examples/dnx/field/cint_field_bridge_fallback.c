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
 * cint ../../../src/examples/dnx/utility/cint_dnx_util_rch.c
 * cint ../../../src/examples/dnx/field/cint_field_bridge_fallback.c
 * cint;
 * int unit = 0;
 * int recycle_port = 40;
 * cint_field_bridge_fallback_main(unit);
 * int recycle_entry_encap_id;
 * int vsi_profile = 1; 
 * create_recycle_entry(unit, &recycle_entry_encap_id); 
 * cint_field_bridge_fallback_entry_add(unit, recycle_port, vsi_profile, BCM_L3_ITF_VAL_GET(recycle_entry_encap_id));
 * 
 * Configuration example end
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

bcm_field_entry_t  cint_bridge_fallback_entry_id;

/** 
 * \brief 
 * indicate if unknown destination qualifier is supported  
 */ 
int unknown_destination_qualifier_is_supported(int unit) {
    int rv; 
    bcm_field_qualifier_info_get_t qual_info;
    bcm_field_qualifier_info_get_t_init(&qual_info);
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyPacketRes, bcmFieldStageIngressPMF1, &qual_info);
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
    int rv; 
    rv = cint_field_bridge_fallback_select_unknown_dest_and_l3_mc_packets_and_vis_profile_update_lif_and_port(unit);
    if (rv != BCM_E_NONE) {
        printf("Error (%d), in cint_field_bridge_fallback_select_unknown_dest_and_l3_mc_packets_and_vis_profile_update_lif_and_port\n", rv);
        return rv;
    }
    rv = cint_field_bridge_fallback_update_vsi_with_lif_and_pop_lif(unit);
    if (rv != BCM_E_NONE) {
        printf("Error (%d), in cint_field_bridge_fallback_update_vsi_with_lif_and_pop_lif\n", rv);
        return rv;
    }

    return 0;
}

/*
 * A destroy function to clear the configuration made by cint_field_bridge_fallback_main
 */
int cint_field_bridge_fallback_main_destroy (int unit)
{
    int rv = 0;
    bcm_field_stage_t stage = bcmFieldStageIngressPMF1;
    bcm_field_presel_t presel_ipv4 = 50;
    bcm_field_presel_t presel_ipv6 = 51;

    rv = bcm_field_group_context_detach(unit, bridge_fallback_fg_id_casc, bridge_fallback_context_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_group_context_detach\n", rv);
       return rv;
    }

    rv = bcm_field_group_delete(unit, bridge_fallback_fg_id_casc);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    rv = bcm_field_qualifier_destroy(unit, bridge_fallback_fwd_domain_qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    rv = bcm_field_qualifier_destroy(unit, bridge_fallback_is_pmf_hit_duplicate);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_qualifier_create\n", rv);
       return rv;
    }

    rv = bcm_field_qualifier_destroy(unit, bridge_fallback_is_pmf_hit);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_qualifier_create\n", rv);
       return rv;
    }

    rv = bcm_field_group_context_detach(unit, bridge_fallback_fg_id, bridge_fallback_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach\n", rv);
        return rv;
    }
    rv = bcm_field_group_delete(unit, bridge_fallback_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    cint_field_bridge_fallback_set_preselector_ipv4(unit, stage);
    cint_field_bridge_fallback_set_preselector_ipv6(unit, stage);

    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t p_id;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = bridge_fallback_context_id;
    p_id.stage = stage;
    p_id.presel_id = presel_ipv4;
    rv = bcm_field_presel_set(unit, 0, &p_id, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_presel_set\n");
        return rv;
    }

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = bridge_fallback_context_id;
    p_id.stage = stage;
    p_id.presel_id = presel_ipv6;
    rv = bcm_field_presel_set(unit, 0, &p_id, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_presel_set\n");
        return rv;
    }
    rv = bcm_field_context_destroy(unit, stage, bridge_fallback_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy\n", rv);
        return rv;
    }

    rv = bcm_field_action_destroy(unit, ipmf_1_action_type_hit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_destroy\n", rv);
        return rv;
    }

    return 0;
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
    int rv = BCM_E_NONE;
    bcm_field_action_info_t action_info; 

    bcm_field_stage_t stage = bcmFieldStageIngressPMF1;

    bcm_field_action_info_t_init(&action_info);
    action_info.stage = stage;
    action_info.size = 1;
    action_info.action_type = bcmFieldActionVoid;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "action_type_hit", sizeof(action_info.name));
    bcm_field_action_create(unit, 0, &action_info, ipmf_1_action_type_hit);

    bcm_field_context_info_t context_info;
    bcm_field_context_info_t_init(&context_info);
    bcm_field_context_create(unit, 0, stage, &context_info, &bridge_fallback_context_id);

    cint_field_bridge_fallback_set_preselector_ipv4(unit, stage);
    cint_field_bridge_fallback_set_preselector_ipv6(unit, stage);

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
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &bridge_fallback_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
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
    
    rv = bcm_field_group_context_attach(unit, 0, bridge_fallback_fg_id, bridge_fallback_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached  FG (%d) to context (%d)\n",bridge_fallback_fg_id, bridge_fallback_context_id);

    return 0;
}

int cint_field_bridge_fallback_set_preselector_ipv4(int unit, bcm_field_stage_t stage) {
    int rv = BCM_E_NONE;
    bcm_field_presel_t presel_ipv4 = 50;
    bcm_field_presel_entry_id_t p_id = get_ipv4_ipv6_presel_entry_id(stage);
    p_id.presel_id = presel_ipv4;
    bcm_field_presel_entry_data_t p_data = get_ipv4_ipv6_presel_entry_data();
    p_data.qual_data[0].qual_value = bcmFieldAppTypeIp4McastRpf;
    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_presel_set \n", rv);
        return rv;
    }
    return rv;
}

int cint_field_bridge_fallback_set_preselector_ipv6(int unit, bcm_field_stage_t stage) {
    int rv = BCM_E_NONE;
    bcm_field_presel_t presel_ipv6 = 51;
    bcm_field_presel_entry_id_t p_id = get_ipv4_ipv6_presel_entry_id(stage);
    p_id.presel_id = presel_ipv6;
    bcm_field_presel_entry_data_t p_data = get_ipv4_ipv6_presel_entry_data();
    p_data.qual_data[0].qual_value = bcmFieldAppTypeIp6McastRpf;
    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_presel_set \n", rv);
        return rv;
    }
    return rv;
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


int cint_field_bridge_fallback_entry_add(int unit, uint32 destination_port, int vsi_profile, uint32 outlif)
{
    int rv = 0;
    bcm_field_entry_info_t ent_info;

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;


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

    ent_info.nof_entry_actions = 3;
    ent_info.entry_action[0].type = update_destination_action;
    ent_info.entry_action[0].value[0] = destination_port;
    ent_info.entry_action[1].type = update_outlif_action;
    ent_info.entry_action[1].value[0] = outlif;
    ent_info.entry_action[2].type = ipmf_1_action_type_hit;
    ent_info.entry_action[2].value[0] = TRUE;

    rv = bcm_field_entry_add(unit, 0, bridge_fallback_fg_id, &ent_info, &cint_bridge_fallback_entry_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    else {
        printf("Entry add: id:(0x%x) destination:(%d), lif: %d \n",cint_bridge_fallback_entry_id, destination_port, outlif);
    }
    return 0;
}

/** Delete the field entry used for bridge fallback. */
int cint_field_bridge_fallback_entry_destroy(int unit, int vsi_profile)
{
    int rv = 0;

    rv = bcm_field_entry_delete(unit, bridge_fallback_fg_id, NULL, cint_bridge_fallback_entry_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_delete\n", rv);
       return rv;
    }
    return 0;
}

/** 
 * In case qualifiers from cint_field_bridge_fallback_main are hit, this direct extraction will be performed: 
 * update vsi with lif[0] 
 * pop lif: lif[0] = lif[1] 
 */ 
int cint_field_bridge_fallback_update_vsi_with_lif_and_pop_lif(int unit) {

    void *dest_char;
    int rv = 0;

    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualifier_info_get_t qual_info_get;

    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    int ii; 



    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "is_pmf_hit", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &bridge_fallback_is_pmf_hit);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_qualifier_create\n", rv);
       return rv;
    }

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "is_pmf_hit_duplicate", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &bridge_fallback_is_pmf_hit_duplicate);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_qualifier_create\n", rv);
       return rv;
    }

    /**bcmFieldQualifyVrf = bcmFieldActionVSwitchNew = FWD_Domain*/
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyVrf, bcmFieldStageIngressPMF1, &qual_info_get);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_qualifier_info_get\n", rv);
       return rv;
    }

    qual_info.size = qual_info_get.size;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "BRIDGE_FALLBACK_FWD_DOMAIN", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &bridge_fallback_fwd_domain_qual_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_qualifier_create\n", rv);
       return rv;
    }

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
    rv = bcm_field_group_add(unit, 0, &fg_info, &bridge_fallback_fg_id_casc);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_group_add\n", rv);
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
    }

    /**Get the offset of GLOB_IN_LIF_0*/
    rv = bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyInVPort0, bcmFieldStageIngressPMF2, &qual_info_get);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_qualifier_info_get\n", rv);
       return rv;
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeCascaded;
    attach_info.key_info.qual_info[0].input_arg = bridge_fallback_fg_id;
    bcm_field_group_action_offset_get(unit, 0, bridge_fallback_fg_id, ipmf_1_action_type_hit, &(attach_info.key_info.qual_info[0].offset));
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].offset = qual_info_get.offset;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeCascaded;
    attach_info.key_info.qual_info[2].input_arg = bridge_fallback_fg_id;
    bcm_field_group_action_offset_get(unit, 0, bridge_fallback_fg_id, ipmf_1_action_type_hit, &(attach_info.key_info.qual_info[2].offset));
    
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeMetaData;

    rv = bcm_field_group_context_attach(unit, 0, bridge_fallback_fg_id_casc, bridge_fallback_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_group_context_attach\n", rv);
       return rv;
    }
    return 0;

}
