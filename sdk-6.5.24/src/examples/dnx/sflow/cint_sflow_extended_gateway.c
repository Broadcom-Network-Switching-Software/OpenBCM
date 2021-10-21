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
 *  cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 *  cint ../../../../src/examples/dnx/field/cint_field_utils.c
 *  cint ../../../../src/examples/dnx/crps/cint_crps_oam_config.c
 *  cint ../../../../src/examples/dnx/oam/cint_oam_basic.c
 *  cint ../../../../src/examples/sand/cint_ip_route_basic.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_field_extended_gateway.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_field_utils.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_utils.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_basic.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_extended_gateway.c
 *  
 *  c
 * 
 *  cint_sflow_extended_gateway_general_init(unit);
 *  cint_sflow_extended_gateway_route_setup($unit, inPort, routeOutPort); 
 *  cint_sflow_extended_gateway_first_pass_create(unit, inPort, nof_cores, sflow_recycle_port_core_0, sflow_recycle_port_core_1);
 *  cint_sflow_extended_gateway_second_pass_create(unit, inPort, nof_cores, sflow_recycle_port_core_0, sflow_recycle_port_core_1, eventor_id, eventorPort, sflowOutPort);
 *  cint_sflow_extended_gateway_forwarding_setup(unit, eventorPort, sflowOutPort);
 *  cint_sflow_utils_eventor_activate(unit);
 *  
 *  For setting input/output interface value:
 *  cint_sflow_utils_egress_interface_create(unit, port, interface, is_input);
 *
 *  For cleanup:
 *  
 *  cint_sflow_extended_gateway_destroy(unit);
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
   int next_hop_ipv4_address;
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
    int rv;
    bcm_gport_t sflow_mirror_destination_core_0, sflow_mirror_destination_core_1;
    char *proc_name;

    proc_name = "cint_sflow_extended_gateway_first_pass_create";

    /* Create context for "1st pass"*/
    rv = cint_sflow_field_extended_gateway_group_context_create(unit, &cint_sflow_extended_gateway_info.first_pass_context_id, "Sflow 1st context", 1 /* is_first_pass */);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), cint_sflow_field_extended_gateway_group_context_create(Sflow 1st pass)\n", proc_name, rv);
       return rv;
    }

    printf("%s: first_pass_context_id = %d\n", proc_name, cint_sflow_extended_gateway_info.first_pass_context_id);

    /* Set context preselector for "1st pass"*/
    rv = cint_sflow_field_extended_gateway_group_context_presel_set(unit, cint_sflow_extended_gateway_info.first_pass_context_id, 1 /*is_first_pass*/);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), cint_sflow_field_extended_gateway_group_context_presel_set(Sflow 1st pass)\n", proc_name, rv);
       return rv;
    }

    /* Map system to local port for in_port */
    if (!BCM_GPORT_IS_TRUNK(in_port))
    {
      rv = bcm_port_control_set(unit, in_port, bcmPortControlSystemPortInjectedMap, 1);
      if (rv != BCM_E_NONE)
      {
          printf("%s: Error (%d), in bcm_port_control_set failed bcmPortControlSystemPortInjectedMap\n", proc_name, rv);
          return rv;
      }
    }
    /* Set recycle port property: skip the SFLOW Sample Record:
     *   - SFLOW Header Record size = 6 words
     *   - Original Dst-port and Src-port are added = 2 words
     *   Thus skip 8 words for processing the original packet.
     */
    rv = bcm_port_control_set(unit, sflow_recycle_port_core_0, bcmPortControlFirstHeaderSizeAfterInjected, 8*4);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in bcm_port_control_set(port = %d)\n", proc_name, rv, sflow_recycle_port_core_0);
       return rv;
    }

    if (nof_cores == 2) {
       rv = bcm_port_control_set(unit, sflow_recycle_port_core_1, bcmPortControlFirstHeaderSizeAfterInjected, 8*4);
       if (rv != BCM_E_NONE)
       {
          printf("%s: Error (%d), in bcm_port_control_set(port = %d)\n", proc_name, rv, sflow_recycle_port_core_1);
          return rv;
       }
    }

    /* Create "1st pass" SFLOW EEDB */
    bcm_instru_sflow_encap_info_t sflow_encap_info;
    sal_memset(&sflow_encap_info, 0, sizeof(sflow_encap_info));
    sflow_encap_info.flags = BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR;
    rv = bcm_instru_sflow_encap_create(unit, &sflow_encap_info);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in bcm_instru_sflow_encap_create\n", proc_name, rv);
       return rv;
    }

    cint_sflow_extended_gateway_info.sflow_first_pass_encap_id = sflow_encap_info.sflow_encap_id;
    printf("%s: sflow_encap_id = %d 0x%x\n", proc_name, cint_sflow_extended_gateway_info.sflow_first_pass_encap_id, cint_sflow_extended_gateway_info.sflow_first_pass_encap_id);

    cint_sflow_basic_info.sflow_encap_id = cint_sflow_extended_gateway_info.sflow_first_pass_encap_id;

    /*
     * Create a mirror destination of type stat sampling, including ingress crop, and stat rate,
     * and modified ftmh header with sFlow extension.
     */
    rv = cint_sflow_utils_mirror_create(unit, sflow_recycle_port_core_0, cint_sflow_extended_gateway_info.sflow_first_pass_encap_id,
            cint_sflow_basic_info.sample_rate_dividend, cint_sflow_basic_info.sample_rate_divisor,
            &sflow_mirror_destination_core_0, 0);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in cint_sflow_utils_mirror_create(out_port = %d)\n", proc_name, rv, sflow_recycle_port_core_0);
       return rv;
    }

    cint_sflow_extended_gateway_info.sflow_mirror_destination_core_0 = sflow_mirror_destination_core_0;
    printf("%s: sflow_mirror_destination_core_0 = %d 0x%x\n", proc_name, sflow_mirror_destination_core_0, sflow_mirror_destination_core_0);

    if (nof_cores == 2)
    {
       rv = cint_sflow_utils_mirror_create(unit, sflow_recycle_port_core_1, cint_sflow_extended_gateway_info.sflow_first_pass_encap_id,
               cint_sflow_basic_info.sample_rate_dividend, cint_sflow_basic_info.sample_rate_divisor,
               &sflow_mirror_destination_core_1, 0);
       if (rv != BCM_E_NONE)
       {
          printf("%s: Error (%d), in cint_sflow_utils_mirror_create(out_port = %d)\n", proc_name, rv, sflow_recycle_port_core_1);
          return rv;
       }

       cint_sflow_extended_gateway_info.sflow_mirror_destination_core_1 = sflow_mirror_destination_core_1;
       printf("%s: sflow_mirror_destination_core_1 = %d 0x%x\n", proc_name, sflow_mirror_destination_core_1, sflow_mirror_destination_core_1);
    }

    /* Create snoop, attach it to context and sets it's action */
    rv = cint_sflow_field_extended_gateway_first_pass_group_create_and_attach(unit, nof_cores, sflow_mirror_destination_core_0, sflow_mirror_destination_core_1, cint_sflow_extended_gateway_info.first_pass_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), cint_sflow_field_extended_gateway_group_create_and_attach(Sflow 1st pass)\n", proc_name, rv);
        return rv;
    }

    rv = cint_sflow_field_egress_estimate_encap_size(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), cint_sflow_field_egress_estimate_encap_size(Sflow 1st pass)\n", proc_name, rv);
        return rv;
    }

    return rv;
}

/*
 * This function configures sFlow IPoETH second pass. 
 * Thats includes: 
 * 1. PMF setting - snoop "any IPoETH" packet
 * 2. Creates SFOW EEDB of first pass.
 * 3. Create mirroring of the snooped packet and attached it to the SFLOW EEDB.
 * destination.
 */
int
cint_sflow_extended_gateway_second_pass_create(int unit, int in_port, int nof_cores, int sflow_recycle_port_core_0, int sflow_recycle_port_core_1, int eventor_id, int eventor_port, int sflow_out_port, int nof_samples)
{
    int rv = BCM_E_NONE;
    bcm_port_class_t port_class;
    bcm_gport_t gport_sflow_recycle_port;
    char *proc_name;

    proc_name = "cint_sflow_extended_gateway_second_pass_create";

    /* Make a sflow recycle port Gport */
    BCM_GPORT_LOCAL_SET(gport_sflow_recycle_port, sflow_recycle_port_core_0);
    
    port_class = bcmPortClassFieldIngressPMF1TrafficManagementPortCs;

    /* Get the old port_profile value to be resumed after delete.*/
    rv = bcm_port_class_get(unit, gport_sflow_recycle_port, port_class, &cint_sflow_extended_gateway_info.sflow_recycle_port_core_0_profile_old_value);
    if (rv != BCM_E_NONE)
    {
        printf("s%: Error (%d), in bcm_port_class_get\n", proc_name, rv);
        return rv;
    }

    /* Set port profile - to be used for PMF context selection*/
    rv = bcm_port_class_set(unit, gport_sflow_recycle_port, port_class, cint_sflow_extended_gateway_info.sflow_recycle_port_profile);
    if (rv != BCM_E_NONE)
    {
        printf("s%: Error (%d), in bcm_port_class_set(gport = 0x%x, port = %d)\n", proc_name, rv, gport_sflow_recycle_port, sflow_recycle_port_core_0);
        return rv;
    }

    printf("%s: sflow_recycle_port_core_0 = %d, new-profile = %d, old-profile = %d \n", proc_name, sflow_recycle_port_core_0, cint_sflow_extended_gateway_info.sflow_recycle_port_profile, cint_sflow_extended_gateway_info.sflow_recycle_port_core_0_profile_old_value);

    if (nof_cores == 2) {

       /* Make a sflow recycle port Gport */
       BCM_GPORT_LOCAL_SET(gport_sflow_recycle_port, sflow_recycle_port_core_1);
    
       /* Get the old port_profile value to be resumed after delete.*/
       rv = bcm_port_class_get(unit, gport_sflow_recycle_port, port_class, &cint_sflow_extended_gateway_info.sflow_recycle_port_core_1_profile_old_value);
       if (rv != BCM_E_NONE)
       {
           printf("s%: Error (%d), in bcm_port_class_get\n", proc_name, rv);
           return rv;
       }

       /* Set port profile - to be used for PMF context selection*/
       rv = bcm_port_class_set(unit, gport_sflow_recycle_port, port_class, cint_sflow_extended_gateway_info.sflow_recycle_port_profile);
       if (rv != BCM_E_NONE)
       {
           printf("s%: Error (%d), in bcm_port_class_set(gport = 0x%x, port = %d)\n", proc_name, rv, gport_sflow_recycle_port, sflow_recycle_port_core_1);
           return rv;
       }

       printf("%s: sflow_recycle_port_core_1 = %d, new-profile = %d, old-profile = %d \n", proc_name, sflow_recycle_port_core_1, cint_sflow_extended_gateway_info.sflow_recycle_port_profile, cint_sflow_extended_gateway_info.sflow_recycle_port_core_1_profile_old_value);

    }

    /* Create context for "2nd pass"*/
    rv = cint_sflow_field_extended_gateway_group_context_create(unit, &cint_sflow_extended_gateway_info.second_pass_context_id, "Sflow 2nd context",  0 /* is_first_pass */);
    if (rv != BCM_E_NONE)
    {
       printf("%s: : Error (%d), cint_sflow_field_extended_gateway_group_context_create(Sflow 2nd pass)\n", proc_name, rv);
       return rv;
    }
    printf("%s: second_pass_context_id = %d\n", proc_name, cint_sflow_extended_gateway_info.second_pass_context_id);

    /* Set context preselector for "2nd pass"*/
    rv = cint_sflow_field_extended_gateway_group_context_presel_set(unit, cint_sflow_extended_gateway_info.second_pass_context_id, 0 /*is_first_pass*/);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), cint_sflow_field_extended_gateway_group_context_presel_set(Sflow 2nd pass)\n", proc_name, rv);
       return rv;
    }
    
    /* Set IPM3 - avoid ITPP termination from terminating SFLOW Header Record and In/Out-Port value */
    rv = cint_sflow_field_extended_gateway_group_context_ipmf3_create(unit, &cint_sflow_extended_gateway_info.second_pass_ipmf3_context_id, "Sflow 2nd context ipmf3");
    if (rv != BCM_E_NONE)
    {
       printf("%s: : Error (%d), cint_sflow_field_extended_gateway_group_context_ipmf3_create(Sflow 2nd pass)\n", proc_name, rv);
       return rv;
    }
    printf("%s: second_pass_ipmf3_context_id = %d\n", proc_name, cint_sflow_extended_gateway_info.second_pass_ipmf3_context_id);
    
    rv = cint_sflow_field_extended_gateway_group_context_ipmf3_presel_set(unit, cint_sflow_extended_gateway_info.second_pass_ipmf3_context_id, cint_sflow_extended_gateway_info.second_pass_context_id);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), cint_sflow_field_extended_gateway_group_context_ipmf3_presel_set(Sflow 2nd pass)\n", proc_name, rv);
       return rv;
    }
    
    rv = sflow_field_extended_gateway_second_pass_ipmf3_const_create_and_attach(unit, cint_sflow_extended_gateway_info.second_pass_ipmf3_context_id);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in sflow_field_extended_gateway_second_pass_ipmf3_const_create_and_attach\n", proc_name, rv);
       return rv;
    }

    /*
     * Configure the CRPS for the sFlow. 
     * In SFLOW Extended-Gateway, the packet sequence number is "taken" on the Egress 2nd pass, and the destination is the eventor-port.
     */
    rv = set_counter_resource(unit, eventor_port, cint_sflow_basic_info.counter_if, 0, &cint_sflow_basic_info.counter_base);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in set_counter_resource\n");
        return rv;
    }

    printf("%s: counter_if = %d, counter_base = %d created.\n", proc_name, cint_sflow_basic_info.counter_if, cint_sflow_basic_info.counter_base);

    /* Create "2nd pass" SFLOW EEDB */
    bcm_instru_sflow_encap_info_t sflow_encap_info;
    sal_memset(&sflow_encap_info, 0, sizeof(sflow_encap_info));
    sflow_encap_info.flags = BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW | BCM_INSTRU_SFLOW_ENCAP_AGGREGATED;
    sflow_encap_info.counter_command_id = cint_sflow_basic_info.counter_if;
    sflow_encap_info.stat_cmd = cint_sflow_basic_info.counter_base;
    sflow_encap_info.sub_agent_id = 0; /* sub_agent_id is set in the Eventor header */
    sflow_encap_info.eventor_id = eventor_id;
    rv = bcm_instru_sflow_encap_create(unit, &sflow_encap_info);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in bcm_instru_sflow_encap_create\n", proc_name, rv);
       return rv;
    }

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
    rv = bcm_instru_sflow_encap_extended_dst_create(unit, &sflow_encap_extended_dst_info);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in bcm_instru_sflow_encap_extended_dst_create\n", proc_name, rv);
       return rv;
    }

    cint_sflow_extended_gateway_info.sflow_encap_extended_dst_id = sflow_encap_extended_dst_info.sflow_encap_extended_dst_id;
    printf("%s: sflow_encap_extended_dst_id = %d 0x%x\n", proc_name, cint_sflow_extended_gateway_info.sflow_encap_extended_dst_id, cint_sflow_extended_gateway_info.sflow_encap_extended_dst_id);

    /* Create "2nd pass" Extended AS Source LIFs */
    bcm_instru_sflow_encap_extended_src_info_t sflow_encap_extended_src_info;
    sal_memset(&sflow_encap_extended_src_info, 0, sizeof(sflow_encap_extended_src_info));
    sflow_encap_extended_src_info.src_as = cint_sflow_extended_gateway_info.src_as;
    sflow_encap_extended_src_info.src_as_peer = cint_sflow_extended_gateway_info.src_as_peer;
    rv = bcm_instru_sflow_encap_extended_src_create(unit, &sflow_encap_extended_src_info);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in bcm_instru_sflow_encap_extended_src_create\n", proc_name, rv);
       return rv;
    }

    cint_sflow_extended_gateway_info.sflow_encap_extended_src_id = sflow_encap_extended_src_info.sflow_encap_extended_src_id;
    printf("%s: sflow_encap_extended_src_id = %d 0x%x\n", proc_name, cint_sflow_extended_gateway_info.sflow_encap_extended_src_id, cint_sflow_extended_gateway_info.sflow_encap_extended_src_id);


    /* Create a trap for dropping the mirrored packet in the 2nd pass in case the EXEM lookup fails.*/
    rv = cint_sflow_field_extended_gateway_trap_create(unit);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in cint_sflow_field_extended_gateway_trap_create\n", proc_name, rv);
       return rv;
    }

    /* Set drop action in case of DIP lookup fails */
    rv = sflow_field_extended_gateway_second_pass_dip_const_create_and_attach(unit, cint_sflow_extended_gateway_info.second_pass_context_id);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in sflow_field_extended_gateway_second_pass_dip_const_create_and_attach\n", proc_name, rv);
       return rv;
    }

    /* Set drop action in case of SIP lookup fails */
    rv = sflow_field_extended_gateway_second_pass_sip_const_create_and_attach(unit, cint_sflow_extended_gateway_info.second_pass_context_id);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in sflow_field_extended_gateway_second_pass_sip_const_create_and_attach\n", proc_name, rv);
       return rv;
    }

    /* Set EXEM lookup - outlif0 and destination */
    rv = sflow_field_extended_gateway_second_pass_const_create_and_attach(unit, cint_sflow_extended_gateway_info.sflow_second_pass_encap_id, eventor_port, cint_sflow_extended_gateway_info.second_pass_context_id);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in sflow_field_extended_gateway_second_pass_const_create_and_attach\n", proc_name, rv);
       return rv;
    }

    /* Set EXEM lookup:
   *   - Key = DIP Destination (Fwd_Action_Dst)
   *   - Result = outlif1
   */ 
    rv = sflow_field_extended_gateway_second_pass_exem_dip_create_and_attach(unit, cint_sflow_extended_gateway_info.second_pass_context_id);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in sflow_field_extended_gateway_second_pass_exem_dip_create_and_attach\n", proc_name, rv);
       return rv;
    }

    /* Set EXEM lookup:
   *   - Key = SIP Destination (RPF_Dst )
   *   - Result = outlif2
   */ 
    rv = sflow_field_extended_gateway_second_pass_exem_sip_create_and_attach(unit, cint_sflow_extended_gateway_info.second_pass_context_id);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in sflow_field_extended_gateway_second_pass_exem_sip_create_and_attach\n", proc_name, rv);
       return rv;
    }


    /* Add entry to EXEM:
   *   Key = dst DIP destination
   *   Result = sflow_encap_extended_dst_id
   */
    rv = cint_sflow_field_extended_gateway_entry_add(unit, 1/* is_dip */, cint_sflow_basic_info.kaps_result, cint_sflow_extended_gateway_info.sflow_encap_extended_dst_id);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in cint_sflow_field_extended_gateway_entry_add(DIP) \n", proc_name, rv);
       return rv;
    }

    /* Add entry to EXEM:
   *   Key = dst SIP destination
   *   Result = sflow_encap_extended_src_id
   */
    rv = cint_sflow_field_extended_gateway_entry_add(unit, 0/* is_dip */, cint_sflow_basic_info.kaps_result, cint_sflow_extended_gateway_info.sflow_encap_extended_src_id);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in cint_sflow_field_extended_gateway_entry_add(DIP) \n", proc_name, rv);
       return rv;
    }

    rv = cint_sflow_extended_gateway_eventor_set(unit, sflow_out_port, eventor_port, nof_samples ,eventor_id);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in cint_sflow_extended_gateway_eventor_set \n", proc_name, rv);
       return rv;
    }

    return rv;
}



/*
 * This function creates the route from cint_basic_route.c. It is used as a sample flow
 * to sample and send to sFlow.
 */
int
cint_sflow_extended_gateway_route_setup(int unit, int in_port, int out_port, int eventor_port, int sflow_out_port)
{
    bcm_error_t rv = BCM_E_NONE;
    int fec_id;
    bcm_ip_t host_sip;
    char *proc_name;

    proc_name = "cint_sflow_extended_gateway_route_setup";

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit,0, &fec_id);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in get_first_fec_in_range_which_not_in_ecmp_range\n", proc_name, rv);
       return rv;
    }
    cint_sflow_basic_info.kaps_result = fec_id;

    /*
     * Create some traffic flow.
     * In this example we use ip route basic, but it could be anything.
     */
    rv = dnx_basic_example(unit, in_port, out_port, cint_sflow_basic_info.kaps_result);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), dnx_basic_example(in_port = %d, out_port = %d, kaps_result = %d)\n", proc_name, rv, in_port, out_port, cint_sflow_basic_info.kaps_result);
       return rv;
    }

    /* Add Reverse Packet Forwarding lookup to the routing setup */
    host_sip = 0xC0800101; /* 192.128.1.1 */
    rv = add_route_ipv4_rpf(unit, host_sip, 0xffffffff, ingress_rif.vrf, cint_sflow_basic_info.kaps_result);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in_port_intf_get\n", proc_name, rv);
       return rv;
    }

    printf("%s: adding RPF lookup:  host_sip = 0x%08x, vrf = %d, fec = 0x%08X\n", proc_name, host_sip, ingress_rif.vrf, cint_sflow_basic_info.kaps_result);

    return rv;
}

/*
 * This function adds a port the route from cint_basic_route.c
 */
int
cint_sflow_extended_gateway_route_setup_port_add(int unit, int in_port_to_add)
{
    bcm_error_t rv = BCM_E_NONE;
    int fec_id;
    bcm_ip_t host_sip;
    char *proc_name;

    proc_name = "cint_sflow_extended_gateway_route_setup_port_add";

   /* 
    * Set the port to point to the same route configuration,
    * that is points from the port to the AC In-LIF 
    */
    bcm_gport_t in_port_intf;

    /* Get the AC In-LIF */
    rv = in_port_intf_get(unit, 0, &in_port_intf);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in_port_intf_get\n", proc_name, rv);
       return rv;
    }

    bcm_port_match_info_t match_info;

    match_info.match = BCM_PORT_MATCH_PORT;
    match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
    match_info.port = in_port_to_add;

    rv = bcm_port_match_add(unit, in_port_intf, &match_info);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), bcm_port_match_add\n", proc_name, rv);
       return rv;
    }

    printf("%s: set port = %d points to in_port_intf = 0x%08X\n", proc_name, in_port_to_add, in_port_intf);

    return rv;
}


/*
 * Setting eventor
 */
int
cint_sflow_extended_gateway_eventor_set(int unit, int sflow_out_port, int eventor_port, int nof_samples, int eventor_id)
{
   int rv;
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
   rv = cint_sflow_utils_eventor_set(unit, eventor_id, buffer_size, nof_samples, eventor_port, sflow_out_port, 0);
   if (rv != BCM_E_NONE)
   {
      printf("%s:Error, in cint_sflow_utils_eventor_set\n", proc_name);
      return rv;
   }

   return BCM_E_NONE;
}

/*
 * Call this function before calling any other main functions. 
 * It inits the S-Flow virtual registers.
 */
int
cint_sflow_extended_gateway_general_init(int unit)
{
    int rv;
    char *proc_name;

    proc_name = "cint_sflow_extended_gateway_general_init";

    /*
     * Set the global virtual register for sFlow.
     */
    rv = cint_sflow_utils_registers_init(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in cint_sflow_utils_registers_init\n", proc_name, rv);
        return rv;
    }

    /* Set My Router AS Number*/
    rv = bcm_instru_control_set(unit, 0, bcmInstruControlSFlowMyRouterASNumber, cint_sflow_extended_gateway_info.myRouterASNumber);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_dnx_instru_control_get\n", proc_name, rv);
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Create SFLOW DATAGRAM IP Forwarding setup - for Eventor port to SFLOW out port
 */
int cint_sflow_extended_gateway_forwarding_setup(int unit, int eventor_port, int sflow_out_port)
{
   int rv = BCM_E_NONE;
   char *proc_name;

   proc_name = "cint_sflow_extended_gateway_entry_add_eventor_port";

   /*
 * Create a field entry for preventing snooping of the SFLOW DATAGRAM coming from the Eventor port
 */
   rv = cint_sflow_field_utils_entry_add_eventor_port(unit, eventor_port, cint_sflow_snoop_field_fg_id, &cint_cflow_eventor_port_entry_id, cint_field_eventor_port_entry_priority);
   if (rv != BCM_E_NONE)
   {
      printf("%s: Error (%d), in cint_sflow_field_utils_entry_add_eventor_port\n", proc_name, rv);
      return rv;
   }

   printf("%s: TCAM entry = %d is added for fg_id = %d \n", proc_name, cint_cflow_eventor_port_entry_id, cint_sflow_snoop_field_fg_id);

   /*
   * Use IP forwarding for the SFLOW datagram
   */

   /*
    * Set ip route eventor_port ==> sflow_out_port
    */
    rv = cint_sflow_utils_route_sflow_datagram(unit, eventor_port, sflow_out_port, cint_sflow_basic_info.kaps_result +1);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in cint_sflow_utils_route_sflow_datagram\n", proc_name, rv);
       return rv;
    }

   /*
    * Configure SFLOW datagram SN update
    */
   rv = cint_sflow_utils_sflow_sn_config(unit, eventor_port, sflow_out_port, cint_sflow_extended_gateway_info.first_pass_context_id);
   if (rv != BCM_E_NONE)
   {
      printf("%s: Error (%d), in cint_sflow_utils_sflow_sn_config\n", proc_name, rv);
      return rv;
   }
   return rv;
}

int
cint_sflow_extended_gateway_destroy(int unit, int nof_cores)
{
    int rv;
    char *proc_name;

    proc_name = "cint_sflow_extended_gateway_destroy";

    rv = cint_sflow_field_utils_destroy(unit, cint_sflow_extended_gateway_info.first_pass_context_id);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in cint_sflow_field_utils_destroy\n", proc_name, rv);
       return rv;
    }

    /* Delete "1st pass" PMF configuration */
    rv = cint_sflow_field_extended_gateway_destroy(unit, cint_sflow_extended_gateway_info.first_pass_context_id, nof_cores);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in cint_sflow_field_extended_gateway_destroy(1st pass)\n", proc_name, rv);
       return rv;
    }

    /* Delete "1st pass" SFLOW EEDB */
    bcm_instru_sflow_encap_info_t sflow_encap_info;
    sal_memset(&sflow_encap_info, 0, sizeof(sflow_encap_info));
    sflow_encap_info.sflow_encap_id = cint_sflow_extended_gateway_info.sflow_first_pass_encap_id;
    rv = bcm_instru_sflow_encap_delete(unit, &sflow_encap_info);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in bcm_instru_sflow_encap_delete(1st pass sflow_encap_id = %d)\n", proc_name, rv, cint_sflow_extended_gateway_info.sflow_first_pass_encap_id);
       return rv;
    }

    /* Delete "2nd pass" PMF configuration */
/*
    rv = cint_sflow_field_extended_gateway_destroy(unit, cint_sflow_extended_gateway_info.second_pass_context_id);
    if (rv != BCM_E_NONE)
    {
       printf("cint_sflow_extended_gateway_destroy: Error (%d), in cint_sflow_field_extended_gateway_destroy(2nd pass)\n", rv);
       return rv;
    }
*/
    /* Delete "2nd pass" SFLOW EEDB */
 
    sflow_encap_info.sflow_encap_id = cint_sflow_extended_gateway_info.sflow_second_pass_encap_id;
    rv = bcm_instru_sflow_encap_delete(unit, &sflow_encap_info);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in bcm_instru_sflow_encap_delete(2nd pass sflow_encap_id = %d)\n", proc_name, rv, cint_sflow_extended_gateway_info.sflow_second_pass_encap_id);
       return rv;
    }

    return BCM_E_NONE;
}
