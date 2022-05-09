/*
 * Configuration example start:
 *
  cint ../../../src/examples/dnx/field/cint_field_stat_sampling.c
  cint ../../../../src/examples/dnx/field/cint_field_stat_sampling.c
  cint;
  int unit = 0;
  bcm_field_context_t context_id = 0;
  int stat_sampling_code = 0x12;
  int stat_sampling_qualifier = 0xab;
  cint_field_stat_sampling_main(unit, context_id, stat_sampling_code, stat_sampling_qualifier);
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

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyTpid;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionStatSampling;

    printf("Creating IPMF1 FG\n");
    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, &fg_info, &cint_stat_sampling_fg_id));


    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 96;

    printf("Attaching IPMF1 FG\n");
    BCM_IF_ERROR_RETURN(bcm_field_group_context_attach(unit, 0, cint_stat_sampling_fg_id, context_id, &attach_info));


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
    BCM_IF_ERROR_RETURN(bcm_field_entry_add(unit, 0, cint_stat_sampling_fg_id, &ent_info, &cint_stat_sampling_ent_id));

    return BCM_E_NONE;
}

int cint_field_stat_sampling_by_inport(int unit, bcm_field_context_t context_id, bcm_field_stage_t stage,int stat_sampling_code, int stat_sampling_qualifier, int port, int is_trunk)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_action_info_t action_info;

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = stage;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyInPort;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionStatSampling;

    printf("Creating IPMF FG\n");
    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, &fg_info, &cint_stat_sampling_fg_id));


    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    printf("Attaching IPMF FG\n");
    BCM_IF_ERROR_RETURN(bcm_field_group_context_attach(unit, 0, cint_stat_sampling_fg_id, context_id, &attach_info));


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
       uint32 valid_cores_bmp = *(dnxc_data_get(unit, "device", "general", "valid_cores_bitmap", NULL));

       if (!BCM_GPORT_IS_TRUNK(port)) {
           printf("Error! port = 0x%08x is not trunk GPORT!\n");
           return BCM_E_PARAM;
       }

       nof_cores = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);

       ent_info.entry_qual[0].value[0] = port;

       for (core_id = 0; core_id < nof_cores; core_id ++) {
          if ((valid_cores_bmp & (1 << core_id)) == 0)
          {
              continue;
          }
          ent_info.core = core_id;
          printf("Adding Entry to the IPMF FG - core = %d \n", core_id);
          BCM_IF_ERROR_RETURN(bcm_field_entry_add(unit, 0, cint_stat_sampling_fg_id, &ent_info, &cint_stat_sampling_ent_id));

       }

    }
    else {

       ent_info.entry_qual[0].value[0] = port;

       printf("Adding Entry to the IPMF FG\n");
       BCM_IF_ERROR_RETURN(bcm_field_entry_add(unit, 0, cint_stat_sampling_fg_id, &ent_info, &cint_stat_sampling_ent_id));

    }

    return BCM_E_NONE;
}


int cint_field_stat_sampling_by_inport_destroy(int unit, bcm_field_context_t context_id)
{
    BCM_IF_ERROR_RETURN(bcm_field_entry_delete(unit, cint_stat_sampling_fg_id, NULL, cint_stat_sampling_ent_id));

    BCM_IF_ERROR_RETURN(bcm_field_group_context_detach(unit, cint_stat_sampling_fg_id, context_id));

    BCM_IF_ERROR_RETURN(bcm_field_group_delete(unit, cint_stat_sampling_fg_id));


    return BCM_E_NONE;
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
    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, &fg_info, &cint_stat_sampling_fg_id_qual));



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
    BCM_IF_ERROR_RETURN(bcm_field_group_context_attach(unit, 0, cint_stat_sampling_fg_id_qual, context_id, &attach_info));


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
    BCM_IF_ERROR_RETURN(bcm_field_entry_add(unit, 0, cint_stat_sampling_fg_id_qual, &ent_info, &cint_stat_sampling_ent_id_qual));

    return BCM_E_NONE;
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
    BCM_IF_ERROR_RETURN(bcm_field_entry_delete(unit, cint_stat_sampling_fg_id, NULL, cint_stat_sampling_ent_id));

    BCM_IF_ERROR_RETURN(bcm_field_group_context_detach(unit, cint_stat_sampling_fg_id, context_id_ipmf1));

    BCM_IF_ERROR_RETURN(bcm_field_group_delete(unit, cint_stat_sampling_fg_id));

    BCM_IF_ERROR_RETURN(bcm_field_entry_delete(unit, cint_stat_sampling_fg_id_qual, NULL, cint_stat_sampling_ent_id_qual));

    BCM_IF_ERROR_RETURN(bcm_field_group_context_detach(unit, cint_stat_sampling_fg_id_qual, context_id_ipmf3));

    BCM_IF_ERROR_RETURN(bcm_field_group_delete(unit, cint_stat_sampling_fg_id_qual));

    return BCM_E_NONE;
}

