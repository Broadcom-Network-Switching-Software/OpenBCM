/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Configuration example start:
 *
 * cint
 * cint_reset();
 * exit;
 * cint ../../../src/examples/dnx/field/cint_field_mpls_x_presel.c
 * cint ../../../src/examples/dnx/field/cint_field_action_drop.c
 * cint;
 * int unit = 0;
 * int in_port = 200;
 * cint_field_mpls_x_presel__start_run_with_params(unit, in_port);
 *
 * Configuration example end
 *
 * Configure Field Processor Program Selection Preselector for header format, MPLS any label any L2/L3
 */

struct mpls_x_info_s{
    bcm_field_presel_t presel_id;
    int in_port;
};


mpls_x_info_s mpls_x_info_g ={
          22, /*Presel ID*/
          200 /*In port*/
};

bcm_field_action_t action_drop= 0;
bcm_field_group_t  cint_field_mpls_x_presel_fg_id = 0;
bcm_field_context_t cint_field_mpls_x_presel_context_id = 0;
bcm_field_entry_t ent1_id;

/*****************************************************************************
* Function:  field_mpls_x_presel_init
* Purpose:   
* Params:
* unit - 
* Return:    (int)
 */
int cint_field_mpls_x_presel_init(int unit)
{

    return BCM_E_NONE;
}    

/*****************************************************************************
* Function:  field_mpls_x_presel_struct_get
* Purpose:   
* Params:
* unit  - 
* param - 
 */
void cint_field_mpls_x_presel_struct_get(int unit, mpls_x_info_s *param)
{
     sal_memcpy(param,&mpls_x_info_g, sizeof(mpls_x_info_g));
     return;
}

/*****************************************************************************
* Function:  field_mpls_x_presel__start_run_with_port
* Purpose:   
* Params:
* unit      - 
* in_port - 
* Return:    (int)
 */

int cint_field_mpls_x_presel__start_run_with_params
                (int unit, int in_port)
{
    mpls_x_info_s mpls_x_info;
    cint_field_mpls_x_presel_struct_get(unit,&mpls_x_info);
    mpls_x_info.in_port = in_port; 
    return cint_field_mpls_x_presel__start_run(unit,&mpls_x_info);
}

/*****************************************************************************
* Function:  field_mpls_x_presel__start_run
* Purpose:   
* Params:
* unit      - 
* mpls_x_info_p - 
* Return:    (int)
 */
 int  cint_field_mpls_x_presel__start_run(int unit,mpls_x_info_s *mpls_x_info_p)
 {
    int rv = BCM_E_NONE;

    mpls_x_info_s mpls_x_info;
    int group_priority;
    int presel;
    bcm_field_presel_t presel_id;
    bcm_gport_t in_gport;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_action_info_t action_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;

    rv = cint_field_mpls_x_presel_init(unit);
    if (BCM_E_NONE != rv) 
    { 
        return rv;
    }
    
    if(mpls_x_info_p == NULL)
    {
        cint_field_mpls_x_presel_struct_get(unit,&mpls_x_info);
    }
    else
    {
        sal_memcpy(&mpls_x_info,mpls_x_info_p, sizeof(mpls_x_info));
    }

    rv = cint_field_action_drop_main(unit,bcmFieldStageIngressPMF1, &action_drop);
	if(rv != BCM_E_NONE)
	{
		printf("Error in field_action_drop_main\n");
		return rv;
	}

    /*
     * Create a new context for field group
     */
    printf("Creating new context\n");
    bcm_field_context_info_t_init(&context_info);
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &cint_field_mpls_x_presel_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    printf("Context %d created!\n",cint_field_mpls_x_presel_context_id );
    
    /*Configure TCAM Field Group in iPMF1*/

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifySrcPort;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = action_drop;

    printf("Creating first group\n");
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_mpls_x_presel_fg_id );
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    /*
     * Attach the FG to context
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    printf("Attaching field group\n");
    rv = bcm_field_group_context_attach(unit, 0, cint_field_mpls_x_presel_fg_id , cint_field_mpls_x_presel_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    BCM_GPORT_SYSTEM_PORT_ID_SET(in_gport,mpls_x_info.in_port);
    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = in_gport;
    ent_info.entry_qual[0].mask[0] = 0xffff;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = 0;

    printf("Adding Entry\n");
    rv = bcm_field_entry_add(unit, 0, cint_field_mpls_x_presel_fg_id , &ent_info, &ent1_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    
    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = mpls_x_info.presel_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_mpls_x_presel_context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 1;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeMpls;
    p_data.qual_data[0].qual_mask = 0x1F;


    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(MPLS) \n",
    		mpls_x_info.presel_id,cint_field_mpls_x_presel_context_id);
    
    return rv;

 }

