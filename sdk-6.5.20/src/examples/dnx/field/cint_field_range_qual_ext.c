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
 * cint ../../../src/examples/dnx/field/cint_field_range_qual_ext.c
 * cint;
 * int unit = 0;
 * cint_field_range_qual_ext_main(unit);
 *
 * Configuration example end
 *
 * This CINT shows how to set a range qualifier extension, and use it as a qualifier.
 *
 * Configuration steps:
 * 1. Create an user defined range qualifier using bcm_field_range_type_config_set for InPort.
 * 1. Set a range using bcm_field_range_set for needed range Types (user defined, ttl and packet size).
 * 2. Add a field group with qualifier bcmFieldQualifyRangeFirstHit0-3 for configured encoders.
 * 3. Attach the field group to a context.
 * 4. Add an entry to the FG.
 */

 /**
  * Global variables
  */
bcm_field_context_t cint_field_range_qual_ext_ctx_id = BCM_FIELD_CONTEXT_ID_DEFAULT;
bcm_field_qualify_t cint_field_range_qual_ext_qualifier;
bcm_field_group_t cint_field_range_qual_ext_fg_id;
bcm_field_entry_t cint_field_range_qual_ext_entry_id;
uint32 cint_field_range_qual_ext_tc_action_value = 0x2;
int cint_field_range_qual_ext_qual_id;



/**
* \brief
*  This function runs the external main function
* \param [in] unit           - Device ID.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_range_qual_ext_main(
       int unit)
{
   int rv = BCM_E_NONE;

   /**
    * Init function
    */
   rv = cint_field_range_qual_ext_init(unit);
   if (rv != BCM_E_NONE)
   {
       printf("Error (%d), in cint_field_range_qual_ext_init\n", rv);
       return rv;
   }
   /**
    * Set ranges
    */
   rv = cint_field_range_qual_ext_create_range(unit);
   if (rv != BCM_E_NONE)
   {
       printf("Error (%d), in cint_field_range_qual_ext_create_range\n", rv);
       return rv;
   }

   /**
    * Create field group
    */
   rv = cint_field_range_qual_ext_add_fg(unit);
   if (rv != BCM_E_NONE)
   {
       printf("Error (%d), in cint_field_range_qual_ext_add_fg\n", rv);
       return rv;
   }

   return rv;
}

/**
* \brief
*  This function checks if L4 ops extended is enabled for current device
* \param [in] unit           - Device ID.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_range_qual_ext_init(
       int unit)
{
    int extended_l4_ops_enabled = *(dnxc_data_get(unit, "field", "features", "extended_l4_ops", NULL));

    if (!extended_l4_ops_enabled)
    {
        printf("Error, L4 ops extended isn't enabled for this device\n");
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
 }

/**
* \brief
*  This function runs the external main function
* \param [in] unit           - Device ID.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_range_qual_ext_create_range(
       int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_range_info_t range_info;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;
    bcm_field_range_qual_info_t range_qual_info;

    /*
     * Configure user defined range qualifier
     */
    range_qual_info.qual_type = bcmFieldQualifyInPort;
    range_qual_info.qual_info.input_type = bcmFieldInputTypeMetaData;
    bcm_field_range_type_config_set(unit, 0, bcmFieldStageIngressPMF1, 0, 0, &range_qual_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_range_type_config_set\n", rv);
        return rv;
    }

    /*
     * Set operator
     */
    rv = bcm_switch_control_set(unit, bcmSwitchRangeOperator, bcmSwitchRangeOperatorNone);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_switch_control_set\n", rv);
        return rv;
    }

    /*
     * Configure which range types should be checked
     */
    key.type = bcmSwitchRangeTypeSelect;
    key.index = 0;
    value.value = bcmFieldRangeTypeUserDefined1Low;
    rv = bcm_switch_control_indexed_set(unit, key, value);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_switch_control_indexed_set\n", rv);
        return rv;
    }

    key.type = bcmSwitchRangeTypeSelect;
    key.index = 1;
    value.value = bcmFieldRangeTypeInTTL;
    rv = bcm_switch_control_indexed_set(unit, key, value);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_switch_control_indexed_set\n", rv);
        return rv;
    }

    key.type = bcmSwitchRangeTypeSelect;
    key.index = 2;
    value.value = bcmFieldRangeTypeL4OpsPacketHeaderSize;
    rv = bcm_switch_control_indexed_set(unit, key, value);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_switch_control_indexed_set\n", rv);
        return rv;
    }

    key.type = bcmSwitchRangeTypeSelect;
    key.index = 0;
    value.value = bcmFieldRangeTypeUserDefined1Low;
    rv = bcm_switch_control_indexed_set(unit, key, value);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_switch_control_indexed_set\n", rv);
        return rv;
    }

    /*
     * Select encoders to configure how to build the result
     */
    key.type = bcmSwitchRangeResultMap;
    key.index = 0;
    value.value = bcmSwitchRangeResultMapFirstOnly;
    rv = bcm_switch_control_indexed_set(unit, key, value);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_switch_control_indexed_set\n", rv);
        return rv;
    }

    key.type = bcmSwitchRangeResultMap;
    key.index = 1;
    value.value = bcmSwitchRangeResultMapSecondOnly;
    rv = bcm_switch_control_indexed_set(unit, key, value);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_switch_control_indexed_set\n", rv);
        return rv;
    }

    key.type = bcmSwitchRangeResultMap;
    key.index = 2;
    value.value = bcmSwitchRangeResultMapFirstnSecondnThirdnForth;
    rv = bcm_switch_control_indexed_set(unit, key, value);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_switch_control_indexed_set\n", rv);
        return rv;
    }

    /*
     * Set the ranges
     */
    bcm_field_range_info_t_init(&range_info);
    range_info.range_type = bcmFieldRangeTypeUserDefined1Low;
    range_info.min_val = 0;
    range_info.max_val = 10000;
    rv = bcm_field_range_set(unit, 0, bcmFieldStageIngressPMF1, 0, &range_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_range_set\n", rv);
        return rv;
    }

    bcm_field_range_info_t_init(&range_info);
    range_info.range_type = bcmFieldRangeTypeInTTL;
    range_info.min_val = 0;
    range_info.max_val = 100;
    rv = bcm_field_range_set(unit, 0, bcmFieldStageIngressPMF1, 0, &range_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_range_set\n", rv);
        return rv;
    }
    range_info.min_val = 100;
    range_info.max_val = 200;
    rv = bcm_field_range_set(unit, 0, bcmFieldStageIngressPMF1, 1, &range_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_range_set\n", rv);
        return rv;
    }

    bcm_field_range_info_t_init(&range_info);
    range_info.range_type = bcmFieldRangeTypeL4OpsPacketHeaderSize;
    range_info.min_val = 90;
    range_info.max_val = 260;
    rv = bcm_field_range_set(unit, 0, bcmFieldStageIngressPMF1, 0, &range_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_range_set\n", rv);
        return rv;
    }

    bcm_field_range_info_t_init(&range_info);
    range_info.range_type = bcmFieldRangeTypeUserDefined1Low;
    range_info.min_val = 0;
    range_info.max_val = 10000;
    rv = bcm_field_range_set(unit, 0, bcmFieldStageIngressPMF1, 0, &range_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_range_set\n", rv);
        return rv;
    }
    return rv;
 }

/**
* \brief
*  This function creates field group in IPMF1, attaches it, and adds entry
* \param [in] unit           - Device ID.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_range_qual_ext_add_fg(
       int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    void *dest_char;

    bcm_field_group_info_t_init(&fg_info);
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "RANGE", sizeof(fg_info.name));
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_quals = 4;

    /*
     * Configured encoders is being build in a key using these qualifiers
     */
    fg_info.qual_types[0] = bcmFieldQualifyRangeFirstHit0;
    fg_info.qual_types[1] = bcmFieldQualifyRangeFirstHit1;
    fg_info.qual_types[2] = bcmFieldQualifyRangeFirstHit2;
    fg_info.qual_types[3] = bcmFieldQualifyRangeFirstHit3;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionPrioIntNew;
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_range_qual_ext_fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Field Group %d was created. \n", cint_field_range_qual_ext_fg_id);

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info.key_info.qual_types[3] = fg_info.qual_types[3];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeMetaData;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    rv = bcm_field_group_context_attach(unit, 0, cint_field_range_qual_ext_fg_id, cint_field_range_qual_ext_ctx_id, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Field Group %d was attached to context %d. \n", cint_field_range_qual_ext_fg_id, cint_field_range_qual_ext_ctx_id);

    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[1].type = fg_info.qual_types[1];
    entry_info.entry_qual[2].type = fg_info.qual_types[2];
    entry_info.entry_qual[3].type = fg_info.qual_types[3];
    /*
     * entry values consists of 1 valid bit (indicates if at least one hit occurred inside the subset of chosen ranges)
     * and the rest bits represents the index of the relevant range.
     * The valid 1b offset is dynamically allocated according to the number of chosen range types in the subset.
     * (offset is 6 when 1 type is selected, 7 when 2-3 types are selected, and 8 when 4 types are selected)
     */
    entry_info.entry_qual[0].value[0] = 0x20;
    entry_info.entry_qual[1].value[0] = 0x21;
    entry_info.entry_qual[2].value[0] = 0x80;
    entry_info.entry_qual[3].value[0] = 0;
    entry_info.entry_qual[0].mask[0] = 0xFF;
    entry_info.entry_qual[1].mask[0] = 0xFF;
    entry_info.entry_qual[2].mask[0] = 0xFF;
    entry_info.entry_qual[3].mask[0] = 0;

    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = cint_field_range_qual_ext_tc_action_value;

    rv = bcm_field_entry_add(unit, 0, cint_field_range_qual_ext_fg_id, &entry_info, cint_field_range_qual_ext_entry_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("Entry 0x%X (%d) was added to field group %d. \n", cint_field_range_qual_ext_entry_id, cint_field_range_qual_ext_entry_id, cint_field_range_qual_ext_fg_id);

    return rv;
}

/**
 * \brief
 *  This function destroys all allocated data by the configuration
 *
 * \param [in] unit            - Device id
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_range_qual_ext_destroy(
        int unit)
{
    int rv = BCM_E_NONE;
    /*
    * Delete the entries from the field group
    */
    rv = bcm_field_entry_delete(unit, cint_field_range_qual_ext_fg_id, NULL, cint_field_range_qual_ext_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }
    printf("Entry 0x%X (%d) was deleted. \n", cint_field_range_qual_ext_entry_id, cint_field_range_qual_ext_entry_id);

    /*
     * Detach the FG from its context
     */
    rv = bcm_field_group_context_detach(unit, cint_field_range_qual_ext_fg_id, cint_field_range_qual_ext_ctx_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }
    printf("Field Group %d was detach from context %d. \n", cint_field_range_qual_ext_fg_id, cint_field_range_qual_ext_ctx_id);

    /*
     * Delete the FG
     */
    rv = bcm_field_group_delete(unit, cint_field_range_qual_ext_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_delete\n");
        return rv;
    }
    printf("Field Group %d was deleted. \n", cint_field_range_qual_ext_fg_id);

    return rv;
}
