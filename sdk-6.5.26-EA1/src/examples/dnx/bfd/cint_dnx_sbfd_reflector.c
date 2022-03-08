/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~SBFD reflector test~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * $File: cint_dnx_sbfd_reflector.c
 *
 * Purpose: Examples for SBFDoIPV4, SBFDoMPLS reflector
 *
 * Usage:
 *
 *   Load SBFD Reflector soc properties
 * ./bcm.user
 * cd ../../../../regress/bcm
 * bcm shell "config add ucode_port_50.0=RCY.1:core_0.50"
 * bcm shell "config add tm_port_header_type_in_50.0=RCH_0"
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
    uint8 is_sbfd_mpls;
    uint8 is_sbfd_ipv6;
    int local_discr;
    int local_min_rx;
    bcm_ip_t src_ip;
    bcm_ip6_t src_ipv6;
    uint8 reflector_recyle_port;
};

/*
 * SBFD reflector init configuration
 */
dnx_sbfd_reflector_config_s dnx_sbfd_reflector_info_glb = {

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
     * sbfd_reflector_recyle_port
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
    int rv = create_recycle_entry_with_defaults(unit, recycle_entry_encap_id); 
    if (rv != BCM_E_NONE)
    {
        printf("Error, create_recycle_entry_with_defaults \n");
        return rv;
    }
    return rv;
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
    int rv = BCM_E_NONE;
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

    rv = bcm_field_group_add(unit, 0, &fg_info, &fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in iPMF1 \n", fg_id);

    /**Attach context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, &field_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, fg_id, field_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
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

    rv = bcm_field_entry_add(unit, 0,fg_id, &ent_info, &fg_entry_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
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

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(Ethernet) \n",
            attach_presel,field_context_id);

    return rv;
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
    int rv = BCM_E_NONE;
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
    rv = bcm_switch_reflector_create(unit, 0, sbfd_encap_id, &sbfd_reflector);

    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_reflector_create \n");
        return rv;
    }

    printf("sbfd_encap_id:0x%x\n", *sbfd_encap_id);

    return rv;
}

/**
 *  SBFD reflector configuration
 *   1) Load SBFD Reflector soc properties
        "ucode_port_50=RCY.1"
        "tm_port_header_type_in_50=RCH_0"
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
    bcm_error_t rv;
    int reflector_encap_id;
    int recycle_entry_encap_id;
    int trap_code_sbfd;
    int rcy_port = dnx_sbfd_reflector_info_glb.reflector_recyle_port;
    bcm_rx_trap_config_t trap_config_sbfd;

    /** 1. Create RCH encap ***/
    rv = dnx_sbfd_reflector_create_recycle_entry(unit, &recycle_entry_encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in dnx_sbfd_reflector_create_recycle_entry\n");
    }

    /** 2. Configure recycle port */
    rv = bcm_port_control_set(unit, rcy_port, bcmPortControlOverlayRecycle, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_control_set\n");
        return rv;
    }

    /** 3. Configure recycle port term_context_port_profile*/
    rv = bcm_port_control_set(unit, rcy_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppSbfdReflector);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_control_set\n");
        return rv;
    }

    /** 4. Set port class in order for PMF to crop RCH ad IRPP */
    rv = bcm_port_class_set(unit, rcy_port, bcmPortClassFieldIngressPMF3TrafficManagementPortCs, 4);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_class_set\n");
        return rv;
    }

    /** 5. Set ttl to 255 */
    rv = dnx_sbfd_reflector_ttl_set(unit,rcy_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in dnx_sbfd_reflector_ttl_set\n");
        return rv;
    }

    /** 6. Create SBFD reflector encap */
    rv = dnx_sbfd_reflector_create_reflector(unit, &reflector_encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in dnx_sbfd_reflector_create_reflector\n");
        return rv;
    }
    dnx_sbfd_reflector_glb_result.reflector_encap_id = reflector_encap_id;

    /** 7. Create TRAP to trap SBFD packet to recycle port */
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_code_sbfd);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_trap_type_create\n");
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config_sbfd);
    trap_config_sbfd.flags = BCM_RX_TRAP_UPDATE_ENCAP_ID | BCM_RX_TRAP_UPDATE_DEST;

    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(trap_config_sbfd.encap_id2, reflector_encap_id);
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(trap_config_sbfd.encap_id, recycle_entry_encap_id);
    trap_config_sbfd.dest_port = rcy_port;
    rv = bcm_rx_trap_set(unit, trap_code_sbfd, &trap_config_sbfd);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_trap_set\n");
        return rv;
    }

    *trap_code = trap_code_sbfd;
    dnx_sbfd_reflector_glb_result.reflector_trap_code = trap_code_sbfd;

    return rv;
}

/**
* Create SBFD reflector endpoint
 * create SBFD reflector endpoint by BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_REFLECTOR
 * \param [in] unit - The unit number.
 * \param [in] in_port - incoming port
 * \param [in] out_port - outgoing port
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 *
 */
int dnx_sbfd_reflector_endpoint_create(
    int unit,
    int in_port,
    int out_port)
{
    bcm_error_t rv;
    int in_vlan = 200;
    int out_vlan = 201;
    bcm_bfd_endpoint_info_t bfd_endpoint_info;
    int trap_code_sbfd;
    bcm_ip_t dip = 0x5050505;
    int fec_id; /* vrf and fec_id are used for routing the reflected packet back to initiator */
    int vrf;

    if (dnx_sbfd_reflector_info_glb.is_sbfd_mpls)
    {
        /** Use MPLS termination example */
        cint_termination_mpls_basic_info.intf_in = in_vlan;
        cint_termination_mpls_basic_info.intf_out = out_vlan;
        udf_nof_labels=1;

        rv = mpls_termination_basic_main(unit, in_port, out_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in mpls_termination_basic_main\n");
            return rv;
        }

        fec_id = cint_termination_mpls_basic_info.fec_id;
        vrf = cint_termination_mpls_basic_info.vrf;
    }
    else
    {
        /** Use basic IPv4 example */
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
        fec_id = fec_id+5;

        cint_ip_route_info.intf_in = in_vlan;
        cint_ip_route_info.intf_out = out_vlan;
        cint_ip_route_info.is_arp_rif_mode = 1;
        rv = dnx_basic_example_ipv4_inner(unit, in_port, out_port, fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
        vrf = 1;
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

    rv = dnx_sbfd_reflector_configure(unit, &trap_code_sbfd);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in dnx_sbfd_reflector_configure\n");
        return rv;
    }

    /** update classify entry*/
    BCM_GPORT_TRAP_SET(bfd_endpoint_info.remote_gport, trap_code_sbfd, 7, 0);
 
    rv = bcm_bfd_endpoint_create(unit, &bfd_endpoint_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_bfd_endpoint_create\n");
        return rv;
    }

    /** Add host entry to route the reflected packet back to initiator */
    rv = add_host_ipv4(unit, dip, vrf, fec_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in add_host_ipv4\n");
        return rv;
    }

    return rv;
}
