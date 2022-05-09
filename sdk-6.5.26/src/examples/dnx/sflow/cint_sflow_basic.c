/* $Id: cint_sflow_basic.c
*/

/**
 *
 * This cint calls all the required APIs to configure a basic sFlow agent.
 *
 * The general init function initializes general objects like field qualifier and CRPS counters.
 * The egress create function creates the egress side of the sFlow - the encapsulation stack that
 *  builds the datagram and sends it as a UDP packet to the datagram collector.
 * The ingress create function creates the ingress side of the sFlow - creates a mirror destination for stat
 *  sampling, packet crop and other sFflow datagram fields. Then it adds a specific field entry and maps it to
 *  this mirror destination.
 * The main function creates a simple route flow and then calls the ingress and egress creation functions for it.
 *
 * How to run:
 *  cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 *  cint ../../../../src/examples/dnx/field/cint_field_utils.c
 *  cint ../../../../src/examples/dnx/crps/cint_crps_oam_config.c
 *  cint ../../../../src/examples/dnx/oam/cint_oam_basic.c
 *  cint ../../../../src/examples/sand/cint_ip_route_basic.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_field.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_field_utils.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_utils.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_basic.c
 *  
 *  c
 *
 *  if egress sflow is used
 *  cint_sflow_basic_egress_general_init(unit, is_egress_sflow, egress_mirror_port);
 *
 *  And then
 *  cint_sflow_basic_general_init(unit, sflow_out_port, eventor_id,nof_samples, is_aggregated, is_ITMH_forwarding);
 *  
 *  if eventor is used:
 *  sflow_eventor_activate(unit);
 *  
 *  And then:
 *  cint_sflow_basic_route_main(unit, in_port, route_out_port, sflow_out_port, eventor_id, is_aggregated, is_ITMH_forwarding);
 *  
 *  For setting input/output interface value:
 *  cint_sflow_utils_egress_interface_create(unit, port, interface, is_input);
 *
 *  To keep the sFlow timestamp up to date, call sflow_timestamp_update every millisecond with the time of
 *  day in milliseconds.
 *
 */

struct cint_sflow_egress_basic_info_s {
    int is_egress_sflow;
    int egress_mirror_port;
    /* Mirror properties. */
    uint32 sample_rate_dividend; /* Sample rate = rate dividend / rate_divisor. Dividend must always be 1. */
    uint32 sample_rate_divisor;
    bcm_gport_t egress_mirror_destination; /* a mirror destination with a sample rate of dividend / divisor */
};

struct cint_sflow_basic_info_s {
    /* CRPS Properties */
    int counter_if;           /* Stat command for OAM counter */
    int counter_base;         /* Counter base for OAM counter */
    int sn_counter_stat_id;
    /* Mirror properties. */
    uint32 sample_rate_dividend; /* Sample rate = rate dividend / rate_divisor. Dividend must always be 1. */
    uint32 sample_rate_divisor;
    bcm_gport_t sflow_mirror_destination; /* a mirror destination with a sample rate of dividend / divisor */

    /* UDP tunnel properties */
    bcm_gport_t udp_tunnel_id;            /* the created udp tunnel*/
    int out_rif;                          /* Outrif for udp tunnel. */
    bcm_mac_t intf_access_mac_address;    /* mac for RIF */
    bcm_mac_t tunnel_next_hop_mac;        /* mac for the next hop in the tunnel to the collector */
    bcm_ip_t tunnel_dip;                  /* tunnel DIP */
    bcm_ip_t tunnel_sip;                  /* tunnel SIP */
    bcm_ip6_t tunnel_dip6;                /* tunnel DIP v6 */
    bcm_ip6_t tunnel_sip6;                /* tunnel DIP v6 */
    bcm_tunnel_type_t tunnel_type;        /* tunnel type */
    uint16 udp_src_port;                  /* UDP source port */
    uint16 udp_dst_port;                  /* UDP destination port */
    int collector_vlan;                   /* VID assigned to the outgoing packet */
    int tunnel_ttl;                       /* Ip tunnel header TTL */
    uint32 sub_agent_id;                  /* Sub agent ID for the sFlow datagram header. */
    uint32 kaps_result;                   /* kaps result for the basic route. */
    uint32 uptime;                        /* Random uptime for the cint example. */

    int sflow_encap_id;                   /* SFLOW ECAP ID - is needed in the mirroring for setting encapsulation size, see cint_sflow_utils_mirror_create*/

    /* Eventor properties */
    uint32 buffer_size;
    uint32 time_thresh; /* in mSec */
    cint_sflow_egress_basic_info_s egress_sflow;
};

cint_sflow_basic_info_s cint_sflow_basic_info = {
    /* Counter interface | Counter base */
               0,           0,
   /* sn_counter_stat_id */
            111,
    /* Sample rate dividend | divisor */
               1,              1,
   /* sflow_mirror_destination */
      -1,
   /*udp_tunnel_id */
      -1,
   /* out rif */
       120,
    /* intf_access_mac_address             | tunnel_next_hop_mac */
       {0x00, 0x0e, 0x00, 0x02, 0x00, 0x02}, {0x00, 0x0e, 0x00, 0x02, 0x00, 0x03},
    /* tunnel DIP */
       0xA1000011 /* 161.0.0.17 */,
    /* tunnel SIP */
       0xA0000011 /* 160.0.0.17 */,
    /* tunnel IPv6 DIP */
       { 0x20, 0x01, 0x0D, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x22, 0xFF, 0xFE, 0x33, 0x44, 0x55 },
    /* tunnel IPv6 SIP */
       { 0xEC, 0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE1 },
    /* tunnel type */
       bcmTunnelTypeUdp,
    /* UDP SRC | DST ports */
       0, 6343,
    /* collector vlan */
       120,
    /* tunnel_ttl */
       64,
    /* sub_agent_id */
       0x17,
    /* kaps result */
       0xA711,
    /* uptime */
       0x12345678,
    /* sflow_encap_id */
       -1,
    /* buffer_size: in words (4 bytes)
     * Data: 128 bytes = 32 words 
     * Header record: 6 words 
     * Sample Data Header: 10 words 
     */
       48,
    /* time_thresh */
       2000,
    {
    /* is_egress_sflow */
       0,
    /* mirror_port */
       0,
    /* Sample rate dividend | divisor */
       1,              1,
   /* sflow_mirror_destination */
      -1
    }
};

/*
 * This function configures the ingress side of the sFlow, including the PMF selector and mirror
 * destination.
 */
int
cint_sflow_basic_ingress_create(int unit, int in_port, int sflow_out_port, bcm_gport_t sflow_destination)
{
    int rv;
    char *proc_name;

    proc_name = "cint_sflow_basic_ingress_create";
    /*
     * Create a mirror destination of type stat sampling, including ingress crop, and stat rate,
     * and modified ftmh header with sFlow extension.
     */
    rv = cint_sflow_utils_mirror_create(unit, sflow_out_port, sflow_destination,
            cint_sflow_basic_info.sample_rate_dividend, cint_sflow_basic_info.sample_rate_divisor,
            &cint_sflow_basic_info.sflow_mirror_destination, 0);

    printf("%s: cint_sflow_utils_mirror_create - returned mirror dest %d 0x%x\n", proc_name, cint_sflow_basic_info.sflow_mirror_destination, cint_sflow_basic_info.sflow_mirror_destination);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), cint_sflow_utils_mirror_create \n", proc_name, rv);
        return rv;
    }

    /*
     * Create a field entry for IPoETH
     */

    bcm_field_entry_t field_entry_handle;

    rv = cint_sflow_field_entry_add(unit, bcmFieldLayerTypeEth, bcmFieldLayerTypeIp4, TRUE, 
        cint_sflow_basic_info.egress_sflow.is_egress_sflow, in_port, 
        cint_sflow_basic_info.egress_sflow.egress_mirror_destination, cint_sflow_basic_info.sflow_mirror_destination,
                field_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in cint_sflow_field_entry_add\n", proc_name, rv);
        return rv;
    }

    return BCM_E_NONE;
}

bcm_field_context_t context_id = 0;   /* use default context 0*/
bcm_field_context_t egress_context_id = 1;   

/*
 * This function configures the ePMF --> Out-Bound-Mirror, including the PMF selector and mirror
 * destination.
 */

int cint_sflow_basic_egress_create(
    int unit,
    int route_out_port)
{
    int rv;
    bcm_mirror_destination_t mirror_dest;
    char *proc_name;

    proc_name = "cint_sflow_basic_egress_create";

    /*
     * Create the mirror destination.
     */
    bcm_mirror_destination_t_init(&mirror_dest);
    mirror_dest.flags = 0;
    mirror_dest.packet_copy_size = 0; 
    mirror_dest.sample_rate_dividend = cint_sflow_basic_info.egress_sflow.sample_rate_dividend; /* 0 = disable , 1 = enable */
    mirror_dest.sample_rate_divisor = cint_sflow_basic_info.egress_sflow.sample_rate_divisor;
    mirror_dest.gport = cint_sflow_basic_info.egress_sflow.egress_mirror_port;
    rv = bcm_mirror_destination_create(unit, &mirror_dest);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_mirror_destination_create\n", proc_name, rv);
        return rv;
    }

    cint_sflow_basic_info.egress_sflow.egress_mirror_destination = mirror_dest.mirror_dest_id;

	
    /*
     * Create a field entry for IPoETH
     */
    rv = cint_sflow_egress_field_entry_add(unit, route_out_port, cint_sflow_basic_info.egress_sflow.egress_mirror_destination);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in cint_sflow_egress_field_entry_add\n", proc_name, rv);
        return rv;
    }
	
    return rv;
}	

/*
 * This function will create the route from cint_basic_route.c, and will use it as a sample flow
 * to sample and send to sFlow.
 */
int
cint_sflow_basic_route_main(int unit, int in_port, int route_out_port, int sflow_out_port, int eventor_port, int eventor_id, int is_aggregated, int is_ITMH_forwarding)
{
    bcm_gport_t sflow_destination;
    int rv;
    int fec_id;
    char *proc_name;

    proc_name = "cint_sflow_basic_route_main";
    
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
    rv = dnx_basic_example(unit, in_port, route_out_port, cint_sflow_basic_info.kaps_result);

    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in dnx_basic_example\n", proc_name, rv);
        return rv;
    }

    /*
     * Configure the sFlow egress pipeline.
     */
    rv = cint_sflow_utils_egress_create(unit, in_port, sflow_out_port, &sflow_destination, eventor_id, is_aggregated);

    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in cint_sflow_utils_egress_create\n", proc_name, rv);
        return rv;
    }
    cint_sflow_basic_info.sflow_encap_id = sflow_destination;

    if (cint_sflow_basic_info.egress_sflow.is_egress_sflow) {
        rv = cint_sflow_basic_egress_create(unit, route_out_port);		
        if (rv != BCM_E_NONE)
        {
            printf("%s: Error (%d), in cint_sflow_basic_egress_create\n", proc_name, rv);
            return rv;
        }
    }
	
    /*
     * Configure the sFlow ingress pipeline
     */
    if (is_aggregated) {
       rv = cint_sflow_basic_ingress_create(unit, in_port, eventor_port, sflow_destination);
    }
    else {
       rv = cint_sflow_basic_ingress_create(unit, in_port, sflow_out_port, sflow_destination);
    }
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in cint_sflow_basic_ingress_create\n", proc_name, rv);
        return rv;
    }

    /*
     * For aggregated mode, use ITMH or IP forwarding for the SFLOW datagram
     */
    if (is_aggregated && (is_ITMH_forwarding == 0)) {

        /*
        * Set ip route eventor_port ==> sflow_out_port
        */
       rv = cint_sflow_utils_route_sflow_datagram(unit, eventor_port, sflow_out_port, fec_id +1);
       if (rv != BCM_E_NONE)
       {
           printf("%s: Error (%d), in cint_sflow_utils_route_sflow_datagram\n", proc_name, rv);
           return rv;
       }

      /*
      * Create a field entry for preventing snooping of the SFLOW DATAGRAM coming from the Eventor port
      */
      bcm_field_entry_t field_entry_handle;

      rv = cint_sflow_field_utils_entry_add_eventor_port(unit, eventor_port, cint_sflow_field_fg_id, &field_entry_handle, field_eventor_port_entry_priority);
      if (rv != BCM_E_NONE)
      {
         printf("%s: Error (%d), in cint_sflow_field_utils_entry_add_eventor_port\n", proc_name, rv);
         return rv;
      }

      /*
      * Configure SFLOW datagram SN update
      */
      rv = cint_sflow_utils_sflow_sn_config(unit, eventor_port, sflow_out_port, context_id);
      if (rv != BCM_E_NONE)
      {
         printf("%s: Error (%d), in cint_sflow_utils_sflow_sn_config\n", proc_name, rv);
         return rv;
      }

    }

    return BCM_E_NONE;
}

/*
 * Call this function before calling any other main functions. It will init the CRPS and the field
 * processor qualifiers.
 */
int
cint_sflow_basic_general_init(int unit, int sflow_out_port, int eventor_port, int eventor_id, int nof_samples, int is_aggregated, int is_ITMH_forwarding)
{
    int rv;
    char *proc_name;

    proc_name = "cint_sflow_basic_general_init";

    /*
     * Configure the CRPS for the sFlow.
     */
    if (is_aggregated) {
       rv = set_counter_resource(unit, eventor_port, cint_sflow_basic_info.counter_if, 0, &cint_sflow_basic_info.counter_base);
    } else {
    rv = set_counter_resource(unit, sflow_out_port, cint_sflow_basic_info.counter_if, 0, &cint_sflow_basic_info.counter_base);
    }
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in set_counter_resource\n", proc_name, rv);
        return rv;
    }

    printf("%s: counter (counter_if = %d, counter_base = %d) created.\n", proc_name, cint_sflow_basic_info.counter_if, cint_sflow_basic_info.counter_base);

    /*
     * Intialize the s flow field configuration, then map the flow's tuple to the mirror destination.
     */
    rv = cint_sflow_field_main(unit, cint_sflow_basic_info.egress_sflow.is_egress_sflow, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in cint_sflow_field_main\n", proc_name, rv);
        return rv;
    }

    /*
     * Set the global virtual register for sFlow.
     */
    rv = cint_sflow_utils_registers_init(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in cint_sflow_utils_registers_init\n", proc_name, rv);
        return rv;
    }

    if ($is_aggregated) {
       rv = cint_sflow_basic_eventor_set(unit, sflow_out_port, eventor_port, nof_samples, eventor_id, is_ITMH_forwarding);
       if (rv != BCM_E_NONE)
       {
           printf("%s: Error (%d), in cint_sflow_basic_eventor_set\n", proc_name, rv);
           return rv;
       }
    }
    return BCM_E_NONE;
}

/*
 * Call this function before calling any other main functions. It will init the field processor qualifiers.
 */
int
cint_sflow_basic_egress_general_init(int unit, int is_egress_sflow, int egress_mirror_port)
{
    int rv;
    char *proc_name;

    proc_name = "cint_sflow_basic_egress_general_init";

    cint_sflow_basic_info.egress_sflow.is_egress_sflow = is_egress_sflow;
    cint_sflow_basic_info.egress_sflow.egress_mirror_port = egress_mirror_port;

    if (is_egress_sflow) {
        rv = field_presel_fwd_layer_main(unit,40,bcmFieldStageEgress,bcmFieldLayerTypeIp4,&egress_context_id,"egress_mirror_ctx");
        if (rv != BCM_E_NONE)
        {
            printf("%s: Error (%d), in field_presel_fwd_layer_main\n", proc_name, rv);
            return rv;
        }
		
        /*
         * Map the out packet to the mirror destination.
         */
        rv = cint_sflow_egress_field_main(unit, egress_context_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s: Error (%d), in cint_sflow_egress_field_main\n", proc_name, rv);
            return rv;
        }
    }
	
    return BCM_E_NONE;
}

/*
 * Set eventor configuration
 */
int
sflow_basic_eventor_set_internal(int unit, int sflow_out_port, int eventor_port, int nof_samples, int eventor_id, int is_ITMH_forwarding, int reconfigure)
{
    int rv;
    char *proc_name;

    proc_name = "sflow_basic_eventor_set_internal";

   /*
   * Set eventor configuration.
   */
   if (is_ITMH_forwarding) {

      rv = cint_sflow_utils_eventor_itmh_set(unit, eventor_id, cint_sflow_basic_info.buffer_size, nof_samples, eventor_port, sflow_out_port);
      if (rv != BCM_E_NONE)
      {
         printf("%s: Error (%d), in cint_sflow_utils_eventor_itmh_set\n", proc_name, rv);
         return rv;
      }
   }
   else {
      rv = cint_sflow_utils_eventor_set(unit, eventor_id, cint_sflow_basic_info.buffer_size, nof_samples, eventor_port, sflow_out_port, reconfigure);
      if (rv != BCM_E_NONE)
      {
         printf("%s: Error (%d), in cint_sflow_utils_eventor_set\n", proc_name, rv);
         return rv;
      }
   }

   return BCM_E_NONE;
}

/*
 * Set eventor configuration
 */
int
cint_sflow_basic_eventor_set(int unit, int sflow_out_port, int eventor_port, int nof_samples, int eventor_id, int is_ITMH_forwarding)
{
    int rv;
    char *proc_name;

    proc_name = "cint_sflow_basic_eventor_set";

    printf("%s: sflow_out_port=%d, eventor_port = %d, nof_samples=%d, eventor_id=%d\n",
           proc_name, sflow_out_port, eventor_port, nof_samples, eventor_id);

   /*
   * Set eventor configuration.
   */
    rv = sflow_basic_eventor_set_internal(unit, sflow_out_port, eventor_port, nof_samples, eventor_id, is_ITMH_forwarding, 0);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in sflow_basic_eventor_set_internal\n", proc_name, rv);
       return rv;
    }

   return BCM_E_NONE;
}


/*
 * Reconfigure eventor
 *
 * Change Eventor NOF sampling:
 *   1. Disable sampling
 *   2. Wait Eventor Threshold time to send its data and its buffers are clear.
 *   3. Reconfigure Eventor
 *   4. Enable sampling
 */ 
int
cint_sflow_basic_eventor_reconfig(int unit, int sflow_out_port, int eventor_port, int nof_samples, int eventor_id, int is_ITMH_forwarding)
{
    int rv;
    char *proc_name;

    proc_name = "cint_sflow_basic_eventor_reconfig";

    printf("%s: sflow_out_port=%d, eventor_port = %d, nof_samples=%d, eventor_id=%d \n",
           proc_name, sflow_out_port, eventor_port, nof_samples, eventor_id);


   /*
   *   1. Disable sampling (by setting dividend to 0)
   *   2. Wait Eventor Threshold time to send its data and its buffers are clear.
   *   3. Reconfigure Eventor
   *   4. Enable sampling
   */
 
    rv = cint_sflow_utils_mirror_create(unit, eventor_port, cint_sflow_basic_info.sflow_encap_id,
            0, cint_sflow_basic_info.sample_rate_divisor,
            &cint_sflow_basic_info.sflow_mirror_destination, 1);

    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in cint_sflow_utils_mirror_create(dividend=0) \n", proc_name, rv);
       return rv;
    }

   /*
   *   2. Wait Eventor Threshold time to send its data and its buffers are clear.
   */
    sal_usleep(cint_sflow_basic_info.time_thresh);

   /*
   *   3. Reconfigure Eventor
   */     
    rv = sflow_basic_eventor_set_internal(unit, sflow_out_port, eventor_port, nof_samples, eventor_id, is_ITMH_forwarding, 1);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in sflow_basic_eventor_set_internal\n", proc_name, rv);
       return rv;
    }

   /*
   *   4. Enable sampling (setting dividend back to 1)
   */
    rv = cint_sflow_utils_mirror_create(unit, eventor_port, cint_sflow_basic_info.sflow_encap_id,
            cint_sflow_basic_info.sample_rate_dividend, cint_sflow_basic_info.sample_rate_divisor,
            &cint_sflow_basic_info.sflow_mirror_destination, 1);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in cint_sflow_utils_mirror_create \n", proc_name, rv);
       return rv;
    }

   return BCM_E_NONE;
}
