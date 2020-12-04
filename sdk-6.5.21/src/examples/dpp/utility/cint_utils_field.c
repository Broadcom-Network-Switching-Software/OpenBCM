/*
 * $id cint_utils_field.c $
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Different field processor utility functions and constants
 */


/*
 * Pre-defined action codes
 * These values are hard-coded and should not be changed
 */
int prge_action_code_bfd_c_bit_clear_wa        = 1;
int prge_action_code_reflector_1pass           = 2;
int prge_action_code_oam_end_tlv_wa            = 3;

int group_id = -1;
int entry_id = -1;

/*
 * prge_action_create_field_group
 *
 * Allocates Egress PMF program and defines a key and action
 * Key: (DstPort,FheiSize,Fhei.TrapCode)
 * Action: Stat
 */
int field__prge_action_create_field_group(int unit, int group_priority, bcm_field_group_t grp_tcam, int qual_id)
{
    int result;
    int presel_id;
    bcm_field_aset_t aset;
    bcm_field_data_qualifier_t data_qual;
    bcm_field_presel_set_t psset;
    bcm_field_group_config_t grp_conf;

    /* 
     *  For Advanced mode - we must use bcm_field_presel_create_stage_id 
     * , instead of bcm_field_presel_create. Also select some id before creating it.
     *  Important : In advanced mode, we must use BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_EGRESS
     *  with all  functions - create_stage_id, qualify_stage and qualify_ForwardingType
     */
    if ((soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, 0) ==1))   {
         /* Set a presel id */
         presel_id = 4;
         /* Use create_stage_id for advanced mode, also add the bcm_field_presel_advanced_mode_stage_egress flag */
         result = bcm_field_presel_create_stage_id(unit, bcmFieldStageEgress, presel_id | BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_EGRESS);
         if (result != BCM_E_NONE) {
             printf("Error in : bcm_field_presel_create_stage_id returned %d \n", result);
             return result;
         }
         /* Qualify_Stage with included  bcm_field_presel_advanced_mode_stage_egress flag */
         result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_EGRESS, bcmFieldStageEgress);
         if (BCM_E_NONE != result) {
             printf("Error in bcm_field_qualify_Stage\n");
             return result;
         }
         /* Qualify_ForwardingType with included  bcm_field_presel_advanced_mode_stage_egress flag */
         result = bcm_field_qualify_ForwardingType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_EGRESS, bcmFieldForwardingTypeRxReason);
         if (BCM_E_NONE != result) {
             printf("Error in bcm_field_qualify_ForwardingType\n");
             return result;
         }
    } else {
        /* Create a presel entity, regular way */ 
        result = bcm_field_presel_create(unit, &presel_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_id\n");
            result = bcm_field_presel_destroy(unit, presel_id);
            return result;
        }
        /* Qualify_Stage */
        result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageEgress);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_Stage\n");
            return result;
        }
        /* Qualify_ForwardingType */
        result = bcm_field_qualify_ForwardingType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldForwardingTypeRxReason);
        if (BCM_E_NONE != result) {
           printf("Error in bcm_field_qualify_ForwardingType\n");
           return result;
        }
    }

    BCM_FIELD_PRESEL_INIT(psset);
    BCM_FIELD_PRESEL_ADD(psset, presel_id);

    bcm_field_group_config_t_init(&grp_conf);
    grp_conf.group = grp_tcam;
    grp_conf.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp_conf.mode = bcmFieldGroupModeAuto;
    grp_conf.priority = group_priority;
    grp_conf.preselset = psset;

    BCM_FIELD_QSET_INIT(grp_conf.qset);
    BCM_FIELD_QSET_ADD(grp_conf.qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(grp_conf.qset, bcmFieldQualifyDstPort);
    BCM_FIELD_QSET_ADD(grp_conf.qset, bcmFieldQualifyFheiSize);

    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_WITH_ID | BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
    data_qual.qual_id = qual_id;
    data_qual.offset = 0;
    data_qual.qualifier = bcmFieldQualifyFhei;
    data_qual.length = 8;
    data_qual.stage = bcmFieldStageEgress;
    result = bcm_field_data_qualifier_create(unit, &data_qual);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_data_qualifier_create\n");
        return result;
    }

    result = bcm_field_qset_data_qualifier_add(unit, &grp_conf.qset, data_qual.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add\n");
        return result;
    }

    result = bcm_field_group_config_create(unit, &grp_conf);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_config_create for group %d\n", grp_conf.group);
        return result;
    }

    /* Define the ASET - use counter 0. */
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat);

    /* Attach the action set */
    result = bcm_field_group_action_set(unit, grp_tcam, aset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set for group %d\n", grp_tcam);
      return result;
    }

    return result;
}

/*
 * prge_action_strict_qualifier_create_field_group
 *
 * Allocates Egress PMF program and defines a key and action
 * Key: (DstPort,FheiSize,Fhei.TrapCode,Ftmh.pph_type,Otsh.OAM_sub_type)
 * Action: Stat
 */
int prge_action_strict_qualifier_create_field_group(int unit, int group_priority, bcm_field_group_t grp_tcam, int qual_id)
{
    int result;
    int presel_id;
    bcm_field_aset_t aset;
    bcm_field_data_qualifier_t data_qual;
    bcm_field_presel_set_t psset;
    bcm_field_group_config_t grp_conf;

    /* 
     *  For Advanced mode - we must use bcm_field_presel_create_stage_id 
     * , instead of bcm_field_presel_create. Also select some id before creating it.
     *  Important : In advanced mode, we must use BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_EGRESS
     *  with all  functions - create_stage_id, qualify_stage and qualify_ForwardingType
     */
    if ((soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, 0) ==1))   {
         /* Set a presel id */
         presel_id = 4;
         /* Use create_stage_id for advanced mode, also add the bcm_field_presel_advanced_mode_stage_egress flag */
         result = bcm_field_presel_create_stage_id(unit, bcmFieldStageEgress, presel_id | BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_EGRESS);
         if (result != BCM_E_NONE) {
             printf("Error in : bcm_field_presel_create_stage_id returned %d \n", result);
             return result;
         }
         /* Qualify_Stage with included  bcm_field_presel_advanced_mode_stage_egress flag */
         result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_EGRESS, bcmFieldStageEgress);
         if (BCM_E_NONE != result) {
             printf("Error in bcm_field_qualify_Stage\n");
             return result;
         }
         /* Qualify_ForwardingType with included  bcm_field_presel_advanced_mode_stage_egress flag */
         result = bcm_field_qualify_ForwardingType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_EGRESS, bcmFieldForwardingTypeRxReason);
         if (BCM_E_NONE != result) {
             printf("Error in bcm_field_qualify_ForwardingType\n");
             return result;
         }
    } else {
        /* Create a presel entity, regular way */ 
        result = bcm_field_presel_create(unit, &presel_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_id\n");
            result = bcm_field_presel_destroy(unit, presel_id);
            return result;
        }
        /* Qualify_Stage */
        result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageEgress);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_Stage\n");
            return result;
        }
        /* Qualify_ForwardingType */
        result = bcm_field_qualify_ForwardingType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldForwardingTypeRxReason);
        if (BCM_E_NONE != result) {
           printf("Error in bcm_field_qualify_ForwardingType\n");
           return result;
        }
    }

    BCM_FIELD_PRESEL_INIT(psset);
    BCM_FIELD_PRESEL_ADD(psset, presel_id);

    bcm_field_group_config_t_init(&grp_conf);
    grp_conf.group = grp_tcam;
    grp_conf.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp_conf.mode = bcmFieldGroupModeAuto;
    grp_conf.priority = group_priority;
    grp_conf.preselset = psset;

    BCM_FIELD_QSET_INIT(grp_conf.qset);
    BCM_FIELD_QSET_ADD(grp_conf.qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(grp_conf.qset, bcmFieldQualifyDstPort);
    BCM_FIELD_QSET_ADD(grp_conf.qset, bcmFieldQualifyPphType);
    BCM_FIELD_QSET_ADD(grp_conf.qset, bcmFieldQualifyFheiSize);

    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_WITH_ID | BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
    data_qual.qual_id = qual_id;
    data_qual.offset = 0;
    data_qual.qualifier = bcmFieldQualifyFhei;
    data_qual.length = 8;
    data_qual.stage = bcmFieldStageEgress;
    result = bcm_field_data_qualifier_create(unit, &data_qual);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_data_qualifier_create\n");
        return result;
    }

    result = bcm_field_qset_data_qualifier_add(unit, &grp_conf.qset, data_qual.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add\n");
        return result;
    }

    /* Create qualifier for OTSH.OAM_sub_type */
    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_WITH_ID | BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
    data_qual.qual_id = qual_id+1;
    data_qual.offset = 43;
    data_qual.qualifier = bcmFieldQualifyOamTsSystemHeader;
    data_qual.length = 3;
    data_qual.stage = bcmFieldStageEgress;
    result = bcm_field_data_qualifier_create(unit, &data_qual);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_data_qualifier_create\n");
        return result;
    }

    result = bcm_field_qset_data_qualifier_add(unit, &grp_conf.qset, data_qual.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add\n");
        return result;
    }

    result = bcm_field_group_config_create(unit, &grp_conf);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_config_create for group %d\n", grp_conf.group);
        return result;
    }

    /* Define the ASET - use counter 0. */
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat);

    /* Attach the action set */
    result = bcm_field_group_action_set(unit, grp_tcam, aset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set for group %d\n", grp_tcam);
      return result;
    }

    return result;
}

/*
 * prge_action_code_add_entry
 *
 * Adds an entry for the key and sets value=action_code for
 * the action created by field__prge_action_create_field_group
 */
int field__prge_action_code_add_entry(int unit, bcm_field_group_t grp_tcam,
                                      uint8 trap_code, int dest_port, int qual_id,
                                      int action_code)
{
    int result;
    bcm_field_entry_t ent_tcam;
    bcm_gport_t local_gport;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    int modid;
    int ace_var, statId;
    uint8 dqData[1], dqMask[1];

    char *proc_name ;
    bcm_field_action_core_config_t core_config_arr[SOC_DPP_DEFS_MAX_NOF_CORES()] ;
    int core_config_arr_len ;
    unsigned long local_gport_as_long ;
    unsigned long statId_as_long ;
    int action_stat ;
    int cores_num, ii ;

    proc_name = "field__prge_action_code_add_entry" ;
    printf("%s(): Enter\r\n",proc_name) ;
    
    /* Get the actual device cores number */
    cores_num = SOC_DPP_DEFS_GET_NOF_CORES(unit);

    result = bcm_stk_modid_get(unit,&modid);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_stk_modid_get\n");
        return result;
    }

    result = bcm_port_get(unit, dest_port, &flags, &interface_info, &mapping_info);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_port_get\n");
        return result;
    }

    result = bcm_field_entry_create(unit, grp_tcam, &ent_tcam);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }

    /* Entry qualifier values */
    result = bcm_field_qualify_DstPort(unit, ent_tcam,
                                       modid + mapping_info.core, 0xffffffff,
                                       mapping_info.tm_port, 0xffffffff);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_DstPort\n");
        return result;
    }

    result = bcm_field_qualify_FheiSize(unit, ent_tcam, 3, 0xffffffff);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_FheiSize\n");
        return result;
    }

    dqData[0] = trap_code;
    dqMask[0] = 0xFF;
    result = bcm_field_qualify_data(unit, ent_tcam, qual_id, &(dqData[0]), &(dqMask[0]), 1 /* len in bytes */);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_data\n");
        return result;
    }

    /* Entry Action values */
    BCM_GPORT_LOCAL_SET(local_gport, dest_port);
    ace_var = (action_code << 8);
    statId = (ace_var << 19); /* Set internal data */

    action_stat = bcmFieldActionStat ;
    local_gport_as_long = local_gport ;

    if (is_device_or_above(unit, JERICHO)) {
        /*
         * Enter for Jericho and up
         */

        /*
         * Set the counter at the same location as in Arad (since the LS 19 bits are '0'):
         * For Arad, when the LS 19 bits are zero, on statId, selected counter will be as the destination port on the
         * last 256 entries of the Ace table (which is 4K entries long).
         * On Jericho, the local destination port may exceed 255, so the tm_port is used instead.
         */
        statId |= (4*1024 - 256 - 1 + mapping_info.tm_port) ;
        statId_as_long = statId ;

        printf("%s(): JERICHO: statId %d (0x%08lX) local_gport %d (0x%08lX) dest_port %d\r\n",proc_name,statId,statId_as_long,local_gport,local_gport_as_long,dest_port) ;

        for (ii = 0 ; ii < cores_num ; ii++) {
          core_config_arr[ii].param0 = statId ;
          core_config_arr[ii].param1 = local_gport ;
          core_config_arr[ii].param2 = BCM_ILLEGAL_ACTION_PARAMETER ;
        }

        printf("%s(): Call bcm_field_action_config_add(ent_tcam %d bcmFieldActionStat %d statId %d local_gport %d)\r\n",proc_name,ent_tcam,action_stat,statId,local_gport) ;

        /*
         * Setting 'core_config_arr_len' to '1' results in setting all cores the same way.
         */
        core_config_arr_len = cores_num ;
        result = bcm_field_action_config_add(unit, ent_tcam, bcmFieldActionStat, core_config_arr_len, &core_config_arr[0]) ;
        if (BCM_E_NONE != result) {
            printf("%s(): Error in bcm_field_action_config_add\n",proc_name);
            return result;
        }
    } else {
        statId_as_long = statId ;

        printf("%s(): ARAD: statId %d (0x%08lX) local_gport %d (0x%08lX) dest_port %d\r\n",proc_name,statId,statId_as_long,local_gport,local_gport_as_long,dest_port) ;

        result = bcm_field_action_add(unit, ent_tcam, bcmFieldActionStat, statId, local_gport);
        if (BCM_E_NONE != result) {
            printf("%s(): Error in bcm_field_action_add\n",proc_name);
            return result;
        }
    }

    result = bcm_field_entry_install(unit, ent_tcam);
    if (BCM_E_NONE != result) {
        printf("%s(): Error in bcm_field_entry_install\n",proc_name);
        return result;
    }

    printf("%s(): Exit OK\r\n",proc_name) ;
    return result;
}

/*
 * prge_action_code_strict_qualifier_add_entry
 *
 * Adds an entry for the key and sets value=action_code for
 * the action created by field__prge_action_strict_qualifier_create_field_group
 */
int prge_action_code_strict_qualifier_add_entry(int unit, bcm_field_group_t grp_tcam,
                                      uint8 trap_code, int dest_port, int pph_type,
                                      int oam_sub_type, int qual_id, int action_code)
{
    int result;
    bcm_field_entry_t ent_tcam;
    bcm_gport_t local_gport;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    int modid;
    int ace_var, statId;
    uint8 dqData[1], dqMask[1];

    char *proc_name ;
    bcm_field_action_core_config_t core_config_arr[SOC_DPP_DEFS_MAX_NOF_CORES()] ;
    int core_config_arr_len ;
    unsigned long local_gport_as_long ;
    unsigned long statId_as_long ;
    int action_stat ;
    int cores_num, ii ;

    proc_name = "field__prge_action_code_add_entry" ;
    printf("%s(): Enter\r\n",proc_name) ;
    
    /* Get the actual device cores number */
    cores_num = SOC_DPP_DEFS_GET_NOF_CORES(unit);

    result = bcm_stk_modid_get(unit,&modid);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_stk_modid_get\n");
        return result;
    }

    result = bcm_port_get(unit, dest_port, &flags, &interface_info, &mapping_info);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_port_get\n");
        return result;
    }

    result = bcm_field_entry_create(unit, grp_tcam, &ent_tcam);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }

    /* Entry qualifier values */
    result = bcm_field_qualify_DstPort(unit, ent_tcam,
                                       modid + mapping_info.core, 0xffffffff,
                                       mapping_info.tm_port, 0xffffffff);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_DstPort\n");
        return result;
    }

    result = bcm_field_qualify_PphType(unit, ent_tcam, pph_type, (pph_type == 0) ? 0 : 0x3);
    if (result != BCM_E_NONE) {
        printf("Error, bcm_field_qualify_PphType()\n");
        return result;
    }

    result = bcm_field_qualify_FheiSize(unit, ent_tcam, 3, 0xffffffff);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_FheiSize\n");
        return result;
    }

    dqData[0] = trap_code;
    dqMask[0] = 0xFF;
    result = bcm_field_qualify_data(unit, ent_tcam, qual_id, &(dqData[0]), &(dqMask[0]), 1 /* len in bytes */);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_data\n");
        return result;
    }

    dqData[0] = oam_sub_type;
    dqMask[0] = (oam_sub_type == 0) ? 0 : 0x7;
    result = bcm_field_qualify_data(unit, ent_tcam, qual_id+1, &(dqData[0]), &(dqMask[0]), 1 /* len in bytes */);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_data\n");
        return result;
    }

    /* Entry Action values */
    BCM_GPORT_LOCAL_SET(local_gport, dest_port);
    ace_var = (action_code << 8);
    statId = (ace_var << 19); /* Set internal data */

    action_stat = bcmFieldActionStat ;
    local_gport_as_long = local_gport ;

    if (is_device_or_above(unit, JERICHO)) {
        /*
         * Enter for Jericho and up
         */

        /*
         * Set the counter at the same location as in Arad (since the LS 19 bits are '0'):
         * For Arad, when the LS 19 bits are zero, on statId, selected counter will be as the destination port on the
         * last 256 entries of the Ace table (which is 4K entries long).
         * On Jericho, the local destination port may exceed 255, so the tm_port is used instead.
         */
        statId |= (4*1024 - 256 - 1 + mapping_info.tm_port) ;
        statId_as_long = statId ;

        printf("%s(): JERICHO: statId %d (0x%08lX) local_gport %d (0x%08lX) dest_port %d\r\n",proc_name,statId,statId_as_long,local_gport,local_gport_as_long,dest_port) ;

        for (ii = 0 ; ii < cores_num ; ii++) {
          core_config_arr[ii].param0 = statId ;
          core_config_arr[ii].param1 = local_gport ;
          core_config_arr[ii].param2 = BCM_ILLEGAL_ACTION_PARAMETER ;
        }

        printf("%s(): Call bcm_field_action_config_add(ent_tcam %d bcmFieldActionStat %d statId %d local_gport %d)\r\n",proc_name,ent_tcam,action_stat,statId,local_gport) ;

        /*
         * Setting 'core_config_arr_len' to '1' results in setting all cores the same way.
         */
        core_config_arr_len = cores_num ;
        result = bcm_field_action_config_add(unit, ent_tcam, bcmFieldActionStat, core_config_arr_len, &core_config_arr[0]) ;
        if (BCM_E_NONE != result) {
            printf("%s(): Error in bcm_field_action_config_add\n",proc_name);
            return result;
        }
    } else {
        statId_as_long = statId ;

        printf("%s(): ARAD: statId %d (0x%08lX) local_gport %d (0x%08lX) dest_port %d\r\n",proc_name,statId,statId_as_long,local_gport,local_gport_as_long,dest_port) ;

        result = bcm_field_action_add(unit, ent_tcam, bcmFieldActionStat, statId, local_gport);
        if (BCM_E_NONE != result) {
            printf("%s(): Error in bcm_field_action_add\n",proc_name);
            return result;
        }
    }

    result = bcm_field_entry_install(unit, ent_tcam);
    if (BCM_E_NONE != result) {
        printf("%s(): Error in bcm_field_entry_install\n",proc_name);
        return result;
    }

    printf("%s(): Exit OK\r\n",proc_name) ;
    return result;
}

/*
 * Create the ingress field processor rule
 * Key: SrcPort+OuterVLAN
 * Action: Redirect to DstPort and trap packet
 */
int field__redirect_trap_ingress_rule_create(int unit,
                                             bcm_port_t src_port, int vlan,
                                             bcm_port_t dst_port, int trap_code, int trap_strength, uint8 redirect) {

    int rv = BCM_E_NONE;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_group_config_t grp;
    bcm_gport_t trap_gport, src_gport, dst_gport;

    BCM_GPORT_TRAP_SET(trap_gport, trap_code, trap_strength, 0);
    BCM_GPORT_LOCAL_SET(src_gport, src_port);
    BCM_GPORT_LOCAL_SET(dst_gport, dst_port);

    bcm_field_group_config_t_init(&grp);

    /*
     * Define QSET - Key = VLAN + Source port
     */
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);

    /* if Ceragon RH is enabled, set key by port only */
    if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ch_enable", 0)))
    {
        BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyOuterVlan);
    }
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifySrcPort);

    /*
     * Define ASET - Trap and redirect
     */
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionTrap);
    if (redirect) {
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);
    }

    /*
     * Create the Field group with type Direct Extraction
     */
    grp.priority = BCM_FIELD_GROUP_PRIO_ANY;
    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_create\n");
        return rv;
    }
    printf("Created group with id: 0x%08x\n", grp.group);
    group_id = grp.group;

    /*  Attach the action set */
    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_action_set\n");
        return rv;
    }

    /*
     * Create relevant entry
     */
    rv = bcm_field_entry_create(unit, grp.group, &ent);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_create (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Qualifier value */
    rv = bcm_field_qualify_SrcPort(unit, ent, 0, 0, src_gport, -1);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_SrcPort (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* if Ceragon RH is enabled, key doesn't hold outer vlan */
    if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ch_enable", 0)))
    {
        rv = bcm_field_qualify_OuterVlan(unit, ent, vlan, -1);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_qualify_OuterVlan (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }
    /* Actions */
    
    if (redirect) {
        rv = bcm_field_action_add(unit, ent, bcmFieldActionRedirect, 0, dst_gport);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_action_add (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    rv = bcm_field_action_add(unit, ent, bcmFieldActionTrap, trap_gport, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_action_add (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Install entry */
    rv = bcm_field_entry_install(unit, ent);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_install (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    printf("Entry created %d\n", ent);
    entry_id = ent;

    return BCM_E_NONE;
}

int field__get_qualify_IncomingIpIfClass_mask(int unit) {
    uint32 ipmc_l3mcastl2_mode, ipv4_double_capacity_used;
    uint32 nof_dedicated_bits, bit_offset_of_routing_enablers_profile;
    uint32 routing_enablers_bm;
    int qualMask = 0;
    int i;

    ipmc_l3mcastl2_mode  = soc_property_get(unit, spn_IPMC_L3MCASTL2_MODE, 0);
    ipv4_double_capacity_used = (soc_property_get(unit, spn_EXT_IP4_DOUBLE_CAPACITY_FWD_TABLE_SIZE, 0) == 0) ? 0 : 1;
    nof_dedicated_bits = 2 + ipmc_l3mcastl2_mode + ipv4_double_capacity_used;

    routing_enablers_bm = soc_property_get(unit, spn_NUMBER_OF_INRIF_MAC_TERMINATION_COMBINATIONS, 16);

    for (i = 6 - nof_dedicated_bits - 1; i >= 0; i--) {
        if (routing_enablers_bm > (1 << i) ) {
            bit_offset_of_routing_enablers_profile = 6 - nof_dedicated_bits - i -1;
            break;
        }
    }

    qualMask = (1 << (6 - nof_dedicated_bits)) - (1 << bit_offset_of_routing_enablers_profile);
    print qualMask;

    return qualMask;
}


/* 
* Setting a FG with qualifier bcmFieldQualifyIncomingIpIfClass 
* with profile number in_rif_profile and action_in(with param0&param1).
*/
int field__qualify_IncomingIpIfClass(int unit, int in_rif_profile, bcm_field_action_t action_in, uint32 param0, uint32 param1) {
	int result;
    bcm_field_group_config_t fg;
    int qualMask;

    bcm_field_group_config_t_init(&fg);

	/* QSET */
    BCM_FIELD_QSET_INIT(fg.qset);
	BCM_FIELD_QSET_ADD(fg.qset,bcmFieldQualifyIncomingIpIfClass);
    fg.flags = BCM_FIELD_GROUP_CREATE_WITH_ASET;

	/* ASET */
    BCM_FIELD_ASET_INIT(fg.aset);
    BCM_FIELD_ASET_ADD(fg.aset, action_in);
	
    result = bcm_field_group_config_create(unit,&fg);
    if ( result != BCM_E_NONE ) {
        printf("Error in bcm_field_group_config_create\n");
        return result;
    }

	/* entry */
    bcm_field_entry_t ent;
    result = bcm_field_entry_create(unit, fg.group, &ent);
    if ( result != BCM_E_NONE ) {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }
	
    qualMask = field__get_qualify_IncomingIpIfClass_mask(unit);
    result = bcm_field_qualify_IncomingIpIfClass(unit, ent, in_rif_profile, qualMask);
    if ( result != BCM_E_NONE ) {
        printf("bcm_field_qualify_IncomingIpIfClass\n");
        return result;
    }

    /* action add */
    result = bcm_field_action_add(unit, ent, action_in, param0,param1 );
    if ( result != BCM_E_NONE ) {
        printf("bcm_field_action_add\n");
        return result;
    }

	/* installs */
    result = bcm_field_group_install(unit, fg.group);
    if ( result != BCM_E_NONE ) {
        printf("Error in bcm_field_group_install\n");
        return result;
    }

	printf("Field group installed successfully \n");
	return 0;
}
