/*
 * Configuration example start:
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/field/cint_field_tcam_bank_per_core.c
  cint;
  int unit = 0;
  bcm_field_context_t context_id = 0;
  cint_field_tcam_bank_per_core_main(unit, context_id);
  cint_field_tcam_bank_per_core_add_tcam_bank(unit);
  cint_field_tcam_bank_per_core_destroy(unit, context_id);
 * Test Scenario - end
 *
 * Configuration example end
 *
 *   This CINT shows how to allocate TCAM banks per core for TCAM field group.
 *
 */


 /**
  * Global variables
  */
bcm_field_group_t cint_field_tcam_bank_per_core_fg_id = BCM_FIELD_ID_INVALID;
bcm_field_entry_t cint_field_tcam_bank_per_core_entry_id0 = BCM_FIELD_ID_INVALID;
bcm_field_entry_t cint_field_tcam_bank_per_core_entry_id1 = BCM_FIELD_ID_INVALID;
bcm_field_qualify_t cint_field_tcam_bank_per_core_qual = bcmFieldQualifyIp4Ttl;
bcm_field_action_t cint_field_tcam_bank_per_core_action = bcmFieldActionPrioIntNew;

int cint_field_tcam_bank_per_core_ttl = 7;
int cint_field_tcam_bank_per_core_tc_core0 = 5;
int cint_field_tcam_bank_per_core_tc_core1 = 4;

/**
* \brief
* This function creates TCAM field group in IPMF1 with TCAM bank in core 0, and adds entry to core 0.
* \param [in] unit           - Device ID.
* \param [in] context_id     - Context ID.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_tcam_bank_per_core_main(
       int unit,
       int context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    void *dest_char;

    bcm_field_group_info_t_init(&fg_info);
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "TCAM_banks_per_core", sizeof(fg_info.name));
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    fg_info.nof_quals = 1;
    /* Set quals */
    fg_info.qual_types[0] = cint_field_tcam_bank_per_core_qual;
    fg_info.nof_actions = 1;
    /* Set actions */
    fg_info.action_types[0] = cint_field_tcam_bank_per_core_action;
    /* Enable TCAM bank allocation Select mode */
    fg_info.tcam_info.bank_allocation_mode = bcmFieldTcamBankAllocationModeSelect;
    /* Allocate bank_id 8 in core 0 */
    fg_info.tcam_info.nof_tcam_banks = 1;
    fg_info.tcam_info.tcam_bank_ids[0] = 8;
    fg_info.tcam_info.core_ids[0] = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &fg_info, &cint_field_tcam_bank_per_core_fg_id), "");
    printf("Field Group %d was created. \n", cint_field_tcam_bank_per_core_fg_id);

    /* attach the field group to the given context */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, cint_field_tcam_bank_per_core_fg_id, context_id, &attach_info), "");
    printf("Field Group %d was attached to context %d. \n", cint_field_tcam_bank_per_core_fg_id, context_id);

    /* Add entry to core 0 */
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = 1;
    entry_info.nof_entry_actions = 1;
    entry_info.entry_qual[0].type = cint_field_tcam_bank_per_core_qual;
    entry_info.entry_qual[0].value[0] = cint_field_tcam_bank_per_core_ttl;
    entry_info.entry_qual[0].mask[0] = 0xff;
    entry_info.entry_action[0].type = cint_field_tcam_bank_per_core_action;
    entry_info.entry_action[0].value[0] = cint_field_tcam_bank_per_core_tc_core0;
    entry_info.core = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, cint_field_tcam_bank_per_core_fg_id, &entry_info, &cint_field_tcam_bank_per_core_entry_id0), "");
    printf("Entry 0x%X (%d) was added to field group %d cache. \n", cint_field_tcam_bank_per_core_entry_id0, cint_field_tcam_bank_per_core_entry_id0, cint_field_tcam_bank_per_core_fg_id);

    return BCM_E_NONE;
}

/**
* \brief
*  This function adds TCAM bank in core 1, and adds entry to core 1.
* \param [in] unit   - Device ID.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_tcam_bank_per_core_add_tcam_bank(
       int unit)
{
    bcm_field_entry_info_t entry_info;
    bcm_field_tcam_bank_info_t tcam_bank_info;

    /* Add TCAM bank to core 1 */
    bcm_field_tcam_bank_info_t_init(&tcam_bank_info);
    tcam_bank_info.fg_id = cint_field_tcam_bank_per_core_fg_id;
    tcam_bank_info.nof_banks = 1;
    tcam_bank_info.tcam_bank_ids[0] = 5;
    tcam_bank_info.core_ids[0] = 1;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_tcam_bank_add(unit, 0, &tcam_bank_info), "");

    /* Add entry the core 1 */
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = 1;
    entry_info.nof_entry_actions = 1;
    entry_info.entry_qual[0].type = cint_field_tcam_bank_per_core_qual;
    entry_info.entry_qual[0].value[0] = cint_field_tcam_bank_per_core_ttl;
    entry_info.entry_qual[0].mask[0] = 0xff;
    entry_info.entry_action[0].type = cint_field_tcam_bank_per_core_action;
    entry_info.entry_action[0].value[0] = cint_field_tcam_bank_per_core_tc_core1;
    entry_info.core = 1;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, cint_field_tcam_bank_per_core_fg_id, &entry_info, &cint_field_tcam_bank_per_core_entry_id1), "");
    printf("Entry 0x%X (%d) was added to field group %d cache. \n", cint_field_tcam_bank_per_core_entry_id1, cint_field_tcam_bank_per_core_entry_id1, cint_field_tcam_bank_per_core_fg_id);

    return BCM_E_NONE;
}

/**
 * \brief
 *  This function destroys all allocated data by the configuration
 *
 * \param [in] unit            - Device id
 * \param [in] context_id          - Context ID
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_tcam_bank_per_core_destroy(
        int unit,
        int context_id)
{

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, cint_field_tcam_bank_per_core_fg_id, NULL, cint_field_tcam_bank_per_core_entry_id0), "");
    printf("Entry 0x%X (%d) was deleted from field group. \n", cint_field_tcam_bank_per_core_entry_id0, cint_field_tcam_bank_per_core_entry_id0, cint_field_tcam_bank_per_core_fg_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, cint_field_tcam_bank_per_core_fg_id, NULL, cint_field_tcam_bank_per_core_entry_id1), "");
    printf("Entry 0x%X (%d) was deleted from field group. \n", cint_field_tcam_bank_per_core_entry_id1, cint_field_tcam_bank_per_core_entry_id1, cint_field_tcam_bank_per_core_fg_id);

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, cint_field_tcam_bank_per_core_fg_id, context_id), "");
    printf("Field Group %d was detach from context %d. \n", cint_field_tcam_bank_per_core_fg_id, context_id);

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, cint_field_tcam_bank_per_core_fg_id), "");
    printf("Field Group %d was deleted. \n", cint_field_tcam_bank_per_core_fg_id);

    return BCM_E_NONE;
}

