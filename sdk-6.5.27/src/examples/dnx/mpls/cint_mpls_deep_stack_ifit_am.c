/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved. File: cint_mpls_deep_stack_ifit_am.c Purpose: utility for MPLS deep encapsulation stack.. 
 */

/*
 * This cint is an example of In-situ Flow Information Telemetry(iFIT) based RFC321
 * It contains IPv4 forwarding into deep MPLS tunnel stack with iFIT  encapsulation.
 * This cint uses mpls and counter cint utilities and cint_field_alternate_marking.c that contains the
 * field configuration for alternate marking.
 * 
 */

/*
 * Test Scenario: Ingress Node; 
 *
 * Test Scenario - start
  cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
  cint ../../../../src/examples/dnx/instru/cint_field_IFA_datapath.c
  cint ../../../../src/examples/sand/utility/cint_sand_utils_l3.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utility_mpls.c
  cint ../../../../src/examples/dnx/mpls/cint_mpls_deep_stack_alternate_marking.c
  cint ../../../../src/examples/dnx/stat_pp/cint_stat_pp_config.c
  cint ../../../../src/examples/dnx/crps/cint_crps_db_utils.c
  cint ../../../../src/examples/dnx/crps/cint_crps_irpp_config.c
  cint ../../../../src/examples/dnx/field/cint_field_alternate_marking.c
  cint ../../../../src/examples/dnx/crps/cint_crps_oam_config.c
  cint ../../../../src/examples/dnx/mpls/cint_mpls_deep_stack_ifit_am.c
  cint
  tod_append_mode=1;
  test_type_on_encap=0;
  mpls_deep_stack_ifit_rfc8321_encapsulation_example(0,200,201,5,1);
  exit;
 *
  cint
  cint_field_alternate_marking_period_change(0,0,0);
  exit;
 *
 * First paket: alternate marking, L=0, second packet = 0 
  tx 1 psrc=200 data=0x000c00020001000c0002000081000000080045000049000000008006b626a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *  Received 2 packets
 * Snoop copy:
 *  Data: 0x02e800000008000002680000000000000000095ffdf250000800000000000007fa000200000005b50000110000011200000000cd1d8100102888470444527f0333527f0444427f0333427f0444327f0333327f0444227f0333227f0444127f0333127f00000780022776020123403045000049000000007f06b726a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f10111213 
 * Forward copy:
 *  Data: 0x00110000011200000000cd1d8100102888470444527f0333527f0444427f0333427f0444327f0333327f0444227f0333227f0444127f0333127f00000780022776020123403045000049000000007f06b726a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
 * Second packet: alternate marking, L=0, second packet = 1 
  tx 1 psrc=200 data=0x000c00020001000c0002000081000000080045000049000000008006b626a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *  Received 1 packet:
 * Forward copy 
 *  Data: 0x00110000011200000000cd1d8100102888470444527f0333527f0444427f0333427f0444327f0333327f0444227f0333227f0444127f0333127f00000780022772020123403045000049000000007f06b726a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 * 
 * Switch to L=1
  cint
  cint_field_alternate_marking_period_change(0,0,1);
  exit;
 *
 * Third packet: alternate marking, L=1, second packet = 0 
  tx 1 psrc=200 data=0x000c00020001000c0002000081000000080045000049000000008006b626a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received 2 packets
 * Snoop copy:
 *  Data: 0x02e80000000800000268000000000000000009039d9de0000800000000000007fa000200000005b50000110000011200000000cd1d8100102888470444527f0333527f0444427f0333427f0444327f0333327f0444227f0333227f0444127f0333127f0000078002277e020123403045000049000000007f06b726a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f10111213 
 * Forward copy:
 *  Data: 0x00110000011200000000cd1d8100102888470444527f0333527f0444427f0333427f0444327f0333327f0444227f0333227f0444127f0333127f0000078002277e020123403045000049000000007f06b726a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
 * Fourth packete: alternate marking, L=1, second packet = 1 
  tx 1 psrc=200 data=0x000c00020001000c0002000081000000080045000049000000008006b626a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received 1 packet
 * Forward copy
 *  Data: 0x00110000011200000000cd1d8100102888470444527f0333527f0444427f0333427f0444327f0333327f0444227f0333227f0444127f0333127f0000078002277a020123403045000049000000007f06b726a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
  cint
  cint_field_alternate_marking_period_change(0,0,0);
  exit;
 *
 *  alternate marking, expecting snoop after clear on read 
  tx 1 psrc=200 data=0x000c00020001000c0002000081000000080045000049000000008006b626a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received 2 packets:
 * Snoop copy:
 *  Data: 0x02e800000008000002680000000000000000092b3d4d00000800000000000007fa000200000005b50000110000011200000000cd1d8100102888470444527f0333527f0444427f0333427f0444327f0333327f0444227f0333227f0444127f0333127f00000780022776020123403045000049000000007f06b726a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f10111213 
 * Forward copy:
 *  Data: 0x00110000011200000000cd1d8100102888470444527f0333527f0444427f0333427f0444327f0333327f0444227f0333227f0444127f0333127f00000780022776020123403045000049000000007f06b726a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 * Test Scenario - end
 *
 */







bcm_instru_ifit_encap_info_t ifit_info = 
{
        /*Flags*/
        0,

        /*ifit_encap_id*/
        0,

        /*fieh_length*/
        0,

        /*fii_exp*/
        3,

        /*fii_ttl*/
        0x80,

        /*fih_flow_id*/
        0x2277,

        /*fih_r_s_bits*/
        1,

        /*fih_header_type_indicator*/
        2,

        /*fieh_ext_data*/
        {
            0x01234030,
            0xccdd0000,
            0xaabb0000,
        }
};

/**
 * Configure the iFIT header encapsulation.
 * In case L_bit_mapping is set, it initialize the parameters only, since the header
 * Information is set by PMF.
 * 
 * INPUT:
 *   in_port	- traffic incoming port, not used now.
 *   out_port - traffic outgoing port, not used now.
 *   fieh_length - length of FIEH length
 */
int
mpls_deep_stack_ifit_rfc8321_encap_ifit_config(
    int unit,
    int in_port,
    int out_port,
    int fieh_length)
{
    int L_bit_mapping = *(dnxc_data_get(unit, "instru", "alternate_marking", "L_bit_profile_mapping", NULL));

    /* Set Alternate Marking special label switch control */
    int speacial_label_value = 12;
    if (verbose >= 1)
    {
        printf("Set alternate marking encapsulation special label switch control\n");
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_set(unit, bcmSwitchMplsFlowInstructionIndicatorSpecialLabel, speacial_label_value),
        "bcmSwitchMplsAlternateMarkingSpecialLabel");

    /*
     * Add Alternate Marking encapsulation: LOSS bit off
     */
    ifit_info.fieh_length = fieh_length;

    if (L_bit_mapping)
    {
        /** In this cause the iFIT encap entries are not needed. They are configured in ePMF directly.*/
        g_cint_field_am_ing_ent.flow_label = ifit_info.fih_flow_id;
        g_cint_field_am_ing_ent.flow_label_ext = ifit_info.fieh_ext_data[0];
        g_cint_field_am_ing_ent.ext_data0 = ifit_info.fieh_ext_data[1];
        g_cint_field_am_ing_ent.ext_data1 = ifit_info.fieh_ext_data[2];
        g_cint_field_am_ing_ent.flow_label_ext_length = ifit_info.fieh_length;
        return BCM_E_NONE;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_instru_ifit_encap_create(unit,&ifit_info), "");
    cint_mpls_deep_stack_alternate_marking_params.alternate_marking_if_no_loss = ifit_info.ifit_encap_id;
    printf("cint_mpls_deep_stack_alternate_marking_params.alternate_marking_if_no_loss:%d\n/n", cint_mpls_deep_stack_alternate_marking_params.alternate_marking_if_no_loss);

    /*
     * Add alternate marking encapsulation: LOSS bit on
     */


    ifit_info.ifit_encap_id = 0;
    ifit_info.flags = BCM_INSTRU_IFIT_ENCAP_ALTERNATE_MARKING_LOSS_SET;
    BCM_IF_ERROR_RETURN_MSG(bcm_instru_ifit_encap_create(unit,&ifit_info), "");

    cint_mpls_deep_stack_alternate_marking_params.alternate_marking_if_loss = ifit_info.ifit_encap_id;
    printf("cint_mpls_deep_stack_alternate_marking_params.alternate_marking_if_loss:%d\n/n", cint_mpls_deep_stack_alternate_marking_params.alternate_marking_if_loss);

    return BCM_E_NONE;
}


/**
 * Main entrance for iFIT ingress node, including:
 * 1.setup MPLS tunnen and iFIT encapsulation, 
 * 2.PMF rules for altermate marking
 * 3.CRPS configuration
 * 4.Ipv4 service, routing into MPLS tunnel
 * 
 * INPUT:
 *   in_port	- traffic incoming port
 *   out_port - traffic outgoing port
 *   nof_tunnel - number of mpls tunnel
 *   fieh_length - length of FIEH length
*/
int
mpls_deep_stack_ifit_rfc8321_encapsulation_example(
    int unit,
    int in_port,
    int out_port,
    int nof_tunnel,
    int fieh_length)
{
    int encap_access_array[6] = {
                                        bcmEncapAccessTunnel1,
                                        bcmEncapAccessTunnel2,
                                        bcmEncapAccessTunnel3,
                                        bcmEncapAccessTunnel4,
                                        bcmEncapAccessArp,
                                        bcmEncapAccessInvalid
                                    };

    int L_bit_mapping = 0;

    L_bit_mapping = *(dnxc_data_get(unit, "instru", "alternate_marking", "L_bit_profile_mapping", NULL));
    if (L_bit_mapping)
    {
        use_crps_egress_oam_counter = 1;
    }

    BCM_IF_ERROR_RETURN_MSG(mpls_deep_stack_alternate_marking_encap_tunnel_config(unit, in_port, out_port, nof_tunnel, encap_access_array), "");

    BCM_IF_ERROR_RETURN_MSG(mpls_deep_stack_ifit_rfc8321_encap_ifit_config(unit, in_port,out_port, fieh_length), "");

    BCM_IF_ERROR_RETURN_MSG(mpls_deep_stack_alternate_marking_enacp_instru_config(unit, in_port, out_port), "");

    return BCM_E_NONE;
}

