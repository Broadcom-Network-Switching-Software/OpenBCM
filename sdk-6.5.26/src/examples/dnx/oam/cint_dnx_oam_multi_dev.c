/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved. File: cint_dnx_oam_fec_injection.c
 *
 * Purpose: basic example for oam on multi device.
 *
 * Examples:
 *   - 1. Example of trapping CCM by upmep on 2nd device with IOP mode
 *
 */

/*
 * 1. Example of trapping CCM by upmep on 2nd device with IOP mode
 *
 * Packet flow:
 *   In this example, CCM is received on 1st device, and it's switch to 2nd device,
 *   then it's trapped by upmep on 2nd device, finally it's trapped to oamp.
 *
 * Note:
 *   In case 1st device is Jericho2 and 2nd device is Jericho, the system header profile should be updated.
 *   A CINT example is in cint_dnx_field_header_profile_update_example.
 *
 * Objects to be created:
 *   A oam group and ACC endpoint are created.
 *
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 *
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_oam.c
 * cint ../../src/./examples/dnx/oam/cint_dnx_oam_multi_dev.c
 * cint ../../src/./examples/dpp/internal/systems/cint_utils_jr2-comp-jr1-mode.c
 * cint
 * cint_dnx_oam_upmep_on_2nd_device_iop_example(1,2,200,200);
 * cint_dnx_field_header_profile_update_example(1);
 * exit;
 *
 * CCM switch from 1st unit(1) to 2nd unit(2), and trap by Up MEP on rx unit(2)
 *   tx 1 psrc=200 data=0x0000000101010102030405068100000a8902a00104460000000000ff010302abcd00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000200010100
 *
 * Call bcm_oam_endpoint_get with unit(2) to verify if port state was updated by CCM TLV
 *
 */

int vsi_id = 10;
int acOuterVlan_1 = 10;
int acOuterVlan_2 = 100;
int flags = 0;
bcm_gport_t outlif_on_rx;
bcm_oam_endpoint_t rmep_id;

uint8 dmac[6] = {0x00, 0x00, 0x00, 0x01, 0x01, 0x01};

int cint_dnx_field_header_profile_update_entry_add (
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_entry_t *entry_handle)
{
    bcm_field_entry_info_t ent_info;
    int idx;
    int rv;
    bcm_field_entry_info_t_init(&ent_info);

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = bcmFieldActionFabricHeaderSet;
    ent_info.entry_action[0].value[0] = bcmFieldSystemHeaderProfileFtmhPph;

    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = bcmFieldQualifyOamSubtype;
    ent_info.entry_qual[0].mask[0] = -1;
    ent_info.entry_qual[0].value[0] = 0;

    rv = bcm_field_entry_add(unit, 0, fg_id, &ent_info, entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }

    return rv;
}

/*
 * 1. Example of updating header profile to PPHoFTMH for CCMoEth packet on Jericho2 device
 *
 *   Note, In case 1st device is Jericho2 and 2nd device is Jericho,
 *   without this, the header profile is PPHoOTSHoFTMH, then it's wrongly handled on Jericho device
 *
 *  The packet meets following requirements
 *  - forward layer is Ethernet
 *  - forward layer +1 is Y1731
 *  - No trap code
 *  - oam sub type is 0
 */
int cint_dnx_field_header_profile_update_example (
    int unit)
{
    bcm_field_context_t context_id;
    bcm_field_context_info_t context_info;
    bcm_field_context_param_info_t context_param;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;

    bcm_field_group_t oam_fg_id;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    bcm_rx_trap_config_t trap_config;
    void *dest_char;
    int rv = BCM_E_NONE;
    int ii = 0, index = 0;
    int flags=0;
    int trap_code=0;
    bcm_field_entry_t entry_handle;

    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "OAM_hdr_prof_Ctx", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id);
    if (rv != BCM_E_NONE)
    {
      printf("Error (%d), in bcm_field_context_create\n", rv);
      return rv;
    }

    context_param.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    context_param.param_arg = 0;
    context_param.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerOffset1, 0);
    /**Context that was created for iPMF1 is also created for iPMF2*/
    rv = bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, context_id, &context_param);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_param_set oam_fg_id\n", rv);
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyOamSubtype;

    /* Set actions */
    fg_info.nof_actions = 1;

    fg_info.action_types[0] = bcmFieldActionFabricHeaderSet;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "OAM_Hdr_prof_Fg", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, 0, &fg_info, &oam_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add oam_fg_id\n", rv);
        return rv;
    }

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for(ii=0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for(ii=0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    rv = bcm_field_group_context_attach(unit, 0, oam_fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach ipmf1\n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = 100;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id;

    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapDefault, &trap_code);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_rx_trap_type_get \n", rv);
       return rv;
    }

    p_data.nof_qualifiers = 3;
    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeEth;
    p_data.qual_data[0].qual_mask = 0x1F;
    p_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[1].qual_arg = 1;
    p_data.qual_data[1].qual_value = bcmFieldLayerTypeY1731;
    p_data.qual_data[1].qual_mask = 0x1F;
    p_data.qual_data[2].qual_type = bcmFieldQualifyRxTrapCode;
    p_data.qual_data[2].qual_value = trap_code;
    p_data.qual_data[2].qual_mask = 0x1FF;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_presel_set \n", rv);
       return rv;
    }

    rv = cint_dnx_field_header_profile_update_entry_add(unit, oam_fg_id, &entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in cint_dnx_field_header_profile_update_entry_add \n", rv);
       return rv;
    }

    return rv;
}

/*
 * Setup L2 service to forward packet on 1st device to 2nd device, based on MAC address
 */
int cint_dnx_oam_upmep_on_2nd_device_iop_init(
    int tx_unit,
    int rx_unit,
    int in_port,
    int out_port)
{
    bcm_error_t rv;
    int inSP;
    int outSP;
    bcm_vlan_port_t vlan_port_on_tx;
    bcm_vlan_port_t vlan_port_on_rx;
    bcm_l2_addr_t l2addr;

    rv = port_to_system_port(tx_unit, in_port, &inSP);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), in port_to_system_port\n", rv);
        return rv;
    }

    rv = port_to_system_port(rx_unit, out_port, &outSP);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), in port_to_system_port\n", rv);
        return rv;
    }

    rv = bcm_vlan_gport_add(tx_unit, acOuterVlan_1, in_port, 0);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), in bcm_vlan_gport_add\n", rv);
        return rv;
    }

    rv = bcm_vlan_gport_add(tx_unit, acOuterVlan_2, out_port, 0);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), in bcm_vlan_gport_add\n", rv);
        return rv;
    }

    rv = bcm_port_class_set(tx_unit, in_port, bcmPortClassId, in_port);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), in bcm_port_class_set\n", rv);
        return rv;
    }

    rv = bcm_port_class_set(tx_unit, out_port, bcmPortClassId, out_port);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), in bcm_port_class_set\n", rv);
        return rv;
    }

    if (!is_device_or_above(tx_unit, JERICHO2)) {
        rv = bcm_port_tpid_delete_all(tx_unit, in_port);
        if (rv != BCM_E_NONE) {
            printf("Error(%d), in bcm_port_tpid_delete_all\n", rv);
            return rv;
        }
    }

    if (!is_device_or_above(rx_unit, JERICHO2)) {
        rv = bcm_port_tpid_delete_all(rx_unit, out_port);
        if (rv != BCM_E_NONE) {
            printf("Error(%d), in bcm_port_tpid_delete_all\n", rv);
            return rv;
        }

        rv = bcm_port_tpid_set(rx_unit, in_port, 0x8100);
        if (rv != BCM_E_NONE) {
            printf("Error(%d), in bcm_port_tpid_set\n", rv);
            return rv;
        }

        rv = bcm_port_tpid_set(rx_unit, out_port, 0x8100);
        if (rv != BCM_E_NONE) {
            printf("Error(%d), in bcm_port_tpid_set\n", rv);
            return rv;
        }
    }

    rv = bcm_vlan_gport_add(rx_unit, acOuterVlan_1, in_port, 0);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), in bcm_vlan_gport_add\n", rv);
        return rv;
    }

    rv = bcm_vlan_gport_add(rx_unit, acOuterVlan_2, out_port, 0);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), in bcm_vlan_gport_add\n", rv);
        return rv;
    }

    rv = bcm_port_class_set(rx_unit, in_port, bcmPortClassId, in_port);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), in bcm_port_class_set\n", rv);
        return rv;
    }

    rv = bcm_port_class_set(rx_unit, out_port, bcmPortClassId, out_port);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), in bcm_port_class_set\n", rv);
        return rv;
    }

    rv = bcm_vswitch_create_with_id(tx_unit, vsi_id);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), in bcm_vswitch_create_with_id\n", rv);
        return rv;
    }

    rv = bcm_vswitch_create_with_id(rx_unit, vsi_id);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), in bcm_vswitch_create_with_id\n", rv);
        return rv;
    }

    bcm_vlan_port_t_init(&vlan_port_on_tx);
    vlan_port_on_tx.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port_on_tx.port = inSP;
    vlan_port_on_tx.flags = BCM_VLAN_PORT_WITH_ID;
    vlan_port_on_tx.vlan_port_id = 0x44802004;
    vlan_port_on_tx.match_vlan = acOuterVlan_1;
    rv = bcm_vlan_port_create(tx_unit, &vlan_port_on_tx);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), in bcm_vlan_port_create\n", rv);
        return rv;
    }

    rv = bcm_vswitch_port_add(tx_unit, vsi_id, vlan_port_on_tx.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), in bcm_vlan_port_create\n", rv);
        return rv;
    }

    bcm_vlan_port_t_init(&vlan_port_on_rx);
    vlan_port_on_rx.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port_on_rx.port = outSP;
    vlan_port_on_rx.flags = BCM_VLAN_PORT_WITH_ID;
    vlan_port_on_rx.vlan_port_id = 0x44802006;
    vlan_port_on_rx.match_vlan = acOuterVlan_2;
    rv = bcm_vlan_port_create(rx_unit, &vlan_port_on_rx);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), in bcm_vlan_port_create\n", rv);
        return rv;
    }

    rv = bcm_vswitch_port_add(rx_unit, vsi_id, vlan_port_on_rx.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), in bcm_vswitch_port_add\n", rv);
        return rv;
    }

    outlif_on_rx = vlan_port_on_rx.vlan_port_id;

    bcm_l2_addr_t_init(&l2addr, dmac, vsi_id);
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = outSP;
    l2addr.encap_id = vlan_port_on_rx.encap_id;
    rv = bcm_l2_addr_add(tx_unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), in bcm_vswitch_port_add\n", rv);
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * 1. Example of trapping CCM by upmep on 2nd device with IOP mode
 */
int cint_dnx_oam_upmep_on_2nd_device_iop_example (
    int tx_unit,
    int rx_unit,
    int in_port,
    int out_port)
{
    bcm_error_t rv;
    bcm_oam_group_info_t group_info_long_ma_test;
    bcm_oam_group_info_t* group_info;
    bcm_oam_endpoint_info_t default_info;
    bcm_oam_group_info_t group_info_short_ma;
    bcm_mac_t mac_mep_2 = {0x00, 0x00, 0x00, 0xff, 0xff, 0xff};
    bcm_mac_t mac_mep_3 = {0x00, 0x11, 0x00, 0x22, 0xff, 0xff};
    int md_level_2 = 5;
    uint8 short_name[BCM_OAM_GROUP_NAME_LENGTH] = { 1, 3, 2, 0xab, 0xcd };

    unsigned int flags2 = 0;
    int ingress_profile = 1;
    int egress_profile  = 1;
    int acc_ingress_profile = 1;
    int acc_egress_profile = 1;
    int oamp_up_mep_trap_id;
    bcm_gport_t oamp_dest_gport;
    bcm_oam_action_key_t action_key;
    bcm_oam_action_result_t action_result_get;
    bcm_oam_action_result_t action_result;

    rv = cint_dnx_oam_upmep_on_2nd_device_iop_init(tx_unit, rx_unit, in_port, out_port);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), in cint_dnx_oam_upmep_on_2nd_device_iop_init\n", rv);
        return rv;
    }

    if (is_device_or_above(rx_unit, JERICHO2)) {
        rv = bcm_oam_profile_id_get_by_type(rx_unit, ingress_profile, bcmOAMProfileIngressLIF, &flags2, &ingress_profile);
        if (rv != BCM_E_NONE) {
            printf("Error(%d), in bcm_oam_profile_id_get_by_type\n", rv);
            return rv;
        }

        rv = bcm_oam_profile_id_get_by_type(rx_unit, egress_profile, bcmOAMProfileEgressLIF, &flags2, &egress_profile);
        if (rv != BCM_E_NONE) {
            printf("Error(%d), in bcm_oam_profile_id_get_by_type\n", rv);
            return rv;
        }

        rv = bcm_oam_profile_id_get_by_type(rx_unit, acc_ingress_profile, bcmOAMProfileIngressAcceleratedEndpoint, &flags2, &acc_ingress_profile);
        if (rv != BCM_E_NONE) {
            printf("Error(%d), in bcm_oam_profile_id_get_by_type\n", rv);
            return rv;
        }

        rv = bcm_oam_profile_id_get_by_type(rx_unit, acc_egress_profile, bcmOAMProfileEgressAcceleratedEndpoint, &flags2, &acc_egress_profile);
        if (rv != BCM_E_NONE) {
            printf("Error(%d), in bcm_oam_profile_id_get_by_type\n", rv);
            return rv;
        }

        rv = bcm_oam_lif_profile_set(rx_unit, 0, outlif_on_rx, ingress_profile, egress_profile);
        if (rv != BCM_E_NONE) {
            printf("Error(%d), in bcm_oam_lif_profile_set\n", rv);
            return rv;
        }

        rv = bcm_rx_trap_type_get(rx_unit, 0, bcmRxTrapEgTxOamUpMEPOamp, &oamp_up_mep_trap_id);
        if (rv != BCM_E_NONE) {
            printf("Error(%d), in bcm_rx_trap_type_create\n", rv);
            return rv;
        }

        BCM_GPORT_TRAP_SET(oamp_dest_gport, oamp_up_mep_trap_id, 7, 0);

        bcm_oam_action_key_t_init(&action_key);
        bcm_oam_action_result_t_init(&action_result_get);

        action_key.flags = 0;
        action_key.inject = 0;
        action_key.opcode = bcmOamOpcodeCCM;
        action_key.endpoint_type = bcmOAMMatchTypeMEP;
        action_key.dest_mac_type = bcmOAMDestMacTypeMyCfmMac;

        rv = bcm_oam_profile_action_get(rx_unit, 0, acc_egress_profile, &action_key, &action_result_get);
        if (rv != BCM_E_NONE) {
            printf("Error(%d), in bcm_oam_profile_action_get\n", rv);
            return rv;
        }

        action_result = action_result_get;
        action_result.destination = oamp_dest_gport;

        rv = bcm_oam_profile_action_set(rx_unit, 0, acc_egress_profile, &action_key, &action_result);
        if (rv != BCM_E_NONE) {
            printf("Error(%d), in bcm_oam_profile_action_get\n", rv);
            return rv;
        }

        rv = oam__my_cfm_mac_set(rx_unit, out_port, dmac);
        if (rv != BCM_E_NONE) {
            printf("oam__my_cfm_mac_set fail \n");
            return rv;
        }
    }

    printf("Creating short group\n");
    bcm_oam_group_info_t_init(&group_info_short_ma);
    sal_memcpy(group_info_short_ma.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);

    rv = bcm_oam_group_create(rx_unit, &group_info_short_ma);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), in bcm_oam_group_create\n", rv);
        return rv;
    }

    group_info = &group_info_short_ma;
    unsigned int fla1;

    bcm_oam_endpoint_info_t mep_acc_info;
    bcm_oam_endpoint_info_t rmep_info;
    bcm_oam_endpoint_info_t_init(&mep_acc_info);
    mep_acc_info.endpoint_memory_type = bcmOamEndpointMemoryTypeSelfContained;
    mep_acc_info.type = bcmOAMEndpointTypeEthernet;
    mep_acc_info.group = group_info->id;
    mep_acc_info.level = md_level_2;
    mep_acc_info.tx_gport = BCM_GPORT_INVALID; /*Up MEP requires gport invalid.*/
    mep_acc_info.name = 123;
    mep_acc_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
    mep_acc_info.flags |= BCM_OAM_ENDPOINT_UP_FACING;
    mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;

    mep_acc_info.vlan = 5;
    mep_acc_info.pkt_pri = 4;
    mep_acc_info.outer_tpid = 0x8100;
    mep_acc_info.inner_vlan = 0;
    mep_acc_info.inner_pkt_pri = 0;
    mep_acc_info.inner_tpid = 0;
    mep_acc_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_RX_DISABLE;
    sal_memcpy(mep_acc_info.src_mac_address, mac_mep_2, 6);
    if (is_device_or_above(rx_unit, JERICHO2)) {
        mep_acc_info.acc_profile_id = acc_egress_profile;
    } else {
        sal_memcpy(mep_acc_info.dst_mac_address, mac_mep_3, 6);
    }
    mep_acc_info.gport = outlif_on_rx;
    printf("bcm_oam_endpoint_create mep_acc_info\n");
    rv = bcm_oam_endpoint_create(rx_unit, &mep_acc_info);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), in bcm_oam_endpoint_create\n", rv);
        return rv;
    }


    bcm_oam_endpoint_info_t_init(&rmep_info);
    rmep_info.name = 0xff;
    rmep_info.local_id = mep_acc_info.id;
    rmep_info.ccm_period = 0;
    rmep_info.flags |= BCM_OAM_ENDPOINT_REMOTE;
    rmep_info.loc_clear_threshold = 1;
    rmep_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
    rmep_info.id = mep_acc_info.id;
    rmep_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI | BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC;
    printf("bcm_oam_endpoint_create RMEP\n");
    rv = bcm_oam_endpoint_create(rx_unit, &rmep_info);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), in bcm_oam_endpoint_create\n", rv);
        return rv;
    }
    rmep_id = rmep_info.id;

    printf("created RMEP with id %d\n", rmep_info.id);

    return BCM_E_NONE;
}

