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
 * cint ../../../src/examples/dnx/field/cint_field_stat_sampling.c
 * cint;
 * int unit = 0;
 * bcm_field_context_t context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;
 * int stat_sampling_code = 0x12;
 * int stat_sampling_qualifier = 0xab;
 * cint_field_stat_sampling_main(unit, context_id, stat_sampling_code, stat_sampling_qualifier);
 *
 * Configuration example end
 *
 * Example of using inPort qualifier which is encoded as GPORT.
 */

bcm_field_group_t   cint_stat_sampling_fg_id = 0;
bcm_gport_t stat_sampling_gport;
bcm_field_entry_t cint_stat_sampling_ent_id;

bcm_field_group_t   cint_stat_sampling_fg_id_qual = 0;
bcm_field_entry_t cint_stat_sampling_ent_id_qual;


/**
* \brief
*  Configures FG in IPMF1 for action testing.
* \param [in] unit        -  Device id
* \param [in] context_id  -  PMF context Id
* \param [in] stat_sampling_code       -  stat_sampling command (8 bits)
* \param [in] stat_sampling_qualifier  -  stat_sampling qualifier (9 bits)
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_stat_sampling_main(int unit, bcm_field_context_t context_id, int stat_sampling_code, int stat_sampling_qualifier)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_action_info_t action_info;
    int rv = BCM_E_NONE;


    /*
     * Create and attach TCAM group in iPMF1
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyTpid;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionStatSampling;

    printf("Creating IPMF1 FG\n");
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_stat_sampling_fg_id);
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
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 96;

    printf("Attaching IPMF1 FG\n");
    rv = bcm_field_group_context_attach(unit, 0, cint_stat_sampling_fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    BCM_GPORT_MIRROR_SET(stat_sampling_gport, stat_sampling_code);
    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = 0x8100;
    ent_info.entry_qual[0].mask[0] = 0xffff;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = stat_sampling_gport;
    ent_info.entry_action[0].value[1] = stat_sampling_qualifier;

    printf("Adding Entry to the IPMF1 FG\n");
    rv = bcm_field_entry_add(unit, 0, cint_stat_sampling_fg_id, &ent_info, &cint_stat_sampling_ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    return 0;
}

int cint_field_stat_sampling_by_inport(int unit, bcm_field_context_t context_id, bcm_field_stage_t stage,int stat_sampling_code, int stat_sampling_qualifier, int port, int is_trunk)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_action_info_t action_info;
    int rv = BCM_E_NONE;


    /*
     * Create and attach TCAM group in iPMF
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = stage;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyInPort;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionStatSampling;

    printf("Creating IPMF FG\n");
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_stat_sampling_fg_id);
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

    printf("Attaching IPMF FG\n");
    rv = bcm_field_group_context_attach(unit, 0, cint_stat_sampling_fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    BCM_GPORT_MIRROR_SET(stat_sampling_gport, stat_sampling_code);
    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].mask[0] = 0xffff;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = stat_sampling_gport;
    ent_info.entry_action[0].value[1] = stat_sampling_qualifier;


    if (is_trunk) {

       int core_id, nof_cores;

       if (!BCM_GPORT_IS_TRUNK(port)) {
           printf("Error! port = 0x%08x is not trunk GPORT!\n");
           return BCM_E_PARAM;
       }

       nof_cores = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);

       ent_info.entry_qual[0].value[0] = port;

       for (core_id = 0; core_id < nof_cores; core_id ++) {
          ent_info.core = core_id;
          printf("Adding Entry to the IPMF FG - core = %d \n", core_id);
          rv = bcm_field_entry_add(unit, 0, cint_stat_sampling_fg_id, &ent_info, &cint_stat_sampling_ent_id);
          if (rv != BCM_E_NONE)
          {
              printf("Error (%d), in bcm_field_entry_add\n", rv);
              return rv;
          }
       }

    }
    else {

       bcm_gport_t port_gport;
       BCM_GPORT_LOCAL_SET(port_gport, port);

       ent_info.entry_qual[0].value[0] = port_gport;

       printf("Adding Entry to the IPMF FG\n");
       rv = bcm_field_entry_add(unit, 0, cint_stat_sampling_fg_id, &ent_info, &cint_stat_sampling_ent_id);
       if (rv != BCM_E_NONE)
       {
           printf("Error (%d), in bcm_field_entry_add\n", rv);
           return rv;
       }
    }

    return 0;
}


int cint_field_stat_sampling_by_inport_destroy(int unit, bcm_field_context_t context_id)
{
    int rv = BCM_E_NONE;

    /* Delete entry from Field group */
    rv = bcm_field_entry_delete(unit, cint_stat_sampling_fg_id, NULL, cint_stat_sampling_ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete entry %d from fg %d\n", rv, cint_stat_sampling_ent_id, cint_stat_sampling_fg_id);
        return rv;
    }
    /* Detach the IPMF1 FG from its context */
    rv = bcm_field_group_context_detach(unit, cint_stat_sampling_fg_id, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, cint_stat_sampling_fg_id, context_id);
        return rv;
    }

    /* Delete  FG */
    rv = bcm_field_group_delete(unit, cint_stat_sampling_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, cint_stat_sampling_fg_id);
        return rv;
    }

    return rv;
}


/**
* \brief
*  Configures FG in IPMF3 for qualifiers testing.
* \param [in] unit        -  Device id
* \param [in] context_id  -  PMF context Id
* \param [in] stat_sampling_code       -  stat_sampling command (8 bits)
* \param [in] stat_sampling_qualifier  -  stat_sampling qualifier (9 bits)
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_stat_sampling_qual_main(int unit, bcm_field_context_t context_id, int stat_sampling_code, int stat_sampling_qualifier)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_action_info_t action_info;
    int rv = BCM_E_NONE;

    /*
     * Create and attach TCAM group in iPMF3
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF3;
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = bcmFieldQualifyStatSamplingQualifier;
    fg_info.qual_types[1] = bcmFieldQualifyStatSamplingCode;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionPrioIntNew;

    printf("Creating IPMF3 FG\n");
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_stat_sampling_fg_id_qual);
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
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].input_arg = 0;
    attach_info.key_info.qual_info[1].offset = 0;

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];


    printf("Attaching IPMF3 FG\n");
    rv = bcm_field_group_context_attach(unit, 0, cint_stat_sampling_fg_id_qual, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = stat_sampling_qualifier;
    ent_info.entry_qual[0].mask[0] = 0x1ff;

    ent_info.entry_qual[1].type = fg_info.qual_types[1];
    stat_sampling_code += 480;
    ent_info.entry_qual[1].value[0] = stat_sampling_code;
    ent_info.entry_qual[1].mask[0] = 0xff;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = 3;

    printf("Adding Entry to the IPMF3 FG\n");
    rv = bcm_field_entry_add(unit, 0, cint_stat_sampling_fg_id_qual, &ent_info, &cint_stat_sampling_ent_id_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    return 0;
}

/**
* \brief
*  Destroy all configuration done for action test.
* \param [in] unit        -  Device id
* \param [in] context_id_ipmf1  -  PMF context Id for ipmf1 stage
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_stat_sampling_destroy(int unit, bcm_field_context_t context_id_ipmf1, bcm_field_context_t context_id_ipmf3)
{
    int rv = BCM_E_NONE;

    /* Delete entry from Field group */
    rv = bcm_field_entry_delete(unit, cint_stat_sampling_fg_id, NULL, cint_stat_sampling_ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete entry %d from fg %d\n", rv, cint_stat_sampling_ent_id, cint_stat_sampling_fg_id);
        return rv;
    }
    /* Detach the IPMF1 FG from its context */
    rv = bcm_field_group_context_detach(unit, cint_stat_sampling_fg_id, context_id_ipmf1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, cint_stat_sampling_fg_id, context_id_ipmf1);
        return rv;
    }

    /* Delete  FG */
    rv = bcm_field_group_delete(unit, cint_stat_sampling_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, cint_stat_sampling_fg_id);
        return rv;
    }

    /* Delete entry from Field group */
    rv = bcm_field_entry_delete(unit, cint_stat_sampling_fg_id_qual, NULL, cint_stat_sampling_ent_id_qual);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_delete entry %d from fg %d\n", rv, cint_stat_sampling_ent_id_qual, cint_stat_sampling_fg_id_qual);
       return rv;
    }
    /* Detach the IPMF1 FG from its context */
    rv = bcm_field_group_context_detach(unit, cint_stat_sampling_fg_id_qual, context_id_ipmf3);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, cint_stat_sampling_fg_id_qual, context_id_ipmf3);
       return rv;
    }

    /* Delete  FG */
    rv = bcm_field_group_delete(unit, cint_stat_sampling_fg_id_qual);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, cint_stat_sampling_fg_id_qual);
       return rv;
    }

    return rv;
}

