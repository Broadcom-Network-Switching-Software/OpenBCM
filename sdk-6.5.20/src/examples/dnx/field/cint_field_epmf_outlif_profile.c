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
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_tpid.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_unknown_l2uc.c
 * cint ../../src/examples/dnx/field/cint_field_ctest_config.c
 * cint ../../src/examples/dnx/field/cint_field_epmf_outlif_profile.c
 * cint;
 * int unit = 0;
 * bcm_field_context_t context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;
 * bcm_gport_t outlif_gport = 0x44802710;
 * int outlif_profile = 1;
 * uint8 is_rif = 0;
 * cint_field_epmf_outlif_profile_main(unit,context_id,outlif_gport,outlif_profile,is_rif);
 *
 * Configuration example end
 *
 *
 * Example of using ACL OUT LIF/RIF qualifiers which is encoded as GPORT.
 *      - LIF - bcmFieldQualifyOutVportClass
 *      - RIF - bcmFieldQualifyInterfaceClassL2
 */

bcm_field_group_t cint_epmf_outlif_profile_fg_id = 0;
bcm_field_entry_t cint_epmf_outlif_profile_ent_id;

/**
* \brief
*  Configures the written above
* \param [in] unit        -  Device id
* \param [in] context_id  -  PMF context Id
* \param [in] outlif_gport     -  Out-LIF Gport
* \param [in] outlif_profile -  Out-LIF profile to qualify upon
* \param [in] is_rif     -  If set, configure RIF FG
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
cint_field_epmf_outlif_profile_main(
    int unit,
    bcm_field_context_t context_id,
    bcm_gport_t outlif_gport,
    int outlif_profile,
    uint8 is_rif)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_action_info_t action_info;
    void *dest_char;
    uint32 outlif_profile_get;

    int rv = BCM_E_NONE;

    /* Set GLEM (if OUT_LIF gport)*/
    if (is_rif == 0)
    {
        rv = bcm_port_class_set(unit, outlif_gport, bcmPortClassFieldEgressVport, outlif_profile);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_port_class_set\n", rv);
            return rv;
        }

        /* Verify outlif_profile was set */
        rv = bcm_port_class_get(unit, outlif_gport, bcmPortClassFieldEgressVport, &outlif_profile_get);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_port_class_get\n", rv);
            return rv;
        }
        if (outlif_profile_get != outlif_profile)
        {
            printf("Value received from bcm_port_class_get (%d) does not match value set using bcm_port_class_set (%d)\n",
                    outlif_profile_get, outlif_profile);
            return BCM_E_FAIL;
        }
    }

    /*
     * Create and attach TCAM group in ePMF
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageEgress;
    fg_info.nof_quals = 1;
    if (is_rif)
    {
        /* ACL_OUT_RIF profile */
        fg_info.qual_types[0] = bcmFieldQualifyInterfaceClassL2;
    }
    else
    {
        /* ACL_OUT_LIF profile */
        fg_info.qual_types[0] = bcmFieldQualifyOutVportClass;
    }
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionStatId0;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "inlif_fg", sizeof(fg_info.name));

    printf("Creating first group\n");
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_epmf_outlif_profile_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    /*
     * Attach the FG to context
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    printf("Attaching field group\n");
    rv = bcm_field_group_context_attach(unit, 0, cint_epmf_outlif_profile_fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = 1;
    ent_info.entry_qual[0].mask[0] = 1;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = 0xabcd;

    printf("Adding Entry\n");
    rv = bcm_field_entry_add(unit, 0, cint_epmf_outlif_profile_fg_id, &ent_info, &cint_epmf_outlif_profile_ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    return 0;
}

/**
* \brief
*  Destroy all configuration done
* \param [in] unit        -  Device id
* \param [in] context_id  -  PMF context Id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
cint_field_epmf_outlif_profile_destroy(
    int unit,
    bcm_field_context_t context_id)
{
    int rv = BCM_E_NONE;

    /*
     * Delete entry from Field group 
     */
    rv = bcm_field_entry_delete(unit, cint_epmf_outlif_profile_fg_id, NULL, cint_epmf_outlif_profile_ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete entry %d from fg %d\n", rv, cint_epmf_outlif_profile_ent_id,
               cint_epmf_outlif_profile_fg_id);
        return rv;
    }
    /*
     * Detach the IPMF1 FG from its context 
     */
    rv = bcm_field_group_context_detach(unit, cint_epmf_outlif_profile_fg_id, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv,
               cint_epmf_outlif_profile_fg_id, context_id);
        return rv;
    }

    /*
     * Delete FG 
     */
    rv = bcm_field_group_delete(unit, cint_epmf_outlif_profile_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, cint_epmf_outlif_profile_fg_id);
        return rv;
    }

    return rv;
}

/*
 * Add 4 IPv4 routes with consequent OutRif values
 */
int
dnx_ip_route_multiple_rifs(
    int unit,
    int in_port,
    int out_port,
    int kaps_result,
    int outlif_profile)
{
    int rv;
    int intf_out = 100;         /* Outgoing packet ETH-RIF */
    uint32 host = 0x7fffff02;
    int vrf = 1;
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9A };    /* my-MAC */
    bcm_gport_t gport;
    bcm_gport_t intf_gport;
    int encap_id = 0x1384;
    int i;
    char *proc_name;
    uint32 outlif_profile_get;

    proc_name = "ip_route_multiple_rifs";

    /*
     * First create basic l3 route
     */
    rv = dnx_basic_example(unit, in_port, out_port, kaps_result);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n", proc_name);
        return rv;
    }

    BCM_GPORT_TUNNEL_ID_SET(intf_gport, intf_out);
    rv = bcm_port_class_set(unit, intf_gport, bcmPortClassFieldEgressVport, outlif_profile);
    if (rv != BCM_E_NONE)
    {
		printf("Error (%d), in bcm_port_class_set\n", rv);
		return rv;
    }

    /* Verify outlif_profile was set */
    rv = bcm_port_class_get(unit, intf_gport, bcmPortClassFieldEgressVport, &outlif_profile_get);
    if (rv != BCM_E_NONE)
    {
		printf("Error (%d), in bcm_port_class_get\n", rv);
		return rv;
    }
    if (outlif_profile_get != outlif_profile)
    {
        printf("Value received from bcm_port_class_get (%d) does not match value set using bcm_port_class_set (%d)\n",
                outlif_profile_get, outlif_profile);
        return BCM_E_FAIL;
    }

    /*
     * Now add 3 more Out RIFs with different hosts
     */
    for (i = 0; i < 3; i++)
    {
        intf_out_mac_address[5] = intf_out_mac_address[5] + 1;
        intf_out = intf_out + 1;
        host = host + 1;

        /*
         * Create ETH-RIF and set its properties
         */
        rv = intf_eth_rif_create(unit, intf_out, intf_out_mac_address);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, intf_eth_rif_create intf_out\n", proc_name);
            return rv;
        }

        /*
         * Add host entry
         */
        BCM_GPORT_LOCAL_SET(gport, out_port);
        rv = add_host_ipv4(unit, host, vrf, intf_out, encap_id /* arp id */ , gport);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function add_host_ipv4(), \n", proc_name);
            return rv;
        }

        BCM_GPORT_TUNNEL_ID_SET(intf_gport, intf_out);
        rv = bcm_port_class_set(unit, intf_gport, bcmPortClassFieldEgressVport, outlif_profile);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_port_class_set\n", rv);
            return rv;
        }
    }

    printf("%s(): Exit\r\n", proc_name);
    return rv;
}
