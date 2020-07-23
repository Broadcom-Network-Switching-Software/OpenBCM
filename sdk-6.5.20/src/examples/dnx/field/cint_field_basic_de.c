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
 * cint ../../../src/examples/dnx/field/cint_field_basic_de.c
 * cint;
 * int unit = 0;
 * bcm_field_context_t context_id = 20;
 * field_de_main(unit, context_id);
 *
 * Configuration example end
 *
 * Assume that the packet is build from ITMH_PPH
 * Take the 2 msb of reserved field from ITMH packet and append to str_profile field from PPH packet to generate trap strength
 * Trap code is set to 511 since it points to action do nothing
 * trap qualifier is set with zeros
 */


bcm_field_qualify_t itmh_trap_strength_3_bit = 0;
bcm_field_qualify_t pph_trap_strength_1_bit = 0;
bcm_field_qualify_t trap_code_ones = 0;
bcm_field_qualify_t dest_qual = 0;
bcm_field_action_t dest_action = 0;
bcm_field_action_t trap_action = 0;
bcm_field_group_t fg_id = 0;
bcm_field_presel_entry_id_t presel_entry_id;
    
/**
* \brief
*  Configure basic DE
* \param [in] unit        - Device id
* \param [in] context_id  - Context it to attach to Field group
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int field_de_main(int unit, bcm_field_context_t context_id)
{
    
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    bcm_field_group_info_t fg_info;
    int qual_idx = 0;
    int act_idx = 0;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    int ii=0;
    int rv= 0;


    bcm_field_context_info_t_init(&context_info);
    rv = bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF1, &context_info, &context_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_context_create\n");
        return rv;
    }

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 3;
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &itmh_trap_strength_3_bit);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_create itmh_trap_strength_3_bit\n");
        return rv;
    }

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    bcm_field_qualifier_create(unit, 0, &qual_info, &pph_trap_strength_1_bit);

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 21;
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &dest_qual);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_create dest_qual\n");
        return rv;
    }

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 9;
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &trap_code_ones);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_create, trap_code_ones\n");
        return rv;
    }

    /** Create destination action to ignore destination qualifier which is 0*/
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionForward;
    action_info.prefix_size = 11;
    action_info.prefix_value = 0;
    action_info.size = 21;
    action_info.stage = bcmFieldStageIngressPMF2;
    rv = bcm_field_action_create(unit, 0, &action_info, &dest_action);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_action_create dest_action\n");
        return rv;
    }

    /** Create Trap action to have trap qualifier filled with zeros without wasting key space.*/
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionTrap;
    action_info.prefix_size = 19;
    action_info.prefix_value = 0;
    action_info.size = 13;
    action_info.stage = bcmFieldStageIngressPMF2;
    rv = bcm_field_action_create(unit, 0, &action_info, &trap_action);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_action_create trap_action \n");
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.nof_quals = 4;
    fg_info.stage = bcmFieldStageIngressPMF2;

    /**Using the qual_idx so we can move qualifiers in key easy without modifying the index of qual*/
    qual_idx = 0;

    fg_info.qual_types[qual_idx] = dest_qual;
    qual_idx++;
    /*
     * Build the qualifiers for action of trap_profile 32 bit: from lsb -> trap_code, trap_strength, trap_qual
     * Trap Qual will be written as an action prefix for now.
     */
    fg_info.qual_types[qual_idx] = trap_code_ones;
    qual_idx++;
    fg_info.qual_types[qual_idx] = itmh_trap_strength_3_bit;
    qual_idx++;
    fg_info.qual_types[qual_idx] = pph_trap_strength_1_bit;
    qual_idx++;
    

    fg_info.nof_actions = 2;

    /**Same for the actions to move them*/
    act_idx = 0;

    /** Order of actions should be the same as order of qualifiers*/
    fg_info.action_types[act_idx] = dest_action;
    fg_info.action_with_valid_bit[act_idx] = FALSE;
    act_idx++;
    fg_info.action_types[act_idx] = trap_action;
    fg_info.action_with_valid_bit[act_idx] = FALSE;
    act_idx++;

    rv = bcm_field_group_add(unit, 0, &fg_info, &fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_add\n");
        return rv;
    }

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for(ii=0; ii< qual_idx; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for(ii=0; ii<act_idx; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    qual_idx = 0;
    /**Destination 21 bits from ITMH header*/
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[qual_idx].input_arg = 0;
    attach_info.key_info.qual_info[qual_idx].offset = 10;
    qual_idx++;
    /**Trap code*/
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[qual_idx].input_arg = 0;
    
    attach_info.key_info.qual_info[qual_idx].offset = 3339;
    qual_idx++;
    /**First 3 bits of trap strength are taken from reserved bits of ITMH*/
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[qual_idx].input_arg = 0;
    attach_info.key_info.qual_info[qual_idx].offset = 0;
    qual_idx++;
    /** MSB bit of trap strength is taken from PPH header*/
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[qual_idx].input_arg = 0;
    attach_info.key_info.qual_info[qual_idx].offset = 40 + 95;
    qual_idx++;
    


    rv = bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_attach\n");
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    /**For iPMF2, iPMF1 presel must be configured*/
    presel_entry_id.presel_id = 61;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.context_id = context_id;
    presel_entry_data.entry_valid = TRUE;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[0].qual_arg = 0;
    /**31 is ITMH*/
    presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeTm;
    presel_entry_data.qual_data[0].qual_mask = 0x1F;
    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_presel_set\n");
        return rv;
    }

    return 0;
}



/**
* \brief
*  Delete all configuration that was done be main function
* \param [in] unit        - Device ID
* \param [in] context_id  - Context id to destroy
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int field_basic_de_destroy(int unit, bcm_field_context_t context_id)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    int rv;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_presel_set\n");
        return rv;
    }

    rv = bcm_field_group_context_detach(unit,fg_id,context_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }

    rv = bcm_field_group_delete(unit,fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_delete\n");
        return rv;
    }

    rv = bcm_field_context_destroy(unit,bcmFieldStageIngressPMF1,context_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_context_destroy\n");
        return rv;
    }

    rv = bcm_field_qualifier_destroy(unit, itmh_trap_strength_3_bit);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_destroy itmh_trap_strength_3_bit\n");
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, pph_trap_strength_1_bit);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_destroy pph_trap_strength_1_bit \n");
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, trap_code_ones);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_destroy trap_code\n");
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, dest_qual);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_destroy dest_qual\n");
        return rv;
    }
    rv = bcm_field_action_destroy(unit, dest_action);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_action_destroy dest_action\n");
        return rv;
    }
    rv = bcm_field_action_destroy(unit, trap_action);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_action_destroy trap_action\n");
        return rv;
    }
    return 0;
}
