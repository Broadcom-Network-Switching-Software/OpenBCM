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
 * cint ../../../src/examples/dnx/field/cint_field_alternate_marking.c
 * cint;
 * int unit = 0;
 * cint_field_alternate_marking_ingress_node_main(unit);
 * cint_field_alternate_marking_inter_or_egress_node_main(unit);
 * Configuration example end
 *
 *   This cint add support of PMF for RFC8321
 *
 *   For ingress node:
 *     Note that iPMF1 TCAM should be created first in configuration in order other functions to work
 *     For example configuration look at:
 *     mpls_deep_stack_alternate_marking_field()
 *
 *   1)iPMF1 TCAM FG is created - the context selection for that field group is when Forwarding layer is IPv4
 *     The qualifiers are 5 tuple: assuming that IP addresses are on second layer and TCP/UDP ports are in 3rd layer, also there is 32 bit period-profile register.
 *     This is represented by g_KeyGenVar_period_profiles (see item 2)
 *     the actions are 2 counters Stat_id+Profile , as well as outlif value (currently the value expected raw i.e. HW value and not GPORT)
 *     To configure ingress node iPMF1 call:
 *     cint_field_alternate_marking_ingress_node_iPMF1()
 * 
 * 
 *
 *   2) KeyGenVar representation:
 *      The sixth qualifier is a 32 bit KeyGenVar register where all but one bit is masked.
 *      Two iPMF entries should be added per AM session. The differences being
 *          1. The key - The unmasked bit in KeyGenVar. Add one entry with 0 and one with 1.
 *          2. The action - each should be assigned a different counter
 *      Alternating between the L bits is done through cint_field_alternate_marking_period_change(). Provide the new L bit and the profile and the
 * 
 *      For example when using profile 4, set KeyGenVar_profile to 4 and call cint_field_alternate_marking_ingress_node_entry_add() twice with the same values, except once with
 *      g_cint_field_am_ing_ent.KeyGenVar_period = 8 (bit 4 is set) and once  g_cint_field_am_ing_ent.KeyGenVar_period = 0.
 *
 *   3) To add entries to iPMF1 TCAM, by default g_cint_field_am_ing_ent values are used. To add another entry
 *      a) modify g_cint_field_am_ing_ent with set of values
 *      b) call cint_field_alternate_marking_ingress_node_entry_add
 *      Note that all qualifiers have mask all 1's i.e. exact match.
 *
 *   4) ePMF TCAM configuration and ACE configuration
 *     Ace configured first - generates action so that relevant context will be selected in ENCAP_1 and ENCAP_2
 *     ePMF qualify on ASE extension the subtype should be 10 for RFC8321 and the counter should be 0
 *     ePMF action is to:
 *          - Generate Ace pointer which points to action created earlier
 *          - Snoop Id and Snoop Strength to send the packet to CPU
 *     ePMF context selection currently configured always hit with presel id 5 (i.e. only in case there is higher priority context and hit, only then the context will not be chosen)
 *
 *
 *   Intermediate or egress Node
 *     Note that iPMF1 TCAM should be created first
 *     For example configuration look at:
 *     cint_field_alternate_marking_inter_or_egress_node_main()
 *
 *    1) iPMF1 TCAM configuration - the context selection is done when Forwarding or Forwarding -1 layer are MPLS
 *      The qualifiers are NOF_MPLS_QUALS MPLS labels
 *      The action is void since only used to cascade to relevant context in iPMF2
 *
 *    2) To add entries for all NOF_MPLS_QUALS label to find the special label
 *       use g_cint_field_am_i_o_e_ipmf1_ent global structure to set values for entry and the call API:
 *       cint_field_alternate_marking_inter_or_egress_node_iPMF1_entry_add()
 *
 *
 *    3) in iPMF2 we have NOF_MPLS_QUALS contexts with 2 FG TCAM each - each which is chosen based on the result of iPMF1
 *      the qualifiers are:
 *       Flow label (1 of NOF_MPLS_QUALS labels that were inspected in iPMF1)
 *       L bit
 *       The action are statid+profile (for counting)
 *      Another FG is to inspected D bit and to output snoop and and tail edit profile (Note Tail edit command not supported in iPMF2)
 *
 *     4) To set entries for the 2 FG's in iPMF2, same method is used A) set global struct B) call API :
 *      g_cint_field_am_i_o_e_ipmf2_flow_ent / g_cint_field_am_i_o_e_ipmf2_D_ent
 *       cint_field_alternate_marking_inter_or_egress_node_iPMF2_flow_entry_add() / cint_field_alternate_marking_inter_or_egress_node_iPMF2_D_entry_add
 */


bcm_field_group_t cint_field_am_ingress_fg_id=0;
bcm_field_context_t cint_field_am_ingress_ctx_id;
bcm_field_presel_t cint_field_am_presel_id = 5;
bcm_field_group_info_t cint_field_am_ipmf1_ing_node_fg_info;

bcm_field_context_t cint_field_am_egress_ctx_id;
bcm_field_group_t cint_field_am_egress_fg_id=0;
bcm_field_entry_t cint_field_am_egress_entr_id;

/**Used for setting values of ePMF entry in Ingress node*/
uint32 cint_field_am_egress_snoop_id=1;
uint32 cint_field_am_egress_snoop_str=4;

/** L bit configuration for each KeyGenVar profile.
 *  Initialize with 0. Should only be set through
 *  cint_field_alternate_marking_period_change() */
uint32 g_KeyGenVar_period_profiles = 0;

struct cint_field_am_stat_profile_s {
    int is_meter; /* Is meter acts as "counter-increment" indication. Must be set to 1 */
    int is_lm; /* AM latches on to LM counting mechanism - is_lm must be set to one*/
    int type_id; 
    int valid;
};


struct cint_field_am_ingress_node_entry_val_s{
     uint32 SrcIp;
     uint32 DstIp;
     uint32 ProtocolType;
     uint32 L4Dst;
     uint32 L4Src;
     uint32 KeyGenVar_profile; /* Each entry must be associated with one of 32 KeyGenProfiles, each represents the L bit configuration.
     L bit configuration may be changed per profile through  cint_field_alternate_marking_period_change() */
     uint32 KeyGenVar_period;

     uint32 statId0; /*This represents the counter used in the engine.*/
     cint_field_am_stat_profile_s statProfile0; /* Must be used with the values set below*/
     uint32 command_id; /* Counter command ID - must be 7,8 or 9*/
     uint32 out_lif_0_raw;
     uint32 oam_raw;
};

cint_field_am_ingress_node_entry_val_s g_cint_field_am_ing_ent = {
        0x0a000001,
        0x0a000002,
        6, /*TCP*/
        9000,
        8000,
        1, /* KeyGenVar_period*/
        1, /* period profile*/

        7, /* Stat-id represents the counter itself*/
        {1,1,0,1}, /* stat profile - all 1 except type which is 0*/
        7, 
        10,
        /* OAM action is encoded in the following way:
         * LS  Oam-Up-Mep(1), Oam-Sub-Type(4), Oam-Offset(8), Oam-Stamp-Offset(8) MS
         * Use Sub-type 0xA for ePMF qualifier.
 */
        0xA0000,
};


int cint_field_alternate_marking_ingress_node_entry_add(int unit, bcm_field_entry_t *entry_handle)
{
    bcm_field_entry_info_t ent_info;
    int idx, lm_read_index;
    int rv;
    bcm_field_entry_info_t_init(&ent_info);

    ent_info.nof_entry_actions =cint_field_am_ipmf1_ing_node_fg_info.nof_actions;


    /* command-id 7 maps to lm_read_index 0, command-id 8 to lm_read_index 1, command-id 9 to lm_read_index 2,  */
    lm_read_index = bcmFieldStatOamLmIndex0;

    for(idx = 0 ; idx < ent_info.nof_entry_actions; idx++ )
    {
        ent_info.entry_action[idx].type = cint_field_am_ipmf1_ing_node_fg_info.action_types[idx];
    }
    ent_info.entry_action[0].value[0] = g_cint_field_am_ing_ent.statId0;  
    ent_info.entry_action[1].value[0] = g_cint_field_am_ing_ent.statProfile0.is_meter; /* Action 1 - stat profile*/
    ent_info.entry_action[1].value[1] = g_cint_field_am_ing_ent.statProfile0.is_lm; /* Action 1 - stat profile*/
    ent_info.entry_action[1].value[2] = g_cint_field_am_ing_ent.statProfile0.type_id; /* Action 1 - stat profile*/
    ent_info.entry_action[1].value[3] = g_cint_field_am_ing_ent.statProfile0.valid; /* Action 1 - stat profile*/
    ent_info.entry_action[2].value[0] = g_cint_field_am_ing_ent.out_lif_0_raw;
    ent_info.entry_action[3].value[0] = g_cint_field_am_ing_ent.oam_raw;
    ent_info.entry_action[4].value[0] = lm_read_index;


    ent_info.nof_entry_quals = cint_field_am_ipmf1_ing_node_fg_info.nof_quals;
    for(idx = 0 ; idx < ent_info.nof_entry_quals; idx++ )
    {
        ent_info.entry_qual[idx].type = cint_field_am_ipmf1_ing_node_fg_info.qual_types[idx];
        ent_info.entry_qual[idx].mask[0] = -1;
    }
    ent_info.entry_qual[0].value[0] = g_cint_field_am_ing_ent.SrcIp;
    ent_info.entry_qual[1].value[0] = g_cint_field_am_ing_ent.DstIp;
    ent_info.entry_qual[2].value[0] = g_cint_field_am_ing_ent.ProtocolType;
    ent_info.entry_qual[3].value[0] = g_cint_field_am_ing_ent.L4Dst;
    ent_info.entry_qual[4].value[0] = g_cint_field_am_ing_ent.L4Src;
    ent_info.entry_qual[5].value[0] = g_cint_field_am_ing_ent.KeyGenVar_period;
    ent_info.entry_qual[5].mask[0] &= ~(1<<g_cint_field_am_ing_ent.KeyGenVar_profile); /* Unmask "key-gen-profile" bit.*/


    rv = bcm_field_entry_add(unit,0,cint_field_am_ingress_fg_id,&ent_info,entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }


    printf("iPMF1 FG(%d) entry_add 0x(%x): \n SrcIp(0x%x) DstIp(0x%x) ProtocolType(%d) L4Dst(%d) L4Src(%d) KeyGenVar_period(0x%x) \n statId0(%d) statProfile0(0x%x) outlif_0(0x%x) oam_raw(0x%x)\n\n",
            cint_field_am_ingress_fg_id, (*entry_handle),
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
int cint_field_alternate_marking_ingress_node_iPMF1(int unit)
{

    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    int header_qual_index;
    void *dest_char;
    int idx;
    int rv = BCM_E_NONE;


    bcm_field_group_info_t_init(&cint_field_am_ipmf1_ing_node_fg_info);
    cint_field_am_ipmf1_ing_node_fg_info.fg_type = bcmFieldGroupTypeTcam;
    cint_field_am_ipmf1_ing_node_fg_info.stage = bcmFieldStageIngressPMF1;

    /* Set quals */
    cint_field_am_ipmf1_ing_node_fg_info.nof_quals = 6;
    cint_field_am_ipmf1_ing_node_fg_info.qual_types[0] = bcmFieldQualifySrcIp;
    cint_field_am_ipmf1_ing_node_fg_info.qual_types[1] = bcmFieldQualifyDstIp;
    cint_field_am_ipmf1_ing_node_fg_info.qual_types[2] = bcmFieldQualifyIp4Protocol;
    cint_field_am_ipmf1_ing_node_fg_info.qual_types[3] = bcmFieldQualifyL4DstPort;
    cint_field_am_ipmf1_ing_node_fg_info.qual_types[4] = bcmFieldQualifyL4SrcPort;
    cint_field_am_ipmf1_ing_node_fg_info.qual_types[5] = bcmFieldQualifyKeyGenVar;
    /* Set actions */
    cint_field_am_ipmf1_ing_node_fg_info.nof_actions = 5;
    cint_field_am_ipmf1_ing_node_fg_info.action_types[0] = bcmFieldActionStatId0 + g_cint_field_am_ing_ent.command_id; 
    cint_field_am_ipmf1_ing_node_fg_info.action_types[1] = bcmFieldActionStatProfile0 + g_cint_field_am_ing_ent.command_id;
    cint_field_am_ipmf1_ing_node_fg_info.action_types[2] = bcmFieldActionOutVport0Raw;
    cint_field_am_ipmf1_ing_node_fg_info.action_types[3] = bcmFieldActionOamRaw;
    cint_field_am_ipmf1_ing_node_fg_info.action_types[4] = bcmFieldActionStatOamLM;

    dest_char = &(cint_field_am_ipmf1_ing_node_fg_info.name[0]);
    sal_strncpy_s(dest_char, "AM_FG_Ing", sizeof(cint_field_am_ipmf1_ing_node_fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &cint_field_am_ipmf1_ing_node_fg_info, &cint_field_am_ingress_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in iPMF1 \n", cint_field_am_ingress_fg_id);

    /**Attach context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "AM_IPv4_Ing", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &cint_field_am_ingress_ctx_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    attach_info.key_info.nof_quals = cint_field_am_ipmf1_ing_node_fg_info.nof_quals;
    attach_info.payload_info.nof_actions = cint_field_am_ipmf1_ing_node_fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = cint_field_am_ipmf1_ing_node_fg_info.qual_types[0];

    for(idx = 0 ; idx < cint_field_am_ipmf1_ing_node_fg_info.nof_actions; idx++ )
    {
        attach_info.payload_info.action_types[idx] = cint_field_am_ipmf1_ing_node_fg_info.action_types[idx];
    }

    /**IP qualifiers are expected to be on Absolute +1 layer*/
    for(header_qual_index = 0 ; header_qual_index < 3 ; header_qual_index++)
    {
        attach_info.key_info.qual_types[header_qual_index] = cint_field_am_ipmf1_ing_node_fg_info.qual_types[header_qual_index];
        attach_info.key_info.qual_info[header_qual_index].input_type = bcmFieldInputTypeLayerAbsolute;
        attach_info.key_info.qual_info[header_qual_index].input_arg = 1;
        attach_info.key_info.qual_info[header_qual_index].offset = 0;
    }
    /**TCP/UDP qualifiers are expected to be on Absolute +2 layer*/
    for(header_qual_index = 3 ; header_qual_index < 5 ; header_qual_index++)
    {
        attach_info.key_info.qual_types[header_qual_index] = cint_field_am_ipmf1_ing_node_fg_info.qual_types[header_qual_index];
        attach_info.key_info.qual_info[header_qual_index].input_type = bcmFieldInputTypeLayerAbsolute;
        attach_info.key_info.qual_info[header_qual_index].input_arg = 2;
        attach_info.key_info.qual_info[header_qual_index].offset = 0;
    }
    /**Copy the Key Gen Var qualifier*/
    attach_info.key_info.qual_types[5] = cint_field_am_ipmf1_ing_node_fg_info.qual_types[5];
    attach_info.key_info.qual_info[5].input_type = bcmFieldInputTypeMetaData;


    
    rv = bcm_field_group_context_attach(unit, 0, cint_field_am_ingress_fg_id, cint_field_am_ingress_ctx_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached  FG (%d) to context (%d) Stage(iPMF1)\n",cint_field_am_ingress_fg_id,cint_field_am_ingress_ctx_id);


    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_am_presel_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_am_ingress_ctx_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;


    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(IPv4) \n",
            cint_field_am_presel_id,cint_field_am_ingress_ctx_id);




    return 0;
}

/**
* \brief
*  Configures the ingress node ePMF stage, ASE check and ACE creation
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_alternate_marking_ingress_node_ePMF(int unit)
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

    /*
    * Configure the ACE format.
    */
    bcm_field_ace_format_info_t_init(&ace_format_info);

    ace_format_info.nof_actions = 1;

    ace_format_info.action_types[0] = bcmFieldActionAceContextValue;

    dest_char = &(ace_format_info.name[0]);
    sal_strncpy_s(dest_char, "AM_ACE", sizeof(ace_format_info.name));
    rv = bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_ace_format_add \n", rv);
       return rv;
    }

    bcm_field_ace_entry_info_t_init(&ace_entry_info);

    ace_entry_info.nof_entry_actions = 1;
    ace_entry_info.entry_action[0].type = bcmFieldActionAceContextValue;
    ace_entry_info.entry_action[0].value[0] = bcmFieldAceContextAlternateMarking;

    rv = bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, &ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_ace_entry_add \n", rv);
       return rv;
    }
    printf("ACE Format (%d) and ACE entry 0x(%x) created \n", ace_format_id,ace_entry_handle);


    bcm_field_group_info_t_init(&fg_info);

    fg_info.stage = bcmFieldStageEgress;
    fg_info.fg_type = bcmFieldGroupTypeTcam;

    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyOamTsSystemHeader;
    fg_info.nof_actions = 3;
    fg_info.action_types[0] = bcmFieldActionAceEntryId;
    fg_info.action_types[1] = bcmFieldActionSnoopRaw;
    fg_info.action_types[2] = bcmFieldActionSnoopStrengthRaw;


    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "AM_FG_Egr", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_am_egress_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in ePMF \n", cint_field_am_egress_fg_id);

    /**Attach context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "AM_ASE_Egr", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageEgress, &context_info, &cint_field_am_egress_ctx_id);
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

    rv = bcm_field_group_context_attach(unit, 0, cint_field_am_egress_fg_id, cint_field_am_egress_ctx_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached  FG (%d) to context (%d) Stage(ePMF)\n",cint_field_am_egress_fg_id,cint_field_am_egress_ctx_id);

    bcm_field_entry_info_t_init(&ent_info);

    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    /**OAM_Sub_Type =10 when the type is RFC8321 - ASE_EXT is size of 48 all should be 0 but the 4lsb of the type*/
    ent_info.entry_qual[0].value[0] =0;
    ent_info.entry_qual[0].value[1] = 0xa000;
    /** Data in the ASE should be 0, subtype should be A. 9
     *  lowest bits are marked as "don't care" */
    ent_info.entry_qual[0].mask[0] = 0xFFFFFe00;
    ent_info.entry_qual[0].mask[1] = 0xFFFF;  
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = ace_entry_handle;
    ent_info.entry_action[1].type = fg_info.action_types[1];
    ent_info.entry_action[1].value[0] =cint_field_am_egress_snoop_id;
    ent_info.entry_action[2].type = fg_info.action_types[2];
    ent_info.entry_action[2].value[0] =cint_field_am_egress_snoop_str;

    rv = bcm_field_entry_add(unit,0,cint_field_am_egress_fg_id,&ent_info,&cint_field_am_egress_entr_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("FG (%d) Stage(ePMF) , entry_add: handle(%d) \n",cint_field_am_egress_fg_id,cint_field_am_egress_entr_id);
    printf("Entry values: \n  ASE(0x%x%x) \n  AcePointer(0x%x) SnoopId(%d) SnoopStr(%d) \n",
            ent_info.entry_qual[0].value[1],ent_info.entry_qual[0].value[0],
            ent_info.entry_action[0].value[0],ent_info.entry_action[1].value[0],ent_info.entry_action[2].value[0]);

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_am_presel_id;
    p_id.stage = bcmFieldStageEgress;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_am_egress_ctx_id;
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
            cint_field_am_presel_id,cint_field_am_egress_ctx_id);

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
int cint_field_alternate_marking_period_change(int unit,int profile, int l_bit_set)
{
    bcm_field_context_param_info_t context_params;
    int rv;
    uint32 mask=0;

    if (profile > 32)
    {
        printf("Error invalid parameter in cint_field_alternate_marking_period_change. profile must be 0-31, given value %d \n", profile);
        return rv; 
    }

    if (l_bit_set > 1)
    {
        printf("Error invalid parameter in cint_field_alternate_marking_period_change. l_bit_set must be 0or 1, given value %d \n", l_bit_set);
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

    rv = bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF1,cint_field_am_ingress_ctx_id,&context_params);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_context_param_set \n", rv);
       return rv;
    }

    printf("Marking Period (KeyGenVar) was changed to (%d) iPMF1_ctx (%d) \n",l_bit_set,cint_field_am_ingress_ctx_id);

    g_cint_field_am_ing_ent.KeyGenVar_period = l_bit_set;

    return 0;
}


/*
 * *************************************************************************************************************
 * Intermidieate or Egress Node i_o_e
 */
int NOF_MPLS_QUALS=10;
bcm_field_qualify_t cint_field_am_mpls_quals[NOF_MPLS_QUALS];
int cint_field_am_i_o_e_mpls_qual_size=32;
bcm_field_context_t cint_field_am_i_o_e_iPMF1_ctx_id;
bcm_field_context_t cint_field_am_i_o_e_iPMF2_ctx_id[NOF_MPLS_QUALS];
bcm_field_group_t cint_field_am_i_o_e_iPMF1_fg_id=0;
bcm_field_group_t cint_field_am_i_o_e_iPMF2_flow_fg_id=0;
bcm_field_group_t cint_field_am_i_o_e_iPMF2_D_fg_id=0;
bcm_field_group_info_t cint_field_am_i_o_e_iPMF2_flow_fg_info;
bcm_field_group_info_t cint_field_am_i_o_e_iPMF2_D_fg_info;
bcm_field_action_t cint_field_am_i_o_e_void_action;
int cint_field_am_i_o_e_presel_id = 31;
bcm_field_qualify_t cint_field_am_i_o_e_D_bit_qual_id;
/**used for cascding result of iPMF1*/
bcm_field_qualify_t cint_field_am_i_o_e_Flow_label_qual_id;
bcm_field_group_info_t cint_field_am_i_o_e_ipmf1_fg_info;


int cint_field_alternate_marking_i_o_e_node_create_quals_mpls_labels(int unit)
{
    int rv=0;
    bcm_field_qualifier_info_create_t qual_info;
    uint8 idx=0;
    char print_idx;
    void *dest_char;
    int print_quals;

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = cint_field_am_i_o_e_mpls_qual_size;
    dest_char = &(qual_info.name[0]);
    /*sal_strncpy_s(dest_char, "__AM_MPLS_LBL", sizeof(qual_info.name));*/

    for(idx = 0; idx < NOF_MPLS_QUALS; idx++)
    {
        /*qual_info.name[0] = idx;*/
        rv = bcm_field_qualifier_create(unit,0,&qual_info,&cint_field_am_mpls_quals[idx]);
        if (rv != BCM_E_NONE)
        {
           printf("Error (%d), in bcm_field_qualifier_create idx(%d) \n", rv,idx);
           return rv;
        }
    }

    print_quals = cint_field_am_mpls_quals[idx-1];
    printf("(%d) Mpls Qualifiers were last id created id (0x%x) \n", NOF_MPLS_QUALS,print_quals);

    return 0;
}

struct cint_field_am_i_o_r_node_ipmf1_entry_val_s{
     uint32 mpls_labels[NOF_MPLS_QUALS];
     int void_action_shift;
};

cint_field_am_i_o_r_node_ipmf1_entry_val_s g_cint_field_am_i_o_e_ipmf1_ent = {
        {1,2,3,4,5,6,7,8,9,10},
        1
};

int cint_field_alternate_marking_inter_or_egress_node_iPMF1_entry_add(int unit,bcm_field_entry_t *entry_handle)
{
    bcm_field_entry_info_t ent_info;
    bcm_field_group_info_t *fg_info = &cint_field_am_i_o_e_ipmf1_fg_info;
    int entry_idx    , qual_idx;
    int rv;
    bcm_field_entry_info_t_init(&ent_info);


    ent_info.nof_entry_actions =fg_info->nof_actions;

    for (qual_idx= 0; qual_idx< NOF_MPLS_QUALS; ++qual_idx)
    {
        for (entry_idx     = 0; entry_idx < ent_info.nof_entry_actions; entry_idx++)
        {
            ent_info.entry_action[entry_idx].type = fg_info->action_types[entry_idx];
        }
        ent_info.entry_action[0].value[0] = qual_idx +1;

        
        ent_info.nof_entry_quals = qual_idx +1;
        for (entry_idx = 0; entry_idx < ent_info.nof_entry_quals -1; entry_idx++)
        {
            ent_info.entry_qual[entry_idx].type = fg_info->qual_types[entry_idx];
            ent_info.entry_qual[entry_idx].mask[0] = 0x100;
            ent_info.entry_qual[entry_idx].value[0] = 0;  /* BOS should be 0*/
        }
        ent_info.entry_qual[entry_idx].type = fg_info->qual_types[entry_idx];
        ent_info.entry_qual[entry_idx].mask[0] = 0xFFFFF000;
        ent_info.entry_qual[entry_idx].value[0] = 0xa000; /* Search for the special label*/


        rv = bcm_field_entry_add(unit, 0, cint_field_am_i_o_e_iPMF1_fg_id, &ent_info, entry_handle);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_entry_add\n", rv);
            return rv;
        }
    }

    printf("iPMF1 FG(%d) entry_add 0x(%x): \n ", cint_field_am_ingress_fg_id, (*entry_handle));
    for(entry_idx = 0 ; entry_idx < ent_info.nof_entry_quals; entry_idx++ )
    {
        printf("Mpls_%d (0x%x) ",entry_idx,ent_info.entry_qual[entry_idx].value[0]);
    }
    printf("\n Void Shift Action (%d) ",ent_info.entry_action[0].value[0]);


    return 0;
}

/**
 * Configure intermidiate or egress node in iPMF1.
 *
 * @param unit
 * @param is_inter_pe 1 is for intermediate PE node; 
 *
 * @return int
 */
int cint_field_alternate_marking_inter_or_egress_node_iPMF1(int unit, int is_inter_pe)
{
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_group_info_t fg_info;
    bcm_field_presel_entry_id_t p_id;
    int idx;
    bcm_field_action_info_t action_info;
    uint32 print_action;
    void *dest_char;
    int rv = BCM_E_NONE;

    /*This action is generated to control the context selection of iPMF2 stage,
     * will indicate which of the NOF_MPLS_QUALS labels that are checked in iPMF1 is the Special label
     * Basicaly maximum value for this action should be NOF_MPLS_QUALS
 */
    bcm_field_action_info_t_init(&action_info);
    action_info.size=4;
    action_info.action_type = bcmFieldActionVoid;
    action_info.stage = bcmFieldStageIngressPMF1;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "AM_i_o_e_Shift", sizeof(action_info.name));

    rv= bcm_field_action_create(unit, 0 ,&action_info, &cint_field_am_i_o_e_void_action);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_action_create  \n", rv);
       return rv;
    }

    print_action = cint_field_am_i_o_e_void_action;
    printf("Cascading Action created size of (%d) , id (0x%x) \n",action_info.size,print_action);

    rv = cint_field_alternate_marking_i_o_e_node_create_quals_mpls_labels(unit);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in cint_field_alternate_marking_i_o_e_node_create_quals_mpls_labels  \n", rv);
       return rv;
    }


    bcm_field_group_info_t_init(&fg_info);

    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.fg_type = bcmFieldGroupTypeTcam;

    fg_info.nof_quals = NOF_MPLS_QUALS;
    for(idx=0; idx < NOF_MPLS_QUALS; idx++)
    {
        fg_info.qual_types[idx] = cint_field_am_mpls_quals[idx];
    }
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = cint_field_am_i_o_e_void_action;
    fg_info.action_with_valid_bit[0] = FALSE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "AM_i_o_e_FG", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_am_i_o_e_iPMF1_fg_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_group_add\n", rv);
       return rv;
    }
    printf("Created TCAM field group (%d) in iPMF1 I o E node \n", cint_field_am_i_o_e_iPMF1_fg_id);

    /**Attach context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "AM_I_o_e_Ctx", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &cint_field_am_i_o_e_iPMF1_ctx_id);
    if (rv != BCM_E_NONE)
    {
      printf("Error (%d), in bcm_field_context_create\n", rv);
      return rv;
    }


    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    for(idx = 0 ; idx < NOF_MPLS_QUALS ; idx++)
    {
        attach_info.key_info.qual_types[idx] = fg_info.qual_types[idx];
        attach_info.key_info.qual_info[idx].input_type = bcmFieldInputTypeLayerAbsolute;
        attach_info.key_info.qual_info[idx].input_arg = 1;
        attach_info.key_info.qual_info[idx].offset = idx*cint_field_am_i_o_e_mpls_qual_size;
    }

    rv = bcm_field_group_context_attach(unit, 0, cint_field_am_i_o_e_iPMF1_fg_id, cint_field_am_i_o_e_iPMF1_ctx_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached  FG (%d) to context (%d) Stage(iPMF1)\n",cint_field_am_i_o_e_iPMF1_fg_id,cint_field_am_i_o_e_iPMF1_ctx_id);


    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_am_i_o_e_presel_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_am_i_o_e_iPMF1_ctx_id;

    if (is_inter_pe) {
        /* For intermediate PE node, the forward layer is Ip */
        p_data.nof_qualifiers = 2;
        p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
        p_data.qual_data[0].qual_arg = -1;
        p_data.qual_data[0].qual_value = bcmFieldLayerTypeForwardingMPLS;
        p_data.qual_data[0].qual_mask = 0x1F;
        p_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
        p_data.qual_data[1].qual_arg = 0;
        p_data.qual_data[1].qual_value = bcmFieldLayerTypeIp4;
        p_data.qual_data[1].qual_mask = 0x1F;
    } else {
        p_data.nof_qualifiers = 1;

        p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
        p_data.qual_data[0].qual_arg = 0;
        p_data.qual_data[0].qual_value = bcmFieldLayerTypeForwardingMPLS;
        p_data.qual_data[0].qual_mask = 0x1F;
    }

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(MPLS) \n",
            cint_field_am_i_o_e_presel_id,cint_field_am_i_o_e_iPMF1_ctx_id);

    sal_memcpy(&cint_field_am_i_o_e_ipmf1_fg_info,&fg_info,sizeof(fg_info));
    return 0;
}

struct cint_field_am_i_o_e_node_ipmf2_flow_entry_val_s{
    int flow_label;
    int L_bit;

    int statId0;
    cint_field_am_stat_profile_s statProfile0; /* Must be used with the values set below*/
    int command_id; 
};

cint_field_am_i_o_e_node_ipmf2_flow_entry_val_s g_cint_field_am_i_o_e_ipmf2_flow_ent = {
        20,
        1,

        1000,
        {1,1,0,1}, /* stat profile - all 1 except type which is 0*/
        7,
};

int cint_field_alternate_marking_inter_or_egress_node_iPMF2_flow_entry_add(int unit,bcm_field_entry_t *entry_handle)
{
    bcm_field_entry_info_t ent_info;
    bcm_field_group_info_t *fg_info = &cint_field_am_i_o_e_iPMF2_flow_fg_info;
    uint32 flow_label_and_L_bit=0;
    int idx;
    int rv;
    int lm_read_index;
    bcm_field_entry_info_t_init(&ent_info);

    /* command-id 7 maps to lm_read_index 0, command-id 8 to lm_read_index 1, command-id 9 to lm_read_index 2 */
    lm_read_index = bcmFieldStatOamLmIndex0;

    ent_info.nof_entry_actions =fg_info->nof_actions;

    for(idx = 0 ; idx < ent_info.nof_entry_actions; idx++ )
    {
        ent_info.entry_action[idx].type = fg_info->action_types[idx];
    }
    ent_info.entry_action[0].value[0] = g_cint_field_am_i_o_e_ipmf2_flow_ent.statId0;
    ent_info.entry_action[1].value[0] = g_cint_field_am_i_o_e_ipmf2_flow_ent.statProfile0.is_meter;
    ent_info.entry_action[1].value[1] = g_cint_field_am_i_o_e_ipmf2_flow_ent.statProfile0.is_lm;
    ent_info.entry_action[1].value[2] = g_cint_field_am_i_o_e_ipmf2_flow_ent.statProfile0.type_id;
    ent_info.entry_action[1].value[3] = g_cint_field_am_i_o_e_ipmf2_flow_ent.statProfile0.valid;
    ent_info.entry_action[2].value[0] = lm_read_index;

    ent_info.nof_entry_quals = fg_info->nof_quals;
    for(idx = 0 ; idx < ent_info.nof_entry_quals; idx++ )
    {
        ent_info.entry_qual[idx].type = fg_info->qual_types[idx];
        ent_info.entry_qual[idx].mask[0] = -1;
    }

    /**we use same qualifier for flow_id and Lbit to save FFC (those 2 field are next to each other)*/
    flow_label_and_L_bit = g_cint_field_am_i_o_e_ipmf2_flow_ent.flow_label << 1;
    flow_label_and_L_bit += g_cint_field_am_i_o_e_ipmf2_flow_ent.L_bit;
    ent_info.entry_qual[0].value[0] = flow_label_and_L_bit;

    rv = bcm_field_entry_add(unit,0,cint_field_am_i_o_e_iPMF2_flow_fg_id,&ent_info,entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }

    printf("iPMF1 FG(%d) entry_add 0x(%x): \n flow_label(0x%x) L_bit(%d)\n StatId0(%d) StatProfile0(0x%x) \n\n ", cint_field_am_i_o_e_iPMF2_flow_fg_id, (*entry_handle),
            ent_info.entry_qual[0].value[0],ent_info.entry_qual[1].value[0],
            ent_info.entry_action[0].value[0],ent_info.entry_action[1].value[0]);




    return 0;
}


struct cint_field_am_i_o_e_node_ipmf2_D_entry_val_s{
    int D_bit;

    int SnoopRaw;
};

cint_field_am_i_o_e_node_ipmf2_D_entry_val_s g_cint_field_am_i_o_e_ipmf2_D_ent = {
        1,

        0xE01, /*Command 1 with strength 7. Command should be updated with a valid command*/
};

int cint_field_alternate_marking_inter_or_egress_node_iPMF2_D_entry_add(int unit,bcm_field_entry_t *entry_handle)
{
    bcm_field_entry_info_t ent_info;
    bcm_field_group_info_t *fg_info = &cint_field_am_i_o_e_iPMF2_D_fg_info;
    int idx;
    int rv;
    bcm_field_entry_info_t_init(&ent_info);


    ent_info.nof_entry_actions =fg_info->nof_actions;

    for(idx = 0 ; idx < ent_info.nof_entry_actions; idx++ )
    {
        ent_info.entry_action[idx].type = fg_info->action_types[idx];
    }
    ent_info.entry_action[0].value[0] = g_cint_field_am_i_o_e_ipmf2_D_ent.SnoopRaw;


    ent_info.nof_entry_quals = fg_info->nof_quals;
    for(idx = 0 ; idx < ent_info.nof_entry_quals; idx++ )
    {
        ent_info.entry_qual[idx].type = fg_info->qual_types[idx];
        ent_info.entry_qual[idx].mask[0] = -1;
    }

    ent_info.entry_qual[0].value[0] = g_cint_field_am_i_o_e_ipmf2_D_ent.D_bit;

    rv = bcm_field_entry_add(unit,0,cint_field_am_i_o_e_iPMF2_D_fg_id,&ent_info,entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }

    printf("iPMF1 FG(%d) entry_add 0x(%x): \n  D_bit(%d)\n SnoopRaw(0x%x) IPT_Profile(%d) \n\n ", cint_field_am_i_o_e_iPMF2_D_fg_id, (*entry_handle),
            ent_info.entry_qual[0].value[0],
            ent_info.entry_action[0].value[0],ent_info.entry_action[1].value[0]);




    return 0;
}

int cint_field_alternate_marking_inter_or_egress_node_iPMF2_fg_add(int unit)
{
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_group_info_t fg_info;
    bcm_field_presel_entry_id_t p_id;
    int idx;
    bcm_field_action_info_t action_info;
    void *dest_char;
    int rv = BCM_E_NONE;
    bcm_field_qualifier_info_create_t qual_info;

    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    qual_info.size = 1;
    sal_strncpy_s(dest_char, "AM_i_o_e_D", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit,0,&qual_info,&cint_field_am_i_o_e_D_bit_qual_id);
    if (rv != BCM_E_NONE)
    {
      printf("Error (%d), in bcm_field_qualifier_create D bit \n", rv);
      return rv;
    }

    /**21 bit include flow label and L bit, done to save FFCs (for initial keys of cascading from context)*/
    qual_info.size = 21;
    sal_strncpy_s(dest_char, "AM_i_o_e_Flow_L", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit,0,&qual_info,&cint_field_am_i_o_e_Flow_label_qual_id);
    if (rv != BCM_E_NONE)
    {
      printf("Error (%d), in bcm_field_qualifier_create Flow bit \n", rv);
      return rv;
    }


    bcm_field_group_info_t_init(&fg_info);

    fg_info.stage = bcmFieldStageIngressPMF2;
    fg_info.fg_type = bcmFieldGroupTypeTcam;

    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = cint_field_am_i_o_e_Flow_label_qual_id;

    fg_info.nof_actions = 3;
    fg_info.action_types[0] = bcmFieldActionStatId0 + g_cint_field_am_i_o_e_ipmf2_flow_ent.command_id;
    fg_info.action_types[1] = bcmFieldActionStatProfile0 + g_cint_field_am_i_o_e_ipmf2_flow_ent.command_id;
    fg_info.action_types[2] = bcmFieldActionStatOamLM;


    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "AM_i_o_e_FG_flow", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_am_i_o_e_iPMF2_flow_fg_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_group_add\n", rv);
       return rv;
    }
    printf("Created TCAM field group (%d) in iPMF2_Flow I o E node \n", cint_field_am_i_o_e_iPMF2_flow_fg_id);
    sal_memcpy(&cint_field_am_i_o_e_iPMF2_flow_fg_info,&fg_info,sizeof(fg_info));

    bcm_field_group_info_t_init(&fg_info);

    fg_info.stage = bcmFieldStageIngressPMF2;
    fg_info.fg_type = bcmFieldGroupTypeTcam;

    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = cint_field_am_i_o_e_D_bit_qual_id;

    fg_info.nof_actions = 1;
    /*In case D bit is set*/
    fg_info.action_types[0] = bcmFieldActionSnoopRaw;


    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "AM_i_o_e_FG_D", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_am_i_o_e_iPMF2_D_fg_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_group_add\n", rv);
       return rv;
    }
    printf("Created TCAM field group (%d) in iPMF2_D I o E node \n", cint_field_am_i_o_e_iPMF2_D_fg_id);
    sal_memcpy(&cint_field_am_i_o_e_iPMF2_D_fg_info,&fg_info,sizeof(fg_info));


    return 0;
}

int cint_field_alternate_marking_inter_or_egress_iPMF2_attach_per_context(int unit, int index)
{
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_group_info_t fg_info;
    bcm_field_presel_entry_id_t p_id;
    int idx;
    bcm_field_action_info_t action_info;
    void *dest_char;
    int rv = BCM_E_NONE;
    int current_mpls_label_offset = 0;
    /**Create context**/
    bcm_field_context_info_t_init(&context_info);

    

    dest_char = &(context_info.name[0]);
    /*sal_strncpy_s(dest_char, "__AM_I_o_e_Ctx", sizeof(context_info.name));*/
    context_info.cascaded_from = cint_field_am_i_o_e_iPMF1_ctx_id;
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF2, &context_info, &cint_field_am_i_o_e_iPMF2_ctx_id[index]);
    if (rv != BCM_E_NONE)
    {
      printf("Error (%d), in bcm_field_context_create\n", rv);
      return rv;
    }

    /*Attach Context to FG Flow Label and L bit*/
    bcm_field_group_attach_info_t_init(&attach_info);
    sal_memcpy(&fg_info,&cint_field_am_i_o_e_iPMF2_flow_fg_info,sizeof(fg_info));
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for(idx=0; idx < attach_info.payload_info.nof_actions; idx++)
    {
        attach_info.payload_info.action_types[idx] = fg_info.action_types[idx];
    }

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    for(idx=0; idx < attach_info.key_info.nof_quals; idx++)
    {
        attach_info.key_info.qual_types[idx] = fg_info.qual_types[idx];
    }
    current_mpls_label_offset = (index + 1)*cint_field_am_i_o_e_mpls_qual_size; /* index points to the "special label", index + 1 points to the flow id lable*/
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = current_mpls_label_offset;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_am_i_o_e_iPMF2_flow_fg_id, cint_field_am_i_o_e_iPMF2_ctx_id[index], &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached  FG_Flow (%d) to context (%d) Stage(iPMF2)\n",cint_field_am_i_o_e_iPMF2_flow_fg_id,cint_field_am_i_o_e_iPMF2_ctx_id[index]);


    /*Attach Context to FG Flow Label and L bit*/
    bcm_field_group_attach_info_t_init(&attach_info);
    sal_memcpy(&fg_info,&cint_field_am_i_o_e_iPMF2_D_fg_info,sizeof(fg_info));
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for(idx=0; idx < attach_info.payload_info.nof_actions; idx++)
    {
        attach_info.payload_info.action_types[idx] = fg_info.action_types[idx];
    }

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    current_mpls_label_offset = (index + 1)*cint_field_am_i_o_e_mpls_qual_size;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 1;
    /**D bit is located 21 bit after the start of MPLS label*/
    attach_info.key_info.qual_info[0].offset = current_mpls_label_offset+21;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_am_i_o_e_iPMF2_D_fg_id, cint_field_am_i_o_e_iPMF2_ctx_id[index], &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached  FG_D (%d) to context (%d) Stage(iPMF2)\n",cint_field_am_i_o_e_iPMF2_D_fg_id,cint_field_am_i_o_e_iPMF2_ctx_id[index]);



    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_am_i_o_e_presel_id+index;
    p_id.stage = bcmFieldStageIngressPMF2;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_am_i_o_e_iPMF2_ctx_id[index];
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyCascadedKeyValue;
    p_data.qual_data[0].qual_arg = cint_field_am_i_o_e_iPMF1_fg_id;
    p_data.qual_data[0].qual_value = index +1;
    p_data.qual_data[0].qual_mask = 0xF;


    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
      printf("Error (%d), in dnx_field_presel_set \n", rv);
      return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF2) context(%d) CascadedVal(%d) \n",
           (cint_field_am_i_o_e_presel_id+index),cint_field_am_i_o_e_iPMF2_ctx_id[index],index);


    return 0;
}

int cint_field_alternate_marking_inter_or_egress_iPMF2_attach(int unit)
{
    int idx;
    int rv=0;

    for(idx=0; idx< NOF_MPLS_QUALS ; idx++)
    {
        rv = cint_field_alternate_marking_inter_or_egress_iPMF2_attach_per_context(unit,idx);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in cint_field_alternate_marking_inter_or_egress_iPMF2_attach_per_context idx(%d)\n", rv,idx);
            return rv;
        }
    }
    return 0;
}

int cint_field_alternate_marking_inter_or_egress_node_main(int unit)
{
    bcm_field_entry_t entry_handle;
    int rv = BCM_E_NONE;
    printf("=================================== \n");
    printf("intermidiate or egress node Start \n");
    printf("=================================== \n");

    rv = cint_field_alternate_marking_inter_or_egress_node_iPMF1(unit);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in cint_field_alternate_marking_inter_or_egress_node_iPMF1 \n", rv);
       return rv;
    }

    printf("I o E Node iPMF1 configuration Done! \n");
    rv = cint_field_alternate_marking_inter_or_egress_node_iPMF2_fg_add(unit);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in cint_field_alternate_marking_inter_or_egress_iPMF2_fg_add \n", rv);
       return rv;
    }

    printf("I o E Node iPMF2 FG_ADD Done! \n");

    rv = cint_field_alternate_marking_inter_or_egress_iPMF2_attach(unit);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in cint_field_alternate_marking_inter_or_egress_iPMF2_attach \n", rv);
       return rv;
    }
    printf("I o E Node iPMF2 Attach Done! \n");

    rv = cint_field_alternate_marking_inter_or_egress_node_iPMF1_entry_add(unit,&entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in cint_field_alternate_marking_inter_or_egress_node_iPMF1_entry_add \n", rv);
       return rv;
    }
    rv = cint_field_alternate_marking_inter_or_egress_node_iPMF2_flow_entry_add(unit,&entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in cint_field_alternate_marking_inter_or_egress_node_iPMF2_flow_entry_add \n", rv);
       return rv;
    }
    rv = cint_field_alternate_marking_inter_or_egress_node_iPMF2_D_entry_add(unit,&entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in cint_field_alternate_marking_inter_or_egress_node_iPMF2_D_entry_add \n", rv);
       return rv;
    }
    return 0;

}

bcm_field_qualify_t cint_field_am_i_pe_Flow_label_qual_id;
bcm_field_qualify_t cint_field_am_i_pe_Flow_label_ind_qual_id;
bcm_field_qualify_t cint_field_am_i_pe_udh_size_qual_id;
bcm_field_group_t cint_field_am_i_pe_iPMF2_fg_id=0;

/*
 * Configuration for intermediate PE node
 * In iPMF2, Load the Flow label(32bits) on UDH1_Data and Flow label indication(32bits) on UDH2_Data.
 */
int
cint_field_alternate_marking_inter_pe_ipmf2_udh(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    bcm_field_fem_action_info_t fem_action_info;

    void *dest_char;
    int rv = BCM_E_NONE;
    int ii = 0;
    int idx;

    /* 32 bits for flow label */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    qual_info.size = 32;
    sal_strncpy_s(dest_char, "AM_i_pe_Flow_L", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_am_i_pe_Flow_label_qual_id);
    if (rv != BCM_E_NONE)
    {
      printf("Error (%d), in bcm_field_qualifier_create Flow bit \n", rv);
      return rv;
    }

    /* 32 bits for flow label indication */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 32;
    sal_strncpy_s(dest_char, "AM_i_pe_Flow_L_I", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_am_i_pe_Flow_label_ind_qual_id);
    if (rv != BCM_E_NONE)
    {
      printf("Error (%d), in bcm_field_qualifier_create Flow bit \n", rv);
      return rv;
    }

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 8;
    sal_strncpy_s(dest_char, "AM_i_pe_udh_size_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_am_i_pe_udh_size_qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_create UDH size\n");
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "AM_i_pe_FG_flow", sizeof(fg_info.name));

    /*
     * Set quals
     */
    fg_info.nof_quals = 3;
    fg_info.qual_types[0] = cint_field_am_i_pe_Flow_label_qual_id;
    fg_info.qual_types[1] = cint_field_am_i_pe_Flow_label_ind_qual_id;
    fg_info.qual_types[2] = cint_field_am_i_pe_udh_size_qual_id;

    /*
     * Set actions
     */
    fg_info.nof_actions = 6;

    /*
     * Write the 64 bits to UDH, from MSB to lsb
     * bcmFieldActionUDHData[0-1] actions are used to fill-in each of the 4 UDH data buffers
     */
    fg_info.action_types[0] = bcmFieldActionUDHData0;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = bcmFieldActionUDHData1;
    fg_info.action_with_valid_bit[1] = FALSE;
    /*
     * Write the UDH type for each 32 bits, indicating the UDH size through IPPD_UDH_TYPE_TO_SIZE_MAP.
     * bcmFieldActionUDHBase[0-3] actions are used to set the sizes of the 4 UDH data buffers - can be
     * 0/8/16/32
     */
    fg_info.action_types[2] = bcmFieldActionUDHBase0;
    fg_info.action_with_valid_bit[2] = FALSE;
    fg_info.action_types[3] = bcmFieldActionUDHBase1;
    fg_info.action_with_valid_bit[3] = FALSE;
    fg_info.action_types[4] = bcmFieldActionUDHBase2;
    fg_info.action_with_valid_bit[4] = FALSE;
    fg_info.action_types[5] = bcmFieldActionUDHBase3;
    fg_info.action_with_valid_bit[5] = FALSE;

    rv = bcm_field_group_add(unit, 0, &fg_info, &cint_field_am_i_pe_iPMF2_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add ipmf2_fg\n", rv);
        return rv;
    }

    for(idx = 0; idx < NOF_MPLS_QUALS; idx++)
    {
        bcm_field_group_attach_info_t_init(&attach_info);
        attach_info.payload_info.nof_actions = fg_info.nof_actions;
        for(ii = 0; ii < attach_info.payload_info.nof_actions; ii++)
        {
            attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
            attach_info.key_info.qual_info[ii].input_type = bcmFieldInputTypeLayerAbsolute;
            attach_info.key_info.qual_info[ii].input_arg = 1;
        }
        attach_info.key_info.qual_info[0].offset = (idx)*cint_field_am_i_o_e_mpls_qual_size;;
        attach_info.key_info.qual_info[1].offset = (idx + 1)*cint_field_am_i_o_e_mpls_qual_size;;
        /*
         * Use type 3 for each of the four UDH types to get UDH size 4 for each
         * We write a constand value, which is 0xAA, which maps to configure the UDH to 32bits
         */
        attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeConst;
        attach_info.key_info.qual_info[2].input_arg = 0xFF;

        attach_info.key_info.nof_quals = fg_info.nof_quals;
        for(ii=0; ii < attach_info.key_info.nof_quals; ii++)
        {
            attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
        }

        rv = bcm_field_group_context_attach(unit, 0, cint_field_am_i_pe_iPMF2_fg_id, cint_field_am_i_o_e_iPMF2_ctx_id[idx], &attach_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_group_context_attach\n", rv);
            return rv;
        }
        printf("Attached  FG_Flow (%d) to context (%d) Stage(iPMF2)\n", cint_field_am_i_pe_iPMF2_fg_id, cint_field_am_i_o_e_iPMF2_ctx_id[idx]);
    }

    return 0;
}

int CINT_FIELD_AM_NOF_EPMF_BASIC_QUALS   = 2;
int CINT_FIELD_AM_NOF_EPMF_BASIC_ACTIONS = 1;

/** Context Selection info per field stage (IPMF1-2-3 and EPMF). */
cint_field_udh_cascading_cs_info_t cint_field_am_i_pe_udh_epmf_cs_info_array = {
    /**  context_id   |   context_name   |   presel_id    |  fwd_layer  */
    0, "AM_i_pe_epmf_ctx", 53, bcmFieldLayerTypeIp4};

/** EPMF Qualifiers info. */
cint_field_udh_cascading_qual_info_t cint_field_am_i_pe_udh_epmf_qual_info_array[CINT_FIELD_AM_NOF_EPMF_BASIC_QUALS] = {
 /**  qual_type                 |   qual_value              |   qual_mask    |  input_type    |    input_arg  |  qual_offset  */
    {bcmFieldQualifyUDHData0, 0x0000a000, 0xFFFFF000, {bcmFieldInputTypeMetaData, 0, 0}},
    {bcmFieldQualifyUDHBase0, 0x3, 0x3, {bcmFieldInputTypeMetaData, 0, 0}}
};

/** BASIC Actions info. */
cint_field_udh_cascading_action_info_t cint_field_am_i_pe_udh_epmf_action_info_array[CINT_FIELD_AM_NOF_EPMF_BASIC_ACTIONS] = {
 /**     action_type       |   action_value   */
    {NULL, bcmFieldActionAceEntryId, 0, {0}},
};

cint_field_udh_cascading_fg_info_t cint_field_am_i_pe_udh_epmf_fg_info_array[1] = {
    /** EPMF info. */
    {
     0,                            /** Field group ID */
     "AM_i_pe_epmf_fg",            /** Field group Name */
     bcmFieldStageEgress,          /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_AM_NOF_EPMF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_AM_NOF_EPMF_BASIC_ACTIONS,       /** Number of tested actions */
     cint_field_am_i_pe_udh_epmf_cs_info_array,     /** Context Selection info */
     cint_field_am_i_pe_udh_epmf_qual_info_array,         /** Qualifiers info */
     cint_field_am_i_pe_udh_epmf_action_info_array        /** Actions info */
     }
};

/*
 * Configuration for intermediate PE node.
 * In ePMF, Identify the packet carries the FLoFLI and apply AceContextValue action.
 */
int
cint_field_alternate_marking_inter_pe_epmf_udh(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_ace_format_t ace_format_id;
    uint32 ace_entry_handle;
    void *dest_char;

    /*
     * Configure the ACE format.
     */
    bcm_field_ace_format_info_t_init(&ace_format_info);

    ace_format_info.nof_actions = 1;
    ace_format_info.action_types[0] = bcmFieldActionAceContextValue;

    dest_char = &(ace_format_info.name[0]);
    sal_strncpy_s(dest_char, "AM_ACE", sizeof(ace_format_info.name));
    rv = bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_ace_format_add \n", rv);
       return rv;
    }

    bcm_field_ace_entry_info_t_init(&ace_entry_info);
    ace_entry_info.nof_entry_actions = 1;
    ace_entry_info.entry_action[0].type = bcmFieldActionAceContextValue;
    ace_entry_info.entry_action[0].value[0] = bcmFieldAceContextAlternateMarkingIntermediate;

    rv = bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, &ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_ace_entry_add \n", rv);
       return rv;
    }
    printf("ACE Format (%d) and ACE entry 0x(%x) created \n", ace_format_id, ace_entry_handle);

    cint_field_am_i_pe_udh_epmf_action_info_array[0].action_value = ace_entry_handle;

    /*
     * Creating EPMF Field Group.
     */
    rv = cint_field_udh_cascading_group_config(unit, cint_field_am_i_pe_udh_epmf_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}
