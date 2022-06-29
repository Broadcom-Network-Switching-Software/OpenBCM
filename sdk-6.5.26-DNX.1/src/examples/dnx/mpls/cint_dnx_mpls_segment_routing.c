/*~~~~~~~~~Multi-Dev MPLS Segment Routing~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: cint_dnx_mpls_segment_routing.c
 * Purpose: Demo MPLS Segment Routing for JR2 above device with multi device system
 *
 * Multi-Device system require the ingress/egress configuration separately and relatively.
 * This cint demo how to configurer the MPLS SR service under multi-device system.
 *  1. Create egress push 6 labels
 *  2. Create In-RIF and host entry on ingress device
 *  3. Send MPLS packet to verify the result
 *
 * Test Scenario 
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_multi_device.c
 * cint ../../src/./examples/dnx/mpls/cint_mpls_deep_stack.c
 * cint ../../src/./examples/dnx/mpls/cint_dnx_mpls_segment_routing.c
 * cint
 * multi_dev_mpls_segment_routing_example(0,1,1811939528,1811940552,1);
 * exit;
 *
 *  Ipv4 over ethernet packet 
 * tx 1 psrc=200 data=0x000000000011000c0002000081000064080045000035000000008000b640a0a1a1a2a0a1a1a3000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 1 should be: 
 *  Source port: 0, Destination port: 0 
 *  Data: 0x00110000011200000000cd1d8100002888470444107f0333117f45000035000000007f00b740a0a1a1a2a0a1a1a3000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
 *  Ipv6 over ethernet packet 
 * tx 1 psrc=200 data=0x000000000011000c000200008100006486dd60000000002100800000000000000000123400000000ff130000000000000000123400000000ff44000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 1 should be: 
 *  Source port: 0, Destination port: 0 
 *  Data: 0x00110000011200000000cd1d8100002888470444107f0333117f600000000021007f0000000000000000123400000000ff130000000000000000123400000000ff44000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_multi_device.c
 * cint ../../src/./examples/dnx/mpls/cint_mpls_deep_stack.c
 * cint ../../src/./examples/dnx/mpls/cint_dnx_mpls_segment_routing.c
 * cint
 * multi_dev_mpls_segment_routing_example(0,1,1811939528,1811940552,2);
 * exit;
 *
 *  Ipv4 over ethernet packet 
 * tx 1 psrc=200 data=0x000000000011000c0002000081000064080045000035000000008000b640a0a1a1a2a0a1a1a3000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 1 should be: 
 *  Source port: 0, Destination port: 0 
 *  Data: 0x0011000001120000000000008100002888470444207f0333207f0444107f0333117f45000035000000007f00b740a0a1a1a2a0a1a1a3000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
 *  Ipv6 over ethernet packet 
 * tx 1 psrc=200 data=0x000000000011000c000200008100006486dd60000000002100800000000000000000123400000000ff130000000000000000123400000000ff44000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 1 should be: 
 *  Source port: 0, Destination port: 0 
 *  Data: 0x0011000001120000000000008100002888470444207f0333207f0444107f0333117f600000000021007f0000000000000000123400000000ff130000000000000000123400000000ff44000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_multi_device.c
 * cint ../../src/./examples/dnx/mpls/cint_mpls_deep_stack.c
 * cint ../../src/./examples/dnx/mpls/cint_dnx_mpls_segment_routing.c
 * cint
 * multi_dev_mpls_segment_routing_example(0,1,1811939528,1811940552,3);
 * exit;
 *
 *  Ipv4 over ethernet packet 
 * tx 1 psrc=200 data=0x000000000011000c0002000081000064080045000035000000008000b640a0a1a1a2a0a1a1a3000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 1 should be: 
 *  Source port: 0, Destination port: 0 
 *  Data: 0x0011000001120000000000008100002888470444307f0333307f0444207f0333207f0444107f0333117f45000035000000007f00b740a0a1a1a2a0a1a1a3000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
 *  Ipv6 over ethernet packet 
 * tx 1 psrc=200 data=0x000000000011000c000200008100006486dd60000000002100800000000000000000123400000000ff130000000000000000123400000000ff44000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 1 should be: 
 *  Source port: 0, Destination port: 0 
 *  Data: 0x0011000001120000000000008100002888470444307f0333307f0444207f0333207f0444107f0333117f600000000021007f0000000000000000123400000000ff130000000000000000123400000000ff44000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_multi_device.c
 * cint ../../src/./examples/dnx/mpls/cint_mpls_deep_stack.c
 * cint ../../src/./examples/dnx/mpls/cint_dnx_mpls_segment_routing.c
 * cint
 * multi_dev_mpls_segment_routing_example(0,1,1811939528,1811940552,4);
 * exit;
 *
 *  Ipv4 over ethernet packet 
 * tx 1 psrc=200 data=0x000000000011000c0002000081000064080045000035000000008000b640a0a1a1a2a0a1a1a3000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 1 should be: 
 *  Source port: 0, Destination port: 0 
 *  Data: 0x0011000001120000000000008100002888470444407f0333407f0444307f0333307f0444207f0333207f0444107f0333117f45000035000000007f00b740a0a1a1a2a0a1a1a3000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
 *  Ipv6 over ethernet packet 
 * tx 1 psrc=200 data=0x000000000011000c000200008100006486dd60000000002100800000000000000000123400000000ff130000000000000000123400000000ff44000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 1 should be: 
 *  Source port: 0, Destination port: 0 
 *  Data: 0x0011000001120000000000008100002888470444407f0333407f0444307f0333307f0444207f0333207f0444107f0333117f600000000021007f0000000000000000123400000000ff130000000000000000123400000000ff44000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_multi_device.c
 * cint ../../src/./examples/dnx/mpls/cint_mpls_deep_stack.c
 * cint ../../src/./examples/dnx/mpls/cint_dnx_mpls_segment_routing.c
 * cint
 * multi_dev_mpls_segment_routing_example(0,1,1811939528,1811940552,5);
 * exit;
 *
 *  Ipv4 over ethernet packet 
 * tx 1 psrc=200 data=0x000000000011000c0002000081000064080045000035000000008000b640a0a1a1a2a0a1a1a3000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 1 should be: 
 *  Source port: 0, Destination port: 0 
 *  Data: 0x0011000001120000000000008100002888470444507f0333507f0444407f0333407f0444307f0333307f0444207f0333207f0444107f0333117f45000035000000007f00b740a0a1a1a2a0a1a1a3000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
 *  Ipv6 over ethernet packet 
 * tx 1 psrc=200 data=0x000000000011000c000200008100006486dd60000000002100800000000000000000123400000000ff130000000000000000123400000000ff44000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 1 should be: 
 *  Source port: 0, Destination port: 0 
 *  Data: 0x0011000001120000000000008100002888470444507f0333507f0444407f0333407f0444307f0333307f0444207f0333207f0444107f0333117f600000000021007f0000000000000000123400000000ff130000000000000000123400000000ff44000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_multi_device.c
 * cint ../../src/./examples/dnx/mpls/cint_mpls_deep_stack.c
 * cint ../../src/./examples/dnx/mpls/cint_dnx_mpls_segment_routing.c
 * cint
 * multi_dev_mpls_segment_routing_example(0,1,1811939528,1811940552,6);
 * exit;
 *
 *  Ipv4 over ethernet packet 
 * tx 1 psrc=200 data=0x000000000011000c0002000081000064080045000035000000008000b640a0a1a1a2a0a1a1a3000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 1 should be: 
 *  Source port: 0, Destination port: 0 
 *  Data: 0x0011000001120000000000008100002888470444607f0333607f0444507f0333507f0444407f0333407f0444307f0333307f0444207f0333207f0444107f0333117f45000035000000007f00b740a0a1a1a2a0a1a1a3000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
 *  Ipv6 over ethernet packet 
 * tx 1 psrc=200 data=0x000000000011000c000200008100006486dd60000000002100800000000000000000123400000000ff130000000000000000123400000000ff44000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 1 should be: 
 *  Source port: 0, Destination port: 0 
 *  Data: 0x0011000001120000000000008100002888470444607f0333607f0444507f0333507f0444407f0333407f0444307f0333307f0444207f0333207f0444107f0333117f600000000021007f0000000000000000123400000000ff130000000000000000123400000000ff44000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_multi_device.c
 * cint ../../src/./examples/dnx/mpls/cint_mpls_deep_stack.c
 * cint ../../src/./examples/dnx/mpls/cint_dnx_mpls_segment_routing.c
 * cint
 * multi_dev_mpls_segment_routing_example(0,1,1811939528,1811940552,7);
 * exit;
 *
 *  Ipv4 over ethernet packet 
 * tx 1 psrc=200 data=0x000000000011000c0002000081000064080045000035000000008000b640a0a1a1a2a0a1a1a3000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 1 should be: 
 *  Source port: 0, Destination port: 0 
 *  Data: 0x0011000001120000000000008100002888470444707f0333707f0444607f0333607f0444507f0333507f0444407f0333407f0444307f0333307f0444207f0333207f0444107f0333117f45000035000000007f00b740a0a1a1a2a0a1a1a3000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
 *  Ipv6 over ethernet packet 
 * tx 1 psrc=200 data=0x000000000011000c000200008100006486dd60000000002100800000000000000000123400000000ff130000000000000000123400000000ff44000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 1 should be: 
 *  Source port: 0, Destination port: 0 
 *  Data: 0x0011000001120000000000008100002888470444707f0333707f0444607f0333607f0444507f0333507f0444407f0333407f0444307f0333307f0444207f0333207f0444107f0333117f600000000021007f0000000000000000123400000000ff130000000000000000123400000000ff44000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
 */


struct cint_mpls_sr_info_s
 {
     int fec_id;                  /*FEC ID for arp, for case 1,2,3,4*/
 };

cint_mpls_sr_info_s cint_mpls_sr_info;

/**
 * Initiation for system parameter
 * INPUT:
 *   ingress_unit- traffic incoming unit
 *   egress_unit- traffic ougtoing unit
 *   in_sys_port- traffic incoming port
 *   out_sys_port- traffic outgoing port
*/
 int
mpls_segment_routing_init(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{
    int rv;

    rv = multi_dev_ip_route_init(ingress_unit,egress_unit,in_sys_port,out_sys_port);
    if (rv != BCM_E_NONE)
    {
         printf("Error, in multi_dev_ipv4_uc_init\n");
         return rv;
    }

    cint_mpls_sr_info.fec_id = cint_multi_dev_ip_route_info.level1_fec_id;

    return rv;
}



/*
 * l3 FEC replace
 *
 */
int
mpls_segment_routing_fec_configuration(
    int unit,
    int out_sys_port)
{
    int rv;
    dnx_utils_l3_fec_s fec_structure;

    dnx_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    fec_structure.destination  = out_sys_port;
    fec_structure.tunnel_gport = mpls_util_entity[0].mpls_encap_tunnel[0].tunnel_id;
    fec_structure.allocation_flags =   BCM_L3_WITH_ID | BCM_L3_INGRESS_ONLY;
    fec_structure.fec_id = cint_mpls_sr_info.fec_id;
    rv = dnx_utils_l3_fec_create(unit, &fec_structure);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in dnx_utils_l3_fec_create\n");
        return rv;
    }
    return rv;

}

int
mpls_segment_routing_tunnel_configuration(
    int unit,
    int in_port,
    int out_port,
    int nof_tunnel,
    int start_phase)
{
    int rv = BCM_E_NONE;
    int encap_access_array[8] = {
                                        bcmEncapAccessRif,
                                        bcmEncapAccessNativeArp,
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
    int update_encap_access = FALSE;
    int label_to_build = 0, tunnel_left = nof_tunnel;
    int enc_stage_idx = 0,mpls_label_idx = 0;

    MPLS_UTIL_MAX_NOF_ENTITIES = 2;

    init_default_mpls_params(unit, in_port, out_port);

    mpls_concatened_tunnels_scenario = 1;

    mpls_tunnel_initiator_create_s_init(mpls_util_entity[0].mpls_encap_tunnel, MPLS_UTIL_MAX_NOF_TUNNELS);

    if (*dnxc_data_get(unit, "flow", "general", "is_flow_enabled_for_legacy_applications", NULL))
    {
        /**
         * When flow is used for tunnel creation, multiple tunnel could be
         * created by one call
         */
        mpls_tunnel_id = 4; /*By default, all 4 stages are used*/
        for (enc_stage_idx = 0; enc_stage_idx < 4; enc_stage_idx++, encap_access_idx++)
        {
            label_to_build = (tunnel_left >= 2)?4:(tunnel_left >= 1)?2:0;
            tunnel_left -= label_to_build/2;
            if (label_to_build == 0)
            {
                mpls_tunnel_id = enc_stage_idx;
                break;
            }
        
            update_encap_access = TRUE;
            for (mpls_label_idx = 0; mpls_label_idx < label_to_build; mpls_label_idx++)
            {
                if (mpls_label_idx == 0){
                    mpls_util_entity[0].mpls_encap_tunnel[enc_stage_idx].label[0] = stage_enc_label_1;
                    mpls_util_entity[0].mpls_encap_tunnel[enc_stage_idx].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
                    mpls_util_entity[0].mpls_encap_tunnel[enc_stage_idx].l3_intf_id = &mpls_util_entity[0].mpls_encap_tunnel[enc_stage_idx+1].tunnel_id;
                    mpls_util_entity[0].mpls_encap_tunnel[enc_stage_idx].num_labels = label_to_build;
                    mpls_util_entity[0].mpls_encap_tunnel[enc_stage_idx].encap_access = encap_access_array[encap_access_idx+start_phase];
                }
                else if (mpls_label_idx == 1){
                    mpls_util_entity[0].mpls_encap_tunnel[enc_stage_idx].label[1] = stage_enc_label_2;
                }
                else
                {
                    if (update_encap_access == TRUE)
                    {
                        encap_access_idx++;
                        update_encap_access = FALSE;
                    }
                    mpls_util_entity[0].mpls_encap_tunnel[enc_stage_idx].data_label[mpls_label_idx - 2] = 
                        mpls_label_idx%2?stage_enc_label_2:stage_enc_label_1;
                }
                /*One more tunnel entry needed*/
                if (mpls_label_idx%2 == 1)
                {
                    stage_enc_label_1++;
                    stage_enc_label_2++;
                }
            }
        }

    }
    else
    {
        /* encapsulation entries */
        for (mpls_tunnel_id = 0; mpls_tunnel_id < nof_tunnel; mpls_tunnel_id++, encap_access_idx++, stage_enc_label_1++, stage_enc_label_2++)
        {
            printf("mpls_tunnel_id=%d\n",mpls_tunnel_id);
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].label[0] = stage_enc_label_1;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].label[1] = stage_enc_label_2;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].num_labels = 2;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | BCM_MPLS_EGRESS_LABEL_TANDEM;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].l3_intf_id = &mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id+1].tunnel_id;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].encap_access = encap_access_array[encap_access_idx+start_phase];
        }

    }

    mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id-1].l3_intf_id = &mpls_util_entity[1].arps[0].arp;

    /* configure ARP+AC in single ETPS entry */
    mpls_util_entity[1].arps[0].flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;

    rv = mpls_util_main(unit,in_port,out_port);
    if (rv) {
        printf("Error, create_mpls_tunnel\n");
        return rv;
    }

    return rv;
}


 /**
  * Port, RIF and ARP configuration for mpls application
 */
 int
mpls_segment_routing_l2_l3_configuration(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{
    int rv;

    rv = mpls_segment_routing_init(ingress_unit,egress_unit,in_sys_port,out_sys_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, mpls_segment_routing_init\n");
        return rv;
    }

    /*
     * step 1.ingress L2/L3 configuration(Port,In-RIF)
     */
    rv = multi_dev_ingress_port_configuration(ingress_unit,in_sys_port,cint_multi_dev_ip_route_info.in_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multi_dev_ingress_port_configuration\n");
        return rv;
    }

    rv = multi_dev_ip_route_l3_intf_configuration(ingress_unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multi_dev_ip_route_l3_intf_configuration\n");
        return rv;
    }


    return rv;

}

/**
 * MPLS segment routing
 * Max pushing label number: 14
*/
 int
 multi_dev_mpls_segment_routing_example(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port,
    int nof_tunnels)
{

    int rv = BCM_E_NONE;

    /*
     * step 1: create in and out rif
     */
    rv = mpls_segment_routing_l2_l3_configuration(ingress_unit, egress_unit,in_sys_port,out_sys_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in mpls_segment_routing_l2_l3_configuration\n", rv);
        return rv;
    }

    /*
     * step 2: tunnel configuration
     */
    rv =  mpls_segment_routing_tunnel_configuration(egress_unit, cint_multi_dev_ip_route_info.in_local_port,
                             cint_multi_dev_ip_route_info.out_local_port, nof_tunnels, 0);
    if (rv != BCM_E_NONE)
    {
         printf("Error (%d), in mpls_segment_routing_encap_configuration\n", rv);
         return rv;
    }

    /*
     * step 3: FEC configuration
     */
    rv = mpls_segment_routing_fec_configuration(ingress_unit, out_sys_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_segment_routing_fec_configuration \n");
        return rv;
    }

    /*
     * step 4: push over IPv4
     */
    rv = add_host_ipv4(ingress_unit, mpls_util_entity[0].hosts_ipvx[0].ipv4, cint_multi_dev_ip_route_info.vrf, cint_mpls_sr_info.fec_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in add_host_ipv4\n", rv);
        return rv;
    }


    /*
     * step 5: push over IPv6
     */
    rv = add_host_ipv6(ingress_unit, mpls_util_entity[0].hosts_ipvx[0].ipv6, cint_multi_dev_ip_route_info.vrf, cint_mpls_sr_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in add_host_ipv4\n", rv);
        return rv;
    }

    return rv;

}


