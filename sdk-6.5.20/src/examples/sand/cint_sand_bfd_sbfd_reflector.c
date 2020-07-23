/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~BFD test~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * $Id: cint_dnx_oam_mpls_transit_signal.c
l.c This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
l.c 
l.c Copyright 2007-2020 Broadcom Inc. All rights reserved.File: cint_sand_bfd_sbfd_reflector.c
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
 */
int sbfd_reflector_create_recycle_entry(
    int unit,
    int *recycle_entry_encap_id)
{
    int rv = BCM_E_NONE;
    bcm_l2_egress_t recycle_entry;

    bcm_l2_egress_t_init(&recycle_entry);
    recycle_entry.flags = BCM_L2_EGRESS_RECYCLE_HEADER;
    recycle_entry.recycle_app = bcmL2EgressRecycleAppDropAndContinue;
    rv = bcm_l2_egress_create(unit, &recycle_entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_egress_create \n");
        return rv;
    }

    *recycle_entry_encap_id = recycle_entry.encap_id;

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

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id);
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
 * \param [in] is_ipv6 - The flag indicates it is the ipv6.
 * \param [in] next_outlif - The RCH encapuslation outlif.
 * \param [out] recycle_entry_encap_id - The TWAMP encapuslation id.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int sbfd_reflector_create_reflector(
    int unit,
    int is_mpls,
    int next_outlif,
    int *sbfd_encap_id)
{
    int rv = BCM_E_NONE;
    bcm_switch_reflector_data_t sbfd_reflector;

    sbfd_reflector.type = is_mpls ? bcmSwitchReflectorSbfdMpls : bcmSwitchReflectorSbfdIp4;
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
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int sbfd_reflector_configure_jr2(int unit, int *trap_code)
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

    /** 4,  set ttl to 255 */
    rv = dnx_sbfd_reflector_ttl_set(unit,rcy_port);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_control_set $rv\n");
        return rv;
    }
    /** 5, create SBFD reflector encap ***/
    rv = sbfd_reflector_create_reflector(unit,is_s_bfd_mpls, recycle_entry_encap_id, &reflector_encap_id);
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
 * \param [in] local_min_tx - Local min tx
 * \param [in] local_state -  Local state
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int sbfd_reflector_endpoint_create(int unit, bcm_ip_t src_ip, uint32 local_min_tx, bcm_bfd_state_t local_state)
{
    bcm_error_t rv;
    bcm_bfd_endpoint_info_t bfd_endpoint_info;
    int dummy_lif;
    int trap_code_sbfd;

    bcm_bfd_endpoint_info_t_init(&bfd_endpoint_info);
    bfd_endpoint_info.type = is_s_bfd_mpls ? bcmBFDTunnelTypeMpls : bcmBFDTunnelTypeUdp ;
    bfd_endpoint_info.local_discr = bfd_local_discr;
    bfd_endpoint_info.src_ip_addr = src_ip;
    bfd_endpoint_info.flags2 |= BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_REFLECTOR;
    bfd_endpoint_info.local_state = is_device_or_above(unit, JERICHO2) ? 0 : local_state;
    bfd_endpoint_info.local_min_rx = 0x10;
    bfd_endpoint_info.local_min_echo =  is_device_or_above(unit, JERICHO2) ? 0x20 : 0;
    bfd_endpoint_info.bfd_period = 0;

    if(is_device_or_above(unit, JERICHO2))
    {
        rv = sbfd_reflector_configure_jr2( unit, &trap_code_sbfd);
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


