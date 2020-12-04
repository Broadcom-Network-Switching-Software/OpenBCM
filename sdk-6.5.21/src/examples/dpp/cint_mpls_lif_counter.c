/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
* 
* File: cint_mpls_lif_counter.c
* Purpose: An example of the mpls lif counter.

* There are totally 9 statistics modes:
    ----------------------------------------------------------------------------------------------------------------------------------
    |stat_mode           | counting pkts                                                                                             |
    ----------------------------------------------------------------------------------------------------------------------------------
    |all                 | total pkts                                                                                                |                   
    |fwd                 | enqueued pkts                                                                                             |
    |drop                | discarded pkts                                                                                            |
    |fwd_drop            | enqueued and discarded pkts                                                                               |
    |simple_color        | green pkts, not green pkts                                                                                |
    |simple_color_fwd    | enqueued green pkts, enqueued nongreen pkts                                                               |
    |simple_color_drop   | discarded green pkts, discarded nongreen pkts                                                             |
    |green_not_green     | enqueued green pkts, discarded green pkts, enqueued nongreen pkts, discarded nongreen pkts                |
    |full_color          | enqueued green pkts, discarded green pkts, enqueued yellow pkts, discarded yellow pkts, discarded red pkts|
    ----------------------------------------------------------------------------------------------------------------------------------

* SOC property:    
    ---------------------------------------------------------------------------------------------------------
    |        init soc	                               |                 term soc                           |
    ---------------------------------------------------------------------------------------------------------
    |counter_engine_source_0.0=EGRESS_OUT_LIF_0_0_0    |      counter_engine_source_0.0=INGRESS_IN_LIF_0_0_0|
    |counter_engine_source_1.0=EGRESS_OUT_LIF_0_0_1    |      counter_engine_source_1.0=INGRESS_IN_LIF_0_0_1|
    |counter_engine_source_2.0=EGRESS_OUT_LIF_0_0_2    |      counter_engine_source_2.0=INGRESS_IN_LIF_0_0_2|
    |counter_engine_source_3.0=EGRESS_OUT_LIF_0_0_3    |      counter_engine_source_3.0=INGRESS_IN_LIF_0_0_3|
    |counter_engine_statistics_0.0=$mode               |      counter_engine_statistics_0.0=$mode           |
    |counter_engine_statistics_1.0=$mode               |      counter_engine_statistics_1.0=$mode           |
    |counter_engine_statistics_2.0=$mode               |      counter_engine_statistics_2.0=$mode           |
    |counter_engine_statistics_3.0=$mode	       |      counter_engine_statistics_3.0=$mode           |
    ---------------------------------------------------------------------------------------------------------

* mpls service: 
  Initiator:
    *  Default Example, Set MPLS LSR to Outgoing Tunnels B (1001) ,A (1000)
    *  Send Packet:
    *  DA = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33}
    *  VID = 200 
    *  MPLS label 6000
    *  Expected Packet:
    *  DA = {0x00, 0x00, 0x00, 0x00, 0x00, 0x44}
    *  SA = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33}
    *  MPLS label 9000, MPLS label 1001, MPLS label 1000
    *   Sending packet from in_port: 
    *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+    |
    *   |    | DA |SA||TIPD1 |Prio|VID||   MPLS   || Data |    |
    *   |    |0:33|  ||0x8100|    |200||Label:6000||      |    | 
    *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+    |
    *   |   +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+   |
    *   |   | Figure 1: Sending Packet from in_port   |   | 
    *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
    *
    * * The packet will be received in out_port with following header 
    *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+--+    |
    *   |    |DA    | SA ||   MPLS   ||MPLS      ||MPLS      || Data |    |
    *   |    |0:44  |0:33||Label:1000||Label:1001||Label:9000||      |    | 
    *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+--+    |
    *   |   +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
    *   |   |         Figure 2: Packets Received on out_port          |   | 
    *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
  Terminator:
    *  Send Packet:
    *  DA = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11}
    *  VID = 100 
    *  MPLS label 4096 MPLS label 5000
    *  Expected Packet:
    *  DA = {0x00, 0x00, 0x00, 0x00, 0x00, 0x22}
    *  SA = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11}
    *  MPLS label 8000
    *   Sending packet from in_port: 
    *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+--+    |
    *   |    |DA    | SA ||   MPLS   ||MPLS      || Data |    |
    *   |    |0:22  |0:11||Label:4096||Label:5000||           |      |    | 
    *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+--+    |
    *   |   +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
    *   |   |         Figure 3: Sending Packet from in_port           |   | 
    *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
    
    * * The packet will be received in out_port with following header 
    *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+    |
    *   |    | DA | SA ||   MPLS   || Data |    |
    *   |    |0:22|0:11||Label:8000||      |    | 
    *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+    |
    *   |   +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+   |
    *   |   | Figure 4: Packets Received on out_port       |   | 
    *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  

* Calling sequence:
  --------------------------------------------------------
  |                 The example of fwd                   |
  --------------------------------------------------------
    cd ../../../../
    cint src/examples/sand/utility/cint_sand_utils_global.c
    cint src/examples/dpp/utility/cint_utils_l3.c
    cint src/examples/dpp/utility/cint_utils_l2.c
    cint src/examples/sand/utility/cint_sand_utils_mpls.c
    cint src/examples/dpp/cint_qos.c
    cint src/examples/dpp/cint_multi_device_utils.c
    cint src/examples/dpp/cint_mpls_lsr.c
    cint src/examples/dpp/cint_mpls_tunnel_initiator.c
    cint src/examples/dpp/cint_mpls_lif_counter.c
    cint
    print mpls_lif_counter_set(unit, fwd, init);
    ******** send the packet in Figure1 *******
    print mpls_lif_counter_get(unit, fwd, init);
  --------------------------------------------------------
  |         The example of simple_color_drop             |
  --------------------------------------------------------
    ./bcm.user
    cd ../../../../
    cint src/examples/sand/utility/cint_sand_utils_global.c
    cint src/examples/dpp/utility/cint_utils_l3.c
    cint src/examples/dpp/utility/cint_utils_l2.c
    cint src/examples/sand/utility/cint_sand_utils_mpls.c
    cint src/examples/dpp/cint_qos.c
    cint src/examples/dpp/cint_multi_device_utils.c
    cint src/examples/dpp/cint_mpls_lsr.c
    cint src/examples/dpp/cint_mpls_tunnel_initiator.c
    cint src/examples/dpp/cint_mpls_lif_counter.c
    cint
    print mpls_lif_counter_set(unit, simple_color_drop, term);
    ******** send the packet in Figure3 *******
    print mpls_lif_counter_get(unit, simple_color_drop, term);
    
*/

int unit = 0;
int units_ids[1] = {0};
int nof_units = 1;
bcm_port_t in_port = 13;
bcm_port_t out_port = 14;
uint64 value;

/* the inlif gport and outlif gport */
bcm_gport_t inlif_gport;
bcm_gport_t outlif_gport;

/* start_lif, end_lif, lif_counting_profile */
int start_lif;
int end_lif;
int lif_counting_profile;
uint64 stat_array[13];

/* all kinds of stat_mode */
typedef enum stat_format_t {
    fwd,
    drop,
    fwd_drop,
    simple_color,
    simple_color_fwd,
    simple_color_drop,
    green_not_green,
    full_color,
    all
};

/* mpls initiator or terminator */
typedef enum mpls_init_or_term_t {
    init,
    term
};

/* set the counting source and lif range of lif counting profile */
int lif_counter_set(int unit, stat_format_t stat_format, mpls_init_or_term_t init_or_term) 
{
	bcm_stat_lif_counting_t lif_counting;
	int lif_stack_level;
	int priority;
    int count_source;
    int rv;
    
    bcm_info_t info;
    int device_type;
    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
    }
    /* if device is/not J+/QAX/QUX*/
    device_type = (((info.device == 0x8470) || (info.device == 0x8270) ) || (info.device == 0x8680))? 1 : -1;

    switch (init_or_term) {
        case init:
            count_source = bcmStatCounterSourceEgressTransmitOutlif;
            lif_stack_level = bcmStatCounterLifStackId0;
            start_lif = 8*1024;
            end_lif = 9*1024-1;
            lif_counting_profile = 0;
            break;
        case term:
            count_source = bcmStatCounterSourceIngressInlif;
            lif_stack_level = bcmStatCounterLifStackId1;
            /* QAX*/
            if (info.device == 0x8470)
            {
                start_lif = 4*1024;
                end_lif = 8*1024;
            }
            /* QUX*/
            else if(info.device == 0x8270)
            {
                start_lif = 1*1024;
                end_lif = 4*1024-1;
            }
            /* Jericho, JerichoPlus*/
            else
            {
                start_lif = 32*1024;
                end_lif = 64*1024;
            }
            lif_counting_profile = 2;
            break;
        default:
            return BCM_E_PARAM;
    }
    
    /* lif counting source. */
    lif_counting.source.type = count_source;
	lif_counting.source.command_id = 0;
	lif_counting.source.stif_counter_id = -1;    
    
    /* a range of lif IDs. */
    lif_counting.range.start = start_lif;
    lif_counting.range.end = end_lif;
    
    /* mapping a lif-couting-profile to a counting-source and lif-counting-range. */
	rv = bcm_stat_lif_counting_profile_set(unit, 0, lif_counting_profile, &lif_counting);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stat_lif_counting_profile_set $rv\n");
        return rv;
    }

    if(device_type != 1 || init_or_term == term) {
    	priority = 1; /* we are couting the second LIF on the stack - set it to a higher priority */
    	rv = bcm_stat_lif_counting_stack_level_priority_set(unit, 0, &lif_counting.source, lif_stack_level, priority);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_stat_lif_counting_stack_level_priority_set $rv\n");
            return rv;
        }
    }
    
    return rv;
}

/* set up the mpls service and get the inlif_gport or outlif_gport */
int mpls_tunnel_set(int unit, mpls_init_or_term_t init_or_term) 
{
    bcm_gport_t in_modport, in_sysport;
    bcm_gport_t out_modport, out_sysport;
    int term_label = 0x1000;
    int pipe_mode_exp_set = 0;
    int exp_set = 2;
    int rv;
    
    switch (init_or_term) {
    case init:
        outlif_counting_profile = lif_counting_profile;
        rv = mpls_tunnel_initiator_run_with_defaults_aux(unit, -1, in_port, out_port, pipe_mode_exp_set, exp_set);
        if (rv != BCM_E_NONE) {
            printf("Error, mpls_tunnel_initiator_run_with_defaults_aux $rv\n");
            return rv;
        }
    
        outlif_gport = outlif_to_count;
        printf("The tunnel outlif_gport is %d\n",outlif_gport);
        
        break;
    case term:
        rv = bcm_port_gport_get(unit, in_port, &in_modport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_gport_get $rv\n");
            return rv;
        }
    
        rv = bcm_port_gport_get(unit, out_port, &out_modport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_gport_get $rv\n");
            return rv;
        }
    
        rv = bcm_stk_gport_sysport_get(unit, in_modport, &in_sysport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_stk_gport_sysport_get $rv\n");
            return rv;
        }
    
        rv = bcm_stk_gport_sysport_get(unit, out_modport, &out_sysport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_stk_gport_sysport_get $rv\n");
            return rv;
        }
    
        rv = mpls_lsr_run_with_defaults_multi_device(units_ids, nof_units, in_sysport, out_sysport);
        if (rv != BCM_E_NONE) {
            printf("Error, mpls_lsr_run_with_defaults_multi_device $rv\n");
            return rv;
        }
		
        inlif_counting_profile = lif_counting_profile;
        rv = mpls_add_term_entry(unit, term_label, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, mpls_add_term_entry $rv\n");
            return rv;
        }
    
        inlif_gport = ingress_tunnel_id;
        printf("The tunnel inlif_gport is %d\n",inlif_gport);

        break;
    default:
        return BCM_E_PARAM;
    }
    
    return rv;
}

int mpls_lif_counter_set(int unit, stat_format_t stat_format, mpls_init_or_term_t init_or_term)   
{
    int rv;
    bcm_gport_t local_gport = 0;   
    
    rv = lif_counter_set(unit, stat_format, init_or_term);
    if (rv != BCM_E_NONE) {
        printf("Error, lif_counter_set $rv\n");
        return rv;
    }
  
    if (stat_format == drop || stat_format == fwd_drop) {
        /* Shape related */ 
        BCM_GPORT_LOCAL_SET(local_gport, out_port);
        rv = bcm_cosq_gport_bandwidth_set(unit, local_gport, 0, 0, 1000000, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_bandwidth_set $rv\n");
            return rv;
        }
    } else if (stat_format == simple_color || stat_format == simple_color_fwd) {
        /* qos related */
        rv = qos_map_mpls_ingress_profile(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, qos_map_mpls_ingress_profile $rv\n");
            return rv;
        }
    } else if (stat_format == simple_color_drop || stat_format == green_not_green || stat_format == full_color) {
        /* Shape related */ 
        BCM_GPORT_LOCAL_SET(local_gport, out_port);
        rv = bcm_cosq_gport_bandwidth_set(unit, local_gport, 0, 0, 1000000, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_bandwidth_set $rv\n");
            return rv;
        }
            
        /* qos related */
        rv = qos_map_mpls_ingress_profile(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, qos_map_mpls_ingress_profile $rv\n");
            return rv;
        }
    }
    
    rv = mpls_tunnel_set(unit, init_or_term);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_tunnel_set $rv\n");
        return rv;
    }
    
    return rv;
}

int mpls_lif_counter_get(int unit, stat_format_t stat_format, mpls_init_or_term_t init_or_term, int command_id, int core_id)  
{
    int index;
    int rv;
    
    bcm_info_t info;
    int device_type;
    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
    }
    bcm_field_stat_t stats[13]= { bcmFieldStatAcceptedPackets, bcmFieldStatDroppedPackets, 
                                bcmFieldStatGreenPackets, bcmFieldStatNotGreenPackets, 
                                bcmFieldStatAcceptedGreenPackets, bcmFieldStatAcceptedNotGreenPackets,
                                bcmFieldStatDroppedGreenPackets, bcmFieldStatDroppedNotGreenPackets,
                                bcmFieldStatAcceptedYellowPackets, bcmFieldStatDroppedYellowPackets, bcmFieldStatDroppedRedPackets, bcmFieldStatPackets
                                };
    bcm_stat_counter_input_data_t input_data;
    bcm_stat_counter_input_data_t_init(&input_data);
    bcm_stat_counter_output_data_t output_data;
    bcm_stat_counter_value_t value_arr[13];
    output_data.value_arr = value_arr;
    switch (init_or_term) {
        case init:
            input_data.counter_source_type = bcmStatCounterSourceEgressTransmitOutlif;
            input_data.counter_source_gport = outlif_gport;
            break;
        case term:
            input_data.counter_source_type = bcmStatCounterSourceIngressInlif;
            input_data.counter_source_gport = inlif_gport;
            break;
        default:
            return BCM_E_PARAM;
        }
    input_data.command_id = command_id;
    input_data.core_id = core_id; 
    
    switch (stat_format) {
        case fwd:
            input_data.nstat = 1;
            input_data.stat_arr = stats; /* use only bcmFieldStatAcceptedPackets*/
            break;
        case drop:
            input_data.nstat = 1;
            stats[0] = stats[1];  /* use only bcmFieldStatDroppedPackets*/
            input_data.stat_arr = stats;
            break;
        case fwd_drop:
            input_data.nstat = 2;
            input_data.stat_arr = stats; /* use bcmFieldStatAcceptedPackets and bcmFieldStatDroppedPackets*/
            break;
        case simple_color:
            input_data.nstat = 2;
            for (index = 0; index < 2; index ++) {
                stats[index] = stats[index+2];
            } /* use bcmFieldStatGreenPackets and bcmFieldStatNotGreenPackets*/
            input_data.stat_arr = stats;
            break;
        case simple_color_fwd:
            input_data.nstat = 2;
            for (index = 0; index < 2; index ++) {
                stats[index] = stats[index+4];
            } /* use bcmFieldStatAcceptedGreenPackets and bcmFieldStatAcceptedNotGreenPackets*/
            input_data.stat_arr = stats;
            break;
        case simple_color_drop:
            input_data.nstat = 2;
            for (index = 0; index < 2; index ++) {
                stats[index] = stats[index+6];
            } /* use bcmFieldStatDroppedGreenPackets and bcmFieldStatDroppedNotGreenPackets*/
            input_data.stat_arr = stats;
            break;
        case green_not_green:
            input_data.nstat = 4;
            for (index = 0; index < 4; index ++) {
                stats[index] = stats[index+4];
            } /* use bcmFieldStatAcceptedGreenPackets,bcmFieldStatAcceptedNotGreenPackets,bcmFieldStatDroppedGreenPackets and bcmFieldStatDroppedNotGreenPackets*/
            input_data.stat_arr = stats;
            break;
        case full_color:
            input_data.nstat = 5;
            /* use bcmFieldStatAcceptedGreenPackets,bcmFieldStatAcceptedNotGreenPackets, bcmFieldStatDroppedYellowPackets, bcmFieldStatDroppedRedPackets*/
            stats[0] = stats[4];
            stats[1] = stats[6];
            stats[2] = stats[10];
            stats[3] = stats[11];
            input_data.stat_arr = stats;
            break;
        case all:
            input_data.nstat = 1;
            stats[0] = stats[12];
            input_data.stat_arr = stats;
            break;
        default:
            return BCM_E_PARAM;
    }
       
    /* get 64-bit counter value for specific statistic type */
    rv = bcm_stat_counter_get(unit, 0, &input_data, &output_data);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stat_counter_get $rv\n");
        return rv;
    }
        
    for (index=0; index<13; index++) {
        stat_array[index] = output_data.value_arr[index].value;
        print("value %d \n",stat_array[index]);
    }
    return rv;
}

/*  counter get using API bcm_stat_counter_get */
int lif_counter_get(int unit, int command_id, int core_id, bcm_field_stat_t stat, int source_type, bcm_gport_t lif_gport,uint32 source_id)
{
    int rv = BCM_E_NONE;
    if (source_type != bcmStatCounterSourceEgressTransmitOutlif && source_type != bcmStatCounterSourceIngressInlif ){
        printf("Error, source type should be EgressTransmitOutlif or IngressInLif\n");
        return rv;
    }
    bcm_field_stat_t stats[1];
    stats[0] = stat;
    bcm_stat_counter_input_data_t input_data;
    bcm_stat_counter_input_data_t_init(&input_data);
    bcm_stat_counter_output_data_t output_data;
    bcm_stat_counter_value_t value_arr[12];
    output_data.value_arr = value_arr;
    input_data.counter_source_type = source_type; 
    input_data.counter_source_gport = lif_gport;
    input_data.counter_source_id = source_id;
    input_data.command_id = command_id;
    input_data.core_id = core_id; 
    input_data.nstat = 1;
    input_data.stat_arr = stats;
    /* get 64-bit counter value for specific statistic type */
    rv = bcm_stat_counter_get(unit, 0, &input_data, &output_data);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stat_counter_get $rv\n");
        return rv;
    }
    value = output_data.value_arr[0].value;		
    return rv;
}

/* counter profile set */
int lif_counting_profile_set(int unit, int source_type, int command_id, int stif_counter_id, int start_lif, int end_lif, int double_entry, int lif_counting_profile, int engine_range_offset) 
{	
    int rv = BCM_E_NONE;
    if (source_type != bcmStatCounterSourceEgressTransmitOutlif && source_type != bcmStatCounterSourceIngressInlif ){
        printf("Error, source type should be EgressTransmitOutlif or IngressInLif\n");
        return rv;
    }
    bcm_stat_lif_counting_t lif_counting;
    bcm_stat_lif_counting_t lif_counting_get;    
    uint32 flags = 0;

    bcm_info_t info;
    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
    }    
    /* lif counting source. */
    lif_counting.source.type = source_type;
    lif_counting.source.command_id = command_id;
    /** J+ do not support stif_counter_id although the new mechanism of out lif counting exists for J+. therefore, must be set as -1 */    
    lif_counting.source.stif_counter_id = (info.device == 0x8680)? -1 : stif_counter_id; 
    lif_counting.source.engine_range_offset = engine_range_offset;
    
    /* a range of lif IDs. */
    lif_counting.range.start = start_lif;
    lif_counting.range.end = end_lif;
    lif_counting.range.is_double_entry = double_entry;
    if(engine_range_offset != 0)
    {
        flags |= BCM_STAT_COUNT_LIF_ENGINE_RANGE_OFFSET_ENABLE;
    }
    /* mapping a lif-couting-profile to a counting-source and lif-counting-range. */
    rv = bcm_stat_lif_counting_profile_set(unit, flags, lif_counting_profile, &lif_counting);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stat_lif_counting_profile_set $rv\n");
        return rv;
    }

    rv = bcm_stat_lif_counting_profile_get(unit, 0, lif_counting_profile, &lif_counting_get);

    /* the offset is parameter that is relevant only in the get API, so no need to compare it */
    lif_counting.source.offset = lif_counting_get.source.offset;
    if(sal_memcmp(&lif_counting, &lif_counting_get, sizeof(lif_counting_get)))
    {
        printf("Error: lif_counting_get != lif_counting: \n");
        printf("\n expected= \n");
        print (lif_counting);
        printf("\n recieved= \n");
        print(lif_counting_get);
        printf("\n");
        return BCM_E_FAIL;
    }
    return rv;
}

