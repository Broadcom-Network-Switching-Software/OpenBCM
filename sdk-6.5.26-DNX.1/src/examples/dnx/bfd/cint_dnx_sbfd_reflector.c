/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~SBFD reflector test~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * $File: cint_dnx_sbfd_reflector.c
 *
 * Purpose: Examples for SBFDoIPV4, SBFDoIPV6, SBFDoMPLS reflector
 *
 * Usage:
 *
 *   Load SBFD Reflector soc properties
 * ./bcm.user
 * cd ../../../../regress/bcm
 * bcm shell "config add ucode_port_50.0=RCY.1:core_0.50"
 * bcm shell "config add tm_port_header_type_in_50.0=RCH_0" (or IBCH1_MODE, if supported)
 * bcm shell "config add tm_port_header_type_out_50.0=ETH"
 * bcm shell "0: tr 141 NoInit=1"
 *
 * #############################
 * # SBFD reflector over IPv4
 * #############################
 *
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_mpls.c
 * cint ../../src/./examples/dnx/bfd/cint_dnx_bfd.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_util_rch.c
 * cint ../../src/./examples/dnx/bfd/cint_dnx_sbfd_reflector.c
 * cint
 * dnx_sbfd_reflector_endpoint_create(0,200,201);
 * exit;
 *
 * tx SBFDoIPv4 packet for reflecting:
 *   tx 1 psrc=200 data=0x000c0002000000000000cd1d810000c8080045ff003400000000ff11a6a005050505030f070156ce1e68002000002002d0180001000200031034000000020000000000000000
 *
 * Received packets on unit 0 should be:
 *   Data: 0x00000000cd1d00123456789a8100f0c9080045ff003400000000fe11a7a0030f0701050505051e6856ce002000002000d0180003103400010002000000020000000a0000002000000000
 *
 * #############################
 * # SBFD reflector over IPv6
 * #############################
 *
 * Same as reflector over IPv4, except below:
 * cint
 * dnx_sbfd_reflector_info_glb.is_sbfd_ipv6=1;
 * dnx_sbfd_reflector_endpoint_create(0,200,201);
 *
 * -- Send packet --
 *   tx 1 psrc=200 data=0x000c0002000000000700010086dd60000000002011ff00000000000000000000ffffc08001010000000000000000123400000000ff134e341e6800209f662082d0180000200400031034000000020000000a00000000
 *
 * Received packets on unit 0 should be: 
 *   Data: 0x00000000cd1d00123456789a810000c986dd60000000002011fe0000000000000000123400000000ff1300000000000000000000ffffc08001011e684e3400209fc82000d0180003103400002004000000020000000a00000020
 *
 * #############################
 * # SBFD reflector over MPLS
 * #############################
 *
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_mpls.c
 * cint ../../src/./examples/dnx/mpls/cint_dnx_mpls_termination_basic.c
 * cint ../../src/./examples/dnx/bfd/cint_dnx_bfd.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_util_rch.c
 * cint ../../src/./examples/dnx/bfd/cint_dnx_sbfd_reflector.c
 * cint
 * dnx_sbfd_reflector_info_glb.is_sbfd_mpls=1;
 * dnx_sbfd_reflector_endpoint_create(0,200,201);
 * exit;
 *
 * tx SBFDoMPLS packet for reflecting:
 *   tx 1 psrc=200 data=0x000c0002000000000000cd1d810000c88847017701ff45ff0034000000000111a4a105050505030f070156ce1e68002000002002d0180001000200031034000000020000000000000000
 *
 * Received packets on unit 0 should be:
 *   Data: 0x00000000cd1d000c000200018100f0c9080045ff003400000000fe11a7a0030f0701050505051e6856ce002000002000d0180003103400010002000000020000000a0000002000000000
 *
*/

/*
 * SBFD reflector variables
 */
struct dnx_sbfd_reflector_config_s
{
    uint32 flags2;
    uint8 is_sbfd_mpls;
    uint8 is_sbfd_ipv6;
    int local_discr;
    int local_min_rx;
    bcm_ip_t src_ip;
    bcm_ip6_t src_ipv6;
    uint8 reflector_recycle_port;
};

/*
 * SBFD reflector init configuration
 */
dnx_sbfd_reflector_config_s dnx_sbfd_reflector_info_glb = {

    /*
     * flags2
     */
    0,
    /*
     * is_sbfd_mpls
     */
    0,
    /*
     * is_sbfd_ipv6
     */
    0,
    /*
     * local_discr
     */
    0x31034,
    /*
     * local_min_rx
     */
    10,
    /*
     * src_ip
     */
    0x30F0701,
    /*
     * src_ipv6
     */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13},
    /*
     * sbfd_reflector_recycle_port
     */
    50
};

/*
 * The structure will hold the results for returned values
 */
struct dnx_sbfd_reflector_result_s
{
    int reflector_trap_code;
    int reflector_encap_id;
    bcm_bfd_endpoint_t mep_id;
};

dnx_sbfd_reflector_result_s dnx_sbfd_reflector_glb_result = {0};


/**
 * \brief
 *
 *  This function creates the RCH encapsulation for the SBFD.
 *
 * \param [in] unit - The unit number.
 * \param [out] recycle_entry_encap_id - The RCH encapuslation id.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 *
 */
int dnx_sbfd_reflector_create_recycle_entry(
    int unit,
    int *recycle_entry_encap_id)
{
    BCM_IF_ERROR_RETURN_MSG(create_recycle_entry_with_defaults(unit, recycle_entry_encap_id), "");
    return BCM_E_NONE;
}

/**
 * \brief
 *
 *  This function creates PMF to set TTL to 255
 *
 * \param [in] unit - The unit number.
 * \param [in] recycle_port - recycle port.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 *
 */
int dnx_sbfd_reflector_ttl_set(
    int unit,
    bcm_port_t recycle_port)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    int trap_code_qualifier;
    uint32 attach_presel = 10;
    bcm_gport_t gport;
    bcm_field_group_t fg_id=0;
    bcm_field_entry_t fg_entry_id = 0;
    bcm_field_context_t field_context_id;

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF3;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyInPort;

    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionTtlSet;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &fg_info, &fg_id), "");
    printf("Created TCAM field group (%d) in iPMF1 \n", fg_id);

    /**Attach context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    BCM_IF_ERROR_RETURN_MSG(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, &field_context_id), "");

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, fg_id, field_context_id, &attach_info), "");
    printf("Attached  FG (%d) to context (%d)\n",fg_id,field_context_id);

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    ent_info.nof_entry_quals = 1;

    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    /**in port 13*/
    BCM_GPORT_LOCAL_SET(gport, recycle_port);
    ent_info.entry_qual[0].value[0] = gport;
    ent_info.entry_qual[0].mask[0] = 0xffffffff;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = 255;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0,fg_id, &ent_info, &fg_entry_id), "");
    printf("Entry add: id:(0x%x)  Src_ip:(0x%x) tc_val:(%d)\n",fg_entry_id,ent_info.entry_qual[0].value[0],
                                                                            ent_info.entry_action[0].value[0] );

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = attach_presel;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = TRUE;
    p_data.context_id = field_context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(unit, 0, &p_id, &p_data), "");

    printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(Ethernet) \n",
            attach_presel,field_context_id);

    return BCM_E_NONE;
}

/**
 * \brief
 *
 *  This function creates the sbfd reflector encapsulation.
 *
 * \param [in] unit - The unit number.
 * \param [out] recycle_entry_encap_id - The TWAMP encapuslation id.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 *
 */
int dnx_sbfd_reflector_create_reflector(
    int unit,
    int *sbfd_encap_id)
{
    bcm_switch_reflector_data_t sbfd_reflector;
    if(dnx_sbfd_reflector_info_glb.is_sbfd_ipv6)
    {
        sbfd_reflector.type = bcmSwitchReflectorSbfdIp6;
    } else if(dnx_sbfd_reflector_info_glb.is_sbfd_mpls)
    {
        sbfd_reflector.type = bcmSwitchReflectorSbfdMpls;
    } else
    {
        sbfd_reflector.type = bcmSwitchReflectorSbfdIp4;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_reflector_create(unit, 0, sbfd_encap_id, &sbfd_reflector), "");

    printf("sbfd_encap_id:0x%x\n", *sbfd_encap_id);

    return BCM_E_NONE;
}

/**
 *  SBFD reflector configuration
 *   1) Load SBFD Reflector soc properties
        "ucode_port_50=RCY.1"
        "tm_port_header_type_in_50=RCH_0" (or IBCH1_MODE, if supported)
        "tm_port_header_type_out_50=ETH"
 *   2) Create RCH encap
 *   3) Configure recycle port and term_context_port_profile  by bcmPortControlRecycleAppSbfdReflector
 *   4) Create SBFD reflector outlif by bcmSwitchReflectorSbfdIp4 and bcmSwitchReflectorSbfdMpls
 *   5) Call PMF to set TTL to 255
 *   6) Create TRAP to trap SBFD packet to recycle port
 * \param [in] unit - The unit number.
 * \param [out] trap_code -  trap code
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 *
 */
int dnx_sbfd_reflector_configure(
    int unit,
    int *trap_code)
{
    int reflector_encap_id;
    int recycle_entry_encap_id;
    int trap_code_sbfd;
    int rcy_port = dnx_sbfd_reflector_info_glb.reflector_recycle_port;
    bcm_rx_trap_config_t trap_config_sbfd;
    uint32 ibch1_supported = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL);

    /** 1. Create RCH encap ***/
    BCM_IF_ERROR_RETURN_MSG(dnx_sbfd_reflector_create_recycle_entry(unit, &recycle_entry_encap_id), "");

    if (!ibch1_supported)
    {
        /** 2. Configure recycle port */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, rcy_port, bcmPortControlOverlayRecycle, 1), "");
        /** 3. Configure recycle port term_context_port_profile*/
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, rcy_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppSbfdReflector), "");
    }
    /** 4. Set port class in order for PMF to crop RCH ad IRPP */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, rcy_port, bcmPortClassFieldIngressPMF3TrafficManagementPortCs, 4), "");

    /** 5. Set ttl to 255 */
    BCM_IF_ERROR_RETURN_MSG(dnx_sbfd_reflector_ttl_set(unit,rcy_port), "");

    /** 6. Create SBFD reflector encap */
    BCM_IF_ERROR_RETURN_MSG(dnx_sbfd_reflector_create_reflector(unit, &reflector_encap_id), "");
    dnx_sbfd_reflector_glb_result.reflector_encap_id = reflector_encap_id;

    /** 7. Create TRAP to trap SBFD packet to recycle port */
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_code_sbfd), "");

    bcm_rx_trap_config_t_init(&trap_config_sbfd);
    trap_config_sbfd.flags = BCM_RX_TRAP_UPDATE_ENCAP_ID | BCM_RX_TRAP_UPDATE_DEST;

    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(trap_config_sbfd.encap_id2, reflector_encap_id);
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(trap_config_sbfd.encap_id, recycle_entry_encap_id);
    trap_config_sbfd.dest_port = rcy_port;
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, trap_code_sbfd, &trap_config_sbfd), "");

    *trap_code = trap_code_sbfd;
    dnx_sbfd_reflector_glb_result.reflector_trap_code = trap_code_sbfd;

    return BCM_E_NONE;
}

/**
* Create SBFD reflector endpoint
 * create SBFD reflector endpoint by BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_REFLECTOR
 * \param [in] unit - The unit number.
 * \param [in] in_port - incoming port
 * \param [in] out_port - outgoing port
 * \param [in] create_service - if creating IPv4/IPv6/MPLS service
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 *
 */
int dnx_sbfd_reflector_endpoint_create(
    int unit,
    int in_port,
    int out_port,
    int create_service)
{
    int in_vlan = 200;
    int out_vlan = 201;
    bcm_bfd_endpoint_info_t bfd_endpoint_info;
    int trap_code_sbfd;
    bcm_ip_t dip = 0x5050505;
    bcm_ip6_t cint_ip_route_sbfd_reflector_ipv6_route ={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xc0, 0x80, 0x01, 0x01};
    bcm_ip6_t dipv6 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13 };
    bcm_ip6_t ipv6_mask = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0 };
    int fec_id; /* vrf and fec_id are used for routing the reflected packet back to initiator */
    int vrf=2; /* Init vrf to be 2 in case srv6 case, otherwise set in below service */
    int use_bfd_my_dip_destination = ((dnx_sbfd_reflector_info_glb.flags2 & BCM_BFD_ENDPOINT_FLAGS2_USE_MY_DIP_DESTINATION) != 0);
    int my_bfd_dip_destination=0xabcd;

    if (create_service)
    {
        if (dnx_sbfd_reflector_info_glb.is_sbfd_mpls)
        {
            /** Use MPLS termination example */
            cint_termination_mpls_basic_info.intf_in = in_vlan;
            cint_termination_mpls_basic_info.intf_out = out_vlan;
            udf_nof_labels=1;

            BCM_IF_ERROR_RETURN_MSG(mpls_termination_basic_main(unit, in_port, out_port), "");

            fec_id = cint_termination_mpls_basic_info.fec_id;
            vrf = cint_termination_mpls_basic_info.vrf;
        }
        else if (dnx_sbfd_reflector_info_glb.is_sbfd_ipv6)
        {
            /** Use basic IPv6 example */
            cint_ip_route_info.intf_in = in_vlan;
            cint_ip_route_info.intf_out = out_vlan;
            BCM_IF_ERROR_RETURN_MSG(dnx_basic_example_ipv6(unit, in_port, out_port, 0), "");
            fec_id = ip_route_fec;
            vrf = 1;
        }
        else
        {
            /** Use basic IPv4 example */
            BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &fec_id), "");
            fec_id = fec_id+5;

            cint_ip_route_info.intf_in = in_vlan;
            cint_ip_route_info.intf_out = out_vlan;
            cint_ip_route_info.is_arp_rif_mode = 1;
            BCM_IF_ERROR_RETURN_MSG(dnx_basic_example_ipv4_inner(unit, in_port, out_port, fec_id), "");
            vrf = 1;
        }
    }

    bcm_bfd_endpoint_info_t_init(&bfd_endpoint_info);
    bfd_endpoint_info.type = dnx_sbfd_reflector_info_glb.is_sbfd_mpls ? bcmBFDTunnelTypeMpls : bcmBFDTunnelTypeUdp;
    bfd_endpoint_info.local_discr = dnx_sbfd_reflector_info_glb.local_discr;
    bfd_endpoint_info.flags2 |= BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_REFLECTOR;
    bfd_endpoint_info.local_state = 0;
    bfd_endpoint_info.local_min_rx = dnx_sbfd_reflector_info_glb.local_min_rx;
    bfd_endpoint_info.local_min_echo = 0x20;
    bfd_endpoint_info.bfd_period = 0;
    bfd_endpoint_info.src_ip_addr = dnx_sbfd_reflector_info_glb.src_ip;
    if(dnx_sbfd_reflector_info_glb.is_sbfd_ipv6)
    {
        bfd_endpoint_info.flags = BCM_BFD_ENDPOINT_IPV6;
        bfd_endpoint_info.src_ip_addr = 0;
        sal_memset(bfd_endpoint_info.src_ip6_addr, 0, 16);
        bfd_endpoint_info.flags2 |= BCM_BFD_ENDPOINT_FLAGS2_USE_MY_DIP_DESTINATION;
        /** Set My-BFD-DIP-Destination */
        BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_set(unit, bcmSwitchBfdMyDipDestination, my_bfd_dip_destination), "");

        /** Set up forwarding from the IP to the destination */
        bcm_l3_route_t l3rt;
        bcm_l3_route_t_init(&l3rt);
        sal_memcpy(l3rt.l3a_ip6_net, dipv6, 16);
        sal_memcpy(l3rt.l3a_ip6_mask, ipv6_mask, 16);
        l3rt.l3a_intf = my_bfd_dip_destination;
        l3rt.l3a_vrf = vrf;
        l3rt.l3a_flags = BCM_L3_IP6;
        l3rt.l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;
        BCM_IF_ERROR_RETURN_MSG(bcm_l3_route_add(unit, &l3rt), "");
    }

    BCM_IF_ERROR_RETURN_MSG(dnx_sbfd_reflector_configure(unit, &trap_code_sbfd), "");

    /** update classify entry*/
    BCM_GPORT_TRAP_SET(bfd_endpoint_info.remote_gport, trap_code_sbfd, 7, 0);
 
    BCM_IF_ERROR_RETURN_MSG(bcm_bfd_endpoint_create(unit, &bfd_endpoint_info), "");

    dnx_sbfd_reflector_glb_result.mep_id = bfd_endpoint_info.id;

    if (create_service)
    {
        int system_headers_mode = *(dnxc_data_get(unit,  "headers", "system_headers", "system_headers_mode", NULL));
        /** Add host entry to route the reflected packet back to initiator */
        if (system_headers_mode==0)
        {
            bcm_l3_route_t bfd_route;
            bfd_route.l3a_subnet = dip;
            bfd_route.l3a_ip_mask = 0xfffffff0;
            bfd_route.l3a_vrf = 0;
            int is_kbp_lookup = *(dnxc_data_get(unit, "elk", "application", "ipv4", NULL)) && *(dnxc_data_get(unit, "elk", "application", "split_rpf", NULL));;
            if(is_kbp_lookup)
            {
                bfd_route.l3a_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
            }
            bfd_route.l3a_intf = fec_id;
            BCM_IF_ERROR_RETURN_MSG(bcm_l3_route_add(unit, &bfd_route), "");
        }

        BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, dip, vrf, fec_id, 0, 0), "");

        if(dnx_sbfd_reflector_info_glb.is_sbfd_ipv6)
        {
            /* Add route entry for reflected packet */
            BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_ip_route_sbfd_reflector_ipv6_route, ipv6_mask, vrf, fec_id), "");
        }
    }

    return BCM_E_NONE;
}

/*
 * This is a function that would set the configuration for basic forwarding of an SBFDoUDPoIPV6oETH packet.
 *
 * \param [in] unit - The unit number.
 * \param [in] in_port - packet to send 
 * \param [in] out_port - packet expect to receive
 *
 */
int
dnx_sbfd_reflector_ipv6(
    int unit,
    int in_port,
    int out_port)
{
    int intf_in = 15;           /* Incoming packet ETH-RIF */
    int intf_out = 200;         /* Outgoing packet ETH-RIF */
    int fec = ip_route_fec;
    int vrf = 1;
    int vsi = 0;
    int encap_id = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x386;  /* ARP-Link-layer */
    bcm_gport_t gport = 0;
    int vlan = 200;
    bcm_mac_t intf_in_mac_address = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a};    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x11, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_ip6_t cint_ip_route_sbfd_reflector_ipv6_route ={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xc0, 0x80, 0x01, 0x01};
    int is_srv6 = sbfd_reflector_ipv6_with_srv6;
    l3_ingress_intf ingress_itf;
    char error_msg[200]={0,};
    l3_ingress_intf_init(&ingress_itf);

    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &fec), "");

    fec += 0x600;
    ip_route_fec = fec;

    if(is_srv6)
    {
        intf_in = 100;
        vrf = 2;
    }

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    snprintf(error_msg, sizeof(error_msg), "n_port = %d, intf_in = %d", in_port, intf_in);
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port, intf_in), error_msg);

    /*
     * 2. Set Out-Port default properties
     */
    snprintf(error_msg, sizeof(error_msg), "out_port = %d", out_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port), error_msg);

    /*
     * 3. Create ETH-RIF and set its properties
     */
    if(!is_srv6)
    {
        snprintf(error_msg, sizeof(error_msg), "intf_in = %d", intf_in);
        BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_in, intf_in_mac_address), error_msg);
    }
    snprintf(error_msg, sizeof(error_msg), "intf_out = %d", intf_out);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_out, intf_out_mac_address), error_msg);

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_itf.vrf = vrf;
    ingress_itf.intf_id = intf_in;

    snprintf(error_msg, sizeof(error_msg), "intf_in = %d, vrf = %d", intf_in, vrf);
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_itf), error_msg);

    /*
     * 5. Create FEC and set its properties
     */
    BCM_GPORT_LOCAL_SET(gport, out_port);
    dnx_utils_l3_fec_s fec_structure;
    dnx_utils_l3_fec_s_common_init(unit, BCM_L3_WITH_ID, 0, &fec_structure, fec, 0, 0, gport, intf_out, encap_id);
    BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_fec_create(unit, &fec_structure), "");

    fec = fec_structure.l3eg_fec_id;

    /*
     * 6. Create ARP and set its properties
     */
    snprintf(error_msg, sizeof(error_msg), "create egress object ARP only: encap_id = %d, vsi = %d", encap_id, vsi);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, 0, encap_id, arp_next_hop_mac, vsi), error_msg);

    /*
     * 7. Add Route entry
     */
    snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d", vrf, fec);
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_ip_route_sbfd_reflector_ipv6_route, cint_ip_route_basic_ipv6_mask, vrf, fec), error_msg);

    /*
     * 7. Add host entry
     */
    snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d", vrf, fec);
    BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(unit, cint_ip_route_basic_ipv6_host, vrf, fec), error_msg);

    return BCM_E_NONE;
}

/**
* Create SBFD reflector over ipv6 endpoint base on SRV6 
 * \param [in] unit - The unit number.
 * \param [in] in_port - in port.
 * \param [in] recycle_port1 - srv6_recycle_port
 * \param [in] recycle_port2 - sbfd_reflector_in_port
 * \param [in] recycle_port3 - sbfd_reflector_recycle_port
 * \param [in] out_port - out port
 * \param [in] srv6_type - srv6_type(1:psp, 2:usp, 3:usd)
 * \return
 *  int - Error Type, in sense of bcm_error_t
 *
 */
int dnx_sbfd_reflector_ipv6_with_srv6_run(int unit, int in_port, int recycle_port1, int recycle_port2, int recycle_port3, int out_port, int srv6_type)
{
    int sbfd_reflector_in_port = 0;
    int sbfd_reflector_out_port = 0;
    int sbfd_reflector_recycle_port = 0;
    int srv6_recycle_port = 0;
    int endpoint_id = 8200;
    uint8 is_termination = 0;
    int srh_next_protocol = 41;
    srv6_recycle_port = recycle_port1;
    sbfd_reflector_in_port = recycle_port2;
    sbfd_reflector_recycle_port= recycle_port3;
    sbfd_reflector_out_port = out_port;

    if ((srv6_type != 1) && (srv6_type != 2 ) && (srv6_type != 3 ))
    {
        printf("srv6_flag should be psp=1, usp=2, usd=3 \n");
        return BCM_E_PARAM;
    }
    /** init reflector parameter*/
    dnx_sbfd_reflector_info_glb.is_sbfd_ipv6=1;
    dnx_sbfd_reflector_info_glb.local_discr=0x80033034;

    sbfd_reflector_ipv6_with_srv6=1;
    sbfd_reflector_ipv6_with_srv6_inner_mac=1;
    dnx_sbfd_reflector_info_glb.reflector_recycle_port=sbfd_reflector_recycle_port;

    /** config srv6*/
    if (srv6_type != 1)
    {
        BCM_IF_ERROR_RETURN_MSG(basic_example_ipv6(unit, in_port, sbfd_reflector_in_port, 0,0,0), "");
    }
    if (srv6_type == 2)
    {
        sbfd_reflector_ipv6_with_srv6_usp=1;
    }
    else {
        is_termination = 1;
    }

    BCM_IF_ERROR_RETURN_MSG(srv6_endpoint_tunnel(unit,0,is_termination,0,0), "");

    /** srv6_flag 1: psp, 2: usp case, 3: usd case*/
    if (srv6_type == 1)
    {
       BCM_IF_ERROR_RETURN_MSG(srv6_egress_tunnel_test(unit,in_port,srv6_recycle_port, sbfd_reflector_in_port,0), "");
    }
    else if (srv6_type == 2)
    {
       BCM_IF_ERROR_RETURN_MSG(srv6_endpoint_psp_extended_configuration(unit,srv6_recycle_port,8,8,0), "");
    }
    else if (srv6_type == 3)
    {
        BCM_IF_ERROR_RETURN_MSG(dnx_basic_example_ipvx_for_srv6(unit,srh_next_protocol,srv6_recycle_port,sbfd_reflector_in_port), "");
    }

    /** Create ipv6 service*/
    BCM_IF_ERROR_RETURN_MSG(dnx_sbfd_reflector_ipv6(unit,sbfd_reflector_in_port,sbfd_reflector_out_port), "");

    /** Create BFD over IPv6 endpoint */
    BCM_IF_ERROR_RETURN_MSG(dnx_sbfd_reflector_endpoint_create(unit,sbfd_reflector_in_port,sbfd_reflector_out_port,0), "");

    return BCM_E_NONE;
}
