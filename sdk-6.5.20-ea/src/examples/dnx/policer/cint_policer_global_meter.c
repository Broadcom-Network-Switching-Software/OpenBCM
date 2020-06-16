/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_policer_global_meter.c
 * Purpose:     example for global meter configuration
 * 
 * *
 * The cint include:
 *  - sequence example of configuring global meter (ingress). Creating groups from L2 forwarding types (UC/MC/BC/UUC/UMC) 
 *      and configure its coresponding meter profiles.
 *  - sequence example of reconfigure the the global meter groups and profiles
 *  - configure the ingress final resolved table in such way that if global meter indicates "drop", the ingress DP will be red or black.
 */

/**
 * \brief
 *      cint example of creating global policer groups based on L2forwarding types (UC/UUC/MC/UMC/BC), configure its meter profiles and activate it.
 *      The sequence is:
 *      a. create group of L2 forwarding types. Each group goes to different global meter.
 *      b. configure the meters. one meter for each group.
 *      c. enable the global meter operation.
 * \param [in] unit -unit id
 * \param [in] core_id -core id (All/0/1)
 * \param [in] packet_mode -is meter is made based on packets or bytes
 * \param [in] color_blind_mode -if true, configure meter in blind mode. else color aware mode.
 * \param [in] header_truncate -if set, enable header_truncate for meter ingress compensation.
 * \return
 *   error indication.
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
policer_global_meter_example(
    int unit,
    bcm_core_t core,
    int packet_mode,
    int color_blind_mode,
    int header_truncate)
{
    int rv;
    int i, flags = 0;
    bcm_policer_expansion_group_t expansion_group;
    bcm_policer_config_t pol_cfg, pol_cfg_get;
    int policer_id, database_handle;

    bcm_policer_expansion_group_t_init(&expansion_group);
    /** step 1: configure expansion groups based on l2 forwarding types */
    /** in this example there are 3 groups: UC, MC+UMC, BC. (UUC does not go in any global meter) */
    expansion_group.config[bcmPolicerFwdTypeUc].offset = 0;
    expansion_group.config[bcmPolicerFwdTypeUc].valid = TRUE;
    expansion_group.config[bcmPolicerFwdTypeMc].offset = 1;
    expansion_group.config[bcmPolicerFwdTypeMc].valid = TRUE;
    expansion_group.config[bcmPolicerFwdTypeUnkownMc].offset = 1;
    expansion_group.config[bcmPolicerFwdTypeUnkownMc].valid = TRUE;
    expansion_group.config[bcmPolicerFwdTypeBc].offset = 2;
    expansion_group.config[bcmPolicerFwdTypeBc].valid = TRUE;

    expansion_group.config[bcmPolicerFwdTypeUnkownUc].valid = FALSE;

    rv = bcm_policer_expansion_groups_set(unit, flags, core, &expansion_group);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_policer_expansion_groups_set\n");
        return rv;
    }

    /** step 2: for each group need to configure its global meter. the offset indicates on the meter index */

    /** decode into database_handle, the database parameter that are revevant - core and is_global = TRUE*/
    BCM_POLICER_DATABASE_HANDLE_SET(database_handle, TRUE, TRUE, core, 0);

    /** set the meter configuration for offset 0 */
    bcm_policer_config_t_init(&pol_cfg);

    if (color_blind_mode == TRUE)
    {
        pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
    }
    if (packet_mode == TRUE)
    {
        pol_cfg.flags |= BCM_POLICER_MODE_PACKETS;
    }
    if (header_truncate == TRUE)
    {
        pol_cfg.flags |= BCM_POLICER_PKT_ADJ_HEADER_TRUNCATE;
    }
    pol_cfg.mode = bcmPolicerModeCommitted; /** global meter is always in committed mode (single bucket, one rate: drop or not) */
    pol_cfg.ckbits_sec = 30000; /** 30MB */
    pol_cfg.ckbits_burst = 1000;

    /** set the policer id, using the macro for offset 0*/
    BCM_POLICER_ID_SET(policer_id, database_handle, 0);

    rv = bcm_policer_set(unit, policer_id, &pol_cfg);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_policer_set for global meter offset 0\n");
        print rv;
        return rv;
    }

    /** set the meter configuration for offset 1 */
    pol_cfg.ckbits_sec = 30; /** 30Kbps - min rate for 1Ghz clock rate */
    pol_cfg.ckbits_burst = (32000 - 1); /** max bucket size (4MB) without using large bucket mode. */
    /** set the policer id, using the macro for offset 0*/
    BCM_POLICER_ID_SET(policer_id, database_handle, 1);

    rv = bcm_policer_set(unit, policer_id, &pol_cfg);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_policer_set for global meter offset 1\n");
        print rv;
        return rv;
    }

    /** set the meter configuration for offset 2 */
    pol_cfg.ckbits_sec = 1000000; /** 1Gbps */
    pol_cfg.ckbits_burst = (2048000 - 1); /** max bucket size (256MB) when using large bucket mode. 
                                                                                   this will influence badly on the output rate accuracy in the HW */
    /** set the policer id, using the macro for offset 2*/
    BCM_POLICER_ID_SET(policer_id, database_handle, 2);

    rv = bcm_policer_set(unit, policer_id, &pol_cfg);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_policer_set for global meter offset 2\n");
        print rv;
        return rv;
    }

    /** step 3: enable the global policer */
    rv = bcm_policer_database_enable_set(unit, flags, database_handle, TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_policer_database_enable_set for global meter\n");
        print rv;
        return rv;
    }

    return BCM_E_NONE;
}

/**
 * \brief
 *      cint example of getting global counters statistics for a
 *      given statistic and expected value(32b) - call API
 *      bcm_policer_stat_get and compare expected value with
 *      received value from the API
 * \param [in] unit -unit id
 * \param [in] policer_id -policer id
 * \param [in] stat - statistic to be retrieved - 
 *        RED/BLACK/REJECTED pkts/bytes
 * \param [in] expected -expected value(32b) 
 * \return
 *   error indication.
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
policer_global_meter_stat_get(
    int unit,
    bcm_policer_t policer_id,
    bcm_policer_stat_t stat,
    uint32 expected)
{
    int rv;
    uint64 value, expected_value;
    uint32 high32_data;

    COMPILER_64_SET(expected_value, high32_data, expected);

    rv = bcm_policer_stat_get(unit, policer_id, 0, stat, &value);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_policer_stat_get for global meter offset 0\n");
        print rv;
        return rv;
    }

    if ((COMPILER_64_HI(value) != COMPILER_64_HI(expected_value))
        || (COMPILER_64_LO(value) != COMPILER_64_LO(expected_value)))
    {
        printf("\t FAIL: policer_id=%d received value={0x%x,0x%x} expected value={0x%x,0x%x}\n",
               policer_id, COMPILER_64_HI(value), COMPILER_64_LO(value),
               COMPILER_64_HI(expected_value), COMPILER_64_LO(expected_value));
    }
    return BCM_E_NONE;
}

/**
 * \brief
 *      cint example of creating global policer groups based on L2forwarding types (UC/UUC/MC/UMC/BC), configure its meter profiles and activate it.
 *      The sequence is:
 *      a. create group of L2 forwarding types. 
 *      b. configure the meter
 *      c. enable the global meter operation.
 * \param [in] unit -unit id
 * \param [in] core_id -core id (All/0/1)
 * \param [in] database_id - database id
 * \param [in] policer_id - policer id
 * \param [in] database_handle -database_handle.
 * \return
 *   error indication.
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
policer_global_meter_single_group_example(
    int unit,
    bcm_core_t core,
    int database_id)
{
    int rv;
    int i, flags = 0;
    bcm_policer_expansion_group_t expansion_group;
    bcm_policer_config_t pol_cfg, pol_cfg_get;
    int policer_id, database_handle;

    bcm_policer_expansion_group_t_init(&expansion_group);
    /** step 1: configure expansion groups based on l2 forwarding types */
    /** in this example there is 1 group - UC */
    expansion_group.config[bcmPolicerFwdTypeUc].offset = 0;
    expansion_group.config[bcmPolicerFwdTypeUc].valid = TRUE;
    expansion_group.config[bcmPolicerFwdTypeMc].valid = FALSE;
    expansion_group.config[bcmPolicerFwdTypeUnkownMc].valid = FALSE;
    expansion_group.config[bcmPolicerFwdTypeBc].valid = FALSE;
    expansion_group.config[bcmPolicerFwdTypeUnkownUc].valid = FALSE;

    rv = bcm_policer_expansion_groups_set(unit, flags, core, &expansion_group);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_policer_expansion_groups_set\n");
        return rv;
    }

    /** step 2: for each group need to configure its global meter. the offset indicates on the meter index */

    /** decode into database_handle, the database parameter that are revevant - core and is_global = TRUE*/
    BCM_POLICER_DATABASE_HANDLE_SET(database_handle, TRUE, TRUE, core, database_id);
    /** set the meter configuration for offset 0 */
    bcm_policer_config_t_init(&pol_cfg);
    /**pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
    pol_cfg.flags |= BCM_POLICER_MODE_PACKETS;*/
    pol_cfg.mode = bcmPolicerModeCommitted; /** global meter is always in committed mode (single bucket, one rate: drop or not) */
    pol_cfg.ckbits_sec = 30000; /** 30MB */
    pol_cfg.ckbits_burst = 1000;

    /** set the policer id, using the macro for offset 0*/
    BCM_POLICER_ID_SET(policer_id, database_handle, 0);

    rv = bcm_policer_set(unit, policer_id, &pol_cfg);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_policer_set for global meter offset 0\n");
        print rv;
        return rv;
    }

    /** step 3: enable the global policer */
    rv = bcm_policer_database_enable_set(unit, flags, database_handle, TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_policer_database_enable_set for global meter\n");
        print rv;
        return rv;
    }

    return BCM_E_NONE;
}

bcm_field_group_t policer_fg_id;
bcm_field_entry_t policer_fe_id;

/*
 * Brief/
 * create global meter pointer, which represent the forwarding type using ingress PMF1.
 * user select the qualifier and the fwd_type.
 */
int
policer_global_meter_ptr_set_by_acl(
    int unit,
    bcm_field_qualify_t qual_type,
    uint32 qualify_mask,
    uint32 qualify_value,
    bcm_policer_fwd_types_t fwd_type)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    int in_port;
    bcm_gport_t gport;
    int rv;
    
    /** Create and attach first group */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = qual_type;
    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionForwardingTypeMeterMap;
    
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &policer_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }        
        
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];    
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    /* Change to DNX_FIELD_CONTEXT_DEFAULT_ID */
    rv = bcm_field_group_context_attach(unit, 0, policer_fg_id, BCM_FIELD_CONTEXT_ID_DEFAULT, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }    
    
    
    /* Add entry */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = qualify_value;
    ent_info.entry_qual[0].mask[0] = qualify_mask;

    if (qual_type == bcmFieldQualifyInPort)
    {
    	in_port = qualify_value;
        BCM_GPORT_LOCAL_SET(gport, in_port);
        ent_info.entry_qual[0].value[0] = gport;
    }

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = fwd_type;
    
    rv = bcm_field_entry_add(unit, 0, policer_fg_id, &ent_info, &policer_fe_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    printf("Entry add: id:(0x%x) global_meter_idx:(%d)\n", policer_fg_id ,fwd_type);

    return rv;            

}

/** Destroy ACL rule related - filed entry/ field group/
 *  field context - select index */
int policer_acl_rule_destroy(int unit)
{
    int rv = BCM_E_NONE;

    rv = bcm_field_entry_delete(unit, policer_fg_id, NULL, policer_fe_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete  fg %d \n", rv, policer_fg_id);
        return rv;
    }
    /** Sequence to destroy the context and delete the groups -
     *  for now is used the default context -  Cannot destroy
     *  context ID 0, since it is the default context */
     
    rv = bcm_field_group_context_detach(unit, policer_fg_id, BCM_FIELD_CONTEXT_ID_DEFAULT);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d stage %s\n", rv, policer_fg_id, 0);
        return rv;
    } 
    rv = bcm_field_group_delete(unit, policer_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, policer_fg_id);
        return rv;
    } 

    return rv;
}

