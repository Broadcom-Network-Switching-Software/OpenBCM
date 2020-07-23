/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 *
 *  In order to use any of the functions of this cint, you should load following files:
 *
 *  cint ../../src/examples/dpp/cint_field_entry_hit.c
 *
 *  Test scenario:
 *      - Use cint_field_entry_hit_group_config_main(unit) to:
 *          Configure a simple TCAM field group in Ingress stage to qualify upon ETH type and to perform
 *          In Ttl action.
 *
 *      - Use cint_field_entry_hit_verify(unit, entry_hit_verify):
 *          Compares the given 'entry_hit_verify' to the returned result
 *          from the bcm_field_entry_hit_get().
 *
 *      - Use cint_field_entry_hit_destroy(unit):
 *          Destroys all configuration done by above functions.
 */

bcm_field_entry_t cint_field_entry_hit_entry_handle;

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
cint_field_entry_hit_group_config_main(
    int unit,
    int group_id,
    int group_prio,
    int eth_type)
{
    int rv = BCM_E_NONE;
    bcm_field_group_config_t group;

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
    BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionTtlSet);

    rv = bcm_field_group_config_create(unit, &group);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_group_config_create\n");
        return rv;
    }

    printf("Field Group ID %d was created. \n", group_id);

    /* entry */
    rv = bcm_field_entry_create(unit, group_id, &cint_field_entry_hit_entry_handle);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_entry_create\n");
        return rv;
    }

    rv = bcm_field_qualify_EtherType(unit, cint_field_entry_hit_entry_handle, eth_type, -1);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_qualify_InPort\n");
        return rv;
    }

    /* action add  */
    rv = bcm_field_action_add(unit, cint_field_entry_hit_entry_handle, bcmFieldActionTtlSet, 6, 0);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_action_add\n");
        return rv;
    }

    /* install  */
    rv = bcm_field_group_install(unit, group_id);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_group_install\n");
        return rv;
    }

    printf("Entry ID 0x%X (%d) was added to Field Group ID %d. \n", cint_field_entry_hit_entry_handle,
           cint_field_entry_hit_entry_handle, group_id);

    return rv;
}

/**
 * \brief
 *  Function to be used for verifying the entry hit results.
 *
 * \param [in] unit                 - Device id
 * \param [in] entry_hit_verify  -
 *  Verify value, which will be compared with the returned
 *  value from the bcm_field_entry_hit_get() and
 *  bcm_field_entry_hit_flush() APIs.
 *  Possible values to be verified:
 *      - '0' - in case we want to verify the FLUSH API, we expect 0
 *      to be returned by the GET API.
 *      - '1' - in case we want to verify that the HIT indication API.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_entry_hit_verify(
    int unit,
    uint8 entry_hit_verify)
{
    int rv = BCM_E_NONE;
    uint8 entry_hit;

    rv = bcm_field_entry_hit_get(unit, 0, cint_field_entry_hit_entry_handle, &entry_hit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_hit_get\n", rv);
        return rv;
    }

    if (entry_hit == entry_hit_verify && entry_hit_verify != 0)
    {
        printf("Entry Hit Get was successful for entry ID 0x%X (%d). \n", cint_field_entry_hit_entry_handle,
               cint_field_entry_hit_entry_handle);
    }
    else if (entry_hit == 0 && entry_hit_verify == 0)
    {
        printf("Entry Hit Flush was successful for entry ID 0x%X (%d). \n", cint_field_entry_hit_entry_handle,
               cint_field_entry_hit_entry_handle);
    }
    else if (entry_hit != 0 && entry_hit_verify == 0)
    {
        printf("Entry Hit Flush failed for entry ID 0x%X (%d). Expected value %d | Retrieved value %d \n",
               cint_field_entry_hit_entry_handle, cint_field_entry_hit_entry_handle, entry_hit_verify,
               entry_hit);
        rv = BCM_E_INTERNAL;
    }
    else
    {
        printf("Entry Hit Get failed for entry ID 0x%X (%d). Expected value %d | Retrieved value %d \n",
               cint_field_entry_hit_entry_handle, cint_field_entry_hit_entry_handle, entry_hit_verify,
               entry_hit);
        rv = BCM_E_INTERNAL;
    }

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
cint_field_entry_hit_destroy(
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
