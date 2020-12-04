/*~~~~~~~~~~~~~~~~~~~~~~~~~~Cosq: Egress Transmit~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *  
 * File:        cint_egress_transmit_scheduler.c
 * Purpose:     Example of simple Egress transmit scheduling setup 
 *  
 * Example includes:
 *  o     Port shaper 
 *  o     Queue's weight (WFQ) 
 *  o     Strict priority 
 *  o     Incoming TC/DP mapping 
 *
 * It is assumed diag_init is executed.
 * 
 * Settings include:
 * Set the OFP Bandwidth to 5G using the port shaper.
 *  1.  Set the OFP Bandwidth to 5G using the port shaper. 
 *    2.    Set queues priorities WFQ/SP as follows:
 *         -    For the high-priority queues, WFQ is set: UC will get 2/3 of the BW and MC will get 1/3 of the BW.
 *         -    For the low-priority queues, SP UC is set over MC.
 *    3.    Configure TC/DP egress mapping. Map low-priority traffic (0 - 3 for UC, 0  - 5 for MC)
 *     to low-priority queues, and high-priority traffic (4  - 7 for UC, 6  - 7 for MC) to high-priority queues.
 * 
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  |                                                                        |
 *  |                                 |\                                     |
 *  |   bcmCosqGportTypeUnicastEgress | \                                    |
 *  |             +-----------+       |  \                                   |
 *  |   TC 0-3 -->| HP UC  || |-------|2/3\                                  |
 *  |             +-----------+       |    |                                 |
 *  |  bcmCosqGportTypeMulticastEgress|WFQ |-----+                           |
 *  |             +-----------+       |    |     |     |\                    |
 *  |   TC 0-5 -->| HP MC  || |-------|1/3/      |     | \                   |
 *  |             +-----------+       |  /       +---->|H \                  |
 *  |                                 | /              |   \                 |
 *  |                                 |/               |    |    5G          |
 *  |                                                  | SP |----SPR---->    |
 *  |   bcmCosqGportTypeUnicastEgress |\               |    |                |
 *  |             +-----------+       | \              |   /                 |
 *  |   TC 4-7 -->| LP UC  || |-------|H \       +---->|L /                  |
 *  |             +-----------+       |   |      |     | /                   |
 *  |  bcmCosqGportTypeMulticastEgress| SP|------+     |/                    |
 *  |             +-----------+       |   |                                  |
 *  |   TC 6-7 -->| LP MC  || |-------|L /          +----------------+       |
 *  |             +-----------+       | /           |      KEY       |       |
 *  |                                 |/            +----------------+       |
 *  |                                               |SPR- Rate Shaper|       |
 *  |                                               |                |       |
 *  |                                               +----------------+       |
 *  |              +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+              |
 *  |              |   Figure 3: Egress Transmit Scheduler    |              |
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  cint ../../../../src/examples/dpp/utility/cint_utils_multicast.c  
 */

/* Set OFP Bandwidth (max KB per sec)
 * Configure the Port Shaper's max rate
 */
 int set_ofp_bandwidth(int unit, int local_port_id, int max_kbits_sec){
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_type_t gport_type;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t out_gport;

    int min_kbits_rate = 0;
    int flags = 0;

    /* Set GPORT according to the given local_port_id */
    BCM_GPORT_LOCAL_SET(gport_info.in_gport, local_port_id); 
  
    gport_type = bcmCosqGportTypeLocalPort; 

    rv = bcm_cosq_gport_handle_get(unit, gport_type, &gport_info);
    if (rv != BCM_E_NONE) {
        printf("Error, in handle get, gport_type $gport_type \n");
        return rv;
    }

    out_gport = gport_info.out_gport;
    rv = bcm_cosq_gport_bandwidth_set(unit, out_gport, 0, min_kbits_rate, max_kbits_sec, flags);
    if (rv != BCM_E_NONE) {
        printf("Error, in bandwidth set, out_gport $out_gport max_kbits_sec $max_kbits_sec \n");
        return rv;
    }
    
    return rv;
}

/* Set OFP Burst (max size)
 * Configure the Port Shaper's max burst
 */
int set_ofp_burst(int unit, int local_port_id, int max_burst_kbits){
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_type_t gport_type;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t out_gport;

    /* Set GPORT according to the given local_port_id */
    BCM_GPORT_LOCAL_SET(gport_info.in_gport, local_port_id); 

    gport_type = bcmCosqGportTypeLocalPort; 

    rv = bcm_cosq_gport_handle_get(unit, gport_type, &gport_info);
    if (rv != BCM_E_NONE) {
        printf("Error, in handle get, gport_type $gport_type \n");
        return rv;
    }
    
    out_gport = gport_info.out_gport;
    rv = bcm_cosq_control_set(unit, out_gport, -1, bcmCosqControlBandwidthBurstMax, max_burst_kbits);
    if (rv != BCM_E_NONE) {
        printf("Error, in burst set, out_gport $out_gport max_burst_kbits $max_burst_kbits \n");
        return rv;
    }
    
    return rv;
}

/* Set Strict Priority Configuration 
 * queue = BCM_COSQ_LOW_PRIORITY
 *         BCM_COSQ_HIGH_PRIORITY
 * SP_Type = 0 for UC over MC
 *           1 for MC over UC
 */
int set_sp(int unit, int local_port_id, int queue, int sp_type){
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_type_t gport_type;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t out_gport;
    
    /* Set GPORT according to the given local_port_id */
    BCM_GPORT_LOCAL_SET(gport_info.in_gport,local_port_id); 

    /* We will define MC as high or low priority */
    gport_type = bcmCosqGportTypeMulticastEgress; 

    rv = bcm_cosq_gport_handle_get(unit,gport_type,&gport_info);
    if (rv != BCM_E_NONE) {
        printf("Error, in handle get, gport_type $gport_type \n");
        return rv;
    }
    
    out_gport = gport_info.out_gport;
    if(sp_type == 0) {
        /* Setting with BCM_COSQ_SP1 will set the MC to be LOWER priority --> UC over MC */
        rv = bcm_cosq_gport_sched_set(unit, out_gport, queue, BCM_COSQ_SP1, -1);
    } else {
        /* Setting with BCM_COSQ_SP0 will set the MC to be HIGHER priority --> MC over UC */
        rv = bcm_cosq_gport_sched_set(unit, out_gport, queue, BCM_COSQ_SP0, -1);
    }
    
    if (rv != BCM_E_NONE) {
        printf("Error, in SP set, out_gport $out_gport queue $queue sp_type  $sp_type \n");
        return rv;
    }
    
    return rv;
}

/* Generic set weight function 
 * queue = BCM_COSQ_LOW_PRIORITY
 *         BCM_COSQ_HIGH_PRIORITY
 * uc_mc = 0 for UC
 *         1 for MC 
 */
int set_weight(int unit, int local_port_id, int queue, int uc_mc, int weight){
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_type_t gport_type;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t out_gport;

    /* Set GPORT according to the given local_port_id */
    BCM_GPORT_LOCAL_SET(gport_info.in_gport,local_port_id); 

    if(uc_mc == 0) {
        gport_type = bcmCosqGportTypeUnicastEgress; 
    } else {
        gport_type = bcmCosqGportTypeMulticastEgress; 
    }

    rv = bcm_cosq_gport_handle_get(unit,gport_type,&gport_info);
    if (rv != BCM_E_NONE) {
        printf("Error, in handle get, gport_type $gport_type \n");
        return rv;
    }
    
    out_gport = gport_info.out_gport;

    /* Set the weight of the designated queue
     * mode = -1 becuase we are setting weight and not strict priority
     */
    rv = bcm_cosq_gport_sched_set(unit, out_gport, queue, -1, weight);
    if (rv != BCM_E_NONE) {
        printf("Error, in weight set, out_gport $out_gport queue $queue weight $weight \n");
        return rv;
    }
    
    return rv;
}

int
_bcm_petra_egress_queue_from_cosq(int unit,
                                  int *queue_id,
                                  int cosq)
{
    bcm_error_t rv = BCM_E_NONE;
 
    switch (cosq) {
        case BCM_COSQ_HIGH_PRIORITY:
        case 0:
            *queue_id = 0;
            break;
        case BCM_COSQ_LOW_PRIORITY:
        case 1:
            *queue_id = 1;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            *queue_id = cosq;
            break;
    }
 
    return rv;
}

/* Map incoming UC TC/DP to L/H queue
 *  incoming_tc = 0-7
 *  incoming_dp = 0-3
 *  queue = BCM_COSQ_LOW_PRIORITY
 *          BCM_COSQ_HIGH_PRIORITY
 */
int set_uc_queue_mapping(int unit, int local_port_id, int incoming_tc, int incoming_dp, int queue){
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_type_t gport_type;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t out_gport;
    int queue_id;

    /* Set GPORT according to the given local_port_id */
    BCM_GPORT_LOCAL_SET(gport_info.in_gport,local_port_id); 

    gport_type = bcmCosqGportTypeUnicastEgress; 

    rv = bcm_cosq_gport_handle_get(unit,gport_type,&gport_info);
    if (rv != BCM_E_NONE) {
        printf("Error, in handle get, gport_type $gport_type \n");
        return rv;
    }
    
    out_gport = gport_info.out_gport;
    rv = _bcm_petra_egress_queue_from_cosq(unit,&queue_id,queue);
    rv = bcm_cosq_gport_egress_map_set(unit, out_gport, incoming_tc, incoming_dp, queue_id);
    
    if (rv != BCM_E_NONE) {
        printf("Error, in uc queue mapping, out_gport $out_gport incoming_tc $incoming_tc incoming_dp $incoming_dp queue $queue \n");
        return rv;
    }
    
    return rv;
}
 
/*  Map incoming MC TC/DP to L/H queue
 *  incoming_tc = 0-7
 *  incoming_dp = 0-3
 *  queue = BCM_COSQ_LOW_PRIORITY
 *          BCM_COSQ_HIGH_PRIORITY
 */
 int set_mc_queue_mapping(int unit, int local_port_id, int incoming_tc, int incoming_dp, int queue){
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_type_t gport_type;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t out_gport;
    int queue_id;

    /* Set GPORT according to the given local_port_id */
    BCM_GPORT_LOCAL_SET(gport_info.in_gport,local_port_id); 

    gport_type = bcmCosqGportTypeLocalPort; 

    rv = bcm_cosq_gport_handle_get(unit,gport_type,&gport_info);
    if (rv != BCM_E_NONE) {
        printf("Error, in handle get, gport_type $gport_type \n");
        return rv;
    }
    
    out_gport = gport_info.out_gport;

    rv = _bcm_petra_egress_queue_from_cosq(unit,&queue_id,queue);
    bcm_cosq_egress_multicast_config_t  multicast_egress_mapping =  { 0, queue_id,-1,-1};
    
    rv = bcm_cosq_gport_egress_multicast_config_set(unit, out_gport, incoming_tc, incoming_dp, 
                                                    BCM_COSQ_MULTICAST_SCHEDULED, 
                                                    multicast_egress_mapping );
    
    if (rv != BCM_E_NONE) {
        printf("Error, in mc queue mapping, out_gport $out_gport incoming_tc $incoming_tc incoming_dp $incoming_dp queue $queue \n");
        return rv;
    }
    
    return rv;
}

/* Setup MAC forwading
 * dest_type = 0 for Local Port
 *             1 for MC Group
 */
int setup_mac_forwarding(int unit, bcm_mac_t mac, bcm_vlan_t vlan, int dest_type, int dest_id){
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t dest_gport;
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, mac, vlan);   
    /* Create MC or PORT address forwarding */
    if(dest_type == 0) {
        l2_addr.flags = BCM_L2_STATIC;
        BCM_GPORT_LOCAL_SET(dest_gport, dest_id);
        l2_addr.port = dest_gport;
    } else {
        l2_addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;
        l2_addr.l2mc_group = dest_id;
    }
    rv = bcm_l2_addr_add(unit,&l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, in setup_mac_forwarding, dest_type $dest_type dest_id $dest_id \n");
        return rv;
    }

    return rv;
}

/*  Main function
 *  bcm_local_port_id = The designated port we want to configure
 *  is_tm = 0 for PP Ports
 *          1 for TM ports
 */
int egress_transmit_application(int unit, int bcm_local_port_id, int is_tm){
    bcm_error_t rv = BCM_E_NONE;
    int tc = 0;
    int dp = 0;
    int cosq_levels = 8;

    /* Init */
    print("Starting Egress Transmit Application\n");

    /* Create MC group */
    printf("Setting up Multicast Groups.\n");
    int mc_group_id = 5005;
    int cud = 0;
    bcm_multicast_t mc_group = mc_group_id;

    /* We want to overwrite any existing group */
    bcm_multicast_destroy(unit, mc_group);
    
    /* Open Egress MC with the designated port as destination and 0 as cud */
    rv = multicast__open_egress_mc_group_with_local_ports(unit, mc_group_id, &bcm_local_port_id, &cud, 1, 0);
    if (rv != BCM_E_NONE) return rv;

    /* Forwarding Setup, only relevant for PP ports */
    if(is_tm == 0) {
        printf("Setting up MAC Forwarding for PP Ports.\n");
        bcm_mac_t incoming_mac_uc;
        bcm_mac_t incoming_mac_mc;
        incoming_mac_uc[5] = 0x1;
        int incoming_vlan_uc = 1;
        incoming_mac_mc[5] = 0x2;
        int incoming_vlan_mc = 1;
        
        rv = setup_mac_forwarding(unit, incoming_mac_uc, incoming_vlan_uc, 0, bcm_local_port_id);
        if (rv != BCM_E_NONE) return rv;
        rv = setup_mac_forwarding(unit, incoming_mac_mc, incoming_vlan_mc, 1, mc_group_id);
        if (rv != BCM_E_NONE) return rv;
    }

    /* Set OFP Bandwidth to 5G */
    printf("Setting Port Bandwidth.\n");
    int max_bandwidth = 5000000; 
    rv = set_ofp_bandwidth(unit, bcm_local_port_id, max_bandwidth);
    if (rv != BCM_E_NONE) return rv;

    /* Set Weight for the Low Priority Queues */
    printf("Setting Weight for low priority queues.\n");
    int uc_weight =1;
    int mc_weight = 2;
    rv = set_weight(unit, bcm_local_port_id, BCM_COSQ_LOW_PRIORITY, 0, uc_weight);
    if (rv != BCM_E_NONE) return rv;
    rv = set_weight(unit, bcm_local_port_id, BCM_COSQ_LOW_PRIORITY, 1, mc_weight);
    if (rv != BCM_E_NONE) return rv;

    /* Set UC over MC for the High Priority Queues */
    printf("Setting Strict Priority for high priority queues.\n");
    rv = set_sp(unit,bcm_local_port_id, BCM_COSQ_HIGH_PRIORITY, 0);
    if (rv != BCM_E_NONE) return rv;

    /* Set UC Queue Mapping */
    printf("Setting UC queue mapping.\n");
    for (tc=0; tc <= 3 ; tc++) { 
        for(dp = 0; dp <=3; dp++ ) {
            /* printf("\t TC $tc DP $dp --> Low Priority.\n"); */
            rv = set_uc_queue_mapping(unit, bcm_local_port_id, tc, dp, BCM_COSQ_LOW_PRIORITY);
            if (rv != BCM_E_NONE) return rv;
        }
    }
    for (tc=4; tc <= 7 ; tc++) {
        for(dp = 0; dp <=3; dp++ ) {
            /* printf("\t TC $tc DP $dp --> High Priority.\n"); */
            rv = set_uc_queue_mapping(unit, bcm_local_port_id, tc, dp, BCM_COSQ_HIGH_PRIORITY);
            if (rv != BCM_E_NONE) return rv;
        }
    }

    /* Set MC Queue Mapping */
    printf("Setting MC queue mapping.\n");
    for (tc=0; tc <= 3 ; tc++) {
        for(dp = 0; dp <=3; dp++ ) {
            /*printf("\t TC $tc DP $dp --> Low Priority.\n"); */
            rv = set_mc_queue_mapping(unit, bcm_local_port_id, tc, dp, BCM_COSQ_LOW_PRIORITY);
            if (rv != BCM_E_NONE) return rv;
        }
    }
    for (tc=4; tc <= 7 ; tc++) {
        for(dp = 0; dp <=3; dp++ ) {
            /* printf("\t TC $tc DP $dp --> High Priority.\n"); */
            rv = set_mc_queue_mapping(unit, bcm_local_port_id, tc, dp, BCM_COSQ_HIGH_PRIORITY);
            if (rv != BCM_E_NONE) return rv;
        }
    }
    
    printf("Engress Transmit Application Completed Successfully.\n");

    return rv;
}

/* Call example */
/*egress_transmit_application(0, 13, 0);*/
