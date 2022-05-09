/* $Id: cint_sflow_utils.c
*/

/**
 *
 * This cint contains utility functions helping to configure
 * sFlow agent. 
 *
 * How to run: 
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_utility.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_field.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_field_advanced.c 
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_field_extended_gateway.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_basic.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_advanced.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_extended_gateway.c
 *  
 * See 
 * cint_sflow_basic.c/cint_sflow_advanced.c/cint_sflow_extended_gateway.c
 * for usage. 
 *
 */


struct cint_sflow_utils_info_s {

   int sn_update_already_called;       /* Prevent reconfiguration of SFLOW datagram SN update */
   int route_datagram_already_called;  /* Prevent reconfiguration of route setup for the SFLOW datagram packet */
   int counter_value_before_clear;     /* The SN counter value before reset, see cint_sflow_utils_sflow_sn_reset */
   bcm_gport_t trunk_gport;            /* The trunk GPORT created by cint_sflow_utils_lag_create*/

   int disable_eventor_config;         /* For running under adapter, disable eventor configuration (which is not supported in simulation)*/
};

cint_sflow_utils_info_s cint_sflow_utils_info = {

   /* sn_update_already_called */
   0,
   /* route_datagram_already_called */
   0,
   /* counter_value_before_clear */
   -1,
   /* trunk_gport */
   -1,
   /* disable_eventor_config */
   0
};

/*
 * This function updates the uptime virtual register. It should be called every millisecond to update the timestamp
 * for sFlow packets.
 */
int
cint_sflow_utils_timestamp_update(int unit, uint32 time_msec)
{
    int rv;

    rv = bcm_instru_control_set(unit, 0, bcmInstruControlSFlowUpTime, time_msec);
    if (rv != BCM_E_NONE)
    {
        printf("cint_sflow_utils_timestamp_update: Error (%d), bcm_instru_control_set(type = %d, value = %d)\n", rv, bcmInstruControlSFlowUpTime, time_msec);
        return rv;
    }

    return rv;
}

/*
 * This function creates an ethernet rif + arp + udp tunnel, that the sflow packet will be sent to.
 */
int
cint_sflow_utils_udp_tunnel_create(int unit, bcm_gport_t *udp_tunnel)
{
    int rv;
    bcm_l3_intf_t l3_intf;
    bcm_if_t arp_itf;
    char *proc_name;

    proc_name = "cint_sflow_utils_udp_tunnel_create";

    /*
     * Create rif for the arp.
     */
    rv = intf_eth_rif_create(unit, cint_sflow_basic_info.out_rif,
            cint_sflow_basic_info.intf_access_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in intf_eth_rif_create\n", proc_name, rv);
        return rv;
    }


    /*
     * Create arp with AC for the udp tunnel. 
     * Note: 
     * SFLOW does two ESEM access to resolve SFLOW input and output interface field.
     * EEDB AC entry must be created to avoid port ESEM access which overrides the above ESEM access.
     */
    rv = create_arp_with_next_ac_type_vlan_translate_none(unit, cint_sflow_basic_info.tunnel_next_hop_mac, &arp_itf, cint_sflow_basic_info.collector_vlan);

    if (rv != BCM_E_NONE){

       printf("%s: Error (%d), in create_arp_with_next_ac_type\n", proc_name, rv);
       return rv;
    }


    /* Create IP tunnel initiator for encapsulating UDPoIPv4 tunnel header*/
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t_init(&l3_intf);
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip        = cint_sflow_basic_info.tunnel_dip;
    tunnel_init.sip        = cint_sflow_basic_info.tunnel_sip;
    tunnel_init.flags      = 0;
    tunnel_init.type       = cint_sflow_basic_info.tunnel_type;
    tunnel_init.udp_dst_port = cint_sflow_basic_info.udp_dst_port;
    tunnel_init.udp_src_port = cint_sflow_basic_info.udp_src_port;
    tunnel_init.ttl        = cint_sflow_basic_info.tunnel_ttl;
    tunnel_init.l3_intf_id = arp_itf;
    tunnel_init.encap_access = bcmEncapAccessTunnel4; /* Use tunnel 4 to allow access from sFlow encap
                                                                    in encap access 1+2*/

    rv = bcm_tunnel_initiator_create(unit,&l3_intf, tunnel_init);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_tunnel_initiator_create\n", proc_name, rv);
        return rv;
    }

    *udp_tunnel = tunnel_init.tunnel_id;

    return rv;
}


/*
 * This function creates/replace a mirror destination with a sample rate of dividend / divisor, sets it to
 * crop the packet and add an sFlow systems header, and sets its destination to the sFlow encap id + out_port.
 * Note: 
 * For create - mirror_dest_id is returned. 
 * For replace - mirror_dest_id must contains the mirror encap id. 
 * The user can disable / enable a created mirror by performing replace and set the sample_rate_dividend to 0 /1.
 */
int
cint_sflow_utils_mirror_create(int unit,
                    int out_port,
                    bcm_gport_t sflow_encap_id,
                    int sample_rate_dividend,
                    int sample_rate_divisor,
                    int *mirror_dest_id,
					int is_replace)
{
    int rv;
    bcm_mirror_destination_t mirror_dest;
    char *proc_name;
    uint32 estimate_encap_size_required =
    		*dnxc_data_get(unit, "lif", "out_lif", "global_lif_allocation_estimated_encap_size_enable", NULL);

    proc_name = "cint_sflow_utils_mirror_create";

    /*
     * Create the mirror destination.
     */
    bcm_mirror_destination_t_init(&mirror_dest);

    if (is_replace == 0) {
       /* It is create: */
       mirror_dest.flags = BCM_MIRROR_DEST_IS_STAT_SAMPLE;
    } else {
       /* It is replace: */
       mirror_dest.flags = BCM_MIRROR_DEST_IS_STAT_SAMPLE | BCM_MIRROR_DEST_WITH_ID | BCM_MIRROR_DEST_REPLACE;
       mirror_dest.mirror_dest_id = *mirror_dest_id;
    }

    mirror_dest.packet_copy_size = 256; /* Fixed value. */
    mirror_dest.sample_rate_dividend = sample_rate_dividend; /* 0 = disable , 1 = enable */
    mirror_dest.sample_rate_divisor = sample_rate_divisor;
    mirror_dest.gport = out_port;

    
    if (*dnxc_data_get(unit, "instru", "sflow", "hard_logic_supported", NULL))
    {
        /* For SFLOW hard-logic, need to add FHEI extension to system headers */
        mirror_dest.packet_control_updates.valid |= BCM_MIRROR_PKT_HEADER_UPDATE_FABRIC_HEADER_EDITING;

        printf("%s: mirror_dest.packet_control_updates.valid = 0x%08X\n", proc_name, mirror_dest.packet_control_updates.valid);

        /* 
         *  When using SFLOW hard-logic, corp the data to exact size!
         *  It should be exact data size of 128 bytes.
         *  In addition, pad data to corp size if it is samller
         */
        mirror_dest.packet_copy_size = 128; /* Fixed value. */
        mirror_dest.flags2 |= BCM_MIRROR_DEST_FLAGS2_PAD_TO_CROP_SIZE_ENABLE;
    }

    rv = bcm_mirror_destination_create(unit, &mirror_dest);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_mirror_destination_create\n", proc_name, rv);
        return rv;
    }

    printf("%s: bcm_mirror_destination_create(flags = 0x%08X, mirror_dest_id = 0x%08X)\n", proc_name, mirror_dest.flags, mirror_dest.mirror_dest_id);

    /*
     * Modify FTMH header on mirror destination to the sFlow channel and sFlow systems header.
     * Ase extension is used to pass sFlow data.
     */
    bcm_mirror_header_info_t mirror_header_info;
    bcm_mirror_header_info_t_init(&mirror_header_info);
    mirror_header_info.tm.ase_ext.valid = TRUE;
    mirror_header_info.tm.ase_ext.ase_type = bcmPktDnxAseTypeSFlow;
    mirror_header_info.tm.flow_id_ext.valid = TRUE;
    mirror_header_info.tm.flow_id_ext.flow_id = 0;
    mirror_header_info.tm.flow_id_ext.flow_profile = 0;

    /* We need to encapsulate the sflow encap id with tunnel so the mirror API will recognize it */
    BCM_GPORT_TUNNEL_ID_SET(mirror_header_info.tm.out_vport, sflow_encap_id);

    /** add indication on UDH base that this is sflow packet. */
    /** It will be used to estimate the encap size by the epmf. See function cint_sflow_field_egress_estimate_encap_size */
    /** the indication will be UDH_Base0=0x3, UDH_base1=0x2 */
    if (estimate_encap_size_required)
    {
        mirror_header_info.udh[0].size = 0x3;
        mirror_header_info.udh[0].data = 0;
        mirror_header_info.udh[1].size = 0x2;
        mirror_header_info.udh[1].data = 0;
    }

    /* keep original FTMH. It's used to retrieve original source port and destination port */
    rv = bcm_mirror_header_info_set(unit, BCM_MIRROR_DEST_IS_STAT_SAMPLE | BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER, mirror_dest.mirror_dest_id, &mirror_header_info);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in bcm_mirror_header_info_set\n", proc_name, rv);
       return rv;
    }

    printf("%s: bcm_mirror_header_info_set(mirror_dest_id = 0x%08X, sflow_encap_id = 0x%08X, out_vport = 0x%08X)\n", proc_name, mirror_dest.mirror_dest_id, sflow_encap_id, mirror_header_info.tm.out_vport);

    *mirror_dest_id = mirror_dest.mirror_dest_id;
    return rv;
}

/*
 * This function creates the egress pipe configuration for the sFlow - it configures a crps counter,
 * and configures the sFlow encap.
 * in case of DP: configures udp tunnel to which the sflow will be sent.
 */
int
cint_sflow_utils_egress_create(int unit, int in_port, int out_port, bcm_gport_t *sflow_destination, int eventor_id, int is_aggregated)
{
    int rv;
    bcm_gport_t udp_tunnel_id;
    char *proc_name;

    proc_name = "cint_sflow_utils_egress_create";


    printf("%s: in_port = %d, out_port= %d, eventor_id = %d, is_aggregated = %d\n",
           proc_name, in_port, out_port, eventor_id, is_aggregated);

    if (!is_aggregated)
    {
       if (cint_sflow_basic_info.udp_tunnel_id == -1) {
           /*
            * Create the sFlow collector UDP tunnel outlif.
            */
           rv = cint_sflow_utils_udp_tunnel_create(unit, &udp_tunnel_id);

           if (rv != BCM_E_NONE)
           {
               printf("%s: Error (%d), in cint_sflow_utils_udp_tunnel_create\n", proc_name, rv);
               return rv;
           }

           cint_sflow_basic_info.udp_tunnel_id = udp_tunnel_id;
       }
       else 
       {
          udp_tunnel_id = cint_sflow_basic_info.udp_tunnel_id;
       }
    }
    else
    {
       /*
       * UDP tunnel is irrelevant when using eventor
       */
       udp_tunnel_id = 0;
    }

    /*
     * Create the sFlow ETPS entry. UDP tunnel and counter base as input.
     */
    bcm_instru_sflow_encap_info_t sflow_encap_info;
    sal_memset(&sflow_encap_info, 0, sizeof(sflow_encap_info));

    sflow_encap_info.counter_command_id = cint_sflow_basic_info.counter_if;
    sflow_encap_info.stat_cmd = cint_sflow_basic_info.counter_base;
    sflow_encap_info.tunnel_id = udp_tunnel_id;
    sflow_encap_info.sub_agent_id = cint_sflow_basic_info.sub_agent_id;
    sflow_encap_info.eventor_id= eventor_id;

    if (is_aggregated)
    {
        sflow_encap_info.flags |= BCM_INSTRU_SFLOW_ENCAP_AGGREGATED;
    }

    rv = bcm_instru_sflow_encap_create(unit, &sflow_encap_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_instru_sflow_encap_create\n", proc_name, rv);
        return rv;
    }

    *sflow_destination = sflow_encap_info.sflow_encap_id;
    printf("%s: Sflow outlif = 0x%x\n", proc_name, sflow_encap_info.sflow_encap_id);

    rv = cint_sflow_field_egress_estimate_encap_size(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in cint_sflow_field_egress_estimate_encap_size\n", proc_name, rv);
        return rv;
    }
    return BCM_E_NONE;
}

/*
* Functions handling the configuration of SFLOW datagram SN update. 
* It is only valid on SFLOW datagram IP forwarding (ITMH mode is not supported) 
*/
int
cint_sflow_utils_sflow_sn_config(int unit, int eventor_port, int sflow_out_port, int context_id)
{
   int rv = BCM_E_NONE;
   char *proc_name;

   proc_name = "cint_sflow_utils_sflow_sn_update";

   if (cint_sflow_utils_info.sn_update_already_called)
   {
      printf("%s: SKIP setup! sn_update_already_called = %d\n", proc_name, cint_sflow_utils_info.sn_update_already_called);

      cint_sflow_utils_info.sn_update_already_called++;
      return BCM_E_NONE;
   }
   cint_sflow_utils_info.sn_update_already_called++;

   /** Create SFLOW SN qualifiers */
   rv = cint_sflow_field_utils_sn_update_quals_set(unit, eventor_port, context_id);
   if (rv != BCM_E_NONE)
   {
      printf("%s: Error (%d), in cint_sflow_field_sn_update_quals_set\n", proc_name, rv);
      return rv;
   }
   
   /** Add SFLOW SN entry */
   rv = cint_sflow_field_utils_sn_update_entry_add(unit, eventor_port);
   if (rv != BCM_E_NONE)
   {
      printf("%s: Error (%d), in cint_sflow_field_sn_update_entry_add\n", proc_name, rv);
      return rv;
   }
   return rv; 
}


/*
* Functions handling SFLOW datagram SN reset. 
* Note: 
* The below implementation is based on the counter allocated by set_counter_resource. 
* cint_sflow_utils_sflow_sn_config must be called for configuration of SFLOW datagram SN update. 
*/
int
cint_sflow_utils_sflow_sn_reset(int unit, int eventor_port)
{
   int core_id, tm_port;

   bcm_stat_counter_database_t database;
   bcm_stat_eviction_boundaries_t boundaries;
   int database_id;
   bcm_stat_counter_enable_t enable_config;
   bcm_stat_counter_input_data_t input_data;
   bcm_stat_counter_output_data_t output_data;
   int stat_array[64];
   uint64 counter_value_64b;
   const uint32 *max_counter_set_size = dnxc_data_get(unit, "crps", "engine", "max_counter_set_size", NULL);
   bcm_stat_counter_value_t counter_value[(*max_counter_set_size) * 2];

   int rv = BCM_E_NONE;
   char *proc_name;

   proc_name = "cint_sflow_utils_sflow_sn_reset";

   /* Procedure for reset a counter: 
    * 1. Set eviction boundaries to disable eviction on active 
    * counter. 
    *  
    * 2. Enable eviction on the engine. Due to step 1 eviction will 
    * only be enabled on the inactive counter. 
    *  
    * 3. Wait. This is required for eviction to occur (SW counters 
    * get the HW counters) 
    *  
    * 4. Read the inactive counter 
    *  
    * 5. Disable eviction globally 
    */
   rv = get_core_and_tm_port_from_port(unit, eventor_port, &core_id, &tm_port);
   if (rv != BCM_E_NONE) {
      printf("%s: Error (%d), in get_core_and_tm_port_from_port\n", proc_name, rv);
      return rv;
   }

   bcm_stat_eviction_boundaries_t_init(&boundaries);
   bcm_stat_counter_database_t_init(&database);
   bcm_stat_counter_enable_t_init(&enable_config);

   /*
   * See set_counter_resource for database_id calculation
   */
   database_id = core_id*6 + cint_sflow_basic_info.counter_if;
   database.database_id = database_id;
   database.core_id = core_id;
   
   /* 
    * Set boundaries for specific counter 
    */
   boundaries.start = cint_sflow_basic_info.sn_counter_stat_id;
   boundaries.end = cint_sflow_basic_info.sn_counter_stat_id;
   rv = bcm_stat_database_eviction_boundaries_set(unit, 0, &database, 0, &boundaries);
   if (rv != BCM_E_NONE) {
      printf("%s: Error (%d), in bcm_stat_database_eviction_boundaries_set\n", proc_name, rv);
      return rv;
   }

   /* 
    * Enable eviction on the engine.
    * Eviction will only take place on the specific counter. 
    */ 
   enable_config.enable_counting = TRUE; /* Set to true in any case*/
   enable_config.enable_eviction = TRUE; 
   rv = bcm_stat_counter_database_enable_set(unit, 0, &database, &enable_config);
   if (rv != BCM_E_NONE) {
      printf("%s: Error (%d), in bcm_stat_counter_database_enable_set\n", proc_name, rv);
      return rv;
   }

   printf("%s: database_id = %d, core_id = %d, eviction boundaries (start = %d, end = %d) set and enabled eviction. Going to sleep to allow reading the counters.\n", 
          proc_name, database.database_id, database.core_id, boundaries.start, boundaries.end);

   sal_sleep(2); /* Wait for eviction to take place*/

   /*
    * Read (and clear) the inactive counter
    */
   bcm_stat_counter_input_data_t_init(&input_data);
   bcm_stat_counter_output_data_t_init(&output_data);

   stat_array[0] = BCM_STAT_COUNTER_STAT_SET(0,0);
   input_data.core_id = core_id;
   input_data.database_id = database_id;
   input_data.type_id = 0;
   input_data.counter_source_id = cint_sflow_basic_info.sn_counter_stat_id;
   input_data.nstat = 1;
   input_data.stat_arr = stat_array;

   output_data.value_arr = counter_value;

   rv = bcm_stat_counter_get(unit, BCM_STAT_COUNTER_CLEAR_ON_READ, &input_data, &output_data);
   if (rv != BCM_E_NONE) {
      printf("%s: Error (%d), in bcm_stat_counter_get\n", proc_name, rv);
      return rv;
   }

   counter_value_64b = output_data.value_arr[0].value;
   COMPILER_64_TO_32_LO(cint_sflow_utils_info.counter_value_before_clear,counter_value_64b);

   printf("%s: counter_value_before_clear = %d \n", proc_name, cint_sflow_utils_info.counter_value_before_clear);

   /* 
    * Now disable eviction back.
    */
   database.database_id = database_id;
   database.core_id = core_id;
   enable_config.enable_counting = TRUE; /* Set to true in any case*/
   enable_config.enable_eviction = FALSE; 
   rv = bcm_stat_counter_database_enable_set(unit, 0, &database, &enable_config);
   if (rv != BCM_E_NONE) {
      printf("%s: Error (%d), in bcm_stat_counter_database_enable_set(\n", proc_name, rv);
      return rv;
   }

   return rv; 
}


/*
* This function create route setup for the SFLOW datagram packet. 
*/
int
cint_sflow_utils_route_sflow_datagram(int unit, int in_port, int sflow_out_port, int kaps_result)
{
   int rv = BCM_E_NONE;
   char *proc_name;

   int fec = kaps_result;
   int vrf = 20;
   int intf_in = 250;           /* Incoming packet ETH-RIF */
   int intf_out = 200;          /* Outgoing packet ETH-RIF */
   int vlan = 200;

   bcm_mac_t intf_in_mac_address =  { 0x00, 0x0e, 0x00, 0x02, 0x00, 0x01 };   /* my-MAC */
   bcm_mac_t intf_out_mac_address = { 0x00, 0x0e, 0x00, 0x02, 0x00, 0x02 };   
   bcm_mac_t arp_next_hop_mac =     { 0x00, 0x0e, 0x00, 0x02, 0x00, 0x03 };   /* next_hop_mac */

    
   bcm_vlan_port_t vlan_port;
   bcm_port_match_info_t match_info;

   bcm_l3_intf_t l3if;
   bcm_l3_ingress_t l3_ing_if;
   bcm_l3_egress_t l3eg;
   bcm_if_t l3egid_null;      /* not intersting */
   int arp_encap_id = 8195;   /* ARP-Link-layer (needs to be higher than 8192 for Jer Plus) */
   bcm_gport_t gport;
   bcm_if_t encoded_fec;

   bcm_l3_host_t host_info;
   int l3_itf;
   bcm_l3_route_t l3rt;

   uint32 ipv4_host  = 0xA1000011; /* 161.0.0.17*/
   uint32 ipv4_route = 0xA1000000; /* 161.0.0.xx*/
   uint32 ipv4_mask  = 0xffffff00;

   bcm_ip6_t ipv6_host = { 0x20, 0x01, 0x0D, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x22, 0xFF, 0xFE, 0x33, 0x44, 0x55};
   bcm_ip6_t ipv6_route ={ 0x20, 0x01, 0x0D, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x22, 0xFF, 0xFE, 0x33, 0x44, 0x00};
   bcm_ip6_t ipv6_mask = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};

   proc_name = "cint_sflow_utils_route_sflow_datagram";

   if (cint_sflow_utils_info.route_datagram_already_called)
   {
      printf("%s: SKIP setup! route_datagram_already_called = %d\n", proc_name, cint_sflow_utils_info.route_datagram_already_called);

      cint_sflow_utils_info.route_datagram_already_called++;
      return BCM_E_NONE;
   }
   cint_sflow_utils_info.route_datagram_already_called++;

   /*
   * 1. Set in-port to ETH-RIF 
   */
   bcm_vlan_port_t_init(&vlan_port);
   vlan_port.port = in_port;
   vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
   vlan_port.vsi = intf_in; /* ETH-RIF */
   vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;

   rv = bcm_vlan_port_create(unit, vlan_port);
   if (rv != BCM_E_NONE)
   {
      printf("%s: Error (%d), bcm_vlan_port_create ETH-RIF\n", proc_name, rv);
      return rv;
   }

   printf("1. %s: port = %d, vsi = %d, vlan_port_id = 0x%08X\n", proc_name, vlan_port.port, vlan_port.vsi, vlan_port.vlan_port_id);

   /*
   * 2. Set out-port to default AC LIF
   */
   bcm_vlan_port_t_init(&vlan_port);

   vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
   vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_DEFAULT | BCM_VLAN_PORT_VLAN_TRANSLATION;
   rv = bcm_vlan_port_create(unit, &vlan_port);
   if (rv != BCM_E_NONE)
   {
      printf("%s: Error (%d), bcm_vlan_port_create OUT-RIF\n",proc_name, rv);
      return rv;
   }

   bcm_port_match_info_t_init(&match_info);
   match_info.match = BCM_PORT_MATCH_PORT;
   match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY;
   match_info.port = sflow_out_port;

   rv = bcm_port_match_add(unit, vlan_port.vlan_port_id, &match_info);
   if (rv != BCM_E_NONE)
   {
      printf("%s: Error (%d), bcm_port_match_add\n", proc_name, rv);
      return rv;
   }

   printf("2. %s: port = %d, vlan_port_id = 0x%08X\n", proc_name, sflow_out_port, vlan_port.vlan_port_id);

   /*
   * 3. Create ETH-RIF/OUT-RIF and set its properties
   */
   bcm_l3_intf_t_init(&l3if);
   l3if.l3a_flags = BCM_L3_WITH_ID;
   l3if.l3a_mac_addr = intf_in_mac_address;
   l3if.l3a_intf_id = intf_in;
   l3if.l3a_vid = intf_in;
   /* set qos map id to 0 as default */
   l3if.dscp_qos.qos_map_id = 0;

   rv = bcm_l3_intf_create(unit, &l3if);
   if (rv != BCM_E_NONE)
   {
      printf("%s: Error (%d), bcm_l3_intf_create ETH-RIF \n", proc_name, rv);
      return rv;
   }

   bcm_l3_intf_t_init(&l3if);
   l3if.l3a_flags = BCM_L3_WITH_ID;
   l3if.l3a_mac_addr = intf_out_mac_address;
   l3if.l3a_intf_id = intf_out;
   l3if.l3a_vid = intf_out;
   /* set qos map id to 0 as default */
   l3if.dscp_qos.qos_map_id = 0;

   rv = bcm_l3_intf_create(unit, &l3if);
   if (rv != BCM_E_NONE)
   {
      printf("%s: Error (%d), bcm_l3_intf_create OUT-RIF\n", proc_name, rv);
      return rv;
   }

   /*
   * 4. Set Incoming ETH-RIF properties
   */
   bcm_l3_ingress_t_init(&l3_ing_if);
   l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID;      /* must, as we update exist RIF */
   l3_ing_if.vrf = vrf;
   l3_ing_if.qos_map_id = 0;

   rv = bcm_l3_ingress_create(unit, &l3_ing_if, intf_in);
   if (rv != BCM_E_NONE)
   {
      printf("%s: Error (%d), bcm_l3_ingress_create\n", proc_name, rv);
      return rv;
   }
 
   /*
   * 5. Create ARP and set its properties
   */
   bcm_l3_egress_t_init(&l3eg);

   l3eg.mac_addr = arp_next_hop_mac;
   l3eg.encap_id = arp_encap_id;
   l3eg.vlan = vlan;
   l3eg.flags = 0;

   rv = bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &l3egid_null);
   if (rv != BCM_E_NONE)
   {
     printf("%s: Error (%d), bcm_l3_egress_create \n", proc_name);
     return rv;
   }
 
   arp_encap_id = l3eg.encap_id;

   printf("5. %s: arp_encap_id = 0x%08X\n", proc_name, arp_encap_id);


   /*
   * 6. Create FEC and set its properties
   */
   if (BCM_GPORT_IS_SET(sflow_out_port)) {
      gport = sflow_out_port;
   }
   else {
      BCM_GPORT_LOCAL_SET(gport, sflow_out_port);
   }
 
   bcm_l3_egress_t_init(&l3eg);
   l3eg.intf = intf_out;
   l3eg.encap_id = arp_encap_id;
   l3eg.failover_id = 0;
   l3eg.port = gport;
   l3eg.flags = 0;
   encoded_fec = fec;

   if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
   {
       l3eg.flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
   }

   rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &l3eg, &encoded_fec);
   if (rv != BCM_E_NONE)
   {
      printf("%s: Error (%d), bcm_l3_egress_create \n", proc_name, rv);
      return rv;
   }

    printf("6: %s: fec = 0x%08X, encoded_fec = 0x%08X \n",proc_name, fec, encoded_fec);

   /*
   * 7. Add Route entry
   */
    bcm_l3_route_t_init(l3rt);
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = fec;

    if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        l3rt.l3a_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    if (cint_sflow_basic_info.tunnel_type == bcmTunnelTypeUdp)
    {
       l3rt.l3a_subnet = ipv4_route;
       l3rt.l3a_ip_mask = ipv4_mask;
    }
    else if (cint_sflow_basic_info.tunnel_type == bcmTunnelTypeUdp6)
    {
       l3rt.l3a_ip6_net = ipv6_route;
       l3rt.l3a_ip6_mask = ipv6_mask;
       l3rt.l3a_flags = BCM_L3_IP6;
    }
    else
    {
       printf("%s: tunnel_type = %d is not supported! Only support bcmTunnelTypeUdp (%d) and bcmTunnelTypeUdp6 (%d) \n", 
              proc_name, cint_sflow_basic_info.tunnel_type, bcmTunnelTypeUdp, bcmTunnelTypeUdp6);
       return BCM_E_PARAM;
    }

    rv = bcm_l3_route_add(unit, &l3rt);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), bcm_l3_route_add \n", proc_name, rv);
       return rv;
    }

   /*
   * 8. Add host entry
   */
    BCM_L3_ITF_SET(&l3_itf, BCM_L3_ITF_TYPE_FEC, fec);

    if (!*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL))
    {
        if (cint_sflow_basic_info.tunnel_type == bcmTunnelTypeUdp)
        {
            rv = add_route_ipv4(unit, ipv4_host, 0xffffffff, vrf, l3_itf);
            if (rv != BCM_E_NONE)
            {
                printf("%s: Error (%d), add_route_ipv4 \n", proc_name, rv);
                return rv;
            }
        }
        else if (cint_sflow_basic_info.tunnel_type == bcmTunnelTypeUdp6)
        {
            rv = add_route_ipv6(unit, ipv6_host, 0xffffffff, vrf, l3_itf);
            if (rv != BCM_E_NONE)
            {
                printf("%s: Error (%d), add_route_ipv6 \n", proc_name, rv);
                return rv;
            }
        }
        else
        {
            printf("%s: tunnel_type = %d is not supported! Only support bcmTunnelTypeUdp (%d) and bcmTunnelTypeUdp6 (%d) \n", 
                  proc_name, cint_sflow_basic_info.tunnel_type, bcmTunnelTypeUdp, bcmTunnelTypeUdp6);
            return BCM_E_PARAM;
        }       
    }
    else
    {
       bcm_l3_host_t_init(&host_info);
       host_info.l3a_vrf = vrf;
       host_info.l3a_intf = l3_itf;

       if (cint_sflow_basic_info.tunnel_type == bcmTunnelTypeUdp)
       {
          host_info.l3a_ip_addr = ipv4_host;
       }
       else if (cint_sflow_basic_info.tunnel_type == bcmTunnelTypeUdp6)
       {
          host_info.l3a_ip6_addr = ipv6_host;
          host_info.l3a_flags = BCM_L3_IP6;
       }
       else
       {
          printf("%s: tunnel_type = %d is not supported! Only support bcmTunnelTypeUdp (%d) and bcmTunnelTypeUdp6 (%d) \n", 
                 proc_name, cint_sflow_basic_info.tunnel_type, bcmTunnelTypeUdp, bcmTunnelTypeUdp6);
          return BCM_E_PARAM;
       }

       rv = bcm_l3_host_add(unit, &host_info);
       if (rv != BCM_E_NONE)
       {
         printf("%s: Error (%d), bcm_l3_host_add failed: %x \n",proc_name, rv);
         return rv;
       }
    }

   return rv;
}

/*
 * This functions configures the eventor for Sflow - samples size, number of samples per sflow datagram etc.
 * Note:
 * For simplicity - this function sets Eventor RX double buffer to be the pair 
 * eventor_id and eventor_id+1 (modulo 8).
 * Thus, when using multi channel configuration, each channel actually occupies two eventor IDs thus each channel's 
 * eventor_id is +2 from previous channel's eventor_id.
 */
int
cint_sflow_utils_eventor_set(int unit, int eventor_id, int buffer_size, int nof_samples, int eventor_port, int sflow_out_port, int reconfigure)
{
    int rv;
    uint32 flags = 0;
    int buffer_size_thr = buffer_size * nof_samples;
    int context;
    int builder;
    uint32 header_length;
    uint32 udp_header_offset;
    uint32 ip_length_offset;
    bcm_instru_eventor_context_conf_t context_conf = {0};
    bcm_instru_eventor_builder_conf_t builder_conf = {0};
    uint8 * header_data;
    bcm_port_interface_info_t int_inf;
    bcm_port_mapping_info_t mapping_info;
    char *proc_name;

    proc_name = "cint_sflow_utils_eventor_set";

    if (cint_sflow_utils_info.disable_eventor_config == 1) {
       printf("%s: Eventor configuration is skipped!!!\n", proc_name);

       return BCM_E_NONE;
    }

    /* TX header data (SFLOW_DATAGRAMoUDPoIPv4oETH0oPTCH2)*/
   uint8 header_data_udp_tunnel[128] = {
            /* PTCH2 - 2 bytes*/
            0x00, 0x00,
            /* ETH0 -  6+6 bytes*/
            0x00, 0x0e, 0x00, 0x02, 0x00, 0x01, /* DMAC should be myMac so IP forwarding will be applied */
            0x00, 0x0e, 0x00, 0x02, 0x00, 0x00, /* SMAC */
            /* Ether type - 2 bytes*/
            0x08, 0x00,
            /* IPV4 header - 4 x 5 bytes */
            0x45, 0x00, 0x06, 0x38,
            0x00, 0x00, 0x00, 0x00,
            0x40, 0x11, 0x33, 0x93,
            0xA0, 0x00, 0x00, 0x11, /* SIP = 160.0.0.17 */
            0xA1, 0x00, 0x00, 0x11, /* DIP = 161.0.0.17 */
            /* UDP header - 2 x 4 bytes */
            0x17, 0x70, 0x13, 0x88, 
            0x00, 0x59, 0x00, 0x00,
            /* SFLOW DATAGRAM header - 7 x 4 bytes*/
            0x00, 0x00, 0x00, 0x05,
            0x00, 0x00, 0x00, 0x01,
            0xA0, 0x00, 0x00, 0x11,
            0x00, 0x00, 0x00, 0x17,
            0x00, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 
            /* Dont' care*/
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00
   };

    /* TX header data (SFLOW_DATAGRAMoUDPoIPv6oETH0oPTCH2)*/
    uint8 header_data_udp6_tunnel[128] = {
            /* PTCH2 - 2 bytes*/
            0x00, 0x00,
            /* ETH1 - 6+6 bytes*/
            0x00, 0x0e, 0x00, 0x02, 0x00, 0x01, /* DMAC should be myMac so IP forwarding will be applied */
            0x00, 0x0e, 0x00, 0x02, 0x00, 0x00, /* SMAC */
            /* Ether type - 2 bytes*/
            0x86, 0xDD,
            /* IPV6 header  10 x 4 bytes */
            0x60, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x11, 0x40,
            0xEC, 0xEF, 0xEE, 0xED, /* Source Address*/
            0xEC, 0xEB, 0xEA, 0xE9,
            0xE8, 0xE7, 0xE6, 0xE5,
            0xE4, 0xE3, 0xE2, 0xE1,
            0x20, 0x01, 0x0D, 0xB8, /* Destination Address */
            0x00, 0x00, 0x00, 0x00,
            0x02, 0x11, 0x22, 0xFF,
            0xFE, 0x33, 0x44, 0x55,
            /* UDP header - 2 x 4 bytes */
            0x17, 0x70, 0x13, 0x88,
            0x00, 0x59, 0x00, 0x00,
            /* SFLOW DATAGRAM header - 7 x 4 bytes*/
            0x00, 0x00, 0x00, 0x05,
            0x00, 0x00, 0x00, 0x01,
            0xA0, 0x00, 0x00, 0x11, 
            0x00, 0x00, 0x00, 0x17,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            /* Dont' care*/
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00
    };

    /* Get Eventor PP port */
    rv = bcm_port_get(unit, eventor_port, &flags, &int_inf, &mapping_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_port_get\n", proc_name, rv);
        return rv;
    }
    flags = 0;

    printf("%s: eventor_id = %d, eventor_port = %d, eventor_pp_port = %d, buffer_size = %d, nof_samples = %d, buffer_size_thr = %d, reconfigure = %d \n", proc_name, eventor_id, eventor_port, mapping_info.pp_port, buffer_size, nof_samples, buffer_size_thr, reconfigure);

    /* in PTCH2, In_PP_Port is 10 bits [9:0]*/
    uint8 eventor_pp_port_high = ((mapping_info.pp_port) >> 8) & 0x3;
    uint8 eventor_pp_port_low = (mapping_info.pp_port) & 0xFF;

    if (cint_sflow_basic_info.tunnel_type == bcmTunnelTypeUdp)
    {
        /* Set TX header size and data (SFLOW_DATAGRAMoUDPoIPv4oETH0oPTCH2)*/
        header_data = header_data_udp_tunnel;
        /*
        * PTCH2 = 2 bytes
        * ETH0 = 6+6 bytes
        * EtherType = 2 bytes
        * IPv4 header = 5x4 bytes
        * UDP header = 2x4 bytes 
        * SFLOW header = 4x4 (the header is 7 words, but only 4 words are static, sequence number + time stamp + nof samples are not part of the static header) 
        */
        header_length = 2 + (6+6) + 2 + (5*4) + (2*4) + (4*4);
        udp_header_offset = 2 + (6+6) + 2 + (5*4);
        ip_length_offset = 2 + (6+6) + 2 + 2;

    }
    else if (cint_sflow_basic_info.tunnel_type == bcmTunnelTypeUdp6)
    {
        /* Set TX header size and data (SFLOW_DATAGRAMoUDPoIPv6oETH0oPTCH2)*/
        header_data = header_data_udp6_tunnel;
        /*
        * PTCH2 = 2 bytes
        * ETH1 = 6+6 bytes
        * EtherType = 2 bytes
        * IPv6 header = 10x4 bytes
        * UDP header = 2x4 bytes 
        * SFLOW header = 4x4 (the header is 7 words, but only 4 words are static, sequence number + time stamp + nof samples are not part of the static header) 
        */
        header_length = 2 + (6+6) + 2 + (10*4) + (2*4) + (4*4);
        udp_header_offset = 2 + (6+6) + 2 + (10*4);
        ip_length_offset = 2 + (6+6) + 2 + 4;
    }
    else
    {
        rv =  BCM_E_FAIL;
        printf("%s: Error (%d), only the following tunnel types are supported: bcmTunnelTypeUdp, bcmTunnelTypeUdp6 \n", proc_name, rv);
        return rv;
    }

    /* PTCH2 : In_PP_Port [9:0], Parser_Program_Control [15] - '1' - Next header should be deduced from the SSP*/
    header_data[0] = eventor_pp_port_high | 0x80;
    header_data[1] = eventor_pp_port_low;

    /* UDP port for SFLOW = 6343 = 0x18C7 */
    header_data[udp_header_offset] = cint_sflow_basic_info.udp_src_port >> 8;
    header_data[udp_header_offset+1] = cint_sflow_basic_info.udp_src_port & 0xFF;
    header_data[udp_header_offset+2] = cint_sflow_basic_info.udp_dst_port >> 8;
    header_data[udp_header_offset+3] = cint_sflow_basic_info.udp_dst_port & 0xFF;
    /* 
      UDP length =
      UDP header (8 bytes) + payload data
      payload data = SFLOW Datagram header (7 words) + aggregated data
       */
    uint32 udp_header_length = 8 + (7 + buffer_size_thr)*4;
    header_data[udp_header_offset+4] = udp_header_length >> 8;
    header_data[udp_header_offset+5] = udp_header_length & 0xFF;

    /* 
      IP Header - total length =
      UDP header + 20 bytes
       */
    uint32 ip_header_total_length = udp_header_length;
    if (cint_sflow_basic_info.tunnel_type == bcmTunnelTypeUdp)
    {
        ip_header_total_length += 20;
    }
    header_data[ip_length_offset] = ip_header_total_length >> 8;
    header_data[ip_length_offset+1] = ip_header_total_length & 0xFF;

    if (cint_sflow_basic_info.tunnel_type == bcmTunnelTypeUdp)
    {
        /*
            The CHECKSUM field in the IPV4 header is set to fit 8 samples.
            Need to change it if nof_samples is diffetent (in our testing it is reconfigured to 5).
        */
        if (nof_samples == 5)
        {
            header_data[ip_length_offset + 8] = (0x35D3) >> 8;
            header_data[ip_length_offset + 9] = (0x35D3) & 0xFF;
        }    
    }
    
    printf("%s: IP LENGTH = 0x%04X, UDP LENGTH = 0x%04X, udp_src_port = 0x%04X, udp_dst_port = 0x%04X \n", proc_name, ip_header_total_length, udp_header_length, cint_sflow_basic_info.udp_src_port, cint_sflow_basic_info.udp_dst_port);

    context = builder = eventor_id;
    /* init eventor RX context */

    /*
     * 1. Can simply use different pair of banks per context (but since there are 8 banks, there can be only 4 contexs ...)
     * 2. Can use same pair of banks but with different offset (bank size is 13K bytes)
     * Here, for simplicity, use #1 
     *  
     * Note: performing modulo 8 operation by & 0x7
     */
    context_conf.bank1 = context & 0x7;
    context_conf.bank2 = (context+1) & 0x7;
    printf("%s: bank1 = %d, bank2 = %d\n", proc_name, context_conf.bank1, context_conf.bank2);

    context_conf.buffer_size = buffer_size;
    context_conf.buffer1_start = 0;
    context_conf.buffer2_start = 256;

    /* init builder */
    builder_conf.flags = 0;
    builder_conf.thresh_size = buffer_size_thr;
    builder_conf.thresh_time = cint_sflow_basic_info.time_thresh; 
    builder_conf.header_data = header_data;
    builder_conf.header_length = header_length;

    /*
     * Set eventor Rx context (Rx buffers)
     */
    if (reconfigure == 0) {
       rv = bcm_instru_eventor_context_set(unit, flags, context, bcmEventorContextTypeRx, &context_conf);
       if (rv != BCM_E_NONE)
       {
           printf("%s: Error (%d), in bcm_instru_eventor_context_set\n", proc_name, rv);
           return rv;
       }
    }
    
    if (reconfigure == 0) {
        flags = 0;
    } else {
        flags = BCM_INSTRU_EVENTOR_BUILDER_RECONFIG_WITH_NO_TRAFFIC;
    }
    /*
     * Set eventor builder (Tx buffers)
     */
    rv = bcm_instru_eventor_builder_set(unit, flags, builder, &builder_conf);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_instru_eventor_builder_set\n", proc_name, rv);
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * This functions configures the eventor for Sflow - samples size, number of samples per sflow datagram etc.
 * Note:
 * For simplicity - this function sets Eventor RX double buffer to be the pair 
 * eventor_id and eventor_id+1 (modulo 8).
 * Thus, when using multi channel configuration, each channel actually occupies two eventor IDs thus each channel's 
 * eventor_id is +2 from previous channel's eventor_id.
 */
int
cint_sflow_utils_eventor_itmh_set(int unit, int eventor_id, int buffer_size, int nof_samples, int eventor_port, int sflow_out_port)
{
    int rv;
    uint32 flags = 0;
    int buffer_size_thr = buffer_size * nof_samples;
    int context;
    int builder;
    uint32 header_length;
    uint32 udp_header_offset;
    uint32 ip_length_offset;
    bcm_instru_eventor_context_conf_t context_conf = {0};
    bcm_instru_eventor_builder_conf_t builder_conf = {0};
    uint8 * header_data;
    bcm_port_interface_info_t int_inf;
    bcm_port_mapping_info_t mapping_info;
    char *proc_name;

    proc_name = "cint_sflow_utils_eventor_itmh_set";

    if (cint_sflow_utils_info.disable_eventor_config == 1) {
       printf("%s: Eventor configuration is skipped!!!\n", proc_name);

       return BCM_E_NONE;
    }

    /* TX header data (SFLOW_DATAGRAMoUDPoIPv4oETH1oITMHoPTCH2)*/
   uint8 header_data_udp_tunnel[128] = {
            /* PTCH2 - 2 bytes*/
            0x00, 0x00,
            /* ITMH - 5+3 bytes */
            0x00, 0x00, 0x00, 0x00, 0x01,
            0x00, 0x00, 0x00,
            /* ETH1 -  6+6+4 bytes*/
            0x00, 0x0e, 0x00, 0x02, 0x00, 0x03, /* DMAC */
            0x00, 0x0e, 0x00, 0x02, 0x00, 0x02, /* SMAC */
            0x81, 0x00, 0x00, 0x64,
            /* Ether type - 2 bytes*/
            0x08, 0x00, 
            /* IPV4 header - 4 x 5 bytes */
            0x45, 0x00, 0x06, 0x38,
            0x00, 0x00, 0x00, 0x00,
            0x40, 0x11, 0x33, 0x93, 
            0xA0, 0x00, 0x00, 0x11, /* SIP = 160.0.0.17 */
            0xA1, 0x00, 0x00, 0x11, /* DIP = 161.0.0.17 */
            /* UDP header - 2 x 4 bytes */
            0x17, 0x70, 0x13, 0x88, 
            0x00, 0x59, 0x00, 0x00,
            /* SFLOW DATAGRAM header - 7 x 4 bytes*/
            0x00, 0x00, 0x00, 0x05,
            0x00, 0x00, 0x00, 0x01,
            0xA0, 0x00, 0x00, 0x11,
            0x00, 0x00, 0x00, 0x17,
            0x00, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 
            /* Dont' care*/
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00
   };

    /* TX header data (SFLOW_DATAGRAMoUDPoIPv6oETH1oITMHoPTCH2)*/
    uint8 header_data_udp6_tunnel[128] = {
            /* PTCH2 - 2 bytes*/
            0x00, 0x00,
            /* ITMH - 5+3 bytes */
            0x00, 0x00, 0x00, 0x00, 0x01,
            0x00, 0x00, 0x00,
            /* ETH1 - 6+6+4 bytes*/
            0x00, 0x0e, 0x00, 0x02, 0x00, 0x03, /* DMAC */
            0x00, 0x0e, 0x00, 0x02, 0x00, 0x02, /* SMAC */
            0x81, 0x00, 0x00, 0x64,
            /* Ether type - 2 bytes*/
            0x86, 0xDD,
            /* IPV6 header  10 x 4 bytes */
            0x60, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x11, 0x40,
            0xEC, 0xEF, 0xEE, 0xED, /* Source Address*/
            0xEC, 0xEB, 0xEA, 0xE9,
            0xE8, 0xE7, 0xE6, 0xE5,
            0xE4, 0xE3, 0xE2, 0xE1,
            0x20, 0x01, 0x0D, 0xB8, /* Destination Address */
            0x00, 0x00, 0x00, 0x00,
            0x02, 0x11, 0x22, 0xFF,
            0xFE, 0x33, 0x44, 0x55,
            /* UDP header - 2 x 4 bytes */
            0x17, 0x70, 0x13, 0x88,
            0x00, 0x59, 0x00, 0x00,
            /* SFLOW DATAGRAM header - 7 x 4 bytes*/
            0x00, 0x00, 0x00, 0x05,
            0x00, 0x00, 0x00, 0x01,
            0xA0, 0x00, 0x00, 0x11, 
            0x00, 0x00, 0x00, 0x17,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            /* Dont' care*/
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00
    };

    /* Get Eventor PP port */
    rv = bcm_port_get(unit, eventor_port, &flags, &int_inf, &mapping_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_port_get\n", proc_name, rv);
        return rv;
    }
    flags = 0;

    printf("%s: eventor_id = %d, eventor_port = %d, eventor_pp_port = %d, buffer_size = %d, nof_samples = %d, buffer_size_thr = %d \n", proc_name, eventor_id, eventor_port, mapping_info.pp_port, buffer_size, nof_samples, buffer_size_thr);

    /* in PTCH2, In_PP_Port is 10 bits [9:0]*/
    uint8 eventor_pp_port_high = ((mapping_info.pp_port) >> 8) & 0x3;
    uint8 eventor_pp_port_low = (mapping_info.pp_port) & 0xFF;

    /* In ITMH, the destination is 21 bits [29:9], but only 16 bits are system port.
       In addition, need to shift left one bit beause it start from bit 9.
    */
    uint8 sflow_port_high = (sflow_out_port <<1) >> 16;
    uint8 sflow_port_mid = ((sflow_out_port <<1) & 0xFF00) >> 8;
    uint8 sflow_port_low = ((sflow_out_port <<1) & 0xFF);

    if (cint_sflow_basic_info.tunnel_type == bcmTunnelTypeUdp)
    {
        /* Set TX header size and data (SFLOW_DATAGRAMoUDPoIPv4oETH1oITMHoPTCH2)*/
        header_data = header_data_udp_tunnel;
        /*
        * PTCH2 = 2 bytes 
        * ITMH = 5+3 bytes 
        * ETH1 = 6+6+4 bytes
        * EtherType = 2 bytes
        * IPv4 header = 5x4 bytes
        * UDP header = 2x4 bytes 
        * SFLOW header = 4x4 (the header is 7 words, but only 4 words are static, sequence number + time stamp + nof samples are not part of the static header) 
        */
        header_length = 2 + (5+3) + (6+6+4) + 2 + (5*4) + (2*4) + (4*4);
        udp_header_offset = 2 + (5+3) + (6+6+4) + 2 + (5*4);
        ip_length_offset = 2 + (5+3) + (6+6+4) + 2 + 2;

    }
    else if (cint_sflow_basic_info.tunnel_type == bcmTunnelTypeUdp6)
    {
        /* Set TX header size and data (SFLOW_DATAGRAMoUDPoIPv6oETH1oITMHoPTCH2)*/
        header_data = header_data_udp6_tunnel;
        /*
        * PTCH2 = 2 bytes 
        * ITMH = 5+3 bytes 
        * ETH1 = 6+6+4 bytes
        * EtherType = 2 bytes
        * IPv6 header = 10x4 bytes
        * UDP header = 2x4 bytes 
        * SFLOW header = 4x4 (the header is 7 words, but only 4 words are static, sequence number + time stamp + nof samples are not part of the static header) 
        */
        header_length = 2 + (5+3) + (6+6+4) + 2 + (10*4) + (2*4) + (4*4);
        udp_header_offset = 2 + (5+3) + (6+6+4) + 2 + (10*4);
        ip_length_offset = 2 + (5+3) + (6+6) + 2 + 4;
    }
    else
    {
        printf("%s: Error (%d),  only the following tunnel types are supported: bcmTunnelTypeUdp, bcmTunnelTypeUdp6 \n", proc_name, BCM_E_FAIL);
        return BCM_E_FAIL;
    }

    /* PTCH2 : In_PP_Port [9:0], Parser_Program_Control [15] - '0' - Indicates that next header is ITMH */
    header_data[0] = eventor_pp_port_high;
    header_data[1] = eventor_pp_port_low;

    /* Note: the assumption here is that the Sflow output destination is System Port */
    header_data[3] = 0x18 | sflow_port_high ; /* See destination encoding*/
    header_data[4] = sflow_port_mid;
    header_data[5] = sflow_port_low;

    /* UDP port for SFLOW = 6343 = 0x18C7 */
    header_data[udp_header_offset] = cint_sflow_basic_info.udp_src_port >> 8;
    header_data[udp_header_offset+1] = cint_sflow_basic_info.udp_src_port & 0xFF;
    header_data[udp_header_offset+2] = cint_sflow_basic_info.udp_dst_port >> 8;
    header_data[udp_header_offset+3] = cint_sflow_basic_info.udp_dst_port & 0xFF;
    /* 
      UDP length =
      UDP header (8 bytes) + payload data
      payload data = SFLOW Datagram header (7 words) + aggregated data
       */
    uint32 udp_header_length = 8 + (7 + buffer_size_thr)*4;
    header_data[udp_header_offset+4] = udp_header_length >> 8;
    header_data[udp_header_offset+5] = udp_header_length & 0xFF;

    /* 
      IP Header - total length =
      UDP header + 20 bytes
       */
    uint32 ip_header_total_length = udp_header_length;
    if (cint_sflow_basic_info.tunnel_type == bcmTunnelTypeUdp)
    {
        ip_header_total_length += 20;
    }
    header_data[ip_length_offset] = ip_header_total_length >> 8;
    header_data[ip_length_offset+1] = ip_header_total_length & 0xFF;

    printf("%s: IP LENGTH = 0x%04X, UDP LENGTH = 0x%04X, udp_src_port = 0x%04X, udp_dst_port = 0x%04X \n", proc_name, ip_header_total_length, udp_header_length, cint_sflow_basic_info.udp_src_port, cint_sflow_basic_info.udp_dst_port);

    context = builder = eventor_id;
    /* init eventor RX context */

    /*
     * 1. Can simply use different pair of banks per context (but since there are 8 banks, there can be only 4 contexs ...)
     * 2. Can use same pair of banks but with different offset (bank size is 13K bytes)
     * Here, for simplicity, use #1 
     *  
     * Note: performing modulo 8 operation by & 0x7
     */
    context_conf.bank1 = context & 0x7;
    context_conf.bank2 = (context+1) & 0x7;
    printf("%s: bank1 = %d, bank2 = %d\n", proc_name, context_conf.bank1, context_conf.bank2);

    context_conf.buffer_size = buffer_size;
    context_conf.buffer1_start = 0;
    context_conf.buffer2_start = 256;

    /* init builder */
    builder_conf.flags = 0;
    builder_conf.thresh_size = buffer_size_thr;
    builder_conf.thresh_time = cint_sflow_basic_info.time_thresh;
    builder_conf.header_data = header_data;
    builder_conf.header_length = header_length;

    /*
     * Set eventor Rx context (Rx buffers)
     */
    rv = bcm_instru_eventor_context_set(unit, flags, context, bcmEventorContextTypeRx, &context_conf);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_instru_eventor_context_set\n", proc_name, rv);
        return rv;
    }

    /*
     * Set eventor builder (Tx buffers)
     */
    rv = bcm_instru_eventor_builder_set(unit, flags, builder, &builder_conf);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in bcm_instru_eventor_builder_set\n", proc_name, rv);
       return rv;
    }

    return BCM_E_NONE;
}

/*
 * This functions activates the eventor for Sflow.
 */
int
cint_sflow_utils_eventor_activate(int unit)
{
    int rv;
    uint32 flags = 0;
    char *proc_name;

    proc_name = "cint_sflow_utils_eventor_activate";

   if (cint_sflow_utils_info.disable_eventor_config == 1) {
      printf("%s: Eventor activation is skipped!!!\n", proc_name);

      return BCM_E_NONE;
   }

   /*
   * Activate eventor
   */
   rv = bcm_instru_eventor_active_set(unit, flags, 1);
   if (rv != BCM_E_NONE)
   {
      printf("%s: Error (%d), in bcm_instru_eventor_active_set\n", proc_name, rv);
      return rv;
   }

   printf("%s: Eventor is activated.\n", proc_name);

   return BCM_E_NONE;
}

/*
 * Sflow registers registers initialization 
 */
int
cint_sflow_utils_registers_init(int unit)
{
    int rv;
    char *proc_name;

    proc_name = "cint_sflow_utils_registers_init";

    /*
     * Set the global virtual register for sFlow sampling rate.
     */
    rv = bcm_instru_control_set(unit, 0, bcmInstruControlSFlowSamplingRate, cint_sflow_basic_info.sample_rate_divisor);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_dnx_instru_control_get\n", proc_name, rv);
        return rv;
    }

    /*
     * Set the global sFlow agent IP address.
     */
    rv = bcm_instru_control_set(unit, 0, bcmInstruControlSFlowAgentIPAddress, cint_sflow_basic_info.tunnel_sip);
    if (rv != BCM_E_NONE)
    {
         printf("%s: Error (%d), in bcm_instru_control_set\n", proc_name, rv);
        return rv;
    }

    /*
     * For the purpose of the example cint, we'll use a meaningless timestamp.
     */
    rv = cint_sflow_utils_timestamp_update(unit, cint_sflow_basic_info.uptime);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in cint_sflow_utils_timestamp_update\n", proc_name, rv);
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * This function creates port-to-interface mapping in SFLOW the egress pipe. 
 * The port can be: 
 *    For input interface:
 *       - system port gport.
 *       - system port value.
 *    Note:  for trunk, need to add each of the trunk member separately.
 *    For output interface:
 *       - system port gport.
 *       - system port value.
 *       - trunk gport.
 *       - Multicast gport.
 *       - Flow-ID gport.
 *  
 * For more information about SFLOW input/output interface please 
 * refer to BCM API bcm_instru_sflow_sample_interface_add. 
 */
int
cint_sflow_utils_egress_interface_create(int unit, int port, int interface, int is_input)
{
    int rv;
    bcm_gport_t gport;
    bcm_instru_sflow_sample_interface_info_t sample_interface_info;
    char *proc_name;

    proc_name = "cint_sflow_utils_egress_interface_create";

    if (BCM_GPORT_IS_SET(port)) {
       gport = port;
    }
    else {
       BCM_GPORT_SYSTEM_PORT_ID_SET(gport, port);
    }

    printf("%s: port = %d, gport = 0x%08x, interface = 0x%08x,  is_input = %d\n",
           proc_name, port, gport, interface, is_input);

    sample_interface_info.flags = is_input? BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_INPUT:BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_OUTPUT;
    sample_interface_info.port = gport;
    sample_interface_info.interface = interface;

    rv = bcm_instru_sflow_sample_interface_add(unit, &sample_interface_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_instru_sflow_sample_interface_add\n", proc_name, rv);
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * This function removes port-to-interface mapping in SFLOW the egress pipe
 */
int
cint_sflow_utils_egress_interface_remove(int unit, int port, int is_input)
{
    int rv;
    bcm_gport_t gport;
    bcm_instru_sflow_sample_interface_info_t sample_interface_info;
    char *proc_name;

    proc_name = "cint_sflow_utils_egress_interface_remove";

    if (BCM_GPORT_IS_SET(port)) {
       gport = port;
    }
    else {
       BCM_GPORT_SYSTEM_PORT_ID_SET(gport, port);
    }

    printf("%s: port = %d, gport = 0x%08x, is_input = %d\n", proc_name, port, gport, is_input);

    sample_interface_info.flags = is_input? BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_INPUT:BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_OUTPUT;
    sample_interface_info.port = gport;
    sample_interface_info.interface = 0;

    rv = bcm_instru_sflow_sample_interface_remove(unit, &sample_interface_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_instru_sflow_sample_interface_remove\n", proc_name, rv);
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * This function creates a trunk with 3 port memebers.
 */
int
cint_sflow_utils_lag_create(int unit, int port1, int port2, int port3)
{
    int rv;
    char *proc_name;
    cint_trunk_utils_s trunk;

    proc_name = "cint_sflow_utils_lag_create";

    trunk.ports_nof = 3;
    trunk.ports[0] = port1;
    trunk.ports[1] = port2;
    trunk.ports[2] = port3;
    trunk.pool = 0;
    trunk.group = 10;
    trunk.headers_type_in = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    trunk.headers_type_out = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    trunk.vlan_domain = 128;
    trunk.vlan = 15; /* see dnx_basic_example_inner - intf_in */

    /* Create trunk  */
    rv = cint_trunk_utils_create(unit, &trunk);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in cint_trunk_utils_create\n", proc_name, rv);
       return rv;
    }

    printf("%s: tid = %d was created (pool = %d, group = %d, gport = 0x%08X)\n", proc_name, trunk.tid, trunk.pool, trunk.group, trunk.gport);

    cint_sflow_utils_info.trunk_gport = trunk.gport;

    return BCM_E_NONE;
}
