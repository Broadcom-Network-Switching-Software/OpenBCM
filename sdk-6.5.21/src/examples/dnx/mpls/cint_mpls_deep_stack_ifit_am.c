/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_mpls_deep_stack_ifit_am.c Purpose: utility for MPLS deep encapsulation stack.. 
 */

/*
 * This cint is an example of In-situ Flow Information Telemetry(iFIT) based RFC321
 * It contains IPv4 forwarding into deep MPLS tunnel stack with iFIT  encapsulation.
 * This cint uses mpls and counter cint utilities and cint_field_alternate_marking.c that contains the
 * field configuration for alternate marking.
 */

/*
 * Test Scenario: Ingress Node; 
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/dnx/instru/cint_field_IFA_datapath.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint ../../src/./examples/dnx/mpls/cint_mpls_deep_stack_alternate_marking.c
 * cint ../../src/./examples/dnx/stat_pp/cint_stat_pp_config.c
 * cint ../../src/./examples/dnx/crps/cint_crps_db_utils.c
 * cint ../../src/./examples/dnx/crps/cint_crps_irpp_config.c
 * cint ../../src/./examples/dnx/field/cint_field_alternate_marking.c
 * cint ../../src/./examples/dnx/crps/cint_crps_oam_config.c
 * cint ../../src/./examples/dnx/mpls/cint_mpls_deep_stack_ifit_am.c
 * cint
 * tod_append_mode=1;
 * test_type_on_encap=0;
 * mpls_deep_stack_ifit_rfc8321_encapsulation_example(0,200,201,5,1);
 * exit;
 *
 * cint
 * cint_field_alternate_marking_period_change(0,0,0);
 * exit;
 *
 * First paket: alternate marking, L=0, second packet = 0 
 * tx 1 psrc=200 data=0x000c00020001000c0002000081000000080045000049000000008006b626a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *  Received 2 packets
 * Snoop copy:
 *  Data: 0x02e800000008000002680000000000000000095ffdf250000800000000000007fa000200000005b50000110000011200000000cd1d8100102888470444527f0333527f0444427f0333427f0444327f0333327f0444227f0333227f0444127f0333127f00000780022776020123403045000049000000007f06b726a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f10111213 
 * Forward copy:
 *  Data: 0x00110000011200000000cd1d8100102888470444527f0333527f0444427f0333427f0444327f0333327f0444227f0333227f0444127f0333127f00000780022776020123403045000049000000007f06b726a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
 * Second packet: alternate marking, L=0, second packet = 1 
 * tx 1 psrc=200 data=0x000c00020001000c0002000081000000080045000049000000008006b626a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *  Received 1 packet:
 * Forward copy 
 *  Data: 0x00110000011200000000cd1d8100102888470444527f0333527f0444427f0333427f0444327f0333327f0444227f0333227f0444127f0333127f00000780022772020123403045000049000000007f06b726a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 * 
 * Switch to L=1
 * cint
 * cint_field_alternate_marking_period_change(0,0,1);
 * exit;
 *
 * Third packet: alternate marking, L=1, second packet = 0 
 * tx 1 psrc=200 data=0x000c00020001000c0002000081000000080045000049000000008006b626a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received 2 packets
 * Snoop copy:
 *  Data: 0x02e80000000800000268000000000000000009039d9de0000800000000000007fa000200000005b50000110000011200000000cd1d8100102888470444527f0333527f0444427f0333427f0444327f0333327f0444227f0333227f0444127f0333127f0000078002277e020123403045000049000000007f06b726a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f10111213 
 * Forward copy:
 *  Data: 0x00110000011200000000cd1d8100102888470444527f0333527f0444427f0333427f0444327f0333327f0444227f0333227f0444127f0333127f0000078002277e020123403045000049000000007f06b726a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
 * Fourth packete: alternate marking, L=1, second packet = 1 
 * tx 1 psrc=200 data=0x000c00020001000c0002000081000000080045000049000000008006b626a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received 1 packet
 * Forward copy
 *  Data: 0x00110000011200000000cd1d8100102888470444527f0333527f0444427f0333427f0444327f0333327f0444227f0333227f0444127f0333127f0000078002277a020123403045000049000000007f06b726a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
 * cint
 * cint_field_alternate_marking_period_change(0,0,0);
 * exit;
 *
 *  alternate marking, expecting snoop after clear on read 
 * tx 1 psrc=200 data=0x000c00020001000c0002000081000000080045000049000000008006b626a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received 2 packets:
 * Snoop copy:
 *  Data: 0x02e800000008000002680000000000000000092b3d4d00000800000000000007fa000200000005b50000110000011200000000cd1d8100102888470444527f0333527f0444427f0333427f0444327f0333327f0444227f0333227f0444127f0333127f00000780022776020123403045000049000000007f06b726a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f10111213 
 * Forward copy:
 *  Data: 0x00110000011200000000cd1d8100102888470444527f0333527f0444427f0333427f0444327f0333327f0444227f0333227f0444127f0333127f00000780022776020123403045000049000000007f06b726a0a1a1a2a0a1a1a31f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
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
        2,

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
    int rv = BCM_E_NONE;
    int encap_access_array[6] = {
                                        bcmEncapAccessTunnel1,
                                        bcmEncapAccessTunnel2,
                                        bcmEncapAccessTunnel3,
                                        bcmEncapAccessTunnel4,
                                        bcmEncapAccessArp,
                                        bcmEncapAccessInvalid
                                    };
    int encap_access_idx = 0;
    int mpls_tunnel_id = 0;
    bcm_mpls_label_t stage_enc_label_1 = 0x3331;
    bcm_mpls_label_t stage_enc_label_2 = 0x4441;

    MPLS_UTIL_MAX_NOF_ENTITIES = 2;

    init_default_mpls_params(unit, in_port, out_port);

    mpls_tunnel_initiator_create_s_init(mpls_util_entity[0].mpls_encap_tunnel, MPLS_UTIL_MAX_NOF_TUNNELS);
    
    /* encapsulation entries, not including alternate marking entries */
    for (mpls_tunnel_id = 0; mpls_tunnel_id < nof_tunnel; mpls_tunnel_id++, encap_access_idx++, stage_enc_label_1++, stage_enc_label_2++)
    {
        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].label[0] = stage_enc_label_1;
        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].label[1] = stage_enc_label_2;
        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].num_labels = 2;
        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | BCM_MPLS_EGRESS_LABEL_TANDEM;
        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].l3_intf_id = &mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id+1].tunnel_id;
        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].encap_access = encap_access_array[encap_access_idx];
        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].egress_qos_model.egress_qos = bcmQosEgressModelPipeMyNameSpace;
        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].exp0 = 1;
        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].exp1 = 1;
    }

    if (mpls_2plus1_on_vpn_label == test_type_on_encap)
    {
        mpls_util_entity[0].mpls_encap_tunnel[0].num_labels = 1;
    }
    else if (mpls_2_on_vpn_label == test_type_on_encap)
    {
        mpls_util_entity[0].mpls_encap_tunnel[0].flags &= ~BCM_MPLS_EGRESS_LABEL_TANDEM;
    }

    mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id-1].l3_intf_id = &mpls_util_entity[1].arps[0].arp;

    /* configure ARP+AC in single ETPS entry */
    mpls_util_entity[1].arps[0].flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;

    rv = mpls_util_main(unit,in_port,out_port);
    if (rv) {
        printf("Error, create_mpls_tunnel\n");
        return rv;
    }

    /* Set Alternate Marking special label switch control */
    int speacial_label_value = 12;
    if (verbose >= 1)
    {
        printf("Set alternate marking encapsulation special label switch control\n");
    }
    rv = bcm_switch_control_set(unit, bcmSwitchMplsFlowInstructionIndicatorSpecialLabel, speacial_label_value);
    if (rv) {
        printf("Error, bcm_switch_control_set bcmSwitchMplsAlternateMarkingSpecialLabel\n");
        return rv;
    }

    /*
     * Add Alternate Marking encapsulation: LOSS bit off
     */
    ifit_info.fieh_length = fieh_length;
    rv = bcm_instru_ifit_encap_create(unit,&ifit_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_ifit_encap_create\n");
        return rv;
    }
    cint_mpls_deep_stack_alternate_marking_params.alternate_marking_if_no_loss = ifit_info.ifit_encap_id;
    printf("cint_mpls_deep_stack_alternate_marking_params.alternate_marking_if_no_loss:%d\n/n", cint_mpls_deep_stack_alternate_marking_params.alternate_marking_if_no_loss);

    /*
     * Add alternate marking encapsulation: LOSS bit on
     */


    ifit_info.ifit_encap_id = 0;
    ifit_info.flags = BCM_INSTRU_IFIT_ENCAP_ALTERNATE_MARKING_LOSS_SET;
    rv = bcm_instru_ifit_encap_create(unit,&ifit_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_ifit_encap_create\n");
        return rv;
    }

    cint_mpls_deep_stack_alternate_marking_params.alternate_marking_if_loss = ifit_info.ifit_encap_id;
    printf("cint_mpls_deep_stack_alternate_marking_params.alternate_marking_if_loss:%d\n/n", cint_mpls_deep_stack_alternate_marking_params.alternate_marking_if_loss);

    /*
     * Add CRPS configuration
     */
    rv = mpls_deep_stack_alternate_marking_crps(unit, in_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error in mpls_deep_stack_alternate_marking_crps\n");
        return rv;
    }

    /*
     * Configure PMF rules
     */
    rv = mpls_deep_stack_alternate_marking_ingress_field(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error in mpls_deep_stack_alternate_marking_ingress_field\n");
        return rv;
    }

    rv = mpls_deep_stack_alternate_marking_snoop(unit, cint_field_am_egress_snoop_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in mpls_deep_stack_alternate_marking_field\n");
        return rv;
    }

    if (tod_append_mode ==2)
    {
        /*
         * Time of day gotten through mirror additional info. 
         * This requires initializing through a cint from cint_ifa.c 
         * Only when the in_port is a NIF port. 
         */
        if (in_port_is_nif)
        {
            BCM_IF_ERROR_RETURN( cint_instru_config_nif_rx_timestamp(unit,in_port,0));
        }
    } 
    else if (tod_append_mode ==1)
    {
        /*
        * Timestamp information when D bit is set is gotten through tail-edit meta data.
        * Switch id parameter is the node ID field.
        */
        BCM_IF_ERROR_RETURN(cint_instru_ipt_main(unit, 0xab, 0)); 
    }
    else 
    {
        print "ERROR: tod_append_mode must be either 1 (use tail-edit) or 2 (use mirror-additional-info)";
        return  123;
    }
    return rv;
}

