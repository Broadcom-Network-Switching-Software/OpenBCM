/*
 * This CINT contains three examples of trapping DM packets with 4Bytes Second TOD to external CPU (RCPU).
 *
 * 1st example is for DM trapped by down MEP to RCPU
 * 2nd example is for DM trapped by up MEP to RCPU
 * 3rd example is for TWAMP trapped by PMF to RCPU
 *
 * The expected format of trapped DM to RCPU is
 *     Original-Packet o4Bytes-Second oSystem-Header oEthernet-Header
 *
 */

/*
 * 1st example - trapping DM on Downmep to RCPU.
 *
 * Test Scenario - start
 *
 * ./bcm.user
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dnx/field/cint_field_oam_layer_index.c
 * cint
 * field_oam_layer_index_main(0,0);
 * exit;
 *
 * 0: tr 141 NoInit=1
 *
 * config add tm_port_header_type_out_203.0=ENCAP_EXTERNAL_CPU
 * config add custom_feature_init_verify=1
 *
 * 0: tr 141 NoDeinit=1
 *
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_oam.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_tpid.c
 * cint ../../../../src/examples/sand/cint_vswitch_metro_mp.c
 * cint ../../../../src/examples/sand/cint_sand_advanced_vlan_translation_mode.c
 * cint ../../../../src/examples/sand/cint_l2_traverse.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_vlan.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint ../../../../src/examples/sand/cint_vswitch_cross_connect_p2p_ac_pwe.c
 * cint ../../../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_pwe.c
 * cint ../../../../src/examples/sand/cint_sand_oam.c
 * cint
 * vswitch_cross_connect_p2p_ac_pwe_with_ports__main_config__start_run(0,201,200,202,203);
 * exit;
 *
 * cint ../../../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dnx/internal/cint_internal_oam_jr2.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dnx/oam/cint_oam_basic.c
 * cint ../../../../src/examples/dnx/crps/cint_crps_oam_config.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dnx/oam/cint_oam_basic.c
 * cint
 * oam_create_eth_mep(0,0x44801001,7,4,8,0,0,0);
 * exit;
 *
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dnx/oam/cint_dnx_oam_trap_dm_to_external_cpu.c
 * cint
 * trap_dm_to_rcpu_info.rcpu_port=203;
 * print rcpu_tunnel_create(0);
 * print rcpu_global_set(0);
 * print rcpu_port_set(0);
 *
 * // below trap_id can be obtained by calling bcm_rx_trap_type_get with bcmRxTrapBfdOamDownMEP
 * trap_dm_to_rcpu_info.rcpu_ingress_cpu_trap_id=253;
 * print rcpu_uc_trap_set(0);
 *
 * print rcpu_global_outlifs_set(0,BCM_L3_ITF_VAL_GET(trap_dm_to_rcpu_info.rcpu_l3a_intf_id),0,0);
 *
 * // below trap_code can be obtained by calling bcm_rx_trap_type_get with bcmRxTrapBfdOamDownMEP
 * trap_dm_to_rcpu_info.rcpu_trap_code_in_sys_header=253;
 * print cint_dnx_oam_trap_dm_with_4b_tod_to_rcpu_epmf_set(0, 1);
 * exit;
 *
 * // sending a DMR to check if TOD is expected
 * tx 1 psrc=200 data=0x0000000102340000174b0c8b810000668902812e0020000000000000000000000000000000000000000000000000000000000000000000
 *
 * // Received packets on unit 0 should be:
 * // Source port: 200, Destination port: 91
 * // Data: 0x00121314150100121314150281000065899901b000642d80010003e81bb96b2004a6e7484c3f90fb8a000008000000800807fa058100950e2fd500000000
 *
 * // sending a DMM to check if TOD is expected
 * tx 1 psrc=200 data=0x0000000102340000174b0c8b810000668902812f0020000000000000000000000000000000000000000000000000000000000000000000
 *
 * // Received packets on unit 0 should be:
 * // Source port: 200, Destination port: 91
 * // Data: 0x00121314150100121314150281000065899901b000642d80010003e81bb96b20043e62002c7335b12e000008000000800807fa058100952e2fd500000000
 *
 * // sending a DMR to check if TOD is expected
 * tx 1 psrc=200 data=0x0000000102340000174b0c8b810000668902812e0020000000000000000000000000000000000000000000000000000000000000000000
 *
 * // Received packets on unit 0 should be:
 * // Source port: 200, Destination port: 91
 * // Data: 0x00121314150100121314150281000065899901b000642d80010003e81bb96b300f19fdbe4ca6e7b7a6000008000000800807fa058100950e4fd500000000
 *
 * // sending a DMM to check if TOD is expected
 * tx 1 psrc=200 data=0x0000000102340000174b0c8b810000668902812f0020000000000000000000000000000000000000000000000000000000000000000000
 *
 * // Received packets on unit 0 should be:
 * // Source port: 200, Destination port: 91
 * // Data: 0x00121314150100121314150281000065899901b000642d80010003e81bb96b300f5d43262cdad1648c000008000000800807fa058100952e4fd500000000
 *
 * Test Scenario - end
*/

/*
 * 2nd example - trapping DM on Upmep to RCPU.
 *
 * Test Scenario - start
 *
 * ./bcm.user
 * 0: tr 141 NoInit=1
 *
 * config add tm_port_header_type_out_203.0=ENCAP_EXTERNAL_CPU
 * config add custom_feature_init_verify=1
 *
 * 0: tr 141 NoDeinit=1
 *
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dnx/field/cint_field_oam_layer_index.c
 * cint
 * field_oam_layer_index_main(0,0);
 * exit;
 *
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_tpid.c
 * cint ../../../../src/examples/sand/cint_vswitch_metro_mp.c
 * cint ../../../../src/examples/sand/cint_sand_advanced_vlan_translation_mode.c
 * cint ../../../../src/examples/sand/cint_l2_traverse.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_vlan.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_oam.c
 * cint ../../../../src/examples/sand/cint_sand_oam.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dnx/oam/cint_oam_basic.c
 * cint ../../../../src/examples/dnx/crps/cint_crps_oam_config.c
 * cint ../../../../src/examples/dnx/internal/cint_internal_oam_jr2.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint
 * single_vlan_tag=1;
 * oam_run_with_defaults(0,200,201,202);
 * exit;
 *
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dnx/oam/cint_dnx_oam_trap_dm_to_external_cpu.c
 * cint
 * trap_dm_to_rcpu_info.rcpu_port=203;
 * print rcpu_tunnel_create(0);
 * print rcpu_global_set(0);
 * print rcpu_port_set(0);
 *
 * // below trap_id can be obtained when calling appl_dnx_oam_trap_config with bcmRxTrapUserDefine in appl_ref_oam_init.c
 * trap_dm_to_rcpu_info.rcpu_ingress_cpu_trap_id=0xBD;
 * print rcpu_uc_trap_set(0);
 *
 * print rcpu_global_outlifs_set(0,BCM_L3_ITF_VAL_GET(trap_dm_to_rcpu_info.rcpu_l3a_intf_id),0,0);
 *
 * // below trap_code is in system header
 * trap_dm_to_rcpu_info.rcpu_trap_code_in_sys_header=0xE0;
 * print cint_dnx_oam_trap_dm_with_4b_tod_to_rcpu_epmf_set(0, 1);
 * exit;
 *
 * // sending a DMR to check if TOD is expected
 * tx 1 psrc=200 data=0x000000ffffff0000174b0c8b8100000a8902a12e0020000000000000000000000000000000000000000000000000000000000000000000
 *
 * // Received packets on unit 0 should be:
 * // Source port: 200, Destination port: 91
 * // Data: 0x00121314150100121314150281000065899901a000642d80010013a800000028047282bc00000008000400800007fa05830090008e050000000064000000
 *
 * // sending a DMM to check if TOD is expected
 * tx 1 psrc=200 data=0x000000ffffff0000174b0c8b8100000a8902a12f0020000000000000000000000000000000000000000000000000000000000000000000
 *
 * // Received packets on unit 0 should be:
 * // Source port: 200, Destination port: 91
 * // Data: 0x00121314150100121314150281000065899901a000642d80010013a800000028041cb8f400000008000400800007fa05830090008e050000000064000000
 *
 * // sending a DMR to check if TOD is expected
 * tx 1 psrc=200 data=0x000000ffffff0000174b0c8b8100000a8902a12e0020000000000000000000000000000000000000000000000000000000000000000000
 *
 * // Received packets on unit 0 should be:
 * // Source port: 200, Destination port: 91
 * // Data: 0x00121314150100121314150281000065899901a000642d80010013a8000000380f22f51000000008000400800007fa05830090008e050000000064000000
 *
 * // sending a DMM to check if TOD is expected
 * tx 1 psrc=200 data=0x000000ffffff0000174b0c8b8100000a8902a12f0020000000000000000000000000000000000000000000000000000000000000000000
 *
 * // Received packets on unit 0 should be:
 * // Source port: 200, Destination port: 91
 * // Data: 0x00121314150100121314150281000065899901a000642d80010013a8000000380f1c1b8200000008000400800007fa05830090008e050000000064000000
 *
 * Test Scenario - end
 */

/*
 *
 * 3rd example - TWAMP trapped by PMF to RCPU
 *
 * Test Scenario - start
 *
 * ./bcm.user
 * 0: tr 141 NoInit=1
 *
 * config add tm_port_header_type_out_203.0=ENCAP_EXTERNAL_CPU
 * config add custom_feature_init_verify=1
 *
 * 0: tr 141 NoDeinit=1
 *
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_multicast.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vlan.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../../../src/examples/dnx/twamp/cint_dnx_twamp_field.c
 * cint ../../../../src/examples/dnx/twamp/cint_dnx_twamp.c
 * cint
 * twamp_tx_rx_example(0,0,200,201,203,-1);
 * exit;
 *
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dnx/oam/cint_dnx_oam_trap_dm_to_external_cpu.c
 * cint
 * trap_dm_to_rcpu_info.rcpu_port=203;
 * rcpu_tunnel_create(0);
 * rcpu_global_set(0);
 * rcpu_port_set(0);
 *
 * // below trap_id can be obtained when calling bcm_rx_trap_type_create with bcmRxTrapUserDefine in twamp_tx_rx_example()
 * trap_dm_to_rcpu_info.rcpu_ingress_cpu_trap_id=0xBE;
 * rcpu_uc_trap_set(0,1);
 *
 * // below trap_code is in system header
 * trap_dm_to_rcpu_info.rcpu_trap_code_in_sys_header=0xBE;
 * cint_dnx_oam_trap_dm_with_4b_tod_to_rcpu_epmf_set(0,0);
 * exit;
 *
 * // Jericho2 -> CPU (TWAMP RX)
 * tx 1 psrc=200 data=0x000c000100880000000255008100000a08004500004800000000ff119da1140000020a000002c351035e00340abe00000000000000000000000000010000000000000000000000000001000000000000111100010000ff000000
 *
 * // Received packets on unit 0 should be:
 * // Source port: 200, Destination port: 91
 * // Data: 0x001213141501001213141502810000658999021400642d81000033e8c7e2bc30b43f5cf28ce1b773fc000416000a00005007fa01030002468be50001ffff0c000c000100880000000255008100000a08004500004800000000ff
 *
 * Test Scenario - end
*/

struct dnx_oam_trap_dm_to_rcpu_info_t
{
    /* Remote port information */
    int rcpu_port;
    int rcpu_port_profile;

    /* Ethernet header information */
    bcm_mac_t rcpu_src_mac;
    bcm_mac_t rcpu_dst_mac_trap;
    int rcpu_ethertype;
    int rcpu_eth_out_rif;
    bcm_if_t rcpu_l3a_intf_id;

    /* Trap information */
    int is_upmep;
    int rcpu_ingress_cpu_trap_id;
    int rcpu_trap_code_in_sys_header;

    /* General config */
    int rcpu_global_outlifs[4];
};

dnx_oam_trap_dm_to_rcpu_info_t trap_dm_to_rcpu_info = {
    /* rcpu_port | rcpu_port_profile*/
    0,           3,
    /* rcpu_src_mac                     | rcpu_dst_mac_trap */
    {0x00, 0x12, 0x13, 0x14, 0x15, 0x02}, {0x00, 0x12, 0x13, 0x14, 0x15, 0x01},
    /* rcpu_ethertype | rcpu_eth_out_rif | rcpu_l3a_intf_id */
    0x8999,           101,            0,
    /* is_upmep */
    0,
    /* rcpu_ingress_cpu_trap_id | rcpu_trap_code_in_sys_header */
    0,                              0xE0,
    /* rcpu_global_outlifs */
    {0}
};

/*
 * Global setting for packet to External CPU.
 */
int rcpu_global_set (
    int unit)
{
    bcm_error_t rv;

    /*
     * FTMH Ethertype is device configuration
     */
    rv = bcm_switch_control_set(unit, bcmSwitchFtmhEtherType, trap_dm_to_rcpu_info.rcpu_ethertype);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_control_set with type=bcmSwitchFTMHEthertype\n");
        return rv;
    }

    return rv;
}

/*
 * Port setting for External CPU.
 */
int rcpu_port_set (
    int unit)
{
    bcm_error_t rv;

    rv = bcm_port_class_set(unit, trap_dm_to_rcpu_info.rcpu_port, bcmPortClassFieldEgressPacketProcessingPortCs, trap_dm_to_rcpu_info.rcpu_port_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcmPortClassFieldEgressPacketProcessingPortCs \n");
        print rv;
        return rv;
    }

    return rv;
}

/*
 * create RCPU tunnel and vlan editing
 * Relevant for JR2 device only
 * In JR2 RCPU tunnel is done by Routing processing of Intf and ARP+AC object.
 */
int rcpu_tunnel_create (
    int unit)
{
    bcm_error_t rv;
    int eth_out_rif;
    int flags;
    int arp_outlif;
    bcm_gport_t arp_ac_gport;
    bcm_mac_t src_mac = {0x00, 0x12, 0x13, 0x14, 0x15, 0x02};
    bcm_mac_t dst_mac = {0x00, 0x12, 0x13, 0x14, 0x15, 0x01};

    eth_out_rif = trap_dm_to_rcpu_info.rcpu_eth_out_rif;
    /*
     * Create Eth rif for outer Ethernet SRC MAC
     */
    rv = intf_eth_rif_create(unit, eth_out_rif, src_mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    /*
     * Create ARP+AC entry for outer Ethernet and set its properties
     * ARP+AC indication is flag BCM_L3_FLAGS2_VLAN_TRANSLATION
     */
    rv = l3__egress_only_encap__create_inner(unit, 0, &trap_dm_to_rcpu_info.rcpu_l3a_intf_id, trap_dm_to_rcpu_info.rcpu_dst_mac_trap, eth_out_rif, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only\n");
        return rv;
    }

    return rv;
}

/*
 * Update trap for CFM packets to remote CPU
 */
int rcpu_uc_trap_set (
    int unit,
    int update_rcpu_encap_id)
{
    bcm_error_t rv;
    bcm_rx_trap_config_t config;
    int flags = 0;

    sal_memset(&config, 0, sizeof(config));

    rv = bcm_rx_trap_get(unit, trap_dm_to_rcpu_info.rcpu_ingress_cpu_trap_id, &config);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap set \n");
        return rv;
    }

    /* For port dest change */
    config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
    if (config.egress_forwarding_index != 0)
    {
        config.flags |= BCM_RX_TRAP_UPDATE_EGRESS_FWD_INDEX;
    }
    if (update_rcpu_encap_id)
    {
        bcm_gport_t gport_tunnel_temp;      /* rx trap works with GPORTs */
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport_tunnel_temp, trap_dm_to_rcpu_info.rcpu_l3a_intf_id);

        config.flags |= BCM_RX_TRAP_UPDATE_ENCAP_ID;
        config.encap_id = gport_tunnel_temp;
    }

    config.dest_port = trap_dm_to_rcpu_info.rcpu_port;

    rv = bcm_rx_trap_set(unit, trap_dm_to_rcpu_info.rcpu_ingress_cpu_trap_id, &config);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap set \n");
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Update trap for CFM packets to remote CPU
 */
int rcpu_global_outlifs_set (
    int unit,
    int g_outlif0,
    int g_outlif1,
    int g_outlif2)
{
    trap_dm_to_rcpu_info.rcpu_global_outlifs[0] = g_outlif0;
    trap_dm_to_rcpu_info.rcpu_global_outlifs[1] = g_outlif1;
    trap_dm_to_rcpu_info.rcpu_global_outlifs[2] = g_outlif2;

    return BCM_E_NONE;
}

/**
* \brief
*  Example to override the global_outlifs with any tunnel encapsulation pointers and update ACE value
*  The ePMF CS is based on the out-port to the remote cpu.
*  The ePMF Qualifier is also based on the out-port to the remote cpu(Actual not needed in the example).
*
*  It's recommended the users to define thier own ePMF configurations based on their applications.
*
*/
int cint_dnx_oam_trap_dm_with_4b_tod_to_rcpu_epmf_set (
    int unit,
    int update_global_outlif)
{
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_format_t ace_format_id;
    uint32 ace_entry_handle;

    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_group_info_t fg_info;
    bcm_field_presel_entry_id_t p_id;

    bcm_field_context_t context_id;
    bcm_field_presel_t presel_id = 60;
    bcm_field_group_t fg_id;
    bcm_field_entry_t epmf_entry_handle;

    int rcpu_port_profile = 3;
    void *dest_char;
    int idx;
    int is_epmf_ace_support = *(dnxc_data_get(unit, "field", "ace", "supported", NULL));
    int rv = BCM_E_NONE;

    /*
     * Create the ACE entry for updating the outLIFs in some devices
     */
    if (is_epmf_ace_support)
    {
        /** define ace action */
        bcm_field_ace_format_info_t_init(&ace_format_info);
        ace_format_info.nof_actions = update_global_outlif ? 4 : 1;
        ace_format_info.action_types[0] = bcmFieldActionAceContextValue;
        ace_format_info.action_types[1] = bcmFieldActionOutVport0Raw;
        ace_format_info.action_types[2] = bcmFieldActionOutVport1Raw;
        ace_format_info.action_types[3] = bcmFieldActionOutVport2Raw;
        rv = bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_ace_format_add\n", rv);
            return rv;
        }

        bcm_field_ace_entry_info_t_init(&ace_entry_info);
        ace_entry_info.nof_entry_actions = ace_format_info.nof_actions;
        ace_entry_info.entry_action[0].type = bcmFieldActionAceContextValue;
        ace_entry_info.entry_action[0].value[0] = bcmFieldAceContextDMToRCPU;
        ace_entry_info.entry_action[1].type = bcmFieldActionOutVport0Raw;
        ace_entry_info.entry_action[1].value[0] = trap_dm_to_rcpu_info.rcpu_global_outlifs[0];
        ace_entry_info.entry_action[2].type = bcmFieldActionOutVport1Raw;
        ace_entry_info.entry_action[2].value[0] = trap_dm_to_rcpu_info.rcpu_global_outlifs[1];
        ace_entry_info.entry_action[3].type = bcmFieldActionOutVport2Raw;
        ace_entry_info.entry_action[3].value[0] = trap_dm_to_rcpu_info.rcpu_global_outlifs[2];
        rv = bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, &ace_entry_handle);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_ace_entry_add\n", rv);
            return rv;
        }
    }

    /*
     * Context create
     */
    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "External_CPU_Tunnel_Ptr_Set_ePMF_CTX", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageEgress, &context_info, &context_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_context_create\n", rv);
       return rv;
    }

    /** Set the preselectors. */
    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = presel_id;
    p_id.stage = bcmFieldStageEgress;

    p_data.entry_valid = TRUE;
    p_data.context_id = context_id;
    p_data.nof_qualifiers = 1;
    p_data.qual_data[0].qual_type = bcmFieldQualifyPortClassPacketProcessing;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = rcpu_port_profile;
    p_data.qual_data[0].qual_mask = 7;
    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(ePMF) context(%d)! \n", presel_id, context_id);


    /*
     * Add the FG.
     */
    bcm_field_group_info_t_init(&fg_info);

    fg_info.stage = bcmFieldStageEgress;
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.nof_quals = 3;
    fg_info.qual_types[0] = bcmFieldQualifyPortClassPacketProcessing;
    fg_info.qual_types[1] = bcmFieldQualifyOamTsSystemHeader;
    fg_info.qual_types[2] = bcmFieldQualifyFhei;

    if (is_epmf_ace_support) {
        fg_info.nof_actions = 1;
        fg_info.action_types[0] = bcmFieldActionAceEntryId;
    } else {
        fg_info.nof_actions = 4;
        fg_info.action_types[0] = bcmFieldActionAceContextValue;
        fg_info.action_types[1] = bcmFieldActionOutVport0Raw;
        fg_info.action_types[2] = bcmFieldActionOutVport1Raw;
        fg_info.action_types[3] = bcmFieldActionOutVport2Raw;
    }
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "Ext_CPU_4B_TOD_Set_ePMF_GRP", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, 0, &fg_info, &fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in ePMF \n", fg_id);


    /*
     *Attach the FG to the context
     */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for(idx=0; idx < fg_info.nof_quals; idx++)
    {
        attach_info.key_info.qual_types[idx] = fg_info.qual_types[idx];
        attach_info.key_info.qual_info[idx].input_type = bcmFieldInputTypeMetaData;
    }
    for(idx=0; idx < fg_info.nof_actions; idx++)
    {
        attach_info.payload_info.action_types[idx] = fg_info.action_types[idx];
    }

    rv = bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached  FG (%d) to context (%d) Stage(ePMF)\n", fg_id, context_id);


    /*
     * Add an entry to update the outLIFs with the given values
     */
    bcm_field_entry_info_t_init(&ent_info);

    ent_info.nof_entry_quals = fg_info.nof_quals;

    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = rcpu_port_profile;
    ent_info.entry_qual[0].mask[0] = 0x0;

    /* Subtype should be 2 or 3 */
    ent_info.entry_qual[1].type = fg_info.qual_types[1];
    ent_info.entry_qual[1].value[0] =0;
    ent_info.entry_qual[1].value[1] = 0x2000;
    ent_info.entry_qual[1].mask[0] = 0x00000000;
    ent_info.entry_qual[1].mask[1] = 0xE000;

    /* fhei-type should be 5, and fhei-code=trap_code */
    ent_info.entry_qual[2].type = fg_info.qual_types[2];
    ent_info.entry_qual[2].value[0] = 0x5 | (trap_dm_to_rcpu_info.rcpu_trap_code_in_sys_header<<4);
    ent_info.entry_qual[2].value[1] = 0x0;
    ent_info.entry_qual[2].mask[0] = 0x1fff;
    ent_info.entry_qual[2].mask[1] = 0x0;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    if (is_epmf_ace_support)
    {
        ent_info.entry_action[0].type = fg_info.action_types[0];
        ent_info.entry_action[0].value[0] = ace_entry_handle;
    }
    else
    {
        ent_info.entry_action[0].type = fg_info.action_types[idx];
        ent_info.entry_action[0].value[0] = bcmFieldAceContextDMToRCPU;
        for(idx=1; idx < fg_info.nof_actions; idx++)
        {
            ent_info.entry_action[idx].type = fg_info.action_types[idx];
            ent_info.entry_action[idx].value[0] = trap_dm_to_rcpu_info.rcpu_global_outlifs[idx-1];
        }
    }

    rv = bcm_field_entry_add(unit, 0, fg_id, &ent_info, &epmf_entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("FG (%d) Stage(ePMF) , entry_add: handle(%d) \n", fg_id, epmf_entry_handle);

    return 0;
}

