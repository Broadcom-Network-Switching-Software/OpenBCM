/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_dnx_oam_eth_o_pwe_o_eth.c
 *
 * Purpose: basic example for oam over native ethernet.
 *
 * Note:
 *   OAM over native Ethernet over PWE for unindexed LIF as data packet or oam packet. 
 *   Following call sequence is needed to idendify oam over native over pwe as data packet, reference code can be found in appl_ref_oam_appl_init.c
 *     - Set SOC in_lif_profile_allocate_indexed_mode to 2
 *     - Call bcm_field_tcam_bank_add with bcmFieldAppDbOamIdentification to create TCAM BANK for OAM identification
 *     - Call bcm_oam_control_set with type=bcmOamControlNativeEthernetOverPweClassification and arg=1
 *
 * Examples:
 *   - 1. Example of testing OAM over native ethernet with PWE raw mode
 *   - 2. Example of testing OAM over native ethernet and PWE OAM with PWE raw mode
 *   - 3. Example of testing OAM over native ethernet with PWE unindexed mode
 */

/*
 * 1. Example of testing OAM over native ethernet with PWE raw mode
 *
 * Purpose:
 *   In this example, pwe termination with raw mode. It shows how to identify OAM over native ethernet as oam packet and as data packet. 
 *   1) By default, expect OAM over native ethernet is not marked as oam packet, and
 *   the packet can be trapped by upmep on AC if upmep is defined.
 *   2) After calling API bcm_oam_control_set with bcmOamControlNativeEthernetOverPweClassification,
 *   the packet is trapped by downmep on PWE LIF.
 *
 * Objects to be created:
 *   A VPLS model with pwe raw mode is setup, and
 *   oam group, lif proflie and one down mep on pwe lif and one up mep on AC are created,
 *
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/sand/cint_ip_route_basic.c
 * cint ../../src/./examples/sand/cint_vpls_mp_basic.c
 * cint ../../src/./examples/sand/cint_sand_advanced_vlan_translation_mode.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_oam.c
 * cint ../../src/./examples/dnx/oam/cint_oam_basic.c
 * cint ../../src/./examples/dnx/crps/cint_crps_oam_config.c
 * cint ../../src/./examples/dnx/oam/cint_oam_action.c
 * cint ../../src/./examples/dnx/oam/cint_dnx_oam_eth_o_pwe_o_eth.c
 * cint
 * add_upmep_on_egress_ac=1;
 * oam_run_over_inner_eth_with_pwe(0,200,201,1);
 * exit;
 *
 * 1. Send CCMoETH2oPWEoETH0 packet, expect the packet is trapped in egress by upmep on AC
 *    tx 1 psrc=201 data=0x000c0002000100000000cd1d884700d0504000d80140001100000112000c000200009100000081000005890260010346000000001000010302abcd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * 2. Received packets on unit 0 should be:
 *    Source port: 201, Destination port: 0
 *    Data: 0x029800648000010013a4000000000000000018000a0138780202058300982e4e0500001100000112000c000200009100000081000005890260010346000000001000010302abcd0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * 3. Call API bcm_oam_control_set with bcmOamControlNativeEthernetOverPweClassification to clear tcam entry in oam tcam identification DB.
 *    then oam over native ethernet will be marked as data packet in ingress
 *
 * 4. Send CCMoETH2oPWEoETH0 packet, expect the packet is trapped in ingress by dnmep on PWE lif
 *    tx 1 psrc=201 data=0x000c0002000100000000cd1d884700d0504000d80140001100000112000c000200009100000081000005890260010346000000001000010302abcd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * 5. Received packets on unit 0 should be:
 *    Source port: 201, Destination port: 0
 *    Data: 0x02f400648000010013e0b844898fe01128100018000a01387802020b0100942f0fd500000c0002000100000000cd1d884700d0504000d80140001100000112000c000200009100000081000005890260010346000000001000010302abcd00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 */

/*
 * 2. Example of testing OAM over native ethernet and PWE OAM with PWE raw mode
 *
 * Purpose:
 *   In this example, pwe termination with raw mode.
 *   1) OAM over native ethernet is not marked as oam packet, and the packet will transmitted as data packet.
 *   2) PWE OAM packet will be trapped by downmep on PWE LIF.
 *
 * Objects to be created:
 *   A VPLS model with pwe raw mode is setup, and
 *   oam group, lif proflie and one down mep on pwe lif are created,
 *
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/sand/cint_ip_route_basic.c
 * cint ../../src/./examples/sand/cint_vpls_mp_basic.c
 * cint ../../src/./examples/sand/cint_sand_advanced_vlan_translation_mode.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_oam.c
 * cint ../../src/./examples/dnx/oam/cint_oam_basic.c
 * cint ../../src/./examples/dnx/crps/cint_crps_oam_config.c
 * cint ../../src/./examples/dnx/oam/cint_oam_action.c
 * cint ../../src/./examples/dnx/oam/cint_dnx_oam_eth_o_pwe_o_eth.c
 * cint
 * cint_vpls_basic_info.cw_present=1;
 * oam_run_over_inner_eth_with_pwe(0,200,201,1);
 * exit;
 *
 * 1. Send PWE OAM (with 0x8902 in special offset) packet, expect the packet is trapped by down mep on PWE LIF
 *    tx 1 psrc=201 data=0x000c0002000100000000cd1d81000000884700d0504000d801401000890260010346000000001000010302abcd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * 2. Received packets on unit 0 should be:
 *    Source port: 201, Destination port: 0
 *    Data: 0x02bc00648000000003e07e224785c477f4100018000a0138780202078100942f0fd500000c0002000100000000cd1d81000000884700d0504000d801401000890260010346000000001000010302abcd00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * 3. Send CFM over PWE (Control word is all zero) packet, expect the packet is forwarded as data
 *    tx 1 psrc=201 data=0x000c0002000100000000cd1d884700d0504000d8014000000000001100000112000c000200009100000081000005890260010346000000001000010302abcd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * 4. Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x001100000112000c000200009100000081000005890260010346000000001000010302abcd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 */

/*
 * 3. Example of testing OAM over native ethernet with PWE unindexed mode
 *
 * Purpose:
 *   In this example, pwe termination with unindexed mode. It shows how to identify OAM over native ethernet as oam packet and as data packet. 
 *   1) By default, expect OAM over native ethernet is not marked as oam packet, and
 *   the packet can be trapped by upmep on AC if upmep is defined.
 *   2) After calling API bcm_oam_control_set with bcmOamControlNativeEthernetOverPweClassification,
 *   the packet is trapped by downmep on PWE LIF.
 *
 * Objects to be created:
 *   A VPLS model with pwe unindexed mode is setup, and
 *   oam group, lif proflie and one down mep on pwe lif and one up mep on AC are created,
 *
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/dnx/field/cint_field_utils.c
 * cint ../../src/./examples/sand/cint_ip_route_basic.c
 * cint ../../src/./examples/dnx/vpls/cint_vpls_pwe_tagged_mode_basic.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_oam.c
 * cint ../../src/./examples/dnx/oam/cint_oam_basic.c
 * cint ../../src/./examples/dnx/crps/cint_crps_oam_config.c
 * cint ../../src/./examples/dnx/oam/cint_oam_action.c
 * cint ../../src/./examples/dnx/oam/cint_dnx_oam_eth_o_pwe_o_eth.c
 * cint
 * add_upmep_on_egress_ac=1;
 * oam_run_over_inner_eth_with_pwe(0,200,201,2);
 * exit;
 *
 * 1. Send CCMoETH2oPWEoETH0 packet, expect the packet is trapped in egress by upmep on ac
 * tx 1 psrc=201 data=0x000f00020a2200000000cd1d884700d05140000c00020c33000c00020c44910003e8810007d0890260010346000000001000010302abcd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * 2. Received packets on unit 0 should be:
 *    Source port: 201, Destination port: 0
 *    Data: 0x028800648000045673a400000000000000001800140138780202048300982e4e0500000c00020c33000c00020c448100000a890260010346000000001000010302abcd0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * 3. Call API bcm_oam_control_set with bcmOamControlNativeEthernetOverPweClassification to clear tcam entry in oam tcam identification DB.
 *    then oam over native ethernet will be marked as data packet in ingress
 *
 * 4. Send CCMoETH2oPWEoETH0 packet, expect the packet is trapped in ingress by dnmep on pwe lif
 *    tx 1 psrc=201 data=0x000f00020a2200000000cd1d884700d05140000c00020c33000c00020c44910003e8810007d0890260010346000000001000010302abcd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * 5. Received packets on unit 0 should be:
 *    Source port: 201, Destination port: 0
 *    Data: 0x02e400648000045673e0c8b8ab919f6b46100018001401387802020a0100942f0fd500000f00020a2200000000cd1d884700d05140000c00020c33000c00020c44910003e8810007d0890260010346000000001000010302abcd00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 */



int add_upmep_on_egress_ac = 0;

/*
 * 1. Example of oamp injection with PWE protection
 */
int
oam_run_over_inner_eth_with_pwe (
    int unit,
    int ac_port,
    int pwe_port,
    int pwe_mode/*1- pwe raw; 2- pwe unindexed tagged*/)
{
    bcm_error_t rv;
    bcm_oam_group_info_t group_info;

    int counter_if = 0;
    int counter_base_id_pwe = 0;
    int counter_base_id_access  = 0;

    uint32 flags=0;
    bcm_oam_profile_t oam_profile_pwe;
    int ing_profile_pwe_id, egr_profile_pwe_id;
    bcm_oam_profile_t oam_profile_eth;
    int ing_profile_eth_id, egr_profile_eth_id;

    bcm_gport_t in_pwe_lif;
    bcm_gport_t eg_access_ac;

    uint8 short_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 3, 2, 0xab, 0xcd};
    bcm_oam_endpoint_info_t dnmep_info;
    bcm_oam_endpoint_info_t upmep_info;

    bcm_mac_t native_dmac;

    /* Setup VPLS model */
    if (pwe_mode == 1) /* pwe raw mode */
    {
        rv = vpls_mp_basic_main(unit, ac_port, pwe_port);
        if (rv != BCM_E_NONE) {
            printf("Error rv(%d), in vpls_mp_basic_main\n", rv);
            return rv;
        }

        rv = vpls_mp_basic_ve_swap(unit, ac_port, 1);
        if (rv != BCM_E_NONE) {
            printf("Error rv(%d), in vpls_mp_basic_ve_swap\n", rv);
            return rv;
        }

        in_pwe_lif = cint_vpls_basic_info.mpls_port_id_ingress;
        eg_access_ac = cint_vpls_basic_info.vlan_port_id;
        sal_memcpy(native_dmac, cint_vpls_basic_info.ac_mac_address, 6);
    }
    else if (pwe_mode == 2) /* pwe unindexed tagged */
    {
        rv = vpls_pwe_tagged_mode_basic_main(unit, ac_port, pwe_port);
        if (rv != BCM_E_NONE) {
            printf("Error rv(%d), in vpls_pwe_tagged_mode_basic_main\n", rv);
            return rv;
        }

        in_pwe_lif = cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_ing;
        eg_access_ac = cint_vpls_pwe_tagged_mode_basic_info.access_egress_vlan_port_id;
        sal_memcpy(native_dmac, cint_vpls_pwe_tagged_mode_basic_info.service_mac_1, 6);
    }
    else {
        rv = BCM_E_UNAVAIL;
        printf("Error rv(%d), unsupported pwe mode(%d) in oam_run_over_inner_eth_with_pwe\n", rv, pwe_mode);
        return rv;
    }

    /* Get counter resource */
    rv = set_counter_resource(unit, pwe_port, counter_if, 1, &counter_base_id_pwe);
    BCM_IF_ERROR_RETURN(rv);
    rv = set_counter_resource(unit, ac_port, counter_if, 1, &counter_base_id_access);
    BCM_IF_ERROR_RETURN(rv);

    /* Create OAM action profiles */
    rv = oam_set_of_action_profiles_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error rv(%d), in oam_set_of_action_profiles_create.\n", rv);
        return rv;
    }

    /* Get OAM ingress/egress profiles */
    oam_profile_pwe = oam_lif_profiles.oam_profile_pwe_single_ended;
    rv = bcm_oam_profile_id_get_by_type(unit, oam_profile_pwe, bcmOAMProfileIngressLIF, &flags, &ing_profile_pwe_id);
    if (rv != BCM_E_NONE)
    {
       printf("bcm_oam_profile_id_get_by_type(ingress) \n");
       return rv;
    }
    rv = bcm_oam_profile_id_get_by_type(unit, oam_profile_pwe, bcmOAMProfileEgressLIF, &flags, &egr_profile_pwe_id);
    if (rv != BCM_E_NONE)
    {
       printf("bcm_oam_profile_id_get_by_type(egress) \n");
       return rv;
    }

    oam_profile_eth = oam_lif_profiles.oam_profile_eth_single_ended;
    rv = bcm_oam_profile_id_get_by_type(unit, oam_profile_eth, bcmOAMProfileIngressLIF, &flags, &ing_profile_eth_id);
    if (rv != BCM_E_NONE)
    {
       printf("bcm_oam_profile_id_get_by_type(ingress) \n");
       return rv;
    }
    rv = bcm_oam_profile_id_get_by_type(unit, oam_profile_eth, bcmOAMProfileEgressLIF, &flags, &egr_profile_eth_id);
    if (rv != BCM_E_NONE)
    {
       printf("bcm_oam_profile_id_get_by_type(egress) \n");
       return rv;
    }

    /* Bind lif with ingress and egress profiles */
    rv = bcm_oam_lif_profile_set(unit, 0, in_pwe_lif, ing_profile_pwe_id, BCM_OAM_PROFILE_INVALID);
    if (rv != BCM_E_NONE)
    {
       printf("Error no %d, in bcm_oam_lif_profile_set.\n", rv);
       return rv;
    }

    rv = bcm_oam_lif_profile_set(unit, 0, eg_access_ac, ing_profile_eth_id, egr_profile_eth_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error no %d, in bcm_oam_lif_profile_set.\n", rv);
       return rv;
    }

    /* Create a OAM group */
    bcm_oam_group_info_t_init(&group_info);
    sal_memcpy(group_info.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
    rv = bcm_oam_group_create(unit, &group_info);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
      return rv;
    }
    printf("Created group short name format\n");

    /* Set My-CFM-MAC (for Jericho2 Only)*/
    rv = oam__my_cfm_mac_set(unit, ac_port, native_dmac);
    if (rv != BCM_E_NONE) {
        printf("oam__my_cfm_mac_set fail \n");
        return rv;
    }

    /* Create downmep on PWE LIF */
    bcm_oam_endpoint_info_t_init(&dnmep_info);
    dnmep_info.type = bcmOAMEndpointTypeBHHPwe;
    dnmep_info.endpoint_memory_type = bcmOamEndpointMemoryTypeSelfContained;

    dnmep_info.group = group_info.id;
    dnmep_info.level = 3;
    dnmep_info.gport = in_pwe_lif;
    dnmep_info.lm_counter_base_id = counter_base_id_pwe;
    dnmep_info.lm_counter_if = 0;
    dnmep_info.flags = 0;

    printf("bcm_oam_endpoint_create dnmep_info\n");
    rv = bcm_oam_endpoint_create(unit, &dnmep_info);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in bcm_oam_endpoint_create\n", rv);
        return rv;
    }

    if (add_upmep_on_egress_ac) {
        /* Create upmep on egress AC */
        bcm_oam_endpoint_info_t_init(&upmep_info);
        upmep_info.type = bcmOAMEndpointTypeEthernet;
        upmep_info.endpoint_memory_type = bcmOamEndpointMemoryTypeSelfContained;

        upmep_info.group = group_info.id;
        upmep_info.level = 3;
        upmep_info.gport = eg_access_ac;
        upmep_info.lm_counter_base_id = counter_base_id_access;
        upmep_info.lm_counter_if = 0;
        upmep_info.flags = BCM_OAM_ENDPOINT_UP_FACING;

        printf("bcm_oam_endpoint_create upmep_info\n");
        rv = bcm_oam_endpoint_create(unit, &upmep_info);
        if (rv != BCM_E_NONE) {
            printf("Error rv(%d), in bcm_oam_endpoint_create\n", rv);
            return rv;
        }
    }

    return rv;
}

