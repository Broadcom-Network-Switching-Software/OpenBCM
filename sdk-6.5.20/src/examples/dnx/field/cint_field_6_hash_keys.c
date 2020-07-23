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
 * cint ../../../src/examples/dnx/field/cint_field_6_hash_keys.c
 * cint;
 * int unit = 0;
 * cint_field_6_hash_keys_main(unit);
 *
 * Configuration example end
 *
 *   This cint creates a hash context using multiple action keys in one configuration.
 *   The function cint_field_6_hash_keys_update also shows an example of updating context hash config.
 */

bcm_field_context_t cint_field_6_hash_keys_context_id = 0;
bcm_field_context_hash_info_t cint_field_6_hash_keys_hash_info;

/**
* \brief
*   Configure hash context with multiple action keys.
*
* \param [in] unit    - Device ID.
*
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_6_hash_keys_main(
    int unit)
{
    bcm_field_context_info_t context_info;
    void *dest_char;
    int rv = BCM_E_NONE;

    /*
    * Create a new context with hashing_enabled
    */
    bcm_field_context_info_t_init(&context_info);
    context_info.hashing_enabled = TRUE;
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "HASH_CONTEXT_6_HASH_KEYS", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &cint_field_6_hash_keys_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    /*
    * Create and configure a hash context with 3 action_keys
    */
    bcm_field_context_hash_info_t_init(&cint_field_6_hash_keys_hash_info);
    cint_field_6_hash_keys_hash_info.hash_function = bcmFieldContextHashFunctionCrc16Xor1;
    cint_field_6_hash_keys_hash_info.key_info.nof_quals = 5;
    cint_field_6_hash_keys_hash_info.key_info.qual_types[0] = bcmFieldQualifySrcIp;
    cint_field_6_hash_keys_hash_info.key_info.qual_types[1] = bcmFieldQualifyDstIp;
    cint_field_6_hash_keys_hash_info.key_info.qual_types[2] = bcmFieldQualifyIp4Protocol;
    cint_field_6_hash_keys_hash_info.key_info.qual_types[3] = bcmFieldQualifyL4SrcPort;
    cint_field_6_hash_keys_hash_info.key_info.qual_types[4] = bcmFieldQualifyL4DstPort;
    cint_field_6_hash_keys_hash_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    cint_field_6_hash_keys_hash_info.key_info.qual_info[0].input_arg = 1;
    cint_field_6_hash_keys_hash_info.key_info.qual_info[0].offset = 0;
    cint_field_6_hash_keys_hash_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    cint_field_6_hash_keys_hash_info.key_info.qual_info[1].input_arg = 1;
    cint_field_6_hash_keys_hash_info.key_info.qual_info[1].offset = 0;
    cint_field_6_hash_keys_hash_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    cint_field_6_hash_keys_hash_info.key_info.qual_info[2].input_arg = 1;
    cint_field_6_hash_keys_hash_info.key_info.qual_info[2].offset = 0;
    cint_field_6_hash_keys_hash_info.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerAbsolute;
    cint_field_6_hash_keys_hash_info.key_info.qual_info[3].input_arg = 2;
    cint_field_6_hash_keys_hash_info.key_info.qual_info[3].offset = 0;
    cint_field_6_hash_keys_hash_info.key_info.qual_info[4].input_type = bcmFieldInputTypeLayerAbsolute;
    cint_field_6_hash_keys_hash_info.key_info.qual_info[4].input_arg = 2;
    cint_field_6_hash_keys_hash_info.key_info.qual_info[4].offset = 0;

    cint_field_6_hash_keys_hash_info.hash_config.action_key = bcmFieldContextHashActionKeyEcmpLbKey0;
    cint_field_6_hash_keys_hash_info.hash_config.function_select = bcmFieldContextHashActionValueAugmentCrc;
    cint_field_6_hash_keys_hash_info.hash_config.crc_select = bcmFieldCrcSelectCrc16P0x10ac5;

    cint_field_6_hash_keys_hash_info.nof_additional_hash_config = 2;
    cint_field_6_hash_keys_hash_info.additional_hash_config[0].action_key = bcmFieldContextHashActionKeyEcmpLbKey1;
    cint_field_6_hash_keys_hash_info.additional_hash_config[0].function_select = bcmFieldContextHashActionValueAugmentCrc;
    cint_field_6_hash_keys_hash_info.additional_hash_config[0].crc_select = bcmFieldCrcSelectCrc16P0x109e7;

    cint_field_6_hash_keys_hash_info.additional_hash_config[1].action_key = bcmFieldContextHashActionKeyEcmpLbKey2;
    cint_field_6_hash_keys_hash_info.additional_hash_config[1].function_select = bcmFieldContextHashActionValueReplaceCrc;

    rv = bcm_field_context_hash_create(unit, 0, bcmFieldStageIngressPMF1, cint_field_6_hash_keys_context_id, &cint_field_6_hash_keys_hash_info);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_context_hash_create \n", rv);
       return rv;
    }

    return rv;
}


/**
* \brief
*   Update hash context and add 3 action keys.
*
* \param [in] unit    - Device ID.
*
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_6_hash_keys_update(
    int unit)
{
    int rv = BCM_E_NONE;

    cint_field_6_hash_keys_hash_info.hash_function = bcmFieldContextHashFunctionCrc16Xor2;

    /* Updating hash_action and crc_select for action_key bcmFieldContextHashActionKeyEcmpLbKey0 */
    cint_field_6_hash_keys_hash_info.hash_config.function_select = bcmFieldContextHashActionValueReplaceCrc;
    /* hash action was changed to replace, so crc_select needs to be changed to invalid*/
    cint_field_6_hash_keys_hash_info.hash_config.crc_select = bcmFieldCrcSelectInvalid;

    cint_field_6_hash_keys_hash_info.nof_additional_hash_config = 5;
    cint_field_6_hash_keys_hash_info.additional_hash_config[2].action_key = bcmFieldContextHashActionKeyNetworkLbKey;
    cint_field_6_hash_keys_hash_info.additional_hash_config[2].function_select = bcmFieldContextHashActionValueAugmentKey;
    cint_field_6_hash_keys_hash_info.additional_hash_config[2].crc_select = bcmFieldCrcSelectCrc16P0x1203d;

    cint_field_6_hash_keys_hash_info.additional_hash_config[3].action_key = bcmFieldContextHashActionKeyLagLbKey;
    cint_field_6_hash_keys_hash_info.additional_hash_config[3].function_select = bcmFieldContextHashActionValueAugmentKey;
    cint_field_6_hash_keys_hash_info.additional_hash_config[3].crc_select = bcmFieldCrcSelectCrc16P0x12105;

    cint_field_6_hash_keys_hash_info.additional_hash_config[4].action_key = bcmFieldContextHashActionKeyAdditionalLbKey;
    cint_field_6_hash_keys_hash_info.additional_hash_config[4].function_select = bcmFieldContextHashActionValueReplaceCrc;

    rv = bcm_field_context_hash_create(unit, BCM_FIELD_FLAG_UPDATE, bcmFieldStageIngressPMF1, cint_field_6_hash_keys_context_id, &cint_field_6_hash_keys_hash_info);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_context_hash_create \n", rv);
       return rv;
    }
    return rv;
}


/**
* \brief
*  Destroys all allocated data by the configuration.
*
* \param [in] unit    - Device ID.
*
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_6_hash_keys_destroy(
    int unit)
{
    int rv = BCM_E_NONE;

    rv = bcm_field_context_hash_destroy(unit, bcmFieldStageIngressPMF1, cint_field_6_hash_keys_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_hash_destroy context_id %d \n", rv, cint_field_6_hash_keys_context_id);
        return rv;
    }

    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, cint_field_6_hash_keys_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy context_id %d \n", rv, cint_field_6_hash_keys_context_id);
        return rv;
    }
    return rv;
}
