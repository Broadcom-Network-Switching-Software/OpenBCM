/*
 * Configuration example start:
 *
 * Test Scenario - start
 * cint;
 * cint_reset();
 * exit;
  cint ../../../../src/examples/dnx/field/cint_field_group_const.c
  cint ../../../../src/examples/dnx/field/cint_field_ipmf2_ipmf3_cascading.c
  cint;
  int unit = 0;
  cint_field_cascaded_main(unit,0,0,0xA0000001,2);
 * Test Scenario - end
 *
 * Configuration example end
 *
 * Since iPMF3 cannot access header information we perform lookup in iPMF2 and cascade the result to iPMF3
 * iPMF3 based on recieved information decides the priority of the packet
 */

/* *INDENT-OFF* */

int NOF_FIELD_GROUPS            = 2;
bcm_field_group_t IPMF2_FG_ID   = -1;
bcm_field_group_t IPMF3_FG_ID   = -1;
bcm_field_entry_t IPMF2_ENTRY_HANDLE       = 0;
bcm_field_entry_t IPMF3_ENTRY_HANDLE       = 0;
int CASCADING_VALUE = 0x12345678;


/* *INDENT-ON* */


/** Main function to configure IPMF2-IPMF3 cascading. */
int
cint_field_cascaded_main(
    int unit,
    bcm_field_context_t ipmf2_context,
    bcm_field_context_t ipmf3_context,
    int expected_src_ipv4,
    int expected_tc)
{
    bcm_field_group_attach_info_t attach_info;
    bcm_field_group_info_t ipmf2_fg_info;
    bcm_field_group_info_t ipmf3_fg_info;
    bcm_field_entry_info_t ent_info;
    void *dest_char;

    /*
     * iPMF2 Configuration
     */
    /*
     * iPMF1/2 use container signal to iPMF3, cant assume that its set to 0 before iPMF1/2 stages
     * Some PP application might use the container, thus we want to assure that Container =0, in case iPMF2 TCAM FG will not hit
     * Action should be lower priority then TCAM FG action
     * */
    cint_const_action_priority = BCM_FIELD_ACTION_PRIORITY(0,0);
    BCM_IF_ERROR_RETURN_MSG(cint_field_group_const_main(unit, bcmFieldStageIngressPMF2,ipmf2_context,bcmFieldActionContainer,0 , "container_exmpl"), "");

    bcm_field_group_info_t_init(&ipmf2_fg_info);
    ipmf2_fg_info.fg_type = bcmFieldGroupTypeTcam;
    ipmf2_fg_info.stage = bcmFieldStageIngressPMF2;

    /* Set quals */
    ipmf2_fg_info.nof_quals = 1;
    ipmf2_fg_info.qual_types[0] = bcmFieldQualifySrcIp;
    /* Set actions */
    ipmf2_fg_info.nof_actions = 1;
    ipmf2_fg_info.action_types[0] = bcmFieldActionContainer;

    dest_char = &(ipmf2_fg_info.name[0]);
    sal_strncpy_s(dest_char, "Cascading_ipmf2", sizeof(ipmf2_fg_info.name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &ipmf2_fg_info, &IPMF2_FG_ID), "");
    printf("Created TCAM field group (%d) in iPMF2 \n", IPMF2_FG_ID);

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = ipmf2_fg_info.nof_quals;
    attach_info.payload_info.nof_actions = ipmf2_fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = ipmf2_fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = ipmf2_fg_info.action_types[0];
    /* Should be higher priority then the action of const Field Group*/
    attach_info.payload_info.action_info[0].priority = BCM_FIELD_ACTION_PRIORITY(0,2);

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, IPMF2_FG_ID, ipmf2_context, &attach_info), "");

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = ipmf2_fg_info.qual_types[0];
    /**IP Address 10.0.0.1*/
    ent_info.entry_qual[0].value[0] = expected_src_ipv4;
    ent_info.entry_qual[0].mask[0] = -1;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = ipmf2_fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = CASCADING_VALUE;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, IPMF2_FG_ID, &ent_info, IPMF2_ENTRY_HANDLE), "");


    /*
     * iPMF3 Configuration
     */

    bcm_field_group_info_t_init(&ipmf3_fg_info);
    ipmf3_fg_info.fg_type = bcmFieldGroupTypeTcam;
    ipmf3_fg_info.stage = bcmFieldStageIngressPMF3;

    /* Set quals */
    ipmf3_fg_info.nof_quals = 1;
    ipmf3_fg_info.qual_types[0] = bcmFieldQualifyContainer;
    /* Set actions */
    ipmf3_fg_info.nof_actions = 1;
    ipmf3_fg_info.action_types[0] = bcmFieldActionPrioIntNew;

    dest_char = &(ipmf3_fg_info.name[0]);
    sal_strncpy_s(dest_char, "Cascading_ipmf3", sizeof(ipmf3_fg_info.name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &ipmf3_fg_info, &IPMF3_FG_ID), "");
    printf("Created TCAM field group (%d) in iPMF3 \n", IPMF3_FG_ID);

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = ipmf3_fg_info.nof_quals;
    attach_info.payload_info.nof_actions = ipmf3_fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = ipmf3_fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = ipmf3_fg_info.action_types[0];


    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, IPMF3_FG_ID, ipmf3_context, &attach_info), "");


    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = ipmf3_fg_info.qual_types[0];
    /* Set the value that expected from iPMF2*/
    ent_info.entry_qual[0].value[0] = CASCADING_VALUE;
    ent_info.entry_qual[0].mask[0] = -1;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = ipmf3_fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = expected_tc;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, IPMF3_FG_ID, &ent_info, IPMF3_ENTRY_HANDLE), "");


    return BCM_E_NONE;
}

/** Destroys all allocated data for the IPMF2-IPMF3 cascading. */
int
cint_field_cascaded_destroy(
    int unit,
    bcm_field_context_t ipmf2_context,
    bcm_field_context_t ipmf3_context)
{
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, IPMF2_FG_ID, ipmf2_context), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, IPMF3_FG_ID, ipmf3_context), "");

    /*Deleting Field group should delete entries as well*/
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit,IPMF2_FG_ID), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit,IPMF3_FG_ID), "");

    BCM_IF_ERROR_RETURN_MSG(cint_field_group_const_destroy(unit,ipmf2_context), "");

    return BCM_E_NONE;

}
