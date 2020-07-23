/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 *
 *  In order to use any of the functions of this cint, you should load following files:
 *
 *  cint ../../src/examples/dpp/cint_field_pph_learn_extension_disable.c
 *
 *  Test scenario:
 *      - Use cint_field_pph_learn_extension_disable_group_config_main(unit) to:
 *          Configure a simple TCAM field group in Ingress stage to qualify upon ETH type and to perform
 *          FabricHeaderSet action, which will disable PPH Learn extension (bcmFieldFabricHeaderEthernetLearnDisable).
 *
 *      - Use cint_field_pph_learn_extension_disable_verify(unit, entry_hit_verify):
 *          Verifies that Learn_Extension_Valid was updated.
 *
 *      - Use cint_field_pph_learn_extension_disable_destroy(unit):
 *          Destroys all configuration done by above functions.
 */

/**
 * \brief
 *  This function creates field group, with all relevant
 *  information to it and adds entry to it.
 *
 * \param [in] unit   - Device id
 * \param [in] group_id   - Group Id
 * \param [in] group_prio   - Group priority
 * \param [in] eth_type   - Ethertype to qualify upon.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_pph_learn_extension_disable_group_config_main(
    int unit,
    int group_id,
    int group_prio,
    int eth_type)
{
    int rv = BCM_E_NONE;
    bcm_field_group_config_t group;
    bcm_field_entry_t entry_handle;

    bcm_field_group_config_t_init(&group);
    group.group = group_id;
    group.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_ASET;
    group.mode = bcmFieldGroupModeAuto;
    group.priority = group_prio;

    /* QSET  */
    BCM_FIELD_QSET_INIT(group.qset);
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyEtherType);

    /* ASET  */
    BCM_FIELD_ASET_INIT(group.aset);
    BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionFabricHeaderSet);

    rv = bcm_field_group_config_create(unit, &group);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_group_config_create\n");
        return rv;
    }

    printf("Field Group ID %d was created. \n", group_id);

    /* Entry create. */
    rv = bcm_field_entry_create(unit, group_id, &entry_handle);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_entry_create\n");
        return rv;
    }

    rv = bcm_field_qualify_EtherType(unit, entry_handle, eth_type, -1);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_qualify_InPort\n");
        return rv;
    }

    /* Action add  */
    rv = bcm_field_action_add(unit, entry_handle, bcmFieldActionFabricHeaderSet, bcmFieldFabricHeaderEthernetLearnDisable, 0);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_action_add\n");
        return rv;
    }

    /* Install  */
    rv = bcm_field_group_install(unit, group_id);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_group_install\n");
        return rv;
    }

    printf("Entry ID 0x%X (%d) was added to Field Group ID %d. \n", entry_handle, entry_handle, group_id);

    return rv;
}

/**
 * \brief
 *  Function to verify that Learn_Extension_Valid was updated.
 *
 * \param [in] unit  - Device id.
 * \param [in] core  - Core of the device.
 * \param [in] learn_extension_valid_expected  - Expected 'Learn_Extension_Valid' value.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_pph_learn_extension_disable_verify(
    int unit,
    int core,
    int learn_extension_valid_expected)
{
    int rv = BCM_E_NONE;
    uint32 value;

    rv = dpp_dsig_read(unit, core, "ERPP", NULL, NULL, "Learn_Extension_Valid", &value, 1);
    if (rv != BCM_E_NONE) {
        printf("Error in dpp_dsig_read returned %d \n", rv);
        return rv;
    }
    printf("Learn_Extension_Valid : 0x%x \n", value);
    if (value != learn_extension_valid_expected) {
        printf("Verify FAIL: Learn_Extension_Valid is expected 0x%x but received 0x%x!\n", learn_extension_valid_expected, value);
       return BCM_E_FAIL;
    }
    printf("Verify Done. Verified that new value 0x%x for Learn_Extension_Valid was set.\n", value);

    return rv;
}

/**
 * \brief
 *  Destroys all allocated data by the configuration.
 *
 * \param [in] unit                 - Device id
 * \param [in] group_id             - Group id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_pph_learn_extension_disable_destroy(
    int unit,
    int group_id)
{
    int rv = BCM_E_NONE;

    rv = bcm_field_group_flush(unit, group_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_flush returned %d \n", rv);
        return rv;
    }

    return rv;
}
