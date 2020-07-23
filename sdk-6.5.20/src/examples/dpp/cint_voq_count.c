/*~~~~~~~~~~~~~~~~~~~~~~~~~~Cosq: VOQ counter processor~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_voq_count.c
 * Purpose:     Example of reading VOQ counters when the counter processor has a VOQ source
 * 
 * The example counts all the possible HW counters of a single VOQ when the counter processor has a VOQ source
 *
 * The settings include:
 *  o    Set the Counter Processor to count VOQ traffic with highest definition (SOC properties)
 *  o    Get all the possible HW counters of a single VOQ. 
 */
 
/* 
 * Set in the SOC properties file the Counter Processor such that: 
 * - the highest counter resolution is set (10 counters per VOQ) 
 * - a maximum number of VOQs is covered 
 *  
 * To do so, set the SOC properties: 
      counter_engine_source_0=INGRESS_VOQ
      counter_engine_source_1=INGRESS_VOQ
      counter_engine_source_2=INGRESS_VOQ
      counter_engine_source_3=INGRESS_VOQ
      counter_engine_voq_min_queue_0=0
      counter_engine_voq_min_queue_1=6553
      counter_engine_voq_min_queue_2=13107
      counter_engine_voq_min_queue_3=19660
      counter_engine_voq_queue_set_size=1
      counter_engine_statistics_0=FULL_COLOR
      counter_engine_statistics_1=FULL_COLOR
      counter_engine_statistics_2=FULL_COLOR
      counter_engine_statistics_3=FULL_COLOR
 */

/*
 * Retrieve all the possible HW counters of a single VOQ. 
 */
int voq_stat_get(/*in*/ int unit, 
                        int base_queue,     /* Base-VOQ */
                        int cos_trf_cls,    /* Local offset in the VOQ bundle */
                /*out*/ uint64 *values       /* Counter value */
                        ) {
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_stats_t stat[12];
    bcm_gport_t g;
    int x;
    int i;
    uint64 value[12];

    stat[0 ] = bcmCosqGportGreenAcceptedPkts  ;
    stat[1 ] = bcmCosqGportGreenAcceptedBytes ;
    stat[2 ] = bcmCosqGportGreenDroppedPkts   ;
    stat[3 ] = bcmCosqGportGreenDroppedBytes  ;
    stat[4 ] = bcmCosqGportNotGreenAcceptedPkts ;
    stat[5 ] = bcmCosqGportNotGreenAcceptedBytes;
    stat[6 ] = bcmCosqGportYellowDroppedPkts  ;
    stat[7 ] = bcmCosqGportYellowDroppedBytes ;
    stat[8 ] = bcmCosqGportRedDroppedPkts     ;
    stat[9 ] = bcmCosqGportRedDroppedBytes    ;
    stat[10] = bcmCosqGportDroppedBytes       ;
    stat[11] = bcmCosqGportDroppedPkts        ;

    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(g, base_queue);
    rv = bcm_cosq_gport_stat_enable_get(unit, base_queue, &x);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_stat_enable_get() failed $rv\n");
        return rv;
    }
    
    /* 
     * Go over all the 12 possible statistics and retrieve them
     */
    for (i = 0; i< 12; i++) {
        rv = bcm_cosq_gport_stat_get(unit, base_queue, cos_trf_cls, stat[i], &value[i]);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_stat_get() failed $rv\n");
            printf ("error in stat %d in queue %d\n", i, base_queue + cos_trf_cls);
            return rv;
        } else {
            /* Print the Counter result */
                switch (i) {
                  case 0  : printf ("stat_id=%d bcmCosqGportGreenAcceptedPkts     = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 1  : printf ("stat_id=%d bcmCosqGportGreenAcceptedBytes    = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 2  : printf ("stat_id=%d bcmCosqGportGreenDroppedPkts      = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 3  : printf ("stat_id=%d bcmCosqGportGreenDroppedBytes     = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 4  : printf ("stat_id=%d bcmCosqGportNotGreenAcceptedPkts  = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 5  : printf ("stat_id=%d bcmCosqGportNotGreenAcceptedBytes = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 6  : printf ("stat_id=%d bcmCosqGportYellowDroppedPkts     = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 7  : printf ("stat_id=%d bcmCosqGportYellowDroppedBytes    = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 8  : printf ("stat_id=%d bcmCosqGportRedDroppedPkts        = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 9  : printf ("stat_id=%d bcmCosqGportRedDroppedBytes       = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 10 : printf ("stat_id=%d bcmCosqGportDroppedBytes          = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 11 : printf ("stat_id=%d bcmCosqGportDroppedPkts           = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                }
            }
    }
    /* Return the value of the green accepted packets*/
    *values = value[0];
    return rv;
}

int main(void)
{
    int unit = 0;
    bcm_error_t rv = BCM_E_NONE;
    uint64 value[12];

    /* Example: get the counter of VOQ 32 */
    rv = voq_stat_get(unit, 32, 0, value);
    if (rv != BCM_E_NONE) {
        printf("voq_stat_get() failed $rv\n");
        return rv;
    }

    printf("cint_voq_count.c completed with status (%s)\n", bcm_errmsg(rv));
    return rv;
}

main();
