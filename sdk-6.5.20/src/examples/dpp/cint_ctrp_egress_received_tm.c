/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_ctrp_egress_received_tm.c
 * Purpose:     Example of reading ERPP forward and drop counters
 * 
 * The example counts all the possible HW counters related to this specific mode
 *
 * The settings include:
 *  o    Set the Counter Processor to count forward and droped counters at the ERPP (SOC properties)
 *  o    Get all the relevant counters. 
 */
 
/* 
 *  
 * To do so, set the SOC properties: 
      counter_engine_source_0.BCM88650=EGRESS_RECEIVE_TM
      counter_engine_statistics_0.BCM88650=FWD_DROP
 
   Actually 2 possibilities of count: per egress queue (EGRESS_RECEIVE_TM)
   or per port (EGRESS_RECEIVE_TM_PORT). In both cases, the API gport is
   accessed per Queue. The translation queue -> port is done internally.
 */

/*
 * Retrieve all the possible HW counters of a single VOQ. 
 */
int egress_receive_tm_stat_get(/*in*/ int unit, 
                        soc_port_t port,
                        bcm_cos_t cos,
                        uint64 *values
                        ) {
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_stats_t stat[6];
    bcm_gport_t gport;
    int i, x;
    uint64 value[6];
    stat[0 ] = bcmCosqGportReceivedBytes;          /* Bytes received into queue. */
    stat[1 ] = bcmCosqGportReceivedPkts;           /* Pkts received into queue. */
    stat[2 ] = bcmCosqGportDroppedBytes;           /* Bytes dropped in queue. */
    stat[3 ] = bcmCosqGportDroppedPkts;            /* Pkts dropped in queue. */   
    stat[4 ] = bcmCosqGportEnqueuedBytes;           /* enqueued bytes */
    stat[5 ] = bcmCosqGportEnqueuedPkts;          /* enqueued packets */
    
    BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET(gport, port);
    rv = bcm_cosq_gport_stat_enable_get(unit, port, &x);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_stat_enable_get() failed $rv\n");
        return rv;
    }
    /* 
     * Go over all the 6 possible statistics and retrieve them
     */
    for (i = 0; i< 6; i++) {
        rv = bcm_cosq_gport_stat_get(unit, port , cos, stat[i], &value[i]);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_stat_get() failed $rv\n");
            printf ("error in stat %d in port %d\n", i, port);
            return rv;
        } else {
        /* Print the Counter result */
            switch (i) {
              case 0  : printf ("stat_id=%d bcmCosqGportReceivedBytes       = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
              case 1  : printf ("stat_id=%d bcmCosqGportReceivedPkts        = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
              case 2  : printf ("stat_id=%d bcmCosqGportDroppedPkts         = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
              case 3  : printf ("stat_id=%d bcmCosqGportDroppedPkts         = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
              case 4  : printf ("stat_id=%d bcmCosqGportEnqueuedBytes       = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
              case 5  : printf ("stat_id=%d bcmCosqGportEnqueuedPkts        = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                }
        }
    }
    /* Return the value of the received packets - dropped/enqueued */
    *values = value[1];
    return rv;
}

int main(void)
{
    int unit = 0;
    int port =  14;
    int cos = 0;
    bcm_error_t rv = BCM_E_NONE;
    uint64 value;
    /* Example: get the counters statistics of port 14 cos 0 */
    rv = egress_receive_tm_stat_get(unit, port, cos, value);
    if (rv != BCM_E_NONE) {
        printf("egress_receive_tm_stat_get() failed $rv\n");
        return rv;
    }

    printf("egress_receive_tm_stat_get completed with status (%s)\n", bcm_errmsg(rv));
    return rv;
}

main();

