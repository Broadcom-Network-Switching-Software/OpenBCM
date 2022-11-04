 /*
 * Configuration example start:
 *  

 * Test Scenario - start
  cint ../../../../src/examples/dnx/field/cint_field_action_drop.c
  cint;
  int unit = 0;
  bcm_field_context_t context_id = 0;
  uint32 src_ip_val = 10;
  cint_field_action_drop_in_tcam_fg_main(0,0,10);
 * Test Scenario - end
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

    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionForward;
    action_info.prefix_size = 31;
    /**11 bit of qualifier are 0 and to drop all dest bit are 1's. With lsb 1 from entry payload we get 0x001FFFFF*/
    action_info.prefix_value = 0x000BFFFF;
    action_info.size = 1;
    action_info.stage = stage;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "action_drop_1b", sizeof(action_info.name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_action_create(unit, 0, &action_info, action_drop_id), "");

    action_id = *action_drop_id;
    printf("Action drop was created with id:(%d) \n",action_id);
    return BCM_E_NONE;
    
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
    BCM_IF_ERROR_RETURN_MSG(bcm_field_action_destroy(unit, action_drop_id), "");

    return BCM_E_NONE;
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

    BCM_IF_ERROR_RETURN_MSG(cint_field_action_drop_main(unit,bcmFieldStageIngressPMF1, &action_drop), "");

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

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &fg_info, &actio_drop_fg_id), "");

    printf("Created TCAM field group (%d) in iPMF1 \n", actio_drop_fg_id);

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0]; 
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, actio_drop_fg_id, context_id, &attach_info), "");

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

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, actio_drop_fg_id, &ent_info, &action_drop_entry_id), "");

    printf("Entry add: id:(0x%x) Src_ip:(0x%x)\n", action_drop_entry_id, src_ip_val);
    
    return BCM_E_NONE;
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

    bcm_field_entry_info_t_init(&ent_info);
    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, actio_drop_fg_id, ent_info.entry_qual, action_drop_entry_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit,actio_drop_fg_id,context_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, actio_drop_fg_id), "");

    BCM_IF_ERROR_RETURN_MSG(cint_field_action_drop_destroy(unit, action_drop), "");
    
    return BCM_E_NONE;
}

