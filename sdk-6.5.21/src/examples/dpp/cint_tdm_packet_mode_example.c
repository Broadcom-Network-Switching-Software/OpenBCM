/*~~~~~~~~~~~~~~~~~~~~~~~~~~OTN/TDM Applications~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *  
 * File: cint_tdm_packet_mode_example.c 
 * Purpose: Example shows setup configuration of TDM packet mode example. 
 *  
 * These modes are for devices that are dedicated to support both Data packets and OTN/TDM traffic only.
 * Packet mode is often used when expected TDM traffic consist of packet sizes that are higher than 256 (in ARAD) / 128 (in Petra-B)
 * 
 * For each port to be supported as TDM packet mode, following soc properties should be set:
 * config add tm_port_header_type_<port_id> = TDM_RAW
 * 
 * On Standard application of TDM, each stream traffic is like a "pipe" where there is a static connection
 * between source-port and destination-port. Both Source and destination ports should be of TDM type.
 * In case header_type is set to TDM_RAW , packet is treated as without header i.e. user should set a static forwarding from
 * source port to destination port.
 *  
 * Application Sequence:
 *  1.  Set static forwarding on TDM ports
 *  2.  For each VOQ (UCAST or MCAST) that TDM traffic is going through it, user should set the following settings
 *      o    Set Queue type to be PUSH_QUEUE 
 *      o    In ARAD (only), set Queue signature to be of TDM priority (3).
 *
 * The CINT gives the ability to set port static forwarding and VOQ settings.
 *  o   Unicast settings:
 *      o   Set static forwarding configuration
 *      o   Set matching VOQ configuration
 *           
 *  o   Multicast settings:
 *      o   Set static forwarding configuration
 *      o   Set matching FMQ configuration
 *  	o   Set multicast configuration
 * 
 *  cint utility/cint_utils_multicast.c 
 */

int static_fwd_dest_set(int unit, bcm_gport_t gport, int destination, int enable) {
  bcm_error_t rv = BCM_E_NONE;

  rv = bcm_port_force_forward_set(unit, gport, destination, enable);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_port_force_forward_set, gport $gport, destination $destination \n");
    return rv;
  }

  if (verbose) {
    printf("static_fwd_dest_set: gport(0x%08x) --> destination 0x%x\n", gport, destination);
  }

  return rv;
}

int tdm_packet_mode_queue_settings (int unit, int base_voq_id, int cosq) {
    int rv;
    int type;
    int gport;
    int mode;

    /* Set Push queue */
    mode = BCM_COSQ_DELAY_TOLERANCE_10G_LOW_DELAY;
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(gport,base_voq_id);
    rv = bcm_cosq_gport_sched_set(unit, gport, cosq, mode, 0);
    if (rv != BCM_E_NONE) {
      printf("Error, in bcm_cosq_gport_sched_set \n");
      return rv;
    }

    /* Set Queue Signature */
    mode = 3; /* TDM always 3 */
    rv = bcm_cosq_control_set(unit, gport, cosq, bcmCosqControlHeaderUpdateField, mode);
    if (rv != BCM_E_NONE) {
      printf("Error, in bcm_cosq_control_set \n");
      return rv;
    }

    /* High priority range */


    if (verbose) {
      printf("tdm_packet_mode_queue_settings: gport VOQ(0x%08x) \n", gport);
    }

    return rv;
}

/* UC TDM Packet mode Example:
 * This example can configure a UC to destination port.
 * The FTMH will be appended in the ingress, and removed in the egress.
 * It also sets the Cells Size Range: 70 to 120
 * src_local_port_id = Incoming BCM local port id.
 * destination_port_id = Outgoing BCM local port id. 
 * voq_id = VOQ ID that matched to Outgoing BCM local port id 
 * Remarks: 
 *  We assume that Scheduling Scheme and mapping Systemport to VOQ to MODPORT already been set 
 */
int tdm_packet_mode_uc_example (int unit, int src_local_port_id, int destination_port_id, int voq_id) {
    int gport;
    int rv;
    int start_range;
    int end_range;  
    bcm_cosq_delay_tolerance_t delay_tolerance;

    /* Set static forwarding */
    rv = static_fwd_dest_set(unit, src_local_port_id, destination_port_id, 1);
    if (rv != BCM_E_NONE) {
      printf("Error, in static_fwd_dest_set \n");
      return rv;
    }

    /* Set matched VOQ settings */
    rv = tdm_packet_mode_queue_settings(unit, voq_id, 0);
    if (rv != BCM_E_NONE) {
      printf("Error, in tdm_packet_mode_queue_settings \n");
      return rv;
    }

    rv = bcm_cosq_delay_tolerance_level_get(unit, BCM_COSQ_DELAY_TOLERANCE_10G_LOW_DELAY, &delay_tolerance);
    if (rv != BCM_E_NONE) {
      printf("Error, in bcm_cosq_delay_tolerance_level_get \n");
      return rv;
    }
    delay_tolerance.flags |= BCM_COSQ_DELAY_TOLERANCE_HIGH_Q_PRIORITY;
    rv = bcm_cosq_delay_tolerance_level_set(unit, BCM_COSQ_DELAY_TOLERANCE_10G_LOW_DELAY, &delay_tolerance);
    if (rv != BCM_E_NONE) {
      printf("Error, in bcm_cosq_delay_tolerance_level_set \n");
      return rv;
    }

    return rv;
}   

/* UC TDM Packet mode Example:
 * This example can configure a UC to destination port.
 * The FTMH will be appended in the ingress, and removed in the egress.
 * It also sets the Cells Size Range: 70 to 120
 * src_local_port_id = Incoming BCM local port id.
 * destination_port_id = Outgoing BCM local port id. 
 * voq_id = VOQ ID that matched to Outgoing BCM local port id 
 * Remarks: 
 *  We assume that Scheduling Scheme and mapping Systemport to VOQ to MODPORT already been set 
 */
int tdm_packet_mode_mc_example (int unit, int src_local_port_id, int mc_id) {
    int gport;
    int rv;
    int mc_cuds[3] = {0};
    int mc_ports[3];
    int mcast_gport;
    int multicast_1k_region_base = 0;
    int multicast_1k_region_max = 1023;
    int voq_id = 32 + (src_local_port_id - 1) * 8; /*A calculation base on the BCM Application*/
    BCM_GPORT_MCAST_SET(mcast_gport, mc_id); 

    /* Set static forwarding */
    rv = static_fwd_dest_set(unit, src_local_port_id, mcast_gport, 1);
    if (rv != BCM_E_NONE) {
      printf("Error, in static_fwd_dest_set \n");
      return rv;
    }

    /* Set matched VOQ settings, always zero for TDM Packet mode Multicast */
    rv = tdm_packet_mode_queue_settings(unit,voq_id , 0);
    if (rv != BCM_E_NONE) {
      printf("Error, in tdm_packet_mode_queue_settings \n");
      return rv;
    }

    /* An example */
    mc_ports[0] = src_local_port_id;
    mc_ports[1] = 14;
    mc_ports[2] = 15;

    rv = multicast__open_egress_mc_group_with_local_ports(unit, mc_id, mc_ports, mc_cuds, 3, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_ingress_mc_group_with_local_ports \n");
        return rv;
    }

    return rv;
}  
/*PetraB examples*/
int run_example_tdm_packet_mode_uc_example(int unit) {    
    return tdm_packet_mode_uc_example(unit,1,2,54);
}

int run_example_tdm_packet_mode_mc_example(int unit) {
    return tdm_packet_mode_mc_example(unit,1,2);
}

/* Run automatic main */
/* run_example_tdm_packet_mode_uc_example(unit);*/
/* run_example_tdm_packet_mode_mc_example(unit);*/
int verbose = 1;
    
    
