 /*
 * Configuration:
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/field/cint_field_utils.c
  cint ../../../../src/examples/dnx/field/cint_field_tcam_1q_1a.c
  cint
  bcm_field_context_t context_id = 0;
  cint_tcam_1q_1a_main(0,bcmFieldStageIngressPMF1,context_id);
  bcm_gport_t trap_gport;
  int trap_id = 257;
  BCM_PORT_TRAP_SET( trap_gport , trap_id, 7,0);
  bcm_field_entry_t entry_id;
  cint_tcam_1q_1a_entry_add(0,10,trap_gport,&entry_id);
 * Test Scenario - end
 *
 *   Simple exmapl of TCAM with 1 qualifier and 1 action,
 *   cint_tcam_1q_1a_main() configures the Field group, the context should be created outside of the cint
 *   cint_tcam_1q_1a_entry_add() adds the entry to the field group, note that the mask is -1 i.e. that all bits are meaningful when the looking is preformed
 *    
 */


bcm_field_group_t tcam_1q_1a_fg_id=0;
bcm_field_stage_t tcam_1q_1a_stage = bcmFieldStageIngressPMF1;

bcm_field_qualify_t tcam_1q_1a_qual = bcmFieldQualifyVrf;
bcm_field_input_types_t tcam_1q_1a_qual_input_t = bcmFieldInputTypeMetaData;
int tcam_1q_1a_qual_input_arg = BCM_FIELD_INVALID;
int tcam_1q_1a_qual_input_off = 0;

bcm_field_action_t tcam_1q_1a_action = bcmFieldActionTrap;
bcm_field_action_priority_t tcam_1q_1a_action_prio= BCM_FIELD_ACTION_DONT_CARE;

int cint_tcam_1q_1a_entry_add(int unit, uint32 qual_val, uint32 act_val, bcm_field_entry_t *entry_id)
{
    bcm_field_entry_info_t ent_info;

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;


    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = tcam_1q_1a_qual;
    ent_info.entry_qual[0].value[0] = qual_val;
    ent_info.entry_qual[0].mask[0] = -1;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = tcam_1q_1a_action;
    ent_info.entry_action[0].value[0] = act_val;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, tcam_1q_1a_fg_id, &ent_info, entry_id), "");

    printf("Entry add: id:(0x%x)  qual_val:(0x%x) action_val:(%d)\n",*entry_id,qual_val, act_val);
    return BCM_E_NONE;
}
/**
* \brief
*  Configures the written above
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_tcam_1q_1a_main(int unit, bcm_field_stage_t stage,bcm_field_context_t context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;

    bcm_field_context_info_t context_info;
    void *dest_char;

    tcam_1q_1a_stage = stage;
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = tcam_1q_1a_stage;

    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = tcam_1q_1a_qual;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = tcam_1q_1a_action;

    dest_char = &(fg_info.name[0]);

    sal_strncpy_s(dest_char, "TCAM_1Q_1A", sizeof(fg_info.name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &fg_info, &tcam_1q_1a_fg_id), "");

    printf("Created TCAM field group (%d) in %s \n", tcam_1q_1a_fg_id, cint_field_utils_stage_name_get(tcam_1q_1a_stage));


    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0]; 
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    attach_info.key_info.qual_info[0].input_type = tcam_1q_1a_qual_input_t;
    attach_info.key_info.qual_info[0].input_arg = tcam_1q_1a_qual_input_arg;
    attach_info.key_info.qual_info[0].offset = tcam_1q_1a_qual_input_off;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, tcam_1q_1a_fg_id, context_id, &attach_info), "");
    
    printf("Attached  FG (%d) to context (%d)\n",tcam_1q_1a_fg_id,context_id);

    return BCM_E_NONE;
}

int field_attach_destroy(int unit,bcm_field_context_t context_id, bcm_field_entry_t entry_id)
{
    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, tcam_1q_1a_fg_id, NULL, entry_id), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, tcam_1q_1a_fg_id, context_id), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit,tcam_1q_1a_fg_id), "");

    return BCM_E_NONE;
}
