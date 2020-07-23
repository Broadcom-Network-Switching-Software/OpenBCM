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
 * cint ../../../src/examples/dnx/field/cint_field_hash_ipt.c
 * cint;
 * int unit = 0;
 * cint_field_hash_ipt_main(unit);
 *
 * Configuration example end
 *
 * Sets-up a hashing key in iPMF1 comprised of 5-tuple qualifiers (src-ip,
 * dst-ip, protocol, L4 source port, L4 dst-port) and build hash key.
 * This hash key serves as qualifier in TCAM FG lookup in iPMF2 stage.
 * The result of iPMF2  is passed to iPMF3 where is being used as key for Lookup in TCAM FG
 *
 * Assumption: packet forwarding layer is IPv4 and packet structure is ETH_IPv4_L4
 *
 * cint_field_hash_ipt_main(unit); - configure FG's/Context/Presel
 * cint_field_hash_ipt_ipmf2_tcam_entry_add(unit,hash) - add entries to TCAM
 * cint_field_hash_ipt_ipmf3_tcam_entry_add(unit,in_port,tc,stat_samp,*handle) - add entries to TCAM
 */

bcm_field_group_t   cint_field_hash_ipt_fg_id_ipmf2_hash = 0;
bcm_field_context_t cint_field_hash_ipt_context_id_ipmf1 = 0;
bcm_field_presel_t              cint_field_hash_ipt_presel_id_ipmf1 = 88;

bcm_field_group_t   cint_field_hash_ipt_fg_id_tcam = 0;
bcm_field_context_t cint_field_hash_ipt_context_id_ipmf3 = 0;
bcm_field_presel_t              cint_field_hash_ipt_presel_id_ipmf3 = 88;



/**
* \brief
*  Adds TCAM  entry to FG of iPMF2
* \param [in] unit        -  Device id
* \param [in] hash        -  hash value to add to identify that packet is hit
* \param [out] entry_handle        -  entry Id (used to manage entry)
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_hash_ipt_ipmf2_tcam_entry_add(int unit, uint32 hash,bcm_field_entry_t *entry_handle)
{
    bcm_field_entry_info_t ent_info;
    int rv = BCM_E_NONE;

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 200;

    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = bcmFieldQualifyHashValue;
    ent_info.entry_qual[0].value[0] = hash;
    ent_info.entry_qual[0].mask[0] = 0xFFFF;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = bcmFieldActionContainer;
    ent_info.entry_action[0].value[0] = 1;

    rv = bcm_field_entry_add(unit, 0, cint_field_hash_ipt_fg_id_ipmf2_hash, &ent_info, entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("iPMF2 TCAM Entry_Id (%d), Entry Hash value (%d)\n", *entry_handle, hash);

    return 0;
}

/**
* \brief
*  Adds TCAM entry to FG of iPMF3
* \param [in] unit        -  Device id
* \param [in] in_port        -  in port value to qualify upon
* \param [in] tc_val        -  tc value to qualify upon
* \param [in] gport_stat_samp - Statistical Sampling profile - as returned from mirror_create() API (its encoded as GPORT)
* \param [in] ipt_profile        -  Chosen IPT profile Action
* \param [out] entry_handle        -  entry Id (used to manage entry)
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_hash_ipt_ipmf3_tcam_entry_add(int unit,uint32 in_port, uint32 tc_val,
                                                    uint32 gport_stat_samp, uint32 ipt_profile,
                                                    bcm_field_entry_t *entry_handle)
{
    bcm_field_entry_info_t ent_info;
    int rv = BCM_E_NONE;

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 200;

    ent_info.nof_entry_quals = 2;
    ent_info.entry_qual[0].type = bcmFieldQualifyContainer;
    ent_info.entry_qual[0].value[0] = 1;
    ent_info.entry_qual[0].mask[0] = 0x1;


    ent_info.entry_qual[1].type = bcmFieldQualifyInPort;
    ent_info.entry_qual[1].value[0] = in_port;
    ent_info.entry_qual[1].mask[0] = 0x1FF;


    ent_info.entry_qual[2].type = bcmFieldQualifyIntPriority;
    ent_info.entry_qual[2].value[0] = tc_val;
    ent_info.entry_qual[2].mask[0] = 0x7;

    ent_info.nof_entry_actions = 3;
    ent_info.entry_action[0].type = bcmFieldActionStatSampling;
    ent_info.entry_action[0].value[0] = gport_stat_samp;
    ent_info.entry_action[0].value[1] = 0;

    ent_info.entry_action[1].type = bcmFieldActionIPTProfile;
    ent_info.entry_action[1].value[0] = ipt_profile;


    ent_info.entry_action[2].type = bcmFieldActionIPTCommand;
    if(ipt_profile > 0)
    {
        ent_info.entry_action[2].value[0] = 1;
    }
    else
    {
        ent_info.entry_action[2].value[0] = 0 ;
    }

    rv = bcm_field_entry_add(unit, 0, cint_field_hash_ipt_fg_id_tcam, &ent_info, entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("TCAM Entry Id (%d) In_port (%d) TC (%d) with Stat_Code (%d) \n",*entry_handle, in_port,tc_val,gport_stat_samp);

    return 0;
}

/**
* \brief
*  Creates and configure Context+FG for iPMF1/2/3
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_hash_ipt_main(int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_hash_info_t hash_info;
    bcm_field_context_info_t context_info;

    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    void *dest_char;
    int rv = BCM_E_NONE;

    /********************
     * Create Context for iPMF1/2
 */
    printf("Creating new context for the hashing\n");
    bcm_field_context_info_t_init(&context_info);
    context_info.hashing_enabled = TRUE;
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "Hash_ipt_ipmf1_2", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &cint_field_hash_ipt_context_id_ipmf1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    printf("Created Context %d for iPMF1 (can also be used for iPMF2)\n", cint_field_hash_ipt_context_id_ipmf1);


    /********************
     * Start iPMF1 configuration - Configure Hash stamp creation
 */

    bcm_field_context_hash_info_t_init(&hash_info);
    hash_info.hash_function = bcmFieldContextHashFunctionCrc16Bisync;
    hash_info.order = TRUE;
    hash_info.hash_config.function_select = bcmFieldContextHashActionValueReplaceCrc;
    hash_info.hash_config.action_key = bcmFieldContextHashActionKeyAdditionalLbKey;
    hash_info.key_info.nof_quals = 5;
    hash_info.key_info.qual_types[0] = bcmFieldQualifySrcIp;
    hash_info.key_info.qual_types[1] = bcmFieldQualifyDstIp;
    hash_info.key_info.qual_types[2] = bcmFieldQualifyIp4Protocol;
    hash_info.key_info.qual_types[3] = bcmFieldQualifyL4SrcPort;
    hash_info.key_info.qual_types[4] = bcmFieldQualifyL4DstPort;
    hash_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[0].input_arg = 1;
    hash_info.key_info.qual_info[0].offset = 0;
    hash_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[1].input_arg = 1;
    hash_info.key_info.qual_info[1].offset = 0;
    hash_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[2].input_arg = 1;
    hash_info.key_info.qual_info[2].offset = 0;
    hash_info.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[3].input_arg = 2;
    hash_info.key_info.qual_info[3].offset = 0;
    hash_info.key_info.qual_info[4].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[4].input_arg = 2;
    hash_info.key_info.qual_info[4].offset = 0;
    bcm_field_context_hash_create(unit, 0, bcmFieldStageIngressPMF1,cint_field_hash_ipt_context_id_ipmf1,&hash_info );


    /********************
     * Create and attach TCAM group in iPMF2
 */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF2;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyHashValue;

    /* Set actions -
     * Container is general bits used to pass info from iPMF1/2 to iPMF3 - here its used as hit bit
 */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionContainer;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "ipt_Hash_lookup", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_hash_ipt_fg_id_ipmf2_hash);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("TCAM FG Id (%d) was add to iPMF2 Stage\n",cint_field_hash_ipt_fg_id_ipmf2_hash);

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0]; 
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_hash_ipt_fg_id_ipmf2_hash, cint_field_hash_ipt_context_id_ipmf1, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("TCAM FG Id (%d) in iPMF2 attached to context (%d) \n", cint_field_hash_ipt_fg_id_ipmf2_hash, cint_field_hash_ipt_context_id_ipmf1);

    /********************
     * Configure Context Selection for iPMF1/2
 */

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_hash_ipt_presel_id_ipmf1;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_hash_ipt_context_id_ipmf1;
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
            cint_field_hash_ipt_presel_id_ipmf1,cint_field_hash_ipt_context_id_ipmf1);


    /********************
     * Create Context for iPMF3
 */
    printf("Creating new context for the hashing\n");
    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "IPT_INT_cmd", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, &cint_field_hash_ipt_context_id_ipmf3);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }
    printf("Created Context %d for iPMF3!\n", cint_field_hash_ipt_context_id_ipmf3);


    /********************
     * Create and attach TCAM group in iPMF3 to lookup based on iPMF2 result + additional info
 */
    bcm_field_group_info_t_init(&fg_info);

    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF3;

    /* Set quals */
    fg_info.nof_quals = 3;
    /**Result of hit/miss in iPMF2*/
    fg_info.qual_types[0] = bcmFieldQualifyContainer;
    /* in port*/
    fg_info.qual_types[1] = bcmFieldQualifyInPort;
    /* Traffic Class / TC / Priority*/
    fg_info.qual_types[2] = bcmFieldQualifyIntPriority;

    /* Set actions - Statistical Sampling code - sniff*/
    fg_info.nof_actions = 3;
    fg_info.action_types[0] = bcmFieldActionStatSampling;
    fg_info.action_types[1] = bcmFieldActionIPTProfile;
    fg_info.action_types[2] = bcmFieldActionIPTCommand;

    /**Set which TCAM banks to access*/
    fg_info.tcam_info.bank_allocation_mode = bcmFieldTcamBankAllocationModeSelect;
    fg_info.tcam_info.nof_tcam_banks = 1;
    /**Select last small bank*/
    fg_info.tcam_info.tcam_bank_ids[0] = 15;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "Hash_ipt_TCAM", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_hash_ipt_fg_id_tcam);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("TCAM FG Id (%d) was add to iPMF3 Stage\n",cint_field_hash_ipt_fg_id_tcam);

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    attach_info.payload_info.action_types[2] = fg_info.action_types[2];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;


    rv = bcm_field_group_context_attach(unit, 0, cint_field_hash_ipt_fg_id_tcam, cint_field_hash_ipt_context_id_ipmf3, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("TCAM FG Id (%d) in iPMF3 attached to context (%d) \n", cint_field_hash_ipt_fg_id_tcam, cint_field_hash_ipt_context_id_ipmf3);

    /********************
     * Configure Context Selection for iPMF3
 */

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_hash_ipt_presel_id_ipmf3;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_hash_ipt_context_id_ipmf3;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyContextId;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = cint_field_hash_ipt_context_id_ipmf1;
    p_data.qual_data[0].qual_mask = 0x3F;


    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) configured for stage(iPMF3), based on iPMF2 context (%d) iPMF3 context(%d) chosen \n",
         cint_field_hash_ipt_presel_id_ipmf1,cint_field_hash_ipt_context_id_ipmf1,cint_field_hash_ipt_context_id_ipmf3);
    return 0;
}


/**
* \brief
*  Destroy the configuration.
*   Disable presel
*   Detach context from FG
*   Delete FG
*   Destroy context
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_hash_ipt_destroy(int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_hash_ipt_presel_id_ipmf1;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = FALSE;
    p_data.context_id = 0;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set iPMF1 \n", rv);
       return rv;
    }

    p_id.presel_id = cint_field_hash_ipt_presel_id_ipmf3;
    p_id.stage = bcmFieldStageIngressPMF3;
    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set iPMF3 \n", rv);
       return rv;
    }


    /* Detach the IPMF2 FG from its context */
    rv = bcm_field_group_context_detach(unit, cint_field_hash_ipt_fg_id_ipmf2_hash, cint_field_hash_ipt_context_id_ipmf1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, cint_field_hash_ipt_fg_id_ipmf2_hash, cint_field_hash_ipt_context_id_ipmf1);
        return rv;
    }

    rv = bcm_field_group_delete(unit, cint_field_hash_ipt_fg_id_ipmf2_hash);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, cint_field_hash_ipt_fg_id_ipmf2_hash);
        return rv;
    }

    rv = bcm_field_context_hash_destroy(unit, bcmFieldStageIngressPMF1, cint_field_hash_ipt_context_id_ipmf1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_hash_destroy context_id %d \n", rv, cint_field_hash_ipt_context_id_ipmf1);
        return rv;
    }

    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, cint_field_hash_ipt_context_id_ipmf1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy context_id %d \n", rv, cint_field_hash_ipt_context_id_ipmf1);
        return rv;
    }

    /* Detach the IPMF3 FG from its context */
    rv = bcm_field_group_context_detach(unit, cint_field_hash_ipt_fg_id_tcam, cint_field_hash_ipt_context_id_ipmf3);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, cint_field_hash_ipt_fg_id_tcam, cint_field_hash_ipt_context_id_ipmf3);
        return rv;
    }

    rv = bcm_field_group_delete(unit, cint_field_hash_ipt_fg_id_tcam);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, cint_field_hash_ipt_fg_id_tcam);
        return rv;
    }

    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF3, cint_field_hash_ipt_context_id_ipmf3);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy context_id %d \n", rv, cint_field_hash_ipt_context_id_ipmf3);
        return rv;
    }
    return rv;
}
