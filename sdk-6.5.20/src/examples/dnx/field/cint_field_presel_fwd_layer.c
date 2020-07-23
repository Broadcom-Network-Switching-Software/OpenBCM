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
 * cint ../../../src/examples/dnx/field/cint_field_presel_fwd_layer.c
 * cint;
 * int unit = 0;
 * bcm_field_context_t context_id;
 * bcm_field_presel_t presel_id = 50;
 * bcm_field_stage_t stage = bcmFieldStageIngressPMF1;
 * bcm_field_layer_type_t fwd_layer = bcmFieldLayerTypeEth;
 * field_presel_fwd_layer_main(unit, presel_id, stage, fwd_layer, &context_id, "ctx_example");
 *
 * Configuration example end
 *
 *   This cint creates a context and configures a presel to qualify on Fowrarding layer at input
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

/**
* \brief
*  Create a context and configure presel to point to that context based on FWD layer type
* \param [in] unit        - Device UD
* \param [in] presel_id   - Presel to configure
* \param [in] fwd_layer   - FWD layer type for presel to qualify upon
* \param [out] context_id  - Context id created
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int field_presel_fwd_layer_main(int unit, bcm_field_presel_t presel_id, bcm_field_stage_t stage ,bcm_field_layer_type_t fwd_layer,
                                        bcm_field_context_t  *context_id, char * context_name)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;    
    bcm_field_presel_entry_id_t p_id;
    int fwd_layer_internal;
    void * dest_char;
    bcm_field_context_param_info_t context_param;
    int rv = BCM_E_NONE;

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, context_name, sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, stage, &context_info, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    /**Assumption the packet are Ethernet packet hence we set relevant system header profile and remove PTCH*/
    if(stage == bcmFieldStageIngressPMF1)
    {
        bcm_field_context_param_info_t_init(&context_param);
        context_param.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
        context_param.param_val = bcmFieldSystemHeaderProfileFtmhTshPphUdh;
        rv = bcm_field_context_param_set(unit,0,bcmFieldStageIngressPMF1,*context_id,&context_param);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_context_param_set system header info\n", rv);
            return rv;
        }
        bcm_field_context_param_info_t_init(&context_param);
        context_param.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
        context_param.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerOffset1, 0);
        rv = bcm_field_context_param_set(unit,0,bcmFieldStageIngressPMF2,*context_id,&context_param);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_context_param_set system header info\n", rv);
            return rv;
        }
    }
    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = presel_id;
    p_id.stage = stage;
    p_data.entry_valid = TRUE;
    p_data.context_id = *context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = fwd_layer;
    p_data.qual_data[0].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set \n", rv);
        return rv;
    }

    fwd_layer_internal = fwd_layer;
    printf("Presel (%d) was configured for stage(%s) context(%d) fwd_layer(%d) \n",
        presel_id, cint_field_stage_name_get(stage), *context_id, fwd_layer_internal);

    return 0;
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
int field_presel_fwd_layer_destroy(int unit, int presel_id,bcm_field_stage_t stage, bcm_field_context_t context_id)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t p_id;
    int rv = BCM_E_NONE;
    
    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    p_id.stage = stage;
    p_id.presel_id = presel_id;
    rv = bcm_field_presel_set(unit, 0, &p_id, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_presel_set\n");
        return rv;
    }

    rv = bcm_field_context_destroy(unit,stage,context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy ipmf1 \n", rv);
        return rv;
    }
    
    
    
    return rv;
}
