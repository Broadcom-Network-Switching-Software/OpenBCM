/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_ifa.c Purpose: basic examples for ifa 1.0 Datapath.
 */

/*
 * Test Scenario - Ingress Node
 *
 * Add Soc Properties for Recycle port:
 *               ucode_port_45=RCY.0:core_0.45
 *               tm_port_header_type_out_45=ETH
 *               tm_port_header_type_in_45=INJECTED
 *               ucode_port_46=RCY.1:core_1.46
 *               tm_port_header_type_out_46=ETH
 *               tm_port_header_type_in_46=INJECTED
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/sand/cint_ip_route_basic.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/dnx/instru/cint_field_IFA_datapath.c
 * cint ../../src/./examples/dnx/internal/cint_route_internal.c
 * cint
 * ifa_route_config(0,202,203,52430);
 * exit;
 *
 * # IPT packet
 * tx 1 psrc=202 data=0x000c00020000000007000100080045000057000000008006fa1dc08001017fffff023a98000000000000000000000010001000000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * # Received packets on unit 0 should be:
 * # Source port: 0, Destination port: 0
 * # Data: 0x00000000cd1d00123456789a81000064080045000057000000007f06fb1dc08001017fffff023a98000000000000000000000010001000000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/dnx/oam/cint_oam_basic.c
 * cint ../../src/./examples/dnx/crps/cint_crps_oam_config.c
 * cint ../../src/./examples/dnx/field/cint_field_ip_snoop.c
 * cint ../../src/./examples/dnx/instru/cint_instru_ipt.c
 * cint ../../src/./examples/dnx/instru/cint_field_IFA_datapath.c
 * cint ../../src/./examples/dnx/instru/cint_ifa.c
 * cint
 * cint_instru_ifa1_initiator(0,202,203,45,46,0xa1f1a1f1,0xa1f2a1f2);
 * exit;
 *
 * # IPT packet
 * tx 1 psrc=202 data=0x000c00020000000007000100080045000057000000008006fa1dc08001017fffff023a98000000000000000000000010001000000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * # Received packets on unit 0 should be:
 * # Source port: 0, Destination port: 0
 * # Data: 0x00000000cd1d00123456789a81000064080045000057000000007f06fb1dc08001017fffff023a98000000000000000000000010001000000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * # Source port: 0, Destination port: 0
 * # Data: 0x00000000cd1d00123456789a81000064080045000093000000007f06fae1c08001017fffff023a98000000000000000000000010001000000000a1f1a1f1a1f2a1f210010200003b0000ff0100001000003cffff00000000ffff4000800067800009ff0a17533f001f4f5ae7000000ca00cb00000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * # IPT packet
 * tx 1 psrc=202 data=0x000c00020000000007000100080045000057000000008006fa1dc08001017fffff023a98000000000000000000000010001000000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * # Received packets on unit 0 should be:
 * # Source port: 0, Destination port: 0
 * # Data: 0x00000000cd1d00123456789a81000064080045000057000000007f06fb1dc08001017fffff023a98000000000000000000000010001000000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * # Source port: 0, Destination port: 0
 * # Data: 0x00000000cd1d00123456789a81000064080045000093000000007f06fae1c08001017fffff023a98000000000000000000000010001000000000a1f1a1f1a1f2a1f210010200003b0000ff0100001000003cffff00010000ffff4000800067800009ff0a17533f000dfb17cb000000ca00cb00000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * # IPT packet
 * tx 1 psrc=202 data=0x000c000200000000070001009100000081000000080045000057000000008006fa1dc08001017fffff023a98000000000000000000000010001000000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * # Received packets on unit 0 should be:
 * # Source port: 0, Destination port: 0
 * # Data: 0x00000000cd1d00123456789a81000064080045000057000000007f06fb1dc08001017fffff023a98000000000000000000000010001000000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * # Source port: 0, Destination port: 0
 * # Data: 0x00000000cd1d00123456789a81000064080045000093000000007f06fae1c08001017fffff023a98000000000000000000000010001000000000a1f1a1f1a1f2a1f210010200003b0000ff0100001000003cffff00020000ffff4000800067800009ff0c001dab00106f4aef000000ca00cb00000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 */
/*
 * Test Scenario Transit & Egress Nodes
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/sand/cint_ip_route_basic.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint
 * dnx_basic_example(0,201,202,52430);
 * exit;
 *
 * # IPT packet
 * tx 1 psrc=201 data=0x000c00020000000007000100080045000073000000008006fa01c08001017fffff023a98000000000000000000000010001000000000a1f1a1f1a1f2a1f21001020000000000ff0100001000001cffff0001000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * # Received packets on unit 0 should be:
 * # Source port: 0, Destination port: 0
 * # Data: 0x00000000cd1d00123456789a81000064080045000073000000007f06fb01c08001017fffff023a98000000000000000000000010001000000000a1f1a1f1a1f2a1f21001020000000000ff0100001000001cffff0001000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/dnx/oam/cint_oam_basic.c
 * cint ../../src/./examples/dnx/crps/cint_crps_oam_config.c
 * cint ../../src/./examples/dnx/instru/cint_field_IFA_datapath.c
 * cint ../../src/./examples/dnx/instru/cint_field_IFA_egress_datapath.c
 * cint ../../src/./examples/dnx/instru/cint_ifa.c
 * cint
 * cint_instru_ifa1_transit(0,0xa1f1a1f1,0xa1f2a1f2,202);
 * exit;
 *
 * # IPT packet
 * tx 1 psrc=201 data=0x000c00020000000007000100080045000073000000008006fa01c08001017fffff023a98000000000000000000000010001000000000a1f1a1f1a1f2a1f21001020000000000ff0100001000001cffff0001000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * # Received packets on unit 0 should be:
 * # Source port: 0, Destination port: 0
 * # Data: 0x00000000cd1d00123456789a81000064080045000093000000007f06fae1c08001017fffff023a98000000000000000000000010001000000000a1f1a1f1a1f2a1f210010200003b0000ff0200001000003cffff00010000ffff4000800067000009ff080e8d19002514c39b000000c900ca00000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/dnx/oam/cint_oam_basic.c
 * cint ../../src/./examples/dnx/crps/cint_crps_oam_config.c
 * cint ../../src/./examples/dnx/instru/cint_field_IFA_datapath.c
 * cint ../../src/./examples/dnx/instru/cint_field_IFA_egress_datapath.c
 * cint ../../src/./examples/dnx/instru/cint_ifa.c
 * cint
 * cint_instru_ifa1_egress(0,0xa1f1a1f1,0xa1f2a1f2,202,245);
 * exit;
 *
 * # IPT packet
 * tx 1 psrc=201 data=0x000c00020000000007000100080045000073000000008006fa01c08001017fffff023a98000000000000000000000010001000000000a1f1a1f1a1f2a1f21001020000000000ff0100001000001cffff0001000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * # Received packets on unit 0 should be:
 * # Source port: 0, Destination port: 0
 * # Data: 0x033400000000000003e4000000000000d939a090000000000000000007fa0001001bd5a1950000000000cd1d00123456789a81000064080045000093000000007f06fae1c08001017fffff023a98000000000000000000000010001000000000a1f1a1f1a1f2a1f210010200003b0000ff0200001000003cffff00010000ffff4000800067000009ff0932f24f001da1d0cf000000c900ca00000000000000000111000000
 */



int DEFAULT_DEVICE_ID = 0xFFFF;
int DEFAULT_HOP_LIMIT = 0xff;
int DEFAULT_MAX_IFA_LENGTH = 0x1000;
int DEFAULT_IFA_SENDER = 0xFFFF;
int DEFAULT_IFA_TEMPLATE_ID = 0x2;

int ifa_encap_id;

/*
 * Get core from port id
 */
int get_core_from_port(int unit, int port, int * core)
{
    bcm_error_t rc ;
    uint32 dummy_flags ;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info ;
    bcm_gport_t gport = port;

    rc = bcm_port_get(unit, gport, &dummy_flags, &interface_info, &mapping_info);
    if (BCM_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
            (BSL_META_U(unit,"bcm_port_get failed. Error:%d (%s)\n"),
                                                    rc, bcm_errmsg(rc)));
        return -1;
    } else {
        *core = mapping_info.core ;
    }
    return 0;
}

/*
 * Example 1.0 Node configuration.
 * This example configures all node parameters through bcm_instru_ifa.
 * The IFA probe header and metadata will be built according to this configuration.
 * This configuration should be performed for all node types (intiator, transit
 * and terminator).
 */
int cint_ifa1_node_parameters_config(
        int unit,
        uint32 probe_1,
        uint32 probe_2)
{
    int rv = BCM_E_NONE;
    bcm_ifa_config_info_t config_data;

    /* Use default values for all fields. Probe Marker 1,2 must be provided */
    config_data.probemarker_1 = probe_1;
    config_data.probemarker_2 = probe_2;

    config_data.device_id = DEFAULT_DEVICE_ID;
    config_data.hop_limit = DEFAULT_HOP_LIMIT;
    config_data.max_payload_length = DEFAULT_MAX_IFA_LENGTH;
    config_data.senders_handle = DEFAULT_IFA_SENDER;
    config_data.template_id = DEFAULT_IFA_TEMPLATE_ID;

    /** Set ifa1 node */
    rv = bcm_ifa_config_info_set(unit , 0, &config_data);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_ifa_config_info_set failed\n");
        printf("rv %d\n",rv);
        return rv;
    }

    return rv;
}

/*
 * Example for counter for IFA
 * Counter will be used to count packets and stamping
 * the value on the IFA Header (sequence number)
 */
int cint_set_IFA_counter(
        int unit,int port, int *counter)
{
   int counter_if;
   int rv = 0;

   /* Allocate Counter */
   counter_if = 0;
   rv = set_counter_resource(unit, port, counter_if, 0, counter);
   if (rv != BCM_E_NONE)
   {
       printf("set_counter_resource failed\n");
       printf("rv %d\n",rv);
       return rv;
   }
   printf("Counter for IFA - 0x%x\n",*counter);

   return 0;
}


int cint_instru_ipt_init(int unit)
{
    int rv;
    uint32 ipt_profile;
    bcm_instru_ipt_t config;
    uint32 supported_flags_per_device = 0;

    ipt_profile = 3;
    bcm_instru_ipt_t_init(&config);

    config.node_type = bcmInstruIptNodeTypeTail;
    config.metadata_flags = supported_flags_per_device;

    rv  = bcm_instru_ipt_profile_set(unit, 0, ipt_profile, &config);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_instru_ipt_profile_set failed for Tail Edit\n");
        return rv;
    }
    return rv;

}


/*
 * Example cint for IFA 1.0 Transit Node.
 * iPMF1 will be used to trap IFA packets. This is done by probe_marker_1 and probe_marker_2
 * fields in IFA probe header. This cint will set the PMF trap with the expected probe_marker
 * values.
 * The PMF will set IPTProfile, IPTCommand and Outlif to add current node's metadata to the packet.
 * The Outlif will be used to access counter for metadata's counter field.
 *
 * If inport is NIF port, rx timestamping is enabled. For this case in_port_is_nif should be set to 1
 */
int cint_instru_ifa1_transit(
        int unit,
        uint32 probe_1,
        uint32 probe_2,
        int in_port,
        int out_port,
		int in_port_is_nif)
{
    bcm_field_entry_t entry_handle;
    int rv = BCM_E_NONE;
    bcm_instru_ifa_info_t ifa_info;
    int outlif;
    int counter;

    if(in_port_is_nif)
    {
      rv = cint_instru_config_nif_rx_timestamp(unit,in_port,0);
      if (rv != BCM_E_NONE)
      {
         printf("Error (%d), in NIF rx timestamp configuration \n", rv);
         return rv;
      }
    }

    if (*dnxc_data_get (unit,"instru","ipt","advanced_ipt",NULL))
    {
      /* devices require ipt configuration */
      rv = cint_instru_ipt_init(unit);
      if (rv != BCM_E_NONE)
      {
         printf("Error (%d), in ipt profile set \n", rv);
         return rv;
      }
    }

    entry_handle = 0;

    /* Configure Transit Node Parameters */
    rv = cint_ifa1_node_parameters_config(unit,probe_1,probe_2);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in configuration IFA node parameters \n", rv);
       return rv;
    }

    /* Configure Field */
    rv = cint_field_IFA_datapath_intermediate_ingress_main(unit);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in adding iPMF1 rule \n", rv);
       return rv;
    }

    rv = cint_field_IFA_datapath_intermediate_ingress_entry_add(unit,probe_1,probe_2,&entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in adding iPMF1 rule \n", rv);
       return rv;
    }

    return rv;
}

/*
 * Example cint for IFA 1.0 Egress Node.
 * ePMF1 will be added for trapping and recycling on egress side.
 * This is done by probe_marker_1 and probe_marker_2
 * fields in IFA probe header. This cint will set the PMF trap with the expected probe_marker
 * values.
 * The PMF will set "IFA-TRAP-Context"
 * This function also set out_port as "IFA-Egress-Port" according these two parameters packet will be recycled
 *
 * This function should be called after cint_instru_ifa1_transit function with same probe parameters.
 */
int cint_instru_ifa1_egress(
        int unit,
        uint32 probe_1,
        uint32 probe_2,
        int out_port)
{
    int rv = BCM_E_NONE;
    bcm_field_entry_t entry_handle = 0;
    bcm_instru_ifa_info_t ifa_info;

    /* Configure field */
    rv = cint_field_IFA_datapath_egress_main(unit);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in ePMF rule creation \n", rv);
       return rv;
    }

    /* Set ePMF to trap IFA packet */
    rv = cint_field_IFA_datapath_egress_entry_add(unit,probe_1,probe_2,&entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in adding ePMF entry \n", rv);
       return rv;
    }

    rv = cint_field_IFA_datapath_ingress_2nd_pass_entry_add(unit,&entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in adding 2nd pass entry \n", rv);
       return rv;
    }

    /* Out port configuration */
    rv = bcm_instru_gport_control_set(unit, out_port,0,
                                                 bcmInstruGportControlIptTrapToRcyEnable, TRUE);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_instru_gport_control_set \n", rv);
       return rv;
    }

    return rv;
}

/*
 * cint_instru_config_nif_rx_timestamp
 * ==========================
 *
 * Enables NIF's ingress timestamp
 *
 * in_port [in] - packet's in port
 * is_loopback_port [in] - Set to 1 in case of configuring port is loopback port
 *                         (used for recycling in IFA 1.0 initiator node.)
 */
int cint_instru_config_nif_rx_timestamp(
        int unit,
        int ingress_port,
		int is_loopback_port)
{
    int rv=0;
    bcm_port_phy_timesync_config_t timesync_phy_config;

    bcm_port_phy_timesync_config_t_init(&timesync_phy_config);
    timesync_phy_config.flags = (BCM_PORT_PHY_TIMESYNC_ENABLE | BCM_PORT_PHY_TIMESYNC_ONE_STEP_ENABLE);
    if(is_loopback_port)
    {
      rv = bcm_port_loopback_set(unit, ingress_port, 0);   /* disabled loopback */
      if (rv != BCM_E_NONE)
      {
         printf("Error (%d), in bcm_port_loopback_set (disable loopback) \n", rv);
         return rv;
      }
    }
    rv = bcm_port_enable_set(unit, ingress_port, 0);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_port_enable_set (disable port) \n", rv);
       return rv;
    }
    rv = bcm_port_phy_timesync_config_set(unit, ingress_port, &timesync_phy_config);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_port_phy_timesync_config_set \n", rv);
       return rv;
    }
    rv = bcm_port_enable_set(unit, ingress_port, 1);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_port_enable_set (enable port) \n", rv);
       return rv;
    }
    if(is_loopback_port)
    {
      rv = bcm_port_loopback_set(unit, ingress_port, 2);   /* enabled phy loopback */
      if (rv != BCM_E_NONE)
      {
         printf("Error (%d), in bcm_port_loopback_set (enabled phy loopback) \n", rv);
         return rv;
      }
    }
    return rv;
}

/*
 * cint_instru_ifa1_set_recycle_port
 * ==========================
 *
 * Chooses recycle port on the same core as the in_port and sets ifa1 trap_context_profile
 * Add system to local port mapping for in_port
 *
 * in_port [in] - packet's in port
 * recycle_port_0 [in] - recycle_port on core 0
 * recycle_port_1 [in] - recycle_port on core 1
 * *recycle_port [out] - chosen recycle_port
 */
int cint_instru_ifa1_set_recycle_port(
        int unit,
        int in_port,
        int recycle_port_0,
        int recycle_port_1,
        int *recycle_port)
{
    int core=0;
    int rv=0;

    rv = get_core_from_port(unit, in_port, &core);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), getting core from in port.\n", rv);
        return rv;
    }
    if(core==0)
    {
        *recycle_port=recycle_port_0;
    }
    else
    {
    	*recycle_port=recycle_port_1;
    }
    printf("Recycling on core %d\n",core);

    rv = bcm_port_control_set(unit, in_port, bcmPortControlSystemPortInjectedMap, 1);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_control_set failed bcmPortControlSystemPortInjectedMap $rv\n");
        return rv;
    }

    rv = bcm_instru_gport_control_set(unit,*recycle_port,0,bcmInstruGportControlIfaInitiatorRcyEnable,1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_instru_gport_control_set .\n", rv);
        return rv;
    }
    return rv;
}


/*
 * cint_instru_ifa1_initiator
 * ==========================
 *
 * Example cint for IFA 1.0 Ingress Node.
 *
 * Operation principle:
 *   - In the first cycle required packets (IPv4 in this example) are snooped.
 *     While original packet is forwarded on the normal path, the snooped copy
 *     is encapsulated with IFA probe-header and recycled.
 *   - In the second cycle the packet is encapsulated with IFA metadata and forwarded
 *     to the same destination as the original packet.
 *
 *
 *     First Cycle: The packet is snooped in iPMF1. Original copy is forwarded to the out_port, so it does
 *                   not hit ePMF. The snooped IFA copy, gets recycle_port so is hit in ePMF. ePMF sets
 *                   Tail_edit_profile=3. This chooses IFA_First Context in Term, Forward and ENC1-3.
 *                   IFA Header is added to the packet, including Hop_Count=0, Length=28 (Header's length)
 *                   and stamped Sequence Number. Then the packet is recycled for metadata encapsulation.
 *     Second Cycle: The packet hits iPMF1 since it has IFA Probe Header. Container is set and tail-edit-profile
 *                   is set to 3. iPMF3 is hit because of the container and then INT is set to 1. The contexts of
 *                   IFA are chosen in Term, Forward and ENC1-3. IFA Header is updated by hop++ and length+=32.
 *                   Sequence number is copied from the original IFA header.
 *
 * Pre-configuration:
 *  - Configure Route both for inport and the recycle port
 *
 * Configuration steps:
 *   - Set Node parameters including IFA session's probe-header
 *   - Configure counter to be used for sequence number stamping
 *   - Setup field configurations
 *   - Create IFA entity for IFA Header
 *   - Create IFA entity for IFA Metadata
 *
 * Recycle options:
 *   - Using recycle ports (one for each core is required)
 *   - Using loopback on NIF ports (one for each core is required).
 *     PHY loopback is used, and timestamping on these ports is enabled.
 *     For this option recycle_ports_are_nif should be set to 1.
 */
int cint_instru_ifa1_initiator(
        int unit,
        int in_port,
        int out_port,
		int recycle_ports_are_nif,
        int recycle_port_0,
        int recycle_port_1,
        uint32 probe_1,
        uint32 probe_2)
{
    bcm_field_entry_t entry_handle;
    bcm_instru_ifa_info_t ifa_info;
    int header_counter;
    int recycle_port;

    entry_handle = 0;

    BCM_IF_ERROR_RETURN(cint_instru_ifa1_set_recycle_port(unit,in_port,recycle_port_0,recycle_port_1,&recycle_port));

    if(recycle_ports_are_nif)
    {
        /* Configure phy loopback on nif port */
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit,recycle_port_0,BCM_PORT_LOOPBACK_PHY));
        print("phy loopback was set port %d",recycle_port_0);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit,recycle_port_1,BCM_PORT_LOOPBACK_PHY));
        print("phy loopback was set port %d",recycle_port_1);

        BCM_IF_ERROR_RETURN(cint_instru_config_nif_rx_timestamp(unit,recycle_port_0,1));
        BCM_IF_ERROR_RETURN(cint_instru_config_nif_rx_timestamp(unit,recycle_port_1,1));
    }

    /* Configure Initiator Node Parameters */
    BCM_IF_ERROR_RETURN(cint_ifa1_node_parameters_config(unit,probe_1,probe_2));

    /* Set counter attached to outlif for sequence counting */
    BCM_IF_ERROR_RETURN(cint_set_IFA_counter(unit,recycle_port,&header_counter));

    /* *Create IFA entity */
    ifa_info.counter_command_id = 0;
    ifa_info.stat_cmd = header_counter;

    BCM_IF_ERROR_RETURN(bcm_instru_ifa_encap_create(unit,&ifa_info));
    ifa_encap_id = ifa_info.ifa_encap_id;

    /* Set pmf to trap all IPv4 packets and start an IFA flow */
    BCM_IF_ERROR_RETURN(cint_field_group_const_example_IFA_gen(unit, ifa_encap_id, recycle_port));

    /* Set pmf to add Metadata */
    BCM_IF_ERROR_RETURN(cint_field_IFA_datapath_intermediate_ingress_entry_add(unit,probe_1,probe_2,&entry_handle));

    return 0;
}
