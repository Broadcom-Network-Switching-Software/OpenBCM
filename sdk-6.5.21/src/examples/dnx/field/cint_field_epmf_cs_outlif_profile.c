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
 * cint ../../src/examples/sand/utility/cint_sand_utils_tpid.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_unknown_l2uc.c
 * cint ../../src/examples/dnx/field/cint_field_ctest_config.c
 * cint ../../src/examples/dnx/field/cint_field_epmf_cs_outlif_profile.c
 * cint;
 * int unit = 0;
 * bcm_gport_t outlif_gport = 0x44802710;
 * int outlif_profile = 1;
 * cint_field_epmf_cs_outlif_profile_main(unit, outlif_gport, outlif_profile);
 *
 * int unit = 0;
 * int in_port = 200;
 * int out_port = 201;
 * int outrif_profile = 1;
 * cint_field_epmf_cs_outrif_profile_main(unit, in_port, out_port, outrif_profile);
 * Configuration example end
 *
 *
 * Example of using ACL OUT LIF/RIF context select qualifiers which is encoded as GPORT.
 *      - LIF - bcmFieldQualifyOutVportClass
 *      - RIF - bcmFieldQualifyInterfaceClassL2
 */

 /**
  * Global variables
  */
bcm_field_group_t epmf_cs_outlif_profile_fg_id = BCM_FIELD_ID_INVALID;
bcm_field_entry_t epmf_cs_outlif_profile_entry_id = BCM_FIELD_ID_INVALID;
bcm_field_context_t epmf_cs_outlif_profile_context_id = BCM_FIELD_ID_INVALID;
bcm_field_presel_t  epmf_cs_outlif_profile_presel_id = 50;
bcm_field_stage_t epmf_cs_outlif_profile_stage = bcmFieldStageEgress;
uint32 epmf_cs_outlif_profile_ipv6_sip[BCM_FIELD_QUAL_WIDTH_IN_WORDS];
uint32 epmf_cs_outlif_profile_ipv6_dip[BCM_FIELD_QUAL_WIDTH_IN_WORDS];
uint32 epmf_cs_outlif_profile_action_value = 0xabcd;


/**
* \brief
*  This function creates context with CS qualifier OutVportClass/InterfaceClassL2
* \param [in] unit           -  Device id
* \param [in] outlif_profile -  Out-LIF/RIF profile to qualify upon
* \param [in] is_lif         -  Qualify upon LIF or RIF
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
cint_field_epmf_cs_outlif_profile_ctx_create(
    int unit,
    int profile,
    int is_lif)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    void * dest_char;
    int rv = BCM_E_NONE;

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "outlif_profile", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, epmf_cs_outlif_profile_stage, &context_info, &epmf_cs_outlif_profile_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }
    printf("Created context (%d)  \n", epmf_cs_outlif_profile_context_id);

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = epmf_cs_outlif_profile_presel_id;
    p_id.stage = epmf_cs_outlif_profile_stage;
    p_data.entry_valid = TRUE;
    p_data.context_id = epmf_cs_outlif_profile_context_id;
    p_data.nof_qualifiers = 1;

    if (is_lif)
    {
        p_data.qual_data[0].qual_type = bcmFieldQualifyOutVportClass;
    }
    else
    {
        p_data.qual_data[0].qual_type = bcmFieldQualifyInterfaceClassL2;
    }
    p_data.qual_data[0].qual_value = profile;
    p_data.qual_data[0].qual_mask = 0x1;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set \n", rv);
        return rv;
    }

    printf("Presel (%d) was configured for context(%d) \n", epmf_cs_outlif_profile_presel_id, epmf_cs_outlif_profile_context_id);

    return rv;
}

/**
* \brief
*  This function creates TCAM fg, attaches it and adds an entry
* \param [in] unit           -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
cint_field_epmf_cs_outlif_profile_fg_create(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_entry_info_t entry_info;
    bcm_field_action_info_t action_info;
    void *dest_char;
    int ii;
    int rv = BCM_E_NONE;

    /*
     * Create and attach TCAM group in EPMF
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = epmf_cs_outlif_profile_stage;
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = bcmFieldQualifySrcIp6;
    fg_info.qual_types[1] = bcmFieldQualifyDstIp6;

    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionStatId0;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "outlif_fg", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &epmf_cs_outlif_profile_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d)  \n", epmf_cs_outlif_profile_fg_id);

    /*
     * Attach the FG to context
     */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[1].input_arg = 1;
    attach_info.key_info.qual_info[1].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, epmf_cs_outlif_profile_fg_id, epmf_cs_outlif_profile_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached  FG (%d) to context (%d)\n", epmf_cs_outlif_profile_fg_id, epmf_cs_outlif_profile_context_id);

    /*
     * Add entry
     */
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[1].type = fg_info.qual_types[1];
    for (ii = 0; ii < BCM_FIELD_QUAL_WIDTH_IN_WORDS; ii++)
    {
        entry_info.entry_qual[0].value[ii] = epmf_cs_outlif_profile_ipv6_sip[ii];
        entry_info.entry_qual[0].mask[ii] = 0xffffffff;
        entry_info.entry_qual[1].value[ii] = epmf_cs_outlif_profile_ipv6_dip[ii];
        entry_info.entry_qual[1].mask[ii] = 0xffffffff;
    }
    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = epmf_cs_outlif_profile_action_value;

    rv = bcm_field_entry_add(unit, 0, epmf_cs_outlif_profile_fg_id, &entry_info, &epmf_cs_outlif_profile_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    printf("Entry 0x%X (%d) was added to field group %d. \n", epmf_cs_outlif_profile_entry_id, epmf_cs_outlif_profile_entry_id, epmf_cs_outlif_profile_fg_id);

    return rv;
}

/**
* \brief
*  Destroy all configuration
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
cint_field_epmf_cs_outlif_profile_destroy(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = epmf_cs_outlif_profile_presel_id;
    p_id.stage = epmf_cs_outlif_profile_stage;
    p_data.entry_valid = FALSE;
    p_data.context_id = epmf_cs_outlif_profile_context_id;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    /*
     * Delete entry from Field group
     */
    rv = bcm_field_entry_delete(unit, epmf_cs_outlif_profile_fg_id, NULL, epmf_cs_outlif_profile_entry_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_delete\n", rv);
       return rv;
    }
    printf("Entry delete: id:(0x%x) from field group %d.\n", epmf_cs_outlif_profile_entry_id, epmf_cs_outlif_profile_fg_id);

    /*
     * Detach the TCAM FG from its context
     */
    rv = bcm_field_group_context_detach(unit, epmf_cs_outlif_profile_fg_id, epmf_cs_outlif_profile_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }
    printf("TCAM Field Group %d was detach from context %d. \n", epmf_cs_outlif_profile_fg_id, epmf_cs_outlif_profile_context_id);

    /*
     * Delete FG
     */
    rv = bcm_field_group_delete(unit, epmf_cs_outlif_profile_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_delete\n");
        return rv;
    }
    printf("TCAM Field Group %d was deleted. \n", epmf_cs_outlif_profile_fg_id);

    /*
     * Delete the context
     */
    rv = bcm_field_context_destroy(unit, epmf_cs_outlif_profile_stage, epmf_cs_outlif_profile_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy context_id %d \n", rv, epmf_cs_outlif_profile_context_id);
        return rv;
    }
    return rv;
}

/**
* \brief
*  This function sets outlif profile
* \param [in] unit           -  Device id
* \param [in] outlif_gport   -  Out-LIF Gport
* \param [in] outlif_profile -  Out-LIF profile to qualify upon
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_epmf_cs_outlif_profile_set_outlif_profile(
    int unit,
    bcm_gport_t outlif_gport,
    int outlif_profile)
{
    int rv;
    rv = bcm_port_class_set(unit, outlif_gport, bcmPortClassFieldEgressVport, outlif_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_port_class_set\n", rv);
        return rv;
    }
    return rv;
}

/**
* \brief
*  This function sets outrif profile
* \param [in] unit           -  Device id
* \param [in] in_port        -  In port
* \param [in] out_port       -  Out port
* \param [in] outrif_profile -  Out-RIF profile to qualify upon
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_epmf_cs_outlif_profile_set_outrif_profile(
    int unit,
    int in_port,
    int out_port,
    int outrif_profile)
{
    int rv;
    int intf_out = 100;
    bcm_gport_t intf_gport;

    rv = basic_example_ipv6(unit, in_port, out_port, 0, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("(): Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    BCM_GPORT_TUNNEL_ID_SET(intf_gport, intf_out);
    rv = bcm_port_class_set(unit, intf_gport, bcmPortClassFieldEgressVport, outrif_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_port_class_set\n", rv);
        return rv;
    }
    return rv;
}


/**
* \brief
*  Runs main function for outlif CS
* \param [in] unit           -  Device id
* \param [in] outlif_gport   -  Out-LIF Gport
* \param [in] outlif_profile -  Out-LIF profile to qualify upon
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
cint_field_epmf_cs_outlif_profile_main(
    int unit,
    bcm_gport_t outlif_gport,
    int outlif_profile)
{
    int rv = BCM_E_NONE;

    rv = cint_field_epmf_cs_outlif_profile_set_outlif_profile(unit, outlif_gport, outlif_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_epmf_cs_outlif_profile_set_outlif_profile\n", rv);
        return rv;
    }

    rv = cint_field_epmf_cs_outlif_profile_ctx_create(unit, outlif_profile, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_epmf_cs_outlif_profile_ctx_create\n", rv);
        return rv;
    }

    rv = cint_field_epmf_cs_outlif_profile_fg_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_epmf_cs_outlif_profile_fg_create\n", rv);
        return rv;
    }

    return rv;
}

/**
* \brief
*  Runs main function for outrif CS
* \param [in] unit           -  Device id
* \param [in] in_port        -  In port
* \param [in] out_port       -  Out port
* \param [in] outrif_profile -  Out-RIF profile to qualify upon
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
cint_field_epmf_cs_outrif_profile_main(
    int unit,
    int in_port,
    int out_port,
    int outrif_profile)
{
    int rv = BCM_E_NONE;

    rv = cint_field_epmf_cs_outlif_profile_set_outrif_profile(unit, in_port, out_port, outrif_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_epmf_cs_outlif_profile_set_outrif_profile\n", rv);
        return rv;
    }

    rv = cint_field_epmf_cs_outlif_profile_ctx_create(unit, outrif_profile, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_epmf_cs_outlif_profile_ctx_create\n", rv);
        return rv;
    }

    rv = cint_field_epmf_cs_outlif_profile_fg_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_epmf_cs_outlif_profile_fg_create\n", rv);
        return rv;
    }

    return rv;
}
