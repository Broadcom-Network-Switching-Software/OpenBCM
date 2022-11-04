 /*
 * Configuration example start:
 *  
 * Test Scenario - start
 * cint;                                                                  
 * cint_reset();                                                          
 * exit; 
  cint ../../../../src/examples/dnx/field/cint_field_always_hit_context.c
  cint
  int unit = 0;
  bcm_field_presel_t presel_id = 8;
  bcm_field_stage_t stage = bcmFieldStageIngressPMF1;
  bcm_field_context_t context_id = -1;
  cint_field_always_hit_context_main(unit, presel_id, stage, &context_id);
  cint_field_always_hit_context_without_sys_hdr_mod_main(unit, presel_id, stage, &context_id);
 * Test Scenario - end
 *
 * Configuration example end
 *
 * Configure for PMF stage to always hit a context on given presel, In general can replace default context
 *
 */



char * cint_field_stage_name_get(bcm_field_stage_t stage)
{
    switch (stage)
    case bcmFieldStageEgress:
    {
        return  "Egress";
        break;
    }
    case bcmFieldStageIngressPMF1:
    {
        return "IngressPMF1";
        break;
    }
    case bcmFieldStageIngressPMF2:
    {
        return "IngressPMF2";
        break;
    }
    case bcmFieldStageIngressPMF3:
    {
        return "IngressPMF3";
        break;
    }
    default:
    {
        return "Invalid";
        break;
    }

    return "Invalid";
}

bcm_field_group_t fg_id_FLOW_id = -1;
bcm_field_action_t flow_action = -1;
int add_flow = TRUE;

int cint_field_flow_id_ext_add(int unit, bcm_field_context_t context_id)
{

    bcm_field_group_info_t fg_info;
    bcm_field_group_t fg_id = 0;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_info_t action_info_predefined;
    void *dest_char;


    BCM_IF_ERROR_RETURN_MSG(bcm_field_action_info_get
                   (unit, bcmFieldActionLatencyFlowId, bcmFieldStageIngressPMF1, &action_info_predefined), "");

    /** Create destination action to ignore destination qualifier which is 0*/
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionLatencyFlowId;
    action_info.prefix_size = action_info_predefined.size;
    /**Valid bit is the lsb*/
    action_info.prefix_value = 1;
    action_info.size = 0;
    action_info.stage = bcmFieldStageIngressPMF1;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "cint_const_flow", sizeof(action_info.name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_action_create(unit, 0, &action_info, &flow_action), "");

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeConst;
    fg_info.stage = bcmFieldStageIngressPMF1;

    fg_info.nof_actions = 1;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[0] = flow_action;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "cint_Flow_ext", sizeof(fg_info.name));

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &fg_info, &fg_id_FLOW_id), "");

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.payload_info.nof_actions = 1;
    attach_info.payload_info.action_types[0] = flow_action;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, fg_id_FLOW_id, context_id, &attach_info), "");

    printf("Flow FG ID (%d) was attached for context(%d)\n",fg_id_FLOW_id,context_id);

    return BCM_E_NONE;
}

/**
* \brief
*  Create a context and configure presel to point to that context based on FWD layer type
* \param [in] unit        - Device UD
* \param [in] presel_id   - Presel to configure
* \param [out] context_id  - Context id created
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_always_hit_context_main(int unit, bcm_field_presel_t presel_id, bcm_field_stage_t stage,
                                                                bcm_field_context_t  *context_id)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;    
    bcm_field_presel_entry_id_t p_id;
    void * dest_char;
    bcm_field_context_param_info_t context_param;
    int flow_id_ext_add = FALSE;


    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "always_hit", sizeof(context_info.name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_context_create(unit, 0, stage, &context_info, context_id), "");

    printf("add_flow_fg is set to (%d) and the flow_FG_ID is (%d) \n", add_flow,fg_id_FLOW_id);
    /**Assumption the packet are Ethernet packet hence we set relevant system header profile and remove PTCH*/
    if(stage == bcmFieldStageIngressPMF1)
    {
        bcm_field_context_param_info_t_init(&context_param);
        context_param.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
        context_param.param_val = bcmFieldSystemHeaderProfileFtmhTshPphUdh;
        BCM_IF_ERROR_RETURN_MSG(bcm_field_context_param_set(unit,0,bcmFieldStageIngressPMF1,*context_id,&context_param), "");

        /*
         * For ITPP termination, parsing_start_offset is used
         */
        if (!*(dnxc_data_get(unit, "headers", "system_headers", "itmh_itpp_termination", NULL)))
        {
            bcm_field_context_param_info_t_init(&context_param);
            context_param.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
            context_param.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerOffset1, 0);
            BCM_IF_ERROR_RETURN_MSG(bcm_field_context_param_set(unit,0,bcmFieldStageIngressPMF2,*context_id,&context_param), "");
        }

        flow_id_ext_add = *(dnxc_data_get(unit, "field", "init", "flow_id", NULL));

        if(flow_id_ext_add && add_flow)
        {
            if(fg_id_FLOW_id != -1)
            {
                printf("More than one (always hit) context is set on iPMF1 stage, please destroy the other before setting new one\n");
            }
            BCM_IF_ERROR_RETURN_MSG(cint_field_flow_id_ext_add(unit,*context_id), "");
        }
    }

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = presel_id;
    p_id.stage = stage;
    p_data.entry_valid = TRUE;
    p_data.context_id = *context_id;
    p_data.nof_qualifiers = 0;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(unit, 0, &p_id, &p_data), "");


    printf("Presel (%d) was configured for stage(%s) context(%d) - This context will always hit! \n",
        presel_id, cint_field_stage_name_get(stage), *context_id);

    return BCM_E_NONE;
}

/**
* \brief
*  Create a context and configure presel to point to that context based on FWD layer type.
*  Without configuring system header mode.
* \param [in] unit        - Device UD
* \param [in] presel_id   - Presel to configure
* \param [in] stage   - Stage on which context will be created.
* \param [out] context_id  - Context id created
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_always_hit_context_without_sys_hdr_mod_main(
    int unit,
    bcm_field_presel_t presel_id,
    bcm_field_stage_t stage,
    bcm_field_context_t  *context_id)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    void * dest_char;


    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "always_hit", sizeof(context_info.name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_context_create(unit, 0, stage, &context_info, context_id), "");

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = presel_id;
    p_id.stage = stage;
    p_data.entry_valid = TRUE;
    p_data.context_id = *context_id;
    p_data.nof_qualifiers = 0;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(unit, 0, &p_id, &p_data), "");


    printf("Presel (%d) was configured for stage(%s) context(%d) - This context will always hit! \n",
        presel_id, cint_field_stage_name_get(stage), *context_id);

    return BCM_E_NONE;
}


/**
* \brief
*  Destroy the context and disable preselector
* \param [in] unit        - Device ID
* \param [in] context_id  - Context to detach the info from
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_always_hit_contex_destroy(int unit, int presel_id,bcm_field_stage_t stage, bcm_field_context_t context_id)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t p_id;
    

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    p_id.stage = stage;
    p_id.presel_id = presel_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(unit, 0, &p_id, &presel_entry_data), "");


    if(stage == bcmFieldStageIngressPMF1)
    {
        printf("Flow FG ID (%d)\n",fg_id_FLOW_id);
        if(fg_id_FLOW_id != -1)
        {
            BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit,fg_id_FLOW_id,context_id), "");

            BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit,fg_id_FLOW_id), "");
            fg_id_FLOW_id = -1;
            BCM_IF_ERROR_RETURN_MSG(bcm_field_action_destroy(unit, flow_action), "");
            flow_action = -1;
        }
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_field_context_destroy(unit,stage,context_id), "");
    
    
    
    return BCM_E_NONE;
}
