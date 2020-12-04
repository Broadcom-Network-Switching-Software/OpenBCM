/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_mtu_filtering.c
 * Purpose: Example for MTU filtering configuration.
 */


 /*
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/dpp/cint_multi_device_utils.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ip_route_scenarios.c
 * cint ../../src/examples/sand/cint_mtu_filtering.c
 * cint
 * mtu_check_setup(units_ids,1,1811939529,1811939530,1500,1500);
 * exit;
 *
 * tx 1 psrc=201 data=0x000c000200000000070001008100000f08004508002e000040004006b8bd020101017fffff033344556600a1a2a300b1b2b3500001f57ebf0000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x00000000cd1d000c000200008100806408004508002e000040003f06b9bd020101017fffff033344556600a1a2a300b1b2b3500001f57ebf0000f5f5f5f5f5f5f5f5f5f5
 */

bcm_field_group_config_t mtu_check_grp_de;
bcm_field_entry_t mtu_check_ent_de;
int mtu_check_presel_id = 0;
int mtu_check_data_qual_id;
int mtu_check_trap_id;
int compressed_layer_type = 2;

/*
 * trap configuration for mtu check
 *   On JER1, bcmRxTrapEgMtuFilter is used
 *   On JR2, bcmRxTrapEgMtuFilter is not supported, and support mtu trap in ETPP.
 */
int
mtu_check_trap_config (
    int unit)
{
    int res;
    bcm_rx_trap_config_t trap_config;

    /* set egress Action-Profile-TTL-Scope trap to drop all packets MC with TTL/hop-limit that is lower than the threshold */
    res = bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgMtuFilter, &mtu_check_trap_id);
    if (res != BCM_E_NONE) {
        printf("Error. bcm_rx_trap_type_create failed: $res\n", res);
        return BCM_E_FAIL;
    }

    sal_memset(&trap_config, 0, sizeof(trap_config));
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    trap_config.dest_port = BCM_GPORT_BLACK_HOLE; /* drop */

    res = bcm_rx_trap_set(unit, mtu_check_trap_id, &trap_config);
    if (res != BCM_E_NONE) {
        printf("Error. bcm_rx_trap_set failed: $res\n", res);
        return BCM_E_FAIL;
    }

    return res;
}

/*
 * header configuration
 *   On JER1, Enable MTU filtering for header code; Set MTU size for profile
 *   On JR2, Creating a compressed layer type
 */
int
mtu_check_header_code_enable (
    int unit,
    int header_code)
{
    int res;
    bcm_l3_intf_t l3if, l3if_ori;
    bcm_switch_control_key_t mtu_enable_type, mtu_size_type, mtu_cfg_type;
    bcm_switch_control_info_t mtu_enable_info, mtu_size_info, mtu_cfg_info;
    int profile_index;

    if (is_device_or_above(unit, JERICHO2))
    {
        /* Creating a compressed layer type */
        mtu_cfg_type.type = bcmSwitchLinkLayerMtuFilter;
        mtu_cfg_type.index = header_code;
        mtu_cfg_info.value = compressed_layer_type;

        res = bcm_switch_control_indexed_set(unit, mtu_cfg_type, mtu_cfg_info);
        if(res != BCM_E_NONE) {
            return res;
        }
    }
    else
    {
        /* Enable Forwarding Layer MTU filtering for header code*/
        mtu_enable_type.type  = bcmSwitchForwardingLayerMtuFilter;
        mtu_enable_type.index = header_code;
        mtu_enable_info.value = TRUE;

        res = bcm_switch_control_indexed_set(unit, mtu_enable_type, mtu_enable_info);
        if (res != BCM_E_NONE) {
            return res;
        }

        /* Enable Forwarding Layer MTU filtering for header code*/
        mtu_enable_type.type  = bcmSwitchLinkLayerMtuFilter;
        mtu_enable_type.index = header_code;
        mtu_enable_info.value = TRUE;

        res = bcm_switch_control_indexed_set(unit, mtu_enable_type, mtu_enable_info);
        if (res != BCM_E_NONE) {
            return res;
        }

        for (profile_index = 1; profile_index <= 3; profile_index++)
        {
            /* Set Forwarding Layer MTU size for profile index */
            mtu_size_type.type  = bcmSwitchForwardingLayerMtuSize;
            mtu_size_type.index = profile_index;
            mtu_size_info.value = default_mtu_forwarding;

            res = bcm_switch_control_indexed_set(unit, mtu_size_type, mtu_size_info);
            if (res != BCM_E_NONE) {
                return res;
            }

            /* Set Link Layer MTU size for profile index */
            mtu_size_type.type  = bcmSwitchLinkLayerMtuSize;
            mtu_size_type.index = profile_index;
            mtu_size_info.value = default_mtu;

            res = bcm_switch_control_indexed_set(unit, mtu_size_type, mtu_size_info);
            if (res != BCM_E_NONE) {
                return res;
            }
        }
    }

    return res;
}


int
mtu_check_field_set_data_qual (
    bcm_field_data_qualifier_t *data_qual)
{
    bcm_field_data_qualifier_t_init(data_qual);
    data_qual->flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
    data_qual->offset = 0;
    data_qual->qualifier =  bcmFieldQualifyConstantOne;
    data_qual->length = 2;
    data_qual->stage = bcmFieldStageIngress;

    return BCM_E_NONE;
}

int
mtu_check_create_field_group (
    int unit)
{
    int result;
    bcm_field_aset_t aset;
    bcm_field_presel_set_t psset;
    bcm_field_data_qualifier_t data_qual_de;
    bcm_field_extraction_action_t action;
    bcm_field_extraction_field_t extract[1];
    int presel_flags = 0;

    /* ********************************************************** */
    /* Preselector Configuration                                  */
    /* ********************************************************** */

    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, mtu_check_presel_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            auxRes = bcm_field_presel_destroy(unit, mtu_check_presel_id | presel_flags);
            return result;
        }
    } else {
        result = bcm_field_presel_create(unit, &mtu_check_presel_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create\n");
            auxRes = bcm_field_presel_destroy(unit, mtu_check_presel_id);
            return result;
        }
    }

    result = bcm_field_qualify_Stage(unit, mtu_check_presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageIngress);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_Stage\n");
        return result;
    }

    result = bcm_field_qualify_HeaderFormat(unit, mtu_check_presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldHeaderFormatIp4AnyL2L3);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_HeaderFormat\n");
        return result;
    }

    BCM_FIELD_PRESEL_INIT(psset);
    BCM_FIELD_PRESEL_ADD(psset, mtu_check_presel_id);


    /* ********************************************************** */
    /* Direct Extraction Field Group                              */
    /* ********************************************************** */

    bcm_field_group_config_t_init(&mtu_check_grp_de);
    mtu_check_grp_de.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    mtu_check_grp_de.mode = bcmFieldGroupModeDirectExtraction;
    mtu_check_grp_de.priority = 10;
    mtu_check_grp_de.preselset = psset;

    BCM_FIELD_QSET_ADD(mtu_check_grp_de.qset, bcmFieldQualifyStageIngress);

    /* We're using UDF in order to extract only two bits from External Lookup value. */
    result = mtu_check_field_set_data_qual(&data_qual_de);
    if (BCM_E_NONE != result) {
        printf("Error in mtu_check_field_set_data_qual\n");
        return result;
    }

    result = bcm_field_data_qualifier_create(unit, &data_qual_de);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_data_qualifier_create\n");
        return result;
    }

    mtu_check_data_qual_id = data_qual_de.qual_id;

    result = bcm_field_qset_data_qualifier_add(unit, &(mtu_check_grp_de.qset), data_qual_de.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add\n");
        return result;
    }

    /*  Create the Field group with type Direct Extraction */
    result = bcm_field_group_config_create(unit, &mtu_check_grp_de);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_config_create for group %d\n", mtu_check_grp_de.group);
        return result;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionVrfSet);

    /*  Attach the action set */
    result = bcm_field_group_action_set(unit, mtu_check_grp_de.group, aset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set for group %d\n", mtu_check_grp_de.group);
        return result;
    }

    /* ********************************************************** */
    /* Add Entry                                                  */
    /* ********************************************************** */

    result = bcm_field_entry_create(unit, mtu_check_grp_de.group, &mtu_check_ent_de);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }

    bcm_field_extraction_action_t_init(&action);
    bcm_field_extraction_field_t_init(&extract[0]);

    action.action = bcmFieldActionVrfSet;

    extract[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    extract[0].bits  = 2;
    extract[0].lsb   = 0;
    extract[0].qualifier = data_qual_de.qual_id;

    result = bcm_field_direct_extraction_action_add(unit,
                                                  mtu_check_ent_de,
                                                  action,
                                                  1 /* count */,
                                                  &extract);

    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return result;
    }

    /* Write entry to HW */
    result = bcm_field_entry_install(unit, mtu_check_ent_de);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_install\n");
        return result;
    }

    return result;
} /* mtu_check_create_field_group */


int
mtu_check_setup_destroy (
    int unit)
{
    int res;

    if(!is_device_or_above(unit, JERICHO))
    {
        res = bcm_field_entry_destroy(unit, mtu_check_ent_de);
        if (res != BCM_E_NONE) {
            return res;
        }

        res = bcm_field_group_destroy(unit, mtu_check_grp_de.group);
        if (res != BCM_E_NONE) {
            return res;
        }

        res = bcm_field_presel_destroy(unit, mtu_check_presel_id);
        if (res != BCM_E_NONE) {
            return res;
        }

        res = bcm_field_data_qualifier_destroy(unit, mtu_check_data_qual_id);
        if (res != BCM_E_NONE) {
            return res;
        }
    }

    if (is_device_or_above(unit, JERICHO2)) {
        res = mtu_check_etpp_rif_set(unit, 100/*out_rif*/, compressed_layer_type, 0/*mtu*/, 0/*is_set*/);
        if (res != BCM_E_NONE) {
            return res;
        }
    }
    else
    {
        res = bcm_rx_trap_type_destroy(unit, mtu_check_trap_id);
        if (res != BCM_E_NONE) {
            return res;
        }
    }

    return res;
}

int
mtu_check_setup (
    int *unit_ids,
    int nof_units,
    int in_sysport,
    int out_sysport,
    int forwarding_mtu,
    int link_mtu)
{
    int res;
    int unit;
    int i;
    int ipv4_layer_protocol;
    int header_code = bcmForwardingTypeIp4Ucast;

    default_mtu_forwarding = forwarding_mtu;
    default_mtu = link_mtu;

    for (i = 0 ; i < nof_units ; i++)
    {
        unit = unit_ids[i];

        if (is_device_or_above(unit, JERICHO2))
        {
            dnx_dbal_fields_enum_value_get(unit, "LAYER_TYPES", "IPV4", &ipv4_layer_protocol);
        }
        else
        {
            ipv4_layer_protocol = 2;
        }

        /* Configure MTU filters */
        if (is_device_or_above(unit, JERICHO2)) {
            header_code = ipv4_layer_protocol;
        }
        res = mtu_check_header_code_enable(unit, header_code);
        if (res != BCM_E_NONE) {
            return res;
        }

        if (!is_device_or_above(unit, JERICHO2)) {
            res = mtu_check_trap_config(unit);
            if (res != BCM_E_NONE) {
                return res;
            }
        }

        /* Configure PMF field group */
        if(!is_device_or_above(unit, JERICHO))
        {
            res = mtu_check_create_field_group(unit);
            if (res != BCM_E_NONE) {
            return res;
            }
        }
    }

    /* Configure L3 interfaces */
    res = basic_example_multi_units(unit_ids, nof_units, in_sysport, out_sysport);
    if (res != BCM_E_NONE) {
        return res;
    }

    return res;
}

int
mtu_check_setup_single_unit(
    int unit,
    int in_sysport,
    int out_sysport,
    int forwarding_mtu,
    int link_mtu)
{
    int units_ids[] = {unit};
    return mtu_check_setup(units_ids, 1, in_sysport, out_sysport, forwarding_mtu, link_mtu);
}

