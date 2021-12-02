/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~BFD test~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * $Id: cint_dnx_oam_mpls_transit_signal.c
l.c This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
l.c 
l.c Copyright 2007-2021 Broadcom Inc. All rights reserved.File: cint_sand_bfd_sbfd_reflector.c
 *
 * Purpose: Examples for SBFDoIPV4, SBFDoMPLS reflector
 *
 * Usage for SBFDoIPV4:
 *
 * cd ../../../../src/examples/dpp
 * cint ../sand/utility/cint_sand_utils_global.c
 * cint internal/cint_bfd_performance_test.c
 * cint utility/cint_utils_field.c
 * cint utility/cint_utils_vlan.c
 * cint utility/cint_utils_l3.c
 * cint ../sand/utility/cint_sand_utils_mpls.c
 * cint utility/cint_utils_oam.c
 * cint ../sand/utility/cint_sand_utils_oam.c
 * cint ../sand/cint_sand_bfd.c
 * cint ../sand/cint_ip_route.c
 * cint cint_bfd.c
 * cint cint_field_bfd_ipv4_single_hop.c
 * cint cint_field_bfd_echo.c
 * cint cint_field_oam_bfd_advanced.c
 * cint internal/cint_mirror.c
 * cint
 * is_s_bfd_reflector=1;
 * sbfd_reflector_recyle_port=50;
 * uint32 sflags=BCM_BFD_ENDPOINT_MULTIHOP;
 * bfd_ipv4_run_with_defaults(0,200,201,-1,0,sflags,0,0);

 * Usage for SBFDoMPLS:
 *
 * cd ../../../../src/examples/dpp
 * cint ../sand/utility/cint_sand_utils_global.c
 * cint internal/cint_bfd_performance_test.c
 * cint utility/cint_utils_field.c
 * cint utility/cint_utils_vlan.c
 * cint utility/cint_utils_l3.c
 * cint ../sand/utility/cint_sand_utils_mpls.c
 * cint utility/cint_utils_oam.c

 * cint ../sand/utility/cint_sand_utils_oam.c
 * cint ../sand/cint_sand_bfd.c
 * cint ../sand/cint_ip_route.c
 * cint cint_bfd.c
 * cint cint_field_bfd_ipv4_single_hop.c
 * cint cint_field_bfd_echo.c
 * cint cint_field_oam_bfd_advanced.c
 * cint internal/cint_mirror.c
 * cint

 * is_s_bfd_reflector=1;
 * is_s_bfd_mpls=1;
 * sbfd_reflector_recyle_port=50;
 * bfd_mpls_run_with_defaults(0,200,201,0);
 *
*/

/* SBFD reflector feature.*/
uint8 is_s_bfd_mpls = 0;
uint8 sbfd_reflector_recyle_port = 0;
int sbfd_reflector_trap_code = 0;
int g_reflector_encap_id = 0;



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
int sbfd_reflector_create_recycle_entry(
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
int dnx_sbfd_reflector_ttl_set(int unit,bcm_port_t recycle_port)
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

    return 0;
}


/**
 * \brief
 *
 *  This function creates the sbfd reflector encapsulation.
 *
 * \param [in] unit - The unit number.
 * \param [in] is_mpls - The flag indicates SBFDoMPLS.
 * \param [in] next_outlif - The RCH encapuslation outlif.
 * \param [out] recycle_entry_encap_id - The TWAMP encapuslation id.
 * \param [in] is_ipv6 - The flag indicates it is the ipv6.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 *
 */
int sbfd_reflector_create_reflector(
    int unit,
    int is_mpls,
    int next_outlif,
    int *sbfd_encap_id,
    int is_ipv6)
{
    int rv = BCM_E_NONE;
    bcm_switch_reflector_data_t sbfd_reflector;
    /** set etps tos profile*/
    sbfd_reflector.type = is_mpls ? bcmSwitchReflectorSbfdMpls : bcmSwitchReflectorSbfdIp4;
    if(is_ipv6)
    {
        sbfd_reflector.type = bcmSwitchReflectorSbfdIp6;
    }
    rv = bcm_switch_reflector_create(unit, 0, sbfd_encap_id, &sbfd_reflector);

    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_reflector_create \n");
        return rv;
    }

    printf("arp_encap_id:0x%x, sbfd_encap_id:0x%x\n", next_outlif, *sbfd_encap_id);

    return rv;
}
/**
 *  SBFD reflector configuration on JR2
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
 * \param [in] is_ipv6 - indicate ipv6
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 *
 */
int sbfd_reflector_configure_jr2(int unit, int *trap_code, int is_ipv6)
{
    bcm_error_t rv;
    int reflector_encap_id;
    int recycle_entry_encap_id;
    int trap_code_sbfd;
    int rcy_port = sbfd_reflector_recyle_port;
    bcm_rx_trap_config_t trap_config_sbfd;

    /** 1, create RCH encap ***/
    rv = sbfd_reflector_create_recycle_entry(unit, &recycle_entry_encap_id);
    if (rv != BCM_E_NONE) {
        printf("twamp_create_recycle_entry $rv");
    }
    /** 2, configure recycle port */
    rv = bcm_port_control_set(unit, rcy_port, bcmPortControlOverlayRecycle, 1);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_control_set $rv\n");
        return rv;
    }

    /** 3, configure recycle port term_context_port_profile*/
    rv = bcm_port_control_set(unit,rcy_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppSbfdReflector);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_control_set $rv\n");
        return rv;
    }
    /** Set port class in order for PMF to crop RCH ad IRPP */
    rv = bcm_port_class_set(unit,rcy_port,bcmPortClassFieldIngressPMF3TrafficManagementPortCs,4);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_class_set \n");
        return rv;
    }

    /** 4,  set ttl to 255 */
    rv = dnx_sbfd_reflector_ttl_set(unit,rcy_port);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_control_set $rv\n");
        return rv;
    }
    /** 5, create SBFD reflector encap ***/
    rv = sbfd_reflector_create_reflector(unit,is_s_bfd_mpls, recycle_entry_encap_id, &reflector_encap_id, is_ipv6);
    if (rv != BCM_E_NONE) {
        printf("sbfd_reflector_create_reflector $rv\n");
        return rv;
    }
    g_reflector_encap_id = reflector_encap_id;
    /** 6, Create TRAP to trap SBFD packet to recycle port */
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_code_sbfd);
    if (rv != BCM_E_NONE) {
        printf("bcm_rx_trap_type_create $rv\n");
        return rv;
    }
    bcm_rx_trap_config_t_init(&trap_config_sbfd);
    trap_config_sbfd.flags = BCM_RX_TRAP_UPDATE_ENCAP_ID | BCM_RX_TRAP_UPDATE_DEST;

    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(trap_config_sbfd.encap_id2, reflector_encap_id);
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(trap_config_sbfd.encap_id, recycle_entry_encap_id);
    trap_config_sbfd.dest_port = rcy_port;
    rv = bcm_rx_trap_set(unit, trap_code_sbfd, &trap_config_sbfd);
    if (rv != BCM_E_NONE) {
        printf("bcm_rx_trap_set $rv\n");
        return rv;
    }
    *trap_code = trap_code_sbfd;
    sbfd_reflector_trap_code = trap_code_sbfd;
    return rv;
}

/**
* Create SBFD reflector endpoint
 * create SBFD reflector endpoint by BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_REFLECTOR
 * \param [in] unit - The unit number.
 * \param [in] src_ip - The src ip of Sbfd.
 * \param [in] local_min_rx - Local min rx
 * \param [in] local_state -  Local state
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 *
 */
int sbfd_reflector_endpoint_create(int unit, bcm_ip_t src_ip, uint32 local_min_rx, bcm_bfd_state_t local_state, int is_ipv6)
{
    bcm_error_t rv;
    bcm_bfd_endpoint_info_t bfd_endpoint_info;
    int dummy_lif;
    int trap_code_sbfd;
    int bfd_my_dip_destination_read;
    bcm_l3_route_t l3rt;
    bcm_ip6_t mask_full = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

    bcm_bfd_endpoint_info_t_init(&bfd_endpoint_info);
    bfd_endpoint_info.type = is_s_bfd_mpls ? bcmBFDTunnelTypeMpls : bcmBFDTunnelTypeUdp ;
    bfd_endpoint_info.local_discr = is_ipv6 ? local_disc : bfd_local_discr;
    bfd_endpoint_info.flags2 |= BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_REFLECTOR;
    bfd_endpoint_info.local_state = is_device_or_above(unit, JERICHO2) ? 0 : local_state;
    bfd_endpoint_info.local_min_rx = local_min_rx;
    bfd_endpoint_info.local_min_echo =  is_device_or_above(unit, JERICHO2) ? 0x20 : 0;
    bfd_endpoint_info.bfd_period = 0;
    if(is_device_or_above(unit, JERICHO2) && is_ipv6)
    {
        bfd_endpoint_info.flags = BCM_BFD_ENDPOINT_IPV6;
        sal_memset(bfd_endpoint_info.src_ip6_addr, 0, 16);
        bfd_endpoint_info.flags2 |= BCM_BFD_ENDPOINT_FLAGS2_USE_MY_DIP_DESTINATION;
        /** Set My-BFD-DIP-Destination */
        rv = bcm_switch_control_set(unit, bcmSwitchBfdMyDipDestination, my_bfd_dip_destination);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
        
        rv = bcm_switch_control_get(unit, bcmSwitchBfdMyDipDestination, &bfd_my_dip_destination_read);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
        
        if(bfd_my_dip_destination_read != my_bfd_dip_destination)
        {
            printf("Setting BFD-My-DIP-destination failed, value read: 0x%08x, value written 0x%08x\n",
                    my_bfd_dip_destination, bfd_my_dip_destination_read);
            return BCM_E_INTERNAL;
        }
        
        /** Set up forwarding from the IP to the destination */
        bcm_l3_route_t_init(&l3rt);
        l3rt.l3a_subnet = bfd_endpoint_info.src_ip_addr;
        l3rt.l3a_ip_mask = 0xFFFFFFFF;
        sal_memcpy(l3rt.l3a_ip6_net, &src_ipv6, 16);
        sal_memcpy(l3rt.l3a_ip6_mask, mask_full, 16);
        l3rt.l3a_intf = my_bfd_dip_destination;
        l3rt.l3a_vrf = sbfd_reflector_ipv6_with_srv6 ? 2 : 1;
        l3rt.l3a_flags = BCM_L3_IP6;
        l3rt.l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;
        rv = bcm_l3_route_add(unit, &l3rt);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }        
    }
    else {
        bfd_endpoint_info.src_ip_addr = src_ip;
    }  
    if(is_device_or_above(unit, JERICHO2))
    {
        rv = sbfd_reflector_configure_jr2( unit, &trap_code_sbfd, is_ipv6);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
        /** update classify entry*/
        BCM_GPORT_TRAP_SET(bfd_endpoint_info.remote_gport, trap_code_sbfd, 7, 0);
    }
    rv = bcm_bfd_endpoint_create(unit, &bfd_endpoint_info);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    bfd_ep1.id = bfd_endpoint_info.id;

    return rv;
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
basic_sbfd_reflector_ipv6(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
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
    char *proc_name;
    char *err_proc_name;
    proc_name = "basic_sbfd_reflector_ipv6";
    l3_ingress_intf ingress_itf;
    l3_ingress_intf_init(&ingress_itf);


    if (is_device_or_above(unit, JERICHO2))
    {
        bcm_switch_fec_property_config_t fec_config;
        fec_config.flags = BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY;
        rv = bcm_switch_fec_property_get(unit,&fec_config);
        fec = fec_config.start + 0x600;
    }

    ip_route_fec = fec;

    if(is_srv6)
    {
        intf_in = 100;
        vrf = 2;
    }
    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, in_port, intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set in_port = %d, intf_in = %d, err_id = %d\n", proc_name, in_port, intf_in,
               rv);
        return rv;
    }

    /*
     * 2. Set Out-Port default properties
     */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_intf_set out_port = %d, err_id = %d\n", proc_name, out_port, rv);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    if(!is_srv6)
    {
        rv = intf_eth_rif_create(unit, intf_in, intf_in_mac_address);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, intf_eth_rif_create intf_in = %d, err_id = %d\n", proc_name, intf_in, rv);
            return rv;
        }
    }
    rv = intf_eth_rif_create(unit, intf_out, intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out = %d, err_id = %d\n", proc_name, intf_out, rv);
        return rv;
    }

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_itf.vrf = vrf;
    ingress_itf.intf_id = intf_in;

    rv = intf_ingress_rif_set(unit, &ingress_itf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in = %d, vrf = %d, err_id = %d\n", proc_name, intf_in, vrf, rv);
        return rv;
    }

    /*
     * 5. Create FEC and set its properties
     */
    BCM_GPORT_LOCAL_SET(gport, out_port);
    sand_utils_l3_fec_s fec_structure;
    sand_utils_l3_fec_s_common_init(unit, BCM_L3_WITH_ID, 0, &fec_structure, fec, 0, 0, gport, intf_out, encap_id);
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_fec_create\n");
        return rv;
    }

    fec = fec_structure.l3eg_fec_id;

    /*
     * 6. Create ARP and set its properties
     */
    rv = l3__egress_only_encap__create(unit, 0, encap_id, arp_next_hop_mac, vsi);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only: encap_id = %d, vsi = %d, err_id = %d\n", proc_name,
               encap_id, vsi, rv);
        return rv;
    }

    /*
     * 7. Add Route entry
     */
    rv = add_route_ipv6(unit, cint_ip_route_sbfd_reflector_ipv6_route, cint_ip_route_basic_ipv6_mask, vrf, fec);
    err_proc_name = "add_route_ipv6";

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function %s: vrf = %d, fec = %d, err_id = %d \n", proc_name, err_proc_name, vrf, fec, rv);
        return rv;
    }

    /*
     * 7. Add host entry
     */
    rv = add_host_ipv6(unit, cint_ip_route_basic_ipv6_host, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv6: vrf = %d, fec = %d, err_id = %d\n", proc_name, vrf, fec, rv);
        return rv;
    }

    return rv;
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
int sbfd_reflector_ipv6_with_srv6_run(int unit, int in_port, int recycle_port1, int recycle_port2, int recycle_port3, int out_port, int srv6_type)
{
    int sbfd_reflector_in_port = 0;
    int sbfd_reflector_out_port = 0;
    int sbfd_reflector_recycle_port = 0;
    int srv6_recycle_port = 0;
    int endpoint_id = 8200;
    uint8 is_termination = 0;
    int srh_next_protocol = 41;
    bcm_error_t rv;
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
    is_s_bfd_reflector=1;
    is_s_bfd_ipv6_reflector_ep=1;
    sbfd_reflector_ipv6_with_srv6=1;
    sbfd_reflector_recyle_port=sbfd_reflector_recycle_port;
    sbfd_reflector_ipv6_with_srv6_inner_mac=1;
    bfd_local_ipv6_disc=0x80033034;

    /** config srv6*/
    if (srv6_type != 1)
    {
        rv = basic_example_ipv6(unit,in_port,sbfd_reflector_in_port, 0,0,0);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }
    if (srv6_type == 2)
    {
        sbfd_reflector_ipv6_with_srv6_usp=1;
    }
    else {
        is_termination = 1;
    }

    rv = srv6_endpoint_tunnel(unit,0,is_termination,0,0);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    /** srv6_flag 1: psp, 2: usp case, 3: usd case*/
    if (srv6_type == 1)
    {
       rv = srv6_egress_tunnel_test(unit,in_port,srv6_recycle_port, sbfd_reflector_in_port,0);
    }
    else if (srv6_type == 2)
    {
       rv = srv6_endpoint_psp_extended_configuration(unit,srv6_recycle_port,8,8,0);
    }
    else if (srv6_type == 3)
    {
        rv = dnx_basic_example_ipvx_for_srv6(unit,srh_next_protocol,srv6_recycle_port,sbfd_reflector_in_port);
    }

    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /** config SBFD reflector over ipv6*/
    rv = bfd_ipv6_service_init(unit,sbfd_reflector_in_port,0,sbfd_reflector_out_port,0,0,-1); 
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = bfd_ipv6_run_with_defaults(unit,endpoint_id,1,1,bcmBFDAuthTypeNone,0,1,0,0,0);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    return rv;
}



