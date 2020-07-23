/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/**
 * Example of cascading on same qualifier twice.
 *
 *
 *  Configuration example:
 *
 *  cint;
 *  cint_reset();
 *  exit;
 *  cint ../../src/examples/dpp/cint_field_large_de_same_qual_cascade.c
 *  cint;
 *  int unit = 0;
 *  int ether_type = 0x0800;
 *  bcm_field_group_t tcam_group_id = 10;
 *  int tcam_group_prio = BCM_FIELD_GROUP_PRIO_ANY;
 *  bcm_field_group_t de_group_id = 20;
 *  int de_group_prio = BCM_FIELD_GROUP_PRIO_ANY;
 *  cint_field_large_de_same_qual_cascade_main(unit, ether_type, tcam_group_id, tcam_group_prio, de_group_id, de_group_prio);
 *
 * This cint shows an example of counting packets per IN_LIF + cascaded_value and VLAN + cascaded_value,
 * where the cascaded_value is identified in the first lookup and cascaded to the second lookup, where it is used in DE with IN_LIF/VLAN.
 *
 * CINT configuration:
 *      1. Creating TCAM FG in first cycle to qualify upon EtherType and set the CascadedKeyValue.
 *      2. Creating DE FG in second cycle to update Stat and Stat1 actions as follow:
 *          - Stat = CascadedKeyValue(4b) + InVPort(16b) + ConstantZero(2b)
 *          - Stat1 = CascadedKeyValue(4b) + InnerVlanId(12b) + ConstantZero(6b)
 */

int Cint_field_large_de_same_qual_cascade_key_value = 0x5;
int Cint_field_large_de_same_qual_cascade_key_size = 4;

/**
 * \brief
 *  This function creates field group, with all relevant
 *  information to it and adds entry to it.
 *
 * \param [in] unit   - Device id
 * \param [in] ether_type   - EtherType to qualify upon in TCAM FG.
 * \param [in] tcam_group_id   - TCAM FG Id
 * \param [in] tcam_group_prio   - Priority of TCAM FG on first cycle.
 * \param [in] de_group_id   - Direct Extraction FG Id.
 * \param [in] de_group_prio   - Priority of DE FG on second cycle.
 * \
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_large_de_same_qual_cascade_main(
    int unit,
    int ether_type,
    bcm_field_group_t tcam_group_id,
    int tcam_group_prio,
    bcm_field_group_t de_group_id,
    int de_group_prio)
{
    int rv = BCM_E_NONE;
    bcm_field_group_config_t tcam_group;
    bcm_field_entry_t tcam_entry_handle;
    bcm_field_group_config_t de_group;
    bcm_field_entry_t de_entry_handle;
    bcm_field_data_qualifier_t data_qual;
    bcm_field_extraction_action_t extraction_action;
    bcm_field_extraction_field_t extraction_fields[3];

    bcm_field_group_config_t_init(&de_group);
    de_group.group = de_group_id;
    de_group.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID |
                  BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_CYCLE |
                  BCM_FIELD_GROUP_CREATE_LARGE;
    de_group.mode = bcmFieldGroupModeDirectExtraction;
    de_group.priority = de_group_prio;
    de_group.cycle = bcmFieldGroupCycle1;

    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES |
      BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
    data_qual.offset = 0;
    data_qual.qualifier = bcmFieldQualifyInVPort;
    data_qual.length = 16;
    data_qual.stage = bcmFieldStageIngress;
    rv = bcm_field_data_qualifier_create(unit, &data_qual);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_data_qualifier_create\n");
        return rv;
    }

    /* QSET */
    BCM_FIELD_QSET_INIT(de_group.qset);
    /* Set cascaded field size */
    rv = bcm_field_control_set(unit, bcmFieldControlCascadedKeyWidth, Cint_field_large_de_same_qual_cascade_key_size);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_control_set\n");
        return rv;
    }
    BCM_FIELD_QSET_ADD(de_group.qset, bcmFieldQualifyCascadedKeyValue);
    rv = bcm_field_qset_data_qualifier_add(unit, &de_group.qset, data_qual.qual_id);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_qset_data_qualifier_add\n");
        return rv;
    }
    BCM_FIELD_QSET_ADD(de_group.qset, bcmFieldQualifyInnerVlanId);

    /* ASET */
    BCM_FIELD_ASET_INIT(de_group.aset);
    BCM_FIELD_ASET_ADD(de_group.aset, bcmFieldActionStat);
    BCM_FIELD_ASET_ADD(de_group.aset, bcmFieldActionStat1);

    rv = bcm_field_group_config_create(unit, &de_group);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_group_config_create\n");
        return rv;
    }

    printf("DE Field Group ID %d was created. \n", de_group_id);

    bcm_field_group_config_t_init(&tcam_group);
    tcam_group.group = tcam_group_id;
    tcam_group.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID |
                  BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_CYCLE;
    tcam_group.mode = bcmFieldGroupModeAuto;
    tcam_group.priority = tcam_group_prio;
    tcam_group.cycle = bcmFieldGroupCycle0;

    /* QSET  */
    BCM_FIELD_QSET_INIT(tcam_group.qset);
    BCM_FIELD_QSET_ADD(tcam_group.qset, bcmFieldQualifyEtherType);

    /* ASET  */
    BCM_FIELD_ASET_INIT(tcam_group.aset);
    BCM_FIELD_ASET_ADD(tcam_group.aset, bcmFieldActionCascadedKeyValueSet);

    rv = bcm_field_group_config_create(unit, &tcam_group);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_group_config_create\n");
        return rv;
    }

    printf("TCAM Field Group ID %d was created. \n", tcam_group_id);

    /* Entry create. */
    rv = bcm_field_entry_create(unit, de_group_id, &de_entry_handle);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_entry_create\n");
        return rv;
    }

    bcm_field_extraction_action_t_init(&extraction_action);
    bcm_field_extraction_field_t_init(&extraction_fields[0]);
    bcm_field_extraction_field_t_init(&extraction_fields[1]);
    bcm_field_extraction_field_t_init(&extraction_fields[2]);

    extraction_action.action =  bcmFieldActionStat;
    extraction_action.bias = 0;

    /** First extraction is extracting 4b of CascadedKeyValue qualifier. */
    extraction_fields[0].flags = 0;
    extraction_fields[0].lsb = 0;
    extraction_fields[0].bits = Cint_field_large_de_same_qual_cascade_key_size;
    extraction_fields[0].qualifier = bcmFieldQualifyCascadedKeyValue;
    /** Second extraction is extracting 16b of InVPort qualifier [0:15]. */
    extraction_fields[1].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    extraction_fields[1].lsb = 0;
    extraction_fields[1].bits = 16;
    extraction_fields[1].qualifier = data_qual.qual_id;
    /** Third extraction is filling the rest of the Stat1 action with 2b zeros. */
    extraction_fields[2].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    extraction_fields[2].bits = 2;
    extraction_fields[2].value = 0;
    extraction_fields[2].qualifier = bcmFieldQualifyConstantZero;

    rv = bcm_field_direct_extraction_action_add(unit, de_entry_handle, extraction_action, 3, &extraction_fields);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return rv;
    }

    bcm_field_extraction_action_t_init(&extraction_action);
    bcm_field_extraction_field_t_init(&extraction_fields[0]);
    bcm_field_extraction_field_t_init(&extraction_fields[1]);
    bcm_field_extraction_field_t_init(&extraction_fields[2]);

    extraction_action.action =  bcmFieldActionStat1;
    extraction_action.bias = 0;

    /** First extraction is extracting 4b of CascadedKeyValue qualifier. */
    extraction_fields[0].flags = 0;
    extraction_fields[0].lsb = 0;
    extraction_fields[0].bits = Cint_field_large_de_same_qual_cascade_key_size;
    extraction_fields[0].qualifier = bcmFieldQualifyCascadedKeyValue;
    /** Second extraction is extracting 12b of InnerVlanId qualifier. */
    extraction_fields[1].flags = 0;
    extraction_fields[1].lsb = 0;
    extraction_fields[1].bits = 12;
    extraction_fields[1].qualifier = bcmFieldQualifyInnerVlanId;
    /** Third extraction is filling the rest of the Stat1 action with 6b zeros. */
    extraction_fields[2].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    extraction_fields[2].bits = 6;
    extraction_fields[2].value = 0;
    extraction_fields[2].qualifier = bcmFieldQualifyConstantZero;

    rv = bcm_field_direct_extraction_action_add(unit, de_entry_handle, extraction_action, 3, &extraction_fields);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return rv;
    }

    /* Write entry to HW */
    rv = bcm_field_entry_install(unit, de_entry_handle);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_entry_install\n");
        return rv;
    }

    printf("DE Entry ID 0x%X (%d) was added to Field Group ID %d. \n", de_entry_handle, de_entry_handle, de_group_id);

    /* Entry create. */
    rv = bcm_field_entry_create(unit, tcam_group_id, &tcam_entry_handle);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_entry_create\n");
        return rv;
    }

    rv = bcm_field_qualify_EtherType(unit, tcam_entry_handle, ether_type, 0xFFFF);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_qualify_OuterVlanId\n");
        return rv;
    }

    /* Action add  */
    rv = bcm_field_action_add(unit, tcam_entry_handle, bcmFieldActionCascadedKeyValueSet, Cint_field_large_de_same_qual_cascade_key_value, 0);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_action_add 'bcmFieldActionCascadedKeyValueSet'\n");
        return rv;
    }

    /* Install  */
    rv = bcm_field_group_install(unit, tcam_group_id);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_group_install\n");
        return rv;
    }

    printf("TCAM Entry ID 0x%X (%d) was added to Field Group ID %d. \n", tcam_entry_handle, tcam_entry_handle, tcam_group_id);

    return rv;
}

/**
 * \brief
 *  Destroys all allocated data by the configuration.
 *
 * \param [in] unit                 - Device id
 * \param [in] tcam_group_id   - TCAM FG Id
 * \param [in] de_group_id   - Direct Extraction FG Id.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_large_de_same_qual_cascade_destroy(
    int unit,
    bcm_field_group_t tcam_group_id,
    bcm_field_group_t de_group_id)
{
    int rv = BCM_E_NONE;

    rv = bcm_field_group_flush(unit, tcam_group_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_flush returned %d \n", rv);
        return rv;
    }

    rv = bcm_field_group_flush(unit, de_group_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_flush returned %d \n", rv);
        return rv;
    }

    return rv;
}
