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
 * cint ../../../src/examples/dnx/field/cint_field_action_redirect.c
 * cint;
 * int unit = 0;
 * bcm_field_context_t context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;
 * uint32 src_ip_val = 10;
 * cint_field_action_redirect_in_tcam_fg_main(unit, context_id, src_ip_val);
 *
 * Configuration example end
 *
 *   Example of using bcmFieldActionRedirect action with value BCM_GPORT_BLACK_HOLE.
 *   Function cint_field_action_redirect_in_tcam_fg_main() show how the action should be used in TCAM field group
 */

bcm_field_group_t Cint_field_action_redirect_fg_id;
bcm_field_entry_t Cint_field_action_redirect_entry_id;


/**
* \brief
*  Create a FG type TCAM with Action Redirect to BCM_GPORT_BLACK_HOLE
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
int cint_field_action_redirect_in_tcam_fg_main(
    int unit,
    bcm_field_context_t context_id,
    uint32 src_ip_val)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    void *dest_char;
    int rv = BCM_E_NONE;

    /** Create and attach first group. */
    bcm_field_group_info_t_init(&fg_info);
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "action_redirect", sizeof(fg_info.name));
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifySrcIp;
    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionRedirect;

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &Cint_field_action_redirect_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in iPMF1 \n", Cint_field_action_redirect_fg_id);

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0]; 
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, Cint_field_action_redirect_fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    printf("Attached  FG (%d) to context (%d)\n", Cint_field_action_redirect_fg_id, context_id);

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = src_ip_val;
    ent_info.entry_qual[0].mask[0] = 0xffffffff;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = BCM_GPORT_BLACK_HOLE;

    rv = bcm_field_entry_add(unit, 0, Cint_field_action_redirect_fg_id, &ent_info, &Cint_field_action_redirect_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    printf("Entry add: id:(0x%x) Src_ip:(0x%x)\n", Cint_field_action_redirect_entry_id, src_ip_val);
    
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
int cint_field_action_redirect_in_tcam_fg_destroy(
    int unit,
    bcm_field_context_t context_id)
{
    bcm_field_entry_info_t ent_info;
    int rv = BCM_E_NONE;

    bcm_field_entry_info_t_init(&ent_info);
    rv = bcm_field_entry_delete(unit, Cint_field_action_redirect_fg_id, NULL, Cint_field_action_redirect_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit,Cint_field_action_redirect_fg_id, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach\n", rv);
        return rv;
    }

    rv = bcm_field_group_delete(unit, Cint_field_action_redirect_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete\n", rv);
        return rv;
    }
    
    return 0;
}

