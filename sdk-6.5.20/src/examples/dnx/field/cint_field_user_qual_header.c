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
 * cint ../../../src/examples/dnx/field/cint_field_user_qual_header.c
 * cint;
 * int unit = 0;
 * cint_field_user_qual_header_main(unit);
 *
 * Configuration example end
 *
 *   This cint show simple example of how to create Haeder User qualifier in this case is SrcIpv4
 *   the cint assume packet structure to be: ETH | IPv4  -> when ETH assumed to be the Forwarding layer
 */


bcm_field_group_t cint_field_user_qual_header_fg_id=0;
bcm_field_entry_t cint_field_user_qual_header_entry_id[2] = {0};
bcm_field_context_t cint_field_user_qual_header_context_id_eth;
bcm_field_presel_t cint_field_user_qual_header_presel_eth = 5;
bcm_field_stage_t stage = bcmFieldStageEgress;
bcm_field_qualify_t  cint_field_user_qual_header_id;
bcm_field_action_t cint_field_user_qual_header_action = bcmFieldActionPrioIntNew;




/**
* \brief
*  Configures the written above
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_user_qual_header_main(int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_qualifier_info_create_t qual_info;
    uint32 qual_print;
    void *dest_char;
    int rv = BCM_E_NONE;

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 32;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "SrcIPv4_q", sizeof(qual_info.name));

    rv = bcm_field_qualifier_create(unit,0,&qual_info,&cint_field_user_qual_header_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }
    qual_print = cint_field_user_qual_header_id;
    printf("Created Qualifier (0x%x)  \n",qual_print);


    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = stage;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = cint_field_user_qual_header_id;
    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = cint_field_user_qual_header_action;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SrcIPv4_Lookup", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_user_qual_header_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in iPMF1 \n", cint_field_user_qual_header_fg_id);

    /**Attach Ethernet context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "Ethernet", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, stage, &context_info, &cint_field_user_qual_header_context_id_eth);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0]; 
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[0].input_arg = 1;
    /**IPV4 Sip offset*/
    attach_info.key_info.qual_info[0].offset = 96;

    
    rv = bcm_field_group_context_attach(unit, 0, cint_field_user_qual_header_fg_id, cint_field_user_qual_header_context_id_eth, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached  FG (%d) to context (%d)\n",cint_field_user_qual_header_fg_id,cint_field_user_qual_header_context_id_eth);

    
    /**Its batter to add entries after attach, for batter bank allocation*/
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;


    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    /**IP Address 10.0.0.1*/
    ent_info.entry_qual[0].value[0] = 0x0A000001;
    ent_info.entry_qual[0].mask[0] = 0xffffffff;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = 1;

    rv = bcm_field_entry_add(unit, 0, cint_field_user_qual_header_fg_id, &ent_info, &(cint_field_user_qual_header_entry_id[0]));
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("Entry add: id:(0x%x)  Qual_val:(0x%x) Action_val:(%d)\n",cint_field_user_qual_header_entry_id[0],ent_info.entry_qual[0].value[0],
                                                                            ent_info.entry_action[0].value[0]);
    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_user_qual_header_presel_eth;
    p_id.stage = stage;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_user_qual_header_context_id_eth;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeEth;
    p_data.qual_data[0].qual_mask = 0x1F;


    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(Ethernet) \n",
            cint_field_user_qual_header_presel_eth,cint_field_user_qual_header_context_id_eth);


    return 0;
}

int cint_field_user_qual_header_destroy(int unit)
{
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_entry_qual_t entry_qual_info[BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP];
    int rv = BCM_E_NONE;

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_user_qual_header_presel_eth;
    p_id.stage = bcmFieldStageEgress;
    p_data.entry_valid = FALSE;
    p_data.context_id = 0;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    rv = bcm_field_group_context_detach(unit, cint_field_user_qual_header_fg_id, cint_field_user_qual_header_context_id_eth);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach eth\n", rv);
        return rv;
    }

    rv = bcm_field_context_destroy(unit,bcmFieldStageEgress,cint_field_user_qual_header_context_id_eth);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy eth\n", rv);
        return rv;
    }

    rv = bcm_field_entry_delete(unit,cint_field_user_qual_header_fg_id,entry_qual_info,cint_field_user_qual_header_entry_id[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete \n", rv);
        return rv;
    }

    rv = bcm_field_group_delete(unit,cint_field_user_qual_header_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete \n", rv);
        return rv;
    }

    rv = bcm_field_qualifier_destroy(unit,cint_field_user_qual_header_id);
    if (rv != BCM_E_NONE)
    {
        uint32 qual_print = cint_field_user_qual_header_id;
        printf("Error (%d), in bcm_field_qualifier_destroy, qual_id 0x%x\n", rv,qual_print);
        return rv;
    }

    return 0;
}
