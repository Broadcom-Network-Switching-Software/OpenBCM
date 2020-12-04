/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 *
 *   This cint add support of PMF for RFC8321 on SRv6
 *
 *   For ingress node:
 *     Note that ipmf2 TCAM should be created first in configuration in order other functions to work
 *     For example configuration look at:
 *     srv6_alternate_marking_ingress_node_field()
 *
 *    1) ipmf2 TCAM configuration - the context selection is done when Forwarding layer is IP
 *       The qualifiers are 5-tuple, including SIP, DIP, IP protocol, SPort and Dport
 *       The action is Container, used to classify the packet to a flow. The flow ID will be used in following iPMF3
 *
 *    2) KeyGenVar representation:
 *       The sixth qualifier is a 32 bit KeyGenVar register where all but one bit is masked.
 *       Two iPMF entries should be added per AM session. The differences being
 *          1. The key - The unmasked bit in KeyGenVar. Add one entry with 0 and one with 1.
 *          2. The action - each should be assigned a different counter
 *       Alternating between the L bits is done through cint_field_srv6_alternate_marking_period_change(). Provide the new L bit and the profile and the
 *
 *       For example when using profile 4, set KeyGenVar_profile to 4 and call cint_field_srv6_alternate_marking_ingress_node_iPMF2_entry_add() twice with the same values, except once with
 *       g_cint_field_srv6_am_ing_ent.KeyGenVar_period = 8 (bit 4 is set) and once  g_cint_field_srv6_am_ing_ent.KeyGenVar_period = 0.
 *
 *    3) ipmf3 TCAM configuration - the context selection is done when Forwarding layer is IP
 *       The qualifiers are flow ID(result in ipmf2) and KeyGenVar
 *       The action includes load balance key, 2 counters Stat_id+Profile and oam subtype
 *
 *    4) ePMF TCAM configuration and ACE configuration
 *       For Srv6 one pass solution
 *          ePMF qualify on ASE extension the subtype should be 11 for RFC8321 and the counter should be 0
 *       For Srv6 extended solution
 *          ePMF qualify on D bit in FTMH LB key extension header
 *       ePMF action is to:
 *          - Snoop Id and Snoop Strength to send the packet to CPU
 */

bcm_field_group_t cint_field_srv6_am_ipmf2_ing_node_fg_id=0;
bcm_field_context_t cint_field_srv6_am_ipmf2_ing_node_ctx_id;
bcm_field_presel_t cint_field_srv6_am_ipmf2_ing_node_presel_id = 5;
bcm_field_group_info_t cint_field_srv6_am_ipmf2_ing_node_fg_info;
bcm_field_action_t cint_field_srv6_am_user_act_id;

bcm_field_group_t cint_field_srv6_am_ipmf3_ing_node_fg_id=0;
bcm_field_context_t cint_field_srv6_am_ipmf3_ing_node_ctx_id;
bcm_field_presel_t cint_field_srv6_am_impf3_ing_node_presel_id = 5;
bcm_field_group_info_t cint_field_srv6_am_ipmf3_ing_node_fg_info;
bcm_field_qualify_t cint_field_srv6_am_user_qual_sip_id;
bcm_field_qualify_t cint_field_srv6_am_user_qual_dip_id;

bcm_field_context_t cint_field_srv6_am_egress_ctx_id;
bcm_field_presel_t cint_field_srv6_am_egress_presel_id = 5;
bcm_field_group_t cint_field_srv6_am_egress_fg_id=0;
bcm_field_entry_t cint_field_srv6_am_egress_entr_id;

/**Used for setting values of ePMF entry in Ingress node*/
uint32 cint_field_srv6_am_egress_snoop_id = 1;
uint32 cint_field_srv6_am_egress_snoop_str = 4;

/** L bit configuration for each KeyGenVar profile.
 *  Initialize with 0. Should only be set through
 *  cint_field_srv6_alternate_marking_period_change() */
uint32 g_KeyGenVar_period_profiles = 0;

struct cint_field_am_stat_profile_s {
    int is_meter; /* Is meter acts as "counter-increment" indication. Must be set to 1 */
    int is_lm; /* AM latches on to LM counting mechanism - is_lm must be set to one*/
    int type_id;
    int valid;
};


struct cint_field_srv6_am_ingress_node_entry_val_s{
     uint32 SrcIp;
     uint32 DstIp;
     uint32 SrcIp6Low[2];
     uint32 DstIp6Low[2];
     uint32 ProtocolType;
     uint32 L4Dst;
     uint32 L4Src;
     uint32 KeyGenVar_profile; /* Each entry must be associated with one of 32 KeyGenProfiles, each represents the L bit configuration.
     L bit configuration may be changed per profile through  cint_field_srv6_alternate_marking_period_change() */
     uint32 KeyGenVar_period;

     uint32 statId0; /*This represents the counter used in the engine.*/
     cint_field_am_stat_profile_s statProfile0; /* Must be used with the values set below*/
     uint32 command_id; /* Counter command ID - must be 7,8 or 9*/
     uint32 l_bit_set;
     int flow_id_2_lsb_in_trunk_hash;
     int flow_id_16_msb_in_network_hash;
     uint32 oam_raw;
     int    is_ext_encap;
     int    nof_sid_1st_pass;
     int    egr_port_pmf_data;
};

cint_field_srv6_am_ingress_node_entry_val_s g_cint_field_srv6_am_ing_ent = {
        0xc0800101,
        0x7fffff02,
        {0x33334444, 0x11112222},
        {0xFF00FF13, 0x1234F000},
        6,         /* TCP */
        9000,      /* L4Dst */
        8000,      /* L4Src */
        1,         /* KeyGenVar_period */
        1,         /* period profile */
        7,         /* Stat-id represents the counter itself */
        {1,1,0,1}, /* stat profile - all 1 except type which is 0 */
        7,         /* Counter command ID */
        1,         /* l_bit_set */
        0,         /* flow_id_2_lsb_in_trunk_hash */
        0,         /* flow_id_16_msb_in_network_hash */
        /* OAM action 
         * Use Sub-type 0xB for ePMF qualifier.
 */
        0xB0000,
        0,         /* is_ext_encap */
        3,         /* nof_sid_1st_pass */
        /*
         * A pmf data per port, user can set it to same value for all out port of Srv6 ingress node.
         * This data is used to indicates packet is in the last pass for Srv6 entended encapsulation.
         * This value can be in range of [1,7].
         */
        0x3
};

int cint_field_srv6_alternate_marking_ingress_node_iPMF2_entry_add(int unit, bcm_field_entry_t *entry_handle)
{
    bcm_field_entry_info_t ent_info;
    int idx;
    int rv;

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.nof_entry_actions = cint_field_srv6_am_ipmf2_ing_node_fg_info.nof_actions;

    for(idx = 0 ; idx < ent_info.nof_entry_actions; idx++ )
    {
        ent_info.entry_action[idx].type = cint_field_srv6_am_ipmf2_ing_node_fg_info.action_types[idx];
    }

    /* L-bit(bit17) and D-bit(bit16). 16LSB is used for flow-ID. */
    ent_info.entry_action[0].value[0] = cint_srv6_am_flow_id;

    ent_info.nof_entry_quals = cint_field_srv6_am_ipmf2_ing_node_fg_info.nof_quals;
    for(idx = 0 ; idx < ent_info.nof_entry_quals; idx++ )
    {
        ent_info.entry_qual[idx].type = cint_field_srv6_am_ipmf2_ing_node_fg_info.qual_types[idx];
        ent_info.entry_qual[idx].mask[0] = -1;
    }
    if (cint_srv6_am_fwd_type == 0) {
        ent_info.entry_qual[0].value[0] = g_cint_field_srv6_am_ing_ent.SrcIp;
        ent_info.entry_qual[1].value[0] = g_cint_field_srv6_am_ing_ent.DstIp;
    } else {
        ent_info.entry_qual[0].value[0] = g_cint_field_srv6_am_ing_ent.SrcIp6Low[0];
        ent_info.entry_qual[0].value[1] = g_cint_field_srv6_am_ing_ent.SrcIp6Low[1];
        ent_info.entry_qual[0].mask[0] = 0xFFFFFFFF;
        ent_info.entry_qual[0].mask[1] = 0xFFFFFFFF;
        ent_info.entry_qual[1].value[0] = g_cint_field_srv6_am_ing_ent.DstIp6Low[0];
        ent_info.entry_qual[1].value[1] = g_cint_field_srv6_am_ing_ent.DstIp6Low[1];
        ent_info.entry_qual[1].mask[0] = 0xFFFFFFFF;
        ent_info.entry_qual[1].mask[1] = 0xFFFFFFFF;
    }
    ent_info.entry_qual[2].value[0] = g_cint_field_srv6_am_ing_ent.ProtocolType;
    ent_info.entry_qual[3].value[0] = g_cint_field_srv6_am_ing_ent.L4Dst;
    ent_info.entry_qual[4].value[0] = g_cint_field_srv6_am_ing_ent.L4Src;

    rv = bcm_field_entry_add(unit,0,cint_field_srv6_am_ipmf2_ing_node_fg_id,&ent_info,entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }

    printf("ipmf2 FG(%d) entry_add 0x(%x): \n SrcIp(0x%x) DstIp(0x%x) ProtocolType(%d) L4Dst(%d) L4Src(%d) KeyGenVar_period(0x%x) \n statId0(%d) statProfile0(0x%x) outlif_0(0x%x) oam_raw(0x%x)\n\n",
            cint_field_srv6_am_ipmf2_ing_node_fg_id, (*entry_handle),
          ent_info.entry_qual[0].value[0],ent_info.entry_qual[1].value[0],ent_info.entry_qual[2].value[0],ent_info.entry_qual[3].value[0],ent_info.entry_qual[4].value[0],ent_info.entry_qual[5].value[0],
          ent_info.entry_action[0].value[0],ent_info.entry_action[1].value[0],ent_info.entry_action[2].value[0],ent_info.entry_action[3].value[0]);


    return 0;
}

int cint_field_srv6_alternate_marking_ingress_node_iPMF3_entry_add(int unit, bcm_field_entry_t *entry_handle)
{
    bcm_field_entry_info_t ent_info;
    int idx;
    int lm_read_index;
    int flow_id_w_L_bit = 0;
    int rv;

    flow_id_w_L_bit = cint_srv6_am_flow_id + (g_cint_field_srv6_am_ing_ent.l_bit_set << 17);
    g_cint_field_srv6_am_ing_ent.flow_id_2_lsb_in_trunk_hash = flow_id_w_L_bit & 0x3;
    g_cint_field_srv6_am_ing_ent.flow_id_16_msb_in_network_hash = flow_id_w_L_bit >> 2;

    /* command-id 7 maps to lm_read_index 0, command-id 8 to lm_read_index 1, command-id 9 to lm_read_index 2,  */
    lm_read_index = bcmFieldStatOamLmIndex0;

    bcm_field_entry_info_t_init(&ent_info);

    ent_info.nof_entry_actions = cint_field_srv6_am_ipmf3_ing_node_fg_info.nof_actions;

    for(idx = 0 ; idx < ent_info.nof_entry_actions; idx++ )
    {
        ent_info.entry_action[idx].type = cint_field_srv6_am_ipmf3_ing_node_fg_info.action_types[idx];
    }

    ent_info.entry_action[0].value[0] = g_cint_field_srv6_am_ing_ent.flow_id_2_lsb_in_trunk_hash;     /* Action 0 - TrunkHashKeySet */
    ent_info.entry_action[1].value[0] = g_cint_field_srv6_am_ing_ent.flow_id_16_msb_in_network_hash;  /* Action 1 - NetworkLoadBalanceKey */
    ent_info.entry_action[2].value[0] = g_cint_field_srv6_am_ing_ent.statId0;
    ent_info.entry_action[3].value[0] = g_cint_field_srv6_am_ing_ent.statProfile0.is_meter; /* Action 1 - stat profile*/
    ent_info.entry_action[3].value[1] = g_cint_field_srv6_am_ing_ent.statProfile0.is_lm;    /* Action 1 - stat profile*/
    ent_info.entry_action[3].value[2] = g_cint_field_srv6_am_ing_ent.statProfile0.type_id;  /* Action 1 - stat profile*/
    ent_info.entry_action[3].value[3] = g_cint_field_srv6_am_ing_ent.statProfile0.valid;    /* Action 1 - stat profile*/
    ent_info.entry_action[4].value[0] = g_cint_field_srv6_am_ing_ent.oam_raw;
    ent_info.entry_action[5].value[0] = lm_read_index;

    ent_info.nof_entry_quals = cint_field_srv6_am_ipmf3_ing_node_fg_info.nof_quals;
    for(idx = 0 ; idx < ent_info.nof_entry_quals; idx++ )
    {
        ent_info.entry_qual[idx].type = cint_field_srv6_am_ipmf3_ing_node_fg_info.qual_types[idx];
        ent_info.entry_qual[idx].mask[0] = -1;
    }

    ent_info.entry_qual[0].value[0] = cint_srv6_am_flow_id;                                 /* cint_field_srv6_am_user_qual_sip_id */
    ent_info.entry_qual[1].value[0] = g_cint_field_srv6_am_ing_ent.KeyGenVar_period;
    ent_info.entry_qual[1].mask[0] &= ~(1<<g_cint_field_srv6_am_ing_ent.KeyGenVar_profile); /* Unmask "key-gen-profile" bit.*/

    rv = bcm_field_entry_add(unit,0,cint_field_srv6_am_ipmf3_ing_node_fg_id,&ent_info,entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }

    printf("iPMF3 FG(%d) entry_add 0x(%x): \n SrcIp(0x%x) DstIp(0x%x) ProtocolType(%d) L4Dst(%d) L4Src(%d) KeyGenVar_period(0x%x) \n statId0(%d) statProfile0(0x%x) outlif_0(0x%x) oam_raw(0x%x)\n\n",
            cint_field_srv6_am_ipmf3_ing_node_fg_id, (*entry_handle),
          ent_info.entry_qual[0].value[0],ent_info.entry_qual[1].value[0],ent_info.entry_qual[2].value[0],ent_info.entry_qual[3].value[0],ent_info.entry_qual[4].value[0],ent_info.entry_qual[5].value[0],
          ent_info.entry_action[0].value[0],ent_info.entry_action[1].value[0],ent_info.entry_action[2].value[0],ent_info.entry_action[3].value[0]);

    return 0;
}

/**
* \brief
*  Configures the ingress node iPMF1 stage TCAM lookup for 5 Tuple + period
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_srv6_alternate_marking_ingress_node_iPMF2(int unit)
{
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t user_action_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    int header_qual_index;
    void *dest_char;
    int idx;
    bcm_field_stage_t stage_for_context;
    int rv = BCM_E_NONE;

    stage_for_context = bcmFieldStageIngressPMF1;

    bcm_field_group_info_t_init(&cint_field_srv6_am_ipmf2_ing_node_fg_info);
    cint_field_srv6_am_ipmf2_ing_node_fg_info.fg_type = bcmFieldGroupTypeTcam;
    cint_field_srv6_am_ipmf2_ing_node_fg_info.stage = bcmFieldStageIngressPMF2;

    cint_field_srv6_am_ipmf2_ing_node_fg_info.nof_quals = 5;
    if (cint_srv6_am_fwd_type == 0) {
        cint_field_srv6_am_ipmf2_ing_node_fg_info.qual_types[0] = bcmFieldQualifySrcIp;
        cint_field_srv6_am_ipmf2_ing_node_fg_info.qual_types[1] = bcmFieldQualifyDstIp;
        cint_field_srv6_am_ipmf2_ing_node_fg_info.qual_types[2] = bcmFieldQualifyIp4Protocol;
    } else {
        cint_field_srv6_am_ipmf2_ing_node_fg_info.qual_types[0] = bcmFieldQualifySrcIp6Low;
        cint_field_srv6_am_ipmf2_ing_node_fg_info.qual_types[1] = bcmFieldQualifyDstIp6Low;
        cint_field_srv6_am_ipmf2_ing_node_fg_info.qual_types[2] = bcmFieldQualifyIp6NextHeader;
    }
    cint_field_srv6_am_ipmf2_ing_node_fg_info.qual_types[3] = bcmFieldQualifyL4DstPort;
    cint_field_srv6_am_ipmf2_ing_node_fg_info.qual_types[4] = bcmFieldQualifyL4SrcPort;

    /* Set actions */
    cint_field_srv6_am_ipmf2_ing_node_fg_info.nof_actions = 1;
    cint_field_srv6_am_ipmf2_ing_node_fg_info.action_types[0] = bcmFieldActionContainer;

    dest_char = &(cint_field_srv6_am_ipmf2_ing_node_fg_info.name[0]);
    sal_strncpy_s(dest_char, "AM_FG_ipmf2", sizeof(cint_field_srv6_am_ipmf2_ing_node_fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &cint_field_srv6_am_ipmf2_ing_node_fg_info, &cint_field_srv6_am_ipmf2_ing_node_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in ipmf2 \n", cint_field_srv6_am_ipmf2_ing_node_fg_id);

    /**Attach context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "AM_IP_Ing", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, stage_for_context, &context_info, &cint_field_srv6_am_ipmf2_ing_node_ctx_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    attach_info.key_info.nof_quals = cint_field_srv6_am_ipmf2_ing_node_fg_info.nof_quals;
    attach_info.payload_info.nof_actions = cint_field_srv6_am_ipmf2_ing_node_fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = cint_field_srv6_am_ipmf2_ing_node_fg_info.qual_types[0];

    for(idx = 0 ; idx < cint_field_srv6_am_ipmf2_ing_node_fg_info.nof_actions; idx++ )
    {
        attach_info.payload_info.action_types[idx] = cint_field_srv6_am_ipmf2_ing_node_fg_info.action_types[idx];
    }

    /**IP qualifiers are expected to be on Absolute +1 layer*/
    for(header_qual_index = 0 ; header_qual_index < 3 ; header_qual_index++)
    {
        attach_info.key_info.qual_types[header_qual_index] = cint_field_srv6_am_ipmf2_ing_node_fg_info.qual_types[header_qual_index];
        attach_info.key_info.qual_info[header_qual_index].input_type = bcmFieldInputTypeLayerAbsolute;
        attach_info.key_info.qual_info[header_qual_index].input_arg = 1;
        attach_info.key_info.qual_info[header_qual_index].offset = 0;
    }
    /**TCP/UDP qualifiers are expected to be on Absolute +2 layer*/
    for(header_qual_index = 3 ; header_qual_index < 5 ; header_qual_index++)
    {
        attach_info.key_info.qual_types[header_qual_index] = cint_field_srv6_am_ipmf2_ing_node_fg_info.qual_types[header_qual_index];
        attach_info.key_info.qual_info[header_qual_index].input_type = bcmFieldInputTypeLayerAbsolute;
        attach_info.key_info.qual_info[header_qual_index].input_arg = 2;
        attach_info.key_info.qual_info[header_qual_index].offset = 0;
    }

    rv = bcm_field_group_context_attach(unit, 0, cint_field_srv6_am_ipmf2_ing_node_fg_id, cint_field_srv6_am_ipmf2_ing_node_ctx_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached  FG (%d) to context (%d) Stage(ipmf2)\n",cint_field_srv6_am_ipmf2_ing_node_fg_id,cint_field_srv6_am_ipmf2_ing_node_ctx_id);

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_srv6_am_ipmf2_ing_node_presel_id;
    p_id.stage = stage_for_context;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_srv6_am_ipmf2_ing_node_ctx_id;
    p_data.nof_qualifiers = 2;
    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = (cint_srv6_am_fwd_type == 0) ? bcmFieldLayerTypeIp4 : bcmFieldLayerTypeIp6;
    p_data.qual_data[0].qual_mask = 0x1F;
    p_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[1].qual_arg = -1;
    p_data.qual_data[1].qual_value = bcmFieldLayerTypeEth;
    p_data.qual_data[1].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(ipmf2) context(%d) fwd_layer(IPv4) \n",
            cint_field_srv6_am_ipmf2_ing_node_presel_id, cint_field_srv6_am_ipmf2_ing_node_ctx_id);

    return 0;
}


/**
* \brief
*  Configures the ingress node iPMF1 stage TCAM lookup for 5 Tuple + period
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_srv6_alternate_marking_ingress_node_iPMF3(int unit)
{
    bcm_field_qualifier_info_create_t qual_info;
    int action_offset;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    int header_qual_index;
    void *dest_char;
    int idx;
    int rv = BCM_E_NONE;

    bcm_field_group_info_t_init(&cint_field_srv6_am_ipmf3_ing_node_fg_info);
    cint_field_srv6_am_ipmf3_ing_node_fg_info.fg_type = bcmFieldGroupTypeTcam;
    cint_field_srv6_am_ipmf3_ing_node_fg_info.stage = bcmFieldStageIngressPMF3;

    /* Set quals */
    cint_field_srv6_am_ipmf3_ing_node_fg_info.nof_quals = 2;
    cint_field_srv6_am_ipmf3_ing_node_fg_info.qual_types[0] = bcmFieldQualifyContainer;
    cint_field_srv6_am_ipmf3_ing_node_fg_info.qual_types[1] = bcmFieldQualifyKeyGenVar;

    /* Set actions */
    cint_field_srv6_am_ipmf3_ing_node_fg_info.nof_actions = 6;
    cint_field_srv6_am_ipmf3_ing_node_fg_info.action_types[0] = bcmFieldActionTrunkHashKeySet;
    cint_field_srv6_am_ipmf3_ing_node_fg_info.action_types[1] = bcmFieldActionNetworkLoadBalanceKey;
    cint_field_srv6_am_ipmf3_ing_node_fg_info.action_types[2] = bcmFieldActionStatId0 + g_cint_field_srv6_am_ing_ent.command_id;
    cint_field_srv6_am_ipmf3_ing_node_fg_info.action_types[3] = bcmFieldActionStatProfile0 + g_cint_field_srv6_am_ing_ent.command_id;
    cint_field_srv6_am_ipmf3_ing_node_fg_info.action_types[4] = bcmFieldActionOamRaw;
    cint_field_srv6_am_ipmf3_ing_node_fg_info.action_types[5] = bcmFieldActionStatOamLM;

    dest_char = &(cint_field_srv6_am_ipmf3_ing_node_fg_info.name[0]);
    sal_strncpy_s(dest_char, "AM_FG_Ipmf3", sizeof(cint_field_srv6_am_ipmf3_ing_node_fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &cint_field_srv6_am_ipmf3_ing_node_fg_info, &cint_field_srv6_am_ipmf3_ing_node_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in ipmf3 \n", cint_field_srv6_am_ipmf3_ing_node_fg_id);

    /**Attach context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "AM_IP_Ing", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, &cint_field_srv6_am_ipmf3_ing_node_ctx_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    attach_info.key_info.nof_quals = cint_field_srv6_am_ipmf3_ing_node_fg_info.nof_quals;
    attach_info.payload_info.nof_actions = cint_field_srv6_am_ipmf3_ing_node_fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = cint_field_srv6_am_ipmf3_ing_node_fg_info.qual_types[0];
    attach_info.key_info.qual_types[1] = cint_field_srv6_am_ipmf3_ing_node_fg_info.qual_types[1];

    for(idx = 0 ; idx < cint_field_srv6_am_ipmf3_ing_node_fg_info.nof_actions; idx++ )
    {
        attach_info.payload_info.action_types[idx] = cint_field_srv6_am_ipmf3_ing_node_fg_info.action_types[idx];
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;
    /**Copy the Key Gen Var qualifier*/
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_srv6_am_ipmf3_ing_node_fg_id, cint_field_srv6_am_ipmf3_ing_node_ctx_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached  FG (%d) to context (%d) Stage(iPMF3)\n",cint_field_srv6_am_ipmf3_ing_node_fg_id,cint_field_srv6_am_ipmf3_ing_node_ctx_id);


    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_srv6_am_impf3_ing_node_presel_id;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_srv6_am_ipmf3_ing_node_ctx_id;

    p_data.nof_qualifiers = 2;
    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = (cint_srv6_am_fwd_type == 0) ? bcmFieldLayerTypeIp4 : bcmFieldLayerTypeIp6;
    p_data.qual_data[0].qual_mask = 0x1F;
    /* Below qualify is to make sure recycle packet won't hit this context.
     * Otherwise, it may intercept packet which hit RCH_EXTENDED_ENCAP originally
     */
    p_data.qual_data[1].qual_type = bcmFieldQualifyPortClassTrafficManagement;
    p_data.qual_data[1].qual_value = 0;
    p_data.qual_data[1].qual_mask = 0x7;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(ipmf3) context(%d) fwd_layer(IPv4) \n",
            cint_field_srv6_am_impf3_ing_node_presel_id, cint_field_srv6_am_ipmf3_ing_node_ctx_id);

    return 0;
}

/**
* \brief
*  Configures the ingress node ePMF stage, ASE check and generate snoop copy
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_srv6_alternate_marking_ingress_node_ePMF(int unit)
{
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_group_info_t fg_info;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_ace_format_t ace_format_id;
    uint32 ace_entry_handle;
    int header_qual_index;
    void *dest_char;
    int idx;
    int rv = BCM_E_NONE;

    bcm_field_group_info_t_init(&fg_info);

    fg_info.stage = bcmFieldStageEgress;
    fg_info.fg_type = bcmFieldGroupTypeTcam;

    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyOamTsSystemHeader;
    fg_info.nof_actions = 2;
    fg_info.action_types[0] = bcmFieldActionSnoopRaw;
    fg_info.action_types[1] = bcmFieldActionSnoopStrengthRaw;


    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "AM_FG_Egr", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_srv6_am_egress_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in ePMF \n", cint_field_srv6_am_egress_fg_id);

    /**Attach context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "AM_ASE_Egr", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageEgress, &context_info, &cint_field_srv6_am_egress_ctx_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_context_create\n", rv);
       return rv;
    }

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    for(idx=0 ; idx < fg_info.nof_actions; idx++)
    {
        attach_info.payload_info.action_types[idx] = fg_info.action_types[idx];
    }

    rv = bcm_field_group_context_attach(unit, 0, cint_field_srv6_am_egress_fg_id, cint_field_srv6_am_egress_ctx_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached  FG (%d) to context (%d) Stage(ePMF)\n",cint_field_srv6_am_egress_fg_id,cint_field_srv6_am_egress_ctx_id);

    bcm_field_entry_info_t_init(&ent_info);

    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    /**OAM_Sub_Type =10 when the type is RFC8321 - ASE_EXT is size of 48 all should be 0 but the 4lsb of the type*/
    ent_info.entry_qual[0].value[0] =0;
    ent_info.entry_qual[0].value[1] = 0xb000;
    /** Data in the ASE should be 0, subtype should be B. 9
     *  lowest bits are marked as "don't care" */
    ent_info.entry_qual[0].mask[0] = 0xFFFFFe00;
    ent_info.entry_qual[0].mask[1] = 0xFFFF;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = cint_field_srv6_am_egress_snoop_id;
    ent_info.entry_action[1].type = fg_info.action_types[1];
    ent_info.entry_action[1].value[0] = cint_field_srv6_am_egress_snoop_str;

    rv = bcm_field_entry_add(unit,0,cint_field_srv6_am_egress_fg_id,&ent_info,&cint_field_srv6_am_egress_entr_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("FG (%d) Stage(ePMF) , entry_add: handle(%d) \n",cint_field_srv6_am_egress_fg_id,cint_field_srv6_am_egress_entr_id);
    printf("Entry values: \n  ASE(0x%x%x) \n  AcePointer(0x%x) SnoopId(%d) SnoopStr(%d) \n",
            ent_info.entry_qual[0].value[1],ent_info.entry_qual[0].value[0],
            ent_info.entry_action[0].value[0],ent_info.entry_action[1].value[0],ent_info.entry_action[2].value[0]);

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_srv6_am_egress_presel_id;
    p_id.stage = bcmFieldStageEgress;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_srv6_am_egress_ctx_id;
    p_data.nof_qualifiers = 1;

    /* Qualifier based on ASE extension.*/
    p_data.qual_data[0].qual_type = bcmFieldQualifyFtmhAsePresent;
    p_data.qual_data[0].qual_value = 1;
    p_data.qual_data[0].qual_mask = -1;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(ePMF) context(%d) Always hit! \n",
            cint_field_srv6_am_egress_presel_id, cint_field_srv6_am_egress_ctx_id);

    return 0;
}

/**
* \brief
*  Configures the ingress node ePMF stage, D bit check and generate snoop copy in last pass
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_srv6_ext_alternate_marking_ingress_node_ePMF(int unit)
{
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_group_info_t fg_info;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_ace_format_t ace_format_id;
    uint32 ace_entry_handle;
    int header_qual_index;
    void *dest_char;
    int idx;
    int rv = BCM_E_NONE;

    bcm_field_group_info_t_init(&fg_info);

    fg_info.stage = bcmFieldStageEgress;
    fg_info.fg_type = bcmFieldGroupTypeTcam;

    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyNetworkLoadBalanceKey;
    fg_info.nof_actions = 2;
    fg_info.action_types[0] = bcmFieldActionSnoopRaw;
    fg_info.action_types[1] = bcmFieldActionSnoopStrengthRaw;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "AM_FG_Egr", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_srv6_am_egress_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in ePMF \n", cint_field_srv6_am_egress_fg_id);

    /**Attach context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "AM_ASE_Egr", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageEgress, &context_info, &cint_field_srv6_am_egress_ctx_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_context_create\n", rv);
       return rv;
    }

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    for(idx=0 ; idx < fg_info.nof_actions; idx++)
    {
        attach_info.payload_info.action_types[idx] = fg_info.action_types[idx];
    }

    rv = bcm_field_group_context_attach(unit, 0, cint_field_srv6_am_egress_fg_id, cint_field_srv6_am_egress_ctx_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached  FG (%d) to context (%d) Stage(ePMF)\n",cint_field_srv6_am_egress_fg_id,cint_field_srv6_am_egress_ctx_id);

    bcm_field_entry_info_t_init(&ent_info);

    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];   /* qualify D bit, NetworkLoadBalanceKey[14] is D bit */
    ent_info.entry_qual[0].value[0] =0x4000;
    ent_info.entry_qual[0].mask[0] = 0x4000;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = cint_field_srv6_am_egress_snoop_id;
    ent_info.entry_action[1].type = fg_info.action_types[1];
    ent_info.entry_action[1].value[0] = cint_field_srv6_am_egress_snoop_str;

    rv = bcm_field_entry_add(unit,0,cint_field_srv6_am_egress_fg_id,&ent_info,&cint_field_srv6_am_egress_entr_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("FG (%d) Stage(ePMF) , entry_add: handle(%d) \n",cint_field_srv6_am_egress_fg_id,cint_field_srv6_am_egress_entr_id);
    printf("Entry values: \n  ASE(0x%x%x) \n  AcePointer(0x%x) SnoopId(%d) SnoopStr(%d) \n",
            ent_info.entry_qual[0].value[1],ent_info.entry_qual[0].value[0],
            ent_info.entry_action[0].value[0],ent_info.entry_action[1].value[0],ent_info.entry_action[2].value[0]);

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_srv6_am_egress_presel_id;
    p_id.stage = bcmFieldStageEgress;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_srv6_am_egress_ctx_id;
    p_data.nof_qualifiers = 1;

    /* Qualifier based on D bit in NetworkLoadBalanceKey.*/
    p_data.qual_data[0].qual_type = bcmFieldQualifyPortClassPacketProcessing;
    p_data.qual_data[0].qual_arg = 0;

    /* Qualifier based on egress port pmf data.
     * This is a pmf data per port, user can set it to same value for all out ports of Srv6 ingress node.
     * This qualifier to make sure the packet egresses out of dest port of Srv6 ingress node, instead of recycle port.
     * Purpose is generating snoop copy in last pass.
     */
    p_data.qual_data[0].qual_value = g_cint_field_srv6_am_ing_ent.egr_port_pmf_data;
    p_data.qual_data[0].qual_mask = 0x7;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(ePMF) context(%d) Always hit! \n",
            cint_field_srv6_am_egress_presel_id, cint_field_srv6_am_egress_ctx_id);

    return 0;
}


/**
* \brief
*  Configures The preiod global value by user input for ingress
*  node.
*
*  Each AM ingress entry is associated with one of 32
*  KeyGenVar_profiles. L bit can be toggled in each of the
*  profiles through this function.
*
* \param [in] unit        -  Device id
* \param [in] profile        -  must be a value between 0-31.
*        Represents the "period profile"
* \param [in] l_bit_set      - 0 or 1
*
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_srv6_alternate_marking_period_change(int unit,int profile, int l_bit_set)
{
    bcm_field_context_param_info_t context_params;
    int rv;
    uint32 mask=0;

    if (profile > 32)
    {
        printf("Error invalid parameter in cint_field_srv6_alternate_marking_period_change. profile must be 0-31, given value %d \n", profile);
        return rv;
    }

    if (l_bit_set > 1)
    {
        printf("Error invalid parameter in cint_field_srv6_alternate_marking_period_change. l_bit_set must be 0or 1, given value %d \n", l_bit_set);
        return rv;
    }

    /* Update the corresponding bit*/
    mask = 1<< profile;
    if (l_bit_set)
    {
        g_KeyGenVar_period_profiles |= mask;
    } else {
        g_KeyGenVar_period_profiles &= ~mask;
    }
    /* in other words g_KeyGenVar_period_profiles ^= (-l_bit_set ^ g_KeyGenVar_period_profiles) & mask; :)*/

    /* Now update the value in the PMF*/
    bcm_field_context_param_info_t_init(&context_params);

    context_params.param_type = bcmFieldContextParamTypeKeyVal;
    context_params.param_val = g_KeyGenVar_period_profiles;

    rv = bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF3,cint_field_srv6_am_ipmf3_ing_node_ctx_id,&context_params);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_context_param_set \n", rv);
       return rv;
    }

    printf("Marking Period (KeyGenVar) was changed to (%d) iPMF3_ctx (%d) \n",l_bit_set,cint_field_srv6_am_ipmf3_ing_node_ctx_id);

    g_cint_field_srv6_am_ing_ent.KeyGenVar_period = l_bit_set;

    return 0;
}

