/* $Id: cint_sflow_extended_gateway.c
*/

/**
 *
 *  This cint calls all the required APIs to configure an IPoETH
 *  sFlow agent.
 *
 * How to run: 
 * Set SOC properties: 
 *  Set sflow_recycle_port as INJECTED and RCY (one port per core)
 *  Set enable small EXEM lookup by IPMF2
 *  set soc_properties_list [list "tm_port_header_type_in_XXX=INJECTED" "tm_port_header_type_out_XXX=ETH" "ucode_port_XXX=RCY.0:core_0.XXX" "pmf_sexem3_stage=IPMF2"]
 * Load following cints:
 * Test Scenario - start
   cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
   cint ../../../../src/examples/dnx/field/cint_field_utils.c
   cint ../../../../src/examples/dnx/crps/cint_crps_oam_config.c
   cint ../../../../src/examples/dnx/oam/cint_oam_basic.c
   cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
   cint ../../../../src/examples/dnx/sflow/cint_sflow_field_extended_gateway.c
   cint ../../../../src/examples/dnx/sflow/cint_sflow_field_utils.c
   cint ../../../../src/examples/dnx/sflow/cint_sflow_utils.c
   cint ../../../../src/examples/dnx/sflow/cint_sflow_basic.c
   cint ../../../../src/examples/dnx/sflow/cint_sflow_extended_gateway.c
 *  
   cint
 * 
   cint_sflow_extended_gateway_general_init(unit);
   cint_sflow_extended_gateway_route_setup($unit, inPort, routeOutPort); 
   cint_sflow_extended_gateway_first_pass_create(unit, inPort, nof_cores, sflow_recycle_port_core_0, sflow_recycle_port_core_1);
   cint_sflow_extended_gateway_second_pass_create(unit, inPort, nof_cores, sflow_recycle_port_core_0, sflow_recycle_port_core_1, eventor_id, eventorPort, sflowOutPort);
   cint_sflow_extended_gateway_forwarding_setup(unit, eventorPort, sflowOutPort);
   cint_sflow_utils_eventor_activate(unit);
 *  
 *  For setting input/output interface value:
   cint_sflow_utils_egress_interface_create(unit, port, interface, is_input);
 *
 *  For cleanup:
 *  
   cint_sflow_extended_gateway_destroy(unit);
 * Test Scenario - end
 *
 */

struct cint_sflow_extended_gateway_info_s {
   bcm_gport_t sflow_mirror_destination_core_0;
   bcm_gport_t sflow_mirror_destination_core_1;
   bcm_field_context_t first_pass_context_id;
   int sflow_first_pass_encap_id;
   bcm_field_context_t second_pass_context_id;
   bcm_field_context_t second_pass_ipmf3_context_id;
   int sflow_second_pass_encap_id;

   int sflow_encap_extended_dst_id;
   int sflow_encap_extended_src_id;

   /* receycle port profile - used for context selection*/
   uint32 sflow_recycle_port_core_0_profile_old_value;
   uint32 sflow_recycle_port_core_1_profile_old_value;
   uint32 sflow_recycle_port_profile;

   /* Switch/Router AS number */
   int myRouterASNumber;

   int dst_as_path_1;
   int dst_as_path_2;
   int dst_as_path_3;
   int dst_as_path_4;
   bcm_ip_t next_hop_ipv4_address;
   bcm_ip6_t next_hop_ipv6_address;
   int src_as;
   int src_as_peer;
};

cint_sflow_extended_gateway_info_s cint_sflow_extended_gateway_info = {
   /* sflow_mirror_destination_core_0 */
   -1,
   /* sflow_mirror_destination_core_1 */
   -1,
   /* first_pass_context_id */
   -1,
   /* sflow_first_pass_encap_id */
   -1,
   /* second_pass_context_id */
   -1,
   /* second_pass_ipmf3_context_id */
   -1,
   /* sflow_second_pass_encap_id */
   -1,

   /* sflow_encap_extended_dst_id */
   -1,
   /* sflow_encap_extended_src_id */
   -1,

   /* sflow_recycle_port_core_0_profile_old_value */
   -1,
   /* sflow_recycle_port_core_1_profile_old_value */
   -1,
   /* sflow_recycle_port_profile */
   5,

   /* myRouterASNumber */
   0xA5A5CC33,

   /* dst_as_path_1 */
   0x22221111,
   /* dst_as_path_2 */
   0x44443333,
   /* dst_as_path_3 */
   0x66665555,
   /* dst_as_path_4 */
   0x88887777,
   /*next_hop_ipv4_address */
   0xA8800001, /* 168.128.0.1*/
   /* next_hop_ipv6_address*/
   {0x00, 0x01, 0x02, 0x03, 0x14, 0x15, 0x16, 0x17, 0x28, 0x29, 0x2A, 0x2B, 0x3C, 0x3D, 0x3E, 0x3F},
   /* src_as */
   0xAAAA9999,
   /* src_as_peer */
   0xCCCCBBBB
};


/*
 * This function configures sFlow IPoETH first pass. 
 * Thats includes: 
 * 1. PMF setting - snoop "any IPoETH" packet
 * 2. Creates SFOW EEDB of first pass.
 * 3. Create mirroring of the snooped packet and attached it to the SFLOW EEDB.
 * destination.
 */
int
cint_sflow_extended_gateway_first_pass_create(int unit, int in_port, int nof_cores, int sflow_recycle_port_core_0, int sflow_recycle_port_core_1)
{
    char error_msg[200]={0,};
    bcm_gport_t sflow_mirror_destination_core_0, sflow_mirror_destination_core_1;
    char *proc_name;

    proc_name = "cint_sflow_extended_gateway_first_pass_create";

    /* Create context for "1st pass"*/
    BCM_IF_ERROR_RETURN_MSG(cint_sflow_field_extended_gateway_group_context_create(unit, &cint_sflow_extended_gateway_info.first_pass_context_id, "Sflow 1st context", 1 /* is_first_pass */),
        "(Sflow 1st pass)");

    printf("%s: first_pass_context_id = %d\n", proc_name, cint_sflow_extended_gateway_info.first_pass_context_id);

    /* Set context preselector for "1st pass"*/
    BCM_IF_ERROR_RETURN_MSG(cint_sflow_field_extended_gateway_group_context_presel_set(unit, cint_sflow_extended_gateway_info.first_pass_context_id, 1 /*is_first_pass*/),
        "(Sflow 1st pass)");

    /* Map system to local port for in_port */
    if (!BCM_GPORT_IS_TRUNK(in_port))
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, in_port, bcmPortControlSystemPortInjectedMap, 1),
            "bcmPortControlSystemPortInjectedMap");
    }
    /* Set recycle port property: skip the SFLOW Sample Record:
     *   - SFLOW Header Record size = 6 words
     *   - Original Dst-port and Src-port are added = 2 words
     *   Thus skip 8 words for processing the original packet.
     */
    snprintf(error_msg, sizeof(error_msg), "(port = %d)", sflow_recycle_port_core_0);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, sflow_recycle_port_core_0, bcmPortControlFirstHeaderSizeAfterInjected, 8*4), error_msg);

    if (nof_cores == 2) {
        snprintf(error_msg, sizeof(error_msg), "(port = %d)", sflow_recycle_port_core_1);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, sflow_recycle_port_core_1, bcmPortControlFirstHeaderSizeAfterInjected, 8*4), error_msg);
    }

    /* Create "1st pass" SFLOW EEDB */
    bcm_instru_sflow_encap_info_t sflow_encap_info;
    sal_memset(&sflow_encap_info, 0, sizeof(sflow_encap_info));
    sflow_encap_info.flags = BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR;
    BCM_IF_ERROR_RETURN_MSG(bcm_instru_sflow_encap_create(unit, &sflow_encap_info), "");

    cint_sflow_extended_gateway_info.sflow_first_pass_encap_id = sflow_encap_info.sflow_encap_id;
    printf("%s: sflow_encap_id = %d 0x%x\n", proc_name, cint_sflow_extended_gateway_info.sflow_first_pass_encap_id, cint_sflow_extended_gateway_info.sflow_first_pass_encap_id);

    cint_sflow_basic_info.sflow_encap_id = cint_sflow_extended_gateway_info.sflow_first_pass_encap_id;

    /*
     * Create a mirror destination of type stat sampling, including ingress crop, and stat rate,
     * and modified ftmh header with sFlow extension.
     */
    snprintf(error_msg, sizeof(error_msg), "(out_port = %d)", sflow_recycle_port_core_0);
    BCM_IF_ERROR_RETURN_MSG(cint_sflow_utils_mirror_create(unit, sflow_recycle_port_core_0, cint_sflow_extended_gateway_info.sflow_first_pass_encap_id,
            cint_sflow_basic_info.sample_rate_dividend, cint_sflow_basic_info.sample_rate_divisor,
            &sflow_mirror_destination_core_0, 0), error_msg);

    cint_sflow_extended_gateway_info.sflow_mirror_destination_core_0 = sflow_mirror_destination_core_0;
    printf("%s: sflow_mirror_destination_core_0 = %d 0x%x\n", proc_name, sflow_mirror_destination_core_0, sflow_mirror_destination_core_0);

    if (nof_cores == 2)
    {
        snprintf(error_msg, sizeof(error_msg), "(out_port = %d)", sflow_recycle_port_core_1);
        BCM_IF_ERROR_RETURN_MSG(cint_sflow_utils_mirror_create(unit, sflow_recycle_port_core_1, cint_sflow_extended_gateway_info.sflow_first_pass_encap_id,
               cint_sflow_basic_info.sample_rate_dividend, cint_sflow_basic_info.sample_rate_divisor,
               &sflow_mirror_destination_core_1, 0), error_msg);

        cint_sflow_extended_gateway_info.sflow_mirror_destination_core_1 = sflow_mirror_destination_core_1;
        printf("%s: sflow_mirror_destination_core_1 = %d 0x%x\n", proc_name, sflow_mirror_destination_core_1, sflow_mirror_destination_core_1);
    }

    /* Create snoop, attach it to context and sets it's action */
    BCM_IF_ERROR_RETURN_MSG(cint_sflow_field_extended_gateway_first_pass_group_create_and_attach(unit, nof_cores, sflow_mirror_destination_core_0, sflow_mirror_destination_core_1, -1, -1, cint_sflow_extended_gateway_info.first_pass_context_id),
        "(Sflow 1st pass)");

    BCM_IF_ERROR_RETURN_MSG(cint_sflow_field_egress_estimate_encap_size(unit), "(Sflow 1st pass)");

    return BCM_E_NONE;
}

/*
 * This function configures sFlow IPoETH second pass. 
 * Thats includes: 
 * 1. PMF setting - snoop "any IPoETH" packet
 * 2. Creates SFOW EEDB of second pass.
 * 3. Create mirroring of the snooped packet and attached it to the SFLOW EEDB.
 * destination.
 */
int
cint_sflow_extended_gateway_second_pass_create(int unit, int in_port, int nof_cores, int sflow_recycle_port_core_0, int sflow_recycle_port_core_1, int eventor_id, int eventor_port, int sflow_out_port, int nof_samples)
{
    char error_msg[200]={0,};
    bcm_port_class_t port_class;
    bcm_gport_t gport_sflow_recycle_port;
    char *proc_name;

    proc_name = "cint_sflow_extended_gateway_second_pass_create";

    /* Make a sflow recycle port Gport */
    BCM_GPORT_LOCAL_SET(gport_sflow_recycle_port, sflow_recycle_port_core_0);
    
    port_class = bcmPortClassFieldIngressPMF1TrafficManagementPortCs;

    /* Get the old port_profile value to be resumed after delete.*/
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_get(unit, gport_sflow_recycle_port, port_class, &cint_sflow_extended_gateway_info.sflow_recycle_port_core_0_profile_old_value), "");

    /* Set port profile - to be used for PMF context selection*/
    snprintf(error_msg, sizeof(error_msg), "(gport = 0x%x, port = %d)", gport_sflow_recycle_port, sflow_recycle_port_core_0);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, gport_sflow_recycle_port, port_class, cint_sflow_extended_gateway_info.sflow_recycle_port_profile), error_msg);

    printf("%s: sflow_recycle_port_core_0 = %d, new-profile = %d, old-profile = %d \n", proc_name, sflow_recycle_port_core_0, cint_sflow_extended_gateway_info.sflow_recycle_port_profile, cint_sflow_extended_gateway_info.sflow_recycle_port_core_0_profile_old_value);

    if (nof_cores == 2) {

        /* Make a sflow recycle port Gport */
        BCM_GPORT_LOCAL_SET(gport_sflow_recycle_port, sflow_recycle_port_core_1);
    
        /* Get the old port_profile value to be resumed after delete.*/
        BCM_IF_ERROR_RETURN_MSG(bcm_port_class_get(unit, gport_sflow_recycle_port, port_class, &cint_sflow_extended_gateway_info.sflow_recycle_port_core_1_profile_old_value), "");

        /* Set port profile - to be used for PMF context selection*/
        snprintf(error_msg, sizeof(error_msg), "(gport = 0x%x, port = %d)", gport_sflow_recycle_port, sflow_recycle_port_core_1);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, gport_sflow_recycle_port, port_class, cint_sflow_extended_gateway_info.sflow_recycle_port_profile), error_msg);

        printf("%s: sflow_recycle_port_core_1 = %d, new-profile = %d, old-profile = %d \n", proc_name, sflow_recycle_port_core_1, cint_sflow_extended_gateway_info.sflow_recycle_port_profile, cint_sflow_extended_gateway_info.sflow_recycle_port_core_1_profile_old_value);

    }

    /* Create context for "2nd pass"*/
    BCM_IF_ERROR_RETURN_MSG(cint_sflow_field_extended_gateway_group_context_create(unit, &cint_sflow_extended_gateway_info.second_pass_context_id, "Sflow 2nd context",  0 /* is_first_pass */),
        "(Sflow 2nd pass)");
    printf("%s: second_pass_context_id = %d\n", proc_name, cint_sflow_extended_gateway_info.second_pass_context_id);

    /* Set context preselector for "2nd pass"*/
    BCM_IF_ERROR_RETURN_MSG(cint_sflow_field_extended_gateway_group_context_presel_set(unit, cint_sflow_extended_gateway_info.second_pass_context_id, 0 /*is_first_pass*/),
        "(Sflow 2nd pass)");
    
    /* Set IPM3 - avoid ITPP termination from terminating SFLOW Header Record and In/Out-Port value */
    BCM_IF_ERROR_RETURN_MSG(cint_sflow_field_extended_gateway_group_context_ipmf3_create(unit, &cint_sflow_extended_gateway_info.second_pass_ipmf3_context_id, "Sflow 2nd context ipmf3"),
        "(Sflow 2nd pass)");
    printf("%s: second_pass_ipmf3_context_id = %d\n", proc_name, cint_sflow_extended_gateway_info.second_pass_ipmf3_context_id);
    
    BCM_IF_ERROR_RETURN_MSG(cint_sflow_field_extended_gateway_group_context_ipmf3_presel_set(unit, cint_sflow_extended_gateway_info.second_pass_ipmf3_context_id, cint_sflow_extended_gateway_info.second_pass_context_id),
        "(Sflow 2nd pass)");
    
    BCM_IF_ERROR_RETURN_MSG(sflow_field_extended_gateway_second_pass_ipmf3_const_create_and_attach(unit, cint_sflow_extended_gateway_info.second_pass_ipmf3_context_id), "");

    /*
     * Configure the CRPS for the sFlow. 
     * In SFLOW Extended-Gateway, the packet sequence number is "taken" on the Egress 2nd pass, and the destination is the eventor-port.
     */
    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(unit, eventor_port, cint_sflow_basic_info.counter_if, 0, &cint_sflow_basic_info.counter_base), "");

    printf("%s: counter_if = %d, counter_base = %d created.\n", proc_name, cint_sflow_basic_info.counter_if, cint_sflow_basic_info.counter_base);

    /* Create "2nd pass" SFLOW EEDB */
    bcm_instru_sflow_encap_info_t sflow_encap_info;
    sal_memset(&sflow_encap_info, 0, sizeof(sflow_encap_info));
    sflow_encap_info.flags = BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW | BCM_INSTRU_SFLOW_ENCAP_AGGREGATED;
    sflow_encap_info.counter_command_id = cint_sflow_basic_info.counter_if;
    sflow_encap_info.stat_cmd = cint_sflow_basic_info.counter_base;
    sflow_encap_info.sub_agent_id = 0; /* sub_agent_id is set in the Eventor header */
    sflow_encap_info.eventor_id = eventor_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_instru_sflow_encap_create(unit, &sflow_encap_info), "");

    cint_sflow_extended_gateway_info.sflow_second_pass_encap_id = sflow_encap_info.sflow_encap_id;

    printf("%s: sflow_encap_id = %d 0x%x\n", proc_name, cint_sflow_extended_gateway_info.sflow_second_pass_encap_id, cint_sflow_extended_gateway_info.sflow_second_pass_encap_id);

    /* Create "2nd pass" Extended AS Destination LIFs */
    bcm_instru_sflow_encap_extended_dst_info_t sflow_encap_extended_dst_info;
    sal_memset(&sflow_encap_extended_dst_info, 0, sizeof(sflow_encap_extended_dst_info));
    sflow_encap_extended_dst_info.dst_as_path[0] = cint_sflow_extended_gateway_info.dst_as_path_1;
    sflow_encap_extended_dst_info.dst_as_path[1] = cint_sflow_extended_gateway_info.dst_as_path_2;
    sflow_encap_extended_dst_info.dst_as_path[2] = cint_sflow_extended_gateway_info.dst_as_path_3;
    sflow_encap_extended_dst_info.dst_as_path[3] = cint_sflow_extended_gateway_info.dst_as_path_4;
    sflow_encap_extended_dst_info.dst_as_path_length = 4;
    sflow_encap_extended_dst_info.next_hop_ipv4_address = cint_sflow_extended_gateway_info.next_hop_ipv4_address;
    BCM_IF_ERROR_RETURN_MSG(bcm_instru_sflow_encap_extended_dst_create(unit, &sflow_encap_extended_dst_info), "");

    cint_sflow_extended_gateway_info.sflow_encap_extended_dst_id = sflow_encap_extended_dst_info.sflow_encap_extended_dst_id;
    printf("%s: sflow_encap_extended_dst_id = %d 0x%x\n", proc_name, cint_sflow_extended_gateway_info.sflow_encap_extended_dst_id, cint_sflow_extended_gateway_info.sflow_encap_extended_dst_id);

    /* Create "2nd pass" Extended AS Source LIFs */
    bcm_instru_sflow_encap_extended_src_info_t sflow_encap_extended_src_info;
    sal_memset(&sflow_encap_extended_src_info, 0, sizeof(sflow_encap_extended_src_info));
    sflow_encap_extended_src_info.src_as = cint_sflow_extended_gateway_info.src_as;
    sflow_encap_extended_src_info.src_as_peer = cint_sflow_extended_gateway_info.src_as_peer;
    BCM_IF_ERROR_RETURN_MSG(bcm_instru_sflow_encap_extended_src_create(unit, &sflow_encap_extended_src_info), "");

    cint_sflow_extended_gateway_info.sflow_encap_extended_src_id = sflow_encap_extended_src_info.sflow_encap_extended_src_id;
    printf("%s: sflow_encap_extended_src_id = %d 0x%x\n", proc_name, cint_sflow_extended_gateway_info.sflow_encap_extended_src_id, cint_sflow_extended_gateway_info.sflow_encap_extended_src_id);


    /* Create a trap for dropping the mirrored packet in the 2nd pass in case the EXEM lookup fails.*/
    BCM_IF_ERROR_RETURN_MSG(cint_sflow_field_extended_gateway_trap_create(unit), "");

    /* Set drop action in case of DIP lookup fails */
    BCM_IF_ERROR_RETURN_MSG(sflow_field_extended_gateway_second_pass_dip_const_create_and_attach(unit, cint_sflow_extended_gateway_info.second_pass_context_id), "");

    /* Set drop action in case of SIP lookup fails */
    BCM_IF_ERROR_RETURN_MSG(sflow_field_extended_gateway_second_pass_sip_const_create_and_attach(unit, cint_sflow_extended_gateway_info.second_pass_context_id), "");

    /* Set EXEM lookup - outlif0 and destination */
    BCM_IF_ERROR_RETURN_MSG(sflow_field_extended_gateway_second_pass_const_create_and_attach(unit, cint_sflow_extended_gateway_info.sflow_second_pass_encap_id, eventor_port, cint_sflow_extended_gateway_info.second_pass_context_id), "");

    /* Set EXEM lookup:
     *   - Key = DIP Destination (Fwd_Action_Dst)
     *   - Result = outlif1
     */ 
    BCM_IF_ERROR_RETURN_MSG(sflow_field_extended_gateway_second_pass_exem_dip_create_and_attach(unit, cint_sflow_extended_gateway_info.second_pass_context_id), "");

    /* Set EXEM lookup:
     *   - Key = SIP Destination (RPF_Dst )
     *   - Result = outlif2
     */ 
    BCM_IF_ERROR_RETURN_MSG(sflow_field_extended_gateway_second_pass_exem_sip_create_and_attach(unit, cint_sflow_extended_gateway_info.second_pass_context_id), "");


    /* Add entry to EXEM:
     *   Key = dst DIP destination
     *   Result = sflow_encap_extended_dst_id
     */
    BCM_IF_ERROR_RETURN_MSG(cint_sflow_field_extended_gateway_entry_add(unit, 1/* is_dip */, cint_sflow_basic_info.kaps_result, cint_sflow_extended_gateway_info.sflow_encap_extended_dst_id), "(DIP)");

    /* Add entry to EXEM:
     *   Key = dst SIP destination
     *   Result = sflow_encap_extended_src_id
     */
    BCM_IF_ERROR_RETURN_MSG(cint_sflow_field_extended_gateway_entry_add(unit, 0/* is_dip */, cint_sflow_basic_info.kaps_result, cint_sflow_extended_gateway_info.sflow_encap_extended_src_id), "(DIP)");

    BCM_IF_ERROR_RETURN_MSG(cint_sflow_extended_gateway_eventor_set(unit, sflow_out_port, eventor_port, nof_samples ,eventor_id), "");

    return BCM_E_NONE;
}



/*
 * This function creates the route from cint_basic_route.c. It is used as a sample flow
 * to sample and send to sFlow.
 */
int
cint_sflow_extended_gateway_route_setup(int unit, int in_port, int out_port, int eventor_port, int sflow_out_port)
{
    char error_msg[200]={0,};
    int fec_id;
    bcm_ip_t host_sip;
    char *proc_name;

    proc_name = "cint_sflow_extended_gateway_route_setup";

    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit,0, 0, &fec_id), "");
    cint_sflow_basic_info.kaps_result = fec_id;

    /*
     * Create some traffic flow.
     * In this example we use ip route basic, but it could be anything.
     */
    snprintf(error_msg, sizeof(error_msg), "(in_port = %d, out_port = %d, kaps_result = %d)",
        in_port, out_port, cint_sflow_basic_info.kaps_result);
    BCM_IF_ERROR_RETURN_MSG(dnx_basic_example(unit, in_port, out_port, cint_sflow_basic_info.kaps_result), error_msg);

    /* Add Reverse Packet Forwarding lookup to the routing setup */
    host_sip = 0xC0800101; /* 192.128.1.1 */
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv4_rpf(unit, host_sip, 0xffffffff, ingress_rif.vrf, cint_sflow_basic_info.kaps_result), "");

    printf("%s: adding RPF lookup:  host_sip = 0x%08x, vrf = %d, fec = 0x%08X\n", proc_name, host_sip, ingress_rif.vrf, cint_sflow_basic_info.kaps_result);

    return BCM_E_NONE;
}

/*
 * This function adds a port the route from cint_basic_route.c
 */
int
cint_sflow_extended_gateway_route_setup_port_add(int unit, int in_port_to_add)
{
    int fec_id;
    bcm_ip_t host_sip;
    char *proc_name;

    proc_name = "cint_sflow_extended_gateway_route_setup_port_add";

    bcm_port_match_info_t match_info;

    match_info.match = BCM_PORT_MATCH_PORT;
    match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
    match_info.port = in_port_to_add;

    BCM_IF_ERROR_RETURN_MSG(bcm_port_match_add(unit, in_port_intf[0], &match_info), "");

    printf("%s: set port = %d points to in_port_intf = 0x%08X\n", proc_name, in_port_to_add, in_port_intf[0]);

    return BCM_E_NONE;
}


/*
 * Setting eventor
 */
int
cint_sflow_extended_gateway_eventor_set(int unit, int sflow_out_port, int eventor_port, int nof_samples, int eventor_id)
{
   uint32 buffer_size;
   char *proc_name;

   proc_name = "cint_sflow_extended_gateway_eventor_set";

   printf("%s: sflow_out_port=%d, eventor_port = %d, nof_samples=%d, eventor_id=%d \n",
        proc_name, sflow_out_port, eventor_port, nof_samples, eventor_id);

   /* Buffer size  - need to add Extended_Gateway_data size */
   buffer_size = cint_sflow_basic_info.buffer_size + 16;

   /*
   * Set eventor configuration.
   */
   BCM_IF_ERROR_RETURN_MSG(cint_sflow_utils_eventor_set(unit, eventor_id, buffer_size, nof_samples, eventor_port, sflow_out_port, 0), "");

   return BCM_E_NONE;
}

/*
 * Call this function before calling any other main functions. 
 * It inits the S-Flow virtual registers.
 */
int
cint_sflow_extended_gateway_general_init(int unit)
{

    /*
     * Set the global virtual register for sFlow.
     */
    BCM_IF_ERROR_RETURN_MSG(cint_sflow_utils_registers_init(unit), "");

    /* Set My Router AS Number*/
    BCM_IF_ERROR_RETURN_MSG(bcm_instru_control_set(unit, 0, bcmInstruControlSFlowMyRouterASNumber, cint_sflow_extended_gateway_info.myRouterASNumber), "");

    return BCM_E_NONE;
}

/*
 * Create SFLOW DATAGRAM IP Forwarding setup - for Eventor port to SFLOW out port
 */
int cint_sflow_extended_gateway_forwarding_setup(int unit, int eventor_port, int sflow_out_port)
{
   char *proc_name;

   proc_name = "cint_sflow_extended_gateway_entry_add_eventor_port";

   /*
    * Create a field entry for preventing snooping of the SFLOW DATAGRAM coming from the Eventor port
    */
   BCM_IF_ERROR_RETURN_MSG(cint_sflow_field_utils_entry_add_eventor_port(unit, eventor_port, cint_sflow_snoop_field_fg_id, &cint_sflow_eventor_port_entry_id, cint_field_eventor_port_entry_priority), "");

   printf("%s: TCAM entry = %d is added for fg_id = %d \n", proc_name, cint_sflow_eventor_port_entry_id, cint_sflow_snoop_field_fg_id);

   /*
    * Use IP forwarding for the SFLOW datagram
    */

   /*
    * Set ip route eventor_port ==> sflow_out_port
    */
    BCM_IF_ERROR_RETURN_MSG(cint_sflow_utils_route_sflow_datagram(unit, eventor_port, sflow_out_port, cint_sflow_basic_info.kaps_result +1), "");

   /*
    * Configure SFLOW datagram SN update
    */
   BCM_IF_ERROR_RETURN_MSG(cint_sflow_utils_sflow_sn_config(unit, eventor_port, sflow_out_port, cint_sflow_extended_gateway_info.first_pass_context_id), "");
   return BCM_E_NONE;
}

int
cint_sflow_extended_gateway_destroy(int unit, int nof_cores)
{
    char error_msg[200]={0,};


    BCM_IF_ERROR_RETURN_MSG(cint_sflow_field_utils_destroy(unit, cint_sflow_extended_gateway_info.first_pass_context_id), "");

    /* Delete "1st pass" PMF configuration */
    BCM_IF_ERROR_RETURN_MSG(cint_sflow_field_extended_gateway_destroy(unit, cint_sflow_extended_gateway_info.first_pass_context_id, nof_cores),
        "(1st pass)");

    /* Delete "1st pass" SFLOW EEDB */
    bcm_instru_sflow_encap_info_t sflow_encap_info;
    sal_memset(&sflow_encap_info, 0, sizeof(sflow_encap_info));
    sflow_encap_info.sflow_encap_id = cint_sflow_extended_gateway_info.sflow_first_pass_encap_id;
    snprintf(error_msg, sizeof(error_msg), "(1st pass sflow_encap_id = %d)",
        cint_sflow_extended_gateway_info.sflow_first_pass_encap_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_instru_sflow_encap_delete(unit, &sflow_encap_info), error_msg);

    /* Delete "2nd pass" PMF configuration */
/*
    BCM_IF_ERROR_RETURN_MSG(cint_sflow_field_extended_gateway_destroy(unit, cint_sflow_extended_gateway_info.second_pass_context_id),
        "(2nd pass)");
*/
    /* Delete "2nd pass" SFLOW EEDB */
 
    sflow_encap_info.sflow_encap_id = cint_sflow_extended_gateway_info.sflow_second_pass_encap_id;
    snprintf(error_msg, sizeof(error_msg), "(2nd pass sflow_encap_id = %d)",
        cint_sflow_extended_gateway_info.sflow_second_pass_encap_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_instru_sflow_encap_delete(unit, &sflow_encap_info), error_msg);

    return BCM_E_NONE;
}
