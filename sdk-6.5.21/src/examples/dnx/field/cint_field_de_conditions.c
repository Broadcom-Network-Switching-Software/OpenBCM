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
 * cint ../../../src/examples/dnx/field/cint_field_de_conditions.c
 * cint;
 * int unit = 0;
 * cint_field_de_conditions_compare_and_fem_example(unit);
 * cint_field_de_conditions_range_and_efes_example(unit);
 *
 * Configuration example end
 *
 *  This CINT shows examples of complex conditions to be used by Direct Extraction field group.
 *
 *  Example 1: cint_field_de_conditions_compare_and_fem_example(unit)
 *      For IPv4oETH packet if (200 < vlanId < 800) increase the value of TTL by 5 and cascade it to bytes_to_remove, otherwise cascade the value 127 to bytes_to_remove,
 *      using context compare, FEM action (2 condition bits out of 4) and cascading.
 *
 *
 *  Example 2: cint_field_de_conditions_range_and_efes_example(unit)
 *      Using range and EFES action, this example is for TCP packets:
 *          if (1000 < source port < 2000): if TTL < 64        => double TTL
 *                                          if 64 <= TTL < 128 => set TTL to 64
 *                                          if TTL >= 128      => set TTL to 128
 *          if (2000 < source port < 3000): if TTL < 64        => double TTL and add 1
 *                                          if 64 <= TTL < 128 => set TTL to 65
 *                                          if TTL >= 128      => set TTL to 129
 *          Using 2 bits condition of EFES (for TTL) and 1 valid bit for range id
 *
 *   Destroy function:
 *   cint_field_de_conditions_compare_and_fem_destroy(unit);
 *   cint_field_de_conditions_range_and_efes_destroy(unit);
 */

 /* Global variables */
bcm_field_presel_t cint_field_de_conditions_presel_id = 50;
bcm_field_group_t cint_field_de_conditions_fg_id_ipmf2;
bcm_field_group_t cint_field_de_conditions_fg_id_ipmf3;
bcm_field_context_t  cint_field_de_conditions_context_id_ipmf1;
bcm_field_context_t  cint_field_de_conditions_context_id_ipmf3;
bcm_field_stage_t cint_field_de_conditions_stage = bcmFieldStageIngressPMF2;

/*
 * Example 1
 */
bcm_field_action_t cint_field_de_conditions_void_ttl_uda;
bcm_field_action_t cint_field_de_conditions_void_ranges_uda;
bcm_field_action_t cint_field_de_conditions_padding_uda;
uint32 cint_field_de_conditions_fem_increment_value = 5;
bcm_field_qualify_t cint_field_de_conditions_smaller_than_udq;
bcm_field_qualify_t cint_field_de_conditions_bigger_than_udq;
bcm_field_qualify_t cint_field_de_conditions_vid_const_qual;
int cint_field_de_conditions_first_pair_id = 0;
int cint_field_de_conditions_second_pair_id = 1;

/**
 * Example 2
 */
bcm_field_action_t cint_field_de_conditions_double_uda[2];
bcm_field_action_t cint_field_de_conditions_const_64_uda[2];
bcm_field_action_t cint_field_de_conditions_const_128_uda[2];
bcm_field_action_t cint_field_de_conditions_condition_uda;
bcm_field_action_t cint_field_de_conditions_valid_bit_1_uda[2];
bcm_field_action_t cint_field_de_conditions_valid_bit_2_uda[2];
bcm_field_action_t cint_field_de_conditions_valid_bit_3_uda[2];

bcm_field_qualify_t cint_field_de_conditions_double_udq[2];
bcm_field_qualify_t cint_field_de_conditions_const_64_udq[2];
bcm_field_qualify_t cint_field_de_conditions_const_128_udq[2];
bcm_field_qualify_t cint_field_de_conditions_condition_udq;
bcm_field_qualify_t cint_field_de_conditions_valid_bit_1_udq[2];
bcm_field_qualify_t cint_field_de_conditions_valid_bit_2_udq[2];
bcm_field_qualify_t cint_field_de_conditions_valid_bit_3_udq[2];
bcm_field_qualify_t cint_field_de_conditions_action_udq[2];
/* User-provided identifier of the range */
int cint_field_de_conditions_range_id_0 = 0;
int cint_field_de_conditions_range_id_1 = 1;

/**
 * \brief
 *  This function creates a context with compare mode enabled
 *
 * \param [in]  unit            - Device id
 * \param [in]  compare_mode    - Whether to enable compare mode
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_de_conditions_context_config(
    int unit,
    int compare_mode)
{
    int rv = BCM_E_NONE;
    bcm_field_context_info_t context_info;
    bcm_field_context_compare_info_t compare_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    void * dest_char;

    bcm_field_context_info_t_init(&context_info);
    if (compare_mode) {
        /**
        * Set the compare mode to be bcmFieldContextCompareTypeDouble for using both key comparison
        */
        context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeDouble;
        context_info.context_compare_modes.compare_2_mode = bcmFieldContextCompareTypeDouble;
    }
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "de_condition_context", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &cint_field_de_conditions_context_id_ipmf1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_de_conditions_presel_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_de_conditions_context_id_ipmf1;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeEth;
    p_data.qual_data[0].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set \n", rv);
        return rv;
    }

    printf("Presel (%d) was configured for context(%d) \n", cint_field_de_conditions_presel_id, cint_field_de_conditions_context_id_ipmf1);

    return rv;
}

/**
 * \brief
 *  This function runs the first example main function:
 *      1) Create compare context and user defined actions and qualifiers.
 *      2) Create DE field group in IPMF2 and configure FEM action.
 *      3) Create cascading context and DE field group in IPMF3.
 *
 * \param [in]  unit            - Device id
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_de_conditions_compare_and_fem_example(
        int unit)
{
    int rv = BCM_E_NONE;

    /*
     * Config context with compare mode
     */
    rv = cint_field_de_conditions_context_config(unit, TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_de_conditions_context_config\n", rv);
         return rv;
    }
    /*
     * Config user defined actions and qualifiers
     */
    rv = cint_field_de_conditions_compare_and_fem_create_uda_udq(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_de_conditions_compare_and_fem_create_uda_udq\n", rv);
         return rv;
    }
    /*
     * Config context compare parameters
     */
    rv = cint_field_de_conditions_compare_and_fem_comapre_config(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_de_conditions_compare_and_fem_comapre_config\n", rv);
         return rv;
    }
    /*
     * Config field group and FEM action in IPMF2
     */
    rv = cint_field_de_conditions_compare_and_fem_fg_config(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_de_conditions_compare_and_fem_fg_config\n", rv);
         return rv;
    }
    /*
     * Config field group in IPMF3
     */
    rv = cint_field_de_conditions_compare_and_fem_cascading_fg_config(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_de_conditions_compare_and_fem_cascading_fg_config\n", rv);
        return rv;
    }
    return rv;
}

/**
 * \brief
 *  This function sets user defined actions and qualifiers
 *
 * \param [in]  unit            - Device id
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_de_conditions_compare_and_fem_create_uda_udq(
        int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_action_info_t action_info;
    bcm_field_qualifier_info_create_t qual_info;
    void *dest_char;

    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    qual_info.size = 1;
    /*
     * 1 bit qualifier for smaller-than condition
     */
    sal_strncpy_s(dest_char, "QUAL_1_BIT_SMALLER", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_de_conditions_smaller_than_udq);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create \n", rv);
         return rv;
    }
    /*
     * 1 bit qualifier for bigger-than condition
     */
    sal_strncpy_s(dest_char, "QUAL_1_BIT_BIGGER", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_de_conditions_bigger_than_udq);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create \n", rv);
         return rv;
    }
    /*
     * const UDQ for compare
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    qual_info.size = 12;
    sal_strncpy_s(dest_char, "QUAL_VLAN_ID_CONST", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_de_conditions_vid_const_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create \n", rv);
         return rv;
    }
    /*
     * user defined action for FEM increment
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionVoid;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "ACTION_VOID_TTL", sizeof(action_info.name));
    action_info.size = 8;
    action_info.stage = bcmFieldStageIngressPMF2;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_de_conditions_void_ttl_uda);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionVoid;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "ACTION_VOID_RANGES", sizeof(action_info.name));
    action_info.size = 2;
    action_info.stage = bcmFieldStageIngressPMF2;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_de_conditions_void_ranges_uda);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }
    /*
     * user defined action for padding in IPMF3 field group
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionVoid;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "ACTION_PADDING", sizeof(action_info.name));
    action_info.size = 23;
    action_info.stage = bcmFieldStageIngressPMF3;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_de_conditions_padding_uda);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *  This function configures the compare mode for the context
 *
 * \param [in]  unit            - Device id
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_de_conditions_compare_and_fem_comapre_config(
        int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_context_compare_info_t compare_info;
    void *dest_char;

    /*
     * Configure compare keys for (vlanId < 800) check
     */
    bcm_field_context_compare_info_t_init(&compare_info);
    compare_info.first_key_info.nof_quals = 1;
    compare_info.first_key_info.qual_types[0] = bcmFieldQualifyVlanId;
    compare_info.first_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.first_key_info.qual_info[0].input_arg = 0;
    compare_info.first_key_info.qual_info[0].offset = 96;   /* For this qualifier we should specify the specific offset in the header.*/

    compare_info.second_key_info.nof_quals = 1;
    compare_info.second_key_info.qual_types[0] = cint_field_de_conditions_vid_const_qual;
    compare_info.second_key_info.qual_info[0].input_type = bcmFieldInputTypeConst;
    compare_info.second_key_info.qual_info[0].input_arg = 800;
    compare_info.second_key_info.qual_info[0].offset = 0;

    rv = bcm_field_context_compare_create(unit, 0, bcmFieldStageIngressPMF1, cint_field_de_conditions_context_id_ipmf1, cint_field_de_conditions_first_pair_id, &compare_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    /*
     * Configure compare keys for (vlanId > 200) check
     */
    bcm_field_context_compare_info_t_init(&compare_info);
    compare_info.first_key_info.nof_quals = 1;
    compare_info.first_key_info.qual_types[0] = bcmFieldQualifyVlanId;
    compare_info.first_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.first_key_info.qual_info[0].input_arg = 0;
    compare_info.first_key_info.qual_info[0].offset = 96;

    compare_info.second_key_info.nof_quals = 1;
    compare_info.second_key_info.qual_types[0] = cint_field_de_conditions_vid_const_qual;
    compare_info.second_key_info.qual_info[0].input_type = bcmFieldInputTypeConst;
    compare_info.second_key_info.qual_info[0].input_arg = 200;
    compare_info.second_key_info.qual_info[0].offset = 0;

    rv = bcm_field_context_compare_create(unit, 0, bcmFieldStageIngressPMF1, cint_field_de_conditions_context_id_ipmf1, cint_field_de_conditions_second_pair_id, &compare_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    return rv;
}

/**
* \brief
*  This function creates DE field group, adds FEM action and attaches the field group to the context compare
* \param [in] unit           - Device ID.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_de_conditions_compare_and_fem_fg_config(
       int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_fem_action_info_t  fem_action_info;
    bcm_field_action_priority_t action_priority;
    bcm_field_qualifier_info_get_t compare0_qual_info_get, compare1_qual_info_get;
    int mask_smaller_than, mask_bigger_than;
    int fem_mapping_bits_index, fem_condition_index, key_smaller_offset, key_bigger_offset, ii;
    void *dest_char;

    bcm_field_group_info_t_init(&fg_info);
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "DE_IPMF2", sizeof(fg_info.name));
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;

    /* Set quals */
    fg_info.nof_quals = 3;
    fg_info.qual_types[0] = cint_field_de_conditions_smaller_than_udq;
    fg_info.qual_types[1] = cint_field_de_conditions_bigger_than_udq;
    fg_info.qual_types[2] = bcmFieldQualifyIp4Ttl;

    /* Set actions */
    fg_info.nof_actions = 2;
    fg_info.action_types[0] = cint_field_de_conditions_void_ranges_uda;
    fg_info.action_types[1] = cint_field_de_conditions_void_ttl_uda;
    fg_info.action_with_valid_bit[0] = 0;
    fg_info.action_with_valid_bit[1] = 0;

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_de_conditions_fg_id_ipmf2);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    /** FEM actions. */
    bcm_field_fem_action_info_t_init(&fem_action_info);
    /*
     * FG result buffer is composed from 3 bit sequences:
     * |   8b   |       1b          |      1b           |
     * |  TTL   | bigger than cond. | smaller bit cond. |
     *
     * We want to qualify upon the 2 condition bits to be 1, i.e. value of 3.
     * FEM condition size is 4 bits, then we'll configure the msb of the condition to be 3
     * Since the condition bits overlap with TTL bits, we'll configure the action for the following values: 0x0011, 0x0111, 0x1011, 0x1111
     */
    fem_action_info.fem_input.input_offset = 0;
    fem_action_info.condition_msb = 3;
    mask_smaller_than = 1 << 0;
    mask_bigger_than = 1 << 1;
    for (ii = 0; ii < BCM_FIELD_NUMBER_OF_CONDITIONS_PER_FEM; ii++)
    {
        /* if the 2 LSBs are 1 set extraction_id 0 */
        if ((ii & mask_smaller_than) && (ii & mask_bigger_than))
        {
            fem_action_info.fem_condition[ii].extraction_id = 0;
            fem_action_info.fem_condition[ii].is_action_valid = 1;
        } else {
            fem_action_info.fem_condition[ii].extraction_id = 1;
            fem_action_info.fem_condition[ii].is_action_valid = 1;
        }

    }

    /**
     * extraction_id 0 is set for case (200 < vlanId < 800).
     * In this case we get TTL value + 5 into action container.
     */
    fem_action_info.fem_extraction[0].action_type = bcmFieldActionContainer;
    fem_action_info.fem_extraction[0].increment = cint_field_de_conditions_fem_increment_value;
    for (fem_mapping_bits_index = 0; fem_mapping_bits_index < 8; fem_mapping_bits_index++)
    {
        /**
         * We need to extract the TTL bits for the action values, starting from offset 2 (2 condition bits)
         */
        fem_action_info.fem_extraction[0].output_bit[fem_mapping_bits_index].source_type = bcmFieldFemExtractionOutputSourceTypeKey;
        fem_action_info.fem_extraction[0].output_bit[fem_mapping_bits_index].offset = fem_mapping_bits_index + 2;
    }

    /**
     * extraction_id 1 is set for case (vlanId <= 200 or vlanId => 800).
     * In this case we put the value 0x7f into action container.
     */
    fem_action_info.fem_extraction[1].action_type = bcmFieldActionContainer;
    for (fem_mapping_bits_index = 0; fem_mapping_bits_index < 7; fem_mapping_bits_index++)
    {
        /*
         * In case the condition is not met, set all the bits to one.
         */
        fem_action_info.fem_extraction[1].output_bit[fem_mapping_bits_index].source_type = bcmFieldFemExtractionOutputSourceTypeForce;
        fem_action_info.fem_extraction[1].output_bit[fem_mapping_bits_index].forced_value = 1;
    }

    action_priority = BCM_FIELD_ACTION_POSITION(3, 2);
    rv = bcm_field_fem_action_add(unit,0 , cint_field_de_conditions_fg_id_ipmf2, action_priority, &fem_action_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_fem_action_add\n", rv);
        return rv;
    }

    /**
     * Retrieve the desired result offset of the CompareKeysResult0 qualifier.
     */
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyCompareKeysResult0, bcmFieldStageIngressPMF2, &compare0_qual_info_get);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_group_qualifier_offset_get\n", rv);
       return rv;
    }

    /**
     * Since CompareKeysResult0 qualifier is composed from all compare operands, we need to extract the offset of the desirable operand
     * Possible operands: Equal, NotEqual, FirstKeySmaller, FirstKeyNotSmaller, FirstKeyBigger, FirstKeyNotBigger
     */
    rv = bcm_field_compare_operand_offset_get(unit, cint_field_de_conditions_first_pair_id, bcmFieldCompareOperandFirstKeySmaller, &key_smaller_offset);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_compare_operand_offset_get\n", rv);
        return rv;
    }

    /*
     * Get the same offset for bigger-than condition
     */
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyCompareKeysResult1, bcmFieldStageIngressPMF2, &compare1_qual_info_get);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_group_qualifier_offset_get\n", rv);
       return rv;
    }
    rv = bcm_field_compare_operand_offset_get(unit, cint_field_de_conditions_second_pair_id, bcmFieldCompareOperandFirstKeyBigger, &key_bigger_offset);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_compare_operand_offset_get\n", rv);
        return rv;
    }

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = key_smaller_offset + compare0_qual_info_get.offset;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].offset = key_bigger_offset + compare1_qual_info_get.offset;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[2].input_arg = 1;
    attach_info.key_info.qual_info[2].offset = 0;
    rv = bcm_field_group_context_attach(unit, 0, cint_field_de_conditions_fg_id_ipmf2, cint_field_de_conditions_context_id_ipmf1, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    return rv;
}

/**
* \brief
*  This function creates DE field group in IPMF3 to cascade action buffer from IPMF2
* \param [in] unit           - Device ID.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_de_conditions_compare_and_fem_cascading_fg_config(
       int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_context_compare_info_t compare_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    void *dest_char;

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "ipmf3_cascading_context", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, &cint_field_de_conditions_context_id_ipmf3);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    p_id.presel_id = cint_field_de_conditions_presel_id;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_de_conditions_context_id_ipmf3;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyContextId;
    p_data.qual_data[0].qual_value = cint_field_de_conditions_context_id_ipmf1;
    p_data.qual_data[0].qual_mask = 0x3f;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set \n", rv);
        return rv;
    }

    printf("Presel (%d) was configured for context(%d) \n", cint_field_de_conditions_presel_id, cint_field_de_conditions_context_id_ipmf3);

    bcm_field_group_info_t_init(&fg_info);
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "DE_IPMF3", sizeof(fg_info.name));
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;

    /* Set quals */
    fg_info.nof_quals = 1;
    /*
     * using bcmFieldQualifyContainer to receive the action buffer when cascading between IPMF2 and IPMF3
     */
    fg_info.qual_types[0] = bcmFieldQualifyContainer;
    /* Set actions */
    fg_info.nof_actions = 2;
    /*
     * Using an IPMF3 only action
     */
    fg_info.action_types[0] = bcmFieldActionStartPacketStrip;
    fg_info.action_types[1] = cint_field_de_conditions_padding_uda;
    fg_info.action_with_valid_bit[0] = 0;
    fg_info.action_with_valid_bit[1] = 0;

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_de_conditions_fg_id_ipmf3);
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
    rv = bcm_field_group_context_attach(unit, 0, cint_field_de_conditions_fg_id_ipmf3, cint_field_de_conditions_context_id_ipmf3, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    return rv;
}

/**
* \brief
*  This function destroys all allocated data by the configuration
* \param [in] unit           - Device ID.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_de_conditions_compare_and_fem_destroy(
        int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_action_priority_t action_priority;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;

    bcm_field_presel_entry_data_info_init(&p_data);
    p_data.entry_valid = TRUE;
    p_data.context_id = 0;
    p_id.presel_id = cint_field_de_conditions_presel_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_presel_set\n", rv);
        return rv;
    }
    rv = bcm_field_group_context_detach(unit, cint_field_de_conditions_fg_id_ipmf2, cint_field_de_conditions_context_id_ipmf1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach\n", rv);
        return rv;
    }

    action_priority = BCM_FIELD_ACTION_POSITION(3, 2);
    rv = bcm_field_fem_action_delete(unit, cint_field_de_conditions_fg_id_ipmf2, action_priority);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_fem_action_delete. \n", rv);
        return rv;
    }

    rv = bcm_field_group_delete(unit, cint_field_de_conditions_fg_id_ipmf2);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete\n", rv);
        return rv;
    }
    rv = bcm_field_action_destroy(unit, cint_field_de_conditions_void_ttl_uda);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_action_destroy. \n");
        return rv;
    }
    rv = bcm_field_action_destroy(unit, cint_field_de_conditions_void_ranges_uda);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_action_destroy. \n");
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, cint_field_de_conditions_smaller_than_udq);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qual_destroy. \n");
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, cint_field_de_conditions_bigger_than_udq);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qual_destroy. \n");
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, cint_field_de_conditions_vid_const_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qual_destroy. \n");
        return rv;
    }
    /**
     * Compare destroy the first pair_id
     */
    rv = bcm_field_context_compare_destroy(unit, bcmFieldStageIngressPMF1, cint_field_de_conditions_context_id_ipmf1, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_compare_destroy pair1\n", rv);
        return rv;
    }
    /**
     * Compare destroy the second pair_id
     */
    rv = bcm_field_context_compare_destroy(unit, bcmFieldStageIngressPMF1, cint_field_de_conditions_context_id_ipmf1, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_compare_destroy pair2\n", rv);
        return rv;
    }
    /**
    * Destroy the context
    */
    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, cint_field_de_conditions_context_id_ipmf1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy\n", rv);
        return rv;
    }
    rv = bcm_field_group_context_detach(unit, cint_field_de_conditions_fg_id_ipmf3, cint_field_de_conditions_context_id_ipmf3);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach\n", rv);
        return rv;
    }
    rv = bcm_field_group_delete(unit, cint_field_de_conditions_fg_id_ipmf3);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete\n", rv);
        return rv;
    }
    bcm_field_presel_entry_data_info_init(&p_data);
    p_data.entry_valid = TRUE;
    p_data.context_id = 0;
    p_id.presel_id = cint_field_de_conditions_presel_id;
    p_id.stage = bcmFieldStageIngressPMF3;
    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_presel_set\n", rv);
        return rv;
    }
    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF3, cint_field_de_conditions_context_id_ipmf3);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy\n", rv);
        return rv;
    }
    return rv;
}

 /**
 * \brief
 *  This function runs the second example main function:
 *      1) Create context and user defined actions and qualifiers.
 *      2) Set 2 ranges for source port.
 *      3) Create DE field group in IPMF2 and configure 2 EFESs actions (one per each range).
 * \param [in] unit           - Device ID.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_de_conditions_range_and_efes_example(
        int unit)
{
    int rv = BCM_E_NONE;

    /*
     * Create context
     */
    rv = cint_field_de_conditions_context_config(unit, FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_de_conditions_context_config\n", rv);
         return rv;
    }
    /*
     * Config user defined actions and qualifiers
     */
    rv = cint_field_de_conditions_range_and_efes_create_uda_udq(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_de_conditions_range_and_efes_create_uda_udq\n", rv);
        return rv;
    }
    /*
     * Set 2 ranges for source port
     */
    rv = cint_field_de_conditions_range_and_efes_set_range(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_de_conditions_range_and_efes_set_range\n", rv);
        return rv;
    }
    /*
     * Config DE field group in IPMF2 and add EFESs actions
     */
    rv = cint_field_de_conditions_range_and_efes_fg_config(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_de_conditions_fg_config \n", rv);
        return rv;
    }

    return rv;
}

/**
* \brief
*  This function creates DE field group, attaches it to context and adds EFES action
* \param [in] unit           - Device ID.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_de_conditions_range_and_efes_fg_config(
        int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_action_info_t action_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualifier_info_get_t ttl_qual_info_get;
    bcm_field_qualifier_info_get_t range_qual_info_get;
    bcm_field_efes_action_info_t efes_action_info;
    bcm_field_action_priority_t action_priority;
    bcm_field_action_t action = bcmFieldActionTtlSet;
    void *dest_char;
    int condition_action_offset_within_fg, const_64_action_offset_within_fg[2], const_128_action_offset_within_fg[2], double_action_offset_within_fg[2];
    int ii;
    int action_size;

    /*
     * get qualifiers info for offset
     */
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyIp4Ttl, cint_field_de_conditions_stage, &ttl_qual_info_get);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_info_get\n", rv);
        return rv;
    }
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyL4PortRangeCheck, cint_field_de_conditions_stage, &range_qual_info_get);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_info_get\n", rv);
        return rv;
    }
    /*
     * get TTL action size
     */
    rv = bcm_field_action_info_get(unit, action, cint_field_de_conditions_stage, &action_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_info_get\n", rv);
        return rv;
    }
    action_size = action_info.size;

    /*
     *  Create a field group.
     */
    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "range_and_efes", sizeof(fg_info.name));
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = cint_field_de_conditions_stage;

    /* Set quals */
    fg_info.nof_quals = 15;
    attach_info.key_info.nof_quals = fg_info.nof_quals;

    /*
     * First EFES data
     */
    /* range 0 bit is on bcmFieldQualifyL4PortRangeCheck offset */
    fg_info.qual_types[0] = cint_field_de_conditions_valid_bit_1_udq[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = range_qual_info_get.offset;
    /* double udq adds one bit of 0 at LSB */
    fg_info.qual_types[1] = cint_field_de_conditions_double_udq[0];
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[1].input_arg = 0;
    /* take 6 LSBs of TTL */
    fg_info.qual_types[2] = cint_field_de_conditions_action_udq[0];
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[2].input_arg = 1;
    attach_info.key_info.qual_info[2].offset = ttl_qual_info_get.offset + 2;

    fg_info.qual_types[3] = cint_field_de_conditions_valid_bit_2_udq[0];
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[3].offset = range_qual_info_get.offset;

    fg_info.qual_types[4] = cint_field_de_conditions_const_64_udq[0];
    attach_info.key_info.qual_info[4].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[4].input_arg = 64;

    fg_info.qual_types[5] = cint_field_de_conditions_valid_bit_3_udq[0];
    attach_info.key_info.qual_info[5].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[5].offset = range_qual_info_get.offset;

    fg_info.qual_types[6] = cint_field_de_conditions_const_128_udq[0];
    attach_info.key_info.qual_info[6].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[6].input_arg = 128;

    /*
     * Second EFES data
     */
    /* range 1 bit is on bcmFieldQualifyL4PortRangeCheck offset + 1 */
    fg_info.qual_types[7] = cint_field_de_conditions_valid_bit_1_udq[1];
    attach_info.key_info.qual_info[7].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[7].offset = range_qual_info_get.offset + 1;

    fg_info.qual_types[8] = cint_field_de_conditions_double_udq[1];
    attach_info.key_info.qual_info[8].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[8].input_arg = 1;

    fg_info.qual_types[9] = cint_field_de_conditions_action_udq[1];
    attach_info.key_info.qual_info[9].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[9].input_arg = 1;
    attach_info.key_info.qual_info[9].offset = ttl_qual_info_get.offset + 2;

    fg_info.qual_types[10] = cint_field_de_conditions_valid_bit_2_udq[1];
    attach_info.key_info.qual_info[10].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[10].offset = range_qual_info_get.offset+1;

    fg_info.qual_types[11] = cint_field_de_conditions_const_64_udq[1];
    attach_info.key_info.qual_info[11].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[11].input_arg = 65;

    fg_info.qual_types[12] = cint_field_de_conditions_valid_bit_3_udq[1];
    attach_info.key_info.qual_info[12].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[12].offset = range_qual_info_get.offset+1;

    fg_info.qual_types[13] = cint_field_de_conditions_const_128_udq[1];
    attach_info.key_info.qual_info[13].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[13].input_arg = 129;

    /*
     * Condition data for both EFESs (2 MSBs of TTL)
     * Condition is last because it must align to the MSB of a 32 bit chunk
     */
    fg_info.qual_types[14] = cint_field_de_conditions_condition_udq;
    attach_info.key_info.qual_info[14].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[14].input_arg = 1;
    attach_info.key_info.qual_info[14].offset = ttl_qual_info_get.offset;

    /* Set actions */
    fg_info.nof_actions = 13;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    fg_info.action_types[0] = cint_field_de_conditions_valid_bit_1_uda[0];
    fg_info.action_types[1] = cint_field_de_conditions_double_uda[0];
    fg_info.action_types[2] = cint_field_de_conditions_valid_bit_2_uda[0];
    fg_info.action_types[3] = cint_field_de_conditions_const_64_uda[0];
    fg_info.action_types[4] = cint_field_de_conditions_valid_bit_3_uda[0];
    fg_info.action_types[5] = cint_field_de_conditions_const_128_uda[0];
    fg_info.action_types[6] = cint_field_de_conditions_valid_bit_1_uda[1];
    fg_info.action_types[7] = cint_field_de_conditions_double_uda[1];
    fg_info.action_types[8] = cint_field_de_conditions_valid_bit_2_uda[1];
    fg_info.action_types[9] = cint_field_de_conditions_const_64_uda[1];
    fg_info.action_types[10] = cint_field_de_conditions_valid_bit_3_uda[1];
    fg_info.action_types[11] = cint_field_de_conditions_const_128_uda[1];
    fg_info.action_types[12] = cint_field_de_conditions_condition_uda;
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        fg_info.action_with_valid_bit[ii] = 0;
    }
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_de_conditions_fg_id_ipmf2);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    for(ii=0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for(ii=0; ii<fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    /* For direct extraction with EFES condition, we use the BCM_FIELD_FLAG_32_RESULT_MSB_ALIGN flag.*/
    rv = bcm_field_group_context_attach(unit, BCM_FIELD_FLAG_32_RESULT_MSB_ALIGN, cint_field_de_conditions_fg_id_ipmf2, cint_field_de_conditions_context_id_ipmf1, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    /*
     * Find position of void actions on the field group.
     */
    for(ii = 0; ii < 2; ii++)
    {
        rv = bcm_field_group_action_offset_get(unit, 0, cint_field_de_conditions_fg_id_ipmf2, cint_field_de_conditions_const_64_uda[ii], &const_64_action_offset_within_fg[ii]);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_group_action_offset_get\n", rv);
            return rv;
        }
        rv = bcm_field_group_action_offset_get(unit, 0, cint_field_de_conditions_fg_id_ipmf2, cint_field_de_conditions_const_128_uda[ii], &const_128_action_offset_within_fg[ii]);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_group_action_offset_get\n", rv);
            return rv;
        }
        rv = bcm_field_group_action_offset_get(unit, 0, cint_field_de_conditions_fg_id_ipmf2, cint_field_de_conditions_double_uda[ii], &double_action_offset_within_fg[ii]);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_group_action_offset_get\n", rv);
            return rv;
        }
    }
    rv = bcm_field_group_action_offset_get(unit, 0, cint_field_de_conditions_fg_id_ipmf2, cint_field_de_conditions_condition_uda, &condition_action_offset_within_fg);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_action_offset_get\n", rv);
        return rv;
    }

    /*
     * Configure 2 EFESs, one for each range id
     */

    for (ii = 0; ii < 2; ii++)
    {
        bcm_field_efes_action_info_t_init(&efes_action_info);
        efes_action_info.is_condition_valid = TRUE;
        efes_action_info.condition_msb = condition_action_offset_within_fg + 1;

        /**
        * For each condition index
        * TTL < 64 (2 msb are 00)
        */
        efes_action_info.efes_condition_conf[0].is_action_valid = TRUE;
        efes_action_info.efes_condition_conf[0].action_type = action;
        efes_action_info.efes_condition_conf[0].action_lsb = double_action_offset_within_fg[ii];
        efes_action_info.efes_condition_conf[0].action_nof_bits = action_size - 1;
        efes_action_info.efes_condition_conf[0].action_with_valid_bit = TRUE;
        efes_action_info.efes_condition_conf[0].valid_bit_polarity = 1;     /* valid bit represent src_port is in range, should be 1 */

        /**
        * 64 <= TTL < 128 (2 msb are 01)
        */
        efes_action_info.efes_condition_conf[1].is_action_valid = TRUE;
        efes_action_info.efes_condition_conf[1].action_type = action;
        efes_action_info.efes_condition_conf[1].action_lsb = const_64_action_offset_within_fg[ii];
        efes_action_info.efes_condition_conf[1].action_nof_bits = 7;
        efes_action_info.efes_condition_conf[1].action_with_valid_bit = TRUE;
        efes_action_info.efes_condition_conf[1].valid_bit_polarity = 1;
        /*
        * TTL >= 128 (msb is 1)
        */
        efes_action_info.efes_condition_conf[2].is_action_valid = TRUE;
        efes_action_info.efes_condition_conf[2].action_type = action;
        efes_action_info.efes_condition_conf[2].action_lsb = const_128_action_offset_within_fg[ii];
        efes_action_info.efes_condition_conf[2].action_nof_bits = action_size;
        efes_action_info.efes_condition_conf[2].action_with_valid_bit = TRUE;
        efes_action_info.efes_condition_conf[2].valid_bit_polarity = 1;

        efes_action_info.efes_condition_conf[3].is_action_valid = TRUE;
        efes_action_info.efes_condition_conf[3].action_type = action;
        efes_action_info.efes_condition_conf[3].action_lsb = const_128_action_offset_within_fg[ii];
        efes_action_info.efes_condition_conf[3].action_nof_bits = action_size;
        efes_action_info.efes_condition_conf[3].action_with_valid_bit = TRUE;
        efes_action_info.efes_condition_conf[3].valid_bit_polarity = 1;

        /*
         * Each EFES action must have different priority.
         * In this case priority doesn't matter, because the EFESs condition are contrasting with each other
         */
        action_priority = BCM_FIELD_ACTION_POSITION(0, ii+1);
        rv = bcm_field_efes_action_add(unit, 0, cint_field_de_conditions_fg_id_ipmf2, cint_field_de_conditions_context_id_ipmf1, &action_priority, &efes_action_info);
        if(rv != BCM_E_NONE)
        {
           printf("Error (%d), in bcm_field_efes_action_add\n", rv);
           return rv;
        }
    }
    return rv;
}

/**
* \brief
*  This function sets 2 ranges for source port
* \param [in] unit           - Device ID.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_de_conditions_range_and_efes_set_range(
        int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_range_info_t range_info;

    bcm_field_range_info_t_init(&range_info);
    range_info.range_type = bcmFieldRangeTypeL4SrcPort;
    range_info.min_val = 1000;
    range_info.max_val = 2000;
    /* User provided range id, can choose any value  */
    rv = bcm_field_range_set(unit, 0, bcmFieldStageIngressPMF2, cint_field_de_conditions_range_id_0, &range_info);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_range_set\n", rv);
       return rv;
    }

    bcm_field_range_info_t_init(&range_info);
    range_info.range_type = bcmFieldRangeTypeL4SrcPort;
    range_info.min_val = 2000;
    range_info.max_val = 3000;
    rv = bcm_field_range_set(unit, 0, bcmFieldStageIngressPMF2, cint_field_de_conditions_range_id_1, &range_info);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_range_set\n", rv);
       return rv;
    }
    return rv;
}

/**
 * \brief
 *  This function sets user defined actions and qualifiers
 *
 * \param [in]  unit            - Device id
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_de_conditions_range_and_efes_create_uda_udq(
        int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_action_info_t action_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualifier_info_get_t ttl_qual_info_get;
    bcm_field_action_t action = bcmFieldActionTtlSet;
    void *dest_char;
    int ii;
    int action_size;

    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyIp4Ttl, cint_field_de_conditions_stage, &ttl_qual_info_get);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_info_get\n", rv);
        return rv;
    }

    rv = bcm_field_action_info_get(unit, action, cint_field_de_conditions_stage, &action_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_info_get\n", rv);
        return rv;
    }
    action_size = action_info.size;
    /*
     *  Create void user defined action and user defined qualifiers
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.stage = cint_field_de_conditions_stage;
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = 1;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    dest_char = &(action_info.name[0]);
    /*
     *  Create valid bit user defined action for range ids
     */
    sal_strncpy_s(dest_char, "ACTION_VALID_BIT_1_RANGE_0", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_de_conditions_valid_bit_1_uda[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }
    sal_strncpy_s(dest_char, "ACTION_VALID_BIT_1_RANGE_1", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_de_conditions_valid_bit_1_uda[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "ACTION_VALID_BIT_2_RANGE_0", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_de_conditions_valid_bit_2_uda[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }
    sal_strncpy_s(dest_char, "ACTION_VALID_BIT_2_RANGE_1", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_de_conditions_valid_bit_2_uda[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "ACTION_VALID_BIT_3_RANGE_0", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_de_conditions_valid_bit_3_uda[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }
    sal_strncpy_s(dest_char, "ACTION_VALID_BIT_3_RANGE_1", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_de_conditions_valid_bit_3_uda[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }

    /*
     * Create 2 bit user defined action for 2 msb of TTL as condition index
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.stage = cint_field_de_conditions_stage;
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = 2;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "ACTION_CONDITION", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_de_conditions_condition_uda);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }

    /*
     * Create user defined action for double TTL smaller than 64
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.stage = cint_field_de_conditions_stage;
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = action_size - 1;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "ACTION_DOUBLE_ACTION", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_de_conditions_double_uda[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }
    sal_strncpy_s(dest_char, "ACTION_DOUBLE_PLUS_ONE_ACTION", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_de_conditions_double_uda[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }

    /*
     * Create const user defined action for TTL between 64-128
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.stage = cint_field_de_conditions_stage;
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = 7;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    sal_strncpy_s(dest_char, "ACTION_CONST_64", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_de_conditions_const_64_uda[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }
    sal_strncpy_s(dest_char, "ACTION_CONST_65", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_de_conditions_const_64_uda[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }

    /*
     * Create const user defined action for TTL bigger than 128
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.stage = cint_field_de_conditions_stage;
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = action_size;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    sal_strncpy_s(dest_char, "ACTION_CONST_128", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_de_conditions_const_128_uda[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }
    sal_strncpy_s(dest_char, "ACTION_CONST_129", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_de_conditions_const_128_uda[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "QUAL_VALID_BIT_1_RANGE_0", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_de_conditions_valid_bit_1_udq[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }
    sal_strncpy_s(dest_char, "QUAL_VALID_BIT_1_RANGE_1", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_de_conditions_valid_bit_1_udq[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "QUAL_VALID_BIT_2_RANGE_0", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_de_conditions_valid_bit_2_udq[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }
    sal_strncpy_s(dest_char, "QUAL_VALID_BIT_2_RANGE_1", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_de_conditions_valid_bit_2_udq[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "QUAL_VALID_BIT_3_RANGE_0", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_de_conditions_valid_bit_3_udq[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }
    sal_strncpy_s(dest_char, "QUAL_VALID_BIT_3_RANGE_1", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_de_conditions_valid_bit_3_udq[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    /*
     * Create const user defined qualifier for TTL between 64-128
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 7;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "QUAL_CONST_64", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_de_conditions_const_64_udq[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }
    sal_strncpy_s(dest_char, "QUAL_CONST_65", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_de_conditions_const_64_udq[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    /*
     * Create const user defined qualifier for TTL bigger than 128
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = action_size;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "QUAL_CONST_128", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_de_conditions_const_128_udq[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }
    sal_strncpy_s(dest_char, "QUAL_CONST_129", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_de_conditions_const_128_udq[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    /*
     * Create user defined qualifier for 6 lsb of TTL
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = ttl_qual_info_get.size - 2;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "QUAL_ACTION", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_de_conditions_action_udq[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }
    sal_strncpy_s(dest_char, "QUAL_ACTION_1", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_de_conditions_action_udq[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    /*
     * Create user defined qualifier for double TTL smaller than 64
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "QUAL_DOUBLE_CONST", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_de_conditions_double_udq[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }
    sal_strncpy_s(dest_char, "QUAL_DOUBLE_PLUS_ONE_CONST", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_de_conditions_double_udq[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    /*
     * Create 2 bit user defined action for 2 msb of TTL as condition index
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 2;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "QUAL_CONDITION", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_de_conditions_condition_udq);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    return rv;
}

/**
* \brief
*  This function destroys all allocated data by the configuration
* \param [in] unit           - Device ID.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_de_conditions_range_and_efes_destroy(
        int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    int ii;

    bcm_field_presel_entry_data_info_init(&p_data);
    p_data.entry_valid = TRUE;
    p_data.context_id = 0;
    p_id.presel_id = cint_field_de_conditions_presel_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_presel_set\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, cint_field_de_conditions_fg_id_ipmf2, cint_field_de_conditions_context_id_ipmf1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach\n", rv);
        return rv;
    }
    rv = bcm_field_group_delete(unit, cint_field_de_conditions_fg_id_ipmf2);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete\n", rv);
        return rv;
    }
    rv = bcm_field_action_destroy(unit, cint_field_de_conditions_condition_uda);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_action_destroy. \n");
        return rv;
    }
    for (ii = 0; ii < 2; ii++) {
        rv = bcm_field_action_destroy(unit, cint_field_de_conditions_double_uda[ii]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_action_destroy. \n");
            return rv;
        }
        rv = bcm_field_action_destroy(unit, cint_field_de_conditions_const_64_uda[ii]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_action_destroy. \n");
            return rv;
        }
        rv = bcm_field_action_destroy(unit, cint_field_de_conditions_const_128_uda[ii]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_action_destroy. \n");
            return rv;
        }
        rv = bcm_field_action_destroy(unit, cint_field_de_conditions_valid_bit_1_uda[ii]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_action_destroy. \n");
            return rv;
        }
        rv = bcm_field_action_destroy(unit, cint_field_de_conditions_valid_bit_2_uda[ii]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_action_destroy. \n");
            return rv;
        }
        rv = bcm_field_action_destroy(unit, cint_field_de_conditions_valid_bit_3_uda[ii]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_action_destroy. \n");
            return rv;
        }
        rv = bcm_field_qualifier_destroy(unit, cint_field_de_conditions_double_udq[ii]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_qualifier_destroy. \n");
            return rv;
        }
        rv = bcm_field_qualifier_destroy(unit, cint_field_de_conditions_const_64_udq[ii]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_qualifier_destroy. \n");
            return rv;
        }
        rv = bcm_field_qualifier_destroy(unit, cint_field_de_conditions_const_128_udq[ii]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_qualifier_destroy. \n");
            return rv;
        }
        rv = bcm_field_qualifier_destroy(unit, cint_field_de_conditions_valid_bit_1_udq[ii]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_qualifier_destroy. \n");
            return rv;
        }
        rv = bcm_field_qualifier_destroy(unit, cint_field_de_conditions_valid_bit_2_udq[ii]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_qualifier_destroy. \n");
            return rv;
        }
        rv = bcm_field_qualifier_destroy(unit, cint_field_de_conditions_valid_bit_3_udq[ii]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_qualifier_destroy. \n");
            return rv;
        }
        rv = bcm_field_qualifier_destroy(unit, cint_field_de_conditions_action_udq[ii]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_qualifier_destroy. \n");
            return rv;
        }
    }
    rv = bcm_field_qualifier_destroy(unit, cint_field_de_conditions_condition_udq);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_destroy. \n");
        return rv;
    }

    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, cint_field_de_conditions_context_id_ipmf1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy\n", rv);
        return rv;
    }
    return rv;
}
