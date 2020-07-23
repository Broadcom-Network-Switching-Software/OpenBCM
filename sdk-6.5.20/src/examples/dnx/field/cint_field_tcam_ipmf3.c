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
 * cint ../../../src/examples/dnx/field/cint_field_tcam_ipmf3.c
 * cint;
 * int unit = 0;
 * field_tcam_ipmf3_main(unit);
 *
 * Configuration example end
 *
 *   Basic example for TCAM on iPMF3 stage
 */


bcm_field_group_t cint_tcam_ipmf3_fg_id=0;
bcm_field_entry_t cint_tcam_ipmf3_entry_id[2] = {0};
bcm_field_context_t cint_tcam_ipmf3_context_id;
bcm_field_presel_t cint_tcam_ipmf3_presel = 5;




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
int field_tcam_ipmf3_main(int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    int rv = BCM_E_NONE;


    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF3;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyGeneratedTtl;
    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionPrioIntNew;


    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_tcam_ipmf3_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in iPMF3 \n", cint_tcam_ipmf3_fg_id);

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = 0x8;
    ent_info.entry_qual[0].mask[0] = 0xff;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = 1;

    rv = bcm_field_entry_add(unit, 0, cint_tcam_ipmf3_fg_id, &ent_info, &(cint_tcam_ipmf3_entry_id[0]));
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("Entry add: id:(0x%x)  In_TTL:(0x%x) tc_val:(%d)\n",cint_tcam_ipmf3_entry_id[0],ent_info.entry_qual[0].value[0],
                                                                            ent_info.entry_action[0].value[0] );



    /**Attach Ethernet context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, &cint_tcam_ipmf3_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0]; 
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    
    rv = bcm_field_group_context_attach(unit, 0, cint_tcam_ipmf3_fg_id, cint_tcam_ipmf3_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached  FG (%d) to context (%d)\n",cint_tcam_ipmf3_fg_id,cint_tcam_ipmf3_context_id);


    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_tcam_ipmf3_presel;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_tcam_ipmf3_context_id;
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

    printf("Presel (%d) was configured for stage(iPMF3) context(%d) fwd_layer(Ethernet) \n",
            cint_tcam_ipmf3_presel,cint_tcam_ipmf3_context_id);




    return 0;
}

int field_attach_destroy(int unit)
{
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_entry_qual_t entry_qual_info[BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP];
    int rv = BCM_E_NONE;

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_tcam_ipmf3_presel;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = FALSE;
    p_data.context_id = 0;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }
	
    rv = bcm_field_group_context_detach(unit, cint_tcam_ipmf3_fg_id, cint_tcam_ipmf3_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach eth\n", rv);
        return rv;
    }

    rv = bcm_field_context_destroy(unit,bcmFieldStageIngressPMF3,cint_tcam_ipmf3_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy eth\n", rv);
        return rv;
    }

    rv = bcm_field_entry_delete(unit,cint_tcam_ipmf3_fg_id,entry_qual_info,cint_tcam_ipmf3_entry_id[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete \n", rv);
        return rv;
    }

    rv = bcm_field_group_delete(unit,cint_tcam_ipmf3_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete ipv4\n", rv);
        return rv;
    }

    return 0;
}
