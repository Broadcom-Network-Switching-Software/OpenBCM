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
 * cint ../../../src/examples/dnx/field/cint_field_hashing.c
 * cint;
 * int unit = 0;
 * cint_field_hashing_main(unit);
 *
 * Configuration example end
 *
 * Sets-up a hashing key in iPMF1 comprised of 5-tuple qualifiers (src-ip,
 * dst-ip, protocol, L4 source port, L4 dst-port) and build hash key.
 * This hash key serves as qualifier in TCAM FG lookup in iPMF2 stage.
 */

bcm_field_group_t   cint_hashing_fg_id = 0;
bcm_field_context_t cint_hashing_context_id = 0;
bcm_field_entry_t              cint_hashing_ent_id = 0;
bcm_field_presel_t              cint_hashing_presel_id = 120;





int cint_field_hashing_main(int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_hash_info_t hash_info;
    bcm_field_context_info_t context_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_gport_t         gport_mirror;
    int rv = BCM_E_NONE;

    /*
     * Create a new context for first group, since two groups on same context can't share banks
     */
    printf("Creating new context for the hashing\n");
    bcm_field_context_info_t_init(&context_info);
    context_info.hashing_enabled = TRUE;
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &cint_hashing_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    printf("Context created!\n");

    /*create the hash key and attach it to the context */
    bcm_field_context_hash_info_t_init(&hash_info);
    hash_info.hash_function = bcmFieldContextHashFunctionCrc16Bisync;
    hash_info.order = TRUE;
    hash_info.hash_config.function_select = bcmFieldContextHashActionValueReplaceCrc;
    hash_info.hash_config.action_key = bcmFieldContextHashActionKeyAdditionalLbKey;
    hash_info.key_info.nof_quals = 5;
    hash_info.key_info.qual_types[0] = bcmFieldQualifySrcIp;
    hash_info.key_info.qual_types[1] = bcmFieldQualifyDstIp;
    hash_info.key_info.qual_types[2] = bcmFieldQualifyIp4Protocol;
    hash_info.key_info.qual_types[3] = bcmFieldQualifyL4SrcPort;
    hash_info.key_info.qual_types[4] = bcmFieldQualifyL4DstPort;
    hash_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[0].input_arg = 1;
    hash_info.key_info.qual_info[0].offset = 0;
    hash_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[1].input_arg = 1;
    hash_info.key_info.qual_info[1].offset = 0;
    hash_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[2].input_arg = 1;
    hash_info.key_info.qual_info[2].offset = 0;
    hash_info.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[3].input_arg = 2;
    hash_info.key_info.qual_info[3].offset = 0;
    hash_info.key_info.qual_info[4].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[4].input_arg = 2;
    hash_info.key_info.qual_info[4].offset = 0;
    bcm_field_context_hash_create(unit, 0, bcmFieldStageIngressPMF1,cint_hashing_context_id,&hash_info );

    /*
     * Create and attach TCAM group in iPMF2
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF2;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyHashValue;

    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionMirrorIngress;

    printf("Creating first group\n");
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_hashing_fg_id);
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

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    printf("Attaching field group\n");
    rv = bcm_field_group_context_attach(unit, 0, cint_hashing_fg_id, cint_hashing_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    /*
     * Add entry
     */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 16;
    
    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = 0x1b5d;
    ent_info.entry_qual[0].mask[0] = 0xffff;

    BCM_GPORT_MIRROR_SET(gport_mirror, 1);
    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = gport_mirror;

    printf("Adding  entry\n");
    rv = bcm_field_entry_add(unit, 0, cint_hashing_fg_id, &ent_info, &cint_hashing_ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_hashing_presel_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_hashing_context_id;
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

    printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(Ethernet) \n",
    		cint_hashing_presel_id,cint_hashing_context_id);

    return 0;
}

int cint_field_hashing_destroy(int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_hashing_presel_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = FALSE;
    p_data.context_id = 0;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    rv = bcm_field_entry_delete(unit, cint_hashing_fg_id, NULL, cint_hashing_ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete  fg %d \n", rv, cint_hashing_fg_id);
        return rv;
    }

    /* Detach the IPMF2 FG from its context */
    rv = bcm_field_group_context_detach(unit, cint_hashing_fg_id, cint_hashing_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, cint_hashing_fg_id, cint_hashing_context_id);
        return rv;
    }

    rv = bcm_field_group_delete(unit, cint_hashing_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, cint_hashing_fg_id);
        return rv;
    }

    rv = bcm_field_context_hash_destroy(unit, bcmFieldStageIngressPMF1, cint_hashing_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_hash_destroy context_id %d \n", rv, cint_hashing_context_id);
        return rv;
    }

    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, cint_hashing_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy context_id %d \n", rv, cint_hashing_context_id);
        return rv;
    }
    return rv;
}

/*
 * Example function for configuring IPv6 asymmetrical hashing.
 * Load balancing configured as ECMP hash config, we convert the ECMP to asymmetrical using the PMF.
 * The example is made under the assumption that the packet type is (L4o)IPv6oETH where the forwarding layer is ETH.
 *
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dnx/field/cint_field_hashing.c
 * cint
 * int unit = 0;
 * cint_field_hashing_ipv6_asymmetrical(unit);
 *
 * Add basic bridge configuration to forward packet:
 * port = 200
 * vid = 5
 * DMAC = 00:00:9f:e2:11:dc
 * exit;
 *
 * tx 1 psrc=200 data=0x00009fe211dc0000ceab9ee88100000586dd60000000002d84800fe1b03074b805ba0f65cf29f37f76e0cc05aac2f4138439fbe76c3ea4bb2914009b00bb0000000000000000000102030405060708090a0b0c0d0e0f
 */
int cint_field_hashing_ipv6_asymmetrical(int unit)
{
    bcm_field_context_hash_info_t hash_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    int rv = BCM_E_NONE;
    int ecmp_hash_config_crc;
    bcm_field_crc_select_t field_hash_crc_select;

    /* Get the crc function used for load balancing (that was done with bcmSwitchECMPHashConfig) and translate it to hash crc select.
     * We don't really have to use the same crc_select for the hashing process
     * we can use any crc_select we want */
    rv = bcm_switch_control_get(unit, bcmSwitchECMPHashConfig, &ecmp_hash_config_crc);
    if(rv != BCM_E_NONE)
    {
        printf("bcm_switch_control_get bcmSwitch-Config client failed \n");
        return rv;
    }

    switch (ecmp_hash_config_crc) {
    case BCM_HASH_CONFIG_CRC16_0x1015d:
        field_hash_crc_select = bcmFieldCrcSelectCrc16P0x1015d;
        break;
    case BCM_HASH_CONFIG_CRC16_0x100d7:
        field_hash_crc_select = bcmFieldCrcSelectCrc16P0x100d7;
        break;
    case BCM_HASH_CONFIG_CRC16_0x10039:
        field_hash_crc_select = bcmFieldCrcSelectCrc16P0x10039;
        break;
    case BCM_HASH_CONFIG_CRC16_0x10ac5:
        field_hash_crc_select = bcmFieldCrcSelectCrc16P0x10ac5;
        break;
    case BCM_HASH_CONFIG_CRC16_0x109e7:
        field_hash_crc_select = bcmFieldCrcSelectCrc16P0x109e7;
        break;
    case BCM_HASH_CONFIG_CRC16_0x10939:
        field_hash_crc_select = bcmFieldCrcSelectCrc16P0x10939;
        break;
    case BCM_HASH_CONFIG_CRC16_0x12105:
        field_hash_crc_select = bcmFieldCrcSelectCrc16P0x12105;
        break;
    case BCM_HASH_CONFIG_CRC16_0x1203d:
        field_hash_crc_select = bcmFieldCrcSelectCrc16P0x1203d;
        break;
    }

    /* Create a new context */
    printf("Creating new context for the hashing\n");
    bcm_field_context_info_t_init(&context_info);
    context_info.hashing_enabled = TRUE;
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &cint_hashing_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    /* Create the hash key and attach it to the context */
    bcm_field_context_hash_info_t_init(&hash_info);
    hash_info.hash_function = bcmFieldContextHashFunctionCrc32BHigh;
    hash_info.order = FALSE;
    /* Since the key is larger than 64b, we use compression */
    hash_info.compression = TRUE;
    /* We augment the ECMP hash symmetrical key to make it asymmetrical */
    hash_info.hash_config.function_select = bcmFieldContextHashActionValueAugmentCrc;
    hash_info.hash_config.action_key = bcmFieldContextHashActionKeyEcmpLbKey0;
    hash_info.hash_config.crc_select = field_hash_crc_select;
    hash_info.key_info.nof_quals = 1;
    hash_info.key_info.qual_types[0] = bcmFieldQualifyDstIp6;
    hash_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[0].input_arg = 1;
    hash_info.key_info.qual_info[0].offset = 0;

    rv = bcm_field_context_hash_create(unit, 0, bcmFieldStageIngressPMF1, cint_hashing_context_id, &hash_info);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_context_hash_create \n", rv);
       return rv;
    }

    /* This presel configuration apply to packets with forwarding type ETH and forwarding type +1 IPv6,
     * but it can be modified according to the type of packets we want */
    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_hashing_presel_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_hashing_context_id;
    p_data.nof_qualifiers = 2;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeEth;
    p_data.qual_data[0].qual_mask = 0x1F;
    p_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[1].qual_arg = 1;
    p_data.qual_data[1].qual_value = bcmFieldLayerTypeIp6;
    p_data.qual_data[1].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(Ethernet) \n",
            cint_hashing_presel_id,cint_hashing_context_id);

    return 0;
}

/**
 * This function should be used to clean up after the cint_field_hashing_ipv6_asymmetrical.
 */
int cint_field_hashing_ipv6_asymmetrical_destroy(int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_hashing_presel_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = FALSE;
    p_data.context_id = 0;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    rv = bcm_field_context_hash_destroy(unit, bcmFieldStageIngressPMF1, cint_hashing_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_hash_destroy context_id %d \n", rv, cint_hashing_context_id);
        return rv;
    }

    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, cint_hashing_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy context_id %d \n", rv, cint_hashing_context_id);
        return rv;
    }
    return rv;
}
