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
 * cint ../../../src/examples/dnx/field/cint_field_vid_qual.c
 * cint;
 * int unit = 0;
 * int vid_value = 100;
 * bcm_field_action_t action_type = bcmFieldActionPrioIntNew;
 * int action_value = 1;
 * cint_field_vid_qual_main(unit, vid_value, action_type, action_value);
 *
 * Configuration example end
 *
 *   This cint shows how to qualify upon VID, since VLAN is not separate layer the offset needs to be set at attach API
 *   Case: show how the
 */


bcm_field_group_t cint_field_vid_qual_fg_id=0;
bcm_field_entry_t cint_field_vid_qual_entry_id = 0;
bcm_field_context_t cint_field_vid_qual_context_id;
bcm_field_presel_t cint_field_vid_qual_presel_id = 8;




/**
* \brief
*  Configures the written above
* \param [in] unit        -  Device id
* \param [in] vid_value        -  VID value to qualify
* \param [in] action_type        -  Action Type to perform
* \param [in] action_value        -  action value to perform
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_vid_qual_main(int unit,int vid_value, bcm_field_action_t action_type, int action_value)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_info_t context_info;
    void *dest_char;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_presel_entry_id_t presel_entry_id;
    int rv;

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "VID_QUAL", sizeof(context_info.name));
    rv =  bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &cint_field_vid_qual_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create \n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    bcm_field_presel_entry_data_info_init(&presel_entry_data);

    presel_entry_id.presel_id = cint_field_vid_qual_presel_id;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    presel_entry_data.context_id = cint_field_vid_qual_context_id;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.nof_qualifiers = 1;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyVlanFormat;
    presel_entry_data.qual_data[0].qual_value = BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
    /**This qualify has no meaning of mask*/
    presel_entry_data.qual_data[0].qual_mask = 0;
    presel_entry_data.qual_data[0].qual_arg = 0;

    rv =  bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_presel_set \n", rv);
        return rv;
    }


    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyVlanId;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = action_type;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "Eth_VID", sizeof(fg_info.name));
    rv =  bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_vid_qual_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add \n", rv);
        return rv;
    }

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 96;

    rv =  bcm_field_group_context_attach(unit, 0, cint_field_vid_qual_fg_id, cint_field_vid_qual_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach \n", rv);
        return rv;
    }

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = vid_value;
    ent_info.entry_qual[0].mask[0] = 0xFFF;
    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = action_value;

    rv =  bcm_field_entry_add(unit, 0, cint_field_vid_qual_fg_id, &ent_info, &cint_field_vid_qual_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add \n", rv);
        return rv;
    }

    return 0;
}

int cint_field_vid_qual_destroy(int unit)
{
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_entry_qual_t entry_qual_info[BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP];
    int rv = BCM_E_NONE;

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_vid_qual_presel_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = FALSE;
    p_data.context_id = 0;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }


    rv = bcm_field_group_context_detach(unit, cint_field_vid_qual_fg_id, cint_field_vid_qual_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach ipv4\n", rv);
        return rv;
    }


    rv = bcm_field_context_destroy(unit,bcmFieldStageIngressPMF1,cint_field_vid_qual_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy eth\n", rv);
        return rv;
    }

    rv = bcm_field_entry_delete(unit,cint_field_vid_qual_fg_id,entry_qual_info,cint_field_vid_qual_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete \n", rv);
        return rv;
    }

    rv = bcm_field_group_delete(unit,cint_field_vid_qual_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete ipv4\n", rv);
        return rv;
    }

    return 0;
}
