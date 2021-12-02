/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2021 Broadcom Inc. All rights reserved. File: cint_srv6_alternate_marking.c.
 */

/*
 * This cint is an example of Alternate Marking (RFC8321) of Ingress Node on SRv6.
 * It contains IPv4 forwarding into Srv6 tunnel with Alternate Marking encapsulation.
 *
 * The Alternate Marking setting are on the flow label in IPv6 header.
 * - flow_label[15:0] is the flow id
 * - flow_label[16] is D bit, for measuring end-to-end latency between ingress and egress node. 
 *       If D=1, then a copy of the packet is generated with timestamp.
 * - flow_label[17] is L bit, for color information.
 *
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/dnx/field/cint_field_utils.c
 * cint ../../src/./examples/sand/cint_ip_route_basic.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint ../../src/./examples/dnx/srv6/cint_srv6_basic.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/./examples/dnx/srv6/cint_srv6_alternate_marking.c
 * cint ../../src/./examples/dnx/stat_pp/cint_stat_pp_config.c
 * cint ../../src/./examples/dnx/crps/cint_crps_db_utils.c
 * cint ../../src/./examples/dnx/crps/cint_crps_irpp_config.c
 * cint ../../src/./examples/dnx/field/cint_field_ipv6_alternate_marking.c
 * cint ../../src/./examples/dnx/srv6/cint_srv6_alternate_marking.c
 * cint ../../src/./examples/dnx/crps/cint_crps_oam_config.c
 * cint
 * srv6_alternate_marking_ingress_node_example(0,200,202,201,4,0,0,0,0);
 * exit;
 *
 * cint
 * cint_field_ipv6_alternate_marking_period_change(0,0,0);
 * exit;
 *
 * 1. SRv6 Ingress Tunnel packet with L=0, D=1
 *     tx 1 psrc=200 data=0x000c0002000000000700010081000000080045000049000000008006fa2bc08001017fffff021f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Should recieve 2 packets: forward copy + snoop copy. ToD timestamp in the snooped copy.
 *     Fwd copy:
 *         Data: 0x03d80000000800000268000000000000000009a62a9608000800000000000007fa000200000005b50000000000cd1d00123456789a8100006486dd6001123400912b80123456789abceeffffeecba9876543211234567887654321a5a5a5a55a5a5a5a0408040303000000abcddbca123443211010989845679abcaaaabbbbccccddddeeeeffff11112222111122223333444455556666777788881234567887654321a5a5a5a55a5a5a5a45000049000000007f06fb2bc08001017fffff021f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f10111213
 *     snoop copy:
 *         Data: 0x00000000cd1d00123456789a8100006486dd6001123400912b80123456789abceeffffeecba9876543211234567887654321a5a5a5a55a5a5a5a0408040303000000abcddbca123443211010989845679abcaaaabbbbccccddddeeeeffff11112222111122223333444455556666777788881234567887654321a5a5a5a55a5a5a5a45000049000000007f06fb2bc08001017fffff021f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * 2. SRv6 Ingress Tunnel packet with L=0, D=0
 *     tx 1 psrc=200 data=0x000c0002000000000700010081000000080045000049000000008006fa2bc08001017fffff021f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *     Fwd copy:
 *         Data: 0x00000000cd1d00123456789a8100006486dd6000123400912b80123456789abceeffffeecba9876543211234567887654321a5a5a5a55a5a5a5a0408040303000000abcddbca123443211010989845679abcaaaabbbbccccddddeeeeffff11112222111122223333444455556666777788881234567887654321a5a5a5a55a5a5a5a45000049000000007f06fb2bc08001017fffff021f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * cint
 * cint_field_ipv6_alternate_marking_period_change(0,0,1);
 * exit;
 *
 * 3. SRv6 Ingress Tunnel packet with L=1, D=1
 *     tx 1 psrc=200 data=0x000c0002000000000700010081000000080045000049000000008006fa2bc08001017fffff021f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Should recieve 2 packets: forward copy + snoop copy. ToD timestamp in the snooped copy.
 *     Fwd copy:
 *         Data: 0x03d8000000080000026800000000000000000969d46fce000800000000000007fa000200000005b50000000000cd1d00123456789a8100006486dd6003123400912b80123456789abceeffffeecba9876543211234567887654321a5a5a5a55a5a5a5a0408040303000000abcddbca123443211010989845679abcaaaabbbbccccddddeeeeffff11112222111122223333444455556666777788881234567887654321a5a5a5a55a5a5a5a45000049000000007f06fb2bc08001017fffff021f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f10111213
 *     snoop copy:
 *         Data: 0x00000000cd1d00123456789a8100006486dd6003123400912b80123456789abceeffffeecba9876543211234567887654321a5a5a5a55a5a5a5a0408040303000000abcddbca123443211010989845679abcaaaabbbbccccddddeeeeffff11112222111122223333444455556666777788881234567887654321a5a5a5a55a5a5a5a45000049000000007f06fb2bc08001017fffff021f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * 4. SRv6 Ingress Tunnel packet with L=1, D=0
 *     tx 1 psrc=200 data=0x000c0002000000000700010081000000080045000049000000008006fa2bc08001017fffff021f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *     Fwd copy:
 *         Data: 0x00000000cd1d00123456789a8100006486dd6002123400912b80123456789abceeffffeecba9876543211234567887654321a5a5a5a55a5a5a5a0408040303000000abcddbca123443211010989845679abcaaaabbbbccccddddeeeeffff11112222111122223333444455556666777788881234567887654321a5a5a5a55a5a5a5a45000049000000007f06fb2bc08001017fffff021f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 */


/*
 * Add ingress node PMF configuration per 5-tuple defining flow ID.
 *
 * Ingress PMF:
 * - Per flow (5-tuple), adding one PMF entries in iPMF2, assign a flow ID.
 * - Per flow ID, adding two PMF entries in iPMF3, each action assigns a different counter.
 *
 * Egress PMF:
 * - ASE check and generate snoop copy
 *
 */
int
srv6_alternate_marking_ingress_node_field(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_entry_t entry_handle;

    /*
    * Configure cint params. First the entry for L bit =1.
    */
    g_cint_field_ipv6_am_ing_ent.SrcIp = 0xc0800101;
    g_cint_field_ipv6_am_ing_ent.DstIp = 0x7fffff02;
    g_cint_field_ipv6_am_ing_ent.ProtocolType = 6; /*TCP*/
    g_cint_field_ipv6_am_ing_ent.L4Dst = 9000;
    g_cint_field_ipv6_am_ing_ent.L4Src = 8000;

    rv = cint_field_ipv6_alternate_marking_ingress_node_iPMF2(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_ipv6_alternate_marking_ingress_node_iPMF2 \n", rv);
        return rv;
    }

    rv = cint_field_ipv6_alternate_marking_ingress_node_iPMF2_entry_add(unit, &ipmf2_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_ipv6_alternate_marking_ingress_node_iPMF2_entry_add \n", rv);
        return rv;
    }

    printf("Ingress node iPMF2 configuration Done! \n\n");

    rv = cint_field_ipv6_alternate_marking_ingress_node_iPMF3(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_ipv6_alternate_marking_ingress_node_iPMF3 \n", rv);
        return rv;
    }

    g_cint_field_ipv6_am_ing_ent.KeyGenVar_period = cint_ipv6_alternate_marking_params.loss_KeyGenVar_period;
    g_cint_field_ipv6_am_ing_ent.statId0 = 1; /* Counter 1*/
    g_cint_field_ipv6_am_ing_ent.l_bit_set = 1;

    rv = cint_field_ipv6_alternate_marking_ingress_node_iPMF3_entry_add(unit, &ipmf3_entry_handle_L_bit_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_ipv6_alternate_marking_ingress_node_iPMF3_entry_add \n", rv);
        return rv;
    }


    /* Alternate between the two counters per flow - Now configure the entry with L bit =0.*/
    /*For eviction purposes in the CRPS, its best to take counters as far away from each other.
    In this example we assume
    L=1 range: 1- total_nof_counters/2
    L=0 range: total_nof_counters/2 + 1 - total_nof_counters */
    g_cint_field_ipv6_am_ing_ent.statId0  = cint_ipv6_alternate_marking_crps_params.total_nof_counters /2 + 1;
    g_cint_field_ipv6_am_ing_ent.KeyGenVar_period = cint_ipv6_alternate_marking_params.no_loss_KeyGenVar_period;
    g_cint_field_ipv6_am_ing_ent.l_bit_set = 0;

    rv = cint_field_ipv6_alternate_marking_ingress_node_iPMF3_entry_add(unit, &ipmf3_entry_handle_L_bit_0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_ipv6_alternate_marking_ingress_node_iPMF3_entry_add \n", rv);
        return rv;
    }

    printf("Ingress node iPMF3 configuration Done! \n\n");

    if (g_cint_field_ipv6_am_ing_ent.is_ext_encap == 1) {
        rv = cint_field_ipv6_ext_alternate_marking_ingress_node_ePMF(unit);
    }
    else {
        rv = cint_field_ipv6_alternate_marking_ingress_node_ePMF(unit);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_ipv6_alternate_marking_ingress_node_ePMF \n", rv);
        return rv;
    }
    printf("Ingress node ePMF configuration Done! \n\n");

    return rv;
}

/*
 * An example of Alternate Marking (RFC8321) of Ingress Node on SRv6
 * This example is only for SRv6 packet with 1-pass solution
 *
 * @param unit
 * @param in_port                 - Incoming port of SRV6 Ingress Tunnel
 * @param in_port_eth             - Incoming ETH port of SRV6 Ingress Tunnel
 * @param out_port                - Outgoing port of SRV6 Ingress Tunnel 
 * @param nof_sids
 * @param secondary_srh_flags     - additional SRH Encap flags such as BCM_SRV6_SRH_BASE_INITIATOR_UNIFIED
 * @param secondary_sids_flags    - additional SIDs Encap flags such as BCM_SRV6_SID_INITIATOR_VIRTUAL_EGRESS_POINTED
 * @param is_t_insert_encap
 * @param last_sid_is_dip         - if set, last SID (VPN) is given by IPv6 tunnel encapsulation
 *
 * @return int
 */
int
srv6_alternate_marking_ingress_node_example (
    int unit,
    int in_port,
    int in_port_eth,
    int out_port,
    int nof_sids,
    int secondary_srh_flags,
    int secondary_sids_flags,
    int is_t_insert_encap,
    int last_sid_is_dip)
{
    int rv = BCM_E_NONE;

    rv = srv6_ingress_tunnel_config(unit, in_port, in_port_eth, out_port, nof_sids, secondary_srh_flags, secondary_sids_flags, is_t_insert_encap, last_sid_is_dip);
    if (rv != BCM_E_NONE)
    {
        printf("Error in srv6_ingress_tunnel_config\n");
        return rv;
    }

    /*
     * Add CRPS configuration
     */
    rv = ipv6_alternate_marking_crps(unit, in_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error in ipv6_alternate_marking_crps\n");
        return rv;
    }

    /*
     * Configure PMF rules
     */
    rv = srv6_alternate_marking_ingress_node_field(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error in srv6_alternate_marking_ingress_node_field\n");
        return rv;
    }

    rv = ipv6_alternate_marking_snoop(unit, cint_field_ipv6_am_egress_snoop_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in ipv6_alternate_marking_snoop\n");
        return rv;
    }

    /*
    * Timestamp information when D bit is set is gotten through meta data.
    * Switch id parameter is the node ID field.
    */
    BCM_IF_ERROR_RETURN(cint_instru_ipt_main(unit, 0xab, 0));

    return rv;
}

/*
 * An example of Alternate Marking (RFC8321) of Ingress Node on SRv6
 * This example is only for SRv6 packet with extended encaupsulation solution
 *
 * @param unit
 * @param in_port                 - Incoming port of SRV6 Ingress Tunnel
 * @param out_port                - Outgoing port of SRV6 Ingress Tunnel 
 * @param rch_port                - Recycle Port
 * @param nof_sids_first_pass     - Number of SIDs encapsulated in first pass
 * @param nof_sids_second_pass    - Number of SIDs encapsulated in second pass
 * @param nof_sids_third_pass     - Number of SIDs encapsulated in third pass
 *
 * @return int
 */
int
srv6_ext_encap_alternate_marking_ingress_node_example (
    int unit,
    int in_port,
    int out_port,
    int rch_port,
    int nof_sids_first_pass,
    int nof_sids_second_pass,
    int nof_sids_third_pass)
{
    int rv = BCM_E_NONE;
    bcm_gport_t gport_out;

    BCM_GPORT_LOCAL_SET(gport_out, out_port);

    g_cint_field_ipv6_am_ing_ent.is_ext_encap = 1;
    g_cint_field_ipv6_am_ing_ent.nof_sid_1st_pass = nof_sids_first_pass;

    /*
     * Configuring Extended ISR (Ingress SRv6 extednded encapsulation Tunnel)
     * This will create encapsulation of SRH,SIDs and IPv4 onto the packet
     */
    rv = srv6_ingress_tunnel_ext_encap_config(unit, in_port, out_port, rch_port, nof_sids_first_pass, nof_sids_second_pass, nof_sids_third_pass, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error in srv6_ingress_tunnel_ext_encap_config\n");
        return rv;
    }

    /*
     * Add CRPS configuration
     */
    rv = ipv6_alternate_marking_crps(unit, in_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error in ipv6_alternate_marking_crps\n");
        return rv;
    }

    /*
     * Set pmf data for each out port of Srv6 packet.
     * This pmf data is used to distinguish between recycle port and out port of Srv6 packet by epmf presel
     * See cint_field_ipv6_ext_alternate_marking_ingress_node_ePMF
     */
    g_cint_field_ipv6_am_ing_ent.egr_port_pmf_data = 0x3;
    rv = bcm_port_class_set(unit, out_port, bcmPortClassFieldEgressPacketProcessingPortCs, g_cint_field_ipv6_am_ing_ent.egr_port_pmf_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_port_class_set\n");
        return rv;
    }

    /*
     * Configure PMF rules
     */
    rv = srv6_alternate_marking_ingress_node_field(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error in srv6_alternate_marking_ingress_node_field\n");
        return rv;
    }

    /*
     * Configure Snooping for alternate marking ingress node
     */
    rv = ipv6_alternate_marking_snoop(unit, cint_field_ipv6_am_egress_snoop_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in ipv6_alternate_marking_snoop\n");
        return rv;
    }

    /*
    * Timestamp information when D bit is set is gotten through meta data.
    * Switch id parameter is the node ID field.
    */
    BCM_IF_ERROR_RETURN(cint_instru_ipt_main(unit, 0xab, 0));

    return rv;
}



