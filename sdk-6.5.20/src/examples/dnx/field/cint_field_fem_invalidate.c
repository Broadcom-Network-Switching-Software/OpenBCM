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
 * cint ../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../src/examples/dnx/field/cint_field_presel_fwd_layer.c
 * cint ../../../src/examples/dnx/field/cint_field_fem_invalidate.c
 * cint;
 * int unit = 0;
 * bcm_field_context_t context_ipv4 = BCM_FIELD_CONTEXT_ID_DEFAULT;
 * bcm_field_context_t context_ipv6 = BCM_FIELD_CONTEXT_ID_DEFAULT;
 * bcm_field_presel_t ip4_presel_id = 50;
 * bcm_field_presel_t ip6_presel_id = 51;
 * bcm_field_stage_t ipmf1_stage = bcmFieldStageIngressPMF1;
 * bcm_field_layer_type_t ip4_fwd_layer = bcmFieldLayerTypeIp4;
 * bcm_field_layer_type_t ip6_fwd_layer = bcmFieldLayerTypeIp6;
 * field_presel_fwd_layer_main(unit, ip4_presel_id, ipmf1_stage, ip4_fwd_layer, &context_ipv4, "ipv4_fem_invalidate");
 * field_presel_fwd_layer_main(unit, ip6_presel_id, ipmf1_stage, ip6_fwd_layer, &context_ipv6, "ipv6_fem_invalidate");
 * cint_field_fem_invalidate_main(unit, context_ipv4, context_ipv6);
 *
 * Configuration example end
 *
 * This CINT creates two FEM actions and attaches only one to each of two contexts.
 */
 /**
  * Global variables, used to save information for destroy function.
  */
/**
 * FG ID
 */
int Cint_field_fem_invalidate_fg_id = BCM_FIELD_INVALID;
/**
 * User defined qualifier
 */
bcm_field_qualify_t Cint_field_fem_invalidate_udq = BCM_FIELD_INVALID;
/**
 * User defined action to match the user defined qualifier for direct extraction.
 */
bcm_field_action_t Cint_field_fem_invalidate_uda = BCM_FIELD_INVALID;
/**
 * Position of first FEM
 */
bcm_field_action_priority_t Cint_field_fem_invalidate_fem_position_1 = BCM_FIELD_ACTION_POSITION(3, 0);
/**
 * Position of second FEM
 */
bcm_field_action_priority_t Cint_field_fem_invalidate_fem_position_2 = BCM_FIELD_ACTION_POSITION(3, 1);






/**
 * This function configures a field group, adds two FEM actions and then attaches only one to each of two contexts.
 * For IPv4, the FEM will add 5 to the TTL, for IPv6 the FEM will add 6 to the hop limit.
 *
 * \param [in] unit - The unit number.
 * \param [in] context_ipv4 - The first context ID, expects an IPv4 packet.
 * \param [in] context_ipv6 - The second context ID, expects an IPv6 packet.
 */
int
cint_field_fem_invalidate_main(
    int unit,
    bcm_field_context_t context_ipv4,
    bcm_field_context_t context_ipv6)
{
    char *proc_name;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t fg_attach_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    bcm_field_qualifier_info_get_t qual_info_get;
    bcm_field_action_info_t action_info_get;
    bcm_field_fem_action_info_t fem_action_info;
    void * dest_char;
    int qual_action_size;
    int ttl_offset;
    int hop_limit_offset;
    int fem_condition_index;
    int fem_extraction_index;
    bcm_field_action_priority_t invalidate_fems[1];

    int rv = BCM_E_NONE;
    proc_name = "cint_field_fem_invalidate_main";

    qual_action_size = 8;

    /*
     * Create a user defined qualifier to collect the TTL from IPv4 and hop limit from IPv6.
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = qual_action_size;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "FEM_invalidate_udq", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &Cint_field_fem_invalidate_udq);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_qualifier_create\n", proc_name, rv);
        return rv;
    }

    /*
     * Create a user defined action to be used by EFES. 
     * Could have been a void action for the purpose of the example, by using bcmFieldActionVoid as base action.
     */
    rv = bcm_field_action_info_get(unit, bcmFieldActionUDHData0, bcmFieldStageIngressPMF2, &action_info_get);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_action_create \n", proc_name, rv);
        return rv;
    }
    bcm_field_action_info_t_init(&action_info);
    action_info.stage = bcmFieldStageIngressPMF2;
    action_info.action_type = bcmFieldActionUDHData0;
    action_info.size = qual_action_size;
    action_info.prefix_size = action_info_get.size - action_info.size;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "FEM_invalidate_uda", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &Cint_field_fem_invalidate_uda);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    /* 
     * Create the field group.
     */
    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&fg_attach_info);
    fg_info.stage = bcmFieldStageIngressPMF2;
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = Cint_field_fem_invalidate_udq;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = Cint_field_fem_invalidate_uda;
    fg_info.action_with_valid_bit[0] = FALSE;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "FEM_invalidate_fg", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, 0, &fg_info, &Cint_field_fem_invalidate_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_group_add\n", proc_name, rv);
        return rv;
    }

    /* 
     * Add the first FEM, adding 5.
     * Place it on the first FEM of the second array.
     */
    bcm_field_fem_action_info_t_init(&fem_action_info);
    fem_action_info.fem_input.input_offset = 0;
    fem_action_info.condition_msb = 3;
    for (fem_condition_index = 0; fem_condition_index < BCM_FIELD_NUMBER_OF_CONDITIONS_PER_FEM; fem_condition_index++)
    {
        fem_action_info.fem_condition[fem_condition_index].is_action_valid = TRUE;
        fem_action_info.fem_condition[fem_condition_index].extraction_id = 0;
    }
    fem_action_info.fem_extraction[0].action_type = bcmFieldActionUDHData1;
    /*  Extract the TTL/hop_limit only. */
    for (fem_extraction_index = 0; fem_extraction_index < qual_action_size; fem_extraction_index++)
    {
        /*
         * Use a bit from the input key
         */
        fem_action_info.fem_extraction[0].output_bit[fem_extraction_index].source_type = bcmFieldFemExtractionOutputSourceTypeKey;
        fem_action_info.fem_extraction[0].output_bit[fem_extraction_index].offset = fem_extraction_index;
    }
    for (; fem_extraction_index < BCM_FIELD_NUMBER_OF_MAPPING_BITS_PER_FEM; fem_extraction_index++)
    {
        /*
         * Use a constant value for the rest of the bits
         */
        fem_action_info.fem_extraction[0].output_bit[fem_extraction_index].source_type = bcmFieldFemExtractionOutputSourceTypeForce;
        fem_action_info.fem_extraction[0].output_bit[fem_extraction_index].forced_value = 0;
    }
    fem_action_info.fem_extraction[0].increment = 5;
    rv = bcm_field_fem_action_add(unit, 0, Cint_field_fem_invalidate_fg_id, Cint_field_fem_invalidate_fem_position_1, &fem_action_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_fem_action_add\n", proc_name, rv);
        return rv;
    }

    /*
     * Add the second FEM, adding 6.
     * Place it on the second FEM of the second array.
     */
    fem_action_info.fem_extraction[0].increment = 6;
    rv = bcm_field_fem_action_add(unit, 0, Cint_field_fem_invalidate_fg_id, Cint_field_fem_invalidate_fem_position_2, &fem_action_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_fem_action_add\n", proc_name, rv);
        return rv;
    }

    /* 
     * Get the offsets of TTL and hop limit
     */
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyIp4Ttl, bcmFieldStageIngressPMF2, &qual_info_get);
    if(rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d) in bcm_field_qualifier_info_get\n", proc_name, rv);
        return rv;
    }
    ttl_offset = qual_info_get.offset;
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyIp6HopLimit, bcmFieldStageIngressPMF2, &qual_info_get);
    if(rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d) in bcm_field_qualifier_info_get\n", proc_name, rv);
        return rv;
    }
    hop_limit_offset = qual_info_get.offset;

    /* 
     * Attach to the IPv4 context. 
     */
    bcm_field_group_attach_info_t_init(&fg_attach_info);
    fg_attach_info.key_info.nof_quals = fg_info.nof_quals;
    fg_attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    fg_attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    fg_attach_info.key_info.qual_info[0].input_arg = 0;
    fg_attach_info.key_info.qual_info[0].offset = ttl_offset;
    fg_attach_info.payload_info.nof_actions = fg_info.nof_actions;
    fg_attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    /* Inavlidate the second FEM.*/
    invalidate_fems[0] = Cint_field_fem_invalidate_fem_position_2;
    fg_attach_info.payload_info.nof_invalidate_fems = 1;
    fg_attach_info.payload_info.invalidate_fems = invalidate_fems;
    rv = bcm_field_group_context_attach(unit, 0, Cint_field_fem_invalidate_fg_id, context_ipv4, &fg_attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_group_context_attach to IPv4\n", proc_name, rv);
        return rv;
    }

    /* 
     * Attach to the IPv6 context. 
     */
    fg_attach_info.key_info.qual_info[0].offset = hop_limit_offset;
    /* Inavlidate the first FEM.*/
    invalidate_fems[0] = Cint_field_fem_invalidate_fem_position_1;
    fg_attach_info.payload_info.nof_invalidate_fems = 1;
    fg_attach_info.payload_info.invalidate_fems = invalidate_fems;
    rv = bcm_field_group_context_attach(unit, 0, Cint_field_fem_invalidate_fg_id, context_ipv6, &fg_attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_group_context_attach to IPv6\n", proc_name, rv);
        return rv;
    }

    return rv;
}

/**
 * This function deletes what was configured in cint_field_fem_invalidate_main
 *
 * \param [in] unit - The unit number.
 * \param [in] context_ipv4 - The first context ID, expects an IPv4 packet.
 * \param [in] context_ipv6 - The second context ID, expects an IPv6 packet.
 */
int
cint_field_fem_invalidate_destroy(
    int unit,
    bcm_field_context_t context_ipv4,
    bcm_field_context_t context_ipv6)
{
    char *proc_name;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_fem_invalidate_destroy";

    rv = bcm_field_group_context_detach(unit, Cint_field_fem_invalidate_fg_id, context_ipv4);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_group_context_detach\n", proc_name, rv);
        return rv;
    }
    rv = bcm_field_group_context_detach(unit, Cint_field_fem_invalidate_fg_id, context_ipv6);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_group_context_detach\n", proc_name, rv);
        return rv;
    }

    rv = bcm_field_fem_action_delete(unit, Cint_field_fem_invalidate_fg_id, Cint_field_fem_invalidate_fem_position_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_fem_action_delete\n", proc_name, rv);
        return rv;
    }
    rv = bcm_field_fem_action_delete(unit, Cint_field_fem_invalidate_fg_id, Cint_field_fem_invalidate_fem_position_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_fem_action_delete\n", proc_name, rv);
        return rv;
    }

    rv = bcm_field_group_delete(unit, Cint_field_fem_invalidate_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_group_delete\n", proc_name, rv);
        return rv;
    }

    rv = bcm_field_qualifier_destroy(unit, Cint_field_fem_invalidate_udq);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_qualifier_destroy \n", proc_name, rv);
        return rv;
    }

    rv = bcm_field_action_destroy(unit, Cint_field_fem_invalidate_uda);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_action_destroy\n", proc_name, rv);
        return rv;
    }
    
    return rv;
}
