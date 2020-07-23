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
 * cint ../../../src/examples/dnx/field/cint_field_action_drop.c
 * cint;
 * int unit = 0;
 * bcm_field_context_t context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;
 * uint32 src_ip_val = 10;
 * cint_field_action_drop_in_tcam_fg_main(0,0,10);
 *
 * Configuration example end
 *
 *   This cint creates an constant action drop (all 1s), still use one bit of action payload (since can't invalidate all)
 *   Function field_actio_drop_in_tcam_fg_main() show how the action should be used in TCAM field group
 */


/**
* \brief
*  create action drop which is all constant
* \param [in] unit        - Device ID
* \param [in] stage  - Stage for which action have to be created.
* \param [in] action_drop_id  - Created action ID
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_action_drop_main(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_action_t *action_drop_id)
{
    bcm_field_action_info_t action_info;
    uint32 action_id=0;
    void *dest_char;
    int rv = BCM_E_NONE;

    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionForward;
    action_info.prefix_size = 31;
    /**11 bit of qualifier are 0 and to drop all dest bit are 1's. With lsb 1 from entry payload we get 0x001FFFFF*/
    action_info.prefix_value = 0x000BFFFF;
    action_info.size = 1;
    action_info.stage = stage;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "action_drop_1b", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, action_drop_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_action_create action_prefix_stat_action\n");
        return rv;
    }
    action_id = *action_drop_id;
    printf("Action drop was created with id:(%d) \n",action_id);
    return 0;
    
}


/**
* \brief
*  Destroy the drop action
* \param [in] unit        - Device ID
* \param [in] action_drop_id  - Action drop id to destroy
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_action_drop_destroy(
    int unit,
    bcm_field_action_t action_drop_id)
{
    int rv = BCM_E_NONE;
    int ii= 0;

    rv = bcm_field_action_destroy(unit, action_drop_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_action_destroy action_prefix_stat_action\n");
        return rv;
    }
    
    return rv;
}


bcm_field_group_t actio_drop_fg_id=0;
bcm_field_action_t action_drop= 0;
bcm_field_entry_t action_drop_entry_id=0;

/**
* \brief
*  Create a FG typ TCAM with Action drop based on qualifiers values
* \param [in] unit        - Device Id
* \param [in] context_id  - Context to attach FG to
* \param [in] src_ip_val  - SrcIP value to qualify upon
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_action_drop_in_tcam_fg_main(
    int unit,
    bcm_field_context_t context_id,
    uint32 src_ip_val)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    int rv = BCM_E_NONE;

    rv = cint_field_action_drop_main(unit,bcmFieldStageIngressPMF1, &action_drop);
    if(rv != BCM_E_NONE)
    {
        printf("Error in field_action_drop_main\n");
        return rv;
    }
    /** Create and attach first group. */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifySrcIp;
    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = action_drop;

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &actio_drop_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in iPMF1 \n", actio_drop_fg_id);

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0]; 
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, actio_drop_fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    printf("Attached  FG (%d) to context (%d)\n",actio_drop_fg_id,context_id);

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = src_ip_val;
    ent_info.entry_qual[0].mask[0] = 0xffffffff;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    /** We need to write the LSB of the drop action.*/
    ent_info.entry_action[0].value[0] = 1;

    rv = bcm_field_entry_add(unit, 0, actio_drop_fg_id, &ent_info, &action_drop_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    printf("Entry add: id:(0x%x) Src_ip:(0x%x)\n", action_drop_entry_id, src_ip_val);
    
    return 0;
}

/**
* \brief
*  Destroy all configuration done
* \param [in] unit        -  Device id
* \param [in] context_id        -  Context id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_action_drop_in_tcam_fg_destroy(
    int unit,
    bcm_field_context_t context_id)
{
    bcm_field_entry_info_t ent_info;
    int rv = BCM_E_NONE;

    bcm_field_entry_info_t_init(&ent_info);
    rv = bcm_field_entry_delete(unit, actio_drop_fg_id, ent_info.entry_qual, action_drop_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit,actio_drop_fg_id,context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach\n", rv);
        return rv;
    }

    rv = bcm_field_group_delete(unit, actio_drop_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete\n", rv);
        return rv;
    }
    
    rv = cint_field_action_drop_destroy(unit, action_drop);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_action_drop_destroy\n", rv);
        return rv;
    }

    return 0;
}

