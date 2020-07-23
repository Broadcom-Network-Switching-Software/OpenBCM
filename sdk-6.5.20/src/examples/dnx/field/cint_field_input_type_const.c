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
 * cint ../../../src/examples/dnx/field/cint_field_input_type_const.c
 * cint;
 * int unit = 0;
 * bcm_field_context_t context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;
 * field_input_type_const_main(unit, context_id);
 *
 * Configuration example end
 *
 *   This cint creates DE in iPMF2 so that Meter ID[19:0] = <0xA, learn_data_dst_port[4:0], TTL[7:0], TC[2:0]>, using input type_const
 */


/** To hold trap_code[4:0] */
bcm_field_qualify_t input_type_const_qual_lrn_data_dst_port;
/** To hold 0xA */
bcm_field_qualify_t input_type_const_qual_const;

bcm_field_group_t input_type_const_fg_id;





/**
* \brief
*  create DE in iPMF2 so that Meter ID[19:0] = <0xA, learn_data_dst_port[4:0], TTL[7:0], TC[2:0]>, using input type_const
* \param [in] unit        - Device ID
* \param [in] context_id  - Context to attach FG to
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int field_input_type_const_main(int unit ,bcm_field_context_t context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t qual_info;
    void *dest_char;
    int rv = BCM_E_NONE;
    int ii = 0;



    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 5;
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &input_type_const_qual_lrn_data_dst_port);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_create, input_type_const_qual_lrn_data_dst_port\n");
        return rv;
    }

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 4;
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &input_type_const_qual_const);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_create, action_prefix_qual_trap\n");
        return rv;
    }


    
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.nof_quals = 4;
    fg_info.stage = bcmFieldStageIngressPMF2;


    fg_info.qual_types[0] = bcmFieldQualifyIntPriority;
    fg_info.qual_types[1] = bcmFieldQualifyGeneratedTtl;
    fg_info.qual_types[2] = input_type_const_qual_lrn_data_dst_port;
    fg_info.qual_types[3] = input_type_const_qual_const;
    

    fg_info.nof_actions = 1;

    /** Order of actions should be the same as order of qualifiers*/
    fg_info.action_types[0] = bcmFieldActionStatId0;
    fg_info.action_with_valid_bit[0] = FALSE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "basic_const_input_type", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, 0, &fg_info, &input_type_const_fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_add\n");
        return rv;
    }


    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for(ii=0; ii< fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for(ii=0; ii<fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[2].input_arg = 0;
    attach_info.key_info.qual_info[2].offset = (2556 + 9);
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[3].input_arg = 0xA;
    attach_info.key_info.qual_info[3].offset = 0;


    rv = bcm_field_group_context_attach(unit, 0, input_type_const_fg_id, context_id, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_attach\n");
        return rv;
    }

    printf("Input type const FG_ID(%d) was attached fot context (%d) \n",input_type_const_fg_id,context_id);
    
    return 0;
}


/**
* \brief
*  Delete the field group and created actions
* \param [in] unit        - Device ID
* \param [in] context_id  - Context to detach the info from
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int field_input_type_const_destroy(int unit, bcm_field_context_t context_id)
{
    int rv = BCM_E_NONE;
    int ii= 0;

    rv = bcm_field_group_context_detach(unit,input_type_const_fg_id,context_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }   
    printf("Input type const detach FG_ID(%d) from context (%d) \n",input_type_const_fg_id,context_id);
  
    rv = bcm_field_group_delete(unit, input_type_const_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete\n", rv);
        return rv;
    }

    rv = bcm_field_qualifier_destroy(unit, input_type_const_qual_lrn_data_dst_port);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_destroy input_type_const_qual_lrn_data_dst_port\n");
        return rv;
    }
    
    rv = bcm_field_qualifier_destroy(unit, input_type_const_qual_const);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_destroy input_type_const_qual_const\n");
        return rv;
    }
    
    return rv;
}
