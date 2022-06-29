 /*
 * Configuration example start:
 *
  cint ../../../src/examples/dnx/field/cint_field_group_const.c
  cint ../../../../src/examples/dnx/field/cint_field_group_const.c
  cint;
  int unit = 0;
  bcm_field_context_t context_id = 0;
  cint_field_group_const_example_ipt_gen(unit, context_id);
  cint_field_group_const_example_UDH_gen(unit, context_id);
 *
 * Configuration example end
 *
 * example of setting a constant FG where the key is empty (size=o) and a constant value
 * action is performed on all packets selected in the contexts
 *
 * for 'how to use' look at functions:
 * cint_field_group_const_example_ipt_gen(0,BCM_FIELD_CONTEXT_ID_DEFAULT);
 * cint_field_group_const_example_UDH_gen(0,BCM_FIELD_CONTEXT_ID_DEFAULT);
 */

bcm_field_group_t   cint_const_fg_id = 0;
bcm_field_action_t  cint_const_action;
int cint_const_action_priority = BCM_FIELD_ACTION_DONT_CARE;

/**
* \brief
*  Configures the written above
* \param [in] unit        -  Device id
* \param [in] stage -  PMF Stage
* \param [in] context_id  -  PMF context Id
* \param [in] action_type  -  PMF action_type
* \param [in] action_value  -  PMF action_value
* \param [in] action_value  -  Name of The Field group and action
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_group_const_main(int unit, bcm_field_stage_t stage , bcm_field_context_t context_id,
         bcm_field_action_t action_type, int action_value,char * name)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_info_t action_info_get_size;
    void *dest_char;
    int id,id2;



    BCM_IF_ERROR_RETURN(bcm_field_action_info_get(unit,action_type,stage,&action_info_get_size));

    /*
     * Create a user define action with size 0, value is fixed
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = action_type;
    action_info.prefix_size = action_info_get_size.size;
    action_info.prefix_value = action_value;
    action_info.size = 0;
    action_info.stage = stage;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, name, sizeof(action_info.name));
    BCM_IF_ERROR_RETURN(bcm_field_action_create(unit, 0, &action_info, &cint_const_action));

    id = cint_const_action;
    printf("Action (%s) created ID: %d \n", name,id);

    /*
     * Create and attach Const Field Group
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeConst;
    fg_info.stage = stage;

    /* Constant FG : number of qualifiers in key must be 0 (empty key) */
    fg_info.nof_quals = 0;

    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = cint_const_action;
    fg_info.action_with_valid_bit[0] = FALSE;


    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, name, sizeof(fg_info.name));
    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, &fg_info, &cint_const_fg_id));
    id = cint_const_fg_id;
    printf("Const FG (%s) created ID: %d \n", name,id);


    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    /*In case 2 same actions attached on same context, need to indicate priorty who should win*/
    attach_info.payload_info.action_info[0].priority = cint_const_action_priority;

    BCM_IF_ERROR_RETURN(bcm_field_group_context_attach(unit, 0, cint_const_fg_id, context_id, &attach_info));

    id2 = context_id;
    printf("FG (%d) Attached to context: %d \n",id,id2);

    return BCM_E_NONE;
}

/**
* \brief
*  Destroy all configuration done
* \param [in] unit        -  Device id
* \param [in] context_id  -  PMF context Id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_group_const_destroy(int unit, bcm_field_context_t context_id)
{
    BCM_IF_ERROR_RETURN(bcm_field_group_context_detach(unit, cint_const_fg_id, context_id));

    /* Delete constant FG */
    BCM_IF_ERROR_RETURN(bcm_field_group_delete(unit, cint_const_fg_id));

    /* Delete user define action */
    BCM_IF_ERROR_RETURN(bcm_field_action_destroy(unit, cint_const_action));

    return BCM_E_NONE;
}


/**
* \brief
*  Example to always generate IPT action in iPMF3
*  Attached to default context i.e. always hit (in case no other CS occur)
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_group_const_example_ipt_gen(int unit, bcm_field_context_t context_id)
{

    bcm_field_stage_t stage = bcmFieldStageIngressPMF3;

    /**To generate IPT 2 actions needed*/
    BCM_IF_ERROR_RETURN(cint_field_group_const_main(unit, stage,context_id,bcmFieldActionIPTProfile, 2 , "IPT_prof_2"));

    BCM_IF_ERROR_RETURN(cint_field_group_const_main(unit, stage,context_id,bcmFieldActionIPTCommand, 1 , "IPT_com_1"));


    return BCM_E_NONE;
}

/**
* \brief
*  Exmaple to always generate UDH action in iPMF1
*  Attached to default context i.e. alwyas hit (in case no other CS occur)
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_group_const_example_UDH_gen(int unit, bcm_field_context_t context_id)
{

    BCM_IF_ERROR_RETURN(cint_field_group_const_main(unit, bcmFieldStageIngressPMF1,context_id,bcmFieldActionUDHData2,0xABCDABCD , "UDH2_const"));

    return BCM_E_NONE;
}
