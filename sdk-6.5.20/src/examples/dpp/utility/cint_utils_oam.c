/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    cint_utils_oam.c
 * Purpose: Utility functions and variables to be used by all OAM/BFD cints
 */

/* Enum signifying the eth format header */
enum eth_tag_type_t {
    untagged = 0,
    single_tag = 1,
    double_tag = 2
};

eth_tag_type_t eth_tag_type;


/* HLM counters */
int counter_engines[3];
int counter_base_ids[4];
/* HLM MEPs */
bcm_oam_endpoint_info_t meps[3];

/****************************************************************/
/*                    VSWITCH INITIALIZATION FUNCTIONS                                               */
/****************************************************************/






/* Two copies: one per core */
int current_lm_counter_base[2]={0};
int current_counter_range_index[2]={0};
/* All ingress oam counter engines should be configured with a
   lower counter engine ID than all egress counter engines IDs */
int next_available_ingress_counter_engine=0; /* Assuming no other application is calling counter_config_set() */
int next_available_egress_counter_engine=8; /* reserving first four engines for ingress oam counter engine*/
int current_counter_id=16;

int counter_engine_status[16] = {0};
/* Global variable to hold the assigned engine for egress and ingress OAM. This is needed so TCL scripts will know which
   engine to access when reading counters */
int global_first_egress_oam_engine_id = -1;
int global_first_ingress_oam_engine_id = -1;
/* In order to count traffic over GREoIP tunnels this function should be called
 * The returned value (counter ID) should go into the field val in bcm_port_stat_set()
 * Each call allocates 1 counter IDs 
*/ 
int set_counter_source_and_engines_for_tunnel_counting(int unit, int * counter_base, int port){

    int rv=0;
    int counter_id;
    int core, tm_port;
    int double_mode_multiplier = 2;

    rv = get_core_and_tm_port_from_port(unit, port, &core, &tm_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in get_core_and_tm_port_from_port\n");
        return rv;
    }

    if (current_counter_id == 16) {
        rv = set_counter_source_and_engines(unit, &counter_id, port);
        if (rv != BCM_E_NONE) {
            printf("Error, in get_core_and_tm_port_from_port\n");
            return rv;
        }
        current_counter_id =1;
    } else {
        current_counter_id ++;
    }
    /* The functions reserves the last 16 entries in the counter engine for the tunnel counters (you can reserve more if needed) So it returns a counter ID that is the last counter index in the counter engine - 16, and subsequent calls will give the next counter index. The reason we substract 24 and not 16 is because current_lm_counter_base was added 8 when it was created to reserve counters for LM. */
    int NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE = SOC_DPP_DEFS_GET_COUNTERS_PER_COUNTER_PROCESSOR(unit) * double_mode_multiplier;
    *counter_base = (current_lm_counter_base[core] + NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE - 24) + current_counter_id;
    
    return 0;

}

/* For HLM supported devices such as QAX\QUX, first range (which includes counter pointer 0),
   must be configured (but cannot be used)in addition to configuring regular OAM counter engines.
   This is required because the defult counter index output of OAM engine is zero(for disabling counting packets)/
   Ingress\egress pipeline always expects to get a result back from a counter engine, therefore range zero
   should always be configured 
   The function allocates 2 engines for OAM
   One for ingress counter engine index zero and one for egress counter engine index zero. */
int set_first_range_for_qumran(int unit)
{
       int      index1, index2;
       int      rv;
       bcm_stat_counter_engine_t counter_engine;
       bcm_stat_counter_config_t counter_config;
       uint32   counter_engine_flags = 0;
       uint8    drop_fwd[] = { 0, 1 };
       bcm_color_t colors[] = { bcmColorGreen, bcmColorYellow, bcmColorRed, bcmColorBlack };
       int double_mode_multiplier = 2;
       
       int NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE = SOC_DPP_DEFS_GET_COUNTERS_PER_COUNTER_PROCESSOR(unit) * double_mode_multiplier;
       counter_config.format.format_type = bcmStatCounterFormatDoublePackets;
       /* Since we are indifferent to color/droped-forwarded we only need one counter set. */
       counter_config.format.counter_set_mapping.counter_set_size = 1;
       /* Using 8 entries for 8 PCP values. */
       counter_config.format.counter_set_mapping.num_of_mapping_entries = 8;
       for (index1 = 0; index1 < bcmColorPreserve; index1++) {
          for (index2 = 0; index2 < 2; index2++) {
             /* Counter configuration is independent on the color, drop precedence.*/
             counter_config.format.counter_set_mapping.entry_mapping[index1 * 2 + index2].offset = 0; /* Must be zero since counter_set_size is 1 */
             counter_config.format.counter_set_mapping.entry_mapping[index1 * 2 + index2].entry.color = colors[index1];
             counter_config.format.counter_set_mapping.entry_mapping[index1 * 2 + index2].entry.is_forward_not_drop = drop_fwd[index2];
          }
       }
       counter_config.source.core_id = 0;

       /* In double mode, two engines are required for the same range. One for egress and one for ingress.
          Setting the first counter base id accordingly. */
       counter_config.source.pointer_range.start = 0;
       counter_config.source.pointer_range.end =  NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE - 1;
       current_counter_range_index[0]++;

       /* Set lm_counter_base_id to the first one in the range. */
       current_lm_counter_base[0] = counter_config.source.pointer_range.start;

       /* Egress OAM counter engines should be configured with a
          higher counter engine IDs than all ingress counter engines IDs */
       counter_config.source.engine_source = bcmStatCounterSourceEgressOam;
       counter_engine.engine_id = next_available_egress_counter_engine++;
       counter_engine.flags = counter_engine_flags;

       rv = bcm_stat_counter_config_set(unit, &counter_engine, &counter_config);
       printf("Engine id %d for egress , core %d configured with range 0x%05x - 0x%05x\n",counter_engine.engine_id,
              0, counter_config.source.pointer_range.start, counter_config.source.pointer_range.end);
       counter_engine_status[counter_engine.engine_id] = 1;

       /* Configuring Ingress engine (Same range)
          ingress OAM counter engines should be configured with a
          lower counter engine IDs than all egress counter engines IDs */

       counter_config.source.engine_source = bcmStatCounterSourceIngressOam;

       counter_engine.engine_id = next_available_ingress_counter_engine++;

       rv = bcm_stat_counter_config_set(unit, &counter_engine, &counter_config);
       BCM_IF_ERROR_RETURN(rv);
       printf("Engine id %d for ingress, core %d configured with range 0x%05x - 0x%05x\n",counter_engine.engine_id,
              0, counter_config.source.pointer_range.start, counter_config.source.pointer_range.end);
       counter_engine_status[counter_engine.engine_id] = 1;
       
       return rv;
}


/** 
 * Whenever creating an endpoint on a new LIF this function 
 *  should be called. The value returned in counter_base should
 *  go into the field lm_counter_base_id in
 *  bcm_oam_endpoint_create().
 *  
 *  Allocate counters. In Arad counters have been statically
 *  allocated via soc property, simply maintain a global
 *  variable.
 *  
 *  In Jericho Counter engines must be allocated on the fly.
 *  Each call to bcm_stat_counter_config_set() allocates 16K (8K
 *  in QAX) counter IDs. Maintain a global counter and
 *  whenever counters ids have run out call counter_config_set()
 *  (i.e. on the first call, 16K-th call, etc.). Each call
 *  allocates 8 counter ids to account for PCP based LM.
 *  
 *  Ingress and egress counter engine whose range includes
 *  pointer 0 must be configured prior to any traffic in the
 *  device
 *  
 *  All ingress oam counter engines should be configured with a
 *  lower counter engine id than all egress counter engines ids
 *  
 * @author sinai (28/12/2014)
 * 
 * @param unit 
 * @param counter_base - return value 
 * @param port - Port on which MEP is defined, used for 
 *             extracting core ID. All endpoints must be defined
 *             on same core.
 *         
 * 
 * @return int 
 */
int set_counter_source_and_engines(int unit, int *counter_base, int port) {
   bcm_stat_counter_engine_t counter_engine;
   bcm_stat_counter_config_t counter_config;
   bcm_color_t colors[] = { bcmColorGreen, bcmColorYellow, bcmColorRed, bcmColorBlack };
   uint8    drop_fwd[] = { 0, 1 };
   int      index1, index2;
   int      rv;
   int      core, tm_port;
   uint64   arg;
   uint32   counter_engine_flags = 0;
   int      double_mode_multiplier = 2;

   /* This should be initialized by now anyways, but check again*/
   rv = oam__device_type_get(unit, &device_type);
   BCM_IF_ERROR_RETURN(rv);

  /* HLM supported devices - need to set first counter processor range */
  if (device_type >= device_type_qax && !current_counter_range_index[0]) {
        rv = set_first_range_for_qumran(unit);
        BCM_IF_ERROR_RETURN(rv);
    }


   rv = get_core_and_tm_port_from_port(unit, port, &core, &tm_port);
   if (rv != BCM_E_NONE) {
      printf("Error, in get_core_and_tm_port_from_port\n");
      return rv;
   }

   if (device_type < device_type_jericho) {
      /* In Arad the counters have been statically allocated via soc properties. Furthurmore LM-PCP is not available.
         simply increment the counter and return.*/
      *counter_base = ++current_lm_counter_base[core];
      return 0;
   }

   if (device_type >= device_type_qax) {
       /* QAX may recevive the counter from a 2nd source.
          For this two work multiple sources per counter engine should be enbaled. */
       counter_engine_flags = BCM_STAT_COUNTER_MULTIPLE_SOURCES_PER_ENGINE;
   }

   /* The counter processor can work in double mode. When working in double mode,
      each counter engine can support twice the number of counters. This is acheived by
      splitting each entry into two entries of 32b. This is the suggested mode for OAM
      as OAM messages has 32b fields for loss measaurment. Working in this mode is MANDATORY
      for OAM to prevent the accessing of the same engine from both ingress / egress
      at the same time (which can cause errors with counter stamping / increment). */

   /* Get number of counter engines and number of counter pointers per counter engine. */
   int NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE = SOC_DPP_DEFS_GET_COUNTERS_PER_COUNTER_PROCESSOR(unit) * double_mode_multiplier;

   if (current_lm_counter_base[core] % NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE == 0) {
       /* Ran out of available counters. Allocate another bunch. */
       if (next_available_egress_counter_engine >= 16 || bcmStatCounterSourceIngressOam >= 8) {
          print "Out of counter engines.";
          return 444;
       }

       /* Configuring Egress engine. */

       /* Setting format to double mode. */
       counter_config.format.format_type = bcmStatCounterFormatDoublePackets;
       /* Since we are indifferent to color/droped-forwarded we only need one counter set. */
       counter_config.format.counter_set_mapping.counter_set_size = 1;
       /* Using 8 entries for 8 PCP values. */
       counter_config.format.counter_set_mapping.num_of_mapping_entries = 8;
       for (index1 = 0; index1 < bcmColorPreserve; index1++) {
          for (index2 = 0; index2 < 2; index2++) {
             /* Counter configuration is independent on the color, drop precedence.*/
             counter_config.format.counter_set_mapping.entry_mapping[index1 * 2 + index2].offset = 0; /* Must be zero since counter_set_size is 1 */
             counter_config.format.counter_set_mapping.entry_mapping[index1 * 2 + index2].entry.color = colors[index1];
             counter_config.format.counter_set_mapping.entry_mapping[index1 * 2 + index2].entry.is_forward_not_drop = drop_fwd[index2];
          }
       }
       counter_config.source.core_id = core;

       /* In double mode, two engines are required for the same range. One for egress and one for ingress.
          Setting the first counter base id accordingly. */
       counter_config.source.pointer_range.start = current_counter_range_index[core] * NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE;
       counter_config.source.pointer_range.end = counter_config.source.pointer_range.start + NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE - 1;
       current_counter_range_index[core]++;

       /* Set lm_counter_base_id to the first one in the range. */
       current_lm_counter_base[core] = counter_config.source.pointer_range.start;

       /* Egress OAM counter engines should be configured with a
          higher counter engine IDs than all ingress counter engines IDs */
       counter_config.source.engine_source = bcmStatCounterSourceEgressOam;
       counter_engine.engine_id = next_available_egress_counter_engine++;
       counter_engine.flags = counter_engine_flags;

       rv = bcm_stat_counter_config_set(unit, &counter_engine, &counter_config);
       BCM_IF_ERROR_RETURN(rv);
       printf("Engine id %d for egress , core %d configured with range 0x%05x - 0x%05x\n",counter_engine.engine_id,
              core, counter_config.source.pointer_range.start, counter_config.source.pointer_range.end);
       counter_engine_status[counter_engine.engine_id] = 1;
       if (global_first_egress_oam_engine_id == -1) {
	 global_first_egress_oam_engine_id = counter_engine.engine_id;
       }
       
       /* Configuring Ingress engine (Same range). 
          ingress OAM counter engines should be configured with a
          lower counter engine IDs than all egress counter engines IDs */

       counter_config.source.engine_source = bcmStatCounterSourceIngressOam;

       counter_engine.engine_id = next_available_ingress_counter_engine++;

       rv = bcm_stat_counter_config_set(unit, &counter_engine, &counter_config);
       BCM_IF_ERROR_RETURN(rv);
       printf("Engine id %d for ingress, core %d configured with range 0x%05x - 0x%05x\n",counter_engine.engine_id,
              core, counter_config.source.pointer_range.start, counter_config.source.pointer_range.end);
       counter_engine_status[counter_engine.engine_id] = 1;
       if (global_first_ingress_oam_engine_id == -1) {
	 global_first_ingress_oam_engine_id = counter_engine.engine_id;
       }
   }

   /* counter engines are set. Return a counter base. */
   /* Increment by 8 to allow PCP based LM if needed. */
   current_lm_counter_base[core] += 8;
   /* ID 0 cannot be used - the value 0 is reserved to signify disabling LM counters. */
   *counter_base = current_lm_counter_base[core];

   return 0;
}


/**
 *  Allocate counters for pmf.
 *  In Arad counters have been statically
 *  allocated via soc property, simply maintain a global
 *  variable.
 *
 *  In Jericho Counter engines must be allocated on the fly.
 *  Each call to bcm_stat_counter_config_set() allocates 16K counter IDs.
 *
 * @author mark (15/12/2015)
 *
 * @param unit
 * @param counter_offset - counter offset should be multiple of engine counter size(16K).
 * @param engine_id - allocate engine (0-15)
 * @param core      - core (0-1)
 * @return int
 */
int pmf_ccm_counter_engine_allocation(int unit, int counter_start, int engine_id, int core)
{
    bcm_stat_counter_engine_t counter_engine ;
    bcm_stat_counter_config_t counter_config ;
    bcm_color_t colors[] = { bcmColorGreen, bcmColorYellow, bcmColorRed, bcmColorBlack };
    uint8 drop_fwd[] = { 0,1 };
    int rv=0;
    int index1, index2;

    /* This should be initialized by now anyways, but check again*/
    rv = oam__device_type_get(unit, &device_type);
    BCM_IF_ERROR_RETURN(rv);

    if (device_type<device_type_jericho ) {
        /* In Arad the counters have been statically allocated via soc properties. */
        return 0;
    }

    if (SOC_DPP_DEFS_GET_NOF_CORES(unit) > 1) {
        if (((sal_strcmp(soc_property_get_str(unit, spn_DEVICE_CORE_MODE), "SINGLE_CORE")) == 0) && (core == 1)) 
        {
            print "We are in single core mode! Counter shold not be initialized for core 1.";
            return 0;
        }
    }

    if(counter_engine_status[engine_id] == 1) {
        printf("Engine %d allocated already",engine_id);
        return 444;
    }

    int NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE = SOC_DPP_DEFS_GET_COUNTERS_PER_COUNTER_PROCESSOR(unit);

    if ( counter_start % NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE != 0  ) {
            print "Start should be multiple of engine size(16K).";
            return 444;
    }

    counter_config.format.format_type = bcmStatCounterFormatPackets; /* This gives us 16K counter ids per engine, but only 29 bits for the counter.*/
    counter_config.format.counter_set_mapping.counter_set_size = 1;/* Since we are indifferent to color/droped-forwarded we only need one counter set.*/
    counter_config.format.counter_set_mapping.num_of_mapping_entries = 8; /* Using 8 entries for 8 PCP values.*/
    if(soc_property_get(unit , "oam_statistics_per_mep_enabled",1) == 2)
        counter_config.source.command_id = 1; /*stat1*/
    else
        counter_config.source.command_id = 0; /*stat0*/
    for (index1 = 0; index1 < bcmColorPreserve; index1++) {
        for (index2 = 0; index2 < 2; index2++) {
            /* Counter configuration is independent on the color, drop precedence.*/
            counter_config.format.counter_set_mapping.entry_mapping[index1 * 2 + index2].offset = 0; /* Must be zero since counter_set_size is 1 */
            counter_config.format.counter_set_mapping.entry_mapping[index1 * 2 + index2].entry.color = colors[index1];
            counter_config.format.counter_set_mapping.entry_mapping[index1 * 2 + index2].entry.is_forward_not_drop = drop_fwd[index2];
        }
    }
    counter_config.source.core_id = core;

    counter_config.source.pointer_range.start =(counter_start / NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE)  *NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE ;
    counter_config.source.pointer_range.end = ((counter_start / NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE) +1)  *NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE -1;

    counter_config.source.engine_source = bcmStatCounterSourceIngressField;
    counter_engine.engine_id = engine_id;

    rv = bcm_stat_counter_config_set(unit, &counter_engine, &counter_config);
    BCM_IF_ERROR_RETURN(rv);
    counter_engine_status[engine_id]=1;

    return 0;
}


/**
 *  Function creates an accelerated endpoint.
 *  group_info_short_ma  must be initialized before this
 *  function is called.
 *  Likewise the function create_vswitch_and_mac_entries() must
 *  also  be called before this function.
 *  
 * @author liat 
 * 
 * @param unit 
 * @param is_up - direction of the endpoint.
 * @param eth_tag_type - vlan tag format.
 * 
 * @return int 
 */

int port_1_interface_2_both_3_status = 0;

int create_oam_accelerated_endpoint(int unit , int is_up, eth_tag_type_t eth_tag_type, int is_itmh_mc) {
   bcm_oam_endpoint_info_t  acc_endpoint;
   bcm_oam_endpoint_info_t_init(&acc_endpoint);

  /*TX*/
  acc_endpoint.type = bcmOAMEndpointTypeEthernet;
  acc_endpoint.group = group_info_short_ma.id;
  acc_endpoint.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
  acc_endpoint.timestamp_format = get_oam_timestamp_format(unit);

  switch (eth_tag_type) {
  case untagged:
          acc_endpoint.level = 4;
		  acc_endpoint.vlan = 0; 
		  acc_endpoint.pkt_pri = 0;
		  acc_endpoint.outer_tpid = 0;
		  break;
  case single_tag:
        acc_endpoint.level = 2;
		  acc_endpoint.vlan = 5; 
		  acc_endpoint.pkt_pri = 0 + (2<<1); /* dei(1bit) + (pcp(3bit) << 1)*/
		  acc_endpoint.outer_tpid = 0x8100;
		  break;
	  case double_tag:
		  acc_endpoint.vlan = 10;
		  acc_endpoint.pkt_pri = mep_acc_info.pkt_pri = 0 + (1<<1); /* dei(1bit) + (pcp(3bit) << 1)*/
		  acc_endpoint.outer_tpid = 0x8100;
		  acc_endpoint.inner_vlan = 5;
		  acc_endpoint.inner_pkt_pri = 3;
		  acc_endpoint.inner_tpid = 0x8100;
          acc_endpoint.level = 3;
		  break;
	   default:
		   printf("Error, non valid eth_tag_type\n");
	  }

  if (is_up) {
        /*TX*/
      acc_endpoint.level = 5;
      acc_endpoint.name = 123;     
      acc_endpoint.flags |= BCM_OAM_ENDPOINT_UP_FACING;
      acc_endpoint.int_pri = 3 + (1<<2);
      /* The MAC address that the CCM packets are sent with*/
      src_mac_mep_2[5] = port_2;
      sal_memcpy(acc_endpoint.src_mac_address, src_mac_mep_2, 6);
      /*RX*/
      acc_endpoint.gport = gport2;
      sal_memcpy(acc_endpoint.dst_mac_address, mac_mep_2, 6);
      acc_endpoint.lm_counter_base_id = 6;
      acc_endpoint.tx_gport = BCM_GPORT_INVALID;
  } else { /** Down*/
      
      BCM_GPORT_SYSTEM_PORT_ID_SET(acc_endpoint.tx_gport, port_1);
      acc_endpoint.name = 456;     
      acc_endpoint.ccm_period = 100;
      acc_endpoint.int_pri = 1 + (3<<2);
      /* The MAC address that the CCM packets are sent with*/
        sal_memcpy(acc_endpoint.src_mac_address, src_mac_mep_3, 6);

        /*RX*/
        /* enable oamp injection with mc destination on the ITMH*/
        if (is_itmh_mc) {

            int rv =0;
            bcm_gport_t gport;
            bcm_multicast_t mc_group=0x5000;
            BCM_GPORT_MCAST_SET(gport, mc_group);
            int unit = 0;
            rv =  bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID, &mc_group);
            BCM_IF_ERROR_RETURN(rv);

            rv =  bcm_multicast_ingress_add(unit, mc_group, 200, 130);
            BCM_IF_ERROR_RETURN(rv);

            rv =  bcm_multicast_ingress_add(unit, mc_group, 201, 140);
            BCM_IF_ERROR_RETURN(rv);

            acc_endpoint.tx_gport = gport;
            sal_memcpy(acc_endpoint.dst_mac_address, mac_mep_3, 6);
            acc_endpoint.lm_counter_base_id = 6;

        }
            acc_endpoint.gport = gport1;
            sal_memcpy(acc_endpoint.dst_mac_address, mac_mep_3, 6);
            acc_endpoint.lm_counter_base_id = 6;
    }

    if (port_1_interface_2_both_3_status == 1) {
        acc_endpoint.flags |= BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
        acc_endpoint.port_state = BCM_OAM_PORT_TLV_UP;
    } 
    else if(port_1_interface_2_both_3_status == 2) {
        acc_endpoint.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE;
        acc_endpoint.interface_state = BCM_OAM_INTERFACE_TLV_UP;
    }
    else if (port_1_interface_2_both_3_status == 3) {
        acc_endpoint.flags |= BCM_OAM_ENDPOINT_PORT_STATE_UPDATE | BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE;
        acc_endpoint.port_state = BCM_OAM_PORT_TLV_UP;
        acc_endpoint.interface_state = BCM_OAM_INTERFACE_TLV_UP;
    }

  return  bcm_oam_endpoint_create(unit, &acc_endpoint);
}


/**
 * Get OAMP gport
 *
 * @param unit
 */
int oamp_gport_get(int unit, bcm_gport_t *oamp_gport) {
    bcm_error_t rv;
    int count;
    bcm_gport_t gports[2];

    rv = bcm_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, 2, gports, &count);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    if (count < 1) {
        printf("OAMP GPort not found.\n");
        return BCM_E_NOT_FOUND;
    }

    *oamp_gport = gports[0];
    return BCM_E_NONE;
}

/**
 * Enable or Disable the OAM Measure next SLR feature for offloaded MEPs
 *
 * @param unit
 * @param is_enabled
 *
 * @return error code
 */
int enable_oam_measure_next_slr(int unit, int is_enabled) {
    uint64 control_value;
    bcm_oam_control_type_t control_var = bcmOamControlEnableNextReceivedSlr;

    COMPILER_64_SET(control_value, 0, is_enabled);
    return bcm_oam_control_set(unit,control_var,control_value);
}

int get_oam_tod(int unit, uint64 *time, int using_1588) {
    bcm_oam_control_type_t control_var = using_1588? bcmOamControl1588ToD : bcmOamControlNtpToD;

    return bcm_oam_control_get(unit,control_var,time);
}


/**
 * Setup all the necessary counters for hierarchical LM
 * Each packet can cause a single access to a counter engine so if we
 * want a packet to be counted on more then one counter, each must be on
 * a different engine.
 * This function allocates num_engines engines and
 * num_counters_per_engine counters on each engine. 
 * flags - holds the engine flags to be set at engine config 
 */
int hierarchical_lm_counters_setup(int unit,
                                   int num_engines,
                                   int num_counters_per_engine,
                                   int *engine_arr,
                                   int *counter_base_ids,
                                   uint32 flags) {

    bcm_error_t rv = BCM_E_NONE;
    int engine_index, counter_index = 0, i;
    bcm_stat_counter_engine_t counter_engine;
    bcm_stat_counter_config_t counter_config;
    bcm_color_t colors[] = { bcmColorGreen, bcmColorYellow, bcmColorRed, bcmColorBlack };
    uint8 drop_fwd[] = { 0, 1 };
    int index1, index2;
    int NUMBER_OF_COUNTER_INCREASE_COMMANDS = 2;
    int START_ENGINE = 2;
    int double_mode_multiplier = 2;

    /* This is meant for QAX and above only  */
    rv = oam__device_type_get(unit, &device_type);
    BCM_IF_ERROR_RETURN(rv);
    if (device_type < device_type_qax) {
        return BCM_E_UNAVAIL;
    }
    
    /* HLM supported devices - need to set first counter processor range */
    rv = set_first_range_for_qumran(unit);
    BCM_IF_ERROR_RETURN(rv);

    /* The counter processor can work in double mode. When working in double mode,
       each counter engine can support twice the number of counters. This is acheived by
       splitting each entry into two entries of 32b. This is the suggested mode for OAM
       as OAM messages has 32b fields for loss measaurment. Working in this mode is MANDATORY
       for OAM to prevent the accessing of the same engine from both ingress / egress
       at the same time (which can cause errors with counter stamping / increment). */

    /* Get number of counter engines and number of counter pointers per counter engine. */
    int NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE = SOC_DPP_DEFS_GET_COUNTERS_PER_COUNTER_PROCESSOR(unit) * double_mode_multiplier;

    /* Setup counter engines */
    for (engine_index = START_ENGINE; engine_index < (num_engines * double_mode_multiplier + START_ENGINE); engine_index+=2) {

        /* Configuring Egress engine. */

        counter_config.format.format_type = bcmStatCounterFormatDoublePackets; /* This gives us 16K counter ids per engine, but only 29 bits for the counter.*/
        counter_config.format.counter_set_mapping.counter_set_size = 1; /* Since we are indifferent to color/droped-forwarded we only need one counter set.*/
        counter_config.format.counter_set_mapping.num_of_mapping_entries = 8; /* Using 8 entries for 8 PCP values.*/

        counter_config.source.pointer_range.start = (engine_index / double_mode_multiplier) * NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE;
        counter_config.source.pointer_range.end = counter_config.source.pointer_range.start + NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE - 1;

        counter_config.source.engine_source = bcmStatCounterSourceEgressOam;
        counter_config.source.command_id = (((engine_index-START_ENGINE) / double_mode_multiplier) % NUMBER_OF_COUNTER_INCREASE_COMMANDS);
        for (index1 = 0; index1 < bcmColorPreserve; index1++) {
            for (index2 = 0; index2 < 2; index2++) {
                /* Counter configuration is independent on the color, drop precedence.*/
                counter_config.format.counter_set_mapping.entry_mapping[index1 * 2 + index2].offset = 0; /* Must be zero since counter_set_size is 1 */
                counter_config.format.counter_set_mapping.entry_mapping[index1 * 2 + index2].entry.color = colors[index1];
                counter_config.format.counter_set_mapping.entry_mapping[index1 * 2 + index2].entry.is_forward_not_drop = drop_fwd[index2];
            }
        }

        counter_engine.engine_id = next_available_egress_counter_engine++;
        counter_engine.flags = flags;

        rv = bcm_stat_counter_config_set(unit, &counter_engine, &counter_config);
        BCM_IF_ERROR_RETURN(rv);
        printf("Engine id %d for Egress  configured with range 0x%05x - 0x%05x. Command ID is %d\n",counter_engine.engine_id,
               counter_config.source.pointer_range.start, counter_config.source.pointer_range.end,counter_config.source.command_id);

        engine_arr[(engine_index - START_ENGINE) / double_mode_multiplier] = counter_engine.engine_id;
        for (i = 0; i < num_counters_per_engine; i++) {
            counter_base_ids[counter_index++] = counter_config.source.pointer_range.start + (8*i);
        }

        /* Configuring Ingress engine. */
        counter_config.source.engine_source = bcmStatCounterSourceIngressOam;

        counter_engine.engine_id = next_available_ingress_counter_engine++;

        rv = bcm_stat_counter_config_set(unit, &counter_engine, &counter_config);
        BCM_IF_ERROR_RETURN(rv);
        printf("Engine id %d for ingress configured with range 0x%05x - 0x%05x. Command ID is %d\n",counter_engine.engine_id,
               counter_config.source.pointer_range.start, counter_config.source.pointer_range.end,counter_config.source.command_id);
    }

    return BCM_E_NONE;
}

/**
 * Cint that demonstrate how to configure PCP counters support
 *
 * @param unit
 * @param gport
 *
 * @return int
 */
int oam_qos_map_create(int unit,bcm_gport_t gport, bcm_gport_t gport2, bcm_gport_t gport3) {

    bcm_qos_map_t oam_pcp_qos_map;
    int     oam_pcp_qos_map_id;
    int     egress_tc;
    int     up_mep_lm_counter_base , down_mep_lm_counter_base;
    uint64  arg;
    bcm_error_t rv = BCM_E_NONE;

   /* Config Range to : Min=0, Max=0 . That allows the counters to be set */
    COMPILER_64_ZERO(arg);
    rv = bcm_oam_control_set(unit, bcmOamControlLmPcpCounterRangeMin, arg);
    BCM_IF_ERROR_RETURN(rv);
    rv = bcm_oam_control_set(unit, bcmOamControlLmPcpCounterRangeMax, arg);
    BCM_IF_ERROR_RETURN(rv);
    
    
    bcm_qos_map_t_init(&oam_pcp_qos_map);

    oam_pcp_qos_map_id = 0; /* Use OAM=outlif-profile 0 (Only available value in Jericho A0. In Jericho B and above this may be 1.)*/

    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_OAM_PCP, &oam_pcp_qos_map_id);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    for (egress_tc = 0; egress_tc < 8; egress_tc++) {

        oam_pcp_qos_map.int_pri = egress_tc;
        oam_pcp_qos_map.pkt_pri = egress_tc; /* OAM-PCP taken from this value. Use a 1:1 mapping in this case from the TC to the OAM-PCP.*/

        rv = bcm_qos_map_add(unit, 0, &oam_pcp_qos_map, oam_pcp_qos_map_id);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
    }

    /* Set the OAM-outlif-profile on the MEP's gport.*/

    if (gport != BCM_GPORT_INVALID) {
        rv = bcm_qos_port_map_set(unit, gport, -1 /* ingress map */, oam_pcp_qos_map_id);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
    }

    if (gport2 != BCM_GPORT_INVALID) {
        rv = bcm_qos_port_map_set(unit, gport2, -1 /* ingress map */, oam_pcp_qos_map_id);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
    }

    if (gport3 != BCM_GPORT_INVALID) {
        rv = bcm_qos_port_map_set(unit, gport,3 -1 /* ingress map */, oam_pcp_qos_map_id);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
    }
    

    return rv;
}

int ChanTypeMissErr_trap_set(int unit, int dest_port_or_queue, int is_queue) {
    bcm_rx_trap_config_t trap_config;
    int trap_code=0x416; /* trap code on FHEI  will be (trap_code & 0xff), in this case 0x16.*/
    /* valid trap codes for oamp traps are 0x401 - 0x4ff */
    int rv;

    rv =  bcm_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapOampChanTypeMissErr, &trap_code);
    if (rv != BCM_E_NONE) {
       printf("trap create: (%s) \n",bcm_errmsg(rv));
       return rv;
   }

    if (is_queue) {
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(trap_config.dest_port, dest_port_or_queue);
    }
    else {
        BCM_GPORT_SYSTEM_PORT_ID_SET(trap_config.dest_port, dest_port_or_queue);
    }

    rv = bcm_rx_trap_set(unit, trap_code, trap_config);
    if (rv != BCM_E_NONE) {
       printf("trap set: (%s) \n",bcm_errmsg(rv));
       return rv;
   }

    return rv;
}

