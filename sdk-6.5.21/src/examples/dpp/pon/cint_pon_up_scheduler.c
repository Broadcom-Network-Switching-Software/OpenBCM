/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
*
* File: cint_pon_up_scheduler.c  
* 
* Purpose: examples of TM scheduler BCM API.  
*
* - Port Scheduler
*   - HR Scheduler (SP mode).
*     - CL Scheduler (SP mode), Hookup to HR Scheduler (SP1).
*       - FQ Schedulers  
*         Instance 0,1,2,3 hookup to CL Scheduler (SP0, SP1, SP2,SP3).
*     - CL Scheduler (WRR mode), Hookup to HR Scheduler (SP1).
*       - FQ Schedulers  
*         Instance 0,1,2,3 hookup to CL Scheduler (SP0:W1, SP0:W2, SP0:W3,SP0:W4).
*     - CL Scheduler (SP+WRR mode), Hookup to HR Scheduler (SP1).
*       - FQ Schedulers  
*         Instance 0,1,2,3 hookup to CL Scheduler (SP0, SP1:W1, SP2:W2,SP3:W3).
*         - Unicast VOQ and VOQ connector.
*           VOQ and VOQ connector (COS 6:7, COS 4:5, COS 2:3, COS 0:1) hookup to FQ Scheduler 0,1,2,3
* 
*  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*  |                                                                                 | 
*  |                                        |                                        | 
*  |                                       SPR (500 Mbps)                            | 
*  |                                        |                                        | 
*  |                                       \/                                        | 
*  |                                      +----+                                     | 
*  |                                     /  HR  \                                    | 
*  |                                    /        \                                   | 
*  |                                   /   SP1    \                                  | 
*  |                                  +------------+                                 | 
*  |                                        |                                        | 
*  |                                       SPR (500 Mbps)                            | 
*  |                                        |                                        | 
*  |                                       \/                                        | 
*  |                                +----------------+                               | 
*  |                               / CL (4 SP levels) \                              | 
*  |                              /                    \                             | 
*  |                             / SP0   SP1  SP2   SP3 \                            | 
*  |                            +------------------------+                           | 
*  |                              /      |      |     \                              | 
*  |                             /       |      |      \                             | 
*  |                           SPR     SPR     SPR    SPR                            | 
*  |                         /           |      |         \                          | 
*  |                        /            |      |          \                         | 
*  |                       |/           \/     \/          \/                        | 
*  |                   +------+   +------+   +------+   +------+                     | 
*  |                  /   FQ   \ /   FQ   \ /   FQ   \ /   FQ   \                    | 
*  |                 +----------+----------+----------+----------+                   | 
*  |                    |    |     |    |     |    |     |    |                      |
*  |                COS7|COS6| COS5|COS4| COS3|COS2| COS1|COS0|                      |
*  |                    |    |     |    |     |    |     |    |                      |
*  |                   \/   \/    \/   \/    \/   \/    \/   \/                      |
*  |                 +-------------------------------------------+                   |
*  |                 |                                           |                   |
*  |                 |                  V O Q                    |                   |
*  |                 |                                           |                   |
*  |                 +-------------------------------------------+                   |
*  |                                                                                 |
*  |                                                                                 |
*  |                                                                                 |
*  |                          +~~~~~~~~~~~~~~~~~~~~~+                                |
*  |                          | SCHEDULER MODEL: SP |                                | 
*  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
*  
*  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*  |                                                                                 | 
*  |                                        |                                        | 
*  |                                       SPR (500 Mbps)                            | 
*  |                                        |                                        | 
*  |                                       \/                                        | 
*  |                                      +----+                                     | 
*  |                                     /  HR  \                                    | 
*  |                                    /        \                                   | 
*  |                                   /   SP1    \                                  | 
*  |                                  +------------+                                 | 
*  |                                        |                                        | 
*  |                                       SPR (500 Mbps)                            | 
*  |                                        |                                        | 
*  |                                       \/                                        | 
*  |                                +----------------+                               | 
*  |                               /  CL (1 SP level, \                              | 
*  |                              /   independent WFQ) \                             | 
*  |                             /          SP0         \                            | 
*  |                            +------------------------+                           | 
*  |                            W1/    W2|    W3|   W4\                              | 
*  |                             /       |      |      \                             | 
*  |                           SPR     SPR     SPR    SPR                            | 
*  |                         /           |      |         \                          | 
*  |                        /            |      |          \                         | 
*  |                       |/           \/     \/          \/                        | 
*  |                   +------+   +------+   +------+   +------+                     | 
*  |                  /   FQ   \ /   FQ   \ /   FQ   \ /   FQ   \                    | 
*  |                 +----------+----------+----------+----------+                   | 
*  |                    |    |     |    |     |    |     |    |                      |
*  |                COS7|COS6| COS5|COS4| COS3|COS2| COS1|COS0|                      |
*  |                    |    |     |    |     |    |     |    |                      |
*  |                   \/   \/    \/   \/    \/   \/    \/   \/                      |
*  |                 +-------------------------------------------+                   |
*  |                 |                                           |                   |
*  |                 |                  V O Q                    |                   |
*  |                 |                                           |                   |
*  |                 +-------------------------------------------+                   |
*  |                                                                                 |
*  |                                                                                 |
*  |                                                                                 |
*  |                          +~~~~~~~~~~~~~~~~~~~~~~+                               |
*  |                          | SCHEDULER MODEL: WRR |                               | 
*  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
*  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*  |                                                                                 | 
*  |                                        |                                        | 
*  |                                       SPR (500 Mbps)                            | 
*  |                                        |                                        | 
*  |                                       \/                                        | 
*  |                                      +----+                                     | 
*  |                                     /  HR  \                                    | 
*  |                                    /        \                                   | 
*  |                                   /   SP1    \                                  | 
*  |                                  +------------+                                 | 
*  |                                        |                                        | 
*  |                                       SPR (500 Mbps)                            | 
*  |                                        |                                        | 
*  |                                       \/                                        | 
*  |                                +----------------+                               | 
*  |                               /  CL (2 SP levels,\                              | 
*  |                              /   independent WFQ) \                             | 
*  |                             / SP0        SP1       \                            | 
*  |                            +------------------------+                           | 
*  |                              /    W1|    W2|   W3\                              | 
*  |                             /       |      |      \                             | 
*  |                           SPR     SPR     SPR    SPR                            | 
*  |                         /           |      |         \                          | 
*  |                        /            |      |          \                         | 
*  |                       |/           \/     \/          \/                        | 
*  |                   +------+   +------+   +------+   +------+                     | 
*  |                  /   FQ   \ /   FQ   \ /   FQ   \ /   FQ   \                    | 
*  |                 +----------+----------+----------+----------+                   | 
*  |                    |    |     |    |     |    |     |    |                      |
*  |                COS7|COS6| COS5|COS4| COS3|COS2| COS1|COS0|                      |
*  |                    |    |     |    |     |    |     |    |                      |
*  |                   \/   \/    \/   \/    \/   \/    \/   \/                      |
*  |                 +-------------------------------------------+                   |
*  |                 |                                           |                   |
*  |                 |                  V O Q                    |                   |
*  |                 |                                           |                   |
*  |                 +-------------------------------------------+                   |
*  |                                                                                 |
*  |                                                                                 |
*  |                                                                                 |
*  |                          +~~~~~~~~~~~~~~~~~~~~~~~~~+                            |
*  |                          | SCHEDULER MODEL: SP+WRR |                            | 
*  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~




*       
* Note: If we want to use the WFQ independent mode, we must calculate the least
*    common multiple(LMM), then use the LMM divide the expect weight value(WE) 
*    to calculate the actual weight value.   
*    For example: WE1 : WE2 : WE3 : WE4 = 4 : 3 : 3 : 2.
*        LMM = 12.
*        WA1 = LMM / WE1 = 12 / 4 = 3.         
*        WA2 = LMM / WE2 = 12 / 3 = 4.
*        WA3 = LMM / WE3 = 12 / 3 = 4.
*        WA4 = LMM / WE4 = 12 / 2 = 6.
*        So WA1 : WA2 : WA3 : WA4 = 3 : 4 : 4 : 6.
 */

/*******************************************************************************
*
*  Upstream schefuler
 */
bcm_gport_t nni_gport;
bcm_gport_t e2e_nni_gport;
bcm_gport_t gport_scheduler_cl;    
bcm_gport_t gport_scheduler_fq[4];
bcm_gport_t gport_voq_connector_group;
bcm_gport_t gport_voq_group;
int scheduler_fq_nums = 4;
int fq_scheduler_weight[4] = { 4, 3, 2, 1 };  
int fq_scheduler_weight_set[4];


/*******************************************************************************
* Calculate the common divisor                                           
 */
int pon_up_scheduler_common_divisor(int a, int b)
{
    int temp;

    temp = (a > b) ? a : b;

    while(temp != 0)
    {
        temp = a % b;
        a = b;
        b = temp;
    }

    return a;
}


/*******************************************************************************
* Calculate the common multiple                                          
 */
int pon_up_scheduler_common_mutiple(int a, int b)
{
    int temp;

    temp = pon_up_scheduler_common_divisor(a, b);

    return (a * b / temp);
}


/*******************************************************************************
* Calculate the min mutiple                                          
 */
int pon_up_scheduler_min_mutiple(int *array, int nums)
{
    int i;
    int multiple;
    int pre;
    int next;

    multiple = array[0];

    for (i = 0; i < (nums - 1); i++)
    {
        pre = multiple;
        next = array[i + 1];
        multiple = pon_up_scheduler_common_mutiple(pre, next);
    }

    return multiple;
}


/*******************************************************************************
* Upstream FQ scheduler weight configuration                                          
 */
void pon_up_scheduler_fq_weight_config(void)
{   
    int i;
    int multiple;

    multiple = pon_up_scheduler_min_mutiple(fq_scheduler_weight, scheduler_fq_nums);

    for ( i = 0; i < scheduler_fq_nums; i++)
    {    
        fq_scheduler_weight_set[i] = multiple/fq_scheduler_weight[i];
    }    
}    

/*******************************************************************************
* Upstream scheduler init                                            
 */
int pon_up_scheduler_init(int unit)
{
	int rv = BCM_E_NONE;

    pon_up_scheduler_fq_weight_config();

	return rv;
}  


/*******************************************************************************
* Upstream scheduler modify                                                 
 */ 
int pon_up_scheduler_modify(int unit, int mode)
 {
    int i;    
    int rv = BCM_E_NONE;
    int flags = 0;
    
    rv = bcm_cosq_gport_detach(unit, e2e_nni_gport, gport_scheduler_cl, 0);    
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_detach failed $rv\n");
        return rv;
    }

    for (i = 0; i < scheduler_fq_nums; i++)
    {           
        rv = bcm_cosq_gport_detach(unit, gport_scheduler_cl, gport_scheduler_fq[i], 0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_detach failed $rv\n");
            return rv;
        }         
    }

    rv = bcm_cosq_gport_delete(unit, gport_scheduler_cl);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_delete failed $rv\n");
        return rv;
    }


    /* create CL enhanced scheduler */
    switch(mode)
    {
        case 0:
        flags = BCM_COSQ_GPORT_SCHEDULER | 
            BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE1_4SP;        
        break;
            
        case 1:
        flags = BCM_COSQ_GPORT_SCHEDULER | 
            BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE5_1SP_WFQ | 
            BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_INDEPENDENT;
        break;
            
        case 2:
        flags = BCM_COSQ_GPORT_SCHEDULER | 
            BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE4_2SP_WFQ | 
            BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_INDEPENDENT;
        break;
    }       


    rv = bcm_cosq_gport_add(unit, nni_gport, 1, flags, &gport_scheduler_cl);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return rv;
    }

    rv = bcm_cosq_gport_sched_set(unit,
                      gport_scheduler_cl,
                      0,
                      BCM_COSQ_SP1,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    rv = bcm_cosq_gport_attach(unit, e2e_nni_gport, gport_scheduler_cl, 0);    
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }

    /* create FQ scheduler */
    for (i = 0; i < scheduler_fq_nums; i++)
    {
        switch(mode)
        {
            case 0:
            rv = bcm_cosq_gport_sched_set(unit, 
                          gport_scheduler_fq[i],
                          0,
                          BCM_COSQ_SP0 + i,
                          0);
            if (rv != BCM_E_NONE) {
                printf("bcm_cosq_gport_sched_set failed $rv\n");
                return rv;
            }
            break;

            case 1:
            rv = bcm_cosq_gport_sched_set(unit,
                          gport_scheduler_fq[i],
                          0,
                          BCM_COSQ_SP0,
                          fq_scheduler_weight_set[i]);
            if (rv != BCM_E_NONE) {
                printf("bcm_cosq_gport_sched_set failed $rv\n");
                return rv;
            }
            break;

            case 2:
            if (i == 0) {
                rv = bcm_cosq_gport_sched_set(unit,
                              gport_scheduler_fq[i],
                              0,
                              BCM_COSQ_SP0,
                              0);
                if (rv != BCM_E_NONE) {
                    printf("bcm_cosq_gport_sched_set failed $rv\n");
                    return rv;
                }
            } else {
                rv = bcm_cosq_gport_sched_set(unit,
                              gport_scheduler_fq[i],
                              0,
                              BCM_COSQ_SP1,
                              fq_scheduler_weight_set[i]);
                if (rv != BCM_E_NONE) {
                    printf("bcm_cosq_gport_sched_set failed $rv\n");
                    return rv;
                }
            }                
            break;
        }        
    
        rv = bcm_cosq_gport_attach(unit, gport_scheduler_cl, 
                       gport_scheduler_fq[i], 0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_attach failed $rv\n");
            return rv;
        }       
    }         
    
    printf("pon_up_scheduler_modify completed with status (%s)\n", bcm_errmsg(rv));
    return rv;

}



/*******************************************************************************
* Upstream scheduler setup                                                 
 */ 
int pon_up_scheduler_setup(int unit)
{       
    bcm_error_t rv = BCM_E_NONE;    
    int my_modid = 0;
    int num_cos = 8;
    int flags = 0;    
    int i;
    int cosq;
    int voq;
    int kbits_sec_max;
    int max_burst;  
    bcm_gport_t gport_scheduler;
    bcm_gport_t e2e_parent_gport;      
    bcm_cosq_gport_connection_t connection; 
    bcm_cosq_voq_connector_gport_t config;
    int core, tm_port;
    rv = bcm_stk_modid_get(unit, &my_modid);
    if (rv != BCM_E_NONE) {
        printf("bcm_stk_modid_get failed $rv\n");
        return rv;
    }
    rv = get_core_and_tm_port_from_port(unit, nni_port, &core, &tm_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in get_core_and_tm_port_from_port\n");
        return rc;
    }

    BCM_GPORT_MODPORT_SET(nni_gport, my_modid+core, tm_port); 
    
    /**************************************************************************/
    /* create CL WFQ independent scheduler */
    /**************************************************************************/
    flags = BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE5_1SP_WFQ | BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_INDEPENDENT;

    rv = bcm_cosq_gport_add(unit, nni_gport, 1, flags, &gport_scheduler_cl);
    if (rv != BCM_E_NONE) {
        printf("CL bcm_cosq_gport_add scheduler failed $rv\n");
        return rv;
    }

    rv = bcm_cosq_gport_sched_set(unit,
                      gport_scheduler_cl,
                      0,
                      BCM_COSQ_SP1,
                      0);
    if (rv != BCM_E_NONE) {
        printf("CL bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    BCM_COSQ_GPORT_E2E_PORT_SET(e2e_nni_gport, nni_port);

    rv = bcm_cosq_gport_attach(unit, e2e_nni_gport, 
                   gport_scheduler_cl, 0);
    
    if (rv != BCM_E_NONE) {
        printf("CL bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }  
    
    for (i = 0; i < scheduler_fq_nums; i++) 
    {
        /**************************************************************************/
        /* create FQ scheduler */
        /**************************************************************************/
        flags = BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_FQ;
        
        rv = bcm_cosq_gport_add(unit, nni_gport, 1, 
                    flags, &gport_scheduler_fq[i]);
        
        if (rv != BCM_E_NONE) {
            printf("FQ (%d) bcm_cosq_gport_add scheduler failed $rv\n", i);
            return rv;
        }
        rv = bcm_cosq_gport_sched_set(unit,
                      gport_scheduler_fq[i],
                      0,
                      BCM_COSQ_SP0,
                      fq_scheduler_weight_set[i]);
        if (rv != BCM_E_NONE) {
            printf("FQ (%d) bcm_cosq_gport_sched_set failed $rv\n", i);
            return rv;
        }
        
        rv = bcm_cosq_gport_attach(unit, gport_scheduler_cl, 
                       gport_scheduler_fq[i], 0);
        if (rv != BCM_E_NONE) {
            printf("FQ (%d) bcm_cosq_gport_attach failed $rv\n", i);
            return rv;
        }
    }

    /**************************************************************************/
    /* create voq connector */     
    /**************************************************************************/
    if (is_device_or_above(unit, JERICHO)) {
        config.port = nni_gport;
        config.flags = BCM_COSQ_GPORT_VOQ_CONNECTOR ;
        config.numq = num_cos ;
        config.remote_modid = my_modid ;
        config.nof_remote_cores = 1 ;
        rv = bcm_cosq_voq_connector_gport_add(unit, &config,  &gport_voq_connector_group);
    }else{
        rv = bcm_cosq_gport_add(unit, nni_gport, num_cos, 
         BCM_COSQ_GPORT_VOQ_CONNECTOR, &gport_voq_connector_group);
    }

    if (rv != BCM_E_NONE) {
        printf("voq connector bcm_cosq_gport_add connector failed $rv\n");
        return rv;
    }    
    
    for (cosq = 0; cosq < num_cos; cosq++)
    {
        /* COS 0-7 */
        rv = bcm_cosq_gport_sched_set(unit,
                          gport_voq_connector_group,
                          cosq,
                          BCM_COSQ_SP0,
                          0);
        if (rv != BCM_E_NONE) {
            printf("voq connector bcm_cosq_gport_sched_set failed $rv\n");
            return rv;
        }

        
        rv = bcm_cosq_gport_attach(unit, gport_scheduler_fq[scheduler_fq_nums - 1 - cosq/2], 
                           gport_voq_connector_group, cosq);
        if (rv != BCM_E_NONE) {
            printf("voq connector bcm_cosq_gport_attach failed $rv\n");
            return rv;
        }
    }

    /**************************************************************************/
    /* create voq */     
    /**************************************************************************/
    rv = bcm_cosq_gport_add(unit, nni_gport, num_cos, 
                BCM_COSQ_GPORT_UCAST_QUEUE_GROUP, &gport_voq_group);

    if (rv != BCM_E_NONE) {
        printf("voq mcast bcm_cosq_gport_add queue failed $rv\n");
        return rv;
    }

    /**************************************************************************/
    /* connect voq to voq connector */     
    /**************************************************************************/
    /* connect voq to flow */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
    connection.remote_modid = my_modid;
    connection.voq = gport_voq_group;
    connection.voq_connector = gport_voq_connector_group;
    
    rv = bcm_cosq_gport_connection_set(unit, &connection);

    if (rv != BCM_E_NONE) {
        printf("connect mcast bcm_cosq_gport_connection_set ingress failed $rv\n");
        return rv;
    }

    /* connect voq to flow */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    connection.remote_modid = my_modid;
    connection.voq = gport_voq_group;
    connection.voq_connector = gport_voq_connector_group;
    
    rv = bcm_cosq_gport_connection_set(unit, &connection);
    if (rv != BCM_E_NONE) {
        printf("connect mcast bcm_cosq_gport_connection_set egress failed $rv\n");
        return rv;
    }
    
    /*
     * Set rate on the E2E Interface
     */
    kbits_sec_max = 10000000; /* 10Gbps */

    BCM_COSQ_GPORT_E2E_PORT_SET(e2e_nni_gport, nni_port);

    rv = bcm_fabric_port_get(unit,
                 e2e_nni_gport,
                 0,
                 &e2e_parent_gport);
    if (rv != BCM_E_NONE) {
        printf("bcm_fabric_port_get failed $rv\n");
        return rv;
    }

    rv = bcm_cosq_gport_bandwidth_set(unit,
                      e2e_parent_gport,
                      0,
                      0,
                      kbits_sec_max,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set failed $rv\n");
        return rv;
    }
    
    
    /*
     * Set rate on the E2E port
     */
    kbits_sec_max = 10000000; /* 10Gbps */
    
    BCM_COSQ_GPORT_E2E_PORT_SET(e2e_nni_gport, nni_port);

    rv = bcm_cosq_gport_bandwidth_set(unit,
                      e2e_nni_gport,
                      0,
                      0,
                      kbits_sec_max,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set failed $rv\n");
        return rv;
    }  
    
    printf("pon_up_scheduler_setup completed with status (%s)\n", bcm_errmsg(rv));
    return rv;
}


/*******************************************************************************
* Upstream scheduler cleanup                                                    
 */
int pon_up_scheduler_cleanup(int unit)
{  
    int my_modid = 0;
    int num_cos = 8;
    int flags = 0;    
    int i;
    int cosq; 
    int voq;
    bcm_error_t rv = BCM_E_NONE;      
    bcm_cosq_gport_connection_t connection;
    
    rv = bcm_stk_modid_get(unit, &my_modid);
    if (rv != BCM_E_NONE) {
        printf("bcm_stk_my_modid_get failed $rv\n");
        return rv;
    }       
    
    /**************************************************************************/
    /* detach CL scheduler */
    /**************************************************************************/    
    rv = bcm_cosq_gport_detach(unit, e2e_nni_gport, 
                   gport_scheduler_cl, 0);
    
    if (rv != BCM_E_NONE) {
        printf("CL bcm_cosq_gport_detach failed $rv\n");
        return rv;
    }  


    /**************************************************************************/
    /* dis-connect voq to voq connector */     
    /**************************************************************************/
    /* dis-connect voq to flow */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
    connection.remote_modid = my_modid;
    connection.voq = gport_voq_group;
    connection.voq_connector = gport_voq_connector_group;
    
    rv = bcm_cosq_gport_connection_set(unit, &connection);

    if (rv != BCM_E_NONE) {
        printf("connect bcm_cosq_gport_connection_set ingress failed $rv\n");
        return rv;
    }

    /* dis-connect voq to flow */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
    connection.remote_modid = my_modid;
    connection.voq = gport_voq_group;
    connection.voq_connector = gport_voq_connector_group;
    
    rv = bcm_cosq_gport_connection_set(unit, &connection);
    if (rv != BCM_E_NONE) {
        printf("connect bcm_cosq_gport_connection_set egress failed $rv\n");
        return rv;
    }
    
    /**************************************************************************/
    /* delete voq */     
    /**************************************************************************/        
    rv = bcm_cosq_gport_delete(unit, gport_voq_group);
    if (rv != BCM_E_NONE) {
        printf("voq bcm_cosq_gport_delete queue failed $rv\n");
        return rv;
    }
    
    /**************************************************************************/
    /* detach voq connector */     
    /**************************************************************************/        
    for (cosq = 0; cosq < num_cos; cosq++)
    {      
        rv = bcm_cosq_gport_detach(unit, gport_scheduler_fq[scheduler_fq_nums - 1 - cosq/2], 
                           gport_voq_connector_group, cosq);
        if (rv != BCM_E_NONE) {
            printf("voq connector bcm_cosq_gport_detach failed $rv\n");
            return rv;
        }
    }
    
    /**************************************************************************/
    /* delete voq connector */     
    /**************************************************************************/        
    rv = bcm_cosq_gport_delete(unit, gport_voq_connector_group);
    if (rv != BCM_E_NONE) {
        printf("voq connector bcm_cosq_gport_delete failed $rv\n");
        return rv;
    }

    for (i = 0; i < scheduler_fq_nums; i ++) 
    {                
        /**************************************************************************/
        /* detach FQ scheduler */
        /**************************************************************************/
        rv = bcm_cosq_gport_detach(unit, gport_scheduler_cl, 
                       gport_scheduler_fq[i], 0);
        if (rv != BCM_E_NONE) {
            printf("FQ %d bcm_cosq_gport_detach failed $rv\n", i);
            return rv;
        } 
        /**************************************************************************/
        /* delete FQ scheduler */
        /**************************************************************************/
        rv = bcm_cosq_gport_delete(unit, gport_scheduler_fq[i]);
        if (rv != BCM_E_NONE) {
            printf("FQ %d bcm_cosq_gport_delete failed $rv\n");
            return rv;
        }
    }

    /**************************************************************************/
    /* delete CL scheduler */
    /**************************************************************************/    
    rv = bcm_cosq_gport_delete(unit, gport_scheduler_cl);
    
    if (rv != BCM_E_NONE) {
        printf("CL bcm_cosq_gport_delete failed $rv\n");
        return rv;
    }  
    
    printf("pon_up_scheduler_cleanup completed with status (%s)\n", bcm_errmsg(rv));
    return rv;
}


/*******************************************************************************
* Upstream scheduler NNI rate limit                                           
 */
int pon_up_scheduler_ratelimit_nni(int unit, int cir)
{
    bcm_error_t rv = BCM_E_NONE;          
    
    rv = bcm_cosq_gport_bandwidth_set(unit,
                      gport_scheduler_cl,
                      0,
                      0,
                      cir,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set connector failed $rv\n");
        return rv;
    }
    return rv;
}



