/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_field_advanced_presel.c
 * Purpose: Example of preselection with all the stages by the API bcm_field_presel_create_stage_id.
 *
 * Pre-Requirement: Peselectors management advanced mode should be enabled by SOC property.
 *
 *
 * CINT Usage:
 *
 *  1. Set SOC property:
 *      field_presel_mgmt_advanced_mode=1
 *
 *  2. Run the main function:
 *        field_presel_stage_run(IN int unit,
 *                               IN bcm_port_t in_port,
 *                               IN bcm_field_stage_t stage,
 *                               IN int presel_id);
 *      parameters :
 *      unit - the device unit
 *      in_port - the pkt incomming pp port
 *      stage - preselection stage (bcmFieldStageIngress , bcmFieldStageHash , bcmFieldStageEgress)
 *      presel_id - User provides preselector ID
 *
 *  Ranges for allowed preselector IDs:
 *   for bcmFieldStageIngress: 0..47
 *   for bcmFieldStageHash: 0..11
 *   for bcmFieldStageEgress: 0..15
 *
 * Flow:
 *  Each function creates a preselector in the desired stage,
 *  and then uses it in a field group.
 *  The preselector ID is predefined
 *  (needs to be input by the user to the API bcm_field_presel_create_stage_id)
 */

/*flag to indicate if we create the preselector and field group with program ID*/
int is_with_program_id=0;

/* Utility to get stage qualifier aand flag from BCM stage */
void presel_get_stage_info(/* in */ bcm_field_stage_t stage,
                       /* out */ bcm_field_qualify_t *qual,
                       /* out */ int *flag)
{
    switch(stage)
    {
    case bcmFieldStageEgress:
        *qual = bcmFieldQualifyStageEgress;
        *flag = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_EGRESS;
        break;
    case bcmFieldStageHash:
        *qual = bcmFieldQualifyStageHash;
        *flag = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_HASH;
        break;
     case bcmFieldStageHash:
        *qual = bcmFieldQualifyStageExternal;
        *flag = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_EXTERNAL;
        break;
    default:
        *qual = bcmFieldQualifyStageIngress;
        *flag = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        break;
    }
}

/* Creates a preselector in the desired stage */
int presel_create(/* in */ int unit,
                  /* in */ bcm_port_t in_port,
                  /* in */ bcm_field_stage_t stage,
                  /* in */ int presel_id)
{
    bcm_port_t mask=-1;
    int result = BCM_E_NONE;
    int auxRes;
    int presel_flags;
    int stage_flag;
    bcm_field_qualify_t dummy;
    char *proc_name="presel_create";

    /* Get stage-specific presel flag */
    presel_get_stage_info(stage, &dummy, &stage_flag);
    presel_flags = BCM_FIELD_QUALIFY_PRESEL;
    presel_flags |= stage_flag;

    if (is_with_program_id) {
        bcm_field_presel_entry_id_t entry_id;
        entry_id.stage = stage;
        entry_id.presel_id=presel_id;
        bcm_field_presel_entry_data_t entry_data;
        entry_data.context_id=10;

        result = bcm_field_presel_set(unit, 0, &entry_id, &entry_data); 
    } else {
        result = bcm_field_presel_create_stage_id(unit, stage, presel_id);
    }

    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_presel_create_stage_id\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id);
        return result;
    }
    /* Add InPort qualifier to the preselector */
    result = bcm_field_qualify_InPort(unit, presel_id | presel_flags, in_port, mask);
      if (BCM_E_NONE != result) {
        printf("%s():Error in bcm_field_qualify_InPort\n", proc_name);
        return result;
    }

    return result;
}


/* Creates a preselector in the desired stage */
int presel_create_second(/* in */ int unit,
                  /* in */ bcm_port_t in_port,
                  /* in */ bcm_field_stage_t stage,
                  /* in */ int presel_id)
{
    bcm_port_t mask=-1;
    int result = BCM_E_NONE;
    int auxRes;
    int presel_flags;
    int stage_flag;
    bcm_field_qualify_t dummy;
    char *proc_name="presel_create";

    /* Get stage-specific presel flag */
    presel_get_stage_info(stage, &dummy, &stage_flag);
    presel_flags = BCM_FIELD_QUALIFY_PRESEL;
    presel_flags |= stage_flag;

    if (is_with_program_id) {
        uint32 flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS; 
        bcm_field_presel_entry_id_t entry_id;
        entry_id.stage = stage;
        entry_id.presel_id=presel_id;
        bcm_field_presel_entry_data_t entry_data;
        entry_data.context_id=12;
        result = bcm_field_presel_set(unit, 0, &entry_id, &entry_data); 
    } else {
        result = bcm_field_presel_create_stage_id(unit, stage, presel_id); 
    }

    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_presel_create_stage_id\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id);
        return result;
    }

    /* Add InPort qualifier to the preselector */
    result = bcm_field_qualify_InPort(unit, presel_id | presel_flags, in_port+1, mask);
      if (BCM_E_NONE != result) {
        printf("%s():Error in bcm_field_qualify_InPort\n", proc_name);
        return result;
    }

    return result;
}

/* Function for creating a group with the API bcm_field_group_config_create */
int field_group_create_cfg(/* in */ int unit,
                /* in */  bcm_field_stage_t stage,
                 /* in */ int presel_id1,
                          int presel_id2)
{
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t entry;
    bcm_field_presel_set_t psset;
    bcm_field_group_config_t group;
    bcm_port_t mask=-1;
    int presel_flags = 0;
    int result = BCM_E_NONE;
    int auxRes;
    int dummy;
    bcm_field_qualify_t stage_qual;
    char *proc_name= "group_create";
    uint32 pgm_bmp = 0x1400; /* bit map for programs 10 and 12 */

    /* Obtain stage qualifier */
    presel_get_stage_info(stage, &stage_qual, &dummy);

    /* group ID */
    bcm_field_group_config_t_init(group);
    group.flags= 0;
    printf("%s(): Group ID = %d\n", proc_name, group.group);

    /* priority */
    group.priority = BCM_FIELD_GROUP_PRIO_ANY;
    printf("%s(): Group priority = %d\n", proc_name, BCM_FIELD_GROUP_PRIO_ANY);

    /* Qualifiers Set */
    BCM_FIELD_QSET_INIT(group.qset);
    BCM_FIELD_QSET_ADD(qset, stage_qual);
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyOuterVlan);

    /* Actions Set */
    group.flags |= BCM_FIELD_GROUP_CREATE_WITH_ASET;
    BCM_FIELD_ASET_INIT(group.aset);
    BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionDrop);

    /* Preselectors Set */
    BCM_FIELD_PRESEL_INIT(psset);
    BCM_FIELD_PRESEL_ADD(psset, presel_id1);
    if (is_with_program_id) {
        BCM_FIELD_PRESEL_ADD(psset, presel_id2);
    }
     
    /* Make field group with auto mode */
    group.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE ;
    group.mode = bcmFieldGroupModeAuto;

    /* Make the field group use preselset */
    group.flags |= BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    group.preselset = psset;

    /* Create the field group with program_id*/
    if (is_with_program_id) {
        group.flags |= BCM_FIELD_GROUP_CREATE_WITH_PROGRAMS; 
        group.pgm_bmp = pgm_bmp;
    }
     
    /* Create the field group */
    result = bcm_field_group_config_create(unit, &group);
    if (BCM_E_NONE != result) {
        printf("%s(): Error in bcm_field_group_config_create\n", proc_name);
        return result;
    }
    printf("%s(): Group created.\n", proc_name);

    /* Create entries */
    result = bcm_field_entry_create(unit, group.group, &entry);
     if (BCM_E_NONE != result) {
            printf("%s(): Error in bcm_field_entry_create\n", proc_name);
            auxRes = bcm_field_entry_destroy(unit, entry);
            auxRes = bcm_field_group_destroy(unit, group.group);
            return result;
        }
    printf("%s(): Entry %d created.\n", proc_name, entry);

    /* Create qualifier OuterVlan */
    result = bcm_field_qualify_OuterVlan(unit,entry,100,mask);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_OuterVlan %x\n", result);
        return result;
    }
    printf("%s(): Qualifier added to entry %d.\n", proc_name,entry);

    /* Create action */
    result = bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != result) {
        printf("%s(): Error in bcm_field_action_add\n", proc_name);
        auxRes = bcm_field_entry_destroy(unit,entry);
        auxRes = bcm_field_group_destroy(unit, group.group);
        return result;
    }
    printf("%s(): Action(s) added to entry %d.\n", proc_name, entry);

    /* Install group to the HW */
    result = bcm_field_group_install(unit,group.group);
    if (BCM_E_NONE != result) {
        printf("%s(): Error in bcm_field_group_install\n", proc_name);
        auxRes = bcm_field_group_destroy(unit, group.group);
        return result;
    }
    printf("%s(): Entry %d installed.\n", proc_name, entry);

    return result;
}


/* Main function */
int field_presel_stage_run(/* in */ int unit, 
                           /* in */ bcm_port_t in_port, 
                           /* in */ bcm_field_stage_t stage,
                           /* in */ int presel_id1,
                                    int presel_id2)
{
    int result = BCM_E_NONE;
    char *proc_name="field_presel_stage_run";
    /* Create a preselector instance */
    result = presel_create(unit, in_port, stage, presel_id1);
    if (BCM_E_NONE != result) {
        printf("%s():Error in presel_create function\n", proc_name);
        return result;
    }

    if (is_with_program_id) {
        result = presel_create_second(unit, in_port, stage, presel_id2); 
        if (BCM_E_NONE != result) {
            printf("%s():Error in presel_create function\n", proc_name);
            return result;
        } 
    }
    
    /* Create a field group */
    result = field_group_create_cfg(unit, stage, presel_id1, presel_id2);
    if (BCM_E_NONE != result) {
        printf("%s():Error in group_create function\n", proc_name);
        return result;
    }

    return result;
}


